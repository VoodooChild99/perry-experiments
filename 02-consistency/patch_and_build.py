import os

from pathlib import Path
from loguru import logger

if 'QEMU_DIR' in os.environ:    
    QEMU_HW_ARM_PATH = Path(os.environ['QEMU_DIR']) / "hw/arm"
    QEMU_DIR = Path(os.environ['QEMU_DIR'])
    if not QEMU_HW_ARM_PATH.exists():
        logger.error("Failed to locate hw/arm directory under {}. This is rare.".format(QEMU_HW_ARM_PATH))
        os._exit(1)
else:
    logger.info("Set `QEMU_DIR` to point to QEMU directory")
    os._exit(1)

def patch_k64f():
    k64f_path = QEMU_HW_ARM_PATH / "nxp_frdm_k64f.c"
    if not k64f_path.exists():
        logger.error("Failed to patch {} becuase it does not exist!".format(k64f_path))
        return
    with open(k64f_path, 'r') as f:
        lines  = f.readlines()
    start_processed = False
    patched = False
    end_processed = False
    new_lines = []
    for line in lines:
        new_lines.append(line)
        if end_processed or patched:
            continue
        if 'static void frdmk64f_i2c_update(FRDMK64FI2C *t)' in line:
            start_processed = True
            continue
        if 'static int frdmk64f_i2c_can_receive(void *opaque)' in line:
            end_processed = True
            continue
        if start_processed and not end_processed:
            if 'int conditions = ' in line and not patched:
                # patch it
                new_lines.append('\t// PERRY PATCH\n')
                new_lines.append('\tconditions = conditions && (t->C1 & 0x40);\n')
                patched = True
    if patched:
        with open(k64f_path, 'w') as f:
            f.writelines(new_lines)
        logger.success("{} patched".format(k64f_path))
    else:
        logger.error("Failed to patch {}".format(k64f_path))

def patch_atsam3x8e():
    sam3x8e_path = QEMU_HW_ARM_PATH / "atsam3x8e.c"
    if not sam3x8e_path.exists():
        logger.error("Failed to patch {} becuase it does not exist!".format(sam3x8e_path))
        return
    with open(sam3x8e_path, 'r') as f:
        lines  = f.readlines()
    start_processed = False
    ier_patched = False
    idr_patched = False
    end_processed = False
    new_lines = []
    for line in lines:
        new_lines.append(line)
        if end_processed or (ier_patched and idr_patched):
            continue
        if 'static void atsam3x8e_uart_write(void *opaque, hwaddr offset, uint64_t value, unsigned size)' in line:
            start_processed = True
            continue
        if 'qemu_log_mask(LOG_GUEST_ERROR, "ATSAM3X8E UART write: bad offset %x\\n", (int)offset);' in line:
            end_processed = True
            continue
        if start_processed and not end_processed:
            if 't->IDR = value;' in line and not idr_patched:
                # patch it
                new_lines.append('\t\t\t// PERRY PATCH\n')
                new_lines.append('\t\t\tt->IMR &= (~value); atsam3x8e_uart_update(t);\n')
                idr_patched = True
                continue
            if 't->IER = value;' in line and not ier_patched:
                # patch it
                new_lines.append('\t\t\t// PERRY PATCH\n')
                new_lines.append('\t\t\tt->IMR |= value; atsam3x8e_uart_update(t);\n')
                ier_patched = True
                continue
    if ier_patched and idr_patched:
        with open(sam3x8e_path, 'w') as f:
            f.writelines(new_lines)
        logger.success("{} patched".format(sam3x8e_path))
    else:
        logger.error("Failed to patch {}".format(sam3x8e_path))

def patch_stm32f1():
    stm32f1_path = QEMU_HW_ARM_PATH / "stm32f103.c"
    if not stm32f1_path.exists():
        logger.error("Failed to patch {} becuase it does not exist!".format(stm32f1_path))
        return
    with open(stm32f1_path, 'r') as f:
        lines  = f.readlines()
    start_processed = False
    patched = False
    end_processed = False
    new_lines = []
    for line in lines:
        new_lines.append(line)
        if end_processed or patched:
            continue
        if 'static void stm32f1_rcc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size)' in line:
            start_processed = True
            continue
        if 'qemu_log_mask(LOG_GUEST_ERROR, "STM32F1 RCC write: bad offset %x\\n", (int)offset);' in line:
            end_processed = True
            continue
        if start_processed and not end_processed:
            if 'if ((!(value & 0x40000))) {' in line and not patched:
                 # patch it
                new_lines.pop()
                new_lines.append('\t\t\t// PERRY PATCH\n')
                new_lines.append('\t\t\t// {}\n'.format(line.strip()))
                new_lines.append('\t\t\tif ((!(value & 0x10000))) {\n')
                patched = True
    
    lines = new_lines
    new_lines = []
    start_processed = False
    i2c_patched = False
    end_processed = False
    for line in lines:
        new_lines.append(line)
        if end_processed:
            continue
        if 'static void stm32f1_i2c_receive(void *opaque, const uint8_t *buf, int size)' in line:
            start_processed = True
            continue
        if 'stm32f1_i2c_update(t);' in line:
            end_processed = True
            new_lines.insert(-1, '\t// PERRY PATCH\n')
            new_lines.insert(-1, '\tt->SR2 &= (~(0x4));\n')
            i2c_patched = True
            continue
        if start_processed and not end_processed:
            if 't->SR1 |= 0x100;' in line or 't->SR1 |= 0x400;' in line or 't->SR1 |= 0x200;' in line or 't->SR1 |= 0x800;' in line or 't->SR1 &= (~(0x4));' in line:
                 # patch it
                new_lines.pop()
                new_lines.append('\t// PERRY PATCH: REMOVED\n')
                new_lines.append('\t// {}\n'.format(line.strip()))
                i2c_patched = True

    if patched and i2c_patched:
        with open(stm32f1_path, 'w') as f:
            f.writelines(new_lines)
        logger.success("{} patched".format(stm32f1_path))
    else:
        logger.error("Failed to patch {}".format(stm32f1_path))

if __name__ == '__main__':
    patch_k64f()
    patch_atsam3x8e()
    patch_stm32f1()
    os.system("cd {}/build && make -j$(nproc)".format(QEMU_DIR))
