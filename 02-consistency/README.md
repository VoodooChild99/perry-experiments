# Perry Consistency Experiment
In this experiment, we use p2im unit tests to validate the consistency of `Perry`-generated hardware models. The unit test binaries are placed under [`p2im-unit-tests`](./p2im-unit-tests). [`groundtruth.toml`] records the involved unit tests and the passing conditions.

## Running the experiment
1. First, integrate `Perry`-generated hardware models into QEMU and rebuild it.
```shell
# PERRY_OUTPUT_DIR points to the directory where *.c files locate
QEMU_DIR=/path/to/qemu PERRY_OUTPUT_DIR=/path/to/perry_output_dir ./prepare_qemu_emu.sh
```

2. Run unit tests and collect the passing rate. Note that the passing rate and failed unit tests are directly printed on the screen.
```shell
QEMU_DIR=/path/to/qemu python run.py
```

3. Now apply patches to fix the models and run the unit tests again. You should be able to achieve a 100% passing rate. If some of the I2C unit tests fail (which could happen sometime), please retry running the unit tests multiple times.
```shell
# patch models
QEMU_DIR=/path/to/qemu python patch.py
# build QEMU
cd /path/to/qemu/build && make -j$(nproc)
# run the unit tests again
QEMU_DIR=/path/to/qemu python run.py
```