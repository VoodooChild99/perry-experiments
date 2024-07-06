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


#define TYPE_FRDMKL25Z_RTC "frdmkl25z-rtc"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMKL25ZRTC, FRDMKL25Z_RTC)

struct FRDMKL25ZRTC {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[2];

	/*registers*/
	uint32_t TSR;
	uint32_t TPR;
	uint32_t TAR;
	uint32_t TCR;
	uint32_t CR;
	uint32_t SR;
	uint32_t LR;
	uint32_t IER;
	/* base */
	uint32_t base;

};
#define FRDMKL25Z_RTC_SIZE				0x20

REG32(RTC_TSR, 0x0)
	FIELD(RTC_TSR, TSR, 0, 32)
REG32(RTC_TPR, 0x4)
	FIELD(RTC_TPR, TPR, 0, 16)
REG32(RTC_TAR, 0x8)
	FIELD(RTC_TAR, TAR, 0, 32)
REG32(RTC_TCR, 0xc)
	FIELD(RTC_TCR, TCR, 0, 8)
	FIELD(RTC_TCR, CIR, 8, 8)
	FIELD(RTC_TCR, TCV, 16, 8)
	FIELD(RTC_TCR, CIC, 24, 8)
REG32(RTC_CR, 0x10)
	FIELD(RTC_CR, SWR, 0, 1)
	FIELD(RTC_CR, WPE, 1, 1)
	FIELD(RTC_CR, SUP, 2, 1)
	FIELD(RTC_CR, UM, 3, 1)
	FIELD(RTC_CR, OSCE, 8, 1)
	FIELD(RTC_CR, CLKO, 9, 1)
	FIELD(RTC_CR, SC16P, 10, 1)
	FIELD(RTC_CR, SC8P, 11, 1)
	FIELD(RTC_CR, SC4P, 12, 1)
	FIELD(RTC_CR, SC2P, 13, 1)
REG32(RTC_SR, 0x14)
	FIELD(RTC_SR, TIF, 0, 1)
	FIELD(RTC_SR, TOF, 1, 1)
	FIELD(RTC_SR, TAF, 2, 1)
	FIELD(RTC_SR, TCE, 4, 1)
REG32(RTC_LR, 0x18)
	FIELD(RTC_LR, TCL, 3, 1)
	FIELD(RTC_LR, CRL, 4, 1)
	FIELD(RTC_LR, SRL, 5, 1)
	FIELD(RTC_LR, LRL, 6, 1)
REG32(RTC_IER, 0x1c)
	FIELD(RTC_IER, TIIE, 0, 1)
	FIELD(RTC_IER, TOIE, 1, 1)
	FIELD(RTC_IER, TAIE, 2, 1)
	FIELD(RTC_IER, TSIE, 4, 1)
	FIELD(RTC_IER, WPON, 7, 1)


static void frdmkl25z_rtc_register_reset(FRDMKL25ZRTC *t) {
	t->TSR = 0x0;
	t->TPR = 0x0;
	t->TAR = 0x0;
	t->TCR = 0x0;
	t->CR = 0x0;
	t->SR = 0x1;
	t->LR = 0xff;
	t->IER = 0x7;

}

