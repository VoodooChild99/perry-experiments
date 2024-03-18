#!/bin/bash

if [ -z "$PERRY_PATH" ]; then
    echo "[x] Please set PERRY_PATH to perry directory"
    exit 1
fi

if [ "$#" -ne 2 ]; then
    echo "Usage: run_exp.sh [config dir] [output dir]"
    exit 1
fi

if [ -z "$1" ] || [ ! -d "$1" ]; then
    echo "[x] config dir must exist"
    exit 1
fi

if [ -z "$2" ]; then
    echo "[x] output dir must not be empty"
    exit 1
fi

if [ ! -d "$2" ]; then
    echo "[*] output dir $2 does not exist, creating it..."
    mkdir -p "$2"
fi

python "$PERRY_PATH"/synthesizer/synthesize.py -c "$1"/config.yaml -o "$2" -a