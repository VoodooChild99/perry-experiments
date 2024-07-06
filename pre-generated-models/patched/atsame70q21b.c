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


#define TYPE_ATSAME70Q21B_UART "atsame70q21b-uart"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAME70Q21BUART, ATSAME70Q21B_UART)

struct ATSAME70Q21BUART {
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
	uint32_t CMPR;
	uint32_t WPMR;
	/* chardev backend */
	CharBackend chr;
	guint watch_tag;

	/* base */
	uint32_t base;

};
#define ATSAME70Q21B_UART_SIZE				0xe8

REG32(UART_CR, 0x0)
	FIELD(UART_CR, RSTRX, 2, 1)
	FIELD(UART_CR, RSTTX, 3, 1)
	FIELD(UART_CR, RXEN, 4, 1)
	FIELD(UART_CR, RXDIS, 5, 1)
	FIELD(UART_CR, TXEN, 6, 1)
	FIELD(UART_CR, TXDIS, 7, 1)
	FIELD(UART_CR, RSTSTA, 8, 1)
	FIELD(UART_CR, REQCLR, 12, 1)
REG32(UART_MR, 0x4)
	FIELD(UART_MR, FILTER, 4, 1)
	FIELD(UART_MR, PAR, 9, 3)
	FIELD(UART_MR, BRSRCCK, 12, 1)
	FIELD(UART_MR, CHMODE, 14, 2)
REG32(UART_IER, 0x8)
	FIELD(UART_IER, RXRDY, 0, 1)
	FIELD(UART_IER, TXRDY, 1, 1)
	FIELD(UART_IER, OVRE, 5, 1)
	FIELD(UART_IER, FRAME, 6, 1)
	FIELD(UART_IER, PARE, 7, 1)
	FIELD(UART_IER, TXEMPTY, 9, 1)
	FIELD(UART_IER, CMP, 15, 1)
REG32(UART_IDR, 0xc)
	FIELD(UART_IDR, RXRDY, 0, 1)
	FIELD(UART_IDR, TXRDY, 1, 1)
	FIELD(UART_IDR, OVRE, 5, 1)
	FIELD(UART_IDR, FRAME, 6, 1)
	FIELD(UART_IDR, PARE, 7, 1)
	FIELD(UART_IDR, TXEMPTY, 9, 1)
	FIELD(UART_IDR, CMP, 15, 1)
REG32(UART_IMR, 0x10)
	FIELD(UART_IMR, RXRDY, 0, 1)
	FIELD(UART_IMR, TXRDY, 1, 1)
	FIELD(UART_IMR, OVRE, 5, 1)
	FIELD(UART_IMR, FRAME, 6, 1)
	FIELD(UART_IMR, PARE, 7, 1)
	FIELD(UART_IMR, TXEMPTY, 9, 1)
	FIELD(UART_IMR, CMP, 15, 1)
REG32(UART_SR, 0x14)
	FIELD(UART_SR, RXRDY, 0, 1)
	FIELD(UART_SR, TXRDY, 1, 1)
	FIELD(UART_SR, OVRE, 5, 1)
	FIELD(UART_SR, FRAME, 6, 1)
	FIELD(UART_SR, PARE, 7, 1)
	FIELD(UART_SR, TXEMPTY, 9, 1)
	FIELD(UART_SR, CMP, 15, 1)
REG32(UART_RHR, 0x18)
	FIELD(UART_RHR, RXCHR, 0, 8)
REG32(UART_THR, 0x1c)
	FIELD(UART_THR, TXCHR, 0, 8)
REG32(UART_BRGR, 0x20)
	FIELD(UART_BRGR, CD, 0, 16)
REG32(UART_CMPR, 0x24)
	FIELD(UART_CMPR, VAL1, 0, 8)
	FIELD(UART_CMPR, CMPMODE, 12, 1)
	FIELD(UART_CMPR, CMPPAR, 14, 1)
	FIELD(UART_CMPR, VAL2, 16, 8)
REG32(UART_WPMR, 0xe4)
	FIELD(UART_WPMR, WPEN, 0, 1)
	FIELD(UART_WPMR, WPKEY, 8, 24)


__attribute__((unused))
static void atsame70q21b_uart_update(ATSAME70Q21BUART *t) {
	int conditions = (((t->IMR & 0x1) && (t->SR & 0x1)) || ((t->IMR & 0x2) && (t->SR & 0x2)) || ((t->SR & 0x200) && (t->IMR & 0x200)));
	qemu_set_irq(t->irq[0], conditions);

}

static int atsame70q21b_uart_can_receive(void *opaque) {
	return 1;
}

static void atsame70q21b_uart_receive(void *opaque, const uint8_t *buf, int size) {
	ATSAME70Q21BUART *t = ATSAME70Q21B_UART(opaque);

	t->RHR = *buf;
	t->SR |= 0x1;

	atsame70q21b_uart_update(t);
}

static gboolean atsame70q21b_uart_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	ATSAME70Q21BUART *t = ATSAME70Q21B_UART(opaque);
	int ret;

	t->watch_tag = 0;
	t->SR &= (~(0x2));

	atsame70q21b_uart_update(t);

	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->THR), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, atsame70q21b_uart_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->SR |= 0x2;

	atsame70q21b_uart_update(t);

	return FALSE;
}

static void atsame70q21b_uart_register_reset(ATSAME70Q21BUART *t) {
	t->SR |= 0x2;

}

static uint64_t atsame70q21b_uart_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAME70Q21BUART *t = ATSAME70Q21B_UART(opaque);
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
			atsame70q21b_uart_update(t);
			break;
		case A_UART_BRGR:
			ret = t->BRGR;
			break;
		case A_UART_CMPR:
			ret = t->CMPR;
			break;
		case A_UART_WPMR:
			ret = t->WPMR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAME70Q21B UART read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsame70q21b_uart_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAME70Q21BUART *t = ATSAME70Q21B_UART(opaque);


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
		case A_UART_CMPR ... A_UART_CMPR + 3:
			value = value << ((offset - A_UART_CMPR) << 3);
			offset = A_UART_CMPR;
			break;
		case A_UART_WPMR ... A_UART_WPMR + 3:
			value = value << ((offset - A_UART_WPMR) << 3);
			offset = A_UART_WPMR;
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
			break;
		case A_UART_IDR:
			t->IDR = value;
			break;
		case A_UART_THR:
			t->THR = value;
			atsame70q21b_uart_transmit(NULL, G_IO_OUT, t);
			break;
		case A_UART_BRGR:
			t->BRGR = value;
			break;
		case A_UART_CMPR:
			t->CMPR = value;
			break;
		case A_UART_WPMR:
			t->WPMR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAME70Q21B UART write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsame70q21b_uart_ops = {
	.read = atsame70q21b_uart_read,
	.write = atsame70q21b_uart_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsame70q21b_uart_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAME70Q21BUART *t = ATSAME70Q21B_UART(obj);
	memory_region_init_io(&(t->iomem), obj, &atsame70q21b_uart_ops, t, "atsame70q21b-uart", ATSAME70Q21B_UART_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void atsame70q21b_uart_realize(DeviceState *dev, Error **errp) {
	ATSAME70Q21BUART *t = ATSAME70Q21B_UART(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), atsame70q21b_uart_can_receive, atsame70q21b_uart_receive,
		NULL, NULL, t, NULL, true);

}

static void atsame70q21b_uart_reset_enter(Object *obj, ResetType type) {
	ATSAME70Q21BUART *t = ATSAME70Q21B_UART(obj);
	atsame70q21b_uart_register_reset(t);
}

static const VMStateDescription atsame70q21b_uart_vmstate = {
	.name = "atsame70q21b-uart",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR, ATSAME70Q21BUART),
		VMSTATE_UINT32(MR, ATSAME70Q21BUART),
		VMSTATE_UINT32(IER, ATSAME70Q21BUART),
		VMSTATE_UINT32(IDR, ATSAME70Q21BUART),
		VMSTATE_UINT32(IMR, ATSAME70Q21BUART),
		VMSTATE_UINT32(SR, ATSAME70Q21BUART),
		VMSTATE_UINT32(RHR, ATSAME70Q21BUART),
		VMSTATE_UINT32(THR, ATSAME70Q21BUART),
		VMSTATE_UINT32(BRGR, ATSAME70Q21BUART),
		VMSTATE_UINT32(CMPR, ATSAME70Q21BUART),
		VMSTATE_UINT32(WPMR, ATSAME70Q21BUART),

		VMSTATE_END_OF_LIST()
	}
};

static Property atsame70q21b_uart_properties[] = {
	DEFINE_PROP_CHR("chardev", ATSAME70Q21BUART, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void atsame70q21b_uart_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsame70q21b_uart_vmstate;
	dc->realize = atsame70q21b_uart_realize;
	rc->phases.enter = atsame70q21b_uart_reset_enter;
	device_class_set_props(dc, atsame70q21b_uart_properties);

}

static const TypeInfo atsame70q21b_uart_info = {
	.name = TYPE_ATSAME70Q21B_UART,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAME70Q21BUART),
	.instance_init = atsame70q21b_uart_init,
	.class_init = atsame70q21b_uart_class_init,
};

static void atsame70q21b_uart_register_types(void) {
	type_register_static(&atsame70q21b_uart_info);
}

type_init(atsame70q21b_uart_register_types);

#define TYPE_ATSAME70Q21B_USART "atsame70q21b-usart"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAME70Q21BUSART, ATSAME70Q21B_USART)

struct ATSAME70Q21BUSART {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t US_CR_USART_MODE;
	uint32_t US_CR_SPI_MODE;
	uint32_t US_CR_LIN_MODE;
	uint32_t US_MR_USART_MODE;
	uint32_t US_MR_SPI_MODE;
	uint32_t US_IER_USART_MODE;
	uint32_t US_IER_SPI_MODE;
	uint32_t US_IER_LIN_MODE;
	uint32_t US_IER_LON_MODE;
	uint32_t US_IDR_USART_MODE;
	uint32_t US_IDR_SPI_MODE;
	uint32_t US_IDR_LIN_MODE;
	uint32_t US_IDR_LON_MODE;
	uint32_t US_IMR_USART_MODE;
	uint32_t US_IMR_SPI_MODE;
	uint32_t US_IMR_LIN_MODE;
	uint32_t US_IMR_LON_MODE;
	uint32_t US_CSR_USART_MODE;
	uint32_t US_CSR_SPI_MODE;
	uint32_t US_CSR_LIN_MODE;
	uint32_t US_CSR_LON_MODE;
	uint32_t US_RHR;
	uint32_t US_THR;
	uint32_t US_BRGR;
	uint32_t US_RTOR;
	uint32_t US_TTGR_USART_MODE;
	uint32_t US_TTGR_LON_MODE;
	uint32_t US_FIDI_USART_MODE;
	uint32_t US_FIDI_LON_MODE;
	uint32_t US_NER;
	uint32_t US_IF;
	uint32_t US_MAN;
	uint32_t US_LINMR;
	uint32_t US_LINIR;
	uint32_t US_LINBRR;
	uint32_t US_LONMR;
	uint32_t US_LONPR;
	uint32_t US_LONDL;
	uint32_t US_LONL2HDR;
	uint32_t US_LONBL;
	uint32_t US_LONB1TX;
	uint32_t US_LONB1RX;
	uint32_t US_LONPRIO;
	uint32_t US_IDTTX;
	uint32_t US_IDTRX;
	uint32_t US_ICDIFF;
	uint32_t US_WPMR;
	uint32_t US_WPSR;
	/* chardev backend */
	CharBackend chr;
	guint watch_tag;

	/* base */
	uint32_t base;

};
#define ATSAME70Q21B_USART_SIZE				0xec

REG32(USART_US_CR_USART_MODE, 0x0)
	FIELD(USART_US_CR_USART_MODE, RSTRX, 2, 1)
	FIELD(USART_US_CR_USART_MODE, RSTTX, 3, 1)
	FIELD(USART_US_CR_USART_MODE, RXEN, 4, 1)
	FIELD(USART_US_CR_USART_MODE, RXDIS, 5, 1)
	FIELD(USART_US_CR_USART_MODE, TXEN, 6, 1)
	FIELD(USART_US_CR_USART_MODE, TXDIS, 7, 1)
	FIELD(USART_US_CR_USART_MODE, RSTSTA, 8, 1)
	FIELD(USART_US_CR_USART_MODE, STTBRK, 9, 1)
	FIELD(USART_US_CR_USART_MODE, STPBRK, 10, 1)
	FIELD(USART_US_CR_USART_MODE, STTTO, 11, 1)
	FIELD(USART_US_CR_USART_MODE, SENDA, 12, 1)
	FIELD(USART_US_CR_USART_MODE, RSTIT, 13, 1)
	FIELD(USART_US_CR_USART_MODE, RSTNACK, 14, 1)
	FIELD(USART_US_CR_USART_MODE, RETTO, 15, 1)
	FIELD(USART_US_CR_USART_MODE, DTREN, 16, 1)
	FIELD(USART_US_CR_USART_MODE, DTRDIS, 17, 1)
	FIELD(USART_US_CR_USART_MODE, RTSEN, 18, 1)
	FIELD(USART_US_CR_USART_MODE, RTSDIS, 19, 1)
REG32(USART_US_CR_SPI_MODE, 0x0)
	FIELD(USART_US_CR_SPI_MODE, RSTRX, 2, 1)
	FIELD(USART_US_CR_SPI_MODE, RSTTX, 3, 1)
	FIELD(USART_US_CR_SPI_MODE, RXEN, 4, 1)
	FIELD(USART_US_CR_SPI_MODE, RXDIS, 5, 1)
	FIELD(USART_US_CR_SPI_MODE, TXEN, 6, 1)
	FIELD(USART_US_CR_SPI_MODE, TXDIS, 7, 1)
	FIELD(USART_US_CR_SPI_MODE, RSTSTA, 8, 1)
	FIELD(USART_US_CR_SPI_MODE, FCS, 18, 1)
	FIELD(USART_US_CR_SPI_MODE, RCS, 19, 1)
REG32(USART_US_CR_LIN_MODE, 0x0)
	FIELD(USART_US_CR_LIN_MODE, RSTRX, 2, 1)
	FIELD(USART_US_CR_LIN_MODE, RSTTX, 3, 1)
	FIELD(USART_US_CR_LIN_MODE, RXEN, 4, 1)
	FIELD(USART_US_CR_LIN_MODE, RXDIS, 5, 1)
	FIELD(USART_US_CR_LIN_MODE, TXEN, 6, 1)
	FIELD(USART_US_CR_LIN_MODE, TXDIS, 7, 1)
	FIELD(USART_US_CR_LIN_MODE, RSTSTA, 8, 1)
	FIELD(USART_US_CR_LIN_MODE, LINABT, 20, 1)
	FIELD(USART_US_CR_LIN_MODE, LINWKUP, 21, 1)
REG32(USART_US_MR_USART_MODE, 0x4)
	FIELD(USART_US_MR_USART_MODE, USART_MODE, 0, 4)
	FIELD(USART_US_MR_USART_MODE, USCLKS, 4, 2)
	FIELD(USART_US_MR_USART_MODE, CHRL, 6, 2)
	FIELD(USART_US_MR_USART_MODE, SYNC, 8, 1)
	FIELD(USART_US_MR_USART_MODE, PAR, 9, 3)
	FIELD(USART_US_MR_USART_MODE, NBSTOP, 12, 2)
	FIELD(USART_US_MR_USART_MODE, CHMODE, 14, 2)
	FIELD(USART_US_MR_USART_MODE, MSBF, 16, 1)
	FIELD(USART_US_MR_USART_MODE, MODE9, 17, 1)
	FIELD(USART_US_MR_USART_MODE, CLKO, 18, 1)
	FIELD(USART_US_MR_USART_MODE, OVER, 19, 1)
	FIELD(USART_US_MR_USART_MODE, INACK, 20, 1)
	FIELD(USART_US_MR_USART_MODE, DSNACK, 21, 1)
	FIELD(USART_US_MR_USART_MODE, VAR_SYNC, 22, 1)
	FIELD(USART_US_MR_USART_MODE, INVDATA, 23, 1)
	FIELD(USART_US_MR_USART_MODE, MAX_ITERATION, 24, 3)
	FIELD(USART_US_MR_USART_MODE, FILTER, 28, 1)
	FIELD(USART_US_MR_USART_MODE, MAN, 29, 1)
	FIELD(USART_US_MR_USART_MODE, MODSYNC, 30, 1)
	FIELD(USART_US_MR_USART_MODE, ONEBIT, 31, 1)
REG32(USART_US_MR_SPI_MODE, 0x4)
	FIELD(USART_US_MR_SPI_MODE, USART_MODE, 0, 4)
	FIELD(USART_US_MR_SPI_MODE, USCLKS, 4, 2)
	FIELD(USART_US_MR_SPI_MODE, CHRL, 6, 2)
	FIELD(USART_US_MR_SPI_MODE, CLKO, 18, 1)
	FIELD(USART_US_MR_SPI_MODE, CPHA, 8, 1)
	FIELD(USART_US_MR_SPI_MODE, CPOL, 16, 1)
	FIELD(USART_US_MR_SPI_MODE, WRDBT, 20, 1)
REG32(USART_US_IER_USART_MODE, 0x8)
	FIELD(USART_US_IER_USART_MODE, RXRDY, 0, 1)
	FIELD(USART_US_IER_USART_MODE, TXRDY, 1, 1)
	FIELD(USART_US_IER_USART_MODE, RXBRK, 2, 1)
	FIELD(USART_US_IER_USART_MODE, OVRE, 5, 1)
	FIELD(USART_US_IER_USART_MODE, FRAME, 6, 1)
	FIELD(USART_US_IER_USART_MODE, PARE, 7, 1)
	FIELD(USART_US_IER_USART_MODE, TIMEOUT, 8, 1)
	FIELD(USART_US_IER_USART_MODE, TXEMPTY, 9, 1)
	FIELD(USART_US_IER_USART_MODE, ITER, 10, 1)
	FIELD(USART_US_IER_USART_MODE, NACK, 13, 1)
	FIELD(USART_US_IER_USART_MODE, RIIC, 16, 1)
	FIELD(USART_US_IER_USART_MODE, DSRIC, 17, 1)
	FIELD(USART_US_IER_USART_MODE, DCDIC, 18, 1)
	FIELD(USART_US_IER_USART_MODE, CTSIC, 19, 1)
	FIELD(USART_US_IER_USART_MODE, MANE, 24, 1)
REG32(USART_US_IER_SPI_MODE, 0x8)
	FIELD(USART_US_IER_SPI_MODE, RXRDY, 0, 1)
	FIELD(USART_US_IER_SPI_MODE, TXRDY, 1, 1)
	FIELD(USART_US_IER_SPI_MODE, OVRE, 5, 1)
	FIELD(USART_US_IER_SPI_MODE, TXEMPTY, 9, 1)
	FIELD(USART_US_IER_SPI_MODE, UNRE, 10, 1)
	FIELD(USART_US_IER_SPI_MODE, NSSE, 19, 1)
REG32(USART_US_IER_LIN_MODE, 0x8)
	FIELD(USART_US_IER_LIN_MODE, RXRDY, 0, 1)
	FIELD(USART_US_IER_LIN_MODE, TXRDY, 1, 1)
	FIELD(USART_US_IER_LIN_MODE, OVRE, 5, 1)
	FIELD(USART_US_IER_LIN_MODE, FRAME, 6, 1)
	FIELD(USART_US_IER_LIN_MODE, PARE, 7, 1)
	FIELD(USART_US_IER_LIN_MODE, TIMEOUT, 8, 1)
	FIELD(USART_US_IER_LIN_MODE, TXEMPTY, 9, 1)
	FIELD(USART_US_IER_LIN_MODE, LINBK, 13, 1)
	FIELD(USART_US_IER_LIN_MODE, LINID, 14, 1)
	FIELD(USART_US_IER_LIN_MODE, LINTC, 15, 1)
	FIELD(USART_US_IER_LIN_MODE, LINBE, 25, 1)
	FIELD(USART_US_IER_LIN_MODE, LINISFE, 26, 1)
	FIELD(USART_US_IER_LIN_MODE, LINIPE, 27, 1)
	FIELD(USART_US_IER_LIN_MODE, LINCE, 28, 1)
	FIELD(USART_US_IER_LIN_MODE, LINSNRE, 29, 1)
	FIELD(USART_US_IER_LIN_MODE, LINSTE, 30, 1)
	FIELD(USART_US_IER_LIN_MODE, LINHTE, 31, 1)
REG32(USART_US_IER_LON_MODE, 0x8)
	FIELD(USART_US_IER_LON_MODE, RXRDY, 0, 1)
	FIELD(USART_US_IER_LON_MODE, TXRDY, 1, 1)
	FIELD(USART_US_IER_LON_MODE, OVRE, 5, 1)
	FIELD(USART_US_IER_LON_MODE, LSFE, 6, 1)
	FIELD(USART_US_IER_LON_MODE, LCRCE, 7, 1)
	FIELD(USART_US_IER_LON_MODE, TXEMPTY, 9, 1)
	FIELD(USART_US_IER_LON_MODE, UNRE, 10, 1)
	FIELD(USART_US_IER_LON_MODE, LTXD, 24, 1)
	FIELD(USART_US_IER_LON_MODE, LCOL, 25, 1)
	FIELD(USART_US_IER_LON_MODE, LFET, 26, 1)
	FIELD(USART_US_IER_LON_MODE, LRXD, 27, 1)
	FIELD(USART_US_IER_LON_MODE, LBLOVFE, 28, 1)
REG32(USART_US_IDR_USART_MODE, 0xc)
	FIELD(USART_US_IDR_USART_MODE, RXRDY, 0, 1)
	FIELD(USART_US_IDR_USART_MODE, TXRDY, 1, 1)
	FIELD(USART_US_IDR_USART_MODE, RXBRK, 2, 1)
	FIELD(USART_US_IDR_USART_MODE, OVRE, 5, 1)
	FIELD(USART_US_IDR_USART_MODE, FRAME, 6, 1)
	FIELD(USART_US_IDR_USART_MODE, PARE, 7, 1)
	FIELD(USART_US_IDR_USART_MODE, TIMEOUT, 8, 1)
	FIELD(USART_US_IDR_USART_MODE, TXEMPTY, 9, 1)
	FIELD(USART_US_IDR_USART_MODE, ITER, 10, 1)
	FIELD(USART_US_IDR_USART_MODE, NACK, 13, 1)
	FIELD(USART_US_IDR_USART_MODE, RIIC, 16, 1)
	FIELD(USART_US_IDR_USART_MODE, DSRIC, 17, 1)
	FIELD(USART_US_IDR_USART_MODE, DCDIC, 18, 1)
	FIELD(USART_US_IDR_USART_MODE, CTSIC, 19, 1)
	FIELD(USART_US_IDR_USART_MODE, MANE, 24, 1)
REG32(USART_US_IDR_SPI_MODE, 0xc)
	FIELD(USART_US_IDR_SPI_MODE, RXRDY, 0, 1)
	FIELD(USART_US_IDR_SPI_MODE, TXRDY, 1, 1)
	FIELD(USART_US_IDR_SPI_MODE, OVRE, 5, 1)
	FIELD(USART_US_IDR_SPI_MODE, TXEMPTY, 9, 1)
	FIELD(USART_US_IDR_SPI_MODE, UNRE, 10, 1)
	FIELD(USART_US_IDR_SPI_MODE, NSSE, 19, 1)
REG32(USART_US_IDR_LIN_MODE, 0xc)
	FIELD(USART_US_IDR_LIN_MODE, RXRDY, 0, 1)
	FIELD(USART_US_IDR_LIN_MODE, TXRDY, 1, 1)
	FIELD(USART_US_IDR_LIN_MODE, OVRE, 5, 1)
	FIELD(USART_US_IDR_LIN_MODE, FRAME, 6, 1)
	FIELD(USART_US_IDR_LIN_MODE, PARE, 7, 1)
	FIELD(USART_US_IDR_LIN_MODE, TIMEOUT, 8, 1)
	FIELD(USART_US_IDR_LIN_MODE, TXEMPTY, 9, 1)
	FIELD(USART_US_IDR_LIN_MODE, LINBK, 13, 1)
	FIELD(USART_US_IDR_LIN_MODE, LINID, 14, 1)
	FIELD(USART_US_IDR_LIN_MODE, LINTC, 15, 1)
	FIELD(USART_US_IDR_LIN_MODE, LINBE, 25, 1)
	FIELD(USART_US_IDR_LIN_MODE, LINISFE, 26, 1)
	FIELD(USART_US_IDR_LIN_MODE, LINIPE, 27, 1)
	FIELD(USART_US_IDR_LIN_MODE, LINCE, 28, 1)
	FIELD(USART_US_IDR_LIN_MODE, LINSNRE, 29, 1)
	FIELD(USART_US_IDR_LIN_MODE, LINSTE, 30, 1)
	FIELD(USART_US_IDR_LIN_MODE, LINHTE, 31, 1)
REG32(USART_US_IDR_LON_MODE, 0xc)
	FIELD(USART_US_IDR_LON_MODE, RXRDY, 0, 1)
	FIELD(USART_US_IDR_LON_MODE, TXRDY, 1, 1)
	FIELD(USART_US_IDR_LON_MODE, OVRE, 5, 1)
	FIELD(USART_US_IDR_LON_MODE, LSFE, 6, 1)
	FIELD(USART_US_IDR_LON_MODE, LCRCE, 7, 1)
	FIELD(USART_US_IDR_LON_MODE, TXEMPTY, 9, 1)
	FIELD(USART_US_IDR_LON_MODE, UNRE, 10, 1)
	FIELD(USART_US_IDR_LON_MODE, LTXD, 24, 1)
	FIELD(USART_US_IDR_LON_MODE, LCOL, 25, 1)
	FIELD(USART_US_IDR_LON_MODE, LFET, 26, 1)
	FIELD(USART_US_IDR_LON_MODE, LRXD, 27, 1)
	FIELD(USART_US_IDR_LON_MODE, LBLOVFE, 28, 1)
REG32(USART_US_IMR_USART_MODE, 0x10)
	FIELD(USART_US_IMR_USART_MODE, RXRDY, 0, 1)
	FIELD(USART_US_IMR_USART_MODE, TXRDY, 1, 1)
	FIELD(USART_US_IMR_USART_MODE, RXBRK, 2, 1)
	FIELD(USART_US_IMR_USART_MODE, OVRE, 5, 1)
	FIELD(USART_US_IMR_USART_MODE, FRAME, 6, 1)
	FIELD(USART_US_IMR_USART_MODE, PARE, 7, 1)
	FIELD(USART_US_IMR_USART_MODE, TIMEOUT, 8, 1)
	FIELD(USART_US_IMR_USART_MODE, TXEMPTY, 9, 1)
	FIELD(USART_US_IMR_USART_MODE, ITER, 10, 1)
	FIELD(USART_US_IMR_USART_MODE, NACK, 13, 1)
	FIELD(USART_US_IMR_USART_MODE, RIIC, 16, 1)
	FIELD(USART_US_IMR_USART_MODE, DSRIC, 17, 1)
	FIELD(USART_US_IMR_USART_MODE, DCDIC, 18, 1)
	FIELD(USART_US_IMR_USART_MODE, CTSIC, 19, 1)
	FIELD(USART_US_IMR_USART_MODE, MANE, 24, 1)
REG32(USART_US_IMR_SPI_MODE, 0x10)
	FIELD(USART_US_IMR_SPI_MODE, RXRDY, 0, 1)
	FIELD(USART_US_IMR_SPI_MODE, TXRDY, 1, 1)
	FIELD(USART_US_IMR_SPI_MODE, OVRE, 5, 1)
	FIELD(USART_US_IMR_SPI_MODE, UNRE, 10, 1)
	FIELD(USART_US_IMR_SPI_MODE, TXEMPTY, 9, 1)
	FIELD(USART_US_IMR_SPI_MODE, NSSE, 19, 1)
REG32(USART_US_IMR_LIN_MODE, 0x10)
	FIELD(USART_US_IMR_LIN_MODE, RXRDY, 0, 1)
	FIELD(USART_US_IMR_LIN_MODE, TXRDY, 1, 1)
	FIELD(USART_US_IMR_LIN_MODE, OVRE, 5, 1)
	FIELD(USART_US_IMR_LIN_MODE, FRAME, 6, 1)
	FIELD(USART_US_IMR_LIN_MODE, PARE, 7, 1)
	FIELD(USART_US_IMR_LIN_MODE, TIMEOUT, 8, 1)
	FIELD(USART_US_IMR_LIN_MODE, TXEMPTY, 9, 1)
	FIELD(USART_US_IMR_LIN_MODE, LINBK, 13, 1)
	FIELD(USART_US_IMR_LIN_MODE, LINID, 14, 1)
	FIELD(USART_US_IMR_LIN_MODE, LINTC, 15, 1)
	FIELD(USART_US_IMR_LIN_MODE, LINBE, 25, 1)
	FIELD(USART_US_IMR_LIN_MODE, LINISFE, 26, 1)
	FIELD(USART_US_IMR_LIN_MODE, LINIPE, 27, 1)
	FIELD(USART_US_IMR_LIN_MODE, LINCE, 28, 1)
	FIELD(USART_US_IMR_LIN_MODE, LINSNRE, 29, 1)
	FIELD(USART_US_IMR_LIN_MODE, LINSTE, 30, 1)
	FIELD(USART_US_IMR_LIN_MODE, LINHTE, 31, 1)
REG32(USART_US_IMR_LON_MODE, 0x10)
	FIELD(USART_US_IMR_LON_MODE, RXRDY, 0, 1)
	FIELD(USART_US_IMR_LON_MODE, TXRDY, 1, 1)
	FIELD(USART_US_IMR_LON_MODE, OVRE, 5, 1)
	FIELD(USART_US_IMR_LON_MODE, LSFE, 6, 1)
	FIELD(USART_US_IMR_LON_MODE, LCRCE, 7, 1)
	FIELD(USART_US_IMR_LON_MODE, TXEMPTY, 9, 1)
	FIELD(USART_US_IMR_LON_MODE, UNRE, 10, 1)
	FIELD(USART_US_IMR_LON_MODE, LTXD, 24, 1)
	FIELD(USART_US_IMR_LON_MODE, LCOL, 25, 1)
	FIELD(USART_US_IMR_LON_MODE, LFET, 26, 1)
	FIELD(USART_US_IMR_LON_MODE, LRXD, 27, 1)
	FIELD(USART_US_IMR_LON_MODE, LBLOVFE, 28, 1)
REG32(USART_US_CSR_USART_MODE, 0x14)
	FIELD(USART_US_CSR_USART_MODE, RXRDY, 0, 1)
	FIELD(USART_US_CSR_USART_MODE, TXRDY, 1, 1)
	FIELD(USART_US_CSR_USART_MODE, RXBRK, 2, 1)
	FIELD(USART_US_CSR_USART_MODE, OVRE, 5, 1)
	FIELD(USART_US_CSR_USART_MODE, FRAME, 6, 1)
	FIELD(USART_US_CSR_USART_MODE, PARE, 7, 1)
	FIELD(USART_US_CSR_USART_MODE, TIMEOUT, 8, 1)
	FIELD(USART_US_CSR_USART_MODE, TXEMPTY, 9, 1)
	FIELD(USART_US_CSR_USART_MODE, ITER, 10, 1)
	FIELD(USART_US_CSR_USART_MODE, NACK, 13, 1)
	FIELD(USART_US_CSR_USART_MODE, RIIC, 16, 1)
	FIELD(USART_US_CSR_USART_MODE, DSRIC, 17, 1)
	FIELD(USART_US_CSR_USART_MODE, DCDIC, 18, 1)
	FIELD(USART_US_CSR_USART_MODE, CTSIC, 19, 1)
	FIELD(USART_US_CSR_USART_MODE, RI, 20, 1)
	FIELD(USART_US_CSR_USART_MODE, DSR, 21, 1)
	FIELD(USART_US_CSR_USART_MODE, DCD, 22, 1)
	FIELD(USART_US_CSR_USART_MODE, CTS, 23, 1)
	FIELD(USART_US_CSR_USART_MODE, MANERR, 24, 1)
REG32(USART_US_CSR_SPI_MODE, 0x14)
	FIELD(USART_US_CSR_SPI_MODE, RXRDY, 0, 1)
	FIELD(USART_US_CSR_SPI_MODE, TXRDY, 1, 1)
	FIELD(USART_US_CSR_SPI_MODE, OVRE, 5, 1)
	FIELD(USART_US_CSR_SPI_MODE, TXEMPTY, 9, 1)
	FIELD(USART_US_CSR_SPI_MODE, UNRE, 10, 1)
	FIELD(USART_US_CSR_SPI_MODE, NSSE, 19, 1)
	FIELD(USART_US_CSR_SPI_MODE, NSS, 23, 1)
REG32(USART_US_CSR_LIN_MODE, 0x14)
	FIELD(USART_US_CSR_LIN_MODE, RXRDY, 0, 1)
	FIELD(USART_US_CSR_LIN_MODE, TXRDY, 1, 1)
	FIELD(USART_US_CSR_LIN_MODE, OVRE, 5, 1)
	FIELD(USART_US_CSR_LIN_MODE, FRAME, 6, 1)
	FIELD(USART_US_CSR_LIN_MODE, PARE, 7, 1)
	FIELD(USART_US_CSR_LIN_MODE, TIMEOUT, 8, 1)
	FIELD(USART_US_CSR_LIN_MODE, TXEMPTY, 9, 1)
	FIELD(USART_US_CSR_LIN_MODE, LINBK, 13, 1)
	FIELD(USART_US_CSR_LIN_MODE, LINID, 14, 1)
	FIELD(USART_US_CSR_LIN_MODE, LINTC, 15, 1)
	FIELD(USART_US_CSR_LIN_MODE, LINBLS, 23, 1)
	FIELD(USART_US_CSR_LIN_MODE, LINBE, 25, 1)
	FIELD(USART_US_CSR_LIN_MODE, LINISFE, 26, 1)
	FIELD(USART_US_CSR_LIN_MODE, LINIPE, 27, 1)
	FIELD(USART_US_CSR_LIN_MODE, LINCE, 28, 1)
	FIELD(USART_US_CSR_LIN_MODE, LINSNRE, 29, 1)
	FIELD(USART_US_CSR_LIN_MODE, LINSTE, 30, 1)
	FIELD(USART_US_CSR_LIN_MODE, LINHTE, 31, 1)
REG32(USART_US_CSR_LON_MODE, 0x14)
	FIELD(USART_US_CSR_LON_MODE, RXRDY, 0, 1)
	FIELD(USART_US_CSR_LON_MODE, TXRDY, 1, 1)
	FIELD(USART_US_CSR_LON_MODE, OVRE, 5, 1)
	FIELD(USART_US_CSR_LON_MODE, LSFE, 6, 1)
	FIELD(USART_US_CSR_LON_MODE, LCRCE, 7, 1)
	FIELD(USART_US_CSR_LON_MODE, TXEMPTY, 9, 1)
	FIELD(USART_US_CSR_LON_MODE, UNRE, 10, 1)
	FIELD(USART_US_CSR_LON_MODE, LTXD, 24, 1)
	FIELD(USART_US_CSR_LON_MODE, LCOL, 25, 1)
	FIELD(USART_US_CSR_LON_MODE, LFET, 26, 1)
	FIELD(USART_US_CSR_LON_MODE, LRXD, 27, 1)
	FIELD(USART_US_CSR_LON_MODE, LBLOVFE, 28, 1)
