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


#define TYPE_LPC51U68_RTC "lpc51u68-rtc"
OBJECT_DECLARE_SIMPLE_TYPE(LPC51U68RTC, LPC51U68_RTC)

struct LPC51U68RTC {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t CTRL;
	uint32_t MATCH;
	uint32_t COUNT;
	uint32_t WAKE;
	/* base */
	uint32_t base;

};
#define LPC51U68_RTC_SIZE				0x10

REG32(RTC_CTRL, 0x0)
	FIELD(RTC_CTRL, SWRESET, 0, 1)
	FIELD(RTC_CTRL, ALARM1HZ, 2, 1)
	FIELD(RTC_CTRL, WAKE1KHZ, 3, 1)
	FIELD(RTC_CTRL, ALARMDPD_EN, 4, 1)
	FIELD(RTC_CTRL, WAKEDPD_EN, 5, 1)
	FIELD(RTC_CTRL, RTC1KHZ_EN, 6, 1)
	FIELD(RTC_CTRL, RTC_EN, 7, 1)
	FIELD(RTC_CTRL, RTC_OSC_PD, 8, 1)
REG32(RTC_MATCH, 0x4)
	FIELD(RTC_MATCH, MATVAL, 0, 32)
REG32(RTC_COUNT, 0x8)
	FIELD(RTC_COUNT, VAL, 0, 32)
REG32(RTC_WAKE, 0xc)
	FIELD(RTC_WAKE, VAL, 0, 16)


static void lpc51u68_rtc_register_reset(LPC51U68RTC *t) {
	t->CTRL = 0x1;
	t->MATCH = 0xffffffff;
	t->COUNT = 0x0;
	t->WAKE = 0x0;

}

