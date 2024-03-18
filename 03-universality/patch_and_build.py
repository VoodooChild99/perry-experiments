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

def patch_sam4expro():
    sam4e_path = QEMU_HW_ARM_PATH / "atsam4e16e.c"
    if not sam4e_path.exists():
        logger.error("Failed to patch {} becuase it does not exist!".format(sam4e_path))
        return
    with open(sam4e_path, 'r') as f:
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
        if 'static void atsam4e16e_uart_write(void *opaque, hwaddr offset, uint64_t value, unsigned size)' in line:
            start_processed = True
            continue
        if 'qemu_log_mask(LOG_GUEST_ERROR, "ATSAM4E16E UART write: bad offset %x\\n", (int)offset);' in line:
            end_processed = True
            continue
        if start_processed and not end_processed:
            if 't->IDR = value;' in line and not idr_patched:
                # patch it
                new_lines.append('\t\t\t// PERRY PATCH\n')
                new_lines.append('\t\t\tt->IMR &= (~value); atsam4e16e_uart_update(t);\n')
                idr_patched = True
                continue
            if 't->IER = value;' in line and not ier_patched:
                # patch it
                new_lines.append('\t\t\t// PERRY PATCH\n')
                new_lines.append('\t\t\tt->IMR |= value; atsam4e16e_uart_update(t);\n')
                ier_patched = True
                continue
    if ier_patched and idr_patched:
        with open(sam4e_path, 'w') as f:
            f.writelines(new_lines)
        logger.success("{} patched".format(sam4e_path))
    else:
        logger.error("Failed to patch {}".format(sam4e_path))

def patch_sam4lek():
    sam4lek_path = QEMU_HW_ARM_PATH / "atsam4lc4c.c"
    if not sam4lek_path.exists():
        logger.error("Failed to patch {} becuase it does not exist!".format(sam4lek_path))
        return
    with open(sam4lek_path, 'r') as f:
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
        if 'static void atsam4lc4c_usart_write(void *opaque, hwaddr offset, uint64_t value, unsigned size)' in line:
            start_processed = True
            continue
        if 'qemu_log_mask(LOG_GUEST_ERROR, "ATSAM4E16E UART write: bad offset %x\\n", (int)offset);' in line:
            end_processed = True
            continue
        if start_processed and not end_processed:
            if 't->IDR = value;' in line and not idr_patched:
                # patch it
                new_lines.append('\t\t\t// PERRY PATCH\n')
                new_lines.append('\t\t\tt->IMR &= (~value); atsam4lc4c_usart_update(t);\n')
                idr_patched = True
                continue
            if 't->IER = value;' in line and not ier_patched:
                # patch it
                new_lines.append('\t\t\t// PERRY PATCH\n')
                new_lines.append('\t\t\tt->IMR |= value; atsam4lc4c_usart_update(t);\n')
                ier_patched = True
                continue
    if ier_patched and idr_patched:
        with open(sam4lek_path, 'w') as f:
            f.writelines(new_lines)
        logger.success("{} patched".format(sam4lek_path))
    else:
        logger.error("Failed to patch {}".format(sam4lek_path))

def patch_sam4s():
    sam4s_path = QEMU_HW_ARM_PATH / "atsam4s16c.c"
    if not sam4s_path.exists():
        logger.error("Failed to patch {} becuase it does not exist!".format(sam4s_path))
        return
    with open(sam4s_path, 'r') as f:
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
        if 'static void atsam4s16c_uart_write(void *opaque, hwaddr offset, uint64_t value, unsigned size)' in line:
            start_processed = True
            continue
        if 'qemu_log_mask(LOG_GUEST_ERROR, "ATSAM4S16C UART write: bad offset %x\\n", (int)offset);' in line:
            end_processed = True
            continue
        if start_processed and not end_processed:
            if 't->IDR = value;' in line and not idr_patched:
                # patch it
                new_lines.append('\t\t\t// PERRY PATCH\n')
                new_lines.append('\t\t\tt->IMR &= (~value); atsam4s16c_uart_update(t);\n')
                idr_patched = True
                continue
            if 't->IER = value;' in line and not ier_patched:
                # patch it
                new_lines.append('\t\t\t// PERRY PATCH\n')
                new_lines.append('\t\t\tt->IMR |= value; atsam4s16c_uart_update(t);\n')
                ier_patched = True
                continue
    if ier_patched and idr_patched:
        with open(sam4s_path, 'w') as f:
            f.writelines(new_lines)
        logger.success("{} patched".format(sam4s_path))
    else:
        logger.error("Failed to patch {}".format(sam4s_path))

