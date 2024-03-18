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
    "-d", "exec",
]

def run_one(bin, config, timeout):
    bin_path = BIN_PATH / "{}.elf".format(bin)
    board = config['board']
    input = config['input']
    real_qemu_cmd = QEMU_CMD + ['-kernel', str(bin_path), '-machine', board]
    run_pipe_args = ["{}/run_pipes.sh".format(ROOT.parent), "5"]
    for i, id in enumerate(input):
        real_qemu_cmd += ['-chardev', 'pipe,id={},path={}'.format(id, ROOT.parent / '{}.in'.format(i + 1))]
    # run pipes
    _ = subprocess.Popen(run_pipe_args, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    # run qemu
    p = subprocess.Popen(
        real_qemu_cmd,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.PIPE,
        encoding="utf-8"
    )
    try:
        _, outs = p.communicate(timeout=timeout)
    except KeyboardInterrupt:
        p.kill()
        raise KeyboardInterrupt
    except subprocess.TimeoutExpired:
        p.kill()
        _, outs = p.communicate()
    except Exception as e:
        logger.error("Unhandled exception when testing {}: {}".format(bin, e))
        return
    finally:
        # kill pipes
        os.system("killall run_pipes.sh")
    with open(ROOT / "{}.trace".format(bin), 'w') as f:
        f.write(outs)

DEFAULT_TIMEOUT = 10

def main():
    with open(CONFIG_PATH, 'r') as f:
        configs = tomlkit.load(f)
    for bin in configs:
        if 'shell' in configs[bin] and configs[bin]['shell']:
            continue
        timeout = DEFAULT_TIMEOUT
        logger.info("Execute {} for {} seconds".format(bin, timeout))
        run_one(bin, configs[bin], timeout)

if __name__ == '__main__':
    # create pipes
    os.system("{}/create_pipes.sh 5".format(ROOT.parent))
    try:
        main()
    except KeyboardInterrupt:
        logger.info("Interrupt by user, exit")