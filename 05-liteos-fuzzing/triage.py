import argparse
from pathlib import Path
import os
import subprocess

ROOT_DIR = Path(os.path.dirname(os.path.abspath(__file__)))
TARGETS = [
    "lwm2m_coap_parse_message",
    "lwm2m_data_parse_json",
    "lwm2m_data_parse_tlv",
    "mqtt_ack",
    "mqtt_connack",
    "mqtt_connect",
    "mqtt_pub",
    "mqtt_suback",
    "mqtt_subscribe",
    "mqtt_unsuback",
    "mqtt_unsubscribe",
]

if 'QEMU_PATH' in os.environ:    
    QEMU_PATH = Path(os.environ['QEMU_PATH']) / "build/qemu-system-arm"
    if not QEMU_PATH.exists():
        print("Failed to locate qemu-system-arm under {}. Did you build it?".format(QEMU_PATH))
        os._exit(1)
    QEMU_PATH = str(QEMU_PATH)
else:
    print("Set `QEMU_PATH` to point to QEMU directory")
    os._exit(1)
QEMU_CMD = [
    QEMU_PATH,
    "-icount", "shift=0,align=off,sleep=off",
    "-machine", "stm32f769",
    "-nographic",
    "-monitor", "null",
    "-serial", "null",
    "-chardev", "stdio,id=usart1"
]

def main(args):
    target = args.target
    ipt = args.input
    debug = args.debug
    elf_path = "{}/{}/Huawei_LiteOS.elf".format(ROOT_DIR, target)
    cur_qemu_cmd = QEMU_CMD + ['-kernel', elf_path]
    cur_qemu_cmd += ['-afl-input-file', ipt]
    if debug:
        cur_qemu_cmd += ['-s', '-S']
    subprocess.run(cur_qemu_cmd)


def _to_abs_path(path: str) -> str:
    p = Path(path)
    if not p.exists():
        raise argparse.ArgumentTypeError('path {} does not exist'.format(p))
    return str(p.resolve())

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--target', choices=TARGETS, required=True, help="the fuzz target")
    parser.add_argument('input', type=_to_abs_path, help="the input to triage")
    parser.add_argument('-d', '--debug', action='store_true', default=False, help="debug with GDB")
    args = parser.parse_args()
    main(args)