def patch_same70():
    same70_path = QEMU_HW_ARM_PATH / "atsame70q21b.c"
    if not same70_path.exists():
        logger.error("Failed to patch {} becuase it does not exist!".format(same70_path))
        return
    with open(same70_path, 'r') as f:
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
        if 'static void atsame70q21b_usart_write(void *opaque, hwaddr offset, uint64_t value, unsigned size)' in line:
            start_processed = True
            continue
        if 'qemu_log_mask(LOG_GUEST_ERROR, "ATSAME70Q21B USART write: bad offset %x\\n", (int)offset);' in line:
            end_processed = True
            continue
        if start_processed and not end_processed:
            if 't->US_IDR_USART_MODE = value;' in line and not idr_patched:
                # patch it
                new_lines.append('\t\t\t// PERRY PATCH\n')
                new_lines.append('\t\t\tt->US_IMR_USART_MODE &= (~value); atsame70q21b_usart_update(t);\n')
                idr_patched = True
                continue
            if 't->US_IER_USART_MODE = value;' in line and not ier_patched:
                # patch it
                new_lines.append('\t\t\t// PERRY PATCH\n')
                new_lines.append('\t\t\tt->US_IMR_USART_MODE |= value; atsame70q21b_usart_update(t);\n')
                ier_patched = True
                continue
    if ier_patched and idr_patched:
        with open(same70_path, 'w') as f:
            f.writelines(new_lines)
        logger.success("{} patched".format(same70_path))
    else:
        logger.error("Failed to patch {}".format(same70_path))

def patch_samv71():
    samv71_path = QEMU_HW_ARM_PATH / "atsamv71q21b.c"
    if not samv71_path.exists():
        logger.error("Failed to patch {} becuase it does not exist!".format(samv71_path))
        return
    with open(samv71_path, 'r') as f:
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
        if 'static void atsamv71q21b_usart_write(void *opaque, hwaddr offset, uint64_t value, unsigned size)' in line:
            start_processed = True
            continue
        if 'qemu_log_mask(LOG_GUEST_ERROR, "ATSAMV71Q21B USART write: bad offset %x\\n", (int)offset);' in line:
            end_processed = True
            continue
        if start_processed and not end_processed:
            if 't->US_IDR_USART_MODE = value;' in line and not idr_patched:
                # patch it
                new_lines.append('\t\t\t// PERRY PATCH\n')
                new_lines.append('\t\t\tt->US_IMR_USART_MODE &= (~value); atsamv71q21b_usart_update(t);\n')
                idr_patched = True
                continue
            if 't->US_IER_USART_MODE = value;' in line and not ier_patched:
                # patch it
                new_lines.append('\t\t\t// PERRY PATCH\n')
                new_lines.append('\t\t\tt->US_IMR_USART_MODE |= value; atsamv71q21b_usart_update(t);\n')
                ier_patched = True
                continue
    if ier_patched and idr_patched:
        with open(samv71_path, 'w') as f:
            f.writelines(new_lines)
        logger.success("{} patched".format(samv71_path))
    else:
        logger.error("Failed to patch {}".format(samv71_path))

def patch_stm32f0():
    stm32f0_path = QEMU_HW_ARM_PATH / "stm32f072.c"
    if not stm32f0_path.exists():
        logger.error("Failed to patch {} becuase it does not exist!".format(stm32f0_path))
        return
    with open(stm32f0_path, 'r') as f:
        lines  = f.readlines()
    start_processed = False
    struct_patched = False
    write_patched = False
    init_patched = False
    end_processed = False
    new_lines = []
    for line in lines:
        new_lines.append(line)
        if end_processed or (struct_patched and write_patched and init_patched):
            continue
        if 'struct STM32F0SYSCFG {' in line:
            start_processed = True
            continue
        if '#define STM32F0_SYSCFG_SIZE' in line:
            end_processed = True
            continue
        if start_processed and not end_processed:
            if 'uint32_t base;' in line and not struct_patched:
                # patch it
                new_lines.append('\t// PERRY PATCH\n')
                new_lines.append('\tARMCPU *cpu;\n')
                struct_patched = True
                continue
        if 't->CFGR1 = value;' in line and not write_patched:
            new_lines.append('\t\t\t// PERRY PATCH\n')
            new_lines.append('\t\t\tif ((value & 3) == 3) { t->cpu->env.v7m.vecbase[0] = 0x20000000; }\n')
            write_patched = True
            continue
        if 'sysbus_mmio_map(SYS_BUS_DEVICE(p8), 0, 0x40010000);' in line and not init_patched:
            new_lines.append('\t// PERRY PATCH\n')
            new_lines.append('\tp8->cpu = ARM_CPU(first_cpu);\n')
            init_patched = True
            continue
    
    lines = new_lines
    new_lines = []
    start_processed = False
    patched = False
    end_processed = False
    for line in lines:
        new_lines.append(line)
        if end_processed or patched:
            continue
        if 'if ((!(value & 0x40000))) {' in line:
            start_processed = True
            new_lines.pop()
            new_lines.append('\t\t\t// PERRY PATCH\n')
            new_lines.append('\t\t\t// {}\n'.format(line.strip()))
            new_lines.append('\t\t\tif ((!(value & 0x10000))) {\n')
            continue
        if '}' in line:
            end_processed = True
            continue
        if start_processed and not end_processed:
            if 't->CR &= (~(0x2000000));' in line and not patched:
                # patch it
                new_lines.pop()
                new_lines.append('\t\t\t\t// {}\n'.format(line.strip()))
                patched = True

    if struct_patched and write_patched and init_patched and patched:
        with open(stm32f0_path, 'w') as f:
            f.writelines(new_lines)
        logger.success("{} patched".format(stm32f0_path))
    else:
        logger.error("Failed to patch {}".format(stm32f0_path))