REG32(USART_US_RHR, 0x18)
	FIELD(USART_US_RHR, RXCHR, 0, 9)
	FIELD(USART_US_RHR, RXSYNH, 15, 1)
REG32(USART_US_THR, 0x1c)
	FIELD(USART_US_THR, TXCHR, 0, 9)
	FIELD(USART_US_THR, TXSYNH, 15, 1)
REG32(USART_US_BRGR, 0x20)
	FIELD(USART_US_BRGR, CD, 0, 16)
	FIELD(USART_US_BRGR, FP, 16, 3)
REG32(USART_US_RTOR, 0x24)
	FIELD(USART_US_RTOR, TO, 0, 17)
REG32(USART_US_TTGR_USART_MODE, 0x28)
	FIELD(USART_US_TTGR_USART_MODE, TG, 0, 8)
REG32(USART_US_TTGR_LON_MODE, 0x28)
	FIELD(USART_US_TTGR_LON_MODE, PCYCLE, 0, 24)
REG32(USART_US_FIDI_USART_MODE, 0x40)
	FIELD(USART_US_FIDI_USART_MODE, FI_DI_RATIO, 0, 16)
REG32(USART_US_FIDI_LON_MODE, 0x40)
	FIELD(USART_US_FIDI_LON_MODE, BETA2, 0, 24)
REG32(USART_US_NER, 0x44)
	FIELD(USART_US_NER, NB_ERRORS, 0, 8)
REG32(USART_US_IF, 0x4c)
	FIELD(USART_US_IF, IRDA_FILTER, 0, 8)
REG32(USART_US_MAN, 0x50)
	FIELD(USART_US_MAN, TX_PL, 0, 4)
	FIELD(USART_US_MAN, TX_PP, 8, 2)
	FIELD(USART_US_MAN, TX_MPOL, 12, 1)
	FIELD(USART_US_MAN, RX_PL, 16, 4)
	FIELD(USART_US_MAN, RX_PP, 24, 2)
	FIELD(USART_US_MAN, RX_MPOL, 28, 1)
	FIELD(USART_US_MAN, ONE, 29, 1)
	FIELD(USART_US_MAN, DRIFT, 30, 1)
	FIELD(USART_US_MAN, RXIDLEV, 31, 1)
REG32(USART_US_LINMR, 0x54)
	FIELD(USART_US_LINMR, NACT, 0, 2)
	FIELD(USART_US_LINMR, PARDIS, 2, 1)
	FIELD(USART_US_LINMR, CHKDIS, 3, 1)
	FIELD(USART_US_LINMR, CHKTYP, 4, 1)
	FIELD(USART_US_LINMR, DLM, 5, 1)
	FIELD(USART_US_LINMR, FSDIS, 6, 1)
	FIELD(USART_US_LINMR, WKUPTYP, 7, 1)
	FIELD(USART_US_LINMR, DLC, 8, 8)
	FIELD(USART_US_LINMR, PDCM, 16, 1)
	FIELD(USART_US_LINMR, SYNCDIS, 17, 1)
REG32(USART_US_LINIR, 0x58)
	FIELD(USART_US_LINIR, IDCHR, 0, 8)
REG32(USART_US_LINBRR, 0x5c)
	FIELD(USART_US_LINBRR, LINCD, 0, 16)
	FIELD(USART_US_LINBRR, LINFP, 16, 3)
REG32(USART_US_LONMR, 0x60)
	FIELD(USART_US_LONMR, COMMT, 0, 1)
	FIELD(USART_US_LONMR, COLDET, 1, 1)
	FIELD(USART_US_LONMR, TCOL, 2, 1)
	FIELD(USART_US_LONMR, CDTAIL, 3, 1)
	FIELD(USART_US_LONMR, DMAM, 4, 1)
	FIELD(USART_US_LONMR, LCDS, 5, 1)
	FIELD(USART_US_LONMR, EOFS, 16, 8)
REG32(USART_US_LONPR, 0x64)
	FIELD(USART_US_LONPR, LONPL, 0, 14)
REG32(USART_US_LONDL, 0x68)
	FIELD(USART_US_LONDL, LONDL, 0, 8)
REG32(USART_US_LONL2HDR, 0x6c)
	FIELD(USART_US_LONL2HDR, BLI, 0, 6)
	FIELD(USART_US_LONL2HDR, ALTP, 6, 1)
	FIELD(USART_US_LONL2HDR, PB, 7, 1)
REG32(USART_US_LONBL, 0x70)
	FIELD(USART_US_LONBL, LONBL, 0, 6)
REG32(USART_US_LONB1TX, 0x74)
	FIELD(USART_US_LONB1TX, BETA1TX, 0, 24)
REG32(USART_US_LONB1RX, 0x78)
	FIELD(USART_US_LONB1RX, BETA1RX, 0, 24)
REG32(USART_US_LONPRIO, 0x7c)
	FIELD(USART_US_LONPRIO, PSNB, 0, 7)
	FIELD(USART_US_LONPRIO, NPS, 8, 7)
REG32(USART_US_IDTTX, 0x80)
	FIELD(USART_US_IDTTX, IDTTX, 0, 24)
REG32(USART_US_IDTRX, 0x84)
	FIELD(USART_US_IDTRX, IDTRX, 0, 24)
REG32(USART_US_ICDIFF, 0x88)
	FIELD(USART_US_ICDIFF, ICDIFF, 0, 4)
REG32(USART_US_WPMR, 0xe4)
	FIELD(USART_US_WPMR, WPEN, 0, 1)
	FIELD(USART_US_WPMR, WPKEY, 8, 24)
REG32(USART_US_WPSR, 0xe8)
	FIELD(USART_US_WPSR, WPVS, 0, 1)
	FIELD(USART_US_WPSR, WPVSRC, 8, 16)


__attribute__((unused))
static void atsame70q21b_usart_update(ATSAME70Q21BUSART *t) {
	int conditions = (((t->US_IMR_USART_MODE & 0x1) && (t->US_CSR_USART_MODE & 0x1)) || ((t->US_IMR_USART_MODE & 0x2) && (t->US_CSR_USART_MODE & 0x2)) || ((t->US_CSR_USART_MODE & 0x200) && (t->US_IMR_USART_MODE & 0x200)));
	qemu_set_irq(t->irq[0], conditions);

}

static int atsame70q21b_usart_can_receive(void *opaque) {
	return 1;
}

static void atsame70q21b_usart_receive(void *opaque, const uint8_t *buf, int size) {
	ATSAME70Q21BUSART *t = ATSAME70Q21B_USART(opaque);

	t->US_RHR = *buf;
	t->US_CSR_USART_MODE |= 0x1;

	atsame70q21b_usart_update(t);
}

static gboolean atsame70q21b_usart_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	ATSAME70Q21BUSART *t = ATSAME70Q21B_USART(opaque);
	int ret;

	t->watch_tag = 0;
	t->US_CSR_USART_MODE &= (~(0x2));

	atsame70q21b_usart_update(t);

	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->US_THR), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, atsame70q21b_usart_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->US_CSR_USART_MODE |= 0x2;

	atsame70q21b_usart_update(t);

	return FALSE;
}

static void atsame70q21b_usart_register_reset(ATSAME70Q21BUSART *t) {
	t->US_CSR_USART_MODE |= 0x2;

}

