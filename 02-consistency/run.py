import argparse
import tomlkit
import os
import subprocess
import re

from loguru import logger
from pathlib import Path

ROOT = Path(os.path.dirname(os.path.abspath(__file__)))
GROUNDTRUTH_PATH = ROOT / "groundtruth.toml"
BIN_PATH = ROOT / "p2im-unit-tests"

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

QEMU_LOG_REGEX = re.compile(r'Trace 0: 0x[0-9a-f]+ \[[0-9a-f]+/([0-9a-f]+)/[0-9a-f]+/[0-9a-f]+\] (.+)')

def run_one(bin, config, timeout):
    bin_path = BIN_PATH / "{}.elf".format(bin)
    board = config['board']
    input = config['input']
    unit_tests = config['unittest']
    real_qemu_cmd = QEMU_CMD + ['-kernel', str(bin_path), '-machine', board]
    run_pipe_args = ["{}/run_pipes.sh".format(ROOT.parent), "5"]
    for i, id in enumerate(input):
        if not isinstance(id, str):
            # check for value
            interface = id['if']
            value = id['value']
            real_qemu_cmd += ['-chardev', 'pipe,id={},path={}'.format(interface, ROOT.parent / '{}.in'.format(i + 1))]
            run_pipe_args += ["{}={:x}".format(i + 1, value)]
        else:
            real_qemu_cmd += ['-chardev', 'pipe,id={},path={}'.format(id, ROOT.parent / '{}.in'.format(i + 1))]
    # run pipes
    _ = subprocess.Popen(run_pipe_args, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
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
        return [], [{bin : ut} for ut in unit_tests]
    finally:
        # kill pipes
        os.system("killall run_pipes.sh")
    all_addr = set()
    all_fn = set()
    for line in outs.splitlines():
        res = QEMU_LOG_REGEX.match(line)
        if res is None:
            continue
        addr, fn = res.groups()
        addr = int(addr, base=16)
        all_addr.add(addr)
        all_fn.add(fn)
    fal = []
    suc = []
    for ut in unit_tests:
        passed = True
        for p in ut['pass']:
            if isinstance(p, int):
                if p not in all_addr:
                    fal.append({bin : ut})
                    passed = False
                    break
            else:
                assert(isinstance(p, str))
                if p not in all_fn:
                    fal.append({bin : ut})
                    passed = False
                    break
        if passed:
            suc.append({bin : ut})
    return suc, fal
    
DEFAULT_TIMEOUT=2

def main(args):
    with open(GROUNDTRUTH_PATH, 'r') as f:
        binaries = tomlkit.load(f)
    success = []
    failed = []
    for bin in binaries:
        if args.retry_i2c and 'I2C' not in bin:
            continue
        timeout = DEFAULT_TIMEOUT
        if bin == 'NUTTX-F103-I2C':
            logger.info("Testing NUTTX-F103-I2C takes more time, please be patient")
            timeout = 25
        elif bin == 'ARDUINO-F103-I2C':
            logger.info("Testing ARDUINO-F103-I2C takes more time, please be patient")
            timeout = 15
        elif bin == 'K64F-RIOT-I2C':
            logger.info("Testing K64F-RIOT-I2C takes more time, please be patient")
            timeout = 15
        suc, fal = run_one(bin, binaries[bin], timeout)
        if len(fal) == 0:
            logger.success("{}: all passed!".format(bin))
        else:
            logger.error("{} has failed unit tests: {}".format(bin, fal))
        success += suc
        failed += fal
    num_total_unit_tests = len(success) + len(failed)
    logger.success("All done!")
    logger.info("Passing rate: {} ({}/{})".format(len(success) / num_total_unit_tests, len(success), num_total_unit_tests))
    if num_total_unit_tests != len(success):
        logger.info("Failed unit tests: {}".format(failed))

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--retry-i2c', action='store_true', default=False)
    args = parser.parse_args()
    # create pipes
    os.system("{}/create_pipes.sh 5".format(ROOT.parent))
    try:
        main(args)
    except KeyboardInterrupt:
        logger.info("Interrupt by user, exit")