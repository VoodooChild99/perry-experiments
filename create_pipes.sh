#!/bin/bash

DIR=$(dirname "$(realpath "${BASH_SOURCE[0]}")")

if [ $# -ne 1 ]; then
    echo "Usage: create_pipes.sh [num]"
    exit 1
fi

for i in $(seq 1 "$1"); do
    if [ -e "$DIR/$i.in" ]; then
        rm -f "$DIR/$i.in"
    fi
    if [ -e "$DIR/$i.out" ]; then
        rm -f "$DIR/$i.out"
    fi
    mkfifo "$DIR/$i.in"
    mkfifo "$DIR/$i.out"
done

