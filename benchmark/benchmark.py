import glob
import subprocess
from halo import Halo
from datetime import datetime
from pathlib import Path
import signal


SCENES_DIR = "./benchmark/scenes"
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


def benchmark(scene: str, dir_path: str) -> list[str]:
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
            "-w",
            scene,
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
    # make clean all
    spinner.text = "building"
    spinner.start()
    try:
        # Intentionally running a failing command
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

    # glob all files in scenes directory
    spinner.text = "getting scenes"
    scenes = glob.glob(f"{SCENES_DIR}/**/*.scene", recursive=True)
    scenes.sort()

    # FIXME: skipping 50x50x50 for now since it takes waaaay too long
    scenes = [scene for scene in scenes if "50x50x50" not in scene]

    start_time = datetime.now().strftime("%Y-%m-%d-%H:%M:%S")

    for scene in scenes:
        spinner.text = f"benchmarking {scene}"

        # the type of material, sparse or full, and the scene name
        scene_path = Path(scene)
        (material, scene_type, scene_name) = scene_path.relative_to(SCENES_DIR).parts[
            0:3
        ]

        dir_path = Path(
            f"{RESULTS_DIR}/{start_time}/{material}/{scene_type}/{scene_name}"
        )
        dir_path.mkdir(parents=True, exist_ok=True)

        # profile each file getting the avg fps, and flamegraph and more data???
        fps = "\n".join(benchmark(scene, str(dir_path)))

        with open(
            f"{dir_path}/{FPS_FILE_NAME}",
            "w",
            encoding="utf-8",
        ) as file:
            file.write(fps)

        # make folder in results with the timedate and save the results there
        spinner.text = "saving results"

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

        file.write(f"{system_spec.stdout}\n\n")

        file.write("=== Benchmark Results ===\n")
        file.write(f"Start Time : {start_time}\n")
        file.write(f"End Time   : {end_time}\n")
        file.write(f"Total Time : {total_time} seconds\n")

    spinner.stop()
