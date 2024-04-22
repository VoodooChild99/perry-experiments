# Perry Universality Experiment
This is the experiment described in Section 5.2 ("Universality" and "Scalability") of our paper, where we emulate `P2IM` firmware samples and several `Zephyr`/`LiteOS` shell firmware samples to validate the universality of `Perry`-generated hardware models. The firmware samples are placed under [`firmware`](./firmware). [`config.toml`](./config.toml) records basic information about these firmware (e.g., input interfaces).

## Preparation
You **MUST** finish the experiments in [`02-consistency`](../02-consistency).

## Estimated Time Consumption
* 20 compute-minutes
* 20 human-minutes

## Running the experiment
0. Change directory
```shell
cd perry-experiments/03-universality
```

1. Fix models by applying patches and rebuild QEMU. If you are running within a container, `QEMU_DIR=/root/qemu`.
```shell
QEMU_DIR=/path/to/qemu python patch_and_build.py
```

2. Execute non-interactive firmware samples. Non-interactive firmware samples include: `CNC`, `Drone`, `Gateway`, `Heat_Press`, `PLC`, `Reflow_Oven`, `Robot`, `Soldering_Iron`, `Steering_Control`. Run these firmware samples with the following commands. Note that each firmware sample will be executed for 10 seconds, its execution traces will be recorded and placed under `*.trace`. If you are running within a container, `QEMU_DIR=/root/qemu`.
```shell
QEMU_DIR=/path/to/qemu python run_noninteractive.py
```

3. Execute interactive firmware samples. Interactive firmware samples include: `Console`, `*-shell`. Execute these firmware samples with the following command, and you should be able to see a shell launched. You can interact with it by entering and executing shell commands (e.g., `help`). Press `Ctrl-C` to interrupt emulating the current firmware and switch to the next firmware. If you are running within a container, `QEMU_DIR=/root/qemu`.
```shell
QEMU_DIR=/path/to/qemu python run_interactive.py
```
## Expected Results
* For non-interactive firmware samples, you should be able to get full execution traces (`*.trace`) indicating successful emulation.
* For interactive firmware samples, you should be able to interact with the emulated shell.

> To check the patches, you can search for the string `PERRY PATCH` within the patched model files (`/root/qemu/hw/arm/*.c`), which should be consistent with Table 5 ("Loc to Fix").