import tomlkit
import os
import subprocess

from loguru import logger
from pathlib import Path

ROOT = Path(os.path.dirname(os.path.abspath(__file__)))
CONFIG_PATH = ROOT / "config.toml"
BIN_PATH = ROOT / "firmware"

if 'QEMU_DIR' in os.environ:    
    QEMU_DIR = Path(os.environ['QEMU_DIR']) / "build/qemu-system-arm"
    if not QEMU_DIR.exists():
        print("Failed to locate qemu-system-arm under {}. Did you build it?".format(QEMU_DIR))
        os._exit(1)
    QEMU_DIR = str(QEMU_DIR)
else:
    print("Set `QEMU_DIR` to point to QEMU directory")
    os._exit(1)
QEMU_CMD = [
    QEMU_DIR,
    "-nographic",
    "-monitor", "null",
    "-serial", "null",
]

def run_one(bin, config):
    bin_path = BIN_PATH / "{}.elf".format(bin)
    board = config['board']
    input = config['input']
    real_qemu_cmd = QEMU_CMD + ['-kernel', str(bin_path), '-machine', board]
    for id in input:
        real_qemu_cmd += ['-chardev', 'stdio,id={}'.format(id)]
    # run qemu
    p = subprocess.Popen(real_qemu_cmd)
    try:
        p.wait()
    except KeyboardInterrupt:
        p.kill()
        raise KeyboardInterrupt
    except Exception as e:
        logger.error("Unhandled exception when executing {}: {}".format(bin, e))
        return

def main():
    with open(CONFIG_PATH, 'r') as f:
        configs = tomlkit.load(f)
    for bin in configs:
        if 'shell' not in configs[bin] or not configs[bin]['shell']:
            continue
        logger.info("Execute {}".format(bin))
        try:
            run_one(bin, configs[bin])
        except KeyboardInterrupt:
            logger.info("Interrupt by user, run next one")
            continue

if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        logger.info("Interrupt by user, exit")