def patch_stm32f4():
    stm32f4_path = QEMU_HW_ARM_PATH / "stm32f407.c"
    if not stm32f4_path.exists():
        logger.error("Failed to patch {} becuase it does not exist!".format(stm32f4_path))
        return
    with open(stm32f4_path, 'r') as f:
        lines  = f.readlines()
    start_processed = False
    patched = False
    end_processed = False
    new_lines = []
    for line in lines:
        new_lines.append(line)
        if end_processed or patched:
            continue
        if 'static void stm32f4_rcc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size)' in line:
            start_processed = True
            continue
        if 'qemu_log_mask(LOG_GUEST_ERROR, "STM32F4 RCC write: bad offset %x\\n", (int)offset);' in line:
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
    if patched:
        with open(stm32f4_path, 'w') as f:
            f.writelines(new_lines)
        logger.success("{} patched".format(stm32f4_path))
    else:
        logger.error("Failed to patch {}".format(stm32f4_path))

def patch_stm32f7():
    stm32f7_path = QEMU_HW_ARM_PATH / "stm32f769.c"
    if not stm32f7_path.exists():
        logger.error("Failed to patch {} becuase it does not exist!".format(stm32f7_path))
        return
    with open(stm32f7_path, 'r') as f:
        lines  = f.readlines()
    start_processed = False
    patched = False
    end_processed = False
    new_lines = []
    for line in lines:
        new_lines.append(line)
        if end_processed or patched:
            continue
        if 'static void stm32f7_rcc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size)' in line:
            start_processed = True
            continue
        if 'qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 RCC write: bad offset %x\\n", (int)offset);' in line:
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
    if patched:
        with open(stm32f7_path, 'w') as f:
            f.writelines(new_lines)
        logger.success("{} patched".format(stm32f7_path))
    else:
        logger.error("Failed to patch {}".format(stm32f7_path))

def patch_stm32l0():
    stm32l0_path = QEMU_HW_ARM_PATH / "stm32l073.c"
    if not stm32l0_path.exists():
        logger.error("Failed to patch {} becuase it does not exist!".format(stm32l0_path))
        return
    with open(stm32l0_path, 'r') as f:
        lines  = f.readlines()
    start_processed = False
    struct_patched = False
    write_patched = False
    init_patched = False
    end_processed = False
    new_lines = []
    for line in lines:
        new_lines.append(line)
        if end_processed or (struct_patched and write_patched and init_patched):
            continue
        if 'struct STM32L0SYSCFG {' in line:
            start_processed = True
            continue
        if '#define STM32L0_SYSCFG_SIZE' in line:
            end_processed = True
            continue
        if start_processed and not end_processed:
            if 'uint32_t base;' in line and not struct_patched:
                # patch it
                new_lines.append('\t// PERRY PATCH\n')
                new_lines.append('\tARMCPU *cpu;\n')
                struct_patched = True
                continue
        if 't->CFGR1 = value;' in line and not write_patched:
            new_lines.append('\t\t\t// PERRY PATCH\n')
            new_lines.append('\t\t\tif ((value & 3) == 3) { t->cpu->env.v7m.vecbase[0] = 0x20000000; }\n')
            write_patched = True
            continue
        if 'sysbus_mmio_map(SYS_BUS_DEVICE(p7), 0, 0x40010000);' in line and not init_patched:
            new_lines.append('\t// PERRY PATCH\n')
            new_lines.append('\tp7->cpu = ARM_CPU(first_cpu);\n')
            init_patched = True
            continue

    if struct_patched and write_patched and init_patched:
        with open(stm32l0_path, 'w') as f:
            f.writelines(new_lines)
        logger.success("{} patched".format(stm32l0_path))
    else:
        logger.error("Failed to patch {}".format(stm32l0_path))

if __name__ == '__main__':
    patch_sam4expro()
    patch_sam4lek()
    patch_sam4s()
    patch_same70()
    patch_samv71()
    patch_stm32f0()
    patch_stm32f4()
    patch_stm32f7()
    patch_stm32l0()
    os.system("cd {}/build && make -j$(nproc)".format(QEMU_DIR))
