#!/bin/bash

DIR=$(dirname "$(realpath "${BASH_SOURCE[0]}")")

if [ -z "$QEMU_DIR" ]; then
    echo "[x] please set QEMU_DIR to QEMU directory"
    exit 1
fi

if [ -z "$PERRY_OUTPUT_DIR" ]; then
    echo "[x] please set PERRY_OUTPUT_DIR to Perry output directory"
    exit 1
fi

cp "$PERRY_OUTPUT_DIR"/*.c "$QEMU_DIR"/hw/arm || exit 1

cp "$DIR"/default.patch "$QEMU_DIR"/default.patch || exit 1
cp "$DIR"/kconfig.patch "$QEMU_DIR"/kconfig.patch || exit 1
cp "$DIR"/meson.patch "$QEMU_DIR"/meson.patch || exit 1

cd "$QEMU_DIR" || exit 1
git apply default.patch && git apply kconfig.patch && git apply meson.patch

cd build && make -j$(nproc)