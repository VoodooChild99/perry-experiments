# Perry Efficiency Experiment
This is the experiment described in Section 5.2 ("Efficiency") of our paper, where we use `Perry` to synthesize hardware models for targets listed in Table 2. The consumed time to synthesize each model is collected to measure `Perry`'s efficiency. We conduct experiments for each MCU 5 times as stated in the paper (Section 5.2 "Efficiency").

## Estimated Time Consumption
* 100 compute-hours
* 5 human-minutes

> Estimated according to Figure 3 in the paper. 5 rounds, ~20 compute-hours per round -> 100 compute-hours

## Running the experiment
Execute the following commands. If you are running within a container, `PERRY_PATH=/root/perry`
```shell
# change directory
cd perry-experiments/01-efficiency
# Then run the experiment
PERRY_PATH=/path/to/perry ./run.sh
```

## Expected Results
* `01-efficiency/result.csv` records the time `Perry` consumes to synthesize a hardware model for a target, which should be comparable with Figure 3 in the paper.

> There will also be 5 directories `exp-1` ~ `exp-5`, corresponding to the 5 rounds of experiments. Within each directory, you can find the inferred hardware behaviors (`*.json`), the generated hardware models (`*.c`), and the generated logs (`*.log`).