#include "qemu/osdep.h"
#include "qemu/log.h"
#include "qemu/bitops.h"
#include "hw/sysbus.h"
#include "hw/irq.h"
#include "migration/vmstate.h"
#include "hw/registerfields.h"
#include "hw/resettable.h"
#include "hw/qdev-properties-system.h"
#include "exec/cpu-common.h"
#include "qom/object.h"
#include "qemu/units.h"
#include "qapi/error.h"
#include "hw/arm/boot.h"
#include "hw/arm/armv7m.h"
#include "hw/boards.h"
#include "exec/address-spaces.h"
#include "hw/misc/unimp.h"
#include "hw/clock.h"
#include "hw/qdev-clock.h"
#include "sysemu/sysemu.h"
#include "chardev/char-fe.h"
#include "net/net.h"
#include "net/eth.h"
#include "sysemu/dma.h"


#define TYPE_STM32F7_PWR "stm32f7-pwr"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F7PWR, STM32F7_PWR)

struct STM32F7PWR {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/*registers*/
	uint32_t CR1;
	uint32_t CSR1;
	uint32_t CR2;
	uint32_t CSR2;
	/* base */
	uint32_t base;

};
#define STM32F7_PWR_SIZE				0x10

REG32(PWR_CR1, 0x0)
	FIELD(PWR_CR1, LPDS, 0, 1)
	FIELD(PWR_CR1, PDDS, 1, 1)
	FIELD(PWR_CR1, CSBF, 3, 1)
	FIELD(PWR_CR1, PVDE, 4, 1)
	FIELD(PWR_CR1, PLS, 5, 3)
	FIELD(PWR_CR1, DBP, 8, 1)
	FIELD(PWR_CR1, FPDS, 9, 1)
	FIELD(PWR_CR1, LPUDS, 10, 1)
	FIELD(PWR_CR1, MRUDS, 11, 1)
	FIELD(PWR_CR1, ADCDC1, 13, 1)
	FIELD(PWR_CR1, VOS, 14, 2)
	FIELD(PWR_CR1, ODEN, 16, 1)
	FIELD(PWR_CR1, ODSWEN, 17, 1)
	FIELD(PWR_CR1, UDEN, 18, 2)
REG32(PWR_CSR1, 0x4)
	FIELD(PWR_CSR1, WUIF, 0, 1)
	FIELD(PWR_CSR1, SBF, 1, 1)
	FIELD(PWR_CSR1, PVDO, 2, 1)
	FIELD(PWR_CSR1, BRR, 3, 1)
	FIELD(PWR_CSR1, BRE, 9, 1)
	FIELD(PWR_CSR1, VOSRDY, 14, 1)
	FIELD(PWR_CSR1, ODRDY, 16, 1)
	FIELD(PWR_CSR1, ODSWRDY, 17, 1)
	FIELD(PWR_CSR1, UDRDY, 18, 2)
REG32(PWR_CR2, 0x8)
	FIELD(PWR_CR2, CWUPF1, 0, 1)
	FIELD(PWR_CR2, CWUPF2, 1, 1)
	FIELD(PWR_CR2, CWUPF3, 2, 1)
	FIELD(PWR_CR2, CWUPF4, 3, 1)
	FIELD(PWR_CR2, CWUPF5, 4, 1)
	FIELD(PWR_CR2, CWUPF6, 5, 1)
	FIELD(PWR_CR2, WUPP1, 8, 1)
	FIELD(PWR_CR2, WUPP2, 9, 1)
	FIELD(PWR_CR2, WUPP3, 10, 1)
	FIELD(PWR_CR2, WUPP4, 11, 1)
	FIELD(PWR_CR2, WUPP5, 12, 1)
	FIELD(PWR_CR2, WUPP6, 13, 1)
REG32(PWR_CSR2, 0xc)
	FIELD(PWR_CSR2, WUPF1, 0, 1)
	FIELD(PWR_CSR2, WUPF2, 1, 1)
	FIELD(PWR_CSR2, WUPF3, 2, 1)
	FIELD(PWR_CSR2, WUPF4, 3, 1)
	FIELD(PWR_CSR2, WUPF5, 4, 1)
	FIELD(PWR_CSR2, WUPF6, 5, 1)
	FIELD(PWR_CSR2, EWUP1, 8, 1)
	FIELD(PWR_CSR2, EWUP2, 9, 1)
	FIELD(PWR_CSR2, EWUP3, 10, 1)
	FIELD(PWR_CSR2, EWUP4, 11, 1)
	FIELD(PWR_CSR2, EWUP5, 12, 1)
	FIELD(PWR_CSR2, EWUP6, 13, 1)


static void stm32f7_pwr_register_reset(STM32F7PWR *t) {
	t->CR1 = 0xc000;
	t->CSR1 = 0x0;
	t->CR2 = 0x0;
	t->CSR2 = 0x0;

}