static uint64_t frdmkl25z_rtc_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMKL25ZRTC *t = FRDMKL25Z_RTC(opaque);
	uint64_t ret;

	switch (offset) {
		case A_RTC_TSR:
			ret = t->TSR;
			break;
		case A_RTC_TPR:
			ret = t->TPR;
			break;
		case A_RTC_TAR:
			ret = t->TAR;
			break;
		case A_RTC_TCR:
			ret = t->TCR;
			break;
		case A_RTC_CR:
			ret = t->CR;
			break;
		case A_RTC_SR:
			ret = t->SR;
			break;
		case A_RTC_LR:
			ret = t->LR;
			break;
		case A_RTC_IER:
			ret = t->IER;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMKL25Z RTC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmkl25z_rtc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMKL25ZRTC *t = FRDMKL25Z_RTC(opaque);


	switch (offset) {
		case A_RTC_TSR ... A_RTC_TSR + 3:
			value = value << ((offset - A_RTC_TSR) << 3);
			offset = A_RTC_TSR;
			break;
		case A_RTC_TPR ... A_RTC_TPR + 3:
			value = value << ((offset - A_RTC_TPR) << 3);
			offset = A_RTC_TPR;
			break;
		case A_RTC_TAR ... A_RTC_TAR + 3:
			value = value << ((offset - A_RTC_TAR) << 3);
			offset = A_RTC_TAR;
			break;
		case A_RTC_TCR ... A_RTC_TCR + 3:
			value = value << ((offset - A_RTC_TCR) << 3);
			offset = A_RTC_TCR;
			break;
		case A_RTC_CR ... A_RTC_CR + 3:
			value = value << ((offset - A_RTC_CR) << 3);
			offset = A_RTC_CR;
			break;
		case A_RTC_SR ... A_RTC_SR + 3:
			value = value << ((offset - A_RTC_SR) << 3);
			offset = A_RTC_SR;
			break;
		case A_RTC_LR ... A_RTC_LR + 3:
			value = value << ((offset - A_RTC_LR) << 3);
			offset = A_RTC_LR;
			break;
		case A_RTC_IER ... A_RTC_IER + 3:
			value = value << ((offset - A_RTC_IER) << 3);
			offset = A_RTC_IER;
			break;

		default: break;
	}

	switch (offset) {
		case A_RTC_TSR:
			t->TSR = value;
			break;
		case A_RTC_TPR:
			t->TPR = value;
			break;
		case A_RTC_TAR:
			t->TAR = value;
			break;
		case A_RTC_TCR:
			t->TCR = value;
			break;
		case A_RTC_CR:
			t->CR = value;
			break;
		case A_RTC_SR:
			t->SR = value;
			break;
		case A_RTC_LR:
			t->LR = value;
			break;
		case A_RTC_IER:
			t->IER = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMKL25Z RTC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmkl25z_rtc_ops = {
	.read = frdmkl25z_rtc_read,
	.write = frdmkl25z_rtc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmkl25z_rtc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMKL25ZRTC *t = FRDMKL25Z_RTC(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmkl25z_rtc_ops, t, "frdmkl25z-rtc", FRDMKL25Z_RTC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 2; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void frdmkl25z_rtc_realize(DeviceState *dev, Error **errp) {
	return;

}

static void frdmkl25z_rtc_reset_enter(Object *obj, ResetType type) {
	FRDMKL25ZRTC *t = FRDMKL25Z_RTC(obj);
	frdmkl25z_rtc_register_reset(t);
}

static const VMStateDescription frdmkl25z_rtc_vmstate = {
	.name = "frdmkl25z-rtc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(TSR, FRDMKL25ZRTC),
		VMSTATE_UINT32(TPR, FRDMKL25ZRTC),
		VMSTATE_UINT32(TAR, FRDMKL25ZRTC),
		VMSTATE_UINT32(TCR, FRDMKL25ZRTC),
		VMSTATE_UINT32(CR, FRDMKL25ZRTC),
		VMSTATE_UINT32(SR, FRDMKL25ZRTC),
		VMSTATE_UINT32(LR, FRDMKL25ZRTC),
		VMSTATE_UINT32(IER, FRDMKL25ZRTC),

		VMSTATE_END_OF_LIST()
	}
};

static void frdmkl25z_rtc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmkl25z_rtc_vmstate;
	dc->realize = frdmkl25z_rtc_realize;
	rc->phases.enter = frdmkl25z_rtc_reset_enter;

}

static const TypeInfo frdmkl25z_rtc_info = {
	.name = TYPE_FRDMKL25Z_RTC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMKL25ZRTC),
	.instance_init = frdmkl25z_rtc_init,
	.class_init = frdmkl25z_rtc_class_init,
};

static void frdmkl25z_rtc_register_types(void) {
	type_register_static(&frdmkl25z_rtc_info);
}

type_init(frdmkl25z_rtc_register_types);

#define TYPE_FRDMKL25Z_SIM "frdmkl25z-sim"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMKL25ZSIM, FRDMKL25Z_SIM)

struct FRDMKL25ZSIM {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/*registers*/
	uint32_t SOPT1;
	uint32_t SOPT1CFG;
	uint32_t SOPT2;
	uint32_t SOPT4;
	uint32_t SOPT5;
	uint32_t SOPT7;
	uint32_t SDID;
	uint32_t SCGC4;
	uint32_t SCGC5;
	uint32_t SCGC6;
	uint32_t SCGC7;
	uint32_t CLKDIV1;
	uint32_t FCFG1;
	uint32_t FCFG2;
	uint32_t UIDMH;
	uint32_t UIDML;
	uint32_t UIDL;
	uint32_t COPC;
	uint32_t SRVCOP;
	/* base */
	uint32_t base;

};
#define FRDMKL25Z_SIM_SIZE				0x1108

REG32(SIM_SOPT1, 0x0)
	FIELD(SIM_SOPT1, OSC32KSEL, 18, 2)
	FIELD(SIM_SOPT1, USBVSTBY, 29, 1)
	FIELD(SIM_SOPT1, USBSSTBY, 30, 1)
	FIELD(SIM_SOPT1, USBREGEN, 31, 1)
REG32(SIM_SOPT1CFG, 0x4)
	FIELD(SIM_SOPT1CFG, URWE, 24, 1)
	FIELD(SIM_SOPT1CFG, UVSWE, 25, 1)
	FIELD(SIM_SOPT1CFG, USSWE, 26, 1)
REG32(SIM_SOPT2, 0x1004)
	FIELD(SIM_SOPT2, RTCCLKOUTSEL, 4, 1)
	FIELD(SIM_SOPT2, CLKOUTSEL, 5, 3)
	FIELD(SIM_SOPT2, PLLFLLSEL, 16, 1)
	FIELD(SIM_SOPT2, USBSRC, 18, 1)
	FIELD(SIM_SOPT2, TPMSRC, 24, 2)
	FIELD(SIM_SOPT2, UART0SRC, 26, 2)
REG32(SIM_SOPT4, 0x100c)
	FIELD(SIM_SOPT4, TPM1CH0SRC, 18, 1)
	FIELD(SIM_SOPT4, TPM2CH0SRC, 20, 1)
	FIELD(SIM_SOPT4, TPM0CLKSEL, 24, 1)
	FIELD(SIM_SOPT4, TPM1CLKSEL, 25, 1)
	FIELD(SIM_SOPT4, TPM2CLKSEL, 26, 1)
REG32(SIM_SOPT5, 0x1010)
	FIELD(SIM_SOPT5, UART0TXSRC, 0, 2)
	FIELD(SIM_SOPT5, UART0RXSRC, 2, 1)
	FIELD(SIM_SOPT5, UART1TXSRC, 4, 2)
	FIELD(SIM_SOPT5, UART1RXSRC, 6, 1)
	FIELD(SIM_SOPT5, UART0ODE, 16, 1)
	FIELD(SIM_SOPT5, UART1ODE, 17, 1)
	FIELD(SIM_SOPT5, UART2ODE, 18, 1)
REG32(SIM_SOPT7, 0x1018)
	FIELD(SIM_SOPT7, ADC0TRGSEL, 0, 4)
	FIELD(SIM_SOPT7, ADC0PRETRGSEL, 4, 1)
	FIELD(SIM_SOPT7, ADC0ALTTRGEN, 7, 1)
REG32(SIM_SDID, 0x1024)
	FIELD(SIM_SDID, PINID, 0, 4)
	FIELD(SIM_SDID, DIEID, 7, 5)
	FIELD(SIM_SDID, REVID, 12, 4)
	FIELD(SIM_SDID, SRAMSIZE, 16, 4)
	FIELD(SIM_SDID, SERIESID, 20, 4)
	FIELD(SIM_SDID, SUBFAMID, 24, 4)
	FIELD(SIM_SDID, FAMID, 28, 4)
REG32(SIM_SCGC4, 0x1034)
	FIELD(SIM_SCGC4, I2C0, 6, 1)
	FIELD(SIM_SCGC4, I2C1, 7, 1)
	FIELD(SIM_SCGC4, UART0, 10, 1)
	FIELD(SIM_SCGC4, UART1, 11, 1)
	FIELD(SIM_SCGC4, UART2, 12, 1)
	FIELD(SIM_SCGC4, USBOTG, 18, 1)
	FIELD(SIM_SCGC4, CMP, 19, 1)
	FIELD(SIM_SCGC4, SPI0, 22, 1)
	FIELD(SIM_SCGC4, SPI1, 23, 1)
REG32(SIM_SCGC5, 0x1038)
	FIELD(SIM_SCGC5, LPTMR, 0, 1)
	FIELD(SIM_SCGC5, TSI, 5, 1)
	FIELD(SIM_SCGC5, PORTA, 9, 1)
	FIELD(SIM_SCGC5, PORTB, 10, 1)
	FIELD(SIM_SCGC5, PORTC, 11, 1)
	FIELD(SIM_SCGC5, PORTD, 12, 1)
	FIELD(SIM_SCGC5, PORTE, 13, 1)
REG32(SIM_SCGC6, 0x103c)
	FIELD(SIM_SCGC6, FTF, 0, 1)
	FIELD(SIM_SCGC6, DMAMUX, 1, 1)
	FIELD(SIM_SCGC6, PIT, 23, 1)
	FIELD(SIM_SCGC6, TPM0, 24, 1)
	FIELD(SIM_SCGC6, TPM1, 25, 1)
	FIELD(SIM_SCGC6, TPM2, 26, 1)
	FIELD(SIM_SCGC6, ADC0, 27, 1)
	FIELD(SIM_SCGC6, RTC, 29, 1)
	FIELD(SIM_SCGC6, DAC0, 31, 1)
REG32(SIM_SCGC7, 0x1040)
	FIELD(SIM_SCGC7, DMA, 8, 1)
REG32(SIM_CLKDIV1, 0x1044)
	FIELD(SIM_CLKDIV1, OUTDIV4, 16, 3)
	FIELD(SIM_CLKDIV1, OUTDIV1, 28, 4)
REG32(SIM_FCFG1, 0x104c)
	FIELD(SIM_FCFG1, FLASHDIS, 0, 1)
	FIELD(SIM_FCFG1, FLASHDOZE, 1, 1)
	FIELD(SIM_FCFG1, PFSIZE, 24, 4)
REG32(SIM_FCFG2, 0x1050)
	FIELD(SIM_FCFG2, MAXADDR0, 24, 7)
REG32(SIM_UIDMH, 0x1058)
	FIELD(SIM_UIDMH, UID, 0, 16)
REG32(SIM_UIDML, 0x105c)
	FIELD(SIM_UIDML, UID, 0, 32)
REG32(SIM_UIDL, 0x1060)
	FIELD(SIM_UIDL, UID, 0, 32)
REG32(SIM_COPC, 0x1100)
	FIELD(SIM_COPC, COPW, 0, 1)
	FIELD(SIM_COPC, COPCLKS, 1, 1)
	FIELD(SIM_COPC, COPT, 2, 2)
REG32(SIM_SRVCOP, 0x1104)
	FIELD(SIM_SRVCOP, SRVCOP, 0, 8)


static void frdmkl25z_sim_register_reset(FRDMKL25ZSIM *t) {
	t->SOPT1 = 0x80000000;
	t->SOPT1CFG = 0x0;
	t->SOPT2 = 0x0;
	t->SOPT4 = 0x0;
	t->SOPT5 = 0x0;
	t->SOPT7 = 0x0;
	t->SDID = 0x100480;
	t->SCGC4 = 0xf0000030;
	t->SCGC5 = 0x180;
	t->SCGC6 = 0x1;
	t->SCGC7 = 0x100;
	t->CLKDIV1 = 0x10000;
	t->FCFG1 = 0xf000000;
	t->FCFG2 = 0x7f800000;
	t->UIDMH = 0x0;
	t->UIDML = 0x0;
	t->UIDL = 0x0;
	t->COPC = 0xc;
	t->SRVCOP = 0x0;

}

static uint64_t frdmkl25z_sim_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMKL25ZSIM *t = FRDMKL25Z_SIM(opaque);
	uint64_t ret;

	switch (offset) {
		case A_SIM_SOPT1:
			ret = t->SOPT1;
			break;
		case A_SIM_SOPT1CFG:
			ret = t->SOPT1CFG;
			break;
		case A_SIM_SOPT2:
			ret = t->SOPT2;
			break;
		case A_SIM_SOPT4:
			ret = t->SOPT4;
			break;
		case A_SIM_SOPT5:
			ret = t->SOPT5;
			break;
		case A_SIM_SOPT7:
			ret = t->SOPT7;
			break;
		case A_SIM_SDID:
			ret = t->SDID;
			break;
		case A_SIM_SCGC4:
			ret = t->SCGC4;
			break;
		case A_SIM_SCGC5:
			ret = t->SCGC5;
			break;
		case A_SIM_SCGC6:
			ret = t->SCGC6;
			break;
		case A_SIM_SCGC7:
			ret = t->SCGC7;
			break;
		case A_SIM_CLKDIV1:
			ret = t->CLKDIV1;
			break;
		case A_SIM_FCFG1:
			ret = t->FCFG1;
			break;
		case A_SIM_FCFG2:
			ret = t->FCFG2;
			break;
		case A_SIM_UIDMH:
			ret = t->UIDMH;
			break;
		case A_SIM_UIDML:
			ret = t->UIDML;
			break;
		case A_SIM_UIDL:
			ret = t->UIDL;
			break;
		case A_SIM_COPC:
			ret = t->COPC;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMKL25Z SIM read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmkl25z_sim_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMKL25ZSIM *t = FRDMKL25Z_SIM(opaque);


	switch (offset) {
		case A_SIM_SOPT1 ... A_SIM_SOPT1 + 3:
			value = value << ((offset - A_SIM_SOPT1) << 3);
			offset = A_SIM_SOPT1;
			break;
		case A_SIM_SOPT1CFG ... A_SIM_SOPT1CFG + 3:
			value = value << ((offset - A_SIM_SOPT1CFG) << 3);
			offset = A_SIM_SOPT1CFG;
			break;
		case A_SIM_SOPT2 ... A_SIM_SOPT2 + 3:
			value = value << ((offset - A_SIM_SOPT2) << 3);
			offset = A_SIM_SOPT2;
			break;
		case A_SIM_SOPT4 ... A_SIM_SOPT4 + 3:
			value = value << ((offset - A_SIM_SOPT4) << 3);
			offset = A_SIM_SOPT4;
			break;
		case A_SIM_SOPT5 ... A_SIM_SOPT5 + 3:
			value = value << ((offset - A_SIM_SOPT5) << 3);
			offset = A_SIM_SOPT5;
			break;
		case A_SIM_SOPT7 ... A_SIM_SOPT7 + 3:
			value = value << ((offset - A_SIM_SOPT7) << 3);
			offset = A_SIM_SOPT7;
			break;
		case A_SIM_SCGC4 ... A_SIM_SCGC4 + 3:
			value = value << ((offset - A_SIM_SCGC4) << 3);
			offset = A_SIM_SCGC4;
			break;
		case A_SIM_SCGC5 ... A_SIM_SCGC5 + 3:
			value = value << ((offset - A_SIM_SCGC5) << 3);
			offset = A_SIM_SCGC5;
			break;
		case A_SIM_SCGC6 ... A_SIM_SCGC6 + 3:
			value = value << ((offset - A_SIM_SCGC6) << 3);
			offset = A_SIM_SCGC6;
			break;
		case A_SIM_SCGC7 ... A_SIM_SCGC7 + 3:
			value = value << ((offset - A_SIM_SCGC7) << 3);
			offset = A_SIM_SCGC7;
			break;
		case A_SIM_CLKDIV1 ... A_SIM_CLKDIV1 + 3:
			value = value << ((offset - A_SIM_CLKDIV1) << 3);
			offset = A_SIM_CLKDIV1;
			break;
		case A_SIM_FCFG1 ... A_SIM_FCFG1 + 3:
			value = value << ((offset - A_SIM_FCFG1) << 3);
			offset = A_SIM_FCFG1;
			break;
		case A_SIM_COPC ... A_SIM_COPC + 3:
			value = value << ((offset - A_SIM_COPC) << 3);
			offset = A_SIM_COPC;
			break;
		case A_SIM_SRVCOP ... A_SIM_SRVCOP + 3:
			value = value << ((offset - A_SIM_SRVCOP) << 3);
			offset = A_SIM_SRVCOP;
			break;

		default: break;
	}

	switch (offset) {
		case A_SIM_SOPT1:
			t->SOPT1 = value;
			break;
		case A_SIM_SOPT1CFG:
			t->SOPT1CFG = value;
			break;
		case A_SIM_SOPT2:
			t->SOPT2 = value;
			break;
		case A_SIM_SOPT4:
			t->SOPT4 = value;
			break;
		case A_SIM_SOPT5:
			t->SOPT5 = value;
			break;
		case A_SIM_SOPT7:
			t->SOPT7 = value;
			break;
		case A_SIM_SCGC4:
			t->SCGC4 = value;
			break;
		case A_SIM_SCGC5:
			t->SCGC5 = value;
			break;
		case A_SIM_SCGC6:
			t->SCGC6 = value;
			break;
		case A_SIM_SCGC7:
			t->SCGC7 = value;
			break;
		case A_SIM_CLKDIV1:
			t->CLKDIV1 = value;
			break;
		case A_SIM_FCFG1:
			t->FCFG1 = value;
			break;
		case A_SIM_COPC:
			t->COPC = value;
			break;
		case A_SIM_SRVCOP:
			t->SRVCOP = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMKL25Z SIM write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmkl25z_sim_ops = {
	.read = frdmkl25z_sim_read,
	.write = frdmkl25z_sim_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmkl25z_sim_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMKL25ZSIM *t = FRDMKL25Z_SIM(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmkl25z_sim_ops, t, "frdmkl25z-sim", FRDMKL25Z_SIM_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void frdmkl25z_sim_realize(DeviceState *dev, Error **errp) {
	return;

}

static void frdmkl25z_sim_reset_enter(Object *obj, ResetType type) {
	FRDMKL25ZSIM *t = FRDMKL25Z_SIM(obj);
	frdmkl25z_sim_register_reset(t);
}

static const VMStateDescription frdmkl25z_sim_vmstate = {
	.name = "frdmkl25z-sim",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(SOPT1, FRDMKL25ZSIM),
		VMSTATE_UINT32(SOPT1CFG, FRDMKL25ZSIM),
		VMSTATE_UINT32(SOPT2, FRDMKL25ZSIM),
		VMSTATE_UINT32(SOPT4, FRDMKL25ZSIM),
		VMSTATE_UINT32(SOPT5, FRDMKL25ZSIM),
		VMSTATE_UINT32(SOPT7, FRDMKL25ZSIM),
		VMSTATE_UINT32(SDID, FRDMKL25ZSIM),
		VMSTATE_UINT32(SCGC4, FRDMKL25ZSIM),
		VMSTATE_UINT32(SCGC5, FRDMKL25ZSIM),
		VMSTATE_UINT32(SCGC6, FRDMKL25ZSIM),
		VMSTATE_UINT32(SCGC7, FRDMKL25ZSIM),
		VMSTATE_UINT32(CLKDIV1, FRDMKL25ZSIM),
		VMSTATE_UINT32(FCFG1, FRDMKL25ZSIM),
		VMSTATE_UINT32(FCFG2, FRDMKL25ZSIM),
		VMSTATE_UINT32(UIDMH, FRDMKL25ZSIM),
		VMSTATE_UINT32(UIDML, FRDMKL25ZSIM),
		VMSTATE_UINT32(UIDL, FRDMKL25ZSIM),
		VMSTATE_UINT32(COPC, FRDMKL25ZSIM),
		VMSTATE_UINT32(SRVCOP, FRDMKL25ZSIM),

		VMSTATE_END_OF_LIST()
	}
};

static void frdmkl25z_sim_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmkl25z_sim_vmstate;
	dc->realize = frdmkl25z_sim_realize;
	rc->phases.enter = frdmkl25z_sim_reset_enter;

}

static const TypeInfo frdmkl25z_sim_info = {
	.name = TYPE_FRDMKL25Z_SIM,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMKL25ZSIM),
	.instance_init = frdmkl25z_sim_init,
	.class_init = frdmkl25z_sim_class_init,
};

static void frdmkl25z_sim_register_types(void) {
	type_register_static(&frdmkl25z_sim_info);
}

type_init(frdmkl25z_sim_register_types);

#define TYPE_FRDMKL25Z_GPIO "frdmkl25z-gpio"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMKL25ZGPIO, FRDMKL25Z_GPIO)

struct FRDMKL25ZGPIO {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t PDOR;
	uint32_t PSOR;
	uint32_t PCOR;
	uint32_t PTOR;
	uint32_t PDIR;
	uint32_t PDDR;
	/* base */
	uint32_t base;

};
#define FRDMKL25Z_GPIO_SIZE				0x18

REG32(GPIO_PDOR, 0x0)
	FIELD(GPIO_PDOR, PDO, 0, 32)
REG32(GPIO_PSOR, 0x4)
	FIELD(GPIO_PSOR, PTSO, 0, 32)
REG32(GPIO_PCOR, 0x8)
	FIELD(GPIO_PCOR, PTCO, 0, 32)
REG32(GPIO_PTOR, 0xc)
	FIELD(GPIO_PTOR, PTTO, 0, 32)
REG32(GPIO_PDIR, 0x10)
	FIELD(GPIO_PDIR, PDI, 0, 32)
REG32(GPIO_PDDR, 0x14)
	FIELD(GPIO_PDDR, PDD, 0, 32)


static void frdmkl25z_gpio_register_reset(FRDMKL25ZGPIO *t) {
	t->PDOR = 0x0;
	t->PSOR = 0x0;
	t->PCOR = 0x0;
	t->PTOR = 0x0;
	t->PDIR = 0x0;
	t->PDDR = 0x0;

}

static uint64_t frdmkl25z_gpio_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMKL25ZGPIO *t = FRDMKL25Z_GPIO(opaque);
	uint64_t ret;

	switch (offset) {
		case A_GPIO_PDOR:
			ret = t->PDOR;
			break;
		case A_GPIO_PDIR:
			ret = t->PDIR;
			break;
		case A_GPIO_PDDR:
			ret = t->PDDR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMKL25Z GPIO read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmkl25z_gpio_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMKL25ZGPIO *t = FRDMKL25Z_GPIO(opaque);


	switch (offset) {
		case A_GPIO_PDOR ... A_GPIO_PDOR + 3:
			value = value << ((offset - A_GPIO_PDOR) << 3);
			offset = A_GPIO_PDOR;
			break;
		case A_GPIO_PSOR ... A_GPIO_PSOR + 3:
			value = value << ((offset - A_GPIO_PSOR) << 3);
			offset = A_GPIO_PSOR;
			break;
		case A_GPIO_PCOR ... A_GPIO_PCOR + 3:
			value = value << ((offset - A_GPIO_PCOR) << 3);
			offset = A_GPIO_PCOR;
			break;
		case A_GPIO_PTOR ... A_GPIO_PTOR + 3:
			value = value << ((offset - A_GPIO_PTOR) << 3);
			offset = A_GPIO_PTOR;
			break;
		case A_GPIO_PDDR ... A_GPIO_PDDR + 3:
			value = value << ((offset - A_GPIO_PDDR) << 3);
			offset = A_GPIO_PDDR;
			break;

		default: break;
	}

	switch (offset) {
		case A_GPIO_PDOR:
			t->PDOR = value;
			break;
		case A_GPIO_PSOR:
			t->PSOR = value;
			break;
		case A_GPIO_PCOR:
			t->PCOR = value;
			break;
		case A_GPIO_PTOR:
			t->PTOR = value;
			break;
		case A_GPIO_PDDR:
			t->PDDR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMKL25Z GPIO write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmkl25z_gpio_ops = {
	.read = frdmkl25z_gpio_read,
	.write = frdmkl25z_gpio_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmkl25z_gpio_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMKL25ZGPIO *t = FRDMKL25Z_GPIO(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmkl25z_gpio_ops, t, "frdmkl25z-gpio", FRDMKL25Z_GPIO_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void frdmkl25z_gpio_realize(DeviceState *dev, Error **errp) {
	return;

}

static void frdmkl25z_gpio_reset_enter(Object *obj, ResetType type) {
	FRDMKL25ZGPIO *t = FRDMKL25Z_GPIO(obj);
	frdmkl25z_gpio_register_reset(t);
}

static const VMStateDescription frdmkl25z_gpio_vmstate = {
	.name = "frdmkl25z-gpio",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(PDOR, FRDMKL25ZGPIO),
		VMSTATE_UINT32(PSOR, FRDMKL25ZGPIO),
		VMSTATE_UINT32(PCOR, FRDMKL25ZGPIO),
		VMSTATE_UINT32(PTOR, FRDMKL25ZGPIO),
		VMSTATE_UINT32(PDIR, FRDMKL25ZGPIO),
		VMSTATE_UINT32(PDDR, FRDMKL25ZGPIO),

		VMSTATE_END_OF_LIST()
	}
};

static void frdmkl25z_gpio_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmkl25z_gpio_vmstate;
	dc->realize = frdmkl25z_gpio_realize;
	rc->phases.enter = frdmkl25z_gpio_reset_enter;

}

static const TypeInfo frdmkl25z_gpio_info = {
	.name = TYPE_FRDMKL25Z_GPIO,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMKL25ZGPIO),
	.instance_init = frdmkl25z_gpio_init,
	.class_init = frdmkl25z_gpio_class_init,
};

static void frdmkl25z_gpio_register_types(void) {
	type_register_static(&frdmkl25z_gpio_info);
}

type_init(frdmkl25z_gpio_register_types);

#define TYPE_FRDMKL25Z_PORT "frdmkl25z-port"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMKL25ZPORT, FRDMKL25Z_PORT)

struct FRDMKL25ZPORT {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t PCR0;
	uint32_t PCR1;
	uint32_t PCR2;
	uint32_t PCR3;
	uint32_t PCR4;
	uint32_t PCR5;
	uint32_t PCR6;
	uint32_t PCR7;
	uint32_t PCR8;
	uint32_t PCR9;
	uint32_t PCR10;
	uint32_t PCR11;
	uint32_t PCR12;
	uint32_t PCR13;
	uint32_t PCR14;
	uint32_t PCR15;
	uint32_t PCR16;
	uint32_t PCR17;
	uint32_t PCR18;
	uint32_t PCR19;
	uint32_t PCR20;
	uint32_t PCR21;
	uint32_t PCR22;
	uint32_t PCR23;
	uint32_t PCR24;
	uint32_t PCR25;
	uint32_t PCR26;
	uint32_t PCR27;
	uint32_t PCR28;
	uint32_t PCR29;
	uint32_t PCR30;
	uint32_t PCR31;
	uint32_t GPCLR;
	uint32_t GPCHR;
	uint32_t ISFR;
	/* chardev backend */
	CharBackend chr;
	/* base */
	uint32_t base;

};
#define FRDMKL25Z_PORT_SIZE				0xa4

REG32(PORT_PCR0, 0x0)
	FIELD(PORT_PCR0, PS, 0, 1)
	FIELD(PORT_PCR0, PE, 1, 1)
	FIELD(PORT_PCR0, SRE, 2, 1)
	FIELD(PORT_PCR0, PFE, 4, 1)
	FIELD(PORT_PCR0, DSE, 6, 1)
	FIELD(PORT_PCR0, MUX, 8, 3)
	FIELD(PORT_PCR0, IRQC, 16, 4)
	FIELD(PORT_PCR0, ISF, 24, 1)
REG32(PORT_PCR1, 0x4)
	FIELD(PORT_PCR1, PS, 0, 1)
	FIELD(PORT_PCR1, PE, 1, 1)
	FIELD(PORT_PCR1, SRE, 2, 1)
	FIELD(PORT_PCR1, PFE, 4, 1)
	FIELD(PORT_PCR1, DSE, 6, 1)
	FIELD(PORT_PCR1, MUX, 8, 3)
	FIELD(PORT_PCR1, IRQC, 16, 4)
	FIELD(PORT_PCR1, ISF, 24, 1)
REG32(PORT_PCR2, 0x8)
	FIELD(PORT_PCR2, PS, 0, 1)
	FIELD(PORT_PCR2, PE, 1, 1)
	FIELD(PORT_PCR2, SRE, 2, 1)
	FIELD(PORT_PCR2, PFE, 4, 1)
	FIELD(PORT_PCR2, DSE, 6, 1)
	FIELD(PORT_PCR2, MUX, 8, 3)
	FIELD(PORT_PCR2, IRQC, 16, 4)
	FIELD(PORT_PCR2, ISF, 24, 1)
REG32(PORT_PCR3, 0xc)
	FIELD(PORT_PCR3, PS, 0, 1)
	FIELD(PORT_PCR3, PE, 1, 1)
	FIELD(PORT_PCR3, SRE, 2, 1)
	FIELD(PORT_PCR3, PFE, 4, 1)
	FIELD(PORT_PCR3, DSE, 6, 1)
	FIELD(PORT_PCR3, MUX, 8, 3)
	FIELD(PORT_PCR3, IRQC, 16, 4)
	FIELD(PORT_PCR3, ISF, 24, 1)
REG32(PORT_PCR4, 0x10)
	FIELD(PORT_PCR4, PS, 0, 1)
	FIELD(PORT_PCR4, PE, 1, 1)
	FIELD(PORT_PCR4, SRE, 2, 1)
	FIELD(PORT_PCR4, PFE, 4, 1)
	FIELD(PORT_PCR4, DSE, 6, 1)
	FIELD(PORT_PCR4, MUX, 8, 3)
	FIELD(PORT_PCR4, IRQC, 16, 4)
	FIELD(PORT_PCR4, ISF, 24, 1)
REG32(PORT_PCR5, 0x14)
	FIELD(PORT_PCR5, PS, 0, 1)
	FIELD(PORT_PCR5, PE, 1, 1)
	FIELD(PORT_PCR5, SRE, 2, 1)
	FIELD(PORT_PCR5, PFE, 4, 1)
	FIELD(PORT_PCR5, DSE, 6, 1)
	FIELD(PORT_PCR5, MUX, 8, 3)
	FIELD(PORT_PCR5, IRQC, 16, 4)
	FIELD(PORT_PCR5, ISF, 24, 1)
REG32(PORT_PCR6, 0x18)
	FIELD(PORT_PCR6, PS, 0, 1)
	FIELD(PORT_PCR6, PE, 1, 1)
	FIELD(PORT_PCR6, SRE, 2, 1)
	FIELD(PORT_PCR6, PFE, 4, 1)
	FIELD(PORT_PCR6, DSE, 6, 1)
	FIELD(PORT_PCR6, MUX, 8, 3)
	FIELD(PORT_PCR6, IRQC, 16, 4)
	FIELD(PORT_PCR6, ISF, 24, 1)
REG32(PORT_PCR7, 0x1c)
	FIELD(PORT_PCR7, PS, 0, 1)
	FIELD(PORT_PCR7, PE, 1, 1)
	FIELD(PORT_PCR7, SRE, 2, 1)
	FIELD(PORT_PCR7, PFE, 4, 1)
	FIELD(PORT_PCR7, DSE, 6, 1)
	FIELD(PORT_PCR7, MUX, 8, 3)
	FIELD(PORT_PCR7, IRQC, 16, 4)
	FIELD(PORT_PCR7, ISF, 24, 1)
REG32(PORT_PCR8, 0x20)
	FIELD(PORT_PCR8, PS, 0, 1)
	FIELD(PORT_PCR8, PE, 1, 1)
	FIELD(PORT_PCR8, SRE, 2, 1)
	FIELD(PORT_PCR8, PFE, 4, 1)
	FIELD(PORT_PCR8, DSE, 6, 1)
	FIELD(PORT_PCR8, MUX, 8, 3)
	FIELD(PORT_PCR8, IRQC, 16, 4)
	FIELD(PORT_PCR8, ISF, 24, 1)
REG32(PORT_PCR9, 0x24)
	FIELD(PORT_PCR9, PS, 0, 1)
	FIELD(PORT_PCR9, PE, 1, 1)
	FIELD(PORT_PCR9, SRE, 2, 1)
	FIELD(PORT_PCR9, PFE, 4, 1)
	FIELD(PORT_PCR9, DSE, 6, 1)
	FIELD(PORT_PCR9, MUX, 8, 3)
	FIELD(PORT_PCR9, IRQC, 16, 4)
	FIELD(PORT_PCR9, ISF, 24, 1)
REG32(PORT_PCR10, 0x28)
	FIELD(PORT_PCR10, PS, 0, 1)
	FIELD(PORT_PCR10, PE, 1, 1)
	FIELD(PORT_PCR10, SRE, 2, 1)
	FIELD(PORT_PCR10, PFE, 4, 1)
	FIELD(PORT_PCR10, DSE, 6, 1)
	FIELD(PORT_PCR10, MUX, 8, 3)
	FIELD(PORT_PCR10, IRQC, 16, 4)
	FIELD(PORT_PCR10, ISF, 24, 1)
REG32(PORT_PCR11, 0x2c)
	FIELD(PORT_PCR11, PS, 0, 1)
	FIELD(PORT_PCR11, PE, 1, 1)
	FIELD(PORT_PCR11, SRE, 2, 1)
	FIELD(PORT_PCR11, PFE, 4, 1)
	FIELD(PORT_PCR11, DSE, 6, 1)
	FIELD(PORT_PCR11, MUX, 8, 3)
	FIELD(PORT_PCR11, IRQC, 16, 4)
	FIELD(PORT_PCR11, ISF, 24, 1)
REG32(PORT_PCR12, 0x30)
	FIELD(PORT_PCR12, PS, 0, 1)
	FIELD(PORT_PCR12, PE, 1, 1)
	FIELD(PORT_PCR12, SRE, 2, 1)
	FIELD(PORT_PCR12, PFE, 4, 1)
	FIELD(PORT_PCR12, DSE, 6, 1)
	FIELD(PORT_PCR12, MUX, 8, 3)
	FIELD(PORT_PCR12, IRQC, 16, 4)
	FIELD(PORT_PCR12, ISF, 24, 1)
REG32(PORT_PCR13, 0x34)
	FIELD(PORT_PCR13, PS, 0, 1)
	FIELD(PORT_PCR13, PE, 1, 1)
	FIELD(PORT_PCR13, SRE, 2, 1)
	FIELD(PORT_PCR13, PFE, 4, 1)
	FIELD(PORT_PCR13, DSE, 6, 1)
	FIELD(PORT_PCR13, MUX, 8, 3)
	FIELD(PORT_PCR13, IRQC, 16, 4)
	FIELD(PORT_PCR13, ISF, 24, 1)
REG32(PORT_PCR14, 0x38)
	FIELD(PORT_PCR14, PS, 0, 1)
	FIELD(PORT_PCR14, PE, 1, 1)
	FIELD(PORT_PCR14, SRE, 2, 1)
	FIELD(PORT_PCR14, PFE, 4, 1)
	FIELD(PORT_PCR14, DSE, 6, 1)
	FIELD(PORT_PCR14, MUX, 8, 3)
	FIELD(PORT_PCR14, IRQC, 16, 4)
	FIELD(PORT_PCR14, ISF, 24, 1)
REG32(PORT_PCR15, 0x3c)
	FIELD(PORT_PCR15, PS, 0, 1)
	FIELD(PORT_PCR15, PE, 1, 1)
	FIELD(PORT_PCR15, SRE, 2, 1)
	FIELD(PORT_PCR15, PFE, 4, 1)
	FIELD(PORT_PCR15, DSE, 6, 1)
	FIELD(PORT_PCR15, MUX, 8, 3)
	FIELD(PORT_PCR15, IRQC, 16, 4)
	FIELD(PORT_PCR15, ISF, 24, 1)
REG32(PORT_PCR16, 0x40)
	FIELD(PORT_PCR16, PS, 0, 1)
	FIELD(PORT_PCR16, PE, 1, 1)
	FIELD(PORT_PCR16, SRE, 2, 1)
	FIELD(PORT_PCR16, PFE, 4, 1)
	FIELD(PORT_PCR16, DSE, 6, 1)
	FIELD(PORT_PCR16, MUX, 8, 3)
	FIELD(PORT_PCR16, IRQC, 16, 4)
	FIELD(PORT_PCR16, ISF, 24, 1)
REG32(PORT_PCR17, 0x44)
	FIELD(PORT_PCR17, PS, 0, 1)
	FIELD(PORT_PCR17, PE, 1, 1)
	FIELD(PORT_PCR17, SRE, 2, 1)
	FIELD(PORT_PCR17, PFE, 4, 1)
	FIELD(PORT_PCR17, DSE, 6, 1)
	FIELD(PORT_PCR17, MUX, 8, 3)
	FIELD(PORT_PCR17, IRQC, 16, 4)
	FIELD(PORT_PCR17, ISF, 24, 1)
REG32(PORT_PCR18, 0x48)
	FIELD(PORT_PCR18, PS, 0, 1)
	FIELD(PORT_PCR18, PE, 1, 1)
	FIELD(PORT_PCR18, SRE, 2, 1)
	FIELD(PORT_PCR18, PFE, 4, 1)
	FIELD(PORT_PCR18, DSE, 6, 1)
	FIELD(PORT_PCR18, MUX, 8, 3)
	FIELD(PORT_PCR18, IRQC, 16, 4)
	FIELD(PORT_PCR18, ISF, 24, 1)
REG32(PORT_PCR19, 0x4c)
	FIELD(PORT_PCR19, PS, 0, 1)
	FIELD(PORT_PCR19, PE, 1, 1)
	FIELD(PORT_PCR19, SRE, 2, 1)
	FIELD(PORT_PCR19, PFE, 4, 1)
	FIELD(PORT_PCR19, DSE, 6, 1)
	FIELD(PORT_PCR19, MUX, 8, 3)
	FIELD(PORT_PCR19, IRQC, 16, 4)
	FIELD(PORT_PCR19, ISF, 24, 1)
REG32(PORT_PCR20, 0x50)
	FIELD(PORT_PCR20, PS, 0, 1)
	FIELD(PORT_PCR20, PE, 1, 1)
	FIELD(PORT_PCR20, SRE, 2, 1)
	FIELD(PORT_PCR20, PFE, 4, 1)
	FIELD(PORT_PCR20, DSE, 6, 1)
	FIELD(PORT_PCR20, MUX, 8, 3)
	FIELD(PORT_PCR20, IRQC, 16, 4)
	FIELD(PORT_PCR20, ISF, 24, 1)
REG32(PORT_PCR21, 0x54)
	FIELD(PORT_PCR21, PS, 0, 1)
	FIELD(PORT_PCR21, PE, 1, 1)
	FIELD(PORT_PCR21, SRE, 2, 1)
	FIELD(PORT_PCR21, PFE, 4, 1)
	FIELD(PORT_PCR21, DSE, 6, 1)
	FIELD(PORT_PCR21, MUX, 8, 3)
	FIELD(PORT_PCR21, IRQC, 16, 4)
	FIELD(PORT_PCR21, ISF, 24, 1)
REG32(PORT_PCR22, 0x58)
	FIELD(PORT_PCR22, PS, 0, 1)
	FIELD(PORT_PCR22, PE, 1, 1)
	FIELD(PORT_PCR22, SRE, 2, 1)
	FIELD(PORT_PCR22, PFE, 4, 1)
	FIELD(PORT_PCR22, DSE, 6, 1)
	FIELD(PORT_PCR22, MUX, 8, 3)
	FIELD(PORT_PCR22, IRQC, 16, 4)
	FIELD(PORT_PCR22, ISF, 24, 1)
REG32(PORT_PCR23, 0x5c)
	FIELD(PORT_PCR23, PS, 0, 1)
	FIELD(PORT_PCR23, PE, 1, 1)
	FIELD(PORT_PCR23, SRE, 2, 1)
	FIELD(PORT_PCR23, PFE, 4, 1)
	FIELD(PORT_PCR23, DSE, 6, 1)
	FIELD(PORT_PCR23, MUX, 8, 3)
	FIELD(PORT_PCR23, IRQC, 16, 4)
	FIELD(PORT_PCR23, ISF, 24, 1)
REG32(PORT_PCR24, 0x60)
	FIELD(PORT_PCR24, PS, 0, 1)
	FIELD(PORT_PCR24, PE, 1, 1)
	FIELD(PORT_PCR24, SRE, 2, 1)
	FIELD(PORT_PCR24, PFE, 4, 1)
	FIELD(PORT_PCR24, DSE, 6, 1)
	FIELD(PORT_PCR24, MUX, 8, 3)
	FIELD(PORT_PCR24, IRQC, 16, 4)
	FIELD(PORT_PCR24, ISF, 24, 1)
REG32(PORT_PCR25, 0x64)
	FIELD(PORT_PCR25, PS, 0, 1)
	FIELD(PORT_PCR25, PE, 1, 1)
	FIELD(PORT_PCR25, SRE, 2, 1)
	FIELD(PORT_PCR25, PFE, 4, 1)
	FIELD(PORT_PCR25, DSE, 6, 1)
	FIELD(PORT_PCR25, MUX, 8, 3)
	FIELD(PORT_PCR25, IRQC, 16, 4)
	FIELD(PORT_PCR25, ISF, 24, 1)
REG32(PORT_PCR26, 0x68)
	FIELD(PORT_PCR26, PS, 0, 1)
	FIELD(PORT_PCR26, PE, 1, 1)
	FIELD(PORT_PCR26, SRE, 2, 1)
	FIELD(PORT_PCR26, PFE, 4, 1)
	FIELD(PORT_PCR26, DSE, 6, 1)
	FIELD(PORT_PCR26, MUX, 8, 3)
	FIELD(PORT_PCR26, IRQC, 16, 4)
	FIELD(PORT_PCR26, ISF, 24, 1)
REG32(PORT_PCR27, 0x6c)
	FIELD(PORT_PCR27, PS, 0, 1)
	FIELD(PORT_PCR27, PE, 1, 1)
	FIELD(PORT_PCR27, SRE, 2, 1)
	FIELD(PORT_PCR27, PFE, 4, 1)
	FIELD(PORT_PCR27, DSE, 6, 1)
	FIELD(PORT_PCR27, MUX, 8, 3)
	FIELD(PORT_PCR27, IRQC, 16, 4)
	FIELD(PORT_PCR27, ISF, 24, 1)
REG32(PORT_PCR28, 0x70)
	FIELD(PORT_PCR28, PS, 0, 1)
	FIELD(PORT_PCR28, PE, 1, 1)
	FIELD(PORT_PCR28, SRE, 2, 1)
	FIELD(PORT_PCR28, PFE, 4, 1)
	FIELD(PORT_PCR28, DSE, 6, 1)
	FIELD(PORT_PCR28, MUX, 8, 3)
	FIELD(PORT_PCR28, IRQC, 16, 4)
	FIELD(PORT_PCR28, ISF, 24, 1)
REG32(PORT_PCR29, 0x74)
	FIELD(PORT_PCR29, PS, 0, 1)
	FIELD(PORT_PCR29, PE, 1, 1)
	FIELD(PORT_PCR29, SRE, 2, 1)
	FIELD(PORT_PCR29, PFE, 4, 1)
	FIELD(PORT_PCR29, DSE, 6, 1)
	FIELD(PORT_PCR29, MUX, 8, 3)
	FIELD(PORT_PCR29, IRQC, 16, 4)
	FIELD(PORT_PCR29, ISF, 24, 1)
REG32(PORT_PCR30, 0x78)
	FIELD(PORT_PCR30, PS, 0, 1)
	FIELD(PORT_PCR30, PE, 1, 1)
	FIELD(PORT_PCR30, SRE, 2, 1)
	FIELD(PORT_PCR30, PFE, 4, 1)
	FIELD(PORT_PCR30, DSE, 6, 1)
	FIELD(PORT_PCR30, MUX, 8, 3)
	FIELD(PORT_PCR30, IRQC, 16, 4)
	FIELD(PORT_PCR30, ISF, 24, 1)
REG32(PORT_PCR31, 0x7c)
	FIELD(PORT_PCR31, PS, 0, 1)
	FIELD(PORT_PCR31, PE, 1, 1)
	FIELD(PORT_PCR31, SRE, 2, 1)
	FIELD(PORT_PCR31, PFE, 4, 1)
	FIELD(PORT_PCR31, DSE, 6, 1)
	FIELD(PORT_PCR31, MUX, 8, 3)
	FIELD(PORT_PCR31, IRQC, 16, 4)
	FIELD(PORT_PCR31, ISF, 24, 1)
REG32(PORT_GPCLR, 0x80)
	FIELD(PORT_GPCLR, GPWD, 0, 16)
	FIELD(PORT_GPCLR, GPWE, 16, 16)
REG32(PORT_GPCHR, 0x84)
	FIELD(PORT_GPCHR, GPWD, 0, 16)
	FIELD(PORT_GPCHR, GPWE, 16, 16)
REG32(PORT_ISFR, 0xa0)
	FIELD(PORT_ISFR, ISF, 0, 32)


static int frdmkl25z_port_can_receive(void *opaque) {
	return 1;
}

static void frdmkl25z_port_receive(void *opaque, const uint8_t *buf, int size) {
	FRDMKL25ZPORT *t = FRDMKL25Z_PORT(opaque);

	(void)t;


}

static void frdmkl25z_port_register_reset(FRDMKL25ZPORT *t) {
	t->PCR0 = 0x706;
	t->PCR1 = 0x5;
	t->PCR2 = 0x5;
	t->PCR3 = 0x703;
	t->PCR4 = 0x707;
	t->PCR5 = 0x5;
	t->PCR6 = 0x5;
	t->PCR7 = 0x5;
	t->PCR8 = 0x0;
	t->PCR9 = 0x0;
	t->PCR10 = 0x0;
	t->PCR11 = 0x0;
	t->PCR12 = 0x5;
	t->PCR13 = 0x5;
	t->PCR14 = 0x1;
	t->PCR15 = 0x1;
	t->PCR16 = 0x1;
	t->PCR17 = 0x1;
	t->PCR18 = 0x5;
	t->PCR19 = 0x5;
	t->PCR20 = 0x17;
	t->PCR21 = 0x0;
	t->PCR22 = 0x0;
	t->PCR23 = 0x0;
	t->PCR24 = 0x0;
	t->PCR25 = 0x0;
	t->PCR26 = 0x0;
	t->PCR27 = 0x0;
	t->PCR28 = 0x0;
	t->PCR29 = 0x0;
	t->PCR30 = 0x0;
	t->PCR31 = 0x0;
	t->GPCLR = 0x0;
	t->GPCHR = 0x0;
	t->ISFR = 0x0;

}

static uint64_t frdmkl25z_port_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMKL25ZPORT *t = FRDMKL25Z_PORT(opaque);
	uint64_t ret;

	switch (offset) {
		case A_PORT_PCR0:
			ret = t->PCR0;
			break;
		case A_PORT_PCR1:
			ret = t->PCR1;
			break;
		case A_PORT_PCR2:
			ret = t->PCR2;
			break;
		case A_PORT_PCR3:
			ret = t->PCR3;
			break;
		case A_PORT_PCR4:
			ret = t->PCR4;
			break;
		case A_PORT_PCR5:
			ret = t->PCR5;
			break;
		case A_PORT_PCR6:
			ret = t->PCR6;
			break;
		case A_PORT_PCR7:
			ret = t->PCR7;
			break;
		case A_PORT_PCR8:
			ret = t->PCR8;
			break;
		case A_PORT_PCR9:
			ret = t->PCR9;
			break;
		case A_PORT_PCR10:
			ret = t->PCR10;
			break;
		case A_PORT_PCR11:
			ret = t->PCR11;
			break;
		case A_PORT_PCR12:
			ret = t->PCR12;
			break;
		case A_PORT_PCR13:
			ret = t->PCR13;
			break;
		case A_PORT_PCR14:
			ret = t->PCR14;
			break;
		case A_PORT_PCR15:
			ret = t->PCR15;
			break;
		case A_PORT_PCR16:
			ret = t->PCR16;
			break;
		case A_PORT_PCR17:
			ret = t->PCR17;
			break;
		case A_PORT_PCR18:
			ret = t->PCR18;
			break;
		case A_PORT_PCR19:
			ret = t->PCR19;
			break;
		case A_PORT_PCR20:
			ret = t->PCR20;
			break;
		case A_PORT_PCR21:
			ret = t->PCR21;
			break;
		case A_PORT_PCR22:
			ret = t->PCR22;
			break;
		case A_PORT_PCR23:
			ret = t->PCR23;
			break;
		case A_PORT_PCR24:
			ret = t->PCR24;
			break;
		case A_PORT_PCR25:
			ret = t->PCR25;
			break;
		case A_PORT_PCR26:
			ret = t->PCR26;
			break;
		case A_PORT_PCR27:
			ret = t->PCR27;
			break;
		case A_PORT_PCR28:
			ret = t->PCR28;
			break;
		case A_PORT_PCR29:
			ret = t->PCR29;
			break;
		case A_PORT_PCR30:
			ret = t->PCR30;
			break;
		case A_PORT_PCR31:
			ret = t->PCR31;
			break;
		case A_PORT_ISFR:
			ret = t->ISFR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMKL25Z PORT read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmkl25z_port_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMKL25ZPORT *t = FRDMKL25Z_PORT(opaque);


	switch (offset) {
		case A_PORT_PCR0 ... A_PORT_PCR0 + 3:
			value = value << ((offset - A_PORT_PCR0) << 3);
			offset = A_PORT_PCR0;
			break;
		case A_PORT_PCR1 ... A_PORT_PCR1 + 3:
			value = value << ((offset - A_PORT_PCR1) << 3);
			offset = A_PORT_PCR1;
			break;
		case A_PORT_PCR2 ... A_PORT_PCR2 + 3:
			value = value << ((offset - A_PORT_PCR2) << 3);
			offset = A_PORT_PCR2;
			break;
		case A_PORT_PCR3 ... A_PORT_PCR3 + 3:
			value = value << ((offset - A_PORT_PCR3) << 3);
			offset = A_PORT_PCR3;
			break;
		case A_PORT_PCR4 ... A_PORT_PCR4 + 3:
			value = value << ((offset - A_PORT_PCR4) << 3);
			offset = A_PORT_PCR4;
			break;
		case A_PORT_PCR5 ... A_PORT_PCR5 + 3:
			value = value << ((offset - A_PORT_PCR5) << 3);
			offset = A_PORT_PCR5;
			break;
		case A_PORT_PCR6 ... A_PORT_PCR6 + 3:
			value = value << ((offset - A_PORT_PCR6) << 3);
			offset = A_PORT_PCR6;
			break;
		case A_PORT_PCR7 ... A_PORT_PCR7 + 3:
			value = value << ((offset - A_PORT_PCR7) << 3);
			offset = A_PORT_PCR7;
			break;
		case A_PORT_PCR8 ... A_PORT_PCR8 + 3:
			value = value << ((offset - A_PORT_PCR8) << 3);
			offset = A_PORT_PCR8;
			break;
		case A_PORT_PCR9 ... A_PORT_PCR9 + 3:
			value = value << ((offset - A_PORT_PCR9) << 3);
			offset = A_PORT_PCR9;
			break;
		case A_PORT_PCR10 ... A_PORT_PCR10 + 3:
			value = value << ((offset - A_PORT_PCR10) << 3);
			offset = A_PORT_PCR10;
			break;
		case A_PORT_PCR11 ... A_PORT_PCR11 + 3:
			value = value << ((offset - A_PORT_PCR11) << 3);
			offset = A_PORT_PCR11;
			break;
		case A_PORT_PCR12 ... A_PORT_PCR12 + 3:
			value = value << ((offset - A_PORT_PCR12) << 3);
			offset = A_PORT_PCR12;
			break;
		case A_PORT_PCR13 ... A_PORT_PCR13 + 3:
			value = value << ((offset - A_PORT_PCR13) << 3);
			offset = A_PORT_PCR13;
			break;
		case A_PORT_PCR14 ... A_PORT_PCR14 + 3:
			value = value << ((offset - A_PORT_PCR14) << 3);
			offset = A_PORT_PCR14;
			break;
		case A_PORT_PCR15 ... A_PORT_PCR15 + 3:
			value = value << ((offset - A_PORT_PCR15) << 3);
			offset = A_PORT_PCR15;
			break;
		case A_PORT_PCR16 ... A_PORT_PCR16 + 3:
			value = value << ((offset - A_PORT_PCR16) << 3);
			offset = A_PORT_PCR16;
			break;
		case A_PORT_PCR17 ... A_PORT_PCR17 + 3:
			value = value << ((offset - A_PORT_PCR17) << 3);
			offset = A_PORT_PCR17;
			break;
		case A_PORT_PCR18 ... A_PORT_PCR18 + 3:
			value = value << ((offset - A_PORT_PCR18) << 3);
			offset = A_PORT_PCR18;
			break;
		case A_PORT_PCR19 ... A_PORT_PCR19 + 3:
			value = value << ((offset - A_PORT_PCR19) << 3);
			offset = A_PORT_PCR19;
			break;
		case A_PORT_PCR20 ... A_PORT_PCR20 + 3:
			value = value << ((offset - A_PORT_PCR20) << 3);
			offset = A_PORT_PCR20;
			break;
		case A_PORT_PCR21 ... A_PORT_PCR21 + 3:
			value = value << ((offset - A_PORT_PCR21) << 3);
			offset = A_PORT_PCR21;
			break;
		case A_PORT_PCR22 ... A_PORT_PCR22 + 3:
			value = value << ((offset - A_PORT_PCR22) << 3);
			offset = A_PORT_PCR22;
			break;
		case A_PORT_PCR23 ... A_PORT_PCR23 + 3:
			value = value << ((offset - A_PORT_PCR23) << 3);
			offset = A_PORT_PCR23;
			break;
		case A_PORT_PCR24 ... A_PORT_PCR24 + 3:
			value = value << ((offset - A_PORT_PCR24) << 3);
			offset = A_PORT_PCR24;
			break;
		case A_PORT_PCR25 ... A_PORT_PCR25 + 3:
			value = value << ((offset - A_PORT_PCR25) << 3);
			offset = A_PORT_PCR25;
			break;
		case A_PORT_PCR26 ... A_PORT_PCR26 + 3:
			value = value << ((offset - A_PORT_PCR26) << 3);
			offset = A_PORT_PCR26;
			break;
		case A_PORT_PCR27 ... A_PORT_PCR27 + 3:
			value = value << ((offset - A_PORT_PCR27) << 3);
			offset = A_PORT_PCR27;
			break;
		case A_PORT_PCR28 ... A_PORT_PCR28 + 3:
			value = value << ((offset - A_PORT_PCR28) << 3);
			offset = A_PORT_PCR28;
			break;
		case A_PORT_PCR29 ... A_PORT_PCR29 + 3:
			value = value << ((offset - A_PORT_PCR29) << 3);
			offset = A_PORT_PCR29;
			break;
		case A_PORT_PCR30 ... A_PORT_PCR30 + 3:
			value = value << ((offset - A_PORT_PCR30) << 3);
			offset = A_PORT_PCR30;
			break;
		case A_PORT_PCR31 ... A_PORT_PCR31 + 3:
			value = value << ((offset - A_PORT_PCR31) << 3);
			offset = A_PORT_PCR31;
			break;
		case A_PORT_GPCLR ... A_PORT_GPCLR + 3:
			value = value << ((offset - A_PORT_GPCLR) << 3);
			offset = A_PORT_GPCLR;
			break;
		case A_PORT_GPCHR ... A_PORT_GPCHR + 3:
			value = value << ((offset - A_PORT_GPCHR) << 3);
			offset = A_PORT_GPCHR;
			break;
		case A_PORT_ISFR ... A_PORT_ISFR + 3:
			value = value << ((offset - A_PORT_ISFR) << 3);
			offset = A_PORT_ISFR;
			break;

		default: break;
	}

	switch (offset) {
		case A_PORT_PCR0:
			t->PCR0 = value;
			break;
		case A_PORT_PCR1:
			t->PCR1 = value;
			break;
		case A_PORT_PCR2:
			t->PCR2 = value;
			break;
		case A_PORT_PCR3:
			t->PCR3 = value;
			break;
		case A_PORT_PCR4:
			t->PCR4 = value;
			break;
		case A_PORT_PCR5:
			t->PCR5 = value;
			break;
		case A_PORT_PCR6:
			t->PCR6 = value;
			break;
		case A_PORT_PCR7:
			t->PCR7 = value;
			break;
		case A_PORT_PCR8:
			t->PCR8 = value;
			break;
		case A_PORT_PCR9:
			t->PCR9 = value;
			break;
		case A_PORT_PCR10:
			t->PCR10 = value;
			break;
		case A_PORT_PCR11:
			t->PCR11 = value;
			break;
		case A_PORT_PCR12:
			t->PCR12 = value;
			break;
		case A_PORT_PCR13:
			t->PCR13 = value;
			break;
		case A_PORT_PCR14:
			t->PCR14 = value;
			break;
		case A_PORT_PCR15:
			t->PCR15 = value;
			break;
		case A_PORT_PCR16:
			t->PCR16 = value;
			break;
		case A_PORT_PCR17:
			t->PCR17 = value;
			break;
		case A_PORT_PCR18:
			t->PCR18 = value;
			break;
		case A_PORT_PCR19:
			t->PCR19 = value;
			break;
		case A_PORT_PCR20:
			t->PCR20 = value;
			break;
		case A_PORT_PCR21:
			t->PCR21 = value;
			break;
		case A_PORT_PCR22:
			t->PCR22 = value;
			break;
		case A_PORT_PCR23:
			t->PCR23 = value;
			break;
		case A_PORT_PCR24:
			t->PCR24 = value;
			break;
		case A_PORT_PCR25:
			t->PCR25 = value;
			break;
		case A_PORT_PCR26:
			t->PCR26 = value;
			break;
		case A_PORT_PCR27:
			t->PCR27 = value;
			break;
		case A_PORT_PCR28:
			t->PCR28 = value;
			break;
		case A_PORT_PCR29:
			t->PCR29 = value;
			break;
		case A_PORT_PCR30:
			t->PCR30 = value;
			break;
		case A_PORT_PCR31:
			t->PCR31 = value;
			break;
		case A_PORT_GPCLR:
			t->GPCLR = value;
			break;
		case A_PORT_GPCHR:
			t->GPCHR = value;
			break;
		case A_PORT_ISFR:
			t->ISFR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMKL25Z PORT write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmkl25z_port_ops = {
	.read = frdmkl25z_port_read,
	.write = frdmkl25z_port_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmkl25z_port_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMKL25ZPORT *t = FRDMKL25Z_PORT(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmkl25z_port_ops, t, "frdmkl25z-port", FRDMKL25Z_PORT_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void frdmkl25z_port_realize(DeviceState *dev, Error **errp) {
	FRDMKL25ZPORT *t = FRDMKL25Z_PORT(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), frdmkl25z_port_can_receive, frdmkl25z_port_receive,
		NULL, NULL, t, NULL, true);

}

static void frdmkl25z_port_reset_enter(Object *obj, ResetType type) {
	FRDMKL25ZPORT *t = FRDMKL25Z_PORT(obj);
	frdmkl25z_port_register_reset(t);
}

static const VMStateDescription frdmkl25z_port_vmstate = {
	.name = "frdmkl25z-port",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(PCR0, FRDMKL25ZPORT),
		VMSTATE_UINT32(PCR1, FRDMKL25ZPORT),
		VMSTATE_UINT32(PCR2, FRDMKL25ZPORT),
		VMSTATE_UINT32(PCR3, FRDMKL25ZPORT),
		VMSTATE_UINT32(PCR4, FRDMKL25ZPORT),
		VMSTATE_UINT32(PCR5, FRDMKL25ZPORT),
		VMSTATE_UINT32(PCR6, FRDMKL25ZPORT),
		VMSTATE_UINT32(PCR7, FRDMKL25ZPORT),
		VMSTATE_UINT32(PCR8, FRDMKL25ZPORT),
		VMSTATE_UINT32(PCR9, FRDMKL25ZPORT),
		VMSTATE_UINT32(PCR10, FRDMKL25ZPORT),
		VMSTATE_UINT32(PCR11, FRDMKL25ZPORT),
		VMSTATE_UINT32(PCR12, FRDMKL25ZPORT),
		VMSTATE_UINT32(PCR13, FRDMKL25ZPORT),
		VMSTATE_UINT32(PCR14, FRDMKL25ZPORT),
		VMSTATE_UINT32(PCR15, FRDMKL25ZPORT),
		VMSTATE_UINT32(PCR16, FRDMKL25ZPORT),
		VMSTATE_UINT32(PCR17, FRDMKL25ZPORT),
		VMSTATE_UINT32(PCR18, FRDMKL25ZPORT),
		VMSTATE_UINT32(PCR19, FRDMKL25ZPORT),
		VMSTATE_UINT32(PCR20, FRDMKL25ZPORT),
		VMSTATE_UINT32(PCR21, FRDMKL25ZPORT),
		VMSTATE_UINT32(PCR22, FRDMKL25ZPORT),
		VMSTATE_UINT32(PCR23, FRDMKL25ZPORT),
		VMSTATE_UINT32(PCR24, FRDMKL25ZPORT),
		VMSTATE_UINT32(PCR25, FRDMKL25ZPORT),
		VMSTATE_UINT32(PCR26, FRDMKL25ZPORT),
		VMSTATE_UINT32(PCR27, FRDMKL25ZPORT),
		VMSTATE_UINT32(PCR28, FRDMKL25ZPORT),
		VMSTATE_UINT32(PCR29, FRDMKL25ZPORT),
		VMSTATE_UINT32(PCR30, FRDMKL25ZPORT),
		VMSTATE_UINT32(PCR31, FRDMKL25ZPORT),
		VMSTATE_UINT32(GPCLR, FRDMKL25ZPORT),
		VMSTATE_UINT32(GPCHR, FRDMKL25ZPORT),
		VMSTATE_UINT32(ISFR, FRDMKL25ZPORT),

		VMSTATE_END_OF_LIST()
	}
};

static Property frdmkl25z_port_properties[] = {
	DEFINE_PROP_CHR("chardev", FRDMKL25ZPORT, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void frdmkl25z_port_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmkl25z_port_vmstate;
	dc->realize = frdmkl25z_port_realize;
	rc->phases.enter = frdmkl25z_port_reset_enter;
	device_class_set_props(dc, frdmkl25z_port_properties);

}

static const TypeInfo frdmkl25z_port_info = {
	.name = TYPE_FRDMKL25Z_PORT,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMKL25ZPORT),
	.instance_init = frdmkl25z_port_init,
	.class_init = frdmkl25z_port_class_init,
};

static void frdmkl25z_port_register_types(void) {
	type_register_static(&frdmkl25z_port_info);
}

type_init(frdmkl25z_port_register_types);

#define TYPE_FRDMKL25Z_PMC "frdmkl25z-pmc"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMKL25ZPMC, FRDMKL25Z_PMC)

struct FRDMKL25ZPMC {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint8_t LVDSC1;
	uint8_t LVDSC2;
	uint8_t REGSC;
	/* base */
	uint32_t base;

};
#define FRDMKL25Z_PMC_SIZE				0x3

REG8(PMC_LVDSC1, 0x0)
	FIELD(PMC_LVDSC1, LVDV, 0, 2)
	FIELD(PMC_LVDSC1, LVDRE, 4, 1)
	FIELD(PMC_LVDSC1, LVDIE, 5, 1)
	FIELD(PMC_LVDSC1, LVDACK, 6, 1)
	FIELD(PMC_LVDSC1, LVDF, 7, 1)
REG8(PMC_LVDSC2, 0x1)
	FIELD(PMC_LVDSC2, LVWV, 0, 2)
	FIELD(PMC_LVDSC2, LVWIE, 5, 1)
	FIELD(PMC_LVDSC2, LVWACK, 6, 1)
	FIELD(PMC_LVDSC2, LVWF, 7, 1)
REG8(PMC_REGSC, 0x2)
	FIELD(PMC_REGSC, BGBE, 0, 1)
	FIELD(PMC_REGSC, REGONS, 2, 1)
	FIELD(PMC_REGSC, ACKISO, 3, 1)
	FIELD(PMC_REGSC, BGEN, 4, 1)


static void frdmkl25z_pmc_register_reset(FRDMKL25ZPMC *t) {
	t->LVDSC1 = 0x10;
	t->LVDSC2 = 0x0;
	t->REGSC = 0x4;

}

static uint64_t frdmkl25z_pmc_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMKL25ZPMC *t = FRDMKL25Z_PMC(opaque);
	uint64_t ret;

	switch (offset) {
		case A_PMC_LVDSC1:
			ret = t->LVDSC1;
			break;
		case A_PMC_LVDSC2:
			ret = t->LVDSC2;
			break;
		case A_PMC_REGSC:
			ret = t->REGSC;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMKL25Z PMC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmkl25z_pmc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMKL25ZPMC *t = FRDMKL25Z_PMC(opaque);


	switch (offset) {

		default: break;
	}

	switch (offset) {
		case A_PMC_LVDSC1:
			t->LVDSC1 = value;
			break;
		case A_PMC_LVDSC2:
			t->LVDSC2 = value;
			break;
		case A_PMC_REGSC:
			t->REGSC = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMKL25Z PMC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmkl25z_pmc_ops = {
	.read = frdmkl25z_pmc_read,
	.write = frdmkl25z_pmc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmkl25z_pmc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMKL25ZPMC *t = FRDMKL25Z_PMC(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmkl25z_pmc_ops, t, "frdmkl25z-pmc", FRDMKL25Z_PMC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void frdmkl25z_pmc_realize(DeviceState *dev, Error **errp) {
	return;

}

static void frdmkl25z_pmc_reset_enter(Object *obj, ResetType type) {
	FRDMKL25ZPMC *t = FRDMKL25Z_PMC(obj);
	frdmkl25z_pmc_register_reset(t);
}

static const VMStateDescription frdmkl25z_pmc_vmstate = {
	.name = "frdmkl25z-pmc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT8(LVDSC1, FRDMKL25ZPMC),
		VMSTATE_UINT8(LVDSC2, FRDMKL25ZPMC),
		VMSTATE_UINT8(REGSC, FRDMKL25ZPMC),

		VMSTATE_END_OF_LIST()
	}
};

static void frdmkl25z_pmc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmkl25z_pmc_vmstate;
	dc->realize = frdmkl25z_pmc_realize;
	rc->phases.enter = frdmkl25z_pmc_reset_enter;

}

static const TypeInfo frdmkl25z_pmc_info = {
	.name = TYPE_FRDMKL25Z_PMC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMKL25ZPMC),
	.instance_init = frdmkl25z_pmc_init,
	.class_init = frdmkl25z_pmc_class_init,
};

static void frdmkl25z_pmc_register_types(void) {
	type_register_static(&frdmkl25z_pmc_info);
}

type_init(frdmkl25z_pmc_register_types);

#define TYPE_FRDMKL25Z_OSC0 "frdmkl25z-osc0"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMKL25ZOSC0, FRDMKL25Z_OSC0)

struct FRDMKL25ZOSC0 {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/*registers*/
	uint8_t CR;
	/* base */
	uint32_t base;

};
#define FRDMKL25Z_OSC0_SIZE				0x1

REG8(OSC0_CR, 0x0)
	FIELD(OSC0_CR, SC16P, 0, 1)
	FIELD(OSC0_CR, SC8P, 1, 1)
	FIELD(OSC0_CR, SC4P, 2, 1)
	FIELD(OSC0_CR, SC2P, 3, 1)
	FIELD(OSC0_CR, EREFSTEN, 5, 1)
	FIELD(OSC0_CR, ERCLKEN, 7, 1)


static void frdmkl25z_osc0_register_reset(FRDMKL25ZOSC0 *t) {
	t->CR = 0x0;

}

static uint64_t frdmkl25z_osc0_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMKL25ZOSC0 *t = FRDMKL25Z_OSC0(opaque);
	uint64_t ret;

	switch (offset) {
		case A_OSC0_CR:
			ret = t->CR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMKL25Z OSC0 read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmkl25z_osc0_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMKL25ZOSC0 *t = FRDMKL25Z_OSC0(opaque);


	switch (offset) {

		default: break;
	}

	switch (offset) {
		case A_OSC0_CR:
			t->CR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMKL25Z OSC0 write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmkl25z_osc0_ops = {
	.read = frdmkl25z_osc0_read,
	.write = frdmkl25z_osc0_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmkl25z_osc0_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMKL25ZOSC0 *t = FRDMKL25Z_OSC0(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmkl25z_osc0_ops, t, "frdmkl25z-osc0", FRDMKL25Z_OSC0_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void frdmkl25z_osc0_realize(DeviceState *dev, Error **errp) {
	return;

}

static void frdmkl25z_osc0_reset_enter(Object *obj, ResetType type) {
	FRDMKL25ZOSC0 *t = FRDMKL25Z_OSC0(obj);
	frdmkl25z_osc0_register_reset(t);
}

static const VMStateDescription frdmkl25z_osc0_vmstate = {
	.name = "frdmkl25z-osc0",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT8(CR, FRDMKL25ZOSC0),

		VMSTATE_END_OF_LIST()
	}
};

static void frdmkl25z_osc0_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmkl25z_osc0_vmstate;
	dc->realize = frdmkl25z_osc0_realize;
	rc->phases.enter = frdmkl25z_osc0_reset_enter;

}

static const TypeInfo frdmkl25z_osc0_info = {
	.name = TYPE_FRDMKL25Z_OSC0,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMKL25ZOSC0),
	.instance_init = frdmkl25z_osc0_init,
	.class_init = frdmkl25z_osc0_class_init,
};

static void frdmkl25z_osc0_register_types(void) {
	type_register_static(&frdmkl25z_osc0_info);
}

type_init(frdmkl25z_osc0_register_types);

#define TYPE_FRDMKL25Z_MCG "frdmkl25z-mcg"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMKL25ZMCG, FRDMKL25Z_MCG)

struct FRDMKL25ZMCG {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint8_t C1;
	uint8_t C2;
	uint8_t C3;
	uint8_t C4;
	uint8_t C5;
	uint8_t C6;
	uint8_t S;
	uint8_t SC;
	uint8_t ATCVH;
	uint8_t ATCVL;
	uint8_t C7;
	uint8_t C8;
	uint8_t C9;
	uint8_t C10;
	/* base */
	uint32_t base;

};
#define FRDMKL25Z_MCG_SIZE				0x10

REG8(MCG_C1, 0x0)
	FIELD(MCG_C1, IREFSTEN, 0, 1)
	FIELD(MCG_C1, IRCLKEN, 1, 1)
	FIELD(MCG_C1, IREFS, 2, 1)
	FIELD(MCG_C1, FRDIV, 3, 3)
	FIELD(MCG_C1, CLKS, 6, 2)
REG8(MCG_C2, 0x1)
	FIELD(MCG_C2, IRCS, 0, 1)
	FIELD(MCG_C2, LP, 1, 1)
	FIELD(MCG_C2, EREFS0, 2, 1)
	FIELD(MCG_C2, HGO0, 3, 1)
	FIELD(MCG_C2, RANGE0, 4, 2)
	FIELD(MCG_C2, LOCRE0, 7, 1)
REG8(MCG_C3, 0x2)
	FIELD(MCG_C3, SCTRIM, 0, 8)
REG8(MCG_C4, 0x3)
	FIELD(MCG_C4, SCFTRIM, 0, 1)
	FIELD(MCG_C4, FCTRIM, 1, 4)
	FIELD(MCG_C4, DRST_DRS, 5, 2)
	FIELD(MCG_C4, DMX32, 7, 1)
REG8(MCG_C5, 0x4)
	FIELD(MCG_C5, PRDIV0, 0, 5)
	FIELD(MCG_C5, PLLSTEN0, 5, 1)
	FIELD(MCG_C5, PLLCLKEN0, 6, 1)
REG8(MCG_C6, 0x5)
	FIELD(MCG_C6, VDIV0, 0, 5)
	FIELD(MCG_C6, CME0, 5, 1)
	FIELD(MCG_C6, PLLS, 6, 1)
	FIELD(MCG_C6, LOLIE0, 7, 1)
REG8(MCG_S, 0x6)
	FIELD(MCG_S, IRCST, 0, 1)
	FIELD(MCG_S, OSCINIT0, 1, 1)
	FIELD(MCG_S, CLKST, 2, 2)
	FIELD(MCG_S, IREFST, 4, 1)
	FIELD(MCG_S, PLLST, 5, 1)
	FIELD(MCG_S, LOCK0, 6, 1)
	FIELD(MCG_S, LOLS0, 7, 1)
REG8(MCG_SC, 0x8)
	FIELD(MCG_SC, LOCS0, 0, 1)
	FIELD(MCG_SC, FCRDIV, 1, 3)
	FIELD(MCG_SC, FLTPRSRV, 4, 1)
	FIELD(MCG_SC, ATMF, 5, 1)
	FIELD(MCG_SC, ATMS, 6, 1)
	FIELD(MCG_SC, ATME, 7, 1)
REG8(MCG_ATCVH, 0xa)
	FIELD(MCG_ATCVH, ATCVH, 0, 8)
REG8(MCG_ATCVL, 0xb)
	FIELD(MCG_ATCVL, ATCVL, 0, 8)
REG8(MCG_C7, 0xc)
REG8(MCG_C8, 0xd)
	FIELD(MCG_C8, LOLRE, 6, 1)
REG8(MCG_C9, 0xe)
REG8(MCG_C10, 0xf)


static void frdmkl25z_mcg_register_reset(FRDMKL25ZMCG *t) {
	t->C1 = 0x4;
	t->C2 = 0x80;
	t->C3 = 0x0;
	t->C4 = 0x0;
	t->C5 = 0x0;
	t->C6 = 0x0;
	t->S = 0x10;
	t->SC = 0x2;
	t->ATCVH = 0x0;
	t->ATCVL = 0x0;
	t->C7 = 0x0;
	t->C8 = 0x80;
	t->C9 = 0x0;
	t->C10 = 0x0;

}

static uint64_t frdmkl25z_mcg_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMKL25ZMCG *t = FRDMKL25Z_MCG(opaque);
	uint64_t ret;

	switch (offset) {
		case A_MCG_C1:
			ret = t->C1;
			break;
		case A_MCG_C2:
			ret = t->C2;
			break;
		case A_MCG_C3:
			ret = t->C3;
			break;
		case A_MCG_C4:
			ret = t->C4;
			break;
		case A_MCG_C5:
			ret = t->C5;
			break;
		case A_MCG_C6:
			ret = t->C6;
			break;
		case A_MCG_S:
			ret = t->S;
			break;
		case A_MCG_SC:
			ret = t->SC;
			break;
		case A_MCG_ATCVH:
			ret = t->ATCVH;
			break;
		case A_MCG_ATCVL:
			ret = t->ATCVL;
			break;
		case A_MCG_C7:
			ret = t->C7;
			break;
		case A_MCG_C8:
			ret = t->C8;
			break;
		case A_MCG_C9:
			ret = t->C9;
			break;
		case A_MCG_C10:
			ret = t->C10;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMKL25Z MCG read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmkl25z_mcg_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMKL25ZMCG *t = FRDMKL25Z_MCG(opaque);


	switch (offset) {

		default: break;
	}

	switch (offset) {
		case A_MCG_C1:
			t->C1 = value;
			if (((!(value & 0x4)) && (!(value & 0x40)) && (value & 0x80))) {
				t->S &= (~(0x10));
				t->S &= (~(0x4));
				t->S |= 0x8;
			} else if (((value & 0x4) && (!(value & 0x40)) && (!(value & 0x80)))) {
				t->S |= 0x10;
			} else if (((!(value & 0x40)) && (!(value & 0x80)))) {
				t->S |= 0x4;
				t->S |= 0x8;
			} else if (((!(value & 0x40)) && (value & 0x80))) {
				t->S &= (~(0x4));
				t->S |= 0x8;
			} else if (((value & 0x40) && (!(value & 0x80)))) {
				t->S |= 0x4;
				t->S &= (~(0x8));
			}
			break;
		case A_MCG_C2:
			t->C2 = value;
			if ((value & 0x4)) {
				t->S |= 0x2;
			}
			if ((!(value & 0x1))) {
				t->S &= (~(0x1));
			}
			t->S |= 0x2;
			break;
		case A_MCG_C3:
			t->C3 = value;
			break;
		case A_MCG_C4:
			t->C4 = value;
			t->C4 = value;
			t->C4 = value;
			t->S &= (~(0x4));
			break;
		case A_MCG_C5:
			t->C5 = value;
			if ((value & 0x40)) {
				t->S |= 0x40;
			}
			break;
		case A_MCG_C6:
			t->C6 = value;
			if ((!(value & 0x40))) {
				t->S &= (~(0x20));
			} else if ((value & 0x40)) {
				t->S |= 0x20;
			}
			break;
		case A_MCG_S:
			t->S = value;
			break;
		case A_MCG_SC:
			t->SC = value;
			if (((!(value & 0x1)) && (value & 0x20) && (value & 0x40) && (value & 0x80))) {
				t->SC &= (~(0x80));
			} else if (((!(value & 0x1)) && (value & 0x20) && (!(value & 0x40)) && (value & 0x80))) {
				t->SC &= (~(0x80));
			}
			break;
		case A_MCG_ATCVH:
			t->ATCVH = value;
			break;
		case A_MCG_ATCVL:
			t->ATCVL = value;
			break;
		case A_MCG_C8:
			t->C8 = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMKL25Z MCG write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmkl25z_mcg_ops = {
	.read = frdmkl25z_mcg_read,
	.write = frdmkl25z_mcg_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmkl25z_mcg_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMKL25ZMCG *t = FRDMKL25Z_MCG(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmkl25z_mcg_ops, t, "frdmkl25z-mcg", FRDMKL25Z_MCG_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void frdmkl25z_mcg_realize(DeviceState *dev, Error **errp) {
	return;

}

static void frdmkl25z_mcg_reset_enter(Object *obj, ResetType type) {
	FRDMKL25ZMCG *t = FRDMKL25Z_MCG(obj);
	frdmkl25z_mcg_register_reset(t);
}

static const VMStateDescription frdmkl25z_mcg_vmstate = {
	.name = "frdmkl25z-mcg",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT8(C1, FRDMKL25ZMCG),
		VMSTATE_UINT8(C2, FRDMKL25ZMCG),
		VMSTATE_UINT8(C3, FRDMKL25ZMCG),
		VMSTATE_UINT8(C4, FRDMKL25ZMCG),
		VMSTATE_UINT8(C5, FRDMKL25ZMCG),
		VMSTATE_UINT8(C6, FRDMKL25ZMCG),
		VMSTATE_UINT8(S, FRDMKL25ZMCG),
		VMSTATE_UINT8(SC, FRDMKL25ZMCG),
		VMSTATE_UINT8(ATCVH, FRDMKL25ZMCG),
		VMSTATE_UINT8(ATCVL, FRDMKL25ZMCG),
		VMSTATE_UINT8(C7, FRDMKL25ZMCG),
		VMSTATE_UINT8(C8, FRDMKL25ZMCG),
		VMSTATE_UINT8(C9, FRDMKL25ZMCG),
		VMSTATE_UINT8(C10, FRDMKL25ZMCG),

		VMSTATE_END_OF_LIST()
	}
};

static void frdmkl25z_mcg_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmkl25z_mcg_vmstate;
	dc->realize = frdmkl25z_mcg_realize;
	rc->phases.enter = frdmkl25z_mcg_reset_enter;

}

static const TypeInfo frdmkl25z_mcg_info = {
	.name = TYPE_FRDMKL25Z_MCG,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMKL25ZMCG),
	.instance_init = frdmkl25z_mcg_init,
	.class_init = frdmkl25z_mcg_class_init,
};

static void frdmkl25z_mcg_register_types(void) {
	type_register_static(&frdmkl25z_mcg_info);
}

type_init(frdmkl25z_mcg_register_types);

#define TYPE_FRDMKL25Z_UART "frdmkl25z-uart"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMKL25ZUART, FRDMKL25Z_UART)

struct FRDMKL25ZUART {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint8_t BDH;
	uint8_t BDL;
	uint8_t C1;
	uint8_t C2;
	uint8_t S1;
	uint8_t S2;
	uint8_t C3;
	uint8_t D;
	uint8_t MA1;
	uint8_t MA2;
	uint8_t C4;
	uint8_t C5;
	/* chardev backend */
	CharBackend chr;
	guint watch_tag;

	/* base */
	uint32_t base;

};
#define FRDMKL25Z_UART_SIZE				0xc

REG8(UART_BDH, 0x0)
	FIELD(UART_BDH, SBR, 0, 5)
	FIELD(UART_BDH, SBNS, 5, 1)
	FIELD(UART_BDH, RXEDGIE, 6, 1)
	FIELD(UART_BDH, LBKDIE, 7, 1)
REG8(UART_BDL, 0x1)
	FIELD(UART_BDL, SBR, 0, 8)
REG8(UART_C1, 0x2)
	FIELD(UART_C1, PT, 0, 1)
	FIELD(UART_C1, PE, 1, 1)
	FIELD(UART_C1, ILT, 2, 1)
	FIELD(UART_C1, WAKE, 3, 1)
	FIELD(UART_C1, M, 4, 1)
	FIELD(UART_C1, RSRC, 5, 1)
	FIELD(UART_C1, DOZEEN, 6, 1)
	FIELD(UART_C1, LOOPS, 7, 1)
REG8(UART_C2, 0x3)
	FIELD(UART_C2, SBK, 0, 1)
	FIELD(UART_C2, RWU, 1, 1)
	FIELD(UART_C2, RE, 2, 1)
	FIELD(UART_C2, TE, 3, 1)
	FIELD(UART_C2, ILIE, 4, 1)
	FIELD(UART_C2, RIE, 5, 1)
	FIELD(UART_C2, TCIE, 6, 1)
	FIELD(UART_C2, TIE, 7, 1)
REG8(UART_S1, 0x4)
	FIELD(UART_S1, PF, 0, 1)
	FIELD(UART_S1, FE, 1, 1)
	FIELD(UART_S1, NF, 2, 1)
	FIELD(UART_S1, OR, 3, 1)
	FIELD(UART_S1, IDLE, 4, 1)
	FIELD(UART_S1, RDRF, 5, 1)
	FIELD(UART_S1, TC, 6, 1)
	FIELD(UART_S1, TDRE, 7, 1)
REG8(UART_S2, 0x5)
	FIELD(UART_S2, RAF, 0, 1)
	FIELD(UART_S2, LBKDE, 1, 1)
	FIELD(UART_S2, BRK13, 2, 1)
	FIELD(UART_S2, RWUID, 3, 1)
	FIELD(UART_S2, RXINV, 4, 1)
	FIELD(UART_S2, MSBF, 5, 1)
	FIELD(UART_S2, RXEDGIF, 6, 1)
	FIELD(UART_S2, LBKDIF, 7, 1)
REG8(UART_C3, 0x6)
	FIELD(UART_C3, PEIE, 0, 1)
	FIELD(UART_C3, FEIE, 1, 1)
	FIELD(UART_C3, NEIE, 2, 1)
	FIELD(UART_C3, ORIE, 3, 1)
	FIELD(UART_C3, TXINV, 4, 1)
	FIELD(UART_C3, TXDIR, 5, 1)
	FIELD(UART_C3, R9T8, 6, 1)
	FIELD(UART_C3, R8T9, 7, 1)
REG8(UART_D, 0x7)
	FIELD(UART_D, R0T0, 0, 1)
	FIELD(UART_D, R1T1, 1, 1)
	FIELD(UART_D, R2T2, 2, 1)
	FIELD(UART_D, R3T3, 3, 1)
	FIELD(UART_D, R4T4, 4, 1)
	FIELD(UART_D, R5T5, 5, 1)
	FIELD(UART_D, R6T6, 6, 1)
	FIELD(UART_D, R7T7, 7, 1)
REG8(UART_MA1, 0x8)
	FIELD(UART_MA1, MA, 0, 8)
REG8(UART_MA2, 0x9)
	FIELD(UART_MA2, MA, 0, 8)
REG8(UART_C4, 0xa)
	FIELD(UART_C4, OSR, 0, 5)
	FIELD(UART_C4, M10, 5, 1)
	FIELD(UART_C4, MAEN2, 6, 1)
	FIELD(UART_C4, MAEN1, 7, 1)
REG8(UART_C5, 0xb)
	FIELD(UART_C5, RESYNCDIS, 0, 1)
	FIELD(UART_C5, BOTHEDGE, 1, 1)
	FIELD(UART_C5, RDMAE, 5, 1)
	FIELD(UART_C5, TDMAE, 7, 1)


__attribute__((unused))
static void frdmkl25z_uart_update(FRDMKL25ZUART *t) {
	int conditions = ((t->S1 & 0x8) || (t->S1 & 0x2) || (t->S1 & 0x1) || ((t->S1 & 0x80) && (t->C2 & 0x80)) || ((t->C2 & 0x20) && (t->S1 & 0x20)));
	qemu_set_irq(t->irq[0], conditions);

}

static int frdmkl25z_uart_can_receive(void *opaque) {
	return 1;
}

static void frdmkl25z_uart_receive(void *opaque, const uint8_t *buf, int size) {
	FRDMKL25ZUART *t = FRDMKL25Z_UART(opaque);

	t->D = *buf;
	t->S1 |= 0x20;

	frdmkl25z_uart_update(t);
}

static gboolean frdmkl25z_uart_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	FRDMKL25ZUART *t = FRDMKL25Z_UART(opaque);
	int ret;

	t->watch_tag = 0;
	t->S1 &= (~(0x80));

	frdmkl25z_uart_update(t);

	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->D), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, frdmkl25z_uart_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->S1 |= 0x80;

	frdmkl25z_uart_update(t);

	return FALSE;
}

static void frdmkl25z_uart_register_reset(FRDMKL25ZUART *t) {
	t->BDH = 0x0;
	t->BDL = 0x4;
	t->C1 = 0x0;
	t->C2 = 0x0;
	t->S1 = 0xc0;
	t->S2 = 0x0;
	t->C3 = 0x0;
	t->D = 0x0;
	t->MA1 = 0x0;
	t->MA2 = 0x0;
	t->C4 = 0xf;
	t->C5 = 0x0;
	t->S1 |= 0x80;

}

static uint64_t frdmkl25z_uart_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMKL25ZUART *t = FRDMKL25Z_UART(opaque);
	uint64_t ret;

	switch (offset) {
		case A_UART_BDH:
			ret = t->BDH;
			break;
		case A_UART_BDL:
			ret = t->BDL;
			break;
		case A_UART_C1:
			ret = t->C1;
			break;
		case A_UART_C2:
			ret = t->C2;
			break;
		case A_UART_S1:
			ret = t->S1;
			break;
		case A_UART_S2:
			ret = t->S2;
			break;
		case A_UART_C3:
			ret = t->C3;
			break;
		case A_UART_D:
			ret = t->D;
			t->S1 &= (~(0x20));
			qemu_chr_fe_accept_input(&(t->chr));
			frdmkl25z_uart_update(t);
			break;
		case A_UART_MA1:
			ret = t->MA1;
			break;
		case A_UART_MA2:
			ret = t->MA2;
			break;
		case A_UART_C4:
			ret = t->C4;
			break;
		case A_UART_C5:
			ret = t->C5;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMKL25Z UART read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmkl25z_uart_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMKL25ZUART *t = FRDMKL25Z_UART(opaque);


	switch (offset) {

		default: break;
	}

	switch (offset) {
		case A_UART_BDH:
			t->BDH = value;
			break;
		case A_UART_BDL:
			t->BDL = value;
			break;
		case A_UART_C1:
			t->C1 = value;
			break;
		case A_UART_C2:
			t->C2 = value;
			frdmkl25z_uart_update(t);
			break;
		case A_UART_S1:
			t->S1 &= value;
			t->S1 |= 0x80;
			if ((value & 0x8)) {
				t->S1 &= (~(0x20));
			}
			frdmkl25z_uart_update(t);
			break;
		case A_UART_S2:
			t->S2 = value;
			break;
		case A_UART_C3:
			t->C3 = value;
			break;
		case A_UART_D:
			t->D = value;
			frdmkl25z_uart_transmit(NULL, G_IO_OUT, t);
			break;
		case A_UART_MA1:
			t->MA1 = value;
			break;
		case A_UART_MA2:
			t->MA2 = value;
			break;
		case A_UART_C4:
			t->C4 = value;
			break;
		case A_UART_C5:
			t->C5 = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMKL25Z UART write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmkl25z_uart_ops = {
	.read = frdmkl25z_uart_read,
	.write = frdmkl25z_uart_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmkl25z_uart_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMKL25ZUART *t = FRDMKL25Z_UART(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmkl25z_uart_ops, t, "frdmkl25z-uart", FRDMKL25Z_UART_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void frdmkl25z_uart_realize(DeviceState *dev, Error **errp) {
	FRDMKL25ZUART *t = FRDMKL25Z_UART(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), frdmkl25z_uart_can_receive, frdmkl25z_uart_receive,
		NULL, NULL, t, NULL, true);

}

static void frdmkl25z_uart_reset_enter(Object *obj, ResetType type) {
	FRDMKL25ZUART *t = FRDMKL25Z_UART(obj);
	frdmkl25z_uart_register_reset(t);
}

static const VMStateDescription frdmkl25z_uart_vmstate = {
	.name = "frdmkl25z-uart",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT8(BDH, FRDMKL25ZUART),
		VMSTATE_UINT8(BDL, FRDMKL25ZUART),
		VMSTATE_UINT8(C1, FRDMKL25ZUART),
		VMSTATE_UINT8(C2, FRDMKL25ZUART),
		VMSTATE_UINT8(S1, FRDMKL25ZUART),
		VMSTATE_UINT8(S2, FRDMKL25ZUART),
		VMSTATE_UINT8(C3, FRDMKL25ZUART),
		VMSTATE_UINT8(D, FRDMKL25ZUART),
		VMSTATE_UINT8(MA1, FRDMKL25ZUART),
		VMSTATE_UINT8(MA2, FRDMKL25ZUART),
		VMSTATE_UINT8(C4, FRDMKL25ZUART),
		VMSTATE_UINT8(C5, FRDMKL25ZUART),

		VMSTATE_END_OF_LIST()
	}
};

static Property frdmkl25z_uart_properties[] = {
	DEFINE_PROP_CHR("chardev", FRDMKL25ZUART, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void frdmkl25z_uart_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmkl25z_uart_vmstate;
	dc->realize = frdmkl25z_uart_realize;
	rc->phases.enter = frdmkl25z_uart_reset_enter;
	device_class_set_props(dc, frdmkl25z_uart_properties);

}

static const TypeInfo frdmkl25z_uart_info = {
	.name = TYPE_FRDMKL25Z_UART,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMKL25ZUART),
	.instance_init = frdmkl25z_uart_init,
	.class_init = frdmkl25z_uart_class_init,
};

static void frdmkl25z_uart_register_types(void) {
	type_register_static(&frdmkl25z_uart_info);
}

type_init(frdmkl25z_uart_register_types);

#define TYPE_FRDMKL25Z_UART1 "frdmkl25z-uart1"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMKL25ZUART1, FRDMKL25Z_UART1)

struct FRDMKL25ZUART1 {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint8_t BDH;
	uint8_t BDL;
	uint8_t C1;
	uint8_t C2;
	uint8_t S1;
	uint8_t S2;
	uint8_t C3;
	uint8_t D;
	uint8_t C4;
	/* chardev backend */
	CharBackend chr;
	guint watch_tag;

	/* base */
	uint32_t base;

};
#define FRDMKL25Z_UART1_SIZE				0x9

REG8(UART1_BDH, 0x0)
	FIELD(UART1_BDH, SBR, 0, 5)
	FIELD(UART1_BDH, SBNS, 5, 1)
	FIELD(UART1_BDH, RXEDGIE, 6, 1)
	FIELD(UART1_BDH, LBKDIE, 7, 1)
REG8(UART1_BDL, 0x1)
	FIELD(UART1_BDL, SBR, 0, 8)
REG8(UART1_C1, 0x2)
	FIELD(UART1_C1, PT, 0, 1)
	FIELD(UART1_C1, PE, 1, 1)
	FIELD(UART1_C1, ILT, 2, 1)
	FIELD(UART1_C1, WAKE, 3, 1)
	FIELD(UART1_C1, M, 4, 1)
	FIELD(UART1_C1, RSRC, 5, 1)
	FIELD(UART1_C1, UARTSWAI, 6, 1)
	FIELD(UART1_C1, LOOPS, 7, 1)
REG8(UART1_C2, 0x3)
	FIELD(UART1_C2, SBK, 0, 1)
	FIELD(UART1_C2, RWU, 1, 1)
	FIELD(UART1_C2, RE, 2, 1)
	FIELD(UART1_C2, TE, 3, 1)
	FIELD(UART1_C2, ILIE, 4, 1)
	FIELD(UART1_C2, RIE, 5, 1)
	FIELD(UART1_C2, TCIE, 6, 1)
	FIELD(UART1_C2, TIE, 7, 1)
REG8(UART1_S1, 0x4)
	FIELD(UART1_S1, PF, 0, 1)
	FIELD(UART1_S1, FE, 1, 1)
	FIELD(UART1_S1, NF, 2, 1)
	FIELD(UART1_S1, OR, 3, 1)
	FIELD(UART1_S1, IDLE, 4, 1)
	FIELD(UART1_S1, RDRF, 5, 1)
	FIELD(UART1_S1, TC, 6, 1)
	FIELD(UART1_S1, TDRE, 7, 1)
REG8(UART1_S2, 0x5)
	FIELD(UART1_S2, RAF, 0, 1)
	FIELD(UART1_S2, LBKDE, 1, 1)
	FIELD(UART1_S2, BRK13, 2, 1)
	FIELD(UART1_S2, RWUID, 3, 1)
	FIELD(UART1_S2, RXINV, 4, 1)
	FIELD(UART1_S2, RXEDGIF, 6, 1)
	FIELD(UART1_S2, LBKDIF, 7, 1)
REG8(UART1_C3, 0x6)
	FIELD(UART1_C3, PEIE, 0, 1)
	FIELD(UART1_C3, FEIE, 1, 1)
	FIELD(UART1_C3, NEIE, 2, 1)
	FIELD(UART1_C3, ORIE, 3, 1)
	FIELD(UART1_C3, TXINV, 4, 1)
	FIELD(UART1_C3, TXDIR, 5, 1)
	FIELD(UART1_C3, T8, 6, 1)
	FIELD(UART1_C3, R8, 7, 1)
REG8(UART1_D, 0x7)
	FIELD(UART1_D, R0T0, 0, 1)
	FIELD(UART1_D, R1T1, 1, 1)
	FIELD(UART1_D, R2T2, 2, 1)
	FIELD(UART1_D, R3T3, 3, 1)
	FIELD(UART1_D, R4T4, 4, 1)
	FIELD(UART1_D, R5T5, 5, 1)
	FIELD(UART1_D, R6T6, 6, 1)
	FIELD(UART1_D, R7T7, 7, 1)
REG8(UART1_C4, 0x8)
	FIELD(UART1_C4, RDMAS, 5, 1)
	FIELD(UART1_C4, TDMAS, 7, 1)


__attribute__((unused))
static void frdmkl25z_uart1_update(FRDMKL25ZUART1 *t) {
	int conditions = ((t->S1 & 0x1) || (t->S1 & 0x2) || (t->S1 & 0x8) || ((t->S1 & 0x20) && (t->C2 & 0x20)) || ((t->C2 & 0x80) && (t->S1 & 0x80)));
	qemu_set_irq(t->irq[0], conditions);

}

static int frdmkl25z_uart1_can_receive(void *opaque) {
	return 1;
}

static void frdmkl25z_uart1_receive(void *opaque, const uint8_t *buf, int size) {
	FRDMKL25ZUART1 *t = FRDMKL25Z_UART1(opaque);

	t->D = *buf;
	t->S1 |= 0x20;

	frdmkl25z_uart1_update(t);
}

static gboolean frdmkl25z_uart1_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	FRDMKL25ZUART1 *t = FRDMKL25Z_UART1(opaque);
	int ret;

	t->watch_tag = 0;
	t->S1 &= (~(0x80));

	frdmkl25z_uart1_update(t);

	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->D), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, frdmkl25z_uart1_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->S1 |= 0x80;

	frdmkl25z_uart1_update(t);

	return FALSE;
}

static void frdmkl25z_uart1_register_reset(FRDMKL25ZUART1 *t) {
	t->BDH = 0x0;
	t->BDL = 0x4;
	t->C1 = 0x0;
	t->C2 = 0x0;
	t->S1 = 0xc0;
	t->S2 = 0x0;
	t->C3 = 0x0;
	t->D = 0x0;
	t->C4 = 0x0;
	t->S1 |= 0x80;

}

static uint64_t frdmkl25z_uart1_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMKL25ZUART1 *t = FRDMKL25Z_UART1(opaque);
	uint64_t ret;

	switch (offset) {
		case A_UART1_BDH:
			ret = t->BDH;
			break;
		case A_UART1_BDL:
			ret = t->BDL;
			break;
		case A_UART1_C1:
			ret = t->C1;
			break;
		case A_UART1_C2:
			ret = t->C2;
			break;
		case A_UART1_S1:
			ret = t->S1;
			break;
		case A_UART1_S2:
			ret = t->S2;
			break;
		case A_UART1_C3:
			ret = t->C3;
			break;
		case A_UART1_D:
			ret = t->D;
			t->S1 &= (~(0x20));
			qemu_chr_fe_accept_input(&(t->chr));
			frdmkl25z_uart1_update(t);
			break;
		case A_UART1_C4:
			ret = t->C4;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMKL25Z UART1 read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmkl25z_uart1_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMKL25ZUART1 *t = FRDMKL25Z_UART1(opaque);


	switch (offset) {

		default: break;
	}

	switch (offset) {
		case A_UART1_BDH:
			t->BDH = value;
			break;
		case A_UART1_BDL:
			t->BDL = value;
			break;
		case A_UART1_C1:
			t->C1 = value;
			break;
		case A_UART1_C2:
			t->C2 = value;
			frdmkl25z_uart1_update(t);
			break;
		case A_UART1_S2:
			t->S2 = value;
			break;
		case A_UART1_C3:
			t->C3 = value;
			break;
		case A_UART1_D:
			t->D = value;
			frdmkl25z_uart1_transmit(NULL, G_IO_OUT, t);
			break;
		case A_UART1_C4:
			t->C4 = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMKL25Z UART1 write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmkl25z_uart1_ops = {
	.read = frdmkl25z_uart1_read,
	.write = frdmkl25z_uart1_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmkl25z_uart1_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMKL25ZUART1 *t = FRDMKL25Z_UART1(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmkl25z_uart1_ops, t, "frdmkl25z-uart1", FRDMKL25Z_UART1_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void frdmkl25z_uart1_realize(DeviceState *dev, Error **errp) {
	FRDMKL25ZUART1 *t = FRDMKL25Z_UART1(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), frdmkl25z_uart1_can_receive, frdmkl25z_uart1_receive,
		NULL, NULL, t, NULL, true);

}

static void frdmkl25z_uart1_reset_enter(Object *obj, ResetType type) {
	FRDMKL25ZUART1 *t = FRDMKL25Z_UART1(obj);
	frdmkl25z_uart1_register_reset(t);
}

static const VMStateDescription frdmkl25z_uart1_vmstate = {
	.name = "frdmkl25z-uart1",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT8(BDH, FRDMKL25ZUART1),
		VMSTATE_UINT8(BDL, FRDMKL25ZUART1),
		VMSTATE_UINT8(C1, FRDMKL25ZUART1),
		VMSTATE_UINT8(C2, FRDMKL25ZUART1),
		VMSTATE_UINT8(S1, FRDMKL25ZUART1),
		VMSTATE_UINT8(S2, FRDMKL25ZUART1),
		VMSTATE_UINT8(C3, FRDMKL25ZUART1),
		VMSTATE_UINT8(D, FRDMKL25ZUART1),
		VMSTATE_UINT8(C4, FRDMKL25ZUART1),

		VMSTATE_END_OF_LIST()
	}
};

static Property frdmkl25z_uart1_properties[] = {
	DEFINE_PROP_CHR("chardev", FRDMKL25ZUART1, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void frdmkl25z_uart1_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmkl25z_uart1_vmstate;
	dc->realize = frdmkl25z_uart1_realize;
	rc->phases.enter = frdmkl25z_uart1_reset_enter;
	device_class_set_props(dc, frdmkl25z_uart1_properties);

}

static const TypeInfo frdmkl25z_uart1_info = {
	.name = TYPE_FRDMKL25Z_UART1,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMKL25ZUART1),
	.instance_init = frdmkl25z_uart1_init,
	.class_init = frdmkl25z_uart1_class_init,
};

static void frdmkl25z_uart1_register_types(void) {
	type_register_static(&frdmkl25z_uart1_info);
}

type_init(frdmkl25z_uart1_register_types);

#define TYPE_FRDMKL25Z_SPI "frdmkl25z-spi"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMKL25ZSPI, FRDMKL25Z_SPI)

struct FRDMKL25ZSPI {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint8_t C1;
	uint8_t C2;
	uint8_t BR;
	uint8_t S;
	uint8_t D;
	uint8_t M;
	/* chardev backend */
	CharBackend chr;
	guint watch_tag;

	/* base */
	uint32_t base;

};
#define FRDMKL25Z_SPI_SIZE				0x8

REG8(SPI_C1, 0x0)
	FIELD(SPI_C1, LSBFE, 0, 1)
	FIELD(SPI_C1, SSOE, 1, 1)
	FIELD(SPI_C1, CPHA, 2, 1)
	FIELD(SPI_C1, CPOL, 3, 1)
	FIELD(SPI_C1, MSTR, 4, 1)
	FIELD(SPI_C1, SPTIE, 5, 1)
	FIELD(SPI_C1, SPE, 6, 1)
	FIELD(SPI_C1, SPIE, 7, 1)
REG8(SPI_C2, 0x1)
	FIELD(SPI_C2, SPC0, 0, 1)
	FIELD(SPI_C2, SPISWAI, 1, 1)
	FIELD(SPI_C2, RXDMAE, 2, 1)
	FIELD(SPI_C2, BIDIROE, 3, 1)
	FIELD(SPI_C2, MODFEN, 4, 1)
	FIELD(SPI_C2, TXDMAE, 5, 1)
	FIELD(SPI_C2, SPMIE, 7, 1)
REG8(SPI_BR, 0x2)
	FIELD(SPI_BR, SPR, 0, 4)
	FIELD(SPI_BR, SPPR, 4, 3)
REG8(SPI_S, 0x3)
	FIELD(SPI_S, MODF, 4, 1)
	FIELD(SPI_S, SPTEF, 5, 1)
	FIELD(SPI_S, SPMF, 6, 1)
	FIELD(SPI_S, SPRF, 7, 1)
REG8(SPI_D, 0x5)
	FIELD(SPI_D, Bits, 0, 8)
REG8(SPI_M, 0x7)
	FIELD(SPI_M, Bits, 0, 8)


static int frdmkl25z_spi_can_receive(void *opaque) {
	return 1;
}

static void frdmkl25z_spi_receive(void *opaque, const uint8_t *buf, int size) {
	FRDMKL25ZSPI *t = FRDMKL25Z_SPI(opaque);

	t->D = *buf;
	t->S |= 0x80;


}

static gboolean frdmkl25z_spi_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	FRDMKL25ZSPI *t = FRDMKL25Z_SPI(opaque);
	int ret;

	t->watch_tag = 0;
	t->S &= (~(0x20));
	t->S &= (~(0x80));



	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->D), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, frdmkl25z_spi_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->S |= 0x80;
	t->S |= 0x20;



	return FALSE;
}

static void frdmkl25z_spi_register_reset(FRDMKL25ZSPI *t) {
	t->C1 = 0x4;
	t->C2 = 0x0;
	t->BR = 0x0;
	t->S = 0x20;
	t->D = 0x0;
	t->M = 0x0;
	t->S |= 0x80;
	t->S |= 0x20;

}

static uint64_t frdmkl25z_spi_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMKL25ZSPI *t = FRDMKL25Z_SPI(opaque);
	uint64_t ret;

	switch (offset) {
		case A_SPI_C1:
			ret = t->C1;
			break;
		case A_SPI_C2:
			ret = t->C2;
			break;
		case A_SPI_BR:
			ret = t->BR;
			break;
		case A_SPI_S:
			ret = t->S;
			break;
		case A_SPI_D:
			ret = t->D;
			t->S &= (~(0x80));
			qemu_chr_fe_accept_input(&(t->chr));
			break;
		case A_SPI_M:
			ret = t->M;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMKL25Z SPI read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmkl25z_spi_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMKL25ZSPI *t = FRDMKL25Z_SPI(opaque);


	switch (offset) {

		default: break;
	}

	switch (offset) {
		case A_SPI_C1:
			t->C1 = value;
			break;
		case A_SPI_C2:
			t->C2 = value;
			break;
		case A_SPI_BR:
			t->BR = value;
			break;
		case A_SPI_S:
			t->S &= value;
			t->S |= 0x80;
			t->S |= 0x20;
			break;
		case A_SPI_D:
			t->D = value;
			frdmkl25z_spi_transmit(NULL, G_IO_OUT, t);
			break;
		case A_SPI_M:
			t->M = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMKL25Z SPI write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmkl25z_spi_ops = {
	.read = frdmkl25z_spi_read,
	.write = frdmkl25z_spi_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmkl25z_spi_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMKL25ZSPI *t = FRDMKL25Z_SPI(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmkl25z_spi_ops, t, "frdmkl25z-spi", FRDMKL25Z_SPI_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void frdmkl25z_spi_realize(DeviceState *dev, Error **errp) {
	FRDMKL25ZSPI *t = FRDMKL25Z_SPI(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), frdmkl25z_spi_can_receive, frdmkl25z_spi_receive,
		NULL, NULL, t, NULL, true);

}

static void frdmkl25z_spi_reset_enter(Object *obj, ResetType type) {
	FRDMKL25ZSPI *t = FRDMKL25Z_SPI(obj);
	frdmkl25z_spi_register_reset(t);
}

static const VMStateDescription frdmkl25z_spi_vmstate = {
	.name = "frdmkl25z-spi",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT8(C1, FRDMKL25ZSPI),
		VMSTATE_UINT8(C2, FRDMKL25ZSPI),
		VMSTATE_UINT8(BR, FRDMKL25ZSPI),
		VMSTATE_UINT8(S, FRDMKL25ZSPI),
		VMSTATE_UINT8(D, FRDMKL25ZSPI),
		VMSTATE_UINT8(M, FRDMKL25ZSPI),

		VMSTATE_END_OF_LIST()
	}
};

static Property frdmkl25z_spi_properties[] = {
	DEFINE_PROP_CHR("chardev", FRDMKL25ZSPI, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void frdmkl25z_spi_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmkl25z_spi_vmstate;
	dc->realize = frdmkl25z_spi_realize;
	rc->phases.enter = frdmkl25z_spi_reset_enter;
	device_class_set_props(dc, frdmkl25z_spi_properties);

}

static const TypeInfo frdmkl25z_spi_info = {
	.name = TYPE_FRDMKL25Z_SPI,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMKL25ZSPI),
	.instance_init = frdmkl25z_spi_init,
	.class_init = frdmkl25z_spi_class_init,
};

static void frdmkl25z_spi_register_types(void) {
	type_register_static(&frdmkl25z_spi_info);
}

type_init(frdmkl25z_spi_register_types);

#define TYPE_FRDMKL25Z_I2C "frdmkl25z-i2c"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMKL25ZI2C, FRDMKL25Z_I2C)

struct FRDMKL25ZI2C {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint8_t A1;
	uint8_t F;
	uint8_t C1;
	uint8_t S;
	uint8_t D;
	uint8_t C2;
	uint8_t FLT;
	uint8_t RA;
	uint8_t SMB;
	uint8_t A2;
	uint8_t SLTH;
	uint8_t SLTL;
	/* chardev backend */
	CharBackend chr;
	guint watch_tag;

	/* base */
	uint32_t base;

};
#define FRDMKL25Z_I2C_SIZE				0xc

REG8(I2C_A1, 0x0)
	FIELD(I2C_A1, AD, 1, 7)
REG8(I2C_F, 0x1)
	FIELD(I2C_F, ICR, 0, 6)
	FIELD(I2C_F, MULT, 6, 2)
REG8(I2C_C1, 0x2)
	FIELD(I2C_C1, DMAEN, 0, 1)
	FIELD(I2C_C1, WUEN, 1, 1)
	FIELD(I2C_C1, RSTA, 2, 1)
	FIELD(I2C_C1, TXAK, 3, 1)
	FIELD(I2C_C1, TX, 4, 1)
	FIELD(I2C_C1, MST, 5, 1)
	FIELD(I2C_C1, IICIE, 6, 1)
	FIELD(I2C_C1, IICEN, 7, 1)
REG8(I2C_S, 0x3)
	FIELD(I2C_S, RXAK, 0, 1)
	FIELD(I2C_S, IICIF, 1, 1)
	FIELD(I2C_S, SRW, 2, 1)
	FIELD(I2C_S, RAM, 3, 1)
	FIELD(I2C_S, ARBL, 4, 1)
	FIELD(I2C_S, BUSY, 5, 1)
	FIELD(I2C_S, IAAS, 6, 1)
	FIELD(I2C_S, TCF, 7, 1)
REG8(I2C_D, 0x4)
	FIELD(I2C_D, DATA, 0, 8)
REG8(I2C_C2, 0x5)
	FIELD(I2C_C2, AD, 0, 3)
	FIELD(I2C_C2, RMEN, 3, 1)
	FIELD(I2C_C2, SBRC, 4, 1)
	FIELD(I2C_C2, HDRS, 5, 1)
	FIELD(I2C_C2, ADEXT, 6, 1)
	FIELD(I2C_C2, GCAEN, 7, 1)
REG8(I2C_FLT, 0x6)
	FIELD(I2C_FLT, FLT, 0, 5)
	FIELD(I2C_FLT, STOPIE, 5, 1)
	FIELD(I2C_FLT, STOPF, 6, 1)
	FIELD(I2C_FLT, SHEN, 7, 1)
REG8(I2C_RA, 0x7)
	FIELD(I2C_RA, RAD, 1, 7)
REG8(I2C_SMB, 0x8)
	FIELD(I2C_SMB, SHTF2IE, 0, 1)
	FIELD(I2C_SMB, SHTF2, 1, 1)
	FIELD(I2C_SMB, SHTF1, 2, 1)
	FIELD(I2C_SMB, SLTF, 3, 1)
	FIELD(I2C_SMB, TCKSEL, 4, 1)
	FIELD(I2C_SMB, SIICAEN, 5, 1)
	FIELD(I2C_SMB, ALERTEN, 6, 1)
	FIELD(I2C_SMB, FACK, 7, 1)
REG8(I2C_A2, 0x9)
	FIELD(I2C_A2, SAD, 1, 7)
REG8(I2C_SLTH, 0xa)
	FIELD(I2C_SLTH, SSLT, 0, 8)
REG8(I2C_SLTL, 0xb)
	FIELD(I2C_SLTL, SSLT, 0, 8)


__attribute__((unused))
static void frdmkl25z_i2c_update(FRDMKL25ZI2C *t) {
	int conditions = ((!(t->S & 0x10)) && (!(t->S & 0x1)) && (t->C1 & 0x20));
	qemu_set_irq(t->irq[0], conditions);

}

static int frdmkl25z_i2c_can_receive(void *opaque) {
	return 1;
}

static void frdmkl25z_i2c_receive(void *opaque, const uint8_t *buf, int size) {
	FRDMKL25ZI2C *t = FRDMKL25Z_I2C(opaque);

	t->D = *buf;
	t->S |= 0x2;
	t->S &= (~(0x1));
	t->S |= 0x80;
	t->S &= (~(0x10));
	t->S |= 0x40;
	t->S &= (~(0x20));

	frdmkl25z_i2c_update(t);
}

static gboolean frdmkl25z_i2c_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	FRDMKL25ZI2C *t = FRDMKL25Z_I2C(opaque);
	int ret;

	t->watch_tag = 0;
	t->S &= (~(0x40));
	t->S |= 0x10;
	t->S &= (~(0x2));
	t->S |= 0x1;
	t->S &= (~(0x80));
	t->S |= 0x20;

	frdmkl25z_i2c_update(t);

	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->D), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, frdmkl25z_i2c_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->S |= 0x2;
	t->S &= (~(0x1));
	t->S |= 0x80;
	t->S &= (~(0x10));
	t->S |= 0x40;
	t->S &= (~(0x20));

	frdmkl25z_i2c_update(t);

	return FALSE;
}

static void frdmkl25z_i2c_register_reset(FRDMKL25ZI2C *t) {
	t->A1 = 0x0;
	t->F = 0x0;
	t->C1 = 0x0;
	t->S = 0x80;
	t->D = 0x0;
	t->C2 = 0x0;
	t->FLT = 0x0;
	t->RA = 0x0;
	t->SMB = 0x0;
	t->A2 = 0xc2;
	t->SLTH = 0x0;
	t->SLTL = 0x0;
	t->S |= 0x2;
	t->S &= (~(0x1));
	t->S |= 0x80;
	t->S &= (~(0x10));
	t->S |= 0x40;
	t->S &= (~(0x20));

}

static uint64_t frdmkl25z_i2c_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMKL25ZI2C *t = FRDMKL25Z_I2C(opaque);
	uint64_t ret;

	switch (offset) {
		case A_I2C_A1:
			ret = t->A1;
			break;
		case A_I2C_F:
			ret = t->F;
			break;
		case A_I2C_C1:
			ret = t->C1;
			break;
		case A_I2C_S:
			ret = t->S;
			break;
		case A_I2C_D:
			ret = t->D;
			t->S &= (~(0x40));
			t->S |= 0x10;
			t->S &= (~(0x2));
			t->S |= 0x1;
			t->S &= (~(0x80));
			t->S |= 0x20;
			qemu_chr_fe_accept_input(&(t->chr));
			frdmkl25z_i2c_update(t);
			break;
		case A_I2C_C2:
			ret = t->C2;
			break;
		case A_I2C_FLT:
			ret = t->FLT;
			break;
		case A_I2C_RA:
			ret = t->RA;
			break;
		case A_I2C_SMB:
			ret = t->SMB;
			break;
		case A_I2C_A2:
			ret = t->A2;
			break;
		case A_I2C_SLTH:
			ret = t->SLTH;
			break;
		case A_I2C_SLTL:
			ret = t->SLTL;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMKL25Z I2C read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmkl25z_i2c_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMKL25ZI2C *t = FRDMKL25Z_I2C(opaque);


	switch (offset) {

		default: break;
	}

	switch (offset) {
		case A_I2C_A1:
			t->A1 = value;
			break;
		case A_I2C_F:
			t->F = value;
			break;
		case A_I2C_C1:
			t->C1 = value;
			if (((!(value & 0x8)) && (!(value & 0x10)) && (!(value & 0x20)))) {
				t->S &= (~(0x20));
			} else if ((value & 0x8)) {
				t->S |= 0x2;
			}
			if ((!(value & 0x40))) {
				t->S |= 0x2;
			}
			frdmkl25z_i2c_update(t);
			break;
		case A_I2C_S:
			t->S &= value;
			if (((!(value & 0x1)) && (!(value & 0x10)))) {
				t->S |= 0x80;
			} else if ((!(value & 0x10))) {
				t->S |= 0x80;
			}
			if ((value == 0x12)) {
				t->S |= 0x80;
			}
			frdmkl25z_i2c_update(t);
			break;
		case A_I2C_D:
			t->D = value;
			frdmkl25z_i2c_transmit(NULL, G_IO_OUT, t);
			break;
		case A_I2C_C2:
			t->C2 = value;
			break;
		case A_I2C_FLT:
			t->FLT = value;
			break;
		case A_I2C_RA:
			t->RA = value;
			break;
		case A_I2C_SMB:
			t->SMB = value;
			break;
		case A_I2C_A2:
			t->A2 = value;
			break;
		case A_I2C_SLTH:
			t->SLTH = value;
			break;
		case A_I2C_SLTL:
			t->SLTL = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMKL25Z I2C write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmkl25z_i2c_ops = {
	.read = frdmkl25z_i2c_read,
	.write = frdmkl25z_i2c_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmkl25z_i2c_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMKL25ZI2C *t = FRDMKL25Z_I2C(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmkl25z_i2c_ops, t, "frdmkl25z-i2c", FRDMKL25Z_I2C_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void frdmkl25z_i2c_realize(DeviceState *dev, Error **errp) {
	FRDMKL25ZI2C *t = FRDMKL25Z_I2C(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), frdmkl25z_i2c_can_receive, frdmkl25z_i2c_receive,
		NULL, NULL, t, NULL, true);

}

static void frdmkl25z_i2c_reset_enter(Object *obj, ResetType type) {
	FRDMKL25ZI2C *t = FRDMKL25Z_I2C(obj);
	frdmkl25z_i2c_register_reset(t);
}

static const VMStateDescription frdmkl25z_i2c_vmstate = {
	.name = "frdmkl25z-i2c",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT8(A1, FRDMKL25ZI2C),
		VMSTATE_UINT8(F, FRDMKL25ZI2C),
		VMSTATE_UINT8(C1, FRDMKL25ZI2C),
		VMSTATE_UINT8(S, FRDMKL25ZI2C),
		VMSTATE_UINT8(D, FRDMKL25ZI2C),
		VMSTATE_UINT8(C2, FRDMKL25ZI2C),
		VMSTATE_UINT8(FLT, FRDMKL25ZI2C),
		VMSTATE_UINT8(RA, FRDMKL25ZI2C),
		VMSTATE_UINT8(SMB, FRDMKL25ZI2C),
		VMSTATE_UINT8(A2, FRDMKL25ZI2C),
		VMSTATE_UINT8(SLTH, FRDMKL25ZI2C),
		VMSTATE_UINT8(SLTL, FRDMKL25ZI2C),

		VMSTATE_END_OF_LIST()
	}
};

static Property frdmkl25z_i2c_properties[] = {
	DEFINE_PROP_CHR("chardev", FRDMKL25ZI2C, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void frdmkl25z_i2c_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmkl25z_i2c_vmstate;
	dc->realize = frdmkl25z_i2c_realize;
	rc->phases.enter = frdmkl25z_i2c_reset_enter;
	device_class_set_props(dc, frdmkl25z_i2c_properties);

}

static const TypeInfo frdmkl25z_i2c_info = {
	.name = TYPE_FRDMKL25Z_I2C,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMKL25ZI2C),
	.instance_init = frdmkl25z_i2c_init,
	.class_init = frdmkl25z_i2c_class_init,
};

static void frdmkl25z_i2c_register_types(void) {
	type_register_static(&frdmkl25z_i2c_info);
}

type_init(frdmkl25z_i2c_register_types);

#define TYPE_FRDMKL25Z_ADC "frdmkl25z-adc"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMKL25ZADC, FRDMKL25Z_ADC)

struct FRDMKL25ZADC {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t CFG1;
	uint32_t CFG2;
	uint32_t SC2;
	uint32_t SC3;
	uint32_t OFS;
	uint32_t PG;
	uint32_t MG;
	uint32_t CLPD;
	uint32_t CLPS;
	uint32_t CLP4;
	uint32_t CLP3;
	uint32_t CLP2;
	uint32_t CLP1;
	uint32_t CLP0;
	uint32_t CLMD;
	uint32_t CLMS;
	uint32_t CLM4;
	uint32_t CLM3;
	uint32_t CLM2;
	uint32_t CLM1;
	uint32_t CLM0;
	uint32_t SC1A;
	uint32_t SC1B;
	uint32_t RA;
	uint32_t RB;
	uint32_t CV1;
	uint32_t CV2;
	/* base */
	uint32_t base;

};
#define FRDMKL25Z_ADC_SIZE				0x70

REG32(ADC_CFG1, 0x8)
	FIELD(ADC_CFG1, ADICLK, 0, 2)
	FIELD(ADC_CFG1, MODE, 2, 2)
	FIELD(ADC_CFG1, ADLSMP, 4, 1)
	FIELD(ADC_CFG1, ADIV, 5, 2)
	FIELD(ADC_CFG1, ADLPC, 7, 1)
REG32(ADC_CFG2, 0xc)
	FIELD(ADC_CFG2, ADLSTS, 0, 2)
	FIELD(ADC_CFG2, ADHSC, 2, 1)
	FIELD(ADC_CFG2, ADACKEN, 3, 1)
	FIELD(ADC_CFG2, MUXSEL, 4, 1)
REG32(ADC_SC2, 0x20)
	FIELD(ADC_SC2, REFSEL, 0, 2)
	FIELD(ADC_SC2, DMAEN, 2, 1)
	FIELD(ADC_SC2, ACREN, 3, 1)
	FIELD(ADC_SC2, ACFGT, 4, 1)
	FIELD(ADC_SC2, ACFE, 5, 1)
	FIELD(ADC_SC2, ADTRG, 6, 1)
	FIELD(ADC_SC2, ADACT, 7, 1)
REG32(ADC_SC3, 0x24)
	FIELD(ADC_SC3, AVGS, 0, 2)
	FIELD(ADC_SC3, AVGE, 2, 1)
	FIELD(ADC_SC3, ADCO, 3, 1)
	FIELD(ADC_SC3, CALF, 6, 1)
	FIELD(ADC_SC3, CAL, 7, 1)
REG32(ADC_OFS, 0x28)
	FIELD(ADC_OFS, OFS, 0, 16)
REG32(ADC_PG, 0x2c)
	FIELD(ADC_PG, PG, 0, 16)
REG32(ADC_MG, 0x30)
	FIELD(ADC_MG, MG, 0, 16)
REG32(ADC_CLPD, 0x34)
	FIELD(ADC_CLPD, CLPD, 0, 6)
REG32(ADC_CLPS, 0x38)
	FIELD(ADC_CLPS, CLPS, 0, 6)
REG32(ADC_CLP4, 0x3c)
	FIELD(ADC_CLP4, CLP4, 0, 10)
REG32(ADC_CLP3, 0x40)
	FIELD(ADC_CLP3, CLP3, 0, 9)
REG32(ADC_CLP2, 0x44)
	FIELD(ADC_CLP2, CLP2, 0, 8)
REG32(ADC_CLP1, 0x48)
	FIELD(ADC_CLP1, CLP1, 0, 7)
REG32(ADC_CLP0, 0x4c)
	FIELD(ADC_CLP0, CLP0, 0, 6)
REG32(ADC_CLMD, 0x54)
	FIELD(ADC_CLMD, CLMD, 0, 6)
REG32(ADC_CLMS, 0x58)
	FIELD(ADC_CLMS, CLMS, 0, 6)
REG32(ADC_CLM4, 0x5c)
	FIELD(ADC_CLM4, CLM4, 0, 10)
REG32(ADC_CLM3, 0x60)
	FIELD(ADC_CLM3, CLM3, 0, 9)
REG32(ADC_CLM2, 0x64)
	FIELD(ADC_CLM2, CLM2, 0, 8)
REG32(ADC_CLM1, 0x68)
	FIELD(ADC_CLM1, CLM1, 0, 7)
REG32(ADC_CLM0, 0x6c)
	FIELD(ADC_CLM0, CLM0, 0, 6)
REG32(ADC_SC1A, 0x0)
	FIELD(ADC_SC1A, ADCH, 0, 5)
	FIELD(ADC_SC1A, DIFF, 5, 1)
	FIELD(ADC_SC1A, AIEN, 6, 1)
	FIELD(ADC_SC1A, COCO, 7, 1)
REG32(ADC_SC1B, 0x4)
	FIELD(ADC_SC1B, ADCH, 0, 5)
	FIELD(ADC_SC1B, DIFF, 5, 1)
	FIELD(ADC_SC1B, AIEN, 6, 1)
	FIELD(ADC_SC1B, COCO, 7, 1)
REG32(ADC_RA, 0x10)
	FIELD(ADC_RA, D, 0, 16)
REG32(ADC_RB, 0x14)
	FIELD(ADC_RB, D, 0, 16)
REG32(ADC_CV1, 0x18)
	FIELD(ADC_CV1, CV, 0, 16)
REG32(ADC_CV2, 0x1c)
	FIELD(ADC_CV2, CV, 0, 16)


static void frdmkl25z_adc_register_reset(FRDMKL25ZADC *t) {
	t->CFG1 = 0x0;
	t->CFG2 = 0x0;
	t->SC2 = 0x0;
	t->SC3 = 0x0;
	t->OFS = 0x4;
	t->PG = 0x8200;
	t->MG = 0x8200;
	t->CLPD = 0xa;
	t->CLPS = 0x20;
	t->CLP4 = 0x200;
	t->CLP3 = 0x100;
	t->CLP2 = 0x80;
	t->CLP1 = 0x40;
	t->CLP0 = 0x20;
	t->CLMD = 0xa;
	t->CLMS = 0x20;
	t->CLM4 = 0x200;
	t->CLM3 = 0x100;
	t->CLM2 = 0x80;
	t->CLM1 = 0x40;
	t->CLM0 = 0x20;
	t->SC1A = 0x1f;
	t->SC1B = 0x1f;
	t->RA = 0x0;
	t->RB = 0x0;
	t->CV1 = 0x0;
	t->CV2 = 0x0;

}

static uint64_t frdmkl25z_adc_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMKL25ZADC *t = FRDMKL25Z_ADC(opaque);
	uint64_t ret;

	switch (offset) {
		case A_ADC_CFG1:
			ret = t->CFG1;
			break;
		case A_ADC_CFG2:
			ret = t->CFG2;
			break;
		case A_ADC_SC2:
			ret = t->SC2;
			break;
		case A_ADC_SC3:
			ret = t->SC3;
			break;
		case A_ADC_OFS:
			ret = t->OFS;
			break;
		case A_ADC_PG:
			ret = t->PG;
			break;
		case A_ADC_MG:
			ret = t->MG;
			break;
		case A_ADC_CLPD:
			ret = t->CLPD;
			break;
		case A_ADC_CLPS:
			ret = t->CLPS;
			break;
		case A_ADC_CLP4:
			ret = t->CLP4;
			break;
		case A_ADC_CLP3:
			ret = t->CLP3;
			break;
		case A_ADC_CLP2:
			ret = t->CLP2;
			break;
		case A_ADC_CLP1:
			ret = t->CLP1;
			break;
		case A_ADC_CLP0:
			ret = t->CLP0;
			break;
		case A_ADC_CLMD:
			ret = t->CLMD;
			break;
		case A_ADC_CLMS:
			ret = t->CLMS;
			break;
		case A_ADC_CLM4:
			ret = t->CLM4;
			break;
		case A_ADC_CLM3:
			ret = t->CLM3;
			break;
		case A_ADC_CLM2:
			ret = t->CLM2;
			break;
		case A_ADC_CLM1:
			ret = t->CLM1;
			break;
		case A_ADC_CLM0:
			ret = t->CLM0;
			break;
		case A_ADC_SC1A:
			ret = t->SC1A;
			break;
		case A_ADC_SC1B:
			ret = t->SC1B;
			break;
		case A_ADC_RA:
			ret = t->RA;
			break;
		case A_ADC_RB:
			ret = t->RB;
			break;
		case A_ADC_CV1:
			ret = t->CV1;
			break;
		case A_ADC_CV2:
			ret = t->CV2;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMKL25Z ADC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmkl25z_adc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMKL25ZADC *t = FRDMKL25Z_ADC(opaque);


	switch (offset) {
		case A_ADC_CFG1 ... A_ADC_CFG1 + 3:
			value = value << ((offset - A_ADC_CFG1) << 3);
			offset = A_ADC_CFG1;
			break;
		case A_ADC_CFG2 ... A_ADC_CFG2 + 3:
			value = value << ((offset - A_ADC_CFG2) << 3);
			offset = A_ADC_CFG2;
			break;
		case A_ADC_SC2 ... A_ADC_SC2 + 3:
			value = value << ((offset - A_ADC_SC2) << 3);
			offset = A_ADC_SC2;
			break;
		case A_ADC_SC3 ... A_ADC_SC3 + 3:
			value = value << ((offset - A_ADC_SC3) << 3);
			offset = A_ADC_SC3;
			break;
		case A_ADC_OFS ... A_ADC_OFS + 3:
			value = value << ((offset - A_ADC_OFS) << 3);
			offset = A_ADC_OFS;
			break;
		case A_ADC_PG ... A_ADC_PG + 3:
			value = value << ((offset - A_ADC_PG) << 3);
			offset = A_ADC_PG;
			break;
		case A_ADC_MG ... A_ADC_MG + 3:
			value = value << ((offset - A_ADC_MG) << 3);
			offset = A_ADC_MG;
			break;
		case A_ADC_CLPD ... A_ADC_CLPD + 3:
			value = value << ((offset - A_ADC_CLPD) << 3);
			offset = A_ADC_CLPD;
			break;
		case A_ADC_CLPS ... A_ADC_CLPS + 3:
			value = value << ((offset - A_ADC_CLPS) << 3);
			offset = A_ADC_CLPS;
			break;
		case A_ADC_CLP4 ... A_ADC_CLP4 + 3:
			value = value << ((offset - A_ADC_CLP4) << 3);
			offset = A_ADC_CLP4;
			break;
		case A_ADC_CLP3 ... A_ADC_CLP3 + 3:
			value = value << ((offset - A_ADC_CLP3) << 3);
			offset = A_ADC_CLP3;
			break;
		case A_ADC_CLP2 ... A_ADC_CLP2 + 3:
			value = value << ((offset - A_ADC_CLP2) << 3);
			offset = A_ADC_CLP2;
			break;
		case A_ADC_CLP1 ... A_ADC_CLP1 + 3:
			value = value << ((offset - A_ADC_CLP1) << 3);
			offset = A_ADC_CLP1;
			break;
		case A_ADC_CLP0 ... A_ADC_CLP0 + 3:
			value = value << ((offset - A_ADC_CLP0) << 3);
			offset = A_ADC_CLP0;
			break;
		case A_ADC_CLMD ... A_ADC_CLMD + 3:
			value = value << ((offset - A_ADC_CLMD) << 3);
			offset = A_ADC_CLMD;
			break;
		case A_ADC_CLMS ... A_ADC_CLMS + 3:
			value = value << ((offset - A_ADC_CLMS) << 3);
			offset = A_ADC_CLMS;
			break;
		case A_ADC_CLM4 ... A_ADC_CLM4 + 3:
			value = value << ((offset - A_ADC_CLM4) << 3);
			offset = A_ADC_CLM4;
			break;
		case A_ADC_CLM3 ... A_ADC_CLM3 + 3:
			value = value << ((offset - A_ADC_CLM3) << 3);
			offset = A_ADC_CLM3;
			break;
		case A_ADC_CLM2 ... A_ADC_CLM2 + 3:
			value = value << ((offset - A_ADC_CLM2) << 3);
			offset = A_ADC_CLM2;
			break;
		case A_ADC_CLM1 ... A_ADC_CLM1 + 3:
			value = value << ((offset - A_ADC_CLM1) << 3);
			offset = A_ADC_CLM1;
			break;
		case A_ADC_CLM0 ... A_ADC_CLM0 + 3:
			value = value << ((offset - A_ADC_CLM0) << 3);
			offset = A_ADC_CLM0;
			break;
		case A_ADC_SC1A ... A_ADC_SC1A + 3:
			value = value << ((offset - A_ADC_SC1A) << 3);
			offset = A_ADC_SC1A;
			break;
		case A_ADC_SC1B ... A_ADC_SC1B + 3:
			value = value << ((offset - A_ADC_SC1B) << 3);
			offset = A_ADC_SC1B;
			break;
		case A_ADC_CV1 ... A_ADC_CV1 + 3:
			value = value << ((offset - A_ADC_CV1) << 3);
			offset = A_ADC_CV1;
			break;
		case A_ADC_CV2 ... A_ADC_CV2 + 3:
			value = value << ((offset - A_ADC_CV2) << 3);
			offset = A_ADC_CV2;
			break;

		default: break;
	}

	switch (offset) {
		case A_ADC_CFG1:
			t->CFG1 = value;
			break;
		case A_ADC_CFG2:
			t->CFG2 = value;
			break;
		case A_ADC_SC2:
			t->SC2 = value;
			break;
		case A_ADC_SC3:
			t->SC3 = value;
			break;
		case A_ADC_OFS:
			t->OFS = value;
			break;
		case A_ADC_PG:
			t->PG = value;
			break;
		case A_ADC_MG:
			t->MG = value;
			break;
		case A_ADC_CLPD:
			t->CLPD = value;
			break;
		case A_ADC_CLPS:
			t->CLPS = value;
			break;
		case A_ADC_CLP4:
			t->CLP4 = value;
			break;
		case A_ADC_CLP3:
			t->CLP3 = value;
			break;
		case A_ADC_CLP2:
			t->CLP2 = value;
			break;
		case A_ADC_CLP1:
			t->CLP1 = value;
			break;
		case A_ADC_CLP0:
			t->CLP0 = value;
			break;
		case A_ADC_CLMD:
			t->CLMD = value;
			break;
		case A_ADC_CLMS:
			t->CLMS = value;
			break;
		case A_ADC_CLM4:
			t->CLM4 = value;
			break;
		case A_ADC_CLM3:
			t->CLM3 = value;
			break;
		case A_ADC_CLM2:
			t->CLM2 = value;
			break;
		case A_ADC_CLM1:
			t->CLM1 = value;
			break;
		case A_ADC_CLM0:
			t->CLM0 = value;
			break;
		case A_ADC_SC1A:
			t->SC1A = value;
			break;
		case A_ADC_SC1B:
			t->SC1B = value;
			break;
		case A_ADC_CV1:
			t->CV1 = value;
			break;
		case A_ADC_CV2:
			t->CV2 = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMKL25Z ADC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmkl25z_adc_ops = {
	.read = frdmkl25z_adc_read,
	.write = frdmkl25z_adc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmkl25z_adc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMKL25ZADC *t = FRDMKL25Z_ADC(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmkl25z_adc_ops, t, "frdmkl25z-adc", FRDMKL25Z_ADC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void frdmkl25z_adc_realize(DeviceState *dev, Error **errp) {
	return;

}

static void frdmkl25z_adc_reset_enter(Object *obj, ResetType type) {
	FRDMKL25ZADC *t = FRDMKL25Z_ADC(obj);
	frdmkl25z_adc_register_reset(t);
}

static const VMStateDescription frdmkl25z_adc_vmstate = {
	.name = "frdmkl25z-adc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CFG1, FRDMKL25ZADC),
		VMSTATE_UINT32(CFG2, FRDMKL25ZADC),
		VMSTATE_UINT32(SC2, FRDMKL25ZADC),
		VMSTATE_UINT32(SC3, FRDMKL25ZADC),
		VMSTATE_UINT32(OFS, FRDMKL25ZADC),
		VMSTATE_UINT32(PG, FRDMKL25ZADC),
		VMSTATE_UINT32(MG, FRDMKL25ZADC),
		VMSTATE_UINT32(CLPD, FRDMKL25ZADC),
		VMSTATE_UINT32(CLPS, FRDMKL25ZADC),
		VMSTATE_UINT32(CLP4, FRDMKL25ZADC),
		VMSTATE_UINT32(CLP3, FRDMKL25ZADC),
		VMSTATE_UINT32(CLP2, FRDMKL25ZADC),
		VMSTATE_UINT32(CLP1, FRDMKL25ZADC),
		VMSTATE_UINT32(CLP0, FRDMKL25ZADC),
		VMSTATE_UINT32(CLMD, FRDMKL25ZADC),
		VMSTATE_UINT32(CLMS, FRDMKL25ZADC),
		VMSTATE_UINT32(CLM4, FRDMKL25ZADC),
		VMSTATE_UINT32(CLM3, FRDMKL25ZADC),
		VMSTATE_UINT32(CLM2, FRDMKL25ZADC),
		VMSTATE_UINT32(CLM1, FRDMKL25ZADC),
		VMSTATE_UINT32(CLM0, FRDMKL25ZADC),
		VMSTATE_UINT32(SC1A, FRDMKL25ZADC),
		VMSTATE_UINT32(SC1B, FRDMKL25ZADC),
		VMSTATE_UINT32(RA, FRDMKL25ZADC),
		VMSTATE_UINT32(RB, FRDMKL25ZADC),
		VMSTATE_UINT32(CV1, FRDMKL25ZADC),
		VMSTATE_UINT32(CV2, FRDMKL25ZADC),

		VMSTATE_END_OF_LIST()
	}
};

static void frdmkl25z_adc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmkl25z_adc_vmstate;
	dc->realize = frdmkl25z_adc_realize;
	rc->phases.enter = frdmkl25z_adc_reset_enter;

}

static const TypeInfo frdmkl25z_adc_info = {
	.name = TYPE_FRDMKL25Z_ADC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMKL25ZADC),
	.instance_init = frdmkl25z_adc_init,
	.class_init = frdmkl25z_adc_class_init,
};

static void frdmkl25z_adc_register_types(void) {
	type_register_static(&frdmkl25z_adc_info);
}

type_init(frdmkl25z_adc_register_types);

#define TYPE_FRDMKL25Z_DAC "frdmkl25z-dac"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMKL25ZDAC, FRDMKL25Z_DAC)

struct FRDMKL25ZDAC {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint8_t SR;
	uint8_t C0;
	uint8_t C1;
	uint8_t C2;
	uint8_t DAT0L;
	uint8_t DAT1L;
	uint8_t DAT0H;
	uint8_t DAT1H;
	/* base */
	uint32_t base;

};
#define FRDMKL25Z_DAC_SIZE				0x24

REG8(DAC_SR, 0x20)
	FIELD(DAC_SR, DACBFRPBF, 0, 1)
	FIELD(DAC_SR, DACBFRPTF, 1, 1)
REG8(DAC_C0, 0x21)
	FIELD(DAC_C0, DACBBIEN, 0, 1)
	FIELD(DAC_C0, DACBTIEN, 1, 1)
	FIELD(DAC_C0, LPEN, 3, 1)
	FIELD(DAC_C0, DACSWTRG, 4, 1)
	FIELD(DAC_C0, DACTRGSEL, 5, 1)
	FIELD(DAC_C0, DACRFS, 6, 1)
	FIELD(DAC_C0, DACEN, 7, 1)
REG8(DAC_C1, 0x22)
	FIELD(DAC_C1, DACBFEN, 0, 1)
	FIELD(DAC_C1, DACBFMD, 2, 1)
	FIELD(DAC_C1, DMAEN, 7, 1)
REG8(DAC_C2, 0x23)
	FIELD(DAC_C2, DACBFUP, 0, 1)
	FIELD(DAC_C2, DACBFRP, 4, 1)
REG8(DAC_DAT0L, 0x0)
	FIELD(DAC_DAT0L, DATA0, 0, 8)
REG8(DAC_DAT1L, 0x2)
	FIELD(DAC_DAT1L, DATA0, 0, 8)
REG8(DAC_DAT0H, 0x1)
	FIELD(DAC_DAT0H, DATA1, 0, 4)
REG8(DAC_DAT1H, 0x3)
	FIELD(DAC_DAT1H, DATA1, 0, 4)


static void frdmkl25z_dac_register_reset(FRDMKL25ZDAC *t) {
	t->SR = 0x2;
	t->C0 = 0x0;
	t->C1 = 0x0;
	t->C2 = 0x1;
	t->DAT0L = 0x0;
	t->DAT1L = 0x0;
	t->DAT0H = 0x0;
	t->DAT1H = 0x0;

}

static uint64_t frdmkl25z_dac_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMKL25ZDAC *t = FRDMKL25Z_DAC(opaque);
	uint64_t ret;

	switch (offset) {
		case A_DAC_SR:
			ret = t->SR;
			break;
		case A_DAC_C0:
			ret = t->C0;
			break;
		case A_DAC_C1:
			ret = t->C1;
			break;
		case A_DAC_C2:
			ret = t->C2;
			break;
		case A_DAC_DAT0L:
			ret = t->DAT0L;
			break;
		case A_DAC_DAT1L:
			ret = t->DAT1L;
			break;
		case A_DAC_DAT0H:
			ret = t->DAT0H;
			break;
		case A_DAC_DAT1H:
			ret = t->DAT1H;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMKL25Z DAC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmkl25z_dac_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMKL25ZDAC *t = FRDMKL25Z_DAC(opaque);


	switch (offset) {

		default: break;
	}

	switch (offset) {
		case A_DAC_SR:
			t->SR = value;
			break;
		case A_DAC_C0:
			t->C0 = value;
			break;
		case A_DAC_C1:
			t->C1 = value;
			break;
		case A_DAC_C2:
			t->C2 = value;
			break;
		case A_DAC_DAT0L:
			t->DAT0L = value;
			break;
		case A_DAC_DAT1L:
			t->DAT1L = value;
			break;
		case A_DAC_DAT0H:
			t->DAT0H = value;
			break;
		case A_DAC_DAT1H:
			t->DAT1H = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMKL25Z DAC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmkl25z_dac_ops = {
	.read = frdmkl25z_dac_read,
	.write = frdmkl25z_dac_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmkl25z_dac_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMKL25ZDAC *t = FRDMKL25Z_DAC(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmkl25z_dac_ops, t, "frdmkl25z-dac", FRDMKL25Z_DAC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void frdmkl25z_dac_realize(DeviceState *dev, Error **errp) {
	return;

}

static void frdmkl25z_dac_reset_enter(Object *obj, ResetType type) {
	FRDMKL25ZDAC *t = FRDMKL25Z_DAC(obj);
	frdmkl25z_dac_register_reset(t);
}

static const VMStateDescription frdmkl25z_dac_vmstate = {
	.name = "frdmkl25z-dac",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT8(SR, FRDMKL25ZDAC),
		VMSTATE_UINT8(C0, FRDMKL25ZDAC),
		VMSTATE_UINT8(C1, FRDMKL25ZDAC),
		VMSTATE_UINT8(C2, FRDMKL25ZDAC),
		VMSTATE_UINT8(DAT0L, FRDMKL25ZDAC),
		VMSTATE_UINT8(DAT1L, FRDMKL25ZDAC),
		VMSTATE_UINT8(DAT0H, FRDMKL25ZDAC),
		VMSTATE_UINT8(DAT1H, FRDMKL25ZDAC),

		VMSTATE_END_OF_LIST()
	}
};

static void frdmkl25z_dac_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmkl25z_dac_vmstate;
	dc->realize = frdmkl25z_dac_realize;
	rc->phases.enter = frdmkl25z_dac_reset_enter;

}

static const TypeInfo frdmkl25z_dac_info = {
	.name = TYPE_FRDMKL25Z_DAC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMKL25ZDAC),
	.instance_init = frdmkl25z_dac_init,
	.class_init = frdmkl25z_dac_class_init,
};

static void frdmkl25z_dac_register_types(void) {
	type_register_static(&frdmkl25z_dac_info);
}

type_init(frdmkl25z_dac_register_types);

#define TYPE_FRDMKL25Z_PIT "frdmkl25z-pit"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMKL25ZPIT, FRDMKL25Z_PIT)

struct FRDMKL25ZPIT {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t MCR;
	uint32_t LTMR64H;
	uint32_t LTMR64L;
	uint32_t LDVAL0;
	uint32_t LDVAL1;
	uint32_t CVAL0;
	uint32_t CVAL1;
	uint32_t TCTRL0;
	uint32_t TCTRL1;
	uint32_t TFLG0;
	uint32_t TFLG1;
	/* base */
	uint32_t base;

};
#define FRDMKL25Z_PIT_SIZE				0x120

REG32(PIT_MCR, 0x0)
	FIELD(PIT_MCR, FRZ, 0, 1)
	FIELD(PIT_MCR, MDIS, 1, 1)
REG32(PIT_LTMR64H, 0xe0)
	FIELD(PIT_LTMR64H, LTH, 0, 32)
REG32(PIT_LTMR64L, 0xe4)
	FIELD(PIT_LTMR64L, LTL, 0, 32)
REG32(PIT_LDVAL0, 0x100)
	FIELD(PIT_LDVAL0, TSV, 0, 32)
REG32(PIT_LDVAL1, 0x110)
	FIELD(PIT_LDVAL1, TSV, 0, 32)
REG32(PIT_CVAL0, 0x104)
	FIELD(PIT_CVAL0, TVL, 0, 32)
REG32(PIT_CVAL1, 0x114)
	FIELD(PIT_CVAL1, TVL, 0, 32)
REG32(PIT_TCTRL0, 0x108)
	FIELD(PIT_TCTRL0, TEN, 0, 1)
	FIELD(PIT_TCTRL0, TIE, 1, 1)
	FIELD(PIT_TCTRL0, CHN, 2, 1)
REG32(PIT_TCTRL1, 0x118)
	FIELD(PIT_TCTRL1, TEN, 0, 1)
	FIELD(PIT_TCTRL1, TIE, 1, 1)
	FIELD(PIT_TCTRL1, CHN, 2, 1)
REG32(PIT_TFLG0, 0x10c)
	FIELD(PIT_TFLG0, TIF, 0, 1)
REG32(PIT_TFLG1, 0x11c)
	FIELD(PIT_TFLG1, TIF, 0, 1)


static void frdmkl25z_pit_register_reset(FRDMKL25ZPIT *t) {
	t->MCR = 0x2;
	t->LTMR64H = 0x0;
	t->LTMR64L = 0x0;
	t->LDVAL0 = 0x0;
	t->LDVAL1 = 0x0;
	t->CVAL0 = 0x0;
	t->CVAL1 = 0x0;
	t->TCTRL0 = 0x0;
	t->TCTRL1 = 0x0;
	t->TFLG0 = 0x0;
	t->TFLG1 = 0x0;

}

static uint64_t frdmkl25z_pit_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMKL25ZPIT *t = FRDMKL25Z_PIT(opaque);
	uint64_t ret;

	switch (offset) {
		case A_PIT_MCR:
			ret = t->MCR;
			break;
		case A_PIT_LTMR64H:
			ret = t->LTMR64H;
			break;
		case A_PIT_LTMR64L:
			ret = t->LTMR64L;
			break;
		case A_PIT_LDVAL0:
			ret = t->LDVAL0;
			break;
		case A_PIT_LDVAL1:
			ret = t->LDVAL1;
			break;
		case A_PIT_CVAL0:
			ret = t->CVAL0;
			break;
		case A_PIT_CVAL1:
			ret = t->CVAL1;
			break;
		case A_PIT_TCTRL0:
			ret = t->TCTRL0;
			break;
		case A_PIT_TCTRL1:
			ret = t->TCTRL1;
			break;
		case A_PIT_TFLG0:
			ret = t->TFLG0;
			break;
		case A_PIT_TFLG1:
			ret = t->TFLG1;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMKL25Z PIT read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmkl25z_pit_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMKL25ZPIT *t = FRDMKL25Z_PIT(opaque);


	switch (offset) {
		case A_PIT_MCR ... A_PIT_MCR + 3:
			value = value << ((offset - A_PIT_MCR) << 3);
			offset = A_PIT_MCR;
			break;
		case A_PIT_LDVAL0 ... A_PIT_LDVAL0 + 3:
			value = value << ((offset - A_PIT_LDVAL0) << 3);
			offset = A_PIT_LDVAL0;
			break;
		case A_PIT_LDVAL1 ... A_PIT_LDVAL1 + 3:
			value = value << ((offset - A_PIT_LDVAL1) << 3);
			offset = A_PIT_LDVAL1;
			break;
		case A_PIT_TCTRL0 ... A_PIT_TCTRL0 + 3:
			value = value << ((offset - A_PIT_TCTRL0) << 3);
			offset = A_PIT_TCTRL0;
			break;
		case A_PIT_TCTRL1 ... A_PIT_TCTRL1 + 3:
			value = value << ((offset - A_PIT_TCTRL1) << 3);
			offset = A_PIT_TCTRL1;
			break;
		case A_PIT_TFLG0 ... A_PIT_TFLG0 + 3:
			value = value << ((offset - A_PIT_TFLG0) << 3);
			offset = A_PIT_TFLG0;
			break;
		case A_PIT_TFLG1 ... A_PIT_TFLG1 + 3:
			value = value << ((offset - A_PIT_TFLG1) << 3);
			offset = A_PIT_TFLG1;
			break;

		default: break;
	}

	switch (offset) {
		case A_PIT_MCR:
			t->MCR = value;
			break;
		case A_PIT_LDVAL0:
			t->LDVAL0 = value;
			break;
		case A_PIT_LDVAL1:
			t->LDVAL1 = value;
			break;
		case A_PIT_TCTRL0:
			t->TCTRL0 = value;
			break;
		case A_PIT_TCTRL1:
			t->TCTRL1 = value;
			break;
		case A_PIT_TFLG0:
			t->TFLG0 = value;
			break;
		case A_PIT_TFLG1:
			t->TFLG1 = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMKL25Z PIT write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmkl25z_pit_ops = {
	.read = frdmkl25z_pit_read,
	.write = frdmkl25z_pit_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmkl25z_pit_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMKL25ZPIT *t = FRDMKL25Z_PIT(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmkl25z_pit_ops, t, "frdmkl25z-pit", FRDMKL25Z_PIT_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void frdmkl25z_pit_realize(DeviceState *dev, Error **errp) {
	return;

}

static void frdmkl25z_pit_reset_enter(Object *obj, ResetType type) {
	FRDMKL25ZPIT *t = FRDMKL25Z_PIT(obj);
	frdmkl25z_pit_register_reset(t);
}

static const VMStateDescription frdmkl25z_pit_vmstate = {
	.name = "frdmkl25z-pit",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(MCR, FRDMKL25ZPIT),
		VMSTATE_UINT32(LTMR64H, FRDMKL25ZPIT),
		VMSTATE_UINT32(LTMR64L, FRDMKL25ZPIT),
		VMSTATE_UINT32(LDVAL0, FRDMKL25ZPIT),
		VMSTATE_UINT32(LDVAL1, FRDMKL25ZPIT),
		VMSTATE_UINT32(CVAL0, FRDMKL25ZPIT),
		VMSTATE_UINT32(CVAL1, FRDMKL25ZPIT),
		VMSTATE_UINT32(TCTRL0, FRDMKL25ZPIT),
		VMSTATE_UINT32(TCTRL1, FRDMKL25ZPIT),
		VMSTATE_UINT32(TFLG0, FRDMKL25ZPIT),
		VMSTATE_UINT32(TFLG1, FRDMKL25ZPIT),

		VMSTATE_END_OF_LIST()
	}
};

static void frdmkl25z_pit_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmkl25z_pit_vmstate;
	dc->realize = frdmkl25z_pit_realize;
	rc->phases.enter = frdmkl25z_pit_reset_enter;

}

static const TypeInfo frdmkl25z_pit_info = {
	.name = TYPE_FRDMKL25Z_PIT,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMKL25ZPIT),
	.instance_init = frdmkl25z_pit_init,
	.class_init = frdmkl25z_pit_class_init,
};

static void frdmkl25z_pit_register_types(void) {
	type_register_static(&frdmkl25z_pit_info);
}

type_init(frdmkl25z_pit_register_types);

#define TYPE_NXP_FRDM_KL25Z_MACHINE MACHINE_TYPE_NAME("nxp_frdm_kl25z")
OBJECT_DECLARE_TYPE(NXP_FRDM_KL25ZMachineState, NXP_FRDM_KL25ZMachineClass, NXP_FRDM_KL25Z_MACHINE)

struct NXP_FRDM_KL25ZMachineClass {
	MachineClass parent;
};

struct NXP_FRDM_KL25ZMachineState {
	MachineState parent;
	ARMv7MState armv7m;
};

static void nxp_frdm_kl25z_periph_init(MachineState *machine) {
	NXP_FRDM_KL25ZMachineState *sms = NXP_FRDM_KL25Z_MACHINE(machine);
	FRDMKL25ZUART1 *p0 = g_new(FRDMKL25ZUART1, 1);
	object_initialize_child(OBJECT(sms), "UART1", p0, TYPE_FRDMKL25Z_UART1);
	qdev_prop_set_chr(DEVICE(p0), "chardev", qemu_chr_new("soc-uart1", "chardev:uart1", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p0), &error_fatal);
	p0->base = 0x4006b000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p0), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 13));
	sysbus_mmio_map(SYS_BUS_DEVICE(p0), 0, 0x4006b000);

	FRDMKL25ZUART1 *p1 = g_new(FRDMKL25ZUART1, 1);
	object_initialize_child(OBJECT(sms), "UART2", p1, TYPE_FRDMKL25Z_UART1);
	qdev_prop_set_chr(DEVICE(p1), "chardev", qemu_chr_new("soc-uart2", "chardev:uart2", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p1), &error_fatal);
	p1->base = 0x4006c000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p1), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 14));
	sysbus_mmio_map(SYS_BUS_DEVICE(p1), 0, 0x4006c000);

	FRDMKL25ZUART *p2 = g_new(FRDMKL25ZUART, 1);
	object_initialize_child(OBJECT(sms), "UART0", p2, TYPE_FRDMKL25Z_UART);
	qdev_prop_set_chr(DEVICE(p2), "chardev", qemu_chr_new("soc-uart0", "chardev:uart0", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p2), &error_fatal);
	p2->base = 0x4006a000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p2), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 12));
	sysbus_mmio_map(SYS_BUS_DEVICE(p2), 0, 0x4006a000);

	FRDMKL25ZSPI *p3 = g_new(FRDMKL25ZSPI, 1);
	object_initialize_child(OBJECT(sms), "SPI0", p3, TYPE_FRDMKL25Z_SPI);
	qdev_prop_set_chr(DEVICE(p3), "chardev", qemu_chr_new("soc-spi0", "chardev:spi0", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p3), &error_fatal);
	p3->base = 0x40076000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p3), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 10));
	sysbus_mmio_map(SYS_BUS_DEVICE(p3), 0, 0x40076000);

	FRDMKL25ZSPI *p4 = g_new(FRDMKL25ZSPI, 1);
	object_initialize_child(OBJECT(sms), "SPI1", p4, TYPE_FRDMKL25Z_SPI);
	qdev_prop_set_chr(DEVICE(p4), "chardev", qemu_chr_new("soc-spi1", "chardev:spi1", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p4), &error_fatal);
	p4->base = 0x40077000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p4), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 11));
	sysbus_mmio_map(SYS_BUS_DEVICE(p4), 0, 0x40077000);

	FRDMKL25ZSIM *p5 = g_new(FRDMKL25ZSIM, 1);
	object_initialize_child(OBJECT(sms), "SIM", p5, TYPE_FRDMKL25Z_SIM);
	sysbus_realize(SYS_BUS_DEVICE(p5), &error_fatal);
	p5->base = 0x40047000;
	sysbus_mmio_map(SYS_BUS_DEVICE(p5), 0, 0x40047000);

	FRDMKL25ZRTC *p6 = g_new(FRDMKL25ZRTC, 1);
	object_initialize_child(OBJECT(sms), "RTC", p6, TYPE_FRDMKL25Z_RTC);
	sysbus_realize(SYS_BUS_DEVICE(p6), &error_fatal);
	p6->base = 0x4003d000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p6), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 20));
	sysbus_connect_irq(SYS_BUS_DEVICE(p6), 1, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 21));
	sysbus_mmio_map(SYS_BUS_DEVICE(p6), 0, 0x4003d000);

	FRDMKL25ZPMC *p7 = g_new(FRDMKL25ZPMC, 1);
	object_initialize_child(OBJECT(sms), "PMC", p7, TYPE_FRDMKL25Z_PMC);
	sysbus_realize(SYS_BUS_DEVICE(p7), &error_fatal);
	p7->base = 0x4007d000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p7), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 6));
	sysbus_mmio_map(SYS_BUS_DEVICE(p7), 0, 0x4007d000);

	FRDMKL25ZOSC0 *p8 = g_new(FRDMKL25ZOSC0, 1);
	object_initialize_child(OBJECT(sms), "OSC0", p8, TYPE_FRDMKL25Z_OSC0);
	sysbus_realize(SYS_BUS_DEVICE(p8), &error_fatal);
	p8->base = 0x40065000;
	sysbus_mmio_map(SYS_BUS_DEVICE(p8), 0, 0x40065000);

	FRDMKL25ZMCG *p9 = g_new(FRDMKL25ZMCG, 1);
	object_initialize_child(OBJECT(sms), "MCG", p9, TYPE_FRDMKL25Z_MCG);
	sysbus_realize(SYS_BUS_DEVICE(p9), &error_fatal);
	p9->base = 0x40064000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p9), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 27));
	sysbus_mmio_map(SYS_BUS_DEVICE(p9), 0, 0x40064000);

	FRDMKL25ZGPIO *p10 = g_new(FRDMKL25ZGPIO, 1);
	object_initialize_child(OBJECT(sms), "GPIOA", p10, TYPE_FRDMKL25Z_GPIO);
	sysbus_realize(SYS_BUS_DEVICE(p10), &error_fatal);
	p10->base = 0x400ff000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p10), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 30));
	sysbus_mmio_map(SYS_BUS_DEVICE(p10), 0, 0x400ff000);

	FRDMKL25ZGPIO *p11 = g_new(FRDMKL25ZGPIO, 1);
	object_initialize_child(OBJECT(sms), "GPIOB", p11, TYPE_FRDMKL25Z_GPIO);
	sysbus_realize(SYS_BUS_DEVICE(p11), &error_fatal);
	p11->base = 0x400ff040;
	sysbus_mmio_map(SYS_BUS_DEVICE(p11), 0, 0x400ff040);

	FRDMKL25ZGPIO *p12 = g_new(FRDMKL25ZGPIO, 1);
	object_initialize_child(OBJECT(sms), "GPIOC", p12, TYPE_FRDMKL25Z_GPIO);
	sysbus_realize(SYS_BUS_DEVICE(p12), &error_fatal);
	p12->base = 0x400ff080;
	sysbus_mmio_map(SYS_BUS_DEVICE(p12), 0, 0x400ff080);

	FRDMKL25ZGPIO *p13 = g_new(FRDMKL25ZGPIO, 1);
	object_initialize_child(OBJECT(sms), "GPIOD", p13, TYPE_FRDMKL25Z_GPIO);
	sysbus_realize(SYS_BUS_DEVICE(p13), &error_fatal);
	p13->base = 0x400ff0c0;
	sysbus_connect_irq(SYS_BUS_DEVICE(p13), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 31));
	sysbus_mmio_map(SYS_BUS_DEVICE(p13), 0, 0x400ff0c0);

	FRDMKL25ZGPIO *p14 = g_new(FRDMKL25ZGPIO, 1);
	object_initialize_child(OBJECT(sms), "GPIOE", p14, TYPE_FRDMKL25Z_GPIO);
	sysbus_realize(SYS_BUS_DEVICE(p14), &error_fatal);
	p14->base = 0x400ff100;
	sysbus_mmio_map(SYS_BUS_DEVICE(p14), 0, 0x400ff100);

	FRDMKL25ZPORT *p15 = g_new(FRDMKL25ZPORT, 1);
	object_initialize_child(OBJECT(sms), "PORTA", p15, TYPE_FRDMKL25Z_PORT);
	qdev_prop_set_chr(DEVICE(p15), "chardev", qemu_chr_new("soc-porta", "chardev:porta", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p15), &error_fatal);
	p15->base = 0x40049000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p15), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 30));
	sysbus_mmio_map(SYS_BUS_DEVICE(p15), 0, 0x40049000);

	FRDMKL25ZPORT *p16 = g_new(FRDMKL25ZPORT, 1);
	object_initialize_child(OBJECT(sms), "PORTB", p16, TYPE_FRDMKL25Z_PORT);
	qdev_prop_set_chr(DEVICE(p16), "chardev", qemu_chr_new("soc-portb", "chardev:portb", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p16), &error_fatal);
	p16->base = 0x4004a000;
	sysbus_mmio_map(SYS_BUS_DEVICE(p16), 0, 0x4004a000);

	FRDMKL25ZPORT *p17 = g_new(FRDMKL25ZPORT, 1);
	object_initialize_child(OBJECT(sms), "PORTC", p17, TYPE_FRDMKL25Z_PORT);
	qdev_prop_set_chr(DEVICE(p17), "chardev", qemu_chr_new("soc-portc", "chardev:portc", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p17), &error_fatal);
	p17->base = 0x4004b000;
	sysbus_mmio_map(SYS_BUS_DEVICE(p17), 0, 0x4004b000);

	FRDMKL25ZPORT *p18 = g_new(FRDMKL25ZPORT, 1);
	object_initialize_child(OBJECT(sms), "PORTD", p18, TYPE_FRDMKL25Z_PORT);
	qdev_prop_set_chr(DEVICE(p18), "chardev", qemu_chr_new("soc-portd", "chardev:portd", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p18), &error_fatal);
	p18->base = 0x4004c000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p18), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 31));
	sysbus_mmio_map(SYS_BUS_DEVICE(p18), 0, 0x4004c000);

	FRDMKL25ZPORT *p19 = g_new(FRDMKL25ZPORT, 1);
	object_initialize_child(OBJECT(sms), "PORTE", p19, TYPE_FRDMKL25Z_PORT);
	qdev_prop_set_chr(DEVICE(p19), "chardev", qemu_chr_new("soc-porte", "chardev:porte", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p19), &error_fatal);
	p19->base = 0x4004d000;
	sysbus_mmio_map(SYS_BUS_DEVICE(p19), 0, 0x4004d000);

	FRDMKL25ZADC *p20 = g_new(FRDMKL25ZADC, 1);
	object_initialize_child(OBJECT(sms), "ADC0", p20, TYPE_FRDMKL25Z_ADC);
	sysbus_realize(SYS_BUS_DEVICE(p20), &error_fatal);
	p20->base = 0x4003b000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p20), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 15));
	sysbus_mmio_map(SYS_BUS_DEVICE(p20), 0, 0x4003b000);

	FRDMKL25ZDAC *p21 = g_new(FRDMKL25ZDAC, 1);
	object_initialize_child(OBJECT(sms), "DAC0", p21, TYPE_FRDMKL25Z_DAC);
	sysbus_realize(SYS_BUS_DEVICE(p21), &error_fatal);
	p21->base = 0x4003f000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p21), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 25));
	sysbus_mmio_map(SYS_BUS_DEVICE(p21), 0, 0x4003f000);

	FRDMKL25ZI2C *p22 = g_new(FRDMKL25ZI2C, 1);
	object_initialize_child(OBJECT(sms), "I2C0", p22, TYPE_FRDMKL25Z_I2C);
	qdev_prop_set_chr(DEVICE(p22), "chardev", qemu_chr_new("soc-i2c0", "chardev:i2c0", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p22), &error_fatal);
	p22->base = 0x40066000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p22), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 8));
	sysbus_mmio_map(SYS_BUS_DEVICE(p22), 0, 0x40066000);

	FRDMKL25ZPIT *p23 = g_new(FRDMKL25ZPIT, 1);
	object_initialize_child(OBJECT(sms), "PIT", p23, TYPE_FRDMKL25Z_PIT);
	sysbus_realize(SYS_BUS_DEVICE(p23), &error_fatal);
	p23->base = 0x40037000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p23), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 22));
	sysbus_mmio_map(SYS_BUS_DEVICE(p23), 0, 0x40037000);


}

