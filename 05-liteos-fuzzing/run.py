import multiprocessing
import os
from pathlib import Path
import random
import shutil
import subprocess
import datetime


TIME_TO_FUZZ = 6 * 60 * 60
REPEAT = 1
ROOT_DIR = Path(os.path.dirname(os.path.abspath(__file__)))
RAND_INPUT_DIR = ROOT_DIR / "in_rand"
JSON_INPUT_DIR = ROOT_DIR / "in_json"
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

if 'AFL_PATH' in os.environ:    
    AFL_PATH = Path(os.environ['AFL_PATH']) / "afl-fuzz"
    if not AFL_PATH.exists():
        print("Failed to locate afl-fuzz under {}. Did you build it?".format(AFL_PATH))
        os._exit(1)
    AFL_PATH = str(AFL_PATH)
else:
    print("Set `AFL_PATH` to point to AFL directory")
    os._exit(1)
AFL_CMD = [
    AFL_PATH,
    "-t", "1000",
    "-m", "4G",
    "-d"
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
    "-afl-input-file", "@@",
    "-kernel"
]
NUM_CORES = multiprocessing.cpu_count()

def fuzz_one(q):
    while True:
        cmd = q.get()
        print("Fuzz for {1}: {0}".format(' '.join(cmd), datetime.timedelta(seconds=TIME_TO_FUZZ)))
        try:
            subprocess.run(
                cmd, timeout=TIME_TO_FUZZ,
                stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        except subprocess.TimeoutExpired:
            q.task_done()
        except KeyboardInterrupt:
            return

def main():
    tasks = []
    # get tasks
    for t in TARGETS:
        input_dir = JSON_INPUT_DIR if 'json' in t else RAND_INPUT_DIR
        elf_path = "{}/{}/Huawei_LiteOS.elf".format(ROOT_DIR, t)
        cur_qemu_cmd = QEMU_CMD + [elf_path]
        for i in range(0, REPEAT):
            output_dir = "{}/{}/out{}".format(ROOT_DIR, t, i)
            shutil.rmtree(output_dir, ignore_errors=True)
            cur_afl_cmd = AFL_CMD + [
                "-i", "{}".format(input_dir),
                "-o", "{}".format(output_dir)
            ]
            cur_cmd = cur_afl_cmd + cur_qemu_cmd
            tasks.append(cur_cmd)
    random.shuffle(tasks)
    # push tasks into the queue
    try:
        q = multiprocessing.JoinableQueue()
        for t in tasks:
            q.put(t)
        workers = []
        # spawn workers
        for i in range(0, NUM_CORES):
            p = multiprocessing.Process(target=fuzz_one, args=(q,))
            workers.append(p)
            p.start()
        q.join()
    except KeyboardInterrupt:
        print("[*] Interrupted by user, bye :)")
    finally:
        os.system("killall afl-fuzz")
        os.system("killall qemu-system-arm")
        for p in workers:
            p.kill()
            p.join()

if __name__ == "__main__":
    main()