static uint64_t stm32f7_pwr_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F7PWR *t = STM32F7_PWR(opaque);
	uint64_t ret;

	switch (offset) {
		case A_PWR_CR1:
			ret = t->CR1;
			break;
		case A_PWR_CSR1:
			ret = t->CSR1;
			break;
		case A_PWR_CR2:
			ret = t->CR2;
			break;
		case A_PWR_CSR2:
			ret = t->CSR2;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 PWR read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f7_pwr_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F7PWR *t = STM32F7_PWR(opaque);


	switch (offset) {
		case A_PWR_CR1 ... A_PWR_CR1 + 3:
			value = value << ((offset - A_PWR_CR1) << 3);
			offset = A_PWR_CR1;
			break;
		case A_PWR_CSR1 ... A_PWR_CSR1 + 3:
			value = value << ((offset - A_PWR_CSR1) << 3);
			offset = A_PWR_CSR1;
			break;
		case A_PWR_CR2 ... A_PWR_CR2 + 3:
			value = value << ((offset - A_PWR_CR2) << 3);
			offset = A_PWR_CR2;
			break;
		case A_PWR_CSR2 ... A_PWR_CSR2 + 3:
			value = value << ((offset - A_PWR_CSR2) << 3);
			offset = A_PWR_CSR2;
			break;

		default: break;
	}

	switch (offset) {
		case A_PWR_CR1:
			t->CR1 = value;
			if ((value & 0x20000)) {
				t->CSR1 |= 0x20000;
			} else if ((!(value & 0x20000))) {
				t->CSR1 &= (~(0x20000));
			}
			if ((value & 0x10000)) {
				t->CSR1 |= 0x10000;
			} else if ((!(value & 0x10000))) {
				t->CSR1 &= (~(0x10000));
			}
			break;
		case A_PWR_CSR1:
			t->CSR1 = value;
			break;
		case A_PWR_CR2:
			t->CR2 = value;
			break;
		case A_PWR_CSR2:
			t->CSR2 = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 PWR write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f7_pwr_ops = {
	.read = stm32f7_pwr_read,
	.write = stm32f7_pwr_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f7_pwr_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F7PWR *t = STM32F7_PWR(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f7_pwr_ops, t, "stm32f7-pwr", STM32F7_PWR_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void stm32f7_pwr_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32f7_pwr_reset_enter(Object *obj, ResetType type) {
	STM32F7PWR *t = STM32F7_PWR(obj);
	stm32f7_pwr_register_reset(t);
}

static const VMStateDescription stm32f7_pwr_vmstate = {
	.name = "stm32f7-pwr",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR1, STM32F7PWR),
		VMSTATE_UINT32(CSR1, STM32F7PWR),
		VMSTATE_UINT32(CR2, STM32F7PWR),
		VMSTATE_UINT32(CSR2, STM32F7PWR),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f7_pwr_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f7_pwr_vmstate;
	dc->realize = stm32f7_pwr_realize;
	rc->phases.enter = stm32f7_pwr_reset_enter;

}

static const TypeInfo stm32f7_pwr_info = {
	.name = TYPE_STM32F7_PWR,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F7PWR),
	.instance_init = stm32f7_pwr_init,
	.class_init = stm32f7_pwr_class_init,
};

static void stm32f7_pwr_register_types(void) {
	type_register_static(&stm32f7_pwr_info);
}

type_init(stm32f7_pwr_register_types);

#define TYPE_STM32F7_SYSCFG "stm32f7-syscfg"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F7SYSCFG, STM32F7_SYSCFG)

struct STM32F7SYSCFG {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/*registers*/
	uint32_t MEMRM;
	uint32_t PMC;
	uint32_t EXTICR1;
	uint32_t EXTICR2;
	uint32_t EXTICR3;
	uint32_t EXTICR4;
	uint32_t CMPCR;
	/* base */
	uint32_t base;

};
#define STM32F7_SYSCFG_SIZE				0x24

REG32(SYSCFG_MEMRM, 0x0)
	FIELD(SYSCFG_MEMRM, MEM_MODE, 0, 3)
	FIELD(SYSCFG_MEMRM, FB_MODE, 8, 1)
	FIELD(SYSCFG_MEMRM, SWP_FMC, 10, 2)
REG32(SYSCFG_PMC, 0x4)
	FIELD(SYSCFG_PMC, MII_RMII_SEL, 23, 1)
	FIELD(SYSCFG_PMC, ADC1DC2, 16, 1)
	FIELD(SYSCFG_PMC, ADC2DC2, 17, 1)
	FIELD(SYSCFG_PMC, ADC3DC2, 18, 1)
REG32(SYSCFG_EXTICR1, 0x8)
	FIELD(SYSCFG_EXTICR1, EXTI3, 12, 4)
	FIELD(SYSCFG_EXTICR1, EXTI2, 8, 4)
	FIELD(SYSCFG_EXTICR1, EXTI1, 4, 4)
	FIELD(SYSCFG_EXTICR1, EXTI0, 0, 4)
REG32(SYSCFG_EXTICR2, 0xc)
	FIELD(SYSCFG_EXTICR2, EXTI7, 12, 4)
	FIELD(SYSCFG_EXTICR2, EXTI6, 8, 4)
	FIELD(SYSCFG_EXTICR2, EXTI5, 4, 4)
	FIELD(SYSCFG_EXTICR2, EXTI4, 0, 4)
REG32(SYSCFG_EXTICR3, 0x10)
	FIELD(SYSCFG_EXTICR3, EXTI11, 12, 4)
	FIELD(SYSCFG_EXTICR3, EXTI10, 8, 4)
	FIELD(SYSCFG_EXTICR3, EXTI9, 4, 4)
	FIELD(SYSCFG_EXTICR3, EXTI8, 0, 4)
REG32(SYSCFG_EXTICR4, 0x14)
	FIELD(SYSCFG_EXTICR4, EXTI15, 12, 4)
	FIELD(SYSCFG_EXTICR4, EXTI14, 8, 4)
	FIELD(SYSCFG_EXTICR4, EXTI13, 4, 4)
	FIELD(SYSCFG_EXTICR4, EXTI12, 0, 4)
REG32(SYSCFG_CMPCR, 0x20)
	FIELD(SYSCFG_CMPCR, READY, 8, 1)
	FIELD(SYSCFG_CMPCR, CMP_PD, 0, 1)


static void stm32f7_syscfg_register_reset(STM32F7SYSCFG *t) {
	t->MEMRM = 0x0;
	t->PMC = 0x0;
	t->EXTICR1 = 0x0;
	t->EXTICR2 = 0x0;
	t->EXTICR3 = 0x0;
	t->EXTICR4 = 0x0;
	t->CMPCR = 0x0;

}

static uint64_t stm32f7_syscfg_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F7SYSCFG *t = STM32F7_SYSCFG(opaque);
	uint64_t ret;

	switch (offset) {
		case A_SYSCFG_MEMRM:
			ret = t->MEMRM;
			break;
		case A_SYSCFG_PMC:
			ret = t->PMC;
			break;
		case A_SYSCFG_EXTICR1:
			ret = t->EXTICR1;
			break;
		case A_SYSCFG_EXTICR2:
			ret = t->EXTICR2;
			break;
		case A_SYSCFG_EXTICR3:
			ret = t->EXTICR3;
			break;
		case A_SYSCFG_EXTICR4:
			ret = t->EXTICR4;
			break;
		case A_SYSCFG_CMPCR:
			ret = t->CMPCR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 SYSCFG read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f7_syscfg_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F7SYSCFG *t = STM32F7_SYSCFG(opaque);


	switch (offset) {
		case A_SYSCFG_MEMRM ... A_SYSCFG_MEMRM + 3:
			value = value << ((offset - A_SYSCFG_MEMRM) << 3);
			offset = A_SYSCFG_MEMRM;
			break;
		case A_SYSCFG_PMC ... A_SYSCFG_PMC + 3:
			value = value << ((offset - A_SYSCFG_PMC) << 3);
			offset = A_SYSCFG_PMC;
			break;
		case A_SYSCFG_EXTICR1 ... A_SYSCFG_EXTICR1 + 3:
			value = value << ((offset - A_SYSCFG_EXTICR1) << 3);
			offset = A_SYSCFG_EXTICR1;
			break;
		case A_SYSCFG_EXTICR2 ... A_SYSCFG_EXTICR2 + 3:
			value = value << ((offset - A_SYSCFG_EXTICR2) << 3);
			offset = A_SYSCFG_EXTICR2;
			break;
		case A_SYSCFG_EXTICR3 ... A_SYSCFG_EXTICR3 + 3:
			value = value << ((offset - A_SYSCFG_EXTICR3) << 3);
			offset = A_SYSCFG_EXTICR3;
			break;
		case A_SYSCFG_EXTICR4 ... A_SYSCFG_EXTICR4 + 3:
			value = value << ((offset - A_SYSCFG_EXTICR4) << 3);
			offset = A_SYSCFG_EXTICR4;
			break;

		default: break;
	}

	switch (offset) {
		case A_SYSCFG_MEMRM:
			t->MEMRM = value;
			break;
		case A_SYSCFG_PMC:
			t->PMC = value;
			break;
		case A_SYSCFG_EXTICR1:
			t->EXTICR1 = value;
			break;
		case A_SYSCFG_EXTICR2:
			t->EXTICR2 = value;
			break;
		case A_SYSCFG_EXTICR3:
			t->EXTICR3 = value;
			break;
		case A_SYSCFG_EXTICR4:
			t->EXTICR4 = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 SYSCFG write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f7_syscfg_ops = {
	.read = stm32f7_syscfg_read,
	.write = stm32f7_syscfg_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f7_syscfg_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F7SYSCFG *t = STM32F7_SYSCFG(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f7_syscfg_ops, t, "stm32f7-syscfg", STM32F7_SYSCFG_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void stm32f7_syscfg_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32f7_syscfg_reset_enter(Object *obj, ResetType type) {
	STM32F7SYSCFG *t = STM32F7_SYSCFG(obj);
	stm32f7_syscfg_register_reset(t);
}

static const VMStateDescription stm32f7_syscfg_vmstate = {
	.name = "stm32f7-syscfg",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(MEMRM, STM32F7SYSCFG),
		VMSTATE_UINT32(PMC, STM32F7SYSCFG),
		VMSTATE_UINT32(EXTICR1, STM32F7SYSCFG),
		VMSTATE_UINT32(EXTICR2, STM32F7SYSCFG),
		VMSTATE_UINT32(EXTICR3, STM32F7SYSCFG),
		VMSTATE_UINT32(EXTICR4, STM32F7SYSCFG),
		VMSTATE_UINT32(CMPCR, STM32F7SYSCFG),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f7_syscfg_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f7_syscfg_vmstate;
	dc->realize = stm32f7_syscfg_realize;
	rc->phases.enter = stm32f7_syscfg_reset_enter;

}

static const TypeInfo stm32f7_syscfg_info = {
	.name = TYPE_STM32F7_SYSCFG,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F7SYSCFG),
	.instance_init = stm32f7_syscfg_init,
	.class_init = stm32f7_syscfg_class_init,
};

static void stm32f7_syscfg_register_types(void) {
	type_register_static(&stm32f7_syscfg_info);
}

type_init(stm32f7_syscfg_register_types);

#define TYPE_STM32F7_FLASH "stm32f7-flash"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F7FLASH, STM32F7_FLASH)

struct STM32F7FLASH {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t ACR;
	uint32_t KEYR;
	uint32_t OPTKEYR;
	uint32_t SR;
	uint32_t CR;
	uint32_t OPTCR;
	uint32_t OPTCR1;
	/* base */
	uint32_t base;

};
#define STM32F7_FLASH_SIZE				0x1c

REG32(FLASH_ACR, 0x0)
	FIELD(FLASH_ACR, LATENCY, 0, 4)
	FIELD(FLASH_ACR, PRFTEN, 8, 1)
	FIELD(FLASH_ACR, ARTEN, 9, 1)
	FIELD(FLASH_ACR, ARTRST, 11, 1)
REG32(FLASH_KEYR, 0x4)
	FIELD(FLASH_KEYR, KEY, 0, 32)
REG32(FLASH_OPTKEYR, 0x8)
	FIELD(FLASH_OPTKEYR, OPTKEY, 0, 32)
REG32(FLASH_SR, 0xc)
	FIELD(FLASH_SR, EOP, 0, 1)
	FIELD(FLASH_SR, OPERR, 1, 1)
	FIELD(FLASH_SR, WRPERR, 4, 1)
	FIELD(FLASH_SR, PGAERR, 5, 1)
	FIELD(FLASH_SR, PGPERR, 6, 1)
	FIELD(FLASH_SR, PGSERR, 7, 1)
	FIELD(FLASH_SR, BSY, 16, 1)
REG32(FLASH_CR, 0x10)
	FIELD(FLASH_CR, PG, 0, 1)
	FIELD(FLASH_CR, SER, 1, 1)
	FIELD(FLASH_CR, MER, 2, 1)
	FIELD(FLASH_CR, SNB, 3, 5)
	FIELD(FLASH_CR, PSIZE, 8, 2)
	FIELD(FLASH_CR, MER1, 15, 1)
	FIELD(FLASH_CR, STRT, 16, 1)
	FIELD(FLASH_CR, EOPIE, 24, 1)
	FIELD(FLASH_CR, ERRIE, 25, 1)
	FIELD(FLASH_CR, LOCK, 31, 1)
REG32(FLASH_OPTCR, 0x14)
	FIELD(FLASH_OPTCR, OPTLOCK, 0, 1)
	FIELD(FLASH_OPTCR, OPTSTRT, 1, 1)
	FIELD(FLASH_OPTCR, BOR_LEV, 2, 2)
	FIELD(FLASH_OPTCR, WWDG_SW, 4, 1)
	FIELD(FLASH_OPTCR, IWDG_SW, 5, 1)
	FIELD(FLASH_OPTCR, nRST_STOP, 6, 1)
	FIELD(FLASH_OPTCR, nRST_STDBY, 7, 1)
	FIELD(FLASH_OPTCR, RDP, 8, 8)
	FIELD(FLASH_OPTCR, nWRP, 16, 12)
	FIELD(FLASH_OPTCR, nDBOOT, 28, 1)
	FIELD(FLASH_OPTCR, nDBANK, 29, 1)
	FIELD(FLASH_OPTCR, IWDG_STDBY, 30, 1)
	FIELD(FLASH_OPTCR, IWDG_STOP, 31, 1)
REG32(FLASH_OPTCR1, 0x18)
	FIELD(FLASH_OPTCR1, BOOT_ADD0, 0, 16)
	FIELD(FLASH_OPTCR1, BOOT_ADD1, 16, 16)


static void stm32f7_flash_register_reset(STM32F7FLASH *t) {
	t->ACR = 0x0;
	t->KEYR = 0x0;
	t->OPTKEYR = 0x0;
	t->SR = 0x0;
	t->CR = 0x80000000;
	t->OPTCR = 0xfffaaed;
	t->OPTCR1 = 0xfff0000;

}

static uint64_t stm32f7_flash_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F7FLASH *t = STM32F7_FLASH(opaque);
	uint64_t ret;

	switch (offset) {
		case A_FLASH_ACR:
			ret = t->ACR;
			break;
		case A_FLASH_SR:
			ret = t->SR;
			break;
		case A_FLASH_CR:
			ret = t->CR;
			break;
		case A_FLASH_OPTCR:
			ret = t->OPTCR;
			break;
		case A_FLASH_OPTCR1:
			ret = t->OPTCR1;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 FLASH read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f7_flash_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F7FLASH *t = STM32F7_FLASH(opaque);


	switch (offset) {
		case A_FLASH_ACR ... A_FLASH_ACR + 3:
			value = value << ((offset - A_FLASH_ACR) << 3);
			offset = A_FLASH_ACR;
			break;
		case A_FLASH_KEYR ... A_FLASH_KEYR + 3:
			value = value << ((offset - A_FLASH_KEYR) << 3);
			offset = A_FLASH_KEYR;
			break;
		case A_FLASH_OPTKEYR ... A_FLASH_OPTKEYR + 3:
			value = value << ((offset - A_FLASH_OPTKEYR) << 3);
			offset = A_FLASH_OPTKEYR;
			break;
		case A_FLASH_SR ... A_FLASH_SR + 3:
			value = value << ((offset - A_FLASH_SR) << 3);
			offset = A_FLASH_SR;
			break;
		case A_FLASH_CR ... A_FLASH_CR + 3:
			value = value << ((offset - A_FLASH_CR) << 3);
			offset = A_FLASH_CR;
			break;
		case A_FLASH_OPTCR ... A_FLASH_OPTCR + 3:
			value = value << ((offset - A_FLASH_OPTCR) << 3);
			offset = A_FLASH_OPTCR;
			break;
		case A_FLASH_OPTCR1 ... A_FLASH_OPTCR1 + 3:
			value = value << ((offset - A_FLASH_OPTCR1) << 3);
			offset = A_FLASH_OPTCR1;
			break;

		default: break;
	}

	switch (offset) {
		case A_FLASH_ACR:
			t->ACR = value;
			break;
		case A_FLASH_KEYR:
			t->KEYR = value;
			break;
		case A_FLASH_OPTKEYR:
			t->OPTKEYR = value;
			break;
		case A_FLASH_SR:
			t->SR = value;
			break;
		case A_FLASH_CR:
			t->CR = value;
			break;
		case A_FLASH_OPTCR:
			t->OPTCR = value;
			break;
		case A_FLASH_OPTCR1:
			t->OPTCR1 = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 FLASH write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f7_flash_ops = {
	.read = stm32f7_flash_read,
	.write = stm32f7_flash_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f7_flash_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F7FLASH *t = STM32F7_FLASH(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f7_flash_ops, t, "stm32f7-flash", STM32F7_FLASH_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32f7_flash_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32f7_flash_reset_enter(Object *obj, ResetType type) {
	STM32F7FLASH *t = STM32F7_FLASH(obj);
	stm32f7_flash_register_reset(t);
}

static const VMStateDescription stm32f7_flash_vmstate = {
	.name = "stm32f7-flash",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(ACR, STM32F7FLASH),
		VMSTATE_UINT32(KEYR, STM32F7FLASH),
		VMSTATE_UINT32(OPTKEYR, STM32F7FLASH),
		VMSTATE_UINT32(SR, STM32F7FLASH),
		VMSTATE_UINT32(CR, STM32F7FLASH),
		VMSTATE_UINT32(OPTCR, STM32F7FLASH),
		VMSTATE_UINT32(OPTCR1, STM32F7FLASH),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f7_flash_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f7_flash_vmstate;
	dc->realize = stm32f7_flash_realize;
	rc->phases.enter = stm32f7_flash_reset_enter;

}

static const TypeInfo stm32f7_flash_info = {
	.name = TYPE_STM32F7_FLASH,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F7FLASH),
	.instance_init = stm32f7_flash_init,
	.class_init = stm32f7_flash_class_init,
};

static void stm32f7_flash_register_types(void) {
	type_register_static(&stm32f7_flash_info);
}

type_init(stm32f7_flash_register_types);

#define TYPE_STM32F7_RCC "stm32f7-rcc"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F7RCC, STM32F7_RCC)

struct STM32F7RCC {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t CR;
	uint32_t PLLCFGR;
	uint32_t CFGR;
	uint32_t CIR;
	uint32_t AHB1RSTR;
	uint32_t AHB2RSTR;
	uint32_t AHB3RSTR;
	uint32_t APB1RSTR;
	uint32_t APB2RSTR;
	uint32_t AHB1ENR;
	uint32_t AHB2ENR;
	uint32_t AHB3ENR;
	uint32_t APB1ENR;
	uint32_t APB2ENR;
	uint32_t AHB1LPENR;
	uint32_t AHB2LPENR;
	uint32_t AHB3LPENR;
	uint32_t APB1LPENR;
	uint32_t APB2LPENR;
	uint32_t BDCR;
	uint32_t CSR;
	uint32_t SSCGR;
	uint32_t PLLI2SCFGR;
	uint32_t PLLSAICFGR;
	uint32_t DKCFGR1;
	uint32_t DKCFGR2;
	/* base */
	uint32_t base;

};
#define STM32F7_RCC_SIZE				0x94

REG32(RCC_CR, 0x0)
	FIELD(RCC_CR, PLLI2SRDY, 27, 1)
	FIELD(RCC_CR, PLLI2SON, 26, 1)
	FIELD(RCC_CR, PLLRDY, 25, 1)
	FIELD(RCC_CR, PLLON, 24, 1)
	FIELD(RCC_CR, CSSON, 19, 1)
	FIELD(RCC_CR, HSEBYP, 18, 1)
	FIELD(RCC_CR, HSERDY, 17, 1)
	FIELD(RCC_CR, HSEON, 16, 1)
	FIELD(RCC_CR, HSICAL, 8, 8)
	FIELD(RCC_CR, HSITRIM, 3, 5)
	FIELD(RCC_CR, HSIRDY, 1, 1)
	FIELD(RCC_CR, HSION, 0, 1)
REG32(RCC_PLLCFGR, 0x4)
	FIELD(RCC_PLLCFGR, PLLQ3, 27, 1)
	FIELD(RCC_PLLCFGR, PLLQ2, 26, 1)
	FIELD(RCC_PLLCFGR, PLLQ1, 25, 1)
	FIELD(RCC_PLLCFGR, PLLQ0, 24, 1)
	FIELD(RCC_PLLCFGR, PLLSRC, 22, 1)
	FIELD(RCC_PLLCFGR, PLLP1, 17, 1)
	FIELD(RCC_PLLCFGR, PLLP0, 16, 1)
	FIELD(RCC_PLLCFGR, PLLN8, 14, 1)
	FIELD(RCC_PLLCFGR, PLLN7, 13, 1)
	FIELD(RCC_PLLCFGR, PLLN6, 12, 1)
	FIELD(RCC_PLLCFGR, PLLN5, 11, 1)
	FIELD(RCC_PLLCFGR, PLLN4, 10, 1)
	FIELD(RCC_PLLCFGR, PLLN3, 9, 1)
	FIELD(RCC_PLLCFGR, PLLN2, 8, 1)
	FIELD(RCC_PLLCFGR, PLLN1, 7, 1)
	FIELD(RCC_PLLCFGR, PLLN0, 6, 1)
	FIELD(RCC_PLLCFGR, PLLM5, 5, 1)
	FIELD(RCC_PLLCFGR, PLLM4, 4, 1)
	FIELD(RCC_PLLCFGR, PLLM3, 3, 1)
	FIELD(RCC_PLLCFGR, PLLM2, 2, 1)
	FIELD(RCC_PLLCFGR, PLLM1, 1, 1)
	FIELD(RCC_PLLCFGR, PLLM0, 0, 1)
REG32(RCC_CFGR, 0x8)
	FIELD(RCC_CFGR, MCO2, 30, 2)
	FIELD(RCC_CFGR, MCO2PRE, 27, 3)
	FIELD(RCC_CFGR, MCO1PRE, 24, 3)
	FIELD(RCC_CFGR, I2SSRC, 23, 1)
	FIELD(RCC_CFGR, MCO1, 21, 2)
	FIELD(RCC_CFGR, RTCPRE, 16, 5)
	FIELD(RCC_CFGR, PPRE2, 13, 3)
	FIELD(RCC_CFGR, PPRE1, 10, 3)
	FIELD(RCC_CFGR, HPRE, 4, 4)
	FIELD(RCC_CFGR, SWS1, 3, 1)
	FIELD(RCC_CFGR, SWS0, 2, 1)
	FIELD(RCC_CFGR, SW1, 1, 1)
	FIELD(RCC_CFGR, SW0, 0, 1)
REG32(RCC_CIR, 0xc)
	FIELD(RCC_CIR, CSSC, 23, 1)
	FIELD(RCC_CIR, PLLSAIRDYC, 22, 1)
	FIELD(RCC_CIR, PLLI2SRDYC, 21, 1)
	FIELD(RCC_CIR, PLLRDYC, 20, 1)
	FIELD(RCC_CIR, HSERDYC, 19, 1)
	FIELD(RCC_CIR, HSIRDYC, 18, 1)
	FIELD(RCC_CIR, LSERDYC, 17, 1)
	FIELD(RCC_CIR, LSIRDYC, 16, 1)
	FIELD(RCC_CIR, PLLSAIRDYIE, 14, 1)
	FIELD(RCC_CIR, PLLI2SRDYIE, 13, 1)
	FIELD(RCC_CIR, PLLRDYIE, 12, 1)
	FIELD(RCC_CIR, HSERDYIE, 11, 1)
	FIELD(RCC_CIR, HSIRDYIE, 10, 1)
	FIELD(RCC_CIR, LSERDYIE, 9, 1)
	FIELD(RCC_CIR, LSIRDYIE, 8, 1)
	FIELD(RCC_CIR, CSSF, 7, 1)
	FIELD(RCC_CIR, PLLSAIRDYF, 6, 1)
	FIELD(RCC_CIR, PLLI2SRDYF, 5, 1)
	FIELD(RCC_CIR, PLLRDYF, 4, 1)
	FIELD(RCC_CIR, HSERDYF, 3, 1)
	FIELD(RCC_CIR, HSIRDYF, 2, 1)
	FIELD(RCC_CIR, LSERDYF, 1, 1)
	FIELD(RCC_CIR, LSIRDYF, 0, 1)
REG32(RCC_AHB1RSTR, 0x10)
	FIELD(RCC_AHB1RSTR, OTGHSRST, 29, 1)
	FIELD(RCC_AHB1RSTR, ETHMACRST, 25, 1)
	FIELD(RCC_AHB1RSTR, DMA2DRST, 23, 1)
	FIELD(RCC_AHB1RSTR, DMA2RST, 22, 1)
	FIELD(RCC_AHB1RSTR, DMA1RST, 21, 1)
	FIELD(RCC_AHB1RSTR, CRCRST, 12, 1)
	FIELD(RCC_AHB1RSTR, GPIOKRST, 10, 1)
	FIELD(RCC_AHB1RSTR, GPIOJRST, 9, 1)
	FIELD(RCC_AHB1RSTR, GPIOIRST, 8, 1)
	FIELD(RCC_AHB1RSTR, GPIOHRST, 7, 1)
	FIELD(RCC_AHB1RSTR, GPIOGRST, 6, 1)
	FIELD(RCC_AHB1RSTR, GPIOFRST, 5, 1)
	FIELD(RCC_AHB1RSTR, GPIOERST, 4, 1)
	FIELD(RCC_AHB1RSTR, GPIODRST, 3, 1)
	FIELD(RCC_AHB1RSTR, GPIOCRST, 2, 1)
	FIELD(RCC_AHB1RSTR, GPIOBRST, 1, 1)
	FIELD(RCC_AHB1RSTR, GPIOARST, 0, 1)
REG32(RCC_AHB2RSTR, 0x14)
	FIELD(RCC_AHB2RSTR, OTGFSRST, 7, 1)
	FIELD(RCC_AHB2RSTR, RNGRST, 6, 1)
	FIELD(RCC_AHB2RSTR, HSAHRST, 5, 1)
	FIELD(RCC_AHB2RSTR, CRYPRST, 4, 1)
	FIELD(RCC_AHB2RSTR, DCMIRST, 0, 1)
REG32(RCC_AHB3RSTR, 0x18)
	FIELD(RCC_AHB3RSTR, FMCRST, 0, 1)
	FIELD(RCC_AHB3RSTR, QSPIRST, 1, 1)
REG32(RCC_APB1RSTR, 0x20)
	FIELD(RCC_APB1RSTR, TIM2RST, 0, 1)
	FIELD(RCC_APB1RSTR, TIM3RST, 1, 1)
	FIELD(RCC_APB1RSTR, TIM4RST, 2, 1)
	FIELD(RCC_APB1RSTR, TIM5RST, 3, 1)
	FIELD(RCC_APB1RSTR, TIM6RST, 4, 1)
	FIELD(RCC_APB1RSTR, TIM7RST, 5, 1)
	FIELD(RCC_APB1RSTR, TIM12RST, 6, 1)
	FIELD(RCC_APB1RSTR, TIM13RST, 7, 1)
	FIELD(RCC_APB1RSTR, TIM14RST, 8, 1)
	FIELD(RCC_APB1RSTR, WWDGRST, 11, 1)
	FIELD(RCC_APB1RSTR, SPI2RST, 14, 1)
	FIELD(RCC_APB1RSTR, SPI3RST, 15, 1)
	FIELD(RCC_APB1RSTR, UART2RST, 17, 1)
	FIELD(RCC_APB1RSTR, UART3RST, 18, 1)
	FIELD(RCC_APB1RSTR, UART4RST, 19, 1)
	FIELD(RCC_APB1RSTR, UART5RST, 20, 1)
	FIELD(RCC_APB1RSTR, I2C1RST, 21, 1)
	FIELD(RCC_APB1RSTR, I2C2RST, 22, 1)
	FIELD(RCC_APB1RSTR, I2C3RST, 23, 1)
	FIELD(RCC_APB1RSTR, CAN1RST, 25, 1)
	FIELD(RCC_APB1RSTR, CAN2RST, 26, 1)
	FIELD(RCC_APB1RSTR, PWRRST, 28, 1)
	FIELD(RCC_APB1RSTR, DACRST, 29, 1)
	FIELD(RCC_APB1RSTR, UART7RST, 30, 1)
	FIELD(RCC_APB1RSTR, UART8RST, 31, 1)
	FIELD(RCC_APB1RSTR, SPDIFRXRST, 16, 1)
	FIELD(RCC_APB1RSTR, CECRST, 27, 1)
	FIELD(RCC_APB1RSTR, LPTIM1RST, 9, 1)
	FIELD(RCC_APB1RSTR, I2C4RST, 24, 1)
REG32(RCC_APB2RSTR, 0x24)
	FIELD(RCC_APB2RSTR, TIM1RST, 0, 1)
	FIELD(RCC_APB2RSTR, TIM8RST, 1, 1)
	FIELD(RCC_APB2RSTR, USART1RST, 4, 1)
	FIELD(RCC_APB2RSTR, USART6RST, 5, 1)
	FIELD(RCC_APB2RSTR, ADCRST, 8, 1)
	FIELD(RCC_APB2RSTR, SPI1RST, 12, 1)
	FIELD(RCC_APB2RSTR, SPI4RST, 13, 1)
	FIELD(RCC_APB2RSTR, SYSCFGRST, 14, 1)
	FIELD(RCC_APB2RSTR, TIM9RST, 16, 1)
	FIELD(RCC_APB2RSTR, TIM10RST, 17, 1)
	FIELD(RCC_APB2RSTR, TIM11RST, 18, 1)
	FIELD(RCC_APB2RSTR, SPI5RST, 20, 1)
	FIELD(RCC_APB2RSTR, SPI6RST, 21, 1)
	FIELD(RCC_APB2RSTR, SAI1RST, 22, 1)
	FIELD(RCC_APB2RSTR, LTDCRST, 26, 1)
	FIELD(RCC_APB2RSTR, SAI2RST, 23, 1)
	FIELD(RCC_APB2RSTR, SDMMC1RST, 11, 1)
REG32(RCC_AHB1ENR, 0x30)
	FIELD(RCC_AHB1ENR, OTGHSULPIEN, 30, 1)
	FIELD(RCC_AHB1ENR, OTGHSEN, 29, 1)
	FIELD(RCC_AHB1ENR, ETHMACPTPEN, 28, 1)
	FIELD(RCC_AHB1ENR, ETHMACRXEN, 27, 1)
	FIELD(RCC_AHB1ENR, ETHMACTXEN, 26, 1)
	FIELD(RCC_AHB1ENR, ETHMACEN, 25, 1)
	FIELD(RCC_AHB1ENR, DMA2DEN, 23, 1)
	FIELD(RCC_AHB1ENR, DMA2EN, 22, 1)
	FIELD(RCC_AHB1ENR, DMA1EN, 21, 1)
	FIELD(RCC_AHB1ENR, CCMDATARAMEN, 20, 1)
	FIELD(RCC_AHB1ENR, BKPSRAMEN, 18, 1)
	FIELD(RCC_AHB1ENR, CRCEN, 12, 1)
	FIELD(RCC_AHB1ENR, GPIOKEN, 10, 1)
	FIELD(RCC_AHB1ENR, GPIOJEN, 9, 1)
	FIELD(RCC_AHB1ENR, GPIOIEN, 8, 1)
	FIELD(RCC_AHB1ENR, GPIOHEN, 7, 1)
	FIELD(RCC_AHB1ENR, GPIOGEN, 6, 1)
	FIELD(RCC_AHB1ENR, GPIOFEN, 5, 1)
	FIELD(RCC_AHB1ENR, GPIOEEN, 4, 1)
	FIELD(RCC_AHB1ENR, GPIODEN, 3, 1)
	FIELD(RCC_AHB1ENR, GPIOCEN, 2, 1)
	FIELD(RCC_AHB1ENR, GPIOBEN, 1, 1)
	FIELD(RCC_AHB1ENR, GPIOAEN, 0, 1)
REG32(RCC_AHB2ENR, 0x34)
	FIELD(RCC_AHB2ENR, OTGFSEN, 7, 1)
	FIELD(RCC_AHB2ENR, RNGEN, 6, 1)
	FIELD(RCC_AHB2ENR, HASHEN, 5, 1)
	FIELD(RCC_AHB2ENR, CRYPEN, 4, 1)
	FIELD(RCC_AHB2ENR, DCMIEN, 0, 1)
REG32(RCC_AHB3ENR, 0x38)
	FIELD(RCC_AHB3ENR, FMCEN, 0, 1)
	FIELD(RCC_AHB3ENR, QSPIEN, 1, 1)
REG32(RCC_APB1ENR, 0x40)
	FIELD(RCC_APB1ENR, TIM2EN, 0, 1)
	FIELD(RCC_APB1ENR, TIM3EN, 1, 1)
	FIELD(RCC_APB1ENR, TIM4EN, 2, 1)
	FIELD(RCC_APB1ENR, TIM5EN, 3, 1)
	FIELD(RCC_APB1ENR, TIM6EN, 4, 1)
	FIELD(RCC_APB1ENR, TIM7EN, 5, 1)
	FIELD(RCC_APB1ENR, TIM12EN, 6, 1)
	FIELD(RCC_APB1ENR, TIM13EN, 7, 1)
	FIELD(RCC_APB1ENR, TIM14EN, 8, 1)
	FIELD(RCC_APB1ENR, WWDGEN, 11, 1)
	FIELD(RCC_APB1ENR, SPI2EN, 14, 1)
	FIELD(RCC_APB1ENR, SPI3EN, 15, 1)
	FIELD(RCC_APB1ENR, USART2EN, 17, 1)
	FIELD(RCC_APB1ENR, USART3EN, 18, 1)
	FIELD(RCC_APB1ENR, UART4EN, 19, 1)
	FIELD(RCC_APB1ENR, UART5EN, 20, 1)
	FIELD(RCC_APB1ENR, I2C1EN, 21, 1)
	FIELD(RCC_APB1ENR, I2C2EN, 22, 1)
	FIELD(RCC_APB1ENR, I2C3EN, 23, 1)
	FIELD(RCC_APB1ENR, CAN1EN, 25, 1)
	FIELD(RCC_APB1ENR, CAN2EN, 26, 1)
	FIELD(RCC_APB1ENR, PWREN, 28, 1)
	FIELD(RCC_APB1ENR, DACEN, 29, 1)
	FIELD(RCC_APB1ENR, UART7ENR, 30, 1)
	FIELD(RCC_APB1ENR, UART8ENR, 31, 1)
	FIELD(RCC_APB1ENR, SPDIFRXEN, 16, 1)
	FIELD(RCC_APB1ENR, CECEN, 27, 1)
	FIELD(RCC_APB1ENR, LPTMI1EN, 9, 1)
	FIELD(RCC_APB1ENR, I2C4EN, 24, 1)
REG32(RCC_APB2ENR, 0x44)
	FIELD(RCC_APB2ENR, TIM1EN, 0, 1)
	FIELD(RCC_APB2ENR, TIM8EN, 1, 1)
	FIELD(RCC_APB2ENR, USART1EN, 4, 1)
	FIELD(RCC_APB2ENR, USART6EN, 5, 1)
	FIELD(RCC_APB2ENR, ADC1EN, 8, 1)
	FIELD(RCC_APB2ENR, ADC2EN, 9, 1)
	FIELD(RCC_APB2ENR, ADC3EN, 10, 1)
	FIELD(RCC_APB2ENR, SPI1EN, 12, 1)
	FIELD(RCC_APB2ENR, SPI4ENR, 13, 1)
	FIELD(RCC_APB2ENR, SYSCFGEN, 14, 1)
	FIELD(RCC_APB2ENR, TIM9EN, 16, 1)
	FIELD(RCC_APB2ENR, TIM10EN, 17, 1)
	FIELD(RCC_APB2ENR, TIM11EN, 18, 1)
	FIELD(RCC_APB2ENR, SPI5ENR, 20, 1)
	FIELD(RCC_APB2ENR, SPI6ENR, 21, 1)
	FIELD(RCC_APB2ENR, SAI1EN, 22, 1)
	FIELD(RCC_APB2ENR, LTDCEN, 26, 1)
	FIELD(RCC_APB2ENR, SAI2EN, 23, 1)
	FIELD(RCC_APB2ENR, SDMMC1EN, 11, 1)
REG32(RCC_AHB1LPENR, 0x50)
	FIELD(RCC_AHB1LPENR, GPIOALPEN, 0, 1)
	FIELD(RCC_AHB1LPENR, GPIOBLPEN, 1, 1)
	FIELD(RCC_AHB1LPENR, GPIOCLPEN, 2, 1)
	FIELD(RCC_AHB1LPENR, GPIODLPEN, 3, 1)
	FIELD(RCC_AHB1LPENR, GPIOELPEN, 4, 1)
	FIELD(RCC_AHB1LPENR, GPIOFLPEN, 5, 1)
	FIELD(RCC_AHB1LPENR, GPIOGLPEN, 6, 1)
	FIELD(RCC_AHB1LPENR, GPIOHLPEN, 7, 1)
	FIELD(RCC_AHB1LPENR, GPIOILPEN, 8, 1)
	FIELD(RCC_AHB1LPENR, GPIOJLPEN, 9, 1)
	FIELD(RCC_AHB1LPENR, GPIOKLPEN, 10, 1)
	FIELD(RCC_AHB1LPENR, CRCLPEN, 12, 1)
	FIELD(RCC_AHB1LPENR, FLITFLPEN, 15, 1)
	FIELD(RCC_AHB1LPENR, SRAM1LPEN, 16, 1)
	FIELD(RCC_AHB1LPENR, SRAM2LPEN, 17, 1)
	FIELD(RCC_AHB1LPENR, BKPSRAMLPEN, 18, 1)
	FIELD(RCC_AHB1LPENR, SRAM3LPEN, 19, 1)
	FIELD(RCC_AHB1LPENR, DMA1LPEN, 21, 1)
	FIELD(RCC_AHB1LPENR, DMA2LPEN, 22, 1)
	FIELD(RCC_AHB1LPENR, DMA2DLPEN, 23, 1)
	FIELD(RCC_AHB1LPENR, ETHMACLPEN, 25, 1)
	FIELD(RCC_AHB1LPENR, ETHMACTXLPEN, 26, 1)
	FIELD(RCC_AHB1LPENR, ETHMACRXLPEN, 27, 1)
	FIELD(RCC_AHB1LPENR, ETHMACPTPLPEN, 28, 1)
	FIELD(RCC_AHB1LPENR, OTGHSLPEN, 29, 1)
	FIELD(RCC_AHB1LPENR, OTGHSULPILPEN, 30, 1)
REG32(RCC_AHB2LPENR, 0x54)
	FIELD(RCC_AHB2LPENR, OTGFSLPEN, 7, 1)
	FIELD(RCC_AHB2LPENR, RNGLPEN, 6, 1)
	FIELD(RCC_AHB2LPENR, HASHLPEN, 5, 1)
	FIELD(RCC_AHB2LPENR, CRYPLPEN, 4, 1)
	FIELD(RCC_AHB2LPENR, DCMILPEN, 0, 1)
REG32(RCC_AHB3LPENR, 0x58)
	FIELD(RCC_AHB3LPENR, FMCLPEN, 0, 1)
	FIELD(RCC_AHB3LPENR, QSPILPEN, 1, 1)
REG32(RCC_APB1LPENR, 0x60)
	FIELD(RCC_APB1LPENR, TIM2LPEN, 0, 1)
	FIELD(RCC_APB1LPENR, TIM3LPEN, 1, 1)
	FIELD(RCC_APB1LPENR, TIM4LPEN, 2, 1)
	FIELD(RCC_APB1LPENR, TIM5LPEN, 3, 1)
	FIELD(RCC_APB1LPENR, TIM6LPEN, 4, 1)
	FIELD(RCC_APB1LPENR, TIM7LPEN, 5, 1)
	FIELD(RCC_APB1LPENR, TIM12LPEN, 6, 1)
	FIELD(RCC_APB1LPENR, TIM13LPEN, 7, 1)
	FIELD(RCC_APB1LPENR, TIM14LPEN, 8, 1)
	FIELD(RCC_APB1LPENR, WWDGLPEN, 11, 1)
	FIELD(RCC_APB1LPENR, SPI2LPEN, 14, 1)
	FIELD(RCC_APB1LPENR, SPI3LPEN, 15, 1)
	FIELD(RCC_APB1LPENR, USART2LPEN, 17, 1)
	FIELD(RCC_APB1LPENR, USART3LPEN, 18, 1)
	FIELD(RCC_APB1LPENR, UART4LPEN, 19, 1)
	FIELD(RCC_APB1LPENR, UART5LPEN, 20, 1)
	FIELD(RCC_APB1LPENR, I2C1LPEN, 21, 1)
	FIELD(RCC_APB1LPENR, I2C2LPEN, 22, 1)
	FIELD(RCC_APB1LPENR, I2C3LPEN, 23, 1)
	FIELD(RCC_APB1LPENR, CAN1LPEN, 25, 1)
	FIELD(RCC_APB1LPENR, CAN2LPEN, 26, 1)
	FIELD(RCC_APB1LPENR, PWRLPEN, 28, 1)
	FIELD(RCC_APB1LPENR, DACLPEN, 29, 1)
	FIELD(RCC_APB1LPENR, UART7LPEN, 30, 1)
	FIELD(RCC_APB1LPENR, UART8LPEN, 31, 1)
	FIELD(RCC_APB1LPENR, SPDIFRXLPEN, 16, 1)
	FIELD(RCC_APB1LPENR, CECLPEN, 27, 1)
	FIELD(RCC_APB1LPENR, LPTIM1LPEN, 9, 1)
	FIELD(RCC_APB1LPENR, I2C4LPEN, 24, 1)
REG32(RCC_APB2LPENR, 0x64)
	FIELD(RCC_APB2LPENR, TIM1LPEN, 0, 1)
	FIELD(RCC_APB2LPENR, TIM8LPEN, 1, 1)
	FIELD(RCC_APB2LPENR, USART1LPEN, 4, 1)
	FIELD(RCC_APB2LPENR, USART6LPEN, 5, 1)
	FIELD(RCC_APB2LPENR, ADC1LPEN, 8, 1)
	FIELD(RCC_APB2LPENR, ADC2LPEN, 9, 1)
	FIELD(RCC_APB2LPENR, ADC3LPEN, 10, 1)
	FIELD(RCC_APB2LPENR, SPI1LPEN, 12, 1)
	FIELD(RCC_APB2LPENR, SPI4LPEN, 13, 1)
	FIELD(RCC_APB2LPENR, SYSCFGLPEN, 14, 1)
	FIELD(RCC_APB2LPENR, TIM9LPEN, 16, 1)
	FIELD(RCC_APB2LPENR, TIM10LPEN, 17, 1)
	FIELD(RCC_APB2LPENR, TIM11LPEN, 18, 1)
	FIELD(RCC_APB2LPENR, SPI5LPEN, 20, 1)
	FIELD(RCC_APB2LPENR, SPI6LPEN, 21, 1)
	FIELD(RCC_APB2LPENR, SAI1LPEN, 22, 1)
	FIELD(RCC_APB2LPENR, LTDCLPEN, 26, 1)
	FIELD(RCC_APB2LPENR, SAI2LPEN, 23, 1)
	FIELD(RCC_APB2LPENR, SDMMC1LPEN, 11, 1)
REG32(RCC_BDCR, 0x70)
	FIELD(RCC_BDCR, BDRST, 16, 1)
	FIELD(RCC_BDCR, RTCEN, 15, 1)
	FIELD(RCC_BDCR, RTCSEL1, 9, 1)
	FIELD(RCC_BDCR, RTCSEL0, 8, 1)
	FIELD(RCC_BDCR, LSEBYP, 2, 1)
	FIELD(RCC_BDCR, LSERDY, 1, 1)
	FIELD(RCC_BDCR, LSEON, 0, 1)
REG32(RCC_CSR, 0x74)
	FIELD(RCC_CSR, LPWRRSTF, 31, 1)
	FIELD(RCC_CSR, WWDGRSTF, 30, 1)
	FIELD(RCC_CSR, WDGRSTF, 29, 1)
	FIELD(RCC_CSR, SFTRSTF, 28, 1)
	FIELD(RCC_CSR, PORRSTF, 27, 1)
	FIELD(RCC_CSR, PADRSTF, 26, 1)
	FIELD(RCC_CSR, BORRSTF, 25, 1)
	FIELD(RCC_CSR, RMVF, 24, 1)
	FIELD(RCC_CSR, LSIRDY, 1, 1)
	FIELD(RCC_CSR, LSION, 0, 1)
REG32(RCC_SSCGR, 0x80)
	FIELD(RCC_SSCGR, SSCGEN, 31, 1)
	FIELD(RCC_SSCGR, SPREADSEL, 30, 1)
	FIELD(RCC_SSCGR, INCSTEP, 13, 15)
	FIELD(RCC_SSCGR, MODPER, 0, 13)
REG32(RCC_PLLI2SCFGR, 0x84)
	FIELD(RCC_PLLI2SCFGR, PLLI2SR, 28, 3)
	FIELD(RCC_PLLI2SCFGR, PLLI2SQ, 24, 4)
	FIELD(RCC_PLLI2SCFGR, PLLI2SN, 6, 9)
REG32(RCC_PLLSAICFGR, 0x88)
	FIELD(RCC_PLLSAICFGR, PLLSAIN, 6, 9)
	FIELD(RCC_PLLSAICFGR, PLLSAIP, 16, 2)
	FIELD(RCC_PLLSAICFGR, PLLSAIQ, 24, 4)
	FIELD(RCC_PLLSAICFGR, PLLSAIR, 28, 3)
REG32(RCC_DKCFGR1, 0x8c)
	FIELD(RCC_DKCFGR1, PLLI2SDIV, 0, 5)
	FIELD(RCC_DKCFGR1, PLLSAIDIVQ, 8, 5)
	FIELD(RCC_DKCFGR1, PLLSAIDIVR, 16, 2)
	FIELD(RCC_DKCFGR1, SAI1SEL, 20, 2)
	FIELD(RCC_DKCFGR1, SAI2SEL, 22, 2)
	FIELD(RCC_DKCFGR1, TIMPRE, 24, 1)
REG32(RCC_DKCFGR2, 0x90)
	FIELD(RCC_DKCFGR2, USART1SEL, 0, 2)
	FIELD(RCC_DKCFGR2, USART2SEL, 2, 2)
	FIELD(RCC_DKCFGR2, USART3SEL, 4, 2)
	FIELD(RCC_DKCFGR2, UART4SEL, 6, 2)
	FIELD(RCC_DKCFGR2, UART5SEL, 8, 2)
	FIELD(RCC_DKCFGR2, USART6SEL, 10, 2)
	FIELD(RCC_DKCFGR2, UART7SEL, 12, 2)
	FIELD(RCC_DKCFGR2, UART8SEL, 14, 2)
	FIELD(RCC_DKCFGR2, I2C1SEL, 16, 2)
	FIELD(RCC_DKCFGR2, I2C2SEL, 18, 2)
	FIELD(RCC_DKCFGR2, I2C3SEL, 20, 2)
	FIELD(RCC_DKCFGR2, I2C4SEL, 22, 2)
	FIELD(RCC_DKCFGR2, LPTIM1SEL, 24, 2)
	FIELD(RCC_DKCFGR2, CECSEL, 26, 1)
	FIELD(RCC_DKCFGR2, CK48MSEL, 27, 1)
	FIELD(RCC_DKCFGR2, SDMMCSEL, 28, 1)


__attribute__((unused))
static void stm32f7_rcc_update(STM32F7RCC *t) {
	int conditions = (t->CIR & 0x80);
	qemu_set_irq(t->irq[0], conditions);

}

static void stm32f7_rcc_register_reset(STM32F7RCC *t) {
	t->CR = 0x83;
	t->PLLCFGR = 0x24003010;
	t->CFGR = 0x0;
	t->CIR = 0x0;
	t->AHB1RSTR = 0x0;
	t->AHB2RSTR = 0x0;
	t->AHB3RSTR = 0x0;
	t->APB1RSTR = 0x0;
	t->APB2RSTR = 0x0;
	t->AHB1ENR = 0x100000;
	t->AHB2ENR = 0x0;
	t->AHB3ENR = 0x0;
	t->APB1ENR = 0x0;
	t->APB2ENR = 0x0;
	t->AHB1LPENR = 0x7e6791ff;
	t->AHB2LPENR = 0xf1;
	t->AHB3LPENR = 0x1;
	t->APB1LPENR = 0x36fec9ff;
	t->APB2LPENR = 0x75f33;
	t->BDCR = 0x0;
	t->CSR = 0xe000000;
	t->SSCGR = 0x0;
	t->PLLI2SCFGR = 0x20003000;
	t->PLLSAICFGR = 0x20003000;
	t->DKCFGR1 = 0x20003000;
	t->DKCFGR2 = 0x20003000;

}

static uint64_t stm32f7_rcc_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F7RCC *t = STM32F7_RCC(opaque);
	uint64_t ret;

	switch (offset) {
		case A_RCC_CR:
			ret = t->CR;
			break;
		case A_RCC_PLLCFGR:
			ret = t->PLLCFGR;
			break;
		case A_RCC_CFGR:
			ret = t->CFGR;
			break;
		case A_RCC_CIR:
			ret = t->CIR;
			break;
		case A_RCC_AHB1RSTR:
			ret = t->AHB1RSTR;
			break;
		case A_RCC_AHB2RSTR:
			ret = t->AHB2RSTR;
			break;
		case A_RCC_AHB3RSTR:
			ret = t->AHB3RSTR;
			break;
		case A_RCC_APB1RSTR:
			ret = t->APB1RSTR;
			break;
		case A_RCC_APB2RSTR:
			ret = t->APB2RSTR;
			break;
		case A_RCC_AHB1ENR:
			ret = t->AHB1ENR;
			break;
		case A_RCC_AHB2ENR:
			ret = t->AHB2ENR;
			break;
		case A_RCC_AHB3ENR:
			ret = t->AHB3ENR;
			break;
		case A_RCC_APB1ENR:
			ret = t->APB1ENR;
			break;
		case A_RCC_APB2ENR:
			ret = t->APB2ENR;
			break;
		case A_RCC_AHB1LPENR:
			ret = t->AHB1LPENR;
			break;
		case A_RCC_AHB2LPENR:
			ret = t->AHB2LPENR;
			break;
		case A_RCC_AHB3LPENR:
			ret = t->AHB3LPENR;
			break;
		case A_RCC_APB1LPENR:
			ret = t->APB1LPENR;
			break;
		case A_RCC_APB2LPENR:
			ret = t->APB2LPENR;
			break;
		case A_RCC_BDCR:
			ret = t->BDCR;
			break;
		case A_RCC_CSR:
			ret = t->CSR;
			break;
		case A_RCC_SSCGR:
			ret = t->SSCGR;
			break;
		case A_RCC_PLLI2SCFGR:
			ret = t->PLLI2SCFGR;
			break;
		case A_RCC_PLLSAICFGR:
			ret = t->PLLSAICFGR;
			break;
		case A_RCC_DKCFGR1:
			ret = t->DKCFGR1;
			break;
		case A_RCC_DKCFGR2:
			ret = t->DKCFGR2;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 RCC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f7_rcc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F7RCC *t = STM32F7_RCC(opaque);


	switch (offset) {
		case A_RCC_CR ... A_RCC_CR + 3:
			value = value << ((offset - A_RCC_CR) << 3);
			offset = A_RCC_CR;
			break;
		case A_RCC_PLLCFGR ... A_RCC_PLLCFGR + 3:
			value = value << ((offset - A_RCC_PLLCFGR) << 3);
			offset = A_RCC_PLLCFGR;
			break;
		case A_RCC_CFGR ... A_RCC_CFGR + 3:
			value = value << ((offset - A_RCC_CFGR) << 3);
			offset = A_RCC_CFGR;
			break;
		case A_RCC_CIR ... A_RCC_CIR + 3:
			value = value << ((offset - A_RCC_CIR) << 3);
			offset = A_RCC_CIR;
			break;
		case A_RCC_AHB1RSTR ... A_RCC_AHB1RSTR + 3:
			value = value << ((offset - A_RCC_AHB1RSTR) << 3);
			offset = A_RCC_AHB1RSTR;
			break;
		case A_RCC_AHB2RSTR ... A_RCC_AHB2RSTR + 3:
			value = value << ((offset - A_RCC_AHB2RSTR) << 3);
			offset = A_RCC_AHB2RSTR;
			break;
		case A_RCC_AHB3RSTR ... A_RCC_AHB3RSTR + 3:
			value = value << ((offset - A_RCC_AHB3RSTR) << 3);
			offset = A_RCC_AHB3RSTR;
			break;
		case A_RCC_APB1RSTR ... A_RCC_APB1RSTR + 3:
			value = value << ((offset - A_RCC_APB1RSTR) << 3);
			offset = A_RCC_APB1RSTR;
			break;
		case A_RCC_APB2RSTR ... A_RCC_APB2RSTR + 3:
			value = value << ((offset - A_RCC_APB2RSTR) << 3);
			offset = A_RCC_APB2RSTR;
			break;
		case A_RCC_AHB1ENR ... A_RCC_AHB1ENR + 3:
			value = value << ((offset - A_RCC_AHB1ENR) << 3);
			offset = A_RCC_AHB1ENR;
			break;
		case A_RCC_AHB2ENR ... A_RCC_AHB2ENR + 3:
			value = value << ((offset - A_RCC_AHB2ENR) << 3);
			offset = A_RCC_AHB2ENR;
			break;
		case A_RCC_AHB3ENR ... A_RCC_AHB3ENR + 3:
			value = value << ((offset - A_RCC_AHB3ENR) << 3);
			offset = A_RCC_AHB3ENR;
			break;
		case A_RCC_APB1ENR ... A_RCC_APB1ENR + 3:
			value = value << ((offset - A_RCC_APB1ENR) << 3);
			offset = A_RCC_APB1ENR;
			break;
		case A_RCC_APB2ENR ... A_RCC_APB2ENR + 3:
			value = value << ((offset - A_RCC_APB2ENR) << 3);
			offset = A_RCC_APB2ENR;
			break;
		case A_RCC_AHB1LPENR ... A_RCC_AHB1LPENR + 3:
			value = value << ((offset - A_RCC_AHB1LPENR) << 3);
			offset = A_RCC_AHB1LPENR;
			break;
		case A_RCC_AHB2LPENR ... A_RCC_AHB2LPENR + 3:
			value = value << ((offset - A_RCC_AHB2LPENR) << 3);
			offset = A_RCC_AHB2LPENR;
			break;
		case A_RCC_AHB3LPENR ... A_RCC_AHB3LPENR + 3:
			value = value << ((offset - A_RCC_AHB3LPENR) << 3);
			offset = A_RCC_AHB3LPENR;
			break;
		case A_RCC_APB1LPENR ... A_RCC_APB1LPENR + 3:
			value = value << ((offset - A_RCC_APB1LPENR) << 3);
			offset = A_RCC_APB1LPENR;
			break;
		case A_RCC_APB2LPENR ... A_RCC_APB2LPENR + 3:
			value = value << ((offset - A_RCC_APB2LPENR) << 3);
			offset = A_RCC_APB2LPENR;
			break;
		case A_RCC_BDCR ... A_RCC_BDCR + 3:
			value = value << ((offset - A_RCC_BDCR) << 3);
			offset = A_RCC_BDCR;
			break;
		case A_RCC_CSR ... A_RCC_CSR + 3:
			value = value << ((offset - A_RCC_CSR) << 3);
			offset = A_RCC_CSR;
			break;
		case A_RCC_SSCGR ... A_RCC_SSCGR + 3:
			value = value << ((offset - A_RCC_SSCGR) << 3);
			offset = A_RCC_SSCGR;
			break;
		case A_RCC_PLLI2SCFGR ... A_RCC_PLLI2SCFGR + 3:
			value = value << ((offset - A_RCC_PLLI2SCFGR) << 3);
			offset = A_RCC_PLLI2SCFGR;
			break;
		case A_RCC_PLLSAICFGR ... A_RCC_PLLSAICFGR + 3:
			value = value << ((offset - A_RCC_PLLSAICFGR) << 3);
			offset = A_RCC_PLLSAICFGR;
			break;
		case A_RCC_DKCFGR1 ... A_RCC_DKCFGR1 + 3:
			value = value << ((offset - A_RCC_DKCFGR1) << 3);
			offset = A_RCC_DKCFGR1;
			break;
		case A_RCC_DKCFGR2 ... A_RCC_DKCFGR2 + 3:
			value = value << ((offset - A_RCC_DKCFGR2) << 3);
			offset = A_RCC_DKCFGR2;
			break;

		default: break;
	}

	switch (offset) {
		case A_RCC_CR:
			t->CR = value;
			if (((!(value & 0x10000)) && (!(value & 0x40000)) && (!(value & 0x80000)))) {
				t->CR &= (~(0x20000));
			} else if ((value & 0x10000)) {
				t->CR |= 0x20000;
			}
			if ((value & 0x1000000)) {
				t->CR |= 0x2000000;
			} else if ((!(value & 0x1000000))) {
				t->CR &= (~(0x2000000));
			}
			if ((!(value & 0x1))) {
				t->CR &= (~(0x2));
			} else if ((value & 0x1)) {
				t->CR |= 0x2;
			}
			// PERRY PATCH
			// if ((!(value & 0x40000))) {
			if ((!(value & 0x10000))) {
				t->CR &= (~(0x20000));
			}
			if ((!(value & 0x4000000))) {
				t->CR &= (~(0x8000000));
			}
			if ((!(value & 0x10000000))) {
				t->CR &= (~(0x20000000));
			}
			break;
		case A_RCC_PLLCFGR:
			t->PLLCFGR = value;
			break;
		case A_RCC_CFGR:
			t->CFGR = value;
			if (((!(value & 0x1)) && (value & 0x2))) {
				t->CFGR &= (~(0x4));
				t->CFGR |= 0x8;
			} else if (((value & 0x1) && (!(value & 0x2)))) {
				t->CFGR |= 0x4;
				t->CFGR &= (~(0x8));
			} else if (((!(value & 0x1)) && (!(value & 0x2)))) {
				t->CFGR &= (~(0x8));
				t->CFGR &= (~(0x4));
			}
			if ((value == 0x0)) {
				t->CFGR &= (~(0x8));
				t->CFGR &= (~(0x4));
			}
			break;
		case A_RCC_CIR:
			t->CIR = value;
			stm32f7_rcc_update(t);
			break;
		case A_RCC_AHB1RSTR:
			t->AHB1RSTR = value;
			break;
		case A_RCC_AHB2RSTR:
			t->AHB2RSTR = value;
			break;
		case A_RCC_AHB3RSTR:
			t->AHB3RSTR = value;
			break;
		case A_RCC_APB1RSTR:
			t->APB1RSTR = value;
			break;
		case A_RCC_APB2RSTR:
			t->APB2RSTR = value;
			break;
		case A_RCC_AHB1ENR:
			t->AHB1ENR = value;
			break;
		case A_RCC_AHB2ENR:
			t->AHB2ENR = value;
			break;
		case A_RCC_AHB3ENR:
			t->AHB3ENR = value;
			break;
		case A_RCC_APB1ENR:
			t->APB1ENR = value;
			break;
		case A_RCC_APB2ENR:
			t->APB2ENR = value;
			break;
		case A_RCC_AHB1LPENR:
			t->AHB1LPENR = value;
			break;
		case A_RCC_AHB2LPENR:
			t->AHB2LPENR = value;
			break;
		case A_RCC_AHB3LPENR:
			t->AHB3LPENR = value;
			break;
		case A_RCC_APB1LPENR:
			t->APB1LPENR = value;
			break;
		case A_RCC_APB2LPENR:
			t->APB2LPENR = value;
			break;
		case A_RCC_BDCR:
			t->BDCR = value;
			if ((!(value & 0x4))) {
				t->BDCR &= (~(0x2));
			}
			if ((value & 0x1)) {
				t->BDCR |= 0x2;
			}
			break;
		case A_RCC_CSR:
			t->CSR = value;
			if ((value & 0x1)) {
				t->CSR |= 0x2;
			} else if ((!(value & 0x1))) {
				t->CSR &= (~(0x2));
			}
			break;
		case A_RCC_SSCGR:
			t->SSCGR = value;
			break;
		case A_RCC_PLLI2SCFGR:
			t->PLLI2SCFGR = value;
			break;
		case A_RCC_PLLSAICFGR:
			t->PLLSAICFGR = value;
			break;
		case A_RCC_DKCFGR1:
			t->DKCFGR1 = value;
			break;
		case A_RCC_DKCFGR2:
			t->DKCFGR2 = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 RCC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f7_rcc_ops = {
	.read = stm32f7_rcc_read,
	.write = stm32f7_rcc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f7_rcc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F7RCC *t = STM32F7_RCC(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f7_rcc_ops, t, "stm32f7-rcc", STM32F7_RCC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32f7_rcc_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32f7_rcc_reset_enter(Object *obj, ResetType type) {
	STM32F7RCC *t = STM32F7_RCC(obj);
	stm32f7_rcc_register_reset(t);
}

static const VMStateDescription stm32f7_rcc_vmstate = {
	.name = "stm32f7-rcc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR, STM32F7RCC),
		VMSTATE_UINT32(PLLCFGR, STM32F7RCC),
		VMSTATE_UINT32(CFGR, STM32F7RCC),
		VMSTATE_UINT32(CIR, STM32F7RCC),
		VMSTATE_UINT32(AHB1RSTR, STM32F7RCC),
		VMSTATE_UINT32(AHB2RSTR, STM32F7RCC),
		VMSTATE_UINT32(AHB3RSTR, STM32F7RCC),
		VMSTATE_UINT32(APB1RSTR, STM32F7RCC),
		VMSTATE_UINT32(APB2RSTR, STM32F7RCC),
		VMSTATE_UINT32(AHB1ENR, STM32F7RCC),
		VMSTATE_UINT32(AHB2ENR, STM32F7RCC),
		VMSTATE_UINT32(AHB3ENR, STM32F7RCC),
		VMSTATE_UINT32(APB1ENR, STM32F7RCC),
		VMSTATE_UINT32(APB2ENR, STM32F7RCC),
		VMSTATE_UINT32(AHB1LPENR, STM32F7RCC),
		VMSTATE_UINT32(AHB2LPENR, STM32F7RCC),
		VMSTATE_UINT32(AHB3LPENR, STM32F7RCC),
		VMSTATE_UINT32(APB1LPENR, STM32F7RCC),
		VMSTATE_UINT32(APB2LPENR, STM32F7RCC),
		VMSTATE_UINT32(BDCR, STM32F7RCC),
		VMSTATE_UINT32(CSR, STM32F7RCC),
		VMSTATE_UINT32(SSCGR, STM32F7RCC),
		VMSTATE_UINT32(PLLI2SCFGR, STM32F7RCC),
		VMSTATE_UINT32(PLLSAICFGR, STM32F7RCC),
		VMSTATE_UINT32(DKCFGR1, STM32F7RCC),
		VMSTATE_UINT32(DKCFGR2, STM32F7RCC),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f7_rcc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f7_rcc_vmstate;
	dc->realize = stm32f7_rcc_realize;
	rc->phases.enter = stm32f7_rcc_reset_enter;

}

static const TypeInfo stm32f7_rcc_info = {
	.name = TYPE_STM32F7_RCC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F7RCC),
	.instance_init = stm32f7_rcc_init,
	.class_init = stm32f7_rcc_class_init,
};

static void stm32f7_rcc_register_types(void) {
	type_register_static(&stm32f7_rcc_info);
}

type_init(stm32f7_rcc_register_types);

#define TYPE_STM32F7_TIM3 "stm32f7-tim3"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F7TIM3, STM32F7_TIM3)

struct STM32F7TIM3 {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t CR1;
	uint32_t CR2;
	uint32_t SMCR;
	uint32_t DIER;
	uint32_t SR;
	uint32_t EGR;
	uint32_t CCMR1_Output;
	uint32_t CCMR1_Input;
	uint32_t CCMR2_Output;
	uint32_t CCMR2_Input;
	uint32_t CCER;
	uint32_t CNT;
	uint32_t PSC;
	uint32_t ARR;
	uint32_t CCR1;
	uint32_t CCR2;
	uint32_t CCR3;
	uint32_t CCR4;
	uint32_t DCR;
	uint32_t DMAR;
	uint32_t OR1;
	uint32_t OR2;
	/* timer */
	QEMUTimer *timers[1];
	uint8_t enabled[1];

	uint32_t cur_period[1];

	uint8_t period_set[1];

	uint8_t period_reg_set[1];

	/* base */
	uint32_t base;

};
#define STM32F7_TIM3_SIZE				0x64

REG32(TIM3_CR1, 0x0)
	FIELD(TIM3_CR1, CKD, 8, 2)
	FIELD(TIM3_CR1, ARPE, 7, 1)
	FIELD(TIM3_CR1, CMS, 5, 2)
	FIELD(TIM3_CR1, DIR, 4, 1)
	FIELD(TIM3_CR1, OPM, 3, 1)
	FIELD(TIM3_CR1, URS, 2, 1)
	FIELD(TIM3_CR1, UDIS, 1, 1)
	FIELD(TIM3_CR1, CEN, 0, 1)
REG32(TIM3_CR2, 0x4)
	FIELD(TIM3_CR2, TI1S, 7, 1)
	FIELD(TIM3_CR2, MMS, 4, 3)
	FIELD(TIM3_CR2, CCDS, 3, 1)
REG32(TIM3_SMCR, 0x8)
	FIELD(TIM3_SMCR, ETP, 15, 1)
	FIELD(TIM3_SMCR, ECE, 14, 1)
	FIELD(TIM3_SMCR, ETPS, 12, 2)
	FIELD(TIM3_SMCR, ETF, 8, 4)
	FIELD(TIM3_SMCR, MSM, 7, 1)
	FIELD(TIM3_SMCR, TS, 4, 3)
	FIELD(TIM3_SMCR, SMS, 0, 3)
REG32(TIM3_DIER, 0xc)
	FIELD(TIM3_DIER, TDE, 14, 1)
	FIELD(TIM3_DIER, CC4DE, 12, 1)
	FIELD(TIM3_DIER, CC3DE, 11, 1)
	FIELD(TIM3_DIER, CC2DE, 10, 1)
	FIELD(TIM3_DIER, CC1DE, 9, 1)
	FIELD(TIM3_DIER, UDE, 8, 1)
	FIELD(TIM3_DIER, TIE, 6, 1)
	FIELD(TIM3_DIER, CC4IE, 4, 1)
	FIELD(TIM3_DIER, CC3IE, 3, 1)
	FIELD(TIM3_DIER, CC2IE, 2, 1)
	FIELD(TIM3_DIER, CC1IE, 1, 1)
	FIELD(TIM3_DIER, UIE, 0, 1)
REG32(TIM3_SR, 0x10)
	FIELD(TIM3_SR, CC4OF, 12, 1)
	FIELD(TIM3_SR, CC3OF, 11, 1)
	FIELD(TIM3_SR, CC2OF, 10, 1)
	FIELD(TIM3_SR, CC1OF, 9, 1)
	FIELD(TIM3_SR, TIF, 6, 1)
	FIELD(TIM3_SR, CC4IF, 4, 1)
	FIELD(TIM3_SR, CC3IF, 3, 1)
	FIELD(TIM3_SR, CC2IF, 2, 1)
	FIELD(TIM3_SR, CC1IF, 1, 1)
	FIELD(TIM3_SR, UIF, 0, 1)
REG32(TIM3_EGR, 0x14)
	FIELD(TIM3_EGR, TG, 6, 1)
	FIELD(TIM3_EGR, CC4G, 4, 1)
	FIELD(TIM3_EGR, CC3G, 3, 1)
	FIELD(TIM3_EGR, CC2G, 2, 1)
	FIELD(TIM3_EGR, CC1G, 1, 1)
	FIELD(TIM3_EGR, UG, 0, 1)
REG32(TIM3_CCMR1_Output, 0x18)
	FIELD(TIM3_CCMR1_Output, OC2CE, 15, 1)
	FIELD(TIM3_CCMR1_Output, OC2M, 12, 3)
	FIELD(TIM3_CCMR1_Output, OC2PE, 11, 1)
	FIELD(TIM3_CCMR1_Output, OC2FE, 10, 1)
	FIELD(TIM3_CCMR1_Output, CC2S, 8, 2)
	FIELD(TIM3_CCMR1_Output, OC1CE, 7, 1)
	FIELD(TIM3_CCMR1_Output, OC1M, 4, 3)
	FIELD(TIM3_CCMR1_Output, OC1PE, 3, 1)
	FIELD(TIM3_CCMR1_Output, OC1FE, 2, 1)
	FIELD(TIM3_CCMR1_Output, CC1S, 0, 2)
REG32(TIM3_CCMR1_Input, 0x18)
	FIELD(TIM3_CCMR1_Input, IC2F, 12, 4)
	FIELD(TIM3_CCMR1_Input, IC2PCS, 10, 2)
	FIELD(TIM3_CCMR1_Input, CC2S, 8, 2)
	FIELD(TIM3_CCMR1_Input, IC1F, 4, 4)
	FIELD(TIM3_CCMR1_Input, ICPCS, 2, 2)
	FIELD(TIM3_CCMR1_Input, CC1S, 0, 2)
REG32(TIM3_CCMR2_Output, 0x1c)
	FIELD(TIM3_CCMR2_Output, O24CE, 15, 1)
	FIELD(TIM3_CCMR2_Output, OC4M, 12, 3)
	FIELD(TIM3_CCMR2_Output, OC4PE, 11, 1)
	FIELD(TIM3_CCMR2_Output, OC4FE, 10, 1)
	FIELD(TIM3_CCMR2_Output, CC4S, 8, 2)
	FIELD(TIM3_CCMR2_Output, OC3CE, 7, 1)
	FIELD(TIM3_CCMR2_Output, OC3M, 4, 3)
	FIELD(TIM3_CCMR2_Output, OC3PE, 3, 1)
	FIELD(TIM3_CCMR2_Output, OC3FE, 2, 1)
	FIELD(TIM3_CCMR2_Output, CC3S, 0, 2)
REG32(TIM3_CCMR2_Input, 0x1c)
	FIELD(TIM3_CCMR2_Input, IC4F, 12, 4)
	FIELD(TIM3_CCMR2_Input, IC4PSC, 10, 2)
	FIELD(TIM3_CCMR2_Input, CC4S, 8, 2)
	FIELD(TIM3_CCMR2_Input, IC3F, 4, 4)
	FIELD(TIM3_CCMR2_Input, IC3PSC, 2, 2)
	FIELD(TIM3_CCMR2_Input, CC3S, 0, 2)
REG32(TIM3_CCER, 0x20)
	FIELD(TIM3_CCER, CC4NP, 15, 1)
	FIELD(TIM3_CCER, CC4P, 13, 1)
	FIELD(TIM3_CCER, CC4E, 12, 1)
	FIELD(TIM3_CCER, CC3NP, 11, 1)
	FIELD(TIM3_CCER, CC3P, 9, 1)
	FIELD(TIM3_CCER, CC3E, 8, 1)
	FIELD(TIM3_CCER, CC2NP, 7, 1)
	FIELD(TIM3_CCER, CC2P, 5, 1)
	FIELD(TIM3_CCER, CC2E, 4, 1)
	FIELD(TIM3_CCER, CC1NP, 3, 1)
	FIELD(TIM3_CCER, CC1P, 1, 1)
	FIELD(TIM3_CCER, CC1E, 0, 1)
REG32(TIM3_CNT, 0x24)
	FIELD(TIM3_CNT, CNT_H, 16, 16)
	FIELD(TIM3_CNT, CNT_L, 0, 16)
REG32(TIM3_PSC, 0x28)
	FIELD(TIM3_PSC, PSC, 0, 16)
REG32(TIM3_ARR, 0x2c)
	FIELD(TIM3_ARR, ARR_H, 16, 16)
	FIELD(TIM3_ARR, ARR_L, 0, 16)
REG32(TIM3_CCR1, 0x34)
	FIELD(TIM3_CCR1, CCR1_H, 16, 16)
	FIELD(TIM3_CCR1, CCR1_L, 0, 16)
REG32(TIM3_CCR2, 0x38)
	FIELD(TIM3_CCR2, CCR2_H, 16, 16)
	FIELD(TIM3_CCR2, CCR2_L, 0, 16)
REG32(TIM3_CCR3, 0x3c)
	FIELD(TIM3_CCR3, CCR3_H, 16, 16)
	FIELD(TIM3_CCR3, CCR3_L, 0, 16)
REG32(TIM3_CCR4, 0x40)
	FIELD(TIM3_CCR4, CCR4_H, 16, 16)
	FIELD(TIM3_CCR4, CCR4_L, 0, 16)
REG32(TIM3_DCR, 0x48)
	FIELD(TIM3_DCR, DBL, 8, 5)
	FIELD(TIM3_DCR, DBA, 0, 5)
REG32(TIM3_DMAR, 0x4c)
	FIELD(TIM3_DMAR, DMAB, 0, 16)
REG32(TIM3_OR1, 0x50)
	FIELD(TIM3_OR1, TI1_RMP, 0, 2)
REG32(TIM3_OR2, 0x60)
	FIELD(TIM3_OR2, ETRSEL, 14, 3)


__attribute__((unused))
static void stm32f7_tim3_update(STM32F7TIM3 *t) {
	int conditions = (((t->SR & 0x1) && (t->DIER & 0x1)) || ((t->SR & 0x2) && (t->DIER & 0x2)) || ((t->DIER & 0x4) && (t->SR & 0x4)) || ((t->SR & 0x100) && (t->DIER & 0x80)) || ((t->DIER & 0x40) && (t->SR & 0x40)) || ((t->DIER & 0x80) && (t->SR & 0x80)) || ((t->SR & 0x8) && (t->DIER & 0x8)) || ((t->DIER & 0x20) && (t->SR & 0x20)) || ((t->DIER & 0x10) && (t->SR & 0x10)));
	qemu_set_irq(t->irq[0], conditions);

}

static void stm32f7_tim3_timer_callback_0(void *opaque) {
  static uint8_t period_reg_set_last = 0;
  STM32F7TIM3 *t = (STM32F7TIM3*)opaque;

  if (!period_reg_set_last && t->period_reg_set[0]) {
      period_reg_set_last = 1;
      t->CNT = 0;
  }
  t->CNT += 1;
  unsigned prd = t->period_set[0] ? t->cur_period[0] : t->ARR;
  if (t->CNT == prd) {
    t->cur_period[0] = t->ARR;
    t->CNT = 0;
    t->DIER |= 0x1;t->DIER |= 0x8;t->SR |= 0x80;t->SR |= 0x8;t->DIER |= 0x2;t->SR |= 0x20;t->SR |= 0x2;t->SR |= 0x40;t->SR |= 0x4;t->DIER |= 0x4;t->DIER |= 0x80;t->SR |= 0x100;t->SR |= 0x10;t->DIER |= 0x40;t->DIER |= 0x20;t->SR |= 0x1;t->DIER |= 0x10;;
    qemu_set_irq(t->irq[0], 1);
    qemu_set_irq(t->irq[0], 0);
  }

  if (t->enabled[0]) {
    timer_mod(t->timers[0], qemu_clock_get_us(QEMU_CLOCK_VIRTUAL) + 1);
  }
}


static void stm32f7_tim3_register_reset(STM32F7TIM3 *t) {
	t->CR1 = 0x0;
	t->CR2 = 0x0;
	t->SMCR = 0x0;
	t->DIER = 0x0;
	t->SR = 0x0;
	t->EGR = 0x0;
	t->CCMR1_Output = 0x0;
	t->CCMR1_Input = 0x0;
	t->CCMR2_Output = 0x0;
	t->CCMR2_Input = 0x0;
	t->CCER = 0x0;
	t->CNT = 0x0;
	t->PSC = 0x0;
	t->ARR = 0x0;
	t->CCR1 = 0x0;
	t->CCR2 = 0x0;
	t->CCR3 = 0x0;
	t->CCR4 = 0x0;
	t->DCR = 0x0;
	t->DMAR = 0x0;
	t->OR1 = 0x0;
	t->OR2 = 0x0;
	t->enabled[0] = 0;
	t->period_set[0] = 0;
	t->period_reg_set[0] = 0;

}

static uint64_t stm32f7_tim3_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F7TIM3 *t = STM32F7_TIM3(opaque);
	uint64_t ret;

	switch (offset) {
		case A_TIM3_CR1:
			ret = t->CR1;
			break;
		case A_TIM3_CR2:
			ret = t->CR2;
			break;
		case A_TIM3_SMCR:
			ret = t->SMCR;
			break;
		case A_TIM3_DIER:
			ret = t->DIER;
			break;
		case A_TIM3_SR:
			ret = t->SR;
			break;
		case A_TIM3_CCMR1_Output:
			ret = t->CCMR1_Output;
			break;
		case A_TIM3_CCMR2_Output:
			ret = t->CCMR2_Output;
			break;
		case A_TIM3_CCER:
			ret = t->CCER;
			break;
		case A_TIM3_CNT:
			ret = t->CNT;
			break;
		case A_TIM3_PSC:
			ret = t->PSC;
			break;
		case A_TIM3_ARR:
			ret = t->ARR;
			break;
		case A_TIM3_CCR1:
			ret = t->CCR1;
			break;
		case A_TIM3_CCR2:
			ret = t->CCR2;
			break;
		case A_TIM3_CCR3:
			ret = t->CCR3;
			break;
		case A_TIM3_CCR4:
			ret = t->CCR4;
			break;
		case A_TIM3_DCR:
			ret = t->DCR;
			break;
		case A_TIM3_DMAR:
			ret = t->DMAR;
			break;
		case A_TIM3_OR1:
			ret = t->OR1;
			break;
		case A_TIM3_OR2:
			ret = t->OR2;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 TIM3 read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f7_tim3_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F7TIM3 *t = STM32F7_TIM3(opaque);


	switch (offset) {
		case A_TIM3_CR1 ... A_TIM3_CR1 + 3:
			value = value << ((offset - A_TIM3_CR1) << 3);
			offset = A_TIM3_CR1;
			break;
		case A_TIM3_CR2 ... A_TIM3_CR2 + 3:
			value = value << ((offset - A_TIM3_CR2) << 3);
			offset = A_TIM3_CR2;
			break;
		case A_TIM3_SMCR ... A_TIM3_SMCR + 3:
			value = value << ((offset - A_TIM3_SMCR) << 3);
			offset = A_TIM3_SMCR;
			break;
		case A_TIM3_DIER ... A_TIM3_DIER + 3:
			value = value << ((offset - A_TIM3_DIER) << 3);
			offset = A_TIM3_DIER;
			break;
		case A_TIM3_SR ... A_TIM3_SR + 3:
			value = value << ((offset - A_TIM3_SR) << 3);
			offset = A_TIM3_SR;
			break;
		case A_TIM3_EGR ... A_TIM3_EGR + 3:
			value = value << ((offset - A_TIM3_EGR) << 3);
			offset = A_TIM3_EGR;
			break;
		case A_TIM3_CCMR1_Output ... A_TIM3_CCMR1_Output + 3:
			value = value << ((offset - A_TIM3_CCMR1_Output) << 3);
			offset = A_TIM3_CCMR1_Output;
			break;
		case A_TIM3_CCMR2_Output ... A_TIM3_CCMR2_Output + 3:
			value = value << ((offset - A_TIM3_CCMR2_Output) << 3);
			offset = A_TIM3_CCMR2_Output;
			break;
		case A_TIM3_CCER ... A_TIM3_CCER + 3:
			value = value << ((offset - A_TIM3_CCER) << 3);
			offset = A_TIM3_CCER;
			break;
		case A_TIM3_CNT ... A_TIM3_CNT + 3:
			value = value << ((offset - A_TIM3_CNT) << 3);
			offset = A_TIM3_CNT;
			break;
		case A_TIM3_PSC ... A_TIM3_PSC + 3:
			value = value << ((offset - A_TIM3_PSC) << 3);
			offset = A_TIM3_PSC;
			break;
		case A_TIM3_ARR ... A_TIM3_ARR + 3:
			value = value << ((offset - A_TIM3_ARR) << 3);
			offset = A_TIM3_ARR;
			break;
		case A_TIM3_CCR1 ... A_TIM3_CCR1 + 3:
			value = value << ((offset - A_TIM3_CCR1) << 3);
			offset = A_TIM3_CCR1;
			break;
		case A_TIM3_CCR2 ... A_TIM3_CCR2 + 3:
			value = value << ((offset - A_TIM3_CCR2) << 3);
			offset = A_TIM3_CCR2;
			break;
		case A_TIM3_CCR3 ... A_TIM3_CCR3 + 3:
			value = value << ((offset - A_TIM3_CCR3) << 3);
			offset = A_TIM3_CCR3;
			break;
		case A_TIM3_CCR4 ... A_TIM3_CCR4 + 3:
			value = value << ((offset - A_TIM3_CCR4) << 3);
			offset = A_TIM3_CCR4;
			break;
		case A_TIM3_DCR ... A_TIM3_DCR + 3:
			value = value << ((offset - A_TIM3_DCR) << 3);
			offset = A_TIM3_DCR;
			break;
		case A_TIM3_DMAR ... A_TIM3_DMAR + 3:
			value = value << ((offset - A_TIM3_DMAR) << 3);
			offset = A_TIM3_DMAR;
			break;
		case A_TIM3_OR1 ... A_TIM3_OR1 + 3:
			value = value << ((offset - A_TIM3_OR1) << 3);
			offset = A_TIM3_OR1;
			break;
		case A_TIM3_OR2 ... A_TIM3_OR2 + 3:
			value = value << ((offset - A_TIM3_OR2) << 3);
			offset = A_TIM3_OR2;
			break;

		default: break;
	}

	switch (offset) {
		case A_TIM3_CR1:
			t->CR1 = value;
			if ((t->CR1 & 0x1)) {
				t->period_set[0] = t->period_reg_set[0];
				t->enabled[0] = 1;
				t->cur_period[0] = t->ARR;
				timer_mod(t->timers[0], qemu_clock_get_us(QEMU_CLOCK_VIRTUAL) + 1);
			}
			if ((!(t->CR1 & 0x1))) {
				t->enabled[0] = 0;
				timer_free(t->timers[0]);
				t->timers[0] = timer_new(QEMU_CLOCK_VIRTUAL, SCALE_US, stm32f7_tim3_timer_callback_0, t);
			}
			break;
		case A_TIM3_CR2:
			t->CR2 = value;
			break;
		case A_TIM3_SMCR:
			t->SMCR = value;
			break;
		case A_TIM3_DIER:
			t->DIER = value;
			stm32f7_tim3_update(t);
			break;
		case A_TIM3_SR:
			t->SR = value;
			stm32f7_tim3_update(t);
			break;
		case A_TIM3_EGR:
			t->EGR = value;
			break;
		case A_TIM3_CCMR1_Output:
			t->CCMR1_Output = value;
			break;
		case A_TIM3_CCMR2_Output:
			t->CCMR2_Output = value;
			break;
		case A_TIM3_CCER:
			t->CCER = value;
			break;
		case A_TIM3_CNT:
			t->CNT = value;
			break;
		case A_TIM3_PSC:
			t->PSC = value;
			break;
		case A_TIM3_ARR:
			t->ARR = value;
			if (!t->period_reg_set[0]) {
				t->period_reg_set[0] = 1;
			}
			break;
		case A_TIM3_CCR1:
			t->CCR1 = value;
			break;
		case A_TIM3_CCR2:
			t->CCR2 = value;
			break;
		case A_TIM3_CCR3:
			t->CCR3 = value;
			break;
		case A_TIM3_CCR4:
			t->CCR4 = value;
			break;
		case A_TIM3_DCR:
			t->DCR = value;
			break;
		case A_TIM3_DMAR:
			t->DMAR = value;
			break;
		case A_TIM3_OR1:
			t->OR1 = value;
			break;
		case A_TIM3_OR2:
			t->OR2 = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 TIM3 write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f7_tim3_ops = {
	.read = stm32f7_tim3_read,
	.write = stm32f7_tim3_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f7_tim3_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F7TIM3 *t = STM32F7_TIM3(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f7_tim3_ops, t, "stm32f7-tim3", STM32F7_TIM3_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32f7_tim3_realize(DeviceState *dev, Error **errp) {
	STM32F7TIM3 *t = STM32F7_TIM3(dev);
	t->timers[0] = timer_new(QEMU_CLOCK_VIRTUAL, SCALE_US, stm32f7_tim3_timer_callback_0, t);

}

static void stm32f7_tim3_reset_enter(Object *obj, ResetType type) {
	STM32F7TIM3 *t = STM32F7_TIM3(obj);
	stm32f7_tim3_register_reset(t);
}

static const VMStateDescription stm32f7_tim3_vmstate = {
	.name = "stm32f7-tim3",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR1, STM32F7TIM3),
		VMSTATE_UINT32(CR2, STM32F7TIM3),
		VMSTATE_UINT32(SMCR, STM32F7TIM3),
		VMSTATE_UINT32(DIER, STM32F7TIM3),
		VMSTATE_UINT32(SR, STM32F7TIM3),
		VMSTATE_UINT32(EGR, STM32F7TIM3),
		VMSTATE_UINT32(CCMR1_Output, STM32F7TIM3),
		VMSTATE_UINT32(CCMR1_Input, STM32F7TIM3),
		VMSTATE_UINT32(CCMR2_Output, STM32F7TIM3),
		VMSTATE_UINT32(CCMR2_Input, STM32F7TIM3),
		VMSTATE_UINT32(CCER, STM32F7TIM3),
		VMSTATE_UINT32(CNT, STM32F7TIM3),
		VMSTATE_UINT32(PSC, STM32F7TIM3),
		VMSTATE_UINT32(ARR, STM32F7TIM3),
		VMSTATE_UINT32(CCR1, STM32F7TIM3),
		VMSTATE_UINT32(CCR2, STM32F7TIM3),
		VMSTATE_UINT32(CCR3, STM32F7TIM3),
		VMSTATE_UINT32(CCR4, STM32F7TIM3),
		VMSTATE_UINT32(DCR, STM32F7TIM3),
		VMSTATE_UINT32(DMAR, STM32F7TIM3),
		VMSTATE_UINT32(OR1, STM32F7TIM3),
		VMSTATE_UINT32(OR2, STM32F7TIM3),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f7_tim3_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f7_tim3_vmstate;
	dc->realize = stm32f7_tim3_realize;
	rc->phases.enter = stm32f7_tim3_reset_enter;

}

static const TypeInfo stm32f7_tim3_info = {
	.name = TYPE_STM32F7_TIM3,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F7TIM3),
	.instance_init = stm32f7_tim3_init,
	.class_init = stm32f7_tim3_class_init,
};

static void stm32f7_tim3_register_types(void) {
	type_register_static(&stm32f7_tim3_info);
}

type_init(stm32f7_tim3_register_types);

#define TYPE_STM32F7_USART "stm32f7-usart"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F7USART, STM32F7_USART)

struct STM32F7USART {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t CR1;
	uint32_t CR2;
	uint32_t CR3;
	uint32_t BRR;
	uint32_t GTPR;
	uint32_t RTOR;
	uint32_t RQR;
	uint32_t ISR;
	uint32_t ICR;
	uint32_t RDR;
	uint32_t TDR;
	/* chardev backend */
	CharBackend chr;
	guint watch_tag;

	/* base */
	uint32_t base;

};
#define STM32F7_USART_SIZE				0x2c

REG32(USART_CR1, 0x0)
	FIELD(USART_CR1, M1, 28, 1)
	FIELD(USART_CR1, EOBIE, 27, 1)
	FIELD(USART_CR1, RTOIE, 26, 1)
	FIELD(USART_CR1, DEAT4, 25, 1)
	FIELD(USART_CR1, DEAT3, 24, 1)
	FIELD(USART_CR1, DEAT2, 23, 1)
	FIELD(USART_CR1, DEAT1, 22, 1)
	FIELD(USART_CR1, DEAT0, 21, 1)
	FIELD(USART_CR1, DEDT4, 20, 1)
	FIELD(USART_CR1, DEDT3, 19, 1)
	FIELD(USART_CR1, DEDT2, 18, 1)
	FIELD(USART_CR1, DEDT1, 17, 1)
	FIELD(USART_CR1, DEDT0, 16, 1)
	FIELD(USART_CR1, OVER8, 15, 1)
	FIELD(USART_CR1, CMIE, 14, 1)
	FIELD(USART_CR1, MME, 13, 1)
	FIELD(USART_CR1, M0, 12, 1)
	FIELD(USART_CR1, WAKE, 11, 1)
	FIELD(USART_CR1, PCE, 10, 1)
	FIELD(USART_CR1, PS, 9, 1)
	FIELD(USART_CR1, PEIE, 8, 1)
	FIELD(USART_CR1, TXEIE, 7, 1)
	FIELD(USART_CR1, TCIE, 6, 1)
	FIELD(USART_CR1, RXNEIE, 5, 1)
	FIELD(USART_CR1, IDLEIE, 4, 1)
	FIELD(USART_CR1, TE, 3, 1)
	FIELD(USART_CR1, RE, 2, 1)
	FIELD(USART_CR1, UESM, 1, 1)
	FIELD(USART_CR1, UE, 0, 1)
REG32(USART_CR2, 0x4)
	FIELD(USART_CR2, ADD4_7, 28, 4)
	FIELD(USART_CR2, ADD0_3, 24, 4)
	FIELD(USART_CR2, RTOEN, 23, 1)
	FIELD(USART_CR2, ABRMOD1, 22, 1)
	FIELD(USART_CR2, ABRMOD0, 21, 1)
	FIELD(USART_CR2, ABREN, 20, 1)
	FIELD(USART_CR2, MSBFIRST, 19, 1)
	FIELD(USART_CR2, TAINV, 18, 1)
	FIELD(USART_CR2, TXINV, 17, 1)
	FIELD(USART_CR2, RXINV, 16, 1)
	FIELD(USART_CR2, SWAP, 15, 1)
	FIELD(USART_CR2, LINEN, 14, 1)
	FIELD(USART_CR2, STOP, 12, 2)
	FIELD(USART_CR2, CLKEN, 11, 1)
	FIELD(USART_CR2, CPOL, 10, 1)
	FIELD(USART_CR2, CPHA, 9, 1)
	FIELD(USART_CR2, LBCL, 8, 1)
	FIELD(USART_CR2, LBDIE, 6, 1)
	FIELD(USART_CR2, LBDL, 5, 1)
	FIELD(USART_CR2, ADDM7, 4, 1)
REG32(USART_CR3, 0x8)
	FIELD(USART_CR3, WUFIE, 22, 1)
	FIELD(USART_CR3, WUS, 20, 2)
	FIELD(USART_CR3, SCARCNT, 17, 3)
	FIELD(USART_CR3, DEP, 15, 1)
	FIELD(USART_CR3, DEM, 14, 1)
	FIELD(USART_CR3, DDRE, 13, 1)
	FIELD(USART_CR3, OVRDIS, 12, 1)
	FIELD(USART_CR3, ONEBIT, 11, 1)
	FIELD(USART_CR3, CTSIE, 10, 1)
	FIELD(USART_CR3, CTSE, 9, 1)
	FIELD(USART_CR3, RTSE, 8, 1)
	FIELD(USART_CR3, DMAT, 7, 1)
	FIELD(USART_CR3, DMAR, 6, 1)
	FIELD(USART_CR3, SCEN, 5, 1)
	FIELD(USART_CR3, NACK, 4, 1)
	FIELD(USART_CR3, HDSEL, 3, 1)
	FIELD(USART_CR3, IRLP, 2, 1)
	FIELD(USART_CR3, IREN, 1, 1)
	FIELD(USART_CR3, EIE, 0, 1)
REG32(USART_BRR, 0xc)
	FIELD(USART_BRR, DIV_Mantissa, 4, 12)
	FIELD(USART_BRR, DIV_Fraction, 0, 4)
REG32(USART_GTPR, 0x10)
	FIELD(USART_GTPR, GT, 8, 8)
	FIELD(USART_GTPR, PSC, 0, 8)
REG32(USART_RTOR, 0x14)
	FIELD(USART_RTOR, BLEN, 24, 8)
	FIELD(USART_RTOR, RTO, 0, 24)
REG32(USART_RQR, 0x18)
	FIELD(USART_RQR, TXFRQ, 4, 1)
	FIELD(USART_RQR, RXFRQ, 3, 1)
	FIELD(USART_RQR, MMRQ, 2, 1)
	FIELD(USART_RQR, SBKRQ, 1, 1)
	FIELD(USART_RQR, ABRRQ, 0, 1)
REG32(USART_ISR, 0x1c)
	FIELD(USART_ISR, REACK, 22, 1)
	FIELD(USART_ISR, TEACK, 21, 1)
	FIELD(USART_ISR, WUF, 20, 1)
	FIELD(USART_ISR, RWU, 19, 1)
	FIELD(USART_ISR, SBKF, 18, 1)
	FIELD(USART_ISR, CMF, 17, 1)
	FIELD(USART_ISR, BUSY, 16, 1)
	FIELD(USART_ISR, ABRF, 15, 1)
	FIELD(USART_ISR, ABRE, 14, 1)
	FIELD(USART_ISR, EOBF, 12, 1)
	FIELD(USART_ISR, RTOF, 11, 1)
	FIELD(USART_ISR, CTS, 10, 1)
	FIELD(USART_ISR, CTSIF, 9, 1)
	FIELD(USART_ISR, LBDF, 8, 1)
	FIELD(USART_ISR, TXE, 7, 1)
	FIELD(USART_ISR, TC, 6, 1)
	FIELD(USART_ISR, RXNE, 5, 1)
	FIELD(USART_ISR, IDLE, 4, 1)
	FIELD(USART_ISR, ORE, 3, 1)
	FIELD(USART_ISR, NF, 2, 1)
	FIELD(USART_ISR, FE, 1, 1)
	FIELD(USART_ISR, PE, 0, 1)
REG32(USART_ICR, 0x20)
	FIELD(USART_ICR, WUCF, 20, 1)
	FIELD(USART_ICR, CMCF, 17, 1)
	FIELD(USART_ICR, EOBCF, 12, 1)
	FIELD(USART_ICR, RTOCF, 11, 1)
	FIELD(USART_ICR, CTSCF, 9, 1)
	FIELD(USART_ICR, LBDCF, 8, 1)
	FIELD(USART_ICR, TCCF, 6, 1)
	FIELD(USART_ICR, IDLECF, 4, 1)
	FIELD(USART_ICR, ORECF, 3, 1)
	FIELD(USART_ICR, NCF, 2, 1)
	FIELD(USART_ICR, FECF, 1, 1)
	FIELD(USART_ICR, PECF, 0, 1)
REG32(USART_RDR, 0x24)
	FIELD(USART_RDR, RDR, 0, 9)
REG32(USART_TDR, 0x28)
	FIELD(USART_TDR, TDR, 0, 9)


__attribute__((unused))
static void stm32f7_usart_update(STM32F7USART *t) {
	int conditions = (((t->CR3 & 0x1) && (t->ISR & 0x8) && (!(t->CR3 & 0x40))) || ((!(t->ISR & 0x8)) && (!(t->ISR & 0x2)) && (!(t->ISR & 0x10)) && (!(t->ISR & 0x4)) && (!(t->ISR & 0x800)) && (!(t->ISR & 0x1)) && (t->ISR & 0x80) && (t->CR1 & 0x80)) || ((t->CR3 & 0x1) && (t->ISR & 0x1) && (!(t->CR3 & 0x40))) || ((t->ISR & 0x4) && (t->CR3 & 0x1) && (!(t->CR3 & 0x40))) || ((t->CR3 & 0x1) && (!(t->CR3 & 0x40)) && (t->ISR & 0x2)) || ((!(t->ISR & 0x8)) && (!(t->ISR & 0x2)) && (!(t->CR1 & 0x10)) && (!(t->ISR & 0x4)) && (!(t->ISR & 0x800)) && (!(t->ISR & 0x1)) && (t->ISR & 0x80) && (t->CR1 & 0x80)) || ((t->ISR & 0x4) && (t->CR1 & 0x20) && (!(t->CR3 & 0x40))) || ((t->CR1 & 0x20) && (!(t->CR3 & 0x40)) && (t->ISR & 0x2)) || ((t->CR1 & 0x20) && (!(t->CR3 & 0x40)) && (t->ISR & 0x800)) || ((!(t->CR3 & 0x40)) && (t->CR1 & 0x20) && (t->ISR & 0x8)) || ((t->ISR & 0x4) && (!(t->CR3 & 0x40)) && (t->CR1 & 0x4000000)) || ((t->ISR & 0x8) && (!(t->CR3 & 0x40)) && (t->CR1 & 0x4000000)) || ((t->ISR & 0x4) && (!(t->CR3 & 0x40)) && (t->CR1 & 0x100)) || ((t->ISR & 0x8) && (!(t->CR3 & 0x40)) && (t->CR1 & 0x100)) || ((!(t->CR3 & 0x40)) && (t->ISR & 0x2) && (t->CR1 & 0x100)) || ((!(t->CR3 & 0x40)) && (t->ISR & 0x2) && (t->CR1 & 0x4000000)) || ((t->ISR & 0x1) && (t->CR1 & 0x20) && (!(t->CR3 & 0x40))) || ((t->ISR & 0x1) && (!(t->CR3 & 0x40)) && (t->CR1 & 0x4000000)) || ((t->ISR & 0x1) && (!(t->CR3 & 0x40)) && (t->CR1 & 0x100)) || ((!(t->ISR & 0x8)) && (!(t->ISR & 0x2)) && (!(t->CR1 & 0x10)) && (!(t->ISR & 0x4)) && (!(t->ISR & 0x800)) && (!(t->ISR & 0x1)) && (t->ISR & 0x40) && (t->CR1 & 0x40)) || ((!(t->ISR & 0x8)) && (!(t->ISR & 0x2)) && (!(t->ISR & 0x10)) && (!(t->ISR & 0x4)) && (!(t->ISR & 0x800)) && (!(t->ISR & 0x1)) && (t->ISR & 0x40) && (t->CR1 & 0x40)) || ((!(t->ISR & 0x8)) && (!(t->ISR & 0x2)) && (!(t->CR1 & 0x10)) && (!(t->ISR & 0x4)) && (!(t->ISR & 0x800)) && (!(t->ISR & 0x1)) && (t->ISR & 0x100000) && (t->CR3 & 0x400000)) || ((!(t->ISR & 0x8)) && (!(t->ISR & 0x2)) && (!(t->ISR & 0x4)) && (!(t->ISR & 0x800)) && (!(t->ISR & 0x1)) && (t->ISR & 0x80) && (t->CR1 & 0x80)) || ((!(t->CR1 & 0x20)) && (!(t->CR3 & 0x1)) && (!(t->CR1 & 0x100)) && (!(t->CR1 & 0x4000000)) && (t->ISR & 0x40) && (t->CR1 & 0x40)) || ((!(t->ISR & 0x8)) && (!(t->ISR & 0x2)) && (!(t->ISR & 0x4)) && (!(t->ISR & 0x800)) && (!(t->ISR & 0x1)) && (t->ISR & 0x40) && (t->CR1 & 0x40)) || ((!(t->CR1 & 0x20)) && (!(t->ISR & 0x10)) && (!(t->CR3 & 0x1)) && (!(t->CR1 & 0x100)) && (!(t->CR1 & 0x4000000)) && (t->ISR & 0x100000) && (t->CR3 & 0x400000)) || ((!(t->CR1 & 0x20)) && (!(t->ISR & 0x10)) && (!(t->CR3 & 0x1)) && (!(t->CR1 & 0x100)) && (!(t->CR1 & 0x4000000)) && (t->ISR & 0x40) && (t->CR1 & 0x40)) || ((!(t->ISR & 0x8)) && (!(t->ISR & 0x2)) && (!(t->ISR & 0x4)) && (!(t->ISR & 0x800)) && (!(t->ISR & 0x1)) && (t->ISR & 0x100000) && (t->CR3 & 0x400000)) || ((!(t->ISR & 0x8)) && (!(t->ISR & 0x2)) && (!(t->ISR & 0x10)) && (!(t->ISR & 0x4)) && (!(t->ISR & 0x800)) && (!(t->ISR & 0x1)) && (t->ISR & 0x100000) && (t->CR3 & 0x400000)) || ((!(t->CR1 & 0x20)) && (!(t->CR3 & 0x1)) && (!(t->CR1 & 0x100)) && (!(t->CR1 & 0x4000000)) && (t->ISR & 0x80) && (t->CR1 & 0x80)) || ((!(t->CR1 & 0x20)) && (!(t->CR1 & 0x10)) && (!(t->CR3 & 0x1)) && (!(t->CR1 & 0x100)) && (!(t->CR1 & 0x4000000)) && (t->ISR & 0x80) && (t->CR1 & 0x80)) || ((!(t->CR1 & 0x20)) && (!(t->CR1 & 0x10)) && (!(t->CR3 & 0x1)) && (!(t->CR1 & 0x100)) && (!(t->CR1 & 0x4000000)) && (t->ISR & 0x40) && (t->CR1 & 0x40)) || ((!(t->CR1 & 0x20)) && (!(t->ISR & 0x10)) && (!(t->CR3 & 0x1)) && (!(t->CR1 & 0x100)) && (!(t->CR1 & 0x4000000)) && (t->ISR & 0x80) && (t->CR1 & 0x80)) || ((t->CR3 & 0x1) && (!(t->CR3 & 0x40)) && (t->ISR & 0x800)) || ((!(t->CR3 & 0x40)) && (t->CR1 & 0x100) && (t->ISR & 0x800)) || ((!(t->CR3 & 0x40)) && (t->CR1 & 0x4000000) && (t->ISR & 0x800)) || ((!(t->CR1 & 0x20)) && (!(t->CR1 & 0x10)) && (!(t->CR3 & 0x1)) && (!(t->CR1 & 0x100)) && (!(t->CR1 & 0x4000000)) && (t->ISR & 0x100000) && (t->CR3 & 0x400000)) || ((!(t->CR3 & 0x40)) && (t->ISR & 0x10) && (t->CR1 & 0x10)) || ((!(t->CR1 & 0x20)) && (!(t->CR3 & 0x1)) && (!(t->CR1 & 0x100)) && (!(t->CR1 & 0x4000000)) && (t->ISR & 0x100000) && (t->CR3 & 0x400000)) || ((t->CR1 & 0x20) && (t->ISR & 0x20)));
	qemu_set_irq(t->irq[0], conditions);

}

static int stm32f7_usart_can_receive(void *opaque) {
	return 1;
}

static void stm32f7_usart_receive(void *opaque, const uint8_t *buf, int size) {
	STM32F7USART *t = STM32F7_USART(opaque);

	t->RDR = *buf;
	t->ISR |= 0x20;

	stm32f7_usart_update(t);
}

static gboolean stm32f7_usart_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	STM32F7USART *t = STM32F7_USART(opaque);
	int ret;

	t->watch_tag = 0;
	t->ISR &= (~(0x80));
	t->ISR &= (~(0x40));

	stm32f7_usart_update(t);

	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->TDR), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, stm32f7_usart_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->ISR |= 0x40;
	t->ISR |= 0x80;

	stm32f7_usart_update(t);

	return FALSE;
}

static void stm32f7_usart_register_reset(STM32F7USART *t) {
	t->CR1 = 0x0;
	t->CR2 = 0x0;
	t->CR3 = 0x0;
	t->BRR = 0x0;
	t->GTPR = 0x0;
	t->RTOR = 0x0;
	t->RQR = 0x0;
	t->ISR = 0xc0;
	t->ICR = 0x0;
	t->RDR = 0x0;
	t->TDR = 0x0;
	t->ISR |= 0x40;
	t->ISR |= 0x80;

}

static uint64_t stm32f7_usart_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F7USART *t = STM32F7_USART(opaque);
	uint64_t ret;

	switch (offset) {
		case A_USART_CR1:
			ret = t->CR1;
			break;
		case A_USART_CR2:
			ret = t->CR2;
			break;
		case A_USART_CR3:
			ret = t->CR3;
			break;
		case A_USART_BRR:
			ret = t->BRR;
			break;
		case A_USART_GTPR:
			ret = t->GTPR;
			break;
		case A_USART_RTOR:
			ret = t->RTOR;
			break;
		case A_USART_ISR:
			ret = t->ISR;
			break;
		case A_USART_RDR:
			ret = t->RDR;
			t->ISR &= (~(0x20));
			qemu_chr_fe_accept_input(&(t->chr));
			stm32f7_usart_update(t);
			break;
		case A_USART_TDR:
			ret = t->TDR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 USART read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f7_usart_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F7USART *t = STM32F7_USART(opaque);


	switch (offset) {
		case A_USART_CR1 ... A_USART_CR1 + 3:
			value = value << ((offset - A_USART_CR1) << 3);
			offset = A_USART_CR1;
			break;
		case A_USART_CR2 ... A_USART_CR2 + 3:
			value = value << ((offset - A_USART_CR2) << 3);
			offset = A_USART_CR2;
			break;
		case A_USART_CR3 ... A_USART_CR3 + 3:
			value = value << ((offset - A_USART_CR3) << 3);
			offset = A_USART_CR3;
			break;
		case A_USART_BRR ... A_USART_BRR + 3:
			value = value << ((offset - A_USART_BRR) << 3);
			offset = A_USART_BRR;
			break;
		case A_USART_GTPR ... A_USART_GTPR + 3:
			value = value << ((offset - A_USART_GTPR) << 3);
			offset = A_USART_GTPR;
			break;
		case A_USART_RTOR ... A_USART_RTOR + 3:
			value = value << ((offset - A_USART_RTOR) << 3);
			offset = A_USART_RTOR;
			break;
		case A_USART_RQR ... A_USART_RQR + 3:
			value = value << ((offset - A_USART_RQR) << 3);
			offset = A_USART_RQR;
			break;
		case A_USART_ICR ... A_USART_ICR + 3:
			value = value << ((offset - A_USART_ICR) << 3);
			offset = A_USART_ICR;
			break;
		case A_USART_TDR ... A_USART_TDR + 3:
			value = value << ((offset - A_USART_TDR) << 3);
			offset = A_USART_TDR;
			break;

		default: break;
	}

	switch (offset) {
		case A_USART_CR1:
			t->CR1 = value;
			if ((value & 0x4)) {
				t->ISR |= 0x400000;
			}
			if ((value & 0x8)) {
				t->ISR |= 0x200000;
			}
			stm32f7_usart_update(t);
			break;
		case A_USART_CR2:
			t->CR2 = value;
			break;
		case A_USART_CR3:
			t->CR3 = value;
			stm32f7_usart_update(t);
			break;
		case A_USART_BRR:
			t->BRR = value;
			break;
		case A_USART_GTPR:
			t->GTPR = value;
			break;
		case A_USART_RTOR:
			t->RTOR = value;
			break;
		case A_USART_RQR:
			t->RQR = value;
			break;
		case A_USART_ICR:
			t->ICR = value;
			break;
		case A_USART_TDR:
			t->TDR = value;
			stm32f7_usart_transmit(NULL, G_IO_OUT, t);
			if (((!(value & 0x100)) && (!(value & 0x200)) && (!(value & 0x400)) && (!(value & 0x800)) && (!(value & 0x1000)) && (!(value & 0x2000)) && (!(value & 0x4000)) && (!(value & 0x8000)) && (!(value & 0x10000)) && (!(value & 0x20000)) && (!(value & 0x40000)) && (!(value & 0x80000)) && (!(value & 0x100000)) && (!(value & 0x200000)) && (!(value & 0x400000)) && (!(value & 0x800000)) && (!(value & 0x1000000)) && (!(value & 0x2000000)) && (!(value & 0x4000000)) && (!(value & 0x8000000)) && (!(value & 0x10000000)) && (!(value & 0x20000000)) && (!(value & 0x40000000)) && (!(value & 0x80000000)))) {
				t->ISR |= 0x40;
				t->ISR |= 0x80;
			} else if (((!(value & 0x200)) && (!(value & 0x400)) && (!(value & 0x800)) && (!(value & 0x1000)) && (!(value & 0x2000)) && (!(value & 0x4000)) && (!(value & 0x8000)) && (!(value & 0x10000)) && (!(value & 0x20000)) && (!(value & 0x40000)) && (!(value & 0x80000)) && (!(value & 0x100000)) && (!(value & 0x200000)) && (!(value & 0x400000)) && (!(value & 0x800000)) && (!(value & 0x1000000)) && (!(value & 0x2000000)) && (!(value & 0x4000000)) && (!(value & 0x8000000)) && (!(value & 0x10000000)) && (!(value & 0x20000000)) && (!(value & 0x40000000)) && (!(value & 0x80000000)))) {
				t->ISR |= 0x40;
				t->ISR |= 0x80;
			}
			t->ISR |= 0x40;
			stm32f7_usart_update(t);
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 USART write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f7_usart_ops = {
	.read = stm32f7_usart_read,
	.write = stm32f7_usart_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f7_usart_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F7USART *t = STM32F7_USART(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f7_usart_ops, t, "stm32f7-usart", STM32F7_USART_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32f7_usart_realize(DeviceState *dev, Error **errp) {
	STM32F7USART *t = STM32F7_USART(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), stm32f7_usart_can_receive, stm32f7_usart_receive,
		NULL, NULL, t, NULL, true);

}

static void stm32f7_usart_reset_enter(Object *obj, ResetType type) {
	STM32F7USART *t = STM32F7_USART(obj);
	stm32f7_usart_register_reset(t);
}

static const VMStateDescription stm32f7_usart_vmstate = {
	.name = "stm32f7-usart",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR1, STM32F7USART),
		VMSTATE_UINT32(CR2, STM32F7USART),
		VMSTATE_UINT32(CR3, STM32F7USART),
		VMSTATE_UINT32(BRR, STM32F7USART),
		VMSTATE_UINT32(GTPR, STM32F7USART),
		VMSTATE_UINT32(RTOR, STM32F7USART),
		VMSTATE_UINT32(RQR, STM32F7USART),
		VMSTATE_UINT32(ISR, STM32F7USART),
		VMSTATE_UINT32(ICR, STM32F7USART),
		VMSTATE_UINT32(RDR, STM32F7USART),
		VMSTATE_UINT32(TDR, STM32F7USART),

		VMSTATE_END_OF_LIST()
	}
};

static Property stm32f7_usart_properties[] = {
	DEFINE_PROP_CHR("chardev", STM32F7USART, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void stm32f7_usart_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f7_usart_vmstate;
	dc->realize = stm32f7_usart_realize;
	rc->phases.enter = stm32f7_usart_reset_enter;
	device_class_set_props(dc, stm32f7_usart_properties);

}

static const TypeInfo stm32f7_usart_info = {
	.name = TYPE_STM32F7_USART,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F7USART),
	.instance_init = stm32f7_usart_init,
	.class_init = stm32f7_usart_class_init,
};

static void stm32f7_usart_register_types(void) {
	type_register_static(&stm32f7_usart_info);
}

type_init(stm32f7_usart_register_types);

#define TYPE_STM32F7_GPIOA "stm32f7-gpioa"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F7GPIOA, STM32F7_GPIOA)

struct STM32F7GPIOA {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/*registers*/
	uint32_t MODER;
	uint32_t OTYPER;
	uint32_t GPIOB_OSPEEDR;
	uint32_t PUPDR;
	uint32_t IDR;
	uint32_t ODR;
	uint32_t BSRR;
	uint32_t LCKR;
	uint32_t AFRL;
	uint32_t AFRH;
	uint32_t BRR;
	/* base */
	uint32_t base;

};
#define STM32F7_GPIOA_SIZE				0x2c

REG32(GPIOA_MODER, 0x0)
	FIELD(GPIOA_MODER, MODER15, 30, 2)
	FIELD(GPIOA_MODER, MODER14, 28, 2)
	FIELD(GPIOA_MODER, MODER13, 26, 2)
	FIELD(GPIOA_MODER, MODER12, 24, 2)
	FIELD(GPIOA_MODER, MODER11, 22, 2)
	FIELD(GPIOA_MODER, MODER10, 20, 2)
	FIELD(GPIOA_MODER, MODER9, 18, 2)
	FIELD(GPIOA_MODER, MODER8, 16, 2)
	FIELD(GPIOA_MODER, MODER7, 14, 2)
	FIELD(GPIOA_MODER, MODER6, 12, 2)
	FIELD(GPIOA_MODER, MODER5, 10, 2)
	FIELD(GPIOA_MODER, MODER4, 8, 2)
	FIELD(GPIOA_MODER, MODER3, 6, 2)
	FIELD(GPIOA_MODER, MODER2, 4, 2)
	FIELD(GPIOA_MODER, MODER1, 2, 2)
	FIELD(GPIOA_MODER, MODER0, 0, 2)
REG32(GPIOA_OTYPER, 0x4)
	FIELD(GPIOA_OTYPER, OT15, 15, 1)
	FIELD(GPIOA_OTYPER, OT14, 14, 1)
	FIELD(GPIOA_OTYPER, OT13, 13, 1)
	FIELD(GPIOA_OTYPER, OT12, 12, 1)
	FIELD(GPIOA_OTYPER, OT11, 11, 1)
	FIELD(GPIOA_OTYPER, OT10, 10, 1)
	FIELD(GPIOA_OTYPER, OT9, 9, 1)
	FIELD(GPIOA_OTYPER, OT8, 8, 1)
	FIELD(GPIOA_OTYPER, OT7, 7, 1)
	FIELD(GPIOA_OTYPER, OT6, 6, 1)
	FIELD(GPIOA_OTYPER, OT5, 5, 1)
	FIELD(GPIOA_OTYPER, OT4, 4, 1)
	FIELD(GPIOA_OTYPER, OT3, 3, 1)
	FIELD(GPIOA_OTYPER, OT2, 2, 1)
	FIELD(GPIOA_OTYPER, OT1, 1, 1)
	FIELD(GPIOA_OTYPER, OT0, 0, 1)
REG32(GPIOA_GPIOB_OSPEEDR, 0x8)
	FIELD(GPIOA_GPIOB_OSPEEDR, OSPEEDR15, 30, 2)
	FIELD(GPIOA_GPIOB_OSPEEDR, OSPEEDR14, 28, 2)
	FIELD(GPIOA_GPIOB_OSPEEDR, OSPEEDR13, 26, 2)
	FIELD(GPIOA_GPIOB_OSPEEDR, OSPEEDR12, 24, 2)
	FIELD(GPIOA_GPIOB_OSPEEDR, OSPEEDR11, 22, 2)
	FIELD(GPIOA_GPIOB_OSPEEDR, OSPEEDR10, 20, 2)
	FIELD(GPIOA_GPIOB_OSPEEDR, OSPEEDR9, 18, 2)
	FIELD(GPIOA_GPIOB_OSPEEDR, OSPEEDR8, 16, 2)
	FIELD(GPIOA_GPIOB_OSPEEDR, OSPEEDR7, 14, 2)
	FIELD(GPIOA_GPIOB_OSPEEDR, OSPEEDR6, 12, 2)
	FIELD(GPIOA_GPIOB_OSPEEDR, OSPEEDR5, 10, 2)
	FIELD(GPIOA_GPIOB_OSPEEDR, OSPEEDR4, 8, 2)
	FIELD(GPIOA_GPIOB_OSPEEDR, OSPEEDR3, 6, 2)
	FIELD(GPIOA_GPIOB_OSPEEDR, OSPEEDR2, 4, 2)
	FIELD(GPIOA_GPIOB_OSPEEDR, OSPEEDR1, 2, 2)
	FIELD(GPIOA_GPIOB_OSPEEDR, OSPEEDR0, 0, 2)
REG32(GPIOA_PUPDR, 0xc)
	FIELD(GPIOA_PUPDR, PUPDR15, 30, 2)
	FIELD(GPIOA_PUPDR, PUPDR14, 28, 2)
	FIELD(GPIOA_PUPDR, PUPDR13, 26, 2)
	FIELD(GPIOA_PUPDR, PUPDR12, 24, 2)
	FIELD(GPIOA_PUPDR, PUPDR11, 22, 2)
	FIELD(GPIOA_PUPDR, PUPDR10, 20, 2)
	FIELD(GPIOA_PUPDR, PUPDR9, 18, 2)
	FIELD(GPIOA_PUPDR, PUPDR8, 16, 2)
	FIELD(GPIOA_PUPDR, PUPDR7, 14, 2)
	FIELD(GPIOA_PUPDR, PUPDR6, 12, 2)
	FIELD(GPIOA_PUPDR, PUPDR5, 10, 2)
	FIELD(GPIOA_PUPDR, PUPDR4, 8, 2)
	FIELD(GPIOA_PUPDR, PUPDR3, 6, 2)
	FIELD(GPIOA_PUPDR, PUPDR2, 4, 2)
	FIELD(GPIOA_PUPDR, PUPDR1, 2, 2)
	FIELD(GPIOA_PUPDR, PUPDR0, 0, 2)
REG32(GPIOA_IDR, 0x10)
	FIELD(GPIOA_IDR, IDR15, 15, 1)
	FIELD(GPIOA_IDR, IDR14, 14, 1)
	FIELD(GPIOA_IDR, IDR13, 13, 1)
	FIELD(GPIOA_IDR, IDR12, 12, 1)
	FIELD(GPIOA_IDR, IDR11, 11, 1)
	FIELD(GPIOA_IDR, IDR10, 10, 1)
	FIELD(GPIOA_IDR, IDR9, 9, 1)
	FIELD(GPIOA_IDR, IDR8, 8, 1)
	FIELD(GPIOA_IDR, IDR7, 7, 1)
	FIELD(GPIOA_IDR, IDR6, 6, 1)
	FIELD(GPIOA_IDR, IDR5, 5, 1)
	FIELD(GPIOA_IDR, IDR4, 4, 1)
	FIELD(GPIOA_IDR, IDR3, 3, 1)
	FIELD(GPIOA_IDR, IDR2, 2, 1)
	FIELD(GPIOA_IDR, IDR1, 1, 1)
	FIELD(GPIOA_IDR, IDR0, 0, 1)
REG32(GPIOA_ODR, 0x14)
	FIELD(GPIOA_ODR, ODR15, 15, 1)
	FIELD(GPIOA_ODR, ODR14, 14, 1)
	FIELD(GPIOA_ODR, ODR13, 13, 1)
	FIELD(GPIOA_ODR, ODR12, 12, 1)
	FIELD(GPIOA_ODR, ODR11, 11, 1)
	FIELD(GPIOA_ODR, ODR10, 10, 1)
	FIELD(GPIOA_ODR, ODR9, 9, 1)
	FIELD(GPIOA_ODR, ODR8, 8, 1)
	FIELD(GPIOA_ODR, ODR7, 7, 1)
	FIELD(GPIOA_ODR, ODR6, 6, 1)
	FIELD(GPIOA_ODR, ODR5, 5, 1)
	FIELD(GPIOA_ODR, ODR4, 4, 1)
	FIELD(GPIOA_ODR, ODR3, 3, 1)
	FIELD(GPIOA_ODR, ODR2, 2, 1)
	FIELD(GPIOA_ODR, ODR1, 1, 1)
	FIELD(GPIOA_ODR, ODR0, 0, 1)
REG32(GPIOA_BSRR, 0x18)
	FIELD(GPIOA_BSRR, BR15, 31, 1)
	FIELD(GPIOA_BSRR, BR14, 30, 1)
	FIELD(GPIOA_BSRR, BR13, 29, 1)
	FIELD(GPIOA_BSRR, BR12, 28, 1)
	FIELD(GPIOA_BSRR, BR11, 27, 1)
	FIELD(GPIOA_BSRR, BR10, 26, 1)
	FIELD(GPIOA_BSRR, BR9, 25, 1)
	FIELD(GPIOA_BSRR, BR8, 24, 1)
	FIELD(GPIOA_BSRR, BR7, 23, 1)
	FIELD(GPIOA_BSRR, BR6, 22, 1)
	FIELD(GPIOA_BSRR, BR5, 21, 1)
	FIELD(GPIOA_BSRR, BR4, 20, 1)
	FIELD(GPIOA_BSRR, BR3, 19, 1)
	FIELD(GPIOA_BSRR, BR2, 18, 1)
	FIELD(GPIOA_BSRR, BR1, 17, 1)
	FIELD(GPIOA_BSRR, BR0, 16, 1)
	FIELD(GPIOA_BSRR, BS15, 15, 1)
	FIELD(GPIOA_BSRR, BS14, 14, 1)
	FIELD(GPIOA_BSRR, BS13, 13, 1)
	FIELD(GPIOA_BSRR, BS12, 12, 1)
	FIELD(GPIOA_BSRR, BS11, 11, 1)
	FIELD(GPIOA_BSRR, BS10, 10, 1)
	FIELD(GPIOA_BSRR, BS9, 9, 1)
	FIELD(GPIOA_BSRR, BS8, 8, 1)
	FIELD(GPIOA_BSRR, BS7, 7, 1)
	FIELD(GPIOA_BSRR, BS6, 6, 1)
	FIELD(GPIOA_BSRR, BS5, 5, 1)
	FIELD(GPIOA_BSRR, BS4, 4, 1)
	FIELD(GPIOA_BSRR, BS3, 3, 1)
	FIELD(GPIOA_BSRR, BS2, 2, 1)
	FIELD(GPIOA_BSRR, BS1, 1, 1)
	FIELD(GPIOA_BSRR, BS0, 0, 1)
REG32(GPIOA_LCKR, 0x1c)
	FIELD(GPIOA_LCKR, LCKK, 16, 1)
	FIELD(GPIOA_LCKR, LCK15, 15, 1)
	FIELD(GPIOA_LCKR, LCK14, 14, 1)
	FIELD(GPIOA_LCKR, LCK13, 13, 1)
	FIELD(GPIOA_LCKR, LCK12, 12, 1)
	FIELD(GPIOA_LCKR, LCK11, 11, 1)
	FIELD(GPIOA_LCKR, LCK10, 10, 1)
	FIELD(GPIOA_LCKR, LCK9, 9, 1)
	FIELD(GPIOA_LCKR, LCK8, 8, 1)
	FIELD(GPIOA_LCKR, LCK7, 7, 1)
	FIELD(GPIOA_LCKR, LCK6, 6, 1)
	FIELD(GPIOA_LCKR, LCK5, 5, 1)
	FIELD(GPIOA_LCKR, LCK4, 4, 1)
	FIELD(GPIOA_LCKR, LCK3, 3, 1)
	FIELD(GPIOA_LCKR, LCK2, 2, 1)
	FIELD(GPIOA_LCKR, LCK1, 1, 1)
	FIELD(GPIOA_LCKR, LCK0, 0, 1)
REG32(GPIOA_AFRL, 0x20)
	FIELD(GPIOA_AFRL, AFRL7, 28, 4)
	FIELD(GPIOA_AFRL, AFRL6, 24, 4)
	FIELD(GPIOA_AFRL, AFRL5, 20, 4)
	FIELD(GPIOA_AFRL, AFRL4, 16, 4)
	FIELD(GPIOA_AFRL, AFRL3, 12, 4)
	FIELD(GPIOA_AFRL, AFRL2, 8, 4)
	FIELD(GPIOA_AFRL, AFRL1, 4, 4)
	FIELD(GPIOA_AFRL, AFRL0, 0, 4)
REG32(GPIOA_AFRH, 0x24)
	FIELD(GPIOA_AFRH, AFRH15, 28, 4)
	FIELD(GPIOA_AFRH, AFRH14, 24, 4)
	FIELD(GPIOA_AFRH, AFRH13, 20, 4)
	FIELD(GPIOA_AFRH, AFRH12, 16, 4)
	FIELD(GPIOA_AFRH, AFRH11, 12, 4)
	FIELD(GPIOA_AFRH, AFRH10, 8, 4)
	FIELD(GPIOA_AFRH, AFRH9, 4, 4)
	FIELD(GPIOA_AFRH, AFRH8, 0, 4)
REG32(GPIOA_BRR, 0x28)
	FIELD(GPIOA_BRR, BR0, 0, 1)
	FIELD(GPIOA_BRR, BR1, 1, 1)
	FIELD(GPIOA_BRR, BR2, 2, 1)
	FIELD(GPIOA_BRR, BR3, 3, 1)
	FIELD(GPIOA_BRR, BR4, 4, 1)
	FIELD(GPIOA_BRR, BR5, 5, 1)
	FIELD(GPIOA_BRR, BR6, 6, 1)
	FIELD(GPIOA_BRR, BR7, 7, 1)
	FIELD(GPIOA_BRR, BR8, 8, 1)
	FIELD(GPIOA_BRR, BR9, 9, 1)
	FIELD(GPIOA_BRR, BR10, 10, 1)
	FIELD(GPIOA_BRR, BR11, 11, 1)
	FIELD(GPIOA_BRR, BR12, 12, 1)
	FIELD(GPIOA_BRR, BR13, 13, 1)
	FIELD(GPIOA_BRR, BR14, 14, 1)
	FIELD(GPIOA_BRR, BR15, 15, 1)


static void stm32f7_gpioa_register_reset(STM32F7GPIOA *t) {
	t->MODER = 0xa8000000;
	t->OTYPER = 0x0;
	t->GPIOB_OSPEEDR = 0x0;
	t->PUPDR = 0x64000000;
	t->IDR = 0x0;
	t->ODR = 0x0;
	t->BSRR = 0x0;
	t->LCKR = 0x0;
	t->AFRL = 0x0;
	t->AFRH = 0x0;
	t->BRR = 0x0;

}

static uint64_t stm32f7_gpioa_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F7GPIOA *t = STM32F7_GPIOA(opaque);
	uint64_t ret;

	switch (offset) {
		case A_GPIOA_MODER:
			ret = t->MODER;
			break;
		case A_GPIOA_OTYPER:
			ret = t->OTYPER;
			break;
		case A_GPIOA_GPIOB_OSPEEDR:
			ret = t->GPIOB_OSPEEDR;
			break;
		case A_GPIOA_PUPDR:
			ret = t->PUPDR;
			break;
		case A_GPIOA_IDR:
			ret = t->IDR;
			break;
		case A_GPIOA_ODR:
			ret = t->ODR;
			break;
		case A_GPIOA_LCKR:
			ret = t->LCKR;
			break;
		case A_GPIOA_AFRL:
			ret = t->AFRL;
			break;
		case A_GPIOA_AFRH:
			ret = t->AFRH;
			break;
		case A_GPIOA_BRR:
			ret = t->BRR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 GPIOA read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f7_gpioa_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F7GPIOA *t = STM32F7_GPIOA(opaque);


	switch (offset) {
		case A_GPIOA_MODER ... A_GPIOA_MODER + 3:
			value = value << ((offset - A_GPIOA_MODER) << 3);
			offset = A_GPIOA_MODER;
			break;
		case A_GPIOA_OTYPER ... A_GPIOA_OTYPER + 3:
			value = value << ((offset - A_GPIOA_OTYPER) << 3);
			offset = A_GPIOA_OTYPER;
			break;
		case A_GPIOA_GPIOB_OSPEEDR ... A_GPIOA_GPIOB_OSPEEDR + 3:
			value = value << ((offset - A_GPIOA_GPIOB_OSPEEDR) << 3);
			offset = A_GPIOA_GPIOB_OSPEEDR;
			break;
		case A_GPIOA_PUPDR ... A_GPIOA_PUPDR + 3:
			value = value << ((offset - A_GPIOA_PUPDR) << 3);
			offset = A_GPIOA_PUPDR;
			break;
		case A_GPIOA_ODR ... A_GPIOA_ODR + 3:
			value = value << ((offset - A_GPIOA_ODR) << 3);
			offset = A_GPIOA_ODR;
			break;
		case A_GPIOA_BSRR ... A_GPIOA_BSRR + 3:
			value = value << ((offset - A_GPIOA_BSRR) << 3);
			offset = A_GPIOA_BSRR;
			break;
		case A_GPIOA_LCKR ... A_GPIOA_LCKR + 3:
			value = value << ((offset - A_GPIOA_LCKR) << 3);
			offset = A_GPIOA_LCKR;
			break;
		case A_GPIOA_AFRL ... A_GPIOA_AFRL + 3:
			value = value << ((offset - A_GPIOA_AFRL) << 3);
			offset = A_GPIOA_AFRL;
			break;
		case A_GPIOA_AFRH ... A_GPIOA_AFRH + 3:
			value = value << ((offset - A_GPIOA_AFRH) << 3);
			offset = A_GPIOA_AFRH;
			break;
		case A_GPIOA_BRR ... A_GPIOA_BRR + 3:
			value = value << ((offset - A_GPIOA_BRR) << 3);
			offset = A_GPIOA_BRR;
			break;

		default: break;
	}

	switch (offset) {
		case A_GPIOA_MODER:
			t->MODER = value;
			break;
		case A_GPIOA_OTYPER:
			t->OTYPER = value;
			break;
		case A_GPIOA_GPIOB_OSPEEDR:
			t->GPIOB_OSPEEDR = value;
			break;
		case A_GPIOA_PUPDR:
			t->PUPDR = value;
			break;
		case A_GPIOA_ODR:
			t->ODR = value;
			break;
		case A_GPIOA_BSRR:
			t->BSRR = value;
			break;
		case A_GPIOA_LCKR:
			t->LCKR = value;
			break;
		case A_GPIOA_AFRL:
			t->AFRL = value;
			break;
		case A_GPIOA_AFRH:
			t->AFRH = value;
			break;
		case A_GPIOA_BRR:
			t->BRR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 GPIOA write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f7_gpioa_ops = {
	.read = stm32f7_gpioa_read,
	.write = stm32f7_gpioa_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f7_gpioa_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F7GPIOA *t = STM32F7_GPIOA(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f7_gpioa_ops, t, "stm32f7-gpioa", STM32F7_GPIOA_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void stm32f7_gpioa_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32f7_gpioa_reset_enter(Object *obj, ResetType type) {
	STM32F7GPIOA *t = STM32F7_GPIOA(obj);
	stm32f7_gpioa_register_reset(t);
}

static const VMStateDescription stm32f7_gpioa_vmstate = {
	.name = "stm32f7-gpioa",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(MODER, STM32F7GPIOA),
		VMSTATE_UINT32(OTYPER, STM32F7GPIOA),
		VMSTATE_UINT32(GPIOB_OSPEEDR, STM32F7GPIOA),
		VMSTATE_UINT32(PUPDR, STM32F7GPIOA),
		VMSTATE_UINT32(IDR, STM32F7GPIOA),
		VMSTATE_UINT32(ODR, STM32F7GPIOA),
		VMSTATE_UINT32(BSRR, STM32F7GPIOA),
		VMSTATE_UINT32(LCKR, STM32F7GPIOA),
		VMSTATE_UINT32(AFRL, STM32F7GPIOA),
		VMSTATE_UINT32(AFRH, STM32F7GPIOA),
		VMSTATE_UINT32(BRR, STM32F7GPIOA),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f7_gpioa_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f7_gpioa_vmstate;
	dc->realize = stm32f7_gpioa_realize;
	rc->phases.enter = stm32f7_gpioa_reset_enter;

}

static const TypeInfo stm32f7_gpioa_info = {
	.name = TYPE_STM32F7_GPIOA,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F7GPIOA),
	.instance_init = stm32f7_gpioa_init,
	.class_init = stm32f7_gpioa_class_init,
};

static void stm32f7_gpioa_register_types(void) {
	type_register_static(&stm32f7_gpioa_info);
}

type_init(stm32f7_gpioa_register_types);

#define TYPE_STM32F7_GPIOB "stm32f7-gpiob"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F7GPIOB, STM32F7_GPIOB)

struct STM32F7GPIOB {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/*registers*/
	uint32_t MODER;
	uint32_t OTYPER;
	uint32_t GPIOB_OSPEEDR;
	uint32_t PUPDR;
	uint32_t IDR;
	uint32_t ODR;
	uint32_t BSRR;
	uint32_t LCKR;
	uint32_t AFRL;
	uint32_t AFRH;
	uint32_t BRR;
	/* base */
	uint32_t base;

};
#define STM32F7_GPIOB_SIZE				0x2c

REG32(GPIOB_MODER, 0x0)
	FIELD(GPIOB_MODER, MODER15, 30, 2)
	FIELD(GPIOB_MODER, MODER14, 28, 2)
	FIELD(GPIOB_MODER, MODER13, 26, 2)
	FIELD(GPIOB_MODER, MODER12, 24, 2)
	FIELD(GPIOB_MODER, MODER11, 22, 2)
	FIELD(GPIOB_MODER, MODER10, 20, 2)
	FIELD(GPIOB_MODER, MODER9, 18, 2)
	FIELD(GPIOB_MODER, MODER8, 16, 2)
	FIELD(GPIOB_MODER, MODER7, 14, 2)
	FIELD(GPIOB_MODER, MODER6, 12, 2)
	FIELD(GPIOB_MODER, MODER5, 10, 2)
	FIELD(GPIOB_MODER, MODER4, 8, 2)
	FIELD(GPIOB_MODER, MODER3, 6, 2)
	FIELD(GPIOB_MODER, MODER2, 4, 2)
	FIELD(GPIOB_MODER, MODER1, 2, 2)
	FIELD(GPIOB_MODER, MODER0, 0, 2)
REG32(GPIOB_OTYPER, 0x4)
	FIELD(GPIOB_OTYPER, OT15, 15, 1)
	FIELD(GPIOB_OTYPER, OT14, 14, 1)
	FIELD(GPIOB_OTYPER, OT13, 13, 1)
	FIELD(GPIOB_OTYPER, OT12, 12, 1)
	FIELD(GPIOB_OTYPER, OT11, 11, 1)
	FIELD(GPIOB_OTYPER, OT10, 10, 1)
	FIELD(GPIOB_OTYPER, OT9, 9, 1)
	FIELD(GPIOB_OTYPER, OT8, 8, 1)
	FIELD(GPIOB_OTYPER, OT7, 7, 1)
	FIELD(GPIOB_OTYPER, OT6, 6, 1)
	FIELD(GPIOB_OTYPER, OT5, 5, 1)
	FIELD(GPIOB_OTYPER, OT4, 4, 1)
	FIELD(GPIOB_OTYPER, OT3, 3, 1)
	FIELD(GPIOB_OTYPER, OT2, 2, 1)
	FIELD(GPIOB_OTYPER, OT1, 1, 1)
	FIELD(GPIOB_OTYPER, OT0, 0, 1)
REG32(GPIOB_GPIOB_OSPEEDR, 0x8)
	FIELD(GPIOB_GPIOB_OSPEEDR, OSPEEDR15, 30, 2)
	FIELD(GPIOB_GPIOB_OSPEEDR, OSPEEDR14, 28, 2)
	FIELD(GPIOB_GPIOB_OSPEEDR, OSPEEDR13, 26, 2)
	FIELD(GPIOB_GPIOB_OSPEEDR, OSPEEDR12, 24, 2)
	FIELD(GPIOB_GPIOB_OSPEEDR, OSPEEDR11, 22, 2)
	FIELD(GPIOB_GPIOB_OSPEEDR, OSPEEDR10, 20, 2)
	FIELD(GPIOB_GPIOB_OSPEEDR, OSPEEDR9, 18, 2)
	FIELD(GPIOB_GPIOB_OSPEEDR, OSPEEDR8, 16, 2)
	FIELD(GPIOB_GPIOB_OSPEEDR, OSPEEDR7, 14, 2)
	FIELD(GPIOB_GPIOB_OSPEEDR, OSPEEDR6, 12, 2)
	FIELD(GPIOB_GPIOB_OSPEEDR, OSPEEDR5, 10, 2)
	FIELD(GPIOB_GPIOB_OSPEEDR, OSPEEDR4, 8, 2)
	FIELD(GPIOB_GPIOB_OSPEEDR, OSPEEDR3, 6, 2)
	FIELD(GPIOB_GPIOB_OSPEEDR, OSPEEDR2, 4, 2)
	FIELD(GPIOB_GPIOB_OSPEEDR, OSPEEDR1, 2, 2)
	FIELD(GPIOB_GPIOB_OSPEEDR, OSPEEDR0, 0, 2)
REG32(GPIOB_PUPDR, 0xc)
	FIELD(GPIOB_PUPDR, PUPDR15, 30, 2)
	FIELD(GPIOB_PUPDR, PUPDR14, 28, 2)
	FIELD(GPIOB_PUPDR, PUPDR13, 26, 2)
	FIELD(GPIOB_PUPDR, PUPDR12, 24, 2)
	FIELD(GPIOB_PUPDR, PUPDR11, 22, 2)
	FIELD(GPIOB_PUPDR, PUPDR10, 20, 2)
	FIELD(GPIOB_PUPDR, PUPDR9, 18, 2)
	FIELD(GPIOB_PUPDR, PUPDR8, 16, 2)
	FIELD(GPIOB_PUPDR, PUPDR7, 14, 2)
	FIELD(GPIOB_PUPDR, PUPDR6, 12, 2)
	FIELD(GPIOB_PUPDR, PUPDR5, 10, 2)
	FIELD(GPIOB_PUPDR, PUPDR4, 8, 2)
	FIELD(GPIOB_PUPDR, PUPDR3, 6, 2)
	FIELD(GPIOB_PUPDR, PUPDR2, 4, 2)
	FIELD(GPIOB_PUPDR, PUPDR1, 2, 2)
	FIELD(GPIOB_PUPDR, PUPDR0, 0, 2)
REG32(GPIOB_IDR, 0x10)
	FIELD(GPIOB_IDR, IDR15, 15, 1)
	FIELD(GPIOB_IDR, IDR14, 14, 1)
	FIELD(GPIOB_IDR, IDR13, 13, 1)
	FIELD(GPIOB_IDR, IDR12, 12, 1)
	FIELD(GPIOB_IDR, IDR11, 11, 1)
	FIELD(GPIOB_IDR, IDR10, 10, 1)
	FIELD(GPIOB_IDR, IDR9, 9, 1)
	FIELD(GPIOB_IDR, IDR8, 8, 1)
	FIELD(GPIOB_IDR, IDR7, 7, 1)
	FIELD(GPIOB_IDR, IDR6, 6, 1)
	FIELD(GPIOB_IDR, IDR5, 5, 1)
	FIELD(GPIOB_IDR, IDR4, 4, 1)
	FIELD(GPIOB_IDR, IDR3, 3, 1)
	FIELD(GPIOB_IDR, IDR2, 2, 1)
	FIELD(GPIOB_IDR, IDR1, 1, 1)
	FIELD(GPIOB_IDR, IDR0, 0, 1)
REG32(GPIOB_ODR, 0x14)
	FIELD(GPIOB_ODR, ODR15, 15, 1)
	FIELD(GPIOB_ODR, ODR14, 14, 1)
	FIELD(GPIOB_ODR, ODR13, 13, 1)
	FIELD(GPIOB_ODR, ODR12, 12, 1)
	FIELD(GPIOB_ODR, ODR11, 11, 1)
	FIELD(GPIOB_ODR, ODR10, 10, 1)
	FIELD(GPIOB_ODR, ODR9, 9, 1)
	FIELD(GPIOB_ODR, ODR8, 8, 1)
	FIELD(GPIOB_ODR, ODR7, 7, 1)
	FIELD(GPIOB_ODR, ODR6, 6, 1)
	FIELD(GPIOB_ODR, ODR5, 5, 1)
	FIELD(GPIOB_ODR, ODR4, 4, 1)
	FIELD(GPIOB_ODR, ODR3, 3, 1)
	FIELD(GPIOB_ODR, ODR2, 2, 1)
	FIELD(GPIOB_ODR, ODR1, 1, 1)
	FIELD(GPIOB_ODR, ODR0, 0, 1)
REG32(GPIOB_BSRR, 0x18)
	FIELD(GPIOB_BSRR, BR15, 31, 1)
	FIELD(GPIOB_BSRR, BR14, 30, 1)
	FIELD(GPIOB_BSRR, BR13, 29, 1)
	FIELD(GPIOB_BSRR, BR12, 28, 1)
	FIELD(GPIOB_BSRR, BR11, 27, 1)
	FIELD(GPIOB_BSRR, BR10, 26, 1)
	FIELD(GPIOB_BSRR, BR9, 25, 1)
	FIELD(GPIOB_BSRR, BR8, 24, 1)
	FIELD(GPIOB_BSRR, BR7, 23, 1)
	FIELD(GPIOB_BSRR, BR6, 22, 1)
	FIELD(GPIOB_BSRR, BR5, 21, 1)
	FIELD(GPIOB_BSRR, BR4, 20, 1)
	FIELD(GPIOB_BSRR, BR3, 19, 1)
	FIELD(GPIOB_BSRR, BR2, 18, 1)
	FIELD(GPIOB_BSRR, BR1, 17, 1)
	FIELD(GPIOB_BSRR, BR0, 16, 1)
	FIELD(GPIOB_BSRR, BS15, 15, 1)
	FIELD(GPIOB_BSRR, BS14, 14, 1)
	FIELD(GPIOB_BSRR, BS13, 13, 1)
	FIELD(GPIOB_BSRR, BS12, 12, 1)
	FIELD(GPIOB_BSRR, BS11, 11, 1)
	FIELD(GPIOB_BSRR, BS10, 10, 1)
	FIELD(GPIOB_BSRR, BS9, 9, 1)
	FIELD(GPIOB_BSRR, BS8, 8, 1)
	FIELD(GPIOB_BSRR, BS7, 7, 1)
	FIELD(GPIOB_BSRR, BS6, 6, 1)
	FIELD(GPIOB_BSRR, BS5, 5, 1)
	FIELD(GPIOB_BSRR, BS4, 4, 1)
	FIELD(GPIOB_BSRR, BS3, 3, 1)
	FIELD(GPIOB_BSRR, BS2, 2, 1)
	FIELD(GPIOB_BSRR, BS1, 1, 1)
	FIELD(GPIOB_BSRR, BS0, 0, 1)
REG32(GPIOB_LCKR, 0x1c)
	FIELD(GPIOB_LCKR, LCKK, 16, 1)
	FIELD(GPIOB_LCKR, LCK15, 15, 1)
	FIELD(GPIOB_LCKR, LCK14, 14, 1)
	FIELD(GPIOB_LCKR, LCK13, 13, 1)
	FIELD(GPIOB_LCKR, LCK12, 12, 1)
	FIELD(GPIOB_LCKR, LCK11, 11, 1)
	FIELD(GPIOB_LCKR, LCK10, 10, 1)
	FIELD(GPIOB_LCKR, LCK9, 9, 1)
	FIELD(GPIOB_LCKR, LCK8, 8, 1)
	FIELD(GPIOB_LCKR, LCK7, 7, 1)
	FIELD(GPIOB_LCKR, LCK6, 6, 1)
	FIELD(GPIOB_LCKR, LCK5, 5, 1)
	FIELD(GPIOB_LCKR, LCK4, 4, 1)
	FIELD(GPIOB_LCKR, LCK3, 3, 1)
	FIELD(GPIOB_LCKR, LCK2, 2, 1)
	FIELD(GPIOB_LCKR, LCK1, 1, 1)
	FIELD(GPIOB_LCKR, LCK0, 0, 1)
REG32(GPIOB_AFRL, 0x20)
	FIELD(GPIOB_AFRL, AFRL7, 28, 4)
	FIELD(GPIOB_AFRL, AFRL6, 24, 4)
	FIELD(GPIOB_AFRL, AFRL5, 20, 4)
	FIELD(GPIOB_AFRL, AFRL4, 16, 4)
	FIELD(GPIOB_AFRL, AFRL3, 12, 4)
	FIELD(GPIOB_AFRL, AFRL2, 8, 4)
	FIELD(GPIOB_AFRL, AFRL1, 4, 4)
	FIELD(GPIOB_AFRL, AFRL0, 0, 4)
REG32(GPIOB_AFRH, 0x24)
	FIELD(GPIOB_AFRH, AFRH15, 28, 4)
	FIELD(GPIOB_AFRH, AFRH14, 24, 4)
	FIELD(GPIOB_AFRH, AFRH13, 20, 4)
	FIELD(GPIOB_AFRH, AFRH12, 16, 4)
	FIELD(GPIOB_AFRH, AFRH11, 12, 4)
	FIELD(GPIOB_AFRH, AFRH10, 8, 4)
	FIELD(GPIOB_AFRH, AFRH9, 4, 4)
	FIELD(GPIOB_AFRH, AFRH8, 0, 4)
REG32(GPIOB_BRR, 0x28)
	FIELD(GPIOB_BRR, BR0, 0, 1)
	FIELD(GPIOB_BRR, BR1, 1, 1)
	FIELD(GPIOB_BRR, BR2, 2, 1)
	FIELD(GPIOB_BRR, BR3, 3, 1)
	FIELD(GPIOB_BRR, BR4, 4, 1)
	FIELD(GPIOB_BRR, BR5, 5, 1)
	FIELD(GPIOB_BRR, BR6, 6, 1)
	FIELD(GPIOB_BRR, BR7, 7, 1)
	FIELD(GPIOB_BRR, BR8, 8, 1)
	FIELD(GPIOB_BRR, BR9, 9, 1)
	FIELD(GPIOB_BRR, BR10, 10, 1)
	FIELD(GPIOB_BRR, BR11, 11, 1)
	FIELD(GPIOB_BRR, BR12, 12, 1)
	FIELD(GPIOB_BRR, BR13, 13, 1)
	FIELD(GPIOB_BRR, BR14, 14, 1)
	FIELD(GPIOB_BRR, BR15, 15, 1)


static void stm32f7_gpiob_register_reset(STM32F7GPIOB *t) {
	t->MODER = 0x280;
	t->OTYPER = 0x0;
	t->GPIOB_OSPEEDR = 0xc0;
	t->PUPDR = 0x100;
	t->IDR = 0x0;
	t->ODR = 0x0;
	t->BSRR = 0x0;
	t->LCKR = 0x0;
	t->AFRL = 0x0;
	t->AFRH = 0x0;
	t->BRR = 0x0;

}

static uint64_t stm32f7_gpiob_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F7GPIOB *t = STM32F7_GPIOB(opaque);
	uint64_t ret;

	switch (offset) {
		case A_GPIOB_MODER:
			ret = t->MODER;
			break;
		case A_GPIOB_OTYPER:
			ret = t->OTYPER;
			break;
		case A_GPIOB_GPIOB_OSPEEDR:
			ret = t->GPIOB_OSPEEDR;
			break;
		case A_GPIOB_PUPDR:
			ret = t->PUPDR;
			break;
		case A_GPIOB_IDR:
			ret = t->IDR;
			break;
		case A_GPIOB_ODR:
			ret = t->ODR;
			break;
		case A_GPIOB_LCKR:
			ret = t->LCKR;
			break;
		case A_GPIOB_AFRL:
			ret = t->AFRL;
			break;
		case A_GPIOB_AFRH:
			ret = t->AFRH;
			break;
		case A_GPIOB_BRR:
			ret = t->BRR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 GPIOB read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f7_gpiob_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F7GPIOB *t = STM32F7_GPIOB(opaque);


	switch (offset) {
		case A_GPIOB_MODER ... A_GPIOB_MODER + 3:
			value = value << ((offset - A_GPIOB_MODER) << 3);
			offset = A_GPIOB_MODER;
			break;
		case A_GPIOB_OTYPER ... A_GPIOB_OTYPER + 3:
			value = value << ((offset - A_GPIOB_OTYPER) << 3);
			offset = A_GPIOB_OTYPER;
			break;
		case A_GPIOB_GPIOB_OSPEEDR ... A_GPIOB_GPIOB_OSPEEDR + 3:
			value = value << ((offset - A_GPIOB_GPIOB_OSPEEDR) << 3);
			offset = A_GPIOB_GPIOB_OSPEEDR;
			break;
		case A_GPIOB_PUPDR ... A_GPIOB_PUPDR + 3:
			value = value << ((offset - A_GPIOB_PUPDR) << 3);
			offset = A_GPIOB_PUPDR;
			break;
		case A_GPIOB_ODR ... A_GPIOB_ODR + 3:
			value = value << ((offset - A_GPIOB_ODR) << 3);
			offset = A_GPIOB_ODR;
			break;
		case A_GPIOB_BSRR ... A_GPIOB_BSRR + 3:
			value = value << ((offset - A_GPIOB_BSRR) << 3);
			offset = A_GPIOB_BSRR;
			break;
		case A_GPIOB_LCKR ... A_GPIOB_LCKR + 3:
			value = value << ((offset - A_GPIOB_LCKR) << 3);
			offset = A_GPIOB_LCKR;
			break;
		case A_GPIOB_AFRL ... A_GPIOB_AFRL + 3:
			value = value << ((offset - A_GPIOB_AFRL) << 3);
			offset = A_GPIOB_AFRL;
			break;
		case A_GPIOB_AFRH ... A_GPIOB_AFRH + 3:
			value = value << ((offset - A_GPIOB_AFRH) << 3);
			offset = A_GPIOB_AFRH;
			break;
		case A_GPIOB_BRR ... A_GPIOB_BRR + 3:
			value = value << ((offset - A_GPIOB_BRR) << 3);
			offset = A_GPIOB_BRR;
			break;

		default: break;
	}

	switch (offset) {
		case A_GPIOB_MODER:
			t->MODER = value;
			break;
		case A_GPIOB_OTYPER:
			t->OTYPER = value;
			break;
		case A_GPIOB_GPIOB_OSPEEDR:
			t->GPIOB_OSPEEDR = value;
			break;
		case A_GPIOB_PUPDR:
			t->PUPDR = value;
			break;
		case A_GPIOB_ODR:
			t->ODR = value;
			break;
		case A_GPIOB_BSRR:
			t->BSRR = value;
			break;
		case A_GPIOB_LCKR:
			t->LCKR = value;
			break;
		case A_GPIOB_AFRL:
			t->AFRL = value;
			break;
		case A_GPIOB_AFRH:
			t->AFRH = value;
			break;
		case A_GPIOB_BRR:
			t->BRR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 GPIOB write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f7_gpiob_ops = {
	.read = stm32f7_gpiob_read,
	.write = stm32f7_gpiob_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f7_gpiob_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F7GPIOB *t = STM32F7_GPIOB(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f7_gpiob_ops, t, "stm32f7-gpiob", STM32F7_GPIOB_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void stm32f7_gpiob_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32f7_gpiob_reset_enter(Object *obj, ResetType type) {
	STM32F7GPIOB *t = STM32F7_GPIOB(obj);
	stm32f7_gpiob_register_reset(t);
}

static const VMStateDescription stm32f7_gpiob_vmstate = {
	.name = "stm32f7-gpiob",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(MODER, STM32F7GPIOB),
		VMSTATE_UINT32(OTYPER, STM32F7GPIOB),
		VMSTATE_UINT32(GPIOB_OSPEEDR, STM32F7GPIOB),
		VMSTATE_UINT32(PUPDR, STM32F7GPIOB),
		VMSTATE_UINT32(IDR, STM32F7GPIOB),
		VMSTATE_UINT32(ODR, STM32F7GPIOB),
		VMSTATE_UINT32(BSRR, STM32F7GPIOB),
		VMSTATE_UINT32(LCKR, STM32F7GPIOB),
		VMSTATE_UINT32(AFRL, STM32F7GPIOB),
		VMSTATE_UINT32(AFRH, STM32F7GPIOB),
		VMSTATE_UINT32(BRR, STM32F7GPIOB),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f7_gpiob_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f7_gpiob_vmstate;
	dc->realize = stm32f7_gpiob_realize;
	rc->phases.enter = stm32f7_gpiob_reset_enter;

}

static const TypeInfo stm32f7_gpiob_info = {
	.name = TYPE_STM32F7_GPIOB,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F7GPIOB),
	.instance_init = stm32f7_gpiob_init,
	.class_init = stm32f7_gpiob_class_init,
};

static void stm32f7_gpiob_register_types(void) {
	type_register_static(&stm32f7_gpiob_info);
}

type_init(stm32f7_gpiob_register_types);

#define TYPE_STM32F7_GPIOD "stm32f7-gpiod"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F7GPIOD, STM32F7_GPIOD)

struct STM32F7GPIOD {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/*registers*/
	uint32_t MODER;
	uint32_t OTYPER;
	uint32_t GPIOB_OSPEEDR;
	uint32_t PUPDR;
	uint32_t IDR;
	uint32_t ODR;
	uint32_t BSRR;
	uint32_t LCKR;
	uint32_t AFRL;
	uint32_t AFRH;
	uint32_t BRR;
	/* base */
	uint32_t base;

};
#define STM32F7_GPIOD_SIZE				0x2c

REG32(GPIOD_MODER, 0x0)
	FIELD(GPIOD_MODER, MODER15, 30, 2)
	FIELD(GPIOD_MODER, MODER14, 28, 2)
	FIELD(GPIOD_MODER, MODER13, 26, 2)
	FIELD(GPIOD_MODER, MODER12, 24, 2)
	FIELD(GPIOD_MODER, MODER11, 22, 2)
	FIELD(GPIOD_MODER, MODER10, 20, 2)
	FIELD(GPIOD_MODER, MODER9, 18, 2)
	FIELD(GPIOD_MODER, MODER8, 16, 2)
	FIELD(GPIOD_MODER, MODER7, 14, 2)
	FIELD(GPIOD_MODER, MODER6, 12, 2)
	FIELD(GPIOD_MODER, MODER5, 10, 2)
	FIELD(GPIOD_MODER, MODER4, 8, 2)
	FIELD(GPIOD_MODER, MODER3, 6, 2)
	FIELD(GPIOD_MODER, MODER2, 4, 2)
	FIELD(GPIOD_MODER, MODER1, 2, 2)
	FIELD(GPIOD_MODER, MODER0, 0, 2)
REG32(GPIOD_OTYPER, 0x4)
	FIELD(GPIOD_OTYPER, OT15, 15, 1)
	FIELD(GPIOD_OTYPER, OT14, 14, 1)
	FIELD(GPIOD_OTYPER, OT13, 13, 1)
	FIELD(GPIOD_OTYPER, OT12, 12, 1)
	FIELD(GPIOD_OTYPER, OT11, 11, 1)
	FIELD(GPIOD_OTYPER, OT10, 10, 1)
	FIELD(GPIOD_OTYPER, OT9, 9, 1)
	FIELD(GPIOD_OTYPER, OT8, 8, 1)
	FIELD(GPIOD_OTYPER, OT7, 7, 1)
	FIELD(GPIOD_OTYPER, OT6, 6, 1)
	FIELD(GPIOD_OTYPER, OT5, 5, 1)
	FIELD(GPIOD_OTYPER, OT4, 4, 1)
	FIELD(GPIOD_OTYPER, OT3, 3, 1)
	FIELD(GPIOD_OTYPER, OT2, 2, 1)
	FIELD(GPIOD_OTYPER, OT1, 1, 1)
	FIELD(GPIOD_OTYPER, OT0, 0, 1)
REG32(GPIOD_GPIOB_OSPEEDR, 0x8)
	FIELD(GPIOD_GPIOB_OSPEEDR, OSPEEDR15, 30, 2)
	FIELD(GPIOD_GPIOB_OSPEEDR, OSPEEDR14, 28, 2)
	FIELD(GPIOD_GPIOB_OSPEEDR, OSPEEDR13, 26, 2)
	FIELD(GPIOD_GPIOB_OSPEEDR, OSPEEDR12, 24, 2)
	FIELD(GPIOD_GPIOB_OSPEEDR, OSPEEDR11, 22, 2)
	FIELD(GPIOD_GPIOB_OSPEEDR, OSPEEDR10, 20, 2)
	FIELD(GPIOD_GPIOB_OSPEEDR, OSPEEDR9, 18, 2)
	FIELD(GPIOD_GPIOB_OSPEEDR, OSPEEDR8, 16, 2)
	FIELD(GPIOD_GPIOB_OSPEEDR, OSPEEDR7, 14, 2)
	FIELD(GPIOD_GPIOB_OSPEEDR, OSPEEDR6, 12, 2)
	FIELD(GPIOD_GPIOB_OSPEEDR, OSPEEDR5, 10, 2)
	FIELD(GPIOD_GPIOB_OSPEEDR, OSPEEDR4, 8, 2)
	FIELD(GPIOD_GPIOB_OSPEEDR, OSPEEDR3, 6, 2)
	FIELD(GPIOD_GPIOB_OSPEEDR, OSPEEDR2, 4, 2)
	FIELD(GPIOD_GPIOB_OSPEEDR, OSPEEDR1, 2, 2)
	FIELD(GPIOD_GPIOB_OSPEEDR, OSPEEDR0, 0, 2)
REG32(GPIOD_PUPDR, 0xc)
	FIELD(GPIOD_PUPDR, PUPDR15, 30, 2)
	FIELD(GPIOD_PUPDR, PUPDR14, 28, 2)
	FIELD(GPIOD_PUPDR, PUPDR13, 26, 2)
	FIELD(GPIOD_PUPDR, PUPDR12, 24, 2)
	FIELD(GPIOD_PUPDR, PUPDR11, 22, 2)
	FIELD(GPIOD_PUPDR, PUPDR10, 20, 2)
	FIELD(GPIOD_PUPDR, PUPDR9, 18, 2)
	FIELD(GPIOD_PUPDR, PUPDR8, 16, 2)
	FIELD(GPIOD_PUPDR, PUPDR7, 14, 2)
	FIELD(GPIOD_PUPDR, PUPDR6, 12, 2)
	FIELD(GPIOD_PUPDR, PUPDR5, 10, 2)
	FIELD(GPIOD_PUPDR, PUPDR4, 8, 2)
	FIELD(GPIOD_PUPDR, PUPDR3, 6, 2)
	FIELD(GPIOD_PUPDR, PUPDR2, 4, 2)
	FIELD(GPIOD_PUPDR, PUPDR1, 2, 2)
	FIELD(GPIOD_PUPDR, PUPDR0, 0, 2)
REG32(GPIOD_IDR, 0x10)
	FIELD(GPIOD_IDR, IDR15, 15, 1)
	FIELD(GPIOD_IDR, IDR14, 14, 1)
	FIELD(GPIOD_IDR, IDR13, 13, 1)
	FIELD(GPIOD_IDR, IDR12, 12, 1)
	FIELD(GPIOD_IDR, IDR11, 11, 1)
	FIELD(GPIOD_IDR, IDR10, 10, 1)
	FIELD(GPIOD_IDR, IDR9, 9, 1)
	FIELD(GPIOD_IDR, IDR8, 8, 1)
	FIELD(GPIOD_IDR, IDR7, 7, 1)
	FIELD(GPIOD_IDR, IDR6, 6, 1)
	FIELD(GPIOD_IDR, IDR5, 5, 1)
	FIELD(GPIOD_IDR, IDR4, 4, 1)
	FIELD(GPIOD_IDR, IDR3, 3, 1)
	FIELD(GPIOD_IDR, IDR2, 2, 1)
	FIELD(GPIOD_IDR, IDR1, 1, 1)
	FIELD(GPIOD_IDR, IDR0, 0, 1)
REG32(GPIOD_ODR, 0x14)
	FIELD(GPIOD_ODR, ODR15, 15, 1)
	FIELD(GPIOD_ODR, ODR14, 14, 1)
	FIELD(GPIOD_ODR, ODR13, 13, 1)
	FIELD(GPIOD_ODR, ODR12, 12, 1)
	FIELD(GPIOD_ODR, ODR11, 11, 1)
	FIELD(GPIOD_ODR, ODR10, 10, 1)
	FIELD(GPIOD_ODR, ODR9, 9, 1)
	FIELD(GPIOD_ODR, ODR8, 8, 1)
	FIELD(GPIOD_ODR, ODR7, 7, 1)
	FIELD(GPIOD_ODR, ODR6, 6, 1)
	FIELD(GPIOD_ODR, ODR5, 5, 1)
	FIELD(GPIOD_ODR, ODR4, 4, 1)
	FIELD(GPIOD_ODR, ODR3, 3, 1)
	FIELD(GPIOD_ODR, ODR2, 2, 1)
	FIELD(GPIOD_ODR, ODR1, 1, 1)
	FIELD(GPIOD_ODR, ODR0, 0, 1)
REG32(GPIOD_BSRR, 0x18)
	FIELD(GPIOD_BSRR, BR15, 31, 1)
	FIELD(GPIOD_BSRR, BR14, 30, 1)
	FIELD(GPIOD_BSRR, BR13, 29, 1)
	FIELD(GPIOD_BSRR, BR12, 28, 1)
	FIELD(GPIOD_BSRR, BR11, 27, 1)
	FIELD(GPIOD_BSRR, BR10, 26, 1)
	FIELD(GPIOD_BSRR, BR9, 25, 1)
	FIELD(GPIOD_BSRR, BR8, 24, 1)
	FIELD(GPIOD_BSRR, BR7, 23, 1)
	FIELD(GPIOD_BSRR, BR6, 22, 1)
	FIELD(GPIOD_BSRR, BR5, 21, 1)
	FIELD(GPIOD_BSRR, BR4, 20, 1)
	FIELD(GPIOD_BSRR, BR3, 19, 1)
	FIELD(GPIOD_BSRR, BR2, 18, 1)
	FIELD(GPIOD_BSRR, BR1, 17, 1)
	FIELD(GPIOD_BSRR, BR0, 16, 1)
	FIELD(GPIOD_BSRR, BS15, 15, 1)
	FIELD(GPIOD_BSRR, BS14, 14, 1)
	FIELD(GPIOD_BSRR, BS13, 13, 1)
	FIELD(GPIOD_BSRR, BS12, 12, 1)
	FIELD(GPIOD_BSRR, BS11, 11, 1)
	FIELD(GPIOD_BSRR, BS10, 10, 1)
	FIELD(GPIOD_BSRR, BS9, 9, 1)
	FIELD(GPIOD_BSRR, BS8, 8, 1)
	FIELD(GPIOD_BSRR, BS7, 7, 1)
	FIELD(GPIOD_BSRR, BS6, 6, 1)
	FIELD(GPIOD_BSRR, BS5, 5, 1)
	FIELD(GPIOD_BSRR, BS4, 4, 1)
	FIELD(GPIOD_BSRR, BS3, 3, 1)
	FIELD(GPIOD_BSRR, BS2, 2, 1)
	FIELD(GPIOD_BSRR, BS1, 1, 1)
	FIELD(GPIOD_BSRR, BS0, 0, 1)
REG32(GPIOD_LCKR, 0x1c)
	FIELD(GPIOD_LCKR, LCKK, 16, 1)
	FIELD(GPIOD_LCKR, LCK15, 15, 1)
	FIELD(GPIOD_LCKR, LCK14, 14, 1)
	FIELD(GPIOD_LCKR, LCK13, 13, 1)
	FIELD(GPIOD_LCKR, LCK12, 12, 1)
	FIELD(GPIOD_LCKR, LCK11, 11, 1)
	FIELD(GPIOD_LCKR, LCK10, 10, 1)
	FIELD(GPIOD_LCKR, LCK9, 9, 1)
	FIELD(GPIOD_LCKR, LCK8, 8, 1)
	FIELD(GPIOD_LCKR, LCK7, 7, 1)
	FIELD(GPIOD_LCKR, LCK6, 6, 1)
	FIELD(GPIOD_LCKR, LCK5, 5, 1)
	FIELD(GPIOD_LCKR, LCK4, 4, 1)
	FIELD(GPIOD_LCKR, LCK3, 3, 1)
	FIELD(GPIOD_LCKR, LCK2, 2, 1)
	FIELD(GPIOD_LCKR, LCK1, 1, 1)
	FIELD(GPIOD_LCKR, LCK0, 0, 1)
REG32(GPIOD_AFRL, 0x20)
	FIELD(GPIOD_AFRL, AFRL7, 28, 4)
	FIELD(GPIOD_AFRL, AFRL6, 24, 4)
	FIELD(GPIOD_AFRL, AFRL5, 20, 4)
	FIELD(GPIOD_AFRL, AFRL4, 16, 4)
	FIELD(GPIOD_AFRL, AFRL3, 12, 4)
	FIELD(GPIOD_AFRL, AFRL2, 8, 4)
	FIELD(GPIOD_AFRL, AFRL1, 4, 4)
	FIELD(GPIOD_AFRL, AFRL0, 0, 4)
REG32(GPIOD_AFRH, 0x24)
	FIELD(GPIOD_AFRH, AFRH15, 28, 4)
	FIELD(GPIOD_AFRH, AFRH14, 24, 4)
	FIELD(GPIOD_AFRH, AFRH13, 20, 4)
	FIELD(GPIOD_AFRH, AFRH12, 16, 4)
	FIELD(GPIOD_AFRH, AFRH11, 12, 4)
	FIELD(GPIOD_AFRH, AFRH10, 8, 4)
	FIELD(GPIOD_AFRH, AFRH9, 4, 4)
	FIELD(GPIOD_AFRH, AFRH8, 0, 4)
REG32(GPIOD_BRR, 0x28)
	FIELD(GPIOD_BRR, BR0, 0, 1)
	FIELD(GPIOD_BRR, BR1, 1, 1)
	FIELD(GPIOD_BRR, BR2, 2, 1)
	FIELD(GPIOD_BRR, BR3, 3, 1)
	FIELD(GPIOD_BRR, BR4, 4, 1)
	FIELD(GPIOD_BRR, BR5, 5, 1)
	FIELD(GPIOD_BRR, BR6, 6, 1)
	FIELD(GPIOD_BRR, BR7, 7, 1)
	FIELD(GPIOD_BRR, BR8, 8, 1)
	FIELD(GPIOD_BRR, BR9, 9, 1)
	FIELD(GPIOD_BRR, BR10, 10, 1)
	FIELD(GPIOD_BRR, BR11, 11, 1)
	FIELD(GPIOD_BRR, BR12, 12, 1)
	FIELD(GPIOD_BRR, BR13, 13, 1)
	FIELD(GPIOD_BRR, BR14, 14, 1)
	FIELD(GPIOD_BRR, BR15, 15, 1)


static void stm32f7_gpiod_register_reset(STM32F7GPIOD *t) {
	t->MODER = 0x0;
	t->OTYPER = 0x0;
	t->GPIOB_OSPEEDR = 0x0;
	t->PUPDR = 0x0;
	t->IDR = 0x0;
	t->ODR = 0x0;
	t->BSRR = 0x0;
	t->LCKR = 0x0;
	t->AFRL = 0x0;
	t->AFRH = 0x0;
	t->BRR = 0x0;

}

static uint64_t stm32f7_gpiod_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F7GPIOD *t = STM32F7_GPIOD(opaque);
	uint64_t ret;

	switch (offset) {
		case A_GPIOD_MODER:
			ret = t->MODER;
			break;
		case A_GPIOD_OTYPER:
			ret = t->OTYPER;
			break;
		case A_GPIOD_GPIOB_OSPEEDR:
			ret = t->GPIOB_OSPEEDR;
			break;
		case A_GPIOD_PUPDR:
			ret = t->PUPDR;
			break;
		case A_GPIOD_IDR:
			ret = t->IDR;
			break;
		case A_GPIOD_ODR:
			ret = t->ODR;
			break;
		case A_GPIOD_LCKR:
			ret = t->LCKR;
			break;
		case A_GPIOD_AFRL:
			ret = t->AFRL;
			break;
		case A_GPIOD_AFRH:
			ret = t->AFRH;
			break;
		case A_GPIOD_BRR:
			ret = t->BRR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 GPIOD read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f7_gpiod_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F7GPIOD *t = STM32F7_GPIOD(opaque);


	switch (offset) {
		case A_GPIOD_MODER ... A_GPIOD_MODER + 3:
			value = value << ((offset - A_GPIOD_MODER) << 3);
			offset = A_GPIOD_MODER;
			break;
		case A_GPIOD_OTYPER ... A_GPIOD_OTYPER + 3:
			value = value << ((offset - A_GPIOD_OTYPER) << 3);
			offset = A_GPIOD_OTYPER;
			break;
		case A_GPIOD_GPIOB_OSPEEDR ... A_GPIOD_GPIOB_OSPEEDR + 3:
			value = value << ((offset - A_GPIOD_GPIOB_OSPEEDR) << 3);
			offset = A_GPIOD_GPIOB_OSPEEDR;
			break;
		case A_GPIOD_PUPDR ... A_GPIOD_PUPDR + 3:
			value = value << ((offset - A_GPIOD_PUPDR) << 3);
			offset = A_GPIOD_PUPDR;
			break;
		case A_GPIOD_ODR ... A_GPIOD_ODR + 3:
			value = value << ((offset - A_GPIOD_ODR) << 3);
			offset = A_GPIOD_ODR;
			break;
		case A_GPIOD_BSRR ... A_GPIOD_BSRR + 3:
			value = value << ((offset - A_GPIOD_BSRR) << 3);
			offset = A_GPIOD_BSRR;
			break;
		case A_GPIOD_LCKR ... A_GPIOD_LCKR + 3:
			value = value << ((offset - A_GPIOD_LCKR) << 3);
			offset = A_GPIOD_LCKR;
			break;
		case A_GPIOD_AFRL ... A_GPIOD_AFRL + 3:
			value = value << ((offset - A_GPIOD_AFRL) << 3);
			offset = A_GPIOD_AFRL;
			break;
		case A_GPIOD_AFRH ... A_GPIOD_AFRH + 3:
			value = value << ((offset - A_GPIOD_AFRH) << 3);
			offset = A_GPIOD_AFRH;
			break;
		case A_GPIOD_BRR ... A_GPIOD_BRR + 3:
			value = value << ((offset - A_GPIOD_BRR) << 3);
			offset = A_GPIOD_BRR;
			break;

		default: break;
	}

	switch (offset) {
		case A_GPIOD_MODER:
			t->MODER = value;
			break;
		case A_GPIOD_OTYPER:
			t->OTYPER = value;
			break;
		case A_GPIOD_GPIOB_OSPEEDR:
			t->GPIOB_OSPEEDR = value;
			break;
		case A_GPIOD_PUPDR:
			t->PUPDR = value;
			break;
		case A_GPIOD_ODR:
			t->ODR = value;
			break;
		case A_GPIOD_BSRR:
			t->BSRR = value;
			break;
		case A_GPIOD_LCKR:
			t->LCKR = value;
			break;
		case A_GPIOD_AFRL:
			t->AFRL = value;
			break;
		case A_GPIOD_AFRH:
			t->AFRH = value;
			break;
		case A_GPIOD_BRR:
			t->BRR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 GPIOD write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f7_gpiod_ops = {
	.read = stm32f7_gpiod_read,
	.write = stm32f7_gpiod_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f7_gpiod_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F7GPIOD *t = STM32F7_GPIOD(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f7_gpiod_ops, t, "stm32f7-gpiod", STM32F7_GPIOD_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void stm32f7_gpiod_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32f7_gpiod_reset_enter(Object *obj, ResetType type) {
	STM32F7GPIOD *t = STM32F7_GPIOD(obj);
	stm32f7_gpiod_register_reset(t);
}

static const VMStateDescription stm32f7_gpiod_vmstate = {
	.name = "stm32f7-gpiod",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(MODER, STM32F7GPIOD),
		VMSTATE_UINT32(OTYPER, STM32F7GPIOD),
		VMSTATE_UINT32(GPIOB_OSPEEDR, STM32F7GPIOD),
		VMSTATE_UINT32(PUPDR, STM32F7GPIOD),
		VMSTATE_UINT32(IDR, STM32F7GPIOD),
		VMSTATE_UINT32(ODR, STM32F7GPIOD),
		VMSTATE_UINT32(BSRR, STM32F7GPIOD),
		VMSTATE_UINT32(LCKR, STM32F7GPIOD),
		VMSTATE_UINT32(AFRL, STM32F7GPIOD),
		VMSTATE_UINT32(AFRH, STM32F7GPIOD),
		VMSTATE_UINT32(BRR, STM32F7GPIOD),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f7_gpiod_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f7_gpiod_vmstate;
	dc->realize = stm32f7_gpiod_realize;
	rc->phases.enter = stm32f7_gpiod_reset_enter;

}

static const TypeInfo stm32f7_gpiod_info = {
	.name = TYPE_STM32F7_GPIOD,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F7GPIOD),
	.instance_init = stm32f7_gpiod_init,
	.class_init = stm32f7_gpiod_class_init,
};

static void stm32f7_gpiod_register_types(void) {
	type_register_static(&stm32f7_gpiod_info);
}

type_init(stm32f7_gpiod_register_types);

#define TYPE_STM32F7_RNG "stm32f7-rng"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F7RNG, STM32F7_RNG)

struct STM32F7RNG {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t CR;
	uint32_t SR;
	uint32_t DR;
	/* base */
	uint32_t base;

};
#define STM32F7_RNG_SIZE				0xc

REG32(RNG_CR, 0x0)
	FIELD(RNG_CR, IE, 3, 1)
	FIELD(RNG_CR, RNGEN, 2, 1)
REG32(RNG_SR, 0x4)
	FIELD(RNG_SR, SEIS, 6, 1)
	FIELD(RNG_SR, CEIS, 5, 1)
	FIELD(RNG_SR, SECS, 2, 1)
	FIELD(RNG_SR, CECS, 1, 1)
	FIELD(RNG_SR, DRDY, 0, 1)
REG32(RNG_DR, 0x8)
	FIELD(RNG_DR, RNDATA, 0, 32)


__attribute__((unused))
static void stm32f7_rng_update(STM32F7RNG *t) {
	int conditions = ((t->SR & 0x20) || (t->SR & 0x40) || (t->SR & 0x1));
	qemu_set_irq(t->irq[0], conditions);

}

static void stm32f7_rng_register_reset(STM32F7RNG *t) {
	t->CR = 0x0;
	t->SR = 0x0;
	t->DR = 0x0;

}

static uint64_t stm32f7_rng_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F7RNG *t = STM32F7_RNG(opaque);
	uint64_t ret;

	switch (offset) {
		case A_RNG_CR:
			ret = t->CR;
			break;
		case A_RNG_SR:
			ret = t->SR;
			break;
		case A_RNG_DR:
			ret = t->DR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 RNG read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f7_rng_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F7RNG *t = STM32F7_RNG(opaque);


	switch (offset) {
		case A_RNG_CR ... A_RNG_CR + 3:
			value = value << ((offset - A_RNG_CR) << 3);
			offset = A_RNG_CR;
			break;
		case A_RNG_SR ... A_RNG_SR + 3:
			value = value << ((offset - A_RNG_SR) << 3);
			offset = A_RNG_SR;
			break;

		default: break;
	}

	switch (offset) {
		case A_RNG_CR:
			t->CR = value;
			break;
		case A_RNG_SR:
			t->SR = value;
			stm32f7_rng_update(t);
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 RNG write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f7_rng_ops = {
	.read = stm32f7_rng_read,
	.write = stm32f7_rng_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f7_rng_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F7RNG *t = STM32F7_RNG(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f7_rng_ops, t, "stm32f7-rng", STM32F7_RNG_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32f7_rng_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32f7_rng_reset_enter(Object *obj, ResetType type) {
	STM32F7RNG *t = STM32F7_RNG(obj);
	stm32f7_rng_register_reset(t);
}

static const VMStateDescription stm32f7_rng_vmstate = {
	.name = "stm32f7-rng",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR, STM32F7RNG),
		VMSTATE_UINT32(SR, STM32F7RNG),
		VMSTATE_UINT32(DR, STM32F7RNG),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f7_rng_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f7_rng_vmstate;
	dc->realize = stm32f7_rng_realize;
	rc->phases.enter = stm32f7_rng_reset_enter;

}

static const TypeInfo stm32f7_rng_info = {
	.name = TYPE_STM32F7_RNG,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F7RNG),
	.instance_init = stm32f7_rng_init,
	.class_init = stm32f7_rng_class_init,
};

static void stm32f7_rng_register_types(void) {
	type_register_static(&stm32f7_rng_info);
}

type_init(stm32f7_rng_register_types);

#define TYPE_STM32F7_SPI "stm32f7-spi"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F7SPI, STM32F7_SPI)

struct STM32F7SPI {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t CR1;
	uint32_t CR2;
	uint32_t SR;
	uint32_t DR;
	uint32_t CRCPR;
	uint32_t RXCRCR;
	uint32_t TXCRCR;
	uint32_t I2SCFGR;
	uint32_t I2SPR;
	/* chardev backend */
	CharBackend chr;
	guint watch_tag;

	/* base */
	uint32_t base;

};
#define STM32F7_SPI_SIZE				0x24

REG32(SPI_CR1, 0x0)
	FIELD(SPI_CR1, BIDIMODE, 15, 1)
	FIELD(SPI_CR1, BIDIOE, 14, 1)
	FIELD(SPI_CR1, CRCEN, 13, 1)
	FIELD(SPI_CR1, CRCNEXT, 12, 1)
	FIELD(SPI_CR1, CRCL, 11, 1)
	FIELD(SPI_CR1, RXONLY, 10, 1)
	FIELD(SPI_CR1, SSM, 9, 1)
	FIELD(SPI_CR1, SSI, 8, 1)
	FIELD(SPI_CR1, LSBFIRST, 7, 1)
	FIELD(SPI_CR1, SPE, 6, 1)
	FIELD(SPI_CR1, BR, 3, 3)
	FIELD(SPI_CR1, MSTR, 2, 1)
	FIELD(SPI_CR1, CPOL, 1, 1)
	FIELD(SPI_CR1, CPHA, 0, 1)
REG32(SPI_CR2, 0x4)
	FIELD(SPI_CR2, RXDMAEN, 0, 1)
	FIELD(SPI_CR2, TXDMAEN, 1, 1)
	FIELD(SPI_CR2, SSOE, 2, 1)
	FIELD(SPI_CR2, NSSP, 3, 1)
	FIELD(SPI_CR2, FRF, 4, 1)
	FIELD(SPI_CR2, ERRIE, 5, 1)
	FIELD(SPI_CR2, RXNEIE, 6, 1)
	FIELD(SPI_CR2, TXEIE, 7, 1)
	FIELD(SPI_CR2, DS, 8, 4)
	FIELD(SPI_CR2, FRXTH, 12, 1)
	FIELD(SPI_CR2, LDMA_RX, 13, 1)
	FIELD(SPI_CR2, LDMA_TX, 14, 1)
REG32(SPI_SR, 0x8)
	FIELD(SPI_SR, FRE, 8, 1)
	FIELD(SPI_SR, BSY, 7, 1)
	FIELD(SPI_SR, OVR, 6, 1)
	FIELD(SPI_SR, MODF, 5, 1)
	FIELD(SPI_SR, CRCERR, 4, 1)
	FIELD(SPI_SR, UDR, 3, 1)
	FIELD(SPI_SR, CHSIDE, 2, 1)
	FIELD(SPI_SR, TXE, 1, 1)
	FIELD(SPI_SR, RXNE, 0, 1)
	FIELD(SPI_SR, FRLVL, 9, 2)
	FIELD(SPI_SR, FTLVL, 11, 2)
REG32(SPI_DR, 0xc)
	FIELD(SPI_DR, DR, 0, 16)
REG32(SPI_CRCPR, 0x10)
	FIELD(SPI_CRCPR, CRCPOLY, 0, 16)
REG32(SPI_RXCRCR, 0x14)
	FIELD(SPI_RXCRCR, RxCRC, 0, 16)
REG32(SPI_TXCRCR, 0x18)
	FIELD(SPI_TXCRCR, TxCRC, 0, 16)
REG32(SPI_I2SCFGR, 0x1c)
	FIELD(SPI_I2SCFGR, I2SMOD, 11, 1)
	FIELD(SPI_I2SCFGR, I2SE, 10, 1)
	FIELD(SPI_I2SCFGR, I2SCFG, 8, 2)
	FIELD(SPI_I2SCFGR, PCMSYNC, 7, 1)
	FIELD(SPI_I2SCFGR, I2SSTD, 4, 2)
	FIELD(SPI_I2SCFGR, CKPOL, 3, 1)
	FIELD(SPI_I2SCFGR, DATLEN, 1, 2)
	FIELD(SPI_I2SCFGR, CHLEN, 0, 1)
	FIELD(SPI_I2SCFGR, ASTRTEN, 12, 1)
REG32(SPI_I2SPR, 0x20)
	FIELD(SPI_I2SPR, MCKOE, 9, 1)
	FIELD(SPI_I2SPR, ODD, 8, 1)
	FIELD(SPI_I2SPR, I2SDIV, 0, 8)


__attribute__((unused))
static void stm32f7_spi_update(STM32F7SPI *t) {
	int conditions = (((t->CR2 & 0x20) && (!(t->CR2 & 0x1)) && (!(t->CR2 & 0x2)) && (t->SR & 0x40)) || ((t->SR & 0x2) && (t->CR2 & 0x80)) || ((!(t->SR & 0x40)) && (t->CR2 & 0x40) && (t->SR & 0x1)) || ((t->SR & 0x100) && (t->CR2 & 0x20) && (!(t->CR2 & 0x1)) && (!(t->CR2 & 0x2))) || ((t->SR & 0x20) && (t->CR2 & 0x20) && (!(t->CR2 & 0x1)) && (!(t->CR2 & 0x2))));
	qemu_set_irq(t->irq[0], conditions);

}

static int stm32f7_spi_can_receive(void *opaque) {
	return 1;
}

static void stm32f7_spi_receive(void *opaque, const uint8_t *buf, int size) {
	STM32F7SPI *t = STM32F7_SPI(opaque);

	t->DR = *buf;
	t->SR |= 0x1;

	stm32f7_spi_update(t);
}

static gboolean stm32f7_spi_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	STM32F7SPI *t = STM32F7_SPI(opaque);
	int ret;

	t->watch_tag = 0;
	t->SR |= 0x200;
	t->SR |= 0x800;
	t->SR &= (~(0x2));
	t->SR |= 0x1000;
	t->SR |= 0x400;

	stm32f7_spi_update(t);

	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->DR), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, stm32f7_spi_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->SR &= (~(0x1000));
	t->SR &= (~(0x400));
	t->SR |= 0x2;
	t->SR &= (~(0x800));
	t->SR &= (~(0x200));

	stm32f7_spi_update(t);

	return FALSE;
}

static void stm32f7_spi_register_reset(STM32F7SPI *t) {
	t->CR1 = 0x0;
	t->CR2 = 0x700;
	t->SR = 0x2;
	t->DR = 0x0;
	t->CRCPR = 0x7;
	t->RXCRCR = 0x0;
	t->TXCRCR = 0x0;
	t->I2SCFGR = 0x0;
	t->I2SPR = 0xa;
	t->SR &= (~(0x1000));
	t->SR &= (~(0x400));
	t->SR |= 0x2;
	t->SR &= (~(0x800));
	t->SR &= (~(0x200));

}

static uint64_t stm32f7_spi_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F7SPI *t = STM32F7_SPI(opaque);
	uint64_t ret;

	switch (offset) {
		case A_SPI_CR1:
			ret = t->CR1;
			break;
		case A_SPI_CR2:
			ret = t->CR2;
			break;
		case A_SPI_SR:
			ret = t->SR;
			break;
		case A_SPI_DR:
			ret = t->DR;
			t->SR &= (~(0x1));
			qemu_chr_fe_accept_input(&(t->chr));
			stm32f7_spi_update(t);
			break;
		case A_SPI_CRCPR:
			ret = t->CRCPR;
			break;
		case A_SPI_RXCRCR:
			ret = t->RXCRCR;
			break;
		case A_SPI_TXCRCR:
			ret = t->TXCRCR;
			break;
		case A_SPI_I2SCFGR:
			ret = t->I2SCFGR;
			break;
		case A_SPI_I2SPR:
			ret = t->I2SPR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 SPI read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f7_spi_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F7SPI *t = STM32F7_SPI(opaque);


	switch (offset) {
		case A_SPI_CR1 ... A_SPI_CR1 + 3:
			value = value << ((offset - A_SPI_CR1) << 3);
			offset = A_SPI_CR1;
			break;
		case A_SPI_CR2 ... A_SPI_CR2 + 3:
			value = value << ((offset - A_SPI_CR2) << 3);
			offset = A_SPI_CR2;
			break;
		case A_SPI_SR ... A_SPI_SR + 3:
			value = value << ((offset - A_SPI_SR) << 3);
			offset = A_SPI_SR;
			break;
		case A_SPI_DR ... A_SPI_DR + 3:
			value = value << ((offset - A_SPI_DR) << 3);
			offset = A_SPI_DR;
			break;
		case A_SPI_CRCPR ... A_SPI_CRCPR + 3:
			value = value << ((offset - A_SPI_CRCPR) << 3);
			offset = A_SPI_CRCPR;
			break;
		case A_SPI_I2SCFGR ... A_SPI_I2SCFGR + 3:
			value = value << ((offset - A_SPI_I2SCFGR) << 3);
			offset = A_SPI_I2SCFGR;
			break;
		case A_SPI_I2SPR ... A_SPI_I2SPR + 3:
			value = value << ((offset - A_SPI_I2SPR) << 3);
			offset = A_SPI_I2SPR;
			break;

		default: break;
	}

	switch (offset) {
		case A_SPI_CR1:
			t->CR1 = value;
			if ((!(value & 0x40))) {
				t->SR &= (~(0x200));
				t->SR &= (~(0x80));
				t->SR &= (~(0x400));
			}
			t->SR &= (~(0x80));
			stm32f7_spi_update(t);
			break;
		case A_SPI_CR2:
			t->CR2 = value;
			if (((!(value & 0x20)) && (!(value & 0x80)))) {
				t->SR &= (~(0x1000));
				t->SR &= (~(0x800));
			} else if (((!(value & 0x20)) && (!(value & 0x40)))) {
				t->SR &= (~(0x80));
			} else if ((!(value & 0x80))) {
				t->SR &= (~(0x1000));
				t->CR2 &= (~(0x80));
				t->SR &= (~(0x800));
			}
			if ((!(value & 0x40))) {
				t->SR &= (~(0x80));
				t->CR2 &= (~(0x40));
			}
			if ((!(value & 0x20))) {
				t->SR &= (~(0x1000));
				t->SR &= (~(0x800));
			}
			if ((value & 0x1000)) {
				t->SR &= (~(0x1000));
				t->SR &= (~(0x800));
			}
			stm32f7_spi_update(t);
			break;
		case A_SPI_SR:
			t->SR &= value;
			t->SR &= (~(0x1000));
			t->SR &= (~(0x400));
			t->SR |= 0x2;
			t->SR &= (~(0x800));
			t->SR &= (~(0x200));
			stm32f7_spi_update(t);
			break;
		case A_SPI_DR:
			t->DR = value;
			stm32f7_spi_transmit(NULL, G_IO_OUT, t);
			break;
		case A_SPI_CRCPR:
			t->CRCPR = value;
			break;
		case A_SPI_I2SCFGR:
			t->I2SCFGR = value;
			break;
		case A_SPI_I2SPR:
			t->I2SPR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 SPI write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f7_spi_ops = {
	.read = stm32f7_spi_read,
	.write = stm32f7_spi_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f7_spi_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F7SPI *t = STM32F7_SPI(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f7_spi_ops, t, "stm32f7-spi", STM32F7_SPI_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32f7_spi_realize(DeviceState *dev, Error **errp) {
	STM32F7SPI *t = STM32F7_SPI(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), stm32f7_spi_can_receive, stm32f7_spi_receive,
		NULL, NULL, t, NULL, true);

}

static void stm32f7_spi_reset_enter(Object *obj, ResetType type) {
	STM32F7SPI *t = STM32F7_SPI(obj);
	stm32f7_spi_register_reset(t);
}

static const VMStateDescription stm32f7_spi_vmstate = {
	.name = "stm32f7-spi",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR1, STM32F7SPI),
		VMSTATE_UINT32(CR2, STM32F7SPI),
		VMSTATE_UINT32(SR, STM32F7SPI),
		VMSTATE_UINT32(DR, STM32F7SPI),
		VMSTATE_UINT32(CRCPR, STM32F7SPI),
		VMSTATE_UINT32(RXCRCR, STM32F7SPI),
		VMSTATE_UINT32(TXCRCR, STM32F7SPI),
		VMSTATE_UINT32(I2SCFGR, STM32F7SPI),
		VMSTATE_UINT32(I2SPR, STM32F7SPI),

		VMSTATE_END_OF_LIST()
	}
};

static Property stm32f7_spi_properties[] = {
	DEFINE_PROP_CHR("chardev", STM32F7SPI, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void stm32f7_spi_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f7_spi_vmstate;
	dc->realize = stm32f7_spi_realize;
	rc->phases.enter = stm32f7_spi_reset_enter;
	device_class_set_props(dc, stm32f7_spi_properties);

}

static const TypeInfo stm32f7_spi_info = {
	.name = TYPE_STM32F7_SPI,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F7SPI),
	.instance_init = stm32f7_spi_init,
	.class_init = stm32f7_spi_class_init,
};

static void stm32f7_spi_register_types(void) {
	type_register_static(&stm32f7_spi_info);
}

type_init(stm32f7_spi_register_types);

#define TYPE_STM32F7_I2C "stm32f7-i2c"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F7I2C, STM32F7_I2C)

struct STM32F7I2C {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[2];

	/*registers*/
	uint32_t CR1;
	uint32_t CR2;
	uint32_t OAR1;
	uint32_t OAR2;
	uint32_t TIMINGR;
	uint32_t TIMEOUTR;
	uint32_t ISR;
	uint32_t ICR;
	uint32_t PECR;
	uint32_t RXDR;
	uint32_t TXDR;
	/* chardev backend */
	CharBackend chr;
	guint watch_tag;

	/* base */
	uint32_t base;

};
#define STM32F7_I2C_SIZE				0x2c

REG32(I2C_CR1, 0x0)
	FIELD(I2C_CR1, PE, 0, 1)
	FIELD(I2C_CR1, TXIE, 1, 1)
	FIELD(I2C_CR1, RXIE, 2, 1)
	FIELD(I2C_CR1, ADDRIE, 3, 1)
	FIELD(I2C_CR1, NACKIE, 4, 1)
	FIELD(I2C_CR1, STOPIE, 5, 1)
	FIELD(I2C_CR1, TCIE, 6, 1)
	FIELD(I2C_CR1, ERRIE, 7, 1)
	FIELD(I2C_CR1, DNF, 8, 4)
	FIELD(I2C_CR1, ANFOFF, 12, 1)
	FIELD(I2C_CR1, TXDMAEN, 14, 1)
	FIELD(I2C_CR1, RXDMAEN, 15, 1)
	FIELD(I2C_CR1, SBC, 16, 1)
	FIELD(I2C_CR1, NOSTRETCH, 17, 1)
	FIELD(I2C_CR1, WUPEN, 18, 1)
	FIELD(I2C_CR1, GCEN, 19, 1)
	FIELD(I2C_CR1, SMBHEN, 20, 1)
	FIELD(I2C_CR1, SMBDEN, 21, 1)
	FIELD(I2C_CR1, ALERTEN, 22, 1)
	FIELD(I2C_CR1, PECEN, 23, 1)
REG32(I2C_CR2, 0x4)
	FIELD(I2C_CR2, PECBYTE, 26, 1)
	FIELD(I2C_CR2, AUTOEND, 25, 1)
	FIELD(I2C_CR2, RELOAD, 24, 1)
	FIELD(I2C_CR2, NBYTES, 16, 8)
	FIELD(I2C_CR2, NACK, 15, 1)
	FIELD(I2C_CR2, STOP, 14, 1)
	FIELD(I2C_CR2, START, 13, 1)
	FIELD(I2C_CR2, HEAD10R, 12, 1)
	FIELD(I2C_CR2, ADD10, 11, 1)
	FIELD(I2C_CR2, RD_WRN, 10, 1)
	FIELD(I2C_CR2, SADD, 0, 10)
REG32(I2C_OAR1, 0x8)
	FIELD(I2C_OAR1, OA1, 0, 10)
	FIELD(I2C_OAR1, OA1MODE, 10, 1)
	FIELD(I2C_OAR1, OA1EN, 15, 1)
REG32(I2C_OAR2, 0xc)
	FIELD(I2C_OAR2, OA2, 1, 7)
	FIELD(I2C_OAR2, OA2MSK, 8, 3)
	FIELD(I2C_OAR2, OA2EN, 15, 1)
REG32(I2C_TIMINGR, 0x10)
	FIELD(I2C_TIMINGR, SCLL, 0, 8)
	FIELD(I2C_TIMINGR, SCLH, 8, 8)
	FIELD(I2C_TIMINGR, SDADEL, 16, 4)
	FIELD(I2C_TIMINGR, SCLDEL, 20, 4)
	FIELD(I2C_TIMINGR, PRESC, 28, 4)
REG32(I2C_TIMEOUTR, 0x14)
	FIELD(I2C_TIMEOUTR, TIMEOUTA, 0, 12)
	FIELD(I2C_TIMEOUTR, TIDLE, 12, 1)
	FIELD(I2C_TIMEOUTR, TIMOUTEN, 15, 1)
	FIELD(I2C_TIMEOUTR, TIMEOUTB, 16, 12)
	FIELD(I2C_TIMEOUTR, TEXTEN, 31, 1)
REG32(I2C_ISR, 0x18)
	FIELD(I2C_ISR, ADDCODE, 17, 7)
	FIELD(I2C_ISR, DIR, 16, 1)
	FIELD(I2C_ISR, BUSY, 15, 1)
	FIELD(I2C_ISR, ALERT, 13, 1)
	FIELD(I2C_ISR, TIMEOUT, 12, 1)
	FIELD(I2C_ISR, PECERR, 11, 1)
	FIELD(I2C_ISR, OVR, 10, 1)
	FIELD(I2C_ISR, ARLO, 9, 1)
	FIELD(I2C_ISR, BERR, 8, 1)
	FIELD(I2C_ISR, TCR, 7, 1)
	FIELD(I2C_ISR, TC, 6, 1)
	FIELD(I2C_ISR, STOPF, 5, 1)
	FIELD(I2C_ISR, NACKF, 4, 1)
	FIELD(I2C_ISR, ADDR, 3, 1)
	FIELD(I2C_ISR, RXNE, 2, 1)
	FIELD(I2C_ISR, TXIS, 1, 1)
	FIELD(I2C_ISR, TXE, 0, 1)
REG32(I2C_ICR, 0x1c)
	FIELD(I2C_ICR, ALERTCF, 13, 1)
	FIELD(I2C_ICR, TIMOUTCF, 12, 1)
	FIELD(I2C_ICR, PECCF, 11, 1)
	FIELD(I2C_ICR, OVRCF, 10, 1)
	FIELD(I2C_ICR, ARLOCF, 9, 1)
	FIELD(I2C_ICR, BERRCF, 8, 1)
	FIELD(I2C_ICR, STOPCF, 5, 1)
	FIELD(I2C_ICR, NACKCF, 4, 1)
	FIELD(I2C_ICR, ADDRCF, 3, 1)
REG32(I2C_PECR, 0x20)
	FIELD(I2C_PECR, PEC, 0, 8)
REG32(I2C_RXDR, 0x24)
	FIELD(I2C_RXDR, RXDATA, 0, 8)
REG32(I2C_TXDR, 0x28)
	FIELD(I2C_TXDR, TXDATA, 0, 8)


static int stm32f7_i2c_can_receive(void *opaque) {
	return 1;
}

static void stm32f7_i2c_receive(void *opaque, const uint8_t *buf, int size) {
	STM32F7I2C *t = STM32F7_I2C(opaque);

	t->RXDR = *buf;
	t->ISR &= (~(0x8000));
	t->ISR |= 0x20;
	t->ISR &= (~(0x10));
	t->ISR &= (~(0x10000));
	t->ISR |= 0x4;
	t->ISR |= 0x8;
	t->ISR |= 0x10;
	t->ISR &= (~(0x20));


}

static gboolean stm32f7_i2c_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	STM32F7I2C *t = STM32F7_I2C(opaque);
	int ret;

	t->watch_tag = 0;
	t->ISR &= (~(0x400));
	t->ISR &= (~(0x80));
	t->ISR &= (~(0x10));
	t->ISR &= (~(0x2));
	t->ISR |= 0x20;
	t->ISR &= (~(0x8));
	t->ISR |= 0x4;
	t->ISR &= (~(0x10000));
	t->ISR &= (~(0x40));
	t->ISR |= 0x8000;
	t->ISR &= (~(0x200));
	t->ISR &= (~(0x100));
	t->ISR |= 0x10;
	t->ISR &= (~(0x20));



	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->TXDR), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, stm32f7_i2c_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->ISR |= 0x2;
	t->ISR |= 0x80;
	t->ISR &= (~(0x8000));
	t->ISR |= 0x20;
	t->ISR &= (~(0x10));
	t->ISR |= 0x8;
	t->ISR |= 0x10000;
	t->ISR |= 0x40;
	t->ISR &= (~(0x4));
	t->ISR &= (~(0x20));
	t->ISR |= 0x10;
	t->ISR |= 0x400;
	t->ISR |= 0x100;
	t->ISR |= 0x200;



	return FALSE;
}

static void stm32f7_i2c_register_reset(STM32F7I2C *t) {
	t->CR1 = 0x0;
	t->CR2 = 0x0;
	t->OAR1 = 0x0;
	t->OAR2 = 0x0;
	t->TIMINGR = 0x0;
	t->TIMEOUTR = 0x0;
	t->ISR = 0x1;
	t->ICR = 0x0;
	t->PECR = 0x0;
	t->RXDR = 0x0;
	t->TXDR = 0x0;
	t->ISR |= 0x2;
	t->ISR |= 0x80;
	t->ISR &= (~(0x8000));
	t->ISR |= 0x20;
	t->ISR &= (~(0x10));
	t->ISR |= 0x8;
	t->ISR |= 0x10000;
	t->ISR |= 0x40;
	t->ISR &= (~(0x4));
	t->ISR &= (~(0x20));
	t->ISR |= 0x10;
	t->ISR |= 0x400;
	t->ISR |= 0x100;
	t->ISR |= 0x200;

}

static uint64_t stm32f7_i2c_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F7I2C *t = STM32F7_I2C(opaque);
	uint64_t ret;

	switch (offset) {
		case A_I2C_CR1:
			ret = t->CR1;
			break;
		case A_I2C_CR2:
			ret = t->CR2;
			break;
		case A_I2C_OAR1:
			ret = t->OAR1;
			break;
		case A_I2C_OAR2:
			ret = t->OAR2;
			break;
		case A_I2C_TIMINGR:
			ret = t->TIMINGR;
			break;
		case A_I2C_TIMEOUTR:
			ret = t->TIMEOUTR;
			break;
		case A_I2C_ISR:
			ret = t->ISR;
			break;
		case A_I2C_PECR:
			ret = t->PECR;
			break;
		case A_I2C_RXDR:
			ret = t->RXDR;
			t->ISR &= (~(0x10));
			t->ISR |= 0x20;
			t->ISR &= (~(0x8));
			t->ISR |= 0x10000;
			t->ISR &= (~(0x4));
			t->ISR |= 0x8000;
			t->ISR |= 0x10;
			t->ISR &= (~(0x20));
			qemu_chr_fe_accept_input(&(t->chr));
			break;
		case A_I2C_TXDR:
			ret = t->TXDR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 I2C read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f7_i2c_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F7I2C *t = STM32F7_I2C(opaque);


	switch (offset) {
		case A_I2C_CR1 ... A_I2C_CR1 + 3:
			value = value << ((offset - A_I2C_CR1) << 3);
			offset = A_I2C_CR1;
			break;
		case A_I2C_CR2 ... A_I2C_CR2 + 3:
			value = value << ((offset - A_I2C_CR2) << 3);
			offset = A_I2C_CR2;
			break;
		case A_I2C_OAR1 ... A_I2C_OAR1 + 3:
			value = value << ((offset - A_I2C_OAR1) << 3);
			offset = A_I2C_OAR1;
			break;
		case A_I2C_OAR2 ... A_I2C_OAR2 + 3:
			value = value << ((offset - A_I2C_OAR2) << 3);
			offset = A_I2C_OAR2;
			break;
		case A_I2C_TIMINGR ... A_I2C_TIMINGR + 3:
			value = value << ((offset - A_I2C_TIMINGR) << 3);
			offset = A_I2C_TIMINGR;
			break;
		case A_I2C_TIMEOUTR ... A_I2C_TIMEOUTR + 3:
			value = value << ((offset - A_I2C_TIMEOUTR) << 3);
			offset = A_I2C_TIMEOUTR;
			break;
		case A_I2C_ISR ... A_I2C_ISR + 3:
			value = value << ((offset - A_I2C_ISR) << 3);
			offset = A_I2C_ISR;
			break;
		case A_I2C_ICR ... A_I2C_ICR + 3:
			value = value << ((offset - A_I2C_ICR) << 3);
			offset = A_I2C_ICR;
			break;
		case A_I2C_TXDR ... A_I2C_TXDR + 3:
			value = value << ((offset - A_I2C_TXDR) << 3);
			offset = A_I2C_TXDR;
			break;

		default: break;
	}

	switch (offset) {
		case A_I2C_CR1:
			t->CR1 = value;
			break;
		case A_I2C_CR2:
			t->CR2 = value;
			if (((value & 0x400) && (value & 0x2000) && (!(value & 0x4000)) && (!(value & 0x10000)) && (!(value & 0x20000)) && (!(value & 0x40000)) && (!(value & 0x80000)) && (!(value & 0x100000)) && (!(value & 0x200000)) && (!(value & 0x400000)) && (!(value & 0x800000)) && (!(value & 0x1000000)) && (value & 0x2000000))) {
				t->ISR |= 0x20;
			} else if (((!(value & 0x400)) && (value & 0x2000) && (!(value & 0x4000)) && (!(value & 0x10000)) && (!(value & 0x20000)) && (!(value & 0x40000)) && (!(value & 0x80000)) && (!(value & 0x100000)) && (!(value & 0x200000)) && (!(value & 0x400000)) && (!(value & 0x800000)) && (!(value & 0x1000000)) && (value & 0x2000000))) {
				t->ISR |= 0x20;
			} else if (((!(value & 0x400)) && (value & 0x2000) && (!(value & 0x4000)) && (!(value & 0x40000)) && (!(value & 0x80000)) && (!(value & 0x100000)) && (!(value & 0x200000)) && (!(value & 0x400000)) && (!(value & 0x800000)) && (value & 0x1000000) && (!(value & 0x2000000)))) {
				t->ISR |= 0x2;
			} else if (((!(value & 0x400)) && (value & 0x2000) && (!(value & 0x4000)) && (!(value & 0x40000)) && (!(value & 0x80000)) && (!(value & 0x100000)) && (!(value & 0x200000)) && (!(value & 0x400000)) && (!(value & 0x800000)) && (!(value & 0x1000000)) && (!(value & 0x2000000)))) {
				t->ISR |= 0x2;
			} else if (((!(value & 0x400)) && (value & 0x2000) && (!(value & 0x4000)) && (!(value & 0x1000000)) && (value & 0x2000000))) {
				t->ISR |= 0x2;
			} else if (((value & 0x400) && (value & 0x2000) && (!(value & 0x4000)) && (!(value & 0x1000000)) && (value & 0x2000000))) {
				t->ISR |= 0x4;
			} else if (((!(value & 0x2000)) && (!(value & 0x4000)) && (!(value & 0x1000000)) && (value & 0x2000000))) {
				t->ISR |= 0x2;
			}
			if ((!(value & 0x8000))) {
				t->ISR |= 0x8;
			}
			break;
		case A_I2C_OAR1:
			t->OAR1 = value;
			break;
		case A_I2C_OAR2:
			t->OAR2 = value;
			break;
		case A_I2C_TIMINGR:
			t->TIMINGR = value;
			break;
		case A_I2C_TIMEOUTR:
			t->TIMEOUTR = value;
			break;
		case A_I2C_ISR:
			t->ISR &= value;
			break;
		case A_I2C_ICR:
			t->ICR = value;
			if ((value == 0x10)) {
				t->ISR |= 0x20;
			}
			if ((value == 0x20)) {
				t->ISR &= (~(0x8000));
			}
			break;
		case A_I2C_TXDR:
			t->TXDR = value;
			stm32f7_i2c_transmit(NULL, G_IO_OUT, t);
			if (((!(value & 0x100)) && (!(value & 0x200)) && (!(value & 0x400)) && (!(value & 0x800)) && (!(value & 0x1000)) && (!(value & 0x2000)) && (!(value & 0x4000)) && (!(value & 0x8000)) && (!(value & 0x10000)) && (!(value & 0x20000)) && (!(value & 0x40000)) && (!(value & 0x80000)) && (!(value & 0x100000)) && (!(value & 0x200000)) && (!(value & 0x400000)) && (!(value & 0x800000)) && (!(value & 0x1000000)) && (!(value & 0x2000000)) && (!(value & 0x4000000)) && (!(value & 0x8000000)) && (!(value & 0x10000000)) && (!(value & 0x20000000)) && (!(value & 0x40000000)) && (!(value & 0x80000000)))) {
				t->ISR |= 0x2;
				t->ISR |= 0x80;
				t->ISR |= 0x20;
				t->ISR |= 0x40;
				t->ISR |= 0x10;
			}
			t->ISR |= 0x2;
			t->ISR |= 0x80;
			t->ISR |= 0x20;
			t->ISR |= 0x40;
			t->ISR |= 0x10;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 I2C write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f7_i2c_ops = {
	.read = stm32f7_i2c_read,
	.write = stm32f7_i2c_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f7_i2c_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F7I2C *t = STM32F7_I2C(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f7_i2c_ops, t, "stm32f7-i2c", STM32F7_I2C_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 2; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32f7_i2c_realize(DeviceState *dev, Error **errp) {
	STM32F7I2C *t = STM32F7_I2C(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), stm32f7_i2c_can_receive, stm32f7_i2c_receive,
		NULL, NULL, t, NULL, true);

}

static void stm32f7_i2c_reset_enter(Object *obj, ResetType type) {
	STM32F7I2C *t = STM32F7_I2C(obj);
	stm32f7_i2c_register_reset(t);
}

static const VMStateDescription stm32f7_i2c_vmstate = {
	.name = "stm32f7-i2c",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR1, STM32F7I2C),
		VMSTATE_UINT32(CR2, STM32F7I2C),
		VMSTATE_UINT32(OAR1, STM32F7I2C),
		VMSTATE_UINT32(OAR2, STM32F7I2C),
		VMSTATE_UINT32(TIMINGR, STM32F7I2C),
		VMSTATE_UINT32(TIMEOUTR, STM32F7I2C),
		VMSTATE_UINT32(ISR, STM32F7I2C),
		VMSTATE_UINT32(ICR, STM32F7I2C),
		VMSTATE_UINT32(PECR, STM32F7I2C),
		VMSTATE_UINT32(RXDR, STM32F7I2C),
		VMSTATE_UINT32(TXDR, STM32F7I2C),

		VMSTATE_END_OF_LIST()
	}
};

static Property stm32f7_i2c_properties[] = {
	DEFINE_PROP_CHR("chardev", STM32F7I2C, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void stm32f7_i2c_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f7_i2c_vmstate;
	dc->realize = stm32f7_i2c_realize;
	rc->phases.enter = stm32f7_i2c_reset_enter;
	device_class_set_props(dc, stm32f7_i2c_properties);

}

static const TypeInfo stm32f7_i2c_info = {
	.name = TYPE_STM32F7_I2C,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F7I2C),
	.instance_init = stm32f7_i2c_init,
	.class_init = stm32f7_i2c_class_init,
};

static void stm32f7_i2c_register_types(void) {
	type_register_static(&stm32f7_i2c_info);
}

type_init(stm32f7_i2c_register_types);

#define TYPE_STM32F7_DMA "stm32f7-dma"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F7DMA, STM32F7_DMA)

struct STM32F7DMA {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[8];

	/*registers*/
	uint32_t LISR;
	uint32_t HISR;
	uint32_t LIFCR;
	uint32_t HIFCR;
	uint32_t S0CR;
	uint32_t S0NDTR;
	uint32_t S0PAR;
	uint32_t S0M0AR;
	uint32_t S0M1AR;
	uint32_t S0FCR;
	uint32_t S1CR;
	uint32_t S1NDTR;
	uint32_t S1PAR;
	uint32_t S1M0AR;
	uint32_t S1M1AR;
	uint32_t S1FCR;
	uint32_t S2CR;
	uint32_t S2NDTR;
	uint32_t S2PAR;
	uint32_t S2M0AR;
	uint32_t S2M1AR;
	uint32_t S2FCR;
	uint32_t S3CR;
	uint32_t S3NDTR;
	uint32_t S3PAR;
	uint32_t S3M0AR;
	uint32_t S3M1AR;
	uint32_t S3FCR;
	uint32_t S4CR;
	uint32_t S4NDTR;
	uint32_t S4PAR;
	uint32_t S4M0AR;
	uint32_t S4M1AR;
	uint32_t S4FCR;
	uint32_t S5CR;
	uint32_t S5NDTR;
	uint32_t S5PAR;
	uint32_t S5M0AR;
	uint32_t S5M1AR;
	uint32_t S5FCR;
	uint32_t S6CR;
	uint32_t S6NDTR;
	uint32_t S6PAR;
	uint32_t S6M0AR;
	uint32_t S6M1AR;
	uint32_t S6FCR;
	uint32_t S7CR;
	uint32_t S7NDTR;
	uint32_t S7PAR;
	uint32_t S7M0AR;
	uint32_t S7M1AR;
	uint32_t S7FCR;
	/* base */
	uint32_t base;

};
#define STM32F7_DMA_SIZE				0xd0

REG32(DMA_LISR, 0x0)
	FIELD(DMA_LISR, TCIF3, 27, 1)
	FIELD(DMA_LISR, HTIF3, 26, 1)
	FIELD(DMA_LISR, TEIF3, 25, 1)
	FIELD(DMA_LISR, DMEIF3, 24, 1)
	FIELD(DMA_LISR, FEIF3, 22, 1)
	FIELD(DMA_LISR, TCIF2, 21, 1)
	FIELD(DMA_LISR, HTIF2, 20, 1)
	FIELD(DMA_LISR, TEIF2, 19, 1)
	FIELD(DMA_LISR, DMEIF2, 18, 1)
	FIELD(DMA_LISR, FEIF2, 16, 1)
	FIELD(DMA_LISR, TCIF1, 11, 1)
	FIELD(DMA_LISR, HTIF1, 10, 1)
	FIELD(DMA_LISR, TEIF1, 9, 1)
	FIELD(DMA_LISR, DMEIF1, 8, 1)
	FIELD(DMA_LISR, FEIF1, 6, 1)
	FIELD(DMA_LISR, TCIF0, 5, 1)
	FIELD(DMA_LISR, HTIF0, 4, 1)
	FIELD(DMA_LISR, TEIF0, 3, 1)
	FIELD(DMA_LISR, DMEIF0, 2, 1)
	FIELD(DMA_LISR, FEIF0, 0, 1)
REG32(DMA_HISR, 0x4)
	FIELD(DMA_HISR, TCIF7, 27, 1)
	FIELD(DMA_HISR, HTIF7, 26, 1)
	FIELD(DMA_HISR, TEIF7, 25, 1)
	FIELD(DMA_HISR, DMEIF7, 24, 1)
	FIELD(DMA_HISR, FEIF7, 22, 1)
	FIELD(DMA_HISR, TCIF6, 21, 1)
	FIELD(DMA_HISR, HTIF6, 20, 1)
	FIELD(DMA_HISR, TEIF6, 19, 1)
	FIELD(DMA_HISR, DMEIF6, 18, 1)
	FIELD(DMA_HISR, FEIF6, 16, 1)
	FIELD(DMA_HISR, TCIF5, 11, 1)
	FIELD(DMA_HISR, HTIF5, 10, 1)
	FIELD(DMA_HISR, TEIF5, 9, 1)
	FIELD(DMA_HISR, DMEIF5, 8, 1)
	FIELD(DMA_HISR, FEIF5, 6, 1)
	FIELD(DMA_HISR, TCIF4, 5, 1)
	FIELD(DMA_HISR, HTIF4, 4, 1)
	FIELD(DMA_HISR, TEIF4, 3, 1)
	FIELD(DMA_HISR, DMEIF4, 2, 1)
	FIELD(DMA_HISR, FEIF4, 0, 1)
REG32(DMA_LIFCR, 0x8)
	FIELD(DMA_LIFCR, CTCIF3, 27, 1)
	FIELD(DMA_LIFCR, CHTIF3, 26, 1)
	FIELD(DMA_LIFCR, CTEIF3, 25, 1)
	FIELD(DMA_LIFCR, CDMEIF3, 24, 1)
	FIELD(DMA_LIFCR, CFEIF3, 22, 1)
	FIELD(DMA_LIFCR, CTCIF2, 21, 1)
	FIELD(DMA_LIFCR, CHTIF2, 20, 1)
	FIELD(DMA_LIFCR, CTEIF2, 19, 1)
	FIELD(DMA_LIFCR, CDMEIF2, 18, 1)
	FIELD(DMA_LIFCR, CFEIF2, 16, 1)
	FIELD(DMA_LIFCR, CTCIF1, 11, 1)
	FIELD(DMA_LIFCR, CHTIF1, 10, 1)
	FIELD(DMA_LIFCR, CTEIF1, 9, 1)
	FIELD(DMA_LIFCR, CDMEIF1, 8, 1)
	FIELD(DMA_LIFCR, CFEIF1, 6, 1)
	FIELD(DMA_LIFCR, CTCIF0, 5, 1)
	FIELD(DMA_LIFCR, CHTIF0, 4, 1)
	FIELD(DMA_LIFCR, CTEIF0, 3, 1)
	FIELD(DMA_LIFCR, CDMEIF0, 2, 1)
	FIELD(DMA_LIFCR, CFEIF0, 0, 1)
REG32(DMA_HIFCR, 0xc)
	FIELD(DMA_HIFCR, CTCIF7, 27, 1)
	FIELD(DMA_HIFCR, CHTIF7, 26, 1)
	FIELD(DMA_HIFCR, CTEIF7, 25, 1)
	FIELD(DMA_HIFCR, CDMEIF7, 24, 1)
	FIELD(DMA_HIFCR, CFEIF7, 22, 1)
	FIELD(DMA_HIFCR, CTCIF6, 21, 1)
	FIELD(DMA_HIFCR, CHTIF6, 20, 1)
	FIELD(DMA_HIFCR, CTEIF6, 19, 1)
	FIELD(DMA_HIFCR, CDMEIF6, 18, 1)
	FIELD(DMA_HIFCR, CFEIF6, 16, 1)
	FIELD(DMA_HIFCR, CTCIF5, 11, 1)
	FIELD(DMA_HIFCR, CHTIF5, 10, 1)
	FIELD(DMA_HIFCR, CTEIF5, 9, 1)
	FIELD(DMA_HIFCR, CDMEIF5, 8, 1)
	FIELD(DMA_HIFCR, CFEIF5, 6, 1)
	FIELD(DMA_HIFCR, CTCIF4, 5, 1)
	FIELD(DMA_HIFCR, CHTIF4, 4, 1)
	FIELD(DMA_HIFCR, CTEIF4, 3, 1)
	FIELD(DMA_HIFCR, CDMEIF4, 2, 1)
	FIELD(DMA_HIFCR, CFEIF4, 0, 1)
REG32(DMA_S0CR, 0x10)
	FIELD(DMA_S0CR, CHSEL, 25, 4)
	FIELD(DMA_S0CR, MBURST, 23, 2)
	FIELD(DMA_S0CR, PBURST, 21, 2)
	FIELD(DMA_S0CR, CT, 19, 1)
	FIELD(DMA_S0CR, DBM, 18, 1)
	FIELD(DMA_S0CR, PL, 16, 2)
	FIELD(DMA_S0CR, PINCOS, 15, 1)
	FIELD(DMA_S0CR, MSIZE, 13, 2)
	FIELD(DMA_S0CR, PSIZE, 11, 2)
	FIELD(DMA_S0CR, MINC, 10, 1)
	FIELD(DMA_S0CR, PINC, 9, 1)
	FIELD(DMA_S0CR, CIRC, 8, 1)
	FIELD(DMA_S0CR, DIR, 6, 2)
	FIELD(DMA_S0CR, PFCTRL, 5, 1)
	FIELD(DMA_S0CR, TCIE, 4, 1)
	FIELD(DMA_S0CR, HTIE, 3, 1)
	FIELD(DMA_S0CR, TEIE, 2, 1)
	FIELD(DMA_S0CR, DMEIE, 1, 1)
	FIELD(DMA_S0CR, EN, 0, 1)
REG32(DMA_S0NDTR, 0x14)
	FIELD(DMA_S0NDTR, NDT, 0, 16)
REG32(DMA_S0PAR, 0x18)
	FIELD(DMA_S0PAR, PA, 0, 32)
REG32(DMA_S0M0AR, 0x1c)
	FIELD(DMA_S0M0AR, M0A, 0, 32)
REG32(DMA_S0M1AR, 0x20)
	FIELD(DMA_S0M1AR, M1A, 0, 32)
REG32(DMA_S0FCR, 0x24)
	FIELD(DMA_S0FCR, FEIE, 7, 1)
	FIELD(DMA_S0FCR, FS, 3, 3)
	FIELD(DMA_S0FCR, DMDIS, 2, 1)
	FIELD(DMA_S0FCR, FTH, 0, 2)
REG32(DMA_S1CR, 0x28)
	FIELD(DMA_S1CR, CHSEL, 25, 4)
	FIELD(DMA_S1CR, MBURST, 23, 2)
	FIELD(DMA_S1CR, PBURST, 21, 2)
	FIELD(DMA_S1CR, ACK, 20, 1)
	FIELD(DMA_S1CR, CT, 19, 1)
	FIELD(DMA_S1CR, DBM, 18, 1)
	FIELD(DMA_S1CR, PL, 16, 2)
	FIELD(DMA_S1CR, PINCOS, 15, 1)
	FIELD(DMA_S1CR, MSIZE, 13, 2)
	FIELD(DMA_S1CR, PSIZE, 11, 2)
	FIELD(DMA_S1CR, MINC, 10, 1)
	FIELD(DMA_S1CR, PINC, 9, 1)
	FIELD(DMA_S1CR, CIRC, 8, 1)
	FIELD(DMA_S1CR, DIR, 6, 2)
	FIELD(DMA_S1CR, PFCTRL, 5, 1)
	FIELD(DMA_S1CR, TCIE, 4, 1)
	FIELD(DMA_S1CR, HTIE, 3, 1)
	FIELD(DMA_S1CR, TEIE, 2, 1)
	FIELD(DMA_S1CR, DMEIE, 1, 1)
	FIELD(DMA_S1CR, EN, 0, 1)
REG32(DMA_S1NDTR, 0x2c)
	FIELD(DMA_S1NDTR, NDT, 0, 16)
REG32(DMA_S1PAR, 0x30)
	FIELD(DMA_S1PAR, PA, 0, 32)
REG32(DMA_S1M0AR, 0x34)
	FIELD(DMA_S1M0AR, M0A, 0, 32)
REG32(DMA_S1M1AR, 0x38)
	FIELD(DMA_S1M1AR, M1A, 0, 32)
REG32(DMA_S1FCR, 0x3c)
	FIELD(DMA_S1FCR, FEIE, 7, 1)
	FIELD(DMA_S1FCR, FS, 3, 3)
	FIELD(DMA_S1FCR, DMDIS, 2, 1)
	FIELD(DMA_S1FCR, FTH, 0, 2)
REG32(DMA_S2CR, 0x40)
	FIELD(DMA_S2CR, CHSEL, 25, 4)
	FIELD(DMA_S2CR, MBURST, 23, 2)
	FIELD(DMA_S2CR, PBURST, 21, 2)
	FIELD(DMA_S2CR, ACK, 20, 1)
	FIELD(DMA_S2CR, CT, 19, 1)
	FIELD(DMA_S2CR, DBM, 18, 1)
	FIELD(DMA_S2CR, PL, 16, 2)
	FIELD(DMA_S2CR, PINCOS, 15, 1)
	FIELD(DMA_S2CR, MSIZE, 13, 2)
	FIELD(DMA_S2CR, PSIZE, 11, 2)
	FIELD(DMA_S2CR, MINC, 10, 1)
	FIELD(DMA_S2CR, PINC, 9, 1)
	FIELD(DMA_S2CR, CIRC, 8, 1)
	FIELD(DMA_S2CR, DIR, 6, 2)
	FIELD(DMA_S2CR, PFCTRL, 5, 1)
	FIELD(DMA_S2CR, TCIE, 4, 1)
	FIELD(DMA_S2CR, HTIE, 3, 1)
	FIELD(DMA_S2CR, TEIE, 2, 1)
	FIELD(DMA_S2CR, DMEIE, 1, 1)
	FIELD(DMA_S2CR, EN, 0, 1)
REG32(DMA_S2NDTR, 0x44)
	FIELD(DMA_S2NDTR, NDT, 0, 16)
REG32(DMA_S2PAR, 0x48)
	FIELD(DMA_S2PAR, PA, 0, 32)
REG32(DMA_S2M0AR, 0x4c)
	FIELD(DMA_S2M0AR, M0A, 0, 32)
REG32(DMA_S2M1AR, 0x50)
	FIELD(DMA_S2M1AR, M1A, 0, 32)
REG32(DMA_S2FCR, 0x54)
	FIELD(DMA_S2FCR, FEIE, 7, 1)
	FIELD(DMA_S2FCR, FS, 3, 3)
	FIELD(DMA_S2FCR, DMDIS, 2, 1)
	FIELD(DMA_S2FCR, FTH, 0, 2)
REG32(DMA_S3CR, 0x58)
	FIELD(DMA_S3CR, CHSEL, 25, 4)
	FIELD(DMA_S3CR, MBURST, 23, 2)
	FIELD(DMA_S3CR, PBURST, 21, 2)
	FIELD(DMA_S3CR, ACK, 20, 1)
	FIELD(DMA_S3CR, CT, 19, 1)
	FIELD(DMA_S3CR, DBM, 18, 1)
	FIELD(DMA_S3CR, PL, 16, 2)
	FIELD(DMA_S3CR, PINCOS, 15, 1)
	FIELD(DMA_S3CR, MSIZE, 13, 2)
	FIELD(DMA_S3CR, PSIZE, 11, 2)
	FIELD(DMA_S3CR, MINC, 10, 1)
	FIELD(DMA_S3CR, PINC, 9, 1)
	FIELD(DMA_S3CR, CIRC, 8, 1)
	FIELD(DMA_S3CR, DIR, 6, 2)
	FIELD(DMA_S3CR, PFCTRL, 5, 1)
	FIELD(DMA_S3CR, TCIE, 4, 1)
	FIELD(DMA_S3CR, HTIE, 3, 1)
	FIELD(DMA_S3CR, TEIE, 2, 1)
	FIELD(DMA_S3CR, DMEIE, 1, 1)
	FIELD(DMA_S3CR, EN, 0, 1)
REG32(DMA_S3NDTR, 0x5c)
	FIELD(DMA_S3NDTR, NDT, 0, 16)
REG32(DMA_S3PAR, 0x60)
	FIELD(DMA_S3PAR, PA, 0, 32)
REG32(DMA_S3M0AR, 0x64)
	FIELD(DMA_S3M0AR, M0A, 0, 32)
REG32(DMA_S3M1AR, 0x68)
	FIELD(DMA_S3M1AR, M1A, 0, 32)
REG32(DMA_S3FCR, 0x6c)
	FIELD(DMA_S3FCR, FEIE, 7, 1)
	FIELD(DMA_S3FCR, FS, 3, 3)
	FIELD(DMA_S3FCR, DMDIS, 2, 1)
	FIELD(DMA_S3FCR, FTH, 0, 2)
REG32(DMA_S4CR, 0x70)
	FIELD(DMA_S4CR, CHSEL, 25, 4)
	FIELD(DMA_S4CR, MBURST, 23, 2)
	FIELD(DMA_S4CR, PBURST, 21, 2)
	FIELD(DMA_S4CR, ACK, 20, 1)
	FIELD(DMA_S4CR, CT, 19, 1)
	FIELD(DMA_S4CR, DBM, 18, 1)
	FIELD(DMA_S4CR, PL, 16, 2)
	FIELD(DMA_S4CR, PINCOS, 15, 1)
	FIELD(DMA_S4CR, MSIZE, 13, 2)
	FIELD(DMA_S4CR, PSIZE, 11, 2)
	FIELD(DMA_S4CR, MINC, 10, 1)
	FIELD(DMA_S4CR, PINC, 9, 1)
	FIELD(DMA_S4CR, CIRC, 8, 1)
	FIELD(DMA_S4CR, DIR, 6, 2)
	FIELD(DMA_S4CR, PFCTRL, 5, 1)
	FIELD(DMA_S4CR, TCIE, 4, 1)
	FIELD(DMA_S4CR, HTIE, 3, 1)
	FIELD(DMA_S4CR, TEIE, 2, 1)
	FIELD(DMA_S4CR, DMEIE, 1, 1)
	FIELD(DMA_S4CR, EN, 0, 1)
REG32(DMA_S4NDTR, 0x74)
	FIELD(DMA_S4NDTR, NDT, 0, 16)
REG32(DMA_S4PAR, 0x78)
	FIELD(DMA_S4PAR, PA, 0, 32)
REG32(DMA_S4M0AR, 0x7c)
	FIELD(DMA_S4M0AR, M0A, 0, 32)
REG32(DMA_S4M1AR, 0x80)
	FIELD(DMA_S4M1AR, M1A, 0, 32)
REG32(DMA_S4FCR, 0x84)
	FIELD(DMA_S4FCR, FEIE, 7, 1)
	FIELD(DMA_S4FCR, FS, 3, 3)
	FIELD(DMA_S4FCR, DMDIS, 2, 1)
	FIELD(DMA_S4FCR, FTH, 0, 2)
REG32(DMA_S5CR, 0x88)
	FIELD(DMA_S5CR, CHSEL, 25, 4)
	FIELD(DMA_S5CR, MBURST, 23, 2)
	FIELD(DMA_S5CR, PBURST, 21, 2)
	FIELD(DMA_S5CR, ACK, 20, 1)
	FIELD(DMA_S5CR, CT, 19, 1)
	FIELD(DMA_S5CR, DBM, 18, 1)
	FIELD(DMA_S5CR, PL, 16, 2)
	FIELD(DMA_S5CR, PINCOS, 15, 1)
	FIELD(DMA_S5CR, MSIZE, 13, 2)
	FIELD(DMA_S5CR, PSIZE, 11, 2)
	FIELD(DMA_S5CR, MINC, 10, 1)
	FIELD(DMA_S5CR, PINC, 9, 1)
	FIELD(DMA_S5CR, CIRC, 8, 1)
	FIELD(DMA_S5CR, DIR, 6, 2)
	FIELD(DMA_S5CR, PFCTRL, 5, 1)
	FIELD(DMA_S5CR, TCIE, 4, 1)
	FIELD(DMA_S5CR, HTIE, 3, 1)
	FIELD(DMA_S5CR, TEIE, 2, 1)
	FIELD(DMA_S5CR, DMEIE, 1, 1)
	FIELD(DMA_S5CR, EN, 0, 1)
REG32(DMA_S5NDTR, 0x8c)
	FIELD(DMA_S5NDTR, NDT, 0, 16)
REG32(DMA_S5PAR, 0x90)
	FIELD(DMA_S5PAR, PA, 0, 32)
REG32(DMA_S5M0AR, 0x94)
	FIELD(DMA_S5M0AR, M0A, 0, 32)
REG32(DMA_S5M1AR, 0x98)
	FIELD(DMA_S5M1AR, M1A, 0, 32)
REG32(DMA_S5FCR, 0x9c)
	FIELD(DMA_S5FCR, FEIE, 7, 1)
	FIELD(DMA_S5FCR, FS, 3, 3)
	FIELD(DMA_S5FCR, DMDIS, 2, 1)
	FIELD(DMA_S5FCR, FTH, 0, 2)
REG32(DMA_S6CR, 0xa0)
	FIELD(DMA_S6CR, CHSEL, 25, 4)
	FIELD(DMA_S6CR, MBURST, 23, 2)
	FIELD(DMA_S6CR, PBURST, 21, 2)
	FIELD(DMA_S6CR, ACK, 20, 1)
	FIELD(DMA_S6CR, CT, 19, 1)
	FIELD(DMA_S6CR, DBM, 18, 1)
	FIELD(DMA_S6CR, PL, 16, 2)
	FIELD(DMA_S6CR, PINCOS, 15, 1)
	FIELD(DMA_S6CR, MSIZE, 13, 2)
	FIELD(DMA_S6CR, PSIZE, 11, 2)
	FIELD(DMA_S6CR, MINC, 10, 1)
	FIELD(DMA_S6CR, PINC, 9, 1)
	FIELD(DMA_S6CR, CIRC, 8, 1)
	FIELD(DMA_S6CR, DIR, 6, 2)
	FIELD(DMA_S6CR, PFCTRL, 5, 1)
	FIELD(DMA_S6CR, TCIE, 4, 1)
	FIELD(DMA_S6CR, HTIE, 3, 1)
	FIELD(DMA_S6CR, TEIE, 2, 1)
	FIELD(DMA_S6CR, DMEIE, 1, 1)
	FIELD(DMA_S6CR, EN, 0, 1)
REG32(DMA_S6NDTR, 0xa4)
	FIELD(DMA_S6NDTR, NDT, 0, 16)
REG32(DMA_S6PAR, 0xa8)
	FIELD(DMA_S6PAR, PA, 0, 32)
REG32(DMA_S6M0AR, 0xac)
	FIELD(DMA_S6M0AR, M0A, 0, 32)
REG32(DMA_S6M1AR, 0xb0)
	FIELD(DMA_S6M1AR, M1A, 0, 32)
REG32(DMA_S6FCR, 0xb4)
	FIELD(DMA_S6FCR, FEIE, 7, 1)
	FIELD(DMA_S6FCR, FS, 3, 3)
	FIELD(DMA_S6FCR, DMDIS, 2, 1)
	FIELD(DMA_S6FCR, FTH, 0, 2)
REG32(DMA_S7CR, 0xb8)
	FIELD(DMA_S7CR, CHSEL, 25, 4)
	FIELD(DMA_S7CR, MBURST, 23, 2)
	FIELD(DMA_S7CR, PBURST, 21, 2)
	FIELD(DMA_S7CR, ACK, 20, 1)
	FIELD(DMA_S7CR, CT, 19, 1)
	FIELD(DMA_S7CR, DBM, 18, 1)
	FIELD(DMA_S7CR, PL, 16, 2)
	FIELD(DMA_S7CR, PINCOS, 15, 1)
	FIELD(DMA_S7CR, MSIZE, 13, 2)
	FIELD(DMA_S7CR, PSIZE, 11, 2)
	FIELD(DMA_S7CR, MINC, 10, 1)
	FIELD(DMA_S7CR, PINC, 9, 1)
	FIELD(DMA_S7CR, CIRC, 8, 1)
	FIELD(DMA_S7CR, DIR, 6, 2)
	FIELD(DMA_S7CR, PFCTRL, 5, 1)
	FIELD(DMA_S7CR, TCIE, 4, 1)
	FIELD(DMA_S7CR, HTIE, 3, 1)
	FIELD(DMA_S7CR, TEIE, 2, 1)
	FIELD(DMA_S7CR, DMEIE, 1, 1)
	FIELD(DMA_S7CR, EN, 0, 1)
REG32(DMA_S7NDTR, 0xbc)
	FIELD(DMA_S7NDTR, NDT, 0, 16)
REG32(DMA_S7PAR, 0xc0)
	FIELD(DMA_S7PAR, PA, 0, 32)
REG32(DMA_S7M0AR, 0xc4)
	FIELD(DMA_S7M0AR, M0A, 0, 32)
REG32(DMA_S7M1AR, 0xc8)
	FIELD(DMA_S7M1AR, M1A, 0, 32)
REG32(DMA_S7FCR, 0xcc)
	FIELD(DMA_S7FCR, FEIE, 7, 1)
	FIELD(DMA_S7FCR, FS, 3, 3)
	FIELD(DMA_S7FCR, DMDIS, 2, 1)
	FIELD(DMA_S7FCR, FTH, 0, 2)


static void stm32f7_dma_register_reset(STM32F7DMA *t) {
	t->LISR = 0x0;
	t->HISR = 0x0;
	t->LIFCR = 0x0;
	t->HIFCR = 0x0;
	t->S0CR = 0x0;
	t->S0NDTR = 0x0;
	t->S0PAR = 0x0;
	t->S0M0AR = 0x0;
	t->S0M1AR = 0x0;
	t->S0FCR = 0x21;
	t->S1CR = 0x0;
	t->S1NDTR = 0x0;
	t->S1PAR = 0x0;
	t->S1M0AR = 0x0;
	t->S1M1AR = 0x0;
	t->S1FCR = 0x21;
	t->S2CR = 0x0;
	t->S2NDTR = 0x0;
	t->S2PAR = 0x0;
	t->S2M0AR = 0x0;
	t->S2M1AR = 0x0;
	t->S2FCR = 0x21;
	t->S3CR = 0x0;
	t->S3NDTR = 0x0;
	t->S3PAR = 0x0;
	t->S3M0AR = 0x0;
	t->S3M1AR = 0x0;
	t->S3FCR = 0x21;
	t->S4CR = 0x0;
	t->S4NDTR = 0x0;
	t->S4PAR = 0x0;
	t->S4M0AR = 0x0;
	t->S4M1AR = 0x0;
	t->S4FCR = 0x21;
	t->S5CR = 0x0;
	t->S5NDTR = 0x0;
	t->S5PAR = 0x0;
	t->S5M0AR = 0x0;
	t->S5M1AR = 0x0;
	t->S5FCR = 0x21;
	t->S6CR = 0x0;
	t->S6NDTR = 0x0;
	t->S6PAR = 0x0;
	t->S6M0AR = 0x0;
	t->S6M1AR = 0x0;
	t->S6FCR = 0x21;
	t->S7CR = 0x0;
	t->S7NDTR = 0x0;
	t->S7PAR = 0x0;
	t->S7M0AR = 0x0;
	t->S7M1AR = 0x0;
	t->S7FCR = 0x21;

}

static uint64_t stm32f7_dma_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F7DMA *t = STM32F7_DMA(opaque);
	uint64_t ret;

	switch (offset) {
		case A_DMA_LISR:
			ret = t->LISR;
			break;
		case A_DMA_HISR:
			ret = t->HISR;
			break;
		case A_DMA_LIFCR:
			ret = t->LIFCR;
			break;
		case A_DMA_HIFCR:
			ret = t->HIFCR;
			break;
		case A_DMA_S0CR:
			ret = t->S0CR;
			break;
		case A_DMA_S0NDTR:
			ret = t->S0NDTR;
			break;
		case A_DMA_S0PAR:
			ret = t->S0PAR;
			break;
		case A_DMA_S0M0AR:
			ret = t->S0M0AR;
			break;
		case A_DMA_S0M1AR:
			ret = t->S0M1AR;
			break;
		case A_DMA_S0FCR:
			ret = t->S0FCR;
			break;
		case A_DMA_S1CR:
			ret = t->S1CR;
			break;
		case A_DMA_S1NDTR:
			ret = t->S1NDTR;
			break;
		case A_DMA_S1PAR:
			ret = t->S1PAR;
			break;
		case A_DMA_S1M0AR:
			ret = t->S1M0AR;
			break;
		case A_DMA_S1M1AR:
			ret = t->S1M1AR;
			break;
		case A_DMA_S1FCR:
			ret = t->S1FCR;
			break;
		case A_DMA_S2CR:
			ret = t->S2CR;
			break;
		case A_DMA_S2NDTR:
			ret = t->S2NDTR;
			break;
		case A_DMA_S2PAR:
			ret = t->S2PAR;
			break;
		case A_DMA_S2M0AR:
			ret = t->S2M0AR;
			break;
		case A_DMA_S2M1AR:
			ret = t->S2M1AR;
			break;
		case A_DMA_S2FCR:
			ret = t->S2FCR;
			break;
		case A_DMA_S3CR:
			ret = t->S3CR;
			break;
		case A_DMA_S3NDTR:
			ret = t->S3NDTR;
			break;
		case A_DMA_S3PAR:
			ret = t->S3PAR;
			break;
		case A_DMA_S3M0AR:
			ret = t->S3M0AR;
			break;
		case A_DMA_S3M1AR:
			ret = t->S3M1AR;
			break;
		case A_DMA_S3FCR:
			ret = t->S3FCR;
			break;
		case A_DMA_S4CR:
			ret = t->S4CR;
			break;
		case A_DMA_S4NDTR:
			ret = t->S4NDTR;
			break;
		case A_DMA_S4PAR:
			ret = t->S4PAR;
			break;
		case A_DMA_S4M0AR:
			ret = t->S4M0AR;
			break;
		case A_DMA_S4M1AR:
			ret = t->S4M1AR;
			break;
		case A_DMA_S4FCR:
			ret = t->S4FCR;
			break;
		case A_DMA_S5CR:
			ret = t->S5CR;
			break;
		case A_DMA_S5NDTR:
			ret = t->S5NDTR;
			break;
		case A_DMA_S5PAR:
			ret = t->S5PAR;
			break;
		case A_DMA_S5M0AR:
			ret = t->S5M0AR;
			break;
		case A_DMA_S5M1AR:
			ret = t->S5M1AR;
			break;
		case A_DMA_S5FCR:
			ret = t->S5FCR;
			break;
		case A_DMA_S6CR:
			ret = t->S6CR;
			break;
		case A_DMA_S6NDTR:
			ret = t->S6NDTR;
			break;
		case A_DMA_S6PAR:
			ret = t->S6PAR;
			break;
		case A_DMA_S6M0AR:
			ret = t->S6M0AR;
			break;
		case A_DMA_S6M1AR:
			ret = t->S6M1AR;
			break;
		case A_DMA_S6FCR:
			ret = t->S6FCR;
			break;
		case A_DMA_S7CR:
			ret = t->S7CR;
			break;
		case A_DMA_S7NDTR:
			ret = t->S7NDTR;
			break;
		case A_DMA_S7PAR:
			ret = t->S7PAR;
			break;
		case A_DMA_S7M0AR:
			ret = t->S7M0AR;
			break;
		case A_DMA_S7M1AR:
			ret = t->S7M1AR;
			break;
		case A_DMA_S7FCR:
			ret = t->S7FCR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 DMA read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f7_dma_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F7DMA *t = STM32F7_DMA(opaque);


	switch (offset) {
		case A_DMA_LIFCR ... A_DMA_LIFCR + 3:
			value = value << ((offset - A_DMA_LIFCR) << 3);
			offset = A_DMA_LIFCR;
			break;
		case A_DMA_HIFCR ... A_DMA_HIFCR + 3:
			value = value << ((offset - A_DMA_HIFCR) << 3);
			offset = A_DMA_HIFCR;
			break;
		case A_DMA_S0CR ... A_DMA_S0CR + 3:
			value = value << ((offset - A_DMA_S0CR) << 3);
			offset = A_DMA_S0CR;
			break;
		case A_DMA_S0NDTR ... A_DMA_S0NDTR + 3:
			value = value << ((offset - A_DMA_S0NDTR) << 3);
			offset = A_DMA_S0NDTR;
			break;
		case A_DMA_S0PAR ... A_DMA_S0PAR + 3:
			value = value << ((offset - A_DMA_S0PAR) << 3);
			offset = A_DMA_S0PAR;
			break;
		case A_DMA_S0M0AR ... A_DMA_S0M0AR + 3:
			value = value << ((offset - A_DMA_S0M0AR) << 3);
			offset = A_DMA_S0M0AR;
			break;
		case A_DMA_S0M1AR ... A_DMA_S0M1AR + 3:
			value = value << ((offset - A_DMA_S0M1AR) << 3);
			offset = A_DMA_S0M1AR;
			break;
		case A_DMA_S0FCR ... A_DMA_S0FCR + 3:
			value = value << ((offset - A_DMA_S0FCR) << 3);
			offset = A_DMA_S0FCR;
			break;
		case A_DMA_S1CR ... A_DMA_S1CR + 3:
			value = value << ((offset - A_DMA_S1CR) << 3);
			offset = A_DMA_S1CR;
			break;
		case A_DMA_S1NDTR ... A_DMA_S1NDTR + 3:
			value = value << ((offset - A_DMA_S1NDTR) << 3);
			offset = A_DMA_S1NDTR;
			break;
		case A_DMA_S1PAR ... A_DMA_S1PAR + 3:
			value = value << ((offset - A_DMA_S1PAR) << 3);
			offset = A_DMA_S1PAR;
			break;
		case A_DMA_S1M0AR ... A_DMA_S1M0AR + 3:
			value = value << ((offset - A_DMA_S1M0AR) << 3);
			offset = A_DMA_S1M0AR;
			break;
		case A_DMA_S1M1AR ... A_DMA_S1M1AR + 3:
			value = value << ((offset - A_DMA_S1M1AR) << 3);
			offset = A_DMA_S1M1AR;
			break;
		case A_DMA_S1FCR ... A_DMA_S1FCR + 3:
			value = value << ((offset - A_DMA_S1FCR) << 3);
			offset = A_DMA_S1FCR;
			break;
		case A_DMA_S2CR ... A_DMA_S2CR + 3:
			value = value << ((offset - A_DMA_S2CR) << 3);
			offset = A_DMA_S2CR;
			break;
		case A_DMA_S2NDTR ... A_DMA_S2NDTR + 3:
			value = value << ((offset - A_DMA_S2NDTR) << 3);
			offset = A_DMA_S2NDTR;
			break;
		case A_DMA_S2PAR ... A_DMA_S2PAR + 3:
			value = value << ((offset - A_DMA_S2PAR) << 3);
			offset = A_DMA_S2PAR;
			break;
		case A_DMA_S2M0AR ... A_DMA_S2M0AR + 3:
			value = value << ((offset - A_DMA_S2M0AR) << 3);
			offset = A_DMA_S2M0AR;
			break;
		case A_DMA_S2M1AR ... A_DMA_S2M1AR + 3:
			value = value << ((offset - A_DMA_S2M1AR) << 3);
			offset = A_DMA_S2M1AR;
			break;
		case A_DMA_S2FCR ... A_DMA_S2FCR + 3:
			value = value << ((offset - A_DMA_S2FCR) << 3);
			offset = A_DMA_S2FCR;
			break;
		case A_DMA_S3CR ... A_DMA_S3CR + 3:
			value = value << ((offset - A_DMA_S3CR) << 3);
			offset = A_DMA_S3CR;
			break;
		case A_DMA_S3NDTR ... A_DMA_S3NDTR + 3:
			value = value << ((offset - A_DMA_S3NDTR) << 3);
			offset = A_DMA_S3NDTR;
			break;
		case A_DMA_S3PAR ... A_DMA_S3PAR + 3:
			value = value << ((offset - A_DMA_S3PAR) << 3);
			offset = A_DMA_S3PAR;
			break;
		case A_DMA_S3M0AR ... A_DMA_S3M0AR + 3:
			value = value << ((offset - A_DMA_S3M0AR) << 3);
			offset = A_DMA_S3M0AR;
			break;
		case A_DMA_S3M1AR ... A_DMA_S3M1AR + 3:
			value = value << ((offset - A_DMA_S3M1AR) << 3);
			offset = A_DMA_S3M1AR;
			break;
		case A_DMA_S3FCR ... A_DMA_S3FCR + 3:
			value = value << ((offset - A_DMA_S3FCR) << 3);
			offset = A_DMA_S3FCR;
			break;
		case A_DMA_S4CR ... A_DMA_S4CR + 3:
			value = value << ((offset - A_DMA_S4CR) << 3);
			offset = A_DMA_S4CR;
			break;
		case A_DMA_S4NDTR ... A_DMA_S4NDTR + 3:
			value = value << ((offset - A_DMA_S4NDTR) << 3);
			offset = A_DMA_S4NDTR;
			break;
		case A_DMA_S4PAR ... A_DMA_S4PAR + 3:
			value = value << ((offset - A_DMA_S4PAR) << 3);
			offset = A_DMA_S4PAR;
			break;
		case A_DMA_S4M0AR ... A_DMA_S4M0AR + 3:
			value = value << ((offset - A_DMA_S4M0AR) << 3);
			offset = A_DMA_S4M0AR;
			break;
		case A_DMA_S4M1AR ... A_DMA_S4M1AR + 3:
			value = value << ((offset - A_DMA_S4M1AR) << 3);
			offset = A_DMA_S4M1AR;
			break;
		case A_DMA_S4FCR ... A_DMA_S4FCR + 3:
			value = value << ((offset - A_DMA_S4FCR) << 3);
			offset = A_DMA_S4FCR;
			break;
		case A_DMA_S5CR ... A_DMA_S5CR + 3:
			value = value << ((offset - A_DMA_S5CR) << 3);
			offset = A_DMA_S5CR;
			break;
		case A_DMA_S5NDTR ... A_DMA_S5NDTR + 3:
			value = value << ((offset - A_DMA_S5NDTR) << 3);
			offset = A_DMA_S5NDTR;
			break;
		case A_DMA_S5PAR ... A_DMA_S5PAR + 3:
			value = value << ((offset - A_DMA_S5PAR) << 3);
			offset = A_DMA_S5PAR;
			break;
		case A_DMA_S5M0AR ... A_DMA_S5M0AR + 3:
			value = value << ((offset - A_DMA_S5M0AR) << 3);
			offset = A_DMA_S5M0AR;
			break;
		case A_DMA_S5M1AR ... A_DMA_S5M1AR + 3:
			value = value << ((offset - A_DMA_S5M1AR) << 3);
			offset = A_DMA_S5M1AR;
			break;
		case A_DMA_S5FCR ... A_DMA_S5FCR + 3:
			value = value << ((offset - A_DMA_S5FCR) << 3);
			offset = A_DMA_S5FCR;
			break;
		case A_DMA_S6CR ... A_DMA_S6CR + 3:
			value = value << ((offset - A_DMA_S6CR) << 3);
			offset = A_DMA_S6CR;
			break;
		case A_DMA_S6NDTR ... A_DMA_S6NDTR + 3:
			value = value << ((offset - A_DMA_S6NDTR) << 3);
			offset = A_DMA_S6NDTR;
			break;
		case A_DMA_S6PAR ... A_DMA_S6PAR + 3:
			value = value << ((offset - A_DMA_S6PAR) << 3);
			offset = A_DMA_S6PAR;
			break;
		case A_DMA_S6M0AR ... A_DMA_S6M0AR + 3:
			value = value << ((offset - A_DMA_S6M0AR) << 3);
			offset = A_DMA_S6M0AR;
			break;
		case A_DMA_S6M1AR ... A_DMA_S6M1AR + 3:
			value = value << ((offset - A_DMA_S6M1AR) << 3);
			offset = A_DMA_S6M1AR;
			break;
		case A_DMA_S6FCR ... A_DMA_S6FCR + 3:
			value = value << ((offset - A_DMA_S6FCR) << 3);
			offset = A_DMA_S6FCR;
			break;
		case A_DMA_S7CR ... A_DMA_S7CR + 3:
			value = value << ((offset - A_DMA_S7CR) << 3);
			offset = A_DMA_S7CR;
			break;
		case A_DMA_S7NDTR ... A_DMA_S7NDTR + 3:
			value = value << ((offset - A_DMA_S7NDTR) << 3);
			offset = A_DMA_S7NDTR;
			break;
		case A_DMA_S7PAR ... A_DMA_S7PAR + 3:
			value = value << ((offset - A_DMA_S7PAR) << 3);
			offset = A_DMA_S7PAR;
			break;
		case A_DMA_S7M0AR ... A_DMA_S7M0AR + 3:
			value = value << ((offset - A_DMA_S7M0AR) << 3);
			offset = A_DMA_S7M0AR;
			break;
		case A_DMA_S7M1AR ... A_DMA_S7M1AR + 3:
			value = value << ((offset - A_DMA_S7M1AR) << 3);
			offset = A_DMA_S7M1AR;
			break;
		case A_DMA_S7FCR ... A_DMA_S7FCR + 3:
			value = value << ((offset - A_DMA_S7FCR) << 3);
			offset = A_DMA_S7FCR;
			break;

		default: break;
	}

	switch (offset) {
		case A_DMA_LIFCR:
			t->LIFCR = value;
			break;
		case A_DMA_HIFCR:
			t->HIFCR = value;
			break;
		case A_DMA_S0CR:
			t->S0CR = value;
			break;
		case A_DMA_S0NDTR:
			t->S0NDTR = value;
			break;
		case A_DMA_S0PAR:
			t->S0PAR = value;
			break;
		case A_DMA_S0M0AR:
			t->S0M0AR = value;
			break;
		case A_DMA_S0M1AR:
			t->S0M1AR = value;
			break;
		case A_DMA_S0FCR:
			t->S0FCR = value;
			break;
		case A_DMA_S1CR:
			t->S1CR = value;
			break;
		case A_DMA_S1NDTR:
			t->S1NDTR = value;
			break;
		case A_DMA_S1PAR:
			t->S1PAR = value;
			break;
		case A_DMA_S1M0AR:
			t->S1M0AR = value;
			break;
		case A_DMA_S1M1AR:
			t->S1M1AR = value;
			break;
		case A_DMA_S1FCR:
			t->S1FCR = value;
			break;
		case A_DMA_S2CR:
			t->S2CR = value;
			break;
		case A_DMA_S2NDTR:
			t->S2NDTR = value;
			break;
		case A_DMA_S2PAR:
			t->S2PAR = value;
			break;
		case A_DMA_S2M0AR:
			t->S2M0AR = value;
			break;
		case A_DMA_S2M1AR:
			t->S2M1AR = value;
			break;
		case A_DMA_S2FCR:
			t->S2FCR = value;
			break;
		case A_DMA_S3CR:
			t->S3CR = value;
			break;
		case A_DMA_S3NDTR:
			t->S3NDTR = value;
			break;
		case A_DMA_S3PAR:
			t->S3PAR = value;
			break;
		case A_DMA_S3M0AR:
			t->S3M0AR = value;
			break;
		case A_DMA_S3M1AR:
			t->S3M1AR = value;
			break;
		case A_DMA_S3FCR:
			t->S3FCR = value;
			break;
		case A_DMA_S4CR:
			t->S4CR = value;
			break;
		case A_DMA_S4NDTR:
			t->S4NDTR = value;
			break;
		case A_DMA_S4PAR:
			t->S4PAR = value;
			break;
		case A_DMA_S4M0AR:
			t->S4M0AR = value;
			break;
		case A_DMA_S4M1AR:
			t->S4M1AR = value;
			break;
		case A_DMA_S4FCR:
			t->S4FCR = value;
			break;
		case A_DMA_S5CR:
			t->S5CR = value;
			break;
		case A_DMA_S5NDTR:
			t->S5NDTR = value;
			break;
		case A_DMA_S5PAR:
			t->S5PAR = value;
			break;
		case A_DMA_S5M0AR:
			t->S5M0AR = value;
			break;
		case A_DMA_S5M1AR:
			t->S5M1AR = value;
			break;
		case A_DMA_S5FCR:
			t->S5FCR = value;
			break;
		case A_DMA_S6CR:
			t->S6CR = value;
			break;
		case A_DMA_S6NDTR:
			t->S6NDTR = value;
			break;
		case A_DMA_S6PAR:
			t->S6PAR = value;
			break;
		case A_DMA_S6M0AR:
			t->S6M0AR = value;
			break;
		case A_DMA_S6M1AR:
			t->S6M1AR = value;
			break;
		case A_DMA_S6FCR:
			t->S6FCR = value;
			break;
		case A_DMA_S7CR:
			t->S7CR = value;
			break;
		case A_DMA_S7NDTR:
			t->S7NDTR = value;
			break;
		case A_DMA_S7PAR:
			t->S7PAR = value;
			break;
		case A_DMA_S7M0AR:
			t->S7M0AR = value;
			break;
		case A_DMA_S7M1AR:
			t->S7M1AR = value;
			break;
		case A_DMA_S7FCR:
			t->S7FCR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 DMA write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f7_dma_ops = {
	.read = stm32f7_dma_read,
	.write = stm32f7_dma_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f7_dma_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F7DMA *t = STM32F7_DMA(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f7_dma_ops, t, "stm32f7-dma", STM32F7_DMA_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 8; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32f7_dma_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32f7_dma_reset_enter(Object *obj, ResetType type) {
	STM32F7DMA *t = STM32F7_DMA(obj);
	stm32f7_dma_register_reset(t);
}

static const VMStateDescription stm32f7_dma_vmstate = {
	.name = "stm32f7-dma",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(LISR, STM32F7DMA),
		VMSTATE_UINT32(HISR, STM32F7DMA),
		VMSTATE_UINT32(LIFCR, STM32F7DMA),
		VMSTATE_UINT32(HIFCR, STM32F7DMA),
		VMSTATE_UINT32(S0CR, STM32F7DMA),
		VMSTATE_UINT32(S0NDTR, STM32F7DMA),
		VMSTATE_UINT32(S0PAR, STM32F7DMA),
		VMSTATE_UINT32(S0M0AR, STM32F7DMA),
		VMSTATE_UINT32(S0M1AR, STM32F7DMA),
		VMSTATE_UINT32(S0FCR, STM32F7DMA),
		VMSTATE_UINT32(S1CR, STM32F7DMA),
		VMSTATE_UINT32(S1NDTR, STM32F7DMA),
		VMSTATE_UINT32(S1PAR, STM32F7DMA),
		VMSTATE_UINT32(S1M0AR, STM32F7DMA),
		VMSTATE_UINT32(S1M1AR, STM32F7DMA),
		VMSTATE_UINT32(S1FCR, STM32F7DMA),
		VMSTATE_UINT32(S2CR, STM32F7DMA),
		VMSTATE_UINT32(S2NDTR, STM32F7DMA),
		VMSTATE_UINT32(S2PAR, STM32F7DMA),
		VMSTATE_UINT32(S2M0AR, STM32F7DMA),
		VMSTATE_UINT32(S2M1AR, STM32F7DMA),
		VMSTATE_UINT32(S2FCR, STM32F7DMA),
		VMSTATE_UINT32(S3CR, STM32F7DMA),
		VMSTATE_UINT32(S3NDTR, STM32F7DMA),
		VMSTATE_UINT32(S3PAR, STM32F7DMA),
		VMSTATE_UINT32(S3M0AR, STM32F7DMA),
		VMSTATE_UINT32(S3M1AR, STM32F7DMA),
		VMSTATE_UINT32(S3FCR, STM32F7DMA),
		VMSTATE_UINT32(S4CR, STM32F7DMA),
		VMSTATE_UINT32(S4NDTR, STM32F7DMA),
		VMSTATE_UINT32(S4PAR, STM32F7DMA),
		VMSTATE_UINT32(S4M0AR, STM32F7DMA),
		VMSTATE_UINT32(S4M1AR, STM32F7DMA),
		VMSTATE_UINT32(S4FCR, STM32F7DMA),
		VMSTATE_UINT32(S5CR, STM32F7DMA),
		VMSTATE_UINT32(S5NDTR, STM32F7DMA),
		VMSTATE_UINT32(S5PAR, STM32F7DMA),
		VMSTATE_UINT32(S5M0AR, STM32F7DMA),
		VMSTATE_UINT32(S5M1AR, STM32F7DMA),
		VMSTATE_UINT32(S5FCR, STM32F7DMA),
		VMSTATE_UINT32(S6CR, STM32F7DMA),
		VMSTATE_UINT32(S6NDTR, STM32F7DMA),
		VMSTATE_UINT32(S6PAR, STM32F7DMA),
		VMSTATE_UINT32(S6M0AR, STM32F7DMA),
		VMSTATE_UINT32(S6M1AR, STM32F7DMA),
		VMSTATE_UINT32(S6FCR, STM32F7DMA),
		VMSTATE_UINT32(S7CR, STM32F7DMA),
		VMSTATE_UINT32(S7NDTR, STM32F7DMA),
		VMSTATE_UINT32(S7PAR, STM32F7DMA),
		VMSTATE_UINT32(S7M0AR, STM32F7DMA),
		VMSTATE_UINT32(S7M1AR, STM32F7DMA),
		VMSTATE_UINT32(S7FCR, STM32F7DMA),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f7_dma_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f7_dma_vmstate;
	dc->realize = stm32f7_dma_realize;
	rc->phases.enter = stm32f7_dma_reset_enter;

}

static const TypeInfo stm32f7_dma_info = {
	.name = TYPE_STM32F7_DMA,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F7DMA),
	.instance_init = stm32f7_dma_init,
	.class_init = stm32f7_dma_class_init,
};

static void stm32f7_dma_register_types(void) {
	type_register_static(&stm32f7_dma_info);
}

type_init(stm32f7_dma_register_types);

#define TYPE_STM32F7_FMC "stm32f7-fmc"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F7FMC, STM32F7_FMC)

struct STM32F7FMC {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t BCR1;
	uint32_t BTR1;
	uint32_t BCR2;
	uint32_t BTR2;
	uint32_t BCR3;
	uint32_t BTR3;
	uint32_t BCR4;
	uint32_t BTR4;
	uint32_t PCR;
	uint32_t SR;
	uint32_t PMEM;
	uint32_t PATT;
	uint32_t ECCR;
	uint32_t BWTR1;
	uint32_t BWTR2;
	uint32_t BWTR3;
	uint32_t BWTR4;
	uint32_t SDCR1;
	uint32_t SDCR2;
	uint32_t SDTR1;
	uint32_t SDTR2;
	uint32_t SDCMR;
	uint32_t SDRTR;
	uint32_t SDSR;
	/* base */
	uint32_t base;

};
#define STM32F7_FMC_SIZE				0x15c

REG32(FMC_BCR1, 0x0)
	FIELD(FMC_BCR1, CCLKEN, 20, 1)
	FIELD(FMC_BCR1, CBURSTRW, 19, 1)
	FIELD(FMC_BCR1, ASYNCWAIT, 15, 1)
	FIELD(FMC_BCR1, EXTMOD, 14, 1)
	FIELD(FMC_BCR1, WAITEN, 13, 1)
	FIELD(FMC_BCR1, WREN, 12, 1)
	FIELD(FMC_BCR1, WAITCFG, 11, 1)
	FIELD(FMC_BCR1, WAITPOL, 9, 1)
	FIELD(FMC_BCR1, BURSTEN, 8, 1)
	FIELD(FMC_BCR1, FACCEN, 6, 1)
	FIELD(FMC_BCR1, MWID, 4, 2)
	FIELD(FMC_BCR1, MTYP, 2, 2)
	FIELD(FMC_BCR1, MUXEN, 1, 1)
	FIELD(FMC_BCR1, MBKEN, 0, 1)
REG32(FMC_BTR1, 0x4)
	FIELD(FMC_BTR1, ACCMOD, 28, 2)
	FIELD(FMC_BTR1, DATLAT, 24, 4)
	FIELD(FMC_BTR1, CLKDIV, 20, 4)
	FIELD(FMC_BTR1, BUSTURN, 16, 4)
	FIELD(FMC_BTR1, DATAST, 8, 8)
	FIELD(FMC_BTR1, ADDHLD, 4, 4)
	FIELD(FMC_BTR1, ADDSET, 0, 4)
REG32(FMC_BCR2, 0x8)
	FIELD(FMC_BCR2, CBURSTRW, 19, 1)
	FIELD(FMC_BCR2, ASYNCWAIT, 15, 1)
	FIELD(FMC_BCR2, EXTMOD, 14, 1)
	FIELD(FMC_BCR2, WAITEN, 13, 1)
	FIELD(FMC_BCR2, WREN, 12, 1)
	FIELD(FMC_BCR2, WAITCFG, 11, 1)
	FIELD(FMC_BCR2, WRAPMOD, 10, 1)
	FIELD(FMC_BCR2, WAITPOL, 9, 1)
	FIELD(FMC_BCR2, BURSTEN, 8, 1)
	FIELD(FMC_BCR2, FACCEN, 6, 1)
	FIELD(FMC_BCR2, MWID, 4, 2)
	FIELD(FMC_BCR2, MTYP, 2, 2)
	FIELD(FMC_BCR2, MUXEN, 1, 1)
	FIELD(FMC_BCR2, MBKEN, 0, 1)
REG32(FMC_BTR2, 0xc)
	FIELD(FMC_BTR2, ACCMOD, 28, 2)
	FIELD(FMC_BTR2, DATLAT, 24, 4)
	FIELD(FMC_BTR2, CLKDIV, 20, 4)
	FIELD(FMC_BTR2, BUSTURN, 16, 4)
	FIELD(FMC_BTR2, DATAST, 8, 8)
	FIELD(FMC_BTR2, ADDHLD, 4, 4)
	FIELD(FMC_BTR2, ADDSET, 0, 4)
REG32(FMC_BCR3, 0x10)
	FIELD(FMC_BCR3, CBURSTRW, 19, 1)
	FIELD(FMC_BCR3, ASYNCWAIT, 15, 1)
	FIELD(FMC_BCR3, EXTMOD, 14, 1)
	FIELD(FMC_BCR3, WAITEN, 13, 1)
	FIELD(FMC_BCR3, WREN, 12, 1)
	FIELD(FMC_BCR3, WAITCFG, 11, 1)
	FIELD(FMC_BCR3, WRAPMOD, 10, 1)
	FIELD(FMC_BCR3, WAITPOL, 9, 1)
	FIELD(FMC_BCR3, BURSTEN, 8, 1)
	FIELD(FMC_BCR3, FACCEN, 6, 1)
	FIELD(FMC_BCR3, MWID, 4, 2)
	FIELD(FMC_BCR3, MTYP, 2, 2)
	FIELD(FMC_BCR3, MUXEN, 1, 1)
	FIELD(FMC_BCR3, MBKEN, 0, 1)
REG32(FMC_BTR3, 0x14)
	FIELD(FMC_BTR3, ACCMOD, 28, 2)
	FIELD(FMC_BTR3, DATLAT, 24, 4)
	FIELD(FMC_BTR3, CLKDIV, 20, 4)
	FIELD(FMC_BTR3, BUSTURN, 16, 4)
	FIELD(FMC_BTR3, DATAST, 8, 8)
	FIELD(FMC_BTR3, ADDHLD, 4, 4)
	FIELD(FMC_BTR3, ADDSET, 0, 4)
REG32(FMC_BCR4, 0x18)
	FIELD(FMC_BCR4, CBURSTRW, 19, 1)
	FIELD(FMC_BCR4, ASYNCWAIT, 15, 1)
	FIELD(FMC_BCR4, EXTMOD, 14, 1)
	FIELD(FMC_BCR4, WAITEN, 13, 1)
	FIELD(FMC_BCR4, WREN, 12, 1)
	FIELD(FMC_BCR4, WAITCFG, 11, 1)
	FIELD(FMC_BCR4, WRAPMOD, 10, 1)
	FIELD(FMC_BCR4, WAITPOL, 9, 1)
	FIELD(FMC_BCR4, BURSTEN, 8, 1)
	FIELD(FMC_BCR4, FACCEN, 6, 1)
	FIELD(FMC_BCR4, MWID, 4, 2)
	FIELD(FMC_BCR4, MTYP, 2, 2)
	FIELD(FMC_BCR4, MUXEN, 1, 1)
	FIELD(FMC_BCR4, MBKEN, 0, 1)
REG32(FMC_BTR4, 0x1c)
	FIELD(FMC_BTR4, ACCMOD, 28, 2)
	FIELD(FMC_BTR4, DATLAT, 24, 4)
	FIELD(FMC_BTR4, CLKDIV, 20, 4)
	FIELD(FMC_BTR4, BUSTURN, 16, 4)
	FIELD(FMC_BTR4, DATAST, 8, 8)
	FIELD(FMC_BTR4, ADDHLD, 4, 4)
	FIELD(FMC_BTR4, ADDSET, 0, 4)
REG32(FMC_PCR, 0x80)
	FIELD(FMC_PCR, ECCPS, 17, 3)
	FIELD(FMC_PCR, TAR, 13, 4)
	FIELD(FMC_PCR, TCLR, 9, 4)
	FIELD(FMC_PCR, ECCEN, 6, 1)
	FIELD(FMC_PCR, PWID, 4, 2)
	FIELD(FMC_PCR, PTYP, 3, 1)
	FIELD(FMC_PCR, PBKEN, 2, 1)
	FIELD(FMC_PCR, PWAITEN, 1, 1)
REG32(FMC_SR, 0x84)
	FIELD(FMC_SR, FEMPT, 6, 1)
	FIELD(FMC_SR, IFEN, 5, 1)
	FIELD(FMC_SR, ILEN, 4, 1)
	FIELD(FMC_SR, IREN, 3, 1)
	FIELD(FMC_SR, IFS, 2, 1)
	FIELD(FMC_SR, ILS, 1, 1)
	FIELD(FMC_SR, IRS, 0, 1)
REG32(FMC_PMEM, 0x88)
	FIELD(FMC_PMEM, MEMHIZx, 24, 8)
	FIELD(FMC_PMEM, MEMHOLDx, 16, 8)
	FIELD(FMC_PMEM, MEMWAITx, 8, 8)
	FIELD(FMC_PMEM, MEMSETx, 0, 8)
REG32(FMC_PATT, 0x8c)
	FIELD(FMC_PATT, ATTHIZx, 24, 8)
	FIELD(FMC_PATT, ATTHOLDx, 16, 8)
	FIELD(FMC_PATT, ATTWAITx, 8, 8)
	FIELD(FMC_PATT, ATTSETx, 0, 8)
REG32(FMC_ECCR, 0x94)
	FIELD(FMC_ECCR, ECCx, 0, 32)
REG32(FMC_BWTR1, 0x104)
	FIELD(FMC_BWTR1, ACCMOD, 28, 2)
	FIELD(FMC_BWTR1, DATLAT, 24, 4)
	FIELD(FMC_BWTR1, CLKDIV, 20, 4)
	FIELD(FMC_BWTR1, DATAST, 8, 8)
	FIELD(FMC_BWTR1, ADDHLD, 4, 4)
	FIELD(FMC_BWTR1, ADDSET, 0, 4)
REG32(FMC_BWTR2, 0x10c)
	FIELD(FMC_BWTR2, ACCMOD, 28, 2)
	FIELD(FMC_BWTR2, DATLAT, 24, 4)
	FIELD(FMC_BWTR2, CLKDIV, 20, 4)
	FIELD(FMC_BWTR2, DATAST, 8, 8)
	FIELD(FMC_BWTR2, ADDHLD, 4, 4)
	FIELD(FMC_BWTR2, ADDSET, 0, 4)
REG32(FMC_BWTR3, 0x114)
	FIELD(FMC_BWTR3, ACCMOD, 28, 2)
	FIELD(FMC_BWTR3, DATLAT, 24, 4)
	FIELD(FMC_BWTR3, CLKDIV, 20, 4)
	FIELD(FMC_BWTR3, DATAST, 8, 8)
	FIELD(FMC_BWTR3, ADDHLD, 4, 4)
	FIELD(FMC_BWTR3, ADDSET, 0, 4)
REG32(FMC_BWTR4, 0x11c)
	FIELD(FMC_BWTR4, ACCMOD, 28, 2)
	FIELD(FMC_BWTR4, DATLAT, 24, 4)
	FIELD(FMC_BWTR4, CLKDIV, 20, 4)
	FIELD(FMC_BWTR4, DATAST, 8, 8)
	FIELD(FMC_BWTR4, ADDHLD, 4, 4)
	FIELD(FMC_BWTR4, ADDSET, 0, 4)
REG32(FMC_SDCR1, 0x140)
	FIELD(FMC_SDCR1, NC, 0, 2)
	FIELD(FMC_SDCR1, NR, 2, 2)
	FIELD(FMC_SDCR1, MWID, 4, 2)
	FIELD(FMC_SDCR1, NB, 6, 1)
	FIELD(FMC_SDCR1, CAS, 7, 2)
	FIELD(FMC_SDCR1, WP, 9, 1)
	FIELD(FMC_SDCR1, SDCLK, 10, 2)
	FIELD(FMC_SDCR1, RBURST, 12, 1)
	FIELD(FMC_SDCR1, RPIPE, 13, 2)
REG32(FMC_SDCR2, 0x144)
	FIELD(FMC_SDCR2, NC, 0, 2)
	FIELD(FMC_SDCR2, NR, 2, 2)
	FIELD(FMC_SDCR2, MWID, 4, 2)
	FIELD(FMC_SDCR2, NB, 6, 1)
	FIELD(FMC_SDCR2, CAS, 7, 2)
	FIELD(FMC_SDCR2, WP, 9, 1)
	FIELD(FMC_SDCR2, SDCLK, 10, 2)
	FIELD(FMC_SDCR2, RBURST, 12, 1)
	FIELD(FMC_SDCR2, RPIPE, 13, 2)
REG32(FMC_SDTR1, 0x148)
	FIELD(FMC_SDTR1, TMRD, 0, 4)
	FIELD(FMC_SDTR1, TXSR, 4, 4)
	FIELD(FMC_SDTR1, TRAS, 8, 4)
	FIELD(FMC_SDTR1, TRC, 12, 4)
	FIELD(FMC_SDTR1, TWR, 16, 4)
	FIELD(FMC_SDTR1, TRP, 20, 4)
	FIELD(FMC_SDTR1, TRCD, 24, 4)
REG32(FMC_SDTR2, 0x14c)
	FIELD(FMC_SDTR2, TMRD, 0, 4)
	FIELD(FMC_SDTR2, TXSR, 4, 4)
	FIELD(FMC_SDTR2, TRAS, 8, 4)
	FIELD(FMC_SDTR2, TRC, 12, 4)
	FIELD(FMC_SDTR2, TWR, 16, 4)
	FIELD(FMC_SDTR2, TRP, 20, 4)
	FIELD(FMC_SDTR2, TRCD, 24, 4)
REG32(FMC_SDCMR, 0x150)
	FIELD(FMC_SDCMR, MODE, 0, 3)
	FIELD(FMC_SDCMR, CTB2, 3, 1)
	FIELD(FMC_SDCMR, CTB1, 4, 1)
	FIELD(FMC_SDCMR, NRFS, 5, 4)
	FIELD(FMC_SDCMR, MRD, 9, 13)
REG32(FMC_SDRTR, 0x154)
	FIELD(FMC_SDRTR, CRE, 0, 1)
	FIELD(FMC_SDRTR, COUNT, 1, 13)
	FIELD(FMC_SDRTR, REIE, 14, 1)
REG32(FMC_SDSR, 0x158)
	FIELD(FMC_SDSR, RE, 0, 1)
	FIELD(FMC_SDSR, MODES1, 1, 2)
	FIELD(FMC_SDSR, MODES2, 3, 2)
	FIELD(FMC_SDSR, BUSY, 5, 1)


static void stm32f7_fmc_register_reset(STM32F7FMC *t) {
	t->BCR1 = 0x30d0;
	t->BTR1 = 0xffffffff;
	t->BCR2 = 0x30d0;
	t->BTR2 = 0xffffffff;
	t->BCR3 = 0x30d0;
	t->BTR3 = 0xffffffff;
	t->BCR4 = 0x30d0;
	t->BTR4 = 0xffffffff;
	t->PCR = 0x18;
	t->SR = 0x40;
	t->PMEM = 0xfcfcfcfc;
	t->PATT = 0xfcfcfcfc;
	t->ECCR = 0x0;
	t->BWTR1 = 0xfffffff;
	t->BWTR2 = 0xfffffff;
	t->BWTR3 = 0xfffffff;
	t->BWTR4 = 0xfffffff;
	t->SDCR1 = 0x2d0;
	t->SDCR2 = 0x2d0;
	t->SDTR1 = 0xfffffff;
	t->SDTR2 = 0xfffffff;
	t->SDCMR = 0x0;
	t->SDRTR = 0x0;
	t->SDSR = 0x0;

}

static uint64_t stm32f7_fmc_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F7FMC *t = STM32F7_FMC(opaque);
	uint64_t ret;

	switch (offset) {
		case A_FMC_BCR1:
			ret = t->BCR1;
			break;
		case A_FMC_BTR1:
			ret = t->BTR1;
			break;
		case A_FMC_BCR2:
			ret = t->BCR2;
			break;
		case A_FMC_BTR2:
			ret = t->BTR2;
			break;
		case A_FMC_BCR3:
			ret = t->BCR3;
			break;
		case A_FMC_BTR3:
			ret = t->BTR3;
			break;
		case A_FMC_BCR4:
			ret = t->BCR4;
			break;
		case A_FMC_BTR4:
			ret = t->BTR4;
			break;
		case A_FMC_PCR:
			ret = t->PCR;
			break;
		case A_FMC_SR:
			ret = t->SR;
			break;
		case A_FMC_PMEM:
			ret = t->PMEM;
			break;
		case A_FMC_PATT:
			ret = t->PATT;
			break;
		case A_FMC_ECCR:
			ret = t->ECCR;
			break;
		case A_FMC_BWTR1:
			ret = t->BWTR1;
			break;
		case A_FMC_BWTR2:
			ret = t->BWTR2;
			break;
		case A_FMC_BWTR3:
			ret = t->BWTR3;
			break;
		case A_FMC_BWTR4:
			ret = t->BWTR4;
			break;
		case A_FMC_SDCR1:
			ret = t->SDCR1;
			break;
		case A_FMC_SDCR2:
			ret = t->SDCR2;
			break;
		case A_FMC_SDTR1:
			ret = t->SDTR1;
			break;
		case A_FMC_SDTR2:
			ret = t->SDTR2;
			break;
		case A_FMC_SDCMR:
			ret = t->SDCMR;
			break;
		case A_FMC_SDRTR:
			ret = t->SDRTR;
			break;
		case A_FMC_SDSR:
			ret = t->SDSR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 FMC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f7_fmc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F7FMC *t = STM32F7_FMC(opaque);


	switch (offset) {
		case A_FMC_BCR1 ... A_FMC_BCR1 + 3:
			value = value << ((offset - A_FMC_BCR1) << 3);
			offset = A_FMC_BCR1;
			break;
		case A_FMC_BTR1 ... A_FMC_BTR1 + 3:
			value = value << ((offset - A_FMC_BTR1) << 3);
			offset = A_FMC_BTR1;
			break;
		case A_FMC_BCR2 ... A_FMC_BCR2 + 3:
			value = value << ((offset - A_FMC_BCR2) << 3);
			offset = A_FMC_BCR2;
			break;
		case A_FMC_BTR2 ... A_FMC_BTR2 + 3:
			value = value << ((offset - A_FMC_BTR2) << 3);
			offset = A_FMC_BTR2;
			break;
		case A_FMC_BCR3 ... A_FMC_BCR3 + 3:
			value = value << ((offset - A_FMC_BCR3) << 3);
			offset = A_FMC_BCR3;
			break;
		case A_FMC_BTR3 ... A_FMC_BTR3 + 3:
			value = value << ((offset - A_FMC_BTR3) << 3);
			offset = A_FMC_BTR3;
			break;
		case A_FMC_BCR4 ... A_FMC_BCR4 + 3:
			value = value << ((offset - A_FMC_BCR4) << 3);
			offset = A_FMC_BCR4;
			break;
		case A_FMC_BTR4 ... A_FMC_BTR4 + 3:
			value = value << ((offset - A_FMC_BTR4) << 3);
			offset = A_FMC_BTR4;
			break;
		case A_FMC_PCR ... A_FMC_PCR + 3:
			value = value << ((offset - A_FMC_PCR) << 3);
			offset = A_FMC_PCR;
			break;
		case A_FMC_SR ... A_FMC_SR + 3:
			value = value << ((offset - A_FMC_SR) << 3);
			offset = A_FMC_SR;
			break;
		case A_FMC_PMEM ... A_FMC_PMEM + 3:
			value = value << ((offset - A_FMC_PMEM) << 3);
			offset = A_FMC_PMEM;
			break;
		case A_FMC_PATT ... A_FMC_PATT + 3:
			value = value << ((offset - A_FMC_PATT) << 3);
			offset = A_FMC_PATT;
			break;
		case A_FMC_BWTR1 ... A_FMC_BWTR1 + 3:
			value = value << ((offset - A_FMC_BWTR1) << 3);
			offset = A_FMC_BWTR1;
			break;
		case A_FMC_BWTR2 ... A_FMC_BWTR2 + 3:
			value = value << ((offset - A_FMC_BWTR2) << 3);
			offset = A_FMC_BWTR2;
			break;
		case A_FMC_BWTR3 ... A_FMC_BWTR3 + 3:
			value = value << ((offset - A_FMC_BWTR3) << 3);
			offset = A_FMC_BWTR3;
			break;
		case A_FMC_BWTR4 ... A_FMC_BWTR4 + 3:
			value = value << ((offset - A_FMC_BWTR4) << 3);
			offset = A_FMC_BWTR4;
			break;
		case A_FMC_SDCR1 ... A_FMC_SDCR1 + 3:
			value = value << ((offset - A_FMC_SDCR1) << 3);
			offset = A_FMC_SDCR1;
			break;
		case A_FMC_SDCR2 ... A_FMC_SDCR2 + 3:
			value = value << ((offset - A_FMC_SDCR2) << 3);
			offset = A_FMC_SDCR2;
			break;
		case A_FMC_SDTR1 ... A_FMC_SDTR1 + 3:
			value = value << ((offset - A_FMC_SDTR1) << 3);
			offset = A_FMC_SDTR1;
			break;
		case A_FMC_SDTR2 ... A_FMC_SDTR2 + 3:
			value = value << ((offset - A_FMC_SDTR2) << 3);
			offset = A_FMC_SDTR2;
			break;
		case A_FMC_SDCMR ... A_FMC_SDCMR + 3:
			value = value << ((offset - A_FMC_SDCMR) << 3);
			offset = A_FMC_SDCMR;
			break;
		case A_FMC_SDRTR ... A_FMC_SDRTR + 3:
			value = value << ((offset - A_FMC_SDRTR) << 3);
			offset = A_FMC_SDRTR;
			break;

		default: break;
	}

	switch (offset) {
		case A_FMC_BCR1:
			t->BCR1 = value;
			break;
		case A_FMC_BTR1:
			t->BTR1 = value;
			break;
		case A_FMC_BCR2:
			t->BCR2 = value;
			break;
		case A_FMC_BTR2:
			t->BTR2 = value;
			break;
		case A_FMC_BCR3:
			t->BCR3 = value;
			break;
		case A_FMC_BTR3:
			t->BTR3 = value;
			break;
		case A_FMC_BCR4:
			t->BCR4 = value;
			break;
		case A_FMC_BTR4:
			t->BTR4 = value;
			break;
		case A_FMC_PCR:
			t->PCR = value;
			break;
		case A_FMC_SR:
			t->SR = value;
			break;
		case A_FMC_PMEM:
			t->PMEM = value;
			break;
		case A_FMC_PATT:
			t->PATT = value;
			break;
		case A_FMC_BWTR1:
			t->BWTR1 = value;
			break;
		case A_FMC_BWTR2:
			t->BWTR2 = value;
			break;
		case A_FMC_BWTR3:
			t->BWTR3 = value;
			break;
		case A_FMC_BWTR4:
			t->BWTR4 = value;
			break;
		case A_FMC_SDCR1:
			t->SDCR1 = value;
			break;
		case A_FMC_SDCR2:
			t->SDCR2 = value;
			break;
		case A_FMC_SDTR1:
			t->SDTR1 = value;
			break;
		case A_FMC_SDTR2:
			t->SDTR2 = value;
			break;
		case A_FMC_SDCMR:
			t->SDCMR = value;
			break;
		case A_FMC_SDRTR:
			t->SDRTR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 FMC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f7_fmc_ops = {
	.read = stm32f7_fmc_read,
	.write = stm32f7_fmc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f7_fmc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F7FMC *t = STM32F7_FMC(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f7_fmc_ops, t, "stm32f7-fmc", STM32F7_FMC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32f7_fmc_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32f7_fmc_reset_enter(Object *obj, ResetType type) {
	STM32F7FMC *t = STM32F7_FMC(obj);
	stm32f7_fmc_register_reset(t);
}

static const VMStateDescription stm32f7_fmc_vmstate = {
	.name = "stm32f7-fmc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(BCR1, STM32F7FMC),
		VMSTATE_UINT32(BTR1, STM32F7FMC),
		VMSTATE_UINT32(BCR2, STM32F7FMC),
		VMSTATE_UINT32(BTR2, STM32F7FMC),
		VMSTATE_UINT32(BCR3, STM32F7FMC),
		VMSTATE_UINT32(BTR3, STM32F7FMC),
		VMSTATE_UINT32(BCR4, STM32F7FMC),
		VMSTATE_UINT32(BTR4, STM32F7FMC),
		VMSTATE_UINT32(PCR, STM32F7FMC),
		VMSTATE_UINT32(SR, STM32F7FMC),
		VMSTATE_UINT32(PMEM, STM32F7FMC),
		VMSTATE_UINT32(PATT, STM32F7FMC),
		VMSTATE_UINT32(ECCR, STM32F7FMC),
		VMSTATE_UINT32(BWTR1, STM32F7FMC),
		VMSTATE_UINT32(BWTR2, STM32F7FMC),
		VMSTATE_UINT32(BWTR3, STM32F7FMC),
		VMSTATE_UINT32(BWTR4, STM32F7FMC),
		VMSTATE_UINT32(SDCR1, STM32F7FMC),
		VMSTATE_UINT32(SDCR2, STM32F7FMC),
		VMSTATE_UINT32(SDTR1, STM32F7FMC),
		VMSTATE_UINT32(SDTR2, STM32F7FMC),
		VMSTATE_UINT32(SDCMR, STM32F7FMC),
		VMSTATE_UINT32(SDRTR, STM32F7FMC),
		VMSTATE_UINT32(SDSR, STM32F7FMC),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f7_fmc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f7_fmc_vmstate;
	dc->realize = stm32f7_fmc_realize;
	rc->phases.enter = stm32f7_fmc_reset_enter;

}

static const TypeInfo stm32f7_fmc_info = {
	.name = TYPE_STM32F7_FMC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F7FMC),
	.instance_init = stm32f7_fmc_init,
	.class_init = stm32f7_fmc_class_init,
};

static void stm32f7_fmc_register_types(void) {
	type_register_static(&stm32f7_fmc_info);
}

type_init(stm32f7_fmc_register_types);

#define TYPE_STM32F769_MACHINE MACHINE_TYPE_NAME("stm32f769")
OBJECT_DECLARE_TYPE(STM32F769MachineState, STM32F769MachineClass, STM32F769_MACHINE)

struct STM32F769MachineClass {
	MachineClass parent;
};

struct STM32F769MachineState {
	MachineState parent;
	ARMv7MState armv7m;
};

static void stm32f769_periph_init(MachineState *machine) {
	STM32F769MachineState *sms = STM32F769_MACHINE(machine);
	STM32F7TIM3 *p0 = g_new(STM32F7TIM3, 1);
	object_initialize_child(OBJECT(sms), "TIM3", p0, TYPE_STM32F7_TIM3);
	sysbus_realize(SYS_BUS_DEVICE(p0), &error_fatal);
	p0->base = 0x40000400;
	sysbus_connect_irq(SYS_BUS_DEVICE(p0), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 29));
	sysbus_mmio_map(SYS_BUS_DEVICE(p0), 0, 0x40000400);

	STM32F7USART *p1 = g_new(STM32F7USART, 1);
	object_initialize_child(OBJECT(sms), "USART1", p1, TYPE_STM32F7_USART);
	qdev_prop_set_chr(DEVICE(p1), "chardev", qemu_chr_new("soc-usart1", "chardev:usart1", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p1), &error_fatal);
	p1->base = 0x40011000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p1), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 37));
	sysbus_mmio_map(SYS_BUS_DEVICE(p1), 0, 0x40011000);

	STM32F7USART *p2 = g_new(STM32F7USART, 1);
	object_initialize_child(OBJECT(sms), "USART2", p2, TYPE_STM32F7_USART);
	qdev_prop_set_chr(DEVICE(p2), "chardev", qemu_chr_new("soc-usart2", "chardev:usart2", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p2), &error_fatal);
	p2->base = 0x40004400;
	sysbus_connect_irq(SYS_BUS_DEVICE(p2), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 38));
	sysbus_mmio_map(SYS_BUS_DEVICE(p2), 0, 0x40004400);

	STM32F7USART *p3 = g_new(STM32F7USART, 1);
	object_initialize_child(OBJECT(sms), "USART6", p3, TYPE_STM32F7_USART);
	qdev_prop_set_chr(DEVICE(p3), "chardev", qemu_chr_new("soc-usart6", "chardev:usart6", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p3), &error_fatal);
	p3->base = 0x40011400;
	sysbus_connect_irq(SYS_BUS_DEVICE(p3), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 71));
	sysbus_mmio_map(SYS_BUS_DEVICE(p3), 0, 0x40011400);

	STM32F7USART *p4 = g_new(STM32F7USART, 1);
	object_initialize_child(OBJECT(sms), "USART3", p4, TYPE_STM32F7_USART);
	qdev_prop_set_chr(DEVICE(p4), "chardev", qemu_chr_new("soc-usart3", "chardev:usart3", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p4), &error_fatal);
	p4->base = 0x40004800;
	sysbus_connect_irq(SYS_BUS_DEVICE(p4), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 39));
	sysbus_mmio_map(SYS_BUS_DEVICE(p4), 0, 0x40004800);

	STM32F7PWR *p5 = g_new(STM32F7PWR, 1);
	object_initialize_child(OBJECT(sms), "PWR", p5, TYPE_STM32F7_PWR);
	sysbus_realize(SYS_BUS_DEVICE(p5), &error_fatal);
	p5->base = 0x40007000;
	sysbus_mmio_map(SYS_BUS_DEVICE(p5), 0, 0x40007000);

	STM32F7RCC *p6 = g_new(STM32F7RCC, 1);
	object_initialize_child(OBJECT(sms), "RCC", p6, TYPE_STM32F7_RCC);
	sysbus_realize(SYS_BUS_DEVICE(p6), &error_fatal);
	p6->base = 0x40023800;
	sysbus_connect_irq(SYS_BUS_DEVICE(p6), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 5));
	sysbus_mmio_map(SYS_BUS_DEVICE(p6), 0, 0x40023800);

	STM32F7FLASH *p7 = g_new(STM32F7FLASH, 1);
	object_initialize_child(OBJECT(sms), "Flash", p7, TYPE_STM32F7_FLASH);
	sysbus_realize(SYS_BUS_DEVICE(p7), &error_fatal);
	p7->base = 0x40023c00;
	sysbus_connect_irq(SYS_BUS_DEVICE(p7), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 4));
	sysbus_mmio_map(SYS_BUS_DEVICE(p7), 0, 0x40023c00);

	STM32F7GPIOA *p8 = g_new(STM32F7GPIOA, 1);
	object_initialize_child(OBJECT(sms), "GPIOA", p8, TYPE_STM32F7_GPIOA);
	sysbus_realize(SYS_BUS_DEVICE(p8), &error_fatal);
	p8->base = 0x40020000;
	sysbus_mmio_map(SYS_BUS_DEVICE(p8), 0, 0x40020000);

	STM32F7GPIOB *p9 = g_new(STM32F7GPIOB, 1);
	object_initialize_child(OBJECT(sms), "GPIOB", p9, TYPE_STM32F7_GPIOB);
	sysbus_realize(SYS_BUS_DEVICE(p9), &error_fatal);
	p9->base = 0x40020400;
	sysbus_mmio_map(SYS_BUS_DEVICE(p9), 0, 0x40020400);

	STM32F7GPIOD *p10 = g_new(STM32F7GPIOD, 1);
	object_initialize_child(OBJECT(sms), "GPIOC", p10, TYPE_STM32F7_GPIOD);
	sysbus_realize(SYS_BUS_DEVICE(p10), &error_fatal);
	p10->base = 0x40020800;
	sysbus_mmio_map(SYS_BUS_DEVICE(p10), 0, 0x40020800);

	STM32F7GPIOD *p11 = g_new(STM32F7GPIOD, 1);
	object_initialize_child(OBJECT(sms), "GPIOD", p11, TYPE_STM32F7_GPIOD);
	sysbus_realize(SYS_BUS_DEVICE(p11), &error_fatal);
	p11->base = 0x40020c00;
	sysbus_mmio_map(SYS_BUS_DEVICE(p11), 0, 0x40020c00);

	STM32F7GPIOD *p12 = g_new(STM32F7GPIOD, 1);
	object_initialize_child(OBJECT(sms), "GPIOE", p12, TYPE_STM32F7_GPIOD);
	sysbus_realize(SYS_BUS_DEVICE(p12), &error_fatal);
	p12->base = 0x40021000;
	sysbus_mmio_map(SYS_BUS_DEVICE(p12), 0, 0x40021000);

	STM32F7GPIOD *p13 = g_new(STM32F7GPIOD, 1);
	object_initialize_child(OBJECT(sms), "GPIOF", p13, TYPE_STM32F7_GPIOD);
	sysbus_realize(SYS_BUS_DEVICE(p13), &error_fatal);
	p13->base = 0x40021400;
	sysbus_mmio_map(SYS_BUS_DEVICE(p13), 0, 0x40021400);

	STM32F7GPIOD *p14 = g_new(STM32F7GPIOD, 1);
	object_initialize_child(OBJECT(sms), "GPIOG", p14, TYPE_STM32F7_GPIOD);
	sysbus_realize(SYS_BUS_DEVICE(p14), &error_fatal);
	p14->base = 0x40021800;
	sysbus_mmio_map(SYS_BUS_DEVICE(p14), 0, 0x40021800);

	STM32F7GPIOD *p15 = g_new(STM32F7GPIOD, 1);
	object_initialize_child(OBJECT(sms), "GPIOH", p15, TYPE_STM32F7_GPIOD);
	sysbus_realize(SYS_BUS_DEVICE(p15), &error_fatal);
	p15->base = 0x40021c00;
	sysbus_mmio_map(SYS_BUS_DEVICE(p15), 0, 0x40021c00);

	STM32F7GPIOD *p16 = g_new(STM32F7GPIOD, 1);
	object_initialize_child(OBJECT(sms), "GPIOI", p16, TYPE_STM32F7_GPIOD);
	sysbus_realize(SYS_BUS_DEVICE(p16), &error_fatal);
	p16->base = 0x40022000;
	sysbus_mmio_map(SYS_BUS_DEVICE(p16), 0, 0x40022000);

	STM32F7RNG *p17 = g_new(STM32F7RNG, 1);
	object_initialize_child(OBJECT(sms), "RNG", p17, TYPE_STM32F7_RNG);
	sysbus_realize(SYS_BUS_DEVICE(p17), &error_fatal);
	p17->base = 0x50060800;
	sysbus_connect_irq(SYS_BUS_DEVICE(p17), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 80));
	sysbus_mmio_map(SYS_BUS_DEVICE(p17), 0, 0x50060800);

	STM32F7SPI *p18 = g_new(STM32F7SPI, 1);
	object_initialize_child(OBJECT(sms), "SPI1", p18, TYPE_STM32F7_SPI);
	qdev_prop_set_chr(DEVICE(p18), "chardev", qemu_chr_new("soc-spi1", "chardev:spi1", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p18), &error_fatal);
	p18->base = 0x40013000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p18), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 35));
	sysbus_mmio_map(SYS_BUS_DEVICE(p18), 0, 0x40013000);

	STM32F7SYSCFG *p19 = g_new(STM32F7SYSCFG, 1);
	object_initialize_child(OBJECT(sms), "SYSCFG", p19, TYPE_STM32F7_SYSCFG);
	sysbus_realize(SYS_BUS_DEVICE(p19), &error_fatal);
	p19->base = 0x40013800;
	sysbus_mmio_map(SYS_BUS_DEVICE(p19), 0, 0x40013800);

	STM32F7DMA *p20 = g_new(STM32F7DMA, 1);
	object_initialize_child(OBJECT(sms), "DMA1", p20, TYPE_STM32F7_DMA);
	sysbus_realize(SYS_BUS_DEVICE(p20), &error_fatal);
	p20->base = 0x40026000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p20), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 11));
	sysbus_connect_irq(SYS_BUS_DEVICE(p20), 1, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 12));
	sysbus_connect_irq(SYS_BUS_DEVICE(p20), 2, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 13));
	sysbus_connect_irq(SYS_BUS_DEVICE(p20), 3, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 14));
	sysbus_connect_irq(SYS_BUS_DEVICE(p20), 4, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 15));
	sysbus_connect_irq(SYS_BUS_DEVICE(p20), 5, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 16));
	sysbus_connect_irq(SYS_BUS_DEVICE(p20), 6, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 17));
	sysbus_connect_irq(SYS_BUS_DEVICE(p20), 7, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 47));
	sysbus_mmio_map(SYS_BUS_DEVICE(p20), 0, 0x40026000);

	STM32F7DMA *p21 = g_new(STM32F7DMA, 1);
	object_initialize_child(OBJECT(sms), "DMA2", p21, TYPE_STM32F7_DMA);
	sysbus_realize(SYS_BUS_DEVICE(p21), &error_fatal);
	p21->base = 0x40026400;
	sysbus_connect_irq(SYS_BUS_DEVICE(p21), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 56));
	sysbus_connect_irq(SYS_BUS_DEVICE(p21), 1, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 57));
	sysbus_connect_irq(SYS_BUS_DEVICE(p21), 2, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 58));
	sysbus_connect_irq(SYS_BUS_DEVICE(p21), 3, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 59));
	sysbus_connect_irq(SYS_BUS_DEVICE(p21), 4, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 60));
	sysbus_connect_irq(SYS_BUS_DEVICE(p21), 5, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 68));
	sysbus_connect_irq(SYS_BUS_DEVICE(p21), 6, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 69));
	sysbus_connect_irq(SYS_BUS_DEVICE(p21), 7, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 70));
	sysbus_mmio_map(SYS_BUS_DEVICE(p21), 0, 0x40026400);

	STM32F7FMC *p22 = g_new(STM32F7FMC, 1);
	object_initialize_child(OBJECT(sms), "FMC", p22, TYPE_STM32F7_FMC);
	sysbus_realize(SYS_BUS_DEVICE(p22), &error_fatal);
	p22->base = 0xa0000000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p22), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 48));
	sysbus_mmio_map(SYS_BUS_DEVICE(p22), 0, 0xa0000000);

	STM32F7I2C *p23 = g_new(STM32F7I2C, 1);
	object_initialize_child(OBJECT(sms), "I2C1", p23, TYPE_STM32F7_I2C);
	qdev_prop_set_chr(DEVICE(p23), "chardev", qemu_chr_new("soc-i2c1", "chardev:i2c1", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p23), &error_fatal);
	p23->base = 0x40005400;
	sysbus_connect_irq(SYS_BUS_DEVICE(p23), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 31));
	sysbus_connect_irq(SYS_BUS_DEVICE(p23), 1, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 32));
	sysbus_mmio_map(SYS_BUS_DEVICE(p23), 0, 0x40005400);


}

