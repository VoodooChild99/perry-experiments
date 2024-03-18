#!/bin/bash

DIR=$(dirname "$(realpath "${BASH_SOURCE[0]}")")

if [ -f "$DIR"/result.csv ]; then
    rm "$DIR"/result.csv
fi
touch "$DIR"/result.csv
echo "target, round, time" >> "$DIR"/result.csv

if [ -z "$PERRY_PATH" ]; then
    echo "[x] Please set PERRY_PATH to perry directory"
    exit 1
fi

REPEAT=5
targets=(STM32F072 STM32F103 STM32F407 STM32F769 STM32L073 NXP-FRDM-K22F NXP-FRDM-K64F NXP-FRDM-K82F NXP-FRDM-KL25Z NXP-LPCXpresso51U68 SAM4E-Xplained-Pro SAM4L-EK SAM4S-Xplained SAME70-Xplained SAMV71-Xplained-Ultra SAM3X8E)
for target in "${targets[@]}"
do
    for i in $(seq 1 $REPEAT); do
        echo "[+] running Perry on $target, round $i"
        begin_time=$(date +%s%N)
        "$DIR"/run_one.sh "$PERRY_PATH"/synthesizer/example/"$target" "$DIR" &> "$DIR"/"$target".log
        end_time=$(date +%s%N)
        elapsed_time=$(python -c "print(($end_time - $begin_time) / 1000000000)")
        echo "$target, $i, $elapsed_time" >> "$DIR"/result.csv
    done
done