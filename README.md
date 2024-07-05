# Perry Experiments

This directory contains the data required to reproduce the experiments described in our USENIX 2024 paper: *A Friend's Eye is A Good Mirror: Synthesizing MCU Peripheral Models from Peripheral Drivers*.

For driver source code we used in our experiments, please refer to [this repo](https://github.com/VoodooChild99/perry-drivers).

## Requirements
### Hardware Dependencies
* **Processor**: We used two Intel Xeon E5-2620 v2 CPUs (12 cores, 24 threads). Comparable hardware may also suffice.
* **Memory**: At least 64GB of RAM
* **Bluetooth**: The machine must be equipped with a Bluetooth device to replicate [04-cve-reproducing](./04-cve-reproduce)
### Software Dependencies
* **OS**: We used Ubuntu 20.04. Other systems are not tested.
* **Misc**: git and Docker

## Preparations
It is assumed that you have built Perry with Docker as described [here](https://github.com/VoodooChild99/perry?tab=readme-ov-file#build-with-docker).
All experiments should be conducted within the built container (execute `/perry/run_docker.sh` to launch a shell). This repository will be located under the `/root/perry-experiments` directory within the container.

## Directory Layout
| Directories/Files | Experiment |
| :--- | :--- |
| [01-efficiency](./01-efficiency) | Model synthesis time consumption as described in Section 5.2 ("Efficiency") |
| [02-consistency](./02-consistency) | P2IM unit tests passing rate as described in Section 5.2 ("Consistency" and "Scalability") |
| [03-universality](./03-universality) | Emulating various firmware samples as described in Section 5.2 ("Universality" and "Scalability") |
| [spec-violation-bugs.md](./spec-violation-bugs.md) | The specification violation bugs we found in drivers as described in Section 6.1 |
| [04-cve-reproducing](./04-cve-reproduce) | Reproducing firmware CVEs as described in Section 6.2 ("CS-II") |
| [05-liteos-fuzzing](./05-liteos-fuzzing) | Fuzzing LiteOS as described in Section 6.2 ("CS-III") |

## Running the Experiments
Please refer to the `README` files in [01-efficiency](./01-efficiency), [02-consistency](./02-consistency), [03-universality](./03-universality), [04-cve-reproducing](./04-cve-reproduce) and [05-liteos-fuzzing](./05-liteos-fuzzing) for detailed instructions.