static uint64_t lpc51u68_rtc_read(void *opaque, hwaddr offset, unsigned size) {
	LPC51U68RTC *t = LPC51U68_RTC(opaque);
	uint64_t ret;

	switch (offset) {
		case A_RTC_CTRL:
			ret = t->CTRL;
			break;
		case A_RTC_MATCH:
			ret = t->MATCH;
			break;
		case A_RTC_COUNT:
			ret = t->COUNT;
			break;
		case A_RTC_WAKE:
			ret = t->WAKE;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "LPC51U68 RTC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void lpc51u68_rtc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	LPC51U68RTC *t = LPC51U68_RTC(opaque);


	switch (offset) {
		case A_RTC_CTRL ... A_RTC_CTRL + 3:
			value = value << ((offset - A_RTC_CTRL) << 3);
			offset = A_RTC_CTRL;
			break;
		case A_RTC_MATCH ... A_RTC_MATCH + 3:
			value = value << ((offset - A_RTC_MATCH) << 3);
			offset = A_RTC_MATCH;
			break;
		case A_RTC_COUNT ... A_RTC_COUNT + 3:
			value = value << ((offset - A_RTC_COUNT) << 3);
			offset = A_RTC_COUNT;
			break;
		case A_RTC_WAKE ... A_RTC_WAKE + 3:
			value = value << ((offset - A_RTC_WAKE) << 3);
			offset = A_RTC_WAKE;
			break;

		default: break;
	}

	switch (offset) {
		case A_RTC_CTRL:
			t->CTRL = value;
			break;
		case A_RTC_MATCH:
			t->MATCH = value;
			break;
		case A_RTC_COUNT:
			t->COUNT = value;
			break;
		case A_RTC_WAKE:
			t->WAKE = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "LPC51U68 RTC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps lpc51u68_rtc_ops = {
	.read = lpc51u68_rtc_read,
	.write = lpc51u68_rtc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void lpc51u68_rtc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	LPC51U68RTC *t = LPC51U68_RTC(obj);
	memory_region_init_io(&(t->iomem), obj, &lpc51u68_rtc_ops, t, "lpc51u68-rtc", LPC51U68_RTC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void lpc51u68_rtc_realize(DeviceState *dev, Error **errp) {
	return;

}

static void lpc51u68_rtc_reset_enter(Object *obj, ResetType type) {
	LPC51U68RTC *t = LPC51U68_RTC(obj);
	lpc51u68_rtc_register_reset(t);
}

static const VMStateDescription lpc51u68_rtc_vmstate = {
	.name = "lpc51u68-rtc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CTRL, LPC51U68RTC),
		VMSTATE_UINT32(MATCH, LPC51U68RTC),
		VMSTATE_UINT32(COUNT, LPC51U68RTC),
		VMSTATE_UINT32(WAKE, LPC51U68RTC),

		VMSTATE_END_OF_LIST()
	}
};

static void lpc51u68_rtc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &lpc51u68_rtc_vmstate;
	dc->realize = lpc51u68_rtc_realize;
	rc->phases.enter = lpc51u68_rtc_reset_enter;

}

static const TypeInfo lpc51u68_rtc_info = {
	.name = TYPE_LPC51U68_RTC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(LPC51U68RTC),
	.instance_init = lpc51u68_rtc_init,
	.class_init = lpc51u68_rtc_class_init,
};

static void lpc51u68_rtc_register_types(void) {
	type_register_static(&lpc51u68_rtc_info);
}

type_init(lpc51u68_rtc_register_types);

#define TYPE_LPC51U68_GPIO "lpc51u68-gpio"
OBJECT_DECLARE_SIMPLE_TYPE(LPC51U68GPIO, LPC51U68_GPIO)

struct LPC51U68GPIO {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/*registers*/
	uint8_t B0;
	uint8_t B1;
	uint8_t B2;
	uint8_t B3;
	uint8_t B4;
	uint8_t B5;
	uint8_t B6;
	uint8_t B7;
	uint8_t B8;
	uint8_t B9;
	uint8_t B10;
	uint8_t B11;
	uint8_t B12;
	uint8_t B13;
	uint8_t B14;
	uint8_t B15;
	uint8_t B16;
	uint8_t B17;
	uint8_t B18;
	uint8_t B19;
	uint8_t B20;
	uint8_t B21;
	uint8_t B22;
	uint8_t B23;
	uint8_t B24;
	uint8_t B25;
	uint8_t B26;
	uint8_t B27;
	uint8_t B28;
	uint8_t B29;
	uint8_t B30;
	uint8_t B31;
	uint8_t B32;
	uint8_t B33;
	uint8_t B34;
	uint8_t B35;
	uint8_t B36;
	uint8_t B37;
	uint8_t B38;
	uint8_t B39;
	uint8_t B40;
	uint8_t B41;
	uint8_t B42;
	uint8_t B43;
	uint8_t B44;
	uint8_t B45;
	uint8_t B46;
	uint8_t B47;
	uint8_t B48;
	uint8_t B49;
	uint32_t W0;
	uint32_t W1;
	uint32_t W2;
	uint32_t W3;
	uint32_t W4;
	uint32_t W5;
	uint32_t W6;
	uint32_t W7;
	uint32_t W8;
	uint32_t W9;
	uint32_t W10;
	uint32_t W11;
	uint32_t W12;
	uint32_t W13;
	uint32_t W14;
	uint32_t W15;
	uint32_t W16;
	uint32_t W17;
	uint32_t W18;
	uint32_t W19;
	uint32_t W20;
	uint32_t W21;
	uint32_t W22;
	uint32_t W23;
	uint32_t W24;
	uint32_t W25;
	uint32_t W26;
	uint32_t W27;
	uint32_t W28;
	uint32_t W29;
	uint32_t W30;
	uint32_t W31;
	uint32_t W32;
	uint32_t W33;
	uint32_t W34;
	uint32_t W35;
	uint32_t W36;
	uint32_t W37;
	uint32_t W38;
	uint32_t W39;
	uint32_t W40;
	uint32_t W41;
	uint32_t W42;
	uint32_t W43;
	uint32_t W44;
	uint32_t W45;
	uint32_t W46;
	uint32_t W47;
	uint32_t W48;
	uint32_t W49;
	uint32_t DIR0;
	uint32_t DIR1;
	uint32_t MASK0;
	uint32_t MASK1;
	uint32_t PIN0;
	uint32_t PIN1;
	uint32_t MPIN0;
	uint32_t MPIN1;
	uint32_t SET0;
	uint32_t SET1;
	uint32_t CLR0;
	uint32_t CLR1;
	uint32_t NOT0;
	uint32_t NOT1;
	uint32_t DIRSET0;
	uint32_t DIRSET1;
	uint32_t DIRCLR0;
	uint32_t DIRCLR1;
	uint32_t DIRNOT0;
	uint32_t DIRNOT1;
	/* base */
	uint32_t base;

};
#define LPC51U68_GPIO_SIZE				0x2488

REG8(GPIO_B0, 0x0)
	FIELD(GPIO_B0, PBYTE, 0, 1)
REG8(GPIO_B1, 0x1)
	FIELD(GPIO_B1, PBYTE, 0, 1)
REG8(GPIO_B2, 0x2)
	FIELD(GPIO_B2, PBYTE, 0, 1)
REG8(GPIO_B3, 0x3)
	FIELD(GPIO_B3, PBYTE, 0, 1)
REG8(GPIO_B4, 0x4)
	FIELD(GPIO_B4, PBYTE, 0, 1)
REG8(GPIO_B5, 0x5)
	FIELD(GPIO_B5, PBYTE, 0, 1)
REG8(GPIO_B6, 0x6)
	FIELD(GPIO_B6, PBYTE, 0, 1)
REG8(GPIO_B7, 0x7)
	FIELD(GPIO_B7, PBYTE, 0, 1)
REG8(GPIO_B8, 0x8)
	FIELD(GPIO_B8, PBYTE, 0, 1)
REG8(GPIO_B9, 0x9)
	FIELD(GPIO_B9, PBYTE, 0, 1)
REG8(GPIO_B10, 0xa)
	FIELD(GPIO_B10, PBYTE, 0, 1)
REG8(GPIO_B11, 0xb)
	FIELD(GPIO_B11, PBYTE, 0, 1)
REG8(GPIO_B12, 0xc)
	FIELD(GPIO_B12, PBYTE, 0, 1)
REG8(GPIO_B13, 0xd)
	FIELD(GPIO_B13, PBYTE, 0, 1)
REG8(GPIO_B14, 0xe)
	FIELD(GPIO_B14, PBYTE, 0, 1)
REG8(GPIO_B15, 0xf)
	FIELD(GPIO_B15, PBYTE, 0, 1)
REG8(GPIO_B16, 0x10)
	FIELD(GPIO_B16, PBYTE, 0, 1)
REG8(GPIO_B17, 0x11)
	FIELD(GPIO_B17, PBYTE, 0, 1)
REG8(GPIO_B18, 0x12)
	FIELD(GPIO_B18, PBYTE, 0, 1)
REG8(GPIO_B19, 0x13)
	FIELD(GPIO_B19, PBYTE, 0, 1)
REG8(GPIO_B20, 0x14)
	FIELD(GPIO_B20, PBYTE, 0, 1)
REG8(GPIO_B21, 0x15)
	FIELD(GPIO_B21, PBYTE, 0, 1)
REG8(GPIO_B22, 0x16)
	FIELD(GPIO_B22, PBYTE, 0, 1)
REG8(GPIO_B23, 0x17)
	FIELD(GPIO_B23, PBYTE, 0, 1)
REG8(GPIO_B24, 0x18)
	FIELD(GPIO_B24, PBYTE, 0, 1)
REG8(GPIO_B25, 0x19)
	FIELD(GPIO_B25, PBYTE, 0, 1)
REG8(GPIO_B26, 0x1a)
	FIELD(GPIO_B26, PBYTE, 0, 1)
REG8(GPIO_B27, 0x1b)
	FIELD(GPIO_B27, PBYTE, 0, 1)
REG8(GPIO_B28, 0x1c)
	FIELD(GPIO_B28, PBYTE, 0, 1)
REG8(GPIO_B29, 0x1d)
	FIELD(GPIO_B29, PBYTE, 0, 1)
REG8(GPIO_B30, 0x1e)
	FIELD(GPIO_B30, PBYTE, 0, 1)
REG8(GPIO_B31, 0x1f)
	FIELD(GPIO_B31, PBYTE, 0, 1)
REG8(GPIO_B32, 0x20)
	FIELD(GPIO_B32, PBYTE, 0, 1)
REG8(GPIO_B33, 0x21)
	FIELD(GPIO_B33, PBYTE, 0, 1)
REG8(GPIO_B34, 0x22)
	FIELD(GPIO_B34, PBYTE, 0, 1)
REG8(GPIO_B35, 0x23)
	FIELD(GPIO_B35, PBYTE, 0, 1)
REG8(GPIO_B36, 0x24)
	FIELD(GPIO_B36, PBYTE, 0, 1)
REG8(GPIO_B37, 0x25)
	FIELD(GPIO_B37, PBYTE, 0, 1)
REG8(GPIO_B38, 0x26)
	FIELD(GPIO_B38, PBYTE, 0, 1)
REG8(GPIO_B39, 0x27)
	FIELD(GPIO_B39, PBYTE, 0, 1)
REG8(GPIO_B40, 0x28)
	FIELD(GPIO_B40, PBYTE, 0, 1)
REG8(GPIO_B41, 0x29)
	FIELD(GPIO_B41, PBYTE, 0, 1)
REG8(GPIO_B42, 0x2a)
	FIELD(GPIO_B42, PBYTE, 0, 1)
REG8(GPIO_B43, 0x2b)
	FIELD(GPIO_B43, PBYTE, 0, 1)
REG8(GPIO_B44, 0x2c)
	FIELD(GPIO_B44, PBYTE, 0, 1)
REG8(GPIO_B45, 0x2d)
	FIELD(GPIO_B45, PBYTE, 0, 1)
REG8(GPIO_B46, 0x2e)
	FIELD(GPIO_B46, PBYTE, 0, 1)
REG8(GPIO_B47, 0x2f)
	FIELD(GPIO_B47, PBYTE, 0, 1)
REG8(GPIO_B48, 0x30)
	FIELD(GPIO_B48, PBYTE, 0, 1)
REG8(GPIO_B49, 0x31)
	FIELD(GPIO_B49, PBYTE, 0, 1)
REG32(GPIO_W0, 0x1000)
	FIELD(GPIO_W0, PWORD, 0, 32)
REG32(GPIO_W1, 0x1004)
	FIELD(GPIO_W1, PWORD, 0, 32)
REG32(GPIO_W2, 0x1008)
	FIELD(GPIO_W2, PWORD, 0, 32)
REG32(GPIO_W3, 0x100c)
	FIELD(GPIO_W3, PWORD, 0, 32)
REG32(GPIO_W4, 0x1010)
	FIELD(GPIO_W4, PWORD, 0, 32)
REG32(GPIO_W5, 0x1014)
	FIELD(GPIO_W5, PWORD, 0, 32)
REG32(GPIO_W6, 0x1018)
	FIELD(GPIO_W6, PWORD, 0, 32)
REG32(GPIO_W7, 0x101c)
	FIELD(GPIO_W7, PWORD, 0, 32)
REG32(GPIO_W8, 0x1020)
	FIELD(GPIO_W8, PWORD, 0, 32)
REG32(GPIO_W9, 0x1024)
	FIELD(GPIO_W9, PWORD, 0, 32)
REG32(GPIO_W10, 0x1028)
	FIELD(GPIO_W10, PWORD, 0, 32)
REG32(GPIO_W11, 0x102c)
	FIELD(GPIO_W11, PWORD, 0, 32)
REG32(GPIO_W12, 0x1030)
	FIELD(GPIO_W12, PWORD, 0, 32)
REG32(GPIO_W13, 0x1034)
	FIELD(GPIO_W13, PWORD, 0, 32)
REG32(GPIO_W14, 0x1038)
	FIELD(GPIO_W14, PWORD, 0, 32)
REG32(GPIO_W15, 0x103c)
	FIELD(GPIO_W15, PWORD, 0, 32)
REG32(GPIO_W16, 0x1040)
	FIELD(GPIO_W16, PWORD, 0, 32)
REG32(GPIO_W17, 0x1044)
	FIELD(GPIO_W17, PWORD, 0, 32)
REG32(GPIO_W18, 0x1048)
	FIELD(GPIO_W18, PWORD, 0, 32)
REG32(GPIO_W19, 0x104c)
	FIELD(GPIO_W19, PWORD, 0, 32)
REG32(GPIO_W20, 0x1050)
	FIELD(GPIO_W20, PWORD, 0, 32)
REG32(GPIO_W21, 0x1054)
	FIELD(GPIO_W21, PWORD, 0, 32)
REG32(GPIO_W22, 0x1058)
	FIELD(GPIO_W22, PWORD, 0, 32)
REG32(GPIO_W23, 0x105c)
	FIELD(GPIO_W23, PWORD, 0, 32)
REG32(GPIO_W24, 0x1060)
	FIELD(GPIO_W24, PWORD, 0, 32)
REG32(GPIO_W25, 0x1064)
	FIELD(GPIO_W25, PWORD, 0, 32)
REG32(GPIO_W26, 0x1068)
	FIELD(GPIO_W26, PWORD, 0, 32)
REG32(GPIO_W27, 0x106c)
	FIELD(GPIO_W27, PWORD, 0, 32)
REG32(GPIO_W28, 0x1070)
	FIELD(GPIO_W28, PWORD, 0, 32)
REG32(GPIO_W29, 0x1074)
	FIELD(GPIO_W29, PWORD, 0, 32)
REG32(GPIO_W30, 0x1078)
	FIELD(GPIO_W30, PWORD, 0, 32)
REG32(GPIO_W31, 0x107c)
	FIELD(GPIO_W31, PWORD, 0, 32)
REG32(GPIO_W32, 0x1080)
	FIELD(GPIO_W32, PWORD, 0, 32)
REG32(GPIO_W33, 0x1084)
	FIELD(GPIO_W33, PWORD, 0, 32)
REG32(GPIO_W34, 0x1088)
	FIELD(GPIO_W34, PWORD, 0, 32)
REG32(GPIO_W35, 0x108c)
	FIELD(GPIO_W35, PWORD, 0, 32)
REG32(GPIO_W36, 0x1090)
	FIELD(GPIO_W36, PWORD, 0, 32)
REG32(GPIO_W37, 0x1094)
	FIELD(GPIO_W37, PWORD, 0, 32)
REG32(GPIO_W38, 0x1098)
	FIELD(GPIO_W38, PWORD, 0, 32)
REG32(GPIO_W39, 0x109c)
	FIELD(GPIO_W39, PWORD, 0, 32)
REG32(GPIO_W40, 0x10a0)
	FIELD(GPIO_W40, PWORD, 0, 32)
REG32(GPIO_W41, 0x10a4)
	FIELD(GPIO_W41, PWORD, 0, 32)
REG32(GPIO_W42, 0x10a8)
	FIELD(GPIO_W42, PWORD, 0, 32)
REG32(GPIO_W43, 0x10ac)
	FIELD(GPIO_W43, PWORD, 0, 32)
REG32(GPIO_W44, 0x10b0)
	FIELD(GPIO_W44, PWORD, 0, 32)
REG32(GPIO_W45, 0x10b4)
	FIELD(GPIO_W45, PWORD, 0, 32)
REG32(GPIO_W46, 0x10b8)
	FIELD(GPIO_W46, PWORD, 0, 32)
REG32(GPIO_W47, 0x10bc)
	FIELD(GPIO_W47, PWORD, 0, 32)
REG32(GPIO_W48, 0x10c0)
	FIELD(GPIO_W48, PWORD, 0, 32)
REG32(GPIO_W49, 0x10c4)
	FIELD(GPIO_W49, PWORD, 0, 32)
REG32(GPIO_DIR0, 0x2000)
	FIELD(GPIO_DIR0, DIRP, 0, 32)
REG32(GPIO_DIR1, 0x2004)
	FIELD(GPIO_DIR1, DIRP, 0, 32)
REG32(GPIO_MASK0, 0x2080)
	FIELD(GPIO_MASK0, MASKP, 0, 32)
REG32(GPIO_MASK1, 0x2084)
	FIELD(GPIO_MASK1, MASKP, 0, 32)
REG32(GPIO_PIN0, 0x2100)
	FIELD(GPIO_PIN0, PORT, 0, 32)
REG32(GPIO_PIN1, 0x2104)
	FIELD(GPIO_PIN1, PORT, 0, 32)
REG32(GPIO_MPIN0, 0x2180)
	FIELD(GPIO_MPIN0, MPORTP, 0, 32)
REG32(GPIO_MPIN1, 0x2184)
	FIELD(GPIO_MPIN1, MPORTP, 0, 32)
REG32(GPIO_SET0, 0x2200)
	FIELD(GPIO_SET0, SETP, 0, 32)
REG32(GPIO_SET1, 0x2204)
	FIELD(GPIO_SET1, SETP, 0, 32)
REG32(GPIO_CLR0, 0x2280)
	FIELD(GPIO_CLR0, CLRP, 0, 32)
REG32(GPIO_CLR1, 0x2284)
	FIELD(GPIO_CLR1, CLRP, 0, 32)
REG32(GPIO_NOT0, 0x2300)
	FIELD(GPIO_NOT0, NOTP, 0, 32)
REG32(GPIO_NOT1, 0x2304)
	FIELD(GPIO_NOT1, NOTP, 0, 32)
REG32(GPIO_DIRSET0, 0x2380)
	FIELD(GPIO_DIRSET0, DIRSETP, 0, 32)
REG32(GPIO_DIRSET1, 0x2384)
	FIELD(GPIO_DIRSET1, DIRSETP, 0, 32)
REG32(GPIO_DIRCLR0, 0x2400)
	FIELD(GPIO_DIRCLR0, DIRCLRP, 0, 32)
REG32(GPIO_DIRCLR1, 0x2404)
	FIELD(GPIO_DIRCLR1, DIRCLRP, 0, 32)
REG32(GPIO_DIRNOT0, 0x2480)
	FIELD(GPIO_DIRNOT0, DIRNOTP, 0, 32)
REG32(GPIO_DIRNOT1, 0x2484)
	FIELD(GPIO_DIRNOT1, DIRNOTP, 0, 32)


static void lpc51u68_gpio_register_reset(LPC51U68GPIO *t) {
	t->B0 = 0x0;
	t->B1 = 0x0;
	t->B2 = 0x0;
	t->B3 = 0x0;
	t->B4 = 0x0;
	t->B5 = 0x0;
	t->B6 = 0x0;
	t->B7 = 0x0;
	t->B8 = 0x0;
	t->B9 = 0x0;
	t->B10 = 0x0;
	t->B11 = 0x0;
	t->B12 = 0x0;
	t->B13 = 0x0;
	t->B14 = 0x0;
	t->B15 = 0x0;
	t->B16 = 0x0;
	t->B17 = 0x0;
	t->B18 = 0x0;
	t->B19 = 0x0;
	t->B20 = 0x0;
	t->B21 = 0x0;
	t->B22 = 0x0;
	t->B23 = 0x0;
	t->B24 = 0x0;
	t->B25 = 0x0;
	t->B26 = 0x0;
	t->B27 = 0x0;
	t->B28 = 0x0;
	t->B29 = 0x0;
	t->B30 = 0x0;
	t->B31 = 0x0;
	t->B32 = 0x0;
	t->B33 = 0x0;
	t->B34 = 0x0;
	t->B35 = 0x0;
	t->B36 = 0x0;
	t->B37 = 0x0;
	t->B38 = 0x0;
	t->B39 = 0x0;
	t->B40 = 0x0;
	t->B41 = 0x0;
	t->B42 = 0x0;
	t->B43 = 0x0;
	t->B44 = 0x0;
	t->B45 = 0x0;
	t->B46 = 0x0;
	t->B47 = 0x0;
	t->B48 = 0x0;
	t->B49 = 0x0;
	t->W0 = 0x0;
	t->W1 = 0x0;
	t->W2 = 0x0;
	t->W3 = 0x0;
	t->W4 = 0x0;
	t->W5 = 0x0;
	t->W6 = 0x0;
	t->W7 = 0x0;
	t->W8 = 0x0;
	t->W9 = 0x0;
	t->W10 = 0x0;
	t->W11 = 0x0;
	t->W12 = 0x0;
	t->W13 = 0x0;
	t->W14 = 0x0;
	t->W15 = 0x0;
	t->W16 = 0x0;
	t->W17 = 0x0;
	t->W18 = 0x0;
	t->W19 = 0x0;
	t->W20 = 0x0;
	t->W21 = 0x0;
	t->W22 = 0x0;
	t->W23 = 0x0;
	t->W24 = 0x0;
	t->W25 = 0x0;
	t->W26 = 0x0;
	t->W27 = 0x0;
	t->W28 = 0x0;
	t->W29 = 0x0;
	t->W30 = 0x0;
	t->W31 = 0x0;
	t->W32 = 0x0;
	t->W33 = 0x0;
	t->W34 = 0x0;
	t->W35 = 0x0;
	t->W36 = 0x0;
	t->W37 = 0x0;
	t->W38 = 0x0;
	t->W39 = 0x0;
	t->W40 = 0x0;
	t->W41 = 0x0;
	t->W42 = 0x0;
	t->W43 = 0x0;
	t->W44 = 0x0;
	t->W45 = 0x0;
	t->W46 = 0x0;
	t->W47 = 0x0;
	t->W48 = 0x0;
	t->W49 = 0x0;
	t->DIR0 = 0x0;
	t->DIR1 = 0x0;
	t->MASK0 = 0x0;
	t->MASK1 = 0x0;
	t->PIN0 = 0x0;
	t->PIN1 = 0x0;
	t->MPIN0 = 0x0;
	t->MPIN1 = 0x0;
	t->SET0 = 0x0;
	t->SET1 = 0x0;
	t->CLR0 = 0x0;
	t->CLR1 = 0x0;
	t->NOT0 = 0x0;
	t->NOT1 = 0x0;
	t->DIRSET0 = 0x0;
	t->DIRSET1 = 0x0;
	t->DIRCLR0 = 0x0;
	t->DIRCLR1 = 0x0;
	t->DIRNOT0 = 0x0;
	t->DIRNOT1 = 0x0;

}

static uint64_t lpc51u68_gpio_read(void *opaque, hwaddr offset, unsigned size) {
	LPC51U68GPIO *t = LPC51U68_GPIO(opaque);
	uint64_t ret;

	switch (offset) {
		case A_GPIO_B0:
			ret = t->B0;
			break;
		case A_GPIO_B1:
			ret = t->B1;
			break;
		case A_GPIO_B2:
			ret = t->B2;
			break;
		case A_GPIO_B3:
			ret = t->B3;
			break;
		case A_GPIO_B4:
			ret = t->B4;
			break;
		case A_GPIO_B5:
			ret = t->B5;
			break;
		case A_GPIO_B6:
			ret = t->B6;
			break;
		case A_GPIO_B7:
			ret = t->B7;
			break;
		case A_GPIO_B8:
			ret = t->B8;
			break;
		case A_GPIO_B9:
			ret = t->B9;
			break;
		case A_GPIO_B10:
			ret = t->B10;
			break;
		case A_GPIO_B11:
			ret = t->B11;
			break;
		case A_GPIO_B12:
			ret = t->B12;
			break;
		case A_GPIO_B13:
			ret = t->B13;
			break;
		case A_GPIO_B14:
			ret = t->B14;
			break;
		case A_GPIO_B15:
			ret = t->B15;
			break;
		case A_GPIO_B16:
			ret = t->B16;
			break;
		case A_GPIO_B17:
			ret = t->B17;
			break;
		case A_GPIO_B18:
			ret = t->B18;
			break;
		case A_GPIO_B19:
			ret = t->B19;
			break;
		case A_GPIO_B20:
			ret = t->B20;
			break;
		case A_GPIO_B21:
			ret = t->B21;
			break;
		case A_GPIO_B22:
			ret = t->B22;
			break;
		case A_GPIO_B23:
			ret = t->B23;
			break;
		case A_GPIO_B24:
			ret = t->B24;
			break;
		case A_GPIO_B25:
			ret = t->B25;
			break;
		case A_GPIO_B26:
			ret = t->B26;
			break;
		case A_GPIO_B27:
			ret = t->B27;
			break;
		case A_GPIO_B28:
			ret = t->B28;
			break;
		case A_GPIO_B29:
			ret = t->B29;
			break;
		case A_GPIO_B30:
			ret = t->B30;
			break;
		case A_GPIO_B31:
			ret = t->B31;
			break;
		case A_GPIO_B32:
			ret = t->B32;
			break;
		case A_GPIO_B33:
			ret = t->B33;
			break;
		case A_GPIO_B34:
			ret = t->B34;
			break;
		case A_GPIO_B35:
			ret = t->B35;
			break;
		case A_GPIO_B36:
			ret = t->B36;
			break;
		case A_GPIO_B37:
			ret = t->B37;
			break;
		case A_GPIO_B38:
			ret = t->B38;
			break;
		case A_GPIO_B39:
			ret = t->B39;
			break;
		case A_GPIO_B40:
			ret = t->B40;
			break;
		case A_GPIO_B41:
			ret = t->B41;
			break;
		case A_GPIO_B42:
			ret = t->B42;
			break;
		case A_GPIO_B43:
			ret = t->B43;
			break;
		case A_GPIO_B44:
			ret = t->B44;
			break;
		case A_GPIO_B45:
			ret = t->B45;
			break;
		case A_GPIO_B46:
			ret = t->B46;
			break;
		case A_GPIO_B47:
			ret = t->B47;
			break;
		case A_GPIO_B48:
			ret = t->B48;
			break;
		case A_GPIO_B49:
			ret = t->B49;
			break;
		case A_GPIO_W0:
			ret = t->W0;
			break;
		case A_GPIO_W1:
			ret = t->W1;
			break;
		case A_GPIO_W2:
			ret = t->W2;
			break;
		case A_GPIO_W3:
			ret = t->W3;
			break;
		case A_GPIO_W4:
			ret = t->W4;
			break;
		case A_GPIO_W5:
			ret = t->W5;
			break;
		case A_GPIO_W6:
			ret = t->W6;
			break;
		case A_GPIO_W7:
			ret = t->W7;
			break;
		case A_GPIO_W8:
			ret = t->W8;
			break;
		case A_GPIO_W9:
			ret = t->W9;
			break;
		case A_GPIO_W10:
			ret = t->W10;
			break;
		case A_GPIO_W11:
			ret = t->W11;
			break;
		case A_GPIO_W12:
			ret = t->W12;
			break;
		case A_GPIO_W13:
			ret = t->W13;
			break;
		case A_GPIO_W14:
			ret = t->W14;
			break;
		case A_GPIO_W15:
			ret = t->W15;
			break;
		case A_GPIO_W16:
			ret = t->W16;
			break;
		case A_GPIO_W17:
			ret = t->W17;
			break;
		case A_GPIO_W18:
			ret = t->W18;
			break;
		case A_GPIO_W19:
			ret = t->W19;
			break;
		case A_GPIO_W20:
			ret = t->W20;
			break;
		case A_GPIO_W21:
			ret = t->W21;
			break;
		case A_GPIO_W22:
			ret = t->W22;
			break;
		case A_GPIO_W23:
			ret = t->W23;
			break;
		case A_GPIO_W24:
			ret = t->W24;
			break;
		case A_GPIO_W25:
			ret = t->W25;
			break;
		case A_GPIO_W26:
			ret = t->W26;
			break;
		case A_GPIO_W27:
			ret = t->W27;
			break;
		case A_GPIO_W28:
			ret = t->W28;
			break;
		case A_GPIO_W29:
			ret = t->W29;
			break;
		case A_GPIO_W30:
			ret = t->W30;
			break;
		case A_GPIO_W31:
			ret = t->W31;
			break;
		case A_GPIO_W32:
			ret = t->W32;
			break;
		case A_GPIO_W33:
			ret = t->W33;
			break;
		case A_GPIO_W34:
			ret = t->W34;
			break;
		case A_GPIO_W35:
			ret = t->W35;
			break;
		case A_GPIO_W36:
			ret = t->W36;
			break;
		case A_GPIO_W37:
			ret = t->W37;
			break;
		case A_GPIO_W38:
			ret = t->W38;
			break;
		case A_GPIO_W39:
			ret = t->W39;
			break;
		case A_GPIO_W40:
			ret = t->W40;
			break;
		case A_GPIO_W41:
			ret = t->W41;
			break;
		case A_GPIO_W42:
			ret = t->W42;
			break;
		case A_GPIO_W43:
			ret = t->W43;
			break;
		case A_GPIO_W44:
			ret = t->W44;
			break;
		case A_GPIO_W45:
			ret = t->W45;
			break;
		case A_GPIO_W46:
			ret = t->W46;
			break;
		case A_GPIO_W47:
			ret = t->W47;
			break;
		case A_GPIO_W48:
			ret = t->W48;
			break;
		case A_GPIO_W49:
			ret = t->W49;
			break;
		case A_GPIO_DIR0:
			ret = t->DIR0;
			break;
		case A_GPIO_DIR1:
			ret = t->DIR1;
			break;
		case A_GPIO_MASK0:
			ret = t->MASK0;
			break;
		case A_GPIO_MASK1:
			ret = t->MASK1;
			break;
		case A_GPIO_PIN0:
			ret = t->PIN0;
			break;
		case A_GPIO_PIN1:
			ret = t->PIN1;
			break;
		case A_GPIO_MPIN0:
			ret = t->MPIN0;
			break;
		case A_GPIO_MPIN1:
			ret = t->MPIN1;
			break;
		case A_GPIO_SET0:
			ret = t->SET0;
			break;
		case A_GPIO_SET1:
			ret = t->SET1;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "LPC51U68 GPIO read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void lpc51u68_gpio_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	LPC51U68GPIO *t = LPC51U68_GPIO(opaque);


	switch (offset) {
		case A_GPIO_W0 ... A_GPIO_W0 + 3:
			value = value << ((offset - A_GPIO_W0) << 3);
			offset = A_GPIO_W0;
			break;
		case A_GPIO_W1 ... A_GPIO_W1 + 3:
			value = value << ((offset - A_GPIO_W1) << 3);
			offset = A_GPIO_W1;
			break;
		case A_GPIO_W2 ... A_GPIO_W2 + 3:
			value = value << ((offset - A_GPIO_W2) << 3);
			offset = A_GPIO_W2;
			break;
		case A_GPIO_W3 ... A_GPIO_W3 + 3:
			value = value << ((offset - A_GPIO_W3) << 3);
			offset = A_GPIO_W3;
			break;
		case A_GPIO_W4 ... A_GPIO_W4 + 3:
			value = value << ((offset - A_GPIO_W4) << 3);
			offset = A_GPIO_W4;
			break;
		case A_GPIO_W5 ... A_GPIO_W5 + 3:
			value = value << ((offset - A_GPIO_W5) << 3);
			offset = A_GPIO_W5;
			break;
		case A_GPIO_W6 ... A_GPIO_W6 + 3:
			value = value << ((offset - A_GPIO_W6) << 3);
			offset = A_GPIO_W6;
			break;
		case A_GPIO_W7 ... A_GPIO_W7 + 3:
			value = value << ((offset - A_GPIO_W7) << 3);
			offset = A_GPIO_W7;
			break;
		case A_GPIO_W8 ... A_GPIO_W8 + 3:
			value = value << ((offset - A_GPIO_W8) << 3);
			offset = A_GPIO_W8;
			break;
		case A_GPIO_W9 ... A_GPIO_W9 + 3:
			value = value << ((offset - A_GPIO_W9) << 3);
			offset = A_GPIO_W9;
			break;
		case A_GPIO_W10 ... A_GPIO_W10 + 3:
			value = value << ((offset - A_GPIO_W10) << 3);
			offset = A_GPIO_W10;
			break;
		case A_GPIO_W11 ... A_GPIO_W11 + 3:
			value = value << ((offset - A_GPIO_W11) << 3);
			offset = A_GPIO_W11;
			break;
		case A_GPIO_W12 ... A_GPIO_W12 + 3:
			value = value << ((offset - A_GPIO_W12) << 3);
			offset = A_GPIO_W12;
			break;
		case A_GPIO_W13 ... A_GPIO_W13 + 3:
			value = value << ((offset - A_GPIO_W13) << 3);
			offset = A_GPIO_W13;
			break;
		case A_GPIO_W14 ... A_GPIO_W14 + 3:
			value = value << ((offset - A_GPIO_W14) << 3);
			offset = A_GPIO_W14;
			break;
		case A_GPIO_W15 ... A_GPIO_W15 + 3:
			value = value << ((offset - A_GPIO_W15) << 3);
			offset = A_GPIO_W15;
			break;
		case A_GPIO_W16 ... A_GPIO_W16 + 3:
			value = value << ((offset - A_GPIO_W16) << 3);
			offset = A_GPIO_W16;
			break;
		case A_GPIO_W17 ... A_GPIO_W17 + 3:
			value = value << ((offset - A_GPIO_W17) << 3);
			offset = A_GPIO_W17;
			break;
		case A_GPIO_W18 ... A_GPIO_W18 + 3:
			value = value << ((offset - A_GPIO_W18) << 3);
			offset = A_GPIO_W18;
			break;
		case A_GPIO_W19 ... A_GPIO_W19 + 3:
			value = value << ((offset - A_GPIO_W19) << 3);
			offset = A_GPIO_W19;
			break;
		case A_GPIO_W20 ... A_GPIO_W20 + 3:
			value = value << ((offset - A_GPIO_W20) << 3);
			offset = A_GPIO_W20;
			break;
		case A_GPIO_W21 ... A_GPIO_W21 + 3:
			value = value << ((offset - A_GPIO_W21) << 3);
			offset = A_GPIO_W21;
			break;
		case A_GPIO_W22 ... A_GPIO_W22 + 3:
			value = value << ((offset - A_GPIO_W22) << 3);
			offset = A_GPIO_W22;
			break;
		case A_GPIO_W23 ... A_GPIO_W23 + 3:
			value = value << ((offset - A_GPIO_W23) << 3);
			offset = A_GPIO_W23;
			break;
		case A_GPIO_W24 ... A_GPIO_W24 + 3:
			value = value << ((offset - A_GPIO_W24) << 3);
			offset = A_GPIO_W24;
			break;
		case A_GPIO_W25 ... A_GPIO_W25 + 3:
			value = value << ((offset - A_GPIO_W25) << 3);
			offset = A_GPIO_W25;
			break;
		case A_GPIO_W26 ... A_GPIO_W26 + 3:
			value = value << ((offset - A_GPIO_W26) << 3);
			offset = A_GPIO_W26;
			break;
		case A_GPIO_W27 ... A_GPIO_W27 + 3:
			value = value << ((offset - A_GPIO_W27) << 3);
			offset = A_GPIO_W27;
			break;
		case A_GPIO_W28 ... A_GPIO_W28 + 3:
			value = value << ((offset - A_GPIO_W28) << 3);
			offset = A_GPIO_W28;
			break;
		case A_GPIO_W29 ... A_GPIO_W29 + 3:
			value = value << ((offset - A_GPIO_W29) << 3);
			offset = A_GPIO_W29;
			break;
		case A_GPIO_W30 ... A_GPIO_W30 + 3:
			value = value << ((offset - A_GPIO_W30) << 3);
			offset = A_GPIO_W30;
			break;
		case A_GPIO_W31 ... A_GPIO_W31 + 3:
			value = value << ((offset - A_GPIO_W31) << 3);
			offset = A_GPIO_W31;
			break;
		case A_GPIO_W32 ... A_GPIO_W32 + 3:
			value = value << ((offset - A_GPIO_W32) << 3);
			offset = A_GPIO_W32;
			break;
		case A_GPIO_W33 ... A_GPIO_W33 + 3:
			value = value << ((offset - A_GPIO_W33) << 3);
			offset = A_GPIO_W33;
			break;
		case A_GPIO_W34 ... A_GPIO_W34 + 3:
			value = value << ((offset - A_GPIO_W34) << 3);
			offset = A_GPIO_W34;
			break;
		case A_GPIO_W35 ... A_GPIO_W35 + 3:
			value = value << ((offset - A_GPIO_W35) << 3);
			offset = A_GPIO_W35;
			break;
		case A_GPIO_W36 ... A_GPIO_W36 + 3:
			value = value << ((offset - A_GPIO_W36) << 3);
			offset = A_GPIO_W36;
			break;
		case A_GPIO_W37 ... A_GPIO_W37 + 3:
			value = value << ((offset - A_GPIO_W37) << 3);
			offset = A_GPIO_W37;
			break;
		case A_GPIO_W38 ... A_GPIO_W38 + 3:
			value = value << ((offset - A_GPIO_W38) << 3);
			offset = A_GPIO_W38;
			break;
		case A_GPIO_W39 ... A_GPIO_W39 + 3:
			value = value << ((offset - A_GPIO_W39) << 3);
			offset = A_GPIO_W39;
			break;
		case A_GPIO_W40 ... A_GPIO_W40 + 3:
			value = value << ((offset - A_GPIO_W40) << 3);
			offset = A_GPIO_W40;
			break;
		case A_GPIO_W41 ... A_GPIO_W41 + 3:
			value = value << ((offset - A_GPIO_W41) << 3);
			offset = A_GPIO_W41;
			break;
		case A_GPIO_W42 ... A_GPIO_W42 + 3:
			value = value << ((offset - A_GPIO_W42) << 3);
			offset = A_GPIO_W42;
			break;
		case A_GPIO_W43 ... A_GPIO_W43 + 3:
			value = value << ((offset - A_GPIO_W43) << 3);
			offset = A_GPIO_W43;
			break;
		case A_GPIO_W44 ... A_GPIO_W44 + 3:
			value = value << ((offset - A_GPIO_W44) << 3);
			offset = A_GPIO_W44;
			break;
		case A_GPIO_W45 ... A_GPIO_W45 + 3:
			value = value << ((offset - A_GPIO_W45) << 3);
			offset = A_GPIO_W45;
			break;
		case A_GPIO_W46 ... A_GPIO_W46 + 3:
			value = value << ((offset - A_GPIO_W46) << 3);
			offset = A_GPIO_W46;
			break;
		case A_GPIO_W47 ... A_GPIO_W47 + 3:
			value = value << ((offset - A_GPIO_W47) << 3);
			offset = A_GPIO_W47;
			break;
		case A_GPIO_W48 ... A_GPIO_W48 + 3:
			value = value << ((offset - A_GPIO_W48) << 3);
			offset = A_GPIO_W48;
			break;
		case A_GPIO_W49 ... A_GPIO_W49 + 3:
			value = value << ((offset - A_GPIO_W49) << 3);
			offset = A_GPIO_W49;
			break;
		case A_GPIO_DIR0 ... A_GPIO_DIR0 + 3:
			value = value << ((offset - A_GPIO_DIR0) << 3);
			offset = A_GPIO_DIR0;
			break;
		case A_GPIO_DIR1 ... A_GPIO_DIR1 + 3:
			value = value << ((offset - A_GPIO_DIR1) << 3);
			offset = A_GPIO_DIR1;
			break;
		case A_GPIO_MASK0 ... A_GPIO_MASK0 + 3:
			value = value << ((offset - A_GPIO_MASK0) << 3);
			offset = A_GPIO_MASK0;
			break;
		case A_GPIO_MASK1 ... A_GPIO_MASK1 + 3:
			value = value << ((offset - A_GPIO_MASK1) << 3);
			offset = A_GPIO_MASK1;
			break;
		case A_GPIO_PIN0 ... A_GPIO_PIN0 + 3:
			value = value << ((offset - A_GPIO_PIN0) << 3);
			offset = A_GPIO_PIN0;
			break;
		case A_GPIO_PIN1 ... A_GPIO_PIN1 + 3:
			value = value << ((offset - A_GPIO_PIN1) << 3);
			offset = A_GPIO_PIN1;
			break;
		case A_GPIO_MPIN0 ... A_GPIO_MPIN0 + 3:
			value = value << ((offset - A_GPIO_MPIN0) << 3);
			offset = A_GPIO_MPIN0;
			break;
		case A_GPIO_MPIN1 ... A_GPIO_MPIN1 + 3:
			value = value << ((offset - A_GPIO_MPIN1) << 3);
			offset = A_GPIO_MPIN1;
			break;
		case A_GPIO_SET0 ... A_GPIO_SET0 + 3:
			value = value << ((offset - A_GPIO_SET0) << 3);
			offset = A_GPIO_SET0;
			break;
		case A_GPIO_SET1 ... A_GPIO_SET1 + 3:
			value = value << ((offset - A_GPIO_SET1) << 3);
			offset = A_GPIO_SET1;
			break;
		case A_GPIO_CLR0 ... A_GPIO_CLR0 + 3:
			value = value << ((offset - A_GPIO_CLR0) << 3);
			offset = A_GPIO_CLR0;
			break;
		case A_GPIO_CLR1 ... A_GPIO_CLR1 + 3:
			value = value << ((offset - A_GPIO_CLR1) << 3);
			offset = A_GPIO_CLR1;
			break;
		case A_GPIO_NOT0 ... A_GPIO_NOT0 + 3:
			value = value << ((offset - A_GPIO_NOT0) << 3);
			offset = A_GPIO_NOT0;
			break;
		case A_GPIO_NOT1 ... A_GPIO_NOT1 + 3:
			value = value << ((offset - A_GPIO_NOT1) << 3);
			offset = A_GPIO_NOT1;
			break;
		case A_GPIO_DIRSET0 ... A_GPIO_DIRSET0 + 3:
			value = value << ((offset - A_GPIO_DIRSET0) << 3);
			offset = A_GPIO_DIRSET0;
			break;
		case A_GPIO_DIRSET1 ... A_GPIO_DIRSET1 + 3:
			value = value << ((offset - A_GPIO_DIRSET1) << 3);
			offset = A_GPIO_DIRSET1;
			break;
		case A_GPIO_DIRCLR0 ... A_GPIO_DIRCLR0 + 3:
			value = value << ((offset - A_GPIO_DIRCLR0) << 3);
			offset = A_GPIO_DIRCLR0;
			break;
		case A_GPIO_DIRCLR1 ... A_GPIO_DIRCLR1 + 3:
			value = value << ((offset - A_GPIO_DIRCLR1) << 3);
			offset = A_GPIO_DIRCLR1;
			break;
		case A_GPIO_DIRNOT0 ... A_GPIO_DIRNOT0 + 3:
			value = value << ((offset - A_GPIO_DIRNOT0) << 3);
			offset = A_GPIO_DIRNOT0;
			break;
		case A_GPIO_DIRNOT1 ... A_GPIO_DIRNOT1 + 3:
			value = value << ((offset - A_GPIO_DIRNOT1) << 3);
			offset = A_GPIO_DIRNOT1;
			break;

		default: break;
	}

	switch (offset) {
		case A_GPIO_B0:
			t->B0 = value;
			break;
		case A_GPIO_B1:
			t->B1 = value;
			break;
		case A_GPIO_B2:
			t->B2 = value;
			break;
		case A_GPIO_B3:
			t->B3 = value;
			break;
		case A_GPIO_B4:
			t->B4 = value;
			break;
		case A_GPIO_B5:
			t->B5 = value;
			break;
		case A_GPIO_B6:
			t->B6 = value;
			break;
		case A_GPIO_B7:
			t->B7 = value;
			break;
		case A_GPIO_B8:
			t->B8 = value;
			break;
		case A_GPIO_B9:
			t->B9 = value;
			break;
		case A_GPIO_B10:
			t->B10 = value;
			break;
		case A_GPIO_B11:
			t->B11 = value;
			break;
		case A_GPIO_B12:
			t->B12 = value;
			break;
		case A_GPIO_B13:
			t->B13 = value;
			break;
		case A_GPIO_B14:
			t->B14 = value;
			break;
		case A_GPIO_B15:
			t->B15 = value;
			break;
		case A_GPIO_B16:
			t->B16 = value;
			break;
		case A_GPIO_B17:
			t->B17 = value;
			break;
		case A_GPIO_B18:
			t->B18 = value;
			break;
		case A_GPIO_B19:
			t->B19 = value;
			break;
		case A_GPIO_B20:
			t->B20 = value;
			break;
		case A_GPIO_B21:
			t->B21 = value;
			break;
		case A_GPIO_B22:
			t->B22 = value;
			break;
		case A_GPIO_B23:
			t->B23 = value;
			break;
		case A_GPIO_B24:
			t->B24 = value;
			break;
		case A_GPIO_B25:
			t->B25 = value;
			break;
		case A_GPIO_B26:
			t->B26 = value;
			break;
		case A_GPIO_B27:
			t->B27 = value;
			break;
		case A_GPIO_B28:
			t->B28 = value;
			break;
		case A_GPIO_B29:
			t->B29 = value;
			break;
		case A_GPIO_B30:
			t->B30 = value;
			break;
		case A_GPIO_B31:
			t->B31 = value;
			break;
		case A_GPIO_B32:
			t->B32 = value;
			break;
		case A_GPIO_B33:
			t->B33 = value;
			break;
		case A_GPIO_B34:
			t->B34 = value;
			break;
		case A_GPIO_B35:
			t->B35 = value;
			break;
		case A_GPIO_B36:
			t->B36 = value;
			break;
		case A_GPIO_B37:
			t->B37 = value;
			break;
		case A_GPIO_B38:
			t->B38 = value;
			break;
		case A_GPIO_B39:
			t->B39 = value;
			break;
		case A_GPIO_B40:
			t->B40 = value;
			break;
		case A_GPIO_B41:
			t->B41 = value;
			break;
		case A_GPIO_B42:
			t->B42 = value;
			break;
		case A_GPIO_B43:
			t->B43 = value;
			break;
		case A_GPIO_B44:
			t->B44 = value;
			break;
		case A_GPIO_B45:
			t->B45 = value;
			break;
		case A_GPIO_B46:
			t->B46 = value;
			break;
		case A_GPIO_B47:
			t->B47 = value;
			break;
		case A_GPIO_B48:
			t->B48 = value;
			break;
		case A_GPIO_B49:
			t->B49 = value;
			break;
		case A_GPIO_W0:
			t->W0 = value;
			break;
		case A_GPIO_W1:
			t->W1 = value;
			break;
		case A_GPIO_W2:
			t->W2 = value;
			break;
		case A_GPIO_W3:
			t->W3 = value;
			break;
		case A_GPIO_W4:
			t->W4 = value;
			break;
		case A_GPIO_W5:
			t->W5 = value;
			break;
		case A_GPIO_W6:
			t->W6 = value;
			break;
		case A_GPIO_W7:
			t->W7 = value;
			break;
		case A_GPIO_W8:
			t->W8 = value;
			break;
		case A_GPIO_W9:
			t->W9 = value;
			break;
		case A_GPIO_W10:
			t->W10 = value;
			break;
		case A_GPIO_W11:
			t->W11 = value;
			break;
		case A_GPIO_W12:
			t->W12 = value;
			break;
		case A_GPIO_W13:
			t->W13 = value;
			break;
		case A_GPIO_W14:
			t->W14 = value;
			break;
		case A_GPIO_W15:
			t->W15 = value;
			break;
		case A_GPIO_W16:
			t->W16 = value;
			break;
		case A_GPIO_W17:
			t->W17 = value;
			break;
		case A_GPIO_W18:
			t->W18 = value;
			break;
		case A_GPIO_W19:
			t->W19 = value;
			break;
		case A_GPIO_W20:
			t->W20 = value;
			break;
		case A_GPIO_W21:
			t->W21 = value;
			break;
		case A_GPIO_W22:
			t->W22 = value;
			break;
		case A_GPIO_W23:
			t->W23 = value;
			break;
		case A_GPIO_W24:
			t->W24 = value;
			break;
		case A_GPIO_W25:
			t->W25 = value;
			break;
		case A_GPIO_W26:
			t->W26 = value;
			break;
		case A_GPIO_W27:
			t->W27 = value;
			break;
		case A_GPIO_W28:
			t->W28 = value;
			break;
		case A_GPIO_W29:
			t->W29 = value;
			break;
		case A_GPIO_W30:
			t->W30 = value;
			break;
		case A_GPIO_W31:
			t->W31 = value;
			break;
		case A_GPIO_W32:
			t->W32 = value;
			break;
		case A_GPIO_W33:
			t->W33 = value;
			break;
		case A_GPIO_W34:
			t->W34 = value;
			break;
		case A_GPIO_W35:
			t->W35 = value;
			break;
		case A_GPIO_W36:
			t->W36 = value;
			break;
		case A_GPIO_W37:
			t->W37 = value;
			break;
		case A_GPIO_W38:
			t->W38 = value;
			break;
		case A_GPIO_W39:
			t->W39 = value;
			break;
		case A_GPIO_W40:
			t->W40 = value;
			break;
		case A_GPIO_W41:
			t->W41 = value;
			break;
		case A_GPIO_W42:
			t->W42 = value;
			break;
		case A_GPIO_W43:
			t->W43 = value;
			break;
		case A_GPIO_W44:
			t->W44 = value;
			break;
		case A_GPIO_W45:
			t->W45 = value;
			break;
		case A_GPIO_W46:
			t->W46 = value;
			break;
		case A_GPIO_W47:
			t->W47 = value;
			break;
		case A_GPIO_W48:
			t->W48 = value;
			break;
		case A_GPIO_W49:
			t->W49 = value;
			break;
		case A_GPIO_DIR0:
			t->DIR0 = value;
			break;
		case A_GPIO_DIR1:
			t->DIR1 = value;
			break;
		case A_GPIO_MASK0:
			t->MASK0 = value;
			break;
		case A_GPIO_MASK1:
			t->MASK1 = value;
			break;
		case A_GPIO_PIN0:
			t->PIN0 = value;
			break;
		case A_GPIO_PIN1:
			t->PIN1 = value;
			break;
		case A_GPIO_MPIN0:
			t->MPIN0 = value;
			break;
		case A_GPIO_MPIN1:
			t->MPIN1 = value;
			break;
		case A_GPIO_SET0:
			t->SET0 = value;
			break;
		case A_GPIO_SET1:
			t->SET1 = value;
			break;
		case A_GPIO_CLR0:
			t->CLR0 = value;
			break;
		case A_GPIO_CLR1:
			t->CLR1 = value;
			break;
		case A_GPIO_NOT0:
			t->NOT0 = value;
			break;
		case A_GPIO_NOT1:
			t->NOT1 = value;
			break;
		case A_GPIO_DIRSET0:
			t->DIRSET0 = value;
			break;
		case A_GPIO_DIRSET1:
			t->DIRSET1 = value;
			break;
		case A_GPIO_DIRCLR0:
			t->DIRCLR0 = value;
			break;
		case A_GPIO_DIRCLR1:
			t->DIRCLR1 = value;
			break;
		case A_GPIO_DIRNOT0:
			t->DIRNOT0 = value;
			break;
		case A_GPIO_DIRNOT1:
			t->DIRNOT1 = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "LPC51U68 GPIO write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps lpc51u68_gpio_ops = {
	.read = lpc51u68_gpio_read,
	.write = lpc51u68_gpio_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void lpc51u68_gpio_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	LPC51U68GPIO *t = LPC51U68_GPIO(obj);
	memory_region_init_io(&(t->iomem), obj, &lpc51u68_gpio_ops, t, "lpc51u68-gpio", LPC51U68_GPIO_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void lpc51u68_gpio_realize(DeviceState *dev, Error **errp) {
	return;

}

static void lpc51u68_gpio_reset_enter(Object *obj, ResetType type) {
	LPC51U68GPIO *t = LPC51U68_GPIO(obj);
	lpc51u68_gpio_register_reset(t);
}

static const VMStateDescription lpc51u68_gpio_vmstate = {
	.name = "lpc51u68-gpio",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT8(B0, LPC51U68GPIO),
		VMSTATE_UINT8(B1, LPC51U68GPIO),
		VMSTATE_UINT8(B2, LPC51U68GPIO),
		VMSTATE_UINT8(B3, LPC51U68GPIO),
		VMSTATE_UINT8(B4, LPC51U68GPIO),
		VMSTATE_UINT8(B5, LPC51U68GPIO),
		VMSTATE_UINT8(B6, LPC51U68GPIO),
		VMSTATE_UINT8(B7, LPC51U68GPIO),
		VMSTATE_UINT8(B8, LPC51U68GPIO),
		VMSTATE_UINT8(B9, LPC51U68GPIO),
		VMSTATE_UINT8(B10, LPC51U68GPIO),
		VMSTATE_UINT8(B11, LPC51U68GPIO),
		VMSTATE_UINT8(B12, LPC51U68GPIO),
		VMSTATE_UINT8(B13, LPC51U68GPIO),
		VMSTATE_UINT8(B14, LPC51U68GPIO),
		VMSTATE_UINT8(B15, LPC51U68GPIO),
		VMSTATE_UINT8(B16, LPC51U68GPIO),
		VMSTATE_UINT8(B17, LPC51U68GPIO),
		VMSTATE_UINT8(B18, LPC51U68GPIO),
		VMSTATE_UINT8(B19, LPC51U68GPIO),
		VMSTATE_UINT8(B20, LPC51U68GPIO),
		VMSTATE_UINT8(B21, LPC51U68GPIO),
		VMSTATE_UINT8(B22, LPC51U68GPIO),
		VMSTATE_UINT8(B23, LPC51U68GPIO),
		VMSTATE_UINT8(B24, LPC51U68GPIO),
		VMSTATE_UINT8(B25, LPC51U68GPIO),
		VMSTATE_UINT8(B26, LPC51U68GPIO),
		VMSTATE_UINT8(B27, LPC51U68GPIO),
		VMSTATE_UINT8(B28, LPC51U68GPIO),
		VMSTATE_UINT8(B29, LPC51U68GPIO),
		VMSTATE_UINT8(B30, LPC51U68GPIO),
		VMSTATE_UINT8(B31, LPC51U68GPIO),
		VMSTATE_UINT8(B32, LPC51U68GPIO),
		VMSTATE_UINT8(B33, LPC51U68GPIO),
		VMSTATE_UINT8(B34, LPC51U68GPIO),
		VMSTATE_UINT8(B35, LPC51U68GPIO),
		VMSTATE_UINT8(B36, LPC51U68GPIO),
		VMSTATE_UINT8(B37, LPC51U68GPIO),
		VMSTATE_UINT8(B38, LPC51U68GPIO),
		VMSTATE_UINT8(B39, LPC51U68GPIO),
		VMSTATE_UINT8(B40, LPC51U68GPIO),
		VMSTATE_UINT8(B41, LPC51U68GPIO),
		VMSTATE_UINT8(B42, LPC51U68GPIO),
		VMSTATE_UINT8(B43, LPC51U68GPIO),
		VMSTATE_UINT8(B44, LPC51U68GPIO),
		VMSTATE_UINT8(B45, LPC51U68GPIO),
		VMSTATE_UINT8(B46, LPC51U68GPIO),
		VMSTATE_UINT8(B47, LPC51U68GPIO),
		VMSTATE_UINT8(B48, LPC51U68GPIO),
		VMSTATE_UINT8(B49, LPC51U68GPIO),
		VMSTATE_UINT32(W0, LPC51U68GPIO),
		VMSTATE_UINT32(W1, LPC51U68GPIO),
		VMSTATE_UINT32(W2, LPC51U68GPIO),
		VMSTATE_UINT32(W3, LPC51U68GPIO),
		VMSTATE_UINT32(W4, LPC51U68GPIO),
		VMSTATE_UINT32(W5, LPC51U68GPIO),
		VMSTATE_UINT32(W6, LPC51U68GPIO),
		VMSTATE_UINT32(W7, LPC51U68GPIO),
		VMSTATE_UINT32(W8, LPC51U68GPIO),
		VMSTATE_UINT32(W9, LPC51U68GPIO),
		VMSTATE_UINT32(W10, LPC51U68GPIO),
		VMSTATE_UINT32(W11, LPC51U68GPIO),
		VMSTATE_UINT32(W12, LPC51U68GPIO),
		VMSTATE_UINT32(W13, LPC51U68GPIO),
		VMSTATE_UINT32(W14, LPC51U68GPIO),
		VMSTATE_UINT32(W15, LPC51U68GPIO),
		VMSTATE_UINT32(W16, LPC51U68GPIO),
		VMSTATE_UINT32(W17, LPC51U68GPIO),
		VMSTATE_UINT32(W18, LPC51U68GPIO),
		VMSTATE_UINT32(W19, LPC51U68GPIO),
		VMSTATE_UINT32(W20, LPC51U68GPIO),
		VMSTATE_UINT32(W21, LPC51U68GPIO),
		VMSTATE_UINT32(W22, LPC51U68GPIO),
		VMSTATE_UINT32(W23, LPC51U68GPIO),
		VMSTATE_UINT32(W24, LPC51U68GPIO),
		VMSTATE_UINT32(W25, LPC51U68GPIO),
		VMSTATE_UINT32(W26, LPC51U68GPIO),
		VMSTATE_UINT32(W27, LPC51U68GPIO),
		VMSTATE_UINT32(W28, LPC51U68GPIO),
		VMSTATE_UINT32(W29, LPC51U68GPIO),
		VMSTATE_UINT32(W30, LPC51U68GPIO),
		VMSTATE_UINT32(W31, LPC51U68GPIO),
		VMSTATE_UINT32(W32, LPC51U68GPIO),
		VMSTATE_UINT32(W33, LPC51U68GPIO),
		VMSTATE_UINT32(W34, LPC51U68GPIO),
		VMSTATE_UINT32(W35, LPC51U68GPIO),
		VMSTATE_UINT32(W36, LPC51U68GPIO),
		VMSTATE_UINT32(W37, LPC51U68GPIO),
		VMSTATE_UINT32(W38, LPC51U68GPIO),
		VMSTATE_UINT32(W39, LPC51U68GPIO),
		VMSTATE_UINT32(W40, LPC51U68GPIO),
		VMSTATE_UINT32(W41, LPC51U68GPIO),
		VMSTATE_UINT32(W42, LPC51U68GPIO),
		VMSTATE_UINT32(W43, LPC51U68GPIO),
		VMSTATE_UINT32(W44, LPC51U68GPIO),
		VMSTATE_UINT32(W45, LPC51U68GPIO),
		VMSTATE_UINT32(W46, LPC51U68GPIO),
		VMSTATE_UINT32(W47, LPC51U68GPIO),
		VMSTATE_UINT32(W48, LPC51U68GPIO),
		VMSTATE_UINT32(W49, LPC51U68GPIO),
		VMSTATE_UINT32(DIR0, LPC51U68GPIO),
		VMSTATE_UINT32(DIR1, LPC51U68GPIO),
		VMSTATE_UINT32(MASK0, LPC51U68GPIO),
		VMSTATE_UINT32(MASK1, LPC51U68GPIO),
		VMSTATE_UINT32(PIN0, LPC51U68GPIO),
		VMSTATE_UINT32(PIN1, LPC51U68GPIO),
		VMSTATE_UINT32(MPIN0, LPC51U68GPIO),
		VMSTATE_UINT32(MPIN1, LPC51U68GPIO),
		VMSTATE_UINT32(SET0, LPC51U68GPIO),
		VMSTATE_UINT32(SET1, LPC51U68GPIO),
		VMSTATE_UINT32(CLR0, LPC51U68GPIO),
		VMSTATE_UINT32(CLR1, LPC51U68GPIO),
		VMSTATE_UINT32(NOT0, LPC51U68GPIO),
		VMSTATE_UINT32(NOT1, LPC51U68GPIO),
		VMSTATE_UINT32(DIRSET0, LPC51U68GPIO),
		VMSTATE_UINT32(DIRSET1, LPC51U68GPIO),
		VMSTATE_UINT32(DIRCLR0, LPC51U68GPIO),
		VMSTATE_UINT32(DIRCLR1, LPC51U68GPIO),
		VMSTATE_UINT32(DIRNOT0, LPC51U68GPIO),
		VMSTATE_UINT32(DIRNOT1, LPC51U68GPIO),

		VMSTATE_END_OF_LIST()
	}
};

static void lpc51u68_gpio_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &lpc51u68_gpio_vmstate;
	dc->realize = lpc51u68_gpio_realize;
	rc->phases.enter = lpc51u68_gpio_reset_enter;

}

static const TypeInfo lpc51u68_gpio_info = {
	.name = TYPE_LPC51U68_GPIO,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(LPC51U68GPIO),
	.instance_init = lpc51u68_gpio_init,
	.class_init = lpc51u68_gpio_class_init,
};

static void lpc51u68_gpio_register_types(void) {
	type_register_static(&lpc51u68_gpio_info);
}

type_init(lpc51u68_gpio_register_types);

#define TYPE_LPC51U68_SPI "lpc51u68-spi"
OBJECT_DECLARE_SIMPLE_TYPE(LPC51U68SPI, LPC51U68_SPI)

struct LPC51U68SPI {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t CFG;
	uint32_t DLY;
	uint32_t STAT;
	uint32_t INTENSET;
	uint32_t INTENCLR;
	uint32_t DIV;
	uint32_t INTSTAT;
	uint32_t FIFOCFG;
	uint32_t FIFOSTAT;
	uint32_t FIFOTRIG;
	uint32_t FIFOINTENSET;
	uint32_t FIFOINTENCLR;
	uint32_t FIFOINTSTAT;
	uint32_t FIFOWR;
	uint32_t FIFORD;
	uint32_t FIFORDNOPOP;
	uint32_t ID;
	/* chardev backend */
	CharBackend chr;
	guint watch_tag;

	/* base */
	uint32_t base;

};
#define LPC51U68_SPI_SIZE				0x1000

REG32(SPI_CFG, 0x400)
	FIELD(SPI_CFG, ENABLE, 0, 1)
	FIELD(SPI_CFG, MASTER, 2, 1)
	FIELD(SPI_CFG, LSBF, 3, 1)
	FIELD(SPI_CFG, CPHA, 4, 1)
	FIELD(SPI_CFG, CPOL, 5, 1)
	FIELD(SPI_CFG, LOOP, 7, 1)
	FIELD(SPI_CFG, SPOL0, 8, 1)
	FIELD(SPI_CFG, SPOL1, 9, 1)
	FIELD(SPI_CFG, SPOL2, 10, 1)
	FIELD(SPI_CFG, SPOL3, 11, 1)
REG32(SPI_DLY, 0x404)
	FIELD(SPI_DLY, PRE_DELAY, 0, 4)
	FIELD(SPI_DLY, POST_DELAY, 4, 4)
	FIELD(SPI_DLY, FRAME_DELAY, 8, 4)
	FIELD(SPI_DLY, TRANSFER_DELAY, 12, 4)
REG32(SPI_STAT, 0x408)
	FIELD(SPI_STAT, SSA, 4, 1)
	FIELD(SPI_STAT, SSD, 5, 1)
	FIELD(SPI_STAT, STALLED, 6, 1)
	FIELD(SPI_STAT, ENDTRANSFER, 7, 1)
	FIELD(SPI_STAT, MSTIDLE, 8, 1)
REG32(SPI_INTENSET, 0x40c)
	FIELD(SPI_INTENSET, SSAEN, 4, 1)
	FIELD(SPI_INTENSET, SSDEN, 5, 1)
	FIELD(SPI_INTENSET, MSTIDLEEN, 8, 1)
REG32(SPI_INTENCLR, 0x410)
	FIELD(SPI_INTENCLR, SSAEN, 4, 1)
	FIELD(SPI_INTENCLR, SSDEN, 5, 1)
	FIELD(SPI_INTENCLR, MSTIDLE, 8, 1)
REG32(SPI_DIV, 0x424)
	FIELD(SPI_DIV, DIVVAL, 0, 16)
REG32(SPI_INTSTAT, 0x428)
	FIELD(SPI_INTSTAT, SSA, 4, 1)
	FIELD(SPI_INTSTAT, SSD, 5, 1)
	FIELD(SPI_INTSTAT, MSTIDLE, 8, 1)
REG32(SPI_FIFOCFG, 0xe00)
	FIELD(SPI_FIFOCFG, ENABLETX, 0, 1)
	FIELD(SPI_FIFOCFG, ENABLERX, 1, 1)
	FIELD(SPI_FIFOCFG, SIZE, 4, 2)
	FIELD(SPI_FIFOCFG, DMATX, 12, 1)
	FIELD(SPI_FIFOCFG, DMARX, 13, 1)
	FIELD(SPI_FIFOCFG, WAKETX, 14, 1)
	FIELD(SPI_FIFOCFG, WAKERX, 15, 1)
	FIELD(SPI_FIFOCFG, EMPTYTX, 16, 1)
	FIELD(SPI_FIFOCFG, EMPTYRX, 17, 1)
REG32(SPI_FIFOSTAT, 0xe04)
	FIELD(SPI_FIFOSTAT, TXERR, 0, 1)
	FIELD(SPI_FIFOSTAT, RXERR, 1, 1)
	FIELD(SPI_FIFOSTAT, PERINT, 3, 1)
	FIELD(SPI_FIFOSTAT, TXEMPTY, 4, 1)
	FIELD(SPI_FIFOSTAT, TXNOTFULL, 5, 1)
	FIELD(SPI_FIFOSTAT, RXNOTEMPTY, 6, 1)
	FIELD(SPI_FIFOSTAT, RXFULL, 7, 1)
	FIELD(SPI_FIFOSTAT, TXLVL, 8, 5)
	FIELD(SPI_FIFOSTAT, RXLVL, 16, 5)
REG32(SPI_FIFOTRIG, 0xe08)
	FIELD(SPI_FIFOTRIG, TXLVLENA, 0, 1)
	FIELD(SPI_FIFOTRIG, RXLVLENA, 1, 1)
	FIELD(SPI_FIFOTRIG, TXLVL, 8, 4)
	FIELD(SPI_FIFOTRIG, RXLVL, 16, 4)
REG32(SPI_FIFOINTENSET, 0xe10)
	FIELD(SPI_FIFOINTENSET, TXERR, 0, 1)
	FIELD(SPI_FIFOINTENSET, RXERR, 1, 1)
	FIELD(SPI_FIFOINTENSET, TXLVL, 2, 1)
	FIELD(SPI_FIFOINTENSET, RXLVL, 3, 1)
REG32(SPI_FIFOINTENCLR, 0xe14)
	FIELD(SPI_FIFOINTENCLR, TXERR, 0, 1)
	FIELD(SPI_FIFOINTENCLR, RXERR, 1, 1)
	FIELD(SPI_FIFOINTENCLR, TXLVL, 2, 1)
	FIELD(SPI_FIFOINTENCLR, RXLVL, 3, 1)
REG32(SPI_FIFOINTSTAT, 0xe18)
	FIELD(SPI_FIFOINTSTAT, TXERR, 0, 1)
	FIELD(SPI_FIFOINTSTAT, RXERR, 1, 1)
	FIELD(SPI_FIFOINTSTAT, TXLVL, 2, 1)
	FIELD(SPI_FIFOINTSTAT, RXLVL, 3, 1)
	FIELD(SPI_FIFOINTSTAT, PERINT, 4, 1)
REG32(SPI_FIFOWR, 0xe20)
	FIELD(SPI_FIFOWR, TXDATA, 0, 16)
	FIELD(SPI_FIFOWR, TXSSEL0_N, 16, 1)
	FIELD(SPI_FIFOWR, TXSSEL1_N, 17, 1)
	FIELD(SPI_FIFOWR, TXSSEL2_N, 18, 1)
	FIELD(SPI_FIFOWR, TXSSEL3_N, 19, 1)
	FIELD(SPI_FIFOWR, EOT, 20, 1)
	FIELD(SPI_FIFOWR, EOF, 21, 1)
	FIELD(SPI_FIFOWR, RXIGNORE, 22, 1)
	FIELD(SPI_FIFOWR, LEN, 24, 4)
REG32(SPI_FIFORD, 0xe30)
	FIELD(SPI_FIFORD, RXDATA, 0, 16)
	FIELD(SPI_FIFORD, RXSSEL0_N, 16, 1)
	FIELD(SPI_FIFORD, RXSSEL1_N, 17, 1)
	FIELD(SPI_FIFORD, RXSSEL2_N, 18, 1)
	FIELD(SPI_FIFORD, RXSSEL3_N, 19, 1)
	FIELD(SPI_FIFORD, SOT, 20, 1)
REG32(SPI_FIFORDNOPOP, 0xe40)
	FIELD(SPI_FIFORDNOPOP, RXDATA, 0, 16)
	FIELD(SPI_FIFORDNOPOP, RXSSEL0_N, 16, 1)
	FIELD(SPI_FIFORDNOPOP, RXSSEL1_N, 17, 1)
	FIELD(SPI_FIFORDNOPOP, RXSSEL2_N, 18, 1)
	FIELD(SPI_FIFORDNOPOP, RXSSEL3_N, 19, 1)
	FIELD(SPI_FIFORDNOPOP, SOT, 20, 1)
REG32(SPI_ID, 0xffc)
	FIELD(SPI_ID, APERTURE, 0, 8)
	FIELD(SPI_ID, MINOR_REV, 8, 4)
	FIELD(SPI_ID, MAJOR_REV, 12, 4)
	FIELD(SPI_ID, ID, 16, 16)


__attribute__((unused))
static void lpc51u68_spi_update(LPC51U68SPI *t) {
	int conditions = ((t->FIFOSTAT & 0x40) || ((t->FIFOCFG & 0x10) && (t->FIFOSTAT & 0x20)) || ((t->FIFOCFG & 0x20) && (t->FIFOSTAT & 0x20)));
	qemu_set_irq(t->irq[0], conditions);

}

static int lpc51u68_spi_can_receive(void *opaque) {
	return 1;
}

static void lpc51u68_spi_receive(void *opaque, const uint8_t *buf, int size) {
	LPC51U68SPI *t = LPC51U68_SPI(opaque);

	t->FIFORD = *buf;
	t->FIFOSTAT |= 0x40;

	lpc51u68_spi_update(t);
}

static gboolean lpc51u68_spi_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	LPC51U68SPI *t = LPC51U68_SPI(opaque);
	int ret;

	t->watch_tag = 0;
	t->FIFOSTAT &= (~(0x20));
	t->FIFOCFG &= (~(0x10));
	t->FIFOCFG &= (~(0x20));

	lpc51u68_spi_update(t);

	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->FIFOWR), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, lpc51u68_spi_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->FIFOSTAT |= 0x20;
	t->FIFOCFG |= 0x10;
	t->FIFOCFG |= 0x20;

	lpc51u68_spi_update(t);

	return FALSE;
}

static void lpc51u68_spi_register_reset(LPC51U68SPI *t) {
	t->CFG = 0x0;
	t->DLY = 0x0;
	t->STAT = 0x100;
	t->INTENSET = 0x0;
	t->INTENCLR = 0x0;
	t->DIV = 0x0;
	t->INTSTAT = 0x0;
	t->FIFOCFG = 0x0;
	t->FIFOSTAT = 0x30;
	t->FIFOTRIG = 0x0;
	t->FIFOINTENSET = 0x0;
	t->FIFOINTENCLR = 0x0;
	t->FIFOINTSTAT = 0x0;
	t->FIFOWR = 0x0;
	t->FIFORD = 0x0;
	t->FIFORDNOPOP = 0x0;
	t->ID = 0xe0200000;
	t->FIFOSTAT |= 0x20;
	t->FIFOCFG |= 0x10;
	t->FIFOCFG |= 0x20;

}

static uint64_t lpc51u68_spi_read(void *opaque, hwaddr offset, unsigned size) {
	LPC51U68SPI *t = LPC51U68_SPI(opaque);
	uint64_t ret;

	switch (offset) {
		case A_SPI_CFG:
			ret = t->CFG;
			break;
		case A_SPI_DLY:
			ret = t->DLY;
			break;
		case A_SPI_STAT:
			ret = t->STAT;
			break;
		case A_SPI_INTENSET:
			ret = t->INTENSET;
			break;
		case A_SPI_DIV:
			ret = t->DIV;
			break;
		case A_SPI_INTSTAT:
			ret = t->INTSTAT;
			break;
		case A_SPI_FIFOCFG:
			ret = t->FIFOCFG;
			break;
		case A_SPI_FIFOSTAT:
			ret = t->FIFOSTAT;
			break;
		case A_SPI_FIFOTRIG:
			ret = t->FIFOTRIG;
			break;
		case A_SPI_FIFOINTENSET:
			ret = t->FIFOINTENSET;
			break;
		case A_SPI_FIFOINTENCLR:
			ret = t->FIFOINTENCLR;
			break;
		case A_SPI_FIFOINTSTAT:
			ret = t->FIFOINTSTAT;
			break;
		case A_SPI_FIFOWR:
			ret = t->FIFOWR;
			break;
		case A_SPI_FIFORD:
			ret = t->FIFORD;
			t->FIFOSTAT &= (~(0x40));
			qemu_chr_fe_accept_input(&(t->chr));
			lpc51u68_spi_update(t);
			break;
		case A_SPI_FIFORDNOPOP:
			ret = t->FIFORDNOPOP;
			break;
		case A_SPI_ID:
			ret = t->ID;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "LPC51U68 SPI read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void lpc51u68_spi_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	LPC51U68SPI *t = LPC51U68_SPI(opaque);


	switch (offset) {
		case A_SPI_CFG ... A_SPI_CFG + 3:
			value = value << ((offset - A_SPI_CFG) << 3);
			offset = A_SPI_CFG;
			break;
		case A_SPI_DLY ... A_SPI_DLY + 3:
			value = value << ((offset - A_SPI_DLY) << 3);
			offset = A_SPI_DLY;
			break;
		case A_SPI_STAT ... A_SPI_STAT + 3:
			value = value << ((offset - A_SPI_STAT) << 3);
			offset = A_SPI_STAT;
			break;
		case A_SPI_INTENSET ... A_SPI_INTENSET + 3:
			value = value << ((offset - A_SPI_INTENSET) << 3);
			offset = A_SPI_INTENSET;
			break;
		case A_SPI_INTENCLR ... A_SPI_INTENCLR + 3:
			value = value << ((offset - A_SPI_INTENCLR) << 3);
			offset = A_SPI_INTENCLR;
			break;
		case A_SPI_DIV ... A_SPI_DIV + 3:
			value = value << ((offset - A_SPI_DIV) << 3);
			offset = A_SPI_DIV;
			break;
		case A_SPI_FIFOCFG ... A_SPI_FIFOCFG + 3:
			value = value << ((offset - A_SPI_FIFOCFG) << 3);
			offset = A_SPI_FIFOCFG;
			break;
		case A_SPI_FIFOSTAT ... A_SPI_FIFOSTAT + 3:
			value = value << ((offset - A_SPI_FIFOSTAT) << 3);
			offset = A_SPI_FIFOSTAT;
			break;
		case A_SPI_FIFOTRIG ... A_SPI_FIFOTRIG + 3:
			value = value << ((offset - A_SPI_FIFOTRIG) << 3);
			offset = A_SPI_FIFOTRIG;
			break;
		case A_SPI_FIFOINTENSET ... A_SPI_FIFOINTENSET + 3:
			value = value << ((offset - A_SPI_FIFOINTENSET) << 3);
			offset = A_SPI_FIFOINTENSET;
			break;
		case A_SPI_FIFOINTENCLR ... A_SPI_FIFOINTENCLR + 3:
			value = value << ((offset - A_SPI_FIFOINTENCLR) << 3);
			offset = A_SPI_FIFOINTENCLR;
			break;
		case A_SPI_FIFOWR ... A_SPI_FIFOWR + 3:
			value = value << ((offset - A_SPI_FIFOWR) << 3);
			offset = A_SPI_FIFOWR;
			break;

		default: break;
	}

	switch (offset) {
		case A_SPI_CFG:
			t->CFG = value;
			break;
		case A_SPI_DLY:
			t->DLY = value;
			break;
		case A_SPI_STAT:
			t->STAT = value;
			break;
		case A_SPI_INTENSET:
			t->INTENSET = value;
			break;
		case A_SPI_INTENCLR:
			t->INTENCLR = value;
			break;
		case A_SPI_DIV:
			t->DIV = value;
			break;
		case A_SPI_FIFOCFG:
			t->FIFOCFG &= value;
			lpc51u68_spi_update(t);
			break;
		case A_SPI_FIFOSTAT:
			t->FIFOSTAT &= value;
			if (((value & 0x1) && (value & 0x2))) {
				t->FIFOSTAT |= 0x10;
			}
			lpc51u68_spi_update(t);
			break;
		case A_SPI_FIFOTRIG:
			t->FIFOTRIG = value;
			break;
		case A_SPI_FIFOINTENSET:
			t->FIFOINTENSET = value;
			break;
		case A_SPI_FIFOINTENCLR:
			t->FIFOINTENCLR = value;
			break;
		case A_SPI_FIFOWR:
			t->FIFOWR = value;
			lpc51u68_spi_transmit(NULL, G_IO_OUT, t);
			if (((!(value & 0x100)) && (!(value & 0x200)) && (!(value & 0x400)) && (!(value & 0x800)) && (!(value & 0x1000)) && (!(value & 0x2000)) && (!(value & 0x4000)) && (!(value & 0x8000)) && (!(value & 0x400000)) && (!(value & 0x800000)) && (!(value & 0x8000000)) && (!(value & 0x10000000)) && (!(value & 0x20000000)) && (!(value & 0x40000000)) && (!(value & 0x80000000)))) {
				t->FIFOSTAT |= 0x10;
			} else if (((!(value & 0x100)) && (!(value & 0x200)) && (!(value & 0x400)) && (!(value & 0x800)) && (!(value & 0x1000)) && (!(value & 0x2000)) && (!(value & 0x4000)) && (!(value & 0x8000)))) {
				t->FIFOSTAT |= 0x10;
			} else if (((!(value & 0x400000)) && (!(value & 0x800000)) && (!(value & 0x8000000)) && (!(value & 0x10000000)) && (!(value & 0x20000000)) && (!(value & 0x40000000)) && (!(value & 0x80000000)))) {
				t->FIFOSTAT |= 0x10;
			} else if (((!(value & 0x400000)) && (!(value & 0x800000)) && (!(value & 0x10000000)) && (!(value & 0x20000000)) && (!(value & 0x40000000)) && (!(value & 0x80000000)))) {
				t->FIFOSTAT |= 0x10;
			}
			t->FIFOSTAT |= 0x10;
			lpc51u68_spi_update(t);
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "LPC51U68 SPI write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps lpc51u68_spi_ops = {
	.read = lpc51u68_spi_read,
	.write = lpc51u68_spi_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void lpc51u68_spi_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	LPC51U68SPI *t = LPC51U68_SPI(obj);
	memory_region_init_io(&(t->iomem), obj, &lpc51u68_spi_ops, t, "lpc51u68-spi", LPC51U68_SPI_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void lpc51u68_spi_realize(DeviceState *dev, Error **errp) {
	LPC51U68SPI *t = LPC51U68_SPI(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), lpc51u68_spi_can_receive, lpc51u68_spi_receive,
		NULL, NULL, t, NULL, true);

}

static void lpc51u68_spi_reset_enter(Object *obj, ResetType type) {
	LPC51U68SPI *t = LPC51U68_SPI(obj);
	lpc51u68_spi_register_reset(t);
}

static const VMStateDescription lpc51u68_spi_vmstate = {
	.name = "lpc51u68-spi",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CFG, LPC51U68SPI),
		VMSTATE_UINT32(DLY, LPC51U68SPI),
		VMSTATE_UINT32(STAT, LPC51U68SPI),
		VMSTATE_UINT32(INTENSET, LPC51U68SPI),
		VMSTATE_UINT32(INTENCLR, LPC51U68SPI),
		VMSTATE_UINT32(DIV, LPC51U68SPI),
		VMSTATE_UINT32(INTSTAT, LPC51U68SPI),
		VMSTATE_UINT32(FIFOCFG, LPC51U68SPI),
		VMSTATE_UINT32(FIFOSTAT, LPC51U68SPI),
		VMSTATE_UINT32(FIFOTRIG, LPC51U68SPI),
		VMSTATE_UINT32(FIFOINTENSET, LPC51U68SPI),
		VMSTATE_UINT32(FIFOINTENCLR, LPC51U68SPI),
		VMSTATE_UINT32(FIFOINTSTAT, LPC51U68SPI),
		VMSTATE_UINT32(FIFOWR, LPC51U68SPI),
		VMSTATE_UINT32(FIFORD, LPC51U68SPI),
		VMSTATE_UINT32(FIFORDNOPOP, LPC51U68SPI),
		VMSTATE_UINT32(ID, LPC51U68SPI),

		VMSTATE_END_OF_LIST()
	}
};

static Property lpc51u68_spi_properties[] = {
	DEFINE_PROP_CHR("chardev", LPC51U68SPI, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void lpc51u68_spi_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &lpc51u68_spi_vmstate;
	dc->realize = lpc51u68_spi_realize;
	rc->phases.enter = lpc51u68_spi_reset_enter;
	device_class_set_props(dc, lpc51u68_spi_properties);

}

static const TypeInfo lpc51u68_spi_info = {
	.name = TYPE_LPC51U68_SPI,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(LPC51U68SPI),
	.instance_init = lpc51u68_spi_init,
	.class_init = lpc51u68_spi_class_init,
};

static void lpc51u68_spi_register_types(void) {
	type_register_static(&lpc51u68_spi_info);
}

type_init(lpc51u68_spi_register_types);

#define TYPE_LPC51U68_SYSCON "lpc51u68-syscon"
OBJECT_DECLARE_SIMPLE_TYPE(LPC51U68SYSCON, LPC51U68_SYSCON)

struct LPC51U68SYSCON {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/*registers*/
	uint32_t AHBMATPRIO;
	uint32_t SYSTCKCAL;
	uint32_t NMISRC;
	uint32_t ASYNCAPBCTRL;
	uint32_t PRESETCTRL0;
	uint32_t PRESETCTRL1;
	uint32_t SYSRSTSTAT;
	uint32_t AHBCLKCTRL0;
	uint32_t AHBCLKCTRL1;
	uint32_t MAINCLKSELA;
	uint32_t MAINCLKSELB;
	uint32_t CLKOUTSELA;
	uint32_t SYSPLLCLKSEL;
	uint32_t ADCCLKSEL;
	uint32_t USBCLKSEL;
	uint32_t MCLKCLKSEL;
	uint32_t FRGCLKSEL;
	uint32_t SYSTICKCLKDIV;
	uint32_t AHBCLKDIV;
	uint32_t CLKOUTDIV;
	uint32_t ADCCLKDIV;
	uint32_t USBCLKDIV;
	uint32_t FRGCTRL;
	uint32_t MCLKDIV;
	uint32_t FLASHCFG;
	uint32_t USBCLKCTRL;
	uint32_t USBCLKSTAT;
	uint32_t FREQMECTRL;
	uint32_t MCLKIO;
	uint32_t FROCTRL;
	uint32_t WDTOSCCTRL;
	uint32_t RTCOSCCTRL;
	uint32_t SYSPLLCTRL;
	uint32_t SYSPLLSTAT;
	uint32_t SYSPLLNDEC;
	uint32_t SYSPLLPDEC;
	uint32_t SYSPLLSSCTRL0;
	uint32_t SYSPLLSSCTRL1;
	uint32_t PDSLEEPCFG0;
	uint32_t PDSLEEPCFG1;
	uint32_t PDRUNCFG0;
	uint32_t PDRUNCFGSET0;
	uint32_t PDRUNCFGCLR0;
	uint32_t STARTER0;
	uint32_t STARTERSET0;
	uint32_t STARTERCLR0;
	uint32_t HWWAKE;
	uint32_t JTAGIDCODE;
	uint32_t DEVICE_ID0;
	uint32_t DEVICE_ID1;
	uint32_t BODCTRL;
	uint32_t PIOPORCAP0;
	uint32_t PIOPORCAP1;
	uint32_t PIORESCAP0;
	uint32_t PIORESCAP1;
	uint32_t PRESETCTRLSET0;
	uint32_t PRESETCTRLSET1;
	uint32_t PRESETCTRLCLR0;
	uint32_t PRESETCTRLCLR1;
	uint32_t AHBCLKCTRLSET0;
	uint32_t AHBCLKCTRLSET1;
	uint32_t AHBCLKCTRLCLR0;
	uint32_t AHBCLKCTRLCLR1;
	uint32_t FXCOMCLKSEL0;
	uint32_t FXCOMCLKSEL1;
	uint32_t FXCOMCLKSEL2;
	uint32_t FXCOMCLKSEL3;
	uint32_t FXCOMCLKSEL4;
	uint32_t FXCOMCLKSEL5;
	uint32_t FXCOMCLKSEL6;
	uint32_t FXCOMCLKSEL7;
	/* base */
	uint32_t base;

};
#define LPC51U68_SYSCON_SIZE				0x20048

REG32(SYSCON_AHBMATPRIO, 0x10)
	FIELD(SYSCON_AHBMATPRIO, PRI_ICODE, 0, 2)
	FIELD(SYSCON_AHBMATPRIO, PRI_DCODE, 2, 2)
	FIELD(SYSCON_AHBMATPRIO, PRI_SYS, 4, 2)
	FIELD(SYSCON_AHBMATPRIO, PRI_M0, 6, 2)
	FIELD(SYSCON_AHBMATPRIO, PRI_USB, 8, 2)
	FIELD(SYSCON_AHBMATPRIO, PRI_DMA, 10, 2)
REG32(SYSCON_SYSTCKCAL, 0x40)
	FIELD(SYSCON_SYSTCKCAL, CAL, 0, 24)
	FIELD(SYSCON_SYSTCKCAL, SKEW, 24, 1)
	FIELD(SYSCON_SYSTCKCAL, NOREF, 25, 1)
REG32(SYSCON_NMISRC, 0x48)
	FIELD(SYSCON_NMISRC, IRQM4, 0, 6)
	FIELD(SYSCON_NMISRC, IRQM0, 8, 6)
	FIELD(SYSCON_NMISRC, NMIENM0, 30, 1)
	FIELD(SYSCON_NMISRC, NMIENM4, 31, 1)
REG32(SYSCON_ASYNCAPBCTRL, 0x4c)
	FIELD(SYSCON_ASYNCAPBCTRL, ENABLE, 0, 1)
REG32(SYSCON_PRESETCTRL0, 0x100)
	FIELD(SYSCON_PRESETCTRL0, FLASH_RST, 7, 1)
	FIELD(SYSCON_PRESETCTRL0, FMC_RST, 8, 1)
	FIELD(SYSCON_PRESETCTRL0, MUX_RST, 11, 1)
	FIELD(SYSCON_PRESETCTRL0, IOCON_RST, 13, 1)
	FIELD(SYSCON_PRESETCTRL0, GPIO0_RST, 14, 1)
	FIELD(SYSCON_PRESETCTRL0, GPIO1_RST, 15, 1)
	FIELD(SYSCON_PRESETCTRL0, PINT_RST, 18, 1)
	FIELD(SYSCON_PRESETCTRL0, GINT_RST, 19, 1)
	FIELD(SYSCON_PRESETCTRL0, DMA0_RST, 20, 1)
	FIELD(SYSCON_PRESETCTRL0, CRC_RST, 21, 1)
	FIELD(SYSCON_PRESETCTRL0, WWDT_RST, 22, 1)
	FIELD(SYSCON_PRESETCTRL0, ADC0_RST, 27, 1)
REG32(SYSCON_PRESETCTRL1, 0x104)
	FIELD(SYSCON_PRESETCTRL1, MRT0_RST, 0, 1)
	FIELD(SYSCON_PRESETCTRL1, SCT0_RST, 2, 1)
	FIELD(SYSCON_PRESETCTRL1, UTICK0_RST, 10, 1)
	FIELD(SYSCON_PRESETCTRL1, FC0_RST, 11, 1)
	FIELD(SYSCON_PRESETCTRL1, FC1_RST, 12, 1)
	FIELD(SYSCON_PRESETCTRL1, FC2_RST, 13, 1)
	FIELD(SYSCON_PRESETCTRL1, FC3_RST, 14, 1)
	FIELD(SYSCON_PRESETCTRL1, FC4_RST, 15, 1)
	FIELD(SYSCON_PRESETCTRL1, FC5_RST, 16, 1)
	FIELD(SYSCON_PRESETCTRL1, FC6_RST, 17, 1)
	FIELD(SYSCON_PRESETCTRL1, FC7_RST, 18, 1)
	FIELD(SYSCON_PRESETCTRL1, USB0_RST, 25, 1)
	FIELD(SYSCON_PRESETCTRL1, CTIMER0_RST, 26, 1)
	FIELD(SYSCON_PRESETCTRL1, CTIMER1_RST, 27, 1)
REG32(SYSCON_SYSRSTSTAT, 0x1f0)
	FIELD(SYSCON_SYSRSTSTAT, POR, 0, 1)
	FIELD(SYSCON_SYSRSTSTAT, EXTRST, 1, 1)
	FIELD(SYSCON_SYSRSTSTAT, WDT, 2, 1)
	FIELD(SYSCON_SYSRSTSTAT, BOD, 3, 1)
	FIELD(SYSCON_SYSRSTSTAT, SYSRST, 4, 1)
REG32(SYSCON_AHBCLKCTRL0, 0x200)
	FIELD(SYSCON_AHBCLKCTRL0, ROM, 1, 1)
	FIELD(SYSCON_AHBCLKCTRL0, FLASH, 7, 1)
	FIELD(SYSCON_AHBCLKCTRL0, FMC, 8, 1)
	FIELD(SYSCON_AHBCLKCTRL0, INPUTMUX, 11, 1)
	FIELD(SYSCON_AHBCLKCTRL0, IOCON, 13, 1)
	FIELD(SYSCON_AHBCLKCTRL0, GPIO0, 14, 1)
	FIELD(SYSCON_AHBCLKCTRL0, GPIO1, 15, 1)
	FIELD(SYSCON_AHBCLKCTRL0, PINT, 18, 1)
	FIELD(SYSCON_AHBCLKCTRL0, GINT, 19, 1)
	FIELD(SYSCON_AHBCLKCTRL0, DMA0, 20, 1)
	FIELD(SYSCON_AHBCLKCTRL0, CRC, 21, 1)
	FIELD(SYSCON_AHBCLKCTRL0, WWDT, 22, 1)
	FIELD(SYSCON_AHBCLKCTRL0, RTC, 23, 1)
	FIELD(SYSCON_AHBCLKCTRL0, ADC0, 27, 1)
REG32(SYSCON_AHBCLKCTRL1, 0x204)
	FIELD(SYSCON_AHBCLKCTRL1, MRT0, 0, 1)
	FIELD(SYSCON_AHBCLKCTRL1, SCT0, 2, 1)
	FIELD(SYSCON_AHBCLKCTRL1, UTICK0, 10, 1)
	FIELD(SYSCON_AHBCLKCTRL1, FLEXCOMM0, 11, 1)
	FIELD(SYSCON_AHBCLKCTRL1, FLEXCOMM1, 12, 1)
	FIELD(SYSCON_AHBCLKCTRL1, FLEXCOMM2, 13, 1)
	FIELD(SYSCON_AHBCLKCTRL1, FLEXCOMM3, 14, 1)
	FIELD(SYSCON_AHBCLKCTRL1, FLEXCOMM4, 15, 1)
	FIELD(SYSCON_AHBCLKCTRL1, FLEXCOMM5, 16, 1)
	FIELD(SYSCON_AHBCLKCTRL1, FLEXCOMM6, 17, 1)
	FIELD(SYSCON_AHBCLKCTRL1, FLEXCOMM7, 18, 1)
	FIELD(SYSCON_AHBCLKCTRL1, USB0, 25, 1)
	FIELD(SYSCON_AHBCLKCTRL1, CTIMER0, 26, 1)
	FIELD(SYSCON_AHBCLKCTRL1, CTIMER1, 27, 1)
REG32(SYSCON_MAINCLKSELA, 0x280)
	FIELD(SYSCON_MAINCLKSELA, SEL, 0, 2)
REG32(SYSCON_MAINCLKSELB, 0x284)
	FIELD(SYSCON_MAINCLKSELB, SEL, 0, 2)
REG32(SYSCON_CLKOUTSELA, 0x288)
	FIELD(SYSCON_CLKOUTSELA, SEL, 0, 3)
REG32(SYSCON_SYSPLLCLKSEL, 0x290)
	FIELD(SYSCON_SYSPLLCLKSEL, SEL, 0, 3)
REG32(SYSCON_ADCCLKSEL, 0x2a4)
	FIELD(SYSCON_ADCCLKSEL, SEL, 0, 3)
REG32(SYSCON_USBCLKSEL, 0x2a8)
	FIELD(SYSCON_USBCLKSEL, SEL, 0, 3)
REG32(SYSCON_MCLKCLKSEL, 0x2e0)
	FIELD(SYSCON_MCLKCLKSEL, SEL, 0, 3)
REG32(SYSCON_FRGCLKSEL, 0x2e8)
	FIELD(SYSCON_FRGCLKSEL, SEL, 0, 3)
REG32(SYSCON_SYSTICKCLKDIV, 0x300)
	FIELD(SYSCON_SYSTICKCLKDIV, DIV, 0, 8)
REG32(SYSCON_AHBCLKDIV, 0x380)
	FIELD(SYSCON_AHBCLKDIV, DIV, 0, 8)
REG32(SYSCON_CLKOUTDIV, 0x384)
	FIELD(SYSCON_CLKOUTDIV, DIV, 0, 8)
REG32(SYSCON_ADCCLKDIV, 0x394)
	FIELD(SYSCON_ADCCLKDIV, DIV, 0, 8)
REG32(SYSCON_USBCLKDIV, 0x398)
	FIELD(SYSCON_USBCLKDIV, DIV, 0, 8)
REG32(SYSCON_FRGCTRL, 0x3a0)
	FIELD(SYSCON_FRGCTRL, DIV, 0, 8)
	FIELD(SYSCON_FRGCTRL, MULT, 8, 8)
REG32(SYSCON_MCLKDIV, 0x3ac)
	FIELD(SYSCON_MCLKDIV, DIV, 0, 8)
REG32(SYSCON_FLASHCFG, 0x400)
	FIELD(SYSCON_FLASHCFG, FETCHCFG, 0, 2)
	FIELD(SYSCON_FLASHCFG, DATACFG, 2, 2)
	FIELD(SYSCON_FLASHCFG, ACCEL, 4, 1)
	FIELD(SYSCON_FLASHCFG, PREFEN, 5, 1)
	FIELD(SYSCON_FLASHCFG, PREFOVR, 6, 1)
	FIELD(SYSCON_FLASHCFG, FLASHTIM, 12, 4)
REG32(SYSCON_USBCLKCTRL, 0x40c)
	FIELD(SYSCON_USBCLKCTRL, POL_CLK, 1, 1)
REG32(SYSCON_USBCLKSTAT, 0x410)
	FIELD(SYSCON_USBCLKSTAT, NEED_CLKST, 0, 1)
REG32(SYSCON_FREQMECTRL, 0x418)
	FIELD(SYSCON_FREQMECTRL, CAPVAL, 0, 14)
	FIELD(SYSCON_FREQMECTRL, PROG, 31, 1)
REG32(SYSCON_MCLKIO, 0x420)
	FIELD(SYSCON_MCLKIO, DIR, 0, 1)
REG32(SYSCON_FROCTRL, 0x500)
	FIELD(SYSCON_FROCTRL, TRIM, 0, 14)
	FIELD(SYSCON_FROCTRL, SEL, 14, 1)
	FIELD(SYSCON_FROCTRL, FREQTRIM, 16, 8)
	FIELD(SYSCON_FROCTRL, USBCLKADJ, 24, 1)
	FIELD(SYSCON_FROCTRL, USBMODCHG, 25, 1)
	FIELD(SYSCON_FROCTRL, HSPDCLK, 30, 1)
	FIELD(SYSCON_FROCTRL, WRTRIM, 31, 1)
REG32(SYSCON_WDTOSCCTRL, 0x508)
	FIELD(SYSCON_WDTOSCCTRL, DIVSEL, 0, 5)
	FIELD(SYSCON_WDTOSCCTRL, FREQSEL, 5, 5)
REG32(SYSCON_RTCOSCCTRL, 0x50c)
	FIELD(SYSCON_RTCOSCCTRL, EN, 0, 1)
REG32(SYSCON_SYSPLLCTRL, 0x580)
	FIELD(SYSCON_SYSPLLCTRL, SELR, 0, 4)
	FIELD(SYSCON_SYSPLLCTRL, SELI, 4, 6)
	FIELD(SYSCON_SYSPLLCTRL, SELP, 10, 5)
	FIELD(SYSCON_SYSPLLCTRL, BYPASS, 15, 1)
	FIELD(SYSCON_SYSPLLCTRL, BYPASSCCODIV2, 16, 1)
	FIELD(SYSCON_SYSPLLCTRL, UPLIMOFF, 17, 1)
	FIELD(SYSCON_SYSPLLCTRL, BANDSEL, 18, 1)
	FIELD(SYSCON_SYSPLLCTRL, DIRECTI, 19, 1)
	FIELD(SYSCON_SYSPLLCTRL, DIRECTO, 20, 1)
REG32(SYSCON_SYSPLLSTAT, 0x584)
	FIELD(SYSCON_SYSPLLSTAT, LOCK, 0, 1)
REG32(SYSCON_SYSPLLNDEC, 0x588)
	FIELD(SYSCON_SYSPLLNDEC, NDEC, 0, 10)
	FIELD(SYSCON_SYSPLLNDEC, NREQ, 10, 1)
REG32(SYSCON_SYSPLLPDEC, 0x58c)
	FIELD(SYSCON_SYSPLLPDEC, PDEC, 0, 7)
	FIELD(SYSCON_SYSPLLPDEC, PREQ, 7, 1)
REG32(SYSCON_SYSPLLSSCTRL0, 0x590)
	FIELD(SYSCON_SYSPLLSSCTRL0, MDEC, 0, 17)
	FIELD(SYSCON_SYSPLLSSCTRL0, MREQ, 17, 1)
	FIELD(SYSCON_SYSPLLSSCTRL0, SEL_EXT, 18, 1)
REG32(SYSCON_SYSPLLSSCTRL1, 0x594)
	FIELD(SYSCON_SYSPLLSSCTRL1, MD, 0, 19)
	FIELD(SYSCON_SYSPLLSSCTRL1, MDREQ, 19, 1)
	FIELD(SYSCON_SYSPLLSSCTRL1, MF, 20, 3)
	FIELD(SYSCON_SYSPLLSSCTRL1, MR, 23, 3)
	FIELD(SYSCON_SYSPLLSSCTRL1, MC, 26, 2)
	FIELD(SYSCON_SYSPLLSSCTRL1, PD, 28, 1)
	FIELD(SYSCON_SYSPLLSSCTRL1, DITHER, 29, 1)
REG32(SYSCON_PDSLEEPCFG0, 0x600)
	FIELD(SYSCON_PDSLEEPCFG0, PD_SLEEP, 0, 32)
REG32(SYSCON_PDSLEEPCFG1, 0x604)
	FIELD(SYSCON_PDSLEEPCFG1, PD_SLEEP, 0, 32)
REG32(SYSCON_PDRUNCFG0, 0x610)
	FIELD(SYSCON_PDRUNCFG0, PDEN_FRO, 4, 1)
	FIELD(SYSCON_PDRUNCFG0, PDEN_TS, 6, 1)
	FIELD(SYSCON_PDRUNCFG0, PDEN_BOD_RST, 7, 1)
	FIELD(SYSCON_PDRUNCFG0, PDEN_BOD_INTR, 8, 1)
	FIELD(SYSCON_PDRUNCFG0, PDEN_ADC0, 10, 1)
	FIELD(SYSCON_PDRUNCFG0, PDEN_SRAM0, 13, 1)
	FIELD(SYSCON_PDRUNCFG0, PDEN_SRAMX, 16, 1)
	FIELD(SYSCON_PDRUNCFG0, PDEN_ROM, 17, 1)
	FIELD(SYSCON_PDRUNCFG0, PDEN_VDDA, 19, 1)
	FIELD(SYSCON_PDRUNCFG0, PDEN_WDT_OSC, 20, 1)
	FIELD(SYSCON_PDRUNCFG0, PDEN_USB_PHY, 21, 1)
	FIELD(SYSCON_PDRUNCFG0, PDEN_SYS_PLL, 22, 1)
	FIELD(SYSCON_PDRUNCFG0, PDEN_VREFP, 23, 1)
REG32(SYSCON_PDRUNCFGSET0, 0x620)
	FIELD(SYSCON_PDRUNCFGSET0, PD_SET, 0, 32)
REG32(SYSCON_PDRUNCFGCLR0, 0x630)
	FIELD(SYSCON_PDRUNCFGCLR0, PD_CLR, 0, 32)
REG32(SYSCON_STARTER0, 0x680)
	FIELD(SYSCON_STARTER0, WDT_BOD, 0, 1)
	FIELD(SYSCON_STARTER0, DMA0, 1, 1)
	FIELD(SYSCON_STARTER0, GINT0, 2, 1)
	FIELD(SYSCON_STARTER0, GINT1, 3, 1)
	FIELD(SYSCON_STARTER0, PIN_INT0, 4, 1)
	FIELD(SYSCON_STARTER0, PIN_INT1, 5, 1)
	FIELD(SYSCON_STARTER0, PIN_INT2, 6, 1)
	FIELD(SYSCON_STARTER0, PIN_INT3, 7, 1)
	FIELD(SYSCON_STARTER0, UTICK0, 8, 1)
	FIELD(SYSCON_STARTER0, MRT0, 9, 1)
	FIELD(SYSCON_STARTER0, CTIMER0, 10, 1)
	FIELD(SYSCON_STARTER0, CTIMER1, 11, 1)
	FIELD(SYSCON_STARTER0, SCT0, 12, 1)
	FIELD(SYSCON_STARTER0, CTIMER3, 13, 1)
	FIELD(SYSCON_STARTER0, FLEXCOMM0, 14, 1)
	FIELD(SYSCON_STARTER0, FLEXCOMM1, 15, 1)
	FIELD(SYSCON_STARTER0, FLEXCOMM2, 16, 1)
	FIELD(SYSCON_STARTER0, FLEXCOMM3, 17, 1)
	FIELD(SYSCON_STARTER0, FLEXCOMM4, 18, 1)
	FIELD(SYSCON_STARTER0, FLEXCOMM5, 19, 1)
	FIELD(SYSCON_STARTER0, FLEXCOMM6, 20, 1)
	FIELD(SYSCON_STARTER0, FLEXCOMM7, 21, 1)
	FIELD(SYSCON_STARTER0, USB0_NEEDCLK, 27, 1)
	FIELD(SYSCON_STARTER0, USB0, 28, 1)
	FIELD(SYSCON_STARTER0, RTC, 29, 1)
REG32(SYSCON_STARTERSET0, 0x6a0)
	FIELD(SYSCON_STARTERSET0, START_SET, 0, 32)
REG32(SYSCON_STARTERCLR0, 0x6c0)
	FIELD(SYSCON_STARTERCLR0, START_CLR, 0, 32)
REG32(SYSCON_HWWAKE, 0x780)
	FIELD(SYSCON_HWWAKE, FORCEWAKE, 0, 1)
	FIELD(SYSCON_HWWAKE, FCWAKE, 1, 1)
	FIELD(SYSCON_HWWAKE, WAKEDMA, 3, 1)
REG32(SYSCON_JTAGIDCODE, 0xff4)
	FIELD(SYSCON_JTAGIDCODE, JTAGID, 0, 32)
REG32(SYSCON_DEVICE_ID0, 0xff8)
	FIELD(SYSCON_DEVICE_ID0, PARTID, 0, 32)
REG32(SYSCON_DEVICE_ID1, 0xffc)
	FIELD(SYSCON_DEVICE_ID1, REVID, 0, 32)
REG32(SYSCON_BODCTRL, 0x20044)
	FIELD(SYSCON_BODCTRL, BODRSTLEV, 0, 2)
	FIELD(SYSCON_BODCTRL, BODRSTENA, 2, 1)
	FIELD(SYSCON_BODCTRL, BODINTLEV, 3, 2)
	FIELD(SYSCON_BODCTRL, BODINTENA, 5, 1)
	FIELD(SYSCON_BODCTRL, BODRSTSTAT, 6, 1)
	FIELD(SYSCON_BODCTRL, BODINTSTAT, 7, 1)
REG32(SYSCON_PIOPORCAP0, 0xc0)
	FIELD(SYSCON_PIOPORCAP0, PIOPORCAP, 0, 32)
REG32(SYSCON_PIOPORCAP1, 0xc4)
	FIELD(SYSCON_PIOPORCAP1, PIOPORCAP, 0, 32)
REG32(SYSCON_PIORESCAP0, 0xd0)
	FIELD(SYSCON_PIORESCAP0, PIORESCAP, 0, 32)
REG32(SYSCON_PIORESCAP1, 0xd4)
	FIELD(SYSCON_PIORESCAP1, PIORESCAP, 0, 32)
REG32(SYSCON_PRESETCTRLSET0, 0x120)
	FIELD(SYSCON_PRESETCTRLSET0, RST_SET, 0, 32)
REG32(SYSCON_PRESETCTRLSET1, 0x124)
	FIELD(SYSCON_PRESETCTRLSET1, RST_SET, 0, 32)
REG32(SYSCON_PRESETCTRLCLR0, 0x140)
	FIELD(SYSCON_PRESETCTRLCLR0, RST_CLR, 0, 32)
REG32(SYSCON_PRESETCTRLCLR1, 0x144)
	FIELD(SYSCON_PRESETCTRLCLR1, RST_CLR, 0, 32)
REG32(SYSCON_AHBCLKCTRLSET0, 0x220)
	FIELD(SYSCON_AHBCLKCTRLSET0, CLK_SET, 0, 32)
REG32(SYSCON_AHBCLKCTRLSET1, 0x224)
	FIELD(SYSCON_AHBCLKCTRLSET1, CLK_SET, 0, 32)
REG32(SYSCON_AHBCLKCTRLCLR0, 0x240)
	FIELD(SYSCON_AHBCLKCTRLCLR0, CLK_CLR, 0, 32)
REG32(SYSCON_AHBCLKCTRLCLR1, 0x244)
	FIELD(SYSCON_AHBCLKCTRLCLR1, CLK_CLR, 0, 32)
REG32(SYSCON_FXCOMCLKSEL0, 0x2b0)
	FIELD(SYSCON_FXCOMCLKSEL0, SEL, 0, 3)
REG32(SYSCON_FXCOMCLKSEL1, 0x2b4)
	FIELD(SYSCON_FXCOMCLKSEL1, SEL, 0, 3)
REG32(SYSCON_FXCOMCLKSEL2, 0x2b8)
	FIELD(SYSCON_FXCOMCLKSEL2, SEL, 0, 3)
REG32(SYSCON_FXCOMCLKSEL3, 0x2bc)
	FIELD(SYSCON_FXCOMCLKSEL3, SEL, 0, 3)
REG32(SYSCON_FXCOMCLKSEL4, 0x2c0)
	FIELD(SYSCON_FXCOMCLKSEL4, SEL, 0, 3)
REG32(SYSCON_FXCOMCLKSEL5, 0x2c4)
	FIELD(SYSCON_FXCOMCLKSEL5, SEL, 0, 3)
REG32(SYSCON_FXCOMCLKSEL6, 0x2c8)
	FIELD(SYSCON_FXCOMCLKSEL6, SEL, 0, 3)
REG32(SYSCON_FXCOMCLKSEL7, 0x2cc)
	FIELD(SYSCON_FXCOMCLKSEL7, SEL, 0, 3)


static void lpc51u68_syscon_register_reset(LPC51U68SYSCON *t) {
	t->AHBMATPRIO = 0x0;
	t->SYSTCKCAL = 0x0;
	t->NMISRC = 0x0;
	t->ASYNCAPBCTRL = 0x1;
	t->PRESETCTRL0 = 0x0;
	t->PRESETCTRL1 = 0x0;
	t->SYSRSTSTAT = 0x0;
	t->AHBCLKCTRL0 = 0x183;
	t->AHBCLKCTRL1 = 0x0;
	t->MAINCLKSELA = 0x0;
	t->MAINCLKSELB = 0x0;
	t->CLKOUTSELA = 0x7;
	t->SYSPLLCLKSEL = 0x7;
	t->ADCCLKSEL = 0x7;
	t->USBCLKSEL = 0x7;
	t->MCLKCLKSEL = 0x7;
	t->FRGCLKSEL = 0x7;
	t->SYSTICKCLKDIV = 0x0;
	t->AHBCLKDIV = 0x0;
	t->CLKOUTDIV = 0x0;
	t->ADCCLKDIV = 0x0;
	t->USBCLKDIV = 0x0;
	t->FRGCTRL = 0xff;
	t->MCLKDIV = 0x0;
	t->FLASHCFG = 0x1a;
	t->USBCLKCTRL = 0x0;
	t->USBCLKSTAT = 0x0;
	t->FREQMECTRL = 0x0;
	t->MCLKIO = 0x0;
	t->FROCTRL = 0x40004000;
	t->WDTOSCCTRL = 0xa0;
	t->RTCOSCCTRL = 0x1;
	t->SYSPLLCTRL = 0x0;
	t->SYSPLLSTAT = 0x0;
	t->SYSPLLNDEC = 0x0;
	t->SYSPLLPDEC = 0x0;
	t->SYSPLLSSCTRL0 = 0x40000;
	t->SYSPLLSSCTRL1 = 0x10000000;
	t->PDSLEEPCFG0 = 0xf80540;
	t->PDSLEEPCFG1 = 0x10000000;
	t->PDRUNCFG0 = 0xf80540;
	t->PDRUNCFGSET0 = 0x0;
	t->PDRUNCFGCLR0 = 0x0;
	t->STARTER0 = 0x0;
	t->STARTERSET0 = 0x0;
	t->STARTERCLR0 = 0x0;
	t->HWWAKE = 0x0;
	t->JTAGIDCODE = 0x0;
	t->DEVICE_ID0 = 0x0;
	t->DEVICE_ID1 = 0x0;
	t->BODCTRL = 0x0;
	t->PIOPORCAP0 = 0x0;
	t->PIOPORCAP1 = 0x0;
	t->PIORESCAP0 = 0x0;
	t->PIORESCAP1 = 0x0;
	t->PRESETCTRLSET0 = 0x0;
	t->PRESETCTRLSET1 = 0x0;
	t->PRESETCTRLCLR0 = 0x0;
	t->PRESETCTRLCLR1 = 0x0;
	t->AHBCLKCTRLSET0 = 0x0;
	t->AHBCLKCTRLSET1 = 0x0;
	t->AHBCLKCTRLCLR0 = 0x0;
	t->AHBCLKCTRLCLR1 = 0x0;
	t->FXCOMCLKSEL0 = 0x7;
	t->FXCOMCLKSEL1 = 0x7;
	t->FXCOMCLKSEL2 = 0x7;
	t->FXCOMCLKSEL3 = 0x7;
	t->FXCOMCLKSEL4 = 0x7;
	t->FXCOMCLKSEL5 = 0x7;
	t->FXCOMCLKSEL6 = 0x7;
	t->FXCOMCLKSEL7 = 0x7;

}

static uint64_t lpc51u68_syscon_read(void *opaque, hwaddr offset, unsigned size) {
	LPC51U68SYSCON *t = LPC51U68_SYSCON(opaque);
	uint64_t ret;

	switch (offset) {
		case A_SYSCON_AHBMATPRIO:
			ret = t->AHBMATPRIO;
			break;
		case A_SYSCON_SYSTCKCAL:
			ret = t->SYSTCKCAL;
			break;
		case A_SYSCON_NMISRC:
			ret = t->NMISRC;
			break;
		case A_SYSCON_ASYNCAPBCTRL:
			ret = t->ASYNCAPBCTRL;
			break;
		case A_SYSCON_PRESETCTRL0:
			ret = t->PRESETCTRL0;
			break;
		case A_SYSCON_PRESETCTRL1:
			ret = t->PRESETCTRL1;
			break;
		case A_SYSCON_SYSRSTSTAT:
			ret = t->SYSRSTSTAT;
			break;
		case A_SYSCON_AHBCLKCTRL0:
			ret = t->AHBCLKCTRL0;
			break;
		case A_SYSCON_AHBCLKCTRL1:
			ret = t->AHBCLKCTRL1;
			break;
		case A_SYSCON_MAINCLKSELA:
			ret = t->MAINCLKSELA;
			break;
		case A_SYSCON_MAINCLKSELB:
			ret = t->MAINCLKSELB;
			break;
		case A_SYSCON_CLKOUTSELA:
			ret = t->CLKOUTSELA;
			break;
		case A_SYSCON_SYSPLLCLKSEL:
			ret = t->SYSPLLCLKSEL;
			break;
		case A_SYSCON_ADCCLKSEL:
			ret = t->ADCCLKSEL;
			break;
		case A_SYSCON_USBCLKSEL:
			ret = t->USBCLKSEL;
			break;
		case A_SYSCON_MCLKCLKSEL:
			ret = t->MCLKCLKSEL;
			break;
		case A_SYSCON_FRGCLKSEL:
			ret = t->FRGCLKSEL;
			break;
		case A_SYSCON_SYSTICKCLKDIV:
			ret = t->SYSTICKCLKDIV;
			break;
		case A_SYSCON_AHBCLKDIV:
			ret = t->AHBCLKDIV;
			break;
		case A_SYSCON_CLKOUTDIV:
			ret = t->CLKOUTDIV;
			break;
		case A_SYSCON_ADCCLKDIV:
			ret = t->ADCCLKDIV;
			break;
		case A_SYSCON_USBCLKDIV:
			ret = t->USBCLKDIV;
			break;
		case A_SYSCON_FRGCTRL:
			ret = t->FRGCTRL;
			break;
		case A_SYSCON_MCLKDIV:
			ret = t->MCLKDIV;
			break;
		case A_SYSCON_FLASHCFG:
			ret = t->FLASHCFG;
			break;
		case A_SYSCON_USBCLKCTRL:
			ret = t->USBCLKCTRL;
			break;
		case A_SYSCON_USBCLKSTAT:
			ret = t->USBCLKSTAT;
			break;
		case A_SYSCON_FREQMECTRL:
			ret = t->FREQMECTRL;
			break;
		case A_SYSCON_MCLKIO:
			ret = t->MCLKIO;
			break;
		case A_SYSCON_FROCTRL:
			ret = t->FROCTRL;
			break;
		case A_SYSCON_WDTOSCCTRL:
			ret = t->WDTOSCCTRL;
			break;
		case A_SYSCON_RTCOSCCTRL:
			ret = t->RTCOSCCTRL;
			break;
		case A_SYSCON_SYSPLLCTRL:
			ret = t->SYSPLLCTRL;
			break;
		case A_SYSCON_SYSPLLSTAT:
			ret = t->SYSPLLSTAT;
			break;
		case A_SYSCON_SYSPLLNDEC:
			ret = t->SYSPLLNDEC;
			break;
		case A_SYSCON_SYSPLLPDEC:
			ret = t->SYSPLLPDEC;
			break;
		case A_SYSCON_SYSPLLSSCTRL0:
			ret = t->SYSPLLSSCTRL0;
			break;
		case A_SYSCON_SYSPLLSSCTRL1:
			ret = t->SYSPLLSSCTRL1;
			break;
		case A_SYSCON_PDSLEEPCFG0:
			ret = t->PDSLEEPCFG0;
			break;
		case A_SYSCON_PDSLEEPCFG1:
			ret = t->PDSLEEPCFG1;
			break;
		case A_SYSCON_PDRUNCFG0:
			ret = t->PDRUNCFG0;
			break;
		case A_SYSCON_STARTER0:
			ret = t->STARTER0;
			break;
		case A_SYSCON_HWWAKE:
			ret = t->HWWAKE;
			break;
		case A_SYSCON_JTAGIDCODE:
			ret = t->JTAGIDCODE;
			break;
		case A_SYSCON_DEVICE_ID0:
			ret = t->DEVICE_ID0;
			break;
		case A_SYSCON_DEVICE_ID1:
			ret = t->DEVICE_ID1;
			break;
		case A_SYSCON_BODCTRL:
			ret = t->BODCTRL;
			break;
		case A_SYSCON_PIOPORCAP0:
			ret = t->PIOPORCAP0;
			break;
		case A_SYSCON_PIOPORCAP1:
			ret = t->PIOPORCAP1;
			break;
		case A_SYSCON_PIORESCAP0:
			ret = t->PIORESCAP0;
			break;
		case A_SYSCON_PIORESCAP1:
			ret = t->PIORESCAP1;
			break;
		case A_SYSCON_FXCOMCLKSEL0:
			ret = t->FXCOMCLKSEL0;
			break;
		case A_SYSCON_FXCOMCLKSEL1:
			ret = t->FXCOMCLKSEL1;
			break;
		case A_SYSCON_FXCOMCLKSEL2:
			ret = t->FXCOMCLKSEL2;
			break;
		case A_SYSCON_FXCOMCLKSEL3:
			ret = t->FXCOMCLKSEL3;
			break;
		case A_SYSCON_FXCOMCLKSEL4:
			ret = t->FXCOMCLKSEL4;
			break;
		case A_SYSCON_FXCOMCLKSEL5:
			ret = t->FXCOMCLKSEL5;
			break;
		case A_SYSCON_FXCOMCLKSEL6:
			ret = t->FXCOMCLKSEL6;
			break;
		case A_SYSCON_FXCOMCLKSEL7:
			ret = t->FXCOMCLKSEL7;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "LPC51U68 SYSCON read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void lpc51u68_syscon_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	LPC51U68SYSCON *t = LPC51U68_SYSCON(opaque);


	switch (offset) {
		case A_SYSCON_AHBMATPRIO ... A_SYSCON_AHBMATPRIO + 3:
			value = value << ((offset - A_SYSCON_AHBMATPRIO) << 3);
			offset = A_SYSCON_AHBMATPRIO;
			break;
		case A_SYSCON_SYSTCKCAL ... A_SYSCON_SYSTCKCAL + 3:
			value = value << ((offset - A_SYSCON_SYSTCKCAL) << 3);
			offset = A_SYSCON_SYSTCKCAL;
			break;
		case A_SYSCON_NMISRC ... A_SYSCON_NMISRC + 3:
			value = value << ((offset - A_SYSCON_NMISRC) << 3);
			offset = A_SYSCON_NMISRC;
			break;
		case A_SYSCON_ASYNCAPBCTRL ... A_SYSCON_ASYNCAPBCTRL + 3:
			value = value << ((offset - A_SYSCON_ASYNCAPBCTRL) << 3);
			offset = A_SYSCON_ASYNCAPBCTRL;
			break;
		case A_SYSCON_PRESETCTRL0 ... A_SYSCON_PRESETCTRL0 + 3:
			value = value << ((offset - A_SYSCON_PRESETCTRL0) << 3);
			offset = A_SYSCON_PRESETCTRL0;
			break;
		case A_SYSCON_PRESETCTRL1 ... A_SYSCON_PRESETCTRL1 + 3:
			value = value << ((offset - A_SYSCON_PRESETCTRL1) << 3);
			offset = A_SYSCON_PRESETCTRL1;
			break;
		case A_SYSCON_SYSRSTSTAT ... A_SYSCON_SYSRSTSTAT + 3:
			value = value << ((offset - A_SYSCON_SYSRSTSTAT) << 3);
			offset = A_SYSCON_SYSRSTSTAT;
			break;
		case A_SYSCON_AHBCLKCTRL0 ... A_SYSCON_AHBCLKCTRL0 + 3:
			value = value << ((offset - A_SYSCON_AHBCLKCTRL0) << 3);
			offset = A_SYSCON_AHBCLKCTRL0;
			break;
		case A_SYSCON_AHBCLKCTRL1 ... A_SYSCON_AHBCLKCTRL1 + 3:
			value = value << ((offset - A_SYSCON_AHBCLKCTRL1) << 3);
			offset = A_SYSCON_AHBCLKCTRL1;
			break;
		case A_SYSCON_MAINCLKSELA ... A_SYSCON_MAINCLKSELA + 3:
			value = value << ((offset - A_SYSCON_MAINCLKSELA) << 3);
			offset = A_SYSCON_MAINCLKSELA;
			break;
		case A_SYSCON_MAINCLKSELB ... A_SYSCON_MAINCLKSELB + 3:
			value = value << ((offset - A_SYSCON_MAINCLKSELB) << 3);
			offset = A_SYSCON_MAINCLKSELB;
			break;
		case A_SYSCON_CLKOUTSELA ... A_SYSCON_CLKOUTSELA + 3:
			value = value << ((offset - A_SYSCON_CLKOUTSELA) << 3);
			offset = A_SYSCON_CLKOUTSELA;
			break;
		case A_SYSCON_SYSPLLCLKSEL ... A_SYSCON_SYSPLLCLKSEL + 3:
			value = value << ((offset - A_SYSCON_SYSPLLCLKSEL) << 3);
			offset = A_SYSCON_SYSPLLCLKSEL;
			break;
		case A_SYSCON_ADCCLKSEL ... A_SYSCON_ADCCLKSEL + 3:
			value = value << ((offset - A_SYSCON_ADCCLKSEL) << 3);
			offset = A_SYSCON_ADCCLKSEL;
			break;
		case A_SYSCON_USBCLKSEL ... A_SYSCON_USBCLKSEL + 3:
			value = value << ((offset - A_SYSCON_USBCLKSEL) << 3);
			offset = A_SYSCON_USBCLKSEL;
			break;
		case A_SYSCON_MCLKCLKSEL ... A_SYSCON_MCLKCLKSEL + 3:
			value = value << ((offset - A_SYSCON_MCLKCLKSEL) << 3);
			offset = A_SYSCON_MCLKCLKSEL;
			break;
		case A_SYSCON_FRGCLKSEL ... A_SYSCON_FRGCLKSEL + 3:
			value = value << ((offset - A_SYSCON_FRGCLKSEL) << 3);
			offset = A_SYSCON_FRGCLKSEL;
			break;
		case A_SYSCON_SYSTICKCLKDIV ... A_SYSCON_SYSTICKCLKDIV + 3:
			value = value << ((offset - A_SYSCON_SYSTICKCLKDIV) << 3);
			offset = A_SYSCON_SYSTICKCLKDIV;
			break;
		case A_SYSCON_AHBCLKDIV ... A_SYSCON_AHBCLKDIV + 3:
			value = value << ((offset - A_SYSCON_AHBCLKDIV) << 3);
			offset = A_SYSCON_AHBCLKDIV;
			break;
		case A_SYSCON_CLKOUTDIV ... A_SYSCON_CLKOUTDIV + 3:
			value = value << ((offset - A_SYSCON_CLKOUTDIV) << 3);
			offset = A_SYSCON_CLKOUTDIV;
			break;
		case A_SYSCON_ADCCLKDIV ... A_SYSCON_ADCCLKDIV + 3:
			value = value << ((offset - A_SYSCON_ADCCLKDIV) << 3);
			offset = A_SYSCON_ADCCLKDIV;
			break;
		case A_SYSCON_USBCLKDIV ... A_SYSCON_USBCLKDIV + 3:
			value = value << ((offset - A_SYSCON_USBCLKDIV) << 3);
			offset = A_SYSCON_USBCLKDIV;
			break;
		case A_SYSCON_FRGCTRL ... A_SYSCON_FRGCTRL + 3:
			value = value << ((offset - A_SYSCON_FRGCTRL) << 3);
			offset = A_SYSCON_FRGCTRL;
			break;
		case A_SYSCON_MCLKDIV ... A_SYSCON_MCLKDIV + 3:
			value = value << ((offset - A_SYSCON_MCLKDIV) << 3);
			offset = A_SYSCON_MCLKDIV;
			break;
		case A_SYSCON_FLASHCFG ... A_SYSCON_FLASHCFG + 3:
			value = value << ((offset - A_SYSCON_FLASHCFG) << 3);
			offset = A_SYSCON_FLASHCFG;
			break;
		case A_SYSCON_USBCLKCTRL ... A_SYSCON_USBCLKCTRL + 3:
			value = value << ((offset - A_SYSCON_USBCLKCTRL) << 3);
			offset = A_SYSCON_USBCLKCTRL;
			break;
		case A_SYSCON_USBCLKSTAT ... A_SYSCON_USBCLKSTAT + 3:
			value = value << ((offset - A_SYSCON_USBCLKSTAT) << 3);
			offset = A_SYSCON_USBCLKSTAT;
			break;
		case A_SYSCON_FREQMECTRL ... A_SYSCON_FREQMECTRL + 3:
			value = value << ((offset - A_SYSCON_FREQMECTRL) << 3);
			offset = A_SYSCON_FREQMECTRL;
			break;
		case A_SYSCON_MCLKIO ... A_SYSCON_MCLKIO + 3:
			value = value << ((offset - A_SYSCON_MCLKIO) << 3);
			offset = A_SYSCON_MCLKIO;
			break;
		case A_SYSCON_FROCTRL ... A_SYSCON_FROCTRL + 3:
			value = value << ((offset - A_SYSCON_FROCTRL) << 3);
			offset = A_SYSCON_FROCTRL;
			break;
		case A_SYSCON_WDTOSCCTRL ... A_SYSCON_WDTOSCCTRL + 3:
			value = value << ((offset - A_SYSCON_WDTOSCCTRL) << 3);
			offset = A_SYSCON_WDTOSCCTRL;
			break;
		case A_SYSCON_RTCOSCCTRL ... A_SYSCON_RTCOSCCTRL + 3:
			value = value << ((offset - A_SYSCON_RTCOSCCTRL) << 3);
			offset = A_SYSCON_RTCOSCCTRL;
			break;
		case A_SYSCON_SYSPLLCTRL ... A_SYSCON_SYSPLLCTRL + 3:
			value = value << ((offset - A_SYSCON_SYSPLLCTRL) << 3);
			offset = A_SYSCON_SYSPLLCTRL;
			break;
		case A_SYSCON_SYSPLLNDEC ... A_SYSCON_SYSPLLNDEC + 3:
			value = value << ((offset - A_SYSCON_SYSPLLNDEC) << 3);
			offset = A_SYSCON_SYSPLLNDEC;
			break;
		case A_SYSCON_SYSPLLPDEC ... A_SYSCON_SYSPLLPDEC + 3:
			value = value << ((offset - A_SYSCON_SYSPLLPDEC) << 3);
			offset = A_SYSCON_SYSPLLPDEC;
			break;
		case A_SYSCON_SYSPLLSSCTRL0 ... A_SYSCON_SYSPLLSSCTRL0 + 3:
			value = value << ((offset - A_SYSCON_SYSPLLSSCTRL0) << 3);
			offset = A_SYSCON_SYSPLLSSCTRL0;
			break;
		case A_SYSCON_SYSPLLSSCTRL1 ... A_SYSCON_SYSPLLSSCTRL1 + 3:
			value = value << ((offset - A_SYSCON_SYSPLLSSCTRL1) << 3);
			offset = A_SYSCON_SYSPLLSSCTRL1;
			break;
		case A_SYSCON_PDSLEEPCFG0 ... A_SYSCON_PDSLEEPCFG0 + 3:
			value = value << ((offset - A_SYSCON_PDSLEEPCFG0) << 3);
			offset = A_SYSCON_PDSLEEPCFG0;
			break;
		case A_SYSCON_PDSLEEPCFG1 ... A_SYSCON_PDSLEEPCFG1 + 3:
			value = value << ((offset - A_SYSCON_PDSLEEPCFG1) << 3);
			offset = A_SYSCON_PDSLEEPCFG1;
			break;
		case A_SYSCON_PDRUNCFG0 ... A_SYSCON_PDRUNCFG0 + 3:
			value = value << ((offset - A_SYSCON_PDRUNCFG0) << 3);
			offset = A_SYSCON_PDRUNCFG0;
			break;
		case A_SYSCON_PDRUNCFGSET0 ... A_SYSCON_PDRUNCFGSET0 + 3:
			value = value << ((offset - A_SYSCON_PDRUNCFGSET0) << 3);
			offset = A_SYSCON_PDRUNCFGSET0;
			break;
		case A_SYSCON_PDRUNCFGCLR0 ... A_SYSCON_PDRUNCFGCLR0 + 3:
			value = value << ((offset - A_SYSCON_PDRUNCFGCLR0) << 3);
			offset = A_SYSCON_PDRUNCFGCLR0;
			break;
		case A_SYSCON_STARTER0 ... A_SYSCON_STARTER0 + 3:
			value = value << ((offset - A_SYSCON_STARTER0) << 3);
			offset = A_SYSCON_STARTER0;
			break;
		case A_SYSCON_STARTERSET0 ... A_SYSCON_STARTERSET0 + 3:
			value = value << ((offset - A_SYSCON_STARTERSET0) << 3);
			offset = A_SYSCON_STARTERSET0;
			break;
		case A_SYSCON_STARTERCLR0 ... A_SYSCON_STARTERCLR0 + 3:
			value = value << ((offset - A_SYSCON_STARTERCLR0) << 3);
			offset = A_SYSCON_STARTERCLR0;
			break;
		case A_SYSCON_HWWAKE ... A_SYSCON_HWWAKE + 3:
			value = value << ((offset - A_SYSCON_HWWAKE) << 3);
			offset = A_SYSCON_HWWAKE;
			break;
		case A_SYSCON_BODCTRL ... A_SYSCON_BODCTRL + 3:
			value = value << ((offset - A_SYSCON_BODCTRL) << 3);
			offset = A_SYSCON_BODCTRL;
			break;
		case A_SYSCON_PRESETCTRLSET0 ... A_SYSCON_PRESETCTRLSET0 + 3:
			value = value << ((offset - A_SYSCON_PRESETCTRLSET0) << 3);
			offset = A_SYSCON_PRESETCTRLSET0;
			break;
		case A_SYSCON_PRESETCTRLSET1 ... A_SYSCON_PRESETCTRLSET1 + 3:
			value = value << ((offset - A_SYSCON_PRESETCTRLSET1) << 3);
			offset = A_SYSCON_PRESETCTRLSET1;
			break;
		case A_SYSCON_PRESETCTRLCLR0 ... A_SYSCON_PRESETCTRLCLR0 + 3:
			value = value << ((offset - A_SYSCON_PRESETCTRLCLR0) << 3);
			offset = A_SYSCON_PRESETCTRLCLR0;
			break;
		case A_SYSCON_PRESETCTRLCLR1 ... A_SYSCON_PRESETCTRLCLR1 + 3:
			value = value << ((offset - A_SYSCON_PRESETCTRLCLR1) << 3);
			offset = A_SYSCON_PRESETCTRLCLR1;
			break;
		case A_SYSCON_AHBCLKCTRLSET0 ... A_SYSCON_AHBCLKCTRLSET0 + 3:
			value = value << ((offset - A_SYSCON_AHBCLKCTRLSET0) << 3);
			offset = A_SYSCON_AHBCLKCTRLSET0;
			break;
		case A_SYSCON_AHBCLKCTRLSET1 ... A_SYSCON_AHBCLKCTRLSET1 + 3:
			value = value << ((offset - A_SYSCON_AHBCLKCTRLSET1) << 3);
			offset = A_SYSCON_AHBCLKCTRLSET1;
			break;
		case A_SYSCON_AHBCLKCTRLCLR0 ... A_SYSCON_AHBCLKCTRLCLR0 + 3:
			value = value << ((offset - A_SYSCON_AHBCLKCTRLCLR0) << 3);
			offset = A_SYSCON_AHBCLKCTRLCLR0;
			break;
		case A_SYSCON_AHBCLKCTRLCLR1 ... A_SYSCON_AHBCLKCTRLCLR1 + 3:
			value = value << ((offset - A_SYSCON_AHBCLKCTRLCLR1) << 3);
			offset = A_SYSCON_AHBCLKCTRLCLR1;
			break;
		case A_SYSCON_FXCOMCLKSEL0 ... A_SYSCON_FXCOMCLKSEL0 + 3:
			value = value << ((offset - A_SYSCON_FXCOMCLKSEL0) << 3);
			offset = A_SYSCON_FXCOMCLKSEL0;
			break;
		case A_SYSCON_FXCOMCLKSEL1 ... A_SYSCON_FXCOMCLKSEL1 + 3:
			value = value << ((offset - A_SYSCON_FXCOMCLKSEL1) << 3);
			offset = A_SYSCON_FXCOMCLKSEL1;
			break;
		case A_SYSCON_FXCOMCLKSEL2 ... A_SYSCON_FXCOMCLKSEL2 + 3:
			value = value << ((offset - A_SYSCON_FXCOMCLKSEL2) << 3);
			offset = A_SYSCON_FXCOMCLKSEL2;
			break;
		case A_SYSCON_FXCOMCLKSEL3 ... A_SYSCON_FXCOMCLKSEL3 + 3:
			value = value << ((offset - A_SYSCON_FXCOMCLKSEL3) << 3);
			offset = A_SYSCON_FXCOMCLKSEL3;
			break;
		case A_SYSCON_FXCOMCLKSEL4 ... A_SYSCON_FXCOMCLKSEL4 + 3:
			value = value << ((offset - A_SYSCON_FXCOMCLKSEL4) << 3);
			offset = A_SYSCON_FXCOMCLKSEL4;
			break;
		case A_SYSCON_FXCOMCLKSEL5 ... A_SYSCON_FXCOMCLKSEL5 + 3:
			value = value << ((offset - A_SYSCON_FXCOMCLKSEL5) << 3);
			offset = A_SYSCON_FXCOMCLKSEL5;
			break;
		case A_SYSCON_FXCOMCLKSEL6 ... A_SYSCON_FXCOMCLKSEL6 + 3:
			value = value << ((offset - A_SYSCON_FXCOMCLKSEL6) << 3);
			offset = A_SYSCON_FXCOMCLKSEL6;
			break;
		case A_SYSCON_FXCOMCLKSEL7 ... A_SYSCON_FXCOMCLKSEL7 + 3:
			value = value << ((offset - A_SYSCON_FXCOMCLKSEL7) << 3);
			offset = A_SYSCON_FXCOMCLKSEL7;
			break;

		default: break;
	}

	switch (offset) {
		case A_SYSCON_AHBMATPRIO:
			t->AHBMATPRIO = value;
			break;
		case A_SYSCON_SYSTCKCAL:
			t->SYSTCKCAL = value;
			break;
		case A_SYSCON_NMISRC:
			t->NMISRC = value;
			break;
		case A_SYSCON_ASYNCAPBCTRL:
			t->ASYNCAPBCTRL = value;
			break;
		case A_SYSCON_PRESETCTRL0:
			t->PRESETCTRL0 = value;
			break;
		case A_SYSCON_PRESETCTRL1:
			t->PRESETCTRL1 = value;
			break;
		case A_SYSCON_SYSRSTSTAT:
			t->SYSRSTSTAT = value;
			break;
		case A_SYSCON_AHBCLKCTRL0:
			t->AHBCLKCTRL0 = value;
			break;
		case A_SYSCON_AHBCLKCTRL1:
			t->AHBCLKCTRL1 = value;
			break;
		case A_SYSCON_MAINCLKSELA:
			t->MAINCLKSELA = value;
			break;
		case A_SYSCON_MAINCLKSELB:
			t->MAINCLKSELB = value;
			break;
		case A_SYSCON_CLKOUTSELA:
			t->CLKOUTSELA = value;
			break;
		case A_SYSCON_SYSPLLCLKSEL:
			t->SYSPLLCLKSEL = value;
			break;
		case A_SYSCON_ADCCLKSEL:
			t->ADCCLKSEL = value;
			break;
		case A_SYSCON_USBCLKSEL:
			t->USBCLKSEL = value;
			break;
		case A_SYSCON_MCLKCLKSEL:
			t->MCLKCLKSEL = value;
			break;
		case A_SYSCON_FRGCLKSEL:
			t->FRGCLKSEL = value;
			break;
		case A_SYSCON_SYSTICKCLKDIV:
			t->SYSTICKCLKDIV = value;
			break;
		case A_SYSCON_AHBCLKDIV:
			t->AHBCLKDIV = value;
			break;
		case A_SYSCON_CLKOUTDIV:
			t->CLKOUTDIV = value;
			break;
		case A_SYSCON_ADCCLKDIV:
			t->ADCCLKDIV = value;
			break;
		case A_SYSCON_USBCLKDIV:
			t->USBCLKDIV = value;
			break;
		case A_SYSCON_FRGCTRL:
			t->FRGCTRL = value;
			break;
		case A_SYSCON_MCLKDIV:
			t->MCLKDIV = value;
			break;
		case A_SYSCON_FLASHCFG:
			t->FLASHCFG = value;
			break;
		case A_SYSCON_USBCLKCTRL:
			t->USBCLKCTRL = value;
			break;
		case A_SYSCON_USBCLKSTAT:
			t->USBCLKSTAT = value;
			break;
		case A_SYSCON_FREQMECTRL:
			t->FREQMECTRL = value;
			break;
		case A_SYSCON_MCLKIO:
			t->MCLKIO = value;
			break;
		case A_SYSCON_FROCTRL:
			t->FROCTRL = value;
			break;
		case A_SYSCON_WDTOSCCTRL:
			t->WDTOSCCTRL = value;
			break;
		case A_SYSCON_RTCOSCCTRL:
			t->RTCOSCCTRL = value;
			break;
		case A_SYSCON_SYSPLLCTRL:
			t->SYSPLLCTRL = value;
			break;
		case A_SYSCON_SYSPLLNDEC:
			t->SYSPLLNDEC = value;
			break;
		case A_SYSCON_SYSPLLPDEC:
			t->SYSPLLPDEC = value;
			break;
		case A_SYSCON_SYSPLLSSCTRL0:
			t->SYSPLLSSCTRL0 = value;
			break;
		case A_SYSCON_SYSPLLSSCTRL1:
			t->SYSPLLSSCTRL1 = value;
			break;
		case A_SYSCON_PDSLEEPCFG0:
			t->PDSLEEPCFG0 = value;
			break;
		case A_SYSCON_PDSLEEPCFG1:
			t->PDSLEEPCFG1 = value;
			break;
		case A_SYSCON_PDRUNCFG0:
			t->PDRUNCFG0 = value;
			break;
		case A_SYSCON_PDRUNCFGSET0:
			t->PDRUNCFGSET0 = value;
			break;
		case A_SYSCON_PDRUNCFGCLR0:
			t->PDRUNCFGCLR0 = value;
			break;
		case A_SYSCON_STARTER0:
			t->STARTER0 = value;
			break;
		case A_SYSCON_STARTERSET0:
			t->STARTERSET0 = value;
			break;
		case A_SYSCON_STARTERCLR0:
			t->STARTERCLR0 = value;
			break;
		case A_SYSCON_HWWAKE:
			t->HWWAKE = value;
			break;
		case A_SYSCON_BODCTRL:
			t->BODCTRL = value;
			break;
		case A_SYSCON_PRESETCTRLSET0:
			t->PRESETCTRLSET0 = value;
			break;
		case A_SYSCON_PRESETCTRLSET1:
			t->PRESETCTRLSET1 = value;
			break;
		case A_SYSCON_PRESETCTRLCLR0:
			t->PRESETCTRLCLR0 = value;
			break;
		case A_SYSCON_PRESETCTRLCLR1:
			t->PRESETCTRLCLR1 = value;
			break;
		case A_SYSCON_AHBCLKCTRLSET0:
			t->AHBCLKCTRLSET0 = value;
			break;
		case A_SYSCON_AHBCLKCTRLSET1:
			t->AHBCLKCTRLSET1 = value;
			break;
		case A_SYSCON_AHBCLKCTRLCLR0:
			t->AHBCLKCTRLCLR0 = value;
			break;
		case A_SYSCON_AHBCLKCTRLCLR1:
			t->AHBCLKCTRLCLR1 = value;
			break;
		case A_SYSCON_FXCOMCLKSEL0:
			t->FXCOMCLKSEL0 = value;
			break;
		case A_SYSCON_FXCOMCLKSEL1:
			t->FXCOMCLKSEL1 = value;
			break;
		case A_SYSCON_FXCOMCLKSEL2:
			t->FXCOMCLKSEL2 = value;
			break;
		case A_SYSCON_FXCOMCLKSEL3:
			t->FXCOMCLKSEL3 = value;
			break;
		case A_SYSCON_FXCOMCLKSEL4:
			t->FXCOMCLKSEL4 = value;
			break;
		case A_SYSCON_FXCOMCLKSEL5:
			t->FXCOMCLKSEL5 = value;
			break;
		case A_SYSCON_FXCOMCLKSEL6:
			t->FXCOMCLKSEL6 = value;
			break;
		case A_SYSCON_FXCOMCLKSEL7:
			t->FXCOMCLKSEL7 = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "LPC51U68 SYSCON write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps lpc51u68_syscon_ops = {
	.read = lpc51u68_syscon_read,
	.write = lpc51u68_syscon_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void lpc51u68_syscon_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	LPC51U68SYSCON *t = LPC51U68_SYSCON(obj);
	memory_region_init_io(&(t->iomem), obj, &lpc51u68_syscon_ops, t, "lpc51u68-syscon", LPC51U68_SYSCON_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void lpc51u68_syscon_realize(DeviceState *dev, Error **errp) {
	return;

}

static void lpc51u68_syscon_reset_enter(Object *obj, ResetType type) {
	LPC51U68SYSCON *t = LPC51U68_SYSCON(obj);
	lpc51u68_syscon_register_reset(t);
}

static const VMStateDescription lpc51u68_syscon_vmstate = {
	.name = "lpc51u68-syscon",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(AHBMATPRIO, LPC51U68SYSCON),
		VMSTATE_UINT32(SYSTCKCAL, LPC51U68SYSCON),
		VMSTATE_UINT32(NMISRC, LPC51U68SYSCON),
		VMSTATE_UINT32(ASYNCAPBCTRL, LPC51U68SYSCON),
		VMSTATE_UINT32(PRESETCTRL0, LPC51U68SYSCON),
		VMSTATE_UINT32(PRESETCTRL1, LPC51U68SYSCON),
		VMSTATE_UINT32(SYSRSTSTAT, LPC51U68SYSCON),
		VMSTATE_UINT32(AHBCLKCTRL0, LPC51U68SYSCON),
		VMSTATE_UINT32(AHBCLKCTRL1, LPC51U68SYSCON),
		VMSTATE_UINT32(MAINCLKSELA, LPC51U68SYSCON),
		VMSTATE_UINT32(MAINCLKSELB, LPC51U68SYSCON),
		VMSTATE_UINT32(CLKOUTSELA, LPC51U68SYSCON),
		VMSTATE_UINT32(SYSPLLCLKSEL, LPC51U68SYSCON),
		VMSTATE_UINT32(ADCCLKSEL, LPC51U68SYSCON),
		VMSTATE_UINT32(USBCLKSEL, LPC51U68SYSCON),
		VMSTATE_UINT32(MCLKCLKSEL, LPC51U68SYSCON),
		VMSTATE_UINT32(FRGCLKSEL, LPC51U68SYSCON),
		VMSTATE_UINT32(SYSTICKCLKDIV, LPC51U68SYSCON),
		VMSTATE_UINT32(AHBCLKDIV, LPC51U68SYSCON),
		VMSTATE_UINT32(CLKOUTDIV, LPC51U68SYSCON),
		VMSTATE_UINT32(ADCCLKDIV, LPC51U68SYSCON),
		VMSTATE_UINT32(USBCLKDIV, LPC51U68SYSCON),
		VMSTATE_UINT32(FRGCTRL, LPC51U68SYSCON),
		VMSTATE_UINT32(MCLKDIV, LPC51U68SYSCON),
		VMSTATE_UINT32(FLASHCFG, LPC51U68SYSCON),
		VMSTATE_UINT32(USBCLKCTRL, LPC51U68SYSCON),
		VMSTATE_UINT32(USBCLKSTAT, LPC51U68SYSCON),
		VMSTATE_UINT32(FREQMECTRL, LPC51U68SYSCON),
		VMSTATE_UINT32(MCLKIO, LPC51U68SYSCON),
		VMSTATE_UINT32(FROCTRL, LPC51U68SYSCON),
		VMSTATE_UINT32(WDTOSCCTRL, LPC51U68SYSCON),
		VMSTATE_UINT32(RTCOSCCTRL, LPC51U68SYSCON),
		VMSTATE_UINT32(SYSPLLCTRL, LPC51U68SYSCON),
		VMSTATE_UINT32(SYSPLLSTAT, LPC51U68SYSCON),
		VMSTATE_UINT32(SYSPLLNDEC, LPC51U68SYSCON),
		VMSTATE_UINT32(SYSPLLPDEC, LPC51U68SYSCON),
		VMSTATE_UINT32(SYSPLLSSCTRL0, LPC51U68SYSCON),
		VMSTATE_UINT32(SYSPLLSSCTRL1, LPC51U68SYSCON),
		VMSTATE_UINT32(PDSLEEPCFG0, LPC51U68SYSCON),
		VMSTATE_UINT32(PDSLEEPCFG1, LPC51U68SYSCON),
		VMSTATE_UINT32(PDRUNCFG0, LPC51U68SYSCON),
		VMSTATE_UINT32(PDRUNCFGSET0, LPC51U68SYSCON),
		VMSTATE_UINT32(PDRUNCFGCLR0, LPC51U68SYSCON),
		VMSTATE_UINT32(STARTER0, LPC51U68SYSCON),
		VMSTATE_UINT32(STARTERSET0, LPC51U68SYSCON),
		VMSTATE_UINT32(STARTERCLR0, LPC51U68SYSCON),
		VMSTATE_UINT32(HWWAKE, LPC51U68SYSCON),
		VMSTATE_UINT32(JTAGIDCODE, LPC51U68SYSCON),
		VMSTATE_UINT32(DEVICE_ID0, LPC51U68SYSCON),
		VMSTATE_UINT32(DEVICE_ID1, LPC51U68SYSCON),
		VMSTATE_UINT32(BODCTRL, LPC51U68SYSCON),
		VMSTATE_UINT32(PIOPORCAP0, LPC51U68SYSCON),
		VMSTATE_UINT32(PIOPORCAP1, LPC51U68SYSCON),
		VMSTATE_UINT32(PIORESCAP0, LPC51U68SYSCON),
		VMSTATE_UINT32(PIORESCAP1, LPC51U68SYSCON),
		VMSTATE_UINT32(PRESETCTRLSET0, LPC51U68SYSCON),
		VMSTATE_UINT32(PRESETCTRLSET1, LPC51U68SYSCON),
		VMSTATE_UINT32(PRESETCTRLCLR0, LPC51U68SYSCON),
		VMSTATE_UINT32(PRESETCTRLCLR1, LPC51U68SYSCON),
		VMSTATE_UINT32(AHBCLKCTRLSET0, LPC51U68SYSCON),
		VMSTATE_UINT32(AHBCLKCTRLSET1, LPC51U68SYSCON),
		VMSTATE_UINT32(AHBCLKCTRLCLR0, LPC51U68SYSCON),
		VMSTATE_UINT32(AHBCLKCTRLCLR1, LPC51U68SYSCON),
		VMSTATE_UINT32(FXCOMCLKSEL0, LPC51U68SYSCON),
		VMSTATE_UINT32(FXCOMCLKSEL1, LPC51U68SYSCON),
		VMSTATE_UINT32(FXCOMCLKSEL2, LPC51U68SYSCON),
		VMSTATE_UINT32(FXCOMCLKSEL3, LPC51U68SYSCON),
		VMSTATE_UINT32(FXCOMCLKSEL4, LPC51U68SYSCON),
		VMSTATE_UINT32(FXCOMCLKSEL5, LPC51U68SYSCON),
		VMSTATE_UINT32(FXCOMCLKSEL6, LPC51U68SYSCON),
		VMSTATE_UINT32(FXCOMCLKSEL7, LPC51U68SYSCON),

		VMSTATE_END_OF_LIST()
	}
};

static void lpc51u68_syscon_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &lpc51u68_syscon_vmstate;
	dc->realize = lpc51u68_syscon_realize;
	rc->phases.enter = lpc51u68_syscon_reset_enter;

}

static const TypeInfo lpc51u68_syscon_info = {
	.name = TYPE_LPC51U68_SYSCON,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(LPC51U68SYSCON),
	.instance_init = lpc51u68_syscon_init,
	.class_init = lpc51u68_syscon_class_init,
};

static void lpc51u68_syscon_register_types(void) {
	type_register_static(&lpc51u68_syscon_info);
}

type_init(lpc51u68_syscon_register_types);

#define TYPE_LPC51U68_ASYNC_SYSCON "lpc51u68-async_syscon"
OBJECT_DECLARE_SIMPLE_TYPE(LPC51U68ASYNC_SYSCON, LPC51U68_ASYNC_SYSCON)

struct LPC51U68ASYNC_SYSCON {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/*registers*/
	uint32_t ASYNCPRESETCTRL;
	uint32_t ASYNCPRESETCTRLSET;
	uint32_t ASYNCPRESETCTRLCLR;
	uint32_t ASYNCAPBCLKCTRL;
	uint32_t ASYNCAPBCLKCTRLSET;
	uint32_t ASYNCAPBCLKCTRLCLR;
	uint32_t ASYNCAPBCLKSELA;
	/* base */
	uint32_t base;

};
#define LPC51U68_ASYNC_SYSCON_SIZE				0x24

REG32(ASYNC_SYSCON_ASYNCPRESETCTRL, 0x0)
	FIELD(ASYNC_SYSCON_ASYNCPRESETCTRL, CTIMER3, 13, 1)
REG32(ASYNC_SYSCON_ASYNCPRESETCTRLSET, 0x4)
	FIELD(ASYNC_SYSCON_ASYNCPRESETCTRLSET, ARST_SET, 0, 32)
REG32(ASYNC_SYSCON_ASYNCPRESETCTRLCLR, 0x8)
	FIELD(ASYNC_SYSCON_ASYNCPRESETCTRLCLR, ARST_CLR, 0, 32)
REG32(ASYNC_SYSCON_ASYNCAPBCLKCTRL, 0x10)
	FIELD(ASYNC_SYSCON_ASYNCAPBCLKCTRL, CTIMER3, 13, 1)
REG32(ASYNC_SYSCON_ASYNCAPBCLKCTRLSET, 0x14)
	FIELD(ASYNC_SYSCON_ASYNCAPBCLKCTRLSET, ACLK_SET, 0, 32)
REG32(ASYNC_SYSCON_ASYNCAPBCLKCTRLCLR, 0x18)
	FIELD(ASYNC_SYSCON_ASYNCAPBCLKCTRLCLR, ACLK_CLR, 0, 32)
REG32(ASYNC_SYSCON_ASYNCAPBCLKSELA, 0x20)
	FIELD(ASYNC_SYSCON_ASYNCAPBCLKSELA, SEL, 0, 2)


static void lpc51u68_async_syscon_register_reset(LPC51U68ASYNC_SYSCON *t) {
	t->ASYNCPRESETCTRL = 0x0;
	t->ASYNCPRESETCTRLSET = 0x0;
	t->ASYNCPRESETCTRLCLR = 0x0;
	t->ASYNCAPBCLKCTRL = 0x0;
	t->ASYNCAPBCLKCTRLSET = 0x0;
	t->ASYNCAPBCLKCTRLCLR = 0x0;
	t->ASYNCAPBCLKSELA = 0x0;

}

static uint64_t lpc51u68_async_syscon_read(void *opaque, hwaddr offset, unsigned size) {
	LPC51U68ASYNC_SYSCON *t = LPC51U68_ASYNC_SYSCON(opaque);
	uint64_t ret;

	switch (offset) {
		case A_ASYNC_SYSCON_ASYNCPRESETCTRL:
			ret = t->ASYNCPRESETCTRL;
			break;
		case A_ASYNC_SYSCON_ASYNCAPBCLKCTRL:
			ret = t->ASYNCAPBCLKCTRL;
			break;
		case A_ASYNC_SYSCON_ASYNCAPBCLKSELA:
			ret = t->ASYNCAPBCLKSELA;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "LPC51U68 ASYNC_SYSCON read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void lpc51u68_async_syscon_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	LPC51U68ASYNC_SYSCON *t = LPC51U68_ASYNC_SYSCON(opaque);


	switch (offset) {
		case A_ASYNC_SYSCON_ASYNCPRESETCTRL ... A_ASYNC_SYSCON_ASYNCPRESETCTRL + 3:
			value = value << ((offset - A_ASYNC_SYSCON_ASYNCPRESETCTRL) << 3);
			offset = A_ASYNC_SYSCON_ASYNCPRESETCTRL;
			break;
		case A_ASYNC_SYSCON_ASYNCPRESETCTRLSET ... A_ASYNC_SYSCON_ASYNCPRESETCTRLSET + 3:
			value = value << ((offset - A_ASYNC_SYSCON_ASYNCPRESETCTRLSET) << 3);
			offset = A_ASYNC_SYSCON_ASYNCPRESETCTRLSET;
			break;
		case A_ASYNC_SYSCON_ASYNCPRESETCTRLCLR ... A_ASYNC_SYSCON_ASYNCPRESETCTRLCLR + 3:
			value = value << ((offset - A_ASYNC_SYSCON_ASYNCPRESETCTRLCLR) << 3);
			offset = A_ASYNC_SYSCON_ASYNCPRESETCTRLCLR;
			break;
		case A_ASYNC_SYSCON_ASYNCAPBCLKCTRL ... A_ASYNC_SYSCON_ASYNCAPBCLKCTRL + 3:
			value = value << ((offset - A_ASYNC_SYSCON_ASYNCAPBCLKCTRL) << 3);
			offset = A_ASYNC_SYSCON_ASYNCAPBCLKCTRL;
			break;
		case A_ASYNC_SYSCON_ASYNCAPBCLKCTRLSET ... A_ASYNC_SYSCON_ASYNCAPBCLKCTRLSET + 3:
			value = value << ((offset - A_ASYNC_SYSCON_ASYNCAPBCLKCTRLSET) << 3);
			offset = A_ASYNC_SYSCON_ASYNCAPBCLKCTRLSET;
			break;
		case A_ASYNC_SYSCON_ASYNCAPBCLKCTRLCLR ... A_ASYNC_SYSCON_ASYNCAPBCLKCTRLCLR + 3:
			value = value << ((offset - A_ASYNC_SYSCON_ASYNCAPBCLKCTRLCLR) << 3);
			offset = A_ASYNC_SYSCON_ASYNCAPBCLKCTRLCLR;
			break;
		case A_ASYNC_SYSCON_ASYNCAPBCLKSELA ... A_ASYNC_SYSCON_ASYNCAPBCLKSELA + 3:
			value = value << ((offset - A_ASYNC_SYSCON_ASYNCAPBCLKSELA) << 3);
			offset = A_ASYNC_SYSCON_ASYNCAPBCLKSELA;
			break;

		default: break;
	}

	switch (offset) {
		case A_ASYNC_SYSCON_ASYNCPRESETCTRL:
			t->ASYNCPRESETCTRL = value;
			break;
		case A_ASYNC_SYSCON_ASYNCPRESETCTRLSET:
			t->ASYNCPRESETCTRLSET = value;
			if (((!(value & 0x1)) && (!(value & 0x2)) && (!(value & 0x4)) && (!(value & 0x8)) && (!(value & 0x10)) && (!(value & 0x20)) && (!(value & 0x40)) && (!(value & 0x80)) && (!(value & 0x100)) && (!(value & 0x200)) && (!(value & 0x400)) && (!(value & 0x800)) && (!(value & 0x1000)) && (value & 0x2000) && (!(value & 0x4000)) && (!(value & 0x8000)) && (!(value & 0x10000)) && (!(value & 0x20000)) && (!(value & 0x40000)) && (!(value & 0x80000)) && (!(value & 0x100000)) && (!(value & 0x200000)) && (!(value & 0x400000)) && (!(value & 0x800000)) && (!(value & 0x1000000)) && (!(value & 0x2000000)) && (!(value & 0x4000000)) && (!(value & 0x8000000)) && (!(value & 0x10000000)) && (!(value & 0x20000000)) && (!(value & 0x40000000)) && (!(value & 0x80000000)))) {
				t->ASYNCPRESETCTRL |= 0x2000;
			}
			break;
		case A_ASYNC_SYSCON_ASYNCPRESETCTRLCLR:
			t->ASYNCPRESETCTRLCLR = value;
			if (((!(value & 0x1)) && (!(value & 0x2)) && (!(value & 0x4)) && (!(value & 0x8)) && (!(value & 0x10)) && (!(value & 0x20)) && (!(value & 0x40)) && (!(value & 0x80)) && (!(value & 0x100)) && (!(value & 0x200)) && (!(value & 0x400)) && (!(value & 0x800)) && (!(value & 0x1000)) && (value & 0x2000) && (!(value & 0x4000)) && (!(value & 0x8000)) && (!(value & 0x10000)) && (!(value & 0x20000)) && (!(value & 0x40000)) && (!(value & 0x80000)) && (!(value & 0x100000)) && (!(value & 0x200000)) && (!(value & 0x400000)) && (!(value & 0x800000)) && (!(value & 0x1000000)) && (!(value & 0x2000000)) && (!(value & 0x4000000)) && (!(value & 0x8000000)) && (!(value & 0x10000000)) && (!(value & 0x20000000)) && (!(value & 0x40000000)) && (!(value & 0x80000000)))) {
				t->ASYNCPRESETCTRL &= (~(0x2000));
			}
			break;
		case A_ASYNC_SYSCON_ASYNCAPBCLKCTRL:
			t->ASYNCAPBCLKCTRL = value;
			break;
		case A_ASYNC_SYSCON_ASYNCAPBCLKCTRLSET:
			t->ASYNCAPBCLKCTRLSET = value;
			break;
		case A_ASYNC_SYSCON_ASYNCAPBCLKCTRLCLR:
			t->ASYNCAPBCLKCTRLCLR = value;
			break;
		case A_ASYNC_SYSCON_ASYNCAPBCLKSELA:
			t->ASYNCAPBCLKSELA = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "LPC51U68 ASYNC_SYSCON write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps lpc51u68_async_syscon_ops = {
	.read = lpc51u68_async_syscon_read,
	.write = lpc51u68_async_syscon_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void lpc51u68_async_syscon_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	LPC51U68ASYNC_SYSCON *t = LPC51U68_ASYNC_SYSCON(obj);
	memory_region_init_io(&(t->iomem), obj, &lpc51u68_async_syscon_ops, t, "lpc51u68-async_syscon", LPC51U68_ASYNC_SYSCON_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void lpc51u68_async_syscon_realize(DeviceState *dev, Error **errp) {
	return;

}

static void lpc51u68_async_syscon_reset_enter(Object *obj, ResetType type) {
	LPC51U68ASYNC_SYSCON *t = LPC51U68_ASYNC_SYSCON(obj);
	lpc51u68_async_syscon_register_reset(t);
}

static const VMStateDescription lpc51u68_async_syscon_vmstate = {
	.name = "lpc51u68-async_syscon",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(ASYNCPRESETCTRL, LPC51U68ASYNC_SYSCON),
		VMSTATE_UINT32(ASYNCPRESETCTRLSET, LPC51U68ASYNC_SYSCON),
		VMSTATE_UINT32(ASYNCPRESETCTRLCLR, LPC51U68ASYNC_SYSCON),
		VMSTATE_UINT32(ASYNCAPBCLKCTRL, LPC51U68ASYNC_SYSCON),
		VMSTATE_UINT32(ASYNCAPBCLKCTRLSET, LPC51U68ASYNC_SYSCON),
		VMSTATE_UINT32(ASYNCAPBCLKCTRLCLR, LPC51U68ASYNC_SYSCON),
		VMSTATE_UINT32(ASYNCAPBCLKSELA, LPC51U68ASYNC_SYSCON),

		VMSTATE_END_OF_LIST()
	}
};

static void lpc51u68_async_syscon_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &lpc51u68_async_syscon_vmstate;
	dc->realize = lpc51u68_async_syscon_realize;
	rc->phases.enter = lpc51u68_async_syscon_reset_enter;

}

static const TypeInfo lpc51u68_async_syscon_info = {
	.name = TYPE_LPC51U68_ASYNC_SYSCON,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(LPC51U68ASYNC_SYSCON),
	.instance_init = lpc51u68_async_syscon_init,
	.class_init = lpc51u68_async_syscon_class_init,
};

static void lpc51u68_async_syscon_register_types(void) {
	type_register_static(&lpc51u68_async_syscon_info);
}

type_init(lpc51u68_async_syscon_register_types);

#define TYPE_LPC51U68_USART "lpc51u68-usart"
OBJECT_DECLARE_SIMPLE_TYPE(LPC51U68USART, LPC51U68_USART)

struct LPC51U68USART {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t CFG;
	uint32_t CTL;
	uint32_t STAT;
	uint32_t INTENSET;
	uint32_t INTENCLR;
	uint32_t BRG;
	uint32_t INTSTAT;
	uint32_t OSR;
	uint32_t ADDR;
	uint32_t FIFOCFG;
	uint32_t FIFOSTAT;
	uint32_t FIFOTRIG;
	uint32_t FIFOINTENSET;
	uint32_t FIFOINTENCLR;
	uint32_t FIFOINTSTAT;
	uint32_t FIFOWR;
	uint32_t FIFORD;
	uint32_t FIFORDNOPOP;
	uint32_t ID;
	/* chardev backend */
	CharBackend chr;
	guint watch_tag;

	/* base */
	uint32_t base;

};
#define LPC51U68_USART_SIZE				0x1000

REG32(USART_CFG, 0x0)
	FIELD(USART_CFG, ENABLE, 0, 1)
	FIELD(USART_CFG, DATALEN, 2, 2)
	FIELD(USART_CFG, PARITYSEL, 4, 2)
	FIELD(USART_CFG, STOPLEN, 6, 1)
	FIELD(USART_CFG, MODE32K, 7, 1)
	FIELD(USART_CFG, LINMODE, 8, 1)
	FIELD(USART_CFG, CTSEN, 9, 1)
	FIELD(USART_CFG, SYNCEN, 11, 1)
	FIELD(USART_CFG, CLKPOL, 12, 1)
	FIELD(USART_CFG, SYNCMST, 14, 1)
	FIELD(USART_CFG, LOOP, 15, 1)
	FIELD(USART_CFG, OETA, 18, 1)
	FIELD(USART_CFG, AUTOADDR, 19, 1)
	FIELD(USART_CFG, OESEL, 20, 1)
	FIELD(USART_CFG, OEPOL, 21, 1)
	FIELD(USART_CFG, RXPOL, 22, 1)
	FIELD(USART_CFG, TXPOL, 23, 1)
REG32(USART_CTL, 0x4)
	FIELD(USART_CTL, TXBRKEN, 1, 1)
	FIELD(USART_CTL, ADDRDET, 2, 1)
	FIELD(USART_CTL, TXDIS, 6, 1)
	FIELD(USART_CTL, CC, 8, 1)
	FIELD(USART_CTL, CLRCCONRX, 9, 1)
	FIELD(USART_CTL, AUTOBAUD, 16, 1)
REG32(USART_STAT, 0x8)
	FIELD(USART_STAT, RXIDLE, 1, 1)
	FIELD(USART_STAT, TXIDLE, 3, 1)
	FIELD(USART_STAT, CTS, 4, 1)
	FIELD(USART_STAT, DELTACTS, 5, 1)
	FIELD(USART_STAT, TXDISSTAT, 6, 1)
	FIELD(USART_STAT, RXBRK, 10, 1)
	FIELD(USART_STAT, DELTARXBRK, 11, 1)
	FIELD(USART_STAT, START, 12, 1)
	FIELD(USART_STAT, FRAMERRINT, 13, 1)
	FIELD(USART_STAT, PARITYERRINT, 14, 1)
	FIELD(USART_STAT, RXNOISEINT, 15, 1)
	FIELD(USART_STAT, ABERR, 16, 1)
REG32(USART_INTENSET, 0xc)
	FIELD(USART_INTENSET, TXIDLEEN, 3, 1)
	FIELD(USART_INTENSET, DELTACTSEN, 5, 1)
	FIELD(USART_INTENSET, TXDISEN, 6, 1)
	FIELD(USART_INTENSET, DELTARXBRKEN, 11, 1)
	FIELD(USART_INTENSET, STARTEN, 12, 1)
	FIELD(USART_INTENSET, FRAMERREN, 13, 1)
	FIELD(USART_INTENSET, PARITYERREN, 14, 1)
	FIELD(USART_INTENSET, RXNOISEEN, 15, 1)
	FIELD(USART_INTENSET, ABERREN, 16, 1)
REG32(USART_INTENCLR, 0x10)
	FIELD(USART_INTENCLR, TXIDLECLR, 3, 1)
	FIELD(USART_INTENCLR, DELTACTSCLR, 5, 1)
	FIELD(USART_INTENCLR, TXDISCLR, 6, 1)
	FIELD(USART_INTENCLR, DELTARXBRKCLR, 11, 1)
	FIELD(USART_INTENCLR, STARTCLR, 12, 1)
	FIELD(USART_INTENCLR, FRAMERRCLR, 13, 1)
	FIELD(USART_INTENCLR, PARITYERRCLR, 14, 1)
	FIELD(USART_INTENCLR, RXNOISECLR, 15, 1)
	FIELD(USART_INTENCLR, ABERRCLR, 16, 1)
REG32(USART_BRG, 0x20)
	FIELD(USART_BRG, BRGVAL, 0, 16)
REG32(USART_INTSTAT, 0x24)
	FIELD(USART_INTSTAT, TXIDLE, 3, 1)
	FIELD(USART_INTSTAT, DELTACTS, 5, 1)
	FIELD(USART_INTSTAT, TXDISINT, 6, 1)
	FIELD(USART_INTSTAT, DELTARXBRK, 11, 1)
	FIELD(USART_INTSTAT, START, 12, 1)
	FIELD(USART_INTSTAT, FRAMERRINT, 13, 1)
	FIELD(USART_INTSTAT, PARITYERRINT, 14, 1)
	FIELD(USART_INTSTAT, RXNOISEINT, 15, 1)
	FIELD(USART_INTSTAT, ABERRINT, 16, 1)
REG32(USART_OSR, 0x28)
	FIELD(USART_OSR, OSRVAL, 0, 4)
REG32(USART_ADDR, 0x2c)
	FIELD(USART_ADDR, ADDRESS, 0, 8)
REG32(USART_FIFOCFG, 0xe00)
	FIELD(USART_FIFOCFG, ENABLETX, 0, 1)
	FIELD(USART_FIFOCFG, ENABLERX, 1, 1)
	FIELD(USART_FIFOCFG, SIZE, 4, 2)
	FIELD(USART_FIFOCFG, DMATX, 12, 1)
	FIELD(USART_FIFOCFG, DMARX, 13, 1)
	FIELD(USART_FIFOCFG, WAKETX, 14, 1)
	FIELD(USART_FIFOCFG, WAKERX, 15, 1)
	FIELD(USART_FIFOCFG, EMPTYTX, 16, 1)
	FIELD(USART_FIFOCFG, EMPTYRX, 17, 1)
REG32(USART_FIFOSTAT, 0xe04)
	FIELD(USART_FIFOSTAT, TXERR, 0, 1)
	FIELD(USART_FIFOSTAT, RXERR, 1, 1)
	FIELD(USART_FIFOSTAT, PERINT, 3, 1)
	FIELD(USART_FIFOSTAT, TXEMPTY, 4, 1)
	FIELD(USART_FIFOSTAT, TXNOTFULL, 5, 1)
	FIELD(USART_FIFOSTAT, RXNOTEMPTY, 6, 1)
	FIELD(USART_FIFOSTAT, RXFULL, 7, 1)
	FIELD(USART_FIFOSTAT, TXLVL, 8, 5)
	FIELD(USART_FIFOSTAT, RXLVL, 16, 5)
REG32(USART_FIFOTRIG, 0xe08)
	FIELD(USART_FIFOTRIG, TXLVLENA, 0, 1)
	FIELD(USART_FIFOTRIG, RXLVLENA, 1, 1)
	FIELD(USART_FIFOTRIG, TXLVL, 8, 4)
	FIELD(USART_FIFOTRIG, RXLVL, 16, 4)
REG32(USART_FIFOINTENSET, 0xe10)
	FIELD(USART_FIFOINTENSET, TXERR, 0, 1)
	FIELD(USART_FIFOINTENSET, RXERR, 1, 1)
	FIELD(USART_FIFOINTENSET, TXLVL, 2, 1)
	FIELD(USART_FIFOINTENSET, RXLVL, 3, 1)
REG32(USART_FIFOINTENCLR, 0xe14)
	FIELD(USART_FIFOINTENCLR, TXERR, 0, 1)
	FIELD(USART_FIFOINTENCLR, RXERR, 1, 1)
	FIELD(USART_FIFOINTENCLR, TXLVL, 2, 1)
	FIELD(USART_FIFOINTENCLR, RXLVL, 3, 1)
REG32(USART_FIFOINTSTAT, 0xe18)
	FIELD(USART_FIFOINTSTAT, TXERR, 0, 1)
	FIELD(USART_FIFOINTSTAT, RXERR, 1, 1)
	FIELD(USART_FIFOINTSTAT, TXLVL, 2, 1)
	FIELD(USART_FIFOINTSTAT, RXLVL, 3, 1)
	FIELD(USART_FIFOINTSTAT, PERINT, 4, 1)
REG32(USART_FIFOWR, 0xe20)
	FIELD(USART_FIFOWR, TXDATA, 0, 9)
REG32(USART_FIFORD, 0xe30)
	FIELD(USART_FIFORD, RXDATA, 0, 9)
	FIELD(USART_FIFORD, FRAMERR, 13, 1)
	FIELD(USART_FIFORD, PARITYERR, 14, 1)
	FIELD(USART_FIFORD, RXNOISE, 15, 1)
REG32(USART_FIFORDNOPOP, 0xe40)
	FIELD(USART_FIFORDNOPOP, RXDATA, 0, 9)
	FIELD(USART_FIFORDNOPOP, FRAMERR, 13, 1)
	FIELD(USART_FIFORDNOPOP, PARITYERR, 14, 1)
	FIELD(USART_FIFORDNOPOP, RXNOISE, 15, 1)
REG32(USART_ID, 0xffc)
	FIELD(USART_ID, APERTURE, 0, 8)
	FIELD(USART_ID, MINOR_REV, 8, 4)
	FIELD(USART_ID, MAJOR_REV, 12, 4)
	FIELD(USART_ID, ID, 16, 16)


__attribute__((unused))
static void lpc51u68_usart_update(LPC51U68USART *t) {
	int conditions = ((t->FIFOSTAT & 0x40) || (t->FIFOSTAT & 0x20) || (t->FIFOSTAT & 0x2) || (t->STAT & 0x4000) || (t->STAT & 0x8000) || (t->FIFOSTAT & 0x1) || (t->STAT & 0x2000) || ((t->INTENSET & 0x8) && (t->INTSTAT & 0x8)));
	qemu_set_irq(t->irq[0], conditions);

}

static int lpc51u68_usart_can_receive(void *opaque) {
	return 1;
}

static void lpc51u68_usart_receive(void *opaque, const uint8_t *buf, int size) {
	LPC51U68USART *t = LPC51U68_USART(opaque);

	t->FIFORD = *buf;
	t->FIFOSTAT |= 0x40;

	lpc51u68_usart_update(t);
}

static gboolean lpc51u68_usart_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	LPC51U68USART *t = LPC51U68_USART(opaque);
	int ret;

	t->watch_tag = 0;
	t->STAT &= (~(0x8));
	t->FIFOSTAT &= (~(0x20));

	lpc51u68_usart_update(t);

	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->FIFOWR), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, lpc51u68_usart_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->STAT |= 0x8;
	t->FIFOSTAT |= 0x20;

	lpc51u68_usart_update(t);

	return FALSE;
}

static void lpc51u68_usart_register_reset(LPC51U68USART *t) {
	t->CFG = 0x0;
	t->CTL = 0x0;
	t->STAT = 0xa;
	t->INTENSET = 0x0;
	t->INTENCLR = 0x0;
	t->BRG = 0x0;
	t->INTSTAT = 0x0;
	t->OSR = 0xf;
	t->ADDR = 0x0;
	t->FIFOCFG = 0x0;
	t->FIFOSTAT = 0x30;
	t->FIFOTRIG = 0x0;
	t->FIFOINTENSET = 0x0;
	t->FIFOINTENCLR = 0x0;
	t->FIFOINTSTAT = 0x0;
	t->FIFOWR = 0x0;
	t->FIFORD = 0x0;
	t->FIFORDNOPOP = 0x0;
	t->ID = 0xe0100000;
	t->STAT |= 0x8;
	t->FIFOSTAT |= 0x20;

}

static uint64_t lpc51u68_usart_read(void *opaque, hwaddr offset, unsigned size) {
	LPC51U68USART *t = LPC51U68_USART(opaque);
	uint64_t ret;

	switch (offset) {
		case A_USART_CFG:
			ret = t->CFG;
			break;
		case A_USART_CTL:
			ret = t->CTL;
			break;
		case A_USART_STAT:
			ret = t->STAT;
			break;
		case A_USART_INTENSET:
			ret = t->INTENSET;
			break;
		case A_USART_BRG:
			ret = t->BRG;
			break;
		case A_USART_INTSTAT:
			ret = t->INTSTAT;
			break;
		case A_USART_OSR:
			ret = t->OSR;
			break;
		case A_USART_ADDR:
			ret = t->ADDR;
			break;
		case A_USART_FIFOCFG:
			ret = t->FIFOCFG;
			break;
		case A_USART_FIFOSTAT:
			ret = t->FIFOSTAT;
			break;
		case A_USART_FIFOTRIG:
			ret = t->FIFOTRIG;
			break;
		case A_USART_FIFOINTENSET:
			ret = t->FIFOINTENSET;
			break;
		case A_USART_FIFOINTENCLR:
			ret = t->FIFOINTENCLR;
			break;
		case A_USART_FIFOINTSTAT:
			ret = t->FIFOINTSTAT;
			break;
		case A_USART_FIFOWR:
			ret = t->FIFOWR;
			break;
		case A_USART_FIFORD:
			ret = t->FIFORD;
			t->FIFOSTAT &= (~(0x40));
			qemu_chr_fe_accept_input(&(t->chr));
			lpc51u68_usart_update(t);
			break;
		case A_USART_FIFORDNOPOP:
			ret = t->FIFORDNOPOP;
			break;
		case A_USART_ID:
			ret = t->ID;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "LPC51U68 USART read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void lpc51u68_usart_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	LPC51U68USART *t = LPC51U68_USART(opaque);


	switch (offset) {
		case A_USART_CFG ... A_USART_CFG + 3:
			value = value << ((offset - A_USART_CFG) << 3);
			offset = A_USART_CFG;
			break;
		case A_USART_CTL ... A_USART_CTL + 3:
			value = value << ((offset - A_USART_CTL) << 3);
			offset = A_USART_CTL;
			break;
		case A_USART_STAT ... A_USART_STAT + 3:
			value = value << ((offset - A_USART_STAT) << 3);
			offset = A_USART_STAT;
			break;
		case A_USART_INTENSET ... A_USART_INTENSET + 3:
			value = value << ((offset - A_USART_INTENSET) << 3);
			offset = A_USART_INTENSET;
			break;
		case A_USART_INTENCLR ... A_USART_INTENCLR + 3:
			value = value << ((offset - A_USART_INTENCLR) << 3);
			offset = A_USART_INTENCLR;
			break;
		case A_USART_BRG ... A_USART_BRG + 3:
			value = value << ((offset - A_USART_BRG) << 3);
			offset = A_USART_BRG;
			break;
		case A_USART_OSR ... A_USART_OSR + 3:
			value = value << ((offset - A_USART_OSR) << 3);
			offset = A_USART_OSR;
			break;
		case A_USART_ADDR ... A_USART_ADDR + 3:
			value = value << ((offset - A_USART_ADDR) << 3);
			offset = A_USART_ADDR;
			break;
		case A_USART_FIFOCFG ... A_USART_FIFOCFG + 3:
			value = value << ((offset - A_USART_FIFOCFG) << 3);
			offset = A_USART_FIFOCFG;
			break;
		case A_USART_FIFOSTAT ... A_USART_FIFOSTAT + 3:
			value = value << ((offset - A_USART_FIFOSTAT) << 3);
			offset = A_USART_FIFOSTAT;
			break;
		case A_USART_FIFOTRIG ... A_USART_FIFOTRIG + 3:
			value = value << ((offset - A_USART_FIFOTRIG) << 3);
			offset = A_USART_FIFOTRIG;
			break;
		case A_USART_FIFOINTENSET ... A_USART_FIFOINTENSET + 3:
			value = value << ((offset - A_USART_FIFOINTENSET) << 3);
			offset = A_USART_FIFOINTENSET;
			break;
		case A_USART_FIFOINTENCLR ... A_USART_FIFOINTENCLR + 3:
			value = value << ((offset - A_USART_FIFOINTENCLR) << 3);
			offset = A_USART_FIFOINTENCLR;
			break;
		case A_USART_FIFOWR ... A_USART_FIFOWR + 3:
			value = value << ((offset - A_USART_FIFOWR) << 3);
			offset = A_USART_FIFOWR;
			break;

		default: break;
	}

	switch (offset) {
		case A_USART_CFG:
			t->CFG = value;
			break;
		case A_USART_CTL:
			t->CTL = value;
			break;
		case A_USART_STAT:
			t->STAT &= value;
			lpc51u68_usart_update(t);
			break;
		case A_USART_INTENSET:
			t->INTENSET = value;
			lpc51u68_usart_update(t);
			break;
		case A_USART_INTENCLR:
			t->INTENCLR = value;
			break;
		case A_USART_BRG:
			t->BRG = value;
			break;
		case A_USART_OSR:
			t->OSR = value;
			break;
		case A_USART_ADDR:
			t->ADDR = value;
			break;
		case A_USART_FIFOCFG:
			t->FIFOCFG = value;
			if ((value & 0x2)) {
				t->FIFOSTAT |= 0x40;
			}
			if ((value & 0x20000)) {
				t->FIFOSTAT &= (~(0x40));
			}
			lpc51u68_usart_update(t);
			break;
		case A_USART_FIFOSTAT:
			t->FIFOSTAT &= value;
			lpc51u68_usart_update(t);
			break;
		case A_USART_FIFOTRIG:
			t->FIFOTRIG = value;
			break;
		case A_USART_FIFOINTENSET:
			t->FIFOINTENSET = value;
			break;
		case A_USART_FIFOINTENCLR:
			t->FIFOINTENCLR = value;
			break;
		case A_USART_FIFOWR:
			t->FIFOWR = value;
			lpc51u68_usart_transmit(NULL, G_IO_OUT, t);
			if (((!(value & 0x100)) && (!(value & 0x200)) && (!(value & 0x400)) && (!(value & 0x800)) && (!(value & 0x1000)) && (!(value & 0x2000)) && (!(value & 0x4000)) && (!(value & 0x8000)) && (!(value & 0x10000)) && (!(value & 0x20000)) && (!(value & 0x40000)) && (!(value & 0x80000)) && (!(value & 0x100000)) && (!(value & 0x200000)) && (!(value & 0x400000)) && (!(value & 0x800000)) && (!(value & 0x1000000)) && (!(value & 0x2000000)) && (!(value & 0x4000000)) && (!(value & 0x8000000)) && (!(value & 0x10000000)) && (!(value & 0x20000000)) && (!(value & 0x40000000)) && (!(value & 0x80000000)))) {
				t->STAT |= 0x8;
				t->FIFOSTAT |= 0x20;
			}
			t->STAT |= 0x8;
			lpc51u68_usart_update(t);
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "LPC51U68 USART write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps lpc51u68_usart_ops = {
	.read = lpc51u68_usart_read,
	.write = lpc51u68_usart_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void lpc51u68_usart_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	LPC51U68USART *t = LPC51U68_USART(obj);
	memory_region_init_io(&(t->iomem), obj, &lpc51u68_usart_ops, t, "lpc51u68-usart", LPC51U68_USART_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void lpc51u68_usart_realize(DeviceState *dev, Error **errp) {
	LPC51U68USART *t = LPC51U68_USART(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), lpc51u68_usart_can_receive, lpc51u68_usart_receive,
		NULL, NULL, t, NULL, true);

}

static void lpc51u68_usart_reset_enter(Object *obj, ResetType type) {
	LPC51U68USART *t = LPC51U68_USART(obj);
	lpc51u68_usart_register_reset(t);
}

static const VMStateDescription lpc51u68_usart_vmstate = {
	.name = "lpc51u68-usart",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CFG, LPC51U68USART),
		VMSTATE_UINT32(CTL, LPC51U68USART),
		VMSTATE_UINT32(STAT, LPC51U68USART),
		VMSTATE_UINT32(INTENSET, LPC51U68USART),
		VMSTATE_UINT32(INTENCLR, LPC51U68USART),
		VMSTATE_UINT32(BRG, LPC51U68USART),
		VMSTATE_UINT32(INTSTAT, LPC51U68USART),
		VMSTATE_UINT32(OSR, LPC51U68USART),
		VMSTATE_UINT32(ADDR, LPC51U68USART),
		VMSTATE_UINT32(FIFOCFG, LPC51U68USART),
		VMSTATE_UINT32(FIFOSTAT, LPC51U68USART),
		VMSTATE_UINT32(FIFOTRIG, LPC51U68USART),
		VMSTATE_UINT32(FIFOINTENSET, LPC51U68USART),
		VMSTATE_UINT32(FIFOINTENCLR, LPC51U68USART),
		VMSTATE_UINT32(FIFOINTSTAT, LPC51U68USART),
		VMSTATE_UINT32(FIFOWR, LPC51U68USART),
		VMSTATE_UINT32(FIFORD, LPC51U68USART),
		VMSTATE_UINT32(FIFORDNOPOP, LPC51U68USART),
		VMSTATE_UINT32(ID, LPC51U68USART),

		VMSTATE_END_OF_LIST()
	}
};

static Property lpc51u68_usart_properties[] = {
	DEFINE_PROP_CHR("chardev", LPC51U68USART, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void lpc51u68_usart_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &lpc51u68_usart_vmstate;
	dc->realize = lpc51u68_usart_realize;
	rc->phases.enter = lpc51u68_usart_reset_enter;
	device_class_set_props(dc, lpc51u68_usart_properties);

}

static const TypeInfo lpc51u68_usart_info = {
	.name = TYPE_LPC51U68_USART,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(LPC51U68USART),
	.instance_init = lpc51u68_usart_init,
	.class_init = lpc51u68_usart_class_init,
};

static void lpc51u68_usart_register_types(void) {
	type_register_static(&lpc51u68_usart_info);
}

type_init(lpc51u68_usart_register_types);

#define TYPE_LPC51U68_I2C "lpc51u68-i2c"
OBJECT_DECLARE_SIMPLE_TYPE(LPC51U68I2C, LPC51U68_I2C)

struct LPC51U68I2C {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t CFG;
	uint32_t STAT;
	uint32_t INTENSET;
	uint32_t INTENCLR;
	uint32_t TIMEOUT;
	uint32_t CLKDIV;
	uint32_t INTSTAT;
	uint32_t MSTCTL;
	uint32_t MSTTIME;
	uint32_t MSTDAT;
	uint32_t SLVCTL;
	uint32_t SLVDAT;
	uint32_t SLVQUAL0;
	uint32_t MONRXDAT;
	uint32_t ID;
	uint32_t SLVADR0;
	uint32_t SLVADR1;
	uint32_t SLVADR2;
	uint32_t SLVADR3;
	/* chardev backend */
	CharBackend chr;
	guint watch_tag;

	/* base */
	uint32_t base;

};
#define LPC51U68_I2C_SIZE				0x1000

REG32(I2C_CFG, 0x800)
	FIELD(I2C_CFG, MSTEN, 0, 1)
	FIELD(I2C_CFG, SLVEN, 1, 1)
	FIELD(I2C_CFG, MONEN, 2, 1)
	FIELD(I2C_CFG, TIMEOUTEN, 3, 1)
	FIELD(I2C_CFG, MONCLKSTR, 4, 1)
	FIELD(I2C_CFG, HSCAPABLE, 5, 1)
REG32(I2C_STAT, 0x804)
	FIELD(I2C_STAT, MSTPENDING, 0, 1)
	FIELD(I2C_STAT, MSTSTATE, 1, 3)
	FIELD(I2C_STAT, MSTARBLOSS, 4, 1)
	FIELD(I2C_STAT, MSTSTSTPERR, 6, 1)
	FIELD(I2C_STAT, SLVPENDING, 8, 1)
	FIELD(I2C_STAT, SLVSTATE, 9, 2)
	FIELD(I2C_STAT, SLVNOTSTR, 11, 1)
	FIELD(I2C_STAT, SLVIDX, 12, 2)
	FIELD(I2C_STAT, SLVSEL, 14, 1)
	FIELD(I2C_STAT, SLVDESEL, 15, 1)
	FIELD(I2C_STAT, MONRDY, 16, 1)
	FIELD(I2C_STAT, MONOV, 17, 1)
	FIELD(I2C_STAT, MONACTIVE, 18, 1)
	FIELD(I2C_STAT, MONIDLE, 19, 1)
	FIELD(I2C_STAT, EVENTTIMEOUT, 24, 1)
	FIELD(I2C_STAT, SCLTIMEOUT, 25, 1)
REG32(I2C_INTENSET, 0x808)
	FIELD(I2C_INTENSET, MSTPENDINGEN, 0, 1)
	FIELD(I2C_INTENSET, MSTARBLOSSEN, 4, 1)
	FIELD(I2C_INTENSET, MSTSTSTPERREN, 6, 1)
	FIELD(I2C_INTENSET, SLVPENDINGEN, 8, 1)
	FIELD(I2C_INTENSET, SLVNOTSTREN, 11, 1)
	FIELD(I2C_INTENSET, SLVDESELEN, 15, 1)
	FIELD(I2C_INTENSET, MONRDYEN, 16, 1)
	FIELD(I2C_INTENSET, MONOVEN, 17, 1)
	FIELD(I2C_INTENSET, MONIDLEEN, 19, 1)
	FIELD(I2C_INTENSET, EVENTTIMEOUTEN, 24, 1)
	FIELD(I2C_INTENSET, SCLTIMEOUTEN, 25, 1)
REG32(I2C_INTENCLR, 0x80c)
	FIELD(I2C_INTENCLR, MSTPENDINGCLR, 0, 1)
	FIELD(I2C_INTENCLR, MSTARBLOSSCLR, 4, 1)
	FIELD(I2C_INTENCLR, MSTSTSTPERRCLR, 6, 1)
	FIELD(I2C_INTENCLR, SLVPENDINGCLR, 8, 1)
	FIELD(I2C_INTENCLR, SLVNOTSTRCLR, 11, 1)
	FIELD(I2C_INTENCLR, SLVDESELCLR, 15, 1)
	FIELD(I2C_INTENCLR, MONRDYCLR, 16, 1)
	FIELD(I2C_INTENCLR, MONOVCLR, 17, 1)
	FIELD(I2C_INTENCLR, MONIDLECLR, 19, 1)
	FIELD(I2C_INTENCLR, EVENTTIMEOUTCLR, 24, 1)
	FIELD(I2C_INTENCLR, SCLTIMEOUTCLR, 25, 1)
REG32(I2C_TIMEOUT, 0x810)
	FIELD(I2C_TIMEOUT, TOMIN, 0, 4)
	FIELD(I2C_TIMEOUT, TO, 4, 12)
REG32(I2C_CLKDIV, 0x814)
	FIELD(I2C_CLKDIV, DIVVAL, 0, 16)
REG32(I2C_INTSTAT, 0x818)
	FIELD(I2C_INTSTAT, MSTPENDING, 0, 1)
	FIELD(I2C_INTSTAT, MSTARBLOSS, 4, 1)
	FIELD(I2C_INTSTAT, MSTSTSTPERR, 6, 1)
	FIELD(I2C_INTSTAT, SLVPENDING, 8, 1)
	FIELD(I2C_INTSTAT, SLVNOTSTR, 11, 1)
	FIELD(I2C_INTSTAT, SLVDESEL, 15, 1)
	FIELD(I2C_INTSTAT, MONRDY, 16, 1)
	FIELD(I2C_INTSTAT, MONOV, 17, 1)
	FIELD(I2C_INTSTAT, MONIDLE, 19, 1)
	FIELD(I2C_INTSTAT, EVENTTIMEOUT, 24, 1)
	FIELD(I2C_INTSTAT, SCLTIMEOUT, 25, 1)
REG32(I2C_MSTCTL, 0x820)
	FIELD(I2C_MSTCTL, MSTCONTINUE, 0, 1)
	FIELD(I2C_MSTCTL, MSTSTART, 1, 1)
	FIELD(I2C_MSTCTL, MSTSTOP, 2, 1)
	FIELD(I2C_MSTCTL, MSTDMA, 3, 1)
REG32(I2C_MSTTIME, 0x824)
	FIELD(I2C_MSTTIME, MSTSCLLOW, 0, 3)
	FIELD(I2C_MSTTIME, MSTSCLHIGH, 4, 3)
REG32(I2C_MSTDAT, 0x828)
	FIELD(I2C_MSTDAT, DATA, 0, 8)
REG32(I2C_SLVCTL, 0x840)
	FIELD(I2C_SLVCTL, SLVCONTINUE, 0, 1)
	FIELD(I2C_SLVCTL, SLVNACK, 1, 1)
	FIELD(I2C_SLVCTL, SLVDMA, 3, 1)
	FIELD(I2C_SLVCTL, AUTOACK, 8, 1)
	FIELD(I2C_SLVCTL, AUTOMATCHREAD, 9, 1)
REG32(I2C_SLVDAT, 0x844)
	FIELD(I2C_SLVDAT, DATA, 0, 8)
REG32(I2C_SLVQUAL0, 0x858)
	FIELD(I2C_SLVQUAL0, QUALMODE0, 0, 1)
	FIELD(I2C_SLVQUAL0, SLVQUAL0, 1, 7)
REG32(I2C_MONRXDAT, 0x880)
	FIELD(I2C_MONRXDAT, MONRXDAT, 0, 8)
	FIELD(I2C_MONRXDAT, MONSTART, 8, 1)
	FIELD(I2C_MONRXDAT, MONRESTART, 9, 1)
	FIELD(I2C_MONRXDAT, MONNACK, 10, 1)
REG32(I2C_ID, 0xffc)
	FIELD(I2C_ID, APERTURE, 0, 8)
	FIELD(I2C_ID, MINOR_REV, 8, 4)
	FIELD(I2C_ID, MAJOR_REV, 12, 4)
	FIELD(I2C_ID, ID, 16, 16)
REG32(I2C_SLVADR0, 0x848)
	FIELD(I2C_SLVADR0, SADISABLE, 0, 1)
	FIELD(I2C_SLVADR0, SLVADR, 1, 7)
	FIELD(I2C_SLVADR0, AUTONACK, 15, 1)
REG32(I2C_SLVADR1, 0x84c)
	FIELD(I2C_SLVADR1, SADISABLE, 0, 1)
	FIELD(I2C_SLVADR1, SLVADR, 1, 7)
	FIELD(I2C_SLVADR1, AUTONACK, 15, 1)
REG32(I2C_SLVADR2, 0x850)
	FIELD(I2C_SLVADR2, SADISABLE, 0, 1)
	FIELD(I2C_SLVADR2, SLVADR, 1, 7)
	FIELD(I2C_SLVADR2, AUTONACK, 15, 1)
REG32(I2C_SLVADR3, 0x854)
	FIELD(I2C_SLVADR3, SADISABLE, 0, 1)
	FIELD(I2C_SLVADR3, SLVADR, 1, 7)
	FIELD(I2C_SLVADR3, AUTONACK, 15, 1)


static int lpc51u68_i2c_can_receive(void *opaque) {
	return 1;
}

static void lpc51u68_i2c_receive(void *opaque, const uint8_t *buf, int size) {
	LPC51U68I2C *t = LPC51U68_I2C(opaque);

	t->MSTDAT = *buf;
	t->SLVDAT = *buf;
	t->STAT &= (~(0x1000000));
	t->STAT |= 0x100;
	t->STAT |= 0x1;
	t->STAT &= (~(0x10));
	t->STAT &= (~(0x40));
	t->STAT &= (~(0x8));
	t->STAT &= (~(0x2000000));
	t->STAT &= (~(0x400));
	t->STAT |= 0x200;
	t->STAT |= 0x2;
	t->STAT &= (~(0x4));


}

static gboolean lpc51u68_i2c_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	LPC51U68I2C *t = LPC51U68_I2C(opaque);
	int ret;

	t->watch_tag = 0;
	t->STAT |= 0x1000000;
	t->STAT &= (~(0x100));
	t->STAT |= 0x10;
	t->STAT |= 0x40;
	t->STAT |= 0x2000000;
	t->STAT &= (~(0x1));
	t->STAT |= 0x200;
	t->STAT &= (~(0x400));



	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->MSTDAT), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, lpc51u68_i2c_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->STAT &= (~(0x1000000));
	t->STAT |= 0x400;
	t->STAT |= 0x1;
	t->STAT &= (~(0x10));
	t->STAT &= (~(0x40));
	t->STAT &= (~(0x2000000));
	t->STAT &= (~(0x200));
	t->STAT |= 0x100;



	return FALSE;
}

static void lpc51u68_i2c_register_reset(LPC51U68I2C *t) {
	t->CFG = 0x0;
	t->STAT = 0x801;
	t->INTENSET = 0x0;
	t->INTENCLR = 0x0;
	t->TIMEOUT = 0xffff;
	t->CLKDIV = 0x0;
	t->INTSTAT = 0x801;
	t->MSTCTL = 0x0;
	t->MSTTIME = 0x77;
	t->MSTDAT = 0x0;
	t->SLVCTL = 0x0;
	t->SLVDAT = 0x0;
	t->SLVQUAL0 = 0x0;
	t->MONRXDAT = 0x0;
	t->ID = 0x0;
	t->SLVADR0 = 0x1;
	t->SLVADR1 = 0x1;
	t->SLVADR2 = 0x1;
	t->SLVADR3 = 0x1;
	t->STAT &= (~(0x1000000));
	t->STAT |= 0x400;
	t->STAT |= 0x1;
	t->STAT &= (~(0x10));
	t->STAT &= (~(0x40));
	t->STAT &= (~(0x2000000));
	t->STAT &= (~(0x200));
	t->STAT |= 0x100;

}

static uint64_t lpc51u68_i2c_read(void *opaque, hwaddr offset, unsigned size) {
	LPC51U68I2C *t = LPC51U68_I2C(opaque);
	uint64_t ret;

	switch (offset) {
		case A_I2C_CFG:
			ret = t->CFG;
			break;
		case A_I2C_STAT:
			ret = t->STAT;
			break;
		case A_I2C_INTENSET:
			ret = t->INTENSET;
			break;
		case A_I2C_TIMEOUT:
			ret = t->TIMEOUT;
			break;
		case A_I2C_CLKDIV:
			ret = t->CLKDIV;
			break;
		case A_I2C_INTSTAT:
			ret = t->INTSTAT;
			break;
		case A_I2C_MSTCTL:
			ret = t->MSTCTL;
			break;
		case A_I2C_MSTTIME:
			ret = t->MSTTIME;
			break;
		case A_I2C_MSTDAT:
			ret = t->MSTDAT;
			t->STAT |= 0x400;
			t->STAT |= 0x1000000;
			t->STAT &= (~(0x100));
			t->STAT |= 0x8;
			t->STAT |= 0x4;
			t->STAT |= 0x10;
			t->STAT &= (~(0x2));
			t->STAT |= 0x2000000;
			t->STAT &= (~(0x200));
			t->STAT |= 0x40;
			t->STAT &= (~(0x1));
			qemu_chr_fe_accept_input(&(t->chr));
			break;
		case A_I2C_SLVCTL:
			ret = t->SLVCTL;
			break;
		case A_I2C_SLVDAT:
			ret = t->SLVDAT;
			t->STAT |= 0x400;
			t->STAT |= 0x1000000;
			t->STAT &= (~(0x100));
			t->STAT |= 0x8;
			t->STAT |= 0x4;
			t->STAT |= 0x10;
			t->STAT &= (~(0x2));
			t->STAT |= 0x2000000;
			t->STAT &= (~(0x200));
			t->STAT |= 0x40;
			t->STAT &= (~(0x1));
			qemu_chr_fe_accept_input(&(t->chr));
			break;
		case A_I2C_SLVQUAL0:
			ret = t->SLVQUAL0;
			break;
		case A_I2C_MONRXDAT:
			ret = t->MONRXDAT;
			break;
		case A_I2C_ID:
			ret = t->ID;
			break;
		case A_I2C_SLVADR0:
			ret = t->SLVADR0;
			break;
		case A_I2C_SLVADR1:
			ret = t->SLVADR1;
			break;
		case A_I2C_SLVADR2:
			ret = t->SLVADR2;
			break;
		case A_I2C_SLVADR3:
			ret = t->SLVADR3;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "LPC51U68 I2C read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void lpc51u68_i2c_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	LPC51U68I2C *t = LPC51U68_I2C(opaque);


	switch (offset) {
		case A_I2C_CFG ... A_I2C_CFG + 3:
			value = value << ((offset - A_I2C_CFG) << 3);
			offset = A_I2C_CFG;
			break;
		case A_I2C_STAT ... A_I2C_STAT + 3:
			value = value << ((offset - A_I2C_STAT) << 3);
			offset = A_I2C_STAT;
			break;
		case A_I2C_INTENSET ... A_I2C_INTENSET + 3:
			value = value << ((offset - A_I2C_INTENSET) << 3);
			offset = A_I2C_INTENSET;
			break;
		case A_I2C_INTENCLR ... A_I2C_INTENCLR + 3:
			value = value << ((offset - A_I2C_INTENCLR) << 3);
			offset = A_I2C_INTENCLR;
			break;
		case A_I2C_TIMEOUT ... A_I2C_TIMEOUT + 3:
			value = value << ((offset - A_I2C_TIMEOUT) << 3);
			offset = A_I2C_TIMEOUT;
			break;
		case A_I2C_CLKDIV ... A_I2C_CLKDIV + 3:
			value = value << ((offset - A_I2C_CLKDIV) << 3);
			offset = A_I2C_CLKDIV;
			break;
		case A_I2C_MSTCTL ... A_I2C_MSTCTL + 3:
			value = value << ((offset - A_I2C_MSTCTL) << 3);
			offset = A_I2C_MSTCTL;
			break;
		case A_I2C_MSTTIME ... A_I2C_MSTTIME + 3:
			value = value << ((offset - A_I2C_MSTTIME) << 3);
			offset = A_I2C_MSTTIME;
			break;
		case A_I2C_MSTDAT ... A_I2C_MSTDAT + 3:
			value = value << ((offset - A_I2C_MSTDAT) << 3);
			offset = A_I2C_MSTDAT;
			break;
		case A_I2C_SLVCTL ... A_I2C_SLVCTL + 3:
			value = value << ((offset - A_I2C_SLVCTL) << 3);
			offset = A_I2C_SLVCTL;
			break;
		case A_I2C_SLVDAT ... A_I2C_SLVDAT + 3:
			value = value << ((offset - A_I2C_SLVDAT) << 3);
			offset = A_I2C_SLVDAT;
			break;
		case A_I2C_SLVQUAL0 ... A_I2C_SLVQUAL0 + 3:
			value = value << ((offset - A_I2C_SLVQUAL0) << 3);
			offset = A_I2C_SLVQUAL0;
			break;
		case A_I2C_SLVADR0 ... A_I2C_SLVADR0 + 3:
			value = value << ((offset - A_I2C_SLVADR0) << 3);
			offset = A_I2C_SLVADR0;
			break;
		case A_I2C_SLVADR1 ... A_I2C_SLVADR1 + 3:
			value = value << ((offset - A_I2C_SLVADR1) << 3);
			offset = A_I2C_SLVADR1;
			break;
		case A_I2C_SLVADR2 ... A_I2C_SLVADR2 + 3:
			value = value << ((offset - A_I2C_SLVADR2) << 3);
			offset = A_I2C_SLVADR2;
			break;
		case A_I2C_SLVADR3 ... A_I2C_SLVADR3 + 3:
			value = value << ((offset - A_I2C_SLVADR3) << 3);
			offset = A_I2C_SLVADR3;
			break;

		default: break;
	}

	switch (offset) {
		case A_I2C_CFG:
			t->CFG = value;
			break;
		case A_I2C_STAT:
			t->STAT &= value;
			t->STAT &= (~(0x1000000));
			t->STAT |= 0x400;
			t->STAT |= 0x1;
			t->STAT &= (~(0x10));
			t->STAT &= (~(0x40));
			t->STAT &= (~(0x2000000));
			t->STAT &= (~(0x200));
			t->STAT |= 0x100;
			break;
		case A_I2C_INTENSET:
			t->INTENSET = value;
			break;
		case A_I2C_INTENCLR:
			t->INTENCLR = value;
			break;
		case A_I2C_TIMEOUT:
			t->TIMEOUT = value;
			break;
		case A_I2C_CLKDIV:
			t->CLKDIV = value;
			break;
		case A_I2C_MSTCTL:
			t->MSTCTL = value;
			break;
		case A_I2C_MSTTIME:
			t->MSTTIME = value;
			break;
		case A_I2C_MSTDAT:
			t->MSTDAT = value;
			lpc51u68_i2c_transmit(NULL, G_IO_OUT, t);
			break;
		case A_I2C_SLVCTL:
			t->SLVCTL = value;
			break;
		case A_I2C_SLVDAT:
			t->SLVDAT = value;
			lpc51u68_i2c_transmit(NULL, G_IO_OUT, t);
			break;
		case A_I2C_SLVQUAL0:
			t->SLVQUAL0 = value;
			break;
		case A_I2C_SLVADR0:
			t->SLVADR0 = value;
			break;
		case A_I2C_SLVADR1:
			t->SLVADR1 = value;
			break;
		case A_I2C_SLVADR2:
			t->SLVADR2 = value;
			break;
		case A_I2C_SLVADR3:
			t->SLVADR3 = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "LPC51U68 I2C write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps lpc51u68_i2c_ops = {
	.read = lpc51u68_i2c_read,
	.write = lpc51u68_i2c_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void lpc51u68_i2c_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	LPC51U68I2C *t = LPC51U68_I2C(obj);
	memory_region_init_io(&(t->iomem), obj, &lpc51u68_i2c_ops, t, "lpc51u68-i2c", LPC51U68_I2C_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void lpc51u68_i2c_realize(DeviceState *dev, Error **errp) {
	LPC51U68I2C *t = LPC51U68_I2C(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), lpc51u68_i2c_can_receive, lpc51u68_i2c_receive,
		NULL, NULL, t, NULL, true);

}

static void lpc51u68_i2c_reset_enter(Object *obj, ResetType type) {
	LPC51U68I2C *t = LPC51U68_I2C(obj);
	lpc51u68_i2c_register_reset(t);
}

static const VMStateDescription lpc51u68_i2c_vmstate = {
	.name = "lpc51u68-i2c",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CFG, LPC51U68I2C),
		VMSTATE_UINT32(STAT, LPC51U68I2C),
		VMSTATE_UINT32(INTENSET, LPC51U68I2C),
		VMSTATE_UINT32(INTENCLR, LPC51U68I2C),
		VMSTATE_UINT32(TIMEOUT, LPC51U68I2C),
		VMSTATE_UINT32(CLKDIV, LPC51U68I2C),
		VMSTATE_UINT32(INTSTAT, LPC51U68I2C),
		VMSTATE_UINT32(MSTCTL, LPC51U68I2C),
		VMSTATE_UINT32(MSTTIME, LPC51U68I2C),
		VMSTATE_UINT32(MSTDAT, LPC51U68I2C),
		VMSTATE_UINT32(SLVCTL, LPC51U68I2C),
		VMSTATE_UINT32(SLVDAT, LPC51U68I2C),
		VMSTATE_UINT32(SLVQUAL0, LPC51U68I2C),
		VMSTATE_UINT32(MONRXDAT, LPC51U68I2C),
		VMSTATE_UINT32(ID, LPC51U68I2C),
		VMSTATE_UINT32(SLVADR0, LPC51U68I2C),
		VMSTATE_UINT32(SLVADR1, LPC51U68I2C),
		VMSTATE_UINT32(SLVADR2, LPC51U68I2C),
		VMSTATE_UINT32(SLVADR3, LPC51U68I2C),

		VMSTATE_END_OF_LIST()
	}
};

static Property lpc51u68_i2c_properties[] = {
	DEFINE_PROP_CHR("chardev", LPC51U68I2C, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void lpc51u68_i2c_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &lpc51u68_i2c_vmstate;
	dc->realize = lpc51u68_i2c_realize;
	rc->phases.enter = lpc51u68_i2c_reset_enter;
	device_class_set_props(dc, lpc51u68_i2c_properties);

}

static const TypeInfo lpc51u68_i2c_info = {
	.name = TYPE_LPC51U68_I2C,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(LPC51U68I2C),
	.instance_init = lpc51u68_i2c_init,
	.class_init = lpc51u68_i2c_class_init,
};

static void lpc51u68_i2c_register_types(void) {
	type_register_static(&lpc51u68_i2c_info);
}

type_init(lpc51u68_i2c_register_types);

#define TYPE_NXP_LPC_51U68_MACHINE MACHINE_TYPE_NAME("nxp_lpc_51u68")
OBJECT_DECLARE_TYPE(NXP_LPC_51U68MachineState, NXP_LPC_51U68MachineClass, NXP_LPC_51U68_MACHINE)

struct NXP_LPC_51U68MachineClass {
	MachineClass parent;
};

struct NXP_LPC_51U68MachineState {
	MachineState parent;
	ARMv7MState armv7m;
};

static void nxp_lpc_51u68_periph_init(MachineState *machine) {
	NXP_LPC_51U68MachineState *sms = NXP_LPC_51U68_MACHINE(machine);
	LPC51U68RTC *p0 = g_new(LPC51U68RTC, 1);
	object_initialize_child(OBJECT(sms), "RTC", p0, TYPE_LPC51U68_RTC);
	sysbus_realize(SYS_BUS_DEVICE(p0), &error_fatal);
	p0->base = 0x4002c000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p0), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 29));
	sysbus_mmio_map(SYS_BUS_DEVICE(p0), 0, 0x4002c000);

	LPC51U68GPIO *p1 = g_new(LPC51U68GPIO, 1);
	object_initialize_child(OBJECT(sms), "GPIO", p1, TYPE_LPC51U68_GPIO);
	sysbus_realize(SYS_BUS_DEVICE(p1), &error_fatal);
	p1->base = 0x4008c000;
	sysbus_mmio_map(SYS_BUS_DEVICE(p1), 0, 0x4008c000);

	LPC51U68SYSCON *p2 = g_new(LPC51U68SYSCON, 1);
	object_initialize_child(OBJECT(sms), "SYSCON", p2, TYPE_LPC51U68_SYSCON);
	sysbus_realize(SYS_BUS_DEVICE(p2), &error_fatal);
	p2->base = 0x40000000;
	sysbus_mmio_map(SYS_BUS_DEVICE(p2), 0, 0x40000000);

	LPC51U68ASYNC_SYSCON *p3 = g_new(LPC51U68ASYNC_SYSCON, 1);
	object_initialize_child(OBJECT(sms), "ASYNC_SYSCON", p3, TYPE_LPC51U68_ASYNC_SYSCON);
	sysbus_realize(SYS_BUS_DEVICE(p3), &error_fatal);
	p3->base = 0x40040000;
	sysbus_mmio_map(SYS_BUS_DEVICE(p3), 0, 0x40040000);

	LPC51U68USART *p4 = g_new(LPC51U68USART, 1);
	object_initialize_child(OBJECT(sms), "USART0", p4, TYPE_LPC51U68_USART);
	qdev_prop_set_chr(DEVICE(p4), "chardev", qemu_chr_new("soc-usart0", "chardev:usart0", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p4), &error_fatal);
	p4->base = 0x40086000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p4), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 14));
	sysbus_mmio_map(SYS_BUS_DEVICE(p4), 0, 0x40086000);

	LPC51U68SPI *p5 = g_new(LPC51U68SPI, 1);
	object_initialize_child(OBJECT(sms), "SPI5", p5, TYPE_LPC51U68_SPI);
	qdev_prop_set_chr(DEVICE(p5), "chardev", qemu_chr_new("soc-spi5", "chardev:spi5", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p5), &error_fatal);
	p5->base = 0x40096000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p5), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 19));
	sysbus_mmio_map(SYS_BUS_DEVICE(p5), 0, 0x40096000);

	LPC51U68I2C *p6 = g_new(LPC51U68I2C, 1);
	object_initialize_child(OBJECT(sms), "I2C0", p6, TYPE_LPC51U68_I2C);
	qdev_prop_set_chr(DEVICE(p6), "chardev", qemu_chr_new("soc-i2c0", "chardev:i2c0", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p6), &error_fatal);
	p6->base = 0x40086000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p6), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 14));
	sysbus_mmio_map(SYS_BUS_DEVICE(p6), 0, 0x40086000);


}

