#!/bin/bash

DIR=$(dirname "$(realpath "${BASH_SOURCE[0]}")")

if [ -z "$FUZZ_QEMU_DIR" ]; then
    echo "[x] please set FUZZ_QEMU_DIR to qemu-system-fuzzing directory"
    exit 1
fi

if [ -z "$PERRY_OUTPUT_DIR" ]; then
    echo "[x] please set PERRY_OUTPUT_DIR to Perry output directory"
    exit 1
fi

cp "$PERRY_OUTPUT_DIR"/stm32f769.c "$FUZZ_QEMU_DIR"/hw/arm || exit 1

cp "$DIR"/default-fuzz.patch "$FUZZ_QEMU_DIR"/default.patch || exit 1
cp "$DIR"/kconfig-fuzz.patch "$FUZZ_QEMU_DIR"/kconfig.patch || exit 1
cp "$DIR"/meson-fuzz.patch "$FUZZ_QEMU_DIR"/meson.patch || exit 1

cd "$FUZZ_QEMU_DIR" || exit 1
git apply default.patch && git apply kconfig.patch && git apply meson.patch
QEMU_PATH=$FUZZ_QEMU_DIR python "$DIR"/patch.py

cd build && make -j$(nproc)
