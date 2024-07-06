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


#define TYPE_ATSAM3X8E_UART "atsam3x8e-uart"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAM3X8EUART, ATSAM3X8E_UART)

struct ATSAM3X8EUART {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t CR;
	uint32_t MR;
	uint32_t IER;
	uint32_t IDR;
	uint32_t IMR;
	uint32_t SR;
	uint32_t RHR;
	uint32_t THR;
	uint32_t BRGR;
	uint32_t RPR;
	uint32_t RCR;
	uint32_t TPR;
	uint32_t TCR;
	uint32_t RNPR;
	uint32_t RNCR;
	uint32_t TNPR;
	uint32_t TNCR;
	uint32_t PTCR;
	uint32_t PTSR;
	/* chardev backend */
	CharBackend chr;
	guint watch_tag;

	/* base */
	uint32_t base;

};
#define ATSAM3X8E_UART_SIZE				0x128

REG32(UART_CR, 0x0)
	FIELD(UART_CR, RSTRX, 2, 1)
	FIELD(UART_CR, RSTTX, 3, 1)
	FIELD(UART_CR, RXEN, 4, 1)
	FIELD(UART_CR, RXDIS, 5, 1)
	FIELD(UART_CR, TXEN, 6, 1)
	FIELD(UART_CR, TXDIS, 7, 1)
	FIELD(UART_CR, RSTSTA, 8, 1)
REG32(UART_MR, 0x4)
	FIELD(UART_MR, PAR, 9, 3)
	FIELD(UART_MR, CHMODE, 14, 2)
REG32(UART_IER, 0x8)
	FIELD(UART_IER, RXRDY, 0, 1)
	FIELD(UART_IER, TXRDY, 1, 1)
	FIELD(UART_IER, ENDRX, 3, 1)
	FIELD(UART_IER, ENDTX, 4, 1)
	FIELD(UART_IER, OVRE, 5, 1)
	FIELD(UART_IER, FRAME, 6, 1)
	FIELD(UART_IER, PARE, 7, 1)
	FIELD(UART_IER, TXEMPTY, 9, 1)
	FIELD(UART_IER, TXBUFE, 11, 1)
	FIELD(UART_IER, RXBUFF, 12, 1)
REG32(UART_IDR, 0xc)
	FIELD(UART_IDR, RXRDY, 0, 1)
	FIELD(UART_IDR, TXRDY, 1, 1)
	FIELD(UART_IDR, ENDRX, 3, 1)
	FIELD(UART_IDR, ENDTX, 4, 1)
	FIELD(UART_IDR, OVRE, 5, 1)
	FIELD(UART_IDR, FRAME, 6, 1)
	FIELD(UART_IDR, PARE, 7, 1)
	FIELD(UART_IDR, TXEMPTY, 9, 1)
	FIELD(UART_IDR, TXBUFE, 11, 1)
	FIELD(UART_IDR, RXBUFF, 12, 1)
REG32(UART_IMR, 0x10)
	FIELD(UART_IMR, RXRDY, 0, 1)
	FIELD(UART_IMR, TXRDY, 1, 1)
	FIELD(UART_IMR, ENDRX, 3, 1)
	FIELD(UART_IMR, ENDTX, 4, 1)
	FIELD(UART_IMR, OVRE, 5, 1)
	FIELD(UART_IMR, FRAME, 6, 1)
	FIELD(UART_IMR, PARE, 7, 1)
	FIELD(UART_IMR, TXEMPTY, 9, 1)
	FIELD(UART_IMR, TXBUFE, 11, 1)
	FIELD(UART_IMR, RXBUFF, 12, 1)
REG32(UART_SR, 0x14)
	FIELD(UART_SR, RXRDY, 0, 1)
	FIELD(UART_SR, TXRDY, 1, 1)
	FIELD(UART_SR, ENDRX, 3, 1)
	FIELD(UART_SR, ENDTX, 4, 1)
	FIELD(UART_SR, OVRE, 5, 1)
	FIELD(UART_SR, FRAME, 6, 1)
	FIELD(UART_SR, PARE, 7, 1)
	FIELD(UART_SR, TXEMPTY, 9, 1)
	FIELD(UART_SR, TXBUFE, 11, 1)
	FIELD(UART_SR, RXBUFF, 12, 1)
REG32(UART_RHR, 0x18)
	FIELD(UART_RHR, RXCHR, 0, 8)
REG32(UART_THR, 0x1c)
	FIELD(UART_THR, TXCHR, 0, 8)
REG32(UART_BRGR, 0x20)
	FIELD(UART_BRGR, CD, 0, 16)
REG32(UART_RPR, 0x100)
	FIELD(UART_RPR, RXPTR, 0, 32)
REG32(UART_RCR, 0x104)
	FIELD(UART_RCR, RXCTR, 0, 16)
REG32(UART_TPR, 0x108)
	FIELD(UART_TPR, TXPTR, 0, 32)
REG32(UART_TCR, 0x10c)
	FIELD(UART_TCR, TXCTR, 0, 16)
REG32(UART_RNPR, 0x110)
	FIELD(UART_RNPR, RXNPTR, 0, 32)
REG32(UART_RNCR, 0x114)
	FIELD(UART_RNCR, RXNCTR, 0, 16)
REG32(UART_TNPR, 0x118)
	FIELD(UART_TNPR, TXNPTR, 0, 32)
REG32(UART_TNCR, 0x11c)
	FIELD(UART_TNCR, TXNCTR, 0, 16)
REG32(UART_PTCR, 0x120)
	FIELD(UART_PTCR, RXTEN, 0, 1)
	FIELD(UART_PTCR, RXTDIS, 1, 1)
	FIELD(UART_PTCR, TXTEN, 8, 1)
	FIELD(UART_PTCR, TXTDIS, 9, 1)
REG32(UART_PTSR, 0x124)
	FIELD(UART_PTSR, RXTEN, 0, 1)
	FIELD(UART_PTSR, TXTEN, 8, 1)


__attribute__((unused))
static void atsam3x8e_uart_update(ATSAM3X8EUART *t) {
	int conditions = (((t->IMR & 0x1) && (t->SR & 0x1)) || ((t->IMR & 0x2) && (t->SR & 0x2)) || ((t->SR & 0x200) && (t->IMR & 0x200)));
	qemu_set_irq(t->irq[0], conditions);

}

static int atsam3x8e_uart_can_receive(void *opaque) {
	return 1;
}

static void atsam3x8e_uart_receive(void *opaque, const uint8_t *buf, int size) {
	ATSAM3X8EUART *t = ATSAM3X8E_UART(opaque);

	t->RHR = *buf;
	t->SR |= 0x1;

	atsam3x8e_uart_update(t);
}

static gboolean atsam3x8e_uart_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	ATSAM3X8EUART *t = ATSAM3X8E_UART(opaque);
	int ret;

	t->watch_tag = 0;
	t->SR &= (~(0x2));

	atsam3x8e_uart_update(t);

	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->THR), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, atsam3x8e_uart_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->SR |= 0x2;

	atsam3x8e_uart_update(t);

	return FALSE;
}

static void atsam3x8e_uart_register_reset(ATSAM3X8EUART *t) {
	t->MR = 0x0;
	t->IMR = 0x0;
	t->RHR = 0x0;
	t->BRGR = 0x0;
	t->RPR = 0x0;
	t->RCR = 0x0;
	t->TPR = 0x0;
	t->TCR = 0x0;
	t->RNPR = 0x0;
	t->RNCR = 0x0;
	t->TNPR = 0x0;
	t->TNCR = 0x0;
	t->PTCR = 0x0;
	t->PTSR = 0x0;
	t->SR |= 0x2;

}

static uint64_t atsam3x8e_uart_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAM3X8EUART *t = ATSAM3X8E_UART(opaque);
	uint64_t ret;

	switch (offset) {
		case A_UART_MR:
			ret = t->MR;
			break;
		case A_UART_IMR:
			ret = t->IMR;
			break;
		case A_UART_SR:
			ret = t->SR;
			break;
		case A_UART_RHR:
			ret = t->RHR;
			t->SR &= (~(0x1));
			qemu_chr_fe_accept_input(&(t->chr));
			atsam3x8e_uart_update(t);
			break;
		case A_UART_BRGR:
			ret = t->BRGR;
			break;
		case A_UART_RPR:
			ret = t->RPR;
			break;
		case A_UART_RCR:
			ret = t->RCR;
			break;
		case A_UART_TPR:
			ret = t->TPR;
			break;
		case A_UART_TCR:
			ret = t->TCR;
			break;
		case A_UART_RNPR:
			ret = t->RNPR;
			break;
		case A_UART_RNCR:
			ret = t->RNCR;
			break;
		case A_UART_TNPR:
			ret = t->TNPR;
			break;
		case A_UART_TNCR:
			ret = t->TNCR;
			break;
		case A_UART_PTSR:
			ret = t->PTSR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM3X8E UART read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsam3x8e_uart_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAM3X8EUART *t = ATSAM3X8E_UART(opaque);


	switch (offset) {
		case A_UART_CR ... A_UART_CR + 3:
			value = value << ((offset - A_UART_CR) << 3);
			offset = A_UART_CR;
			break;
		case A_UART_MR ... A_UART_MR + 3:
			value = value << ((offset - A_UART_MR) << 3);
			offset = A_UART_MR;
			break;
		case A_UART_IER ... A_UART_IER + 3:
			value = value << ((offset - A_UART_IER) << 3);
			offset = A_UART_IER;
			break;
		case A_UART_IDR ... A_UART_IDR + 3:
			value = value << ((offset - A_UART_IDR) << 3);
			offset = A_UART_IDR;
			break;
		case A_UART_THR ... A_UART_THR + 3:
			value = value << ((offset - A_UART_THR) << 3);
			offset = A_UART_THR;
			break;
		case A_UART_BRGR ... A_UART_BRGR + 3:
			value = value << ((offset - A_UART_BRGR) << 3);
			offset = A_UART_BRGR;
			break;
		case A_UART_RPR ... A_UART_RPR + 3:
			value = value << ((offset - A_UART_RPR) << 3);
			offset = A_UART_RPR;
			break;
		case A_UART_RCR ... A_UART_RCR + 3:
			value = value << ((offset - A_UART_RCR) << 3);
			offset = A_UART_RCR;
			break;
		case A_UART_TPR ... A_UART_TPR + 3:
			value = value << ((offset - A_UART_TPR) << 3);
			offset = A_UART_TPR;
			break;
		case A_UART_TCR ... A_UART_TCR + 3:
			value = value << ((offset - A_UART_TCR) << 3);
			offset = A_UART_TCR;
			break;
		case A_UART_RNPR ... A_UART_RNPR + 3:
			value = value << ((offset - A_UART_RNPR) << 3);
			offset = A_UART_RNPR;
			break;
		case A_UART_RNCR ... A_UART_RNCR + 3:
			value = value << ((offset - A_UART_RNCR) << 3);
			offset = A_UART_RNCR;
			break;
		case A_UART_TNPR ... A_UART_TNPR + 3:
			value = value << ((offset - A_UART_TNPR) << 3);
			offset = A_UART_TNPR;
			break;
		case A_UART_TNCR ... A_UART_TNCR + 3:
			value = value << ((offset - A_UART_TNCR) << 3);
			offset = A_UART_TNCR;
			break;
		case A_UART_PTCR ... A_UART_PTCR + 3:
			value = value << ((offset - A_UART_PTCR) << 3);
			offset = A_UART_PTCR;
			break;

		default: break;
	}

	switch (offset) {
		case A_UART_CR:
			t->CR = value;
			break;
		case A_UART_MR:
			t->MR = value;
			break;
		case A_UART_IER:
			t->IER = value;
			// PERRY PATCH
			t->IMR |= value; atsam3x8e_uart_update(t);
			break;
		case A_UART_IDR:
			t->IDR = value;
			// PERRY PATCH
			t->IMR &= (~value); atsam3x8e_uart_update(t);
			break;
		case A_UART_THR:
			t->THR = value;
			atsam3x8e_uart_transmit(NULL, G_IO_OUT, t);
			break;
		case A_UART_BRGR:
			t->BRGR = value;
			break;
		case A_UART_RPR:
			t->RPR = value;
			break;
		case A_UART_RCR:
			t->RCR = value;
			break;
		case A_UART_TPR:
			t->TPR = value;
			break;
		case A_UART_TCR:
			t->TCR = value;
			break;
		case A_UART_RNPR:
			t->RNPR = value;
			break;
		case A_UART_RNCR:
			t->RNCR = value;
			break;
		case A_UART_TNPR:
			t->TNPR = value;
			break;
		case A_UART_TNCR:
			t->TNCR = value;
			break;
		case A_UART_PTCR:
			t->PTCR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM3X8E UART write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsam3x8e_uart_ops = {
	.read = atsam3x8e_uart_read,
	.write = atsam3x8e_uart_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsam3x8e_uart_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAM3X8EUART *t = ATSAM3X8E_UART(obj);
	memory_region_init_io(&(t->iomem), obj, &atsam3x8e_uart_ops, t, "atsam3x8e-uart", ATSAM3X8E_UART_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void atsam3x8e_uart_realize(DeviceState *dev, Error **errp) {
	ATSAM3X8EUART *t = ATSAM3X8E_UART(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), atsam3x8e_uart_can_receive, atsam3x8e_uart_receive,
		NULL, NULL, t, NULL, true);

}

static void atsam3x8e_uart_reset_enter(Object *obj, ResetType type) {
	ATSAM3X8EUART *t = ATSAM3X8E_UART(obj);
	atsam3x8e_uart_register_reset(t);
}

static const VMStateDescription atsam3x8e_uart_vmstate = {
	.name = "atsam3x8e-uart",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR, ATSAM3X8EUART),
		VMSTATE_UINT32(MR, ATSAM3X8EUART),
		VMSTATE_UINT32(IER, ATSAM3X8EUART),
		VMSTATE_UINT32(IDR, ATSAM3X8EUART),
		VMSTATE_UINT32(IMR, ATSAM3X8EUART),
		VMSTATE_UINT32(SR, ATSAM3X8EUART),
		VMSTATE_UINT32(RHR, ATSAM3X8EUART),
		VMSTATE_UINT32(THR, ATSAM3X8EUART),
		VMSTATE_UINT32(BRGR, ATSAM3X8EUART),
		VMSTATE_UINT32(RPR, ATSAM3X8EUART),
		VMSTATE_UINT32(RCR, ATSAM3X8EUART),
		VMSTATE_UINT32(TPR, ATSAM3X8EUART),
		VMSTATE_UINT32(TCR, ATSAM3X8EUART),
		VMSTATE_UINT32(RNPR, ATSAM3X8EUART),
		VMSTATE_UINT32(RNCR, ATSAM3X8EUART),
		VMSTATE_UINT32(TNPR, ATSAM3X8EUART),
		VMSTATE_UINT32(TNCR, ATSAM3X8EUART),
		VMSTATE_UINT32(PTCR, ATSAM3X8EUART),
		VMSTATE_UINT32(PTSR, ATSAM3X8EUART),

		VMSTATE_END_OF_LIST()
	}
};

static Property atsam3x8e_uart_properties[] = {
	DEFINE_PROP_CHR("chardev", ATSAM3X8EUART, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void atsam3x8e_uart_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsam3x8e_uart_vmstate;
	dc->realize = atsam3x8e_uart_realize;
	rc->phases.enter = atsam3x8e_uart_reset_enter;
	device_class_set_props(dc, atsam3x8e_uart_properties);

}

static const TypeInfo atsam3x8e_uart_info = {
	.name = TYPE_ATSAM3X8E_UART,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAM3X8EUART),
	.instance_init = atsam3x8e_uart_init,
	.class_init = atsam3x8e_uart_class_init,
};

static void atsam3x8e_uart_register_types(void) {
	type_register_static(&atsam3x8e_uart_info);
}

type_init(atsam3x8e_uart_register_types);

#define TYPE_ATSAM3X8E_RSTC "atsam3x8e-rstc"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAM3X8ERSTC, ATSAM3X8E_RSTC)

struct ATSAM3X8ERSTC {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/*registers*/
	uint32_t CR;
	uint32_t SR;
	uint32_t MR;
	/* base */
	uint32_t base;

};
#define ATSAM3X8E_RSTC_SIZE				0xc

REG32(RSTC_CR, 0x0)
	FIELD(RSTC_CR, PROCRST, 0, 1)
	FIELD(RSTC_CR, PERRST, 2, 1)
	FIELD(RSTC_CR, EXTRST, 3, 1)
	FIELD(RSTC_CR, KEY, 24, 8)
REG32(RSTC_SR, 0x4)
	FIELD(RSTC_SR, URSTS, 0, 1)
	FIELD(RSTC_SR, RSTTYP, 8, 3)
	FIELD(RSTC_SR, NRSTL, 16, 1)
	FIELD(RSTC_SR, SRCMP, 17, 1)
REG32(RSTC_MR, 0x8)
	FIELD(RSTC_MR, URSTEN, 0, 1)
	FIELD(RSTC_MR, URSTIEN, 4, 1)
	FIELD(RSTC_MR, ERSTL, 8, 4)
	FIELD(RSTC_MR, KEY, 24, 8)


static void atsam3x8e_rstc_register_reset(ATSAM3X8ERSTC *t) {
	t->SR = 0x0;
	t->MR = 0x1;

}

static uint64_t atsam3x8e_rstc_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAM3X8ERSTC *t = ATSAM3X8E_RSTC(opaque);
	uint64_t ret;

	switch (offset) {
		case A_RSTC_SR:
			ret = t->SR;
			break;
		case A_RSTC_MR:
			ret = t->MR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM3X8E RSTC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsam3x8e_rstc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAM3X8ERSTC *t = ATSAM3X8E_RSTC(opaque);


	switch (offset) {
		case A_RSTC_CR ... A_RSTC_CR + 3:
			value = value << ((offset - A_RSTC_CR) << 3);
			offset = A_RSTC_CR;
			break;
		case A_RSTC_MR ... A_RSTC_MR + 3:
			value = value << ((offset - A_RSTC_MR) << 3);
			offset = A_RSTC_MR;
			break;

		default: break;
	}

	switch (offset) {
		case A_RSTC_CR:
			t->CR = value;
			break;
		case A_RSTC_MR:
			t->MR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM3X8E RSTC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsam3x8e_rstc_ops = {
	.read = atsam3x8e_rstc_read,
	.write = atsam3x8e_rstc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsam3x8e_rstc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAM3X8ERSTC *t = ATSAM3X8E_RSTC(obj);
	memory_region_init_io(&(t->iomem), obj, &atsam3x8e_rstc_ops, t, "atsam3x8e-rstc", ATSAM3X8E_RSTC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void atsam3x8e_rstc_realize(DeviceState *dev, Error **errp) {
	return;

}

static void atsam3x8e_rstc_reset_enter(Object *obj, ResetType type) {
	ATSAM3X8ERSTC *t = ATSAM3X8E_RSTC(obj);
	atsam3x8e_rstc_register_reset(t);
}

static const VMStateDescription atsam3x8e_rstc_vmstate = {
	.name = "atsam3x8e-rstc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR, ATSAM3X8ERSTC),
		VMSTATE_UINT32(SR, ATSAM3X8ERSTC),
		VMSTATE_UINT32(MR, ATSAM3X8ERSTC),

		VMSTATE_END_OF_LIST()
	}
};

static void atsam3x8e_rstc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsam3x8e_rstc_vmstate;
	dc->realize = atsam3x8e_rstc_realize;
	rc->phases.enter = atsam3x8e_rstc_reset_enter;

}

static const TypeInfo atsam3x8e_rstc_info = {
	.name = TYPE_ATSAM3X8E_RSTC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAM3X8ERSTC),
	.instance_init = atsam3x8e_rstc_init,
	.class_init = atsam3x8e_rstc_class_init,
};

static void atsam3x8e_rstc_register_types(void) {
	type_register_static(&atsam3x8e_rstc_info);
}

type_init(atsam3x8e_rstc_register_types);

#define TYPE_ATSAM3X8E_WDT "atsam3x8e-wdt"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAM3X8EWDT, ATSAM3X8E_WDT)

struct ATSAM3X8EWDT {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/*registers*/
	uint32_t CR;
	uint32_t MR;
	uint32_t SR;
	/* base */
	uint32_t base;

};
#define ATSAM3X8E_WDT_SIZE				0xc

REG32(WDT_CR, 0x0)
	FIELD(WDT_CR, WDRSTT, 0, 1)
	FIELD(WDT_CR, KEY, 24, 8)
REG32(WDT_MR, 0x4)
	FIELD(WDT_MR, WDV, 0, 12)
	FIELD(WDT_MR, WDFIEN, 12, 1)
	FIELD(WDT_MR, WDRSTEN, 13, 1)
	FIELD(WDT_MR, WDRPROC, 14, 1)
	FIELD(WDT_MR, WDDIS, 15, 1)
	FIELD(WDT_MR, WDD, 16, 12)
	FIELD(WDT_MR, WDDBGHLT, 28, 1)
	FIELD(WDT_MR, WDIDLEHLT, 29, 1)
REG32(WDT_SR, 0x8)
	FIELD(WDT_SR, WDUNF, 0, 1)
	FIELD(WDT_SR, WDERR, 1, 1)


static void atsam3x8e_wdt_register_reset(ATSAM3X8EWDT *t) {
	t->MR = 0x3fff2fff;
	t->SR = 0x0;

}

static uint64_t atsam3x8e_wdt_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAM3X8EWDT *t = ATSAM3X8E_WDT(opaque);
	uint64_t ret;

	switch (offset) {
		case A_WDT_MR:
			ret = t->MR;
			break;
		case A_WDT_SR:
			ret = t->SR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM3X8E WDT read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsam3x8e_wdt_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAM3X8EWDT *t = ATSAM3X8E_WDT(opaque);


	switch (offset) {
		case A_WDT_CR ... A_WDT_CR + 3:
			value = value << ((offset - A_WDT_CR) << 3);
			offset = A_WDT_CR;
			break;
		case A_WDT_MR ... A_WDT_MR + 3:
			value = value << ((offset - A_WDT_MR) << 3);
			offset = A_WDT_MR;
			break;

		default: break;
	}

	switch (offset) {
		case A_WDT_CR:
			t->CR = value;
			break;
		case A_WDT_MR:
			t->MR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM3X8E WDT write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsam3x8e_wdt_ops = {
	.read = atsam3x8e_wdt_read,
	.write = atsam3x8e_wdt_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsam3x8e_wdt_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAM3X8EWDT *t = ATSAM3X8E_WDT(obj);
	memory_region_init_io(&(t->iomem), obj, &atsam3x8e_wdt_ops, t, "atsam3x8e-wdt", ATSAM3X8E_WDT_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void atsam3x8e_wdt_realize(DeviceState *dev, Error **errp) {
	return;

}

static void atsam3x8e_wdt_reset_enter(Object *obj, ResetType type) {
	ATSAM3X8EWDT *t = ATSAM3X8E_WDT(obj);
	atsam3x8e_wdt_register_reset(t);
}

static const VMStateDescription atsam3x8e_wdt_vmstate = {
	.name = "atsam3x8e-wdt",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR, ATSAM3X8EWDT),
		VMSTATE_UINT32(MR, ATSAM3X8EWDT),
		VMSTATE_UINT32(SR, ATSAM3X8EWDT),

		VMSTATE_END_OF_LIST()
	}
};

static void atsam3x8e_wdt_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsam3x8e_wdt_vmstate;
	dc->realize = atsam3x8e_wdt_realize;
	rc->phases.enter = atsam3x8e_wdt_reset_enter;

}

static const TypeInfo atsam3x8e_wdt_info = {
	.name = TYPE_ATSAM3X8E_WDT,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAM3X8EWDT),
	.instance_init = atsam3x8e_wdt_init,
	.class_init = atsam3x8e_wdt_class_init,
};

static void atsam3x8e_wdt_register_types(void) {
	type_register_static(&atsam3x8e_wdt_info);
}

type_init(atsam3x8e_wdt_register_types);

#define TYPE_ATSAM3X8E_EFC0 "atsam3x8e-efc0"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAM3X8EEFC0, ATSAM3X8E_EFC0)

struct ATSAM3X8EEFC0 {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t FMR;
	uint32_t FCR;
	uint32_t FSR;
	uint32_t FRR;
	/* base */
	uint32_t base;

};
#define ATSAM3X8E_EFC0_SIZE				0x10

REG32(EFC0_FMR, 0x0)
	FIELD(EFC0_FMR, FRDY, 0, 1)
	FIELD(EFC0_FMR, FWS, 8, 4)
	FIELD(EFC0_FMR, SCOD, 16, 1)
	FIELD(EFC0_FMR, FAM, 24, 1)
REG32(EFC0_FCR, 0x4)
	FIELD(EFC0_FCR, FCMD, 0, 8)
	FIELD(EFC0_FCR, FARG, 8, 16)
	FIELD(EFC0_FCR, FKEY, 24, 8)
REG32(EFC0_FSR, 0x8)
	FIELD(EFC0_FSR, FRDY, 0, 1)
	FIELD(EFC0_FSR, FCMDE, 1, 1)
	FIELD(EFC0_FSR, FLOCKE, 2, 1)
REG32(EFC0_FRR, 0xc)
	FIELD(EFC0_FRR, FVALUE, 0, 32)


static void atsam3x8e_efc0_register_reset(ATSAM3X8EEFC0 *t) {
	t->FMR = 0x0;
	t->FSR = 0x1;
	t->FRR = 0x0;

}

static uint64_t atsam3x8e_efc0_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAM3X8EEFC0 *t = ATSAM3X8E_EFC0(opaque);
	uint64_t ret;

	switch (offset) {
		case A_EFC0_FMR:
			ret = t->FMR;
			break;
		case A_EFC0_FSR:
			ret = t->FSR;
			break;
		case A_EFC0_FRR:
			ret = t->FRR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM3X8E EFC0 read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsam3x8e_efc0_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAM3X8EEFC0 *t = ATSAM3X8E_EFC0(opaque);


	switch (offset) {
		case A_EFC0_FMR ... A_EFC0_FMR + 3:
			value = value << ((offset - A_EFC0_FMR) << 3);
			offset = A_EFC0_FMR;
			break;
		case A_EFC0_FCR ... A_EFC0_FCR + 3:
			value = value << ((offset - A_EFC0_FCR) << 3);
			offset = A_EFC0_FCR;
			break;

		default: break;
	}

	switch (offset) {
		case A_EFC0_FMR:
			t->FMR = value;
			break;
		case A_EFC0_FCR:
			t->FCR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM3X8E EFC0 write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsam3x8e_efc0_ops = {
	.read = atsam3x8e_efc0_read,
	.write = atsam3x8e_efc0_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsam3x8e_efc0_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAM3X8EEFC0 *t = ATSAM3X8E_EFC0(obj);
	memory_region_init_io(&(t->iomem), obj, &atsam3x8e_efc0_ops, t, "atsam3x8e-efc0", ATSAM3X8E_EFC0_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void atsam3x8e_efc0_realize(DeviceState *dev, Error **errp) {
	return;

}

static void atsam3x8e_efc0_reset_enter(Object *obj, ResetType type) {
	ATSAM3X8EEFC0 *t = ATSAM3X8E_EFC0(obj);
	atsam3x8e_efc0_register_reset(t);
}

static const VMStateDescription atsam3x8e_efc0_vmstate = {
	.name = "atsam3x8e-efc0",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(FMR, ATSAM3X8EEFC0),
		VMSTATE_UINT32(FCR, ATSAM3X8EEFC0),
		VMSTATE_UINT32(FSR, ATSAM3X8EEFC0),
		VMSTATE_UINT32(FRR, ATSAM3X8EEFC0),

		VMSTATE_END_OF_LIST()
	}
};

static void atsam3x8e_efc0_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsam3x8e_efc0_vmstate;
	dc->realize = atsam3x8e_efc0_realize;
	rc->phases.enter = atsam3x8e_efc0_reset_enter;

}

static const TypeInfo atsam3x8e_efc0_info = {
	.name = TYPE_ATSAM3X8E_EFC0,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAM3X8EEFC0),
	.instance_init = atsam3x8e_efc0_init,
	.class_init = atsam3x8e_efc0_class_init,
};

static void atsam3x8e_efc0_register_types(void) {
	type_register_static(&atsam3x8e_efc0_info);
}

type_init(atsam3x8e_efc0_register_types);

#define TYPE_ATSAM3X8E_EFC1 "atsam3x8e-efc1"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAM3X8EEFC1, ATSAM3X8E_EFC1)

struct ATSAM3X8EEFC1 {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t FMR;
	uint32_t FCR;
	uint32_t FSR;
	uint32_t FRR;
	/* base */
	uint32_t base;

};
#define ATSAM3X8E_EFC1_SIZE				0x10

REG32(EFC1_FMR, 0x0)
	FIELD(EFC1_FMR, FRDY, 0, 1)
	FIELD(EFC1_FMR, FWS, 8, 4)
	FIELD(EFC1_FMR, SCOD, 16, 1)
	FIELD(EFC1_FMR, FAM, 24, 1)
REG32(EFC1_FCR, 0x4)
	FIELD(EFC1_FCR, FCMD, 0, 8)
	FIELD(EFC1_FCR, FARG, 8, 16)
	FIELD(EFC1_FCR, FKEY, 24, 8)
REG32(EFC1_FSR, 0x8)
	FIELD(EFC1_FSR, FRDY, 0, 1)
	FIELD(EFC1_FSR, FCMDE, 1, 1)
	FIELD(EFC1_FSR, FLOCKE, 2, 1)
REG32(EFC1_FRR, 0xc)
	FIELD(EFC1_FRR, FVALUE, 0, 32)


static void atsam3x8e_efc1_register_reset(ATSAM3X8EEFC1 *t) {
	t->FMR = 0x0;
	t->FSR = 0x1;
	t->FRR = 0x0;

}

static uint64_t atsam3x8e_efc1_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAM3X8EEFC1 *t = ATSAM3X8E_EFC1(opaque);
	uint64_t ret;

	switch (offset) {
		case A_EFC1_FMR:
			ret = t->FMR;
			break;
		case A_EFC1_FSR:
			ret = t->FSR;
			break;
		case A_EFC1_FRR:
			ret = t->FRR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM3X8E EFC1 read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsam3x8e_efc1_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAM3X8EEFC1 *t = ATSAM3X8E_EFC1(opaque);


	switch (offset) {
		case A_EFC1_FMR ... A_EFC1_FMR + 3:
			value = value << ((offset - A_EFC1_FMR) << 3);
			offset = A_EFC1_FMR;
			break;
		case A_EFC1_FCR ... A_EFC1_FCR + 3:
			value = value << ((offset - A_EFC1_FCR) << 3);
			offset = A_EFC1_FCR;
			break;

		default: break;
	}

	switch (offset) {
		case A_EFC1_FMR:
			t->FMR = value;
			break;
		case A_EFC1_FCR:
			t->FCR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM3X8E EFC1 write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsam3x8e_efc1_ops = {
	.read = atsam3x8e_efc1_read,
	.write = atsam3x8e_efc1_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsam3x8e_efc1_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAM3X8EEFC1 *t = ATSAM3X8E_EFC1(obj);
	memory_region_init_io(&(t->iomem), obj, &atsam3x8e_efc1_ops, t, "atsam3x8e-efc1", ATSAM3X8E_EFC1_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void atsam3x8e_efc1_realize(DeviceState *dev, Error **errp) {
	return;

}

static void atsam3x8e_efc1_reset_enter(Object *obj, ResetType type) {
	ATSAM3X8EEFC1 *t = ATSAM3X8E_EFC1(obj);
	atsam3x8e_efc1_register_reset(t);
}

static const VMStateDescription atsam3x8e_efc1_vmstate = {
	.name = "atsam3x8e-efc1",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(FMR, ATSAM3X8EEFC1),
		VMSTATE_UINT32(FCR, ATSAM3X8EEFC1),
		VMSTATE_UINT32(FSR, ATSAM3X8EEFC1),
		VMSTATE_UINT32(FRR, ATSAM3X8EEFC1),

		VMSTATE_END_OF_LIST()
	}
};

static void atsam3x8e_efc1_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsam3x8e_efc1_vmstate;
	dc->realize = atsam3x8e_efc1_realize;
	rc->phases.enter = atsam3x8e_efc1_reset_enter;

}

static const TypeInfo atsam3x8e_efc1_info = {
	.name = TYPE_ATSAM3X8E_EFC1,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAM3X8EEFC1),
	.instance_init = atsam3x8e_efc1_init,
	.class_init = atsam3x8e_efc1_class_init,
};

static void atsam3x8e_efc1_register_types(void) {
	type_register_static(&atsam3x8e_efc1_info);
}

type_init(atsam3x8e_efc1_register_types);

#define TYPE_ATSAM3X8E_PMC "atsam3x8e-pmc"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAM3X8EPMC, ATSAM3X8E_PMC)

struct ATSAM3X8EPMC {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t PMC_SCER;
	uint32_t PMC_SCDR;
	uint32_t PMC_SCSR;
	uint32_t PMC_PCER0;
	uint32_t PMC_PCDR0;
	uint32_t PMC_PCSR0;
	uint32_t CKGR_UCKR;
	uint32_t CKGR_MOR;
	uint32_t CKGR_MCFR;
	uint32_t CKGR_PLLAR;
	uint32_t PMC_MCKR;
	uint32_t PMC_USB;
	uint32_t PMC_IER;
	uint32_t PMC_IDR;
	uint32_t PMC_SR;
	uint32_t PMC_IMR;
	uint32_t PMC_FSMR;
	uint32_t PMC_FSPR;
	uint32_t PMC_FOCR;
	uint32_t PMC_WPMR;
	uint32_t PMC_WPSR;
	uint32_t PMC_PCER1;
	uint32_t PMC_PCDR1;
	uint32_t PMC_PCSR1;
	uint32_t PMC_PCR;
	uint32_t PMC_PCK0;
	uint32_t PMC_PCK1;
	uint32_t PMC_PCK2;
	/* base */
	uint32_t base;

};
#define ATSAM3X8E_PMC_SIZE				0x110

REG32(PMC_PMC_SCER, 0x0)
	FIELD(PMC_PMC_SCER, UOTGCLK, 5, 1)
	FIELD(PMC_PMC_SCER, PCK0, 8, 1)
	FIELD(PMC_PMC_SCER, PCK1, 9, 1)
	FIELD(PMC_PMC_SCER, PCK2, 10, 1)
REG32(PMC_PMC_SCDR, 0x4)
	FIELD(PMC_PMC_SCDR, UOTGCLK, 5, 1)
	FIELD(PMC_PMC_SCDR, PCK0, 8, 1)
	FIELD(PMC_PMC_SCDR, PCK1, 9, 1)
	FIELD(PMC_PMC_SCDR, PCK2, 10, 1)
REG32(PMC_PMC_SCSR, 0x8)
	FIELD(PMC_PMC_SCSR, UOTGCLK, 5, 1)
	FIELD(PMC_PMC_SCSR, PCK0, 8, 1)
	FIELD(PMC_PMC_SCSR, PCK1, 9, 1)
	FIELD(PMC_PMC_SCSR, PCK2, 10, 1)
REG32(PMC_PMC_PCER0, 0x10)
	FIELD(PMC_PMC_PCER0, PID8, 8, 1)
	FIELD(PMC_PMC_PCER0, PID9, 9, 1)
	FIELD(PMC_PMC_PCER0, PID10, 10, 1)
	FIELD(PMC_PMC_PCER0, PID11, 11, 1)
	FIELD(PMC_PMC_PCER0, PID12, 12, 1)
	FIELD(PMC_PMC_PCER0, PID13, 13, 1)
	FIELD(PMC_PMC_PCER0, PID14, 14, 1)
	FIELD(PMC_PMC_PCER0, PID15, 15, 1)
	FIELD(PMC_PMC_PCER0, PID16, 16, 1)
	FIELD(PMC_PMC_PCER0, PID17, 17, 1)
	FIELD(PMC_PMC_PCER0, PID18, 18, 1)
	FIELD(PMC_PMC_PCER0, PID19, 19, 1)
	FIELD(PMC_PMC_PCER0, PID20, 20, 1)
	FIELD(PMC_PMC_PCER0, PID21, 21, 1)
	FIELD(PMC_PMC_PCER0, PID22, 22, 1)
	FIELD(PMC_PMC_PCER0, PID23, 23, 1)
	FIELD(PMC_PMC_PCER0, PID24, 24, 1)
	FIELD(PMC_PMC_PCER0, PID25, 25, 1)
	FIELD(PMC_PMC_PCER0, PID26, 26, 1)
	FIELD(PMC_PMC_PCER0, PID27, 27, 1)
	FIELD(PMC_PMC_PCER0, PID28, 28, 1)
	FIELD(PMC_PMC_PCER0, PID29, 29, 1)
	FIELD(PMC_PMC_PCER0, PID30, 30, 1)
	FIELD(PMC_PMC_PCER0, PID31, 31, 1)
REG32(PMC_PMC_PCDR0, 0x14)
	FIELD(PMC_PMC_PCDR0, PID8, 8, 1)
	FIELD(PMC_PMC_PCDR0, PID9, 9, 1)
	FIELD(PMC_PMC_PCDR0, PID10, 10, 1)
	FIELD(PMC_PMC_PCDR0, PID11, 11, 1)
	FIELD(PMC_PMC_PCDR0, PID12, 12, 1)
	FIELD(PMC_PMC_PCDR0, PID13, 13, 1)
	FIELD(PMC_PMC_PCDR0, PID14, 14, 1)
	FIELD(PMC_PMC_PCDR0, PID15, 15, 1)
	FIELD(PMC_PMC_PCDR0, PID16, 16, 1)
	FIELD(PMC_PMC_PCDR0, PID17, 17, 1)
	FIELD(PMC_PMC_PCDR0, PID18, 18, 1)
	FIELD(PMC_PMC_PCDR0, PID19, 19, 1)
	FIELD(PMC_PMC_PCDR0, PID20, 20, 1)
	FIELD(PMC_PMC_PCDR0, PID21, 21, 1)
	FIELD(PMC_PMC_PCDR0, PID22, 22, 1)
	FIELD(PMC_PMC_PCDR0, PID23, 23, 1)
	FIELD(PMC_PMC_PCDR0, PID24, 24, 1)
	FIELD(PMC_PMC_PCDR0, PID25, 25, 1)
	FIELD(PMC_PMC_PCDR0, PID26, 26, 1)
	FIELD(PMC_PMC_PCDR0, PID27, 27, 1)
	FIELD(PMC_PMC_PCDR0, PID28, 28, 1)
	FIELD(PMC_PMC_PCDR0, PID29, 29, 1)
	FIELD(PMC_PMC_PCDR0, PID30, 30, 1)
	FIELD(PMC_PMC_PCDR0, PID31, 31, 1)
REG32(PMC_PMC_PCSR0, 0x18)
	FIELD(PMC_PMC_PCSR0, PID8, 8, 1)
	FIELD(PMC_PMC_PCSR0, PID9, 9, 1)
	FIELD(PMC_PMC_PCSR0, PID10, 10, 1)
	FIELD(PMC_PMC_PCSR0, PID11, 11, 1)
	FIELD(PMC_PMC_PCSR0, PID12, 12, 1)
	FIELD(PMC_PMC_PCSR0, PID13, 13, 1)
	FIELD(PMC_PMC_PCSR0, PID14, 14, 1)
	FIELD(PMC_PMC_PCSR0, PID15, 15, 1)
	FIELD(PMC_PMC_PCSR0, PID16, 16, 1)
	FIELD(PMC_PMC_PCSR0, PID17, 17, 1)
	FIELD(PMC_PMC_PCSR0, PID18, 18, 1)
	FIELD(PMC_PMC_PCSR0, PID19, 19, 1)
	FIELD(PMC_PMC_PCSR0, PID20, 20, 1)
	FIELD(PMC_PMC_PCSR0, PID21, 21, 1)
	FIELD(PMC_PMC_PCSR0, PID22, 22, 1)
	FIELD(PMC_PMC_PCSR0, PID23, 23, 1)
	FIELD(PMC_PMC_PCSR0, PID24, 24, 1)
	FIELD(PMC_PMC_PCSR0, PID25, 25, 1)
	FIELD(PMC_PMC_PCSR0, PID26, 26, 1)
	FIELD(PMC_PMC_PCSR0, PID27, 27, 1)
	FIELD(PMC_PMC_PCSR0, PID28, 28, 1)
	FIELD(PMC_PMC_PCSR0, PID29, 29, 1)
	FIELD(PMC_PMC_PCSR0, PID30, 30, 1)
	FIELD(PMC_PMC_PCSR0, PID31, 31, 1)
REG32(PMC_CKGR_UCKR, 0x1c)
	FIELD(PMC_CKGR_UCKR, UPLLEN, 16, 1)
	FIELD(PMC_CKGR_UCKR, UPLLCOUNT, 20, 4)
REG32(PMC_CKGR_MOR, 0x20)
	FIELD(PMC_CKGR_MOR, MOSCXTEN, 0, 1)
	FIELD(PMC_CKGR_MOR, MOSCXTBY, 1, 1)
	FIELD(PMC_CKGR_MOR, MOSCRCEN, 3, 1)
	FIELD(PMC_CKGR_MOR, MOSCRCF, 4, 3)
	FIELD(PMC_CKGR_MOR, MOSCXTST, 8, 8)
	FIELD(PMC_CKGR_MOR, KEY, 16, 8)
	FIELD(PMC_CKGR_MOR, MOSCSEL, 24, 1)
	FIELD(PMC_CKGR_MOR, CFDEN, 25, 1)
REG32(PMC_CKGR_MCFR, 0x24)
	FIELD(PMC_CKGR_MCFR, MAINF, 0, 16)
	FIELD(PMC_CKGR_MCFR, MAINFRDY, 16, 1)
REG32(PMC_CKGR_PLLAR, 0x28)
	FIELD(PMC_CKGR_PLLAR, DIVA, 0, 8)
	FIELD(PMC_CKGR_PLLAR, PLLACOUNT, 8, 6)
	FIELD(PMC_CKGR_PLLAR, MULA, 16, 11)
	FIELD(PMC_CKGR_PLLAR, ONE, 29, 1)
REG32(PMC_PMC_MCKR, 0x30)
	FIELD(PMC_PMC_MCKR, CSS, 0, 2)
	FIELD(PMC_PMC_MCKR, PRES, 4, 3)
	FIELD(PMC_PMC_MCKR, PLLADIV2, 12, 1)
	FIELD(PMC_PMC_MCKR, UPLLDIV2, 13, 1)
REG32(PMC_PMC_USB, 0x38)
	FIELD(PMC_PMC_USB, USBS, 0, 1)
	FIELD(PMC_PMC_USB, USBDIV, 8, 4)
REG32(PMC_PMC_IER, 0x60)
	FIELD(PMC_PMC_IER, MOSCXTS, 0, 1)
	FIELD(PMC_PMC_IER, LOCKA, 1, 1)
	FIELD(PMC_PMC_IER, MCKRDY, 3, 1)
	FIELD(PMC_PMC_IER, LOCKU, 6, 1)
	FIELD(PMC_PMC_IER, PCKRDY0, 8, 1)
	FIELD(PMC_PMC_IER, PCKRDY1, 9, 1)
	FIELD(PMC_PMC_IER, PCKRDY2, 10, 1)
	FIELD(PMC_PMC_IER, MOSCSELS, 16, 1)
	FIELD(PMC_PMC_IER, MOSCRCS, 17, 1)
	FIELD(PMC_PMC_IER, CFDEV, 18, 1)
REG32(PMC_PMC_IDR, 0x64)
	FIELD(PMC_PMC_IDR, MOSCXTS, 0, 1)
	FIELD(PMC_PMC_IDR, LOCKA, 1, 1)
	FIELD(PMC_PMC_IDR, MCKRDY, 3, 1)
	FIELD(PMC_PMC_IDR, LOCKU, 6, 1)
	FIELD(PMC_PMC_IDR, PCKRDY0, 8, 1)
	FIELD(PMC_PMC_IDR, PCKRDY1, 9, 1)
	FIELD(PMC_PMC_IDR, PCKRDY2, 10, 1)
	FIELD(PMC_PMC_IDR, MOSCSELS, 16, 1)
	FIELD(PMC_PMC_IDR, MOSCRCS, 17, 1)
	FIELD(PMC_PMC_IDR, CFDEV, 18, 1)
REG32(PMC_PMC_SR, 0x68)
	FIELD(PMC_PMC_SR, MOSCXTS, 0, 1)
	FIELD(PMC_PMC_SR, LOCKA, 1, 1)
	FIELD(PMC_PMC_SR, MCKRDY, 3, 1)
	FIELD(PMC_PMC_SR, LOCKU, 6, 1)
	FIELD(PMC_PMC_SR, OSCSELS, 7, 1)
	FIELD(PMC_PMC_SR, PCKRDY0, 8, 1)
	FIELD(PMC_PMC_SR, PCKRDY1, 9, 1)
	FIELD(PMC_PMC_SR, PCKRDY2, 10, 1)
	FIELD(PMC_PMC_SR, MOSCSELS, 16, 1)
	FIELD(PMC_PMC_SR, MOSCRCS, 17, 1)
	FIELD(PMC_PMC_SR, CFDEV, 18, 1)
	FIELD(PMC_PMC_SR, CFDS, 19, 1)
	FIELD(PMC_PMC_SR, FOS, 20, 1)
REG32(PMC_PMC_IMR, 0x6c)
	FIELD(PMC_PMC_IMR, MOSCXTS, 0, 1)
	FIELD(PMC_PMC_IMR, LOCKA, 1, 1)
	FIELD(PMC_PMC_IMR, MCKRDY, 3, 1)
	FIELD(PMC_PMC_IMR, LOCKU, 6, 1)
	FIELD(PMC_PMC_IMR, PCKRDY0, 8, 1)
	FIELD(PMC_PMC_IMR, PCKRDY1, 9, 1)
	FIELD(PMC_PMC_IMR, PCKRDY2, 10, 1)
	FIELD(PMC_PMC_IMR, MOSCSELS, 16, 1)
	FIELD(PMC_PMC_IMR, MOSCRCS, 17, 1)
	FIELD(PMC_PMC_IMR, CFDEV, 18, 1)
REG32(PMC_PMC_FSMR, 0x70)
	FIELD(PMC_PMC_FSMR, FSTT0, 0, 1)
	FIELD(PMC_PMC_FSMR, FSTT1, 1, 1)
	FIELD(PMC_PMC_FSMR, FSTT2, 2, 1)
	FIELD(PMC_PMC_FSMR, FSTT3, 3, 1)
	FIELD(PMC_PMC_FSMR, FSTT4, 4, 1)
	FIELD(PMC_PMC_FSMR, FSTT5, 5, 1)
	FIELD(PMC_PMC_FSMR, FSTT6, 6, 1)
	FIELD(PMC_PMC_FSMR, FSTT7, 7, 1)
	FIELD(PMC_PMC_FSMR, FSTT8, 8, 1)
	FIELD(PMC_PMC_FSMR, FSTT9, 9, 1)
	FIELD(PMC_PMC_FSMR, FSTT10, 10, 1)
	FIELD(PMC_PMC_FSMR, FSTT11, 11, 1)
	FIELD(PMC_PMC_FSMR, FSTT12, 12, 1)
	FIELD(PMC_PMC_FSMR, FSTT13, 13, 1)
	FIELD(PMC_PMC_FSMR, FSTT14, 14, 1)
	FIELD(PMC_PMC_FSMR, FSTT15, 15, 1)
	FIELD(PMC_PMC_FSMR, RTTAL, 16, 1)
	FIELD(PMC_PMC_FSMR, RTCAL, 17, 1)
	FIELD(PMC_PMC_FSMR, USBAL, 18, 1)
	FIELD(PMC_PMC_FSMR, LPM, 20, 1)
REG32(PMC_PMC_FSPR, 0x74)
	FIELD(PMC_PMC_FSPR, FSTP0, 0, 1)
	FIELD(PMC_PMC_FSPR, FSTP1, 1, 1)
	FIELD(PMC_PMC_FSPR, FSTP2, 2, 1)
	FIELD(PMC_PMC_FSPR, FSTP3, 3, 1)
	FIELD(PMC_PMC_FSPR, FSTP4, 4, 1)
	FIELD(PMC_PMC_FSPR, FSTP5, 5, 1)
	FIELD(PMC_PMC_FSPR, FSTP6, 6, 1)
	FIELD(PMC_PMC_FSPR, FSTP7, 7, 1)
	FIELD(PMC_PMC_FSPR, FSTP8, 8, 1)
	FIELD(PMC_PMC_FSPR, FSTP9, 9, 1)
	FIELD(PMC_PMC_FSPR, FSTP10, 10, 1)
	FIELD(PMC_PMC_FSPR, FSTP11, 11, 1)
	FIELD(PMC_PMC_FSPR, FSTP12, 12, 1)
	FIELD(PMC_PMC_FSPR, FSTP13, 13, 1)
	FIELD(PMC_PMC_FSPR, FSTP14, 14, 1)
	FIELD(PMC_PMC_FSPR, FSTP15, 15, 1)
REG32(PMC_PMC_FOCR, 0x78)
	FIELD(PMC_PMC_FOCR, FOCLR, 0, 1)
REG32(PMC_PMC_WPMR, 0xe4)
	FIELD(PMC_PMC_WPMR, WPEN, 0, 1)
	FIELD(PMC_PMC_WPMR, WPKEY, 8, 24)
REG32(PMC_PMC_WPSR, 0xe8)
	FIELD(PMC_PMC_WPSR, WPVS, 0, 1)
	FIELD(PMC_PMC_WPSR, WPVSRC, 8, 16)
REG32(PMC_PMC_PCER1, 0x100)
	FIELD(PMC_PMC_PCER1, PID32, 0, 1)
	FIELD(PMC_PMC_PCER1, PID33, 1, 1)
	FIELD(PMC_PMC_PCER1, PID34, 2, 1)
	FIELD(PMC_PMC_PCER1, PID35, 3, 1)
	FIELD(PMC_PMC_PCER1, PID36, 4, 1)
	FIELD(PMC_PMC_PCER1, PID37, 5, 1)
	FIELD(PMC_PMC_PCER1, PID38, 6, 1)
	FIELD(PMC_PMC_PCER1, PID39, 7, 1)
	FIELD(PMC_PMC_PCER1, PID40, 8, 1)
	FIELD(PMC_PMC_PCER1, PID41, 9, 1)
	FIELD(PMC_PMC_PCER1, PID42, 10, 1)
	FIELD(PMC_PMC_PCER1, PID43, 11, 1)
	FIELD(PMC_PMC_PCER1, PID44, 12, 1)
REG32(PMC_PMC_PCDR1, 0x104)
	FIELD(PMC_PMC_PCDR1, PID32, 0, 1)
	FIELD(PMC_PMC_PCDR1, PID33, 1, 1)
	FIELD(PMC_PMC_PCDR1, PID34, 2, 1)
	FIELD(PMC_PMC_PCDR1, PID35, 3, 1)
	FIELD(PMC_PMC_PCDR1, PID36, 4, 1)
	FIELD(PMC_PMC_PCDR1, PID37, 5, 1)
	FIELD(PMC_PMC_PCDR1, PID38, 6, 1)
	FIELD(PMC_PMC_PCDR1, PID39, 7, 1)
	FIELD(PMC_PMC_PCDR1, PID40, 8, 1)
	FIELD(PMC_PMC_PCDR1, PID41, 9, 1)
	FIELD(PMC_PMC_PCDR1, PID42, 10, 1)
	FIELD(PMC_PMC_PCDR1, PID43, 11, 1)
	FIELD(PMC_PMC_PCDR1, PID44, 12, 1)
REG32(PMC_PMC_PCSR1, 0x108)
	FIELD(PMC_PMC_PCSR1, PID32, 0, 1)
	FIELD(PMC_PMC_PCSR1, PID33, 1, 1)
	FIELD(PMC_PMC_PCSR1, PID34, 2, 1)
	FIELD(PMC_PMC_PCSR1, PID35, 3, 1)
	FIELD(PMC_PMC_PCSR1, PID36, 4, 1)
	FIELD(PMC_PMC_PCSR1, PID37, 5, 1)
	FIELD(PMC_PMC_PCSR1, PID38, 6, 1)
	FIELD(PMC_PMC_PCSR1, PID39, 7, 1)
	FIELD(PMC_PMC_PCSR1, PID40, 8, 1)
	FIELD(PMC_PMC_PCSR1, PID41, 9, 1)
	FIELD(PMC_PMC_PCSR1, PID42, 10, 1)
	FIELD(PMC_PMC_PCSR1, PID43, 11, 1)
	FIELD(PMC_PMC_PCSR1, PID44, 12, 1)
REG32(PMC_PMC_PCR, 0x10c)
	FIELD(PMC_PMC_PCR, PID, 0, 6)
	FIELD(PMC_PMC_PCR, CMD, 12, 1)
	FIELD(PMC_PMC_PCR, DIV, 16, 2)
	FIELD(PMC_PMC_PCR, EN, 28, 1)
REG32(PMC_PMC_PCK0, 0x40)
	FIELD(PMC_PMC_PCK0, CSS, 0, 3)
	FIELD(PMC_PMC_PCK0, PRES, 4, 3)
REG32(PMC_PMC_PCK1, 0x44)
	FIELD(PMC_PMC_PCK1, CSS, 0, 3)
	FIELD(PMC_PMC_PCK1, PRES, 4, 3)
REG32(PMC_PMC_PCK2, 0x48)
	FIELD(PMC_PMC_PCK2, CSS, 0, 3)
	FIELD(PMC_PMC_PCK2, PRES, 4, 3)


static void atsam3x8e_pmc_register_reset(ATSAM3X8EPMC *t) {
	t->PMC_SCSR = 0x1;
	t->PMC_PCSR0 = 0x0;
	t->CKGR_UCKR = 0x10200800;
	t->CKGR_MOR = 0x8;
	t->CKGR_MCFR = 0x0;
	t->CKGR_PLLAR = 0x3f00;
	t->PMC_MCKR = 0x1;
	t->PMC_USB = 0x0;
	t->PMC_SR = 0x10008;
	t->PMC_IMR = 0x0;
	t->PMC_FSMR = 0x0;
	t->PMC_FSPR = 0x0;
	t->PMC_WPMR = 0x0;
	t->PMC_WPSR = 0x0;
	t->PMC_PCSR1 = 0x0;
	t->PMC_PCR = 0x0;

}

static uint64_t atsam3x8e_pmc_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAM3X8EPMC *t = ATSAM3X8E_PMC(opaque);
	uint64_t ret;

	switch (offset) {
		case A_PMC_PMC_SCSR:
			ret = t->PMC_SCSR;
			break;
		case A_PMC_PMC_PCSR0:
			ret = t->PMC_PCSR0;
			break;
		case A_PMC_CKGR_UCKR:
			ret = t->CKGR_UCKR;
			break;
		case A_PMC_CKGR_MOR:
			ret = t->CKGR_MOR;
			break;
		case A_PMC_CKGR_MCFR:
			ret = t->CKGR_MCFR;
			break;
		case A_PMC_CKGR_PLLAR:
			ret = t->CKGR_PLLAR;
			break;
		case A_PMC_PMC_MCKR:
			ret = t->PMC_MCKR;
			break;
		case A_PMC_PMC_USB:
			ret = t->PMC_USB;
			break;
		case A_PMC_PMC_SR:
			ret = t->PMC_SR;
			break;
		case A_PMC_PMC_IMR:
			ret = t->PMC_IMR;
			break;
		case A_PMC_PMC_FSMR:
			ret = t->PMC_FSMR;
			break;
		case A_PMC_PMC_FSPR:
			ret = t->PMC_FSPR;
			break;
		case A_PMC_PMC_WPMR:
			ret = t->PMC_WPMR;
			break;
		case A_PMC_PMC_WPSR:
			ret = t->PMC_WPSR;
			break;
		case A_PMC_PMC_PCSR1:
			ret = t->PMC_PCSR1;
			break;
		case A_PMC_PMC_PCR:
			ret = t->PMC_PCR;
			break;
		case A_PMC_PMC_PCK0:
			ret = t->PMC_PCK0;
			break;
		case A_PMC_PMC_PCK1:
			ret = t->PMC_PCK1;
			break;
		case A_PMC_PMC_PCK2:
			ret = t->PMC_PCK2;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM3X8E PMC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsam3x8e_pmc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAM3X8EPMC *t = ATSAM3X8E_PMC(opaque);


	switch (offset) {
		case A_PMC_PMC_SCER ... A_PMC_PMC_SCER + 3:
			value = value << ((offset - A_PMC_PMC_SCER) << 3);
			offset = A_PMC_PMC_SCER;
			break;
		case A_PMC_PMC_SCDR ... A_PMC_PMC_SCDR + 3:
			value = value << ((offset - A_PMC_PMC_SCDR) << 3);
			offset = A_PMC_PMC_SCDR;
			break;
		case A_PMC_PMC_PCER0 ... A_PMC_PMC_PCER0 + 3:
			value = value << ((offset - A_PMC_PMC_PCER0) << 3);
			offset = A_PMC_PMC_PCER0;
			break;
		case A_PMC_PMC_PCDR0 ... A_PMC_PMC_PCDR0 + 3:
			value = value << ((offset - A_PMC_PMC_PCDR0) << 3);
			offset = A_PMC_PMC_PCDR0;
			break;
		case A_PMC_CKGR_UCKR ... A_PMC_CKGR_UCKR + 3:
			value = value << ((offset - A_PMC_CKGR_UCKR) << 3);
			offset = A_PMC_CKGR_UCKR;
			break;
		case A_PMC_CKGR_MOR ... A_PMC_CKGR_MOR + 3:
			value = value << ((offset - A_PMC_CKGR_MOR) << 3);
			offset = A_PMC_CKGR_MOR;
			break;
		case A_PMC_CKGR_PLLAR ... A_PMC_CKGR_PLLAR + 3:
			value = value << ((offset - A_PMC_CKGR_PLLAR) << 3);
			offset = A_PMC_CKGR_PLLAR;
			break;
		case A_PMC_PMC_MCKR ... A_PMC_PMC_MCKR + 3:
			value = value << ((offset - A_PMC_PMC_MCKR) << 3);
			offset = A_PMC_PMC_MCKR;
			break;
		case A_PMC_PMC_USB ... A_PMC_PMC_USB + 3:
			value = value << ((offset - A_PMC_PMC_USB) << 3);
			offset = A_PMC_PMC_USB;
			break;
		case A_PMC_PMC_IER ... A_PMC_PMC_IER + 3:
			value = value << ((offset - A_PMC_PMC_IER) << 3);
			offset = A_PMC_PMC_IER;
			break;
		case A_PMC_PMC_IDR ... A_PMC_PMC_IDR + 3:
			value = value << ((offset - A_PMC_PMC_IDR) << 3);
			offset = A_PMC_PMC_IDR;
			break;
		case A_PMC_PMC_FSMR ... A_PMC_PMC_FSMR + 3:
			value = value << ((offset - A_PMC_PMC_FSMR) << 3);
			offset = A_PMC_PMC_FSMR;
			break;
		case A_PMC_PMC_FSPR ... A_PMC_PMC_FSPR + 3:
			value = value << ((offset - A_PMC_PMC_FSPR) << 3);
			offset = A_PMC_PMC_FSPR;
			break;
		case A_PMC_PMC_FOCR ... A_PMC_PMC_FOCR + 3:
			value = value << ((offset - A_PMC_PMC_FOCR) << 3);
			offset = A_PMC_PMC_FOCR;
			break;
		case A_PMC_PMC_WPMR ... A_PMC_PMC_WPMR + 3:
			value = value << ((offset - A_PMC_PMC_WPMR) << 3);
			offset = A_PMC_PMC_WPMR;
			break;
		case A_PMC_PMC_PCER1 ... A_PMC_PMC_PCER1 + 3:
			value = value << ((offset - A_PMC_PMC_PCER1) << 3);
			offset = A_PMC_PMC_PCER1;
			break;
		case A_PMC_PMC_PCDR1 ... A_PMC_PMC_PCDR1 + 3:
			value = value << ((offset - A_PMC_PMC_PCDR1) << 3);
			offset = A_PMC_PMC_PCDR1;
			break;
		case A_PMC_PMC_PCR ... A_PMC_PMC_PCR + 3:
			value = value << ((offset - A_PMC_PMC_PCR) << 3);
			offset = A_PMC_PMC_PCR;
			break;
		case A_PMC_PMC_PCK0 ... A_PMC_PMC_PCK0 + 3:
			value = value << ((offset - A_PMC_PMC_PCK0) << 3);
			offset = A_PMC_PMC_PCK0;
			break;
		case A_PMC_PMC_PCK1 ... A_PMC_PMC_PCK1 + 3:
			value = value << ((offset - A_PMC_PMC_PCK1) << 3);
			offset = A_PMC_PMC_PCK1;
			break;
		case A_PMC_PMC_PCK2 ... A_PMC_PMC_PCK2 + 3:
			value = value << ((offset - A_PMC_PMC_PCK2) << 3);
			offset = A_PMC_PMC_PCK2;
			break;

		default: break;
	}

	switch (offset) {
		case A_PMC_PMC_SCER:
			t->PMC_SCER = value;
			break;
		case A_PMC_PMC_SCDR:
			t->PMC_SCDR = value;
			break;
		case A_PMC_PMC_PCER0:
			t->PMC_PCER0 = value;
			break;
		case A_PMC_PMC_PCDR0:
			t->PMC_PCDR0 = value;
			break;
		case A_PMC_CKGR_UCKR:
			t->CKGR_UCKR = value;
			if ((value == 0x310000)) {
				t->PMC_SR |= 0x40;
			}
			if (((!(value & 0x1)) && (!(value & 0x2)) && (!(value & 0x4)) && (!(value & 0x8)) && (!(value & 0x10)) && (!(value & 0x20)) && (!(value & 0x40)) && (!(value & 0x80)) && (!(value & 0x100)) && (!(value & 0x200)) && (!(value & 0x400)) && (!(value & 0x800)) && (!(value & 0x1000)) && (!(value & 0x2000)) && (!(value & 0x4000)) && (!(value & 0x8000)) && (value & 0x10000) && (!(value & 0x20000)) && (!(value & 0x40000)) && (!(value & 0x80000)) && (!(value & 0x1000000)) && (!(value & 0x2000000)) && (!(value & 0x4000000)) && (!(value & 0x8000000)) && (!(value & 0x10000000)) && (!(value & 0x20000000)) && (!(value & 0x40000000)) && (!(value & 0x80000000)))) {
				t->PMC_SR |= 0x40;
			}
			break;
		case A_PMC_CKGR_MOR:
			t->CKGR_MOR = value;
			if (((value & 0x1) && (!(value & 0x2)) && (value & 0x10000) && (value & 0x20000) && (value & 0x40000) && (value & 0x100000) && (value & 0x200000))) {
				t->PMC_SR |= 0x1;
			} else if (((value & 0x8) && (value & 0x10000) && (value & 0x20000) && (value & 0x40000) && (value & 0x100000) && (value & 0x200000))) {
				t->PMC_SR |= 0x20000;
			} else if (((value & 0x10000) && (value & 0x20000) && (value & 0x40000) && (value & 0x100000) && (value & 0x200000))) {
				t->PMC_SR |= 0x20000;
			}
			if ((value == 0x137ff09)) {
				t->PMC_SR |= 0x10000;
			}
			break;
		case A_PMC_CKGR_PLLAR:
			t->CKGR_PLLAR = value;
			t->PMC_SR |= 0x2;
			break;
		case A_PMC_PMC_MCKR:
			t->PMC_MCKR = value;
			if (((!(value & 0x1)) && (!(value & 0x2)))) {
				t->PMC_SR |= 0x8;
			} else if (((value & 0x1) && (value & 0x2))) {
				t->PMC_SR |= 0x8;
			} else if (((value & 0x1) && (!(value & 0x2)))) {
				t->PMC_SR |= 0x8;
			} else if (((!(value & 0x1)) && (value & 0x2))) {
				t->PMC_SR |= 0x8;
			}
			t->PMC_SR |= 0x8;
			break;
		case A_PMC_PMC_USB:
			t->PMC_USB = value;
			break;
		case A_PMC_PMC_IER:
			t->PMC_IER = value;
			break;
		case A_PMC_PMC_IDR:
			t->PMC_IDR = value;
			break;
		case A_PMC_PMC_FSMR:
			t->PMC_FSMR = value;
			break;
		case A_PMC_PMC_FSPR:
			t->PMC_FSPR = value;
			break;
		case A_PMC_PMC_FOCR:
			t->PMC_FOCR = value;
			break;
		case A_PMC_PMC_WPMR:
			t->PMC_WPMR = value;
			break;
		case A_PMC_PMC_PCER1:
			t->PMC_PCER1 = value;
			break;
		case A_PMC_PMC_PCDR1:
			t->PMC_PCDR1 = value;
			break;
		case A_PMC_PMC_PCR:
			t->PMC_PCR = value;
			break;
		case A_PMC_PMC_PCK0:
			t->PMC_PCK0 = value;
			if (((value & 0x1) && (value & 0x2))) {
				t->PMC_SR |= 0x100;
			} else if ((value & 0x1)) {
				t->PMC_SR |= 0x100;
			} else if ((value & 0x2)) {
				t->PMC_SR |= 0x100;
			}
			if ((value & 0x4)) {
				t->PMC_SR |= 0x100;
			}
			break;
		case A_PMC_PMC_PCK1:
			t->PMC_PCK1 = value;
			if (((value & 0x1) && (value & 0x2))) {
				t->PMC_SR |= 0x200;
			} else if ((value & 0x1)) {
				t->PMC_SR |= 0x200;
			} else if ((value & 0x2)) {
				t->PMC_SR |= 0x200;
			}
			if ((value & 0x4)) {
				t->PMC_SR |= 0x200;
			}
			break;
		case A_PMC_PMC_PCK2:
			t->PMC_PCK2 = value;
			if (((value & 0x1) && (value & 0x2))) {
				t->PMC_SR |= 0x400;
			} else if ((value & 0x1)) {
				t->PMC_SR |= 0x400;
			} else if ((value & 0x2)) {
				t->PMC_SR |= 0x400;
			}
			if ((value & 0x4)) {
				t->PMC_SR |= 0x400;
			}
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM3X8E PMC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsam3x8e_pmc_ops = {
	.read = atsam3x8e_pmc_read,
	.write = atsam3x8e_pmc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsam3x8e_pmc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAM3X8EPMC *t = ATSAM3X8E_PMC(obj);
	memory_region_init_io(&(t->iomem), obj, &atsam3x8e_pmc_ops, t, "atsam3x8e-pmc", ATSAM3X8E_PMC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void atsam3x8e_pmc_realize(DeviceState *dev, Error **errp) {
	return;

}

static void atsam3x8e_pmc_reset_enter(Object *obj, ResetType type) {
	ATSAM3X8EPMC *t = ATSAM3X8E_PMC(obj);
	atsam3x8e_pmc_register_reset(t);
}

static const VMStateDescription atsam3x8e_pmc_vmstate = {
	.name = "atsam3x8e-pmc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(PMC_SCER, ATSAM3X8EPMC),
		VMSTATE_UINT32(PMC_SCDR, ATSAM3X8EPMC),
		VMSTATE_UINT32(PMC_SCSR, ATSAM3X8EPMC),
		VMSTATE_UINT32(PMC_PCER0, ATSAM3X8EPMC),
		VMSTATE_UINT32(PMC_PCDR0, ATSAM3X8EPMC),
		VMSTATE_UINT32(PMC_PCSR0, ATSAM3X8EPMC),
		VMSTATE_UINT32(CKGR_UCKR, ATSAM3X8EPMC),
		VMSTATE_UINT32(CKGR_MOR, ATSAM3X8EPMC),
		VMSTATE_UINT32(CKGR_MCFR, ATSAM3X8EPMC),
		VMSTATE_UINT32(CKGR_PLLAR, ATSAM3X8EPMC),
		VMSTATE_UINT32(PMC_MCKR, ATSAM3X8EPMC),
		VMSTATE_UINT32(PMC_USB, ATSAM3X8EPMC),
		VMSTATE_UINT32(PMC_IER, ATSAM3X8EPMC),
		VMSTATE_UINT32(PMC_IDR, ATSAM3X8EPMC),
		VMSTATE_UINT32(PMC_SR, ATSAM3X8EPMC),
		VMSTATE_UINT32(PMC_IMR, ATSAM3X8EPMC),
		VMSTATE_UINT32(PMC_FSMR, ATSAM3X8EPMC),
		VMSTATE_UINT32(PMC_FSPR, ATSAM3X8EPMC),
		VMSTATE_UINT32(PMC_FOCR, ATSAM3X8EPMC),
		VMSTATE_UINT32(PMC_WPMR, ATSAM3X8EPMC),
		VMSTATE_UINT32(PMC_WPSR, ATSAM3X8EPMC),
		VMSTATE_UINT32(PMC_PCER1, ATSAM3X8EPMC),
		VMSTATE_UINT32(PMC_PCDR1, ATSAM3X8EPMC),
		VMSTATE_UINT32(PMC_PCSR1, ATSAM3X8EPMC),
		VMSTATE_UINT32(PMC_PCR, ATSAM3X8EPMC),
		VMSTATE_UINT32(PMC_PCK0, ATSAM3X8EPMC),
		VMSTATE_UINT32(PMC_PCK1, ATSAM3X8EPMC),
		VMSTATE_UINT32(PMC_PCK2, ATSAM3X8EPMC),

		VMSTATE_END_OF_LIST()
	}
};

static void atsam3x8e_pmc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsam3x8e_pmc_vmstate;
	dc->realize = atsam3x8e_pmc_realize;
	rc->phases.enter = atsam3x8e_pmc_reset_enter;

}

static const TypeInfo atsam3x8e_pmc_info = {
	.name = TYPE_ATSAM3X8E_PMC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAM3X8EPMC),
	.instance_init = atsam3x8e_pmc_init,
	.class_init = atsam3x8e_pmc_class_init,
};

static void atsam3x8e_pmc_register_types(void) {
	type_register_static(&atsam3x8e_pmc_info);
}

type_init(atsam3x8e_pmc_register_types);

#define TYPE_ATSAM3X8E_PIO "atsam3x8e-pio"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAM3X8EPIO, ATSAM3X8E_PIO)

struct ATSAM3X8EPIO {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t PER;
	uint32_t PDR;
	uint32_t PSR;
	uint32_t OER;
	uint32_t ODR;
	uint32_t OSR;
	uint32_t IFER;
	uint32_t IFDR;
	uint32_t IFSR;
	uint32_t SODR;
	uint32_t CODR;
	uint32_t ODSR;
	uint32_t PDSR;
	uint32_t IER;
	uint32_t IDR;
	uint32_t IMR;
	uint32_t ISR;
	uint32_t MDER;
	uint32_t MDDR;
	uint32_t MDSR;
	uint32_t PUDR;
	uint32_t PUER;
	uint32_t PUSR;
	uint32_t ABSR;
	uint32_t SCIFSR;
	uint32_t DIFSR;
	uint32_t IFDGSR;
	uint32_t SCDR;
	uint32_t OWER;
	uint32_t OWDR;
	uint32_t OWSR;
	uint32_t AIMER;
	uint32_t AIMDR;
	uint32_t AIMMR;
	uint32_t ESR;
	uint32_t LSR;
	uint32_t ELSR;
	uint32_t FELLSR;
	uint32_t REHLSR;
	uint32_t FRLHSR;
	uint32_t LOCKSR;
	uint32_t WPMR;
	uint32_t WPSR;
	/* chardev backend */
	CharBackend chr;
	/* base */
	uint32_t base;

};
#define ATSAM3X8E_PIO_SIZE				0xec

REG32(PIO_PER, 0x0)
	FIELD(PIO_PER, P0, 0, 1)
	FIELD(PIO_PER, P1, 1, 1)
	FIELD(PIO_PER, P2, 2, 1)
	FIELD(PIO_PER, P3, 3, 1)
	FIELD(PIO_PER, P4, 4, 1)
	FIELD(PIO_PER, P5, 5, 1)
	FIELD(PIO_PER, P6, 6, 1)
	FIELD(PIO_PER, P7, 7, 1)
	FIELD(PIO_PER, P8, 8, 1)
	FIELD(PIO_PER, P9, 9, 1)
	FIELD(PIO_PER, P10, 10, 1)
	FIELD(PIO_PER, P11, 11, 1)
	FIELD(PIO_PER, P12, 12, 1)
	FIELD(PIO_PER, P13, 13, 1)
	FIELD(PIO_PER, P14, 14, 1)
	FIELD(PIO_PER, P15, 15, 1)
	FIELD(PIO_PER, P16, 16, 1)
	FIELD(PIO_PER, P17, 17, 1)
	FIELD(PIO_PER, P18, 18, 1)
	FIELD(PIO_PER, P19, 19, 1)
	FIELD(PIO_PER, P20, 20, 1)
	FIELD(PIO_PER, P21, 21, 1)
	FIELD(PIO_PER, P22, 22, 1)
	FIELD(PIO_PER, P23, 23, 1)
	FIELD(PIO_PER, P24, 24, 1)
	FIELD(PIO_PER, P25, 25, 1)
	FIELD(PIO_PER, P26, 26, 1)
	FIELD(PIO_PER, P27, 27, 1)
	FIELD(PIO_PER, P28, 28, 1)
	FIELD(PIO_PER, P29, 29, 1)
	FIELD(PIO_PER, P30, 30, 1)
	FIELD(PIO_PER, P31, 31, 1)
REG32(PIO_PDR, 0x4)
	FIELD(PIO_PDR, P0, 0, 1)
	FIELD(PIO_PDR, P1, 1, 1)
	FIELD(PIO_PDR, P2, 2, 1)
	FIELD(PIO_PDR, P3, 3, 1)
	FIELD(PIO_PDR, P4, 4, 1)
	FIELD(PIO_PDR, P5, 5, 1)
	FIELD(PIO_PDR, P6, 6, 1)
	FIELD(PIO_PDR, P7, 7, 1)
	FIELD(PIO_PDR, P8, 8, 1)
	FIELD(PIO_PDR, P9, 9, 1)
	FIELD(PIO_PDR, P10, 10, 1)
	FIELD(PIO_PDR, P11, 11, 1)
	FIELD(PIO_PDR, P12, 12, 1)
	FIELD(PIO_PDR, P13, 13, 1)
	FIELD(PIO_PDR, P14, 14, 1)
	FIELD(PIO_PDR, P15, 15, 1)
	FIELD(PIO_PDR, P16, 16, 1)
	FIELD(PIO_PDR, P17, 17, 1)
	FIELD(PIO_PDR, P18, 18, 1)
	FIELD(PIO_PDR, P19, 19, 1)
	FIELD(PIO_PDR, P20, 20, 1)
	FIELD(PIO_PDR, P21, 21, 1)
	FIELD(PIO_PDR, P22, 22, 1)
	FIELD(PIO_PDR, P23, 23, 1)
	FIELD(PIO_PDR, P24, 24, 1)
	FIELD(PIO_PDR, P25, 25, 1)
	FIELD(PIO_PDR, P26, 26, 1)
	FIELD(PIO_PDR, P27, 27, 1)
	FIELD(PIO_PDR, P28, 28, 1)
	FIELD(PIO_PDR, P29, 29, 1)
	FIELD(PIO_PDR, P30, 30, 1)
	FIELD(PIO_PDR, P31, 31, 1)
REG32(PIO_PSR, 0x8)
	FIELD(PIO_PSR, P0, 0, 1)
	FIELD(PIO_PSR, P1, 1, 1)
	FIELD(PIO_PSR, P2, 2, 1)
	FIELD(PIO_PSR, P3, 3, 1)
	FIELD(PIO_PSR, P4, 4, 1)
	FIELD(PIO_PSR, P5, 5, 1)
	FIELD(PIO_PSR, P6, 6, 1)
	FIELD(PIO_PSR, P7, 7, 1)
	FIELD(PIO_PSR, P8, 8, 1)
	FIELD(PIO_PSR, P9, 9, 1)
	FIELD(PIO_PSR, P10, 10, 1)
	FIELD(PIO_PSR, P11, 11, 1)
	FIELD(PIO_PSR, P12, 12, 1)
	FIELD(PIO_PSR, P13, 13, 1)
	FIELD(PIO_PSR, P14, 14, 1)
	FIELD(PIO_PSR, P15, 15, 1)
	FIELD(PIO_PSR, P16, 16, 1)
	FIELD(PIO_PSR, P17, 17, 1)
	FIELD(PIO_PSR, P18, 18, 1)
	FIELD(PIO_PSR, P19, 19, 1)
	FIELD(PIO_PSR, P20, 20, 1)
	FIELD(PIO_PSR, P21, 21, 1)
	FIELD(PIO_PSR, P22, 22, 1)
	FIELD(PIO_PSR, P23, 23, 1)
	FIELD(PIO_PSR, P24, 24, 1)
	FIELD(PIO_PSR, P25, 25, 1)
	FIELD(PIO_PSR, P26, 26, 1)
	FIELD(PIO_PSR, P27, 27, 1)
	FIELD(PIO_PSR, P28, 28, 1)
	FIELD(PIO_PSR, P29, 29, 1)
	FIELD(PIO_PSR, P30, 30, 1)
	FIELD(PIO_PSR, P31, 31, 1)
REG32(PIO_OER, 0x10)
	FIELD(PIO_OER, P0, 0, 1)
	FIELD(PIO_OER, P1, 1, 1)
	FIELD(PIO_OER, P2, 2, 1)
	FIELD(PIO_OER, P3, 3, 1)
	FIELD(PIO_OER, P4, 4, 1)
	FIELD(PIO_OER, P5, 5, 1)
	FIELD(PIO_OER, P6, 6, 1)
	FIELD(PIO_OER, P7, 7, 1)
	FIELD(PIO_OER, P8, 8, 1)
	FIELD(PIO_OER, P9, 9, 1)
	FIELD(PIO_OER, P10, 10, 1)
	FIELD(PIO_OER, P11, 11, 1)
	FIELD(PIO_OER, P12, 12, 1)
	FIELD(PIO_OER, P13, 13, 1)
	FIELD(PIO_OER, P14, 14, 1)
	FIELD(PIO_OER, P15, 15, 1)
	FIELD(PIO_OER, P16, 16, 1)
	FIELD(PIO_OER, P17, 17, 1)
	FIELD(PIO_OER, P18, 18, 1)
	FIELD(PIO_OER, P19, 19, 1)
	FIELD(PIO_OER, P20, 20, 1)
	FIELD(PIO_OER, P21, 21, 1)
	FIELD(PIO_OER, P22, 22, 1)
	FIELD(PIO_OER, P23, 23, 1)
	FIELD(PIO_OER, P24, 24, 1)
	FIELD(PIO_OER, P25, 25, 1)
	FIELD(PIO_OER, P26, 26, 1)
	FIELD(PIO_OER, P27, 27, 1)
	FIELD(PIO_OER, P28, 28, 1)
	FIELD(PIO_OER, P29, 29, 1)
	FIELD(PIO_OER, P30, 30, 1)
	FIELD(PIO_OER, P31, 31, 1)
REG32(PIO_ODR, 0x14)
	FIELD(PIO_ODR, P0, 0, 1)
	FIELD(PIO_ODR, P1, 1, 1)
	FIELD(PIO_ODR, P2, 2, 1)
	FIELD(PIO_ODR, P3, 3, 1)
	FIELD(PIO_ODR, P4, 4, 1)
	FIELD(PIO_ODR, P5, 5, 1)
	FIELD(PIO_ODR, P6, 6, 1)
	FIELD(PIO_ODR, P7, 7, 1)
	FIELD(PIO_ODR, P8, 8, 1)
	FIELD(PIO_ODR, P9, 9, 1)
	FIELD(PIO_ODR, P10, 10, 1)
	FIELD(PIO_ODR, P11, 11, 1)
	FIELD(PIO_ODR, P12, 12, 1)
	FIELD(PIO_ODR, P13, 13, 1)
	FIELD(PIO_ODR, P14, 14, 1)
	FIELD(PIO_ODR, P15, 15, 1)
	FIELD(PIO_ODR, P16, 16, 1)
	FIELD(PIO_ODR, P17, 17, 1)
	FIELD(PIO_ODR, P18, 18, 1)
	FIELD(PIO_ODR, P19, 19, 1)
	FIELD(PIO_ODR, P20, 20, 1)
	FIELD(PIO_ODR, P21, 21, 1)
	FIELD(PIO_ODR, P22, 22, 1)
	FIELD(PIO_ODR, P23, 23, 1)
	FIELD(PIO_ODR, P24, 24, 1)
	FIELD(PIO_ODR, P25, 25, 1)
	FIELD(PIO_ODR, P26, 26, 1)
	FIELD(PIO_ODR, P27, 27, 1)
	FIELD(PIO_ODR, P28, 28, 1)
	FIELD(PIO_ODR, P29, 29, 1)
	FIELD(PIO_ODR, P30, 30, 1)
	FIELD(PIO_ODR, P31, 31, 1)
REG32(PIO_OSR, 0x18)
	FIELD(PIO_OSR, P0, 0, 1)
	FIELD(PIO_OSR, P1, 1, 1)
	FIELD(PIO_OSR, P2, 2, 1)
	FIELD(PIO_OSR, P3, 3, 1)
	FIELD(PIO_OSR, P4, 4, 1)
	FIELD(PIO_OSR, P5, 5, 1)
	FIELD(PIO_OSR, P6, 6, 1)
	FIELD(PIO_OSR, P7, 7, 1)
	FIELD(PIO_OSR, P8, 8, 1)
	FIELD(PIO_OSR, P9, 9, 1)
	FIELD(PIO_OSR, P10, 10, 1)
	FIELD(PIO_OSR, P11, 11, 1)
	FIELD(PIO_OSR, P12, 12, 1)
	FIELD(PIO_OSR, P13, 13, 1)
	FIELD(PIO_OSR, P14, 14, 1)
	FIELD(PIO_OSR, P15, 15, 1)
	FIELD(PIO_OSR, P16, 16, 1)
	FIELD(PIO_OSR, P17, 17, 1)
	FIELD(PIO_OSR, P18, 18, 1)
	FIELD(PIO_OSR, P19, 19, 1)
	FIELD(PIO_OSR, P20, 20, 1)
	FIELD(PIO_OSR, P21, 21, 1)
	FIELD(PIO_OSR, P22, 22, 1)
	FIELD(PIO_OSR, P23, 23, 1)
	FIELD(PIO_OSR, P24, 24, 1)
	FIELD(PIO_OSR, P25, 25, 1)
	FIELD(PIO_OSR, P26, 26, 1)
	FIELD(PIO_OSR, P27, 27, 1)
	FIELD(PIO_OSR, P28, 28, 1)
	FIELD(PIO_OSR, P29, 29, 1)
	FIELD(PIO_OSR, P30, 30, 1)
	FIELD(PIO_OSR, P31, 31, 1)
REG32(PIO_IFER, 0x20)
	FIELD(PIO_IFER, P0, 0, 1)
	FIELD(PIO_IFER, P1, 1, 1)
	FIELD(PIO_IFER, P2, 2, 1)
	FIELD(PIO_IFER, P3, 3, 1)
	FIELD(PIO_IFER, P4, 4, 1)
	FIELD(PIO_IFER, P5, 5, 1)
	FIELD(PIO_IFER, P6, 6, 1)
	FIELD(PIO_IFER, P7, 7, 1)
	FIELD(PIO_IFER, P8, 8, 1)
	FIELD(PIO_IFER, P9, 9, 1)
	FIELD(PIO_IFER, P10, 10, 1)
	FIELD(PIO_IFER, P11, 11, 1)
	FIELD(PIO_IFER, P12, 12, 1)
	FIELD(PIO_IFER, P13, 13, 1)
	FIELD(PIO_IFER, P14, 14, 1)
	FIELD(PIO_IFER, P15, 15, 1)
	FIELD(PIO_IFER, P16, 16, 1)
	FIELD(PIO_IFER, P17, 17, 1)
	FIELD(PIO_IFER, P18, 18, 1)
	FIELD(PIO_IFER, P19, 19, 1)
	FIELD(PIO_IFER, P20, 20, 1)
	FIELD(PIO_IFER, P21, 21, 1)
	FIELD(PIO_IFER, P22, 22, 1)
	FIELD(PIO_IFER, P23, 23, 1)
	FIELD(PIO_IFER, P24, 24, 1)
	FIELD(PIO_IFER, P25, 25, 1)
	FIELD(PIO_IFER, P26, 26, 1)
	FIELD(PIO_IFER, P27, 27, 1)
	FIELD(PIO_IFER, P28, 28, 1)
	FIELD(PIO_IFER, P29, 29, 1)
	FIELD(PIO_IFER, P30, 30, 1)
	FIELD(PIO_IFER, P31, 31, 1)
REG32(PIO_IFDR, 0x24)
	FIELD(PIO_IFDR, P0, 0, 1)
	FIELD(PIO_IFDR, P1, 1, 1)
	FIELD(PIO_IFDR, P2, 2, 1)
	FIELD(PIO_IFDR, P3, 3, 1)
	FIELD(PIO_IFDR, P4, 4, 1)
	FIELD(PIO_IFDR, P5, 5, 1)
	FIELD(PIO_IFDR, P6, 6, 1)
	FIELD(PIO_IFDR, P7, 7, 1)
	FIELD(PIO_IFDR, P8, 8, 1)
	FIELD(PIO_IFDR, P9, 9, 1)
	FIELD(PIO_IFDR, P10, 10, 1)
	FIELD(PIO_IFDR, P11, 11, 1)
	FIELD(PIO_IFDR, P12, 12, 1)
	FIELD(PIO_IFDR, P13, 13, 1)
	FIELD(PIO_IFDR, P14, 14, 1)
	FIELD(PIO_IFDR, P15, 15, 1)
	FIELD(PIO_IFDR, P16, 16, 1)
	FIELD(PIO_IFDR, P17, 17, 1)
	FIELD(PIO_IFDR, P18, 18, 1)
	FIELD(PIO_IFDR, P19, 19, 1)
	FIELD(PIO_IFDR, P20, 20, 1)
	FIELD(PIO_IFDR, P21, 21, 1)
	FIELD(PIO_IFDR, P22, 22, 1)
	FIELD(PIO_IFDR, P23, 23, 1)
	FIELD(PIO_IFDR, P24, 24, 1)
	FIELD(PIO_IFDR, P25, 25, 1)
	FIELD(PIO_IFDR, P26, 26, 1)
	FIELD(PIO_IFDR, P27, 27, 1)
	FIELD(PIO_IFDR, P28, 28, 1)
	FIELD(PIO_IFDR, P29, 29, 1)
	FIELD(PIO_IFDR, P30, 30, 1)
	FIELD(PIO_IFDR, P31, 31, 1)
REG32(PIO_IFSR, 0x28)
	FIELD(PIO_IFSR, P0, 0, 1)
	FIELD(PIO_IFSR, P1, 1, 1)
	FIELD(PIO_IFSR, P2, 2, 1)
	FIELD(PIO_IFSR, P3, 3, 1)
	FIELD(PIO_IFSR, P4, 4, 1)
	FIELD(PIO_IFSR, P5, 5, 1)
	FIELD(PIO_IFSR, P6, 6, 1)
	FIELD(PIO_IFSR, P7, 7, 1)
	FIELD(PIO_IFSR, P8, 8, 1)
	FIELD(PIO_IFSR, P9, 9, 1)
	FIELD(PIO_IFSR, P10, 10, 1)
	FIELD(PIO_IFSR, P11, 11, 1)
	FIELD(PIO_IFSR, P12, 12, 1)
	FIELD(PIO_IFSR, P13, 13, 1)
	FIELD(PIO_IFSR, P14, 14, 1)
	FIELD(PIO_IFSR, P15, 15, 1)
	FIELD(PIO_IFSR, P16, 16, 1)
	FIELD(PIO_IFSR, P17, 17, 1)
	FIELD(PIO_IFSR, P18, 18, 1)
	FIELD(PIO_IFSR, P19, 19, 1)
	FIELD(PIO_IFSR, P20, 20, 1)
	FIELD(PIO_IFSR, P21, 21, 1)
	FIELD(PIO_IFSR, P22, 22, 1)
	FIELD(PIO_IFSR, P23, 23, 1)
	FIELD(PIO_IFSR, P24, 24, 1)
	FIELD(PIO_IFSR, P25, 25, 1)
	FIELD(PIO_IFSR, P26, 26, 1)
	FIELD(PIO_IFSR, P27, 27, 1)
	FIELD(PIO_IFSR, P28, 28, 1)
	FIELD(PIO_IFSR, P29, 29, 1)
	FIELD(PIO_IFSR, P30, 30, 1)
	FIELD(PIO_IFSR, P31, 31, 1)
REG32(PIO_SODR, 0x30)
	FIELD(PIO_SODR, P0, 0, 1)
	FIELD(PIO_SODR, P1, 1, 1)
	FIELD(PIO_SODR, P2, 2, 1)
	FIELD(PIO_SODR, P3, 3, 1)
	FIELD(PIO_SODR, P4, 4, 1)
	FIELD(PIO_SODR, P5, 5, 1)
	FIELD(PIO_SODR, P6, 6, 1)
	FIELD(PIO_SODR, P7, 7, 1)
	FIELD(PIO_SODR, P8, 8, 1)
	FIELD(PIO_SODR, P9, 9, 1)
	FIELD(PIO_SODR, P10, 10, 1)
	FIELD(PIO_SODR, P11, 11, 1)
	FIELD(PIO_SODR, P12, 12, 1)
	FIELD(PIO_SODR, P13, 13, 1)
	FIELD(PIO_SODR, P14, 14, 1)
	FIELD(PIO_SODR, P15, 15, 1)
	FIELD(PIO_SODR, P16, 16, 1)
	FIELD(PIO_SODR, P17, 17, 1)
	FIELD(PIO_SODR, P18, 18, 1)
	FIELD(PIO_SODR, P19, 19, 1)
	FIELD(PIO_SODR, P20, 20, 1)
	FIELD(PIO_SODR, P21, 21, 1)
	FIELD(PIO_SODR, P22, 22, 1)
	FIELD(PIO_SODR, P23, 23, 1)
	FIELD(PIO_SODR, P24, 24, 1)
	FIELD(PIO_SODR, P25, 25, 1)
	FIELD(PIO_SODR, P26, 26, 1)
	FIELD(PIO_SODR, P27, 27, 1)
	FIELD(PIO_SODR, P28, 28, 1)
	FIELD(PIO_SODR, P29, 29, 1)
	FIELD(PIO_SODR, P30, 30, 1)
	FIELD(PIO_SODR, P31, 31, 1)
REG32(PIO_CODR, 0x34)
	FIELD(PIO_CODR, P0, 0, 1)
	FIELD(PIO_CODR, P1, 1, 1)
	FIELD(PIO_CODR, P2, 2, 1)
	FIELD(PIO_CODR, P3, 3, 1)
	FIELD(PIO_CODR, P4, 4, 1)
	FIELD(PIO_CODR, P5, 5, 1)
	FIELD(PIO_CODR, P6, 6, 1)
	FIELD(PIO_CODR, P7, 7, 1)
	FIELD(PIO_CODR, P8, 8, 1)
	FIELD(PIO_CODR, P9, 9, 1)
	FIELD(PIO_CODR, P10, 10, 1)
	FIELD(PIO_CODR, P11, 11, 1)
	FIELD(PIO_CODR, P12, 12, 1)
	FIELD(PIO_CODR, P13, 13, 1)
	FIELD(PIO_CODR, P14, 14, 1)
	FIELD(PIO_CODR, P15, 15, 1)
	FIELD(PIO_CODR, P16, 16, 1)
	FIELD(PIO_CODR, P17, 17, 1)
	FIELD(PIO_CODR, P18, 18, 1)
	FIELD(PIO_CODR, P19, 19, 1)
	FIELD(PIO_CODR, P20, 20, 1)
	FIELD(PIO_CODR, P21, 21, 1)
	FIELD(PIO_CODR, P22, 22, 1)
	FIELD(PIO_CODR, P23, 23, 1)
	FIELD(PIO_CODR, P24, 24, 1)
	FIELD(PIO_CODR, P25, 25, 1)
	FIELD(PIO_CODR, P26, 26, 1)
	FIELD(PIO_CODR, P27, 27, 1)
	FIELD(PIO_CODR, P28, 28, 1)
	FIELD(PIO_CODR, P29, 29, 1)
	FIELD(PIO_CODR, P30, 30, 1)
	FIELD(PIO_CODR, P31, 31, 1)
REG32(PIO_ODSR, 0x38)
	FIELD(PIO_ODSR, P0, 0, 1)
	FIELD(PIO_ODSR, P1, 1, 1)
	FIELD(PIO_ODSR, P2, 2, 1)
	FIELD(PIO_ODSR, P3, 3, 1)
	FIELD(PIO_ODSR, P4, 4, 1)
	FIELD(PIO_ODSR, P5, 5, 1)
	FIELD(PIO_ODSR, P6, 6, 1)
	FIELD(PIO_ODSR, P7, 7, 1)
	FIELD(PIO_ODSR, P8, 8, 1)
	FIELD(PIO_ODSR, P9, 9, 1)
	FIELD(PIO_ODSR, P10, 10, 1)
	FIELD(PIO_ODSR, P11, 11, 1)
	FIELD(PIO_ODSR, P12, 12, 1)
	FIELD(PIO_ODSR, P13, 13, 1)
	FIELD(PIO_ODSR, P14, 14, 1)
	FIELD(PIO_ODSR, P15, 15, 1)
	FIELD(PIO_ODSR, P16, 16, 1)
	FIELD(PIO_ODSR, P17, 17, 1)
	FIELD(PIO_ODSR, P18, 18, 1)
	FIELD(PIO_ODSR, P19, 19, 1)
	FIELD(PIO_ODSR, P20, 20, 1)
	FIELD(PIO_ODSR, P21, 21, 1)
	FIELD(PIO_ODSR, P22, 22, 1)
	FIELD(PIO_ODSR, P23, 23, 1)
	FIELD(PIO_ODSR, P24, 24, 1)
	FIELD(PIO_ODSR, P25, 25, 1)
	FIELD(PIO_ODSR, P26, 26, 1)
	FIELD(PIO_ODSR, P27, 27, 1)
	FIELD(PIO_ODSR, P28, 28, 1)
	FIELD(PIO_ODSR, P29, 29, 1)
	FIELD(PIO_ODSR, P30, 30, 1)
	FIELD(PIO_ODSR, P31, 31, 1)
REG32(PIO_PDSR, 0x3c)
	FIELD(PIO_PDSR, P0, 0, 1)
	FIELD(PIO_PDSR, P1, 1, 1)
	FIELD(PIO_PDSR, P2, 2, 1)
	FIELD(PIO_PDSR, P3, 3, 1)
	FIELD(PIO_PDSR, P4, 4, 1)
	FIELD(PIO_PDSR, P5, 5, 1)
	FIELD(PIO_PDSR, P6, 6, 1)
	FIELD(PIO_PDSR, P7, 7, 1)
	FIELD(PIO_PDSR, P8, 8, 1)
	FIELD(PIO_PDSR, P9, 9, 1)
	FIELD(PIO_PDSR, P10, 10, 1)
	FIELD(PIO_PDSR, P11, 11, 1)
	FIELD(PIO_PDSR, P12, 12, 1)
	FIELD(PIO_PDSR, P13, 13, 1)
	FIELD(PIO_PDSR, P14, 14, 1)
	FIELD(PIO_PDSR, P15, 15, 1)
	FIELD(PIO_PDSR, P16, 16, 1)
	FIELD(PIO_PDSR, P17, 17, 1)
	FIELD(PIO_PDSR, P18, 18, 1)
	FIELD(PIO_PDSR, P19, 19, 1)
	FIELD(PIO_PDSR, P20, 20, 1)
	FIELD(PIO_PDSR, P21, 21, 1)
	FIELD(PIO_PDSR, P22, 22, 1)
	FIELD(PIO_PDSR, P23, 23, 1)
	FIELD(PIO_PDSR, P24, 24, 1)
	FIELD(PIO_PDSR, P25, 25, 1)
	FIELD(PIO_PDSR, P26, 26, 1)
	FIELD(PIO_PDSR, P27, 27, 1)
	FIELD(PIO_PDSR, P28, 28, 1)
	FIELD(PIO_PDSR, P29, 29, 1)
	FIELD(PIO_PDSR, P30, 30, 1)
	FIELD(PIO_PDSR, P31, 31, 1)
REG32(PIO_IER, 0x40)
	FIELD(PIO_IER, P0, 0, 1)
	FIELD(PIO_IER, P1, 1, 1)
	FIELD(PIO_IER, P2, 2, 1)
	FIELD(PIO_IER, P3, 3, 1)
	FIELD(PIO_IER, P4, 4, 1)
	FIELD(PIO_IER, P5, 5, 1)
	FIELD(PIO_IER, P6, 6, 1)
	FIELD(PIO_IER, P7, 7, 1)
	FIELD(PIO_IER, P8, 8, 1)
	FIELD(PIO_IER, P9, 9, 1)
	FIELD(PIO_IER, P10, 10, 1)
	FIELD(PIO_IER, P11, 11, 1)
	FIELD(PIO_IER, P12, 12, 1)
	FIELD(PIO_IER, P13, 13, 1)
	FIELD(PIO_IER, P14, 14, 1)
	FIELD(PIO_IER, P15, 15, 1)
	FIELD(PIO_IER, P16, 16, 1)
	FIELD(PIO_IER, P17, 17, 1)
	FIELD(PIO_IER, P18, 18, 1)
	FIELD(PIO_IER, P19, 19, 1)
	FIELD(PIO_IER, P20, 20, 1)
	FIELD(PIO_IER, P21, 21, 1)
	FIELD(PIO_IER, P22, 22, 1)
	FIELD(PIO_IER, P23, 23, 1)
	FIELD(PIO_IER, P24, 24, 1)
	FIELD(PIO_IER, P25, 25, 1)
	FIELD(PIO_IER, P26, 26, 1)
	FIELD(PIO_IER, P27, 27, 1)
	FIELD(PIO_IER, P28, 28, 1)
	FIELD(PIO_IER, P29, 29, 1)
	FIELD(PIO_IER, P30, 30, 1)
	FIELD(PIO_IER, P31, 31, 1)
REG32(PIO_IDR, 0x44)
	FIELD(PIO_IDR, P0, 0, 1)
	FIELD(PIO_IDR, P1, 1, 1)
	FIELD(PIO_IDR, P2, 2, 1)
	FIELD(PIO_IDR, P3, 3, 1)
	FIELD(PIO_IDR, P4, 4, 1)
	FIELD(PIO_IDR, P5, 5, 1)
	FIELD(PIO_IDR, P6, 6, 1)
	FIELD(PIO_IDR, P7, 7, 1)
	FIELD(PIO_IDR, P8, 8, 1)
	FIELD(PIO_IDR, P9, 9, 1)
	FIELD(PIO_IDR, P10, 10, 1)
	FIELD(PIO_IDR, P11, 11, 1)
	FIELD(PIO_IDR, P12, 12, 1)
	FIELD(PIO_IDR, P13, 13, 1)
	FIELD(PIO_IDR, P14, 14, 1)
	FIELD(PIO_IDR, P15, 15, 1)
	FIELD(PIO_IDR, P16, 16, 1)
	FIELD(PIO_IDR, P17, 17, 1)
	FIELD(PIO_IDR, P18, 18, 1)
	FIELD(PIO_IDR, P19, 19, 1)
	FIELD(PIO_IDR, P20, 20, 1)
	FIELD(PIO_IDR, P21, 21, 1)
	FIELD(PIO_IDR, P22, 22, 1)
	FIELD(PIO_IDR, P23, 23, 1)
	FIELD(PIO_IDR, P24, 24, 1)
	FIELD(PIO_IDR, P25, 25, 1)
	FIELD(PIO_IDR, P26, 26, 1)
	FIELD(PIO_IDR, P27, 27, 1)
	FIELD(PIO_IDR, P28, 28, 1)
	FIELD(PIO_IDR, P29, 29, 1)
	FIELD(PIO_IDR, P30, 30, 1)
	FIELD(PIO_IDR, P31, 31, 1)
REG32(PIO_IMR, 0x48)
	FIELD(PIO_IMR, P0, 0, 1)
	FIELD(PIO_IMR, P1, 1, 1)
	FIELD(PIO_IMR, P2, 2, 1)
	FIELD(PIO_IMR, P3, 3, 1)
	FIELD(PIO_IMR, P4, 4, 1)
	FIELD(PIO_IMR, P5, 5, 1)
	FIELD(PIO_IMR, P6, 6, 1)
	FIELD(PIO_IMR, P7, 7, 1)
	FIELD(PIO_IMR, P8, 8, 1)
	FIELD(PIO_IMR, P9, 9, 1)
	FIELD(PIO_IMR, P10, 10, 1)
	FIELD(PIO_IMR, P11, 11, 1)
	FIELD(PIO_IMR, P12, 12, 1)
	FIELD(PIO_IMR, P13, 13, 1)
	FIELD(PIO_IMR, P14, 14, 1)
	FIELD(PIO_IMR, P15, 15, 1)
	FIELD(PIO_IMR, P16, 16, 1)
	FIELD(PIO_IMR, P17, 17, 1)
	FIELD(PIO_IMR, P18, 18, 1)
	FIELD(PIO_IMR, P19, 19, 1)
	FIELD(PIO_IMR, P20, 20, 1)
	FIELD(PIO_IMR, P21, 21, 1)
	FIELD(PIO_IMR, P22, 22, 1)
	FIELD(PIO_IMR, P23, 23, 1)
	FIELD(PIO_IMR, P24, 24, 1)
	FIELD(PIO_IMR, P25, 25, 1)
	FIELD(PIO_IMR, P26, 26, 1)
	FIELD(PIO_IMR, P27, 27, 1)
	FIELD(PIO_IMR, P28, 28, 1)
	FIELD(PIO_IMR, P29, 29, 1)
	FIELD(PIO_IMR, P30, 30, 1)
	FIELD(PIO_IMR, P31, 31, 1)
REG32(PIO_ISR, 0x4c)
	FIELD(PIO_ISR, P0, 0, 1)
	FIELD(PIO_ISR, P1, 1, 1)
	FIELD(PIO_ISR, P2, 2, 1)
	FIELD(PIO_ISR, P3, 3, 1)
	FIELD(PIO_ISR, P4, 4, 1)
	FIELD(PIO_ISR, P5, 5, 1)
	FIELD(PIO_ISR, P6, 6, 1)
	FIELD(PIO_ISR, P7, 7, 1)
	FIELD(PIO_ISR, P8, 8, 1)
	FIELD(PIO_ISR, P9, 9, 1)
	FIELD(PIO_ISR, P10, 10, 1)
	FIELD(PIO_ISR, P11, 11, 1)
	FIELD(PIO_ISR, P12, 12, 1)
	FIELD(PIO_ISR, P13, 13, 1)
	FIELD(PIO_ISR, P14, 14, 1)
	FIELD(PIO_ISR, P15, 15, 1)
	FIELD(PIO_ISR, P16, 16, 1)
	FIELD(PIO_ISR, P17, 17, 1)
	FIELD(PIO_ISR, P18, 18, 1)
	FIELD(PIO_ISR, P19, 19, 1)
	FIELD(PIO_ISR, P20, 20, 1)
	FIELD(PIO_ISR, P21, 21, 1)
	FIELD(PIO_ISR, P22, 22, 1)
	FIELD(PIO_ISR, P23, 23, 1)
	FIELD(PIO_ISR, P24, 24, 1)
	FIELD(PIO_ISR, P25, 25, 1)
	FIELD(PIO_ISR, P26, 26, 1)
	FIELD(PIO_ISR, P27, 27, 1)
	FIELD(PIO_ISR, P28, 28, 1)
	FIELD(PIO_ISR, P29, 29, 1)
	FIELD(PIO_ISR, P30, 30, 1)
	FIELD(PIO_ISR, P31, 31, 1)
REG32(PIO_MDER, 0x50)
	FIELD(PIO_MDER, P0, 0, 1)
	FIELD(PIO_MDER, P1, 1, 1)
	FIELD(PIO_MDER, P2, 2, 1)
	FIELD(PIO_MDER, P3, 3, 1)
	FIELD(PIO_MDER, P4, 4, 1)
	FIELD(PIO_MDER, P5, 5, 1)
	FIELD(PIO_MDER, P6, 6, 1)
	FIELD(PIO_MDER, P7, 7, 1)
	FIELD(PIO_MDER, P8, 8, 1)
	FIELD(PIO_MDER, P9, 9, 1)
	FIELD(PIO_MDER, P10, 10, 1)
	FIELD(PIO_MDER, P11, 11, 1)
	FIELD(PIO_MDER, P12, 12, 1)
	FIELD(PIO_MDER, P13, 13, 1)
	FIELD(PIO_MDER, P14, 14, 1)
	FIELD(PIO_MDER, P15, 15, 1)
	FIELD(PIO_MDER, P16, 16, 1)
	FIELD(PIO_MDER, P17, 17, 1)
	FIELD(PIO_MDER, P18, 18, 1)
	FIELD(PIO_MDER, P19, 19, 1)
	FIELD(PIO_MDER, P20, 20, 1)
	FIELD(PIO_MDER, P21, 21, 1)
	FIELD(PIO_MDER, P22, 22, 1)
	FIELD(PIO_MDER, P23, 23, 1)
	FIELD(PIO_MDER, P24, 24, 1)
	FIELD(PIO_MDER, P25, 25, 1)
	FIELD(PIO_MDER, P26, 26, 1)
	FIELD(PIO_MDER, P27, 27, 1)
	FIELD(PIO_MDER, P28, 28, 1)
	FIELD(PIO_MDER, P29, 29, 1)
	FIELD(PIO_MDER, P30, 30, 1)
	FIELD(PIO_MDER, P31, 31, 1)
REG32(PIO_MDDR, 0x54)
	FIELD(PIO_MDDR, P0, 0, 1)
	FIELD(PIO_MDDR, P1, 1, 1)
	FIELD(PIO_MDDR, P2, 2, 1)
	FIELD(PIO_MDDR, P3, 3, 1)
	FIELD(PIO_MDDR, P4, 4, 1)
	FIELD(PIO_MDDR, P5, 5, 1)
	FIELD(PIO_MDDR, P6, 6, 1)
	FIELD(PIO_MDDR, P7, 7, 1)
	FIELD(PIO_MDDR, P8, 8, 1)
	FIELD(PIO_MDDR, P9, 9, 1)
	FIELD(PIO_MDDR, P10, 10, 1)
	FIELD(PIO_MDDR, P11, 11, 1)
	FIELD(PIO_MDDR, P12, 12, 1)
	FIELD(PIO_MDDR, P13, 13, 1)
	FIELD(PIO_MDDR, P14, 14, 1)
	FIELD(PIO_MDDR, P15, 15, 1)
	FIELD(PIO_MDDR, P16, 16, 1)
	FIELD(PIO_MDDR, P17, 17, 1)
	FIELD(PIO_MDDR, P18, 18, 1)
	FIELD(PIO_MDDR, P19, 19, 1)
	FIELD(PIO_MDDR, P20, 20, 1)
	FIELD(PIO_MDDR, P21, 21, 1)
	FIELD(PIO_MDDR, P22, 22, 1)
	FIELD(PIO_MDDR, P23, 23, 1)
	FIELD(PIO_MDDR, P24, 24, 1)
	FIELD(PIO_MDDR, P25, 25, 1)
	FIELD(PIO_MDDR, P26, 26, 1)
	FIELD(PIO_MDDR, P27, 27, 1)
	FIELD(PIO_MDDR, P28, 28, 1)
	FIELD(PIO_MDDR, P29, 29, 1)
	FIELD(PIO_MDDR, P30, 30, 1)
	FIELD(PIO_MDDR, P31, 31, 1)
REG32(PIO_MDSR, 0x58)
	FIELD(PIO_MDSR, P0, 0, 1)
	FIELD(PIO_MDSR, P1, 1, 1)
	FIELD(PIO_MDSR, P2, 2, 1)
	FIELD(PIO_MDSR, P3, 3, 1)
	FIELD(PIO_MDSR, P4, 4, 1)
	FIELD(PIO_MDSR, P5, 5, 1)
	FIELD(PIO_MDSR, P6, 6, 1)
	FIELD(PIO_MDSR, P7, 7, 1)
	FIELD(PIO_MDSR, P8, 8, 1)
	FIELD(PIO_MDSR, P9, 9, 1)
	FIELD(PIO_MDSR, P10, 10, 1)
	FIELD(PIO_MDSR, P11, 11, 1)
	FIELD(PIO_MDSR, P12, 12, 1)
	FIELD(PIO_MDSR, P13, 13, 1)
	FIELD(PIO_MDSR, P14, 14, 1)
	FIELD(PIO_MDSR, P15, 15, 1)
	FIELD(PIO_MDSR, P16, 16, 1)
	FIELD(PIO_MDSR, P17, 17, 1)
	FIELD(PIO_MDSR, P18, 18, 1)
	FIELD(PIO_MDSR, P19, 19, 1)
	FIELD(PIO_MDSR, P20, 20, 1)
	FIELD(PIO_MDSR, P21, 21, 1)
	FIELD(PIO_MDSR, P22, 22, 1)
	FIELD(PIO_MDSR, P23, 23, 1)
	FIELD(PIO_MDSR, P24, 24, 1)
	FIELD(PIO_MDSR, P25, 25, 1)
	FIELD(PIO_MDSR, P26, 26, 1)
	FIELD(PIO_MDSR, P27, 27, 1)
	FIELD(PIO_MDSR, P28, 28, 1)
	FIELD(PIO_MDSR, P29, 29, 1)
	FIELD(PIO_MDSR, P30, 30, 1)
	FIELD(PIO_MDSR, P31, 31, 1)
REG32(PIO_PUDR, 0x60)
	FIELD(PIO_PUDR, P0, 0, 1)
	FIELD(PIO_PUDR, P1, 1, 1)
	FIELD(PIO_PUDR, P2, 2, 1)
	FIELD(PIO_PUDR, P3, 3, 1)
	FIELD(PIO_PUDR, P4, 4, 1)
	FIELD(PIO_PUDR, P5, 5, 1)
	FIELD(PIO_PUDR, P6, 6, 1)
	FIELD(PIO_PUDR, P7, 7, 1)
	FIELD(PIO_PUDR, P8, 8, 1)
	FIELD(PIO_PUDR, P9, 9, 1)
	FIELD(PIO_PUDR, P10, 10, 1)
	FIELD(PIO_PUDR, P11, 11, 1)
	FIELD(PIO_PUDR, P12, 12, 1)
	FIELD(PIO_PUDR, P13, 13, 1)
	FIELD(PIO_PUDR, P14, 14, 1)
	FIELD(PIO_PUDR, P15, 15, 1)
	FIELD(PIO_PUDR, P16, 16, 1)
	FIELD(PIO_PUDR, P17, 17, 1)
	FIELD(PIO_PUDR, P18, 18, 1)
	FIELD(PIO_PUDR, P19, 19, 1)
	FIELD(PIO_PUDR, P20, 20, 1)
	FIELD(PIO_PUDR, P21, 21, 1)
	FIELD(PIO_PUDR, P22, 22, 1)
	FIELD(PIO_PUDR, P23, 23, 1)
	FIELD(PIO_PUDR, P24, 24, 1)
	FIELD(PIO_PUDR, P25, 25, 1)
	FIELD(PIO_PUDR, P26, 26, 1)
	FIELD(PIO_PUDR, P27, 27, 1)
	FIELD(PIO_PUDR, P28, 28, 1)
	FIELD(PIO_PUDR, P29, 29, 1)
	FIELD(PIO_PUDR, P30, 30, 1)
	FIELD(PIO_PUDR, P31, 31, 1)
REG32(PIO_PUER, 0x64)
	FIELD(PIO_PUER, P0, 0, 1)
	FIELD(PIO_PUER, P1, 1, 1)
	FIELD(PIO_PUER, P2, 2, 1)
	FIELD(PIO_PUER, P3, 3, 1)
	FIELD(PIO_PUER, P4, 4, 1)
	FIELD(PIO_PUER, P5, 5, 1)
	FIELD(PIO_PUER, P6, 6, 1)
	FIELD(PIO_PUER, P7, 7, 1)
	FIELD(PIO_PUER, P8, 8, 1)
	FIELD(PIO_PUER, P9, 9, 1)
	FIELD(PIO_PUER, P10, 10, 1)
	FIELD(PIO_PUER, P11, 11, 1)
	FIELD(PIO_PUER, P12, 12, 1)
	FIELD(PIO_PUER, P13, 13, 1)
	FIELD(PIO_PUER, P14, 14, 1)
	FIELD(PIO_PUER, P15, 15, 1)
	FIELD(PIO_PUER, P16, 16, 1)
	FIELD(PIO_PUER, P17, 17, 1)
	FIELD(PIO_PUER, P18, 18, 1)
	FIELD(PIO_PUER, P19, 19, 1)
	FIELD(PIO_PUER, P20, 20, 1)
	FIELD(PIO_PUER, P21, 21, 1)
	FIELD(PIO_PUER, P22, 22, 1)
	FIELD(PIO_PUER, P23, 23, 1)
	FIELD(PIO_PUER, P24, 24, 1)
	FIELD(PIO_PUER, P25, 25, 1)
	FIELD(PIO_PUER, P26, 26, 1)
	FIELD(PIO_PUER, P27, 27, 1)
	FIELD(PIO_PUER, P28, 28, 1)
	FIELD(PIO_PUER, P29, 29, 1)
	FIELD(PIO_PUER, P30, 30, 1)
	FIELD(PIO_PUER, P31, 31, 1)
REG32(PIO_PUSR, 0x68)
	FIELD(PIO_PUSR, P0, 0, 1)
	FIELD(PIO_PUSR, P1, 1, 1)
	FIELD(PIO_PUSR, P2, 2, 1)
	FIELD(PIO_PUSR, P3, 3, 1)
	FIELD(PIO_PUSR, P4, 4, 1)
	FIELD(PIO_PUSR, P5, 5, 1)
	FIELD(PIO_PUSR, P6, 6, 1)
	FIELD(PIO_PUSR, P7, 7, 1)
	FIELD(PIO_PUSR, P8, 8, 1)
	FIELD(PIO_PUSR, P9, 9, 1)
	FIELD(PIO_PUSR, P10, 10, 1)
	FIELD(PIO_PUSR, P11, 11, 1)
	FIELD(PIO_PUSR, P12, 12, 1)
	FIELD(PIO_PUSR, P13, 13, 1)
	FIELD(PIO_PUSR, P14, 14, 1)
	FIELD(PIO_PUSR, P15, 15, 1)
	FIELD(PIO_PUSR, P16, 16, 1)
	FIELD(PIO_PUSR, P17, 17, 1)
	FIELD(PIO_PUSR, P18, 18, 1)
	FIELD(PIO_PUSR, P19, 19, 1)
	FIELD(PIO_PUSR, P20, 20, 1)
	FIELD(PIO_PUSR, P21, 21, 1)
	FIELD(PIO_PUSR, P22, 22, 1)
	FIELD(PIO_PUSR, P23, 23, 1)
	FIELD(PIO_PUSR, P24, 24, 1)
	FIELD(PIO_PUSR, P25, 25, 1)
	FIELD(PIO_PUSR, P26, 26, 1)
	FIELD(PIO_PUSR, P27, 27, 1)
	FIELD(PIO_PUSR, P28, 28, 1)
	FIELD(PIO_PUSR, P29, 29, 1)
	FIELD(PIO_PUSR, P30, 30, 1)
	FIELD(PIO_PUSR, P31, 31, 1)
REG32(PIO_ABSR, 0x70)
	FIELD(PIO_ABSR, P0, 0, 1)
	FIELD(PIO_ABSR, P1, 1, 1)
	FIELD(PIO_ABSR, P2, 2, 1)
	FIELD(PIO_ABSR, P3, 3, 1)
	FIELD(PIO_ABSR, P4, 4, 1)
	FIELD(PIO_ABSR, P5, 5, 1)
	FIELD(PIO_ABSR, P6, 6, 1)
	FIELD(PIO_ABSR, P7, 7, 1)
	FIELD(PIO_ABSR, P8, 8, 1)
	FIELD(PIO_ABSR, P9, 9, 1)
	FIELD(PIO_ABSR, P10, 10, 1)
	FIELD(PIO_ABSR, P11, 11, 1)
	FIELD(PIO_ABSR, P12, 12, 1)
	FIELD(PIO_ABSR, P13, 13, 1)
	FIELD(PIO_ABSR, P14, 14, 1)
	FIELD(PIO_ABSR, P15, 15, 1)
	FIELD(PIO_ABSR, P16, 16, 1)
	FIELD(PIO_ABSR, P17, 17, 1)
	FIELD(PIO_ABSR, P18, 18, 1)
	FIELD(PIO_ABSR, P19, 19, 1)
	FIELD(PIO_ABSR, P20, 20, 1)
	FIELD(PIO_ABSR, P21, 21, 1)
	FIELD(PIO_ABSR, P22, 22, 1)
	FIELD(PIO_ABSR, P23, 23, 1)
	FIELD(PIO_ABSR, P24, 24, 1)
	FIELD(PIO_ABSR, P25, 25, 1)
	FIELD(PIO_ABSR, P26, 26, 1)
	FIELD(PIO_ABSR, P27, 27, 1)
	FIELD(PIO_ABSR, P28, 28, 1)
	FIELD(PIO_ABSR, P29, 29, 1)
	FIELD(PIO_ABSR, P30, 30, 1)
	FIELD(PIO_ABSR, P31, 31, 1)
REG32(PIO_SCIFSR, 0x80)
	FIELD(PIO_SCIFSR, P0, 0, 1)
	FIELD(PIO_SCIFSR, P1, 1, 1)
	FIELD(PIO_SCIFSR, P2, 2, 1)
	FIELD(PIO_SCIFSR, P3, 3, 1)
	FIELD(PIO_SCIFSR, P4, 4, 1)
	FIELD(PIO_SCIFSR, P5, 5, 1)
	FIELD(PIO_SCIFSR, P6, 6, 1)
	FIELD(PIO_SCIFSR, P7, 7, 1)
	FIELD(PIO_SCIFSR, P8, 8, 1)
	FIELD(PIO_SCIFSR, P9, 9, 1)
	FIELD(PIO_SCIFSR, P10, 10, 1)
	FIELD(PIO_SCIFSR, P11, 11, 1)
	FIELD(PIO_SCIFSR, P12, 12, 1)
	FIELD(PIO_SCIFSR, P13, 13, 1)
	FIELD(PIO_SCIFSR, P14, 14, 1)
	FIELD(PIO_SCIFSR, P15, 15, 1)
	FIELD(PIO_SCIFSR, P16, 16, 1)
	FIELD(PIO_SCIFSR, P17, 17, 1)
	FIELD(PIO_SCIFSR, P18, 18, 1)
	FIELD(PIO_SCIFSR, P19, 19, 1)
	FIELD(PIO_SCIFSR, P20, 20, 1)
	FIELD(PIO_SCIFSR, P21, 21, 1)
	FIELD(PIO_SCIFSR, P22, 22, 1)
	FIELD(PIO_SCIFSR, P23, 23, 1)
	FIELD(PIO_SCIFSR, P24, 24, 1)
	FIELD(PIO_SCIFSR, P25, 25, 1)
	FIELD(PIO_SCIFSR, P26, 26, 1)
	FIELD(PIO_SCIFSR, P27, 27, 1)
	FIELD(PIO_SCIFSR, P28, 28, 1)
	FIELD(PIO_SCIFSR, P29, 29, 1)
	FIELD(PIO_SCIFSR, P30, 30, 1)
	FIELD(PIO_SCIFSR, P31, 31, 1)
REG32(PIO_DIFSR, 0x84)
	FIELD(PIO_DIFSR, P0, 0, 1)
	FIELD(PIO_DIFSR, P1, 1, 1)
	FIELD(PIO_DIFSR, P2, 2, 1)
	FIELD(PIO_DIFSR, P3, 3, 1)
	FIELD(PIO_DIFSR, P4, 4, 1)
	FIELD(PIO_DIFSR, P5, 5, 1)
	FIELD(PIO_DIFSR, P6, 6, 1)
	FIELD(PIO_DIFSR, P7, 7, 1)
	FIELD(PIO_DIFSR, P8, 8, 1)
	FIELD(PIO_DIFSR, P9, 9, 1)
	FIELD(PIO_DIFSR, P10, 10, 1)
	FIELD(PIO_DIFSR, P11, 11, 1)
	FIELD(PIO_DIFSR, P12, 12, 1)
	FIELD(PIO_DIFSR, P13, 13, 1)
	FIELD(PIO_DIFSR, P14, 14, 1)
	FIELD(PIO_DIFSR, P15, 15, 1)
	FIELD(PIO_DIFSR, P16, 16, 1)
	FIELD(PIO_DIFSR, P17, 17, 1)
	FIELD(PIO_DIFSR, P18, 18, 1)
	FIELD(PIO_DIFSR, P19, 19, 1)
	FIELD(PIO_DIFSR, P20, 20, 1)
	FIELD(PIO_DIFSR, P21, 21, 1)
	FIELD(PIO_DIFSR, P22, 22, 1)
	FIELD(PIO_DIFSR, P23, 23, 1)
	FIELD(PIO_DIFSR, P24, 24, 1)
	FIELD(PIO_DIFSR, P25, 25, 1)
	FIELD(PIO_DIFSR, P26, 26, 1)
	FIELD(PIO_DIFSR, P27, 27, 1)
	FIELD(PIO_DIFSR, P28, 28, 1)
	FIELD(PIO_DIFSR, P29, 29, 1)
	FIELD(PIO_DIFSR, P30, 30, 1)
	FIELD(PIO_DIFSR, P31, 31, 1)
REG32(PIO_IFDGSR, 0x88)
	FIELD(PIO_IFDGSR, P0, 0, 1)
	FIELD(PIO_IFDGSR, P1, 1, 1)
	FIELD(PIO_IFDGSR, P2, 2, 1)
	FIELD(PIO_IFDGSR, P3, 3, 1)
	FIELD(PIO_IFDGSR, P4, 4, 1)
	FIELD(PIO_IFDGSR, P5, 5, 1)
	FIELD(PIO_IFDGSR, P6, 6, 1)
	FIELD(PIO_IFDGSR, P7, 7, 1)
	FIELD(PIO_IFDGSR, P8, 8, 1)
	FIELD(PIO_IFDGSR, P9, 9, 1)
	FIELD(PIO_IFDGSR, P10, 10, 1)
	FIELD(PIO_IFDGSR, P11, 11, 1)
	FIELD(PIO_IFDGSR, P12, 12, 1)
	FIELD(PIO_IFDGSR, P13, 13, 1)
	FIELD(PIO_IFDGSR, P14, 14, 1)
	FIELD(PIO_IFDGSR, P15, 15, 1)
	FIELD(PIO_IFDGSR, P16, 16, 1)
	FIELD(PIO_IFDGSR, P17, 17, 1)
	FIELD(PIO_IFDGSR, P18, 18, 1)
	FIELD(PIO_IFDGSR, P19, 19, 1)
	FIELD(PIO_IFDGSR, P20, 20, 1)
	FIELD(PIO_IFDGSR, P21, 21, 1)
	FIELD(PIO_IFDGSR, P22, 22, 1)
	FIELD(PIO_IFDGSR, P23, 23, 1)
	FIELD(PIO_IFDGSR, P24, 24, 1)
	FIELD(PIO_IFDGSR, P25, 25, 1)
	FIELD(PIO_IFDGSR, P26, 26, 1)
	FIELD(PIO_IFDGSR, P27, 27, 1)
	FIELD(PIO_IFDGSR, P28, 28, 1)
	FIELD(PIO_IFDGSR, P29, 29, 1)
	FIELD(PIO_IFDGSR, P30, 30, 1)
	FIELD(PIO_IFDGSR, P31, 31, 1)
REG32(PIO_SCDR, 0x8c)
	FIELD(PIO_SCDR, DIV, 0, 14)
REG32(PIO_OWER, 0xa0)
	FIELD(PIO_OWER, P0, 0, 1)
	FIELD(PIO_OWER, P1, 1, 1)
	FIELD(PIO_OWER, P2, 2, 1)
	FIELD(PIO_OWER, P3, 3, 1)
	FIELD(PIO_OWER, P4, 4, 1)
	FIELD(PIO_OWER, P5, 5, 1)
	FIELD(PIO_OWER, P6, 6, 1)
	FIELD(PIO_OWER, P7, 7, 1)
	FIELD(PIO_OWER, P8, 8, 1)
	FIELD(PIO_OWER, P9, 9, 1)
	FIELD(PIO_OWER, P10, 10, 1)
	FIELD(PIO_OWER, P11, 11, 1)
	FIELD(PIO_OWER, P12, 12, 1)
	FIELD(PIO_OWER, P13, 13, 1)
	FIELD(PIO_OWER, P14, 14, 1)
	FIELD(PIO_OWER, P15, 15, 1)
	FIELD(PIO_OWER, P16, 16, 1)
	FIELD(PIO_OWER, P17, 17, 1)
	FIELD(PIO_OWER, P18, 18, 1)
	FIELD(PIO_OWER, P19, 19, 1)
	FIELD(PIO_OWER, P20, 20, 1)
	FIELD(PIO_OWER, P21, 21, 1)
	FIELD(PIO_OWER, P22, 22, 1)
	FIELD(PIO_OWER, P23, 23, 1)
	FIELD(PIO_OWER, P24, 24, 1)
	FIELD(PIO_OWER, P25, 25, 1)
	FIELD(PIO_OWER, P26, 26, 1)
	FIELD(PIO_OWER, P27, 27, 1)
	FIELD(PIO_OWER, P28, 28, 1)
	FIELD(PIO_OWER, P29, 29, 1)
	FIELD(PIO_OWER, P30, 30, 1)
	FIELD(PIO_OWER, P31, 31, 1)
REG32(PIO_OWDR, 0xa4)
	FIELD(PIO_OWDR, P0, 0, 1)
	FIELD(PIO_OWDR, P1, 1, 1)
	FIELD(PIO_OWDR, P2, 2, 1)
	FIELD(PIO_OWDR, P3, 3, 1)
	FIELD(PIO_OWDR, P4, 4, 1)
	FIELD(PIO_OWDR, P5, 5, 1)
	FIELD(PIO_OWDR, P6, 6, 1)
	FIELD(PIO_OWDR, P7, 7, 1)
	FIELD(PIO_OWDR, P8, 8, 1)
	FIELD(PIO_OWDR, P9, 9, 1)
	FIELD(PIO_OWDR, P10, 10, 1)
	FIELD(PIO_OWDR, P11, 11, 1)
	FIELD(PIO_OWDR, P12, 12, 1)
	FIELD(PIO_OWDR, P13, 13, 1)
	FIELD(PIO_OWDR, P14, 14, 1)
	FIELD(PIO_OWDR, P15, 15, 1)
	FIELD(PIO_OWDR, P16, 16, 1)
	FIELD(PIO_OWDR, P17, 17, 1)
	FIELD(PIO_OWDR, P18, 18, 1)
	FIELD(PIO_OWDR, P19, 19, 1)
	FIELD(PIO_OWDR, P20, 20, 1)
	FIELD(PIO_OWDR, P21, 21, 1)
	FIELD(PIO_OWDR, P22, 22, 1)
	FIELD(PIO_OWDR, P23, 23, 1)
	FIELD(PIO_OWDR, P24, 24, 1)
	FIELD(PIO_OWDR, P25, 25, 1)
	FIELD(PIO_OWDR, P26, 26, 1)
	FIELD(PIO_OWDR, P27, 27, 1)
	FIELD(PIO_OWDR, P28, 28, 1)
	FIELD(PIO_OWDR, P29, 29, 1)
	FIELD(PIO_OWDR, P30, 30, 1)
	FIELD(PIO_OWDR, P31, 31, 1)
REG32(PIO_OWSR, 0xa8)
	FIELD(PIO_OWSR, P0, 0, 1)
	FIELD(PIO_OWSR, P1, 1, 1)
	FIELD(PIO_OWSR, P2, 2, 1)
	FIELD(PIO_OWSR, P3, 3, 1)
	FIELD(PIO_OWSR, P4, 4, 1)
	FIELD(PIO_OWSR, P5, 5, 1)
	FIELD(PIO_OWSR, P6, 6, 1)
	FIELD(PIO_OWSR, P7, 7, 1)
	FIELD(PIO_OWSR, P8, 8, 1)
	FIELD(PIO_OWSR, P9, 9, 1)
	FIELD(PIO_OWSR, P10, 10, 1)
	FIELD(PIO_OWSR, P11, 11, 1)
	FIELD(PIO_OWSR, P12, 12, 1)
	FIELD(PIO_OWSR, P13, 13, 1)
	FIELD(PIO_OWSR, P14, 14, 1)
	FIELD(PIO_OWSR, P15, 15, 1)
	FIELD(PIO_OWSR, P16, 16, 1)
	FIELD(PIO_OWSR, P17, 17, 1)
	FIELD(PIO_OWSR, P18, 18, 1)
	FIELD(PIO_OWSR, P19, 19, 1)
	FIELD(PIO_OWSR, P20, 20, 1)
	FIELD(PIO_OWSR, P21, 21, 1)
	FIELD(PIO_OWSR, P22, 22, 1)
	FIELD(PIO_OWSR, P23, 23, 1)
	FIELD(PIO_OWSR, P24, 24, 1)
	FIELD(PIO_OWSR, P25, 25, 1)
	FIELD(PIO_OWSR, P26, 26, 1)
	FIELD(PIO_OWSR, P27, 27, 1)
	FIELD(PIO_OWSR, P28, 28, 1)
	FIELD(PIO_OWSR, P29, 29, 1)
	FIELD(PIO_OWSR, P30, 30, 1)
	FIELD(PIO_OWSR, P31, 31, 1)
REG32(PIO_AIMER, 0xb0)
	FIELD(PIO_AIMER, P0, 0, 1)
	FIELD(PIO_AIMER, P1, 1, 1)
	FIELD(PIO_AIMER, P2, 2, 1)
	FIELD(PIO_AIMER, P3, 3, 1)
	FIELD(PIO_AIMER, P4, 4, 1)
	FIELD(PIO_AIMER, P5, 5, 1)
	FIELD(PIO_AIMER, P6, 6, 1)
	FIELD(PIO_AIMER, P7, 7, 1)
	FIELD(PIO_AIMER, P8, 8, 1)
	FIELD(PIO_AIMER, P9, 9, 1)
	FIELD(PIO_AIMER, P10, 10, 1)
	FIELD(PIO_AIMER, P11, 11, 1)
	FIELD(PIO_AIMER, P12, 12, 1)
	FIELD(PIO_AIMER, P13, 13, 1)
	FIELD(PIO_AIMER, P14, 14, 1)
	FIELD(PIO_AIMER, P15, 15, 1)
	FIELD(PIO_AIMER, P16, 16, 1)
	FIELD(PIO_AIMER, P17, 17, 1)
	FIELD(PIO_AIMER, P18, 18, 1)
	FIELD(PIO_AIMER, P19, 19, 1)
	FIELD(PIO_AIMER, P20, 20, 1)
	FIELD(PIO_AIMER, P21, 21, 1)
	FIELD(PIO_AIMER, P22, 22, 1)
	FIELD(PIO_AIMER, P23, 23, 1)
	FIELD(PIO_AIMER, P24, 24, 1)
	FIELD(PIO_AIMER, P25, 25, 1)
	FIELD(PIO_AIMER, P26, 26, 1)
	FIELD(PIO_AIMER, P27, 27, 1)
	FIELD(PIO_AIMER, P28, 28, 1)
	FIELD(PIO_AIMER, P29, 29, 1)
	FIELD(PIO_AIMER, P30, 30, 1)
	FIELD(PIO_AIMER, P31, 31, 1)
REG32(PIO_AIMDR, 0xb4)
	FIELD(PIO_AIMDR, P0, 0, 1)
	FIELD(PIO_AIMDR, P1, 1, 1)
	FIELD(PIO_AIMDR, P2, 2, 1)
	FIELD(PIO_AIMDR, P3, 3, 1)
	FIELD(PIO_AIMDR, P4, 4, 1)
	FIELD(PIO_AIMDR, P5, 5, 1)
	FIELD(PIO_AIMDR, P6, 6, 1)
	FIELD(PIO_AIMDR, P7, 7, 1)
	FIELD(PIO_AIMDR, P8, 8, 1)
	FIELD(PIO_AIMDR, P9, 9, 1)
	FIELD(PIO_AIMDR, P10, 10, 1)
	FIELD(PIO_AIMDR, P11, 11, 1)
	FIELD(PIO_AIMDR, P12, 12, 1)
	FIELD(PIO_AIMDR, P13, 13, 1)
	FIELD(PIO_AIMDR, P14, 14, 1)
	FIELD(PIO_AIMDR, P15, 15, 1)
	FIELD(PIO_AIMDR, P16, 16, 1)
	FIELD(PIO_AIMDR, P17, 17, 1)
	FIELD(PIO_AIMDR, P18, 18, 1)
	FIELD(PIO_AIMDR, P19, 19, 1)
	FIELD(PIO_AIMDR, P20, 20, 1)
	FIELD(PIO_AIMDR, P21, 21, 1)
	FIELD(PIO_AIMDR, P22, 22, 1)
	FIELD(PIO_AIMDR, P23, 23, 1)
	FIELD(PIO_AIMDR, P24, 24, 1)
	FIELD(PIO_AIMDR, P25, 25, 1)
	FIELD(PIO_AIMDR, P26, 26, 1)
	FIELD(PIO_AIMDR, P27, 27, 1)
	FIELD(PIO_AIMDR, P28, 28, 1)
	FIELD(PIO_AIMDR, P29, 29, 1)
	FIELD(PIO_AIMDR, P30, 30, 1)
	FIELD(PIO_AIMDR, P31, 31, 1)
REG32(PIO_AIMMR, 0xb8)
	FIELD(PIO_AIMMR, P0, 0, 1)
	FIELD(PIO_AIMMR, P1, 1, 1)
	FIELD(PIO_AIMMR, P2, 2, 1)
	FIELD(PIO_AIMMR, P3, 3, 1)
	FIELD(PIO_AIMMR, P4, 4, 1)
	FIELD(PIO_AIMMR, P5, 5, 1)
	FIELD(PIO_AIMMR, P6, 6, 1)
	FIELD(PIO_AIMMR, P7, 7, 1)
	FIELD(PIO_AIMMR, P8, 8, 1)
	FIELD(PIO_AIMMR, P9, 9, 1)
	FIELD(PIO_AIMMR, P10, 10, 1)
	FIELD(PIO_AIMMR, P11, 11, 1)
	FIELD(PIO_AIMMR, P12, 12, 1)
	FIELD(PIO_AIMMR, P13, 13, 1)
	FIELD(PIO_AIMMR, P14, 14, 1)
	FIELD(PIO_AIMMR, P15, 15, 1)
	FIELD(PIO_AIMMR, P16, 16, 1)
	FIELD(PIO_AIMMR, P17, 17, 1)
	FIELD(PIO_AIMMR, P18, 18, 1)
	FIELD(PIO_AIMMR, P19, 19, 1)
	FIELD(PIO_AIMMR, P20, 20, 1)
	FIELD(PIO_AIMMR, P21, 21, 1)
	FIELD(PIO_AIMMR, P22, 22, 1)
	FIELD(PIO_AIMMR, P23, 23, 1)
	FIELD(PIO_AIMMR, P24, 24, 1)
	FIELD(PIO_AIMMR, P25, 25, 1)
	FIELD(PIO_AIMMR, P26, 26, 1)
	FIELD(PIO_AIMMR, P27, 27, 1)
	FIELD(PIO_AIMMR, P28, 28, 1)
	FIELD(PIO_AIMMR, P29, 29, 1)
	FIELD(PIO_AIMMR, P30, 30, 1)
	FIELD(PIO_AIMMR, P31, 31, 1)
REG32(PIO_ESR, 0xc0)
	FIELD(PIO_ESR, P0, 0, 1)
	FIELD(PIO_ESR, P1, 1, 1)
	FIELD(PIO_ESR, P2, 2, 1)
	FIELD(PIO_ESR, P3, 3, 1)
	FIELD(PIO_ESR, P4, 4, 1)
	FIELD(PIO_ESR, P5, 5, 1)
	FIELD(PIO_ESR, P6, 6, 1)
	FIELD(PIO_ESR, P7, 7, 1)
	FIELD(PIO_ESR, P8, 8, 1)
	FIELD(PIO_ESR, P9, 9, 1)
	FIELD(PIO_ESR, P10, 10, 1)
	FIELD(PIO_ESR, P11, 11, 1)
	FIELD(PIO_ESR, P12, 12, 1)
	FIELD(PIO_ESR, P13, 13, 1)
	FIELD(PIO_ESR, P14, 14, 1)
	FIELD(PIO_ESR, P15, 15, 1)
	FIELD(PIO_ESR, P16, 16, 1)
	FIELD(PIO_ESR, P17, 17, 1)
	FIELD(PIO_ESR, P18, 18, 1)
	FIELD(PIO_ESR, P19, 19, 1)
	FIELD(PIO_ESR, P20, 20, 1)
	FIELD(PIO_ESR, P21, 21, 1)
	FIELD(PIO_ESR, P22, 22, 1)
	FIELD(PIO_ESR, P23, 23, 1)
	FIELD(PIO_ESR, P24, 24, 1)
	FIELD(PIO_ESR, P25, 25, 1)
	FIELD(PIO_ESR, P26, 26, 1)
	FIELD(PIO_ESR, P27, 27, 1)
	FIELD(PIO_ESR, P28, 28, 1)
	FIELD(PIO_ESR, P29, 29, 1)
	FIELD(PIO_ESR, P30, 30, 1)
	FIELD(PIO_ESR, P31, 31, 1)
REG32(PIO_LSR, 0xc4)
	FIELD(PIO_LSR, P0, 0, 1)
	FIELD(PIO_LSR, P1, 1, 1)
	FIELD(PIO_LSR, P2, 2, 1)
	FIELD(PIO_LSR, P3, 3, 1)
	FIELD(PIO_LSR, P4, 4, 1)
	FIELD(PIO_LSR, P5, 5, 1)
	FIELD(PIO_LSR, P6, 6, 1)
	FIELD(PIO_LSR, P7, 7, 1)
	FIELD(PIO_LSR, P8, 8, 1)
	FIELD(PIO_LSR, P9, 9, 1)
	FIELD(PIO_LSR, P10, 10, 1)
	FIELD(PIO_LSR, P11, 11, 1)
	FIELD(PIO_LSR, P12, 12, 1)
	FIELD(PIO_LSR, P13, 13, 1)
	FIELD(PIO_LSR, P14, 14, 1)
	FIELD(PIO_LSR, P15, 15, 1)
	FIELD(PIO_LSR, P16, 16, 1)
	FIELD(PIO_LSR, P17, 17, 1)
	FIELD(PIO_LSR, P18, 18, 1)
	FIELD(PIO_LSR, P19, 19, 1)
	FIELD(PIO_LSR, P20, 20, 1)
	FIELD(PIO_LSR, P21, 21, 1)
	FIELD(PIO_LSR, P22, 22, 1)
	FIELD(PIO_LSR, P23, 23, 1)
	FIELD(PIO_LSR, P24, 24, 1)
	FIELD(PIO_LSR, P25, 25, 1)
	FIELD(PIO_LSR, P26, 26, 1)
	FIELD(PIO_LSR, P27, 27, 1)
	FIELD(PIO_LSR, P28, 28, 1)
	FIELD(PIO_LSR, P29, 29, 1)
	FIELD(PIO_LSR, P30, 30, 1)
	FIELD(PIO_LSR, P31, 31, 1)
REG32(PIO_ELSR, 0xc8)
	FIELD(PIO_ELSR, P0, 0, 1)
	FIELD(PIO_ELSR, P1, 1, 1)
	FIELD(PIO_ELSR, P2, 2, 1)
	FIELD(PIO_ELSR, P3, 3, 1)
	FIELD(PIO_ELSR, P4, 4, 1)
	FIELD(PIO_ELSR, P5, 5, 1)
	FIELD(PIO_ELSR, P6, 6, 1)
	FIELD(PIO_ELSR, P7, 7, 1)
	FIELD(PIO_ELSR, P8, 8, 1)
	FIELD(PIO_ELSR, P9, 9, 1)
	FIELD(PIO_ELSR, P10, 10, 1)
	FIELD(PIO_ELSR, P11, 11, 1)
	FIELD(PIO_ELSR, P12, 12, 1)
	FIELD(PIO_ELSR, P13, 13, 1)
	FIELD(PIO_ELSR, P14, 14, 1)
	FIELD(PIO_ELSR, P15, 15, 1)
	FIELD(PIO_ELSR, P16, 16, 1)
	FIELD(PIO_ELSR, P17, 17, 1)
	FIELD(PIO_ELSR, P18, 18, 1)
	FIELD(PIO_ELSR, P19, 19, 1)
	FIELD(PIO_ELSR, P20, 20, 1)
	FIELD(PIO_ELSR, P21, 21, 1)
	FIELD(PIO_ELSR, P22, 22, 1)
	FIELD(PIO_ELSR, P23, 23, 1)
	FIELD(PIO_ELSR, P24, 24, 1)
	FIELD(PIO_ELSR, P25, 25, 1)
	FIELD(PIO_ELSR, P26, 26, 1)
	FIELD(PIO_ELSR, P27, 27, 1)
	FIELD(PIO_ELSR, P28, 28, 1)
	FIELD(PIO_ELSR, P29, 29, 1)
	FIELD(PIO_ELSR, P30, 30, 1)
	FIELD(PIO_ELSR, P31, 31, 1)
REG32(PIO_FELLSR, 0xd0)
	FIELD(PIO_FELLSR, P0, 0, 1)
	FIELD(PIO_FELLSR, P1, 1, 1)
	FIELD(PIO_FELLSR, P2, 2, 1)
	FIELD(PIO_FELLSR, P3, 3, 1)
	FIELD(PIO_FELLSR, P4, 4, 1)
	FIELD(PIO_FELLSR, P5, 5, 1)
	FIELD(PIO_FELLSR, P6, 6, 1)
	FIELD(PIO_FELLSR, P7, 7, 1)
	FIELD(PIO_FELLSR, P8, 8, 1)
	FIELD(PIO_FELLSR, P9, 9, 1)
	FIELD(PIO_FELLSR, P10, 10, 1)
	FIELD(PIO_FELLSR, P11, 11, 1)
	FIELD(PIO_FELLSR, P12, 12, 1)
	FIELD(PIO_FELLSR, P13, 13, 1)
	FIELD(PIO_FELLSR, P14, 14, 1)
	FIELD(PIO_FELLSR, P15, 15, 1)
	FIELD(PIO_FELLSR, P16, 16, 1)
	FIELD(PIO_FELLSR, P17, 17, 1)
	FIELD(PIO_FELLSR, P18, 18, 1)
	FIELD(PIO_FELLSR, P19, 19, 1)
	FIELD(PIO_FELLSR, P20, 20, 1)
	FIELD(PIO_FELLSR, P21, 21, 1)
	FIELD(PIO_FELLSR, P22, 22, 1)
	FIELD(PIO_FELLSR, P23, 23, 1)
	FIELD(PIO_FELLSR, P24, 24, 1)
	FIELD(PIO_FELLSR, P25, 25, 1)
	FIELD(PIO_FELLSR, P26, 26, 1)
	FIELD(PIO_FELLSR, P27, 27, 1)
	FIELD(PIO_FELLSR, P28, 28, 1)
	FIELD(PIO_FELLSR, P29, 29, 1)
	FIELD(PIO_FELLSR, P30, 30, 1)
	FIELD(PIO_FELLSR, P31, 31, 1)
REG32(PIO_REHLSR, 0xd4)
	FIELD(PIO_REHLSR, P0, 0, 1)
	FIELD(PIO_REHLSR, P1, 1, 1)
	FIELD(PIO_REHLSR, P2, 2, 1)
	FIELD(PIO_REHLSR, P3, 3, 1)
	FIELD(PIO_REHLSR, P4, 4, 1)
	FIELD(PIO_REHLSR, P5, 5, 1)
	FIELD(PIO_REHLSR, P6, 6, 1)
	FIELD(PIO_REHLSR, P7, 7, 1)
	FIELD(PIO_REHLSR, P8, 8, 1)
	FIELD(PIO_REHLSR, P9, 9, 1)
	FIELD(PIO_REHLSR, P10, 10, 1)
	FIELD(PIO_REHLSR, P11, 11, 1)
	FIELD(PIO_REHLSR, P12, 12, 1)
	FIELD(PIO_REHLSR, P13, 13, 1)
	FIELD(PIO_REHLSR, P14, 14, 1)
	FIELD(PIO_REHLSR, P15, 15, 1)
	FIELD(PIO_REHLSR, P16, 16, 1)
	FIELD(PIO_REHLSR, P17, 17, 1)
	FIELD(PIO_REHLSR, P18, 18, 1)
	FIELD(PIO_REHLSR, P19, 19, 1)
	FIELD(PIO_REHLSR, P20, 20, 1)
	FIELD(PIO_REHLSR, P21, 21, 1)
	FIELD(PIO_REHLSR, P22, 22, 1)
	FIELD(PIO_REHLSR, P23, 23, 1)
	FIELD(PIO_REHLSR, P24, 24, 1)
	FIELD(PIO_REHLSR, P25, 25, 1)
	FIELD(PIO_REHLSR, P26, 26, 1)
	FIELD(PIO_REHLSR, P27, 27, 1)
	FIELD(PIO_REHLSR, P28, 28, 1)
	FIELD(PIO_REHLSR, P29, 29, 1)
	FIELD(PIO_REHLSR, P30, 30, 1)
	FIELD(PIO_REHLSR, P31, 31, 1)
REG32(PIO_FRLHSR, 0xd8)
	FIELD(PIO_FRLHSR, P0, 0, 1)
	FIELD(PIO_FRLHSR, P1, 1, 1)
	FIELD(PIO_FRLHSR, P2, 2, 1)
	FIELD(PIO_FRLHSR, P3, 3, 1)
	FIELD(PIO_FRLHSR, P4, 4, 1)
	FIELD(PIO_FRLHSR, P5, 5, 1)
	FIELD(PIO_FRLHSR, P6, 6, 1)
	FIELD(PIO_FRLHSR, P7, 7, 1)
	FIELD(PIO_FRLHSR, P8, 8, 1)
	FIELD(PIO_FRLHSR, P9, 9, 1)
	FIELD(PIO_FRLHSR, P10, 10, 1)
	FIELD(PIO_FRLHSR, P11, 11, 1)
	FIELD(PIO_FRLHSR, P12, 12, 1)
	FIELD(PIO_FRLHSR, P13, 13, 1)
	FIELD(PIO_FRLHSR, P14, 14, 1)
	FIELD(PIO_FRLHSR, P15, 15, 1)
	FIELD(PIO_FRLHSR, P16, 16, 1)
	FIELD(PIO_FRLHSR, P17, 17, 1)
	FIELD(PIO_FRLHSR, P18, 18, 1)
	FIELD(PIO_FRLHSR, P19, 19, 1)
	FIELD(PIO_FRLHSR, P20, 20, 1)
	FIELD(PIO_FRLHSR, P21, 21, 1)
	FIELD(PIO_FRLHSR, P22, 22, 1)
	FIELD(PIO_FRLHSR, P23, 23, 1)
	FIELD(PIO_FRLHSR, P24, 24, 1)
	FIELD(PIO_FRLHSR, P25, 25, 1)
	FIELD(PIO_FRLHSR, P26, 26, 1)
	FIELD(PIO_FRLHSR, P27, 27, 1)
	FIELD(PIO_FRLHSR, P28, 28, 1)
	FIELD(PIO_FRLHSR, P29, 29, 1)
	FIELD(PIO_FRLHSR, P30, 30, 1)
	FIELD(PIO_FRLHSR, P31, 31, 1)
REG32(PIO_LOCKSR, 0xe0)
	FIELD(PIO_LOCKSR, P0, 0, 1)
	FIELD(PIO_LOCKSR, P1, 1, 1)
	FIELD(PIO_LOCKSR, P2, 2, 1)
	FIELD(PIO_LOCKSR, P3, 3, 1)
	FIELD(PIO_LOCKSR, P4, 4, 1)
	FIELD(PIO_LOCKSR, P5, 5, 1)
	FIELD(PIO_LOCKSR, P6, 6, 1)
	FIELD(PIO_LOCKSR, P7, 7, 1)
	FIELD(PIO_LOCKSR, P8, 8, 1)
	FIELD(PIO_LOCKSR, P9, 9, 1)
	FIELD(PIO_LOCKSR, P10, 10, 1)
	FIELD(PIO_LOCKSR, P11, 11, 1)
	FIELD(PIO_LOCKSR, P12, 12, 1)
	FIELD(PIO_LOCKSR, P13, 13, 1)
	FIELD(PIO_LOCKSR, P14, 14, 1)
	FIELD(PIO_LOCKSR, P15, 15, 1)
	FIELD(PIO_LOCKSR, P16, 16, 1)
	FIELD(PIO_LOCKSR, P17, 17, 1)
	FIELD(PIO_LOCKSR, P18, 18, 1)
	FIELD(PIO_LOCKSR, P19, 19, 1)
	FIELD(PIO_LOCKSR, P20, 20, 1)
	FIELD(PIO_LOCKSR, P21, 21, 1)
	FIELD(PIO_LOCKSR, P22, 22, 1)
	FIELD(PIO_LOCKSR, P23, 23, 1)
	FIELD(PIO_LOCKSR, P24, 24, 1)
	FIELD(PIO_LOCKSR, P25, 25, 1)
	FIELD(PIO_LOCKSR, P26, 26, 1)
	FIELD(PIO_LOCKSR, P27, 27, 1)
	FIELD(PIO_LOCKSR, P28, 28, 1)
	FIELD(PIO_LOCKSR, P29, 29, 1)
	FIELD(PIO_LOCKSR, P30, 30, 1)
	FIELD(PIO_LOCKSR, P31, 31, 1)
REG32(PIO_WPMR, 0xe4)
	FIELD(PIO_WPMR, WPEN, 0, 1)
	FIELD(PIO_WPMR, WPKEY, 8, 24)
REG32(PIO_WPSR, 0xe8)
	FIELD(PIO_WPSR, WPVS, 0, 1)
	FIELD(PIO_WPSR, WPVSRC, 8, 16)


__attribute__((unused))
static void atsam3x8e_pio_update(ATSAM3X8EPIO *t) {
	int conditions = (((t->IMR & 0x2000000) && (t->ISR & 0x2000000)) || ((t->IMR & 0x80000000) && (t->ISR & 0x80000000)) || ((t->IMR & 0x8) && (t->ISR & 0x8)) || ((t->IMR & 0x80) && (t->ISR & 0x80)) || ((t->IMR & 0x20000000) && (t->ISR & 0x20000000)) || ((t->IMR & 0x8000) && (t->ISR & 0x8000)) || ((t->IMR & 0x200) && (t->ISR & 0x200)) || ((t->IMR & 0x100) && (t->ISR & 0x100)) || ((t->IMR & 0x1) && (t->ISR & 0x1)) || ((t->IMR & 0x100000) && (t->ISR & 0x100000)) || ((t->IMR & 0x800000) && (t->ISR & 0x800000)) || ((t->IMR & 0x40) && (t->ISR & 0x40)) || ((t->IMR & 0x1000) && (t->ISR & 0x1000)) || ((t->IMR & 0x2) && (t->ISR & 0x2)) || ((t->IMR & 0x10000000) && (t->ISR & 0x10000000)) || ((t->IMR & 0x4000000) && (t->ISR & 0x4000000)) || ((t->IMR & 0x1000000) && (t->ISR & 0x1000000)) || ((t->IMR & 0x200000) && (t->ISR & 0x200000)) || ((t->IMR & 0x4) && (t->ISR & 0x4)) || ((t->IMR & 0x4000) && (t->ISR & 0x4000)) || ((t->IMR & 0x40000) && (t->ISR & 0x40000)) || ((t->IMR & 0x40000000) && (t->ISR & 0x40000000)) || ((t->IMR & 0x800) && (t->ISR & 0x800)) || ((t->IMR & 0x80000) && (t->ISR & 0x80000)) || ((t->IMR & 0x8000000) && (t->ISR & 0x8000000)) || ((t->IMR & 0x400) && (t->ISR & 0x400)) || ((t->IMR & 0x20) && (t->ISR & 0x20)) || ((t->IMR & 0x10) && (t->ISR & 0x10)) || ((t->IMR & 0x10000) && (t->ISR & 0x10000)) || ((t->IMR & 0x400000) && (t->ISR & 0x400000)) || ((t->IMR & 0x2000) && (t->ISR & 0x2000)) || ((t->ISR & 0x20000000) && (t->IMR & 0x20000000)) || ((t->ISR & 0x10000000) && (t->IMR & 0x10000000)) || ((t->IMR & 0x20000) && (t->ISR & 0x20000)) || ((t->ISR & 0x80000000) && (t->IMR & 0x80000000)) || ((t->ISR & 0x40000000) && (t->IMR & 0x40000000)));
	qemu_set_irq(t->irq[0], conditions);
	qemu_set_irq(t->irq[0], 0);

}

static int atsam3x8e_pio_can_receive(void *opaque) {
	return 1;
}

static void atsam3x8e_pio_receive(void *opaque, const uint8_t *buf, int size) {
	ATSAM3X8EPIO *t = ATSAM3X8E_PIO(opaque);

	t->IMR = (1 << (*buf % 32));
	t->ISR = (1 << (*buf % 32));

	atsam3x8e_pio_update(t);
}

static void atsam3x8e_pio_register_reset(ATSAM3X8EPIO *t) {
	t->OSR = 0x0;
	t->IFSR = 0x0;
	t->IMR = 0x0;
	t->ISR = 0x0;
	t->MDSR = 0x0;
	t->PUSR = 0x0;
	t->ABSR = 0x0;
	t->IFDGSR = 0x0;
	t->SCDR = 0x0;
	t->OWSR = 0x0;
	t->AIMMR = 0x0;
	t->ELSR = 0x0;
	t->FRLHSR = 0x0;
	t->LOCKSR = 0x0;
	t->WPMR = 0x0;
	t->WPSR = 0x0;

}

static uint64_t atsam3x8e_pio_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAM3X8EPIO *t = ATSAM3X8E_PIO(opaque);
	uint64_t ret;

	switch (offset) {
		case A_PIO_PSR:
			ret = t->PSR;
			break;
		case A_PIO_OSR:
			ret = t->OSR;
			break;
		case A_PIO_IFSR:
			ret = t->IFSR;
			break;
		case A_PIO_ODSR:
			ret = t->ODSR;
			break;
		case A_PIO_PDSR:
			ret = t->PDSR;
			break;
		case A_PIO_IMR:
			ret = t->IMR;
			break;
		case A_PIO_ISR:
			ret = t->ISR;
			break;
		case A_PIO_MDSR:
			ret = t->MDSR;
			break;
		case A_PIO_PUSR:
			ret = t->PUSR;
			break;
		case A_PIO_ABSR:
			ret = t->ABSR;
			break;
		case A_PIO_IFDGSR:
			ret = t->IFDGSR;
			break;
		case A_PIO_SCDR:
			ret = t->SCDR;
			break;
		case A_PIO_OWSR:
			ret = t->OWSR;
			break;
		case A_PIO_AIMMR:
			ret = t->AIMMR;
			break;
		case A_PIO_ELSR:
			ret = t->ELSR;
			break;
		case A_PIO_FRLHSR:
			ret = t->FRLHSR;
			break;
		case A_PIO_LOCKSR:
			ret = t->LOCKSR;
			break;
		case A_PIO_WPMR:
			ret = t->WPMR;
			break;
		case A_PIO_WPSR:
			ret = t->WPSR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM3X8E PIO read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsam3x8e_pio_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAM3X8EPIO *t = ATSAM3X8E_PIO(opaque);


	switch (offset) {
		case A_PIO_PER ... A_PIO_PER + 3:
			value = value << ((offset - A_PIO_PER) << 3);
			offset = A_PIO_PER;
			break;
		case A_PIO_PDR ... A_PIO_PDR + 3:
			value = value << ((offset - A_PIO_PDR) << 3);
			offset = A_PIO_PDR;
			break;
		case A_PIO_OER ... A_PIO_OER + 3:
			value = value << ((offset - A_PIO_OER) << 3);
			offset = A_PIO_OER;
			break;
		case A_PIO_ODR ... A_PIO_ODR + 3:
			value = value << ((offset - A_PIO_ODR) << 3);
			offset = A_PIO_ODR;
			break;
		case A_PIO_IFER ... A_PIO_IFER + 3:
			value = value << ((offset - A_PIO_IFER) << 3);
			offset = A_PIO_IFER;
			break;
		case A_PIO_IFDR ... A_PIO_IFDR + 3:
			value = value << ((offset - A_PIO_IFDR) << 3);
			offset = A_PIO_IFDR;
			break;
		case A_PIO_SODR ... A_PIO_SODR + 3:
			value = value << ((offset - A_PIO_SODR) << 3);
			offset = A_PIO_SODR;
			break;
		case A_PIO_CODR ... A_PIO_CODR + 3:
			value = value << ((offset - A_PIO_CODR) << 3);
			offset = A_PIO_CODR;
			break;
		case A_PIO_ODSR ... A_PIO_ODSR + 3:
			value = value << ((offset - A_PIO_ODSR) << 3);
			offset = A_PIO_ODSR;
			break;
		case A_PIO_IER ... A_PIO_IER + 3:
			value = value << ((offset - A_PIO_IER) << 3);
			offset = A_PIO_IER;
			break;
		case A_PIO_IDR ... A_PIO_IDR + 3:
			value = value << ((offset - A_PIO_IDR) << 3);
			offset = A_PIO_IDR;
			break;
		case A_PIO_MDER ... A_PIO_MDER + 3:
			value = value << ((offset - A_PIO_MDER) << 3);
			offset = A_PIO_MDER;
			break;
		case A_PIO_MDDR ... A_PIO_MDDR + 3:
			value = value << ((offset - A_PIO_MDDR) << 3);
			offset = A_PIO_MDDR;
			break;
		case A_PIO_PUDR ... A_PIO_PUDR + 3:
			value = value << ((offset - A_PIO_PUDR) << 3);
			offset = A_PIO_PUDR;
			break;
		case A_PIO_PUER ... A_PIO_PUER + 3:
			value = value << ((offset - A_PIO_PUER) << 3);
			offset = A_PIO_PUER;
			break;
		case A_PIO_ABSR ... A_PIO_ABSR + 3:
			value = value << ((offset - A_PIO_ABSR) << 3);
			offset = A_PIO_ABSR;
			break;
		case A_PIO_SCIFSR ... A_PIO_SCIFSR + 3:
			value = value << ((offset - A_PIO_SCIFSR) << 3);
			offset = A_PIO_SCIFSR;
			break;
		case A_PIO_DIFSR ... A_PIO_DIFSR + 3:
			value = value << ((offset - A_PIO_DIFSR) << 3);
			offset = A_PIO_DIFSR;
			break;
		case A_PIO_SCDR ... A_PIO_SCDR + 3:
			value = value << ((offset - A_PIO_SCDR) << 3);
			offset = A_PIO_SCDR;
			break;
		case A_PIO_OWER ... A_PIO_OWER + 3:
			value = value << ((offset - A_PIO_OWER) << 3);
			offset = A_PIO_OWER;
			break;
		case A_PIO_OWDR ... A_PIO_OWDR + 3:
			value = value << ((offset - A_PIO_OWDR) << 3);
			offset = A_PIO_OWDR;
			break;
		case A_PIO_AIMER ... A_PIO_AIMER + 3:
			value = value << ((offset - A_PIO_AIMER) << 3);
			offset = A_PIO_AIMER;
			break;
		case A_PIO_AIMDR ... A_PIO_AIMDR + 3:
			value = value << ((offset - A_PIO_AIMDR) << 3);
			offset = A_PIO_AIMDR;
			break;
		case A_PIO_ESR ... A_PIO_ESR + 3:
			value = value << ((offset - A_PIO_ESR) << 3);
			offset = A_PIO_ESR;
			break;
		case A_PIO_LSR ... A_PIO_LSR + 3:
			value = value << ((offset - A_PIO_LSR) << 3);
			offset = A_PIO_LSR;
			break;
		case A_PIO_FELLSR ... A_PIO_FELLSR + 3:
			value = value << ((offset - A_PIO_FELLSR) << 3);
			offset = A_PIO_FELLSR;
			break;
		case A_PIO_REHLSR ... A_PIO_REHLSR + 3:
			value = value << ((offset - A_PIO_REHLSR) << 3);
			offset = A_PIO_REHLSR;
			break;
		case A_PIO_WPMR ... A_PIO_WPMR + 3:
			value = value << ((offset - A_PIO_WPMR) << 3);
			offset = A_PIO_WPMR;
			break;

		default: break;
	}

	switch (offset) {
		case A_PIO_PER:
			t->PER = value;
			break;
		case A_PIO_PDR:
			t->PDR = value;
			break;
		case A_PIO_OER:
			t->OER = value;
			break;
		case A_PIO_ODR:
			t->ODR = value;
			break;
		case A_PIO_IFER:
			t->IFER = value;
			break;
		case A_PIO_IFDR:
			t->IFDR = value;
			break;
		case A_PIO_SODR:
			t->SODR = value;
			break;
		case A_PIO_CODR:
			t->CODR = value;
			break;
		case A_PIO_ODSR:
			t->ODSR = value;
			break;
		case A_PIO_IER:
			t->IER = value;
			break;
		case A_PIO_IDR:
			t->IDR = value;
			break;
		case A_PIO_MDER:
			t->MDER = value;
			break;
		case A_PIO_MDDR:
			t->MDDR = value;
			break;
		case A_PIO_PUDR:
			t->PUDR = value;
			break;
		case A_PIO_PUER:
			t->PUER = value;
			break;
		case A_PIO_ABSR:
			t->ABSR = value;
			break;
		case A_PIO_SCIFSR:
			t->SCIFSR = value;
			break;
		case A_PIO_DIFSR:
			t->DIFSR = value;
			break;
		case A_PIO_SCDR:
			t->SCDR = value;
			break;
		case A_PIO_OWER:
			t->OWER = value;
			break;
		case A_PIO_OWDR:
			t->OWDR = value;
			break;
		case A_PIO_AIMER:
			t->AIMER = value;
			break;
		case A_PIO_AIMDR:
			t->AIMDR = value;
			break;
		case A_PIO_ESR:
			t->ESR = value;
			break;
		case A_PIO_LSR:
			t->LSR = value;
			break;
		case A_PIO_FELLSR:
			t->FELLSR = value;
			break;
		case A_PIO_REHLSR:
			t->REHLSR = value;
			break;
		case A_PIO_WPMR:
			t->WPMR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM3X8E PIO write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsam3x8e_pio_ops = {
	.read = atsam3x8e_pio_read,
	.write = atsam3x8e_pio_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsam3x8e_pio_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAM3X8EPIO *t = ATSAM3X8E_PIO(obj);
	memory_region_init_io(&(t->iomem), obj, &atsam3x8e_pio_ops, t, "atsam3x8e-pio", ATSAM3X8E_PIO_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void atsam3x8e_pio_realize(DeviceState *dev, Error **errp) {
	ATSAM3X8EPIO *t = ATSAM3X8E_PIO(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), atsam3x8e_pio_can_receive, atsam3x8e_pio_receive,
		NULL, NULL, t, NULL, true);

}

static void atsam3x8e_pio_reset_enter(Object *obj, ResetType type) {
	ATSAM3X8EPIO *t = ATSAM3X8E_PIO(obj);
	atsam3x8e_pio_register_reset(t);
}

static const VMStateDescription atsam3x8e_pio_vmstate = {
	.name = "atsam3x8e-pio",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(PER, ATSAM3X8EPIO),
		VMSTATE_UINT32(PDR, ATSAM3X8EPIO),
		VMSTATE_UINT32(PSR, ATSAM3X8EPIO),
		VMSTATE_UINT32(OER, ATSAM3X8EPIO),
		VMSTATE_UINT32(ODR, ATSAM3X8EPIO),
		VMSTATE_UINT32(OSR, ATSAM3X8EPIO),
		VMSTATE_UINT32(IFER, ATSAM3X8EPIO),
		VMSTATE_UINT32(IFDR, ATSAM3X8EPIO),
		VMSTATE_UINT32(IFSR, ATSAM3X8EPIO),
		VMSTATE_UINT32(SODR, ATSAM3X8EPIO),
		VMSTATE_UINT32(CODR, ATSAM3X8EPIO),
		VMSTATE_UINT32(ODSR, ATSAM3X8EPIO),
		VMSTATE_UINT32(PDSR, ATSAM3X8EPIO),
		VMSTATE_UINT32(IER, ATSAM3X8EPIO),
		VMSTATE_UINT32(IDR, ATSAM3X8EPIO),
		VMSTATE_UINT32(IMR, ATSAM3X8EPIO),
		VMSTATE_UINT32(ISR, ATSAM3X8EPIO),
		VMSTATE_UINT32(MDER, ATSAM3X8EPIO),
		VMSTATE_UINT32(MDDR, ATSAM3X8EPIO),
		VMSTATE_UINT32(MDSR, ATSAM3X8EPIO),
		VMSTATE_UINT32(PUDR, ATSAM3X8EPIO),
		VMSTATE_UINT32(PUER, ATSAM3X8EPIO),
		VMSTATE_UINT32(PUSR, ATSAM3X8EPIO),
		VMSTATE_UINT32(ABSR, ATSAM3X8EPIO),
		VMSTATE_UINT32(SCIFSR, ATSAM3X8EPIO),
		VMSTATE_UINT32(DIFSR, ATSAM3X8EPIO),
		VMSTATE_UINT32(IFDGSR, ATSAM3X8EPIO),
		VMSTATE_UINT32(SCDR, ATSAM3X8EPIO),
		VMSTATE_UINT32(OWER, ATSAM3X8EPIO),
		VMSTATE_UINT32(OWDR, ATSAM3X8EPIO),
		VMSTATE_UINT32(OWSR, ATSAM3X8EPIO),
		VMSTATE_UINT32(AIMER, ATSAM3X8EPIO),
		VMSTATE_UINT32(AIMDR, ATSAM3X8EPIO),
		VMSTATE_UINT32(AIMMR, ATSAM3X8EPIO),
		VMSTATE_UINT32(ESR, ATSAM3X8EPIO),
		VMSTATE_UINT32(LSR, ATSAM3X8EPIO),
		VMSTATE_UINT32(ELSR, ATSAM3X8EPIO),
		VMSTATE_UINT32(FELLSR, ATSAM3X8EPIO),
		VMSTATE_UINT32(REHLSR, ATSAM3X8EPIO),
		VMSTATE_UINT32(FRLHSR, ATSAM3X8EPIO),
		VMSTATE_UINT32(LOCKSR, ATSAM3X8EPIO),
		VMSTATE_UINT32(WPMR, ATSAM3X8EPIO),
		VMSTATE_UINT32(WPSR, ATSAM3X8EPIO),

		VMSTATE_END_OF_LIST()
	}
};

static Property atsam3x8e_pio_properties[] = {
	DEFINE_PROP_CHR("chardev", ATSAM3X8EPIO, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void atsam3x8e_pio_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsam3x8e_pio_vmstate;
	dc->realize = atsam3x8e_pio_realize;
	rc->phases.enter = atsam3x8e_pio_reset_enter;
	device_class_set_props(dc, atsam3x8e_pio_properties);

}

static const TypeInfo atsam3x8e_pio_info = {
	.name = TYPE_ATSAM3X8E_PIO,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAM3X8EPIO),
	.instance_init = atsam3x8e_pio_init,
	.class_init = atsam3x8e_pio_class_init,
};

static void atsam3x8e_pio_register_types(void) {
	type_register_static(&atsam3x8e_pio_info);
}

type_init(atsam3x8e_pio_register_types);

#define TYPE_ATSAM3X8E_ADC "atsam3x8e-adc"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAM3X8EADC, ATSAM3X8E_ADC)

struct ATSAM3X8EADC {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t CR;
	uint32_t MR;
	uint32_t SEQR1;
	uint32_t SEQR2;
	uint32_t CHER;
	uint32_t CHDR;
	uint32_t CHSR;
	uint32_t LCDR;
	uint32_t IER;
	uint32_t IDR;
	uint32_t IMR;
	uint32_t ISR;
	uint32_t OVER;
	uint32_t EMR;
	uint32_t CWR;
	uint32_t CGR;
	uint32_t COR;
	uint32_t ACR;
	uint32_t WPMR;
	uint32_t WPSR;
	uint32_t RPR;
	uint32_t RCR;
	uint32_t RNPR;
	uint32_t RNCR;
	uint32_t PTCR;
	uint32_t PTSR;
	uint32_t CDR0;
	uint32_t CDR1;
	uint32_t CDR2;
	uint32_t CDR3;
	uint32_t CDR4;
	uint32_t CDR5;
	uint32_t CDR6;
	uint32_t CDR7;
	uint32_t CDR8;
	uint32_t CDR9;
	uint32_t CDR10;
	uint32_t CDR11;
	uint32_t CDR12;
	uint32_t CDR13;
	uint32_t CDR14;
	uint32_t CDR15;
	/* chardev backend */
	CharBackend chr;
	guint watch_tag;

	/* base */
	uint32_t base;

};
#define ATSAM3X8E_ADC_SIZE				0x128

REG32(ADC_CR, 0x0)
	FIELD(ADC_CR, SWRST, 0, 1)
	FIELD(ADC_CR, START, 1, 1)
REG32(ADC_MR, 0x4)
	FIELD(ADC_MR, TRGEN, 0, 1)
	FIELD(ADC_MR, TRGSEL, 1, 3)
	FIELD(ADC_MR, SLEEP, 5, 1)
	FIELD(ADC_MR, FWUP, 6, 1)
	FIELD(ADC_MR, FREERUN, 7, 1)
	FIELD(ADC_MR, PRESCAL, 8, 8)
	FIELD(ADC_MR, STARTUP, 16, 4)
	FIELD(ADC_MR, SETTLING, 20, 2)
	FIELD(ADC_MR, ANACH, 23, 1)
	FIELD(ADC_MR, TRACKTIM, 24, 4)
	FIELD(ADC_MR, TRANSFER, 28, 2)
	FIELD(ADC_MR, USEQ, 31, 1)
REG32(ADC_SEQR1, 0x8)
	FIELD(ADC_SEQR1, USCH1, 0, 4)
	FIELD(ADC_SEQR1, USCH2, 4, 4)
	FIELD(ADC_SEQR1, USCH3, 8, 4)
	FIELD(ADC_SEQR1, USCH4, 12, 4)
	FIELD(ADC_SEQR1, USCH5, 16, 4)
	FIELD(ADC_SEQR1, USCH6, 20, 4)
	FIELD(ADC_SEQR1, USCH7, 24, 4)
	FIELD(ADC_SEQR1, USCH8, 28, 4)
REG32(ADC_SEQR2, 0xc)
	FIELD(ADC_SEQR2, USCH9, 0, 4)
	FIELD(ADC_SEQR2, USCH10, 4, 4)
	FIELD(ADC_SEQR2, USCH11, 8, 4)
	FIELD(ADC_SEQR2, USCH12, 12, 4)
	FIELD(ADC_SEQR2, USCH13, 16, 4)
	FIELD(ADC_SEQR2, USCH14, 20, 4)
	FIELD(ADC_SEQR2, USCH15, 24, 4)
REG32(ADC_CHER, 0x10)
	FIELD(ADC_CHER, CH0, 0, 1)
	FIELD(ADC_CHER, CH1, 1, 1)
	FIELD(ADC_CHER, CH2, 2, 1)
	FIELD(ADC_CHER, CH3, 3, 1)
	FIELD(ADC_CHER, CH4, 4, 1)
	FIELD(ADC_CHER, CH5, 5, 1)
	FIELD(ADC_CHER, CH6, 6, 1)
	FIELD(ADC_CHER, CH7, 7, 1)
	FIELD(ADC_CHER, CH8, 8, 1)
	FIELD(ADC_CHER, CH9, 9, 1)
	FIELD(ADC_CHER, CH10, 10, 1)
	FIELD(ADC_CHER, CH11, 11, 1)
	FIELD(ADC_CHER, CH12, 12, 1)
	FIELD(ADC_CHER, CH13, 13, 1)
	FIELD(ADC_CHER, CH14, 14, 1)
	FIELD(ADC_CHER, CH15, 15, 1)
REG32(ADC_CHDR, 0x14)
	FIELD(ADC_CHDR, CH0, 0, 1)
	FIELD(ADC_CHDR, CH1, 1, 1)
	FIELD(ADC_CHDR, CH2, 2, 1)
	FIELD(ADC_CHDR, CH3, 3, 1)
	FIELD(ADC_CHDR, CH4, 4, 1)
	FIELD(ADC_CHDR, CH5, 5, 1)
	FIELD(ADC_CHDR, CH6, 6, 1)
	FIELD(ADC_CHDR, CH7, 7, 1)
	FIELD(ADC_CHDR, CH8, 8, 1)
	FIELD(ADC_CHDR, CH9, 9, 1)
	FIELD(ADC_CHDR, CH10, 10, 1)
	FIELD(ADC_CHDR, CH11, 11, 1)
	FIELD(ADC_CHDR, CH12, 12, 1)
	FIELD(ADC_CHDR, CH13, 13, 1)
	FIELD(ADC_CHDR, CH14, 14, 1)
	FIELD(ADC_CHDR, CH15, 15, 1)
REG32(ADC_CHSR, 0x18)
	FIELD(ADC_CHSR, CH0, 0, 1)
	FIELD(ADC_CHSR, CH1, 1, 1)
	FIELD(ADC_CHSR, CH2, 2, 1)
	FIELD(ADC_CHSR, CH3, 3, 1)
	FIELD(ADC_CHSR, CH4, 4, 1)
	FIELD(ADC_CHSR, CH5, 5, 1)
	FIELD(ADC_CHSR, CH6, 6, 1)
	FIELD(ADC_CHSR, CH7, 7, 1)
	FIELD(ADC_CHSR, CH8, 8, 1)
	FIELD(ADC_CHSR, CH9, 9, 1)
	FIELD(ADC_CHSR, CH10, 10, 1)
	FIELD(ADC_CHSR, CH11, 11, 1)
	FIELD(ADC_CHSR, CH12, 12, 1)
	FIELD(ADC_CHSR, CH13, 13, 1)
	FIELD(ADC_CHSR, CH14, 14, 1)
	FIELD(ADC_CHSR, CH15, 15, 1)
REG32(ADC_LCDR, 0x20)
	FIELD(ADC_LCDR, LDATA, 0, 12)
	FIELD(ADC_LCDR, CHNB, 12, 4)
REG32(ADC_IER, 0x24)
	FIELD(ADC_IER, EOC0, 0, 1)
	FIELD(ADC_IER, EOC1, 1, 1)
	FIELD(ADC_IER, EOC2, 2, 1)
	FIELD(ADC_IER, EOC3, 3, 1)
	FIELD(ADC_IER, EOC4, 4, 1)
	FIELD(ADC_IER, EOC5, 5, 1)
	FIELD(ADC_IER, EOC6, 6, 1)
	FIELD(ADC_IER, EOC7, 7, 1)
	FIELD(ADC_IER, EOC8, 8, 1)
	FIELD(ADC_IER, EOC9, 9, 1)
	FIELD(ADC_IER, EOC10, 10, 1)
	FIELD(ADC_IER, EOC11, 11, 1)
	FIELD(ADC_IER, EOC12, 12, 1)
	FIELD(ADC_IER, EOC13, 13, 1)
	FIELD(ADC_IER, EOC14, 14, 1)
	FIELD(ADC_IER, EOC15, 15, 1)
	FIELD(ADC_IER, DRDY, 24, 1)
	FIELD(ADC_IER, GOVRE, 25, 1)
	FIELD(ADC_IER, COMPE, 26, 1)
	FIELD(ADC_IER, ENDRX, 27, 1)
	FIELD(ADC_IER, RXBUFF, 28, 1)
REG32(ADC_IDR, 0x28)
	FIELD(ADC_IDR, EOC0, 0, 1)
	FIELD(ADC_IDR, EOC1, 1, 1)
	FIELD(ADC_IDR, EOC2, 2, 1)
	FIELD(ADC_IDR, EOC3, 3, 1)
	FIELD(ADC_IDR, EOC4, 4, 1)
	FIELD(ADC_IDR, EOC5, 5, 1)
	FIELD(ADC_IDR, EOC6, 6, 1)
	FIELD(ADC_IDR, EOC7, 7, 1)
	FIELD(ADC_IDR, EOC8, 8, 1)
	FIELD(ADC_IDR, EOC9, 9, 1)
	FIELD(ADC_IDR, EOC10, 10, 1)
	FIELD(ADC_IDR, EOC11, 11, 1)
	FIELD(ADC_IDR, EOC12, 12, 1)
	FIELD(ADC_IDR, EOC13, 13, 1)
	FIELD(ADC_IDR, EOC14, 14, 1)
	FIELD(ADC_IDR, EOC15, 15, 1)
	FIELD(ADC_IDR, DRDY, 24, 1)
	FIELD(ADC_IDR, GOVRE, 25, 1)
	FIELD(ADC_IDR, COMPE, 26, 1)
	FIELD(ADC_IDR, ENDRX, 27, 1)
	FIELD(ADC_IDR, RXBUFF, 28, 1)
REG32(ADC_IMR, 0x2c)
	FIELD(ADC_IMR, EOC0, 0, 1)
	FIELD(ADC_IMR, EOC1, 1, 1)
	FIELD(ADC_IMR, EOC2, 2, 1)
	FIELD(ADC_IMR, EOC3, 3, 1)
	FIELD(ADC_IMR, EOC4, 4, 1)
	FIELD(ADC_IMR, EOC5, 5, 1)
	FIELD(ADC_IMR, EOC6, 6, 1)
	FIELD(ADC_IMR, EOC7, 7, 1)
	FIELD(ADC_IMR, EOC8, 8, 1)
	FIELD(ADC_IMR, EOC9, 9, 1)
	FIELD(ADC_IMR, EOC10, 10, 1)
	FIELD(ADC_IMR, EOC11, 11, 1)
	FIELD(ADC_IMR, EOC12, 12, 1)
	FIELD(ADC_IMR, EOC13, 13, 1)
	FIELD(ADC_IMR, EOC14, 14, 1)
	FIELD(ADC_IMR, EOC15, 15, 1)
	FIELD(ADC_IMR, DRDY, 24, 1)
	FIELD(ADC_IMR, GOVRE, 25, 1)
	FIELD(ADC_IMR, COMPE, 26, 1)
	FIELD(ADC_IMR, ENDRX, 27, 1)
	FIELD(ADC_IMR, RXBUFF, 28, 1)
REG32(ADC_ISR, 0x30)
	FIELD(ADC_ISR, EOC0, 0, 1)
	FIELD(ADC_ISR, EOC1, 1, 1)
	FIELD(ADC_ISR, EOC2, 2, 1)
	FIELD(ADC_ISR, EOC3, 3, 1)
	FIELD(ADC_ISR, EOC4, 4, 1)
	FIELD(ADC_ISR, EOC5, 5, 1)
	FIELD(ADC_ISR, EOC6, 6, 1)
	FIELD(ADC_ISR, EOC7, 7, 1)
	FIELD(ADC_ISR, EOC8, 8, 1)
	FIELD(ADC_ISR, EOC9, 9, 1)
	FIELD(ADC_ISR, EOC10, 10, 1)
	FIELD(ADC_ISR, EOC11, 11, 1)
	FIELD(ADC_ISR, EOC12, 12, 1)
	FIELD(ADC_ISR, EOC13, 13, 1)
	FIELD(ADC_ISR, EOC14, 14, 1)
	FIELD(ADC_ISR, EOC15, 15, 1)
	FIELD(ADC_ISR, DRDY, 24, 1)
	FIELD(ADC_ISR, GOVRE, 25, 1)
	FIELD(ADC_ISR, COMPE, 26, 1)
	FIELD(ADC_ISR, ENDRX, 27, 1)
	FIELD(ADC_ISR, RXBUFF, 28, 1)
REG32(ADC_OVER, 0x3c)
	FIELD(ADC_OVER, OVRE0, 0, 1)
	FIELD(ADC_OVER, OVRE1, 1, 1)
	FIELD(ADC_OVER, OVRE2, 2, 1)
	FIELD(ADC_OVER, OVRE3, 3, 1)
	FIELD(ADC_OVER, OVRE4, 4, 1)
	FIELD(ADC_OVER, OVRE5, 5, 1)
	FIELD(ADC_OVER, OVRE6, 6, 1)
	FIELD(ADC_OVER, OVRE7, 7, 1)
	FIELD(ADC_OVER, OVRE8, 8, 1)
	FIELD(ADC_OVER, OVRE9, 9, 1)
	FIELD(ADC_OVER, OVRE10, 10, 1)
	FIELD(ADC_OVER, OVRE11, 11, 1)
	FIELD(ADC_OVER, OVRE12, 12, 1)
	FIELD(ADC_OVER, OVRE13, 13, 1)
	FIELD(ADC_OVER, OVRE14, 14, 1)
	FIELD(ADC_OVER, OVRE15, 15, 1)
REG32(ADC_EMR, 0x40)
	FIELD(ADC_EMR, CMPMODE, 0, 2)
	FIELD(ADC_EMR, CMPSEL, 4, 4)
	FIELD(ADC_EMR, CMPALL, 9, 1)
	FIELD(ADC_EMR, CMPFILTER, 12, 2)
	FIELD(ADC_EMR, TAG, 24, 1)
REG32(ADC_CWR, 0x44)
	FIELD(ADC_CWR, LOWTHRES, 0, 12)
	FIELD(ADC_CWR, HIGHTHRES, 16, 12)
REG32(ADC_CGR, 0x48)
	FIELD(ADC_CGR, GAIN0, 0, 2)
	FIELD(ADC_CGR, GAIN1, 2, 2)
	FIELD(ADC_CGR, GAIN2, 4, 2)
	FIELD(ADC_CGR, GAIN3, 6, 2)
	FIELD(ADC_CGR, GAIN4, 8, 2)
	FIELD(ADC_CGR, GAIN5, 10, 2)
	FIELD(ADC_CGR, GAIN6, 12, 2)
	FIELD(ADC_CGR, GAIN7, 14, 2)
	FIELD(ADC_CGR, GAIN8, 16, 2)
	FIELD(ADC_CGR, GAIN9, 18, 2)
	FIELD(ADC_CGR, GAIN10, 20, 2)
	FIELD(ADC_CGR, GAIN11, 22, 2)
	FIELD(ADC_CGR, GAIN12, 24, 2)
	FIELD(ADC_CGR, GAIN13, 26, 2)
	FIELD(ADC_CGR, GAIN14, 28, 2)
	FIELD(ADC_CGR, GAIN15, 30, 2)
REG32(ADC_COR, 0x4c)
	FIELD(ADC_COR, OFF0, 0, 1)
	FIELD(ADC_COR, OFF1, 1, 1)
	FIELD(ADC_COR, OFF2, 2, 1)
	FIELD(ADC_COR, OFF3, 3, 1)
	FIELD(ADC_COR, OFF4, 4, 1)
	FIELD(ADC_COR, OFF5, 5, 1)
	FIELD(ADC_COR, OFF6, 6, 1)
	FIELD(ADC_COR, OFF7, 7, 1)
	FIELD(ADC_COR, OFF8, 8, 1)
	FIELD(ADC_COR, OFF9, 9, 1)
	FIELD(ADC_COR, OFF10, 10, 1)
	FIELD(ADC_COR, OFF11, 11, 1)
	FIELD(ADC_COR, OFF12, 12, 1)
	FIELD(ADC_COR, OFF13, 13, 1)
	FIELD(ADC_COR, OFF14, 14, 1)
	FIELD(ADC_COR, OFF15, 15, 1)
	FIELD(ADC_COR, DIFF0, 16, 1)
	FIELD(ADC_COR, DIFF1, 17, 1)
	FIELD(ADC_COR, DIFF2, 18, 1)
	FIELD(ADC_COR, DIFF3, 19, 1)
	FIELD(ADC_COR, DIFF4, 20, 1)
	FIELD(ADC_COR, DIFF5, 21, 1)
	FIELD(ADC_COR, DIFF6, 22, 1)
	FIELD(ADC_COR, DIFF7, 23, 1)
	FIELD(ADC_COR, DIFF8, 24, 1)
	FIELD(ADC_COR, DIFF9, 25, 1)
	FIELD(ADC_COR, DIFF10, 26, 1)
	FIELD(ADC_COR, DIFF11, 27, 1)
	FIELD(ADC_COR, DIFF12, 28, 1)
	FIELD(ADC_COR, DIFF13, 29, 1)
	FIELD(ADC_COR, DIFF14, 30, 1)
	FIELD(ADC_COR, DIFF15, 31, 1)
REG32(ADC_ACR, 0x94)
	FIELD(ADC_ACR, TSON, 4, 1)
	FIELD(ADC_ACR, IBCTL, 8, 2)
REG32(ADC_WPMR, 0xe4)
	FIELD(ADC_WPMR, WPEN, 0, 1)
	FIELD(ADC_WPMR, WPKEY, 8, 24)
REG32(ADC_WPSR, 0xe8)
	FIELD(ADC_WPSR, WPVS, 0, 1)
	FIELD(ADC_WPSR, WPVSRC, 8, 16)
REG32(ADC_RPR, 0x100)
	FIELD(ADC_RPR, RXPTR, 0, 32)
REG32(ADC_RCR, 0x104)
	FIELD(ADC_RCR, RXCTR, 0, 16)
REG32(ADC_RNPR, 0x110)
	FIELD(ADC_RNPR, RXNPTR, 0, 32)
REG32(ADC_RNCR, 0x114)
	FIELD(ADC_RNCR, RXNCTR, 0, 16)
REG32(ADC_PTCR, 0x120)
	FIELD(ADC_PTCR, RXTEN, 0, 1)
	FIELD(ADC_PTCR, RXTDIS, 1, 1)
	FIELD(ADC_PTCR, TXTEN, 8, 1)
	FIELD(ADC_PTCR, TXTDIS, 9, 1)
REG32(ADC_PTSR, 0x124)
	FIELD(ADC_PTSR, RXTEN, 0, 1)
	FIELD(ADC_PTSR, TXTEN, 8, 1)
REG32(ADC_CDR0, 0x50)
	FIELD(ADC_CDR0, DATA, 0, 12)
REG32(ADC_CDR1, 0x54)
	FIELD(ADC_CDR1, DATA, 0, 12)
REG32(ADC_CDR2, 0x58)
	FIELD(ADC_CDR2, DATA, 0, 12)
REG32(ADC_CDR3, 0x5c)
	FIELD(ADC_CDR3, DATA, 0, 12)
REG32(ADC_CDR4, 0x60)
	FIELD(ADC_CDR4, DATA, 0, 12)
REG32(ADC_CDR5, 0x64)
	FIELD(ADC_CDR5, DATA, 0, 12)
REG32(ADC_CDR6, 0x68)
	FIELD(ADC_CDR6, DATA, 0, 12)
REG32(ADC_CDR7, 0x6c)
	FIELD(ADC_CDR7, DATA, 0, 12)
REG32(ADC_CDR8, 0x70)
	FIELD(ADC_CDR8, DATA, 0, 12)
REG32(ADC_CDR9, 0x74)
	FIELD(ADC_CDR9, DATA, 0, 12)
REG32(ADC_CDR10, 0x78)
	FIELD(ADC_CDR10, DATA, 0, 12)
REG32(ADC_CDR11, 0x7c)
	FIELD(ADC_CDR11, DATA, 0, 12)
REG32(ADC_CDR12, 0x80)
	FIELD(ADC_CDR12, DATA, 0, 12)
REG32(ADC_CDR13, 0x84)
	FIELD(ADC_CDR13, DATA, 0, 12)
REG32(ADC_CDR14, 0x88)
	FIELD(ADC_CDR14, DATA, 0, 12)
REG32(ADC_CDR15, 0x8c)
	FIELD(ADC_CDR15, DATA, 0, 12)


static int atsam3x8e_adc_can_receive(void *opaque) {
	return 1;
}

static void atsam3x8e_adc_receive(void *opaque, const uint8_t *buf, int size) {
	ATSAM3X8EADC *t = ATSAM3X8E_ADC(opaque);

	t->LCDR = *buf;
	t->ISR |= 0x1000000;


}

static void atsam3x8e_adc_register_reset(ATSAM3X8EADC *t) {
	t->MR = 0x0;
	t->SEQR1 = 0x0;
	t->SEQR2 = 0x0;
	t->CHSR = 0x0;
	t->LCDR = 0x0;
	t->IMR = 0x0;
	t->ISR = 0x0;
	t->OVER = 0x0;
	t->EMR = 0x0;
	t->CWR = 0x0;
	t->CGR = 0x0;
	t->COR = 0x0;
	t->ACR = 0x100;
	t->WPMR = 0x0;
	t->WPSR = 0x0;
	t->RPR = 0x0;
	t->RCR = 0x0;
	t->RNPR = 0x0;
	t->RNCR = 0x0;
	t->PTCR = 0x0;
	t->PTSR = 0x0;

}

static uint64_t atsam3x8e_adc_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAM3X8EADC *t = ATSAM3X8E_ADC(opaque);
	uint64_t ret;

	switch (offset) {
		case A_ADC_MR:
			ret = t->MR;
			break;
		case A_ADC_SEQR1:
			ret = t->SEQR1;
			break;
		case A_ADC_SEQR2:
			ret = t->SEQR2;
			break;
		case A_ADC_CHSR:
			ret = t->CHSR;
			break;
		case A_ADC_LCDR:
			ret = t->LCDR;
			t->ISR &= (~(0x1000000));
			qemu_chr_fe_accept_input(&(t->chr));
			break;
		case A_ADC_IMR:
			ret = t->IMR;
			break;
		case A_ADC_ISR:
			ret = t->ISR;
			break;
		case A_ADC_OVER:
			ret = t->OVER;
			break;
		case A_ADC_EMR:
			ret = t->EMR;
			break;
		case A_ADC_CWR:
			ret = t->CWR;
			break;
		case A_ADC_CGR:
			ret = t->CGR;
			break;
		case A_ADC_COR:
			ret = t->COR;
			break;
		case A_ADC_ACR:
			ret = t->ACR;
			break;
		case A_ADC_WPMR:
			ret = t->WPMR;
			break;
		case A_ADC_WPSR:
			ret = t->WPSR;
			break;
		case A_ADC_RPR:
			ret = t->RPR;
			break;
		case A_ADC_RCR:
			ret = t->RCR;
			break;
		case A_ADC_RNPR:
			ret = t->RNPR;
			break;
		case A_ADC_RNCR:
			ret = t->RNCR;
			break;
		case A_ADC_PTSR:
			ret = t->PTSR;
			break;
		case A_ADC_CDR0:
			ret = t->CDR0;
			break;
		case A_ADC_CDR1:
			ret = t->CDR1;
			break;
		case A_ADC_CDR2:
			ret = t->CDR2;
			break;
		case A_ADC_CDR3:
			ret = t->CDR3;
			break;
		case A_ADC_CDR4:
			ret = t->CDR4;
			break;
		case A_ADC_CDR5:
			ret = t->CDR5;
			break;
		case A_ADC_CDR6:
			ret = t->CDR6;
			break;
		case A_ADC_CDR7:
			ret = t->CDR7;
			break;
		case A_ADC_CDR8:
			ret = t->CDR8;
			break;
		case A_ADC_CDR9:
			ret = t->CDR9;
			break;
		case A_ADC_CDR10:
			ret = t->CDR10;
			break;
		case A_ADC_CDR11:
			ret = t->CDR11;
			break;
		case A_ADC_CDR12:
			ret = t->CDR12;
			break;
		case A_ADC_CDR13:
			ret = t->CDR13;
			break;
		case A_ADC_CDR14:
			ret = t->CDR14;
			break;
		case A_ADC_CDR15:
			ret = t->CDR15;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM3X8E ADC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsam3x8e_adc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAM3X8EADC *t = ATSAM3X8E_ADC(opaque);


	switch (offset) {
		case A_ADC_CR ... A_ADC_CR + 3:
			value = value << ((offset - A_ADC_CR) << 3);
			offset = A_ADC_CR;
			break;
		case A_ADC_MR ... A_ADC_MR + 3:
			value = value << ((offset - A_ADC_MR) << 3);
			offset = A_ADC_MR;
			break;
		case A_ADC_SEQR1 ... A_ADC_SEQR1 + 3:
			value = value << ((offset - A_ADC_SEQR1) << 3);
			offset = A_ADC_SEQR1;
			break;
		case A_ADC_SEQR2 ... A_ADC_SEQR2 + 3:
			value = value << ((offset - A_ADC_SEQR2) << 3);
			offset = A_ADC_SEQR2;
			break;
		case A_ADC_CHER ... A_ADC_CHER + 3:
			value = value << ((offset - A_ADC_CHER) << 3);
			offset = A_ADC_CHER;
			break;
		case A_ADC_CHDR ... A_ADC_CHDR + 3:
			value = value << ((offset - A_ADC_CHDR) << 3);
			offset = A_ADC_CHDR;
			break;
		case A_ADC_IER ... A_ADC_IER + 3:
			value = value << ((offset - A_ADC_IER) << 3);
			offset = A_ADC_IER;
			break;
		case A_ADC_IDR ... A_ADC_IDR + 3:
			value = value << ((offset - A_ADC_IDR) << 3);
			offset = A_ADC_IDR;
			break;
		case A_ADC_EMR ... A_ADC_EMR + 3:
			value = value << ((offset - A_ADC_EMR) << 3);
			offset = A_ADC_EMR;
			break;
		case A_ADC_CWR ... A_ADC_CWR + 3:
			value = value << ((offset - A_ADC_CWR) << 3);
			offset = A_ADC_CWR;
			break;
		case A_ADC_CGR ... A_ADC_CGR + 3:
			value = value << ((offset - A_ADC_CGR) << 3);
			offset = A_ADC_CGR;
			break;
		case A_ADC_COR ... A_ADC_COR + 3:
			value = value << ((offset - A_ADC_COR) << 3);
			offset = A_ADC_COR;
			break;
		case A_ADC_ACR ... A_ADC_ACR + 3:
			value = value << ((offset - A_ADC_ACR) << 3);
			offset = A_ADC_ACR;
			break;
		case A_ADC_WPMR ... A_ADC_WPMR + 3:
			value = value << ((offset - A_ADC_WPMR) << 3);
			offset = A_ADC_WPMR;
			break;
		case A_ADC_RPR ... A_ADC_RPR + 3:
			value = value << ((offset - A_ADC_RPR) << 3);
			offset = A_ADC_RPR;
			break;
		case A_ADC_RCR ... A_ADC_RCR + 3:
			value = value << ((offset - A_ADC_RCR) << 3);
			offset = A_ADC_RCR;
			break;
		case A_ADC_RNPR ... A_ADC_RNPR + 3:
			value = value << ((offset - A_ADC_RNPR) << 3);
			offset = A_ADC_RNPR;
			break;
		case A_ADC_RNCR ... A_ADC_RNCR + 3:
			value = value << ((offset - A_ADC_RNCR) << 3);
			offset = A_ADC_RNCR;
			break;
		case A_ADC_PTCR ... A_ADC_PTCR + 3:
			value = value << ((offset - A_ADC_PTCR) << 3);
			offset = A_ADC_PTCR;
			break;

		default: break;
	}

	switch (offset) {
		case A_ADC_CR:
			t->CR = value;
			break;
		case A_ADC_MR:
			t->MR = value;
			break;
		case A_ADC_SEQR1:
			t->SEQR1 = value;
			break;
		case A_ADC_SEQR2:
			t->SEQR2 = value;
			break;
		case A_ADC_CHER:
			t->CHER = value;
			break;
		case A_ADC_CHDR:
			t->CHDR = value;
			break;
		case A_ADC_IER:
			t->IER = value;
			break;
		case A_ADC_IDR:
			t->IDR = value;
			break;
		case A_ADC_EMR:
			t->EMR = value;
			break;
		case A_ADC_CWR:
			t->CWR = value;
			break;
		case A_ADC_CGR:
			t->CGR = value;
			break;
		case A_ADC_COR:
			t->COR = value;
			break;
		case A_ADC_ACR:
			t->ACR = value;
			break;
		case A_ADC_WPMR:
			t->WPMR = value;
			break;
		case A_ADC_RPR:
			t->RPR = value;
			break;
		case A_ADC_RCR:
			t->RCR = value;
			break;
		case A_ADC_RNPR:
			t->RNPR = value;
			break;
		case A_ADC_RNCR:
			t->RNCR = value;
			break;
		case A_ADC_PTCR:
			t->PTCR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM3X8E ADC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsam3x8e_adc_ops = {
	.read = atsam3x8e_adc_read,
	.write = atsam3x8e_adc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsam3x8e_adc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAM3X8EADC *t = ATSAM3X8E_ADC(obj);
	memory_region_init_io(&(t->iomem), obj, &atsam3x8e_adc_ops, t, "atsam3x8e-adc", ATSAM3X8E_ADC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void atsam3x8e_adc_realize(DeviceState *dev, Error **errp) {
	ATSAM3X8EADC *t = ATSAM3X8E_ADC(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), atsam3x8e_adc_can_receive, atsam3x8e_adc_receive,
		NULL, NULL, t, NULL, true);

}

static void atsam3x8e_adc_reset_enter(Object *obj, ResetType type) {
	ATSAM3X8EADC *t = ATSAM3X8E_ADC(obj);
	atsam3x8e_adc_register_reset(t);
}

static const VMStateDescription atsam3x8e_adc_vmstate = {
	.name = "atsam3x8e-adc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR, ATSAM3X8EADC),
		VMSTATE_UINT32(MR, ATSAM3X8EADC),
		VMSTATE_UINT32(SEQR1, ATSAM3X8EADC),
		VMSTATE_UINT32(SEQR2, ATSAM3X8EADC),
		VMSTATE_UINT32(CHER, ATSAM3X8EADC),
		VMSTATE_UINT32(CHDR, ATSAM3X8EADC),
		VMSTATE_UINT32(CHSR, ATSAM3X8EADC),
		VMSTATE_UINT32(LCDR, ATSAM3X8EADC),
		VMSTATE_UINT32(IER, ATSAM3X8EADC),
		VMSTATE_UINT32(IDR, ATSAM3X8EADC),
		VMSTATE_UINT32(IMR, ATSAM3X8EADC),
		VMSTATE_UINT32(ISR, ATSAM3X8EADC),
		VMSTATE_UINT32(OVER, ATSAM3X8EADC),
		VMSTATE_UINT32(EMR, ATSAM3X8EADC),
		VMSTATE_UINT32(CWR, ATSAM3X8EADC),
		VMSTATE_UINT32(CGR, ATSAM3X8EADC),
		VMSTATE_UINT32(COR, ATSAM3X8EADC),
		VMSTATE_UINT32(ACR, ATSAM3X8EADC),
		VMSTATE_UINT32(WPMR, ATSAM3X8EADC),
		VMSTATE_UINT32(WPSR, ATSAM3X8EADC),
		VMSTATE_UINT32(RPR, ATSAM3X8EADC),
		VMSTATE_UINT32(RCR, ATSAM3X8EADC),
		VMSTATE_UINT32(RNPR, ATSAM3X8EADC),
		VMSTATE_UINT32(RNCR, ATSAM3X8EADC),
		VMSTATE_UINT32(PTCR, ATSAM3X8EADC),
		VMSTATE_UINT32(PTSR, ATSAM3X8EADC),
		VMSTATE_UINT32(CDR0, ATSAM3X8EADC),
		VMSTATE_UINT32(CDR1, ATSAM3X8EADC),
		VMSTATE_UINT32(CDR2, ATSAM3X8EADC),
		VMSTATE_UINT32(CDR3, ATSAM3X8EADC),
		VMSTATE_UINT32(CDR4, ATSAM3X8EADC),
		VMSTATE_UINT32(CDR5, ATSAM3X8EADC),
		VMSTATE_UINT32(CDR6, ATSAM3X8EADC),
		VMSTATE_UINT32(CDR7, ATSAM3X8EADC),
		VMSTATE_UINT32(CDR8, ATSAM3X8EADC),
		VMSTATE_UINT32(CDR9, ATSAM3X8EADC),
		VMSTATE_UINT32(CDR10, ATSAM3X8EADC),
		VMSTATE_UINT32(CDR11, ATSAM3X8EADC),
		VMSTATE_UINT32(CDR12, ATSAM3X8EADC),
		VMSTATE_UINT32(CDR13, ATSAM3X8EADC),
		VMSTATE_UINT32(CDR14, ATSAM3X8EADC),
		VMSTATE_UINT32(CDR15, ATSAM3X8EADC),

		VMSTATE_END_OF_LIST()
	}
};

static Property atsam3x8e_adc_properties[] = {
	DEFINE_PROP_CHR("chardev", ATSAM3X8EADC, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void atsam3x8e_adc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsam3x8e_adc_vmstate;
	dc->realize = atsam3x8e_adc_realize;
	rc->phases.enter = atsam3x8e_adc_reset_enter;
	device_class_set_props(dc, atsam3x8e_adc_properties);

}

static const TypeInfo atsam3x8e_adc_info = {
	.name = TYPE_ATSAM3X8E_ADC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAM3X8EADC),
	.instance_init = atsam3x8e_adc_init,
	.class_init = atsam3x8e_adc_class_init,
};

static void atsam3x8e_adc_register_types(void) {
	type_register_static(&atsam3x8e_adc_info);
}

type_init(atsam3x8e_adc_register_types);

#define TYPE_ATSAM3X8E_TC "atsam3x8e-tc"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAM3X8ETC, ATSAM3X8E_TC)

struct ATSAM3X8ETC {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[3];

	/*registers*/
	uint32_t CCR0;
	uint32_t CMR0;
	uint32_t CMR0_WAVEFORM_MODE;
	uint32_t SMMR0;
	uint32_t CV0;
	uint32_t RA0;
	uint32_t RB0;
	uint32_t RC0;
	uint32_t SR0;
	uint32_t IER0;
	uint32_t IDR0;
	uint32_t IMR0;
	uint32_t CCR1;
	uint32_t CMR1;
	uint32_t CMR1_WAVEFORM_MODE;
	uint32_t SMMR1;
	uint32_t CV1;
	uint32_t RA1;
	uint32_t RB1;
	uint32_t RC1;
	uint32_t SR1;
	uint32_t IER1;
	uint32_t IDR1;
	uint32_t IMR1;
	uint32_t CCR2;
	uint32_t CMR2;
	uint32_t CMR2_WAVEFORM_MODE;
	uint32_t SMMR2;
	uint32_t CV2;
	uint32_t RA2;
	uint32_t RB2;
	uint32_t RC2;
	uint32_t SR2;
	uint32_t IER2;
	uint32_t IDR2;
	uint32_t IMR2;
	uint32_t BCR;
	uint32_t BMR;
	uint32_t QIER;
	uint32_t QIDR;
	uint32_t QIMR;
	uint32_t QISR;
	uint32_t FMR;
	uint32_t WPMR;
	/* timer */
	QEMUTimer *timers[3];
	uint8_t enabled[3];

	uint32_t cur_period[3];

	uint8_t period_set[3];

	uint8_t period_reg_set[3];

	/* base */
	uint32_t base;

};
#define ATSAM3X8E_TC_SIZE				0xe8

REG32(TC_CCR0, 0x0)
	FIELD(TC_CCR0, CLKEN, 0, 1)
	FIELD(TC_CCR0, CLKDIS, 1, 1)
	FIELD(TC_CCR0, SWTRG, 2, 1)
REG32(TC_CMR0, 0x4)
	FIELD(TC_CMR0, TCCLKS, 0, 3)
	FIELD(TC_CMR0, CLKI, 3, 1)
	FIELD(TC_CMR0, BURST, 4, 2)
	FIELD(TC_CMR0, LDBSTOP, 6, 1)
	FIELD(TC_CMR0, LDBDIS, 7, 1)
	FIELD(TC_CMR0, ETRGEDG, 8, 2)
	FIELD(TC_CMR0, ABETRG, 10, 1)
	FIELD(TC_CMR0, CPCTRG, 14, 1)
	FIELD(TC_CMR0, WAVE, 15, 1)
	FIELD(TC_CMR0, LDRA, 16, 2)
	FIELD(TC_CMR0, LDRB, 18, 2)
REG32(TC_CMR0_WAVEFORM_MODE, 0x4)
	FIELD(TC_CMR0_WAVEFORM_MODE, TCCLKS, 0, 3)
	FIELD(TC_CMR0_WAVEFORM_MODE, CLKI, 3, 1)
	FIELD(TC_CMR0_WAVEFORM_MODE, BURST, 4, 2)
	FIELD(TC_CMR0_WAVEFORM_MODE, CPCSTOP, 6, 1)
	FIELD(TC_CMR0_WAVEFORM_MODE, CPCDIS, 7, 1)
	FIELD(TC_CMR0_WAVEFORM_MODE, EEVTEDG, 8, 2)
	FIELD(TC_CMR0_WAVEFORM_MODE, EEVT, 10, 2)
	FIELD(TC_CMR0_WAVEFORM_MODE, ENETRG, 12, 1)
	FIELD(TC_CMR0_WAVEFORM_MODE, WAVSEL, 13, 2)
	FIELD(TC_CMR0_WAVEFORM_MODE, WAVE, 15, 1)
	FIELD(TC_CMR0_WAVEFORM_MODE, ACPA, 16, 2)
	FIELD(TC_CMR0_WAVEFORM_MODE, ACPC, 18, 2)
	FIELD(TC_CMR0_WAVEFORM_MODE, AEEVT, 20, 2)
	FIELD(TC_CMR0_WAVEFORM_MODE, ASWTRG, 22, 2)
	FIELD(TC_CMR0_WAVEFORM_MODE, BCPB, 24, 2)
	FIELD(TC_CMR0_WAVEFORM_MODE, BCPC, 26, 2)
	FIELD(TC_CMR0_WAVEFORM_MODE, BEEVT, 28, 2)
	FIELD(TC_CMR0_WAVEFORM_MODE, BSWTRG, 30, 2)
REG32(TC_SMMR0, 0x8)
	FIELD(TC_SMMR0, GCEN, 0, 1)
	FIELD(TC_SMMR0, DOWN, 1, 1)
REG32(TC_CV0, 0x10)
	FIELD(TC_CV0, CV, 0, 32)
REG32(TC_RA0, 0x14)
	FIELD(TC_RA0, RA, 0, 32)
REG32(TC_RB0, 0x18)
	FIELD(TC_RB0, RB, 0, 32)
REG32(TC_RC0, 0x1c)
	FIELD(TC_RC0, RC, 0, 32)
REG32(TC_SR0, 0x20)
	FIELD(TC_SR0, COVFS, 0, 1)
	FIELD(TC_SR0, LOVRS, 1, 1)
	FIELD(TC_SR0, CPAS, 2, 1)
	FIELD(TC_SR0, CPBS, 3, 1)
	FIELD(TC_SR0, CPCS, 4, 1)
	FIELD(TC_SR0, LDRAS, 5, 1)
	FIELD(TC_SR0, LDRBS, 6, 1)
	FIELD(TC_SR0, ETRGS, 7, 1)
	FIELD(TC_SR0, CLKSTA, 16, 1)
	FIELD(TC_SR0, MTIOA, 17, 1)
	FIELD(TC_SR0, MTIOB, 18, 1)
REG32(TC_IER0, 0x24)
	FIELD(TC_IER0, COVFS, 0, 1)
	FIELD(TC_IER0, LOVRS, 1, 1)
	FIELD(TC_IER0, CPAS, 2, 1)
	FIELD(TC_IER0, CPBS, 3, 1)
	FIELD(TC_IER0, CPCS, 4, 1)
	FIELD(TC_IER0, LDRAS, 5, 1)
	FIELD(TC_IER0, LDRBS, 6, 1)
	FIELD(TC_IER0, ETRGS, 7, 1)
REG32(TC_IDR0, 0x28)
	FIELD(TC_IDR0, COVFS, 0, 1)
	FIELD(TC_IDR0, LOVRS, 1, 1)
	FIELD(TC_IDR0, CPAS, 2, 1)
	FIELD(TC_IDR0, CPBS, 3, 1)
	FIELD(TC_IDR0, CPCS, 4, 1)
	FIELD(TC_IDR0, LDRAS, 5, 1)
	FIELD(TC_IDR0, LDRBS, 6, 1)
	FIELD(TC_IDR0, ETRGS, 7, 1)
REG32(TC_IMR0, 0x2c)
	FIELD(TC_IMR0, COVFS, 0, 1)
	FIELD(TC_IMR0, LOVRS, 1, 1)
	FIELD(TC_IMR0, CPAS, 2, 1)
	FIELD(TC_IMR0, CPBS, 3, 1)
	FIELD(TC_IMR0, CPCS, 4, 1)
	FIELD(TC_IMR0, LDRAS, 5, 1)
	FIELD(TC_IMR0, LDRBS, 6, 1)
	FIELD(TC_IMR0, ETRGS, 7, 1)
REG32(TC_CCR1, 0x40)
	FIELD(TC_CCR1, CLKEN, 0, 1)
	FIELD(TC_CCR1, CLKDIS, 1, 1)
	FIELD(TC_CCR1, SWTRG, 2, 1)
REG32(TC_CMR1, 0x44)
	FIELD(TC_CMR1, TCCLKS, 0, 3)
	FIELD(TC_CMR1, CLKI, 3, 1)
	FIELD(TC_CMR1, BURST, 4, 2)
	FIELD(TC_CMR1, LDBSTOP, 6, 1)
	FIELD(TC_CMR1, LDBDIS, 7, 1)
	FIELD(TC_CMR1, ETRGEDG, 8, 2)
	FIELD(TC_CMR1, ABETRG, 10, 1)
	FIELD(TC_CMR1, CPCTRG, 14, 1)
	FIELD(TC_CMR1, WAVE, 15, 1)
	FIELD(TC_CMR1, LDRA, 16, 2)
	FIELD(TC_CMR1, LDRB, 18, 2)
REG32(TC_CMR1_WAVEFORM_MODE, 0x44)
	FIELD(TC_CMR1_WAVEFORM_MODE, TCCLKS, 0, 3)
	FIELD(TC_CMR1_WAVEFORM_MODE, CLKI, 3, 1)
	FIELD(TC_CMR1_WAVEFORM_MODE, BURST, 4, 2)
	FIELD(TC_CMR1_WAVEFORM_MODE, CPCSTOP, 6, 1)
	FIELD(TC_CMR1_WAVEFORM_MODE, CPCDIS, 7, 1)
	FIELD(TC_CMR1_WAVEFORM_MODE, EEVTEDG, 8, 2)
	FIELD(TC_CMR1_WAVEFORM_MODE, EEVT, 10, 2)
	FIELD(TC_CMR1_WAVEFORM_MODE, ENETRG, 12, 1)
	FIELD(TC_CMR1_WAVEFORM_MODE, WAVSEL, 13, 2)
	FIELD(TC_CMR1_WAVEFORM_MODE, WAVE, 15, 1)
	FIELD(TC_CMR1_WAVEFORM_MODE, ACPA, 16, 2)
	FIELD(TC_CMR1_WAVEFORM_MODE, ACPC, 18, 2)
	FIELD(TC_CMR1_WAVEFORM_MODE, AEEVT, 20, 2)
	FIELD(TC_CMR1_WAVEFORM_MODE, ASWTRG, 22, 2)
	FIELD(TC_CMR1_WAVEFORM_MODE, BCPB, 24, 2)
	FIELD(TC_CMR1_WAVEFORM_MODE, BCPC, 26, 2)
	FIELD(TC_CMR1_WAVEFORM_MODE, BEEVT, 28, 2)
	FIELD(TC_CMR1_WAVEFORM_MODE, BSWTRG, 30, 2)
REG32(TC_SMMR1, 0x48)
	FIELD(TC_SMMR1, GCEN, 0, 1)
	FIELD(TC_SMMR1, DOWN, 1, 1)
REG32(TC_CV1, 0x50)
	FIELD(TC_CV1, CV, 0, 32)
REG32(TC_RA1, 0x54)
	FIELD(TC_RA1, RA, 0, 32)
REG32(TC_RB1, 0x58)
	FIELD(TC_RB1, RB, 0, 32)
REG32(TC_RC1, 0x5c)
	FIELD(TC_RC1, RC, 0, 32)
REG32(TC_SR1, 0x60)
	FIELD(TC_SR1, COVFS, 0, 1)
	FIELD(TC_SR1, LOVRS, 1, 1)
	FIELD(TC_SR1, CPAS, 2, 1)
	FIELD(TC_SR1, CPBS, 3, 1)
	FIELD(TC_SR1, CPCS, 4, 1)
	FIELD(TC_SR1, LDRAS, 5, 1)
	FIELD(TC_SR1, LDRBS, 6, 1)
	FIELD(TC_SR1, ETRGS, 7, 1)
	FIELD(TC_SR1, CLKSTA, 16, 1)
	FIELD(TC_SR1, MTIOA, 17, 1)
	FIELD(TC_SR1, MTIOB, 18, 1)
REG32(TC_IER1, 0x64)
	FIELD(TC_IER1, COVFS, 0, 1)
	FIELD(TC_IER1, LOVRS, 1, 1)
	FIELD(TC_IER1, CPAS, 2, 1)
	FIELD(TC_IER1, CPBS, 3, 1)
	FIELD(TC_IER1, CPCS, 4, 1)
	FIELD(TC_IER1, LDRAS, 5, 1)
	FIELD(TC_IER1, LDRBS, 6, 1)
	FIELD(TC_IER1, ETRGS, 7, 1)
REG32(TC_IDR1, 0x68)
	FIELD(TC_IDR1, COVFS, 0, 1)
	FIELD(TC_IDR1, LOVRS, 1, 1)
	FIELD(TC_IDR1, CPAS, 2, 1)
	FIELD(TC_IDR1, CPBS, 3, 1)
	FIELD(TC_IDR1, CPCS, 4, 1)
	FIELD(TC_IDR1, LDRAS, 5, 1)
	FIELD(TC_IDR1, LDRBS, 6, 1)
	FIELD(TC_IDR1, ETRGS, 7, 1)
REG32(TC_IMR1, 0x6c)
	FIELD(TC_IMR1, COVFS, 0, 1)
	FIELD(TC_IMR1, LOVRS, 1, 1)
	FIELD(TC_IMR1, CPAS, 2, 1)
	FIELD(TC_IMR1, CPBS, 3, 1)
	FIELD(TC_IMR1, CPCS, 4, 1)
	FIELD(TC_IMR1, LDRAS, 5, 1)
	FIELD(TC_IMR1, LDRBS, 6, 1)
	FIELD(TC_IMR1, ETRGS, 7, 1)
REG32(TC_CCR2, 0x80)
	FIELD(TC_CCR2, CLKEN, 0, 1)
	FIELD(TC_CCR2, CLKDIS, 1, 1)
	FIELD(TC_CCR2, SWTRG, 2, 1)
REG32(TC_CMR2, 0x84)
	FIELD(TC_CMR2, TCCLKS, 0, 3)
	FIELD(TC_CMR2, CLKI, 3, 1)
	FIELD(TC_CMR2, BURST, 4, 2)
	FIELD(TC_CMR2, LDBSTOP, 6, 1)
	FIELD(TC_CMR2, LDBDIS, 7, 1)
	FIELD(TC_CMR2, ETRGEDG, 8, 2)
	FIELD(TC_CMR2, ABETRG, 10, 1)
	FIELD(TC_CMR2, CPCTRG, 14, 1)
	FIELD(TC_CMR2, WAVE, 15, 1)
	FIELD(TC_CMR2, LDRA, 16, 2)
	FIELD(TC_CMR2, LDRB, 18, 2)
REG32(TC_CMR2_WAVEFORM_MODE, 0x84)
	FIELD(TC_CMR2_WAVEFORM_MODE, TCCLKS, 0, 3)
	FIELD(TC_CMR2_WAVEFORM_MODE, CLKI, 3, 1)
	FIELD(TC_CMR2_WAVEFORM_MODE, BURST, 4, 2)
	FIELD(TC_CMR2_WAVEFORM_MODE, CPCSTOP, 6, 1)
	FIELD(TC_CMR2_WAVEFORM_MODE, CPCDIS, 7, 1)
	FIELD(TC_CMR2_WAVEFORM_MODE, EEVTEDG, 8, 2)
	FIELD(TC_CMR2_WAVEFORM_MODE, EEVT, 10, 2)
	FIELD(TC_CMR2_WAVEFORM_MODE, ENETRG, 12, 1)
	FIELD(TC_CMR2_WAVEFORM_MODE, WAVSEL, 13, 2)
	FIELD(TC_CMR2_WAVEFORM_MODE, WAVE, 15, 1)
	FIELD(TC_CMR2_WAVEFORM_MODE, ACPA, 16, 2)
	FIELD(TC_CMR2_WAVEFORM_MODE, ACPC, 18, 2)
	FIELD(TC_CMR2_WAVEFORM_MODE, AEEVT, 20, 2)
	FIELD(TC_CMR2_WAVEFORM_MODE, ASWTRG, 22, 2)
	FIELD(TC_CMR2_WAVEFORM_MODE, BCPB, 24, 2)
	FIELD(TC_CMR2_WAVEFORM_MODE, BCPC, 26, 2)
	FIELD(TC_CMR2_WAVEFORM_MODE, BEEVT, 28, 2)
	FIELD(TC_CMR2_WAVEFORM_MODE, BSWTRG, 30, 2)
REG32(TC_SMMR2, 0x88)
	FIELD(TC_SMMR2, GCEN, 0, 1)
	FIELD(TC_SMMR2, DOWN, 1, 1)
REG32(TC_CV2, 0x90)
	FIELD(TC_CV2, CV, 0, 32)
REG32(TC_RA2, 0x94)
	FIELD(TC_RA2, RA, 0, 32)
REG32(TC_RB2, 0x98)
	FIELD(TC_RB2, RB, 0, 32)
REG32(TC_RC2, 0x9c)
	FIELD(TC_RC2, RC, 0, 32)
REG32(TC_SR2, 0xa0)
	FIELD(TC_SR2, COVFS, 0, 1)
	FIELD(TC_SR2, LOVRS, 1, 1)
	FIELD(TC_SR2, CPAS, 2, 1)
	FIELD(TC_SR2, CPBS, 3, 1)
	FIELD(TC_SR2, CPCS, 4, 1)
	FIELD(TC_SR2, LDRAS, 5, 1)
	FIELD(TC_SR2, LDRBS, 6, 1)
	FIELD(TC_SR2, ETRGS, 7, 1)
	FIELD(TC_SR2, CLKSTA, 16, 1)
	FIELD(TC_SR2, MTIOA, 17, 1)
	FIELD(TC_SR2, MTIOB, 18, 1)
REG32(TC_IER2, 0xa4)
	FIELD(TC_IER2, COVFS, 0, 1)
	FIELD(TC_IER2, LOVRS, 1, 1)
	FIELD(TC_IER2, CPAS, 2, 1)
	FIELD(TC_IER2, CPBS, 3, 1)
	FIELD(TC_IER2, CPCS, 4, 1)
	FIELD(TC_IER2, LDRAS, 5, 1)
	FIELD(TC_IER2, LDRBS, 6, 1)
	FIELD(TC_IER2, ETRGS, 7, 1)
REG32(TC_IDR2, 0xa8)
	FIELD(TC_IDR2, COVFS, 0, 1)
	FIELD(TC_IDR2, LOVRS, 1, 1)
	FIELD(TC_IDR2, CPAS, 2, 1)
	FIELD(TC_IDR2, CPBS, 3, 1)
	FIELD(TC_IDR2, CPCS, 4, 1)
	FIELD(TC_IDR2, LDRAS, 5, 1)
	FIELD(TC_IDR2, LDRBS, 6, 1)
	FIELD(TC_IDR2, ETRGS, 7, 1)
REG32(TC_IMR2, 0xac)
	FIELD(TC_IMR2, COVFS, 0, 1)
	FIELD(TC_IMR2, LOVRS, 1, 1)
	FIELD(TC_IMR2, CPAS, 2, 1)
	FIELD(TC_IMR2, CPBS, 3, 1)
	FIELD(TC_IMR2, CPCS, 4, 1)
	FIELD(TC_IMR2, LDRAS, 5, 1)
	FIELD(TC_IMR2, LDRBS, 6, 1)
	FIELD(TC_IMR2, ETRGS, 7, 1)
REG32(TC_BCR, 0xc0)
	FIELD(TC_BCR, SYNC, 0, 1)
REG32(TC_BMR, 0xc4)
	FIELD(TC_BMR, TC0XC0S, 0, 2)
	FIELD(TC_BMR, TC1XC1S, 2, 2)
	FIELD(TC_BMR, TC2XC2S, 4, 2)
	FIELD(TC_BMR, QDEN, 8, 1)
	FIELD(TC_BMR, POSEN, 9, 1)
	FIELD(TC_BMR, SPEEDEN, 10, 1)
	FIELD(TC_BMR, QDTRANS, 11, 1)
	FIELD(TC_BMR, EDGPHA, 12, 1)
	FIELD(TC_BMR, INVA, 13, 1)
	FIELD(TC_BMR, INVB, 14, 1)
	FIELD(TC_BMR, INVIDX, 15, 1)
	FIELD(TC_BMR, SWAP, 16, 1)
	FIELD(TC_BMR, IDXPHB, 17, 1)
	FIELD(TC_BMR, MAXFILT, 20, 6)
REG32(TC_QIER, 0xc8)
	FIELD(TC_QIER, IDX, 0, 1)
	FIELD(TC_QIER, DIRCHG, 1, 1)
	FIELD(TC_QIER, QERR, 2, 1)
REG32(TC_QIDR, 0xcc)
	FIELD(TC_QIDR, IDX, 0, 1)
	FIELD(TC_QIDR, DIRCHG, 1, 1)
	FIELD(TC_QIDR, QERR, 2, 1)
REG32(TC_QIMR, 0xd0)
	FIELD(TC_QIMR, IDX, 0, 1)
	FIELD(TC_QIMR, DIRCHG, 1, 1)
	FIELD(TC_QIMR, QERR, 2, 1)
REG32(TC_QISR, 0xd4)
	FIELD(TC_QISR, IDX, 0, 1)
	FIELD(TC_QISR, DIRCHG, 1, 1)
	FIELD(TC_QISR, QERR, 2, 1)
	FIELD(TC_QISR, DIR, 8, 1)
REG32(TC_FMR, 0xd8)
	FIELD(TC_FMR, ENCF0, 0, 1)
	FIELD(TC_FMR, ENCF1, 1, 1)
REG32(TC_WPMR, 0xe4)
	FIELD(TC_WPMR, WPEN, 0, 1)
	FIELD(TC_WPMR, WPKEY, 8, 24)


__attribute__((unused))
static void atsam3x8e_tc_update(ATSAM3X8ETC *t) {
	int conditions = ((t->SR2 & 0x4) || (t->SR1 & 0x4) || (t->SR0 & 0x4));
	qemu_set_irq(t->irq[0], conditions);

}

static void atsam3x8e_tc_timer_callback_0(void *opaque) {
  static uint8_t period_reg_set_last = 0;
  ATSAM3X8ETC *t = (ATSAM3X8ETC*)opaque;

  if (!period_reg_set_last && t->period_reg_set[0]) {
      period_reg_set_last = 1;
      t->CV0 = 0;
  }
  t->CV0 += 1;
  unsigned prd = t->period_set[0] ? t->cur_period[0] : t->RB0;
  if (t->CV0 == prd) {
    t->cur_period[0] = t->RB0;
    t->CV0 = 0;
    t->SR1 |= 0x4;t->SR0 |= 0x4;t->SR2 |= 0x4;;
    qemu_set_irq(t->irq[0], 1);
    qemu_set_irq(t->irq[0], 0);
  }

  if (t->enabled[0]) {
    timer_mod(t->timers[0], qemu_clock_get_us(QEMU_CLOCK_VIRTUAL) + 1);
  }
}


static void atsam3x8e_tc_timer_callback_1(void *opaque) {
  static uint8_t period_reg_set_last = 0;
  ATSAM3X8ETC *t = (ATSAM3X8ETC*)opaque;

  if (!period_reg_set_last && t->period_reg_set[1]) {
      period_reg_set_last = 1;
      t->CV1 = 0;
  }
  t->CV1 += 1;
  unsigned prd = t->period_set[1] ? t->cur_period[1] : t->RB1;
  if (t->CV1 == prd) {
    t->cur_period[1] = t->RB1;
    t->CV1 = 0;
    t->SR1 |= 0x4;t->SR0 |= 0x4;t->SR2 |= 0x4;;
    qemu_set_irq(t->irq[1], 1);
    qemu_set_irq(t->irq[1], 0);
  }

  if (t->enabled[1]) {
    timer_mod(t->timers[1], qemu_clock_get_us(QEMU_CLOCK_VIRTUAL) + 1);
  }
}


static void atsam3x8e_tc_timer_callback_2(void *opaque) {
  static uint8_t period_reg_set_last = 0;
  ATSAM3X8ETC *t = (ATSAM3X8ETC*)opaque;

  if (!period_reg_set_last && t->period_reg_set[2]) {
      period_reg_set_last = 1;
      t->CV2 = 0;
  }
  t->CV2 += 1;
  unsigned prd = t->period_set[2] ? t->cur_period[2] : t->RB2;
  if (t->CV2 == prd) {
    t->cur_period[2] = t->RB2;
    t->CV2 = 0;
    t->SR1 |= 0x4;t->SR0 |= 0x4;t->SR2 |= 0x4;;
    qemu_set_irq(t->irq[2], 1);
    qemu_set_irq(t->irq[2], 0);
  }

  if (t->enabled[2]) {
    timer_mod(t->timers[2], qemu_clock_get_us(QEMU_CLOCK_VIRTUAL) + 1);
  }
}


static void atsam3x8e_tc_register_reset(ATSAM3X8ETC *t) {
	t->CMR0 = 0x0;
	t->CMR0_WAVEFORM_MODE = 0x0;
	t->SMMR0 = 0x0;
	t->CV0 = 0x0;
	t->RA0 = 0x0;
	t->RB0 = 0x0;
	t->RC0 = 0x0;
	t->SR0 = 0x0;
	t->IMR0 = 0x0;
	t->CMR1 = 0x0;
	t->CMR1_WAVEFORM_MODE = 0x0;
	t->SMMR1 = 0x0;
	t->CV1 = 0x0;
	t->RA1 = 0x0;
	t->RB1 = 0x0;
	t->RC1 = 0x0;
	t->SR1 = 0x0;
	t->IMR1 = 0x0;
	t->CMR2 = 0x0;
	t->CMR2_WAVEFORM_MODE = 0x0;
	t->SMMR2 = 0x0;
	t->CV2 = 0x0;
	t->RA2 = 0x0;
	t->RB2 = 0x0;
	t->RC2 = 0x0;
	t->SR2 = 0x0;
	t->IMR2 = 0x0;
	t->BMR = 0x0;
	t->QIMR = 0x0;
	t->QISR = 0x0;
	t->FMR = 0x0;
	t->WPMR = 0x0;
	t->enabled[0] = 0;
	t->period_set[0] = 0;
	t->period_reg_set[0] = 0;
	t->enabled[1] = 0;
	t->period_set[1] = 0;
	t->period_reg_set[1] = 0;
	t->enabled[2] = 0;
	t->period_set[2] = 0;
	t->period_reg_set[2] = 0;

}

static uint64_t atsam3x8e_tc_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAM3X8ETC *t = ATSAM3X8E_TC(opaque);
	uint64_t ret;

	switch (offset) {
		case A_TC_CMR0:
			ret = t->CMR0;
			break;
		case A_TC_SMMR0:
			ret = t->SMMR0;
			break;
		case A_TC_CV0:
			ret = t->CV0;
			break;
		case A_TC_RA0:
			ret = t->RA0;
			break;
		case A_TC_RB0:
			ret = t->RB0;
			break;
		case A_TC_RC0:
			ret = t->RC0;
			break;
		case A_TC_SR0:
			ret = t->SR0;
			break;
		case A_TC_IMR0:
			ret = t->IMR0;
			break;
		case A_TC_CMR1:
			ret = t->CMR1;
			break;
		case A_TC_SMMR1:
			ret = t->SMMR1;
			break;
		case A_TC_CV1:
			ret = t->CV1;
			break;
		case A_TC_RA1:
			ret = t->RA1;
			break;
		case A_TC_RB1:
			ret = t->RB1;
			break;
		case A_TC_RC1:
			ret = t->RC1;
			break;
		case A_TC_SR1:
			ret = t->SR1;
			break;
		case A_TC_IMR1:
			ret = t->IMR1;
			break;
		case A_TC_CMR2:
			ret = t->CMR2;
			break;
		case A_TC_SMMR2:
			ret = t->SMMR2;
			break;
		case A_TC_CV2:
			ret = t->CV2;
			break;
		case A_TC_RA2:
			ret = t->RA2;
			break;
		case A_TC_RB2:
			ret = t->RB2;
			break;
		case A_TC_RC2:
			ret = t->RC2;
			break;
		case A_TC_SR2:
			ret = t->SR2;
			break;
		case A_TC_IMR2:
			ret = t->IMR2;
			break;
		case A_TC_BMR:
			ret = t->BMR;
			break;
		case A_TC_QIMR:
			ret = t->QIMR;
			break;
		case A_TC_QISR:
			ret = t->QISR;
			break;
		case A_TC_FMR:
			ret = t->FMR;
			break;
		case A_TC_WPMR:
			ret = t->WPMR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM3X8E TC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsam3x8e_tc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAM3X8ETC *t = ATSAM3X8E_TC(opaque);


	switch (offset) {
		case A_TC_CCR0 ... A_TC_CCR0 + 3:
			value = value << ((offset - A_TC_CCR0) << 3);
			offset = A_TC_CCR0;
			break;
		case A_TC_CMR0 ... A_TC_CMR0 + 3:
			value = value << ((offset - A_TC_CMR0) << 3);
			offset = A_TC_CMR0;
			break;
		case A_TC_SMMR0 ... A_TC_SMMR0 + 3:
			value = value << ((offset - A_TC_SMMR0) << 3);
			offset = A_TC_SMMR0;
			break;
		case A_TC_RA0 ... A_TC_RA0 + 3:
			value = value << ((offset - A_TC_RA0) << 3);
			offset = A_TC_RA0;
			break;
		case A_TC_RB0 ... A_TC_RB0 + 3:
			value = value << ((offset - A_TC_RB0) << 3);
			offset = A_TC_RB0;
			break;
		case A_TC_RC0 ... A_TC_RC0 + 3:
			value = value << ((offset - A_TC_RC0) << 3);
			offset = A_TC_RC0;
			break;
		case A_TC_IER0 ... A_TC_IER0 + 3:
			value = value << ((offset - A_TC_IER0) << 3);
			offset = A_TC_IER0;
			break;
		case A_TC_IDR0 ... A_TC_IDR0 + 3:
			value = value << ((offset - A_TC_IDR0) << 3);
			offset = A_TC_IDR0;
			break;
		case A_TC_CCR1 ... A_TC_CCR1 + 3:
			value = value << ((offset - A_TC_CCR1) << 3);
			offset = A_TC_CCR1;
			break;
		case A_TC_CMR1 ... A_TC_CMR1 + 3:
			value = value << ((offset - A_TC_CMR1) << 3);
			offset = A_TC_CMR1;
			break;
		case A_TC_SMMR1 ... A_TC_SMMR1 + 3:
			value = value << ((offset - A_TC_SMMR1) << 3);
			offset = A_TC_SMMR1;
			break;
		case A_TC_RA1 ... A_TC_RA1 + 3:
			value = value << ((offset - A_TC_RA1) << 3);
			offset = A_TC_RA1;
			break;
		case A_TC_RB1 ... A_TC_RB1 + 3:
			value = value << ((offset - A_TC_RB1) << 3);
			offset = A_TC_RB1;
			break;
		case A_TC_RC1 ... A_TC_RC1 + 3:
			value = value << ((offset - A_TC_RC1) << 3);
			offset = A_TC_RC1;
			break;
		case A_TC_IER1 ... A_TC_IER1 + 3:
			value = value << ((offset - A_TC_IER1) << 3);
			offset = A_TC_IER1;
			break;
		case A_TC_IDR1 ... A_TC_IDR1 + 3:
			value = value << ((offset - A_TC_IDR1) << 3);
			offset = A_TC_IDR1;
			break;
		case A_TC_CCR2 ... A_TC_CCR2 + 3:
			value = value << ((offset - A_TC_CCR2) << 3);
			offset = A_TC_CCR2;
			break;
		case A_TC_CMR2 ... A_TC_CMR2 + 3:
			value = value << ((offset - A_TC_CMR2) << 3);
			offset = A_TC_CMR2;
			break;
		case A_TC_SMMR2 ... A_TC_SMMR2 + 3:
			value = value << ((offset - A_TC_SMMR2) << 3);
			offset = A_TC_SMMR2;
			break;
		case A_TC_RA2 ... A_TC_RA2 + 3:
			value = value << ((offset - A_TC_RA2) << 3);
			offset = A_TC_RA2;
			break;
		case A_TC_RB2 ... A_TC_RB2 + 3:
			value = value << ((offset - A_TC_RB2) << 3);
			offset = A_TC_RB2;
			break;
		case A_TC_RC2 ... A_TC_RC2 + 3:
			value = value << ((offset - A_TC_RC2) << 3);
			offset = A_TC_RC2;
			break;
		case A_TC_IER2 ... A_TC_IER2 + 3:
			value = value << ((offset - A_TC_IER2) << 3);
			offset = A_TC_IER2;
			break;
		case A_TC_IDR2 ... A_TC_IDR2 + 3:
			value = value << ((offset - A_TC_IDR2) << 3);
			offset = A_TC_IDR2;
			break;
		case A_TC_BCR ... A_TC_BCR + 3:
			value = value << ((offset - A_TC_BCR) << 3);
			offset = A_TC_BCR;
			break;
		case A_TC_BMR ... A_TC_BMR + 3:
			value = value << ((offset - A_TC_BMR) << 3);
			offset = A_TC_BMR;
			break;
		case A_TC_QIER ... A_TC_QIER + 3:
			value = value << ((offset - A_TC_QIER) << 3);
			offset = A_TC_QIER;
			break;
		case A_TC_QIDR ... A_TC_QIDR + 3:
			value = value << ((offset - A_TC_QIDR) << 3);
			offset = A_TC_QIDR;
			break;
		case A_TC_FMR ... A_TC_FMR + 3:
			value = value << ((offset - A_TC_FMR) << 3);
			offset = A_TC_FMR;
			break;
		case A_TC_WPMR ... A_TC_WPMR + 3:
			value = value << ((offset - A_TC_WPMR) << 3);
			offset = A_TC_WPMR;
			break;

		default: break;
	}

	switch (offset) {
		case A_TC_CCR0:
			t->CCR0 = value;
			if (((t->CCR0 & 0x1) && (!(t->CCR0 & 0x2)) && (t->CCR0 & 0x4) && (!(t->CCR0 & 0x8)) && (!(t->CCR0 & 0x10)) && (!(t->CCR0 & 0x20)) && (!(t->CCR0 & 0x40)) && (!(t->CCR0 & 0x80)) && (!(t->CCR0 & 0x100)) && (!(t->CCR0 & 0x200)) && (!(t->CCR0 & 0x400)) && (!(t->CCR0 & 0x800)) && (!(t->CCR0 & 0x1000)) && (!(t->CCR0 & 0x2000)) && (!(t->CCR0 & 0x4000)) && (!(t->CCR0 & 0x8000)) && (!(t->CCR0 & 0x10000)) && (!(t->CCR0 & 0x20000)) && (!(t->CCR0 & 0x40000)) && (!(t->CCR0 & 0x80000)) && (!(t->CCR0 & 0x100000)) && (!(t->CCR0 & 0x200000)) && (!(t->CCR0 & 0x400000)) && (!(t->CCR0 & 0x800000)) && (!(t->CCR0 & 0x1000000)) && (!(t->CCR0 & 0x2000000)) && (!(t->CCR0 & 0x4000000)) && (!(t->CCR0 & 0x8000000)) && (!(t->CCR0 & 0x10000000)) && (!(t->CCR0 & 0x20000000)) && (!(t->CCR0 & 0x40000000)) && (!(t->CCR0 & 0x80000000)))) {
				t->period_set[0] = t->period_reg_set[0];
				t->enabled[0] = 1;
				t->cur_period[0] = t->RB0;
				timer_mod(t->timers[0], qemu_clock_get_us(QEMU_CLOCK_VIRTUAL) + 1);
			}
			if (((!(t->CCR0 & 0x1)) && (t->CCR0 & 0x2) && (!(t->CCR0 & 0x4)) && (!(t->CCR0 & 0x8)) && (!(t->CCR0 & 0x10)) && (!(t->CCR0 & 0x20)) && (!(t->CCR0 & 0x40)) && (!(t->CCR0 & 0x80)) && (!(t->CCR0 & 0x100)) && (!(t->CCR0 & 0x200)) && (!(t->CCR0 & 0x400)) && (!(t->CCR0 & 0x800)) && (!(t->CCR0 & 0x1000)) && (!(t->CCR0 & 0x2000)) && (!(t->CCR0 & 0x4000)) && (!(t->CCR0 & 0x8000)) && (!(t->CCR0 & 0x10000)) && (!(t->CCR0 & 0x20000)) && (!(t->CCR0 & 0x40000)) && (!(t->CCR0 & 0x80000)) && (!(t->CCR0 & 0x100000)) && (!(t->CCR0 & 0x200000)) && (!(t->CCR0 & 0x400000)) && (!(t->CCR0 & 0x800000)) && (!(t->CCR0 & 0x1000000)) && (!(t->CCR0 & 0x2000000)) && (!(t->CCR0 & 0x4000000)) && (!(t->CCR0 & 0x8000000)) && (!(t->CCR0 & 0x10000000)) && (!(t->CCR0 & 0x20000000)) && (!(t->CCR0 & 0x40000000)) && (!(t->CCR0 & 0x80000000)))) {
				t->enabled[0] = 0;
				timer_free(t->timers[0]);
				t->timers[0] = timer_new(QEMU_CLOCK_VIRTUAL, SCALE_US, atsam3x8e_tc_timer_callback_0, t);
			}
			break;
		case A_TC_CMR0:
			t->CMR0 = value;
			break;
		case A_TC_SMMR0:
			t->SMMR0 = value;
			break;
		case A_TC_RA0:
			t->RA0 = value;
			break;
		case A_TC_RB0:
			t->RB0 = value;
			if (!t->period_reg_set[0]) {
				t->period_reg_set[0] = 1;
			}
			break;
		case A_TC_RC0:
			t->RC0 = value;
			break;
		case A_TC_IER0:
			t->IER0 = value;
			break;
		case A_TC_IDR0:
			t->IDR0 = value;
			break;
		case A_TC_CCR1:
			t->CCR1 = value;
			if (((t->CCR1 & 0x1) && (!(t->CCR1 & 0x2)) && (t->CCR1 & 0x4) && (!(t->CCR1 & 0x8)) && (!(t->CCR1 & 0x10)) && (!(t->CCR1 & 0x20)) && (!(t->CCR1 & 0x40)) && (!(t->CCR1 & 0x80)) && (!(t->CCR1 & 0x100)) && (!(t->CCR1 & 0x200)) && (!(t->CCR1 & 0x400)) && (!(t->CCR1 & 0x800)) && (!(t->CCR1 & 0x1000)) && (!(t->CCR1 & 0x2000)) && (!(t->CCR1 & 0x4000)) && (!(t->CCR1 & 0x8000)) && (!(t->CCR1 & 0x10000)) && (!(t->CCR1 & 0x20000)) && (!(t->CCR1 & 0x40000)) && (!(t->CCR1 & 0x80000)) && (!(t->CCR1 & 0x100000)) && (!(t->CCR1 & 0x200000)) && (!(t->CCR1 & 0x400000)) && (!(t->CCR1 & 0x800000)) && (!(t->CCR1 & 0x1000000)) && (!(t->CCR1 & 0x2000000)) && (!(t->CCR1 & 0x4000000)) && (!(t->CCR1 & 0x8000000)) && (!(t->CCR1 & 0x10000000)) && (!(t->CCR1 & 0x20000000)) && (!(t->CCR1 & 0x40000000)) && (!(t->CCR1 & 0x80000000)))) {
				t->period_set[1] = t->period_reg_set[1];
				t->enabled[1] = 1;
				t->cur_period[1] = t->RB1;
				timer_mod(t->timers[1], qemu_clock_get_us(QEMU_CLOCK_VIRTUAL) + 1);
			}
			if (((!(t->CCR1 & 0x1)) && (t->CCR1 & 0x2) && (!(t->CCR1 & 0x4)) && (!(t->CCR1 & 0x8)) && (!(t->CCR1 & 0x10)) && (!(t->CCR1 & 0x20)) && (!(t->CCR1 & 0x40)) && (!(t->CCR1 & 0x80)) && (!(t->CCR1 & 0x100)) && (!(t->CCR1 & 0x200)) && (!(t->CCR1 & 0x400)) && (!(t->CCR1 & 0x800)) && (!(t->CCR1 & 0x1000)) && (!(t->CCR1 & 0x2000)) && (!(t->CCR1 & 0x4000)) && (!(t->CCR1 & 0x8000)) && (!(t->CCR1 & 0x10000)) && (!(t->CCR1 & 0x20000)) && (!(t->CCR1 & 0x40000)) && (!(t->CCR1 & 0x80000)) && (!(t->CCR1 & 0x100000)) && (!(t->CCR1 & 0x200000)) && (!(t->CCR1 & 0x400000)) && (!(t->CCR1 & 0x800000)) && (!(t->CCR1 & 0x1000000)) && (!(t->CCR1 & 0x2000000)) && (!(t->CCR1 & 0x4000000)) && (!(t->CCR1 & 0x8000000)) && (!(t->CCR1 & 0x10000000)) && (!(t->CCR1 & 0x20000000)) && (!(t->CCR1 & 0x40000000)) && (!(t->CCR1 & 0x80000000)))) {
				t->enabled[1] = 0;
				timer_free(t->timers[1]);
				t->timers[1] = timer_new(QEMU_CLOCK_VIRTUAL, SCALE_US, atsam3x8e_tc_timer_callback_1, t);
			}
			break;
		case A_TC_CMR1:
			t->CMR1 = value;
			break;
		case A_TC_SMMR1:
			t->SMMR1 = value;
			break;
		case A_TC_RA1:
			t->RA1 = value;
			break;
		case A_TC_RB1:
			t->RB1 = value;
			if (!t->period_reg_set[1]) {
				t->period_reg_set[1] = 1;
			}
			break;
		case A_TC_RC1:
			t->RC1 = value;
			break;
		case A_TC_IER1:
			t->IER1 = value;
			break;
		case A_TC_IDR1:
			t->IDR1 = value;
			break;
		case A_TC_CCR2:
			t->CCR2 = value;
			if (((t->CCR2 & 0x1) && (!(t->CCR2 & 0x2)) && (t->CCR2 & 0x4) && (!(t->CCR2 & 0x8)) && (!(t->CCR2 & 0x10)) && (!(t->CCR2 & 0x20)) && (!(t->CCR2 & 0x40)) && (!(t->CCR2 & 0x80)) && (!(t->CCR2 & 0x100)) && (!(t->CCR2 & 0x200)) && (!(t->CCR2 & 0x400)) && (!(t->CCR2 & 0x800)) && (!(t->CCR2 & 0x1000)) && (!(t->CCR2 & 0x2000)) && (!(t->CCR2 & 0x4000)) && (!(t->CCR2 & 0x8000)) && (!(t->CCR2 & 0x10000)) && (!(t->CCR2 & 0x20000)) && (!(t->CCR2 & 0x40000)) && (!(t->CCR2 & 0x80000)) && (!(t->CCR2 & 0x100000)) && (!(t->CCR2 & 0x200000)) && (!(t->CCR2 & 0x400000)) && (!(t->CCR2 & 0x800000)) && (!(t->CCR2 & 0x1000000)) && (!(t->CCR2 & 0x2000000)) && (!(t->CCR2 & 0x4000000)) && (!(t->CCR2 & 0x8000000)) && (!(t->CCR2 & 0x10000000)) && (!(t->CCR2 & 0x20000000)) && (!(t->CCR2 & 0x40000000)) && (!(t->CCR2 & 0x80000000)))) {
				t->period_set[2] = t->period_reg_set[2];
				t->enabled[2] = 1;
				t->cur_period[2] = t->RB2;
				timer_mod(t->timers[2], qemu_clock_get_us(QEMU_CLOCK_VIRTUAL) + 1);
			}
			if (((!(t->CCR2 & 0x1)) && (t->CCR2 & 0x2) && (!(t->CCR2 & 0x4)) && (!(t->CCR2 & 0x8)) && (!(t->CCR2 & 0x10)) && (!(t->CCR2 & 0x20)) && (!(t->CCR2 & 0x40)) && (!(t->CCR2 & 0x80)) && (!(t->CCR2 & 0x100)) && (!(t->CCR2 & 0x200)) && (!(t->CCR2 & 0x400)) && (!(t->CCR2 & 0x800)) && (!(t->CCR2 & 0x1000)) && (!(t->CCR2 & 0x2000)) && (!(t->CCR2 & 0x4000)) && (!(t->CCR2 & 0x8000)) && (!(t->CCR2 & 0x10000)) && (!(t->CCR2 & 0x20000)) && (!(t->CCR2 & 0x40000)) && (!(t->CCR2 & 0x80000)) && (!(t->CCR2 & 0x100000)) && (!(t->CCR2 & 0x200000)) && (!(t->CCR2 & 0x400000)) && (!(t->CCR2 & 0x800000)) && (!(t->CCR2 & 0x1000000)) && (!(t->CCR2 & 0x2000000)) && (!(t->CCR2 & 0x4000000)) && (!(t->CCR2 & 0x8000000)) && (!(t->CCR2 & 0x10000000)) && (!(t->CCR2 & 0x20000000)) && (!(t->CCR2 & 0x40000000)) && (!(t->CCR2 & 0x80000000)))) {
				t->enabled[2] = 0;
				timer_free(t->timers[2]);
				t->timers[2] = timer_new(QEMU_CLOCK_VIRTUAL, SCALE_US, atsam3x8e_tc_timer_callback_2, t);
			}
			break;
		case A_TC_CMR2:
			t->CMR2 = value;
			break;
		case A_TC_SMMR2:
			t->SMMR2 = value;
			break;
		case A_TC_RA2:
			t->RA2 = value;
			break;
		case A_TC_RB2:
			t->RB2 = value;
			if (!t->period_reg_set[2]) {
				t->period_reg_set[2] = 1;
			}
			break;
		case A_TC_RC2:
			t->RC2 = value;
			break;
		case A_TC_IER2:
			t->IER2 = value;
			break;
		case A_TC_IDR2:
			t->IDR2 = value;
			break;
		case A_TC_BCR:
			t->BCR = value;
			break;
		case A_TC_BMR:
			t->BMR = value;
			break;
		case A_TC_QIER:
			t->QIER = value;
			break;
		case A_TC_QIDR:
			t->QIDR = value;
			break;
		case A_TC_FMR:
			t->FMR = value;
			break;
		case A_TC_WPMR:
			t->WPMR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM3X8E TC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsam3x8e_tc_ops = {
	.read = atsam3x8e_tc_read,
	.write = atsam3x8e_tc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsam3x8e_tc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAM3X8ETC *t = ATSAM3X8E_TC(obj);
	memory_region_init_io(&(t->iomem), obj, &atsam3x8e_tc_ops, t, "atsam3x8e-tc", ATSAM3X8E_TC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 3; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void atsam3x8e_tc_realize(DeviceState *dev, Error **errp) {
	ATSAM3X8ETC *t = ATSAM3X8E_TC(dev);
	t->timers[0] = timer_new(QEMU_CLOCK_VIRTUAL, SCALE_US, atsam3x8e_tc_timer_callback_0, t);
	t->timers[1] = timer_new(QEMU_CLOCK_VIRTUAL, SCALE_US, atsam3x8e_tc_timer_callback_1, t);
	t->timers[2] = timer_new(QEMU_CLOCK_VIRTUAL, SCALE_US, atsam3x8e_tc_timer_callback_2, t);

}

static void atsam3x8e_tc_reset_enter(Object *obj, ResetType type) {
	ATSAM3X8ETC *t = ATSAM3X8E_TC(obj);
	atsam3x8e_tc_register_reset(t);
}

static const VMStateDescription atsam3x8e_tc_vmstate = {
	.name = "atsam3x8e-tc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CCR0, ATSAM3X8ETC),
		VMSTATE_UINT32(CMR0, ATSAM3X8ETC),
		VMSTATE_UINT32(CMR0_WAVEFORM_MODE, ATSAM3X8ETC),
		VMSTATE_UINT32(SMMR0, ATSAM3X8ETC),
		VMSTATE_UINT32(CV0, ATSAM3X8ETC),
		VMSTATE_UINT32(RA0, ATSAM3X8ETC),
		VMSTATE_UINT32(RB0, ATSAM3X8ETC),
		VMSTATE_UINT32(RC0, ATSAM3X8ETC),
		VMSTATE_UINT32(SR0, ATSAM3X8ETC),
		VMSTATE_UINT32(IER0, ATSAM3X8ETC),
		VMSTATE_UINT32(IDR0, ATSAM3X8ETC),
		VMSTATE_UINT32(IMR0, ATSAM3X8ETC),
		VMSTATE_UINT32(CCR1, ATSAM3X8ETC),
		VMSTATE_UINT32(CMR1, ATSAM3X8ETC),
		VMSTATE_UINT32(CMR1_WAVEFORM_MODE, ATSAM3X8ETC),
		VMSTATE_UINT32(SMMR1, ATSAM3X8ETC),
		VMSTATE_UINT32(CV1, ATSAM3X8ETC),
		VMSTATE_UINT32(RA1, ATSAM3X8ETC),
		VMSTATE_UINT32(RB1, ATSAM3X8ETC),
		VMSTATE_UINT32(RC1, ATSAM3X8ETC),
		VMSTATE_UINT32(SR1, ATSAM3X8ETC),
		VMSTATE_UINT32(IER1, ATSAM3X8ETC),
		VMSTATE_UINT32(IDR1, ATSAM3X8ETC),
		VMSTATE_UINT32(IMR1, ATSAM3X8ETC),
		VMSTATE_UINT32(CCR2, ATSAM3X8ETC),
		VMSTATE_UINT32(CMR2, ATSAM3X8ETC),
		VMSTATE_UINT32(CMR2_WAVEFORM_MODE, ATSAM3X8ETC),
		VMSTATE_UINT32(SMMR2, ATSAM3X8ETC),
		VMSTATE_UINT32(CV2, ATSAM3X8ETC),
		VMSTATE_UINT32(RA2, ATSAM3X8ETC),
		VMSTATE_UINT32(RB2, ATSAM3X8ETC),
		VMSTATE_UINT32(RC2, ATSAM3X8ETC),
		VMSTATE_UINT32(SR2, ATSAM3X8ETC),
		VMSTATE_UINT32(IER2, ATSAM3X8ETC),
		VMSTATE_UINT32(IDR2, ATSAM3X8ETC),
		VMSTATE_UINT32(IMR2, ATSAM3X8ETC),
		VMSTATE_UINT32(BCR, ATSAM3X8ETC),
		VMSTATE_UINT32(BMR, ATSAM3X8ETC),
		VMSTATE_UINT32(QIER, ATSAM3X8ETC),
		VMSTATE_UINT32(QIDR, ATSAM3X8ETC),
		VMSTATE_UINT32(QIMR, ATSAM3X8ETC),
		VMSTATE_UINT32(QISR, ATSAM3X8ETC),
		VMSTATE_UINT32(FMR, ATSAM3X8ETC),
		VMSTATE_UINT32(WPMR, ATSAM3X8ETC),

		VMSTATE_END_OF_LIST()
	}
};

static void atsam3x8e_tc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsam3x8e_tc_vmstate;
	dc->realize = atsam3x8e_tc_realize;
	rc->phases.enter = atsam3x8e_tc_reset_enter;

}

static const TypeInfo atsam3x8e_tc_info = {
	.name = TYPE_ATSAM3X8E_TC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAM3X8ETC),
	.instance_init = atsam3x8e_tc_init,
	.class_init = atsam3x8e_tc_class_init,
};

static void atsam3x8e_tc_register_types(void) {
	type_register_static(&atsam3x8e_tc_info);
}

type_init(atsam3x8e_tc_register_types);

#define TYPE_ATSAM3X8E_SPI "atsam3x8e-spi"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAM3X8ESPI, ATSAM3X8E_SPI)

struct ATSAM3X8ESPI {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t CR;
	uint32_t MR;
	uint32_t RDR;
	uint32_t TDR;
	uint32_t SR;
	uint32_t IER;
	uint32_t IDR;
	uint32_t IMR;
	uint32_t WPMR;
	uint32_t WPSR;
	uint32_t CSR0;
	uint32_t CSR1;
	uint32_t CSR2;
	uint32_t CSR3;
	/* chardev backend */
	CharBackend chr;
	guint watch_tag;

	/* base */
	uint32_t base;

};
#define ATSAM3X8E_SPI_SIZE				0xec

REG32(SPI_CR, 0x0)
	FIELD(SPI_CR, SPIEN, 0, 1)
	FIELD(SPI_CR, SPIDIS, 1, 1)
	FIELD(SPI_CR, SWRST, 7, 1)
	FIELD(SPI_CR, LASTXFER, 24, 1)
REG32(SPI_MR, 0x4)
	FIELD(SPI_MR, MSTR, 0, 1)
	FIELD(SPI_MR, PS, 1, 1)
	FIELD(SPI_MR, PCSDEC, 2, 1)
	FIELD(SPI_MR, MODFDIS, 4, 1)
	FIELD(SPI_MR, WDRBT, 5, 1)
	FIELD(SPI_MR, LLB, 7, 1)
	FIELD(SPI_MR, PCS, 16, 4)
	FIELD(SPI_MR, DLYBCS, 24, 8)
REG32(SPI_RDR, 0x8)
	FIELD(SPI_RDR, RD, 0, 16)
	FIELD(SPI_RDR, PCS, 16, 4)
REG32(SPI_TDR, 0xc)
	FIELD(SPI_TDR, TD, 0, 16)
	FIELD(SPI_TDR, PCS, 16, 4)
	FIELD(SPI_TDR, LASTXFER, 24, 1)
REG32(SPI_SR, 0x10)
	FIELD(SPI_SR, RDRF, 0, 1)
	FIELD(SPI_SR, TDRE, 1, 1)
	FIELD(SPI_SR, MODF, 2, 1)
	FIELD(SPI_SR, OVRES, 3, 1)
	FIELD(SPI_SR, NSSR, 8, 1)
	FIELD(SPI_SR, TXEMPTY, 9, 1)
	FIELD(SPI_SR, UNDES, 10, 1)
	FIELD(SPI_SR, SPIENS, 16, 1)
REG32(SPI_IER, 0x14)
	FIELD(SPI_IER, RDRF, 0, 1)
	FIELD(SPI_IER, TDRE, 1, 1)
	FIELD(SPI_IER, MODF, 2, 1)
	FIELD(SPI_IER, OVRES, 3, 1)
	FIELD(SPI_IER, NSSR, 8, 1)
	FIELD(SPI_IER, TXEMPTY, 9, 1)
	FIELD(SPI_IER, UNDES, 10, 1)
REG32(SPI_IDR, 0x18)
	FIELD(SPI_IDR, RDRF, 0, 1)
	FIELD(SPI_IDR, TDRE, 1, 1)
	FIELD(SPI_IDR, MODF, 2, 1)
	FIELD(SPI_IDR, OVRES, 3, 1)
	FIELD(SPI_IDR, NSSR, 8, 1)
	FIELD(SPI_IDR, TXEMPTY, 9, 1)
	FIELD(SPI_IDR, UNDES, 10, 1)
REG32(SPI_IMR, 0x1c)
	FIELD(SPI_IMR, RDRF, 0, 1)
	FIELD(SPI_IMR, TDRE, 1, 1)
	FIELD(SPI_IMR, MODF, 2, 1)
	FIELD(SPI_IMR, OVRES, 3, 1)
	FIELD(SPI_IMR, NSSR, 8, 1)
	FIELD(SPI_IMR, TXEMPTY, 9, 1)
	FIELD(SPI_IMR, UNDES, 10, 1)
REG32(SPI_WPMR, 0xe4)
	FIELD(SPI_WPMR, WPEN, 0, 1)
	FIELD(SPI_WPMR, WPKEY, 8, 24)
REG32(SPI_WPSR, 0xe8)
	FIELD(SPI_WPSR, WPVS, 0, 1)
	FIELD(SPI_WPSR, WPVSRC, 8, 8)
REG32(SPI_CSR0, 0x30)
	FIELD(SPI_CSR0, CPOL, 0, 1)
	FIELD(SPI_CSR0, NCPHA, 1, 1)
	FIELD(SPI_CSR0, CSNAAT, 2, 1)
	FIELD(SPI_CSR0, CSAAT, 3, 1)
	FIELD(SPI_CSR0, BITS, 4, 4)
	FIELD(SPI_CSR0, SCBR, 8, 8)
	FIELD(SPI_CSR0, DLYBS, 16, 8)
	FIELD(SPI_CSR0, DLYBCT, 24, 8)
REG32(SPI_CSR1, 0x34)
	FIELD(SPI_CSR1, CPOL, 0, 1)
	FIELD(SPI_CSR1, NCPHA, 1, 1)
	FIELD(SPI_CSR1, CSNAAT, 2, 1)
	FIELD(SPI_CSR1, CSAAT, 3, 1)
	FIELD(SPI_CSR1, BITS, 4, 4)
	FIELD(SPI_CSR1, SCBR, 8, 8)
	FIELD(SPI_CSR1, DLYBS, 16, 8)
	FIELD(SPI_CSR1, DLYBCT, 24, 8)
REG32(SPI_CSR2, 0x38)
	FIELD(SPI_CSR2, CPOL, 0, 1)
	FIELD(SPI_CSR2, NCPHA, 1, 1)
	FIELD(SPI_CSR2, CSNAAT, 2, 1)
	FIELD(SPI_CSR2, CSAAT, 3, 1)
	FIELD(SPI_CSR2, BITS, 4, 4)
	FIELD(SPI_CSR2, SCBR, 8, 8)
	FIELD(SPI_CSR2, DLYBS, 16, 8)
	FIELD(SPI_CSR2, DLYBCT, 24, 8)
REG32(SPI_CSR3, 0x3c)
	FIELD(SPI_CSR3, CPOL, 0, 1)
	FIELD(SPI_CSR3, NCPHA, 1, 1)
	FIELD(SPI_CSR3, CSNAAT, 2, 1)
	FIELD(SPI_CSR3, CSAAT, 3, 1)
	FIELD(SPI_CSR3, BITS, 4, 4)
	FIELD(SPI_CSR3, SCBR, 8, 8)
	FIELD(SPI_CSR3, DLYBS, 16, 8)
	FIELD(SPI_CSR3, DLYBCT, 24, 8)


static int atsam3x8e_spi_can_receive(void *opaque) {
	return 1;
}

static void atsam3x8e_spi_receive(void *opaque, const uint8_t *buf, int size) {
	ATSAM3X8ESPI *t = ATSAM3X8E_SPI(opaque);

	t->RDR = *buf;
	t->SR |= 0x1;


}

static gboolean atsam3x8e_spi_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	ATSAM3X8ESPI *t = ATSAM3X8E_SPI(opaque);
	int ret;

	t->watch_tag = 0;
	t->SR &= (~(0x2));



	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->TDR), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, atsam3x8e_spi_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->SR |= 0x2;



	return FALSE;
}

static void atsam3x8e_spi_register_reset(ATSAM3X8ESPI *t) {
	t->MR = 0x0;
	t->RDR = 0x0;
	t->SR = 0xf0;
	t->IMR = 0x0;
	t->WPMR = 0x0;
	t->WPSR = 0x0;
	t->SR |= 0x2;

}

static uint64_t atsam3x8e_spi_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAM3X8ESPI *t = ATSAM3X8E_SPI(opaque);
	uint64_t ret;

	switch (offset) {
		case A_SPI_MR:
			ret = t->MR;
			break;
		case A_SPI_RDR:
			ret = t->RDR;
			t->SR &= (~(0x1));
			qemu_chr_fe_accept_input(&(t->chr));
			break;
		case A_SPI_SR:
			ret = t->SR;
			break;
		case A_SPI_IMR:
			ret = t->IMR;
			break;
		case A_SPI_WPMR:
			ret = t->WPMR;
			break;
		case A_SPI_WPSR:
			ret = t->WPSR;
			break;
		case A_SPI_CSR0:
			ret = t->CSR0;
			break;
		case A_SPI_CSR1:
			ret = t->CSR1;
			break;
		case A_SPI_CSR2:
			ret = t->CSR2;
			break;
		case A_SPI_CSR3:
			ret = t->CSR3;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM3X8E SPI read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsam3x8e_spi_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAM3X8ESPI *t = ATSAM3X8E_SPI(opaque);


	switch (offset) {
		case A_SPI_CR ... A_SPI_CR + 3:
			value = value << ((offset - A_SPI_CR) << 3);
			offset = A_SPI_CR;
			break;
		case A_SPI_MR ... A_SPI_MR + 3:
			value = value << ((offset - A_SPI_MR) << 3);
			offset = A_SPI_MR;
			break;
		case A_SPI_TDR ... A_SPI_TDR + 3:
			value = value << ((offset - A_SPI_TDR) << 3);
			offset = A_SPI_TDR;
			break;
		case A_SPI_IER ... A_SPI_IER + 3:
			value = value << ((offset - A_SPI_IER) << 3);
			offset = A_SPI_IER;
			break;
		case A_SPI_IDR ... A_SPI_IDR + 3:
			value = value << ((offset - A_SPI_IDR) << 3);
			offset = A_SPI_IDR;
			break;
		case A_SPI_WPMR ... A_SPI_WPMR + 3:
			value = value << ((offset - A_SPI_WPMR) << 3);
			offset = A_SPI_WPMR;
			break;
		case A_SPI_CSR0 ... A_SPI_CSR0 + 3:
			value = value << ((offset - A_SPI_CSR0) << 3);
			offset = A_SPI_CSR0;
			break;
		case A_SPI_CSR1 ... A_SPI_CSR1 + 3:
			value = value << ((offset - A_SPI_CSR1) << 3);
			offset = A_SPI_CSR1;
			break;
		case A_SPI_CSR2 ... A_SPI_CSR2 + 3:
			value = value << ((offset - A_SPI_CSR2) << 3);
			offset = A_SPI_CSR2;
			break;
		case A_SPI_CSR3 ... A_SPI_CSR3 + 3:
			value = value << ((offset - A_SPI_CSR3) << 3);
			offset = A_SPI_CSR3;
			break;

		default: break;
	}

	switch (offset) {
		case A_SPI_CR:
			t->CR = value;
			break;
		case A_SPI_MR:
			t->MR = value;
			break;
		case A_SPI_TDR:
			t->TDR = value;
			atsam3x8e_spi_transmit(NULL, G_IO_OUT, t);
			break;
		case A_SPI_IER:
			t->IER = value;
			break;
		case A_SPI_IDR:
			t->IDR = value;
			break;
		case A_SPI_WPMR:
			t->WPMR = value;
			break;
		case A_SPI_CSR0:
			t->CSR0 = value;
			break;
		case A_SPI_CSR1:
			t->CSR1 = value;
			break;
		case A_SPI_CSR2:
			t->CSR2 = value;
			break;
		case A_SPI_CSR3:
			t->CSR3 = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM3X8E SPI write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsam3x8e_spi_ops = {
	.read = atsam3x8e_spi_read,
	.write = atsam3x8e_spi_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsam3x8e_spi_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAM3X8ESPI *t = ATSAM3X8E_SPI(obj);
	memory_region_init_io(&(t->iomem), obj, &atsam3x8e_spi_ops, t, "atsam3x8e-spi", ATSAM3X8E_SPI_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void atsam3x8e_spi_realize(DeviceState *dev, Error **errp) {
	ATSAM3X8ESPI *t = ATSAM3X8E_SPI(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), atsam3x8e_spi_can_receive, atsam3x8e_spi_receive,
		NULL, NULL, t, NULL, true);

}

static void atsam3x8e_spi_reset_enter(Object *obj, ResetType type) {
	ATSAM3X8ESPI *t = ATSAM3X8E_SPI(obj);
	atsam3x8e_spi_register_reset(t);
}

static const VMStateDescription atsam3x8e_spi_vmstate = {
	.name = "atsam3x8e-spi",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR, ATSAM3X8ESPI),
		VMSTATE_UINT32(MR, ATSAM3X8ESPI),
		VMSTATE_UINT32(RDR, ATSAM3X8ESPI),
		VMSTATE_UINT32(TDR, ATSAM3X8ESPI),
		VMSTATE_UINT32(SR, ATSAM3X8ESPI),
		VMSTATE_UINT32(IER, ATSAM3X8ESPI),
		VMSTATE_UINT32(IDR, ATSAM3X8ESPI),
		VMSTATE_UINT32(IMR, ATSAM3X8ESPI),
		VMSTATE_UINT32(WPMR, ATSAM3X8ESPI),
		VMSTATE_UINT32(WPSR, ATSAM3X8ESPI),
		VMSTATE_UINT32(CSR0, ATSAM3X8ESPI),
		VMSTATE_UINT32(CSR1, ATSAM3X8ESPI),
		VMSTATE_UINT32(CSR2, ATSAM3X8ESPI),
		VMSTATE_UINT32(CSR3, ATSAM3X8ESPI),

		VMSTATE_END_OF_LIST()
	}
};

static Property atsam3x8e_spi_properties[] = {
	DEFINE_PROP_CHR("chardev", ATSAM3X8ESPI, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void atsam3x8e_spi_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsam3x8e_spi_vmstate;
	dc->realize = atsam3x8e_spi_realize;
	rc->phases.enter = atsam3x8e_spi_reset_enter;
	device_class_set_props(dc, atsam3x8e_spi_properties);

}

static const TypeInfo atsam3x8e_spi_info = {
	.name = TYPE_ATSAM3X8E_SPI,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAM3X8ESPI),
	.instance_init = atsam3x8e_spi_init,
	.class_init = atsam3x8e_spi_class_init,
};

static void atsam3x8e_spi_register_types(void) {
	type_register_static(&atsam3x8e_spi_info);
}

type_init(atsam3x8e_spi_register_types);

#define TYPE_ATSAM3X8E_DACC "atsam3x8e-dacc"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAM3X8EDACC, ATSAM3X8E_DACC)

struct ATSAM3X8EDACC {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t CR;
	uint32_t MR;
	uint32_t CHER;
	uint32_t CHDR;
	uint32_t CHSR;
	uint32_t CDR;
	uint32_t IER;
	uint32_t IDR;
	uint32_t IMR;
	uint32_t ISR;
	uint32_t ACR;
	uint32_t WPMR;
	uint32_t WPSR;
	uint32_t TPR;
	uint32_t TCR;
	uint32_t TNPR;
	uint32_t TNCR;
	uint32_t PTCR;
	uint32_t PTSR;
	/* chardev backend */
	CharBackend chr;
	guint watch_tag;

	/* base */
	uint32_t base;

};
#define ATSAM3X8E_DACC_SIZE				0x128

REG32(DACC_CR, 0x0)
	FIELD(DACC_CR, SWRST, 0, 1)
REG32(DACC_MR, 0x4)
	FIELD(DACC_MR, TRGEN, 0, 1)
	FIELD(DACC_MR, TRGSEL, 1, 3)
	FIELD(DACC_MR, WORD, 4, 1)
	FIELD(DACC_MR, SLEEP, 5, 1)
	FIELD(DACC_MR, FASTWKUP, 6, 1)
	FIELD(DACC_MR, REFRESH, 8, 8)
	FIELD(DACC_MR, USER_SEL, 16, 2)
	FIELD(DACC_MR, TAG, 20, 1)
	FIELD(DACC_MR, MAXS, 21, 1)
	FIELD(DACC_MR, STARTUP, 24, 6)
REG32(DACC_CHER, 0x10)
	FIELD(DACC_CHER, CH0, 0, 1)
	FIELD(DACC_CHER, CH1, 1, 1)
REG32(DACC_CHDR, 0x14)
	FIELD(DACC_CHDR, CH0, 0, 1)
	FIELD(DACC_CHDR, CH1, 1, 1)
REG32(DACC_CHSR, 0x18)
	FIELD(DACC_CHSR, CH0, 0, 1)
	FIELD(DACC_CHSR, CH1, 1, 1)
REG32(DACC_CDR, 0x20)
	FIELD(DACC_CDR, DATA, 0, 32)
REG32(DACC_IER, 0x24)
	FIELD(DACC_IER, TXRDY, 0, 1)
	FIELD(DACC_IER, EOC, 1, 1)
	FIELD(DACC_IER, ENDTX, 2, 1)
	FIELD(DACC_IER, TXBUFE, 3, 1)
REG32(DACC_IDR, 0x28)
	FIELD(DACC_IDR, TXRDY, 0, 1)
	FIELD(DACC_IDR, EOC, 1, 1)
	FIELD(DACC_IDR, ENDTX, 2, 1)
	FIELD(DACC_IDR, TXBUFE, 3, 1)
REG32(DACC_IMR, 0x2c)
	FIELD(DACC_IMR, TXRDY, 0, 1)
	FIELD(DACC_IMR, EOC, 1, 1)
	FIELD(DACC_IMR, ENDTX, 2, 1)
	FIELD(DACC_IMR, TXBUFE, 3, 1)
REG32(DACC_ISR, 0x30)
	FIELD(DACC_ISR, TXRDY, 0, 1)
	FIELD(DACC_ISR, EOC, 1, 1)
	FIELD(DACC_ISR, ENDTX, 2, 1)
	FIELD(DACC_ISR, TXBUFE, 3, 1)
REG32(DACC_ACR, 0x94)
	FIELD(DACC_ACR, IBCTLCH0, 0, 2)
	FIELD(DACC_ACR, IBCTLCH1, 2, 2)
	FIELD(DACC_ACR, IBCTLDACCORE, 8, 2)
REG32(DACC_WPMR, 0xe4)
	FIELD(DACC_WPMR, WPEN, 0, 1)
	FIELD(DACC_WPMR, WPKEY, 8, 24)
REG32(DACC_WPSR, 0xe8)
	FIELD(DACC_WPSR, WPROTERR, 0, 1)
	FIELD(DACC_WPSR, WPROTADDR, 8, 8)
REG32(DACC_TPR, 0x108)
	FIELD(DACC_TPR, TXPTR, 0, 32)
REG32(DACC_TCR, 0x10c)
	FIELD(DACC_TCR, TXCTR, 0, 16)
REG32(DACC_TNPR, 0x118)
	FIELD(DACC_TNPR, TXNPTR, 0, 32)
REG32(DACC_TNCR, 0x11c)
	FIELD(DACC_TNCR, TXNCTR, 0, 16)
REG32(DACC_PTCR, 0x120)
	FIELD(DACC_PTCR, RXTEN, 0, 1)
	FIELD(DACC_PTCR, RXTDIS, 1, 1)
	FIELD(DACC_PTCR, TXTEN, 8, 1)
	FIELD(DACC_PTCR, TXTDIS, 9, 1)
REG32(DACC_PTSR, 0x124)
	FIELD(DACC_PTSR, RXTEN, 0, 1)
	FIELD(DACC_PTSR, TXTEN, 8, 1)


static int atsam3x8e_dacc_can_receive(void *opaque) {
	return 1;
}

static void atsam3x8e_dacc_receive(void *opaque, const uint8_t *buf, int size) {
	ATSAM3X8EDACC *t = ATSAM3X8E_DACC(opaque);

	(void)t;


}

static gboolean atsam3x8e_dacc_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	ATSAM3X8EDACC *t = ATSAM3X8E_DACC(opaque);
	int ret;

	t->watch_tag = 0;
	t->ISR &= (~(0x2));



	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->CDR), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, atsam3x8e_dacc_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->ISR |= 0x2;



	return FALSE;
}

static void atsam3x8e_dacc_register_reset(ATSAM3X8EDACC *t) {
	t->MR = 0x0;
	t->CHSR = 0x0;
	t->CDR = 0x0;
	t->IMR = 0x0;
	t->ISR = 0x0;
	t->ACR = 0x0;
	t->WPMR = 0x0;
	t->WPSR = 0x0;
	t->TPR = 0x0;
	t->TCR = 0x0;
	t->TNPR = 0x0;
	t->TNCR = 0x0;
	t->PTCR = 0x0;
	t->PTSR = 0x0;
	t->ISR |= 0x2;

}

static uint64_t atsam3x8e_dacc_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAM3X8EDACC *t = ATSAM3X8E_DACC(opaque);
	uint64_t ret;

	switch (offset) {
		case A_DACC_MR:
			ret = t->MR;
			break;
		case A_DACC_CHSR:
			ret = t->CHSR;
			break;
		case A_DACC_IMR:
			ret = t->IMR;
			break;
		case A_DACC_ISR:
			ret = t->ISR;
			break;
		case A_DACC_ACR:
			ret = t->ACR;
			break;
		case A_DACC_WPMR:
			ret = t->WPMR;
			break;
		case A_DACC_WPSR:
			ret = t->WPSR;
			break;
		case A_DACC_TPR:
			ret = t->TPR;
			break;
		case A_DACC_TCR:
			ret = t->TCR;
			break;
		case A_DACC_TNPR:
			ret = t->TNPR;
			break;
		case A_DACC_TNCR:
			ret = t->TNCR;
			break;
		case A_DACC_PTSR:
			ret = t->PTSR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM3X8E DACC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsam3x8e_dacc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAM3X8EDACC *t = ATSAM3X8E_DACC(opaque);


	switch (offset) {
		case A_DACC_CR ... A_DACC_CR + 3:
			value = value << ((offset - A_DACC_CR) << 3);
			offset = A_DACC_CR;
			break;
		case A_DACC_MR ... A_DACC_MR + 3:
			value = value << ((offset - A_DACC_MR) << 3);
			offset = A_DACC_MR;
			break;
		case A_DACC_CHER ... A_DACC_CHER + 3:
			value = value << ((offset - A_DACC_CHER) << 3);
			offset = A_DACC_CHER;
			break;
		case A_DACC_CHDR ... A_DACC_CHDR + 3:
			value = value << ((offset - A_DACC_CHDR) << 3);
			offset = A_DACC_CHDR;
			break;
		case A_DACC_CDR ... A_DACC_CDR + 3:
			value = value << ((offset - A_DACC_CDR) << 3);
			offset = A_DACC_CDR;
			break;
		case A_DACC_IER ... A_DACC_IER + 3:
			value = value << ((offset - A_DACC_IER) << 3);
			offset = A_DACC_IER;
			break;
		case A_DACC_IDR ... A_DACC_IDR + 3:
			value = value << ((offset - A_DACC_IDR) << 3);
			offset = A_DACC_IDR;
			break;
		case A_DACC_ACR ... A_DACC_ACR + 3:
			value = value << ((offset - A_DACC_ACR) << 3);
			offset = A_DACC_ACR;
			break;
		case A_DACC_WPMR ... A_DACC_WPMR + 3:
			value = value << ((offset - A_DACC_WPMR) << 3);
			offset = A_DACC_WPMR;
			break;
		case A_DACC_TPR ... A_DACC_TPR + 3:
			value = value << ((offset - A_DACC_TPR) << 3);
			offset = A_DACC_TPR;
			break;
		case A_DACC_TCR ... A_DACC_TCR + 3:
			value = value << ((offset - A_DACC_TCR) << 3);
			offset = A_DACC_TCR;
			break;
		case A_DACC_TNPR ... A_DACC_TNPR + 3:
			value = value << ((offset - A_DACC_TNPR) << 3);
			offset = A_DACC_TNPR;
			break;
		case A_DACC_TNCR ... A_DACC_TNCR + 3:
			value = value << ((offset - A_DACC_TNCR) << 3);
			offset = A_DACC_TNCR;
			break;
		case A_DACC_PTCR ... A_DACC_PTCR + 3:
			value = value << ((offset - A_DACC_PTCR) << 3);
			offset = A_DACC_PTCR;
			break;

		default: break;
	}

	switch (offset) {
		case A_DACC_CR:
			t->CR = value;
			break;
		case A_DACC_MR:
			t->MR = value;
			break;
		case A_DACC_CHER:
			t->CHER = value;
			break;
		case A_DACC_CHDR:
			t->CHDR = value;
			break;
		case A_DACC_CDR:
			t->CDR = value;
			atsam3x8e_dacc_transmit(NULL, G_IO_OUT, t);
			break;
		case A_DACC_IER:
			t->IER = value;
			break;
		case A_DACC_IDR:
			t->IDR = value;
			break;
		case A_DACC_ACR:
			t->ACR = value;
			break;
		case A_DACC_WPMR:
			t->WPMR = value;
			break;
		case A_DACC_TPR:
			t->TPR = value;
			break;
		case A_DACC_TCR:
			t->TCR = value;
			break;
		case A_DACC_TNPR:
			t->TNPR = value;
			break;
		case A_DACC_TNCR:
			t->TNCR = value;
			break;
		case A_DACC_PTCR:
			t->PTCR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM3X8E DACC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsam3x8e_dacc_ops = {
	.read = atsam3x8e_dacc_read,
	.write = atsam3x8e_dacc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsam3x8e_dacc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAM3X8EDACC *t = ATSAM3X8E_DACC(obj);
	memory_region_init_io(&(t->iomem), obj, &atsam3x8e_dacc_ops, t, "atsam3x8e-dacc", ATSAM3X8E_DACC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void atsam3x8e_dacc_realize(DeviceState *dev, Error **errp) {
	ATSAM3X8EDACC *t = ATSAM3X8E_DACC(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), atsam3x8e_dacc_can_receive, atsam3x8e_dacc_receive,
		NULL, NULL, t, NULL, true);

}

static void atsam3x8e_dacc_reset_enter(Object *obj, ResetType type) {
	ATSAM3X8EDACC *t = ATSAM3X8E_DACC(obj);
	atsam3x8e_dacc_register_reset(t);
}

static const VMStateDescription atsam3x8e_dacc_vmstate = {
	.name = "atsam3x8e-dacc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR, ATSAM3X8EDACC),
		VMSTATE_UINT32(MR, ATSAM3X8EDACC),
		VMSTATE_UINT32(CHER, ATSAM3X8EDACC),
		VMSTATE_UINT32(CHDR, ATSAM3X8EDACC),
		VMSTATE_UINT32(CHSR, ATSAM3X8EDACC),
		VMSTATE_UINT32(CDR, ATSAM3X8EDACC),
		VMSTATE_UINT32(IER, ATSAM3X8EDACC),
		VMSTATE_UINT32(IDR, ATSAM3X8EDACC),
		VMSTATE_UINT32(IMR, ATSAM3X8EDACC),
		VMSTATE_UINT32(ISR, ATSAM3X8EDACC),
		VMSTATE_UINT32(ACR, ATSAM3X8EDACC),
		VMSTATE_UINT32(WPMR, ATSAM3X8EDACC),
		VMSTATE_UINT32(WPSR, ATSAM3X8EDACC),
		VMSTATE_UINT32(TPR, ATSAM3X8EDACC),
		VMSTATE_UINT32(TCR, ATSAM3X8EDACC),
		VMSTATE_UINT32(TNPR, ATSAM3X8EDACC),
		VMSTATE_UINT32(TNCR, ATSAM3X8EDACC),
		VMSTATE_UINT32(PTCR, ATSAM3X8EDACC),
		VMSTATE_UINT32(PTSR, ATSAM3X8EDACC),

		VMSTATE_END_OF_LIST()
	}
};

static Property atsam3x8e_dacc_properties[] = {
	DEFINE_PROP_CHR("chardev", ATSAM3X8EDACC, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void atsam3x8e_dacc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsam3x8e_dacc_vmstate;
	dc->realize = atsam3x8e_dacc_realize;
	rc->phases.enter = atsam3x8e_dacc_reset_enter;
	device_class_set_props(dc, atsam3x8e_dacc_properties);

}

static const TypeInfo atsam3x8e_dacc_info = {
	.name = TYPE_ATSAM3X8E_DACC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAM3X8EDACC),
	.instance_init = atsam3x8e_dacc_init,
	.class_init = atsam3x8e_dacc_class_init,
};

static void atsam3x8e_dacc_register_types(void) {
	type_register_static(&atsam3x8e_dacc_info);
}

type_init(atsam3x8e_dacc_register_types);

#define TYPE_ATSAM3X8E_TWI "atsam3x8e-twi"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAM3X8ETWI, ATSAM3X8E_TWI)

struct ATSAM3X8ETWI {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t CR;
	uint32_t MMR;
	uint32_t SMR;
	uint32_t IADR;
	uint32_t CWGR;
	uint32_t SR;
	uint32_t IER;
	uint32_t IDR;
	uint32_t IMR;
	uint32_t RHR;
	uint32_t THR;
	uint32_t RPR;
	uint32_t RCR;
	uint32_t TPR;
	uint32_t TCR;
	uint32_t RNPR;
	uint32_t RNCR;
	uint32_t TNPR;
	uint32_t TNCR;
	uint32_t PTCR;
	uint32_t PTSR;
	/* chardev backend */
	CharBackend chr;
	guint watch_tag;

	/* base */
	uint32_t base;

};
#define ATSAM3X8E_TWI_SIZE				0x128

REG32(TWI_CR, 0x0)
	FIELD(TWI_CR, START, 0, 1)
	FIELD(TWI_CR, STOP, 1, 1)
	FIELD(TWI_CR, MSEN, 2, 1)
	FIELD(TWI_CR, MSDIS, 3, 1)
	FIELD(TWI_CR, SVEN, 4, 1)
	FIELD(TWI_CR, SVDIS, 5, 1)
	FIELD(TWI_CR, QUICK, 6, 1)
	FIELD(TWI_CR, SWRST, 7, 1)
REG32(TWI_MMR, 0x4)
	FIELD(TWI_MMR, IADRSZ, 8, 2)
	FIELD(TWI_MMR, MREAD, 12, 1)
	FIELD(TWI_MMR, DADR, 16, 7)
REG32(TWI_SMR, 0x8)
	FIELD(TWI_SMR, SADR, 16, 7)
REG32(TWI_IADR, 0xc)
	FIELD(TWI_IADR, IADR, 0, 24)
REG32(TWI_CWGR, 0x10)
	FIELD(TWI_CWGR, CLDIV, 0, 8)
	FIELD(TWI_CWGR, CHDIV, 8, 8)
	FIELD(TWI_CWGR, CKDIV, 16, 3)
REG32(TWI_SR, 0x20)
	FIELD(TWI_SR, TXCOMP, 0, 1)
	FIELD(TWI_SR, RXRDY, 1, 1)
	FIELD(TWI_SR, TXRDY, 2, 1)
	FIELD(TWI_SR, SVREAD, 3, 1)
	FIELD(TWI_SR, SVACC, 4, 1)
	FIELD(TWI_SR, GACC, 5, 1)
	FIELD(TWI_SR, OVRE, 6, 1)
	FIELD(TWI_SR, NACK, 8, 1)
	FIELD(TWI_SR, ARBLST, 9, 1)
	FIELD(TWI_SR, SCLWS, 10, 1)
	FIELD(TWI_SR, EOSACC, 11, 1)
	FIELD(TWI_SR, ENDRX, 12, 1)
	FIELD(TWI_SR, ENDTX, 13, 1)
	FIELD(TWI_SR, RXBUFF, 14, 1)
	FIELD(TWI_SR, TXBUFE, 15, 1)
REG32(TWI_IER, 0x24)
	FIELD(TWI_IER, TXCOMP, 0, 1)
	FIELD(TWI_IER, RXRDY, 1, 1)
	FIELD(TWI_IER, TXRDY, 2, 1)
	FIELD(TWI_IER, SVACC, 4, 1)
	FIELD(TWI_IER, GACC, 5, 1)
	FIELD(TWI_IER, OVRE, 6, 1)
	FIELD(TWI_IER, NACK, 8, 1)
	FIELD(TWI_IER, ARBLST, 9, 1)
	FIELD(TWI_IER, SCL_WS, 10, 1)
	FIELD(TWI_IER, EOSACC, 11, 1)
	FIELD(TWI_IER, ENDRX, 12, 1)
	FIELD(TWI_IER, ENDTX, 13, 1)
	FIELD(TWI_IER, RXBUFF, 14, 1)
	FIELD(TWI_IER, TXBUFE, 15, 1)
REG32(TWI_IDR, 0x28)
	FIELD(TWI_IDR, TXCOMP, 0, 1)
	FIELD(TWI_IDR, RXRDY, 1, 1)
	FIELD(TWI_IDR, TXRDY, 2, 1)
	FIELD(TWI_IDR, SVACC, 4, 1)
	FIELD(TWI_IDR, GACC, 5, 1)
	FIELD(TWI_IDR, OVRE, 6, 1)
	FIELD(TWI_IDR, NACK, 8, 1)
	FIELD(TWI_IDR, ARBLST, 9, 1)
	FIELD(TWI_IDR, SCL_WS, 10, 1)
	FIELD(TWI_IDR, EOSACC, 11, 1)
	FIELD(TWI_IDR, ENDRX, 12, 1)
	FIELD(TWI_IDR, ENDTX, 13, 1)
	FIELD(TWI_IDR, RXBUFF, 14, 1)
	FIELD(TWI_IDR, TXBUFE, 15, 1)
REG32(TWI_IMR, 0x2c)
	FIELD(TWI_IMR, TXCOMP, 0, 1)
	FIELD(TWI_IMR, RXRDY, 1, 1)
	FIELD(TWI_IMR, TXRDY, 2, 1)
	FIELD(TWI_IMR, SVACC, 4, 1)
	FIELD(TWI_IMR, GACC, 5, 1)
	FIELD(TWI_IMR, OVRE, 6, 1)
	FIELD(TWI_IMR, NACK, 8, 1)
	FIELD(TWI_IMR, ARBLST, 9, 1)
	FIELD(TWI_IMR, SCL_WS, 10, 1)
	FIELD(TWI_IMR, EOSACC, 11, 1)
	FIELD(TWI_IMR, ENDRX, 12, 1)
	FIELD(TWI_IMR, ENDTX, 13, 1)
	FIELD(TWI_IMR, RXBUFF, 14, 1)
	FIELD(TWI_IMR, TXBUFE, 15, 1)
REG32(TWI_RHR, 0x30)
	FIELD(TWI_RHR, RXDATA, 0, 8)
REG32(TWI_THR, 0x34)
	FIELD(TWI_THR, TXDATA, 0, 8)
REG32(TWI_RPR, 0x100)
	FIELD(TWI_RPR, RXPTR, 0, 32)
REG32(TWI_RCR, 0x104)
	FIELD(TWI_RCR, RXCTR, 0, 16)
REG32(TWI_TPR, 0x108)
	FIELD(TWI_TPR, TXPTR, 0, 32)
REG32(TWI_TCR, 0x10c)
	FIELD(TWI_TCR, TXCTR, 0, 16)
REG32(TWI_RNPR, 0x110)
	FIELD(TWI_RNPR, RXNPTR, 0, 32)
REG32(TWI_RNCR, 0x114)
	FIELD(TWI_RNCR, RXNCTR, 0, 16)
REG32(TWI_TNPR, 0x118)
	FIELD(TWI_TNPR, TXNPTR, 0, 32)
REG32(TWI_TNCR, 0x11c)
	FIELD(TWI_TNCR, TXNCTR, 0, 16)
REG32(TWI_PTCR, 0x120)
	FIELD(TWI_PTCR, RXTEN, 0, 1)
	FIELD(TWI_PTCR, RXTDIS, 1, 1)
	FIELD(TWI_PTCR, TXTEN, 8, 1)
	FIELD(TWI_PTCR, TXTDIS, 9, 1)
REG32(TWI_PTSR, 0x124)
	FIELD(TWI_PTSR, RXTEN, 0, 1)
	FIELD(TWI_PTSR, TXTEN, 8, 1)


static int atsam3x8e_twi_can_receive(void *opaque) {
	return 1;
}

static void atsam3x8e_twi_receive(void *opaque, const uint8_t *buf, int size) {
	ATSAM3X8ETWI *t = ATSAM3X8E_TWI(opaque);

	t->RHR = *buf;
	t->SR &= (~(0x100));
	t->SR |= 0x10;
	t->SR |= 0x2;
	t->SR &= (~(0x20));
	t->SR |= 0x8;


}

static gboolean atsam3x8e_twi_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	ATSAM3X8ETWI *t = ATSAM3X8E_TWI(opaque);
	int ret;

	t->watch_tag = 0;
	t->SR &= (~(0x4));
	t->SR |= 0x100;
	t->SR &= (~(0x1));



	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->THR), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, atsam3x8e_twi_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->SR |= 0x4;
	t->SR &= (~(0x100));
	t->SR |= 0x1;



	return FALSE;
}

static void atsam3x8e_twi_register_reset(ATSAM3X8ETWI *t) {
	t->MMR = 0x0;
	t->SMR = 0x0;
	t->IADR = 0x0;
	t->CWGR = 0x0;
	t->SR = 0xf009;
	t->IMR = 0x0;
	t->RHR = 0x0;
	t->THR = 0x0;
	t->RPR = 0x0;
	t->RCR = 0x0;
	t->TPR = 0x0;
	t->TCR = 0x0;
	t->RNPR = 0x0;
	t->RNCR = 0x0;
	t->TNPR = 0x0;
	t->TNCR = 0x0;
	t->PTCR = 0x0;
	t->PTSR = 0x0;
	t->SR |= 0x4;
	t->SR &= (~(0x100));
	t->SR |= 0x1;

}

static uint64_t atsam3x8e_twi_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAM3X8ETWI *t = ATSAM3X8E_TWI(opaque);
	uint64_t ret;

	switch (offset) {
		case A_TWI_MMR:
			ret = t->MMR;
			break;
		case A_TWI_SMR:
			ret = t->SMR;
			break;
		case A_TWI_IADR:
			ret = t->IADR;
			break;
		case A_TWI_CWGR:
			ret = t->CWGR;
			break;
		case A_TWI_SR:
			ret = t->SR;
			break;
		case A_TWI_IMR:
			ret = t->IMR;
			break;
		case A_TWI_RHR:
			ret = t->RHR;
			t->SR &= (~(0x8));
			t->SR |= 0x100;
			t->SR |= 0x20;
			t->SR &= (~(0x2));
			t->SR &= (~(0x10));
			qemu_chr_fe_accept_input(&(t->chr));
			break;
		case A_TWI_RPR:
			ret = t->RPR;
			break;
		case A_TWI_RCR:
			ret = t->RCR;
			break;
		case A_TWI_TPR:
			ret = t->TPR;
			break;
		case A_TWI_TCR:
			ret = t->TCR;
			break;
		case A_TWI_RNPR:
			ret = t->RNPR;
			break;
		case A_TWI_RNCR:
			ret = t->RNCR;
			break;
		case A_TWI_TNPR:
			ret = t->TNPR;
			break;
		case A_TWI_TNCR:
			ret = t->TNCR;
			break;
		case A_TWI_PTSR:
			ret = t->PTSR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM3X8E TWI read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsam3x8e_twi_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAM3X8ETWI *t = ATSAM3X8E_TWI(opaque);


	switch (offset) {
		case A_TWI_CR ... A_TWI_CR + 3:
			value = value << ((offset - A_TWI_CR) << 3);
			offset = A_TWI_CR;
			break;
		case A_TWI_MMR ... A_TWI_MMR + 3:
			value = value << ((offset - A_TWI_MMR) << 3);
			offset = A_TWI_MMR;
			break;
		case A_TWI_SMR ... A_TWI_SMR + 3:
			value = value << ((offset - A_TWI_SMR) << 3);
			offset = A_TWI_SMR;
			break;
		case A_TWI_IADR ... A_TWI_IADR + 3:
			value = value << ((offset - A_TWI_IADR) << 3);
			offset = A_TWI_IADR;
			break;
		case A_TWI_CWGR ... A_TWI_CWGR + 3:
			value = value << ((offset - A_TWI_CWGR) << 3);
			offset = A_TWI_CWGR;
			break;
		case A_TWI_IER ... A_TWI_IER + 3:
			value = value << ((offset - A_TWI_IER) << 3);
			offset = A_TWI_IER;
			break;
		case A_TWI_IDR ... A_TWI_IDR + 3:
			value = value << ((offset - A_TWI_IDR) << 3);
			offset = A_TWI_IDR;
			break;
		case A_TWI_THR ... A_TWI_THR + 3:
			value = value << ((offset - A_TWI_THR) << 3);
			offset = A_TWI_THR;
			break;
		case A_TWI_RPR ... A_TWI_RPR + 3:
			value = value << ((offset - A_TWI_RPR) << 3);
			offset = A_TWI_RPR;
			break;
		case A_TWI_RCR ... A_TWI_RCR + 3:
			value = value << ((offset - A_TWI_RCR) << 3);
			offset = A_TWI_RCR;
			break;
		case A_TWI_TPR ... A_TWI_TPR + 3:
			value = value << ((offset - A_TWI_TPR) << 3);
			offset = A_TWI_TPR;
			break;
		case A_TWI_TCR ... A_TWI_TCR + 3:
			value = value << ((offset - A_TWI_TCR) << 3);
			offset = A_TWI_TCR;
			break;
		case A_TWI_RNPR ... A_TWI_RNPR + 3:
			value = value << ((offset - A_TWI_RNPR) << 3);
			offset = A_TWI_RNPR;
			break;
		case A_TWI_RNCR ... A_TWI_RNCR + 3:
			value = value << ((offset - A_TWI_RNCR) << 3);
			offset = A_TWI_RNCR;
			break;
		case A_TWI_TNPR ... A_TWI_TNPR + 3:
			value = value << ((offset - A_TWI_TNPR) << 3);
			offset = A_TWI_TNPR;
			break;
		case A_TWI_TNCR ... A_TWI_TNCR + 3:
			value = value << ((offset - A_TWI_TNCR) << 3);
			offset = A_TWI_TNCR;
			break;
		case A_TWI_PTCR ... A_TWI_PTCR + 3:
			value = value << ((offset - A_TWI_PTCR) << 3);
			offset = A_TWI_PTCR;
			break;

		default: break;
	}

	switch (offset) {
		case A_TWI_CR:
			t->CR = value;
			if ((value == 0x2)) {
				t->SR |= 0x1;
			}
			break;
		case A_TWI_MMR:
			t->MMR = value;
			break;
		case A_TWI_SMR:
			t->SMR = value;
			break;
		case A_TWI_IADR:
			t->IADR = value;
			break;
		case A_TWI_CWGR:
			t->CWGR = value;
			break;
		case A_TWI_IER:
			t->IER = value;
			break;
		case A_TWI_IDR:
			t->IDR = value;
			break;
		case A_TWI_THR:
			t->THR = value;
			atsam3x8e_twi_transmit(NULL, G_IO_OUT, t);
			break;
		case A_TWI_RPR:
			t->RPR = value;
			break;
		case A_TWI_RCR:
			t->RCR = value;
			break;
		case A_TWI_TPR:
			t->TPR = value;
			break;
		case A_TWI_TCR:
			t->TCR = value;
			break;
		case A_TWI_RNPR:
			t->RNPR = value;
			break;
		case A_TWI_RNCR:
			t->RNCR = value;
			break;
		case A_TWI_TNPR:
			t->TNPR = value;
			break;
		case A_TWI_TNCR:
			t->TNCR = value;
			break;
		case A_TWI_PTCR:
			t->PTCR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM3X8E TWI write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsam3x8e_twi_ops = {
	.read = atsam3x8e_twi_read,
	.write = atsam3x8e_twi_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsam3x8e_twi_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAM3X8ETWI *t = ATSAM3X8E_TWI(obj);
	memory_region_init_io(&(t->iomem), obj, &atsam3x8e_twi_ops, t, "atsam3x8e-twi", ATSAM3X8E_TWI_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void atsam3x8e_twi_realize(DeviceState *dev, Error **errp) {
	ATSAM3X8ETWI *t = ATSAM3X8E_TWI(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), atsam3x8e_twi_can_receive, atsam3x8e_twi_receive,
		NULL, NULL, t, NULL, true);

}

static void atsam3x8e_twi_reset_enter(Object *obj, ResetType type) {
	ATSAM3X8ETWI *t = ATSAM3X8E_TWI(obj);
	atsam3x8e_twi_register_reset(t);
}

static const VMStateDescription atsam3x8e_twi_vmstate = {
	.name = "atsam3x8e-twi",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR, ATSAM3X8ETWI),
		VMSTATE_UINT32(MMR, ATSAM3X8ETWI),
		VMSTATE_UINT32(SMR, ATSAM3X8ETWI),
		VMSTATE_UINT32(IADR, ATSAM3X8ETWI),
		VMSTATE_UINT32(CWGR, ATSAM3X8ETWI),
		VMSTATE_UINT32(SR, ATSAM3X8ETWI),
		VMSTATE_UINT32(IER, ATSAM3X8ETWI),
		VMSTATE_UINT32(IDR, ATSAM3X8ETWI),
		VMSTATE_UINT32(IMR, ATSAM3X8ETWI),
		VMSTATE_UINT32(RHR, ATSAM3X8ETWI),
		VMSTATE_UINT32(THR, ATSAM3X8ETWI),
		VMSTATE_UINT32(RPR, ATSAM3X8ETWI),
		VMSTATE_UINT32(RCR, ATSAM3X8ETWI),
		VMSTATE_UINT32(TPR, ATSAM3X8ETWI),
		VMSTATE_UINT32(TCR, ATSAM3X8ETWI),
		VMSTATE_UINT32(RNPR, ATSAM3X8ETWI),
		VMSTATE_UINT32(RNCR, ATSAM3X8ETWI),
		VMSTATE_UINT32(TNPR, ATSAM3X8ETWI),
		VMSTATE_UINT32(TNCR, ATSAM3X8ETWI),
		VMSTATE_UINT32(PTCR, ATSAM3X8ETWI),
		VMSTATE_UINT32(PTSR, ATSAM3X8ETWI),

		VMSTATE_END_OF_LIST()
	}
};

static Property atsam3x8e_twi_properties[] = {
	DEFINE_PROP_CHR("chardev", ATSAM3X8ETWI, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void atsam3x8e_twi_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsam3x8e_twi_vmstate;
	dc->realize = atsam3x8e_twi_realize;
	rc->phases.enter = atsam3x8e_twi_reset_enter;
	device_class_set_props(dc, atsam3x8e_twi_properties);

}

static const TypeInfo atsam3x8e_twi_info = {
	.name = TYPE_ATSAM3X8E_TWI,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAM3X8ETWI),
	.instance_init = atsam3x8e_twi_init,
	.class_init = atsam3x8e_twi_class_init,
};

static void atsam3x8e_twi_register_types(void) {
	type_register_static(&atsam3x8e_twi_info);
}

type_init(atsam3x8e_twi_register_types);

#define TYPE_ATSAM3X8E_PWM "atsam3x8e-pwm"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAM3X8EPWM, ATSAM3X8E_PWM)

struct ATSAM3X8EPWM {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t CLK;
	uint32_t ENA;
	uint32_t DIS;
	uint32_t SR;
	uint32_t IER1;
	uint32_t IDR1;
	uint32_t IMR1;
	uint32_t ISR1;
	uint32_t SCM;
	uint32_t SCUC;
	uint32_t SCUP;
	uint32_t SCUPUPD;
	uint32_t IER2;
	uint32_t IDR2;
	uint32_t IMR2;
	uint32_t ISR2;
	uint32_t OOV;
	uint32_t OS;
	uint32_t OSS;
	uint32_t OSC;
	uint32_t OSSUPD;
	uint32_t OSCUPD;
	uint32_t FMR;
	uint32_t FSR;
	uint32_t FCR;
	uint32_t FPV;
	uint32_t FPE1;
	uint32_t FPE2;
	uint32_t SMMR;
	uint32_t WPCR;
	uint32_t WPSR;
	uint32_t CMPV0;
	uint32_t CMPVUPD0;
	uint32_t CMPM0;
	uint32_t CMPMUPD0;
	uint32_t CMPV1;
	uint32_t CMPVUPD1;
	uint32_t CMPM1;
	uint32_t CMPMUPD1;
	uint32_t CMPV2;
	uint32_t CMPVUPD2;
	uint32_t CMPM2;
	uint32_t CMPMUPD2;
	uint32_t CMPV3;
	uint32_t CMPVUPD3;
	uint32_t CMPM3;
	uint32_t CMPMUPD3;
	uint32_t CMPV4;
	uint32_t CMPVUPD4;
	uint32_t CMPM4;
	uint32_t CMPMUPD4;
	uint32_t CMPV5;
	uint32_t CMPVUPD5;
	uint32_t CMPM5;
	uint32_t CMPMUPD5;
	uint32_t CMPV6;
	uint32_t CMPVUPD6;
	uint32_t CMPM6;
	uint32_t CMPMUPD6;
	uint32_t CMPV7;
	uint32_t CMPVUPD7;
	uint32_t CMPM7;
	uint32_t CMPMUPD7;
	uint32_t CMR0;
	uint32_t CDTY0;
	uint32_t CDTYUPD0;
	uint32_t CPRD0;
	uint32_t CPRDUPD0;
	uint32_t CCNT0;
	uint32_t DT0;
	uint32_t DTUPD0;
	uint32_t CMR1;
	uint32_t CDTY1;
	uint32_t CDTYUPD1;
	uint32_t CPRD1;
	uint32_t CPRDUPD1;
	uint32_t CCNT1;
	uint32_t DT1;
	uint32_t DTUPD1;
	uint32_t CMR2;
	uint32_t CDTY2;
	uint32_t CDTYUPD2;
	uint32_t CPRD2;
	uint32_t CPRDUPD2;
	uint32_t CCNT2;
	uint32_t DT2;
	uint32_t DTUPD2;
	uint32_t CMR3;
	uint32_t CDTY3;
	uint32_t CDTYUPD3;
	uint32_t CPRD3;
	uint32_t CPRDUPD3;
	uint32_t CCNT3;
	uint32_t DT3;
	uint32_t DTUPD3;
	uint32_t CMR4;
	uint32_t CDTY4;
	uint32_t CDTYUPD4;
	uint32_t CPRD4;
	uint32_t CPRDUPD4;
	uint32_t CCNT4;
	uint32_t DT4;
	uint32_t DTUPD4;
	uint32_t CMR5;
	uint32_t CDTY5;
	uint32_t CDTYUPD5;
	uint32_t CPRD5;
	uint32_t CPRDUPD5;
	uint32_t CCNT5;
	uint32_t DT5;
	uint32_t DTUPD5;
	uint32_t CMR6;
	uint32_t CDTY6;
	uint32_t CDTYUPD6;
	uint32_t CPRD6;
	uint32_t CPRDUPD6;
	uint32_t CCNT6;
	uint32_t DT6;
	uint32_t DTUPD6;
	uint32_t CMR7;
	uint32_t CDTY7;
	uint32_t CDTYUPD7;
	uint32_t CPRD7;
	uint32_t CPRDUPD7;
	uint32_t CCNT7;
	uint32_t DT7;
	uint32_t DTUPD7;
	uint32_t TPR;
	uint32_t TCR;
	uint32_t TNPR;
	uint32_t TNCR;
	uint32_t PTCR;
	uint32_t PTSR;
	uint32_t ELMR0;
	uint32_t ELMR1;
	/* base */
	uint32_t base;

};
#define ATSAM3X8E_PWM_SIZE				0x300

REG32(PWM_CLK, 0x0)
	FIELD(PWM_CLK, DIVA, 0, 8)
	FIELD(PWM_CLK, PREA, 8, 4)
	FIELD(PWM_CLK, DIVB, 16, 8)
	FIELD(PWM_CLK, PREB, 24, 4)
REG32(PWM_ENA, 0x4)
	FIELD(PWM_ENA, CHID0, 0, 1)
	FIELD(PWM_ENA, CHID1, 1, 1)
	FIELD(PWM_ENA, CHID2, 2, 1)
	FIELD(PWM_ENA, CHID3, 3, 1)
	FIELD(PWM_ENA, CHID4, 4, 1)
	FIELD(PWM_ENA, CHID5, 5, 1)
	FIELD(PWM_ENA, CHID6, 6, 1)
	FIELD(PWM_ENA, CHID7, 7, 1)
REG32(PWM_DIS, 0x8)
	FIELD(PWM_DIS, CHID0, 0, 1)
	FIELD(PWM_DIS, CHID1, 1, 1)
	FIELD(PWM_DIS, CHID2, 2, 1)
	FIELD(PWM_DIS, CHID3, 3, 1)
	FIELD(PWM_DIS, CHID4, 4, 1)
	FIELD(PWM_DIS, CHID5, 5, 1)
	FIELD(PWM_DIS, CHID6, 6, 1)
	FIELD(PWM_DIS, CHID7, 7, 1)
REG32(PWM_SR, 0xc)
	FIELD(PWM_SR, CHID0, 0, 1)
	FIELD(PWM_SR, CHID1, 1, 1)
	FIELD(PWM_SR, CHID2, 2, 1)
	FIELD(PWM_SR, CHID3, 3, 1)
	FIELD(PWM_SR, CHID4, 4, 1)
	FIELD(PWM_SR, CHID5, 5, 1)
	FIELD(PWM_SR, CHID6, 6, 1)
	FIELD(PWM_SR, CHID7, 7, 1)
REG32(PWM_IER1, 0x10)
	FIELD(PWM_IER1, CHID0, 0, 1)
	FIELD(PWM_IER1, CHID1, 1, 1)
	FIELD(PWM_IER1, CHID2, 2, 1)
	FIELD(PWM_IER1, CHID3, 3, 1)
	FIELD(PWM_IER1, CHID4, 4, 1)
	FIELD(PWM_IER1, CHID5, 5, 1)
	FIELD(PWM_IER1, CHID6, 6, 1)
	FIELD(PWM_IER1, CHID7, 7, 1)
	FIELD(PWM_IER1, FCHID0, 16, 1)
	FIELD(PWM_IER1, FCHID1, 17, 1)
	FIELD(PWM_IER1, FCHID2, 18, 1)
	FIELD(PWM_IER1, FCHID3, 19, 1)
	FIELD(PWM_IER1, FCHID4, 20, 1)
	FIELD(PWM_IER1, FCHID5, 21, 1)
	FIELD(PWM_IER1, FCHID6, 22, 1)
	FIELD(PWM_IER1, FCHID7, 23, 1)
REG32(PWM_IDR1, 0x14)
	FIELD(PWM_IDR1, CHID0, 0, 1)
	FIELD(PWM_IDR1, CHID1, 1, 1)
	FIELD(PWM_IDR1, CHID2, 2, 1)
	FIELD(PWM_IDR1, CHID3, 3, 1)
	FIELD(PWM_IDR1, CHID4, 4, 1)
	FIELD(PWM_IDR1, CHID5, 5, 1)
	FIELD(PWM_IDR1, CHID6, 6, 1)
	FIELD(PWM_IDR1, CHID7, 7, 1)
	FIELD(PWM_IDR1, FCHID0, 16, 1)
	FIELD(PWM_IDR1, FCHID1, 17, 1)
	FIELD(PWM_IDR1, FCHID2, 18, 1)
	FIELD(PWM_IDR1, FCHID3, 19, 1)
	FIELD(PWM_IDR1, FCHID4, 20, 1)
	FIELD(PWM_IDR1, FCHID5, 21, 1)
	FIELD(PWM_IDR1, FCHID6, 22, 1)
	FIELD(PWM_IDR1, FCHID7, 23, 1)
REG32(PWM_IMR1, 0x18)
	FIELD(PWM_IMR1, CHID0, 0, 1)
	FIELD(PWM_IMR1, CHID1, 1, 1)
	FIELD(PWM_IMR1, CHID2, 2, 1)
	FIELD(PWM_IMR1, CHID3, 3, 1)
	FIELD(PWM_IMR1, CHID4, 4, 1)
	FIELD(PWM_IMR1, CHID5, 5, 1)
	FIELD(PWM_IMR1, CHID6, 6, 1)
	FIELD(PWM_IMR1, CHID7, 7, 1)
	FIELD(PWM_IMR1, FCHID0, 16, 1)
	FIELD(PWM_IMR1, FCHID1, 17, 1)
	FIELD(PWM_IMR1, FCHID2, 18, 1)
	FIELD(PWM_IMR1, FCHID3, 19, 1)
	FIELD(PWM_IMR1, FCHID4, 20, 1)
	FIELD(PWM_IMR1, FCHID5, 21, 1)
	FIELD(PWM_IMR1, FCHID6, 22, 1)
	FIELD(PWM_IMR1, FCHID7, 23, 1)
REG32(PWM_ISR1, 0x1c)
	FIELD(PWM_ISR1, CHID0, 0, 1)
	FIELD(PWM_ISR1, CHID1, 1, 1)
	FIELD(PWM_ISR1, CHID2, 2, 1)
	FIELD(PWM_ISR1, CHID3, 3, 1)
	FIELD(PWM_ISR1, CHID4, 4, 1)
	FIELD(PWM_ISR1, CHID5, 5, 1)
	FIELD(PWM_ISR1, CHID6, 6, 1)
	FIELD(PWM_ISR1, CHID7, 7, 1)
	FIELD(PWM_ISR1, FCHID0, 16, 1)
	FIELD(PWM_ISR1, FCHID1, 17, 1)
	FIELD(PWM_ISR1, FCHID2, 18, 1)
	FIELD(PWM_ISR1, FCHID3, 19, 1)
	FIELD(PWM_ISR1, FCHID4, 20, 1)
	FIELD(PWM_ISR1, FCHID5, 21, 1)
	FIELD(PWM_ISR1, FCHID6, 22, 1)
	FIELD(PWM_ISR1, FCHID7, 23, 1)
REG32(PWM_SCM, 0x20)
	FIELD(PWM_SCM, SYNC0, 0, 1)
	FIELD(PWM_SCM, SYNC1, 1, 1)
	FIELD(PWM_SCM, SYNC2, 2, 1)
	FIELD(PWM_SCM, SYNC3, 3, 1)
	FIELD(PWM_SCM, SYNC4, 4, 1)
	FIELD(PWM_SCM, SYNC5, 5, 1)
	FIELD(PWM_SCM, SYNC6, 6, 1)
	FIELD(PWM_SCM, SYNC7, 7, 1)
	FIELD(PWM_SCM, UPDM, 16, 2)
	FIELD(PWM_SCM, PTRM, 20, 1)
	FIELD(PWM_SCM, PTRCS, 21, 3)
REG32(PWM_SCUC, 0x28)
	FIELD(PWM_SCUC, UPDULOCK, 0, 1)
REG32(PWM_SCUP, 0x2c)
	FIELD(PWM_SCUP, UPR, 0, 4)
	FIELD(PWM_SCUP, UPRCNT, 4, 4)
REG32(PWM_SCUPUPD, 0x30)
	FIELD(PWM_SCUPUPD, UPRUPD, 0, 4)
REG32(PWM_IER2, 0x34)
	FIELD(PWM_IER2, WRDY, 0, 1)
	FIELD(PWM_IER2, ENDTX, 1, 1)
	FIELD(PWM_IER2, TXBUFE, 2, 1)
	FIELD(PWM_IER2, UNRE, 3, 1)
	FIELD(PWM_IER2, CMPM0, 8, 1)
	FIELD(PWM_IER2, CMPM1, 9, 1)
	FIELD(PWM_IER2, CMPM2, 10, 1)
	FIELD(PWM_IER2, CMPM3, 11, 1)
	FIELD(PWM_IER2, CMPM4, 12, 1)
	FIELD(PWM_IER2, CMPM5, 13, 1)
	FIELD(PWM_IER2, CMPM6, 14, 1)
	FIELD(PWM_IER2, CMPM7, 15, 1)
	FIELD(PWM_IER2, CMPU0, 16, 1)
	FIELD(PWM_IER2, CMPU1, 17, 1)
	FIELD(PWM_IER2, CMPU2, 18, 1)
	FIELD(PWM_IER2, CMPU3, 19, 1)
	FIELD(PWM_IER2, CMPU4, 20, 1)
	FIELD(PWM_IER2, CMPU5, 21, 1)
	FIELD(PWM_IER2, CMPU6, 22, 1)
	FIELD(PWM_IER2, CMPU7, 23, 1)
REG32(PWM_IDR2, 0x38)
	FIELD(PWM_IDR2, WRDY, 0, 1)
	FIELD(PWM_IDR2, ENDTX, 1, 1)
	FIELD(PWM_IDR2, TXBUFE, 2, 1)
	FIELD(PWM_IDR2, UNRE, 3, 1)
	FIELD(PWM_IDR2, CMPM0, 8, 1)
	FIELD(PWM_IDR2, CMPM1, 9, 1)
	FIELD(PWM_IDR2, CMPM2, 10, 1)
	FIELD(PWM_IDR2, CMPM3, 11, 1)
	FIELD(PWM_IDR2, CMPM4, 12, 1)
	FIELD(PWM_IDR2, CMPM5, 13, 1)
	FIELD(PWM_IDR2, CMPM6, 14, 1)
	FIELD(PWM_IDR2, CMPM7, 15, 1)
	FIELD(PWM_IDR2, CMPU0, 16, 1)
	FIELD(PWM_IDR2, CMPU1, 17, 1)
	FIELD(PWM_IDR2, CMPU2, 18, 1)
	FIELD(PWM_IDR2, CMPU3, 19, 1)
	FIELD(PWM_IDR2, CMPU4, 20, 1)
	FIELD(PWM_IDR2, CMPU5, 21, 1)
	FIELD(PWM_IDR2, CMPU6, 22, 1)
	FIELD(PWM_IDR2, CMPU7, 23, 1)
REG32(PWM_IMR2, 0x3c)
	FIELD(PWM_IMR2, WRDY, 0, 1)
	FIELD(PWM_IMR2, ENDTX, 1, 1)
	FIELD(PWM_IMR2, TXBUFE, 2, 1)
	FIELD(PWM_IMR2, UNRE, 3, 1)
	FIELD(PWM_IMR2, CMPM0, 8, 1)
	FIELD(PWM_IMR2, CMPM1, 9, 1)
	FIELD(PWM_IMR2, CMPM2, 10, 1)
	FIELD(PWM_IMR2, CMPM3, 11, 1)
	FIELD(PWM_IMR2, CMPM4, 12, 1)
	FIELD(PWM_IMR2, CMPM5, 13, 1)
	FIELD(PWM_IMR2, CMPM6, 14, 1)
	FIELD(PWM_IMR2, CMPM7, 15, 1)
	FIELD(PWM_IMR2, CMPU0, 16, 1)
	FIELD(PWM_IMR2, CMPU1, 17, 1)
	FIELD(PWM_IMR2, CMPU2, 18, 1)
	FIELD(PWM_IMR2, CMPU3, 19, 1)
	FIELD(PWM_IMR2, CMPU4, 20, 1)
	FIELD(PWM_IMR2, CMPU5, 21, 1)
	FIELD(PWM_IMR2, CMPU6, 22, 1)
	FIELD(PWM_IMR2, CMPU7, 23, 1)
REG32(PWM_ISR2, 0x40)
	FIELD(PWM_ISR2, WRDY, 0, 1)
	FIELD(PWM_ISR2, ENDTX, 1, 1)
	FIELD(PWM_ISR2, TXBUFE, 2, 1)
	FIELD(PWM_ISR2, UNRE, 3, 1)
	FIELD(PWM_ISR2, CMPM0, 8, 1)
	FIELD(PWM_ISR2, CMPM1, 9, 1)
	FIELD(PWM_ISR2, CMPM2, 10, 1)
	FIELD(PWM_ISR2, CMPM3, 11, 1)
	FIELD(PWM_ISR2, CMPM4, 12, 1)
	FIELD(PWM_ISR2, CMPM5, 13, 1)
	FIELD(PWM_ISR2, CMPM6, 14, 1)
	FIELD(PWM_ISR2, CMPM7, 15, 1)
	FIELD(PWM_ISR2, CMPU0, 16, 1)
	FIELD(PWM_ISR2, CMPU1, 17, 1)
	FIELD(PWM_ISR2, CMPU2, 18, 1)
	FIELD(PWM_ISR2, CMPU3, 19, 1)
	FIELD(PWM_ISR2, CMPU4, 20, 1)
	FIELD(PWM_ISR2, CMPU5, 21, 1)
	FIELD(PWM_ISR2, CMPU6, 22, 1)
	FIELD(PWM_ISR2, CMPU7, 23, 1)
REG32(PWM_OOV, 0x44)
	FIELD(PWM_OOV, OOVH0, 0, 1)
	FIELD(PWM_OOV, OOVH1, 1, 1)
	FIELD(PWM_OOV, OOVH2, 2, 1)
	FIELD(PWM_OOV, OOVH3, 3, 1)
	FIELD(PWM_OOV, OOVH4, 4, 1)
	FIELD(PWM_OOV, OOVH5, 5, 1)
	FIELD(PWM_OOV, OOVH6, 6, 1)
	FIELD(PWM_OOV, OOVH7, 7, 1)
	FIELD(PWM_OOV, OOVL0, 16, 1)
	FIELD(PWM_OOV, OOVL1, 17, 1)
	FIELD(PWM_OOV, OOVL2, 18, 1)
	FIELD(PWM_OOV, OOVL3, 19, 1)
	FIELD(PWM_OOV, OOVL4, 20, 1)
	FIELD(PWM_OOV, OOVL5, 21, 1)
	FIELD(PWM_OOV, OOVL6, 22, 1)
	FIELD(PWM_OOV, OOVL7, 23, 1)
REG32(PWM_OS, 0x48)
	FIELD(PWM_OS, OSH0, 0, 1)
	FIELD(PWM_OS, OSH1, 1, 1)
	FIELD(PWM_OS, OSH2, 2, 1)
	FIELD(PWM_OS, OSH3, 3, 1)
	FIELD(PWM_OS, OSH4, 4, 1)
	FIELD(PWM_OS, OSH5, 5, 1)
	FIELD(PWM_OS, OSH6, 6, 1)
	FIELD(PWM_OS, OSH7, 7, 1)
	FIELD(PWM_OS, OSL0, 16, 1)
	FIELD(PWM_OS, OSL1, 17, 1)
	FIELD(PWM_OS, OSL2, 18, 1)
	FIELD(PWM_OS, OSL3, 19, 1)
	FIELD(PWM_OS, OSL4, 20, 1)
	FIELD(PWM_OS, OSL5, 21, 1)
	FIELD(PWM_OS, OSL6, 22, 1)
	FIELD(PWM_OS, OSL7, 23, 1)
REG32(PWM_OSS, 0x4c)
	FIELD(PWM_OSS, OSSH0, 0, 1)
	FIELD(PWM_OSS, OSSH1, 1, 1)
	FIELD(PWM_OSS, OSSH2, 2, 1)
	FIELD(PWM_OSS, OSSH3, 3, 1)
	FIELD(PWM_OSS, OSSH4, 4, 1)
	FIELD(PWM_OSS, OSSH5, 5, 1)
	FIELD(PWM_OSS, OSSH6, 6, 1)
	FIELD(PWM_OSS, OSSH7, 7, 1)
	FIELD(PWM_OSS, OSSL0, 16, 1)
	FIELD(PWM_OSS, OSSL1, 17, 1)
	FIELD(PWM_OSS, OSSL2, 18, 1)
	FIELD(PWM_OSS, OSSL3, 19, 1)
	FIELD(PWM_OSS, OSSL4, 20, 1)
	FIELD(PWM_OSS, OSSL5, 21, 1)
	FIELD(PWM_OSS, OSSL6, 22, 1)
	FIELD(PWM_OSS, OSSL7, 23, 1)
REG32(PWM_OSC, 0x50)
	FIELD(PWM_OSC, OSCH0, 0, 1)
	FIELD(PWM_OSC, OSCH1, 1, 1)
	FIELD(PWM_OSC, OSCH2, 2, 1)
	FIELD(PWM_OSC, OSCH3, 3, 1)
	FIELD(PWM_OSC, OSCH4, 4, 1)
	FIELD(PWM_OSC, OSCH5, 5, 1)
	FIELD(PWM_OSC, OSCH6, 6, 1)
	FIELD(PWM_OSC, OSCH7, 7, 1)
	FIELD(PWM_OSC, OSCL0, 16, 1)
	FIELD(PWM_OSC, OSCL1, 17, 1)
	FIELD(PWM_OSC, OSCL2, 18, 1)
	FIELD(PWM_OSC, OSCL3, 19, 1)
	FIELD(PWM_OSC, OSCL4, 20, 1)
	FIELD(PWM_OSC, OSCL5, 21, 1)
	FIELD(PWM_OSC, OSCL6, 22, 1)
	FIELD(PWM_OSC, OSCL7, 23, 1)
REG32(PWM_OSSUPD, 0x54)
	FIELD(PWM_OSSUPD, OSSUPH0, 0, 1)
	FIELD(PWM_OSSUPD, OSSUPH1, 1, 1)
	FIELD(PWM_OSSUPD, OSSUPH2, 2, 1)
	FIELD(PWM_OSSUPD, OSSUPH3, 3, 1)
	FIELD(PWM_OSSUPD, OSSUPH4, 4, 1)
	FIELD(PWM_OSSUPD, OSSUPH5, 5, 1)
	FIELD(PWM_OSSUPD, OSSUPH6, 6, 1)
	FIELD(PWM_OSSUPD, OSSUPH7, 7, 1)
	FIELD(PWM_OSSUPD, OSSUPL0, 16, 1)
	FIELD(PWM_OSSUPD, OSSUPL1, 17, 1)
	FIELD(PWM_OSSUPD, OSSUPL2, 18, 1)
	FIELD(PWM_OSSUPD, OSSUPL3, 19, 1)
	FIELD(PWM_OSSUPD, OSSUPL4, 20, 1)
	FIELD(PWM_OSSUPD, OSSUPL5, 21, 1)
	FIELD(PWM_OSSUPD, OSSUPL6, 22, 1)
	FIELD(PWM_OSSUPD, OSSUPL7, 23, 1)
REG32(PWM_OSCUPD, 0x58)
	FIELD(PWM_OSCUPD, OSCUPH0, 0, 1)
	FIELD(PWM_OSCUPD, OSCUPH1, 1, 1)
	FIELD(PWM_OSCUPD, OSCUPH2, 2, 1)
	FIELD(PWM_OSCUPD, OSCUPH3, 3, 1)
	FIELD(PWM_OSCUPD, OSCUPH4, 4, 1)
	FIELD(PWM_OSCUPD, OSCUPH5, 5, 1)
	FIELD(PWM_OSCUPD, OSCUPH6, 6, 1)
	FIELD(PWM_OSCUPD, OSCUPH7, 7, 1)
	FIELD(PWM_OSCUPD, OSCUPL0, 16, 1)
	FIELD(PWM_OSCUPD, OSCUPL1, 17, 1)
	FIELD(PWM_OSCUPD, OSCUPL2, 18, 1)
	FIELD(PWM_OSCUPD, OSCUPL3, 19, 1)
	FIELD(PWM_OSCUPD, OSCUPL4, 20, 1)
	FIELD(PWM_OSCUPD, OSCUPL5, 21, 1)
	FIELD(PWM_OSCUPD, OSCUPL6, 22, 1)
	FIELD(PWM_OSCUPD, OSCUPL7, 23, 1)
REG32(PWM_FMR, 0x5c)
	FIELD(PWM_FMR, FPOL, 0, 8)
	FIELD(PWM_FMR, FMOD, 8, 8)
	FIELD(PWM_FMR, FFIL, 16, 8)
REG32(PWM_FSR, 0x60)
	FIELD(PWM_FSR, FIV, 0, 8)
	FIELD(PWM_FSR, FS, 8, 8)
REG32(PWM_FCR, 0x64)
	FIELD(PWM_FCR, FCLR, 0, 8)
REG32(PWM_FPV, 0x68)
	FIELD(PWM_FPV, FPVH0, 0, 1)
	FIELD(PWM_FPV, FPVH1, 1, 1)
	FIELD(PWM_FPV, FPVH2, 2, 1)
	FIELD(PWM_FPV, FPVH3, 3, 1)
	FIELD(PWM_FPV, FPVH4, 4, 1)
	FIELD(PWM_FPV, FPVH5, 5, 1)
	FIELD(PWM_FPV, FPVH6, 6, 1)
	FIELD(PWM_FPV, FPVH7, 7, 1)
	FIELD(PWM_FPV, FPVL0, 16, 1)
	FIELD(PWM_FPV, FPVL1, 17, 1)
	FIELD(PWM_FPV, FPVL2, 18, 1)
	FIELD(PWM_FPV, FPVL3, 19, 1)
	FIELD(PWM_FPV, FPVL4, 20, 1)
	FIELD(PWM_FPV, FPVL5, 21, 1)
	FIELD(PWM_FPV, FPVL6, 22, 1)
	FIELD(PWM_FPV, FPVL7, 23, 1)
REG32(PWM_FPE1, 0x6c)
	FIELD(PWM_FPE1, FPE0, 0, 8)
	FIELD(PWM_FPE1, FPE1, 8, 8)
	FIELD(PWM_FPE1, FPE2, 16, 8)
	FIELD(PWM_FPE1, FPE3, 24, 8)
REG32(PWM_FPE2, 0x70)
	FIELD(PWM_FPE2, FPE4, 0, 8)
	FIELD(PWM_FPE2, FPE5, 8, 8)
	FIELD(PWM_FPE2, FPE6, 16, 8)
	FIELD(PWM_FPE2, FPE7, 24, 8)
REG32(PWM_SMMR, 0xb0)
	FIELD(PWM_SMMR, GCEN0, 0, 1)
	FIELD(PWM_SMMR, GCEN1, 1, 1)
	FIELD(PWM_SMMR, GCEN2, 2, 1)
	FIELD(PWM_SMMR, GCEN3, 3, 1)
	FIELD(PWM_SMMR, DOWN0, 16, 1)
	FIELD(PWM_SMMR, DOWN1, 17, 1)
	FIELD(PWM_SMMR, DOWN2, 18, 1)
	FIELD(PWM_SMMR, DOWN3, 19, 1)
REG32(PWM_WPCR, 0xe4)
	FIELD(PWM_WPCR, WPCMD, 0, 2)
	FIELD(PWM_WPCR, WPRG0, 2, 1)
	FIELD(PWM_WPCR, WPRG1, 3, 1)
	FIELD(PWM_WPCR, WPRG2, 4, 1)
	FIELD(PWM_WPCR, WPRG3, 5, 1)
	FIELD(PWM_WPCR, WPRG4, 6, 1)
	FIELD(PWM_WPCR, WPRG5, 7, 1)
	FIELD(PWM_WPCR, WPKEY, 8, 24)
REG32(PWM_WPSR, 0xe8)
	FIELD(PWM_WPSR, WPSWS0, 0, 1)
	FIELD(PWM_WPSR, WPSWS1, 1, 1)
	FIELD(PWM_WPSR, WPSWS2, 2, 1)
	FIELD(PWM_WPSR, WPSWS3, 3, 1)
	FIELD(PWM_WPSR, WPSWS4, 4, 1)
	FIELD(PWM_WPSR, WPSWS5, 5, 1)
	FIELD(PWM_WPSR, WPVS, 7, 1)
	FIELD(PWM_WPSR, WPHWS0, 8, 1)
	FIELD(PWM_WPSR, WPHWS1, 9, 1)
	FIELD(PWM_WPSR, WPHWS2, 10, 1)
	FIELD(PWM_WPSR, WPHWS3, 11, 1)
	FIELD(PWM_WPSR, WPHWS4, 12, 1)
	FIELD(PWM_WPSR, WPHWS5, 13, 1)
	FIELD(PWM_WPSR, WPVSRC, 16, 16)
REG32(PWM_CMPV0, 0x130)
	FIELD(PWM_CMPV0, CV, 0, 24)
	FIELD(PWM_CMPV0, CVM, 24, 1)
REG32(PWM_CMPVUPD0, 0x134)
	FIELD(PWM_CMPVUPD0, CVUPD, 0, 24)
	FIELD(PWM_CMPVUPD0, CVMUPD, 24, 1)
REG32(PWM_CMPM0, 0x138)
	FIELD(PWM_CMPM0, CEN, 0, 1)
	FIELD(PWM_CMPM0, CTR, 4, 4)
	FIELD(PWM_CMPM0, CPR, 8, 4)
	FIELD(PWM_CMPM0, CPRCNT, 12, 4)
	FIELD(PWM_CMPM0, CUPR, 16, 4)
	FIELD(PWM_CMPM0, CUPRCNT, 20, 4)
REG32(PWM_CMPMUPD0, 0x13c)
	FIELD(PWM_CMPMUPD0, CENUPD, 0, 1)
	FIELD(PWM_CMPMUPD0, CTRUPD, 4, 4)
	FIELD(PWM_CMPMUPD0, CPRUPD, 8, 4)
	FIELD(PWM_CMPMUPD0, CUPRUPD, 16, 4)
REG32(PWM_CMPV1, 0x140)
	FIELD(PWM_CMPV1, CV, 0, 24)
	FIELD(PWM_CMPV1, CVM, 24, 1)
REG32(PWM_CMPVUPD1, 0x144)
	FIELD(PWM_CMPVUPD1, CVUPD, 0, 24)
	FIELD(PWM_CMPVUPD1, CVMUPD, 24, 1)
REG32(PWM_CMPM1, 0x148)
	FIELD(PWM_CMPM1, CEN, 0, 1)
	FIELD(PWM_CMPM1, CTR, 4, 4)
	FIELD(PWM_CMPM1, CPR, 8, 4)
	FIELD(PWM_CMPM1, CPRCNT, 12, 4)
	FIELD(PWM_CMPM1, CUPR, 16, 4)
	FIELD(PWM_CMPM1, CUPRCNT, 20, 4)
REG32(PWM_CMPMUPD1, 0x14c)
	FIELD(PWM_CMPMUPD1, CENUPD, 0, 1)
	FIELD(PWM_CMPMUPD1, CTRUPD, 4, 4)
	FIELD(PWM_CMPMUPD1, CPRUPD, 8, 4)
	FIELD(PWM_CMPMUPD1, CUPRUPD, 16, 4)
REG32(PWM_CMPV2, 0x150)
	FIELD(PWM_CMPV2, CV, 0, 24)
	FIELD(PWM_CMPV2, CVM, 24, 1)
REG32(PWM_CMPVUPD2, 0x154)
	FIELD(PWM_CMPVUPD2, CVUPD, 0, 24)
	FIELD(PWM_CMPVUPD2, CVMUPD, 24, 1)
REG32(PWM_CMPM2, 0x158)
	FIELD(PWM_CMPM2, CEN, 0, 1)
	FIELD(PWM_CMPM2, CTR, 4, 4)
	FIELD(PWM_CMPM2, CPR, 8, 4)
	FIELD(PWM_CMPM2, CPRCNT, 12, 4)
	FIELD(PWM_CMPM2, CUPR, 16, 4)
	FIELD(PWM_CMPM2, CUPRCNT, 20, 4)
REG32(PWM_CMPMUPD2, 0x15c)
	FIELD(PWM_CMPMUPD2, CENUPD, 0, 1)
	FIELD(PWM_CMPMUPD2, CTRUPD, 4, 4)
	FIELD(PWM_CMPMUPD2, CPRUPD, 8, 4)
	FIELD(PWM_CMPMUPD2, CUPRUPD, 16, 4)
REG32(PWM_CMPV3, 0x160)
	FIELD(PWM_CMPV3, CV, 0, 24)
	FIELD(PWM_CMPV3, CVM, 24, 1)
REG32(PWM_CMPVUPD3, 0x164)
	FIELD(PWM_CMPVUPD3, CVUPD, 0, 24)
	FIELD(PWM_CMPVUPD3, CVMUPD, 24, 1)
REG32(PWM_CMPM3, 0x168)
	FIELD(PWM_CMPM3, CEN, 0, 1)
	FIELD(PWM_CMPM3, CTR, 4, 4)
	FIELD(PWM_CMPM3, CPR, 8, 4)
	FIELD(PWM_CMPM3, CPRCNT, 12, 4)
	FIELD(PWM_CMPM3, CUPR, 16, 4)
	FIELD(PWM_CMPM3, CUPRCNT, 20, 4)
REG32(PWM_CMPMUPD3, 0x16c)
	FIELD(PWM_CMPMUPD3, CENUPD, 0, 1)
	FIELD(PWM_CMPMUPD3, CTRUPD, 4, 4)
	FIELD(PWM_CMPMUPD3, CPRUPD, 8, 4)
	FIELD(PWM_CMPMUPD3, CUPRUPD, 16, 4)
REG32(PWM_CMPV4, 0x170)
	FIELD(PWM_CMPV4, CV, 0, 24)
	FIELD(PWM_CMPV4, CVM, 24, 1)
REG32(PWM_CMPVUPD4, 0x174)
	FIELD(PWM_CMPVUPD4, CVUPD, 0, 24)
	FIELD(PWM_CMPVUPD4, CVMUPD, 24, 1)
REG32(PWM_CMPM4, 0x178)
	FIELD(PWM_CMPM4, CEN, 0, 1)
	FIELD(PWM_CMPM4, CTR, 4, 4)
	FIELD(PWM_CMPM4, CPR, 8, 4)
	FIELD(PWM_CMPM4, CPRCNT, 12, 4)
	FIELD(PWM_CMPM4, CUPR, 16, 4)
	FIELD(PWM_CMPM4, CUPRCNT, 20, 4)
REG32(PWM_CMPMUPD4, 0x17c)
	FIELD(PWM_CMPMUPD4, CENUPD, 0, 1)
	FIELD(PWM_CMPMUPD4, CTRUPD, 4, 4)
	FIELD(PWM_CMPMUPD4, CPRUPD, 8, 4)
	FIELD(PWM_CMPMUPD4, CUPRUPD, 16, 4)
REG32(PWM_CMPV5, 0x180)
	FIELD(PWM_CMPV5, CV, 0, 24)
	FIELD(PWM_CMPV5, CVM, 24, 1)
REG32(PWM_CMPVUPD5, 0x184)
	FIELD(PWM_CMPVUPD5, CVUPD, 0, 24)
	FIELD(PWM_CMPVUPD5, CVMUPD, 24, 1)
REG32(PWM_CMPM5, 0x188)
	FIELD(PWM_CMPM5, CEN, 0, 1)
	FIELD(PWM_CMPM5, CTR, 4, 4)
	FIELD(PWM_CMPM5, CPR, 8, 4)
	FIELD(PWM_CMPM5, CPRCNT, 12, 4)
	FIELD(PWM_CMPM5, CUPR, 16, 4)
	FIELD(PWM_CMPM5, CUPRCNT, 20, 4)
REG32(PWM_CMPMUPD5, 0x18c)
	FIELD(PWM_CMPMUPD5, CENUPD, 0, 1)
	FIELD(PWM_CMPMUPD5, CTRUPD, 4, 4)
	FIELD(PWM_CMPMUPD5, CPRUPD, 8, 4)
	FIELD(PWM_CMPMUPD5, CUPRUPD, 16, 4)
REG32(PWM_CMPV6, 0x190)
	FIELD(PWM_CMPV6, CV, 0, 24)
	FIELD(PWM_CMPV6, CVM, 24, 1)
REG32(PWM_CMPVUPD6, 0x194)
	FIELD(PWM_CMPVUPD6, CVUPD, 0, 24)
	FIELD(PWM_CMPVUPD6, CVMUPD, 24, 1)
REG32(PWM_CMPM6, 0x198)
	FIELD(PWM_CMPM6, CEN, 0, 1)
	FIELD(PWM_CMPM6, CTR, 4, 4)
	FIELD(PWM_CMPM6, CPR, 8, 4)
	FIELD(PWM_CMPM6, CPRCNT, 12, 4)
	FIELD(PWM_CMPM6, CUPR, 16, 4)
	FIELD(PWM_CMPM6, CUPRCNT, 20, 4)
REG32(PWM_CMPMUPD6, 0x19c)
	FIELD(PWM_CMPMUPD6, CENUPD, 0, 1)
	FIELD(PWM_CMPMUPD6, CTRUPD, 4, 4)
	FIELD(PWM_CMPMUPD6, CPRUPD, 8, 4)
	FIELD(PWM_CMPMUPD6, CUPRUPD, 16, 4)
REG32(PWM_CMPV7, 0x1a0)
	FIELD(PWM_CMPV7, CV, 0, 24)
	FIELD(PWM_CMPV7, CVM, 24, 1)
REG32(PWM_CMPVUPD7, 0x1a4)
	FIELD(PWM_CMPVUPD7, CVUPD, 0, 24)
	FIELD(PWM_CMPVUPD7, CVMUPD, 24, 1)
REG32(PWM_CMPM7, 0x1a8)
	FIELD(PWM_CMPM7, CEN, 0, 1)
	FIELD(PWM_CMPM7, CTR, 4, 4)
	FIELD(PWM_CMPM7, CPR, 8, 4)
	FIELD(PWM_CMPM7, CPRCNT, 12, 4)
	FIELD(PWM_CMPM7, CUPR, 16, 4)
	FIELD(PWM_CMPM7, CUPRCNT, 20, 4)
REG32(PWM_CMPMUPD7, 0x1ac)
	FIELD(PWM_CMPMUPD7, CENUPD, 0, 1)
	FIELD(PWM_CMPMUPD7, CTRUPD, 4, 4)
	FIELD(PWM_CMPMUPD7, CPRUPD, 8, 4)
	FIELD(PWM_CMPMUPD7, CUPRUPD, 16, 4)
REG32(PWM_CMR0, 0x200)
	FIELD(PWM_CMR0, CPRE, 0, 4)
	FIELD(PWM_CMR0, CALG, 8, 1)
	FIELD(PWM_CMR0, CPOL, 9, 1)
	FIELD(PWM_CMR0, CES, 10, 1)
	FIELD(PWM_CMR0, DTE, 16, 1)
	FIELD(PWM_CMR0, DTHI, 17, 1)
	FIELD(PWM_CMR0, DTLI, 18, 1)
REG32(PWM_CDTY0, 0x204)
	FIELD(PWM_CDTY0, CDTY, 0, 24)
REG32(PWM_CDTYUPD0, 0x208)
	FIELD(PWM_CDTYUPD0, CDTYUPD, 0, 24)
REG32(PWM_CPRD0, 0x20c)
	FIELD(PWM_CPRD0, CPRD, 0, 24)
REG32(PWM_CPRDUPD0, 0x210)
	FIELD(PWM_CPRDUPD0, CPRDUPD, 0, 24)
REG32(PWM_CCNT0, 0x214)
	FIELD(PWM_CCNT0, CNT, 0, 24)
REG32(PWM_DT0, 0x218)
	FIELD(PWM_DT0, DTH, 0, 16)
	FIELD(PWM_DT0, DTL, 16, 16)
REG32(PWM_DTUPD0, 0x21c)
	FIELD(PWM_DTUPD0, DTHUPD, 0, 16)
	FIELD(PWM_DTUPD0, DTLUPD, 16, 16)
REG32(PWM_CMR1, 0x220)
	FIELD(PWM_CMR1, CPRE, 0, 4)
	FIELD(PWM_CMR1, CALG, 8, 1)
	FIELD(PWM_CMR1, CPOL, 9, 1)
	FIELD(PWM_CMR1, CES, 10, 1)
	FIELD(PWM_CMR1, DTE, 16, 1)
	FIELD(PWM_CMR1, DTHI, 17, 1)
	FIELD(PWM_CMR1, DTLI, 18, 1)
REG32(PWM_CDTY1, 0x224)
	FIELD(PWM_CDTY1, CDTY, 0, 24)
REG32(PWM_CDTYUPD1, 0x228)
	FIELD(PWM_CDTYUPD1, CDTYUPD, 0, 24)
REG32(PWM_CPRD1, 0x22c)
	FIELD(PWM_CPRD1, CPRD, 0, 24)
REG32(PWM_CPRDUPD1, 0x230)
	FIELD(PWM_CPRDUPD1, CPRDUPD, 0, 24)
REG32(PWM_CCNT1, 0x234)
	FIELD(PWM_CCNT1, CNT, 0, 24)
REG32(PWM_DT1, 0x238)
	FIELD(PWM_DT1, DTH, 0, 16)
	FIELD(PWM_DT1, DTL, 16, 16)
REG32(PWM_DTUPD1, 0x23c)
	FIELD(PWM_DTUPD1, DTHUPD, 0, 16)
	FIELD(PWM_DTUPD1, DTLUPD, 16, 16)
REG32(PWM_CMR2, 0x240)
	FIELD(PWM_CMR2, CPRE, 0, 4)
	FIELD(PWM_CMR2, CALG, 8, 1)
	FIELD(PWM_CMR2, CPOL, 9, 1)
	FIELD(PWM_CMR2, CES, 10, 1)
	FIELD(PWM_CMR2, DTE, 16, 1)
	FIELD(PWM_CMR2, DTHI, 17, 1)
	FIELD(PWM_CMR2, DTLI, 18, 1)
REG32(PWM_CDTY2, 0x244)
	FIELD(PWM_CDTY2, CDTY, 0, 24)
REG32(PWM_CDTYUPD2, 0x248)
	FIELD(PWM_CDTYUPD2, CDTYUPD, 0, 24)
REG32(PWM_CPRD2, 0x24c)
	FIELD(PWM_CPRD2, CPRD, 0, 24)
REG32(PWM_CPRDUPD2, 0x250)
	FIELD(PWM_CPRDUPD2, CPRDUPD, 0, 24)
REG32(PWM_CCNT2, 0x254)
	FIELD(PWM_CCNT2, CNT, 0, 24)
REG32(PWM_DT2, 0x258)
	FIELD(PWM_DT2, DTH, 0, 16)
	FIELD(PWM_DT2, DTL, 16, 16)
REG32(PWM_DTUPD2, 0x25c)
	FIELD(PWM_DTUPD2, DTHUPD, 0, 16)
	FIELD(PWM_DTUPD2, DTLUPD, 16, 16)
REG32(PWM_CMR3, 0x260)
	FIELD(PWM_CMR3, CPRE, 0, 4)
	FIELD(PWM_CMR3, CALG, 8, 1)
	FIELD(PWM_CMR3, CPOL, 9, 1)
	FIELD(PWM_CMR3, CES, 10, 1)
	FIELD(PWM_CMR3, DTE, 16, 1)
	FIELD(PWM_CMR3, DTHI, 17, 1)
	FIELD(PWM_CMR3, DTLI, 18, 1)
REG32(PWM_CDTY3, 0x264)
	FIELD(PWM_CDTY3, CDTY, 0, 24)
REG32(PWM_CDTYUPD3, 0x268)
	FIELD(PWM_CDTYUPD3, CDTYUPD, 0, 24)
REG32(PWM_CPRD3, 0x26c)
	FIELD(PWM_CPRD3, CPRD, 0, 24)
REG32(PWM_CPRDUPD3, 0x270)
	FIELD(PWM_CPRDUPD3, CPRDUPD, 0, 24)
REG32(PWM_CCNT3, 0x274)
	FIELD(PWM_CCNT3, CNT, 0, 24)
REG32(PWM_DT3, 0x278)
	FIELD(PWM_DT3, DTH, 0, 16)
	FIELD(PWM_DT3, DTL, 16, 16)
REG32(PWM_DTUPD3, 0x27c)
	FIELD(PWM_DTUPD3, DTHUPD, 0, 16)
	FIELD(PWM_DTUPD3, DTLUPD, 16, 16)
REG32(PWM_CMR4, 0x280)
	FIELD(PWM_CMR4, CPRE, 0, 4)
	FIELD(PWM_CMR4, CALG, 8, 1)
	FIELD(PWM_CMR4, CPOL, 9, 1)
	FIELD(PWM_CMR4, CES, 10, 1)
	FIELD(PWM_CMR4, DTE, 16, 1)
	FIELD(PWM_CMR4, DTHI, 17, 1)
	FIELD(PWM_CMR4, DTLI, 18, 1)
REG32(PWM_CDTY4, 0x284)
	FIELD(PWM_CDTY4, CDTY, 0, 24)
REG32(PWM_CDTYUPD4, 0x288)
	FIELD(PWM_CDTYUPD4, CDTYUPD, 0, 24)
REG32(PWM_CPRD4, 0x28c)
	FIELD(PWM_CPRD4, CPRD, 0, 24)
REG32(PWM_CPRDUPD4, 0x290)
	FIELD(PWM_CPRDUPD4, CPRDUPD, 0, 24)
REG32(PWM_CCNT4, 0x294)
	FIELD(PWM_CCNT4, CNT, 0, 24)
REG32(PWM_DT4, 0x298)
	FIELD(PWM_DT4, DTH, 0, 16)
	FIELD(PWM_DT4, DTL, 16, 16)
REG32(PWM_DTUPD4, 0x29c)
	FIELD(PWM_DTUPD4, DTHUPD, 0, 16)
	FIELD(PWM_DTUPD4, DTLUPD, 16, 16)
REG32(PWM_CMR5, 0x2a0)
	FIELD(PWM_CMR5, CPRE, 0, 4)
	FIELD(PWM_CMR5, CALG, 8, 1)
	FIELD(PWM_CMR5, CPOL, 9, 1)
	FIELD(PWM_CMR5, CES, 10, 1)
	FIELD(PWM_CMR5, DTE, 16, 1)
	FIELD(PWM_CMR5, DTHI, 17, 1)
	FIELD(PWM_CMR5, DTLI, 18, 1)
REG32(PWM_CDTY5, 0x2a4)
	FIELD(PWM_CDTY5, CDTY, 0, 24)
REG32(PWM_CDTYUPD5, 0x2a8)
	FIELD(PWM_CDTYUPD5, CDTYUPD, 0, 24)
REG32(PWM_CPRD5, 0x2ac)
	FIELD(PWM_CPRD5, CPRD, 0, 24)
REG32(PWM_CPRDUPD5, 0x2b0)
	FIELD(PWM_CPRDUPD5, CPRDUPD, 0, 24)
REG32(PWM_CCNT5, 0x2b4)
	FIELD(PWM_CCNT5, CNT, 0, 24)
REG32(PWM_DT5, 0x2b8)
	FIELD(PWM_DT5, DTH, 0, 16)
	FIELD(PWM_DT5, DTL, 16, 16)
REG32(PWM_DTUPD5, 0x2bc)
	FIELD(PWM_DTUPD5, DTHUPD, 0, 16)
	FIELD(PWM_DTUPD5, DTLUPD, 16, 16)
REG32(PWM_CMR6, 0x2c0)
	FIELD(PWM_CMR6, CPRE, 0, 4)
	FIELD(PWM_CMR6, CALG, 8, 1)
	FIELD(PWM_CMR6, CPOL, 9, 1)
	FIELD(PWM_CMR6, CES, 10, 1)
	FIELD(PWM_CMR6, DTE, 16, 1)
	FIELD(PWM_CMR6, DTHI, 17, 1)
	FIELD(PWM_CMR6, DTLI, 18, 1)
REG32(PWM_CDTY6, 0x2c4)
	FIELD(PWM_CDTY6, CDTY, 0, 24)
REG32(PWM_CDTYUPD6, 0x2c8)
	FIELD(PWM_CDTYUPD6, CDTYUPD, 0, 24)
REG32(PWM_CPRD6, 0x2cc)
	FIELD(PWM_CPRD6, CPRD, 0, 24)
REG32(PWM_CPRDUPD6, 0x2d0)
	FIELD(PWM_CPRDUPD6, CPRDUPD, 0, 24)
REG32(PWM_CCNT6, 0x2d4)
	FIELD(PWM_CCNT6, CNT, 0, 24)
REG32(PWM_DT6, 0x2d8)
	FIELD(PWM_DT6, DTH, 0, 16)
	FIELD(PWM_DT6, DTL, 16, 16)
REG32(PWM_DTUPD6, 0x2dc)
	FIELD(PWM_DTUPD6, DTHUPD, 0, 16)
	FIELD(PWM_DTUPD6, DTLUPD, 16, 16)
REG32(PWM_CMR7, 0x2e0)
	FIELD(PWM_CMR7, CPRE, 0, 4)
	FIELD(PWM_CMR7, CALG, 8, 1)
	FIELD(PWM_CMR7, CPOL, 9, 1)
	FIELD(PWM_CMR7, CES, 10, 1)
	FIELD(PWM_CMR7, DTE, 16, 1)
	FIELD(PWM_CMR7, DTHI, 17, 1)
	FIELD(PWM_CMR7, DTLI, 18, 1)
REG32(PWM_CDTY7, 0x2e4)
	FIELD(PWM_CDTY7, CDTY, 0, 24)
REG32(PWM_CDTYUPD7, 0x2e8)
	FIELD(PWM_CDTYUPD7, CDTYUPD, 0, 24)
REG32(PWM_CPRD7, 0x2ec)
	FIELD(PWM_CPRD7, CPRD, 0, 24)
REG32(PWM_CPRDUPD7, 0x2f0)
	FIELD(PWM_CPRDUPD7, CPRDUPD, 0, 24)
REG32(PWM_CCNT7, 0x2f4)
	FIELD(PWM_CCNT7, CNT, 0, 24)
REG32(PWM_DT7, 0x2f8)
	FIELD(PWM_DT7, DTH, 0, 16)
	FIELD(PWM_DT7, DTL, 16, 16)
REG32(PWM_DTUPD7, 0x2fc)
	FIELD(PWM_DTUPD7, DTHUPD, 0, 16)
	FIELD(PWM_DTUPD7, DTLUPD, 16, 16)
REG32(PWM_TPR, 0x108)
	FIELD(PWM_TPR, TXPTR, 0, 32)
REG32(PWM_TCR, 0x10c)
	FIELD(PWM_TCR, TXCTR, 0, 16)
REG32(PWM_TNPR, 0x118)
	FIELD(PWM_TNPR, TXNPTR, 0, 32)
REG32(PWM_TNCR, 0x11c)
	FIELD(PWM_TNCR, TXNCTR, 0, 16)
REG32(PWM_PTCR, 0x120)
	FIELD(PWM_PTCR, RXTEN, 0, 1)
	FIELD(PWM_PTCR, RXTDIS, 1, 1)
	FIELD(PWM_PTCR, TXTEN, 8, 1)
	FIELD(PWM_PTCR, TXTDIS, 9, 1)
REG32(PWM_PTSR, 0x124)
	FIELD(PWM_PTSR, RXTEN, 0, 1)
	FIELD(PWM_PTSR, TXTEN, 8, 1)
REG32(PWM_ELMR0, 0x7c)
	FIELD(PWM_ELMR0, CSEL0, 0, 1)
	FIELD(PWM_ELMR0, CSEL1, 1, 1)
	FIELD(PWM_ELMR0, CSEL2, 2, 1)
	FIELD(PWM_ELMR0, CSEL3, 3, 1)
	FIELD(PWM_ELMR0, CSEL4, 4, 1)
	FIELD(PWM_ELMR0, CSEL5, 5, 1)
	FIELD(PWM_ELMR0, CSEL6, 6, 1)
	FIELD(PWM_ELMR0, CSEL7, 7, 1)
REG32(PWM_ELMR1, 0x80)
	FIELD(PWM_ELMR1, CSEL0, 0, 1)
	FIELD(PWM_ELMR1, CSEL1, 1, 1)
	FIELD(PWM_ELMR1, CSEL2, 2, 1)
	FIELD(PWM_ELMR1, CSEL3, 3, 1)
	FIELD(PWM_ELMR1, CSEL4, 4, 1)
	FIELD(PWM_ELMR1, CSEL5, 5, 1)
	FIELD(PWM_ELMR1, CSEL6, 6, 1)
	FIELD(PWM_ELMR1, CSEL7, 7, 1)


static void atsam3x8e_pwm_register_reset(ATSAM3X8EPWM *t) {
	t->CLK = 0x0;
	t->SR = 0x0;
	t->IMR1 = 0x0;
	t->ISR1 = 0x0;
	t->SCM = 0x0;
	t->SCUC = 0x0;
	t->SCUP = 0x0;
	t->SCUPUPD = 0x0;
	t->IMR2 = 0x0;
	t->ISR2 = 0x0;
	t->OOV = 0x0;
	t->OS = 0x0;
	t->FMR = 0x0;
	t->FSR = 0x0;
	t->FPV = 0x0;
	t->FPE1 = 0x0;
	t->FPE2 = 0x0;
	t->SMMR = 0x0;
	t->WPSR = 0x0;
	t->CMPV0 = 0x0;
	t->CMPM0 = 0x0;
	t->CMPV1 = 0x0;
	t->CMPM1 = 0x0;
	t->CMPV2 = 0x0;
	t->CMPM2 = 0x0;
	t->CMPV3 = 0x0;
	t->CMPM3 = 0x0;
	t->CMPV4 = 0x0;
	t->CMPM4 = 0x0;
	t->CMPV5 = 0x0;
	t->CMPM5 = 0x0;
	t->CMPV6 = 0x0;
	t->CMPM6 = 0x0;
	t->CMPV7 = 0x0;
	t->CMPM7 = 0x0;
	t->CMR0 = 0x0;
	t->CDTY0 = 0x0;
	t->CPRD0 = 0x0;
	t->CCNT0 = 0x0;
	t->DT0 = 0x0;
	t->CMR1 = 0x0;
	t->CDTY1 = 0x0;
	t->CPRD1 = 0x0;
	t->CCNT1 = 0x0;
	t->DT1 = 0x0;
	t->CMR2 = 0x0;
	t->CDTY2 = 0x0;
	t->CPRD2 = 0x0;
	t->CCNT2 = 0x0;
	t->DT2 = 0x0;
	t->CMR3 = 0x0;
	t->CDTY3 = 0x0;
	t->CPRD3 = 0x0;
	t->CCNT3 = 0x0;
	t->DT3 = 0x0;
	t->CMR4 = 0x0;
	t->CDTY4 = 0x0;
	t->CPRD4 = 0x0;
	t->CCNT4 = 0x0;
	t->DT4 = 0x0;
	t->CMR5 = 0x0;
	t->CDTY5 = 0x0;
	t->CPRD5 = 0x0;
	t->CCNT5 = 0x0;
	t->DT5 = 0x0;
	t->CMR6 = 0x0;
	t->CDTY6 = 0x0;
	t->CPRD6 = 0x0;
	t->CCNT6 = 0x0;
	t->DT6 = 0x0;
	t->CMR7 = 0x0;
	t->CDTY7 = 0x0;
	t->CPRD7 = 0x0;
	t->CCNT7 = 0x0;
	t->DT7 = 0x0;
	t->TPR = 0x0;
	t->TCR = 0x0;
	t->TNPR = 0x0;
	t->TNCR = 0x0;
	t->PTCR = 0x0;
	t->PTSR = 0x0;

}

static uint64_t atsam3x8e_pwm_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAM3X8EPWM *t = ATSAM3X8E_PWM(opaque);
	uint64_t ret;

	switch (offset) {
		case A_PWM_CLK:
			ret = t->CLK;
			break;
		case A_PWM_SR:
			ret = t->SR;
			break;
		case A_PWM_IMR1:
			ret = t->IMR1;
			break;
		case A_PWM_ISR1:
			ret = t->ISR1;
			break;
		case A_PWM_SCM:
			ret = t->SCM;
			break;
		case A_PWM_SCUC:
			ret = t->SCUC;
			break;
		case A_PWM_SCUP:
			ret = t->SCUP;
			break;
		case A_PWM_IMR2:
			ret = t->IMR2;
			break;
		case A_PWM_ISR2:
			ret = t->ISR2;
			break;
		case A_PWM_OOV:
			ret = t->OOV;
			break;
		case A_PWM_OS:
			ret = t->OS;
			break;
		case A_PWM_FMR:
			ret = t->FMR;
			break;
		case A_PWM_FSR:
			ret = t->FSR;
			break;
		case A_PWM_FPV:
			ret = t->FPV;
			break;
		case A_PWM_FPE1:
			ret = t->FPE1;
			break;
		case A_PWM_FPE2:
			ret = t->FPE2;
			break;
		case A_PWM_SMMR:
			ret = t->SMMR;
			break;
		case A_PWM_WPSR:
			ret = t->WPSR;
			break;
		case A_PWM_CMPV0:
			ret = t->CMPV0;
			break;
		case A_PWM_CMPM0:
			ret = t->CMPM0;
			break;
		case A_PWM_CMPV1:
			ret = t->CMPV1;
			break;
		case A_PWM_CMPM1:
			ret = t->CMPM1;
			break;
		case A_PWM_CMPV2:
			ret = t->CMPV2;
			break;
		case A_PWM_CMPM2:
			ret = t->CMPM2;
			break;
		case A_PWM_CMPV3:
			ret = t->CMPV3;
			break;
		case A_PWM_CMPM3:
			ret = t->CMPM3;
			break;
		case A_PWM_CMPV4:
			ret = t->CMPV4;
			break;
		case A_PWM_CMPM4:
			ret = t->CMPM4;
			break;
		case A_PWM_CMPV5:
			ret = t->CMPV5;
			break;
		case A_PWM_CMPM5:
			ret = t->CMPM5;
			break;
		case A_PWM_CMPV6:
			ret = t->CMPV6;
			break;
		case A_PWM_CMPM6:
			ret = t->CMPM6;
			break;
		case A_PWM_CMPV7:
			ret = t->CMPV7;
			break;
		case A_PWM_CMPM7:
			ret = t->CMPM7;
			break;
		case A_PWM_CMR0:
			ret = t->CMR0;
			break;
		case A_PWM_CDTY0:
			ret = t->CDTY0;
			break;
		case A_PWM_CPRD0:
			ret = t->CPRD0;
			break;
		case A_PWM_CCNT0:
			ret = t->CCNT0;
			break;
		case A_PWM_DT0:
			ret = t->DT0;
			break;
		case A_PWM_CMR1:
			ret = t->CMR1;
			break;
		case A_PWM_CDTY1:
			ret = t->CDTY1;
			break;
		case A_PWM_CPRD1:
			ret = t->CPRD1;
			break;
		case A_PWM_CCNT1:
			ret = t->CCNT1;
			break;
		case A_PWM_DT1:
			ret = t->DT1;
			break;
		case A_PWM_CMR2:
			ret = t->CMR2;
			break;
		case A_PWM_CDTY2:
			ret = t->CDTY2;
			break;
		case A_PWM_CPRD2:
			ret = t->CPRD2;
			break;
		case A_PWM_CCNT2:
			ret = t->CCNT2;
			break;
		case A_PWM_DT2:
			ret = t->DT2;
			break;
		case A_PWM_CMR3:
			ret = t->CMR3;
			break;
		case A_PWM_CDTY3:
			ret = t->CDTY3;
			break;
		case A_PWM_CPRD3:
			ret = t->CPRD3;
			break;
		case A_PWM_CCNT3:
			ret = t->CCNT3;
			break;
		case A_PWM_DT3:
			ret = t->DT3;
			break;
		case A_PWM_CMR4:
			ret = t->CMR4;
			break;
		case A_PWM_CDTY4:
			ret = t->CDTY4;
			break;
		case A_PWM_CPRD4:
			ret = t->CPRD4;
			break;
		case A_PWM_CCNT4:
			ret = t->CCNT4;
			break;
		case A_PWM_DT4:
			ret = t->DT4;
			break;
		case A_PWM_CMR5:
			ret = t->CMR5;
			break;
		case A_PWM_CDTY5:
			ret = t->CDTY5;
			break;
		case A_PWM_CPRD5:
			ret = t->CPRD5;
			break;
		case A_PWM_CCNT5:
			ret = t->CCNT5;
			break;
		case A_PWM_DT5:
			ret = t->DT5;
			break;
		case A_PWM_CMR6:
			ret = t->CMR6;
			break;
		case A_PWM_CDTY6:
			ret = t->CDTY6;
			break;
		case A_PWM_CPRD6:
			ret = t->CPRD6;
			break;
		case A_PWM_CCNT6:
			ret = t->CCNT6;
			break;
		case A_PWM_DT6:
			ret = t->DT6;
			break;
		case A_PWM_CMR7:
			ret = t->CMR7;
			break;
		case A_PWM_CDTY7:
			ret = t->CDTY7;
			break;
		case A_PWM_CPRD7:
			ret = t->CPRD7;
			break;
		case A_PWM_CCNT7:
			ret = t->CCNT7;
			break;
		case A_PWM_DT7:
			ret = t->DT7;
			break;
		case A_PWM_TPR:
			ret = t->TPR;
			break;
		case A_PWM_TCR:
			ret = t->TCR;
			break;
		case A_PWM_TNPR:
			ret = t->TNPR;
			break;
		case A_PWM_TNCR:
			ret = t->TNCR;
			break;
		case A_PWM_PTSR:
			ret = t->PTSR;
			break;
		case A_PWM_ELMR0:
			ret = t->ELMR0;
			break;
		case A_PWM_ELMR1:
			ret = t->ELMR1;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM3X8E PWM read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsam3x8e_pwm_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAM3X8EPWM *t = ATSAM3X8E_PWM(opaque);


	switch (offset) {
		case A_PWM_CLK ... A_PWM_CLK + 3:
			value = value << ((offset - A_PWM_CLK) << 3);
			offset = A_PWM_CLK;
			break;
		case A_PWM_ENA ... A_PWM_ENA + 3:
			value = value << ((offset - A_PWM_ENA) << 3);
			offset = A_PWM_ENA;
			break;
		case A_PWM_DIS ... A_PWM_DIS + 3:
			value = value << ((offset - A_PWM_DIS) << 3);
			offset = A_PWM_DIS;
			break;
		case A_PWM_IER1 ... A_PWM_IER1 + 3:
			value = value << ((offset - A_PWM_IER1) << 3);
			offset = A_PWM_IER1;
			break;
		case A_PWM_IDR1 ... A_PWM_IDR1 + 3:
			value = value << ((offset - A_PWM_IDR1) << 3);
			offset = A_PWM_IDR1;
			break;
		case A_PWM_SCM ... A_PWM_SCM + 3:
			value = value << ((offset - A_PWM_SCM) << 3);
			offset = A_PWM_SCM;
			break;
		case A_PWM_SCUC ... A_PWM_SCUC + 3:
			value = value << ((offset - A_PWM_SCUC) << 3);
			offset = A_PWM_SCUC;
			break;
		case A_PWM_SCUP ... A_PWM_SCUP + 3:
			value = value << ((offset - A_PWM_SCUP) << 3);
			offset = A_PWM_SCUP;
			break;
		case A_PWM_SCUPUPD ... A_PWM_SCUPUPD + 3:
			value = value << ((offset - A_PWM_SCUPUPD) << 3);
			offset = A_PWM_SCUPUPD;
			break;
		case A_PWM_IER2 ... A_PWM_IER2 + 3:
			value = value << ((offset - A_PWM_IER2) << 3);
			offset = A_PWM_IER2;
			break;
		case A_PWM_IDR2 ... A_PWM_IDR2 + 3:
			value = value << ((offset - A_PWM_IDR2) << 3);
			offset = A_PWM_IDR2;
			break;
		case A_PWM_OOV ... A_PWM_OOV + 3:
			value = value << ((offset - A_PWM_OOV) << 3);
			offset = A_PWM_OOV;
			break;
		case A_PWM_OS ... A_PWM_OS + 3:
			value = value << ((offset - A_PWM_OS) << 3);
			offset = A_PWM_OS;
			break;
		case A_PWM_OSS ... A_PWM_OSS + 3:
			value = value << ((offset - A_PWM_OSS) << 3);
			offset = A_PWM_OSS;
			break;
		case A_PWM_OSC ... A_PWM_OSC + 3:
			value = value << ((offset - A_PWM_OSC) << 3);
			offset = A_PWM_OSC;
			break;
		case A_PWM_OSSUPD ... A_PWM_OSSUPD + 3:
			value = value << ((offset - A_PWM_OSSUPD) << 3);
			offset = A_PWM_OSSUPD;
			break;
		case A_PWM_OSCUPD ... A_PWM_OSCUPD + 3:
			value = value << ((offset - A_PWM_OSCUPD) << 3);
			offset = A_PWM_OSCUPD;
			break;
		case A_PWM_FMR ... A_PWM_FMR + 3:
			value = value << ((offset - A_PWM_FMR) << 3);
			offset = A_PWM_FMR;
			break;
		case A_PWM_FCR ... A_PWM_FCR + 3:
			value = value << ((offset - A_PWM_FCR) << 3);
			offset = A_PWM_FCR;
			break;
		case A_PWM_FPV ... A_PWM_FPV + 3:
			value = value << ((offset - A_PWM_FPV) << 3);
			offset = A_PWM_FPV;
			break;
		case A_PWM_FPE1 ... A_PWM_FPE1 + 3:
			value = value << ((offset - A_PWM_FPE1) << 3);
			offset = A_PWM_FPE1;
			break;
		case A_PWM_FPE2 ... A_PWM_FPE2 + 3:
			value = value << ((offset - A_PWM_FPE2) << 3);
			offset = A_PWM_FPE2;
			break;
		case A_PWM_SMMR ... A_PWM_SMMR + 3:
			value = value << ((offset - A_PWM_SMMR) << 3);
			offset = A_PWM_SMMR;
			break;
		case A_PWM_WPCR ... A_PWM_WPCR + 3:
			value = value << ((offset - A_PWM_WPCR) << 3);
			offset = A_PWM_WPCR;
			break;
		case A_PWM_CMPV0 ... A_PWM_CMPV0 + 3:
			value = value << ((offset - A_PWM_CMPV0) << 3);
			offset = A_PWM_CMPV0;
			break;
		case A_PWM_CMPVUPD0 ... A_PWM_CMPVUPD0 + 3:
			value = value << ((offset - A_PWM_CMPVUPD0) << 3);
			offset = A_PWM_CMPVUPD0;
			break;
		case A_PWM_CMPM0 ... A_PWM_CMPM0 + 3:
			value = value << ((offset - A_PWM_CMPM0) << 3);
			offset = A_PWM_CMPM0;
			break;
		case A_PWM_CMPMUPD0 ... A_PWM_CMPMUPD0 + 3:
			value = value << ((offset - A_PWM_CMPMUPD0) << 3);
			offset = A_PWM_CMPMUPD0;
			break;
		case A_PWM_CMPV1 ... A_PWM_CMPV1 + 3:
			value = value << ((offset - A_PWM_CMPV1) << 3);
			offset = A_PWM_CMPV1;
			break;
		case A_PWM_CMPVUPD1 ... A_PWM_CMPVUPD1 + 3:
			value = value << ((offset - A_PWM_CMPVUPD1) << 3);
			offset = A_PWM_CMPVUPD1;
			break;
		case A_PWM_CMPM1 ... A_PWM_CMPM1 + 3:
			value = value << ((offset - A_PWM_CMPM1) << 3);
			offset = A_PWM_CMPM1;
			break;
		case A_PWM_CMPMUPD1 ... A_PWM_CMPMUPD1 + 3:
			value = value << ((offset - A_PWM_CMPMUPD1) << 3);
			offset = A_PWM_CMPMUPD1;
			break;
		case A_PWM_CMPV2 ... A_PWM_CMPV2 + 3:
			value = value << ((offset - A_PWM_CMPV2) << 3);
			offset = A_PWM_CMPV2;
			break;
		case A_PWM_CMPVUPD2 ... A_PWM_CMPVUPD2 + 3:
			value = value << ((offset - A_PWM_CMPVUPD2) << 3);
			offset = A_PWM_CMPVUPD2;
			break;
		case A_PWM_CMPM2 ... A_PWM_CMPM2 + 3:
			value = value << ((offset - A_PWM_CMPM2) << 3);
			offset = A_PWM_CMPM2;
			break;
		case A_PWM_CMPMUPD2 ... A_PWM_CMPMUPD2 + 3:
			value = value << ((offset - A_PWM_CMPMUPD2) << 3);
			offset = A_PWM_CMPMUPD2;
			break;
		case A_PWM_CMPV3 ... A_PWM_CMPV3 + 3:
			value = value << ((offset - A_PWM_CMPV3) << 3);
			offset = A_PWM_CMPV3;
			break;
		case A_PWM_CMPVUPD3 ... A_PWM_CMPVUPD3 + 3:
			value = value << ((offset - A_PWM_CMPVUPD3) << 3);
			offset = A_PWM_CMPVUPD3;
			break;
		case A_PWM_CMPM3 ... A_PWM_CMPM3 + 3:
			value = value << ((offset - A_PWM_CMPM3) << 3);
			offset = A_PWM_CMPM3;
			break;
		case A_PWM_CMPMUPD3 ... A_PWM_CMPMUPD3 + 3:
			value = value << ((offset - A_PWM_CMPMUPD3) << 3);
			offset = A_PWM_CMPMUPD3;
			break;
		case A_PWM_CMPV4 ... A_PWM_CMPV4 + 3:
			value = value << ((offset - A_PWM_CMPV4) << 3);
			offset = A_PWM_CMPV4;
			break;
		case A_PWM_CMPVUPD4 ... A_PWM_CMPVUPD4 + 3:
			value = value << ((offset - A_PWM_CMPVUPD4) << 3);
			offset = A_PWM_CMPVUPD4;
			break;
		case A_PWM_CMPM4 ... A_PWM_CMPM4 + 3:
			value = value << ((offset - A_PWM_CMPM4) << 3);
			offset = A_PWM_CMPM4;
			break;
		case A_PWM_CMPMUPD4 ... A_PWM_CMPMUPD4 + 3:
			value = value << ((offset - A_PWM_CMPMUPD4) << 3);
			offset = A_PWM_CMPMUPD4;
			break;
		case A_PWM_CMPV5 ... A_PWM_CMPV5 + 3:
			value = value << ((offset - A_PWM_CMPV5) << 3);
			offset = A_PWM_CMPV5;
			break;
		case A_PWM_CMPVUPD5 ... A_PWM_CMPVUPD5 + 3:
			value = value << ((offset - A_PWM_CMPVUPD5) << 3);
			offset = A_PWM_CMPVUPD5;
			break;
		case A_PWM_CMPM5 ... A_PWM_CMPM5 + 3:
			value = value << ((offset - A_PWM_CMPM5) << 3);
			offset = A_PWM_CMPM5;
			break;
		case A_PWM_CMPMUPD5 ... A_PWM_CMPMUPD5 + 3:
			value = value << ((offset - A_PWM_CMPMUPD5) << 3);
			offset = A_PWM_CMPMUPD5;
			break;
		case A_PWM_CMPV6 ... A_PWM_CMPV6 + 3:
			value = value << ((offset - A_PWM_CMPV6) << 3);
			offset = A_PWM_CMPV6;
			break;
		case A_PWM_CMPVUPD6 ... A_PWM_CMPVUPD6 + 3:
			value = value << ((offset - A_PWM_CMPVUPD6) << 3);
			offset = A_PWM_CMPVUPD6;
			break;
		case A_PWM_CMPM6 ... A_PWM_CMPM6 + 3:
			value = value << ((offset - A_PWM_CMPM6) << 3);
			offset = A_PWM_CMPM6;
			break;
		case A_PWM_CMPMUPD6 ... A_PWM_CMPMUPD6 + 3:
			value = value << ((offset - A_PWM_CMPMUPD6) << 3);
			offset = A_PWM_CMPMUPD6;
			break;
		case A_PWM_CMPV7 ... A_PWM_CMPV7 + 3:
			value = value << ((offset - A_PWM_CMPV7) << 3);
			offset = A_PWM_CMPV7;
			break;
		case A_PWM_CMPVUPD7 ... A_PWM_CMPVUPD7 + 3:
			value = value << ((offset - A_PWM_CMPVUPD7) << 3);
			offset = A_PWM_CMPVUPD7;
			break;
		case A_PWM_CMPM7 ... A_PWM_CMPM7 + 3:
			value = value << ((offset - A_PWM_CMPM7) << 3);
			offset = A_PWM_CMPM7;
			break;
		case A_PWM_CMPMUPD7 ... A_PWM_CMPMUPD7 + 3:
			value = value << ((offset - A_PWM_CMPMUPD7) << 3);
			offset = A_PWM_CMPMUPD7;
			break;
		case A_PWM_CMR0 ... A_PWM_CMR0 + 3:
			value = value << ((offset - A_PWM_CMR0) << 3);
			offset = A_PWM_CMR0;
			break;
		case A_PWM_CDTY0 ... A_PWM_CDTY0 + 3:
			value = value << ((offset - A_PWM_CDTY0) << 3);
			offset = A_PWM_CDTY0;
			break;
		case A_PWM_CDTYUPD0 ... A_PWM_CDTYUPD0 + 3:
			value = value << ((offset - A_PWM_CDTYUPD0) << 3);
			offset = A_PWM_CDTYUPD0;
			break;
		case A_PWM_CPRD0 ... A_PWM_CPRD0 + 3:
			value = value << ((offset - A_PWM_CPRD0) << 3);
			offset = A_PWM_CPRD0;
			break;
		case A_PWM_CPRDUPD0 ... A_PWM_CPRDUPD0 + 3:
			value = value << ((offset - A_PWM_CPRDUPD0) << 3);
			offset = A_PWM_CPRDUPD0;
			break;
		case A_PWM_DT0 ... A_PWM_DT0 + 3:
			value = value << ((offset - A_PWM_DT0) << 3);
			offset = A_PWM_DT0;
			break;
		case A_PWM_DTUPD0 ... A_PWM_DTUPD0 + 3:
			value = value << ((offset - A_PWM_DTUPD0) << 3);
			offset = A_PWM_DTUPD0;
			break;
		case A_PWM_CMR1 ... A_PWM_CMR1 + 3:
			value = value << ((offset - A_PWM_CMR1) << 3);
			offset = A_PWM_CMR1;
			break;
		case A_PWM_CDTY1 ... A_PWM_CDTY1 + 3:
			value = value << ((offset - A_PWM_CDTY1) << 3);
			offset = A_PWM_CDTY1;
			break;
		case A_PWM_CDTYUPD1 ... A_PWM_CDTYUPD1 + 3:
			value = value << ((offset - A_PWM_CDTYUPD1) << 3);
			offset = A_PWM_CDTYUPD1;
			break;
		case A_PWM_CPRD1 ... A_PWM_CPRD1 + 3:
			value = value << ((offset - A_PWM_CPRD1) << 3);
			offset = A_PWM_CPRD1;
			break;
		case A_PWM_CPRDUPD1 ... A_PWM_CPRDUPD1 + 3:
			value = value << ((offset - A_PWM_CPRDUPD1) << 3);
			offset = A_PWM_CPRDUPD1;
			break;
		case A_PWM_DT1 ... A_PWM_DT1 + 3:
			value = value << ((offset - A_PWM_DT1) << 3);
			offset = A_PWM_DT1;
			break;
		case A_PWM_DTUPD1 ... A_PWM_DTUPD1 + 3:
			value = value << ((offset - A_PWM_DTUPD1) << 3);
			offset = A_PWM_DTUPD1;
			break;
		case A_PWM_CMR2 ... A_PWM_CMR2 + 3:
			value = value << ((offset - A_PWM_CMR2) << 3);
			offset = A_PWM_CMR2;
			break;
		case A_PWM_CDTY2 ... A_PWM_CDTY2 + 3:
			value = value << ((offset - A_PWM_CDTY2) << 3);
			offset = A_PWM_CDTY2;
			break;
		case A_PWM_CDTYUPD2 ... A_PWM_CDTYUPD2 + 3:
			value = value << ((offset - A_PWM_CDTYUPD2) << 3);
			offset = A_PWM_CDTYUPD2;
			break;
		case A_PWM_CPRD2 ... A_PWM_CPRD2 + 3:
			value = value << ((offset - A_PWM_CPRD2) << 3);
			offset = A_PWM_CPRD2;
			break;
		case A_PWM_CPRDUPD2 ... A_PWM_CPRDUPD2 + 3:
			value = value << ((offset - A_PWM_CPRDUPD2) << 3);
			offset = A_PWM_CPRDUPD2;
			break;
		case A_PWM_DT2 ... A_PWM_DT2 + 3:
			value = value << ((offset - A_PWM_DT2) << 3);
			offset = A_PWM_DT2;
			break;
		case A_PWM_DTUPD2 ... A_PWM_DTUPD2 + 3:
			value = value << ((offset - A_PWM_DTUPD2) << 3);
			offset = A_PWM_DTUPD2;
			break;
		case A_PWM_CMR3 ... A_PWM_CMR3 + 3:
			value = value << ((offset - A_PWM_CMR3) << 3);
			offset = A_PWM_CMR3;
			break;
		case A_PWM_CDTY3 ... A_PWM_CDTY3 + 3:
			value = value << ((offset - A_PWM_CDTY3) << 3);
			offset = A_PWM_CDTY3;
			break;
		case A_PWM_CDTYUPD3 ... A_PWM_CDTYUPD3 + 3:
			value = value << ((offset - A_PWM_CDTYUPD3) << 3);
			offset = A_PWM_CDTYUPD3;
			break;
		case A_PWM_CPRD3 ... A_PWM_CPRD3 + 3:
			value = value << ((offset - A_PWM_CPRD3) << 3);
			offset = A_PWM_CPRD3;
			break;
		case A_PWM_CPRDUPD3 ... A_PWM_CPRDUPD3 + 3:
			value = value << ((offset - A_PWM_CPRDUPD3) << 3);
			offset = A_PWM_CPRDUPD3;
			break;
		case A_PWM_DT3 ... A_PWM_DT3 + 3:
			value = value << ((offset - A_PWM_DT3) << 3);
			offset = A_PWM_DT3;
			break;
		case A_PWM_DTUPD3 ... A_PWM_DTUPD3 + 3:
			value = value << ((offset - A_PWM_DTUPD3) << 3);
			offset = A_PWM_DTUPD3;
			break;
		case A_PWM_CMR4 ... A_PWM_CMR4 + 3:
			value = value << ((offset - A_PWM_CMR4) << 3);
			offset = A_PWM_CMR4;
			break;
		case A_PWM_CDTY4 ... A_PWM_CDTY4 + 3:
			value = value << ((offset - A_PWM_CDTY4) << 3);
			offset = A_PWM_CDTY4;
			break;
		case A_PWM_CDTYUPD4 ... A_PWM_CDTYUPD4 + 3:
			value = value << ((offset - A_PWM_CDTYUPD4) << 3);
			offset = A_PWM_CDTYUPD4;
			break;
		case A_PWM_CPRD4 ... A_PWM_CPRD4 + 3:
			value = value << ((offset - A_PWM_CPRD4) << 3);
			offset = A_PWM_CPRD4;
			break;
		case A_PWM_CPRDUPD4 ... A_PWM_CPRDUPD4 + 3:
			value = value << ((offset - A_PWM_CPRDUPD4) << 3);
			offset = A_PWM_CPRDUPD4;
			break;
		case A_PWM_DT4 ... A_PWM_DT4 + 3:
			value = value << ((offset - A_PWM_DT4) << 3);
			offset = A_PWM_DT4;
			break;
		case A_PWM_DTUPD4 ... A_PWM_DTUPD4 + 3:
			value = value << ((offset - A_PWM_DTUPD4) << 3);
			offset = A_PWM_DTUPD4;
			break;
		case A_PWM_CMR5 ... A_PWM_CMR5 + 3:
			value = value << ((offset - A_PWM_CMR5) << 3);
			offset = A_PWM_CMR5;
			break;
		case A_PWM_CDTY5 ... A_PWM_CDTY5 + 3:
			value = value << ((offset - A_PWM_CDTY5) << 3);
			offset = A_PWM_CDTY5;
			break;
		case A_PWM_CDTYUPD5 ... A_PWM_CDTYUPD5 + 3:
			value = value << ((offset - A_PWM_CDTYUPD5) << 3);
			offset = A_PWM_CDTYUPD5;
			break;
		case A_PWM_CPRD5 ... A_PWM_CPRD5 + 3:
			value = value << ((offset - A_PWM_CPRD5) << 3);
			offset = A_PWM_CPRD5;
			break;
		case A_PWM_CPRDUPD5 ... A_PWM_CPRDUPD5 + 3:
			value = value << ((offset - A_PWM_CPRDUPD5) << 3);
			offset = A_PWM_CPRDUPD5;
			break;
		case A_PWM_DT5 ... A_PWM_DT5 + 3:
			value = value << ((offset - A_PWM_DT5) << 3);
			offset = A_PWM_DT5;
			break;
		case A_PWM_DTUPD5 ... A_PWM_DTUPD5 + 3:
			value = value << ((offset - A_PWM_DTUPD5) << 3);
			offset = A_PWM_DTUPD5;
			break;
		case A_PWM_CMR6 ... A_PWM_CMR6 + 3:
			value = value << ((offset - A_PWM_CMR6) << 3);
			offset = A_PWM_CMR6;
			break;
		case A_PWM_CDTY6 ... A_PWM_CDTY6 + 3:
			value = value << ((offset - A_PWM_CDTY6) << 3);
			offset = A_PWM_CDTY6;
			break;
		case A_PWM_CDTYUPD6 ... A_PWM_CDTYUPD6 + 3:
			value = value << ((offset - A_PWM_CDTYUPD6) << 3);
			offset = A_PWM_CDTYUPD6;
			break;
		case A_PWM_CPRD6 ... A_PWM_CPRD6 + 3:
			value = value << ((offset - A_PWM_CPRD6) << 3);
			offset = A_PWM_CPRD6;
			break;
		case A_PWM_CPRDUPD6 ... A_PWM_CPRDUPD6 + 3:
			value = value << ((offset - A_PWM_CPRDUPD6) << 3);
			offset = A_PWM_CPRDUPD6;
			break;
		case A_PWM_DT6 ... A_PWM_DT6 + 3:
			value = value << ((offset - A_PWM_DT6) << 3);
			offset = A_PWM_DT6;
			break;
		case A_PWM_DTUPD6 ... A_PWM_DTUPD6 + 3:
			value = value << ((offset - A_PWM_DTUPD6) << 3);
			offset = A_PWM_DTUPD6;
			break;
		case A_PWM_CMR7 ... A_PWM_CMR7 + 3:
			value = value << ((offset - A_PWM_CMR7) << 3);
			offset = A_PWM_CMR7;
			break;
		case A_PWM_CDTY7 ... A_PWM_CDTY7 + 3:
			value = value << ((offset - A_PWM_CDTY7) << 3);
			offset = A_PWM_CDTY7;
			break;
		case A_PWM_CDTYUPD7 ... A_PWM_CDTYUPD7 + 3:
			value = value << ((offset - A_PWM_CDTYUPD7) << 3);
			offset = A_PWM_CDTYUPD7;
			break;
		case A_PWM_CPRD7 ... A_PWM_CPRD7 + 3:
			value = value << ((offset - A_PWM_CPRD7) << 3);
			offset = A_PWM_CPRD7;
			break;
		case A_PWM_CPRDUPD7 ... A_PWM_CPRDUPD7 + 3:
			value = value << ((offset - A_PWM_CPRDUPD7) << 3);
			offset = A_PWM_CPRDUPD7;
			break;
		case A_PWM_DT7 ... A_PWM_DT7 + 3:
			value = value << ((offset - A_PWM_DT7) << 3);
			offset = A_PWM_DT7;
			break;
		case A_PWM_DTUPD7 ... A_PWM_DTUPD7 + 3:
			value = value << ((offset - A_PWM_DTUPD7) << 3);
			offset = A_PWM_DTUPD7;
			break;
		case A_PWM_TPR ... A_PWM_TPR + 3:
			value = value << ((offset - A_PWM_TPR) << 3);
			offset = A_PWM_TPR;
			break;
		case A_PWM_TCR ... A_PWM_TCR + 3:
			value = value << ((offset - A_PWM_TCR) << 3);
			offset = A_PWM_TCR;
			break;
		case A_PWM_TNPR ... A_PWM_TNPR + 3:
			value = value << ((offset - A_PWM_TNPR) << 3);
			offset = A_PWM_TNPR;
			break;
		case A_PWM_TNCR ... A_PWM_TNCR + 3:
			value = value << ((offset - A_PWM_TNCR) << 3);
			offset = A_PWM_TNCR;
			break;
		case A_PWM_PTCR ... A_PWM_PTCR + 3:
			value = value << ((offset - A_PWM_PTCR) << 3);
			offset = A_PWM_PTCR;
			break;
		case A_PWM_ELMR0 ... A_PWM_ELMR0 + 3:
			value = value << ((offset - A_PWM_ELMR0) << 3);
			offset = A_PWM_ELMR0;
			break;
		case A_PWM_ELMR1 ... A_PWM_ELMR1 + 3:
			value = value << ((offset - A_PWM_ELMR1) << 3);
			offset = A_PWM_ELMR1;
			break;

		default: break;
	}

	switch (offset) {
		case A_PWM_CLK:
			t->CLK = value;
			break;
		case A_PWM_ENA:
			t->ENA = value;
			break;
		case A_PWM_DIS:
			t->DIS = value;
			break;
		case A_PWM_IER1:
			t->IER1 = value;
			break;
		case A_PWM_IDR1:
			t->IDR1 = value;
			break;
		case A_PWM_SCM:
			t->SCM = value;
			break;
		case A_PWM_SCUC:
			t->SCUC = value;
			break;
		case A_PWM_SCUP:
			t->SCUP = value;
			break;
		case A_PWM_SCUPUPD:
			t->SCUPUPD = value;
			break;
		case A_PWM_IER2:
			t->IER2 = value;
			break;
		case A_PWM_IDR2:
			t->IDR2 = value;
			break;
		case A_PWM_OOV:
			t->OOV = value;
			break;
		case A_PWM_OS:
			t->OS = value;
			break;
		case A_PWM_OSS:
			t->OSS = value;
			break;
		case A_PWM_OSC:
			t->OSC = value;
			break;
		case A_PWM_OSSUPD:
			t->OSSUPD = value;
			break;
		case A_PWM_OSCUPD:
			t->OSCUPD = value;
			break;
		case A_PWM_FMR:
			t->FMR = value;
			break;
		case A_PWM_FCR:
			t->FCR = value;
			break;
		case A_PWM_FPV:
			t->FPV = value;
			break;
		case A_PWM_FPE1:
			t->FPE1 = value;
			break;
		case A_PWM_FPE2:
			t->FPE2 = value;
			break;
		case A_PWM_SMMR:
			t->SMMR = value;
			break;
		case A_PWM_WPCR:
			t->WPCR = value;
			break;
		case A_PWM_CMPV0:
			t->CMPV0 = value;
			break;
		case A_PWM_CMPVUPD0:
			t->CMPVUPD0 = value;
			break;
		case A_PWM_CMPM0:
			t->CMPM0 = value;
			break;
		case A_PWM_CMPMUPD0:
			t->CMPMUPD0 = value;
			break;
		case A_PWM_CMPV1:
			t->CMPV1 = value;
			break;
		case A_PWM_CMPVUPD1:
			t->CMPVUPD1 = value;
			break;
		case A_PWM_CMPM1:
			t->CMPM1 = value;
			break;
		case A_PWM_CMPMUPD1:
			t->CMPMUPD1 = value;
			break;
		case A_PWM_CMPV2:
			t->CMPV2 = value;
			break;
		case A_PWM_CMPVUPD2:
			t->CMPVUPD2 = value;
			break;
		case A_PWM_CMPM2:
			t->CMPM2 = value;
			break;
		case A_PWM_CMPMUPD2:
			t->CMPMUPD2 = value;
			break;
		case A_PWM_CMPV3:
			t->CMPV3 = value;
			break;
		case A_PWM_CMPVUPD3:
			t->CMPVUPD3 = value;
			break;
		case A_PWM_CMPM3:
			t->CMPM3 = value;
			break;
		case A_PWM_CMPMUPD3:
			t->CMPMUPD3 = value;
			break;
		case A_PWM_CMPV4:
			t->CMPV4 = value;
			break;
		case A_PWM_CMPVUPD4:
			t->CMPVUPD4 = value;
			break;
		case A_PWM_CMPM4:
			t->CMPM4 = value;
			break;
		case A_PWM_CMPMUPD4:
			t->CMPMUPD4 = value;
			break;
		case A_PWM_CMPV5:
			t->CMPV5 = value;
			break;
		case A_PWM_CMPVUPD5:
			t->CMPVUPD5 = value;
			break;
		case A_PWM_CMPM5:
			t->CMPM5 = value;
			break;
		case A_PWM_CMPMUPD5:
			t->CMPMUPD5 = value;
			break;
		case A_PWM_CMPV6:
			t->CMPV6 = value;
			break;
		case A_PWM_CMPVUPD6:
			t->CMPVUPD6 = value;
			break;
		case A_PWM_CMPM6:
			t->CMPM6 = value;
			break;
		case A_PWM_CMPMUPD6:
			t->CMPMUPD6 = value;
			break;
		case A_PWM_CMPV7:
			t->CMPV7 = value;
			break;
		case A_PWM_CMPVUPD7:
			t->CMPVUPD7 = value;
			break;
		case A_PWM_CMPM7:
			t->CMPM7 = value;
			break;
		case A_PWM_CMPMUPD7:
			t->CMPMUPD7 = value;
			break;
		case A_PWM_CMR0:
			t->CMR0 = value;
			break;
		case A_PWM_CDTY0:
			t->CDTY0 = value;
			break;
		case A_PWM_CDTYUPD0:
			t->CDTYUPD0 = value;
			break;
		case A_PWM_CPRD0:
			t->CPRD0 = value;
			break;
		case A_PWM_CPRDUPD0:
			t->CPRDUPD0 = value;
			break;
		case A_PWM_DT0:
			t->DT0 = value;
			break;
		case A_PWM_DTUPD0:
			t->DTUPD0 = value;
			break;
		case A_PWM_CMR1:
			t->CMR1 = value;
			break;
		case A_PWM_CDTY1:
			t->CDTY1 = value;
			break;
		case A_PWM_CDTYUPD1:
			t->CDTYUPD1 = value;
			break;
		case A_PWM_CPRD1:
			t->CPRD1 = value;
			break;
		case A_PWM_CPRDUPD1:
			t->CPRDUPD1 = value;
			break;
		case A_PWM_DT1:
			t->DT1 = value;
			break;
		case A_PWM_DTUPD1:
			t->DTUPD1 = value;
			break;
		case A_PWM_CMR2:
			t->CMR2 = value;
			break;
		case A_PWM_CDTY2:
			t->CDTY2 = value;
			break;
		case A_PWM_CDTYUPD2:
			t->CDTYUPD2 = value;
			break;
		case A_PWM_CPRD2:
			t->CPRD2 = value;
			break;
		case A_PWM_CPRDUPD2:
			t->CPRDUPD2 = value;
			break;
		case A_PWM_DT2:
			t->DT2 = value;
			break;
		case A_PWM_DTUPD2:
			t->DTUPD2 = value;
			break;
		case A_PWM_CMR3:
			t->CMR3 = value;
			break;
		case A_PWM_CDTY3:
			t->CDTY3 = value;
			break;
		case A_PWM_CDTYUPD3:
			t->CDTYUPD3 = value;
			break;
		case A_PWM_CPRD3:
			t->CPRD3 = value;
			break;
		case A_PWM_CPRDUPD3:
			t->CPRDUPD3 = value;
			break;
		case A_PWM_DT3:
			t->DT3 = value;
			break;
		case A_PWM_DTUPD3:
			t->DTUPD3 = value;
			break;
		case A_PWM_CMR4:
			t->CMR4 = value;
			break;
		case A_PWM_CDTY4:
			t->CDTY4 = value;
			break;
		case A_PWM_CDTYUPD4:
			t->CDTYUPD4 = value;
			break;
		case A_PWM_CPRD4:
			t->CPRD4 = value;
			break;
		case A_PWM_CPRDUPD4:
			t->CPRDUPD4 = value;
			break;
		case A_PWM_DT4:
			t->DT4 = value;
			break;
		case A_PWM_DTUPD4:
			t->DTUPD4 = value;
			break;
		case A_PWM_CMR5:
			t->CMR5 = value;
			break;
		case A_PWM_CDTY5:
			t->CDTY5 = value;
			break;
		case A_PWM_CDTYUPD5:
			t->CDTYUPD5 = value;
			break;
		case A_PWM_CPRD5:
			t->CPRD5 = value;
			break;
		case A_PWM_CPRDUPD5:
			t->CPRDUPD5 = value;
			break;
		case A_PWM_DT5:
			t->DT5 = value;
			break;
		case A_PWM_DTUPD5:
			t->DTUPD5 = value;
			break;
		case A_PWM_CMR6:
			t->CMR6 = value;
			break;
		case A_PWM_CDTY6:
			t->CDTY6 = value;
			break;
		case A_PWM_CDTYUPD6:
			t->CDTYUPD6 = value;
			break;
		case A_PWM_CPRD6:
			t->CPRD6 = value;
			break;
		case A_PWM_CPRDUPD6:
			t->CPRDUPD6 = value;
			break;
		case A_PWM_DT6:
			t->DT6 = value;
			break;
		case A_PWM_DTUPD6:
			t->DTUPD6 = value;
			break;
		case A_PWM_CMR7:
			t->CMR7 = value;
			break;
		case A_PWM_CDTY7:
			t->CDTY7 = value;
			break;
		case A_PWM_CDTYUPD7:
			t->CDTYUPD7 = value;
			break;
		case A_PWM_CPRD7:
			t->CPRD7 = value;
			break;
		case A_PWM_CPRDUPD7:
			t->CPRDUPD7 = value;
			break;
		case A_PWM_DT7:
			t->DT7 = value;
			break;
		case A_PWM_DTUPD7:
			t->DTUPD7 = value;
			break;
		case A_PWM_TPR:
			t->TPR = value;
			break;
		case A_PWM_TCR:
			t->TCR = value;
			break;
		case A_PWM_TNPR:
			t->TNPR = value;
			break;
		case A_PWM_TNCR:
			t->TNCR = value;
			break;
		case A_PWM_PTCR:
			t->PTCR = value;
			break;
		case A_PWM_ELMR0:
			t->ELMR0 = value;
			break;
		case A_PWM_ELMR1:
			t->ELMR1 = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM3X8E PWM write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsam3x8e_pwm_ops = {
	.read = atsam3x8e_pwm_read,
	.write = atsam3x8e_pwm_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsam3x8e_pwm_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAM3X8EPWM *t = ATSAM3X8E_PWM(obj);
	memory_region_init_io(&(t->iomem), obj, &atsam3x8e_pwm_ops, t, "atsam3x8e-pwm", ATSAM3X8E_PWM_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void atsam3x8e_pwm_realize(DeviceState *dev, Error **errp) {
	return;

}

static void atsam3x8e_pwm_reset_enter(Object *obj, ResetType type) {
	ATSAM3X8EPWM *t = ATSAM3X8E_PWM(obj);
	atsam3x8e_pwm_register_reset(t);
}

static const VMStateDescription atsam3x8e_pwm_vmstate = {
	.name = "atsam3x8e-pwm",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CLK, ATSAM3X8EPWM),
		VMSTATE_UINT32(ENA, ATSAM3X8EPWM),
		VMSTATE_UINT32(DIS, ATSAM3X8EPWM),
		VMSTATE_UINT32(SR, ATSAM3X8EPWM),
		VMSTATE_UINT32(IER1, ATSAM3X8EPWM),
		VMSTATE_UINT32(IDR1, ATSAM3X8EPWM),
		VMSTATE_UINT32(IMR1, ATSAM3X8EPWM),
		VMSTATE_UINT32(ISR1, ATSAM3X8EPWM),
		VMSTATE_UINT32(SCM, ATSAM3X8EPWM),
		VMSTATE_UINT32(SCUC, ATSAM3X8EPWM),
		VMSTATE_UINT32(SCUP, ATSAM3X8EPWM),
		VMSTATE_UINT32(SCUPUPD, ATSAM3X8EPWM),
		VMSTATE_UINT32(IER2, ATSAM3X8EPWM),
		VMSTATE_UINT32(IDR2, ATSAM3X8EPWM),
		VMSTATE_UINT32(IMR2, ATSAM3X8EPWM),
		VMSTATE_UINT32(ISR2, ATSAM3X8EPWM),
		VMSTATE_UINT32(OOV, ATSAM3X8EPWM),
		VMSTATE_UINT32(OS, ATSAM3X8EPWM),
		VMSTATE_UINT32(OSS, ATSAM3X8EPWM),
		VMSTATE_UINT32(OSC, ATSAM3X8EPWM),
		VMSTATE_UINT32(OSSUPD, ATSAM3X8EPWM),
		VMSTATE_UINT32(OSCUPD, ATSAM3X8EPWM),
		VMSTATE_UINT32(FMR, ATSAM3X8EPWM),
		VMSTATE_UINT32(FSR, ATSAM3X8EPWM),
		VMSTATE_UINT32(FCR, ATSAM3X8EPWM),
		VMSTATE_UINT32(FPV, ATSAM3X8EPWM),
		VMSTATE_UINT32(FPE1, ATSAM3X8EPWM),
		VMSTATE_UINT32(FPE2, ATSAM3X8EPWM),
		VMSTATE_UINT32(SMMR, ATSAM3X8EPWM),
		VMSTATE_UINT32(WPCR, ATSAM3X8EPWM),
		VMSTATE_UINT32(WPSR, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMPV0, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMPVUPD0, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMPM0, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMPMUPD0, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMPV1, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMPVUPD1, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMPM1, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMPMUPD1, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMPV2, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMPVUPD2, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMPM2, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMPMUPD2, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMPV3, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMPVUPD3, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMPM3, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMPMUPD3, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMPV4, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMPVUPD4, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMPM4, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMPMUPD4, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMPV5, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMPVUPD5, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMPM5, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMPMUPD5, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMPV6, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMPVUPD6, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMPM6, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMPMUPD6, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMPV7, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMPVUPD7, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMPM7, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMPMUPD7, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMR0, ATSAM3X8EPWM),
		VMSTATE_UINT32(CDTY0, ATSAM3X8EPWM),
		VMSTATE_UINT32(CDTYUPD0, ATSAM3X8EPWM),
		VMSTATE_UINT32(CPRD0, ATSAM3X8EPWM),
		VMSTATE_UINT32(CPRDUPD0, ATSAM3X8EPWM),
		VMSTATE_UINT32(CCNT0, ATSAM3X8EPWM),
		VMSTATE_UINT32(DT0, ATSAM3X8EPWM),
		VMSTATE_UINT32(DTUPD0, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMR1, ATSAM3X8EPWM),
		VMSTATE_UINT32(CDTY1, ATSAM3X8EPWM),
		VMSTATE_UINT32(CDTYUPD1, ATSAM3X8EPWM),
		VMSTATE_UINT32(CPRD1, ATSAM3X8EPWM),
		VMSTATE_UINT32(CPRDUPD1, ATSAM3X8EPWM),
		VMSTATE_UINT32(CCNT1, ATSAM3X8EPWM),
		VMSTATE_UINT32(DT1, ATSAM3X8EPWM),
		VMSTATE_UINT32(DTUPD1, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMR2, ATSAM3X8EPWM),
		VMSTATE_UINT32(CDTY2, ATSAM3X8EPWM),
		VMSTATE_UINT32(CDTYUPD2, ATSAM3X8EPWM),
		VMSTATE_UINT32(CPRD2, ATSAM3X8EPWM),
		VMSTATE_UINT32(CPRDUPD2, ATSAM3X8EPWM),
		VMSTATE_UINT32(CCNT2, ATSAM3X8EPWM),
		VMSTATE_UINT32(DT2, ATSAM3X8EPWM),
		VMSTATE_UINT32(DTUPD2, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMR3, ATSAM3X8EPWM),
		VMSTATE_UINT32(CDTY3, ATSAM3X8EPWM),
		VMSTATE_UINT32(CDTYUPD3, ATSAM3X8EPWM),
		VMSTATE_UINT32(CPRD3, ATSAM3X8EPWM),
		VMSTATE_UINT32(CPRDUPD3, ATSAM3X8EPWM),
		VMSTATE_UINT32(CCNT3, ATSAM3X8EPWM),
		VMSTATE_UINT32(DT3, ATSAM3X8EPWM),
		VMSTATE_UINT32(DTUPD3, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMR4, ATSAM3X8EPWM),
		VMSTATE_UINT32(CDTY4, ATSAM3X8EPWM),
		VMSTATE_UINT32(CDTYUPD4, ATSAM3X8EPWM),
		VMSTATE_UINT32(CPRD4, ATSAM3X8EPWM),
		VMSTATE_UINT32(CPRDUPD4, ATSAM3X8EPWM),
		VMSTATE_UINT32(CCNT4, ATSAM3X8EPWM),
		VMSTATE_UINT32(DT4, ATSAM3X8EPWM),
		VMSTATE_UINT32(DTUPD4, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMR5, ATSAM3X8EPWM),
		VMSTATE_UINT32(CDTY5, ATSAM3X8EPWM),
		VMSTATE_UINT32(CDTYUPD5, ATSAM3X8EPWM),
		VMSTATE_UINT32(CPRD5, ATSAM3X8EPWM),
		VMSTATE_UINT32(CPRDUPD5, ATSAM3X8EPWM),
		VMSTATE_UINT32(CCNT5, ATSAM3X8EPWM),
		VMSTATE_UINT32(DT5, ATSAM3X8EPWM),
		VMSTATE_UINT32(DTUPD5, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMR6, ATSAM3X8EPWM),
		VMSTATE_UINT32(CDTY6, ATSAM3X8EPWM),
		VMSTATE_UINT32(CDTYUPD6, ATSAM3X8EPWM),
		VMSTATE_UINT32(CPRD6, ATSAM3X8EPWM),
		VMSTATE_UINT32(CPRDUPD6, ATSAM3X8EPWM),
		VMSTATE_UINT32(CCNT6, ATSAM3X8EPWM),
		VMSTATE_UINT32(DT6, ATSAM3X8EPWM),
		VMSTATE_UINT32(DTUPD6, ATSAM3X8EPWM),
		VMSTATE_UINT32(CMR7, ATSAM3X8EPWM),
		VMSTATE_UINT32(CDTY7, ATSAM3X8EPWM),
		VMSTATE_UINT32(CDTYUPD7, ATSAM3X8EPWM),
		VMSTATE_UINT32(CPRD7, ATSAM3X8EPWM),
		VMSTATE_UINT32(CPRDUPD7, ATSAM3X8EPWM),
		VMSTATE_UINT32(CCNT7, ATSAM3X8EPWM),
		VMSTATE_UINT32(DT7, ATSAM3X8EPWM),
		VMSTATE_UINT32(DTUPD7, ATSAM3X8EPWM),
		VMSTATE_UINT32(TPR, ATSAM3X8EPWM),
		VMSTATE_UINT32(TCR, ATSAM3X8EPWM),
		VMSTATE_UINT32(TNPR, ATSAM3X8EPWM),
		VMSTATE_UINT32(TNCR, ATSAM3X8EPWM),
		VMSTATE_UINT32(PTCR, ATSAM3X8EPWM),
		VMSTATE_UINT32(PTSR, ATSAM3X8EPWM),
		VMSTATE_UINT32(ELMR0, ATSAM3X8EPWM),
		VMSTATE_UINT32(ELMR1, ATSAM3X8EPWM),

		VMSTATE_END_OF_LIST()
	}
};

static void atsam3x8e_pwm_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsam3x8e_pwm_vmstate;
	dc->realize = atsam3x8e_pwm_realize;
	rc->phases.enter = atsam3x8e_pwm_reset_enter;

}

static const TypeInfo atsam3x8e_pwm_info = {
	.name = TYPE_ATSAM3X8E_PWM,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAM3X8EPWM),
	.instance_init = atsam3x8e_pwm_init,
	.class_init = atsam3x8e_pwm_class_init,
};

static void atsam3x8e_pwm_register_types(void) {
	type_register_static(&atsam3x8e_pwm_info);
}

type_init(atsam3x8e_pwm_register_types);

#define TYPE_ATSAM3X8E_MACHINE MACHINE_TYPE_NAME("atsam3x8e")
OBJECT_DECLARE_TYPE(ATSAM3X8EMachineState, ATSAM3X8EMachineClass, ATSAM3X8E_MACHINE)

struct ATSAM3X8EMachineClass {
	MachineClass parent;
};

struct ATSAM3X8EMachineState {
	MachineState parent;
	ARMv7MState armv7m;
};

static void atsam3x8e_periph_init(MachineState *machine) {
	ATSAM3X8EMachineState *sms = ATSAM3X8E_MACHINE(machine);
	ATSAM3X8EUART *p0 = g_new(ATSAM3X8EUART, 1);
	object_initialize_child(OBJECT(sms), "UART", p0, TYPE_ATSAM3X8E_UART);
	qdev_prop_set_chr(DEVICE(p0), "chardev", qemu_chr_new("soc-uart", "chardev:uart", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p0), &error_fatal);
	p0->base = 0x400e0800;
	sysbus_connect_irq(SYS_BUS_DEVICE(p0), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 8));
	sysbus_mmio_map(SYS_BUS_DEVICE(p0), 0, 0x400e0800);

	ATSAM3X8ERSTC *p1 = g_new(ATSAM3X8ERSTC, 1);
	object_initialize_child(OBJECT(sms), "RSTC", p1, TYPE_ATSAM3X8E_RSTC);
	sysbus_realize(SYS_BUS_DEVICE(p1), &error_fatal);
	p1->base = 0x400e1a00;
	sysbus_mmio_map(SYS_BUS_DEVICE(p1), 0, 0x400e1a00);

	ATSAM3X8EWDT *p2 = g_new(ATSAM3X8EWDT, 1);
	object_initialize_child(OBJECT(sms), "WDT", p2, TYPE_ATSAM3X8E_WDT);
	sysbus_realize(SYS_BUS_DEVICE(p2), &error_fatal);
	p2->base = 0x400e1a50;
	sysbus_mmio_map(SYS_BUS_DEVICE(p2), 0, 0x400e1a50);

	ATSAM3X8EEFC0 *p3 = g_new(ATSAM3X8EEFC0, 1);
	object_initialize_child(OBJECT(sms), "EFC0", p3, TYPE_ATSAM3X8E_EFC0);
	sysbus_realize(SYS_BUS_DEVICE(p3), &error_fatal);
	p3->base = 0x400e0a00;
	sysbus_connect_irq(SYS_BUS_DEVICE(p3), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 6));
	sysbus_mmio_map(SYS_BUS_DEVICE(p3), 0, 0x400e0a00);

	ATSAM3X8EEFC1 *p4 = g_new(ATSAM3X8EEFC1, 1);
	object_initialize_child(OBJECT(sms), "EFC1", p4, TYPE_ATSAM3X8E_EFC1);
	sysbus_realize(SYS_BUS_DEVICE(p4), &error_fatal);
	p4->base = 0x400e0c00;
	sysbus_connect_irq(SYS_BUS_DEVICE(p4), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 7));
	sysbus_mmio_map(SYS_BUS_DEVICE(p4), 0, 0x400e0c00);

	ATSAM3X8EPMC *p5 = g_new(ATSAM3X8EPMC, 1);
	object_initialize_child(OBJECT(sms), "PMC", p5, TYPE_ATSAM3X8E_PMC);
	sysbus_realize(SYS_BUS_DEVICE(p5), &error_fatal);
	p5->base = 0x400e0600;
	sysbus_connect_irq(SYS_BUS_DEVICE(p5), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 5));
	sysbus_mmio_map(SYS_BUS_DEVICE(p5), 0, 0x400e0600);

	ATSAM3X8EPIO *p6 = g_new(ATSAM3X8EPIO, 1);
	object_initialize_child(OBJECT(sms), "PIOA", p6, TYPE_ATSAM3X8E_PIO);
	qdev_prop_set_chr(DEVICE(p6), "chardev", qemu_chr_new("soc-pioa", "chardev:pioa", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p6), &error_fatal);
	p6->base = 0x400e0e00;
	sysbus_connect_irq(SYS_BUS_DEVICE(p6), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 11));
	sysbus_mmio_map(SYS_BUS_DEVICE(p6), 0, 0x400e0e00);

	ATSAM3X8EPIO *p7 = g_new(ATSAM3X8EPIO, 1);
	object_initialize_child(OBJECT(sms), "PIOB", p7, TYPE_ATSAM3X8E_PIO);
	qdev_prop_set_chr(DEVICE(p7), "chardev", qemu_chr_new("soc-piob", "chardev:piob", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p7), &error_fatal);
	p7->base = 0x400e1000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p7), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 12));
	sysbus_mmio_map(SYS_BUS_DEVICE(p7), 0, 0x400e1000);

	ATSAM3X8EPIO *p8 = g_new(ATSAM3X8EPIO, 1);
	object_initialize_child(OBJECT(sms), "PIOC", p8, TYPE_ATSAM3X8E_PIO);
	qdev_prop_set_chr(DEVICE(p8), "chardev", qemu_chr_new("soc-pioc", "chardev:pioc", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p8), &error_fatal);
	p8->base = 0x400e1200;
	sysbus_connect_irq(SYS_BUS_DEVICE(p8), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 13));
	sysbus_mmio_map(SYS_BUS_DEVICE(p8), 0, 0x400e1200);

	ATSAM3X8EPIO *p9 = g_new(ATSAM3X8EPIO, 1);
	object_initialize_child(OBJECT(sms), "PIOD", p9, TYPE_ATSAM3X8E_PIO);
	qdev_prop_set_chr(DEVICE(p9), "chardev", qemu_chr_new("soc-piod", "chardev:piod", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p9), &error_fatal);
	p9->base = 0x400e1400;
	sysbus_connect_irq(SYS_BUS_DEVICE(p9), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 14));
	sysbus_mmio_map(SYS_BUS_DEVICE(p9), 0, 0x400e1400);

	ATSAM3X8EADC *p10 = g_new(ATSAM3X8EADC, 1);
	object_initialize_child(OBJECT(sms), "ADC", p10, TYPE_ATSAM3X8E_ADC);
	qdev_prop_set_chr(DEVICE(p10), "chardev", qemu_chr_new("soc-adc", "chardev:adc", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p10), &error_fatal);
	p10->base = 0x400c0000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p10), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 37));
	sysbus_mmio_map(SYS_BUS_DEVICE(p10), 0, 0x400c0000);

	ATSAM3X8ESPI *p11 = g_new(ATSAM3X8ESPI, 1);
	object_initialize_child(OBJECT(sms), "SPI0", p11, TYPE_ATSAM3X8E_SPI);
	qdev_prop_set_chr(DEVICE(p11), "chardev", qemu_chr_new("soc-spi0", "chardev:spi0", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p11), &error_fatal);
	p11->base = 0x40008000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p11), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 24));
	sysbus_mmio_map(SYS_BUS_DEVICE(p11), 0, 0x40008000);

	ATSAM3X8EDACC *p12 = g_new(ATSAM3X8EDACC, 1);
	object_initialize_child(OBJECT(sms), "DACC", p12, TYPE_ATSAM3X8E_DACC);
	qdev_prop_set_chr(DEVICE(p12), "chardev", qemu_chr_new("soc-dacc", "chardev:dacc", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p12), &error_fatal);
	p12->base = 0x400c8000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p12), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 38));
	sysbus_mmio_map(SYS_BUS_DEVICE(p12), 0, 0x400c8000);

	ATSAM3X8ETWI *p13 = g_new(ATSAM3X8ETWI, 1);
	object_initialize_child(OBJECT(sms), "TWI0", p13, TYPE_ATSAM3X8E_TWI);
	qdev_prop_set_chr(DEVICE(p13), "chardev", qemu_chr_new("soc-twi0", "chardev:twi0", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p13), &error_fatal);
	p13->base = 0x4008c000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p13), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 22));
	sysbus_mmio_map(SYS_BUS_DEVICE(p13), 0, 0x4008c000);

	ATSAM3X8ETWI *p14 = g_new(ATSAM3X8ETWI, 1);
	object_initialize_child(OBJECT(sms), "TWI1", p14, TYPE_ATSAM3X8E_TWI);
	qdev_prop_set_chr(DEVICE(p14), "chardev", qemu_chr_new("soc-twi1", "chardev:twi1", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p14), &error_fatal);
	p14->base = 0x40090000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p14), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 23));
	sysbus_mmio_map(SYS_BUS_DEVICE(p14), 0, 0x40090000);

	ATSAM3X8EPWM *p15 = g_new(ATSAM3X8EPWM, 1);
	object_initialize_child(OBJECT(sms), "PWM", p15, TYPE_ATSAM3X8E_PWM);
	sysbus_realize(SYS_BUS_DEVICE(p15), &error_fatal);
	p15->base = 0x40094000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p15), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 36));
	sysbus_mmio_map(SYS_BUS_DEVICE(p15), 0, 0x40094000);

	ATSAM3X8ETC *p16 = g_new(ATSAM3X8ETC, 1);
	object_initialize_child(OBJECT(sms), "TC0", p16, TYPE_ATSAM3X8E_TC);
	sysbus_realize(SYS_BUS_DEVICE(p16), &error_fatal);
	p16->base = 0x40080000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p16), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 27));
	sysbus_connect_irq(SYS_BUS_DEVICE(p16), 1, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 28));
	sysbus_connect_irq(SYS_BUS_DEVICE(p16), 2, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 29));
	sysbus_mmio_map(SYS_BUS_DEVICE(p16), 0, 0x40080000);

	ATSAM3X8ETC *p17 = g_new(ATSAM3X8ETC, 1);
	object_initialize_child(OBJECT(sms), "TC1", p17, TYPE_ATSAM3X8E_TC);
	sysbus_realize(SYS_BUS_DEVICE(p17), &error_fatal);
	p17->base = 0x40084000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p17), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 30));
	sysbus_connect_irq(SYS_BUS_DEVICE(p17), 1, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 31));
	sysbus_connect_irq(SYS_BUS_DEVICE(p17), 2, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 32));
	sysbus_mmio_map(SYS_BUS_DEVICE(p17), 0, 0x40084000);


}

static void atsam3x8e_common_init(MachineState *machine) {
	ATSAM3X8EMachineState *sms = ATSAM3X8E_MACHINE(machine);
	MemoryRegion *sysmem = get_system_memory();
	MemoryRegion *mem;
	Clock *cpuclk = clock_new(OBJECT(sms), "SYSCLK");
	clock_set_hz(cpuclk, 84000000);
	Clock *refclk = clock_new(OBJECT(sms), "REFCLK");
	clock_set_hz(refclk, 84000000);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_rom(mem, NULL, "main-flash", 0x200000, &error_fatal);
	memory_region_add_subregion(sysmem, 0x0, mem);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_ram(mem, NULL, "sram", 0x200000, &error_fatal);
	memory_region_add_subregion(sysmem, 0x20000000, mem);


	object_initialize_child(OBJECT(sms), "armv7m", &(sms->armv7m), TYPE_ARMV7M);
	qdev_prop_set_uint32(DEVICE(&(sms->armv7m)),"num-irq", 256);
	qdev_prop_set_string(DEVICE(&(sms->armv7m)), "cpu-type", machine->cpu_type);
	qdev_prop_set_bit(DEVICE(&(sms->armv7m)), "enable-bitband", true);
	qdev_connect_clock_in(DEVICE(&(sms->armv7m)), "cpuclk", cpuclk);
	qdev_connect_clock_in(DEVICE(&(sms->armv7m)), "refclk", refclk);
	qdev_prop_set_uint32(DEVICE(&(sms->armv7m)), "init-nsvtor", 0x80000);
	object_property_set_link(OBJECT(&sms->armv7m), "memory", OBJECT(sysmem), &error_abort);
	sysbus_realize(SYS_BUS_DEVICE(&sms->armv7m), &error_fatal);

	atsam3x8e_periph_init(machine);

	armv7m_load_kernel(ARM_CPU(first_cpu), machine->kernel_filename, 0x0, 0x200000);
}

static void atsam3x8e_class_init(ObjectClass *oc, void *data) {
	MachineClass *mc = MACHINE_CLASS(oc);
	mc->desc = "atsam3x8e";
	mc->init = atsam3x8e_common_init;
	mc->max_cpus = 1;
	mc->default_cpus = 1;
	mc->default_cpu_type = ARM_CPU_TYPE_NAME("cortex-m3");
}

static const TypeInfo atsam3x8e_info = {
	.name = TYPE_ATSAM3X8E_MACHINE,
	.parent = TYPE_MACHINE,
	.instance_size = sizeof(ATSAM3X8EMachineState),
	.class_size = sizeof(ATSAM3X8EMachineClass),
	.class_init = atsam3x8e_class_init
};

static void atsam3x8e_machine_init(void) {
	type_register_static(&atsam3x8e_info);
}

type_init(atsam3x8e_machine_init);
