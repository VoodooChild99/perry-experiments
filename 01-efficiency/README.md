# Perry Efficiency Experiment
This experiment uses `Perry` to synthesize hardware models for targets listed in Table 2. The consumed time to synthesize each model is collected to measure `Perry`'s efficiency. We conduct experiments for each MCU 5 times as stated in the paper (Section 5.2 "Efficiency").

## Running the experiment
Execute the following commands:
```shell
PERRY_PATH=/path/to/perry ./run.sh
```

## Expected Results
`01-efficiency/result.csv` records the time `Perry` consumes to synthesize a hardware model for a target, which should be comparable with Figure 3 in the paper.

There will also be 5 directories `exp-1` ~ `exp-5`, corresponding to the 5 rounds of experiments. Within each directory, you can find the inferred hardware behaviors (`*.json`), the generated hardware models (`*.c`), and the generated logs (`*.log`).