static uint64_t atsame70q21b_usart_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAME70Q21BUSART *t = ATSAME70Q21B_USART(opaque);
	uint64_t ret;

	switch (offset) {
		case A_USART_US_MR_USART_MODE:
			ret = t->US_MR_USART_MODE;
			break;
		case A_USART_US_IMR_USART_MODE:
			ret = t->US_IMR_USART_MODE;
			break;
		case A_USART_US_CSR_USART_MODE:
			ret = t->US_CSR_USART_MODE;
			break;
		case A_USART_US_RHR:
			ret = t->US_RHR;
			t->US_CSR_USART_MODE &= (~(0x1));
			qemu_chr_fe_accept_input(&(t->chr));
			atsame70q21b_usart_update(t);
			break;
		case A_USART_US_BRGR:
			ret = t->US_BRGR;
			break;
		case A_USART_US_RTOR:
			ret = t->US_RTOR;
			break;
		case A_USART_US_TTGR_USART_MODE:
			ret = t->US_TTGR_USART_MODE;
			break;
		case A_USART_US_FIDI_USART_MODE:
			ret = t->US_FIDI_USART_MODE;
			break;
		case A_USART_US_NER:
			ret = t->US_NER;
			break;
		case A_USART_US_IF:
			ret = t->US_IF;
			break;
		case A_USART_US_MAN:
			ret = t->US_MAN;
			break;
		case A_USART_US_LINMR:
			ret = t->US_LINMR;
			break;
		case A_USART_US_LINIR:
			ret = t->US_LINIR;
			break;
		case A_USART_US_LINBRR:
			ret = t->US_LINBRR;
			break;
		case A_USART_US_LONMR:
			ret = t->US_LONMR;
			break;
		case A_USART_US_LONPR:
			ret = t->US_LONPR;
			break;
		case A_USART_US_LONDL:
			ret = t->US_LONDL;
			break;
		case A_USART_US_LONL2HDR:
			ret = t->US_LONL2HDR;
			break;
		case A_USART_US_LONBL:
			ret = t->US_LONBL;
			break;
		case A_USART_US_LONB1TX:
			ret = t->US_LONB1TX;
			break;
		case A_USART_US_LONB1RX:
			ret = t->US_LONB1RX;
			break;
		case A_USART_US_LONPRIO:
			ret = t->US_LONPRIO;
			break;
		case A_USART_US_IDTTX:
			ret = t->US_IDTTX;
			break;
		case A_USART_US_IDTRX:
			ret = t->US_IDTRX;
			break;
		case A_USART_US_ICDIFF:
			ret = t->US_ICDIFF;
			break;
		case A_USART_US_WPMR:
			ret = t->US_WPMR;
			break;
		case A_USART_US_WPSR:
			ret = t->US_WPSR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAME70Q21B USART read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsame70q21b_usart_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAME70Q21BUSART *t = ATSAME70Q21B_USART(opaque);


	switch (offset) {
		case A_USART_US_CR_USART_MODE ... A_USART_US_CR_USART_MODE + 3:
			value = value << ((offset - A_USART_US_CR_USART_MODE) << 3);
			offset = A_USART_US_CR_USART_MODE;
			break;
		case A_USART_US_MR_USART_MODE ... A_USART_US_MR_USART_MODE + 3:
			value = value << ((offset - A_USART_US_MR_USART_MODE) << 3);
			offset = A_USART_US_MR_USART_MODE;
			break;
		case A_USART_US_IER_USART_MODE ... A_USART_US_IER_USART_MODE + 3:
			value = value << ((offset - A_USART_US_IER_USART_MODE) << 3);
			offset = A_USART_US_IER_USART_MODE;
			break;
		case A_USART_US_IDR_USART_MODE ... A_USART_US_IDR_USART_MODE + 3:
			value = value << ((offset - A_USART_US_IDR_USART_MODE) << 3);
			offset = A_USART_US_IDR_USART_MODE;
			break;
		case A_USART_US_THR ... A_USART_US_THR + 3:
			value = value << ((offset - A_USART_US_THR) << 3);
			offset = A_USART_US_THR;
			break;
		case A_USART_US_BRGR ... A_USART_US_BRGR + 3:
			value = value << ((offset - A_USART_US_BRGR) << 3);
			offset = A_USART_US_BRGR;
			break;
		case A_USART_US_RTOR ... A_USART_US_RTOR + 3:
			value = value << ((offset - A_USART_US_RTOR) << 3);
			offset = A_USART_US_RTOR;
			break;
		case A_USART_US_TTGR_USART_MODE ... A_USART_US_TTGR_USART_MODE + 3:
			value = value << ((offset - A_USART_US_TTGR_USART_MODE) << 3);
			offset = A_USART_US_TTGR_USART_MODE;
			break;
		case A_USART_US_FIDI_USART_MODE ... A_USART_US_FIDI_USART_MODE + 3:
			value = value << ((offset - A_USART_US_FIDI_USART_MODE) << 3);
			offset = A_USART_US_FIDI_USART_MODE;
			break;
		case A_USART_US_IF ... A_USART_US_IF + 3:
			value = value << ((offset - A_USART_US_IF) << 3);
			offset = A_USART_US_IF;
			break;
		case A_USART_US_MAN ... A_USART_US_MAN + 3:
			value = value << ((offset - A_USART_US_MAN) << 3);
			offset = A_USART_US_MAN;
			break;
		case A_USART_US_LINMR ... A_USART_US_LINMR + 3:
			value = value << ((offset - A_USART_US_LINMR) << 3);
			offset = A_USART_US_LINMR;
			break;
		case A_USART_US_LINIR ... A_USART_US_LINIR + 3:
			value = value << ((offset - A_USART_US_LINIR) << 3);
			offset = A_USART_US_LINIR;
			break;
		case A_USART_US_LONMR ... A_USART_US_LONMR + 3:
			value = value << ((offset - A_USART_US_LONMR) << 3);
			offset = A_USART_US_LONMR;
			break;
		case A_USART_US_LONPR ... A_USART_US_LONPR + 3:
			value = value << ((offset - A_USART_US_LONPR) << 3);
			offset = A_USART_US_LONPR;
			break;
		case A_USART_US_LONDL ... A_USART_US_LONDL + 3:
			value = value << ((offset - A_USART_US_LONDL) << 3);
			offset = A_USART_US_LONDL;
			break;
		case A_USART_US_LONL2HDR ... A_USART_US_LONL2HDR + 3:
			value = value << ((offset - A_USART_US_LONL2HDR) << 3);
			offset = A_USART_US_LONL2HDR;
			break;
		case A_USART_US_LONB1TX ... A_USART_US_LONB1TX + 3:
			value = value << ((offset - A_USART_US_LONB1TX) << 3);
			offset = A_USART_US_LONB1TX;
			break;
		case A_USART_US_LONB1RX ... A_USART_US_LONB1RX + 3:
			value = value << ((offset - A_USART_US_LONB1RX) << 3);
			offset = A_USART_US_LONB1RX;
			break;
		case A_USART_US_LONPRIO ... A_USART_US_LONPRIO + 3:
			value = value << ((offset - A_USART_US_LONPRIO) << 3);
			offset = A_USART_US_LONPRIO;
			break;
		case A_USART_US_IDTTX ... A_USART_US_IDTTX + 3:
			value = value << ((offset - A_USART_US_IDTTX) << 3);
			offset = A_USART_US_IDTTX;
			break;
		case A_USART_US_IDTRX ... A_USART_US_IDTRX + 3:
			value = value << ((offset - A_USART_US_IDTRX) << 3);
			offset = A_USART_US_IDTRX;
			break;
		case A_USART_US_ICDIFF ... A_USART_US_ICDIFF + 3:
			value = value << ((offset - A_USART_US_ICDIFF) << 3);
			offset = A_USART_US_ICDIFF;
			break;
		case A_USART_US_WPMR ... A_USART_US_WPMR + 3:
			value = value << ((offset - A_USART_US_WPMR) << 3);
			offset = A_USART_US_WPMR;
			break;

		default: break;
	}

	switch (offset) {
		case A_USART_US_CR_USART_MODE:
			t->US_CR_USART_MODE = value;
			break;
		case A_USART_US_MR_USART_MODE:
			t->US_MR_USART_MODE = value;
			break;
		case A_USART_US_IER_USART_MODE:
			t->US_IER_USART_MODE = value;
			// PERRY PATCH
			t->US_IMR_USART_MODE |= value; atsame70q21b_usart_update(t);
			break;
		case A_USART_US_IDR_USART_MODE:
			t->US_IDR_USART_MODE = value;
			// PERRY PATCH
			t->US_IMR_USART_MODE &= (~value); atsame70q21b_usart_update(t);
			break;
		case A_USART_US_THR:
			t->US_THR = value;
			atsame70q21b_usart_transmit(NULL, G_IO_OUT, t);
			if (((!(value & 0x100)) && (!(value & 0x200)) && (!(value & 0x400)) && (!(value & 0x800)) && (!(value & 0x1000)) && (!(value & 0x2000)) && (!(value & 0x4000)) && (!(value & 0x8000)) && (!(value & 0x10000)) && (!(value & 0x20000)) && (!(value & 0x40000)) && (!(value & 0x80000)) && (!(value & 0x100000)) && (!(value & 0x200000)) && (!(value & 0x400000)) && (!(value & 0x800000)) && (!(value & 0x1000000)) && (!(value & 0x2000000)) && (!(value & 0x4000000)) && (!(value & 0x8000000)) && (!(value & 0x10000000)) && (!(value & 0x20000000)) && (!(value & 0x40000000)) && (!(value & 0x80000000)))) {
				t->US_CSR_USART_MODE |= 0x2;
			}
			atsame70q21b_usart_update(t);
			break;
		case A_USART_US_BRGR:
			t->US_BRGR = value;
			break;
		case A_USART_US_RTOR:
			t->US_RTOR = value;
			break;
		case A_USART_US_TTGR_USART_MODE:
			t->US_TTGR_USART_MODE = value;
			break;
		case A_USART_US_FIDI_USART_MODE:
			t->US_FIDI_USART_MODE = value;
			break;
		case A_USART_US_IF:
			t->US_IF = value;
			break;
		case A_USART_US_MAN:
			t->US_MAN = value;
			break;
		case A_USART_US_LINMR:
			t->US_LINMR = value;
			break;
		case A_USART_US_LINIR:
			t->US_LINIR = value;
			break;
		case A_USART_US_LONMR:
			t->US_LONMR = value;
			break;
		case A_USART_US_LONPR:
			t->US_LONPR = value;
			break;
		case A_USART_US_LONDL:
			t->US_LONDL = value;
			break;
		case A_USART_US_LONL2HDR:
			t->US_LONL2HDR = value;
			break;
		case A_USART_US_LONB1TX:
			t->US_LONB1TX = value;
			break;
		case A_USART_US_LONB1RX:
			t->US_LONB1RX = value;
			break;
		case A_USART_US_LONPRIO:
			t->US_LONPRIO = value;
			break;
		case A_USART_US_IDTTX:
			t->US_IDTTX = value;
			break;
		case A_USART_US_IDTRX:
			t->US_IDTRX = value;
			break;
		case A_USART_US_ICDIFF:
			t->US_ICDIFF = value;
			break;
		case A_USART_US_WPMR:
			t->US_WPMR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAME70Q21B USART write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsame70q21b_usart_ops = {
	.read = atsame70q21b_usart_read,
	.write = atsame70q21b_usart_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsame70q21b_usart_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAME70Q21BUSART *t = ATSAME70Q21B_USART(obj);
	memory_region_init_io(&(t->iomem), obj, &atsame70q21b_usart_ops, t, "atsame70q21b-usart", ATSAME70Q21B_USART_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void atsame70q21b_usart_realize(DeviceState *dev, Error **errp) {
	ATSAME70Q21BUSART *t = ATSAME70Q21B_USART(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), atsame70q21b_usart_can_receive, atsame70q21b_usart_receive,
		NULL, NULL, t, NULL, true);

}

static void atsame70q21b_usart_reset_enter(Object *obj, ResetType type) {
	ATSAME70Q21BUSART *t = ATSAME70Q21B_USART(obj);
	atsame70q21b_usart_register_reset(t);
}

static const VMStateDescription atsame70q21b_usart_vmstate = {
	.name = "atsame70q21b-usart",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(US_CR_USART_MODE, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_CR_SPI_MODE, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_CR_LIN_MODE, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_MR_USART_MODE, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_MR_SPI_MODE, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_IER_USART_MODE, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_IER_SPI_MODE, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_IER_LIN_MODE, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_IER_LON_MODE, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_IDR_USART_MODE, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_IDR_SPI_MODE, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_IDR_LIN_MODE, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_IDR_LON_MODE, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_IMR_USART_MODE, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_IMR_SPI_MODE, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_IMR_LIN_MODE, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_IMR_LON_MODE, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_CSR_USART_MODE, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_CSR_SPI_MODE, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_CSR_LIN_MODE, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_CSR_LON_MODE, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_RHR, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_THR, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_BRGR, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_RTOR, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_TTGR_USART_MODE, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_TTGR_LON_MODE, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_FIDI_USART_MODE, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_FIDI_LON_MODE, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_NER, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_IF, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_MAN, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_LINMR, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_LINIR, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_LINBRR, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_LONMR, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_LONPR, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_LONDL, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_LONL2HDR, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_LONBL, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_LONB1TX, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_LONB1RX, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_LONPRIO, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_IDTTX, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_IDTRX, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_ICDIFF, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_WPMR, ATSAME70Q21BUSART),
		VMSTATE_UINT32(US_WPSR, ATSAME70Q21BUSART),

		VMSTATE_END_OF_LIST()
	}
};

static Property atsame70q21b_usart_properties[] = {
	DEFINE_PROP_CHR("chardev", ATSAME70Q21BUSART, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void atsame70q21b_usart_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsame70q21b_usart_vmstate;
	dc->realize = atsame70q21b_usart_realize;
	rc->phases.enter = atsame70q21b_usart_reset_enter;
	device_class_set_props(dc, atsame70q21b_usart_properties);

}

static const TypeInfo atsame70q21b_usart_info = {
	.name = TYPE_ATSAME70Q21B_USART,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAME70Q21BUSART),
	.instance_init = atsame70q21b_usart_init,
	.class_init = atsame70q21b_usart_class_init,
};

static void atsame70q21b_usart_register_types(void) {
	type_register_static(&atsame70q21b_usart_info);
}

type_init(atsame70q21b_usart_register_types);

#define TYPE_ATSAME70Q21B_PMC "atsame70q21b-pmc"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAME70Q21BPMC, ATSAME70Q21B_PMC)

struct ATSAME70Q21BPMC {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t SCER;
	uint32_t SCDR;
	uint32_t SCSR;
	uint32_t PCER0;
	uint32_t PCDR0;
	uint32_t PCSR0;
	uint32_t CKGR_UCKR;
	uint32_t CKGR_MOR;
	uint32_t CKGR_MCFR;
	uint32_t CKGR_PLLAR;
	uint32_t MCKR;
	uint32_t USB;
	uint32_t IER;
	uint32_t IDR;
	uint32_t SR;
	uint32_t IMR;
	uint32_t FSMR;
	uint32_t FSPR;
	uint32_t FOCR;
	uint32_t WPMR;
	uint32_t WPSR;
	uint32_t PCER1;
	uint32_t PCDR1;
	uint32_t PCSR1;
	uint32_t PCR;
	uint32_t OCR;
	uint32_t SLPWK_ER0;
	uint32_t SLPWK_DR0;
	uint32_t SLPWK_SR0;
	uint32_t SLPWK_ASR0;
	uint32_t PMMR;
	uint32_t SLPWK_ER1;
	uint32_t SLPWK_DR1;
	uint32_t SLPWK_SR1;
	uint32_t SLPWK_ASR1;
	uint32_t SLPWK_AIPR;
	uint32_t PCK0;
	uint32_t PCK1;
	uint32_t PCK2;
	uint32_t PCK3;
	uint32_t PCK4;
	uint32_t PCK5;
	uint32_t PCK6;
	uint32_t PCK7;
	/* base */
	uint32_t base;

};
#define ATSAME70Q21B_PMC_SIZE				0x148

REG32(PMC_SCER, 0x0)
	FIELD(PMC_SCER, USBCLK, 5, 1)
	FIELD(PMC_SCER, PCK0, 8, 1)
	FIELD(PMC_SCER, PCK1, 9, 1)
	FIELD(PMC_SCER, PCK2, 10, 1)
	FIELD(PMC_SCER, PCK3, 11, 1)
	FIELD(PMC_SCER, PCK4, 12, 1)
	FIELD(PMC_SCER, PCK5, 13, 1)
	FIELD(PMC_SCER, PCK6, 14, 1)
	FIELD(PMC_SCER, PCK7, 15, 1)
REG32(PMC_SCDR, 0x4)
	FIELD(PMC_SCDR, USBCLK, 5, 1)
	FIELD(PMC_SCDR, PCK0, 8, 1)
	FIELD(PMC_SCDR, PCK1, 9, 1)
	FIELD(PMC_SCDR, PCK2, 10, 1)
	FIELD(PMC_SCDR, PCK3, 11, 1)
	FIELD(PMC_SCDR, PCK4, 12, 1)
	FIELD(PMC_SCDR, PCK5, 13, 1)
	FIELD(PMC_SCDR, PCK6, 14, 1)
	FIELD(PMC_SCDR, PCK7, 15, 1)
REG32(PMC_SCSR, 0x8)
	FIELD(PMC_SCSR, HCLKS, 0, 1)
	FIELD(PMC_SCSR, USBCLK, 5, 1)
	FIELD(PMC_SCSR, PCK0, 8, 1)
	FIELD(PMC_SCSR, PCK1, 9, 1)
	FIELD(PMC_SCSR, PCK2, 10, 1)
	FIELD(PMC_SCSR, PCK3, 11, 1)
	FIELD(PMC_SCSR, PCK4, 12, 1)
	FIELD(PMC_SCSR, PCK5, 13, 1)
	FIELD(PMC_SCSR, PCK6, 14, 1)
	FIELD(PMC_SCSR, PCK7, 15, 1)
REG32(PMC_PCER0, 0x10)
	FIELD(PMC_PCER0, PID7, 7, 1)
	FIELD(PMC_PCER0, PID8, 8, 1)
	FIELD(PMC_PCER0, PID9, 9, 1)
	FIELD(PMC_PCER0, PID10, 10, 1)
	FIELD(PMC_PCER0, PID11, 11, 1)
	FIELD(PMC_PCER0, PID12, 12, 1)
	FIELD(PMC_PCER0, PID13, 13, 1)
	FIELD(PMC_PCER0, PID14, 14, 1)
	FIELD(PMC_PCER0, PID15, 15, 1)
	FIELD(PMC_PCER0, PID16, 16, 1)
	FIELD(PMC_PCER0, PID17, 17, 1)
	FIELD(PMC_PCER0, PID18, 18, 1)
	FIELD(PMC_PCER0, PID19, 19, 1)
	FIELD(PMC_PCER0, PID20, 20, 1)
	FIELD(PMC_PCER0, PID21, 21, 1)
	FIELD(PMC_PCER0, PID22, 22, 1)
	FIELD(PMC_PCER0, PID23, 23, 1)
	FIELD(PMC_PCER0, PID24, 24, 1)
	FIELD(PMC_PCER0, PID25, 25, 1)
	FIELD(PMC_PCER0, PID26, 26, 1)
	FIELD(PMC_PCER0, PID27, 27, 1)
	FIELD(PMC_PCER0, PID28, 28, 1)
	FIELD(PMC_PCER0, PID29, 29, 1)
	FIELD(PMC_PCER0, PID30, 30, 1)
	FIELD(PMC_PCER0, PID31, 31, 1)
REG32(PMC_PCDR0, 0x14)
	FIELD(PMC_PCDR0, PID7, 7, 1)
	FIELD(PMC_PCDR0, PID8, 8, 1)
	FIELD(PMC_PCDR0, PID9, 9, 1)
	FIELD(PMC_PCDR0, PID10, 10, 1)
	FIELD(PMC_PCDR0, PID11, 11, 1)
	FIELD(PMC_PCDR0, PID12, 12, 1)
	FIELD(PMC_PCDR0, PID13, 13, 1)
	FIELD(PMC_PCDR0, PID14, 14, 1)
	FIELD(PMC_PCDR0, PID15, 15, 1)
	FIELD(PMC_PCDR0, PID16, 16, 1)
	FIELD(PMC_PCDR0, PID17, 17, 1)
	FIELD(PMC_PCDR0, PID18, 18, 1)
	FIELD(PMC_PCDR0, PID19, 19, 1)
	FIELD(PMC_PCDR0, PID20, 20, 1)
	FIELD(PMC_PCDR0, PID21, 21, 1)
	FIELD(PMC_PCDR0, PID22, 22, 1)
	FIELD(PMC_PCDR0, PID23, 23, 1)
	FIELD(PMC_PCDR0, PID24, 24, 1)
	FIELD(PMC_PCDR0, PID25, 25, 1)
	FIELD(PMC_PCDR0, PID26, 26, 1)
	FIELD(PMC_PCDR0, PID27, 27, 1)
	FIELD(PMC_PCDR0, PID28, 28, 1)
	FIELD(PMC_PCDR0, PID29, 29, 1)
	FIELD(PMC_PCDR0, PID30, 30, 1)
	FIELD(PMC_PCDR0, PID31, 31, 1)
REG32(PMC_PCSR0, 0x18)
	FIELD(PMC_PCSR0, PID7, 7, 1)
	FIELD(PMC_PCSR0, PID8, 8, 1)
	FIELD(PMC_PCSR0, PID9, 9, 1)
	FIELD(PMC_PCSR0, PID10, 10, 1)
	FIELD(PMC_PCSR0, PID11, 11, 1)
	FIELD(PMC_PCSR0, PID12, 12, 1)
	FIELD(PMC_PCSR0, PID13, 13, 1)
	FIELD(PMC_PCSR0, PID14, 14, 1)
	FIELD(PMC_PCSR0, PID15, 15, 1)
	FIELD(PMC_PCSR0, PID16, 16, 1)
	FIELD(PMC_PCSR0, PID17, 17, 1)
	FIELD(PMC_PCSR0, PID18, 18, 1)
	FIELD(PMC_PCSR0, PID19, 19, 1)
	FIELD(PMC_PCSR0, PID20, 20, 1)
	FIELD(PMC_PCSR0, PID21, 21, 1)
	FIELD(PMC_PCSR0, PID22, 22, 1)
	FIELD(PMC_PCSR0, PID23, 23, 1)
	FIELD(PMC_PCSR0, PID24, 24, 1)
	FIELD(PMC_PCSR0, PID25, 25, 1)
	FIELD(PMC_PCSR0, PID26, 26, 1)
	FIELD(PMC_PCSR0, PID27, 27, 1)
	FIELD(PMC_PCSR0, PID28, 28, 1)
	FIELD(PMC_PCSR0, PID29, 29, 1)
	FIELD(PMC_PCSR0, PID30, 30, 1)
	FIELD(PMC_PCSR0, PID31, 31, 1)
REG32(PMC_CKGR_UCKR, 0x1c)
	FIELD(PMC_CKGR_UCKR, UPLLEN, 16, 1)
	FIELD(PMC_CKGR_UCKR, UPLLCOUNT, 20, 4)
REG32(PMC_CKGR_MOR, 0x20)
	FIELD(PMC_CKGR_MOR, MOSCXTEN, 0, 1)
	FIELD(PMC_CKGR_MOR, MOSCXTBY, 1, 1)
	FIELD(PMC_CKGR_MOR, WAITMODE, 2, 1)
	FIELD(PMC_CKGR_MOR, MOSCRCEN, 3, 1)
	FIELD(PMC_CKGR_MOR, MOSCRCF, 4, 3)
	FIELD(PMC_CKGR_MOR, MOSCXTST, 8, 8)
	FIELD(PMC_CKGR_MOR, KEY, 16, 8)
	FIELD(PMC_CKGR_MOR, MOSCSEL, 24, 1)
	FIELD(PMC_CKGR_MOR, CFDEN, 25, 1)
	FIELD(PMC_CKGR_MOR, XT32KFME, 26, 1)
REG32(PMC_CKGR_MCFR, 0x24)
	FIELD(PMC_CKGR_MCFR, MAINF, 0, 16)
	FIELD(PMC_CKGR_MCFR, MAINFRDY, 16, 1)
	FIELD(PMC_CKGR_MCFR, RCMEAS, 20, 1)
	FIELD(PMC_CKGR_MCFR, CCSS, 24, 1)
REG32(PMC_CKGR_PLLAR, 0x28)
	FIELD(PMC_CKGR_PLLAR, DIVA, 0, 8)
	FIELD(PMC_CKGR_PLLAR, PLLACOUNT, 8, 6)
	FIELD(PMC_CKGR_PLLAR, MULA, 16, 11)
	FIELD(PMC_CKGR_PLLAR, ONE, 29, 1)
REG32(PMC_MCKR, 0x30)
	FIELD(PMC_MCKR, CSS, 0, 2)
	FIELD(PMC_MCKR, PRES, 4, 3)
	FIELD(PMC_MCKR, MDIV, 8, 2)
	FIELD(PMC_MCKR, UPLLDIV2, 13, 1)
REG32(PMC_USB, 0x38)
	FIELD(PMC_USB, USBS, 0, 1)
	FIELD(PMC_USB, USBDIV, 8, 4)
REG32(PMC_IER, 0x60)
	FIELD(PMC_IER, MOSCXTS, 0, 1)
	FIELD(PMC_IER, LOCKA, 1, 1)
	FIELD(PMC_IER, MCKRDY, 3, 1)
	FIELD(PMC_IER, LOCKU, 6, 1)
	FIELD(PMC_IER, PCKRDY0, 8, 1)
	FIELD(PMC_IER, PCKRDY1, 9, 1)
	FIELD(PMC_IER, PCKRDY2, 10, 1)
	FIELD(PMC_IER, PCKRDY3, 11, 1)
	FIELD(PMC_IER, PCKRDY4, 12, 1)
	FIELD(PMC_IER, PCKRDY5, 13, 1)
	FIELD(PMC_IER, PCKRDY6, 14, 1)
	FIELD(PMC_IER, PCKRDY7, 15, 1)
	FIELD(PMC_IER, MOSCSELS, 16, 1)
	FIELD(PMC_IER, MOSCRCS, 17, 1)
	FIELD(PMC_IER, CFDEV, 18, 1)
	FIELD(PMC_IER, XT32KERR, 21, 1)
REG32(PMC_IDR, 0x64)
	FIELD(PMC_IDR, MOSCXTS, 0, 1)
	FIELD(PMC_IDR, LOCKA, 1, 1)
	FIELD(PMC_IDR, MCKRDY, 3, 1)
	FIELD(PMC_IDR, LOCKU, 6, 1)
	FIELD(PMC_IDR, PCKRDY0, 8, 1)
	FIELD(PMC_IDR, PCKRDY1, 9, 1)
	FIELD(PMC_IDR, PCKRDY2, 10, 1)
	FIELD(PMC_IDR, PCKRDY3, 11, 1)
	FIELD(PMC_IDR, PCKRDY4, 12, 1)
	FIELD(PMC_IDR, PCKRDY5, 13, 1)
	FIELD(PMC_IDR, PCKRDY6, 14, 1)
	FIELD(PMC_IDR, PCKRDY7, 15, 1)
	FIELD(PMC_IDR, MOSCSELS, 16, 1)
	FIELD(PMC_IDR, MOSCRCS, 17, 1)
	FIELD(PMC_IDR, CFDEV, 18, 1)
	FIELD(PMC_IDR, XT32KERR, 21, 1)
REG32(PMC_SR, 0x68)
	FIELD(PMC_SR, MOSCXTS, 0, 1)
	FIELD(PMC_SR, LOCKA, 1, 1)
	FIELD(PMC_SR, MCKRDY, 3, 1)
	FIELD(PMC_SR, LOCKU, 6, 1)
	FIELD(PMC_SR, OSCSELS, 7, 1)
	FIELD(PMC_SR, PCKRDY0, 8, 1)
	FIELD(PMC_SR, PCKRDY1, 9, 1)
	FIELD(PMC_SR, PCKRDY2, 10, 1)
	FIELD(PMC_SR, PCKRDY3, 11, 1)
	FIELD(PMC_SR, PCKRDY4, 12, 1)
	FIELD(PMC_SR, PCKRDY5, 13, 1)
	FIELD(PMC_SR, PCKRDY6, 14, 1)
	FIELD(PMC_SR, PCKRDY7, 15, 1)
	FIELD(PMC_SR, MOSCSELS, 16, 1)
	FIELD(PMC_SR, MOSCRCS, 17, 1)
	FIELD(PMC_SR, CFDEV, 18, 1)
	FIELD(PMC_SR, CFDS, 19, 1)
	FIELD(PMC_SR, FOS, 20, 1)
	FIELD(PMC_SR, XT32KERR, 21, 1)
REG32(PMC_IMR, 0x6c)
	FIELD(PMC_IMR, MOSCXTS, 0, 1)
	FIELD(PMC_IMR, LOCKA, 1, 1)
	FIELD(PMC_IMR, MCKRDY, 3, 1)
	FIELD(PMC_IMR, LOCKU, 6, 1)
	FIELD(PMC_IMR, PCKRDY0, 8, 1)
	FIELD(PMC_IMR, PCKRDY1, 9, 1)
	FIELD(PMC_IMR, PCKRDY2, 10, 1)
	FIELD(PMC_IMR, PCKRDY3, 11, 1)
	FIELD(PMC_IMR, PCKRDY4, 12, 1)
	FIELD(PMC_IMR, PCKRDY5, 13, 1)
	FIELD(PMC_IMR, PCKRDY6, 14, 1)
	FIELD(PMC_IMR, PCKRDY7, 15, 1)
	FIELD(PMC_IMR, MOSCSELS, 16, 1)
	FIELD(PMC_IMR, MOSCRCS, 17, 1)
	FIELD(PMC_IMR, CFDEV, 18, 1)
	FIELD(PMC_IMR, XT32KERR, 21, 1)
REG32(PMC_FSMR, 0x70)
	FIELD(PMC_FSMR, FSTT0, 0, 1)
	FIELD(PMC_FSMR, FSTT1, 1, 1)
	FIELD(PMC_FSMR, FSTT2, 2, 1)
	FIELD(PMC_FSMR, FSTT3, 3, 1)
	FIELD(PMC_FSMR, FSTT4, 4, 1)
	FIELD(PMC_FSMR, FSTT5, 5, 1)
	FIELD(PMC_FSMR, FSTT6, 6, 1)
	FIELD(PMC_FSMR, FSTT7, 7, 1)
	FIELD(PMC_FSMR, FSTT8, 8, 1)
	FIELD(PMC_FSMR, FSTT9, 9, 1)
	FIELD(PMC_FSMR, FSTT10, 10, 1)
	FIELD(PMC_FSMR, FSTT11, 11, 1)
	FIELD(PMC_FSMR, FSTT12, 12, 1)
	FIELD(PMC_FSMR, FSTT13, 13, 1)
	FIELD(PMC_FSMR, FSTT14, 14, 1)
	FIELD(PMC_FSMR, FSTT15, 15, 1)
	FIELD(PMC_FSMR, RTTAL, 16, 1)
	FIELD(PMC_FSMR, RTCAL, 17, 1)
	FIELD(PMC_FSMR, USBAL, 18, 1)
	FIELD(PMC_FSMR, LPM, 20, 1)
	FIELD(PMC_FSMR, FLPM, 21, 2)
	FIELD(PMC_FSMR, FFLPM, 23, 1)
REG32(PMC_FSPR, 0x74)
	FIELD(PMC_FSPR, FSTP0, 0, 1)
	FIELD(PMC_FSPR, FSTP1, 1, 1)
	FIELD(PMC_FSPR, FSTP2, 2, 1)
	FIELD(PMC_FSPR, FSTP3, 3, 1)
	FIELD(PMC_FSPR, FSTP4, 4, 1)
	FIELD(PMC_FSPR, FSTP5, 5, 1)
	FIELD(PMC_FSPR, FSTP6, 6, 1)
	FIELD(PMC_FSPR, FSTP7, 7, 1)
	FIELD(PMC_FSPR, FSTP8, 8, 1)
	FIELD(PMC_FSPR, FSTP9, 9, 1)
	FIELD(PMC_FSPR, FSTP10, 10, 1)
	FIELD(PMC_FSPR, FSTP11, 11, 1)
	FIELD(PMC_FSPR, FSTP12, 12, 1)
	FIELD(PMC_FSPR, FSTP13, 13, 1)
	FIELD(PMC_FSPR, FSTP14, 14, 1)
	FIELD(PMC_FSPR, FSTP15, 15, 1)
REG32(PMC_FOCR, 0x78)
	FIELD(PMC_FOCR, FOCLR, 0, 1)
REG32(PMC_WPMR, 0xe4)
	FIELD(PMC_WPMR, WPEN, 0, 1)
	FIELD(PMC_WPMR, WPKEY, 8, 24)
REG32(PMC_WPSR, 0xe8)
	FIELD(PMC_WPSR, WPVS, 0, 1)
	FIELD(PMC_WPSR, WPVSRC, 8, 16)
REG32(PMC_PCER1, 0x100)
	FIELD(PMC_PCER1, PID32, 0, 1)
	FIELD(PMC_PCER1, PID33, 1, 1)
	FIELD(PMC_PCER1, PID34, 2, 1)
	FIELD(PMC_PCER1, PID35, 3, 1)
	FIELD(PMC_PCER1, PID37, 5, 1)
	FIELD(PMC_PCER1, PID39, 7, 1)
	FIELD(PMC_PCER1, PID40, 8, 1)
	FIELD(PMC_PCER1, PID41, 9, 1)
	FIELD(PMC_PCER1, PID42, 10, 1)
	FIELD(PMC_PCER1, PID43, 11, 1)
	FIELD(PMC_PCER1, PID44, 12, 1)
	FIELD(PMC_PCER1, PID45, 13, 1)
	FIELD(PMC_PCER1, PID46, 14, 1)
	FIELD(PMC_PCER1, PID47, 15, 1)
	FIELD(PMC_PCER1, PID48, 16, 1)
	FIELD(PMC_PCER1, PID49, 17, 1)
	FIELD(PMC_PCER1, PID50, 18, 1)
	FIELD(PMC_PCER1, PID51, 19, 1)
	FIELD(PMC_PCER1, PID52, 20, 1)
	FIELD(PMC_PCER1, PID56, 24, 1)
	FIELD(PMC_PCER1, PID57, 25, 1)
	FIELD(PMC_PCER1, PID58, 26, 1)
	FIELD(PMC_PCER1, PID59, 27, 1)
	FIELD(PMC_PCER1, PID60, 28, 1)
REG32(PMC_PCDR1, 0x104)
	FIELD(PMC_PCDR1, PID32, 0, 1)
	FIELD(PMC_PCDR1, PID33, 1, 1)
	FIELD(PMC_PCDR1, PID34, 2, 1)
	FIELD(PMC_PCDR1, PID35, 3, 1)
	FIELD(PMC_PCDR1, PID37, 5, 1)
	FIELD(PMC_PCDR1, PID39, 7, 1)
	FIELD(PMC_PCDR1, PID40, 8, 1)
	FIELD(PMC_PCDR1, PID41, 9, 1)
	FIELD(PMC_PCDR1, PID42, 10, 1)
	FIELD(PMC_PCDR1, PID43, 11, 1)
	FIELD(PMC_PCDR1, PID44, 12, 1)
	FIELD(PMC_PCDR1, PID45, 13, 1)
	FIELD(PMC_PCDR1, PID46, 14, 1)
	FIELD(PMC_PCDR1, PID47, 15, 1)
	FIELD(PMC_PCDR1, PID48, 16, 1)
	FIELD(PMC_PCDR1, PID49, 17, 1)
	FIELD(PMC_PCDR1, PID50, 18, 1)
	FIELD(PMC_PCDR1, PID51, 19, 1)
	FIELD(PMC_PCDR1, PID52, 20, 1)
	FIELD(PMC_PCDR1, PID56, 24, 1)
	FIELD(PMC_PCDR1, PID57, 25, 1)
	FIELD(PMC_PCDR1, PID58, 26, 1)
	FIELD(PMC_PCDR1, PID59, 27, 1)
	FIELD(PMC_PCDR1, PID60, 28, 1)
REG32(PMC_PCSR1, 0x108)
	FIELD(PMC_PCSR1, PID32, 0, 1)
	FIELD(PMC_PCSR1, PID33, 1, 1)
	FIELD(PMC_PCSR1, PID34, 2, 1)
	FIELD(PMC_PCSR1, PID35, 3, 1)
	FIELD(PMC_PCSR1, PID37, 5, 1)
	FIELD(PMC_PCSR1, PID39, 7, 1)
	FIELD(PMC_PCSR1, PID40, 8, 1)
	FIELD(PMC_PCSR1, PID41, 9, 1)
	FIELD(PMC_PCSR1, PID42, 10, 1)
	FIELD(PMC_PCSR1, PID43, 11, 1)
	FIELD(PMC_PCSR1, PID44, 12, 1)
	FIELD(PMC_PCSR1, PID45, 13, 1)
	FIELD(PMC_PCSR1, PID46, 14, 1)
	FIELD(PMC_PCSR1, PID47, 15, 1)
	FIELD(PMC_PCSR1, PID48, 16, 1)
	FIELD(PMC_PCSR1, PID49, 17, 1)
	FIELD(PMC_PCSR1, PID50, 18, 1)
	FIELD(PMC_PCSR1, PID51, 19, 1)
	FIELD(PMC_PCSR1, PID52, 20, 1)
	FIELD(PMC_PCSR1, PID56, 24, 1)
	FIELD(PMC_PCSR1, PID57, 25, 1)
	FIELD(PMC_PCSR1, PID58, 26, 1)
	FIELD(PMC_PCSR1, PID59, 27, 1)
	FIELD(PMC_PCSR1, PID60, 28, 1)
REG32(PMC_PCR, 0x10c)
	FIELD(PMC_PCR, PID, 0, 7)
	FIELD(PMC_PCR, GCLKCSS, 8, 3)
	FIELD(PMC_PCR, CMD, 12, 1)
	FIELD(PMC_PCR, GCLKDIV, 20, 8)
	FIELD(PMC_PCR, EN, 28, 1)
	FIELD(PMC_PCR, GCLKEN, 29, 1)
REG32(PMC_OCR, 0x110)
	FIELD(PMC_OCR, CAL4, 0, 7)
	FIELD(PMC_OCR, SEL4, 7, 1)
	FIELD(PMC_OCR, CAL8, 8, 7)
	FIELD(PMC_OCR, SEL8, 15, 1)
	FIELD(PMC_OCR, CAL12, 16, 7)
	FIELD(PMC_OCR, SEL12, 23, 1)
REG32(PMC_SLPWK_ER0, 0x114)
	FIELD(PMC_SLPWK_ER0, PID7, 7, 1)
	FIELD(PMC_SLPWK_ER0, PID8, 8, 1)
	FIELD(PMC_SLPWK_ER0, PID9, 9, 1)
	FIELD(PMC_SLPWK_ER0, PID10, 10, 1)
	FIELD(PMC_SLPWK_ER0, PID11, 11, 1)
	FIELD(PMC_SLPWK_ER0, PID12, 12, 1)
	FIELD(PMC_SLPWK_ER0, PID13, 13, 1)
	FIELD(PMC_SLPWK_ER0, PID14, 14, 1)
	FIELD(PMC_SLPWK_ER0, PID15, 15, 1)
	FIELD(PMC_SLPWK_ER0, PID16, 16, 1)
	FIELD(PMC_SLPWK_ER0, PID17, 17, 1)
	FIELD(PMC_SLPWK_ER0, PID18, 18, 1)
	FIELD(PMC_SLPWK_ER0, PID19, 19, 1)
	FIELD(PMC_SLPWK_ER0, PID20, 20, 1)
	FIELD(PMC_SLPWK_ER0, PID21, 21, 1)
	FIELD(PMC_SLPWK_ER0, PID22, 22, 1)
	FIELD(PMC_SLPWK_ER0, PID23, 23, 1)
	FIELD(PMC_SLPWK_ER0, PID24, 24, 1)
	FIELD(PMC_SLPWK_ER0, PID25, 25, 1)
	FIELD(PMC_SLPWK_ER0, PID26, 26, 1)
	FIELD(PMC_SLPWK_ER0, PID27, 27, 1)
	FIELD(PMC_SLPWK_ER0, PID28, 28, 1)
	FIELD(PMC_SLPWK_ER0, PID29, 29, 1)
	FIELD(PMC_SLPWK_ER0, PID30, 30, 1)
	FIELD(PMC_SLPWK_ER0, PID31, 31, 1)
REG32(PMC_SLPWK_DR0, 0x118)
	FIELD(PMC_SLPWK_DR0, PID7, 7, 1)
	FIELD(PMC_SLPWK_DR0, PID8, 8, 1)
	FIELD(PMC_SLPWK_DR0, PID9, 9, 1)
	FIELD(PMC_SLPWK_DR0, PID10, 10, 1)
	FIELD(PMC_SLPWK_DR0, PID11, 11, 1)
	FIELD(PMC_SLPWK_DR0, PID12, 12, 1)
	FIELD(PMC_SLPWK_DR0, PID13, 13, 1)
	FIELD(PMC_SLPWK_DR0, PID14, 14, 1)
	FIELD(PMC_SLPWK_DR0, PID15, 15, 1)
	FIELD(PMC_SLPWK_DR0, PID16, 16, 1)
	FIELD(PMC_SLPWK_DR0, PID17, 17, 1)
	FIELD(PMC_SLPWK_DR0, PID18, 18, 1)
	FIELD(PMC_SLPWK_DR0, PID19, 19, 1)
	FIELD(PMC_SLPWK_DR0, PID20, 20, 1)
	FIELD(PMC_SLPWK_DR0, PID21, 21, 1)
	FIELD(PMC_SLPWK_DR0, PID22, 22, 1)
	FIELD(PMC_SLPWK_DR0, PID23, 23, 1)
	FIELD(PMC_SLPWK_DR0, PID24, 24, 1)
	FIELD(PMC_SLPWK_DR0, PID25, 25, 1)
	FIELD(PMC_SLPWK_DR0, PID26, 26, 1)
	FIELD(PMC_SLPWK_DR0, PID27, 27, 1)
	FIELD(PMC_SLPWK_DR0, PID28, 28, 1)
	FIELD(PMC_SLPWK_DR0, PID29, 29, 1)
	FIELD(PMC_SLPWK_DR0, PID30, 30, 1)
	FIELD(PMC_SLPWK_DR0, PID31, 31, 1)
REG32(PMC_SLPWK_SR0, 0x11c)
	FIELD(PMC_SLPWK_SR0, PID7, 7, 1)
	FIELD(PMC_SLPWK_SR0, PID8, 8, 1)
	FIELD(PMC_SLPWK_SR0, PID9, 9, 1)
	FIELD(PMC_SLPWK_SR0, PID10, 10, 1)
	FIELD(PMC_SLPWK_SR0, PID11, 11, 1)
	FIELD(PMC_SLPWK_SR0, PID12, 12, 1)
	FIELD(PMC_SLPWK_SR0, PID13, 13, 1)
	FIELD(PMC_SLPWK_SR0, PID14, 14, 1)
	FIELD(PMC_SLPWK_SR0, PID15, 15, 1)
	FIELD(PMC_SLPWK_SR0, PID16, 16, 1)
	FIELD(PMC_SLPWK_SR0, PID17, 17, 1)
	FIELD(PMC_SLPWK_SR0, PID18, 18, 1)
	FIELD(PMC_SLPWK_SR0, PID19, 19, 1)
	FIELD(PMC_SLPWK_SR0, PID20, 20, 1)
	FIELD(PMC_SLPWK_SR0, PID21, 21, 1)
	FIELD(PMC_SLPWK_SR0, PID22, 22, 1)
	FIELD(PMC_SLPWK_SR0, PID23, 23, 1)
	FIELD(PMC_SLPWK_SR0, PID24, 24, 1)
	FIELD(PMC_SLPWK_SR0, PID25, 25, 1)
	FIELD(PMC_SLPWK_SR0, PID26, 26, 1)
	FIELD(PMC_SLPWK_SR0, PID27, 27, 1)
	FIELD(PMC_SLPWK_SR0, PID28, 28, 1)
	FIELD(PMC_SLPWK_SR0, PID29, 29, 1)
	FIELD(PMC_SLPWK_SR0, PID30, 30, 1)
	FIELD(PMC_SLPWK_SR0, PID31, 31, 1)
REG32(PMC_SLPWK_ASR0, 0x120)
	FIELD(PMC_SLPWK_ASR0, PID7, 7, 1)
	FIELD(PMC_SLPWK_ASR0, PID8, 8, 1)
	FIELD(PMC_SLPWK_ASR0, PID9, 9, 1)
	FIELD(PMC_SLPWK_ASR0, PID10, 10, 1)
	FIELD(PMC_SLPWK_ASR0, PID11, 11, 1)
	FIELD(PMC_SLPWK_ASR0, PID12, 12, 1)
	FIELD(PMC_SLPWK_ASR0, PID13, 13, 1)
	FIELD(PMC_SLPWK_ASR0, PID14, 14, 1)
	FIELD(PMC_SLPWK_ASR0, PID15, 15, 1)
	FIELD(PMC_SLPWK_ASR0, PID16, 16, 1)
	FIELD(PMC_SLPWK_ASR0, PID17, 17, 1)
	FIELD(PMC_SLPWK_ASR0, PID18, 18, 1)
	FIELD(PMC_SLPWK_ASR0, PID19, 19, 1)
	FIELD(PMC_SLPWK_ASR0, PID20, 20, 1)
	FIELD(PMC_SLPWK_ASR0, PID21, 21, 1)
	FIELD(PMC_SLPWK_ASR0, PID22, 22, 1)
	FIELD(PMC_SLPWK_ASR0, PID23, 23, 1)
	FIELD(PMC_SLPWK_ASR0, PID24, 24, 1)
	FIELD(PMC_SLPWK_ASR0, PID25, 25, 1)
	FIELD(PMC_SLPWK_ASR0, PID26, 26, 1)
	FIELD(PMC_SLPWK_ASR0, PID27, 27, 1)
	FIELD(PMC_SLPWK_ASR0, PID28, 28, 1)
	FIELD(PMC_SLPWK_ASR0, PID29, 29, 1)
	FIELD(PMC_SLPWK_ASR0, PID30, 30, 1)
	FIELD(PMC_SLPWK_ASR0, PID31, 31, 1)
REG32(PMC_PMMR, 0x130)
	FIELD(PMC_PMMR, PLLA_MMAX, 0, 11)
REG32(PMC_SLPWK_ER1, 0x134)
	FIELD(PMC_SLPWK_ER1, PID32, 0, 1)
	FIELD(PMC_SLPWK_ER1, PID33, 1, 1)
	FIELD(PMC_SLPWK_ER1, PID34, 2, 1)
	FIELD(PMC_SLPWK_ER1, PID35, 3, 1)
	FIELD(PMC_SLPWK_ER1, PID37, 5, 1)
	FIELD(PMC_SLPWK_ER1, PID39, 7, 1)
	FIELD(PMC_SLPWK_ER1, PID40, 8, 1)
	FIELD(PMC_SLPWK_ER1, PID41, 9, 1)
	FIELD(PMC_SLPWK_ER1, PID42, 10, 1)
	FIELD(PMC_SLPWK_ER1, PID43, 11, 1)
	FIELD(PMC_SLPWK_ER1, PID44, 12, 1)
	FIELD(PMC_SLPWK_ER1, PID45, 13, 1)
	FIELD(PMC_SLPWK_ER1, PID46, 14, 1)
	FIELD(PMC_SLPWK_ER1, PID47, 15, 1)
	FIELD(PMC_SLPWK_ER1, PID48, 16, 1)
	FIELD(PMC_SLPWK_ER1, PID49, 17, 1)
	FIELD(PMC_SLPWK_ER1, PID50, 18, 1)
	FIELD(PMC_SLPWK_ER1, PID51, 19, 1)
	FIELD(PMC_SLPWK_ER1, PID52, 20, 1)
	FIELD(PMC_SLPWK_ER1, PID56, 24, 1)
	FIELD(PMC_SLPWK_ER1, PID57, 25, 1)
	FIELD(PMC_SLPWK_ER1, PID58, 26, 1)
	FIELD(PMC_SLPWK_ER1, PID59, 27, 1)
	FIELD(PMC_SLPWK_ER1, PID60, 28, 1)
REG32(PMC_SLPWK_DR1, 0x138)
	FIELD(PMC_SLPWK_DR1, PID32, 0, 1)
	FIELD(PMC_SLPWK_DR1, PID33, 1, 1)
	FIELD(PMC_SLPWK_DR1, PID34, 2, 1)
	FIELD(PMC_SLPWK_DR1, PID35, 3, 1)
	FIELD(PMC_SLPWK_DR1, PID37, 5, 1)
	FIELD(PMC_SLPWK_DR1, PID39, 7, 1)
	FIELD(PMC_SLPWK_DR1, PID40, 8, 1)
	FIELD(PMC_SLPWK_DR1, PID41, 9, 1)
	FIELD(PMC_SLPWK_DR1, PID42, 10, 1)
	FIELD(PMC_SLPWK_DR1, PID43, 11, 1)
	FIELD(PMC_SLPWK_DR1, PID44, 12, 1)
	FIELD(PMC_SLPWK_DR1, PID45, 13, 1)
	FIELD(PMC_SLPWK_DR1, PID46, 14, 1)
	FIELD(PMC_SLPWK_DR1, PID47, 15, 1)
	FIELD(PMC_SLPWK_DR1, PID48, 16, 1)
	FIELD(PMC_SLPWK_DR1, PID49, 17, 1)
	FIELD(PMC_SLPWK_DR1, PID50, 18, 1)
	FIELD(PMC_SLPWK_DR1, PID51, 19, 1)
	FIELD(PMC_SLPWK_DR1, PID52, 20, 1)
	FIELD(PMC_SLPWK_DR1, PID56, 24, 1)
	FIELD(PMC_SLPWK_DR1, PID57, 25, 1)
	FIELD(PMC_SLPWK_DR1, PID58, 26, 1)
	FIELD(PMC_SLPWK_DR1, PID59, 27, 1)
	FIELD(PMC_SLPWK_DR1, PID60, 28, 1)
REG32(PMC_SLPWK_SR1, 0x13c)
	FIELD(PMC_SLPWK_SR1, PID32, 0, 1)
	FIELD(PMC_SLPWK_SR1, PID33, 1, 1)
	FIELD(PMC_SLPWK_SR1, PID34, 2, 1)
	FIELD(PMC_SLPWK_SR1, PID35, 3, 1)
	FIELD(PMC_SLPWK_SR1, PID37, 5, 1)
	FIELD(PMC_SLPWK_SR1, PID39, 7, 1)
	FIELD(PMC_SLPWK_SR1, PID40, 8, 1)
	FIELD(PMC_SLPWK_SR1, PID41, 9, 1)
	FIELD(PMC_SLPWK_SR1, PID42, 10, 1)
	FIELD(PMC_SLPWK_SR1, PID43, 11, 1)
	FIELD(PMC_SLPWK_SR1, PID44, 12, 1)
	FIELD(PMC_SLPWK_SR1, PID45, 13, 1)
	FIELD(PMC_SLPWK_SR1, PID46, 14, 1)
	FIELD(PMC_SLPWK_SR1, PID47, 15, 1)
	FIELD(PMC_SLPWK_SR1, PID48, 16, 1)
	FIELD(PMC_SLPWK_SR1, PID49, 17, 1)
	FIELD(PMC_SLPWK_SR1, PID50, 18, 1)
	FIELD(PMC_SLPWK_SR1, PID51, 19, 1)
	FIELD(PMC_SLPWK_SR1, PID52, 20, 1)
	FIELD(PMC_SLPWK_SR1, PID56, 24, 1)
	FIELD(PMC_SLPWK_SR1, PID57, 25, 1)
	FIELD(PMC_SLPWK_SR1, PID58, 26, 1)
	FIELD(PMC_SLPWK_SR1, PID59, 27, 1)
	FIELD(PMC_SLPWK_SR1, PID60, 28, 1)
REG32(PMC_SLPWK_ASR1, 0x140)
	FIELD(PMC_SLPWK_ASR1, PID32, 0, 1)
	FIELD(PMC_SLPWK_ASR1, PID33, 1, 1)
	FIELD(PMC_SLPWK_ASR1, PID34, 2, 1)
	FIELD(PMC_SLPWK_ASR1, PID35, 3, 1)
	FIELD(PMC_SLPWK_ASR1, PID37, 5, 1)
	FIELD(PMC_SLPWK_ASR1, PID39, 7, 1)
	FIELD(PMC_SLPWK_ASR1, PID40, 8, 1)
	FIELD(PMC_SLPWK_ASR1, PID41, 9, 1)
	FIELD(PMC_SLPWK_ASR1, PID42, 10, 1)
	FIELD(PMC_SLPWK_ASR1, PID43, 11, 1)
	FIELD(PMC_SLPWK_ASR1, PID44, 12, 1)
	FIELD(PMC_SLPWK_ASR1, PID45, 13, 1)
	FIELD(PMC_SLPWK_ASR1, PID46, 14, 1)
	FIELD(PMC_SLPWK_ASR1, PID47, 15, 1)
	FIELD(PMC_SLPWK_ASR1, PID48, 16, 1)
	FIELD(PMC_SLPWK_ASR1, PID49, 17, 1)
	FIELD(PMC_SLPWK_ASR1, PID50, 18, 1)
	FIELD(PMC_SLPWK_ASR1, PID51, 19, 1)
	FIELD(PMC_SLPWK_ASR1, PID52, 20, 1)
	FIELD(PMC_SLPWK_ASR1, PID56, 24, 1)
	FIELD(PMC_SLPWK_ASR1, PID57, 25, 1)
	FIELD(PMC_SLPWK_ASR1, PID58, 26, 1)
	FIELD(PMC_SLPWK_ASR1, PID59, 27, 1)
	FIELD(PMC_SLPWK_ASR1, PID60, 28, 1)
REG32(PMC_SLPWK_AIPR, 0x144)
	FIELD(PMC_SLPWK_AIPR, AIP, 0, 1)
REG32(PMC_PCK0, 0x40)
	FIELD(PMC_PCK0, CSS, 0, 3)
	FIELD(PMC_PCK0, PRES, 4, 8)
REG32(PMC_PCK1, 0x44)
	FIELD(PMC_PCK1, CSS, 0, 3)
	FIELD(PMC_PCK1, PRES, 4, 8)
REG32(PMC_PCK2, 0x48)
	FIELD(PMC_PCK2, CSS, 0, 3)
	FIELD(PMC_PCK2, PRES, 4, 8)
REG32(PMC_PCK3, 0x4c)
	FIELD(PMC_PCK3, CSS, 0, 3)
	FIELD(PMC_PCK3, PRES, 4, 8)
REG32(PMC_PCK4, 0x50)
	FIELD(PMC_PCK4, CSS, 0, 3)
	FIELD(PMC_PCK4, PRES, 4, 8)
REG32(PMC_PCK5, 0x54)
	FIELD(PMC_PCK5, CSS, 0, 3)
	FIELD(PMC_PCK5, PRES, 4, 8)
REG32(PMC_PCK6, 0x58)
	FIELD(PMC_PCK6, CSS, 0, 3)
	FIELD(PMC_PCK6, PRES, 4, 8)
REG32(PMC_PCK7, 0x5c)
	FIELD(PMC_PCK7, CSS, 0, 3)
	FIELD(PMC_PCK7, PRES, 4, 8)


static void atsame70q21b_pmc_register_reset(ATSAME70Q21BPMC *t) {

}

static uint64_t atsame70q21b_pmc_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAME70Q21BPMC *t = ATSAME70Q21B_PMC(opaque);
	uint64_t ret;

	switch (offset) {
		case A_PMC_SCSR:
			ret = t->SCSR;
			break;
		case A_PMC_PCSR0:
			ret = t->PCSR0;
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
		case A_PMC_MCKR:
			ret = t->MCKR;
			break;
		case A_PMC_USB:
			ret = t->USB;
			break;
		case A_PMC_SR:
			ret = t->SR;
			break;
		case A_PMC_IMR:
			ret = t->IMR;
			break;
		case A_PMC_FSMR:
			ret = t->FSMR;
			break;
		case A_PMC_FSPR:
			ret = t->FSPR;
			break;
		case A_PMC_WPMR:
			ret = t->WPMR;
			break;
		case A_PMC_WPSR:
			ret = t->WPSR;
			break;
		case A_PMC_PCSR1:
			ret = t->PCSR1;
			break;
		case A_PMC_PCR:
			ret = t->PCR;
			break;
		case A_PMC_OCR:
			ret = t->OCR;
			break;
		case A_PMC_SLPWK_SR0:
			ret = t->SLPWK_SR0;
			break;
		case A_PMC_SLPWK_ASR0:
			ret = t->SLPWK_ASR0;
			break;
		case A_PMC_PMMR:
			ret = t->PMMR;
			break;
		case A_PMC_SLPWK_SR1:
			ret = t->SLPWK_SR1;
			break;
		case A_PMC_SLPWK_ASR1:
			ret = t->SLPWK_ASR1;
			break;
		case A_PMC_SLPWK_AIPR:
			ret = t->SLPWK_AIPR;
			break;
		case A_PMC_PCK0:
			ret = t->PCK0;
			break;
		case A_PMC_PCK1:
			ret = t->PCK1;
			break;
		case A_PMC_PCK2:
			ret = t->PCK2;
			break;
		case A_PMC_PCK3:
			ret = t->PCK3;
			break;
		case A_PMC_PCK4:
			ret = t->PCK4;
			break;
		case A_PMC_PCK5:
			ret = t->PCK5;
			break;
		case A_PMC_PCK6:
			ret = t->PCK6;
			break;
		case A_PMC_PCK7:
			ret = t->PCK7;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAME70Q21B PMC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsame70q21b_pmc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAME70Q21BPMC *t = ATSAME70Q21B_PMC(opaque);


	switch (offset) {
		case A_PMC_SCER ... A_PMC_SCER + 3:
			value = value << ((offset - A_PMC_SCER) << 3);
			offset = A_PMC_SCER;
			break;
		case A_PMC_SCDR ... A_PMC_SCDR + 3:
			value = value << ((offset - A_PMC_SCDR) << 3);
			offset = A_PMC_SCDR;
			break;
		case A_PMC_PCER0 ... A_PMC_PCER0 + 3:
			value = value << ((offset - A_PMC_PCER0) << 3);
			offset = A_PMC_PCER0;
			break;
		case A_PMC_PCDR0 ... A_PMC_PCDR0 + 3:
			value = value << ((offset - A_PMC_PCDR0) << 3);
			offset = A_PMC_PCDR0;
			break;
		case A_PMC_CKGR_UCKR ... A_PMC_CKGR_UCKR + 3:
			value = value << ((offset - A_PMC_CKGR_UCKR) << 3);
			offset = A_PMC_CKGR_UCKR;
			break;
		case A_PMC_CKGR_MOR ... A_PMC_CKGR_MOR + 3:
			value = value << ((offset - A_PMC_CKGR_MOR) << 3);
			offset = A_PMC_CKGR_MOR;
			break;
		case A_PMC_CKGR_MCFR ... A_PMC_CKGR_MCFR + 3:
			value = value << ((offset - A_PMC_CKGR_MCFR) << 3);
			offset = A_PMC_CKGR_MCFR;
			break;
		case A_PMC_CKGR_PLLAR ... A_PMC_CKGR_PLLAR + 3:
			value = value << ((offset - A_PMC_CKGR_PLLAR) << 3);
			offset = A_PMC_CKGR_PLLAR;
			break;
		case A_PMC_MCKR ... A_PMC_MCKR + 3:
			value = value << ((offset - A_PMC_MCKR) << 3);
			offset = A_PMC_MCKR;
			break;
		case A_PMC_USB ... A_PMC_USB + 3:
			value = value << ((offset - A_PMC_USB) << 3);
			offset = A_PMC_USB;
			break;
		case A_PMC_IER ... A_PMC_IER + 3:
			value = value << ((offset - A_PMC_IER) << 3);
			offset = A_PMC_IER;
			break;
		case A_PMC_IDR ... A_PMC_IDR + 3:
			value = value << ((offset - A_PMC_IDR) << 3);
			offset = A_PMC_IDR;
			break;
		case A_PMC_FSMR ... A_PMC_FSMR + 3:
			value = value << ((offset - A_PMC_FSMR) << 3);
			offset = A_PMC_FSMR;
			break;
		case A_PMC_FSPR ... A_PMC_FSPR + 3:
			value = value << ((offset - A_PMC_FSPR) << 3);
			offset = A_PMC_FSPR;
			break;
		case A_PMC_FOCR ... A_PMC_FOCR + 3:
			value = value << ((offset - A_PMC_FOCR) << 3);
			offset = A_PMC_FOCR;
			break;
		case A_PMC_WPMR ... A_PMC_WPMR + 3:
			value = value << ((offset - A_PMC_WPMR) << 3);
			offset = A_PMC_WPMR;
			break;
		case A_PMC_PCER1 ... A_PMC_PCER1 + 3:
			value = value << ((offset - A_PMC_PCER1) << 3);
			offset = A_PMC_PCER1;
			break;
		case A_PMC_PCDR1 ... A_PMC_PCDR1 + 3:
			value = value << ((offset - A_PMC_PCDR1) << 3);
			offset = A_PMC_PCDR1;
			break;
		case A_PMC_PCR ... A_PMC_PCR + 3:
			value = value << ((offset - A_PMC_PCR) << 3);
			offset = A_PMC_PCR;
			break;
		case A_PMC_OCR ... A_PMC_OCR + 3:
			value = value << ((offset - A_PMC_OCR) << 3);
			offset = A_PMC_OCR;
			break;
		case A_PMC_SLPWK_ER0 ... A_PMC_SLPWK_ER0 + 3:
			value = value << ((offset - A_PMC_SLPWK_ER0) << 3);
			offset = A_PMC_SLPWK_ER0;
			break;
		case A_PMC_SLPWK_DR0 ... A_PMC_SLPWK_DR0 + 3:
			value = value << ((offset - A_PMC_SLPWK_DR0) << 3);
			offset = A_PMC_SLPWK_DR0;
			break;
		case A_PMC_PMMR ... A_PMC_PMMR + 3:
			value = value << ((offset - A_PMC_PMMR) << 3);
			offset = A_PMC_PMMR;
			break;
		case A_PMC_SLPWK_ER1 ... A_PMC_SLPWK_ER1 + 3:
			value = value << ((offset - A_PMC_SLPWK_ER1) << 3);
			offset = A_PMC_SLPWK_ER1;
			break;
		case A_PMC_SLPWK_DR1 ... A_PMC_SLPWK_DR1 + 3:
			value = value << ((offset - A_PMC_SLPWK_DR1) << 3);
			offset = A_PMC_SLPWK_DR1;
			break;
		case A_PMC_PCK0 ... A_PMC_PCK0 + 3:
			value = value << ((offset - A_PMC_PCK0) << 3);
			offset = A_PMC_PCK0;
			break;
		case A_PMC_PCK1 ... A_PMC_PCK1 + 3:
			value = value << ((offset - A_PMC_PCK1) << 3);
			offset = A_PMC_PCK1;
			break;
		case A_PMC_PCK2 ... A_PMC_PCK2 + 3:
			value = value << ((offset - A_PMC_PCK2) << 3);
			offset = A_PMC_PCK2;
			break;
		case A_PMC_PCK3 ... A_PMC_PCK3 + 3:
			value = value << ((offset - A_PMC_PCK3) << 3);
			offset = A_PMC_PCK3;
			break;
		case A_PMC_PCK4 ... A_PMC_PCK4 + 3:
			value = value << ((offset - A_PMC_PCK4) << 3);
			offset = A_PMC_PCK4;
			break;
		case A_PMC_PCK5 ... A_PMC_PCK5 + 3:
			value = value << ((offset - A_PMC_PCK5) << 3);
			offset = A_PMC_PCK5;
			break;
		case A_PMC_PCK6 ... A_PMC_PCK6 + 3:
			value = value << ((offset - A_PMC_PCK6) << 3);
			offset = A_PMC_PCK6;
			break;
		case A_PMC_PCK7 ... A_PMC_PCK7 + 3:
			value = value << ((offset - A_PMC_PCK7) << 3);
			offset = A_PMC_PCK7;
			break;

		default: break;
	}

	switch (offset) {
		case A_PMC_SCER:
			t->SCER = value;
			break;
		case A_PMC_SCDR:
			t->SCDR = value;
			break;
		case A_PMC_PCER0:
			t->PCER0 = value;
			break;
		case A_PMC_PCDR0:
			t->PCDR0 = value;
			break;
		case A_PMC_CKGR_UCKR:
			t->CKGR_UCKR = value;
			if ((value == 0x310000)) {
				t->SR |= 0x40;
			}
			if (((!(value & 0x1)) && (!(value & 0x2)) && (!(value & 0x4)) && (!(value & 0x8)) && (!(value & 0x10)) && (!(value & 0x20)) && (!(value & 0x40)) && (!(value & 0x80)) && (!(value & 0x100)) && (!(value & 0x200)) && (!(value & 0x400)) && (!(value & 0x800)) && (!(value & 0x1000)) && (!(value & 0x2000)) && (!(value & 0x4000)) && (!(value & 0x8000)) && (value & 0x10000) && (!(value & 0x20000)) && (!(value & 0x40000)) && (!(value & 0x80000)) && (!(value & 0x1000000)) && (!(value & 0x2000000)) && (!(value & 0x4000000)) && (!(value & 0x8000000)) && (!(value & 0x10000000)) && (!(value & 0x20000000)) && (!(value & 0x40000000)) && (!(value & 0x80000000)))) {
				t->SR |= 0x40;
			}
			break;
		case A_PMC_CKGR_MOR:
			t->CKGR_MOR = value;
			if (((value & 0x1) && (!(value & 0x2)) && (value & 0x10000) && (value & 0x20000) && (value & 0x40000) && (value & 0x100000) && (value & 0x200000))) {
				t->SR |= 0x1;
			} else if (((value & 0x8) && (value & 0x10000) && (value & 0x20000) && (value & 0x40000) && (value & 0x100000) && (value & 0x200000))) {
				t->SR |= 0x20000;
			} else if (((value & 0x10000) && (value & 0x20000) && (value & 0x40000) && (value & 0x100000) && (value & 0x200000))) {
				t->SR |= 0x20000;
			}
			if ((value == 0x137ff09)) {
				t->SR |= 0x10000;
			}
			break;
		case A_PMC_CKGR_MCFR:
			t->CKGR_MCFR = value;
			break;
		case A_PMC_CKGR_PLLAR:
			t->CKGR_PLLAR = value;
			t->SR |= 0x2;
			break;
		case A_PMC_MCKR:
			t->MCKR = value;
			if (((!(value & 0x100)) && (!(value & 0x200)))) {
				t->SR |= 0x8;
			} else if (((value & 0x100) && (!(value & 0x200)))) {
				t->SR |= 0x8;
			} else if (((value & 0x100) && (value & 0x200))) {
				t->SR |= 0x8;
			} else if (((!(value & 0x100)) && (value & 0x200))) {
				t->SR |= 0x8;
			}
			if (((!(value & 0x1)) && (!(value & 0x2)))) {
				t->SR |= 0x8;
			} else if (((value & 0x1) && (value & 0x2))) {
				t->SR |= 0x8;
			} else if (((value & 0x1) && (!(value & 0x2)))) {
				t->SR |= 0x8;
			} else if (((!(value & 0x1)) && (value & 0x2))) {
				t->SR |= 0x8;
			}
			t->SR |= 0x8;
			break;
		case A_PMC_USB:
			t->USB = value;
			break;
		case A_PMC_IER:
			t->IER = value;
			break;
		case A_PMC_IDR:
			t->IDR = value;
			break;
		case A_PMC_FSMR:
			t->FSMR = value;
			break;
		case A_PMC_FSPR:
			t->FSPR = value;
			break;
		case A_PMC_FOCR:
			t->FOCR = value;
			break;
		case A_PMC_WPMR:
			t->WPMR = value;
			break;
		case A_PMC_PCER1:
			t->PCER1 = value;
			break;
		case A_PMC_PCDR1:
			t->PCDR1 = value;
			break;
		case A_PMC_PCR:
			t->PCR = value;
			break;
		case A_PMC_OCR:
			t->OCR = value;
			break;
		case A_PMC_SLPWK_ER0:
			t->SLPWK_ER0 = value;
			break;
		case A_PMC_SLPWK_DR0:
			t->SLPWK_DR0 = value;
			break;
		case A_PMC_PMMR:
			t->PMMR = value;
			break;
		case A_PMC_SLPWK_ER1:
			t->SLPWK_ER1 = value;
			break;
		case A_PMC_SLPWK_DR1:
			t->SLPWK_DR1 = value;
			break;
		case A_PMC_PCK0:
			t->PCK0 = value;
			if (((value & 0x1) && (value & 0x2))) {
				t->SR |= 0x100;
			} else if ((value & 0x1)) {
				t->SR |= 0x100;
			} else if ((value & 0x2)) {
				t->SR |= 0x100;
			}
			if ((value & 0x4)) {
				t->SR |= 0x100;
			}
			break;
		case A_PMC_PCK1:
			t->PCK1 = value;
			if (((value & 0x1) && (value & 0x2))) {
				t->SR |= 0x200;
			} else if ((value & 0x1)) {
				t->SR |= 0x200;
			} else if ((value & 0x2)) {
				t->SR |= 0x200;
			}
			if ((value & 0x4)) {
				t->SR |= 0x200;
			}
			break;
		case A_PMC_PCK2:
			t->PCK2 = value;
			if (((value & 0x1) && (value & 0x2))) {
				t->SR |= 0x400;
			} else if ((value & 0x1)) {
				t->SR |= 0x400;
			} else if ((value & 0x2)) {
				t->SR |= 0x400;
			}
			if ((value & 0x4)) {
				t->SR |= 0x400;
			}
			break;
		case A_PMC_PCK3:
			t->PCK3 = value;
			if (((value & 0x1) && (value & 0x2))) {
				t->SR |= 0x800;
			} else if ((value & 0x1)) {
				t->SR |= 0x800;
			} else if ((value & 0x2)) {
				t->SR |= 0x800;
			}
			if ((value & 0x4)) {
				t->SR |= 0x800;
			}
			break;
		case A_PMC_PCK4:
			t->PCK4 = value;
			if (((value & 0x1) && (value & 0x2))) {
				t->SR |= 0x1000;
			} else if ((value & 0x1)) {
				t->SR |= 0x1000;
			} else if ((value & 0x2)) {
				t->SR |= 0x1000;
			}
			if ((value & 0x4)) {
				t->SR |= 0x1000;
			}
			break;
		case A_PMC_PCK5:
			t->PCK5 = value;
			if (((value & 0x1) && (value & 0x2))) {
				t->SR |= 0x2000;
			} else if ((value & 0x1)) {
				t->SR |= 0x2000;
			} else if ((value & 0x2)) {
				t->SR |= 0x2000;
			}
			if ((value & 0x4)) {
				t->SR |= 0x2000;
			}
			break;
		case A_PMC_PCK6:
			t->PCK6 = value;
			if (((value & 0x1) && (value & 0x2))) {
				t->SR |= 0x4000;
			} else if ((value & 0x1)) {
				t->SR |= 0x4000;
			} else if ((value & 0x2)) {
				t->SR |= 0x4000;
			}
			if ((value & 0x4)) {
				t->SR |= 0x4000;
			}
			break;
		case A_PMC_PCK7:
			t->PCK7 = value;
			if (((value & 0x1) && (value & 0x2))) {
				t->SR |= 0x8000;
			} else if ((value & 0x1)) {
				t->SR |= 0x8000;
			} else if ((value & 0x2)) {
				t->SR |= 0x8000;
			}
			if ((value & 0x4)) {
				t->SR |= 0x8000;
			}
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAME70Q21B PMC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsame70q21b_pmc_ops = {
	.read = atsame70q21b_pmc_read,
	.write = atsame70q21b_pmc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsame70q21b_pmc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAME70Q21BPMC *t = ATSAME70Q21B_PMC(obj);
	memory_region_init_io(&(t->iomem), obj, &atsame70q21b_pmc_ops, t, "atsame70q21b-pmc", ATSAME70Q21B_PMC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void atsame70q21b_pmc_realize(DeviceState *dev, Error **errp) {
	return;

}

static void atsame70q21b_pmc_reset_enter(Object *obj, ResetType type) {
	ATSAME70Q21BPMC *t = ATSAME70Q21B_PMC(obj);
	atsame70q21b_pmc_register_reset(t);
}

static const VMStateDescription atsame70q21b_pmc_vmstate = {
	.name = "atsame70q21b-pmc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(SCER, ATSAME70Q21BPMC),
		VMSTATE_UINT32(SCDR, ATSAME70Q21BPMC),
		VMSTATE_UINT32(SCSR, ATSAME70Q21BPMC),
		VMSTATE_UINT32(PCER0, ATSAME70Q21BPMC),
		VMSTATE_UINT32(PCDR0, ATSAME70Q21BPMC),
		VMSTATE_UINT32(PCSR0, ATSAME70Q21BPMC),
		VMSTATE_UINT32(CKGR_UCKR, ATSAME70Q21BPMC),
		VMSTATE_UINT32(CKGR_MOR, ATSAME70Q21BPMC),
		VMSTATE_UINT32(CKGR_MCFR, ATSAME70Q21BPMC),
		VMSTATE_UINT32(CKGR_PLLAR, ATSAME70Q21BPMC),
		VMSTATE_UINT32(MCKR, ATSAME70Q21BPMC),
		VMSTATE_UINT32(USB, ATSAME70Q21BPMC),
		VMSTATE_UINT32(IER, ATSAME70Q21BPMC),
		VMSTATE_UINT32(IDR, ATSAME70Q21BPMC),
		VMSTATE_UINT32(SR, ATSAME70Q21BPMC),
		VMSTATE_UINT32(IMR, ATSAME70Q21BPMC),
		VMSTATE_UINT32(FSMR, ATSAME70Q21BPMC),
		VMSTATE_UINT32(FSPR, ATSAME70Q21BPMC),
		VMSTATE_UINT32(FOCR, ATSAME70Q21BPMC),
		VMSTATE_UINT32(WPMR, ATSAME70Q21BPMC),
		VMSTATE_UINT32(WPSR, ATSAME70Q21BPMC),
		VMSTATE_UINT32(PCER1, ATSAME70Q21BPMC),
		VMSTATE_UINT32(PCDR1, ATSAME70Q21BPMC),
		VMSTATE_UINT32(PCSR1, ATSAME70Q21BPMC),
		VMSTATE_UINT32(PCR, ATSAME70Q21BPMC),
		VMSTATE_UINT32(OCR, ATSAME70Q21BPMC),
		VMSTATE_UINT32(SLPWK_ER0, ATSAME70Q21BPMC),
		VMSTATE_UINT32(SLPWK_DR0, ATSAME70Q21BPMC),
		VMSTATE_UINT32(SLPWK_SR0, ATSAME70Q21BPMC),
		VMSTATE_UINT32(SLPWK_ASR0, ATSAME70Q21BPMC),
		VMSTATE_UINT32(PMMR, ATSAME70Q21BPMC),
		VMSTATE_UINT32(SLPWK_ER1, ATSAME70Q21BPMC),
		VMSTATE_UINT32(SLPWK_DR1, ATSAME70Q21BPMC),
		VMSTATE_UINT32(SLPWK_SR1, ATSAME70Q21BPMC),
		VMSTATE_UINT32(SLPWK_ASR1, ATSAME70Q21BPMC),
		VMSTATE_UINT32(SLPWK_AIPR, ATSAME70Q21BPMC),
		VMSTATE_UINT32(PCK0, ATSAME70Q21BPMC),
		VMSTATE_UINT32(PCK1, ATSAME70Q21BPMC),
		VMSTATE_UINT32(PCK2, ATSAME70Q21BPMC),
		VMSTATE_UINT32(PCK3, ATSAME70Q21BPMC),
		VMSTATE_UINT32(PCK4, ATSAME70Q21BPMC),
		VMSTATE_UINT32(PCK5, ATSAME70Q21BPMC),
		VMSTATE_UINT32(PCK6, ATSAME70Q21BPMC),
		VMSTATE_UINT32(PCK7, ATSAME70Q21BPMC),

		VMSTATE_END_OF_LIST()
	}
};

static void atsame70q21b_pmc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsame70q21b_pmc_vmstate;
	dc->realize = atsame70q21b_pmc_realize;
	rc->phases.enter = atsame70q21b_pmc_reset_enter;

}

static const TypeInfo atsame70q21b_pmc_info = {
	.name = TYPE_ATSAME70Q21B_PMC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAME70Q21BPMC),
	.instance_init = atsame70q21b_pmc_init,
	.class_init = atsame70q21b_pmc_class_init,
};

static void atsame70q21b_pmc_register_types(void) {
	type_register_static(&atsame70q21b_pmc_info);
}

type_init(atsame70q21b_pmc_register_types);

#define TYPE_ATSAME70Q21B_SUPC "atsame70q21b-supc"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAME70Q21BSUPC, ATSAME70Q21B_SUPC)

struct ATSAME70Q21BSUPC {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t CR;
	uint32_t SMMR;
	uint32_t MR;
	uint32_t WUMR;
	uint32_t WUIR;
	uint32_t SR;
	/* base */
	uint32_t base;

};
#define ATSAME70Q21B_SUPC_SIZE				0x18

REG32(SUPC_CR, 0x0)
	FIELD(SUPC_CR, VROFF, 2, 1)
	FIELD(SUPC_CR, XTALSEL, 3, 1)
	FIELD(SUPC_CR, KEY, 24, 8)
REG32(SUPC_SMMR, 0x4)
	FIELD(SUPC_SMMR, SMTH, 0, 4)
	FIELD(SUPC_SMMR, SMSMPL, 8, 3)
	FIELD(SUPC_SMMR, SMRSTEN, 12, 1)
	FIELD(SUPC_SMMR, SMIEN, 13, 1)
REG32(SUPC_MR, 0x8)
	FIELD(SUPC_MR, BODRSTEN, 12, 1)
	FIELD(SUPC_MR, BODDIS, 13, 1)
	FIELD(SUPC_MR, ONREG, 14, 1)
	FIELD(SUPC_MR, BKUPRETON, 17, 1)
	FIELD(SUPC_MR, OSCBYPASS, 20, 1)
	FIELD(SUPC_MR, KEY, 24, 8)
REG32(SUPC_WUMR, 0xc)
	FIELD(SUPC_WUMR, SMEN, 1, 1)
	FIELD(SUPC_WUMR, RTTEN, 2, 1)
	FIELD(SUPC_WUMR, RTCEN, 3, 1)
	FIELD(SUPC_WUMR, LPDBCEN0, 5, 1)
	FIELD(SUPC_WUMR, LPDBCEN1, 6, 1)
	FIELD(SUPC_WUMR, LPDBCCLR, 7, 1)
	FIELD(SUPC_WUMR, WKUPDBC, 12, 3)
	FIELD(SUPC_WUMR, LPDBC, 16, 3)
REG32(SUPC_WUIR, 0x10)
	FIELD(SUPC_WUIR, WKUPEN0, 0, 1)
	FIELD(SUPC_WUIR, WKUPEN1, 1, 1)
	FIELD(SUPC_WUIR, WKUPEN2, 2, 1)
	FIELD(SUPC_WUIR, WKUPEN3, 3, 1)
	FIELD(SUPC_WUIR, WKUPEN4, 4, 1)
	FIELD(SUPC_WUIR, WKUPEN5, 5, 1)
	FIELD(SUPC_WUIR, WKUPEN6, 6, 1)
	FIELD(SUPC_WUIR, WKUPEN7, 7, 1)
	FIELD(SUPC_WUIR, WKUPEN8, 8, 1)
	FIELD(SUPC_WUIR, WKUPEN9, 9, 1)
	FIELD(SUPC_WUIR, WKUPEN10, 10, 1)
	FIELD(SUPC_WUIR, WKUPEN11, 11, 1)
	FIELD(SUPC_WUIR, WKUPEN12, 12, 1)
	FIELD(SUPC_WUIR, WKUPEN13, 13, 1)
	FIELD(SUPC_WUIR, WKUPT0, 16, 1)
	FIELD(SUPC_WUIR, WKUPT1, 17, 1)
	FIELD(SUPC_WUIR, WKUPT2, 18, 1)
	FIELD(SUPC_WUIR, WKUPT3, 19, 1)
	FIELD(SUPC_WUIR, WKUPT4, 20, 1)
	FIELD(SUPC_WUIR, WKUPT5, 21, 1)
	FIELD(SUPC_WUIR, WKUPT6, 22, 1)
	FIELD(SUPC_WUIR, WKUPT7, 23, 1)
	FIELD(SUPC_WUIR, WKUPT8, 24, 1)
	FIELD(SUPC_WUIR, WKUPT9, 25, 1)
	FIELD(SUPC_WUIR, WKUPT10, 26, 1)
	FIELD(SUPC_WUIR, WKUPT11, 27, 1)
	FIELD(SUPC_WUIR, WKUPT12, 28, 1)
	FIELD(SUPC_WUIR, WKUPT13, 29, 1)
REG32(SUPC_SR, 0x14)
	FIELD(SUPC_SR, WKUPS, 1, 1)
	FIELD(SUPC_SR, SMWS, 2, 1)
	FIELD(SUPC_SR, BODRSTS, 3, 1)
	FIELD(SUPC_SR, SMRSTS, 4, 1)
	FIELD(SUPC_SR, SMS, 5, 1)
	FIELD(SUPC_SR, SMOS, 6, 1)
	FIELD(SUPC_SR, OSCSEL, 7, 1)
	FIELD(SUPC_SR, LPDBCS0, 13, 1)
	FIELD(SUPC_SR, LPDBCS1, 14, 1)
	FIELD(SUPC_SR, WKUPIS0, 16, 1)
	FIELD(SUPC_SR, WKUPIS1, 17, 1)
	FIELD(SUPC_SR, WKUPIS2, 18, 1)
	FIELD(SUPC_SR, WKUPIS3, 19, 1)
	FIELD(SUPC_SR, WKUPIS4, 20, 1)
	FIELD(SUPC_SR, WKUPIS5, 21, 1)
	FIELD(SUPC_SR, WKUPIS6, 22, 1)
	FIELD(SUPC_SR, WKUPIS7, 23, 1)
	FIELD(SUPC_SR, WKUPIS8, 24, 1)
	FIELD(SUPC_SR, WKUPIS9, 25, 1)
	FIELD(SUPC_SR, WKUPIS10, 26, 1)
	FIELD(SUPC_SR, WKUPIS11, 27, 1)
	FIELD(SUPC_SR, WKUPIS12, 28, 1)
	FIELD(SUPC_SR, WKUPIS13, 29, 1)


static void atsame70q21b_supc_register_reset(ATSAME70Q21BSUPC *t) {

}

static uint64_t atsame70q21b_supc_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAME70Q21BSUPC *t = ATSAME70Q21B_SUPC(opaque);
	uint64_t ret;

	switch (offset) {
		case A_SUPC_SMMR:
			ret = t->SMMR;
			break;
		case A_SUPC_MR:
			ret = t->MR;
			break;
		case A_SUPC_WUMR:
			ret = t->WUMR;
			break;
		case A_SUPC_WUIR:
			ret = t->WUIR;
			break;
		case A_SUPC_SR:
			ret = t->SR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAME70Q21B SUPC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsame70q21b_supc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAME70Q21BSUPC *t = ATSAME70Q21B_SUPC(opaque);


	switch (offset) {
		case A_SUPC_CR ... A_SUPC_CR + 3:
			value = value << ((offset - A_SUPC_CR) << 3);
			offset = A_SUPC_CR;
			break;
		case A_SUPC_SMMR ... A_SUPC_SMMR + 3:
			value = value << ((offset - A_SUPC_SMMR) << 3);
			offset = A_SUPC_SMMR;
			break;
		case A_SUPC_MR ... A_SUPC_MR + 3:
			value = value << ((offset - A_SUPC_MR) << 3);
			offset = A_SUPC_MR;
			break;
		case A_SUPC_WUMR ... A_SUPC_WUMR + 3:
			value = value << ((offset - A_SUPC_WUMR) << 3);
			offset = A_SUPC_WUMR;
			break;
		case A_SUPC_WUIR ... A_SUPC_WUIR + 3:
			value = value << ((offset - A_SUPC_WUIR) << 3);
			offset = A_SUPC_WUIR;
			break;

		default: break;
	}

	switch (offset) {
		case A_SUPC_CR:
			t->CR = value;
			break;
		case A_SUPC_SMMR:
			t->SMMR = value;
			break;
		case A_SUPC_MR:
			t->MR = value;
			break;
		case A_SUPC_WUMR:
			t->WUMR = value;
			break;
		case A_SUPC_WUIR:
			t->WUIR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAME70Q21B SUPC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsame70q21b_supc_ops = {
	.read = atsame70q21b_supc_read,
	.write = atsame70q21b_supc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsame70q21b_supc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAME70Q21BSUPC *t = ATSAME70Q21B_SUPC(obj);
	memory_region_init_io(&(t->iomem), obj, &atsame70q21b_supc_ops, t, "atsame70q21b-supc", ATSAME70Q21B_SUPC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void atsame70q21b_supc_realize(DeviceState *dev, Error **errp) {
	return;

}

static void atsame70q21b_supc_reset_enter(Object *obj, ResetType type) {
	ATSAME70Q21BSUPC *t = ATSAME70Q21B_SUPC(obj);
	atsame70q21b_supc_register_reset(t);
}

static const VMStateDescription atsame70q21b_supc_vmstate = {
	.name = "atsame70q21b-supc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR, ATSAME70Q21BSUPC),
		VMSTATE_UINT32(SMMR, ATSAME70Q21BSUPC),
		VMSTATE_UINT32(MR, ATSAME70Q21BSUPC),
		VMSTATE_UINT32(WUMR, ATSAME70Q21BSUPC),
		VMSTATE_UINT32(WUIR, ATSAME70Q21BSUPC),
		VMSTATE_UINT32(SR, ATSAME70Q21BSUPC),

		VMSTATE_END_OF_LIST()
	}
};

static void atsame70q21b_supc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsame70q21b_supc_vmstate;
	dc->realize = atsame70q21b_supc_realize;
	rc->phases.enter = atsame70q21b_supc_reset_enter;

}

static const TypeInfo atsame70q21b_supc_info = {
	.name = TYPE_ATSAME70Q21B_SUPC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAME70Q21BSUPC),
	.instance_init = atsame70q21b_supc_init,
	.class_init = atsame70q21b_supc_class_init,
};

static void atsame70q21b_supc_register_types(void) {
	type_register_static(&atsame70q21b_supc_info);
}

type_init(atsame70q21b_supc_register_types);

#define TYPE_ATSAME70Q21B_PIO "atsame70q21b-pio"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAME70Q21BPIO, ATSAME70Q21B_PIO)

struct ATSAME70Q21BPIO {
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
	uint32_t IFSCDR;
	uint32_t IFSCER;
	uint32_t IFSCSR;
	uint32_t SCDR;
	uint32_t PPDDR;
	uint32_t PPDER;
	uint32_t PPDSR;
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
	uint32_t SCHMITT;
	uint32_t DRIVER;
	uint32_t PCMR;
	uint32_t PCIER;
	uint32_t PCIDR;
	uint32_t PCIMR;
	uint32_t PCISR;
	uint32_t PCRHR;
	uint32_t ABCDSR0;
	uint32_t ABCDSR1;
	/* chardev backend */
	CharBackend chr;
	/* base */
	uint32_t base;

};
#define ATSAME70Q21B_PIO_SIZE				0x168

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
REG32(PIO_IFSCDR, 0x80)
	FIELD(PIO_IFSCDR, P0, 0, 1)
	FIELD(PIO_IFSCDR, P1, 1, 1)
	FIELD(PIO_IFSCDR, P2, 2, 1)
	FIELD(PIO_IFSCDR, P3, 3, 1)
	FIELD(PIO_IFSCDR, P4, 4, 1)
	FIELD(PIO_IFSCDR, P5, 5, 1)
	FIELD(PIO_IFSCDR, P6, 6, 1)
	FIELD(PIO_IFSCDR, P7, 7, 1)
	FIELD(PIO_IFSCDR, P8, 8, 1)
	FIELD(PIO_IFSCDR, P9, 9, 1)
	FIELD(PIO_IFSCDR, P10, 10, 1)
	FIELD(PIO_IFSCDR, P11, 11, 1)
	FIELD(PIO_IFSCDR, P12, 12, 1)
	FIELD(PIO_IFSCDR, P13, 13, 1)
	FIELD(PIO_IFSCDR, P14, 14, 1)
	FIELD(PIO_IFSCDR, P15, 15, 1)
	FIELD(PIO_IFSCDR, P16, 16, 1)
	FIELD(PIO_IFSCDR, P17, 17, 1)
	FIELD(PIO_IFSCDR, P18, 18, 1)
	FIELD(PIO_IFSCDR, P19, 19, 1)
	FIELD(PIO_IFSCDR, P20, 20, 1)
	FIELD(PIO_IFSCDR, P21, 21, 1)
	FIELD(PIO_IFSCDR, P22, 22, 1)
	FIELD(PIO_IFSCDR, P23, 23, 1)
	FIELD(PIO_IFSCDR, P24, 24, 1)
	FIELD(PIO_IFSCDR, P25, 25, 1)
	FIELD(PIO_IFSCDR, P26, 26, 1)
	FIELD(PIO_IFSCDR, P27, 27, 1)
	FIELD(PIO_IFSCDR, P28, 28, 1)
	FIELD(PIO_IFSCDR, P29, 29, 1)
	FIELD(PIO_IFSCDR, P30, 30, 1)
	FIELD(PIO_IFSCDR, P31, 31, 1)
REG32(PIO_IFSCER, 0x84)
	FIELD(PIO_IFSCER, P0, 0, 1)
	FIELD(PIO_IFSCER, P1, 1, 1)
	FIELD(PIO_IFSCER, P2, 2, 1)
	FIELD(PIO_IFSCER, P3, 3, 1)
	FIELD(PIO_IFSCER, P4, 4, 1)
	FIELD(PIO_IFSCER, P5, 5, 1)
	FIELD(PIO_IFSCER, P6, 6, 1)
	FIELD(PIO_IFSCER, P7, 7, 1)
	FIELD(PIO_IFSCER, P8, 8, 1)
	FIELD(PIO_IFSCER, P9, 9, 1)
	FIELD(PIO_IFSCER, P10, 10, 1)
	FIELD(PIO_IFSCER, P11, 11, 1)
	FIELD(PIO_IFSCER, P12, 12, 1)
	FIELD(PIO_IFSCER, P13, 13, 1)
	FIELD(PIO_IFSCER, P14, 14, 1)
	FIELD(PIO_IFSCER, P15, 15, 1)
	FIELD(PIO_IFSCER, P16, 16, 1)
	FIELD(PIO_IFSCER, P17, 17, 1)
	FIELD(PIO_IFSCER, P18, 18, 1)
	FIELD(PIO_IFSCER, P19, 19, 1)
	FIELD(PIO_IFSCER, P20, 20, 1)
	FIELD(PIO_IFSCER, P21, 21, 1)
	FIELD(PIO_IFSCER, P22, 22, 1)
	FIELD(PIO_IFSCER, P23, 23, 1)
	FIELD(PIO_IFSCER, P24, 24, 1)
	FIELD(PIO_IFSCER, P25, 25, 1)
	FIELD(PIO_IFSCER, P26, 26, 1)
	FIELD(PIO_IFSCER, P27, 27, 1)
	FIELD(PIO_IFSCER, P28, 28, 1)
	FIELD(PIO_IFSCER, P29, 29, 1)
	FIELD(PIO_IFSCER, P30, 30, 1)
	FIELD(PIO_IFSCER, P31, 31, 1)
REG32(PIO_IFSCSR, 0x88)
	FIELD(PIO_IFSCSR, P0, 0, 1)
	FIELD(PIO_IFSCSR, P1, 1, 1)
	FIELD(PIO_IFSCSR, P2, 2, 1)
	FIELD(PIO_IFSCSR, P3, 3, 1)
	FIELD(PIO_IFSCSR, P4, 4, 1)
	FIELD(PIO_IFSCSR, P5, 5, 1)
	FIELD(PIO_IFSCSR, P6, 6, 1)
	FIELD(PIO_IFSCSR, P7, 7, 1)
	FIELD(PIO_IFSCSR, P8, 8, 1)
	FIELD(PIO_IFSCSR, P9, 9, 1)
	FIELD(PIO_IFSCSR, P10, 10, 1)
	FIELD(PIO_IFSCSR, P11, 11, 1)
	FIELD(PIO_IFSCSR, P12, 12, 1)
	FIELD(PIO_IFSCSR, P13, 13, 1)
	FIELD(PIO_IFSCSR, P14, 14, 1)
	FIELD(PIO_IFSCSR, P15, 15, 1)
	FIELD(PIO_IFSCSR, P16, 16, 1)
	FIELD(PIO_IFSCSR, P17, 17, 1)
	FIELD(PIO_IFSCSR, P18, 18, 1)
	FIELD(PIO_IFSCSR, P19, 19, 1)
	FIELD(PIO_IFSCSR, P20, 20, 1)
	FIELD(PIO_IFSCSR, P21, 21, 1)
	FIELD(PIO_IFSCSR, P22, 22, 1)
	FIELD(PIO_IFSCSR, P23, 23, 1)
	FIELD(PIO_IFSCSR, P24, 24, 1)
	FIELD(PIO_IFSCSR, P25, 25, 1)
	FIELD(PIO_IFSCSR, P26, 26, 1)
	FIELD(PIO_IFSCSR, P27, 27, 1)
	FIELD(PIO_IFSCSR, P28, 28, 1)
	FIELD(PIO_IFSCSR, P29, 29, 1)
	FIELD(PIO_IFSCSR, P30, 30, 1)
	FIELD(PIO_IFSCSR, P31, 31, 1)
REG32(PIO_SCDR, 0x8c)
	FIELD(PIO_SCDR, DIV, 0, 14)
REG32(PIO_PPDDR, 0x90)
	FIELD(PIO_PPDDR, P0, 0, 1)
	FIELD(PIO_PPDDR, P1, 1, 1)
	FIELD(PIO_PPDDR, P2, 2, 1)
	FIELD(PIO_PPDDR, P3, 3, 1)
	FIELD(PIO_PPDDR, P4, 4, 1)
	FIELD(PIO_PPDDR, P5, 5, 1)
	FIELD(PIO_PPDDR, P6, 6, 1)
	FIELD(PIO_PPDDR, P7, 7, 1)
	FIELD(PIO_PPDDR, P8, 8, 1)
	FIELD(PIO_PPDDR, P9, 9, 1)
	FIELD(PIO_PPDDR, P10, 10, 1)
	FIELD(PIO_PPDDR, P11, 11, 1)
	FIELD(PIO_PPDDR, P12, 12, 1)
	FIELD(PIO_PPDDR, P13, 13, 1)
	FIELD(PIO_PPDDR, P14, 14, 1)
	FIELD(PIO_PPDDR, P15, 15, 1)
	FIELD(PIO_PPDDR, P16, 16, 1)
	FIELD(PIO_PPDDR, P17, 17, 1)
	FIELD(PIO_PPDDR, P18, 18, 1)
	FIELD(PIO_PPDDR, P19, 19, 1)
	FIELD(PIO_PPDDR, P20, 20, 1)
	FIELD(PIO_PPDDR, P21, 21, 1)
	FIELD(PIO_PPDDR, P22, 22, 1)
	FIELD(PIO_PPDDR, P23, 23, 1)
	FIELD(PIO_PPDDR, P24, 24, 1)
	FIELD(PIO_PPDDR, P25, 25, 1)
	FIELD(PIO_PPDDR, P26, 26, 1)
	FIELD(PIO_PPDDR, P27, 27, 1)
	FIELD(PIO_PPDDR, P28, 28, 1)
	FIELD(PIO_PPDDR, P29, 29, 1)
	FIELD(PIO_PPDDR, P30, 30, 1)
	FIELD(PIO_PPDDR, P31, 31, 1)
REG32(PIO_PPDER, 0x94)
	FIELD(PIO_PPDER, P0, 0, 1)
	FIELD(PIO_PPDER, P1, 1, 1)
	FIELD(PIO_PPDER, P2, 2, 1)
	FIELD(PIO_PPDER, P3, 3, 1)
	FIELD(PIO_PPDER, P4, 4, 1)
	FIELD(PIO_PPDER, P5, 5, 1)
	FIELD(PIO_PPDER, P6, 6, 1)
	FIELD(PIO_PPDER, P7, 7, 1)
	FIELD(PIO_PPDER, P8, 8, 1)
	FIELD(PIO_PPDER, P9, 9, 1)
	FIELD(PIO_PPDER, P10, 10, 1)
	FIELD(PIO_PPDER, P11, 11, 1)
	FIELD(PIO_PPDER, P12, 12, 1)
	FIELD(PIO_PPDER, P13, 13, 1)
	FIELD(PIO_PPDER, P14, 14, 1)
	FIELD(PIO_PPDER, P15, 15, 1)
	FIELD(PIO_PPDER, P16, 16, 1)
	FIELD(PIO_PPDER, P17, 17, 1)
	FIELD(PIO_PPDER, P18, 18, 1)
	FIELD(PIO_PPDER, P19, 19, 1)
	FIELD(PIO_PPDER, P20, 20, 1)
	FIELD(PIO_PPDER, P21, 21, 1)
	FIELD(PIO_PPDER, P22, 22, 1)
	FIELD(PIO_PPDER, P23, 23, 1)
	FIELD(PIO_PPDER, P24, 24, 1)
	FIELD(PIO_PPDER, P25, 25, 1)
	FIELD(PIO_PPDER, P26, 26, 1)
	FIELD(PIO_PPDER, P27, 27, 1)
	FIELD(PIO_PPDER, P28, 28, 1)
	FIELD(PIO_PPDER, P29, 29, 1)
	FIELD(PIO_PPDER, P30, 30, 1)
	FIELD(PIO_PPDER, P31, 31, 1)
REG32(PIO_PPDSR, 0x98)
	FIELD(PIO_PPDSR, P0, 0, 1)
	FIELD(PIO_PPDSR, P1, 1, 1)
	FIELD(PIO_PPDSR, P2, 2, 1)
	FIELD(PIO_PPDSR, P3, 3, 1)
	FIELD(PIO_PPDSR, P4, 4, 1)
	FIELD(PIO_PPDSR, P5, 5, 1)
	FIELD(PIO_PPDSR, P6, 6, 1)
	FIELD(PIO_PPDSR, P7, 7, 1)
	FIELD(PIO_PPDSR, P8, 8, 1)
	FIELD(PIO_PPDSR, P9, 9, 1)
	FIELD(PIO_PPDSR, P10, 10, 1)
	FIELD(PIO_PPDSR, P11, 11, 1)
	FIELD(PIO_PPDSR, P12, 12, 1)
	FIELD(PIO_PPDSR, P13, 13, 1)
	FIELD(PIO_PPDSR, P14, 14, 1)
	FIELD(PIO_PPDSR, P15, 15, 1)
	FIELD(PIO_PPDSR, P16, 16, 1)
	FIELD(PIO_PPDSR, P17, 17, 1)
	FIELD(PIO_PPDSR, P18, 18, 1)
	FIELD(PIO_PPDSR, P19, 19, 1)
	FIELD(PIO_PPDSR, P20, 20, 1)
	FIELD(PIO_PPDSR, P21, 21, 1)
	FIELD(PIO_PPDSR, P22, 22, 1)
	FIELD(PIO_PPDSR, P23, 23, 1)
	FIELD(PIO_PPDSR, P24, 24, 1)
	FIELD(PIO_PPDSR, P25, 25, 1)
	FIELD(PIO_PPDSR, P26, 26, 1)
	FIELD(PIO_PPDSR, P27, 27, 1)
	FIELD(PIO_PPDSR, P28, 28, 1)
	FIELD(PIO_PPDSR, P29, 29, 1)
	FIELD(PIO_PPDSR, P30, 30, 1)
	FIELD(PIO_PPDSR, P31, 31, 1)
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
REG32(PIO_SCHMITT, 0x100)
	FIELD(PIO_SCHMITT, SCHMITT0, 0, 1)
	FIELD(PIO_SCHMITT, SCHMITT1, 1, 1)
	FIELD(PIO_SCHMITT, SCHMITT2, 2, 1)
	FIELD(PIO_SCHMITT, SCHMITT3, 3, 1)
	FIELD(PIO_SCHMITT, SCHMITT4, 4, 1)
	FIELD(PIO_SCHMITT, SCHMITT5, 5, 1)
	FIELD(PIO_SCHMITT, SCHMITT6, 6, 1)
	FIELD(PIO_SCHMITT, SCHMITT7, 7, 1)
	FIELD(PIO_SCHMITT, SCHMITT8, 8, 1)
	FIELD(PIO_SCHMITT, SCHMITT9, 9, 1)
	FIELD(PIO_SCHMITT, SCHMITT10, 10, 1)
	FIELD(PIO_SCHMITT, SCHMITT11, 11, 1)
	FIELD(PIO_SCHMITT, SCHMITT12, 12, 1)
	FIELD(PIO_SCHMITT, SCHMITT13, 13, 1)
	FIELD(PIO_SCHMITT, SCHMITT14, 14, 1)
	FIELD(PIO_SCHMITT, SCHMITT15, 15, 1)
	FIELD(PIO_SCHMITT, SCHMITT16, 16, 1)
	FIELD(PIO_SCHMITT, SCHMITT17, 17, 1)
	FIELD(PIO_SCHMITT, SCHMITT18, 18, 1)
	FIELD(PIO_SCHMITT, SCHMITT19, 19, 1)
	FIELD(PIO_SCHMITT, SCHMITT20, 20, 1)
	FIELD(PIO_SCHMITT, SCHMITT21, 21, 1)
	FIELD(PIO_SCHMITT, SCHMITT22, 22, 1)
	FIELD(PIO_SCHMITT, SCHMITT23, 23, 1)
	FIELD(PIO_SCHMITT, SCHMITT24, 24, 1)
	FIELD(PIO_SCHMITT, SCHMITT25, 25, 1)
	FIELD(PIO_SCHMITT, SCHMITT26, 26, 1)
	FIELD(PIO_SCHMITT, SCHMITT27, 27, 1)
	FIELD(PIO_SCHMITT, SCHMITT28, 28, 1)
	FIELD(PIO_SCHMITT, SCHMITT29, 29, 1)
	FIELD(PIO_SCHMITT, SCHMITT30, 30, 1)
	FIELD(PIO_SCHMITT, SCHMITT31, 31, 1)
REG32(PIO_DRIVER, 0x118)
	FIELD(PIO_DRIVER, LINE0, 0, 1)
	FIELD(PIO_DRIVER, LINE1, 1, 1)
	FIELD(PIO_DRIVER, LINE2, 2, 1)
	FIELD(PIO_DRIVER, LINE3, 3, 1)
	FIELD(PIO_DRIVER, LINE4, 4, 1)
	FIELD(PIO_DRIVER, LINE5, 5, 1)
	FIELD(PIO_DRIVER, LINE6, 6, 1)
	FIELD(PIO_DRIVER, LINE7, 7, 1)
	FIELD(PIO_DRIVER, LINE8, 8, 1)
	FIELD(PIO_DRIVER, LINE9, 9, 1)
	FIELD(PIO_DRIVER, LINE10, 10, 1)
	FIELD(PIO_DRIVER, LINE11, 11, 1)
	FIELD(PIO_DRIVER, LINE12, 12, 1)
	FIELD(PIO_DRIVER, LINE13, 13, 1)
	FIELD(PIO_DRIVER, LINE14, 14, 1)
	FIELD(PIO_DRIVER, LINE15, 15, 1)
	FIELD(PIO_DRIVER, LINE16, 16, 1)
	FIELD(PIO_DRIVER, LINE17, 17, 1)
	FIELD(PIO_DRIVER, LINE18, 18, 1)
	FIELD(PIO_DRIVER, LINE19, 19, 1)
	FIELD(PIO_DRIVER, LINE20, 20, 1)
	FIELD(PIO_DRIVER, LINE21, 21, 1)
	FIELD(PIO_DRIVER, LINE22, 22, 1)
	FIELD(PIO_DRIVER, LINE23, 23, 1)
	FIELD(PIO_DRIVER, LINE24, 24, 1)
	FIELD(PIO_DRIVER, LINE25, 25, 1)
	FIELD(PIO_DRIVER, LINE26, 26, 1)
	FIELD(PIO_DRIVER, LINE27, 27, 1)
	FIELD(PIO_DRIVER, LINE28, 28, 1)
	FIELD(PIO_DRIVER, LINE29, 29, 1)
	FIELD(PIO_DRIVER, LINE30, 30, 1)
	FIELD(PIO_DRIVER, LINE31, 31, 1)
REG32(PIO_PCMR, 0x150)
	FIELD(PIO_PCMR, PCEN, 0, 1)
	FIELD(PIO_PCMR, DSIZE, 4, 2)
	FIELD(PIO_PCMR, ALWYS, 9, 1)
	FIELD(PIO_PCMR, HALFS, 10, 1)
	FIELD(PIO_PCMR, FRSTS, 11, 1)
REG32(PIO_PCIER, 0x154)
	FIELD(PIO_PCIER, DRDY, 0, 1)
	FIELD(PIO_PCIER, OVRE, 1, 1)
	FIELD(PIO_PCIER, ENDRX, 2, 1)
	FIELD(PIO_PCIER, RXBUFF, 3, 1)
REG32(PIO_PCIDR, 0x158)
	FIELD(PIO_PCIDR, DRDY, 0, 1)
	FIELD(PIO_PCIDR, OVRE, 1, 1)
	FIELD(PIO_PCIDR, ENDRX, 2, 1)
	FIELD(PIO_PCIDR, RXBUFF, 3, 1)
REG32(PIO_PCIMR, 0x15c)
	FIELD(PIO_PCIMR, DRDY, 0, 1)
	FIELD(PIO_PCIMR, OVRE, 1, 1)
	FIELD(PIO_PCIMR, ENDRX, 2, 1)
	FIELD(PIO_PCIMR, RXBUFF, 3, 1)
REG32(PIO_PCISR, 0x160)
	FIELD(PIO_PCISR, DRDY, 0, 1)
	FIELD(PIO_PCISR, OVRE, 1, 1)
REG32(PIO_PCRHR, 0x164)
	FIELD(PIO_PCRHR, RDATA, 0, 32)
REG32(PIO_ABCDSR0, 0x70)
	FIELD(PIO_ABCDSR0, P0, 0, 1)
	FIELD(PIO_ABCDSR0, P1, 1, 1)
	FIELD(PIO_ABCDSR0, P2, 2, 1)
	FIELD(PIO_ABCDSR0, P3, 3, 1)
	FIELD(PIO_ABCDSR0, P4, 4, 1)
	FIELD(PIO_ABCDSR0, P5, 5, 1)
	FIELD(PIO_ABCDSR0, P6, 6, 1)
	FIELD(PIO_ABCDSR0, P7, 7, 1)
	FIELD(PIO_ABCDSR0, P8, 8, 1)
	FIELD(PIO_ABCDSR0, P9, 9, 1)
	FIELD(PIO_ABCDSR0, P10, 10, 1)
	FIELD(PIO_ABCDSR0, P11, 11, 1)
	FIELD(PIO_ABCDSR0, P12, 12, 1)
	FIELD(PIO_ABCDSR0, P13, 13, 1)
	FIELD(PIO_ABCDSR0, P14, 14, 1)
	FIELD(PIO_ABCDSR0, P15, 15, 1)
	FIELD(PIO_ABCDSR0, P16, 16, 1)
	FIELD(PIO_ABCDSR0, P17, 17, 1)
	FIELD(PIO_ABCDSR0, P18, 18, 1)
	FIELD(PIO_ABCDSR0, P19, 19, 1)
	FIELD(PIO_ABCDSR0, P20, 20, 1)
	FIELD(PIO_ABCDSR0, P21, 21, 1)
	FIELD(PIO_ABCDSR0, P22, 22, 1)
	FIELD(PIO_ABCDSR0, P23, 23, 1)
	FIELD(PIO_ABCDSR0, P24, 24, 1)
	FIELD(PIO_ABCDSR0, P25, 25, 1)
	FIELD(PIO_ABCDSR0, P26, 26, 1)
	FIELD(PIO_ABCDSR0, P27, 27, 1)
	FIELD(PIO_ABCDSR0, P28, 28, 1)
	FIELD(PIO_ABCDSR0, P29, 29, 1)
	FIELD(PIO_ABCDSR0, P30, 30, 1)
	FIELD(PIO_ABCDSR0, P31, 31, 1)
REG32(PIO_ABCDSR1, 0x74)
	FIELD(PIO_ABCDSR1, P0, 0, 1)
	FIELD(PIO_ABCDSR1, P1, 1, 1)
	FIELD(PIO_ABCDSR1, P2, 2, 1)
	FIELD(PIO_ABCDSR1, P3, 3, 1)
	FIELD(PIO_ABCDSR1, P4, 4, 1)
	FIELD(PIO_ABCDSR1, P5, 5, 1)
	FIELD(PIO_ABCDSR1, P6, 6, 1)
	FIELD(PIO_ABCDSR1, P7, 7, 1)
	FIELD(PIO_ABCDSR1, P8, 8, 1)
	FIELD(PIO_ABCDSR1, P9, 9, 1)
	FIELD(PIO_ABCDSR1, P10, 10, 1)
	FIELD(PIO_ABCDSR1, P11, 11, 1)
	FIELD(PIO_ABCDSR1, P12, 12, 1)
	FIELD(PIO_ABCDSR1, P13, 13, 1)
	FIELD(PIO_ABCDSR1, P14, 14, 1)
	FIELD(PIO_ABCDSR1, P15, 15, 1)
	FIELD(PIO_ABCDSR1, P16, 16, 1)
	FIELD(PIO_ABCDSR1, P17, 17, 1)
	FIELD(PIO_ABCDSR1, P18, 18, 1)
	FIELD(PIO_ABCDSR1, P19, 19, 1)
	FIELD(PIO_ABCDSR1, P20, 20, 1)
	FIELD(PIO_ABCDSR1, P21, 21, 1)
	FIELD(PIO_ABCDSR1, P22, 22, 1)
	FIELD(PIO_ABCDSR1, P23, 23, 1)
	FIELD(PIO_ABCDSR1, P24, 24, 1)
	FIELD(PIO_ABCDSR1, P25, 25, 1)
	FIELD(PIO_ABCDSR1, P26, 26, 1)
	FIELD(PIO_ABCDSR1, P27, 27, 1)
	FIELD(PIO_ABCDSR1, P28, 28, 1)
	FIELD(PIO_ABCDSR1, P29, 29, 1)
	FIELD(PIO_ABCDSR1, P30, 30, 1)
	FIELD(PIO_ABCDSR1, P31, 31, 1)


__attribute__((unused))
static void atsame70q21b_pio_update(ATSAME70Q21BPIO *t) {
	int conditions = (((t->IMR & 0x2000000) && (t->ISR & 0x2000000)) || ((t->IMR & 0x80000000) && (t->ISR & 0x80000000)) || ((t->IMR & 0x8) && (t->ISR & 0x8)) || ((t->IMR & 0x80) && (t->ISR & 0x80)) || ((t->IMR & 0x20000000) && (t->ISR & 0x20000000)) || ((t->IMR & 0x8000) && (t->ISR & 0x8000)) || ((t->IMR & 0x200) && (t->ISR & 0x200)) || ((t->IMR & 0x100) && (t->ISR & 0x100)) || ((t->IMR & 0x1) && (t->ISR & 0x1)) || ((t->IMR & 0x100000) && (t->ISR & 0x100000)) || ((t->IMR & 0x800000) && (t->ISR & 0x800000)) || ((t->IMR & 0x40) && (t->ISR & 0x40)) || ((t->IMR & 0x1000) && (t->ISR & 0x1000)) || ((t->IMR & 0x2) && (t->ISR & 0x2)) || ((t->IMR & 0x10000000) && (t->ISR & 0x10000000)) || ((t->IMR & 0x4000000) && (t->ISR & 0x4000000)) || ((t->IMR & 0x1000000) && (t->ISR & 0x1000000)) || ((t->IMR & 0x200000) && (t->ISR & 0x200000)) || ((t->IMR & 0x4) && (t->ISR & 0x4)) || ((t->IMR & 0x4000) && (t->ISR & 0x4000)) || ((t->IMR & 0x40000) && (t->ISR & 0x40000)) || ((t->IMR & 0x40000000) && (t->ISR & 0x40000000)) || ((t->IMR & 0x800) && (t->ISR & 0x800)) || ((t->IMR & 0x80000) && (t->ISR & 0x80000)) || ((t->IMR & 0x8000000) && (t->ISR & 0x8000000)) || ((t->IMR & 0x400) && (t->ISR & 0x400)) || ((t->IMR & 0x20) && (t->ISR & 0x20)) || ((t->IMR & 0x10) && (t->ISR & 0x10)) || ((t->IMR & 0x10000) && (t->ISR & 0x10000)) || ((t->IMR & 0x400000) && (t->ISR & 0x400000)) || ((t->IMR & 0x2000) && (t->ISR & 0x2000)) || ((t->ISR & 0x20000000) && (t->IMR & 0x20000000)) || ((t->ISR & 0x10000000) && (t->IMR & 0x10000000)) || ((t->IMR & 0x20000) && (t->ISR & 0x20000)) || ((t->ISR & 0x80000000) && (t->IMR & 0x80000000)) || ((t->ISR & 0x40000000) && (t->IMR & 0x40000000)));
	qemu_set_irq(t->irq[0], conditions);
	qemu_set_irq(t->irq[0], 0);

}

static int atsame70q21b_pio_can_receive(void *opaque) {
	return 1;
}

static void atsame70q21b_pio_receive(void *opaque, const uint8_t *buf, int size) {
	ATSAME70Q21BPIO *t = ATSAME70Q21B_PIO(opaque);

	t->IMR = (1 << (*buf % 32));
	t->ISR = (1 << (*buf % 32));

	atsame70q21b_pio_update(t);
}

static void atsame70q21b_pio_register_reset(ATSAME70Q21BPIO *t) {

}

static uint64_t atsame70q21b_pio_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAME70Q21BPIO *t = ATSAME70Q21B_PIO(opaque);
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
		case A_PIO_IFSCSR:
			ret = t->IFSCSR;
			break;
		case A_PIO_SCDR:
			ret = t->SCDR;
			break;
		case A_PIO_PPDSR:
			ret = t->PPDSR;
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
		case A_PIO_SCHMITT:
			ret = t->SCHMITT;
			break;
		case A_PIO_DRIVER:
			ret = t->DRIVER;
			break;
		case A_PIO_PCMR:
			ret = t->PCMR;
			break;
		case A_PIO_PCIMR:
			ret = t->PCIMR;
			break;
		case A_PIO_PCISR:
			ret = t->PCISR;
			break;
		case A_PIO_PCRHR:
			ret = t->PCRHR;
			break;
		case A_PIO_ABCDSR0:
			ret = t->ABCDSR0;
			break;
		case A_PIO_ABCDSR1:
			ret = t->ABCDSR1;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAME70Q21B PIO read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsame70q21b_pio_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAME70Q21BPIO *t = ATSAME70Q21B_PIO(opaque);


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
		case A_PIO_IFSCDR ... A_PIO_IFSCDR + 3:
			value = value << ((offset - A_PIO_IFSCDR) << 3);
			offset = A_PIO_IFSCDR;
			break;
		case A_PIO_IFSCER ... A_PIO_IFSCER + 3:
			value = value << ((offset - A_PIO_IFSCER) << 3);
			offset = A_PIO_IFSCER;
			break;
		case A_PIO_SCDR ... A_PIO_SCDR + 3:
			value = value << ((offset - A_PIO_SCDR) << 3);
			offset = A_PIO_SCDR;
			break;
		case A_PIO_PPDDR ... A_PIO_PPDDR + 3:
			value = value << ((offset - A_PIO_PPDDR) << 3);
			offset = A_PIO_PPDDR;
			break;
		case A_PIO_PPDER ... A_PIO_PPDER + 3:
			value = value << ((offset - A_PIO_PPDER) << 3);
			offset = A_PIO_PPDER;
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
		case A_PIO_SCHMITT ... A_PIO_SCHMITT + 3:
			value = value << ((offset - A_PIO_SCHMITT) << 3);
			offset = A_PIO_SCHMITT;
			break;
		case A_PIO_DRIVER ... A_PIO_DRIVER + 3:
			value = value << ((offset - A_PIO_DRIVER) << 3);
			offset = A_PIO_DRIVER;
			break;
		case A_PIO_PCMR ... A_PIO_PCMR + 3:
			value = value << ((offset - A_PIO_PCMR) << 3);
			offset = A_PIO_PCMR;
			break;
		case A_PIO_PCIER ... A_PIO_PCIER + 3:
			value = value << ((offset - A_PIO_PCIER) << 3);
			offset = A_PIO_PCIER;
			break;
		case A_PIO_PCIDR ... A_PIO_PCIDR + 3:
			value = value << ((offset - A_PIO_PCIDR) << 3);
			offset = A_PIO_PCIDR;
			break;
		case A_PIO_ABCDSR0 ... A_PIO_ABCDSR0 + 3:
			value = value << ((offset - A_PIO_ABCDSR0) << 3);
			offset = A_PIO_ABCDSR0;
			break;
		case A_PIO_ABCDSR1 ... A_PIO_ABCDSR1 + 3:
			value = value << ((offset - A_PIO_ABCDSR1) << 3);
			offset = A_PIO_ABCDSR1;
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
		case A_PIO_IFSCDR:
			t->IFSCDR = value;
			break;
		case A_PIO_IFSCER:
			t->IFSCER = value;
			break;
		case A_PIO_SCDR:
			t->SCDR = value;
			break;
		case A_PIO_PPDDR:
			t->PPDDR = value;
			break;
		case A_PIO_PPDER:
			t->PPDER = value;
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
		case A_PIO_SCHMITT:
			t->SCHMITT = value;
			break;
		case A_PIO_DRIVER:
			t->DRIVER = value;
			break;
		case A_PIO_PCMR:
			t->PCMR = value;
			break;
		case A_PIO_PCIER:
			t->PCIER = value;
			break;
		case A_PIO_PCIDR:
			t->PCIDR = value;
			break;
		case A_PIO_ABCDSR0:
			t->ABCDSR0 = value;
			break;
		case A_PIO_ABCDSR1:
			t->ABCDSR1 = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAME70Q21B PIO write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsame70q21b_pio_ops = {
	.read = atsame70q21b_pio_read,
	.write = atsame70q21b_pio_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsame70q21b_pio_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAME70Q21BPIO *t = ATSAME70Q21B_PIO(obj);
	memory_region_init_io(&(t->iomem), obj, &atsame70q21b_pio_ops, t, "atsame70q21b-pio", ATSAME70Q21B_PIO_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void atsame70q21b_pio_realize(DeviceState *dev, Error **errp) {
	ATSAME70Q21BPIO *t = ATSAME70Q21B_PIO(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), atsame70q21b_pio_can_receive, atsame70q21b_pio_receive,
		NULL, NULL, t, NULL, true);

}

static void atsame70q21b_pio_reset_enter(Object *obj, ResetType type) {
	ATSAME70Q21BPIO *t = ATSAME70Q21B_PIO(obj);
	atsame70q21b_pio_register_reset(t);
}

static const VMStateDescription atsame70q21b_pio_vmstate = {
	.name = "atsame70q21b-pio",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(PER, ATSAME70Q21BPIO),
		VMSTATE_UINT32(PDR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(PSR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(OER, ATSAME70Q21BPIO),
		VMSTATE_UINT32(ODR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(OSR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(IFER, ATSAME70Q21BPIO),
		VMSTATE_UINT32(IFDR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(IFSR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(SODR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(CODR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(ODSR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(PDSR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(IER, ATSAME70Q21BPIO),
		VMSTATE_UINT32(IDR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(IMR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(ISR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(MDER, ATSAME70Q21BPIO),
		VMSTATE_UINT32(MDDR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(MDSR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(PUDR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(PUER, ATSAME70Q21BPIO),
		VMSTATE_UINT32(PUSR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(IFSCDR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(IFSCER, ATSAME70Q21BPIO),
		VMSTATE_UINT32(IFSCSR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(SCDR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(PPDDR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(PPDER, ATSAME70Q21BPIO),
		VMSTATE_UINT32(PPDSR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(OWER, ATSAME70Q21BPIO),
		VMSTATE_UINT32(OWDR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(OWSR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(AIMER, ATSAME70Q21BPIO),
		VMSTATE_UINT32(AIMDR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(AIMMR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(ESR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(LSR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(ELSR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(FELLSR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(REHLSR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(FRLHSR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(LOCKSR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(WPMR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(WPSR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(SCHMITT, ATSAME70Q21BPIO),
		VMSTATE_UINT32(DRIVER, ATSAME70Q21BPIO),
		VMSTATE_UINT32(PCMR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(PCIER, ATSAME70Q21BPIO),
		VMSTATE_UINT32(PCIDR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(PCIMR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(PCISR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(PCRHR, ATSAME70Q21BPIO),
		VMSTATE_UINT32(ABCDSR0, ATSAME70Q21BPIO),
		VMSTATE_UINT32(ABCDSR1, ATSAME70Q21BPIO),

		VMSTATE_END_OF_LIST()
	}
};

static Property atsame70q21b_pio_properties[] = {
	DEFINE_PROP_CHR("chardev", ATSAME70Q21BPIO, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void atsame70q21b_pio_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsame70q21b_pio_vmstate;
	dc->realize = atsame70q21b_pio_realize;
	rc->phases.enter = atsame70q21b_pio_reset_enter;
	device_class_set_props(dc, atsame70q21b_pio_properties);

}

static const TypeInfo atsame70q21b_pio_info = {
	.name = TYPE_ATSAME70Q21B_PIO,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAME70Q21BPIO),
	.instance_init = atsame70q21b_pio_init,
	.class_init = atsame70q21b_pio_class_init,
};

static void atsame70q21b_pio_register_types(void) {
	type_register_static(&atsame70q21b_pio_info);
}

type_init(atsame70q21b_pio_register_types);

#define TYPE_ATSAME70Q21B_EFC "atsame70q21b-efc"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAME70Q21BEFC, ATSAME70Q21B_EFC)

struct ATSAME70Q21BEFC {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t EEFC_FMR;
	uint32_t EEFC_FCR;
	uint32_t EEFC_FSR;
	uint32_t EEFC_FRR;
	uint32_t EEFC_WPMR;
	/* base */
	uint32_t base;

};
#define ATSAME70Q21B_EFC_SIZE				0xe8

REG32(EFC_EEFC_FMR, 0x0)
	FIELD(EFC_EEFC_FMR, FRDY, 0, 1)
	FIELD(EFC_EEFC_FMR, FWS, 8, 4)
	FIELD(EFC_EEFC_FMR, SCOD, 16, 1)
	FIELD(EFC_EEFC_FMR, CLOE, 26, 1)
REG32(EFC_EEFC_FCR, 0x4)
	FIELD(EFC_EEFC_FCR, FCMD, 0, 8)
	FIELD(EFC_EEFC_FCR, FARG, 8, 16)
	FIELD(EFC_EEFC_FCR, FKEY, 24, 8)
REG32(EFC_EEFC_FSR, 0x8)
	FIELD(EFC_EEFC_FSR, FRDY, 0, 1)
	FIELD(EFC_EEFC_FSR, FCMDE, 1, 1)
	FIELD(EFC_EEFC_FSR, FLOCKE, 2, 1)
	FIELD(EFC_EEFC_FSR, FLERR, 3, 1)
	FIELD(EFC_EEFC_FSR, UECCELSB, 16, 1)
	FIELD(EFC_EEFC_FSR, MECCELSB, 17, 1)
	FIELD(EFC_EEFC_FSR, UECCEMSB, 18, 1)
	FIELD(EFC_EEFC_FSR, MECCEMSB, 19, 1)
REG32(EFC_EEFC_FRR, 0xc)
	FIELD(EFC_EEFC_FRR, FVALUE, 0, 32)
REG32(EFC_EEFC_WPMR, 0xe4)
	FIELD(EFC_EEFC_WPMR, WPEN, 0, 1)
	FIELD(EFC_EEFC_WPMR, WPKEY, 8, 24)


static void atsame70q21b_efc_register_reset(ATSAME70Q21BEFC *t) {

}

static uint64_t atsame70q21b_efc_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAME70Q21BEFC *t = ATSAME70Q21B_EFC(opaque);
	uint64_t ret;

	switch (offset) {
		case A_EFC_EEFC_FMR:
			ret = t->EEFC_FMR;
			break;
		case A_EFC_EEFC_FSR:
			ret = t->EEFC_FSR;
			break;
		case A_EFC_EEFC_FRR:
			ret = t->EEFC_FRR;
			break;
		case A_EFC_EEFC_WPMR:
			ret = t->EEFC_WPMR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAME70Q21B EFC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsame70q21b_efc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAME70Q21BEFC *t = ATSAME70Q21B_EFC(opaque);


	switch (offset) {
		case A_EFC_EEFC_FMR ... A_EFC_EEFC_FMR + 3:
			value = value << ((offset - A_EFC_EEFC_FMR) << 3);
			offset = A_EFC_EEFC_FMR;
			break;
		case A_EFC_EEFC_FCR ... A_EFC_EEFC_FCR + 3:
			value = value << ((offset - A_EFC_EEFC_FCR) << 3);
			offset = A_EFC_EEFC_FCR;
			break;
		case A_EFC_EEFC_WPMR ... A_EFC_EEFC_WPMR + 3:
			value = value << ((offset - A_EFC_EEFC_WPMR) << 3);
			offset = A_EFC_EEFC_WPMR;
			break;

		default: break;
	}

	switch (offset) {
		case A_EFC_EEFC_FMR:
			t->EEFC_FMR = value;
			break;
		case A_EFC_EEFC_FCR:
			t->EEFC_FCR = value;
			break;
		case A_EFC_EEFC_WPMR:
			t->EEFC_WPMR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAME70Q21B EFC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsame70q21b_efc_ops = {
	.read = atsame70q21b_efc_read,
	.write = atsame70q21b_efc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsame70q21b_efc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAME70Q21BEFC *t = ATSAME70Q21B_EFC(obj);
	memory_region_init_io(&(t->iomem), obj, &atsame70q21b_efc_ops, t, "atsame70q21b-efc", ATSAME70Q21B_EFC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void atsame70q21b_efc_realize(DeviceState *dev, Error **errp) {
	return;

}

static void atsame70q21b_efc_reset_enter(Object *obj, ResetType type) {
	ATSAME70Q21BEFC *t = ATSAME70Q21B_EFC(obj);
	atsame70q21b_efc_register_reset(t);
}

static const VMStateDescription atsame70q21b_efc_vmstate = {
	.name = "atsame70q21b-efc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(EEFC_FMR, ATSAME70Q21BEFC),
		VMSTATE_UINT32(EEFC_FCR, ATSAME70Q21BEFC),
		VMSTATE_UINT32(EEFC_FSR, ATSAME70Q21BEFC),
		VMSTATE_UINT32(EEFC_FRR, ATSAME70Q21BEFC),
		VMSTATE_UINT32(EEFC_WPMR, ATSAME70Q21BEFC),

		VMSTATE_END_OF_LIST()
	}
};

static void atsame70q21b_efc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsame70q21b_efc_vmstate;
	dc->realize = atsame70q21b_efc_realize;
	rc->phases.enter = atsame70q21b_efc_reset_enter;

}

static const TypeInfo atsame70q21b_efc_info = {
	.name = TYPE_ATSAME70Q21B_EFC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAME70Q21BEFC),
	.instance_init = atsame70q21b_efc_init,
	.class_init = atsame70q21b_efc_class_init,
};

static void atsame70q21b_efc_register_types(void) {
	type_register_static(&atsame70q21b_efc_info);
}

type_init(atsame70q21b_efc_register_types);

#define TYPE_ATSAME70Q21B_WDT "atsame70q21b-wdt"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAME70Q21BWDT, ATSAME70Q21B_WDT)

struct ATSAME70Q21BWDT {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t CR;
	uint32_t MR;
	uint32_t SR;
	/* base */
	uint32_t base;

};
#define ATSAME70Q21B_WDT_SIZE				0xc

REG32(WDT_CR, 0x0)
	FIELD(WDT_CR, WDRSTT, 0, 1)
	FIELD(WDT_CR, KEY, 24, 8)
REG32(WDT_MR, 0x4)
	FIELD(WDT_MR, WDV, 0, 12)
	FIELD(WDT_MR, WDFIEN, 12, 1)
	FIELD(WDT_MR, WDRSTEN, 13, 1)
	FIELD(WDT_MR, WDDIS, 15, 1)
	FIELD(WDT_MR, WDD, 16, 12)
	FIELD(WDT_MR, WDDBGHLT, 28, 1)
	FIELD(WDT_MR, WDIDLEHLT, 29, 1)
REG32(WDT_SR, 0x8)
	FIELD(WDT_SR, WDUNF, 0, 1)
	FIELD(WDT_SR, WDERR, 1, 1)


static void atsame70q21b_wdt_register_reset(ATSAME70Q21BWDT *t) {

}

static uint64_t atsame70q21b_wdt_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAME70Q21BWDT *t = ATSAME70Q21B_WDT(opaque);
	uint64_t ret;

	switch (offset) {
		case A_WDT_MR:
			ret = t->MR;
			break;
		case A_WDT_SR:
			ret = t->SR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAME70Q21B WDT read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsame70q21b_wdt_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAME70Q21BWDT *t = ATSAME70Q21B_WDT(opaque);


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
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAME70Q21B WDT write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsame70q21b_wdt_ops = {
	.read = atsame70q21b_wdt_read,
	.write = atsame70q21b_wdt_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsame70q21b_wdt_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAME70Q21BWDT *t = ATSAME70Q21B_WDT(obj);
	memory_region_init_io(&(t->iomem), obj, &atsame70q21b_wdt_ops, t, "atsame70q21b-wdt", ATSAME70Q21B_WDT_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void atsame70q21b_wdt_realize(DeviceState *dev, Error **errp) {
	return;

}

static void atsame70q21b_wdt_reset_enter(Object *obj, ResetType type) {
	ATSAME70Q21BWDT *t = ATSAME70Q21B_WDT(obj);
	atsame70q21b_wdt_register_reset(t);
}

static const VMStateDescription atsame70q21b_wdt_vmstate = {
	.name = "atsame70q21b-wdt",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR, ATSAME70Q21BWDT),
		VMSTATE_UINT32(MR, ATSAME70Q21BWDT),
		VMSTATE_UINT32(SR, ATSAME70Q21BWDT),

		VMSTATE_END_OF_LIST()
	}
};

static void atsame70q21b_wdt_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsame70q21b_wdt_vmstate;
	dc->realize = atsame70q21b_wdt_realize;
	rc->phases.enter = atsame70q21b_wdt_reset_enter;

}

static const TypeInfo atsame70q21b_wdt_info = {
	.name = TYPE_ATSAME70Q21B_WDT,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAME70Q21BWDT),
	.instance_init = atsame70q21b_wdt_init,
	.class_init = atsame70q21b_wdt_class_init,
};

static void atsame70q21b_wdt_register_types(void) {
	type_register_static(&atsame70q21b_wdt_info);
}

type_init(atsame70q21b_wdt_register_types);

#define TYPE_ATSAME70Q21B_SMC "atsame70q21b-smc"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAME70Q21BSMC, ATSAME70Q21B_SMC)

struct ATSAME70Q21BSMC {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/*registers*/
	uint32_t OCMS;
	uint32_t KEY1;
	uint32_t KEY2;
	uint32_t WPMR;
	uint32_t WPSR;
	uint32_t SMC_CS_NUMBER0_SETUP;
	uint32_t SMC_CS_NUMBER0_PULSE;
	uint32_t SMC_CS_NUMBER0_CYCLE;
	uint32_t SMC_CS_NUMBER0_MODE;
	uint32_t SMC_CS_NUMBER1_SETUP;
	uint32_t SMC_CS_NUMBER1_PULSE;
	uint32_t SMC_CS_NUMBER1_CYCLE;
	uint32_t SMC_CS_NUMBER1_MODE;
	uint32_t SMC_CS_NUMBER2_SETUP;
	uint32_t SMC_CS_NUMBER2_PULSE;
	uint32_t SMC_CS_NUMBER2_CYCLE;
	uint32_t SMC_CS_NUMBER2_MODE;
	uint32_t SMC_CS_NUMBER3_SETUP;
	uint32_t SMC_CS_NUMBER3_PULSE;
	uint32_t SMC_CS_NUMBER3_CYCLE;
	uint32_t SMC_CS_NUMBER3_MODE;
	/* base */
	uint32_t base;

};
#define ATSAME70Q21B_SMC_SIZE				0xec

REG32(SMC_OCMS, 0x80)
	FIELD(SMC_OCMS, SMSE, 0, 1)
	FIELD(SMC_OCMS, CS0SE, 8, 1)
	FIELD(SMC_OCMS, CS1SE, 9, 1)
	FIELD(SMC_OCMS, CS2SE, 10, 1)
	FIELD(SMC_OCMS, CS3SE, 11, 1)
REG32(SMC_KEY1, 0x84)
	FIELD(SMC_KEY1, KEY1, 0, 32)
REG32(SMC_KEY2, 0x88)
	FIELD(SMC_KEY2, KEY2, 0, 32)
REG32(SMC_WPMR, 0xe4)
	FIELD(SMC_WPMR, WPEN, 0, 1)
	FIELD(SMC_WPMR, WPKEY, 8, 24)
REG32(SMC_WPSR, 0xe8)
	FIELD(SMC_WPSR, WPVS, 0, 1)
	FIELD(SMC_WPSR, WPVSRC, 8, 16)
REG32(SMC_SMC_CS_NUMBER0_SETUP, 0x0)
	FIELD(SMC_SMC_CS_NUMBER0_SETUP, NWE_SETUP, 0, 6)
	FIELD(SMC_SMC_CS_NUMBER0_SETUP, NCS_WR_SETUP, 8, 6)
	FIELD(SMC_SMC_CS_NUMBER0_SETUP, NRD_SETUP, 16, 6)
	FIELD(SMC_SMC_CS_NUMBER0_SETUP, NCS_RD_SETUP, 24, 6)
REG32(SMC_SMC_CS_NUMBER0_PULSE, 0x4)
	FIELD(SMC_SMC_CS_NUMBER0_PULSE, NWE_PULSE, 0, 7)
	FIELD(SMC_SMC_CS_NUMBER0_PULSE, NCS_WR_PULSE, 8, 7)
	FIELD(SMC_SMC_CS_NUMBER0_PULSE, NRD_PULSE, 16, 7)
	FIELD(SMC_SMC_CS_NUMBER0_PULSE, NCS_RD_PULSE, 24, 7)
REG32(SMC_SMC_CS_NUMBER0_CYCLE, 0x8)
	FIELD(SMC_SMC_CS_NUMBER0_CYCLE, NWE_CYCLE, 0, 9)
	FIELD(SMC_SMC_CS_NUMBER0_CYCLE, NRD_CYCLE, 16, 9)
REG32(SMC_SMC_CS_NUMBER0_MODE, 0xc)
	FIELD(SMC_SMC_CS_NUMBER0_MODE, READ_MODE, 0, 1)
	FIELD(SMC_SMC_CS_NUMBER0_MODE, WRITE_MODE, 1, 1)
	FIELD(SMC_SMC_CS_NUMBER0_MODE, EXNW_MODE, 4, 2)
	FIELD(SMC_SMC_CS_NUMBER0_MODE, BAT, 8, 1)
	FIELD(SMC_SMC_CS_NUMBER0_MODE, DBW, 12, 1)
	FIELD(SMC_SMC_CS_NUMBER0_MODE, TDF_CYCLES, 16, 4)
	FIELD(SMC_SMC_CS_NUMBER0_MODE, TDF_MODE, 20, 1)
	FIELD(SMC_SMC_CS_NUMBER0_MODE, PMEN, 24, 1)
	FIELD(SMC_SMC_CS_NUMBER0_MODE, PS, 28, 2)
REG32(SMC_SMC_CS_NUMBER1_SETUP, 0x10)
	FIELD(SMC_SMC_CS_NUMBER1_SETUP, NWE_SETUP, 0, 6)
	FIELD(SMC_SMC_CS_NUMBER1_SETUP, NCS_WR_SETUP, 8, 6)
	FIELD(SMC_SMC_CS_NUMBER1_SETUP, NRD_SETUP, 16, 6)
	FIELD(SMC_SMC_CS_NUMBER1_SETUP, NCS_RD_SETUP, 24, 6)
REG32(SMC_SMC_CS_NUMBER1_PULSE, 0x14)
	FIELD(SMC_SMC_CS_NUMBER1_PULSE, NWE_PULSE, 0, 7)
	FIELD(SMC_SMC_CS_NUMBER1_PULSE, NCS_WR_PULSE, 8, 7)
	FIELD(SMC_SMC_CS_NUMBER1_PULSE, NRD_PULSE, 16, 7)
	FIELD(SMC_SMC_CS_NUMBER1_PULSE, NCS_RD_PULSE, 24, 7)
REG32(SMC_SMC_CS_NUMBER1_CYCLE, 0x18)
	FIELD(SMC_SMC_CS_NUMBER1_CYCLE, NWE_CYCLE, 0, 9)
	FIELD(SMC_SMC_CS_NUMBER1_CYCLE, NRD_CYCLE, 16, 9)
REG32(SMC_SMC_CS_NUMBER1_MODE, 0x1c)
	FIELD(SMC_SMC_CS_NUMBER1_MODE, READ_MODE, 0, 1)
	FIELD(SMC_SMC_CS_NUMBER1_MODE, WRITE_MODE, 1, 1)
	FIELD(SMC_SMC_CS_NUMBER1_MODE, EXNW_MODE, 4, 2)
	FIELD(SMC_SMC_CS_NUMBER1_MODE, BAT, 8, 1)
	FIELD(SMC_SMC_CS_NUMBER1_MODE, DBW, 12, 1)
	FIELD(SMC_SMC_CS_NUMBER1_MODE, TDF_CYCLES, 16, 4)
	FIELD(SMC_SMC_CS_NUMBER1_MODE, TDF_MODE, 20, 1)
	FIELD(SMC_SMC_CS_NUMBER1_MODE, PMEN, 24, 1)
	FIELD(SMC_SMC_CS_NUMBER1_MODE, PS, 28, 2)
REG32(SMC_SMC_CS_NUMBER2_SETUP, 0x20)
	FIELD(SMC_SMC_CS_NUMBER2_SETUP, NWE_SETUP, 0, 6)
	FIELD(SMC_SMC_CS_NUMBER2_SETUP, NCS_WR_SETUP, 8, 6)
	FIELD(SMC_SMC_CS_NUMBER2_SETUP, NRD_SETUP, 16, 6)
	FIELD(SMC_SMC_CS_NUMBER2_SETUP, NCS_RD_SETUP, 24, 6)
REG32(SMC_SMC_CS_NUMBER2_PULSE, 0x24)
	FIELD(SMC_SMC_CS_NUMBER2_PULSE, NWE_PULSE, 0, 7)
	FIELD(SMC_SMC_CS_NUMBER2_PULSE, NCS_WR_PULSE, 8, 7)
	FIELD(SMC_SMC_CS_NUMBER2_PULSE, NRD_PULSE, 16, 7)
	FIELD(SMC_SMC_CS_NUMBER2_PULSE, NCS_RD_PULSE, 24, 7)
REG32(SMC_SMC_CS_NUMBER2_CYCLE, 0x28)
	FIELD(SMC_SMC_CS_NUMBER2_CYCLE, NWE_CYCLE, 0, 9)
	FIELD(SMC_SMC_CS_NUMBER2_CYCLE, NRD_CYCLE, 16, 9)
REG32(SMC_SMC_CS_NUMBER2_MODE, 0x2c)
	FIELD(SMC_SMC_CS_NUMBER2_MODE, READ_MODE, 0, 1)
	FIELD(SMC_SMC_CS_NUMBER2_MODE, WRITE_MODE, 1, 1)
	FIELD(SMC_SMC_CS_NUMBER2_MODE, EXNW_MODE, 4, 2)
	FIELD(SMC_SMC_CS_NUMBER2_MODE, BAT, 8, 1)
	FIELD(SMC_SMC_CS_NUMBER2_MODE, DBW, 12, 1)
	FIELD(SMC_SMC_CS_NUMBER2_MODE, TDF_CYCLES, 16, 4)
	FIELD(SMC_SMC_CS_NUMBER2_MODE, TDF_MODE, 20, 1)
	FIELD(SMC_SMC_CS_NUMBER2_MODE, PMEN, 24, 1)
	FIELD(SMC_SMC_CS_NUMBER2_MODE, PS, 28, 2)
REG32(SMC_SMC_CS_NUMBER3_SETUP, 0x30)
	FIELD(SMC_SMC_CS_NUMBER3_SETUP, NWE_SETUP, 0, 6)
	FIELD(SMC_SMC_CS_NUMBER3_SETUP, NCS_WR_SETUP, 8, 6)
	FIELD(SMC_SMC_CS_NUMBER3_SETUP, NRD_SETUP, 16, 6)
	FIELD(SMC_SMC_CS_NUMBER3_SETUP, NCS_RD_SETUP, 24, 6)
REG32(SMC_SMC_CS_NUMBER3_PULSE, 0x34)
	FIELD(SMC_SMC_CS_NUMBER3_PULSE, NWE_PULSE, 0, 7)
	FIELD(SMC_SMC_CS_NUMBER3_PULSE, NCS_WR_PULSE, 8, 7)
	FIELD(SMC_SMC_CS_NUMBER3_PULSE, NRD_PULSE, 16, 7)
	FIELD(SMC_SMC_CS_NUMBER3_PULSE, NCS_RD_PULSE, 24, 7)
REG32(SMC_SMC_CS_NUMBER3_CYCLE, 0x38)
	FIELD(SMC_SMC_CS_NUMBER3_CYCLE, NWE_CYCLE, 0, 9)
	FIELD(SMC_SMC_CS_NUMBER3_CYCLE, NRD_CYCLE, 16, 9)
REG32(SMC_SMC_CS_NUMBER3_MODE, 0x3c)
	FIELD(SMC_SMC_CS_NUMBER3_MODE, READ_MODE, 0, 1)
	FIELD(SMC_SMC_CS_NUMBER3_MODE, WRITE_MODE, 1, 1)
	FIELD(SMC_SMC_CS_NUMBER3_MODE, EXNW_MODE, 4, 2)
	FIELD(SMC_SMC_CS_NUMBER3_MODE, BAT, 8, 1)
	FIELD(SMC_SMC_CS_NUMBER3_MODE, DBW, 12, 1)
	FIELD(SMC_SMC_CS_NUMBER3_MODE, TDF_CYCLES, 16, 4)
	FIELD(SMC_SMC_CS_NUMBER3_MODE, TDF_MODE, 20, 1)
	FIELD(SMC_SMC_CS_NUMBER3_MODE, PMEN, 24, 1)
	FIELD(SMC_SMC_CS_NUMBER3_MODE, PS, 28, 2)


static void atsame70q21b_smc_register_reset(ATSAME70Q21BSMC *t) {

}

static uint64_t atsame70q21b_smc_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAME70Q21BSMC *t = ATSAME70Q21B_SMC(opaque);
	uint64_t ret;

	switch (offset) {
		case A_SMC_OCMS:
			ret = t->OCMS;
			break;
		case A_SMC_WPMR:
			ret = t->WPMR;
			break;
		case A_SMC_WPSR:
			ret = t->WPSR;
			break;
		case A_SMC_SMC_CS_NUMBER0_SETUP:
			ret = t->SMC_CS_NUMBER0_SETUP;
			break;
		case A_SMC_SMC_CS_NUMBER0_PULSE:
			ret = t->SMC_CS_NUMBER0_PULSE;
			break;
		case A_SMC_SMC_CS_NUMBER0_CYCLE:
			ret = t->SMC_CS_NUMBER0_CYCLE;
			break;
		case A_SMC_SMC_CS_NUMBER0_MODE:
			ret = t->SMC_CS_NUMBER0_MODE;
			break;
		case A_SMC_SMC_CS_NUMBER1_SETUP:
			ret = t->SMC_CS_NUMBER1_SETUP;
			break;
		case A_SMC_SMC_CS_NUMBER1_PULSE:
			ret = t->SMC_CS_NUMBER1_PULSE;
			break;
		case A_SMC_SMC_CS_NUMBER1_CYCLE:
			ret = t->SMC_CS_NUMBER1_CYCLE;
			break;
		case A_SMC_SMC_CS_NUMBER1_MODE:
			ret = t->SMC_CS_NUMBER1_MODE;
			break;
		case A_SMC_SMC_CS_NUMBER2_SETUP:
			ret = t->SMC_CS_NUMBER2_SETUP;
			break;
		case A_SMC_SMC_CS_NUMBER2_PULSE:
			ret = t->SMC_CS_NUMBER2_PULSE;
			break;
		case A_SMC_SMC_CS_NUMBER2_CYCLE:
			ret = t->SMC_CS_NUMBER2_CYCLE;
			break;
		case A_SMC_SMC_CS_NUMBER2_MODE:
			ret = t->SMC_CS_NUMBER2_MODE;
			break;
		case A_SMC_SMC_CS_NUMBER3_SETUP:
			ret = t->SMC_CS_NUMBER3_SETUP;
			break;
		case A_SMC_SMC_CS_NUMBER3_PULSE:
			ret = t->SMC_CS_NUMBER3_PULSE;
			break;
		case A_SMC_SMC_CS_NUMBER3_CYCLE:
			ret = t->SMC_CS_NUMBER3_CYCLE;
			break;
		case A_SMC_SMC_CS_NUMBER3_MODE:
			ret = t->SMC_CS_NUMBER3_MODE;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAME70Q21B SMC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsame70q21b_smc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAME70Q21BSMC *t = ATSAME70Q21B_SMC(opaque);


	switch (offset) {
		case A_SMC_OCMS ... A_SMC_OCMS + 3:
			value = value << ((offset - A_SMC_OCMS) << 3);
			offset = A_SMC_OCMS;
			break;
		case A_SMC_KEY1 ... A_SMC_KEY1 + 3:
			value = value << ((offset - A_SMC_KEY1) << 3);
			offset = A_SMC_KEY1;
			break;
		case A_SMC_KEY2 ... A_SMC_KEY2 + 3:
			value = value << ((offset - A_SMC_KEY2) << 3);
			offset = A_SMC_KEY2;
			break;
		case A_SMC_WPMR ... A_SMC_WPMR + 3:
			value = value << ((offset - A_SMC_WPMR) << 3);
			offset = A_SMC_WPMR;
			break;
		case A_SMC_SMC_CS_NUMBER0_SETUP ... A_SMC_SMC_CS_NUMBER0_SETUP + 3:
			value = value << ((offset - A_SMC_SMC_CS_NUMBER0_SETUP) << 3);
			offset = A_SMC_SMC_CS_NUMBER0_SETUP;
			break;
		case A_SMC_SMC_CS_NUMBER0_PULSE ... A_SMC_SMC_CS_NUMBER0_PULSE + 3:
			value = value << ((offset - A_SMC_SMC_CS_NUMBER0_PULSE) << 3);
			offset = A_SMC_SMC_CS_NUMBER0_PULSE;
			break;
		case A_SMC_SMC_CS_NUMBER0_CYCLE ... A_SMC_SMC_CS_NUMBER0_CYCLE + 3:
			value = value << ((offset - A_SMC_SMC_CS_NUMBER0_CYCLE) << 3);
			offset = A_SMC_SMC_CS_NUMBER0_CYCLE;
			break;
		case A_SMC_SMC_CS_NUMBER0_MODE ... A_SMC_SMC_CS_NUMBER0_MODE + 3:
			value = value << ((offset - A_SMC_SMC_CS_NUMBER0_MODE) << 3);
			offset = A_SMC_SMC_CS_NUMBER0_MODE;
			break;
		case A_SMC_SMC_CS_NUMBER1_SETUP ... A_SMC_SMC_CS_NUMBER1_SETUP + 3:
			value = value << ((offset - A_SMC_SMC_CS_NUMBER1_SETUP) << 3);
			offset = A_SMC_SMC_CS_NUMBER1_SETUP;
			break;
		case A_SMC_SMC_CS_NUMBER1_PULSE ... A_SMC_SMC_CS_NUMBER1_PULSE + 3:
			value = value << ((offset - A_SMC_SMC_CS_NUMBER1_PULSE) << 3);
			offset = A_SMC_SMC_CS_NUMBER1_PULSE;
			break;
		case A_SMC_SMC_CS_NUMBER1_CYCLE ... A_SMC_SMC_CS_NUMBER1_CYCLE + 3:
			value = value << ((offset - A_SMC_SMC_CS_NUMBER1_CYCLE) << 3);
			offset = A_SMC_SMC_CS_NUMBER1_CYCLE;
			break;
		case A_SMC_SMC_CS_NUMBER1_MODE ... A_SMC_SMC_CS_NUMBER1_MODE + 3:
			value = value << ((offset - A_SMC_SMC_CS_NUMBER1_MODE) << 3);
			offset = A_SMC_SMC_CS_NUMBER1_MODE;
			break;
		case A_SMC_SMC_CS_NUMBER2_SETUP ... A_SMC_SMC_CS_NUMBER2_SETUP + 3:
			value = value << ((offset - A_SMC_SMC_CS_NUMBER2_SETUP) << 3);
			offset = A_SMC_SMC_CS_NUMBER2_SETUP;
			break;
		case A_SMC_SMC_CS_NUMBER2_PULSE ... A_SMC_SMC_CS_NUMBER2_PULSE + 3:
			value = value << ((offset - A_SMC_SMC_CS_NUMBER2_PULSE) << 3);
			offset = A_SMC_SMC_CS_NUMBER2_PULSE;
			break;
		case A_SMC_SMC_CS_NUMBER2_CYCLE ... A_SMC_SMC_CS_NUMBER2_CYCLE + 3:
			value = value << ((offset - A_SMC_SMC_CS_NUMBER2_CYCLE) << 3);
			offset = A_SMC_SMC_CS_NUMBER2_CYCLE;
			break;
		case A_SMC_SMC_CS_NUMBER2_MODE ... A_SMC_SMC_CS_NUMBER2_MODE + 3:
			value = value << ((offset - A_SMC_SMC_CS_NUMBER2_MODE) << 3);
			offset = A_SMC_SMC_CS_NUMBER2_MODE;
			break;
		case A_SMC_SMC_CS_NUMBER3_SETUP ... A_SMC_SMC_CS_NUMBER3_SETUP + 3:
			value = value << ((offset - A_SMC_SMC_CS_NUMBER3_SETUP) << 3);
			offset = A_SMC_SMC_CS_NUMBER3_SETUP;
			break;
		case A_SMC_SMC_CS_NUMBER3_PULSE ... A_SMC_SMC_CS_NUMBER3_PULSE + 3:
			value = value << ((offset - A_SMC_SMC_CS_NUMBER3_PULSE) << 3);
			offset = A_SMC_SMC_CS_NUMBER3_PULSE;
			break;
		case A_SMC_SMC_CS_NUMBER3_CYCLE ... A_SMC_SMC_CS_NUMBER3_CYCLE + 3:
			value = value << ((offset - A_SMC_SMC_CS_NUMBER3_CYCLE) << 3);
			offset = A_SMC_SMC_CS_NUMBER3_CYCLE;
			break;
		case A_SMC_SMC_CS_NUMBER3_MODE ... A_SMC_SMC_CS_NUMBER3_MODE + 3:
			value = value << ((offset - A_SMC_SMC_CS_NUMBER3_MODE) << 3);
			offset = A_SMC_SMC_CS_NUMBER3_MODE;
			break;

		default: break;
	}

	switch (offset) {
		case A_SMC_OCMS:
			t->OCMS = value;
			break;
		case A_SMC_KEY1:
			t->KEY1 = value;
			break;
		case A_SMC_KEY2:
			t->KEY2 = value;
			break;
		case A_SMC_WPMR:
			t->WPMR = value;
			break;
		case A_SMC_SMC_CS_NUMBER0_SETUP:
			t->SMC_CS_NUMBER0_SETUP = value;
			break;
		case A_SMC_SMC_CS_NUMBER0_PULSE:
			t->SMC_CS_NUMBER0_PULSE = value;
			break;
		case A_SMC_SMC_CS_NUMBER0_CYCLE:
			t->SMC_CS_NUMBER0_CYCLE = value;
			break;
		case A_SMC_SMC_CS_NUMBER0_MODE:
			t->SMC_CS_NUMBER0_MODE = value;
			break;
		case A_SMC_SMC_CS_NUMBER1_SETUP:
			t->SMC_CS_NUMBER1_SETUP = value;
			break;
		case A_SMC_SMC_CS_NUMBER1_PULSE:
			t->SMC_CS_NUMBER1_PULSE = value;
			break;
		case A_SMC_SMC_CS_NUMBER1_CYCLE:
			t->SMC_CS_NUMBER1_CYCLE = value;
			break;
		case A_SMC_SMC_CS_NUMBER1_MODE:
			t->SMC_CS_NUMBER1_MODE = value;
			break;
		case A_SMC_SMC_CS_NUMBER2_SETUP:
			t->SMC_CS_NUMBER2_SETUP = value;
			break;
		case A_SMC_SMC_CS_NUMBER2_PULSE:
			t->SMC_CS_NUMBER2_PULSE = value;
			break;
		case A_SMC_SMC_CS_NUMBER2_CYCLE:
			t->SMC_CS_NUMBER2_CYCLE = value;
			break;
		case A_SMC_SMC_CS_NUMBER2_MODE:
			t->SMC_CS_NUMBER2_MODE = value;
			break;
		case A_SMC_SMC_CS_NUMBER3_SETUP:
			t->SMC_CS_NUMBER3_SETUP = value;
			break;
		case A_SMC_SMC_CS_NUMBER3_PULSE:
			t->SMC_CS_NUMBER3_PULSE = value;
			break;
		case A_SMC_SMC_CS_NUMBER3_CYCLE:
			t->SMC_CS_NUMBER3_CYCLE = value;
			break;
		case A_SMC_SMC_CS_NUMBER3_MODE:
			t->SMC_CS_NUMBER3_MODE = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAME70Q21B SMC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsame70q21b_smc_ops = {
	.read = atsame70q21b_smc_read,
	.write = atsame70q21b_smc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsame70q21b_smc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAME70Q21BSMC *t = ATSAME70Q21B_SMC(obj);
	memory_region_init_io(&(t->iomem), obj, &atsame70q21b_smc_ops, t, "atsame70q21b-smc", ATSAME70Q21B_SMC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void atsame70q21b_smc_realize(DeviceState *dev, Error **errp) {
	return;

}

static void atsame70q21b_smc_reset_enter(Object *obj, ResetType type) {
	ATSAME70Q21BSMC *t = ATSAME70Q21B_SMC(obj);
	atsame70q21b_smc_register_reset(t);
}

static const VMStateDescription atsame70q21b_smc_vmstate = {
	.name = "atsame70q21b-smc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(OCMS, ATSAME70Q21BSMC),
		VMSTATE_UINT32(KEY1, ATSAME70Q21BSMC),
		VMSTATE_UINT32(KEY2, ATSAME70Q21BSMC),
		VMSTATE_UINT32(WPMR, ATSAME70Q21BSMC),
		VMSTATE_UINT32(WPSR, ATSAME70Q21BSMC),
		VMSTATE_UINT32(SMC_CS_NUMBER0_SETUP, ATSAME70Q21BSMC),
		VMSTATE_UINT32(SMC_CS_NUMBER0_PULSE, ATSAME70Q21BSMC),
		VMSTATE_UINT32(SMC_CS_NUMBER0_CYCLE, ATSAME70Q21BSMC),
		VMSTATE_UINT32(SMC_CS_NUMBER0_MODE, ATSAME70Q21BSMC),
		VMSTATE_UINT32(SMC_CS_NUMBER1_SETUP, ATSAME70Q21BSMC),
		VMSTATE_UINT32(SMC_CS_NUMBER1_PULSE, ATSAME70Q21BSMC),
		VMSTATE_UINT32(SMC_CS_NUMBER1_CYCLE, ATSAME70Q21BSMC),
		VMSTATE_UINT32(SMC_CS_NUMBER1_MODE, ATSAME70Q21BSMC),
		VMSTATE_UINT32(SMC_CS_NUMBER2_SETUP, ATSAME70Q21BSMC),
		VMSTATE_UINT32(SMC_CS_NUMBER2_PULSE, ATSAME70Q21BSMC),
		VMSTATE_UINT32(SMC_CS_NUMBER2_CYCLE, ATSAME70Q21BSMC),
		VMSTATE_UINT32(SMC_CS_NUMBER2_MODE, ATSAME70Q21BSMC),
		VMSTATE_UINT32(SMC_CS_NUMBER3_SETUP, ATSAME70Q21BSMC),
		VMSTATE_UINT32(SMC_CS_NUMBER3_PULSE, ATSAME70Q21BSMC),
		VMSTATE_UINT32(SMC_CS_NUMBER3_CYCLE, ATSAME70Q21BSMC),
		VMSTATE_UINT32(SMC_CS_NUMBER3_MODE, ATSAME70Q21BSMC),

		VMSTATE_END_OF_LIST()
	}
};

static void atsame70q21b_smc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsame70q21b_smc_vmstate;
	dc->realize = atsame70q21b_smc_realize;
	rc->phases.enter = atsame70q21b_smc_reset_enter;

}

static const TypeInfo atsame70q21b_smc_info = {
	.name = TYPE_ATSAME70Q21B_SMC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAME70Q21BSMC),
	.instance_init = atsame70q21b_smc_init,
	.class_init = atsame70q21b_smc_class_init,
};

static void atsame70q21b_smc_register_types(void) {
	type_register_static(&atsame70q21b_smc_info);
}

type_init(atsame70q21b_smc_register_types);

#define TYPE_ATSAME70Q21B_CHIPID "atsame70q21b-chipid"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAME70Q21BCHIPID, ATSAME70Q21B_CHIPID)

struct ATSAME70Q21BCHIPID {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/*registers*/
	uint32_t CIDR;
	uint32_t EXID;
	/* base */
	uint32_t base;

};
#define ATSAME70Q21B_CHIPID_SIZE				0x8

REG32(CHIPID_CIDR, 0x0)
	FIELD(CHIPID_CIDR, VERSION, 0, 5)
	FIELD(CHIPID_CIDR, EPROC, 5, 3)
	FIELD(CHIPID_CIDR, NVPSIZ, 8, 4)
	FIELD(CHIPID_CIDR, NVPSIZ2, 12, 4)
	FIELD(CHIPID_CIDR, SRAMSIZ, 16, 4)
	FIELD(CHIPID_CIDR, ARCH, 20, 8)
	FIELD(CHIPID_CIDR, NVPTYP, 28, 3)
	FIELD(CHIPID_CIDR, EXT, 31, 1)
REG32(CHIPID_EXID, 0x4)
	FIELD(CHIPID_EXID, EXID, 0, 32)


static void atsame70q21b_chipid_register_reset(ATSAME70Q21BCHIPID *t) {

}

static uint64_t atsame70q21b_chipid_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAME70Q21BCHIPID *t = ATSAME70Q21B_CHIPID(opaque);
	uint64_t ret;

	switch (offset) {
		case A_CHIPID_CIDR:
			ret = t->CIDR;
			break;
		case A_CHIPID_EXID:
			ret = t->EXID;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAME70Q21B CHIPID read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsame70q21b_chipid_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAME70Q21BCHIPID *t = ATSAME70Q21B_CHIPID(opaque);
	(void)t;


	switch (offset) {

		default: break;
	}

	switch (offset) {

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAME70Q21B CHIPID write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsame70q21b_chipid_ops = {
	.read = atsame70q21b_chipid_read,
	.write = atsame70q21b_chipid_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsame70q21b_chipid_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAME70Q21BCHIPID *t = ATSAME70Q21B_CHIPID(obj);
	memory_region_init_io(&(t->iomem), obj, &atsame70q21b_chipid_ops, t, "atsame70q21b-chipid", ATSAME70Q21B_CHIPID_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void atsame70q21b_chipid_realize(DeviceState *dev, Error **errp) {
	return;

}

static void atsame70q21b_chipid_reset_enter(Object *obj, ResetType type) {
	ATSAME70Q21BCHIPID *t = ATSAME70Q21B_CHIPID(obj);
	atsame70q21b_chipid_register_reset(t);
}

static const VMStateDescription atsame70q21b_chipid_vmstate = {
	.name = "atsame70q21b-chipid",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CIDR, ATSAME70Q21BCHIPID),
		VMSTATE_UINT32(EXID, ATSAME70Q21BCHIPID),

		VMSTATE_END_OF_LIST()
	}
};

static void atsame70q21b_chipid_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsame70q21b_chipid_vmstate;
	dc->realize = atsame70q21b_chipid_realize;
	rc->phases.enter = atsame70q21b_chipid_reset_enter;

}

static const TypeInfo atsame70q21b_chipid_info = {
	.name = TYPE_ATSAME70Q21B_CHIPID,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAME70Q21BCHIPID),
	.instance_init = atsame70q21b_chipid_init,
	.class_init = atsame70q21b_chipid_class_init,
};

static void atsame70q21b_chipid_register_types(void) {
	type_register_static(&atsame70q21b_chipid_info);
}

type_init(atsame70q21b_chipid_register_types);

#define TYPE_ATSAME70Q21B_MATRIX "atsame70q21b-matrix"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAME70Q21BMATRIX, ATSAME70Q21B_MATRIX)

struct ATSAME70Q21BMATRIX {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/*registers*/
	uint32_t MRCR;
	uint32_t CCFG_CAN0;
	uint32_t CCFG_SYSIO;
	uint32_t CCFG_PCCR;
	uint32_t CCFG_DYNCKG;
	uint32_t CCFG_SMCNFCS;
	uint32_t WPMR;
	uint32_t WPSR;
	uint32_t MCFG0;
	uint32_t MCFG1;
	uint32_t MCFG2;
	uint32_t MCFG3;
	uint32_t MCFG4;
	uint32_t MCFG5;
	uint32_t MCFG6;
	uint32_t MCFG7;
	uint32_t MCFG8;
	uint32_t MCFG9;
	uint32_t MCFG10;
	uint32_t MCFG11;
	uint32_t MCFG12;
	uint32_t SCFG0;
	uint32_t SCFG1;
	uint32_t SCFG2;
	uint32_t SCFG3;
	uint32_t SCFG4;
	uint32_t SCFG5;
	uint32_t SCFG6;
	uint32_t SCFG7;
	uint32_t SCFG8;
	uint32_t MATRIX_PR0_PRAS;
	uint32_t MATRIX_PR0_PRBS;
	uint32_t MATRIX_PR1_PRAS;
	uint32_t MATRIX_PR1_PRBS;
	uint32_t MATRIX_PR2_PRAS;
	uint32_t MATRIX_PR2_PRBS;
	uint32_t MATRIX_PR3_PRAS;
	uint32_t MATRIX_PR3_PRBS;
	uint32_t MATRIX_PR4_PRAS;
	uint32_t MATRIX_PR4_PRBS;
	uint32_t MATRIX_PR5_PRAS;
	uint32_t MATRIX_PR5_PRBS;
	uint32_t MATRIX_PR6_PRAS;
	uint32_t MATRIX_PR6_PRBS;
	uint32_t MATRIX_PR7_PRAS;
	uint32_t MATRIX_PR7_PRBS;
	uint32_t MATRIX_PR8_PRAS;
	uint32_t MATRIX_PR8_PRBS;
	/* base */
	uint32_t base;

};
#define ATSAME70Q21B_MATRIX_SIZE				0x1ec

REG32(MATRIX_MRCR, 0x100)
	FIELD(MATRIX_MRCR, RCB0, 0, 1)
	FIELD(MATRIX_MRCR, RCB1, 1, 1)
	FIELD(MATRIX_MRCR, RCB2, 2, 1)
	FIELD(MATRIX_MRCR, RCB3, 3, 1)
	FIELD(MATRIX_MRCR, RCB4, 4, 1)
	FIELD(MATRIX_MRCR, RCB5, 5, 1)
	FIELD(MATRIX_MRCR, RCB6, 6, 1)
	FIELD(MATRIX_MRCR, RCB8, 8, 1)
	FIELD(MATRIX_MRCR, RCB9, 9, 1)
	FIELD(MATRIX_MRCR, RCB10, 10, 1)
	FIELD(MATRIX_MRCR, RCB11, 11, 1)
	FIELD(MATRIX_MRCR, RCB12, 12, 1)
REG32(MATRIX_CCFG_CAN0, 0x110)
	FIELD(MATRIX_CCFG_CAN0, CAN0DMABA, 16, 16)
REG32(MATRIX_CCFG_SYSIO, 0x114)
	FIELD(MATRIX_CCFG_SYSIO, SYSIO4, 4, 1)
	FIELD(MATRIX_CCFG_SYSIO, SYSIO5, 5, 1)
	FIELD(MATRIX_CCFG_SYSIO, SYSIO6, 6, 1)
	FIELD(MATRIX_CCFG_SYSIO, SYSIO7, 7, 1)
	FIELD(MATRIX_CCFG_SYSIO, SYSIO12, 12, 1)
	FIELD(MATRIX_CCFG_SYSIO, CAN1DMABA, 16, 16)
REG32(MATRIX_CCFG_PCCR, 0x118)
	FIELD(MATRIX_CCFG_PCCR, TC0CC, 20, 1)
	FIELD(MATRIX_CCFG_PCCR, I2SC0CC, 21, 1)
	FIELD(MATRIX_CCFG_PCCR, I2SC1CC, 22, 1)
REG32(MATRIX_CCFG_DYNCKG, 0x11c)
	FIELD(MATRIX_CCFG_DYNCKG, MATCKG, 0, 1)
	FIELD(MATRIX_CCFG_DYNCKG, BRIDCKG, 1, 1)
	FIELD(MATRIX_CCFG_DYNCKG, EFCCKG, 2, 1)
REG32(MATRIX_CCFG_SMCNFCS, 0x124)
	FIELD(MATRIX_CCFG_SMCNFCS, SMC_NFCS0, 0, 1)
	FIELD(MATRIX_CCFG_SMCNFCS, SMC_NFCS1, 1, 1)
	FIELD(MATRIX_CCFG_SMCNFCS, SMC_NFCS2, 2, 1)
	FIELD(MATRIX_CCFG_SMCNFCS, SMC_NFCS3, 3, 1)
	FIELD(MATRIX_CCFG_SMCNFCS, SDRAMEN, 4, 1)
REG32(MATRIX_WPMR, 0x1e4)
	FIELD(MATRIX_WPMR, WPEN, 0, 1)
	FIELD(MATRIX_WPMR, WPKEY, 8, 24)
REG32(MATRIX_WPSR, 0x1e8)
	FIELD(MATRIX_WPSR, WPVS, 0, 1)
	FIELD(MATRIX_WPSR, WPVSRC, 8, 16)
REG32(MATRIX_MCFG0, 0x0)
	FIELD(MATRIX_MCFG0, ULBT, 0, 3)
REG32(MATRIX_MCFG1, 0x4)
	FIELD(MATRIX_MCFG1, ULBT, 0, 3)
REG32(MATRIX_MCFG2, 0x8)
	FIELD(MATRIX_MCFG2, ULBT, 0, 3)
REG32(MATRIX_MCFG3, 0xc)
	FIELD(MATRIX_MCFG3, ULBT, 0, 3)
REG32(MATRIX_MCFG4, 0x10)
	FIELD(MATRIX_MCFG4, ULBT, 0, 3)
REG32(MATRIX_MCFG5, 0x14)
	FIELD(MATRIX_MCFG5, ULBT, 0, 3)
REG32(MATRIX_MCFG6, 0x18)
	FIELD(MATRIX_MCFG6, ULBT, 0, 3)
REG32(MATRIX_MCFG7, 0x1c)
	FIELD(MATRIX_MCFG7, ULBT, 0, 3)
REG32(MATRIX_MCFG8, 0x20)
	FIELD(MATRIX_MCFG8, ULBT, 0, 3)
REG32(MATRIX_MCFG9, 0x24)
	FIELD(MATRIX_MCFG9, ULBT, 0, 3)
REG32(MATRIX_MCFG10, 0x28)
	FIELD(MATRIX_MCFG10, ULBT, 0, 3)
REG32(MATRIX_MCFG11, 0x2c)
	FIELD(MATRIX_MCFG11, ULBT, 0, 3)
REG32(MATRIX_MCFG12, 0x30)
	FIELD(MATRIX_MCFG12, ULBT, 0, 3)
REG32(MATRIX_SCFG0, 0x40)
	FIELD(MATRIX_SCFG0, SLOT_CYCLE, 0, 9)
	FIELD(MATRIX_SCFG0, DEFMSTR_TYPE, 16, 2)
	FIELD(MATRIX_SCFG0, FIXED_DEFMSTR, 18, 4)
REG32(MATRIX_SCFG1, 0x44)
	FIELD(MATRIX_SCFG1, SLOT_CYCLE, 0, 9)
	FIELD(MATRIX_SCFG1, DEFMSTR_TYPE, 16, 2)
	FIELD(MATRIX_SCFG1, FIXED_DEFMSTR, 18, 4)
REG32(MATRIX_SCFG2, 0x48)
	FIELD(MATRIX_SCFG2, SLOT_CYCLE, 0, 9)
	FIELD(MATRIX_SCFG2, DEFMSTR_TYPE, 16, 2)
	FIELD(MATRIX_SCFG2, FIXED_DEFMSTR, 18, 4)
REG32(MATRIX_SCFG3, 0x4c)
	FIELD(MATRIX_SCFG3, SLOT_CYCLE, 0, 9)
	FIELD(MATRIX_SCFG3, DEFMSTR_TYPE, 16, 2)
	FIELD(MATRIX_SCFG3, FIXED_DEFMSTR, 18, 4)
REG32(MATRIX_SCFG4, 0x50)
	FIELD(MATRIX_SCFG4, SLOT_CYCLE, 0, 9)
	FIELD(MATRIX_SCFG4, DEFMSTR_TYPE, 16, 2)
	FIELD(MATRIX_SCFG4, FIXED_DEFMSTR, 18, 4)
REG32(MATRIX_SCFG5, 0x54)
	FIELD(MATRIX_SCFG5, SLOT_CYCLE, 0, 9)
	FIELD(MATRIX_SCFG5, DEFMSTR_TYPE, 16, 2)
	FIELD(MATRIX_SCFG5, FIXED_DEFMSTR, 18, 4)
REG32(MATRIX_SCFG6, 0x58)
	FIELD(MATRIX_SCFG6, SLOT_CYCLE, 0, 9)
	FIELD(MATRIX_SCFG6, DEFMSTR_TYPE, 16, 2)
	FIELD(MATRIX_SCFG6, FIXED_DEFMSTR, 18, 4)
REG32(MATRIX_SCFG7, 0x5c)
	FIELD(MATRIX_SCFG7, SLOT_CYCLE, 0, 9)
	FIELD(MATRIX_SCFG7, DEFMSTR_TYPE, 16, 2)
	FIELD(MATRIX_SCFG7, FIXED_DEFMSTR, 18, 4)
REG32(MATRIX_SCFG8, 0x60)
	FIELD(MATRIX_SCFG8, SLOT_CYCLE, 0, 9)
	FIELD(MATRIX_SCFG8, DEFMSTR_TYPE, 16, 2)
	FIELD(MATRIX_SCFG8, FIXED_DEFMSTR, 18, 4)
REG32(MATRIX_MATRIX_PR0_PRAS, 0x80)
	FIELD(MATRIX_MATRIX_PR0_PRAS, M0PR, 0, 2)
	FIELD(MATRIX_MATRIX_PR0_PRAS, M1PR, 4, 2)
	FIELD(MATRIX_MATRIX_PR0_PRAS, M2PR, 8, 2)
	FIELD(MATRIX_MATRIX_PR0_PRAS, M3PR, 12, 2)
	FIELD(MATRIX_MATRIX_PR0_PRAS, M4PR, 16, 2)
	FIELD(MATRIX_MATRIX_PR0_PRAS, M5PR, 20, 2)
	FIELD(MATRIX_MATRIX_PR0_PRAS, M6PR, 24, 2)
REG32(MATRIX_MATRIX_PR0_PRBS, 0x84)
	FIELD(MATRIX_MATRIX_PR0_PRBS, M8PR, 0, 2)
	FIELD(MATRIX_MATRIX_PR0_PRBS, M9PR, 4, 2)
	FIELD(MATRIX_MATRIX_PR0_PRBS, M10PR, 8, 2)
	FIELD(MATRIX_MATRIX_PR0_PRBS, M11PR, 12, 2)
	FIELD(MATRIX_MATRIX_PR0_PRBS, M12PR, 16, 2)
REG32(MATRIX_MATRIX_PR1_PRAS, 0x88)
	FIELD(MATRIX_MATRIX_PR1_PRAS, M0PR, 0, 2)
	FIELD(MATRIX_MATRIX_PR1_PRAS, M1PR, 4, 2)
	FIELD(MATRIX_MATRIX_PR1_PRAS, M2PR, 8, 2)
	FIELD(MATRIX_MATRIX_PR1_PRAS, M3PR, 12, 2)
	FIELD(MATRIX_MATRIX_PR1_PRAS, M4PR, 16, 2)
	FIELD(MATRIX_MATRIX_PR1_PRAS, M5PR, 20, 2)
	FIELD(MATRIX_MATRIX_PR1_PRAS, M6PR, 24, 2)
REG32(MATRIX_MATRIX_PR1_PRBS, 0x8c)
	FIELD(MATRIX_MATRIX_PR1_PRBS, M8PR, 0, 2)
	FIELD(MATRIX_MATRIX_PR1_PRBS, M9PR, 4, 2)
	FIELD(MATRIX_MATRIX_PR1_PRBS, M10PR, 8, 2)
	FIELD(MATRIX_MATRIX_PR1_PRBS, M11PR, 12, 2)
	FIELD(MATRIX_MATRIX_PR1_PRBS, M12PR, 16, 2)
REG32(MATRIX_MATRIX_PR2_PRAS, 0x90)
	FIELD(MATRIX_MATRIX_PR2_PRAS, M0PR, 0, 2)
	FIELD(MATRIX_MATRIX_PR2_PRAS, M1PR, 4, 2)
	FIELD(MATRIX_MATRIX_PR2_PRAS, M2PR, 8, 2)
	FIELD(MATRIX_MATRIX_PR2_PRAS, M3PR, 12, 2)
	FIELD(MATRIX_MATRIX_PR2_PRAS, M4PR, 16, 2)
	FIELD(MATRIX_MATRIX_PR2_PRAS, M5PR, 20, 2)
	FIELD(MATRIX_MATRIX_PR2_PRAS, M6PR, 24, 2)
REG32(MATRIX_MATRIX_PR2_PRBS, 0x94)
	FIELD(MATRIX_MATRIX_PR2_PRBS, M8PR, 0, 2)
	FIELD(MATRIX_MATRIX_PR2_PRBS, M9PR, 4, 2)
	FIELD(MATRIX_MATRIX_PR2_PRBS, M10PR, 8, 2)
	FIELD(MATRIX_MATRIX_PR2_PRBS, M11PR, 12, 2)
	FIELD(MATRIX_MATRIX_PR2_PRBS, M12PR, 16, 2)
REG32(MATRIX_MATRIX_PR3_PRAS, 0x98)
	FIELD(MATRIX_MATRIX_PR3_PRAS, M0PR, 0, 2)
	FIELD(MATRIX_MATRIX_PR3_PRAS, M1PR, 4, 2)
	FIELD(MATRIX_MATRIX_PR3_PRAS, M2PR, 8, 2)
	FIELD(MATRIX_MATRIX_PR3_PRAS, M3PR, 12, 2)
	FIELD(MATRIX_MATRIX_PR3_PRAS, M4PR, 16, 2)
	FIELD(MATRIX_MATRIX_PR3_PRAS, M5PR, 20, 2)
	FIELD(MATRIX_MATRIX_PR3_PRAS, M6PR, 24, 2)
REG32(MATRIX_MATRIX_PR3_PRBS, 0x9c)
	FIELD(MATRIX_MATRIX_PR3_PRBS, M8PR, 0, 2)
	FIELD(MATRIX_MATRIX_PR3_PRBS, M9PR, 4, 2)
	FIELD(MATRIX_MATRIX_PR3_PRBS, M10PR, 8, 2)
	FIELD(MATRIX_MATRIX_PR3_PRBS, M11PR, 12, 2)
	FIELD(MATRIX_MATRIX_PR3_PRBS, M12PR, 16, 2)
REG32(MATRIX_MATRIX_PR4_PRAS, 0xa0)
	FIELD(MATRIX_MATRIX_PR4_PRAS, M0PR, 0, 2)
	FIELD(MATRIX_MATRIX_PR4_PRAS, M1PR, 4, 2)
	FIELD(MATRIX_MATRIX_PR4_PRAS, M2PR, 8, 2)
	FIELD(MATRIX_MATRIX_PR4_PRAS, M3PR, 12, 2)
	FIELD(MATRIX_MATRIX_PR4_PRAS, M4PR, 16, 2)
	FIELD(MATRIX_MATRIX_PR4_PRAS, M5PR, 20, 2)
	FIELD(MATRIX_MATRIX_PR4_PRAS, M6PR, 24, 2)
REG32(MATRIX_MATRIX_PR4_PRBS, 0xa4)
	FIELD(MATRIX_MATRIX_PR4_PRBS, M8PR, 0, 2)
	FIELD(MATRIX_MATRIX_PR4_PRBS, M9PR, 4, 2)
	FIELD(MATRIX_MATRIX_PR4_PRBS, M10PR, 8, 2)
	FIELD(MATRIX_MATRIX_PR4_PRBS, M11PR, 12, 2)
	FIELD(MATRIX_MATRIX_PR4_PRBS, M12PR, 16, 2)
REG32(MATRIX_MATRIX_PR5_PRAS, 0xa8)
	FIELD(MATRIX_MATRIX_PR5_PRAS, M0PR, 0, 2)
	FIELD(MATRIX_MATRIX_PR5_PRAS, M1PR, 4, 2)
	FIELD(MATRIX_MATRIX_PR5_PRAS, M2PR, 8, 2)
	FIELD(MATRIX_MATRIX_PR5_PRAS, M3PR, 12, 2)
	FIELD(MATRIX_MATRIX_PR5_PRAS, M4PR, 16, 2)
	FIELD(MATRIX_MATRIX_PR5_PRAS, M5PR, 20, 2)
	FIELD(MATRIX_MATRIX_PR5_PRAS, M6PR, 24, 2)
REG32(MATRIX_MATRIX_PR5_PRBS, 0xac)
	FIELD(MATRIX_MATRIX_PR5_PRBS, M8PR, 0, 2)
	FIELD(MATRIX_MATRIX_PR5_PRBS, M9PR, 4, 2)
	FIELD(MATRIX_MATRIX_PR5_PRBS, M10PR, 8, 2)
	FIELD(MATRIX_MATRIX_PR5_PRBS, M11PR, 12, 2)
	FIELD(MATRIX_MATRIX_PR5_PRBS, M12PR, 16, 2)
REG32(MATRIX_MATRIX_PR6_PRAS, 0xb0)
	FIELD(MATRIX_MATRIX_PR6_PRAS, M0PR, 0, 2)
	FIELD(MATRIX_MATRIX_PR6_PRAS, M1PR, 4, 2)
	FIELD(MATRIX_MATRIX_PR6_PRAS, M2PR, 8, 2)
	FIELD(MATRIX_MATRIX_PR6_PRAS, M3PR, 12, 2)
	FIELD(MATRIX_MATRIX_PR6_PRAS, M4PR, 16, 2)
	FIELD(MATRIX_MATRIX_PR6_PRAS, M5PR, 20, 2)
	FIELD(MATRIX_MATRIX_PR6_PRAS, M6PR, 24, 2)
REG32(MATRIX_MATRIX_PR6_PRBS, 0xb4)
	FIELD(MATRIX_MATRIX_PR6_PRBS, M8PR, 0, 2)
	FIELD(MATRIX_MATRIX_PR6_PRBS, M9PR, 4, 2)
	FIELD(MATRIX_MATRIX_PR6_PRBS, M10PR, 8, 2)
	FIELD(MATRIX_MATRIX_PR6_PRBS, M11PR, 12, 2)
	FIELD(MATRIX_MATRIX_PR6_PRBS, M12PR, 16, 2)
REG32(MATRIX_MATRIX_PR7_PRAS, 0xb8)
	FIELD(MATRIX_MATRIX_PR7_PRAS, M0PR, 0, 2)
	FIELD(MATRIX_MATRIX_PR7_PRAS, M1PR, 4, 2)
	FIELD(MATRIX_MATRIX_PR7_PRAS, M2PR, 8, 2)
	FIELD(MATRIX_MATRIX_PR7_PRAS, M3PR, 12, 2)
	FIELD(MATRIX_MATRIX_PR7_PRAS, M4PR, 16, 2)
	FIELD(MATRIX_MATRIX_PR7_PRAS, M5PR, 20, 2)
	FIELD(MATRIX_MATRIX_PR7_PRAS, M6PR, 24, 2)
REG32(MATRIX_MATRIX_PR7_PRBS, 0xbc)
	FIELD(MATRIX_MATRIX_PR7_PRBS, M8PR, 0, 2)
	FIELD(MATRIX_MATRIX_PR7_PRBS, M9PR, 4, 2)
	FIELD(MATRIX_MATRIX_PR7_PRBS, M10PR, 8, 2)
	FIELD(MATRIX_MATRIX_PR7_PRBS, M11PR, 12, 2)
	FIELD(MATRIX_MATRIX_PR7_PRBS, M12PR, 16, 2)
REG32(MATRIX_MATRIX_PR8_PRAS, 0xc0)
	FIELD(MATRIX_MATRIX_PR8_PRAS, M0PR, 0, 2)
	FIELD(MATRIX_MATRIX_PR8_PRAS, M1PR, 4, 2)
	FIELD(MATRIX_MATRIX_PR8_PRAS, M2PR, 8, 2)
	FIELD(MATRIX_MATRIX_PR8_PRAS, M3PR, 12, 2)
	FIELD(MATRIX_MATRIX_PR8_PRAS, M4PR, 16, 2)
	FIELD(MATRIX_MATRIX_PR8_PRAS, M5PR, 20, 2)
	FIELD(MATRIX_MATRIX_PR8_PRAS, M6PR, 24, 2)
REG32(MATRIX_MATRIX_PR8_PRBS, 0xc4)
	FIELD(MATRIX_MATRIX_PR8_PRBS, M8PR, 0, 2)
	FIELD(MATRIX_MATRIX_PR8_PRBS, M9PR, 4, 2)
	FIELD(MATRIX_MATRIX_PR8_PRBS, M10PR, 8, 2)
	FIELD(MATRIX_MATRIX_PR8_PRBS, M11PR, 12, 2)
	FIELD(MATRIX_MATRIX_PR8_PRBS, M12PR, 16, 2)


static void atsame70q21b_matrix_register_reset(ATSAME70Q21BMATRIX *t) {

}

static uint64_t atsame70q21b_matrix_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAME70Q21BMATRIX *t = ATSAME70Q21B_MATRIX(opaque);
	uint64_t ret;

	switch (offset) {
		case A_MATRIX_MRCR:
			ret = t->MRCR;
			break;
		case A_MATRIX_CCFG_CAN0:
			ret = t->CCFG_CAN0;
			break;
		case A_MATRIX_CCFG_SYSIO:
			ret = t->CCFG_SYSIO;
			break;
		case A_MATRIX_CCFG_PCCR:
			ret = t->CCFG_PCCR;
			break;
		case A_MATRIX_CCFG_DYNCKG:
			ret = t->CCFG_DYNCKG;
			break;
		case A_MATRIX_CCFG_SMCNFCS:
			ret = t->CCFG_SMCNFCS;
			break;
		case A_MATRIX_WPMR:
			ret = t->WPMR;
			break;
		case A_MATRIX_WPSR:
			ret = t->WPSR;
			break;
		case A_MATRIX_MCFG0:
			ret = t->MCFG0;
			break;
		case A_MATRIX_MCFG1:
			ret = t->MCFG1;
			break;
		case A_MATRIX_MCFG2:
			ret = t->MCFG2;
			break;
		case A_MATRIX_MCFG3:
			ret = t->MCFG3;
			break;
		case A_MATRIX_MCFG4:
			ret = t->MCFG4;
			break;
		case A_MATRIX_MCFG5:
			ret = t->MCFG5;
			break;
		case A_MATRIX_MCFG6:
			ret = t->MCFG6;
			break;
		case A_MATRIX_MCFG7:
			ret = t->MCFG7;
			break;
		case A_MATRIX_MCFG8:
			ret = t->MCFG8;
			break;
		case A_MATRIX_MCFG9:
			ret = t->MCFG9;
			break;
		case A_MATRIX_MCFG10:
			ret = t->MCFG10;
			break;
		case A_MATRIX_MCFG11:
			ret = t->MCFG11;
			break;
		case A_MATRIX_MCFG12:
			ret = t->MCFG12;
			break;
		case A_MATRIX_SCFG0:
			ret = t->SCFG0;
			break;
		case A_MATRIX_SCFG1:
			ret = t->SCFG1;
			break;
		case A_MATRIX_SCFG2:
			ret = t->SCFG2;
			break;
		case A_MATRIX_SCFG3:
			ret = t->SCFG3;
			break;
		case A_MATRIX_SCFG4:
			ret = t->SCFG4;
			break;
		case A_MATRIX_SCFG5:
			ret = t->SCFG5;
			break;
		case A_MATRIX_SCFG6:
			ret = t->SCFG6;
			break;
		case A_MATRIX_SCFG7:
			ret = t->SCFG7;
			break;
		case A_MATRIX_SCFG8:
			ret = t->SCFG8;
			break;
		case A_MATRIX_MATRIX_PR0_PRAS:
			ret = t->MATRIX_PR0_PRAS;
			break;
		case A_MATRIX_MATRIX_PR0_PRBS:
			ret = t->MATRIX_PR0_PRBS;
			break;
		case A_MATRIX_MATRIX_PR1_PRAS:
			ret = t->MATRIX_PR1_PRAS;
			break;
		case A_MATRIX_MATRIX_PR1_PRBS:
			ret = t->MATRIX_PR1_PRBS;
			break;
		case A_MATRIX_MATRIX_PR2_PRAS:
			ret = t->MATRIX_PR2_PRAS;
			break;
		case A_MATRIX_MATRIX_PR2_PRBS:
			ret = t->MATRIX_PR2_PRBS;
			break;
		case A_MATRIX_MATRIX_PR3_PRAS:
			ret = t->MATRIX_PR3_PRAS;
			break;
		case A_MATRIX_MATRIX_PR3_PRBS:
			ret = t->MATRIX_PR3_PRBS;
			break;
		case A_MATRIX_MATRIX_PR4_PRAS:
			ret = t->MATRIX_PR4_PRAS;
			break;
		case A_MATRIX_MATRIX_PR4_PRBS:
			ret = t->MATRIX_PR4_PRBS;
			break;
		case A_MATRIX_MATRIX_PR5_PRAS:
			ret = t->MATRIX_PR5_PRAS;
			break;
		case A_MATRIX_MATRIX_PR5_PRBS:
			ret = t->MATRIX_PR5_PRBS;
			break;
		case A_MATRIX_MATRIX_PR6_PRAS:
			ret = t->MATRIX_PR6_PRAS;
			break;
		case A_MATRIX_MATRIX_PR6_PRBS:
			ret = t->MATRIX_PR6_PRBS;
			break;
		case A_MATRIX_MATRIX_PR7_PRAS:
			ret = t->MATRIX_PR7_PRAS;
			break;
		case A_MATRIX_MATRIX_PR7_PRBS:
			ret = t->MATRIX_PR7_PRBS;
			break;
		case A_MATRIX_MATRIX_PR8_PRAS:
			ret = t->MATRIX_PR8_PRAS;
			break;
		case A_MATRIX_MATRIX_PR8_PRBS:
			ret = t->MATRIX_PR8_PRBS;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAME70Q21B MATRIX read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsame70q21b_matrix_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAME70Q21BMATRIX *t = ATSAME70Q21B_MATRIX(opaque);


	switch (offset) {
		case A_MATRIX_MRCR ... A_MATRIX_MRCR + 3:
			value = value << ((offset - A_MATRIX_MRCR) << 3);
			offset = A_MATRIX_MRCR;
			break;
		case A_MATRIX_CCFG_CAN0 ... A_MATRIX_CCFG_CAN0 + 3:
			value = value << ((offset - A_MATRIX_CCFG_CAN0) << 3);
			offset = A_MATRIX_CCFG_CAN0;
			break;
		case A_MATRIX_CCFG_SYSIO ... A_MATRIX_CCFG_SYSIO + 3:
			value = value << ((offset - A_MATRIX_CCFG_SYSIO) << 3);
			offset = A_MATRIX_CCFG_SYSIO;
			break;
		case A_MATRIX_CCFG_PCCR ... A_MATRIX_CCFG_PCCR + 3:
			value = value << ((offset - A_MATRIX_CCFG_PCCR) << 3);
			offset = A_MATRIX_CCFG_PCCR;
			break;
		case A_MATRIX_CCFG_DYNCKG ... A_MATRIX_CCFG_DYNCKG + 3:
			value = value << ((offset - A_MATRIX_CCFG_DYNCKG) << 3);
			offset = A_MATRIX_CCFG_DYNCKG;
			break;
		case A_MATRIX_CCFG_SMCNFCS ... A_MATRIX_CCFG_SMCNFCS + 3:
			value = value << ((offset - A_MATRIX_CCFG_SMCNFCS) << 3);
			offset = A_MATRIX_CCFG_SMCNFCS;
			break;
		case A_MATRIX_WPMR ... A_MATRIX_WPMR + 3:
			value = value << ((offset - A_MATRIX_WPMR) << 3);
			offset = A_MATRIX_WPMR;
			break;
		case A_MATRIX_MCFG0 ... A_MATRIX_MCFG0 + 3:
			value = value << ((offset - A_MATRIX_MCFG0) << 3);
			offset = A_MATRIX_MCFG0;
			break;
		case A_MATRIX_MCFG1 ... A_MATRIX_MCFG1 + 3:
			value = value << ((offset - A_MATRIX_MCFG1) << 3);
			offset = A_MATRIX_MCFG1;
			break;
		case A_MATRIX_MCFG2 ... A_MATRIX_MCFG2 + 3:
			value = value << ((offset - A_MATRIX_MCFG2) << 3);
			offset = A_MATRIX_MCFG2;
			break;
		case A_MATRIX_MCFG3 ... A_MATRIX_MCFG3 + 3:
			value = value << ((offset - A_MATRIX_MCFG3) << 3);
			offset = A_MATRIX_MCFG3;
			break;
		case A_MATRIX_MCFG4 ... A_MATRIX_MCFG4 + 3:
			value = value << ((offset - A_MATRIX_MCFG4) << 3);
			offset = A_MATRIX_MCFG4;
			break;
		case A_MATRIX_MCFG5 ... A_MATRIX_MCFG5 + 3:
			value = value << ((offset - A_MATRIX_MCFG5) << 3);
			offset = A_MATRIX_MCFG5;
			break;
		case A_MATRIX_MCFG6 ... A_MATRIX_MCFG6 + 3:
			value = value << ((offset - A_MATRIX_MCFG6) << 3);
			offset = A_MATRIX_MCFG6;
			break;
		case A_MATRIX_MCFG7 ... A_MATRIX_MCFG7 + 3:
			value = value << ((offset - A_MATRIX_MCFG7) << 3);
			offset = A_MATRIX_MCFG7;
			break;
		case A_MATRIX_MCFG8 ... A_MATRIX_MCFG8 + 3:
			value = value << ((offset - A_MATRIX_MCFG8) << 3);
			offset = A_MATRIX_MCFG8;
			break;
		case A_MATRIX_MCFG9 ... A_MATRIX_MCFG9 + 3:
			value = value << ((offset - A_MATRIX_MCFG9) << 3);
			offset = A_MATRIX_MCFG9;
			break;
		case A_MATRIX_MCFG10 ... A_MATRIX_MCFG10 + 3:
			value = value << ((offset - A_MATRIX_MCFG10) << 3);
			offset = A_MATRIX_MCFG10;
			break;
		case A_MATRIX_MCFG11 ... A_MATRIX_MCFG11 + 3:
			value = value << ((offset - A_MATRIX_MCFG11) << 3);
			offset = A_MATRIX_MCFG11;
			break;
		case A_MATRIX_MCFG12 ... A_MATRIX_MCFG12 + 3:
			value = value << ((offset - A_MATRIX_MCFG12) << 3);
			offset = A_MATRIX_MCFG12;
			break;
		case A_MATRIX_SCFG0 ... A_MATRIX_SCFG0 + 3:
			value = value << ((offset - A_MATRIX_SCFG0) << 3);
			offset = A_MATRIX_SCFG0;
			break;
		case A_MATRIX_SCFG1 ... A_MATRIX_SCFG1 + 3:
			value = value << ((offset - A_MATRIX_SCFG1) << 3);
			offset = A_MATRIX_SCFG1;
			break;
		case A_MATRIX_SCFG2 ... A_MATRIX_SCFG2 + 3:
			value = value << ((offset - A_MATRIX_SCFG2) << 3);
			offset = A_MATRIX_SCFG2;
			break;
		case A_MATRIX_SCFG3 ... A_MATRIX_SCFG3 + 3:
			value = value << ((offset - A_MATRIX_SCFG3) << 3);
			offset = A_MATRIX_SCFG3;
			break;
		case A_MATRIX_SCFG4 ... A_MATRIX_SCFG4 + 3:
			value = value << ((offset - A_MATRIX_SCFG4) << 3);
			offset = A_MATRIX_SCFG4;
			break;
		case A_MATRIX_SCFG5 ... A_MATRIX_SCFG5 + 3:
			value = value << ((offset - A_MATRIX_SCFG5) << 3);
			offset = A_MATRIX_SCFG5;
			break;
		case A_MATRIX_SCFG6 ... A_MATRIX_SCFG6 + 3:
			value = value << ((offset - A_MATRIX_SCFG6) << 3);
			offset = A_MATRIX_SCFG6;
			break;
		case A_MATRIX_SCFG7 ... A_MATRIX_SCFG7 + 3:
			value = value << ((offset - A_MATRIX_SCFG7) << 3);
			offset = A_MATRIX_SCFG7;
			break;
		case A_MATRIX_SCFG8 ... A_MATRIX_SCFG8 + 3:
			value = value << ((offset - A_MATRIX_SCFG8) << 3);
			offset = A_MATRIX_SCFG8;
			break;
		case A_MATRIX_MATRIX_PR0_PRAS ... A_MATRIX_MATRIX_PR0_PRAS + 3:
			value = value << ((offset - A_MATRIX_MATRIX_PR0_PRAS) << 3);
			offset = A_MATRIX_MATRIX_PR0_PRAS;
			break;
		case A_MATRIX_MATRIX_PR0_PRBS ... A_MATRIX_MATRIX_PR0_PRBS + 3:
			value = value << ((offset - A_MATRIX_MATRIX_PR0_PRBS) << 3);
			offset = A_MATRIX_MATRIX_PR0_PRBS;
			break;
		case A_MATRIX_MATRIX_PR1_PRAS ... A_MATRIX_MATRIX_PR1_PRAS + 3:
			value = value << ((offset - A_MATRIX_MATRIX_PR1_PRAS) << 3);
			offset = A_MATRIX_MATRIX_PR1_PRAS;
			break;
		case A_MATRIX_MATRIX_PR1_PRBS ... A_MATRIX_MATRIX_PR1_PRBS + 3:
			value = value << ((offset - A_MATRIX_MATRIX_PR1_PRBS) << 3);
			offset = A_MATRIX_MATRIX_PR1_PRBS;
			break;
		case A_MATRIX_MATRIX_PR2_PRAS ... A_MATRIX_MATRIX_PR2_PRAS + 3:
			value = value << ((offset - A_MATRIX_MATRIX_PR2_PRAS) << 3);
			offset = A_MATRIX_MATRIX_PR2_PRAS;
			break;
		case A_MATRIX_MATRIX_PR2_PRBS ... A_MATRIX_MATRIX_PR2_PRBS + 3:
			value = value << ((offset - A_MATRIX_MATRIX_PR2_PRBS) << 3);
			offset = A_MATRIX_MATRIX_PR2_PRBS;
			break;
		case A_MATRIX_MATRIX_PR3_PRAS ... A_MATRIX_MATRIX_PR3_PRAS + 3:
			value = value << ((offset - A_MATRIX_MATRIX_PR3_PRAS) << 3);
			offset = A_MATRIX_MATRIX_PR3_PRAS;
			break;
		case A_MATRIX_MATRIX_PR3_PRBS ... A_MATRIX_MATRIX_PR3_PRBS + 3:
			value = value << ((offset - A_MATRIX_MATRIX_PR3_PRBS) << 3);
			offset = A_MATRIX_MATRIX_PR3_PRBS;
			break;
		case A_MATRIX_MATRIX_PR4_PRAS ... A_MATRIX_MATRIX_PR4_PRAS + 3:
			value = value << ((offset - A_MATRIX_MATRIX_PR4_PRAS) << 3);
			offset = A_MATRIX_MATRIX_PR4_PRAS;
			break;
		case A_MATRIX_MATRIX_PR4_PRBS ... A_MATRIX_MATRIX_PR4_PRBS + 3:
			value = value << ((offset - A_MATRIX_MATRIX_PR4_PRBS) << 3);
			offset = A_MATRIX_MATRIX_PR4_PRBS;
			break;
		case A_MATRIX_MATRIX_PR5_PRAS ... A_MATRIX_MATRIX_PR5_PRAS + 3:
			value = value << ((offset - A_MATRIX_MATRIX_PR5_PRAS) << 3);
			offset = A_MATRIX_MATRIX_PR5_PRAS;
			break;
		case A_MATRIX_MATRIX_PR5_PRBS ... A_MATRIX_MATRIX_PR5_PRBS + 3:
			value = value << ((offset - A_MATRIX_MATRIX_PR5_PRBS) << 3);
			offset = A_MATRIX_MATRIX_PR5_PRBS;
			break;
		case A_MATRIX_MATRIX_PR6_PRAS ... A_MATRIX_MATRIX_PR6_PRAS + 3:
			value = value << ((offset - A_MATRIX_MATRIX_PR6_PRAS) << 3);
			offset = A_MATRIX_MATRIX_PR6_PRAS;
			break;
		case A_MATRIX_MATRIX_PR6_PRBS ... A_MATRIX_MATRIX_PR6_PRBS + 3:
			value = value << ((offset - A_MATRIX_MATRIX_PR6_PRBS) << 3);
			offset = A_MATRIX_MATRIX_PR6_PRBS;
			break;
		case A_MATRIX_MATRIX_PR7_PRAS ... A_MATRIX_MATRIX_PR7_PRAS + 3:
			value = value << ((offset - A_MATRIX_MATRIX_PR7_PRAS) << 3);
			offset = A_MATRIX_MATRIX_PR7_PRAS;
			break;
		case A_MATRIX_MATRIX_PR7_PRBS ... A_MATRIX_MATRIX_PR7_PRBS + 3:
			value = value << ((offset - A_MATRIX_MATRIX_PR7_PRBS) << 3);
			offset = A_MATRIX_MATRIX_PR7_PRBS;
			break;
		case A_MATRIX_MATRIX_PR8_PRAS ... A_MATRIX_MATRIX_PR8_PRAS + 3:
			value = value << ((offset - A_MATRIX_MATRIX_PR8_PRAS) << 3);
			offset = A_MATRIX_MATRIX_PR8_PRAS;
			break;
		case A_MATRIX_MATRIX_PR8_PRBS ... A_MATRIX_MATRIX_PR8_PRBS + 3:
			value = value << ((offset - A_MATRIX_MATRIX_PR8_PRBS) << 3);
			offset = A_MATRIX_MATRIX_PR8_PRBS;
			break;

		default: break;
	}

	switch (offset) {
		case A_MATRIX_MRCR:
			t->MRCR = value;
			break;
		case A_MATRIX_CCFG_CAN0:
			t->CCFG_CAN0 = value;
			break;
		case A_MATRIX_CCFG_SYSIO:
			t->CCFG_SYSIO = value;
			break;
		case A_MATRIX_CCFG_PCCR:
			t->CCFG_PCCR = value;
			break;
		case A_MATRIX_CCFG_DYNCKG:
			t->CCFG_DYNCKG = value;
			break;
		case A_MATRIX_CCFG_SMCNFCS:
			t->CCFG_SMCNFCS = value;
			break;
		case A_MATRIX_WPMR:
			t->WPMR = value;
			break;
		case A_MATRIX_MCFG0:
			t->MCFG0 = value;
			break;
		case A_MATRIX_MCFG1:
			t->MCFG1 = value;
			break;
		case A_MATRIX_MCFG2:
			t->MCFG2 = value;
			break;
		case A_MATRIX_MCFG3:
			t->MCFG3 = value;
			break;
		case A_MATRIX_MCFG4:
			t->MCFG4 = value;
			break;
		case A_MATRIX_MCFG5:
			t->MCFG5 = value;
			break;
		case A_MATRIX_MCFG6:
			t->MCFG6 = value;
			break;
		case A_MATRIX_MCFG7:
			t->MCFG7 = value;
			break;
		case A_MATRIX_MCFG8:
			t->MCFG8 = value;
			break;
		case A_MATRIX_MCFG9:
			t->MCFG9 = value;
			break;
		case A_MATRIX_MCFG10:
			t->MCFG10 = value;
			break;
		case A_MATRIX_MCFG11:
			t->MCFG11 = value;
			break;
		case A_MATRIX_MCFG12:
			t->MCFG12 = value;
			break;
		case A_MATRIX_SCFG0:
			t->SCFG0 = value;
			break;
		case A_MATRIX_SCFG1:
			t->SCFG1 = value;
			break;
		case A_MATRIX_SCFG2:
			t->SCFG2 = value;
			break;
		case A_MATRIX_SCFG3:
			t->SCFG3 = value;
			break;
		case A_MATRIX_SCFG4:
			t->SCFG4 = value;
			break;
		case A_MATRIX_SCFG5:
			t->SCFG5 = value;
			break;
		case A_MATRIX_SCFG6:
			t->SCFG6 = value;
			break;
		case A_MATRIX_SCFG7:
			t->SCFG7 = value;
			break;
		case A_MATRIX_SCFG8:
			t->SCFG8 = value;
			break;
		case A_MATRIX_MATRIX_PR0_PRAS:
			t->MATRIX_PR0_PRAS = value;
			break;
		case A_MATRIX_MATRIX_PR0_PRBS:
			t->MATRIX_PR0_PRBS = value;
			break;
		case A_MATRIX_MATRIX_PR1_PRAS:
			t->MATRIX_PR1_PRAS = value;
			break;
		case A_MATRIX_MATRIX_PR1_PRBS:
			t->MATRIX_PR1_PRBS = value;
			break;
		case A_MATRIX_MATRIX_PR2_PRAS:
			t->MATRIX_PR2_PRAS = value;
			break;
		case A_MATRIX_MATRIX_PR2_PRBS:
			t->MATRIX_PR2_PRBS = value;
			break;
		case A_MATRIX_MATRIX_PR3_PRAS:
			t->MATRIX_PR3_PRAS = value;
			break;
		case A_MATRIX_MATRIX_PR3_PRBS:
			t->MATRIX_PR3_PRBS = value;
			break;
		case A_MATRIX_MATRIX_PR4_PRAS:
			t->MATRIX_PR4_PRAS = value;
			break;
		case A_MATRIX_MATRIX_PR4_PRBS:
			t->MATRIX_PR4_PRBS = value;
			break;
		case A_MATRIX_MATRIX_PR5_PRAS:
			t->MATRIX_PR5_PRAS = value;
			break;
		case A_MATRIX_MATRIX_PR5_PRBS:
			t->MATRIX_PR5_PRBS = value;
			break;
		case A_MATRIX_MATRIX_PR6_PRAS:
			t->MATRIX_PR6_PRAS = value;
			break;
		case A_MATRIX_MATRIX_PR6_PRBS:
			t->MATRIX_PR6_PRBS = value;
			break;
		case A_MATRIX_MATRIX_PR7_PRAS:
			t->MATRIX_PR7_PRAS = value;
			break;
		case A_MATRIX_MATRIX_PR7_PRBS:
			t->MATRIX_PR7_PRBS = value;
			break;
		case A_MATRIX_MATRIX_PR8_PRAS:
			t->MATRIX_PR8_PRAS = value;
			break;
		case A_MATRIX_MATRIX_PR8_PRBS:
			t->MATRIX_PR8_PRBS = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAME70Q21B MATRIX write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsame70q21b_matrix_ops = {
	.read = atsame70q21b_matrix_read,
	.write = atsame70q21b_matrix_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsame70q21b_matrix_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAME70Q21BMATRIX *t = ATSAME70Q21B_MATRIX(obj);
	memory_region_init_io(&(t->iomem), obj, &atsame70q21b_matrix_ops, t, "atsame70q21b-matrix", ATSAME70Q21B_MATRIX_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void atsame70q21b_matrix_realize(DeviceState *dev, Error **errp) {
	return;

}

static void atsame70q21b_matrix_reset_enter(Object *obj, ResetType type) {
	ATSAME70Q21BMATRIX *t = ATSAME70Q21B_MATRIX(obj);
	atsame70q21b_matrix_register_reset(t);
}

static const VMStateDescription atsame70q21b_matrix_vmstate = {
	.name = "atsame70q21b-matrix",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(MRCR, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(CCFG_CAN0, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(CCFG_SYSIO, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(CCFG_PCCR, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(CCFG_DYNCKG, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(CCFG_SMCNFCS, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(WPMR, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(WPSR, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(MCFG0, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(MCFG1, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(MCFG2, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(MCFG3, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(MCFG4, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(MCFG5, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(MCFG6, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(MCFG7, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(MCFG8, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(MCFG9, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(MCFG10, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(MCFG11, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(MCFG12, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(SCFG0, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(SCFG1, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(SCFG2, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(SCFG3, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(SCFG4, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(SCFG5, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(SCFG6, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(SCFG7, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(SCFG8, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(MATRIX_PR0_PRAS, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(MATRIX_PR0_PRBS, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(MATRIX_PR1_PRAS, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(MATRIX_PR1_PRBS, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(MATRIX_PR2_PRAS, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(MATRIX_PR2_PRBS, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(MATRIX_PR3_PRAS, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(MATRIX_PR3_PRBS, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(MATRIX_PR4_PRAS, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(MATRIX_PR4_PRBS, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(MATRIX_PR5_PRAS, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(MATRIX_PR5_PRBS, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(MATRIX_PR6_PRAS, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(MATRIX_PR6_PRBS, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(MATRIX_PR7_PRAS, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(MATRIX_PR7_PRBS, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(MATRIX_PR8_PRAS, ATSAME70Q21BMATRIX),
		VMSTATE_UINT32(MATRIX_PR8_PRBS, ATSAME70Q21BMATRIX),

		VMSTATE_END_OF_LIST()
	}
};

static void atsame70q21b_matrix_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsame70q21b_matrix_vmstate;
	dc->realize = atsame70q21b_matrix_realize;
	rc->phases.enter = atsame70q21b_matrix_reset_enter;

}

static const TypeInfo atsame70q21b_matrix_info = {
	.name = TYPE_ATSAME70Q21B_MATRIX,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAME70Q21BMATRIX),
	.instance_init = atsame70q21b_matrix_init,
	.class_init = atsame70q21b_matrix_class_init,
};

static void atsame70q21b_matrix_register_types(void) {
	type_register_static(&atsame70q21b_matrix_info);
}

type_init(atsame70q21b_matrix_register_types);

#define TYPE_ATSAME70Q21B_DACC "atsame70q21b-dacc"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAME70Q21BDACC, ATSAME70Q21B_DACC)

struct ATSAME70Q21BDACC {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t CR;
	uint32_t MR;
	uint32_t TRIGR;
	uint32_t CHER;
	uint32_t CHDR;
	uint32_t CHSR;
	uint32_t IER;
	uint32_t IDR;
	uint32_t IMR;
	uint32_t ISR;
	uint32_t ACR;
	uint32_t WPMR;
	uint32_t WPSR;
	uint32_t CDR0;
	uint32_t CDR1;
	/* chardev backend */
	CharBackend chr;
	guint watch_tag;

	/* base */
	uint32_t base;

};
#define ATSAME70Q21B_DACC_SIZE				0xec

REG32(DACC_CR, 0x0)
	FIELD(DACC_CR, SWRST, 0, 1)
REG32(DACC_MR, 0x4)
	FIELD(DACC_MR, MAXS0, 0, 1)
	FIELD(DACC_MR, MAXS1, 1, 1)
	FIELD(DACC_MR, WORD, 4, 1)
	FIELD(DACC_MR, ZERO, 5, 1)
	FIELD(DACC_MR, DIFF, 23, 1)
	FIELD(DACC_MR, PRESCALER, 24, 4)
REG32(DACC_TRIGR, 0x8)
	FIELD(DACC_TRIGR, TRGEN0, 0, 1)
	FIELD(DACC_TRIGR, TRGEN1, 1, 1)
	FIELD(DACC_TRIGR, TRGSEL0, 4, 3)
	FIELD(DACC_TRIGR, TRGSEL1, 8, 3)
	FIELD(DACC_TRIGR, OSR0, 16, 3)
	FIELD(DACC_TRIGR, OSR1, 20, 3)
REG32(DACC_CHER, 0x10)
	FIELD(DACC_CHER, CH0, 0, 1)
	FIELD(DACC_CHER, CH1, 1, 1)
REG32(DACC_CHDR, 0x14)
	FIELD(DACC_CHDR, CH0, 0, 1)
	FIELD(DACC_CHDR, CH1, 1, 1)
REG32(DACC_CHSR, 0x18)
	FIELD(DACC_CHSR, CH0, 0, 1)
	FIELD(DACC_CHSR, CH1, 1, 1)
	FIELD(DACC_CHSR, DACRDY0, 8, 1)
	FIELD(DACC_CHSR, DACRDY1, 9, 1)
REG32(DACC_IER, 0x24)
	FIELD(DACC_IER, TXRDY0, 0, 1)
	FIELD(DACC_IER, TXRDY1, 1, 1)
	FIELD(DACC_IER, EOC0, 4, 1)
	FIELD(DACC_IER, EOC1, 5, 1)
REG32(DACC_IDR, 0x28)
	FIELD(DACC_IDR, TXRDY0, 0, 1)
	FIELD(DACC_IDR, TXRDY1, 1, 1)
	FIELD(DACC_IDR, EOC0, 4, 1)
	FIELD(DACC_IDR, EOC1, 5, 1)
REG32(DACC_IMR, 0x2c)
	FIELD(DACC_IMR, TXRDY0, 0, 1)
	FIELD(DACC_IMR, TXRDY1, 1, 1)
	FIELD(DACC_IMR, EOC0, 4, 1)
	FIELD(DACC_IMR, EOC1, 5, 1)
REG32(DACC_ISR, 0x30)
	FIELD(DACC_ISR, TXRDY0, 0, 1)
	FIELD(DACC_ISR, TXRDY1, 1, 1)
	FIELD(DACC_ISR, EOC0, 4, 1)
	FIELD(DACC_ISR, EOC1, 5, 1)
REG32(DACC_ACR, 0x94)
	FIELD(DACC_ACR, IBCTLCH0, 0, 2)
	FIELD(DACC_ACR, IBCTLCH1, 2, 2)
REG32(DACC_WPMR, 0xe4)
	FIELD(DACC_WPMR, WPEN, 0, 1)
	FIELD(DACC_WPMR, WPKEY, 8, 24)
REG32(DACC_WPSR, 0xe8)
	FIELD(DACC_WPSR, WPVS, 0, 1)
	FIELD(DACC_WPSR, WPVSRC, 8, 8)
REG32(DACC_CDR0, 0x1c)
	FIELD(DACC_CDR0, DATA0, 0, 16)
	FIELD(DACC_CDR0, DATA1, 16, 16)
REG32(DACC_CDR1, 0x20)
	FIELD(DACC_CDR1, DATA0, 0, 16)
	FIELD(DACC_CDR1, DATA1, 16, 16)


static int atsame70q21b_dacc_can_receive(void *opaque) {
	return 1;
}

static void atsame70q21b_dacc_receive(void *opaque, const uint8_t *buf, int size) {
	ATSAME70Q21BDACC *t = ATSAME70Q21B_DACC(opaque);

	(void)t;


}

static gboolean atsame70q21b_dacc_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	ATSAME70Q21BDACC *t = ATSAME70Q21B_DACC(opaque);
	int ret;

	t->watch_tag = 0;



	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->CDR0), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, atsame70q21b_dacc_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:



	return FALSE;
}

static void atsame70q21b_dacc_register_reset(ATSAME70Q21BDACC *t) {

}

static uint64_t atsame70q21b_dacc_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAME70Q21BDACC *t = ATSAME70Q21B_DACC(opaque);
	uint64_t ret;

	switch (offset) {
		case A_DACC_MR:
			ret = t->MR;
			break;
		case A_DACC_TRIGR:
			ret = t->TRIGR;
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

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAME70Q21B DACC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsame70q21b_dacc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAME70Q21BDACC *t = ATSAME70Q21B_DACC(opaque);


	switch (offset) {
		case A_DACC_CR ... A_DACC_CR + 3:
			value = value << ((offset - A_DACC_CR) << 3);
			offset = A_DACC_CR;
			break;
		case A_DACC_MR ... A_DACC_MR + 3:
			value = value << ((offset - A_DACC_MR) << 3);
			offset = A_DACC_MR;
			break;
		case A_DACC_TRIGR ... A_DACC_TRIGR + 3:
			value = value << ((offset - A_DACC_TRIGR) << 3);
			offset = A_DACC_TRIGR;
			break;
		case A_DACC_CHER ... A_DACC_CHER + 3:
			value = value << ((offset - A_DACC_CHER) << 3);
			offset = A_DACC_CHER;
			break;
		case A_DACC_CHDR ... A_DACC_CHDR + 3:
			value = value << ((offset - A_DACC_CHDR) << 3);
			offset = A_DACC_CHDR;
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
		case A_DACC_CDR0 ... A_DACC_CDR0 + 3:
			value = value << ((offset - A_DACC_CDR0) << 3);
			offset = A_DACC_CDR0;
			break;
		case A_DACC_CDR1 ... A_DACC_CDR1 + 3:
			value = value << ((offset - A_DACC_CDR1) << 3);
			offset = A_DACC_CDR1;
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
		case A_DACC_TRIGR:
			t->TRIGR = value;
			break;
		case A_DACC_CHER:
			t->CHER = value;
			break;
		case A_DACC_CHDR:
			t->CHDR = value;
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
		case A_DACC_CDR0:
			t->CDR0 = value;
			atsame70q21b_dacc_transmit(NULL, G_IO_OUT, t);
			break;
		case A_DACC_CDR1:
			t->CDR1 = value;
			atsame70q21b_dacc_transmit(NULL, G_IO_OUT, t);
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAME70Q21B DACC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsame70q21b_dacc_ops = {
	.read = atsame70q21b_dacc_read,
	.write = atsame70q21b_dacc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsame70q21b_dacc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAME70Q21BDACC *t = ATSAME70Q21B_DACC(obj);
	memory_region_init_io(&(t->iomem), obj, &atsame70q21b_dacc_ops, t, "atsame70q21b-dacc", ATSAME70Q21B_DACC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void atsame70q21b_dacc_realize(DeviceState *dev, Error **errp) {
	ATSAME70Q21BDACC *t = ATSAME70Q21B_DACC(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), atsame70q21b_dacc_can_receive, atsame70q21b_dacc_receive,
		NULL, NULL, t, NULL, true);

}

static void atsame70q21b_dacc_reset_enter(Object *obj, ResetType type) {
	ATSAME70Q21BDACC *t = ATSAME70Q21B_DACC(obj);
	atsame70q21b_dacc_register_reset(t);
}

static const VMStateDescription atsame70q21b_dacc_vmstate = {
	.name = "atsame70q21b-dacc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR, ATSAME70Q21BDACC),
		VMSTATE_UINT32(MR, ATSAME70Q21BDACC),
		VMSTATE_UINT32(TRIGR, ATSAME70Q21BDACC),
		VMSTATE_UINT32(CHER, ATSAME70Q21BDACC),
		VMSTATE_UINT32(CHDR, ATSAME70Q21BDACC),
		VMSTATE_UINT32(CHSR, ATSAME70Q21BDACC),
		VMSTATE_UINT32(IER, ATSAME70Q21BDACC),
		VMSTATE_UINT32(IDR, ATSAME70Q21BDACC),
		VMSTATE_UINT32(IMR, ATSAME70Q21BDACC),
		VMSTATE_UINT32(ISR, ATSAME70Q21BDACC),
		VMSTATE_UINT32(ACR, ATSAME70Q21BDACC),
		VMSTATE_UINT32(WPMR, ATSAME70Q21BDACC),
		VMSTATE_UINT32(WPSR, ATSAME70Q21BDACC),
		VMSTATE_UINT32(CDR0, ATSAME70Q21BDACC),
		VMSTATE_UINT32(CDR1, ATSAME70Q21BDACC),

		VMSTATE_END_OF_LIST()
	}
};

static Property atsame70q21b_dacc_properties[] = {
	DEFINE_PROP_CHR("chardev", ATSAME70Q21BDACC, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void atsame70q21b_dacc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsame70q21b_dacc_vmstate;
	dc->realize = atsame70q21b_dacc_realize;
	rc->phases.enter = atsame70q21b_dacc_reset_enter;
	device_class_set_props(dc, atsame70q21b_dacc_properties);

}

static const TypeInfo atsame70q21b_dacc_info = {
	.name = TYPE_ATSAME70Q21B_DACC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAME70Q21BDACC),
	.instance_init = atsame70q21b_dacc_init,
	.class_init = atsame70q21b_dacc_class_init,
};

static void atsame70q21b_dacc_register_types(void) {
	type_register_static(&atsame70q21b_dacc_info);
}

type_init(atsame70q21b_dacc_register_types);

#define TYPE_ATSAME70Q21B_SPI "atsame70q21b-spi"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAME70Q21BSPI, ATSAME70Q21B_SPI)

struct ATSAME70Q21BSPI {
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
#define ATSAME70Q21B_SPI_SIZE				0xec

REG32(SPI_CR, 0x0)
	FIELD(SPI_CR, SPIEN, 0, 1)
	FIELD(SPI_CR, SPIDIS, 1, 1)
	FIELD(SPI_CR, SWRST, 7, 1)
	FIELD(SPI_CR, REQCLR, 12, 1)
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


static int atsame70q21b_spi_can_receive(void *opaque) {
	return 1;
}

static void atsame70q21b_spi_receive(void *opaque, const uint8_t *buf, int size) {
	ATSAME70Q21BSPI *t = ATSAME70Q21B_SPI(opaque);

	t->RDR = *buf;
	t->SR |= 0x1;


}

static gboolean atsame70q21b_spi_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	ATSAME70Q21BSPI *t = ATSAME70Q21B_SPI(opaque);
	int ret;

	t->watch_tag = 0;
	t->SR &= (~(0x2));



	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->TDR), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, atsame70q21b_spi_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->SR |= 0x2;



	return FALSE;
}

static void atsame70q21b_spi_register_reset(ATSAME70Q21BSPI *t) {
	t->SR |= 0x2;

}

static uint64_t atsame70q21b_spi_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAME70Q21BSPI *t = ATSAME70Q21B_SPI(opaque);
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
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAME70Q21B SPI read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsame70q21b_spi_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAME70Q21BSPI *t = ATSAME70Q21B_SPI(opaque);


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
			atsame70q21b_spi_transmit(NULL, G_IO_OUT, t);
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
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAME70Q21B SPI write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsame70q21b_spi_ops = {
	.read = atsame70q21b_spi_read,
	.write = atsame70q21b_spi_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsame70q21b_spi_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAME70Q21BSPI *t = ATSAME70Q21B_SPI(obj);
	memory_region_init_io(&(t->iomem), obj, &atsame70q21b_spi_ops, t, "atsame70q21b-spi", ATSAME70Q21B_SPI_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void atsame70q21b_spi_realize(DeviceState *dev, Error **errp) {
	ATSAME70Q21BSPI *t = ATSAME70Q21B_SPI(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), atsame70q21b_spi_can_receive, atsame70q21b_spi_receive,
		NULL, NULL, t, NULL, true);

}

static void atsame70q21b_spi_reset_enter(Object *obj, ResetType type) {
	ATSAME70Q21BSPI *t = ATSAME70Q21B_SPI(obj);
	atsame70q21b_spi_register_reset(t);
}

static const VMStateDescription atsame70q21b_spi_vmstate = {
	.name = "atsame70q21b-spi",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR, ATSAME70Q21BSPI),
		VMSTATE_UINT32(MR, ATSAME70Q21BSPI),
		VMSTATE_UINT32(RDR, ATSAME70Q21BSPI),
		VMSTATE_UINT32(TDR, ATSAME70Q21BSPI),
		VMSTATE_UINT32(SR, ATSAME70Q21BSPI),
		VMSTATE_UINT32(IER, ATSAME70Q21BSPI),
		VMSTATE_UINT32(IDR, ATSAME70Q21BSPI),
		VMSTATE_UINT32(IMR, ATSAME70Q21BSPI),
		VMSTATE_UINT32(WPMR, ATSAME70Q21BSPI),
		VMSTATE_UINT32(WPSR, ATSAME70Q21BSPI),
		VMSTATE_UINT32(CSR0, ATSAME70Q21BSPI),
		VMSTATE_UINT32(CSR1, ATSAME70Q21BSPI),
		VMSTATE_UINT32(CSR2, ATSAME70Q21BSPI),
		VMSTATE_UINT32(CSR3, ATSAME70Q21BSPI),

		VMSTATE_END_OF_LIST()
	}
};

static Property atsame70q21b_spi_properties[] = {
	DEFINE_PROP_CHR("chardev", ATSAME70Q21BSPI, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void atsame70q21b_spi_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsame70q21b_spi_vmstate;
	dc->realize = atsame70q21b_spi_realize;
	rc->phases.enter = atsame70q21b_spi_reset_enter;
	device_class_set_props(dc, atsame70q21b_spi_properties);

}

static const TypeInfo atsame70q21b_spi_info = {
	.name = TYPE_ATSAME70Q21B_SPI,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAME70Q21BSPI),
	.instance_init = atsame70q21b_spi_init,
	.class_init = atsame70q21b_spi_class_init,
};

static void atsame70q21b_spi_register_types(void) {
	type_register_static(&atsame70q21b_spi_info);
}

type_init(atsame70q21b_spi_register_types);

#define TYPE_ATSAME70Q21B_MACHINE MACHINE_TYPE_NAME("atsame70q21b")
OBJECT_DECLARE_TYPE(ATSAME70Q21BMachineState, ATSAME70Q21BMachineClass, ATSAME70Q21B_MACHINE)

struct ATSAME70Q21BMachineClass {
	MachineClass parent;
};

struct ATSAME70Q21BMachineState {
	MachineState parent;
	ARMv7MState armv7m;
};

static void atsame70q21b_periph_init(MachineState *machine) {
	ATSAME70Q21BMachineState *sms = ATSAME70Q21B_MACHINE(machine);
	ATSAME70Q21BUSART *p0 = g_new(ATSAME70Q21BUSART, 1);
	object_initialize_child(OBJECT(sms), "USART0", p0, TYPE_ATSAME70Q21B_USART);
	qdev_prop_set_chr(DEVICE(p0), "chardev", qemu_chr_new("soc-usart0", "chardev:usart0", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p0), &error_fatal);
	p0->base = 0x40024000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p0), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 13));
	sysbus_mmio_map(SYS_BUS_DEVICE(p0), 0, 0x40024000);

	ATSAME70Q21BUSART *p1 = g_new(ATSAME70Q21BUSART, 1);
	object_initialize_child(OBJECT(sms), "USART1", p1, TYPE_ATSAME70Q21B_USART);
	qdev_prop_set_chr(DEVICE(p1), "chardev", qemu_chr_new("soc-usart1", "chardev:usart1", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p1), &error_fatal);
	p1->base = 0x40028000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p1), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 14));
	sysbus_mmio_map(SYS_BUS_DEVICE(p1), 0, 0x40028000);

	ATSAME70Q21BUSART *p2 = g_new(ATSAME70Q21BUSART, 1);
	object_initialize_child(OBJECT(sms), "USART2", p2, TYPE_ATSAME70Q21B_USART);
	qdev_prop_set_chr(DEVICE(p2), "chardev", qemu_chr_new("soc-usart2", "chardev:usart2", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p2), &error_fatal);
	p2->base = 0x4002c000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p2), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 15));
	sysbus_mmio_map(SYS_BUS_DEVICE(p2), 0, 0x4002c000);

	ATSAME70Q21BUART *p3 = g_new(ATSAME70Q21BUART, 1);
	object_initialize_child(OBJECT(sms), "UART0", p3, TYPE_ATSAME70Q21B_UART);
	qdev_prop_set_chr(DEVICE(p3), "chardev", qemu_chr_new("soc-uart0", "chardev:uart0", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p3), &error_fatal);
	p3->base = 0x400e0800;
	sysbus_connect_irq(SYS_BUS_DEVICE(p3), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 7));
	sysbus_mmio_map(SYS_BUS_DEVICE(p3), 0, 0x400e0800);

	ATSAME70Q21BUART *p4 = g_new(ATSAME70Q21BUART, 1);
	object_initialize_child(OBJECT(sms), "UART1", p4, TYPE_ATSAME70Q21B_UART);
	qdev_prop_set_chr(DEVICE(p4), "chardev", qemu_chr_new("soc-uart1", "chardev:uart1", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p4), &error_fatal);
	p4->base = 0x400e0a00;
	sysbus_connect_irq(SYS_BUS_DEVICE(p4), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 8));
	sysbus_mmio_map(SYS_BUS_DEVICE(p4), 0, 0x400e0a00);

	ATSAME70Q21BUART *p5 = g_new(ATSAME70Q21BUART, 1);
	object_initialize_child(OBJECT(sms), "UART2", p5, TYPE_ATSAME70Q21B_UART);
	qdev_prop_set_chr(DEVICE(p5), "chardev", qemu_chr_new("soc-uart2", "chardev:uart2", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p5), &error_fatal);
	p5->base = 0x400e1a00;
	sysbus_connect_irq(SYS_BUS_DEVICE(p5), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 44));
	sysbus_mmio_map(SYS_BUS_DEVICE(p5), 0, 0x400e1a00);

	ATSAME70Q21BUART *p6 = g_new(ATSAME70Q21BUART, 1);
	object_initialize_child(OBJECT(sms), "UART3", p6, TYPE_ATSAME70Q21B_UART);
	qdev_prop_set_chr(DEVICE(p6), "chardev", qemu_chr_new("soc-uart3", "chardev:uart3", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p6), &error_fatal);
	p6->base = 0x400e1c00;
	sysbus_connect_irq(SYS_BUS_DEVICE(p6), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 45));
	sysbus_mmio_map(SYS_BUS_DEVICE(p6), 0, 0x400e1c00);

	ATSAME70Q21BUART *p7 = g_new(ATSAME70Q21BUART, 1);
	object_initialize_child(OBJECT(sms), "UART4", p7, TYPE_ATSAME70Q21B_UART);
	qdev_prop_set_chr(DEVICE(p7), "chardev", qemu_chr_new("soc-uart4", "chardev:uart4", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p7), &error_fatal);
	p7->base = 0x400e1e00;
	sysbus_connect_irq(SYS_BUS_DEVICE(p7), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 46));
	sysbus_mmio_map(SYS_BUS_DEVICE(p7), 0, 0x400e1e00);

	ATSAME70Q21BPIO *p8 = g_new(ATSAME70Q21BPIO, 1);
	object_initialize_child(OBJECT(sms), "PIOA", p8, TYPE_ATSAME70Q21B_PIO);
	qdev_prop_set_chr(DEVICE(p8), "chardev", qemu_chr_new("soc-pioa", "chardev:pioa", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p8), &error_fatal);
	p8->base = 0x400e0e00;
	sysbus_connect_irq(SYS_BUS_DEVICE(p8), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 10));
	sysbus_mmio_map(SYS_BUS_DEVICE(p8), 0, 0x400e0e00);

	ATSAME70Q21BPIO *p9 = g_new(ATSAME70Q21BPIO, 1);
	object_initialize_child(OBJECT(sms), "PIOB", p9, TYPE_ATSAME70Q21B_PIO);
	qdev_prop_set_chr(DEVICE(p9), "chardev", qemu_chr_new("soc-piob", "chardev:piob", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p9), &error_fatal);
	p9->base = 0x400e1000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p9), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 11));
	sysbus_mmio_map(SYS_BUS_DEVICE(p9), 0, 0x400e1000);

	ATSAME70Q21BPIO *p10 = g_new(ATSAME70Q21BPIO, 1);
	object_initialize_child(OBJECT(sms), "PIOC", p10, TYPE_ATSAME70Q21B_PIO);
	qdev_prop_set_chr(DEVICE(p10), "chardev", qemu_chr_new("soc-pioc", "chardev:pioc", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p10), &error_fatal);
	p10->base = 0x400e1200;
	sysbus_connect_irq(SYS_BUS_DEVICE(p10), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 12));
	sysbus_mmio_map(SYS_BUS_DEVICE(p10), 0, 0x400e1200);

	ATSAME70Q21BPIO *p11 = g_new(ATSAME70Q21BPIO, 1);
	object_initialize_child(OBJECT(sms), "PIOD", p11, TYPE_ATSAME70Q21B_PIO);
	qdev_prop_set_chr(DEVICE(p11), "chardev", qemu_chr_new("soc-piod", "chardev:piod", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p11), &error_fatal);
	p11->base = 0x400e1400;
	sysbus_connect_irq(SYS_BUS_DEVICE(p11), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 16));
	sysbus_mmio_map(SYS_BUS_DEVICE(p11), 0, 0x400e1400);

	ATSAME70Q21BPIO *p12 = g_new(ATSAME70Q21BPIO, 1);
	object_initialize_child(OBJECT(sms), "PIOE", p12, TYPE_ATSAME70Q21B_PIO);
	qdev_prop_set_chr(DEVICE(p12), "chardev", qemu_chr_new("soc-pioe", "chardev:pioe", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p12), &error_fatal);
	p12->base = 0x400e1600;
	sysbus_connect_irq(SYS_BUS_DEVICE(p12), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 17));
	sysbus_mmio_map(SYS_BUS_DEVICE(p12), 0, 0x400e1600);

	ATSAME70Q21BPMC *p13 = g_new(ATSAME70Q21BPMC, 1);
	object_initialize_child(OBJECT(sms), "PMC", p13, TYPE_ATSAME70Q21B_PMC);
	sysbus_realize(SYS_BUS_DEVICE(p13), &error_fatal);
	p13->base = 0x400e0600;
	sysbus_connect_irq(SYS_BUS_DEVICE(p13), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 5));
	sysbus_mmio_map(SYS_BUS_DEVICE(p13), 0, 0x400e0600);

	ATSAME70Q21BSUPC *p14 = g_new(ATSAME70Q21BSUPC, 1);
	object_initialize_child(OBJECT(sms), "SUPC", p14, TYPE_ATSAME70Q21B_SUPC);
	sysbus_realize(SYS_BUS_DEVICE(p14), &error_fatal);
	p14->base = 0x400e1810;
	sysbus_connect_irq(SYS_BUS_DEVICE(p14), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 0));
	sysbus_mmio_map(SYS_BUS_DEVICE(p14), 0, 0x400e1810);

	ATSAME70Q21BEFC *p15 = g_new(ATSAME70Q21BEFC, 1);
	object_initialize_child(OBJECT(sms), "EFC", p15, TYPE_ATSAME70Q21B_EFC);
	sysbus_realize(SYS_BUS_DEVICE(p15), &error_fatal);
	p15->base = 0x400e0c00;
	sysbus_connect_irq(SYS_BUS_DEVICE(p15), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 6));
	sysbus_mmio_map(SYS_BUS_DEVICE(p15), 0, 0x400e0c00);

	ATSAME70Q21BWDT *p16 = g_new(ATSAME70Q21BWDT, 1);
	object_initialize_child(OBJECT(sms), "WDT", p16, TYPE_ATSAME70Q21B_WDT);
	sysbus_realize(SYS_BUS_DEVICE(p16), &error_fatal);
	p16->base = 0x400e1850;
	sysbus_connect_irq(SYS_BUS_DEVICE(p16), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 4));
	sysbus_mmio_map(SYS_BUS_DEVICE(p16), 0, 0x400e1850);

	ATSAME70Q21BSMC *p17 = g_new(ATSAME70Q21BSMC, 1);
	object_initialize_child(OBJECT(sms), "SMC", p17, TYPE_ATSAME70Q21B_SMC);
	sysbus_realize(SYS_BUS_DEVICE(p17), &error_fatal);
	p17->base = 0x40080000;
	sysbus_mmio_map(SYS_BUS_DEVICE(p17), 0, 0x40080000);

	ATSAME70Q21BCHIPID *p18 = g_new(ATSAME70Q21BCHIPID, 1);
	object_initialize_child(OBJECT(sms), "CHIPID", p18, TYPE_ATSAME70Q21B_CHIPID);
	sysbus_realize(SYS_BUS_DEVICE(p18), &error_fatal);
	p18->base = 0x400e0940;
	sysbus_mmio_map(SYS_BUS_DEVICE(p18), 0, 0x400e0940);

	ATSAME70Q21BMATRIX *p19 = g_new(ATSAME70Q21BMATRIX, 1);
	object_initialize_child(OBJECT(sms), "MATRIX", p19, TYPE_ATSAME70Q21B_MATRIX);
	sysbus_realize(SYS_BUS_DEVICE(p19), &error_fatal);
	p19->base = 0x40088000;
	sysbus_mmio_map(SYS_BUS_DEVICE(p19), 0, 0x40088000);

	ATSAME70Q21BDACC *p20 = g_new(ATSAME70Q21BDACC, 1);
	object_initialize_child(OBJECT(sms), "DACC", p20, TYPE_ATSAME70Q21B_DACC);
	qdev_prop_set_chr(DEVICE(p20), "chardev", qemu_chr_new("soc-dacc", "chardev:dacc", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p20), &error_fatal);
	p20->base = 0x40040000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p20), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 30));
	sysbus_mmio_map(SYS_BUS_DEVICE(p20), 0, 0x40040000);

	ATSAME70Q21BSPI *p21 = g_new(ATSAME70Q21BSPI, 1);
	object_initialize_child(OBJECT(sms), "SPI0", p21, TYPE_ATSAME70Q21B_SPI);
	qdev_prop_set_chr(DEVICE(p21), "chardev", qemu_chr_new("soc-spi0", "chardev:spi0", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p21), &error_fatal);
	p21->base = 0x40008000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p21), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 21));
	sysbus_mmio_map(SYS_BUS_DEVICE(p21), 0, 0x40008000);


}

static void atsame70q21b_common_init(MachineState *machine) {
	ATSAME70Q21BMachineState *sms = ATSAME70Q21B_MACHINE(machine);
	MemoryRegion *sysmem = get_system_memory();
	MemoryRegion *mem;
	Clock *cpuclk = clock_new(OBJECT(sms), "SYSCLK");
	clock_set_hz(cpuclk, 120000000);
	Clock *refclk = clock_new(OBJECT(sms), "REFCLK");
	clock_set_hz(refclk, 120000000);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_rom(mem, NULL, "main-flash", 0x200000, &error_fatal);
	memory_region_add_subregion(sysmem, 0x400000, mem);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_ram(mem, NULL, "sram", 0xc00000, &error_fatal);
	memory_region_add_subregion(sysmem, 0x20000000, mem);


	object_initialize_child(OBJECT(sms), "armv7m", &(sms->armv7m), TYPE_ARMV7M);
	qdev_prop_set_uint32(DEVICE(&(sms->armv7m)),"num-irq", 256);
	qdev_prop_set_string(DEVICE(&(sms->armv7m)), "cpu-type", machine->cpu_type);
	qdev_prop_set_bit(DEVICE(&(sms->armv7m)), "enable-bitband", false);
	qdev_connect_clock_in(DEVICE(&(sms->armv7m)), "cpuclk", cpuclk);
	qdev_connect_clock_in(DEVICE(&(sms->armv7m)), "refclk", refclk);
	qdev_prop_set_uint32(DEVICE(&(sms->armv7m)), "init-nsvtor", 0x400000);
	object_property_set_link(OBJECT(&sms->armv7m), "memory", OBJECT(sysmem), &error_abort);
	sysbus_realize(SYS_BUS_DEVICE(&sms->armv7m), &error_fatal);

	atsame70q21b_periph_init(machine);

	armv7m_load_kernel(ARM_CPU(first_cpu), machine->kernel_filename, 0x400000, 0x200000);
}

static void atsame70q21b_class_init(ObjectClass *oc, void *data) {
	MachineClass *mc = MACHINE_CLASS(oc);
	mc->desc = "atsame70q21b";
	mc->init = atsame70q21b_common_init;
	mc->max_cpus = 1;
	mc->default_cpus = 1;
	mc->default_cpu_type = ARM_CPU_TYPE_NAME("cortex-m7");
}

static const TypeInfo atsame70q21b_info = {
	.name = TYPE_ATSAME70Q21B_MACHINE,
	.parent = TYPE_MACHINE,
	.instance_size = sizeof(ATSAME70Q21BMachineState),
	.class_size = sizeof(ATSAME70Q21BMachineClass),
	.class_init = atsame70q21b_class_init
};

static void atsame70q21b_machine_init(void) {
	type_register_static(&atsame70q21b_info);
}

type_init(atsame70q21b_machine_init);
