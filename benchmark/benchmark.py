import argparse
import subprocess
from halo import Halo
from datetime import datetime
from pathlib import Path
import signal


MATERIALS = ["lambertian", "metal", "dielectric", "emissive", "random"]
DENSITIES = ["full", "sparse"]
SIZES = ["5x5x5", "10x10x10", "50x50x50"]

DEFAULT_SEED = 42

RESULTS_DIR = "./benchmark/results"
NUM_SAMPLES = 5
FPS_FILE_NAME = "fps.txt"
RVE_BIN_PATH = "./bin/rve"
spinner = Halo(spinner="dots")


def build_flamegraph(dir_path: str, data_name: str) -> None:
    perf_script = subprocess.Popen(
        ["perf", "script", "-i", f"{dir_path}/{data_name}"],
        stdout=subprocess.PIPE,
        text=True,
    )

    stackcollapse = subprocess.Popen(
        ["stackcollapse-perf.pl"],
        stdin=perf_script.stdout,
        stdout=subprocess.PIPE,
        text=True,
    )

    flamegraph = subprocess.Popen(
        ["flamegraph.pl"],
        stdin=stackcollapse.stdout,
        stdout=open(f"{dir_path}/flamegraph.svg", "w"),
        text=True,
    )

    flamegraph.communicate()


def benchmark(run_args: list[str], dir_path: str) -> list[str]:
    proc = subprocess.Popen(
        [
            "perf",
            "record",
            "-F",
            "99",
            "-g",
            "-o",
            dir_path + "/perf.data",
            "--",
            RVE_BIN_PATH,
            *run_args,
        ],
        stdout=subprocess.PIPE,
        stderr=subprocess.DEVNULL,
        text=True,
    )
    samples = []

    if proc.stdout is None:
        return []

    for line in proc.stdout:
        if line.startswith("FPS"):
            samples.append(line.replace("FPS", "").strip())

        if samples and len(samples) >= NUM_SAMPLES:
            break

    proc.send_signal(signal.SIGINT)
    proc.wait()

    build_flamegraph(dir_path, "perf.data")

    return samples


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog="benchmark",
        description="Benchmark RVE over a matrix of seeded generated worlds.",
    )
    parser.add_argument(
        "--seed",
        type=int,
        default=DEFAULT_SEED,
        help="Seed used to generate every benchmarked world (default: %(default)s)",
    )
    parser.add_argument(
        "--material",
        type=str,
        choices=MATERIALS,
        action="append",
        help="Restrict the matrix to this material, repeatable.",
    )
    parser.add_argument(
        "--density",
        type=str,
        choices=DENSITIES,
        action="append",
        help="Restrict the matrix to this density, repeatable.",
    )
    parser.add_argument(
        "--size",
        type=str,
        choices=SIZES,
        action="append",
        help="Restrict the matrix to this size, repeatable.",
    )
    args = parser.parse_args()

    materials = args.material or MATERIALS
    densities = args.density or DENSITIES
    sizes = args.size or SIZES

    # make clean all
    spinner.text = "building"
    spinner.start()
    try:
        subprocess.run(
            ["make", "clean", "all", "BUILD=benchmark"],
            check=True,
            capture_output=True,
            text=True,
        )

    except subprocess.CalledProcessError as e:
        print(f"Command failed with exit code: {e.returncode}")
        print(f"Error message: {e.stderr}")
        exit(1)

    start_time = datetime.now().strftime("%Y-%m-%d-%H:%M:%S")

    for material in materials:
        for density in densities:
            for size in sizes:
                # FIXME: skipping 50x50x50 for now since it takes waaaay too long
                if size == "50x50x50":
                    continue

                spinner.text = f"benchmarking {material}/{density}/{size}"

                run_args = [
                    "--seed",
                    str(args.seed),
                    "--size",
                    size,
                    "--material",
                    material,
                    "--density",
                    density,
                ]

                dir_path = Path(
                    f"{RESULTS_DIR}/{start_time}/{material}/{density}/{size}"
                )
                dir_path.mkdir(parents=True, exist_ok=True)

                # profile each run getting the avg fps, flamegraph and more data???
                fps = "\n".join(benchmark(run_args, str(dir_path)))

                with open(
                    f"{dir_path}/{FPS_FILE_NAME}",
                    "w",
                    encoding="utf-8",
                ) as file:
                    file.write(fps)

    # lets add some metadata to the results
    system_spec = subprocess.run(
        ["inxi", "-C", "-G", "-m", "-S"], check=True, capture_output=True, text=True
    )
    end_time = datetime.now().strftime("%Y-%m-%d-%H:%M:%S")
    total_time = datetime.strptime(end_time, "%Y-%m-%d-%H:%M:%S") - datetime.strptime(
        start_time, "%Y-%m-%d-%H:%M:%S"
    )

    with open(
        f"{RESULTS_DIR}/{start_time}/metadata.txt", "w", encoding="utf-8"
    ) as file:
        file.write("=== System Information ===\n")
        file.write(f"Seed       : {args.seed}\n")
        file.write(f"{system_spec.stdout}\n\n")

        file.write("=== Benchmark Results ===\n")
        file.write(f"Start Time : {start_time}\n")
        file.write(f"End Time   : {end_time}\n")
        file.write(f"Total Time : {total_time} seconds\n")

    spinner.stop()