static void nxp_lpc_51u68_common_init(MachineState *machine) {
	NXP_LPC_51U68MachineState *sms = NXP_LPC_51U68_MACHINE(machine);
	MemoryRegion *sysmem = get_system_memory();
	MemoryRegion *mem;
	Clock *cpuclk = clock_new(OBJECT(sms), "SYSCLK");
	clock_set_hz(cpuclk, 150000000);
	Clock *refclk = clock_new(OBJECT(sms), "REFCLK");
	clock_set_hz(refclk, 150000000);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_rom(mem, NULL, "main-flash", 0x40000, &error_fatal);
	memory_region_add_subregion(sysmem, 0x0, mem);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_ram(mem, NULL, "sramx", 0x8000, &error_fatal);
	memory_region_add_subregion(sysmem, 0x4000000, mem);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_ram(mem, NULL, "sram0", 0x10000, &error_fatal);
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

	nxp_lpc_51u68_periph_init(machine);

	armv7m_load_kernel(ARM_CPU(first_cpu), machine->kernel_filename, 0x0, 0x40000);
}

static void nxp_lpc_51u68_class_init(ObjectClass *oc, void *data) {
	MachineClass *mc = MACHINE_CLASS(oc);
	mc->desc = "nxp_lpc_51u68";
	mc->init = nxp_lpc_51u68_common_init;
	mc->max_cpus = 1;
	mc->default_cpus = 1;
	mc->default_cpu_type = ARM_CPU_TYPE_NAME("cortex-m0");
}

static const TypeInfo nxp_lpc_51u68_info = {
	.name = TYPE_NXP_LPC_51U68_MACHINE,
	.parent = TYPE_MACHINE,
	.instance_size = sizeof(NXP_LPC_51U68MachineState),
	.class_size = sizeof(NXP_LPC_51U68MachineClass),
	.class_init = nxp_lpc_51u68_class_init
};

static void nxp_lpc_51u68_machine_init(void) {
	type_register_static(&nxp_lpc_51u68_info);
}

type_init(nxp_lpc_51u68_machine_init);
