# BVH benchmark comparison

Baseline: `benchmark/results/2026-11-16-18:33:25`

BVH: `benchmark/results/2026-11-16-18:54:55`

| Material | Density | Size | Baseline mean FPS | BVH mean FPS | Speedup |
| --- | --- | --- | ---: | ---: | ---: |
| lambertian | full | 5x5x5 | 1.4221 | 10.8165 | 7.6058x |
| lambertian | full | 10x10x10 | 0.2127 | 8.2626 | 38.8529x |
| lambertian | sparse | 5x5x5 | 3.7631 | 11.8821 | 3.1576x |
| lambertian | sparse | 10x10x10 | 0.5139 | 8.8966 | 17.3135x |
| metal | full | 5x5x5 | 1.9421 | 10.7338 | 5.5269x |
| metal | full | 10x10x10 | 0.2591 | 8.2815 | 31.9634x |
| metal | sparse | 5x5x5 | 3.4938 | 11.1265 | 3.1846x |
| metal | sparse | 10x10x10 | 0.5022 | 7.3948 | 14.7248x |
| dielectric | full | 5x5x5 | 1.9661 | 10.7423 | 5.4639x |
| dielectric | full | 10x10x10 | 0.2582 | 8.2239 | 31.8459x |
| dielectric | sparse | 5x5x5 | 3.4872 | 11.1238 | 3.1899x |
| dielectric | sparse | 10x10x10 | 0.5087 | 7.3312 | 14.4128x |
| emissive | full | 5x5x5 | 1.9632 | 10.7373 | 5.4692x |
| emissive | full | 10x10x10 | 0.2622 | 8.2436 | 31.4451x |
| emissive | sparse | 5x5x5 | 3.4938 | 11.1364 | 3.1874x |
| emissive | sparse | 10x10x10 | 0.5017 | 7.3935 | 14.7364x |
| random | full | 5x5x5 | 1.9548 | 10.7977 | 5.5236x |
| random | full | 10x10x10 | 0.2626 | 8.2940 | 31.5839x |
| random | sparse | 5x5x5 | 2.6333 | 11.3318 | 4.3032x |
| random | sparse | 10x10x10 | 0.3094 | 8.2686 | 26.7262x |

Minimum speedup: 3.1576x

Median speedup: 11.0093x

Maximum speedup: 38.8529x
