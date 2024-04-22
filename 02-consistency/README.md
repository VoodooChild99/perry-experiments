# Perry Consistency Experiment
This is the experiment described in Section 5.2 ("Consistency" and "Scalability") of our paper, where we use P2IM unit tests passing rate to measure the consistency of `Perry`-generated hardware models. The unit test binaries are placed under [`02-consistency/p2im-unit-tests`](./p2im-unit-tests). [`groundtruth.toml`](./groundtruth.toml) records the involved unit tests and the passing conditions. 

## Preparation
You **MUST** finish the experiments in [`01-efficiency`](../01-efficiency/) to generate hardware models.

## Estimated Time Consumption
* 30 compute-minutes
* 10 human-minutes

## Running the experiment

### Running without manual intervention
**Steps:**

0. Change directory
```shell
cd perry-experiments/02-consistency
```

1. Integrate `Perry`-generated hardware models into QEMU and rebuild it. If you are running within a container, `QEMU_DIR=/root/qemu PERRY_OUTPUT_DIR=/root/perry-experiments/01-efficiency/exp-1`.
```shell
# PERRY_OUTPUT_DIR points to the directory where *.c files locate
QEMU_DIR=/path/to/qemu PERRY_OUTPUT_DIR=/path/to/perry_output_dir ./prepare_qemu_emu.sh
```

2. Run unit tests and collect the passing rate. Note that the passing rate and failed unit tests are directly printed on the screen. If you are running within a container, `QEMU_DIR=/root/qemu`.
```shell
QEMU_DIR=/path/to/qemu python run.py
```

**Expected results:**
You should be able to get a 49/66 (74.24%) passing rate at this point according to Table 3 in the paper.

### Running after fixing models
**Steps:**
1. Apply patches to fix the models. If you are running within a container, `QEMU_DIR=/root/qemu`.
```shell
QEMU_DIR=/path/to/qemu python patch_and_build.py
```

2. Run the unit tests again. If you are running within a container, `QEMU_DIR=/root/qemu`.
```shell
QEMU_DIR=/path/to/qemu python run.py
```
If some of the I2C unit tests fail (which could happen sometime), please try running the unit tests multiple times:
```shell
QEMU_DIR=/path/to/qemu python run.py --retry-i2c
```

**Expected results:**
You should be able to achieve a 100% passing rate according to Table 3 in the paper. 

To check the patches, you can search for the string `PERRY PATCH` within the patched model files (`/root/qemu/hw/arm/*.c`), which should be consistent with Table 3 ("Loc to Fix").
