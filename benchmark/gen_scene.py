import argparse
from enum import Enum
import random


class Material(Enum):
    LAMBERTIAN = 1
    METAL = 2
    DIELECTRIC = 3
    EMISSIVE = 4


def make_scene(
    size: tuple[int, int, int], lambertian, metal, dielectric, emissive
) -> str:

    result = ""

    # get the materials
    materials = []
    if lambertian:
        materials.append([Material.LAMBERTIAN])
    if metal:
        materials.append(
            [
                Material.METAL,
                metal[0],
                metal[1],
            ]
        )
    if dielectric:
        materials.append([Material.DIELECTRIC, dielectric[0], dielectric[1]])
    if emissive:
        materials.append([Material.EMISSIVE, emissive[0], emissive[1]])

    x, y, z = size[0], size[1], size[2]
    for i in range(x):
        for j in range(y):
            for k in range(z):
                # pick a random material for the current position
                random_index = random.randint(0, len(materials) - 1)
                material = materials[random_index]
                # and generate random properties for it
                print(material)
                random_property = None
                if material[0] != Material.LAMBERTIAN:
                    random_property = (
                        random.randint(int(material[1] * 100), int(material[2] * 100))
                        / 100
                    )

                # pick random color
                r, g, b = random.random(), random.random(), random.random()

                # x y z 0(LAMBERTIAN)|1(METAL)|2(DIELECTRIC)|3(EMISSIVE) r g b fuzz|refraction_index|brightness
                if random_property is not None:
                    result += f"{i} {j} {k} {material[0].value} {r} {g} {b} {random_property}\n"
                else:
                    result += f"{i} {j} {k} {material[0].value} {r} {g} {b}\n"

    return result


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog="gen_scene", description="Generate a scene for benchmarking."
    )
    parser.add_argument(
        "-o",
        "--output",
        type=str,
        help="Output file for the generated scene.",
    )
    parser.add_argument(
        "-s",
        "--size",
        type=int,
        nargs=3,
        help="Size of the scene in the x, y and z dimensions.",
    )

    parser.add_argument(
        "-l",
        "--lambertian",
        action="store_true",
        help="Lambertian will be used",
    )

    parser.add_argument(
        "-m",
        "--metal",
        type=float,
        nargs=2,
        help="Metal will be used, and its fuzz factor will be in the range of the passed 2 vals",
    )
    parser.add_argument(
        "-d",
        "--dielectric",
        type=float,
        nargs=2,
        help="Dielectric will be used, and its refraction index will be in the range of the passed 2 vals",
    )
    parser.add_argument(
        "-e",
        "--emissive",
        type=float,
        nargs=2,
        help="Emissive will be used, and its brightness will be in the range of the passed 2 vals",
    )

    args = parser.parse_args()

    # make scene
    scene = make_scene(
        args.size, args.lambertian, args.metal, args.dielectric, args.emissive
    )

    # write to file
    writer = open(args.output, "w") if args.output else None

    writer.write(scene) if writer else print(scene)
