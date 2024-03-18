import os

from pathlib import Path
from loguru import logger

if 'QEMU_PATH' in os.environ:    
    QEMU_HW_ARM_PATH = Path(os.environ['QEMU_PATH']) / "hw/arm"
    if not QEMU_HW_ARM_PATH.exists():
        print("Failed to locate hw/arm directory under {}. This is rare.".format(QEMU_HW_ARM_PATH))
        os._exit(1)
else:
    print("Set `QEMU_PATH` to point to QEMU directory")
    os._exit(1)


if __name__ == '__main__':
    stm32f7_path = QEMU_HW_ARM_PATH / "stm32f769.c"
    if not stm32f7_path.exists():
        logger.error("Failed to patch {} becuase it does not exist!".format(stm32f7_path))
        os._exit(1)
    with open(stm32f7_path, 'r') as f:
        lines  = f.readlines()
    patched = False
    new_lines = []
    for line in lines:
        new_lines.append(line)
        if patched:
            continue
        if 'armv7m_load_kernel' in line:
            new_lines.pop()
            new_lines.append('\t// PATCH FOR OLDER QEMU VERSION\n')
            new_lines.append('\t// {}\n'.format(line.strip()))
            new_lines.append('\tarmv7m_load_kernel(ARM_CPU(first_cpu), machine->kernel_filename, 0x800000);\n')
            patched = True
    
    lines = new_lines
    new_lines = []
    start_processed = False
    rcc_patched = False

    for line in lines:
        new_lines.append(line)
        if end_processed or rcc_patched:
            continue
        if 'static void stm32f7_rcc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size)' in line:
            start_processed = True
            continue
        if 'qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 RCC write: bad offset %x\\n", (int)offset);' in line:
            end_processed = True
            continue
        if start_processed and not end_processed:
            if 'if ((!(value & 0x40000))) {' in line and not rcc_patched:
                # patch it
                new_lines.pop()
                new_lines.append('\t\t\t// PERRY PATCH\n')
                new_lines.append('\t\t\t// {}\n'.format(line.strip()))
                new_lines.append('\t\t\tif ((!(value & 0x10000))) {\n')
                rcc_patched = True

    end_processed = False
    if patched and rcc_patched:
        with open(stm32f7_path, 'w') as f:
            f.writelines(new_lines)
        logger.success("{} patched".format(stm32f7_path))
    else:
        logger.error("Failed to patch {}".format(stm32f7_path))