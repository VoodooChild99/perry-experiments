# Perry Universality Experiment
In this experiment, we use `P2IM` firmware samples and several `zephyr`/`LiteOS` shell firmware samples to validate the universality of `Perry`-generated hardware models. The firmware samples are placed under [`firmware`](./firmware). [`config.toml`] records basic information about these firmware (e.g., input interfaces).

## Running the experiment
1. Fix models by applying patches and rebuild QEMU
```shell
QEMU_DIR=/path/to/qemu python patch.py
cd /path/to/qemu/build && make -j$(nproc)
```

2. Execute non-interactive firmware samples. Non-interactive firmware samples include: `CNC`, `Drone`, `Gateway`, `Heat_Press`, `PLC`, `Reflow_Oven`, `Robot`, `Soldering_Iron`, `Steering_Control`. Run these firmware samples with the following commands. Note that each firmware sample will be executed for 10 seconds, its execution traces will be recorded and placed under `*.trace`.
```shell
QEMU_DIR=/path/to/qemu python run_noninteractive.py
```

3. Execute interactive firmware samples. Interactive firmware samples include: `Console`, `*-shell`. Execute these firmware samples with the following command, and you should be able to see a shell launched and interact with it by entering and executing shell commands (e.g., `help`). Press `Ctrl-C` to exit.
```shell
# e.g., QEMU_DIR=/path/to/qemu python run.py Console
QEMU_DIR=/path/to/qemu python run_interactive.py [target]
```