static void stm32f769_common_init(MachineState *machine) {
	STM32F769MachineState *sms = STM32F769_MACHINE(machine);
	MemoryRegion *sysmem = get_system_memory();
	MemoryRegion *mem;
	Clock *cpuclk = clock_new(OBJECT(sms), "SYSCLK");
	clock_set_hz(cpuclk, 216000000);
	Clock *refclk = clock_new(OBJECT(sms), "REFCLK");
	clock_set_hz(refclk, 216000000);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_ram(mem, NULL, "wtf", 0x2000, &error_fatal);
	memory_region_add_subregion(sysmem, 0x0, mem);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_rom(mem, NULL, "main-flash", 0x800000, &error_fatal);
	memory_region_add_subregion(sysmem, 0x8000000, mem);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_rom(mem, NULL, "option-bytes", 0x3800, &error_fatal);
	memory_region_add_subregion(sysmem, 0x1fffc800, mem);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_ram(mem, NULL, "sram", 0x20000000, &error_fatal);
	memory_region_add_subregion(sysmem, 0x20000000, mem);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_ram(mem, NULL, "sdram", 0x1000000, &error_fatal);
	memory_region_add_subregion(sysmem, 0xc0000000, mem);


	object_initialize_child(OBJECT(sms), "armv7m", &(sms->armv7m), TYPE_ARMV7M);
	qdev_prop_set_uint32(DEVICE(&(sms->armv7m)),"num-irq", 256);
	qdev_prop_set_string(DEVICE(&(sms->armv7m)), "cpu-type", machine->cpu_type);
	qdev_prop_set_bit(DEVICE(&(sms->armv7m)), "enable-bitband", false);
	qdev_connect_clock_in(DEVICE(&(sms->armv7m)), "cpuclk", cpuclk);
	qdev_connect_clock_in(DEVICE(&(sms->armv7m)), "refclk", refclk);
	qdev_prop_set_uint32(DEVICE(&(sms->armv7m)), "init-nsvtor", 0x8000000);
	object_property_set_link(OBJECT(&sms->armv7m), "memory", OBJECT(sysmem), &error_abort);
	sysbus_realize(SYS_BUS_DEVICE(&sms->armv7m), &error_fatal);

	stm32f769_periph_init(machine);

	armv7m_load_kernel(ARM_CPU(first_cpu), machine->kernel_filename, 0x8000000, 0x800000);
}

static void stm32f769_class_init(ObjectClass *oc, void *data) {
	MachineClass *mc = MACHINE_CLASS(oc);
	mc->desc = "stm32f769";
	mc->init = stm32f769_common_init;
	mc->max_cpus = 1;
	mc->default_cpus = 1;
	mc->default_cpu_type = ARM_CPU_TYPE_NAME("cortex-m7");
}

static const TypeInfo stm32f769_info = {
	.name = TYPE_STM32F769_MACHINE,
	.parent = TYPE_MACHINE,
	.instance_size = sizeof(STM32F769MachineState),
	.class_size = sizeof(STM32F769MachineClass),
	.class_init = stm32f769_class_init
};

static void stm32f769_machine_init(void) {
	type_register_static(&stm32f769_info);
}

type_init(stm32f769_machine_init);