static void nxp_frdm_kl25z_common_init(MachineState *machine) {
	NXP_FRDM_KL25ZMachineState *sms = NXP_FRDM_KL25Z_MACHINE(machine);
	MemoryRegion *sysmem = get_system_memory();
	MemoryRegion *mem;
	Clock *cpuclk = clock_new(OBJECT(sms), "SYSCLK");
	clock_set_hz(cpuclk, 48000000);
	Clock *refclk = clock_new(OBJECT(sms), "REFCLK");
	clock_set_hz(refclk, 48000000);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_rom(mem, NULL, "main-flash", 0x20000, &error_fatal);
	memory_region_add_subregion(sysmem, 0x0, mem);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_ram(mem, NULL, "sram_l", 0x1000, &error_fatal);
	memory_region_add_subregion(sysmem, 0x1ffff000, mem);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_ram(mem, NULL, "sram_u", 0x3000, &error_fatal);
	memory_region_add_subregion(sysmem, 0x20000000, mem);


	object_initialize_child(OBJECT(sms), "armv7m", &(sms->armv7m), TYPE_ARMV7M);
	qdev_prop_set_uint32(DEVICE(&(sms->armv7m)),"num-irq", 47);
	qdev_prop_set_string(DEVICE(&(sms->armv7m)), "cpu-type", machine->cpu_type);
	qdev_prop_set_bit(DEVICE(&(sms->armv7m)), "enable-bitband", false);
	qdev_connect_clock_in(DEVICE(&(sms->armv7m)), "cpuclk", cpuclk);
	qdev_connect_clock_in(DEVICE(&(sms->armv7m)), "refclk", refclk);
	qdev_prop_set_uint32(DEVICE(&(sms->armv7m)), "init-nsvtor", 0x0);
	object_property_set_link(OBJECT(&sms->armv7m), "memory", OBJECT(sysmem), &error_abort);
	sysbus_realize(SYS_BUS_DEVICE(&sms->armv7m), &error_fatal);

	nxp_frdm_kl25z_periph_init(machine);

	armv7m_load_kernel(ARM_CPU(first_cpu), machine->kernel_filename, 0x0, 0x20000);
}

static void nxp_frdm_kl25z_class_init(ObjectClass *oc, void *data) {
	MachineClass *mc = MACHINE_CLASS(oc);
	mc->desc = "nxp_frdm_kl25z";
	mc->init = nxp_frdm_kl25z_common_init;
	mc->max_cpus = 1;
	mc->default_cpus = 1;
	mc->default_cpu_type = ARM_CPU_TYPE_NAME("cortex-m0");
}

static const TypeInfo nxp_frdm_kl25z_info = {
	.name = TYPE_NXP_FRDM_KL25Z_MACHINE,
	.parent = TYPE_MACHINE,
	.instance_size = sizeof(NXP_FRDM_KL25ZMachineState),
	.class_size = sizeof(NXP_FRDM_KL25ZMachineClass),
	.class_init = nxp_frdm_kl25z_class_init
};

static void nxp_frdm_kl25z_machine_init(void) {
	type_register_static(&nxp_frdm_kl25z_info);
}

type_init(nxp_frdm_kl25z_machine_init);
