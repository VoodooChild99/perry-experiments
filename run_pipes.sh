#!/bin/bash

DIR=$(dirname "$(realpath "${BASH_SOURCE[0]}")")

on_exit() {
    kill $JOBS
    echo "[*] Bye"
    exit
}

trap on_exit INT

if [ $# -lt 1 ]; then
    echo "Usage: run_pipes.sh [num] <id=value> ..."
    exit 1
fi

NUM_PIPES=$1
shift

declare -A id_value_map
while [ $# -ne 0 ]; do
    key=${1%%=*}
    value=${1#*=}
    id_value_map[$key]=$value
    shift
done

for i in $(seq 1 "$NUM_PIPES"); do
    if [ ! -e "$DIR/$i.in" ]; then
        echo "[x] cannot run pipe $i"
    else
        found=0
        for key in ${!id_value_map[*]}; do
            if [ "$key" -eq "$i" ]; then
                echo "channel $key set to value 0x${id_value_map[$key]}"
                while true; do head -c 1 /dev/urandom > /dev/null; echo -n -e "\x${id_value_map[$key]}" > "$DIR"/"$i".in ; done &
                found=1
                break
            fi
        done
        if [ $found -eq 0 ]; then
            while true; do head -c 1 /dev/urandom > "$DIR"/"$i".in ; done &
        fi
    fi
done

JOBS=$(jobs -p)
echo "[*] Wating for Ctrl+C"
while true; do sleep 1; done