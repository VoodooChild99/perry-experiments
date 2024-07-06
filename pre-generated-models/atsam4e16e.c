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


#define TYPE_ATSAM4E16E_UART "atsam4e16e-uart"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAM4E16EUART, ATSAM4E16E_UART)

struct ATSAM4E16EUART {
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
	uint32_t WPMR;
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
#define ATSAM4E16E_UART_SIZE				0x128

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
REG32(UART_WPMR, 0xe4)
	FIELD(UART_WPMR, WPEN, 0, 1)
	FIELD(UART_WPMR, WPKEY, 8, 24)
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
static void atsam4e16e_uart_update(ATSAM4E16EUART *t) {
	int conditions = (((t->IMR & 0x1) && (t->SR & 0x1)) || ((t->IMR & 0x2) && (t->SR & 0x2)) || ((t->SR & 0x200) && (t->IMR & 0x200)));
	qemu_set_irq(t->irq[0], conditions);

}

static int atsam4e16e_uart_can_receive(void *opaque) {
	return 1;
}

static void atsam4e16e_uart_receive(void *opaque, const uint8_t *buf, int size) {
	ATSAM4E16EUART *t = ATSAM4E16E_UART(opaque);

	t->RHR = *buf;
	t->SR |= 0x1;

	atsam4e16e_uart_update(t);
}

static gboolean atsam4e16e_uart_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	ATSAM4E16EUART *t = ATSAM4E16E_UART(opaque);
	int ret;

	t->watch_tag = 0;
	t->SR &= (~(0x2));

	atsam4e16e_uart_update(t);

	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->THR), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, atsam4e16e_uart_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->SR |= 0x2;

	atsam4e16e_uart_update(t);

	return FALSE;
}

static void atsam4e16e_uart_register_reset(ATSAM4E16EUART *t) {
	t->MR = 0x0;
	t->IMR = 0x0;
	t->RHR = 0x0;
	t->BRGR = 0x0;
	t->WPMR = 0x0;
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

static uint64_t atsam4e16e_uart_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAM4E16EUART *t = ATSAM4E16E_UART(opaque);
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
			atsam4e16e_uart_update(t);
			break;
		case A_UART_BRGR:
			ret = t->BRGR;
			break;
		case A_UART_WPMR:
			ret = t->WPMR;
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
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM4E16E UART read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsam4e16e_uart_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAM4E16EUART *t = ATSAM4E16E_UART(opaque);


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
		case A_UART_WPMR ... A_UART_WPMR + 3:
			value = value << ((offset - A_UART_WPMR) << 3);
			offset = A_UART_WPMR;
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
			break;
		case A_UART_IDR:
			t->IDR = value;
			break;
		case A_UART_THR:
			t->THR = value;
			atsam4e16e_uart_transmit(NULL, G_IO_OUT, t);
			break;
		case A_UART_BRGR:
			t->BRGR = value;
			break;
		case A_UART_WPMR:
			t->WPMR = value;
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
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM4E16E UART write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsam4e16e_uart_ops = {
	.read = atsam4e16e_uart_read,
	.write = atsam4e16e_uart_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsam4e16e_uart_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAM4E16EUART *t = ATSAM4E16E_UART(obj);
	memory_region_init_io(&(t->iomem), obj, &atsam4e16e_uart_ops, t, "atsam4e16e-uart", ATSAM4E16E_UART_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void atsam4e16e_uart_realize(DeviceState *dev, Error **errp) {
	ATSAM4E16EUART *t = ATSAM4E16E_UART(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), atsam4e16e_uart_can_receive, atsam4e16e_uart_receive,
		NULL, NULL, t, NULL, true);

}

static void atsam4e16e_uart_reset_enter(Object *obj, ResetType type) {
	ATSAM4E16EUART *t = ATSAM4E16E_UART(obj);
	atsam4e16e_uart_register_reset(t);
}

static const VMStateDescription atsam4e16e_uart_vmstate = {
	.name = "atsam4e16e-uart",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR, ATSAM4E16EUART),
		VMSTATE_UINT32(MR, ATSAM4E16EUART),
		VMSTATE_UINT32(IER, ATSAM4E16EUART),
		VMSTATE_UINT32(IDR, ATSAM4E16EUART),
		VMSTATE_UINT32(IMR, ATSAM4E16EUART),
		VMSTATE_UINT32(SR, ATSAM4E16EUART),
		VMSTATE_UINT32(RHR, ATSAM4E16EUART),
		VMSTATE_UINT32(THR, ATSAM4E16EUART),
		VMSTATE_UINT32(BRGR, ATSAM4E16EUART),
		VMSTATE_UINT32(WPMR, ATSAM4E16EUART),
		VMSTATE_UINT32(RPR, ATSAM4E16EUART),
		VMSTATE_UINT32(RCR, ATSAM4E16EUART),
		VMSTATE_UINT32(TPR, ATSAM4E16EUART),
		VMSTATE_UINT32(TCR, ATSAM4E16EUART),
		VMSTATE_UINT32(RNPR, ATSAM4E16EUART),
		VMSTATE_UINT32(RNCR, ATSAM4E16EUART),
		VMSTATE_UINT32(TNPR, ATSAM4E16EUART),
		VMSTATE_UINT32(TNCR, ATSAM4E16EUART),
		VMSTATE_UINT32(PTCR, ATSAM4E16EUART),
		VMSTATE_UINT32(PTSR, ATSAM4E16EUART),

		VMSTATE_END_OF_LIST()
	}
};

static Property atsam4e16e_uart_properties[] = {
	DEFINE_PROP_CHR("chardev", ATSAM4E16EUART, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void atsam4e16e_uart_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsam4e16e_uart_vmstate;
	dc->realize = atsam4e16e_uart_realize;
	rc->phases.enter = atsam4e16e_uart_reset_enter;
	device_class_set_props(dc, atsam4e16e_uart_properties);

}

static const TypeInfo atsam4e16e_uart_info = {
	.name = TYPE_ATSAM4E16E_UART,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAM4E16EUART),
	.instance_init = atsam4e16e_uart_init,
	.class_init = atsam4e16e_uart_class_init,
};

static void atsam4e16e_uart_register_types(void) {
	type_register_static(&atsam4e16e_uart_info);
}

type_init(atsam4e16e_uart_register_types);

#define TYPE_ATSAM4E16E_USART "atsam4e16e-usart"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAM4E16EUSART, ATSAM4E16E_USART)

struct ATSAM4E16EUSART {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t CR;
	uint32_t CR_SPI_MODE;
	uint32_t MR;
	uint32_t MR_SPI_MODE;
	uint32_t IER;
	uint32_t IER_SPI_MODE;
	uint32_t IDR;
	uint32_t IDR_SPI_MODE;
	uint32_t IMR;
	uint32_t IMR_SPI_MODE;
	uint32_t CSR;
	uint32_t CSR_SPI_MODE;
	uint32_t RHR;
	uint32_t THR;
	uint32_t BRGR;
	uint32_t RTOR;
	uint32_t TTGR;
	uint32_t FIDI;
	uint32_t NER;
	uint32_t IF;
	uint32_t MAN;
	uint32_t WPMR;
	uint32_t WPSR;
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
#define ATSAM4E16E_USART_SIZE				0x128

REG32(USART_CR, 0x0)
	FIELD(USART_CR, RSTRX, 2, 1)
	FIELD(USART_CR, RSTTX, 3, 1)
	FIELD(USART_CR, RXEN, 4, 1)
	FIELD(USART_CR, RXDIS, 5, 1)
	FIELD(USART_CR, TXEN, 6, 1)
	FIELD(USART_CR, TXDIS, 7, 1)
	FIELD(USART_CR, RSTSTA, 8, 1)
	FIELD(USART_CR, STTBRK, 9, 1)
	FIELD(USART_CR, STPBRK, 10, 1)
	FIELD(USART_CR, STTTO, 11, 1)
	FIELD(USART_CR, SENDA, 12, 1)
	FIELD(USART_CR, RSTIT, 13, 1)
	FIELD(USART_CR, RSTNACK, 14, 1)
	FIELD(USART_CR, RETTO, 15, 1)
	FIELD(USART_CR, DTREN, 16, 1)
	FIELD(USART_CR, DTRDIS, 17, 1)
	FIELD(USART_CR, RTSEN, 18, 1)
	FIELD(USART_CR, RTSDIS, 19, 1)
REG32(USART_CR_SPI_MODE, 0x0)
	FIELD(USART_CR_SPI_MODE, RSTRX, 2, 1)
	FIELD(USART_CR_SPI_MODE, RSTTX, 3, 1)
	FIELD(USART_CR_SPI_MODE, RXEN, 4, 1)
	FIELD(USART_CR_SPI_MODE, RXDIS, 5, 1)
	FIELD(USART_CR_SPI_MODE, TXEN, 6, 1)
	FIELD(USART_CR_SPI_MODE, TXDIS, 7, 1)
	FIELD(USART_CR_SPI_MODE, RSTSTA, 8, 1)
	FIELD(USART_CR_SPI_MODE, FCS, 18, 1)
	FIELD(USART_CR_SPI_MODE, RCS, 19, 1)
REG32(USART_MR, 0x4)
	FIELD(USART_MR, USART_MODE, 0, 4)
	FIELD(USART_MR, USCLKS, 4, 2)
	FIELD(USART_MR, CHRL, 6, 2)
	FIELD(USART_MR, SYNC, 8, 1)
	FIELD(USART_MR, PAR, 9, 3)
	FIELD(USART_MR, NBSTOP, 12, 2)
	FIELD(USART_MR, CHMODE, 14, 2)
	FIELD(USART_MR, MSBF, 16, 1)
	FIELD(USART_MR, MODE9, 17, 1)
	FIELD(USART_MR, CLKO, 18, 1)
	FIELD(USART_MR, OVER, 19, 1)
	FIELD(USART_MR, INACK, 20, 1)
	FIELD(USART_MR, DSNACK, 21, 1)
	FIELD(USART_MR, VAR_SYNC, 22, 1)
	FIELD(USART_MR, INVDATA, 23, 1)
	FIELD(USART_MR, MAX_ITERATION, 24, 3)
	FIELD(USART_MR, FILTER, 28, 1)
	FIELD(USART_MR, MAN, 29, 1)
	FIELD(USART_MR, MODSYNC, 30, 1)
	FIELD(USART_MR, ONEBIT, 31, 1)
REG32(USART_MR_SPI_MODE, 0x4)
	FIELD(USART_MR_SPI_MODE, USART_MODE, 0, 4)
	FIELD(USART_MR_SPI_MODE, USCLKS, 4, 2)
	FIELD(USART_MR_SPI_MODE, CHRL, 6, 2)
	FIELD(USART_MR_SPI_MODE, CPHA, 8, 1)
	FIELD(USART_MR_SPI_MODE, CPOL, 16, 1)
	FIELD(USART_MR_SPI_MODE, WRDBT, 20, 1)
REG32(USART_IER, 0x8)
	FIELD(USART_IER, RXRDY, 0, 1)
	FIELD(USART_IER, TXRDY, 1, 1)
	FIELD(USART_IER, RXBRK, 2, 1)
	FIELD(USART_IER, ENDRX, 3, 1)
	FIELD(USART_IER, ENDTX, 4, 1)
	FIELD(USART_IER, OVRE, 5, 1)
	FIELD(USART_IER, FRAME, 6, 1)
	FIELD(USART_IER, PARE, 7, 1)
	FIELD(USART_IER, TIMEOUT, 8, 1)
	FIELD(USART_IER, TXEMPTY, 9, 1)
	FIELD(USART_IER, ITER, 10, 1)
	FIELD(USART_IER, TXBUFE, 11, 1)
	FIELD(USART_IER, RXBUFF, 12, 1)
	FIELD(USART_IER, NACK, 13, 1)
	FIELD(USART_IER, RIIC, 16, 1)
	FIELD(USART_IER, DSRIC, 17, 1)
	FIELD(USART_IER, DCDIC, 18, 1)
	FIELD(USART_IER, CTSIC, 19, 1)
	FIELD(USART_IER, MANE, 24, 1)
REG32(USART_IER_SPI_MODE, 0x8)
	FIELD(USART_IER_SPI_MODE, RXRDY, 0, 1)
	FIELD(USART_IER_SPI_MODE, TXRDY, 1, 1)
	FIELD(USART_IER_SPI_MODE, ENDRX, 3, 1)
	FIELD(USART_IER_SPI_MODE, ENDTX, 4, 1)
	FIELD(USART_IER_SPI_MODE, OVRE, 5, 1)
	FIELD(USART_IER_SPI_MODE, TXEMPTY, 9, 1)
	FIELD(USART_IER_SPI_MODE, UNRE, 10, 1)
	FIELD(USART_IER_SPI_MODE, TXBUFE, 11, 1)
	FIELD(USART_IER_SPI_MODE, RXBUFF, 12, 1)
REG32(USART_IDR, 0xc)
	FIELD(USART_IDR, RXRDY, 0, 1)
	FIELD(USART_IDR, TXRDY, 1, 1)
	FIELD(USART_IDR, RXBRK, 2, 1)
	FIELD(USART_IDR, ENDRX, 3, 1)
	FIELD(USART_IDR, ENDTX, 4, 1)
	FIELD(USART_IDR, OVRE, 5, 1)
	FIELD(USART_IDR, FRAME, 6, 1)
	FIELD(USART_IDR, PARE, 7, 1)
	FIELD(USART_IDR, TIMEOUT, 8, 1)
	FIELD(USART_IDR, TXEMPTY, 9, 1)
	FIELD(USART_IDR, ITER, 10, 1)
	FIELD(USART_IDR, TXBUFE, 11, 1)
	FIELD(USART_IDR, RXBUFF, 12, 1)
	FIELD(USART_IDR, NACK, 13, 1)
	FIELD(USART_IDR, RIIC, 16, 1)
	FIELD(USART_IDR, DSRIC, 17, 1)
	FIELD(USART_IDR, DCDIC, 18, 1)
	FIELD(USART_IDR, CTSIC, 19, 1)
	FIELD(USART_IDR, MANE, 24, 1)
REG32(USART_IDR_SPI_MODE, 0xc)
	FIELD(USART_IDR_SPI_MODE, RXRDY, 0, 1)
	FIELD(USART_IDR_SPI_MODE, TXRDY, 1, 1)
	FIELD(USART_IDR_SPI_MODE, ENDRX, 3, 1)
	FIELD(USART_IDR_SPI_MODE, ENDTX, 4, 1)
	FIELD(USART_IDR_SPI_MODE, OVRE, 5, 1)
	FIELD(USART_IDR_SPI_MODE, TXEMPTY, 9, 1)
	FIELD(USART_IDR_SPI_MODE, UNRE, 10, 1)
	FIELD(USART_IDR_SPI_MODE, TXBUFE, 11, 1)
	FIELD(USART_IDR_SPI_MODE, RXBUFF, 12, 1)
REG32(USART_IMR, 0x10)
	FIELD(USART_IMR, RXRDY, 0, 1)
	FIELD(USART_IMR, TXRDY, 1, 1)
	FIELD(USART_IMR, RXBRK, 2, 1)
	FIELD(USART_IMR, ENDRX, 3, 1)
	FIELD(USART_IMR, ENDTX, 4, 1)
	FIELD(USART_IMR, OVRE, 5, 1)
	FIELD(USART_IMR, FRAME, 6, 1)
	FIELD(USART_IMR, PARE, 7, 1)
	FIELD(USART_IMR, TIMEOUT, 8, 1)
	FIELD(USART_IMR, TXEMPTY, 9, 1)
	FIELD(USART_IMR, ITER, 10, 1)
	FIELD(USART_IMR, TXBUFE, 11, 1)
	FIELD(USART_IMR, RXBUFF, 12, 1)
	FIELD(USART_IMR, NACK, 13, 1)
	FIELD(USART_IMR, RIIC, 16, 1)
	FIELD(USART_IMR, DSRIC, 17, 1)
	FIELD(USART_IMR, DCDIC, 18, 1)
	FIELD(USART_IMR, CTSIC, 19, 1)
	FIELD(USART_IMR, MANE, 24, 1)
REG32(USART_IMR_SPI_MODE, 0x10)
	FIELD(USART_IMR_SPI_MODE, RXRDY, 0, 1)
	FIELD(USART_IMR_SPI_MODE, TXRDY, 1, 1)
	FIELD(USART_IMR_SPI_MODE, ENDRX, 3, 1)
	FIELD(USART_IMR_SPI_MODE, ENDTX, 4, 1)
	FIELD(USART_IMR_SPI_MODE, OVRE, 5, 1)
	FIELD(USART_IMR_SPI_MODE, TXEMPTY, 9, 1)
	FIELD(USART_IMR_SPI_MODE, UNRE, 10, 1)
	FIELD(USART_IMR_SPI_MODE, TXBUFE, 11, 1)
	FIELD(USART_IMR_SPI_MODE, RXBUFF, 12, 1)
REG32(USART_CSR, 0x14)
	FIELD(USART_CSR, RXRDY, 0, 1)
	FIELD(USART_CSR, TXRDY, 1, 1)
	FIELD(USART_CSR, RXBRK, 2, 1)
	FIELD(USART_CSR, ENDRX, 3, 1)
	FIELD(USART_CSR, ENDTX, 4, 1)
	FIELD(USART_CSR, OVRE, 5, 1)
	FIELD(USART_CSR, FRAME, 6, 1)
	FIELD(USART_CSR, PARE, 7, 1)
	FIELD(USART_CSR, TIMEOUT, 8, 1)
	FIELD(USART_CSR, TXEMPTY, 9, 1)
	FIELD(USART_CSR, ITER, 10, 1)
	FIELD(USART_CSR, TXBUFE, 11, 1)
	FIELD(USART_CSR, RXBUFF, 12, 1)
	FIELD(USART_CSR, NACK, 13, 1)
	FIELD(USART_CSR, RIIC, 16, 1)
	FIELD(USART_CSR, DSRIC, 17, 1)
	FIELD(USART_CSR, DCDIC, 18, 1)
	FIELD(USART_CSR, CTSIC, 19, 1)
	FIELD(USART_CSR, RI, 20, 1)
	FIELD(USART_CSR, DSR, 21, 1)
	FIELD(USART_CSR, DCD, 22, 1)
	FIELD(USART_CSR, CTS, 23, 1)
	FIELD(USART_CSR, MANERR, 24, 1)
REG32(USART_CSR_SPI_MODE, 0x14)
	FIELD(USART_CSR_SPI_MODE, RXRDY, 0, 1)
	FIELD(USART_CSR_SPI_MODE, TXRDY, 1, 1)
	FIELD(USART_CSR_SPI_MODE, ENDRX, 3, 1)
	FIELD(USART_CSR_SPI_MODE, ENDTX, 4, 1)
	FIELD(USART_CSR_SPI_MODE, OVRE, 5, 1)
	FIELD(USART_CSR_SPI_MODE, TXEMPTY, 9, 1)
	FIELD(USART_CSR_SPI_MODE, UNRE, 10, 1)
	FIELD(USART_CSR_SPI_MODE, TXBUFE, 11, 1)
	FIELD(USART_CSR_SPI_MODE, RXBUFF, 12, 1)
REG32(USART_RHR, 0x18)
	FIELD(USART_RHR, RXCHR, 0, 9)
	FIELD(USART_RHR, RXSYNH, 15, 1)
REG32(USART_THR, 0x1c)
	FIELD(USART_THR, TXCHR, 0, 9)
	FIELD(USART_THR, TXSYNH, 15, 1)
REG32(USART_BRGR, 0x20)
	FIELD(USART_BRGR, CD, 0, 16)
	FIELD(USART_BRGR, FP, 16, 3)
REG32(USART_RTOR, 0x24)
	FIELD(USART_RTOR, TO, 0, 16)
REG32(USART_TTGR, 0x28)
	FIELD(USART_TTGR, TG, 0, 8)
REG32(USART_FIDI, 0x40)
	FIELD(USART_FIDI, FI_DI_RATIO, 0, 11)
REG32(USART_NER, 0x44)
	FIELD(USART_NER, NB_ERRORS, 0, 8)
REG32(USART_IF, 0x4c)
	FIELD(USART_IF, IRDA_FILTER, 0, 8)
REG32(USART_MAN, 0x50)
	FIELD(USART_MAN, TX_PL, 0, 4)
	FIELD(USART_MAN, TX_PP, 8, 2)
	FIELD(USART_MAN, TX_MPOL, 12, 1)
	FIELD(USART_MAN, RX_PL, 16, 4)
	FIELD(USART_MAN, RX_PP, 24, 2)
	FIELD(USART_MAN, RX_MPOL, 28, 1)
	FIELD(USART_MAN, ONE, 29, 1)
	FIELD(USART_MAN, DRIFT, 30, 1)
REG32(USART_WPMR, 0xe4)
	FIELD(USART_WPMR, WPEN, 0, 1)
	FIELD(USART_WPMR, WPKEY, 8, 24)
REG32(USART_WPSR, 0xe8)
	FIELD(USART_WPSR, WPVS, 0, 1)
	FIELD(USART_WPSR, WPVSRC, 8, 16)
REG32(USART_RPR, 0x100)
	FIELD(USART_RPR, RXPTR, 0, 32)
REG32(USART_RCR, 0x104)
	FIELD(USART_RCR, RXCTR, 0, 16)
REG32(USART_TPR, 0x108)
	FIELD(USART_TPR, TXPTR, 0, 32)
REG32(USART_TCR, 0x10c)
	FIELD(USART_TCR, TXCTR, 0, 16)
REG32(USART_RNPR, 0x110)
	FIELD(USART_RNPR, RXNPTR, 0, 32)
REG32(USART_RNCR, 0x114)
	FIELD(USART_RNCR, RXNCTR, 0, 16)
REG32(USART_TNPR, 0x118)
	FIELD(USART_TNPR, TXNPTR, 0, 32)
REG32(USART_TNCR, 0x11c)
	FIELD(USART_TNCR, TXNCTR, 0, 16)
REG32(USART_PTCR, 0x120)
	FIELD(USART_PTCR, RXTEN, 0, 1)
	FIELD(USART_PTCR, RXTDIS, 1, 1)
	FIELD(USART_PTCR, TXTEN, 8, 1)
	FIELD(USART_PTCR, TXTDIS, 9, 1)
REG32(USART_PTSR, 0x124)
	FIELD(USART_PTSR, RXTEN, 0, 1)
	FIELD(USART_PTSR, TXTEN, 8, 1)


__attribute__((unused))
static void atsam4e16e_usart_update(ATSAM4E16EUSART *t) {
	int conditions = (((t->IMR & 0x1) && (t->CSR & 0x1)) || ((t->IMR & 0x2) && (t->CSR & 0x2)) || ((t->CSR & 0x200) && (t->IMR & 0x200)));
	qemu_set_irq(t->irq[0], conditions);

}

static int atsam4e16e_usart_can_receive(void *opaque) {
	return 1;
}

static void atsam4e16e_usart_receive(void *opaque, const uint8_t *buf, int size) {
	ATSAM4E16EUSART *t = ATSAM4E16E_USART(opaque);

	t->RHR = *buf;
	t->CSR |= 0x1;

	atsam4e16e_usart_update(t);
}

static gboolean atsam4e16e_usart_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	ATSAM4E16EUSART *t = ATSAM4E16E_USART(opaque);
	int ret;

	t->watch_tag = 0;
	t->CSR &= (~(0x2));

	atsam4e16e_usart_update(t);

	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->THR), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, atsam4e16e_usart_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->CSR |= 0x2;

	atsam4e16e_usart_update(t);

	return FALSE;
}

static void atsam4e16e_usart_register_reset(ATSAM4E16EUSART *t) {
	t->IMR = 0x0;
	t->IMR_SPI_MODE = 0x0;
	t->RHR = 0x0;
	t->BRGR = 0x0;
	t->RTOR = 0x0;
	t->TTGR = 0x0;
	t->FIDI = 0x174;
	t->IF = 0x0;
	t->MAN = 0xb0011004;
	t->WPMR = 0x0;
	t->WPSR = 0x0;
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
	t->CSR |= 0x2;

}

static uint64_t atsam4e16e_usart_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAM4E16EUSART *t = ATSAM4E16E_USART(opaque);
	uint64_t ret;

	switch (offset) {
		case A_USART_MR:
			ret = t->MR;
			break;
		case A_USART_IMR:
			ret = t->IMR;
			break;
		case A_USART_CSR:
			ret = t->CSR;
			break;
		case A_USART_RHR:
			ret = t->RHR;
			t->CSR &= (~(0x1));
			qemu_chr_fe_accept_input(&(t->chr));
			atsam4e16e_usart_update(t);
			break;
		case A_USART_BRGR:
			ret = t->BRGR;
			break;
		case A_USART_RTOR:
			ret = t->RTOR;
			break;
		case A_USART_TTGR:
			ret = t->TTGR;
			break;
		case A_USART_FIDI:
			ret = t->FIDI;
			break;
		case A_USART_NER:
			ret = t->NER;
			break;
		case A_USART_IF:
			ret = t->IF;
			break;
		case A_USART_MAN:
			ret = t->MAN;
			break;
		case A_USART_WPMR:
			ret = t->WPMR;
			break;
		case A_USART_WPSR:
			ret = t->WPSR;
			break;
		case A_USART_RPR:
			ret = t->RPR;
			break;
		case A_USART_RCR:
			ret = t->RCR;
			break;
		case A_USART_TPR:
			ret = t->TPR;
			break;
		case A_USART_TCR:
			ret = t->TCR;
			break;
		case A_USART_RNPR:
			ret = t->RNPR;
			break;
		case A_USART_RNCR:
			ret = t->RNCR;
			break;
		case A_USART_TNPR:
			ret = t->TNPR;
			break;
		case A_USART_TNCR:
			ret = t->TNCR;
			break;
		case A_USART_PTSR:
			ret = t->PTSR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM4E16E USART read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsam4e16e_usart_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAM4E16EUSART *t = ATSAM4E16E_USART(opaque);


	switch (offset) {
		case A_USART_CR ... A_USART_CR + 3:
			value = value << ((offset - A_USART_CR) << 3);
			offset = A_USART_CR;
			break;
		case A_USART_MR ... A_USART_MR + 3:
			value = value << ((offset - A_USART_MR) << 3);
			offset = A_USART_MR;
			break;
		case A_USART_IER ... A_USART_IER + 3:
			value = value << ((offset - A_USART_IER) << 3);
			offset = A_USART_IER;
			break;
		case A_USART_IDR ... A_USART_IDR + 3:
			value = value << ((offset - A_USART_IDR) << 3);
			offset = A_USART_IDR;
			break;
		case A_USART_THR ... A_USART_THR + 3:
			value = value << ((offset - A_USART_THR) << 3);
			offset = A_USART_THR;
			break;
		case A_USART_BRGR ... A_USART_BRGR + 3:
			value = value << ((offset - A_USART_BRGR) << 3);
			offset = A_USART_BRGR;
			break;
		case A_USART_RTOR ... A_USART_RTOR + 3:
			value = value << ((offset - A_USART_RTOR) << 3);
			offset = A_USART_RTOR;
			break;
		case A_USART_TTGR ... A_USART_TTGR + 3:
			value = value << ((offset - A_USART_TTGR) << 3);
			offset = A_USART_TTGR;
			break;
		case A_USART_FIDI ... A_USART_FIDI + 3:
			value = value << ((offset - A_USART_FIDI) << 3);
			offset = A_USART_FIDI;
			break;
		case A_USART_IF ... A_USART_IF + 3:
			value = value << ((offset - A_USART_IF) << 3);
			offset = A_USART_IF;
			break;
		case A_USART_MAN ... A_USART_MAN + 3:
			value = value << ((offset - A_USART_MAN) << 3);
			offset = A_USART_MAN;
			break;
		case A_USART_WPMR ... A_USART_WPMR + 3:
			value = value << ((offset - A_USART_WPMR) << 3);
			offset = A_USART_WPMR;
			break;
		case A_USART_RPR ... A_USART_RPR + 3:
			value = value << ((offset - A_USART_RPR) << 3);
			offset = A_USART_RPR;
			break;
		case A_USART_RCR ... A_USART_RCR + 3:
			value = value << ((offset - A_USART_RCR) << 3);
			offset = A_USART_RCR;
			break;
		case A_USART_TPR ... A_USART_TPR + 3:
			value = value << ((offset - A_USART_TPR) << 3);
			offset = A_USART_TPR;
			break;
		case A_USART_TCR ... A_USART_TCR + 3:
			value = value << ((offset - A_USART_TCR) << 3);
			offset = A_USART_TCR;
			break;
		case A_USART_RNPR ... A_USART_RNPR + 3:
			value = value << ((offset - A_USART_RNPR) << 3);
			offset = A_USART_RNPR;
			break;
		case A_USART_RNCR ... A_USART_RNCR + 3:
			value = value << ((offset - A_USART_RNCR) << 3);
			offset = A_USART_RNCR;
			break;
		case A_USART_TNPR ... A_USART_TNPR + 3:
			value = value << ((offset - A_USART_TNPR) << 3);
			offset = A_USART_TNPR;
			break;
		case A_USART_TNCR ... A_USART_TNCR + 3:
			value = value << ((offset - A_USART_TNCR) << 3);
			offset = A_USART_TNCR;
			break;
		case A_USART_PTCR ... A_USART_PTCR + 3:
			value = value << ((offset - A_USART_PTCR) << 3);
			offset = A_USART_PTCR;
			break;

		default: break;
	}

	switch (offset) {
		case A_USART_CR:
			t->CR = value;
			break;
		case A_USART_MR:
			t->MR = value;
			break;
		case A_USART_IER:
			t->IER = value;
			break;
		case A_USART_IDR:
			t->IDR = value;
			break;
		case A_USART_THR:
			t->THR = value;
			atsam4e16e_usart_transmit(NULL, G_IO_OUT, t);
			if (((!(value & 0x100)) && (!(value & 0x200)) && (!(value & 0x400)) && (!(value & 0x800)) && (!(value & 0x1000)) && (!(value & 0x2000)) && (!(value & 0x4000)) && (!(value & 0x8000)) && (!(value & 0x10000)) && (!(value & 0x20000)) && (!(value & 0x40000)) && (!(value & 0x80000)) && (!(value & 0x100000)) && (!(value & 0x200000)) && (!(value & 0x400000)) && (!(value & 0x800000)) && (!(value & 0x1000000)) && (!(value & 0x2000000)) && (!(value & 0x4000000)) && (!(value & 0x8000000)) && (!(value & 0x10000000)) && (!(value & 0x20000000)) && (!(value & 0x40000000)) && (!(value & 0x80000000)))) {
				t->CSR |= 0x2;
			}
			atsam4e16e_usart_update(t);
			break;
		case A_USART_BRGR:
			t->BRGR = value;
			break;
		case A_USART_RTOR:
			t->RTOR = value;
			break;
		case A_USART_TTGR:
			t->TTGR = value;
			break;
		case A_USART_FIDI:
			t->FIDI = value;
			break;
		case A_USART_IF:
			t->IF = value;
			break;
		case A_USART_MAN:
			t->MAN = value;
			break;
		case A_USART_WPMR:
			t->WPMR = value;
			break;
		case A_USART_RPR:
			t->RPR = value;
			break;
		case A_USART_RCR:
			t->RCR = value;
			break;
		case A_USART_TPR:
			t->TPR = value;
			break;
		case A_USART_TCR:
			t->TCR = value;
			break;
		case A_USART_RNPR:
			t->RNPR = value;
			break;
		case A_USART_RNCR:
			t->RNCR = value;
			break;
		case A_USART_TNPR:
			t->TNPR = value;
			break;
		case A_USART_TNCR:
			t->TNCR = value;
			break;
		case A_USART_PTCR:
			t->PTCR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM4E16E USART write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsam4e16e_usart_ops = {
	.read = atsam4e16e_usart_read,
	.write = atsam4e16e_usart_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsam4e16e_usart_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAM4E16EUSART *t = ATSAM4E16E_USART(obj);
	memory_region_init_io(&(t->iomem), obj, &atsam4e16e_usart_ops, t, "atsam4e16e-usart", ATSAM4E16E_USART_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void atsam4e16e_usart_realize(DeviceState *dev, Error **errp) {
	ATSAM4E16EUSART *t = ATSAM4E16E_USART(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), atsam4e16e_usart_can_receive, atsam4e16e_usart_receive,
		NULL, NULL, t, NULL, true);

}

static void atsam4e16e_usart_reset_enter(Object *obj, ResetType type) {
	ATSAM4E16EUSART *t = ATSAM4E16E_USART(obj);
	atsam4e16e_usart_register_reset(t);
}

static const VMStateDescription atsam4e16e_usart_vmstate = {
	.name = "atsam4e16e-usart",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR, ATSAM4E16EUSART),
		VMSTATE_UINT32(CR_SPI_MODE, ATSAM4E16EUSART),
		VMSTATE_UINT32(MR, ATSAM4E16EUSART),
		VMSTATE_UINT32(MR_SPI_MODE, ATSAM4E16EUSART),
		VMSTATE_UINT32(IER, ATSAM4E16EUSART),
		VMSTATE_UINT32(IER_SPI_MODE, ATSAM4E16EUSART),
		VMSTATE_UINT32(IDR, ATSAM4E16EUSART),
		VMSTATE_UINT32(IDR_SPI_MODE, ATSAM4E16EUSART),
		VMSTATE_UINT32(IMR, ATSAM4E16EUSART),
		VMSTATE_UINT32(IMR_SPI_MODE, ATSAM4E16EUSART),
		VMSTATE_UINT32(CSR, ATSAM4E16EUSART),
		VMSTATE_UINT32(CSR_SPI_MODE, ATSAM4E16EUSART),
		VMSTATE_UINT32(RHR, ATSAM4E16EUSART),
		VMSTATE_UINT32(THR, ATSAM4E16EUSART),
		VMSTATE_UINT32(BRGR, ATSAM4E16EUSART),
		VMSTATE_UINT32(RTOR, ATSAM4E16EUSART),
		VMSTATE_UINT32(TTGR, ATSAM4E16EUSART),
		VMSTATE_UINT32(FIDI, ATSAM4E16EUSART),
		VMSTATE_UINT32(NER, ATSAM4E16EUSART),
		VMSTATE_UINT32(IF, ATSAM4E16EUSART),
		VMSTATE_UINT32(MAN, ATSAM4E16EUSART),
		VMSTATE_UINT32(WPMR, ATSAM4E16EUSART),
		VMSTATE_UINT32(WPSR, ATSAM4E16EUSART),
		VMSTATE_UINT32(RPR, ATSAM4E16EUSART),
		VMSTATE_UINT32(RCR, ATSAM4E16EUSART),
		VMSTATE_UINT32(TPR, ATSAM4E16EUSART),
		VMSTATE_UINT32(TCR, ATSAM4E16EUSART),
		VMSTATE_UINT32(RNPR, ATSAM4E16EUSART),
		VMSTATE_UINT32(RNCR, ATSAM4E16EUSART),
		VMSTATE_UINT32(TNPR, ATSAM4E16EUSART),
		VMSTATE_UINT32(TNCR, ATSAM4E16EUSART),
		VMSTATE_UINT32(PTCR, ATSAM4E16EUSART),
		VMSTATE_UINT32(PTSR, ATSAM4E16EUSART),

		VMSTATE_END_OF_LIST()
	}
};

static Property atsam4e16e_usart_properties[] = {
	DEFINE_PROP_CHR("chardev", ATSAM4E16EUSART, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void atsam4e16e_usart_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsam4e16e_usart_vmstate;
	dc->realize = atsam4e16e_usart_realize;
	rc->phases.enter = atsam4e16e_usart_reset_enter;
	device_class_set_props(dc, atsam4e16e_usart_properties);

}

static const TypeInfo atsam4e16e_usart_info = {
	.name = TYPE_ATSAM4E16E_USART,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAM4E16EUSART),
	.instance_init = atsam4e16e_usart_init,
	.class_init = atsam4e16e_usart_class_init,
};

static void atsam4e16e_usart_register_types(void) {
	type_register_static(&atsam4e16e_usart_info);
}

type_init(atsam4e16e_usart_register_types);

#define TYPE_ATSAM4E16E_PMC "atsam4e16e-pmc"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAM4E16EPMC, ATSAM4E16E_PMC)

struct ATSAM4E16EPMC {
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
	uint32_t PMC_OCR;
	uint32_t PMC_PMMR;
	uint32_t PMC_PCK0;
	uint32_t PMC_PCK1;
	uint32_t PMC_PCK2;
	/* base */
	uint32_t base;

};
#define ATSAM4E16E_PMC_SIZE				0x134

REG32(PMC_PMC_SCER, 0x0)
	FIELD(PMC_PMC_SCER, UDP, 7, 1)
	FIELD(PMC_PMC_SCER, PCK0, 8, 1)
	FIELD(PMC_PMC_SCER, PCK1, 9, 1)
	FIELD(PMC_PMC_SCER, PCK2, 10, 1)
REG32(PMC_PMC_SCDR, 0x4)
	FIELD(PMC_PMC_SCDR, UDP, 7, 1)
	FIELD(PMC_PMC_SCDR, PCK0, 8, 1)
	FIELD(PMC_PMC_SCDR, PCK1, 9, 1)
	FIELD(PMC_PMC_SCDR, PCK2, 10, 1)
REG32(PMC_PMC_SCSR, 0x8)
	FIELD(PMC_PMC_SCSR, UDP, 7, 1)
	FIELD(PMC_PMC_SCSR, PCK0, 8, 1)
	FIELD(PMC_PMC_SCSR, PCK1, 9, 1)
	FIELD(PMC_PMC_SCSR, PCK2, 10, 1)
REG32(PMC_PMC_PCER0, 0x10)
	FIELD(PMC_PMC_PCER0, PID7, 7, 1)
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
	FIELD(PMC_PMC_PCDR0, PID7, 7, 1)
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
	FIELD(PMC_PMC_PCSR0, PID7, 7, 1)
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
REG32(PMC_CKGR_MCFR, 0x24)
	FIELD(PMC_CKGR_MCFR, MAINF, 0, 16)
	FIELD(PMC_CKGR_MCFR, MAINFRDY, 16, 1)
	FIELD(PMC_CKGR_MCFR, RCMEAS, 20, 1)
REG32(PMC_CKGR_PLLAR, 0x28)
	FIELD(PMC_CKGR_PLLAR, DIVA, 0, 8)
	FIELD(PMC_CKGR_PLLAR, PLLACOUNT, 8, 6)
	FIELD(PMC_CKGR_PLLAR, MULA, 16, 11)
	FIELD(PMC_CKGR_PLLAR, ONE, 29, 1)
REG32(PMC_PMC_MCKR, 0x30)
	FIELD(PMC_PMC_MCKR, CSS, 0, 2)
	FIELD(PMC_PMC_MCKR, PRES, 4, 3)
	FIELD(PMC_PMC_MCKR, PLLADIV2, 12, 1)
REG32(PMC_PMC_USB, 0x38)
	FIELD(PMC_PMC_USB, USBDIV, 8, 4)
REG32(PMC_PMC_IER, 0x60)
	FIELD(PMC_PMC_IER, MOSCXTS, 0, 1)
	FIELD(PMC_PMC_IER, LOCKA, 1, 1)
	FIELD(PMC_PMC_IER, MCKRDY, 3, 1)
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
	FIELD(PMC_PMC_FSMR, FLPM, 21, 2)
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
	FIELD(PMC_PMC_PCER1, PID45, 13, 1)
	FIELD(PMC_PMC_PCER1, PID46, 14, 1)
	FIELD(PMC_PMC_PCER1, PID47, 15, 1)
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
	FIELD(PMC_PMC_PCDR1, PID45, 13, 1)
	FIELD(PMC_PMC_PCDR1, PID46, 14, 1)
	FIELD(PMC_PMC_PCDR1, PID47, 15, 1)
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
	FIELD(PMC_PMC_PCSR1, PID45, 13, 1)
	FIELD(PMC_PMC_PCSR1, PID46, 14, 1)
	FIELD(PMC_PMC_PCSR1, PID47, 15, 1)
REG32(PMC_PMC_OCR, 0x110)
	FIELD(PMC_PMC_OCR, CAL4, 0, 7)
	FIELD(PMC_PMC_OCR, SEL4, 7, 1)
	FIELD(PMC_PMC_OCR, CAL8, 8, 7)
	FIELD(PMC_PMC_OCR, SEL8, 15, 1)
	FIELD(PMC_PMC_OCR, CAL12, 16, 7)
	FIELD(PMC_PMC_OCR, SEL12, 23, 1)
REG32(PMC_PMC_PMMR, 0x130)
	FIELD(PMC_PMC_PMMR, PLLA_MMAX, 0, 11)
REG32(PMC_PMC_PCK0, 0x40)
	FIELD(PMC_PMC_PCK0, CSS, 0, 3)
	FIELD(PMC_PMC_PCK0, PRES, 4, 3)
REG32(PMC_PMC_PCK1, 0x44)
	FIELD(PMC_PMC_PCK1, CSS, 0, 3)
	FIELD(PMC_PMC_PCK1, PRES, 4, 3)
REG32(PMC_PMC_PCK2, 0x48)
	FIELD(PMC_PMC_PCK2, CSS, 0, 3)
	FIELD(PMC_PMC_PCK2, PRES, 4, 3)


static void atsam4e16e_pmc_register_reset(ATSAM4E16EPMC *t) {
	t->PMC_SCSR = 0x1;
	t->PMC_PCSR0 = 0x0;
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
	t->PMC_OCR = 0x404040;
	t->PMC_PMMR = 0x7ff07ff;

}

static uint64_t atsam4e16e_pmc_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAM4E16EPMC *t = ATSAM4E16E_PMC(opaque);
	uint64_t ret;

	switch (offset) {
		case A_PMC_PMC_SCSR:
			ret = t->PMC_SCSR;
			break;
		case A_PMC_PMC_PCSR0:
			ret = t->PMC_PCSR0;
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
		case A_PMC_PMC_OCR:
			ret = t->PMC_OCR;
			break;
		case A_PMC_PMC_PMMR:
			ret = t->PMC_PMMR;
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
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM4E16E PMC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsam4e16e_pmc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAM4E16EPMC *t = ATSAM4E16E_PMC(opaque);


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
		case A_PMC_PMC_OCR ... A_PMC_PMC_OCR + 3:
			value = value << ((offset - A_PMC_PMC_OCR) << 3);
			offset = A_PMC_PMC_OCR;
			break;
		case A_PMC_PMC_PMMR ... A_PMC_PMC_PMMR + 3:
			value = value << ((offset - A_PMC_PMC_PMMR) << 3);
			offset = A_PMC_PMC_PMMR;
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
			if ((value == 0xfffffffc)) {
				t->PMC_PCSR0 |= 0x800000;
				t->PMC_PCSR0 |= 0x80000;
				t->PMC_PCSR0 |= 0x80;
				t->PMC_PCSR0 |= 0x100;
				t->PMC_PCSR0 |= 0x40000;
				t->PMC_PCSR0 |= 0x1000000;
				t->PMC_PCSR0 |= 0x2000;
				t->PMC_PCSR0 |= 0x4000;
				t->PMC_PCSR0 |= 0x4;
				t->PMC_PCSR0 |= 0x400;
				t->PMC_PCSR0 |= 0x10;
				t->PMC_PCSR0 |= 0x2000000;
				t->PMC_PCSR0 |= 0x4000000;
				t->PMC_PCSR0 |= 0x10000000;
				t->PMC_PCSR0 |= 0x80000000;
				t->PMC_PCSR0 |= 0x8000;
				t->PMC_PCSR0 |= 0x20000000;
				t->PMC_PCSR0 |= 0x200000;
				t->PMC_PCSR0 |= 0x100000;
				t->PMC_PCSR0 |= 0x200;
				t->PMC_PCSR0 |= 0x8000000;
				t->PMC_PCSR0 |= 0x8;
				t->PMC_PCSR0 |= 0x20000;
				t->PMC_PCSR0 |= 0x40;
				t->PMC_PCSR0 |= 0x800;
				t->PMC_PCSR0 |= 0x10000;
				t->PMC_PCSR0 |= 0x1000;
				t->PMC_PCSR0 |= 0x20;
				t->PMC_PCSR0 |= 0x400000;
				t->PMC_PCSR0 |= 0x40000000;
			}
			break;
		case A_PMC_PMC_PCDR0:
			t->PMC_PCDR0 = value;
			if ((value == 0xfffffffc)) {
				t->PMC_PCSR0 &= (~(0x40));
				t->PMC_PCSR0 &= (~(0x10000000));
				t->PMC_PCSR0 &= (~(0x2000000));
				t->PMC_PCSR0 &= (~(0x2000));
				t->PMC_PCSR0 &= (~(0x1000000));
				t->PMC_PCSR0 &= (~(0x40000));
				t->PMC_PCSR0 &= (~(0x8));
				t->PMC_PCSR0 &= (~(0x800));
				t->PMC_PCSR0 &= (~(0x80));
				t->PMC_PCSR0 &= (~(0x10000));
				t->PMC_PCSR0 &= (~(0x200000));
				t->PMC_PCSR0 &= (~(0x400000));
				t->PMC_PCSR0 &= (~(0x40000000));
				t->PMC_PCSR0 &= (~(0x4000));
				t->PMC_PCSR0 &= (~(0x80000000));
				t->PMC_PCSR0 &= (~(0x400));
				t->PMC_PCSR0 &= (~(0x8000));
				t->PMC_PCSR0 &= (~(0x20000));
				t->PMC_PCSR0 &= (~(0x80000));
				t->PMC_PCSR0 &= (~(0x800000));
				t->PMC_PCSR0 &= (~(0x8000000));
				t->PMC_PCSR0 &= (~(0x20));
				t->PMC_PCSR0 &= (~(0x4000000));
				t->PMC_PCSR0 &= (~(0x200));
				t->PMC_PCSR0 &= (~(0x20000000));
				t->PMC_PCSR0 &= (~(0x1000));
				t->PMC_PCSR0 &= (~(0x10));
				t->PMC_PCSR0 &= (~(0x100000));
				t->PMC_PCSR0 &= (~(0x4));
				t->PMC_PCSR0 &= (~(0x100));
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
		case A_PMC_CKGR_MCFR:
			t->CKGR_MCFR = value;
			break;
		case A_PMC_CKGR_PLLAR:
			t->CKGR_PLLAR = value;
			t->PMC_SR |= 0x2;
			break;
		case A_PMC_PMC_MCKR:
			t->PMC_MCKR = value;
			if (((!(value & 0x1)) && (!(value & 0x2)))) {
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
			if ((value == 0xffffffff)) {
				t->PMC_PCSR1 |= 0x40;
				t->PMC_PCSR1 |= 0x200000;
				t->PMC_PCSR1 |= 0x10000;
				t->PMC_PCSR1 |= 0x8000;
				t->PMC_PCSR1 |= 0x8000000;
				t->PMC_PCSR1 |= 0x1;
				t->PMC_PCSR1 |= 0x200;
				t->PMC_PCSR1 |= 0x100;
				t->PMC_PCSR1 |= 0x4000000;
				t->PMC_PCSR1 |= 0x2;
				t->PMC_PCSR1 |= 0x2000000;
				t->PMC_PCSR1 |= 0x8;
				t->PMC_PCSR1 |= 0x1000000;
				t->PMC_PCSR1 |= 0x20;
				t->PMC_PCSR1 |= 0x40000000;
				t->PMC_PCSR1 |= 0x80000;
				t->PMC_PCSR1 |= 0x4;
				t->PMC_PCSR1 |= 0x800000;
				t->PMC_PCSR1 |= 0x10;
				t->PMC_PCSR1 |= 0x1000;
				t->PMC_PCSR1 |= 0x10000000;
				t->PMC_PCSR1 |= 0x80;
				t->PMC_PCSR1 |= 0x400;
				t->PMC_PCSR1 |= 0x400000;
				t->PMC_PCSR1 |= 0x2000;
				t->PMC_PCSR1 |= 0x800;
				t->PMC_PCSR1 |= 0x20000000;
				t->PMC_PCSR1 |= 0x100000;
				t->PMC_PCSR1 |= 0x40000;
				t->PMC_PCSR1 |= 0x80000000;
				t->PMC_PCSR1 |= 0x20000;
				t->PMC_PCSR1 |= 0x4000;
			}
			break;
		case A_PMC_PMC_PCDR1:
			t->PMC_PCDR1 = value;
			if ((value == 0xffffffff)) {
				t->PMC_PCSR1 &= (~(0x100000));
				t->PMC_PCSR1 &= (~(0x4000000));
				t->PMC_PCSR1 &= (~(0x4));
				t->PMC_PCSR1 &= (~(0x10));
				t->PMC_PCSR1 &= (~(0x200));
				t->PMC_PCSR1 &= (~(0x20000));
				t->PMC_PCSR1 &= (~(0x200000));
				t->PMC_PCSR1 &= (~(0x10000));
				t->PMC_PCSR1 &= (~(0x1));
				t->PMC_PCSR1 &= (~(0x40));
				t->PMC_PCSR1 &= (~(0x10000000));
				t->PMC_PCSR1 &= (~(0x2));
				t->PMC_PCSR1 &= (~(0x400000));
				t->PMC_PCSR1 &= (~(0x1000));
				t->PMC_PCSR1 &= (~(0x2000));
				t->PMC_PCSR1 &= (~(0x40000));
				t->PMC_PCSR1 &= (~(0x1000000));
				t->PMC_PCSR1 &= (~(0x400));
				t->PMC_PCSR1 &= (~(0x2000000));
				t->PMC_PCSR1 &= (~(0x20000000));
				t->PMC_PCSR1 &= (~(0x800000));
				t->PMC_PCSR1 &= (~(0x8000000));
				t->PMC_PCSR1 &= (~(0x20));
				t->PMC_PCSR1 &= (~(0x80000000));
				t->PMC_PCSR1 &= (~(0x40000000));
				t->PMC_PCSR1 &= (~(0x4000));
				t->PMC_PCSR1 &= (~(0x800));
				t->PMC_PCSR1 &= (~(0x80));
				t->PMC_PCSR1 &= (~(0x8));
				t->PMC_PCSR1 &= (~(0x100));
				t->PMC_PCSR1 &= (~(0x80000));
				t->PMC_PCSR1 &= (~(0x8000));
			}
			break;
		case A_PMC_PMC_OCR:
			t->PMC_OCR = value;
			break;
		case A_PMC_PMC_PMMR:
			t->PMC_PMMR = value;
			break;
		case A_PMC_PMC_PCK0:
			t->PMC_PCK0 = value;
			if ((value & 0x2)) {
				t->PMC_SR |= 0x100;
			}
			if ((value & 0x1)) {
				t->PMC_SR |= 0x100;
			}
			if ((value & 0x4)) {
				t->PMC_SR |= 0x100;
			}
			break;
		case A_PMC_PMC_PCK1:
			t->PMC_PCK1 = value;
			if ((value & 0x2)) {
				t->PMC_SR |= 0x200;
			}
			if ((value & 0x1)) {
				t->PMC_SR |= 0x200;
			}
			if ((value & 0x4)) {
				t->PMC_SR |= 0x200;
			}
			break;
		case A_PMC_PMC_PCK2:
			t->PMC_PCK2 = value;
			if ((value & 0x2)) {
				t->PMC_SR |= 0x400;
			}
			if ((value & 0x1)) {
				t->PMC_SR |= 0x400;
			}
			if ((value & 0x4)) {
				t->PMC_SR |= 0x400;
			}
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM4E16E PMC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsam4e16e_pmc_ops = {
	.read = atsam4e16e_pmc_read,
	.write = atsam4e16e_pmc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsam4e16e_pmc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAM4E16EPMC *t = ATSAM4E16E_PMC(obj);
	memory_region_init_io(&(t->iomem), obj, &atsam4e16e_pmc_ops, t, "atsam4e16e-pmc", ATSAM4E16E_PMC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void atsam4e16e_pmc_realize(DeviceState *dev, Error **errp) {
	return;

}

static void atsam4e16e_pmc_reset_enter(Object *obj, ResetType type) {
	ATSAM4E16EPMC *t = ATSAM4E16E_PMC(obj);
	atsam4e16e_pmc_register_reset(t);
}

static const VMStateDescription atsam4e16e_pmc_vmstate = {
	.name = "atsam4e16e-pmc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(PMC_SCER, ATSAM4E16EPMC),
		VMSTATE_UINT32(PMC_SCDR, ATSAM4E16EPMC),
		VMSTATE_UINT32(PMC_SCSR, ATSAM4E16EPMC),
		VMSTATE_UINT32(PMC_PCER0, ATSAM4E16EPMC),
		VMSTATE_UINT32(PMC_PCDR0, ATSAM4E16EPMC),
		VMSTATE_UINT32(PMC_PCSR0, ATSAM4E16EPMC),
		VMSTATE_UINT32(CKGR_MOR, ATSAM4E16EPMC),
		VMSTATE_UINT32(CKGR_MCFR, ATSAM4E16EPMC),
		VMSTATE_UINT32(CKGR_PLLAR, ATSAM4E16EPMC),
		VMSTATE_UINT32(PMC_MCKR, ATSAM4E16EPMC),
		VMSTATE_UINT32(PMC_USB, ATSAM4E16EPMC),
		VMSTATE_UINT32(PMC_IER, ATSAM4E16EPMC),
		VMSTATE_UINT32(PMC_IDR, ATSAM4E16EPMC),
		VMSTATE_UINT32(PMC_SR, ATSAM4E16EPMC),
		VMSTATE_UINT32(PMC_IMR, ATSAM4E16EPMC),
		VMSTATE_UINT32(PMC_FSMR, ATSAM4E16EPMC),
		VMSTATE_UINT32(PMC_FSPR, ATSAM4E16EPMC),
		VMSTATE_UINT32(PMC_FOCR, ATSAM4E16EPMC),
		VMSTATE_UINT32(PMC_WPMR, ATSAM4E16EPMC),
		VMSTATE_UINT32(PMC_WPSR, ATSAM4E16EPMC),
		VMSTATE_UINT32(PMC_PCER1, ATSAM4E16EPMC),
		VMSTATE_UINT32(PMC_PCDR1, ATSAM4E16EPMC),
		VMSTATE_UINT32(PMC_PCSR1, ATSAM4E16EPMC),
		VMSTATE_UINT32(PMC_OCR, ATSAM4E16EPMC),
		VMSTATE_UINT32(PMC_PMMR, ATSAM4E16EPMC),
		VMSTATE_UINT32(PMC_PCK0, ATSAM4E16EPMC),
		VMSTATE_UINT32(PMC_PCK1, ATSAM4E16EPMC),
		VMSTATE_UINT32(PMC_PCK2, ATSAM4E16EPMC),

		VMSTATE_END_OF_LIST()
	}
};

static void atsam4e16e_pmc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsam4e16e_pmc_vmstate;
	dc->realize = atsam4e16e_pmc_realize;
	rc->phases.enter = atsam4e16e_pmc_reset_enter;

}

static const TypeInfo atsam4e16e_pmc_info = {
	.name = TYPE_ATSAM4E16E_PMC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAM4E16EPMC),
	.instance_init = atsam4e16e_pmc_init,
	.class_init = atsam4e16e_pmc_class_init,
};

static void atsam4e16e_pmc_register_types(void) {
	type_register_static(&atsam4e16e_pmc_info);
}

type_init(atsam4e16e_pmc_register_types);

#define TYPE_ATSAM4E16E_SUPC "atsam4e16e-supc"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAM4E16ESUPC, ATSAM4E16E_SUPC)

struct ATSAM4E16ESUPC {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

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
#define ATSAM4E16E_SUPC_SIZE				0x18

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
	FIELD(SUPC_MR, OSCBYPASS, 20, 1)
	FIELD(SUPC_MR, KEY, 24, 8)
REG32(SUPC_WUMR, 0xc)
	FIELD(SUPC_WUMR, FWUPEN, 0, 1)
	FIELD(SUPC_WUMR, SMEN, 1, 1)
	FIELD(SUPC_WUMR, RTTEN, 2, 1)
	FIELD(SUPC_WUMR, RTCEN, 3, 1)
	FIELD(SUPC_WUMR, LPDBCEN0, 5, 1)
	FIELD(SUPC_WUMR, LPDBCEN1, 6, 1)
	FIELD(SUPC_WUMR, LPDBCCLR, 7, 1)
	FIELD(SUPC_WUMR, FWUPDBC, 8, 3)
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
	FIELD(SUPC_WUIR, WKUPEN14, 14, 1)
	FIELD(SUPC_WUIR, WKUPEN15, 15, 1)
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
	FIELD(SUPC_WUIR, WKUPT14, 30, 1)
	FIELD(SUPC_WUIR, WKUPT15, 31, 1)
REG32(SUPC_SR, 0x14)
	FIELD(SUPC_SR, FWUPS, 0, 1)
	FIELD(SUPC_SR, WKUPS, 1, 1)
	FIELD(SUPC_SR, SMWS, 2, 1)
	FIELD(SUPC_SR, BODRSTS, 3, 1)
	FIELD(SUPC_SR, SMRSTS, 4, 1)
	FIELD(SUPC_SR, SMS, 5, 1)
	FIELD(SUPC_SR, SMOS, 6, 1)
	FIELD(SUPC_SR, OSCSEL, 7, 1)
	FIELD(SUPC_SR, FWUPIS, 12, 1)
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
	FIELD(SUPC_SR, WKUPIS14, 30, 1)
	FIELD(SUPC_SR, WKUPIS15, 31, 1)


static void atsam4e16e_supc_register_reset(ATSAM4E16ESUPC *t) {
	t->SMMR = 0x0;
	t->MR = 0x5a00;
	t->WUMR = 0x0;
	t->WUIR = 0x0;
	t->SR = 0x0;

}

static uint64_t atsam4e16e_supc_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAM4E16ESUPC *t = ATSAM4E16E_SUPC(opaque);
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
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM4E16E SUPC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsam4e16e_supc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAM4E16ESUPC *t = ATSAM4E16E_SUPC(opaque);


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
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM4E16E SUPC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsam4e16e_supc_ops = {
	.read = atsam4e16e_supc_read,
	.write = atsam4e16e_supc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsam4e16e_supc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAM4E16ESUPC *t = ATSAM4E16E_SUPC(obj);
	memory_region_init_io(&(t->iomem), obj, &atsam4e16e_supc_ops, t, "atsam4e16e-supc", ATSAM4E16E_SUPC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void atsam4e16e_supc_realize(DeviceState *dev, Error **errp) {
	return;

}

static void atsam4e16e_supc_reset_enter(Object *obj, ResetType type) {
	ATSAM4E16ESUPC *t = ATSAM4E16E_SUPC(obj);
	atsam4e16e_supc_register_reset(t);
}

static const VMStateDescription atsam4e16e_supc_vmstate = {
	.name = "atsam4e16e-supc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR, ATSAM4E16ESUPC),
		VMSTATE_UINT32(SMMR, ATSAM4E16ESUPC),
		VMSTATE_UINT32(MR, ATSAM4E16ESUPC),
		VMSTATE_UINT32(WUMR, ATSAM4E16ESUPC),
		VMSTATE_UINT32(WUIR, ATSAM4E16ESUPC),
		VMSTATE_UINT32(SR, ATSAM4E16ESUPC),

		VMSTATE_END_OF_LIST()
	}
};

static void atsam4e16e_supc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsam4e16e_supc_vmstate;
	dc->realize = atsam4e16e_supc_realize;
	rc->phases.enter = atsam4e16e_supc_reset_enter;

}

static const TypeInfo atsam4e16e_supc_info = {
	.name = TYPE_ATSAM4E16E_SUPC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAM4E16ESUPC),
	.instance_init = atsam4e16e_supc_init,
	.class_init = atsam4e16e_supc_class_init,
};

static void atsam4e16e_supc_register_types(void) {
	type_register_static(&atsam4e16e_supc_info);
}

type_init(atsam4e16e_supc_register_types);

#define TYPE_ATSAM4E16E_PIO "atsam4e16e-pio"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAM4E16EPIO, ATSAM4E16E_PIO)

struct ATSAM4E16EPIO {
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
	uint32_t DELAYR;
	uint32_t PCMR;
	uint32_t PCIER;
	uint32_t PCIDR;
	uint32_t PCIMR;
	uint32_t PCISR;
	uint32_t PCRHR;
	uint32_t RPR;
	uint32_t RCR;
	uint32_t RNPR;
	uint32_t RNCR;
	uint32_t PTCR;
	uint32_t PTSR;
	uint32_t ABCDSR0;
	uint32_t ABCDSR1;
	/* chardev backend */
	CharBackend chr;
	/* base */
	uint32_t base;

};
#define ATSAM4E16E_PIO_SIZE				0x190

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
REG32(PIO_DELAYR, 0x110)
	FIELD(PIO_DELAYR, Delay0, 0, 4)
	FIELD(PIO_DELAYR, Delay1, 4, 4)
	FIELD(PIO_DELAYR, Delay2, 8, 4)
	FIELD(PIO_DELAYR, Delay3, 12, 4)
	FIELD(PIO_DELAYR, Delay4, 16, 4)
	FIELD(PIO_DELAYR, Delay5, 20, 4)
	FIELD(PIO_DELAYR, Delay6, 24, 4)
	FIELD(PIO_DELAYR, Delay7, 28, 4)
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
	FIELD(PIO_PCISR, ENDRX, 2, 1)
	FIELD(PIO_PCISR, RXBUFF, 3, 1)
REG32(PIO_PCRHR, 0x164)
	FIELD(PIO_PCRHR, RDATA, 0, 32)
REG32(PIO_RPR, 0x168)
	FIELD(PIO_RPR, RXPTR, 0, 32)
REG32(PIO_RCR, 0x16c)
	FIELD(PIO_RCR, RXCTR, 0, 16)
REG32(PIO_RNPR, 0x178)
	FIELD(PIO_RNPR, RXNPTR, 0, 32)
REG32(PIO_RNCR, 0x17c)
	FIELD(PIO_RNCR, RXNCTR, 0, 16)
REG32(PIO_PTCR, 0x188)
	FIELD(PIO_PTCR, RXTEN, 0, 1)
	FIELD(PIO_PTCR, RXTDIS, 1, 1)
	FIELD(PIO_PTCR, TXTEN, 8, 1)
	FIELD(PIO_PTCR, TXTDIS, 9, 1)
REG32(PIO_PTSR, 0x18c)
	FIELD(PIO_PTSR, RXTEN, 0, 1)
	FIELD(PIO_PTSR, TXTEN, 8, 1)
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
static void atsam4e16e_pio_update(ATSAM4E16EPIO *t) {
	int conditions = (((t->IMR & 0x2000000) && (t->ISR & 0x2000000)) || ((t->IMR & 0x80000000) && (t->ISR & 0x80000000)) || ((t->IMR & 0x8) && (t->ISR & 0x8)) || ((t->IMR & 0x80) && (t->ISR & 0x80)) || ((t->IMR & 0x20000000) && (t->ISR & 0x20000000)) || ((t->IMR & 0x8000) && (t->ISR & 0x8000)) || ((t->IMR & 0x200) && (t->ISR & 0x200)) || ((t->IMR & 0x100) && (t->ISR & 0x100)) || ((t->IMR & 0x1) && (t->ISR & 0x1)) || ((t->IMR & 0x100000) && (t->ISR & 0x100000)) || ((t->IMR & 0x800000) && (t->ISR & 0x800000)) || ((t->IMR & 0x40) && (t->ISR & 0x40)) || ((t->IMR & 0x1000) && (t->ISR & 0x1000)) || ((t->IMR & 0x2) && (t->ISR & 0x2)) || ((t->IMR & 0x10000000) && (t->ISR & 0x10000000)) || ((t->IMR & 0x4000000) && (t->ISR & 0x4000000)) || ((t->IMR & 0x1000000) && (t->ISR & 0x1000000)) || ((t->IMR & 0x200000) && (t->ISR & 0x200000)) || ((t->IMR & 0x4) && (t->ISR & 0x4)) || ((t->IMR & 0x4000) && (t->ISR & 0x4000)) || ((t->IMR & 0x40000) && (t->ISR & 0x40000)) || ((t->IMR & 0x40000000) && (t->ISR & 0x40000000)) || ((t->IMR & 0x800) && (t->ISR & 0x800)) || ((t->IMR & 0x80000) && (t->ISR & 0x80000)) || ((t->IMR & 0x8000000) && (t->ISR & 0x8000000)) || ((t->IMR & 0x400) && (t->ISR & 0x400)) || ((t->IMR & 0x20) && (t->ISR & 0x20)) || ((t->IMR & 0x10) && (t->ISR & 0x10)) || ((t->IMR & 0x10000) && (t->ISR & 0x10000)) || ((t->IMR & 0x400000) && (t->ISR & 0x400000)) || ((t->IMR & 0x2000) && (t->ISR & 0x2000)) || ((t->ISR & 0x20000000) && (t->IMR & 0x20000000)) || ((t->ISR & 0x10000000) && (t->IMR & 0x10000000)) || ((t->IMR & 0x20000) && (t->ISR & 0x20000)) || ((t->ISR & 0x80000000) && (t->IMR & 0x80000000)) || ((t->ISR & 0x40000000) && (t->IMR & 0x40000000)));
	qemu_set_irq(t->irq[0], conditions);
	qemu_set_irq(t->irq[0], 0);

}

static int atsam4e16e_pio_can_receive(void *opaque) {
	return 1;
}

static void atsam4e16e_pio_receive(void *opaque, const uint8_t *buf, int size) {
	ATSAM4E16EPIO *t = ATSAM4E16E_PIO(opaque);

	t->IMR = (1 << (*buf % 32));
	t->ISR = (1 << (*buf % 32));

	atsam4e16e_pio_update(t);
}

static void atsam4e16e_pio_register_reset(ATSAM4E16EPIO *t) {
	t->OSR = 0x0;
	t->IFSR = 0x0;
	t->IMR = 0x0;
	t->ISR = 0x0;
	t->MDSR = 0x0;
	t->IFSCSR = 0x0;
	t->SCDR = 0x0;
	t->OWSR = 0x0;
	t->AIMMR = 0x0;
	t->ELSR = 0x0;
	t->FRLHSR = 0x0;
	t->LOCKSR = 0x0;
	t->WPMR = 0x0;
	t->WPSR = 0x0;
	t->SCHMITT = 0x0;
	t->DELAYR = 0x0;
	t->PCMR = 0x0;
	t->PCIMR = 0x0;
	t->PCISR = 0x0;
	t->PCRHR = 0x0;
	t->RPR = 0x0;
	t->RCR = 0x0;
	t->RNPR = 0x0;
	t->RNCR = 0x0;
	t->PTCR = 0x0;
	t->PTSR = 0x0;

}

static uint64_t atsam4e16e_pio_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAM4E16EPIO *t = ATSAM4E16E_PIO(opaque);
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
		case A_PIO_DELAYR:
			ret = t->DELAYR;
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
		case A_PIO_RPR:
			ret = t->RPR;
			break;
		case A_PIO_RCR:
			ret = t->RCR;
			break;
		case A_PIO_RNPR:
			ret = t->RNPR;
			break;
		case A_PIO_RNCR:
			ret = t->RNCR;
			break;
		case A_PIO_PTSR:
			ret = t->PTSR;
			break;
		case A_PIO_ABCDSR0:
			ret = t->ABCDSR0;
			break;
		case A_PIO_ABCDSR1:
			ret = t->ABCDSR1;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM4E16E PIO read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsam4e16e_pio_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAM4E16EPIO *t = ATSAM4E16E_PIO(opaque);


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
		case A_PIO_DELAYR ... A_PIO_DELAYR + 3:
			value = value << ((offset - A_PIO_DELAYR) << 3);
			offset = A_PIO_DELAYR;
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
		case A_PIO_RPR ... A_PIO_RPR + 3:
			value = value << ((offset - A_PIO_RPR) << 3);
			offset = A_PIO_RPR;
			break;
		case A_PIO_RCR ... A_PIO_RCR + 3:
			value = value << ((offset - A_PIO_RCR) << 3);
			offset = A_PIO_RCR;
			break;
		case A_PIO_RNPR ... A_PIO_RNPR + 3:
			value = value << ((offset - A_PIO_RNPR) << 3);
			offset = A_PIO_RNPR;
			break;
		case A_PIO_RNCR ... A_PIO_RNCR + 3:
			value = value << ((offset - A_PIO_RNCR) << 3);
			offset = A_PIO_RNCR;
			break;
		case A_PIO_PTCR ... A_PIO_PTCR + 3:
			value = value << ((offset - A_PIO_PTCR) << 3);
			offset = A_PIO_PTCR;
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
		case A_PIO_DELAYR:
			t->DELAYR = value;
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
		case A_PIO_RPR:
			t->RPR = value;
			break;
		case A_PIO_RCR:
			t->RCR = value;
			break;
		case A_PIO_RNPR:
			t->RNPR = value;
			break;
		case A_PIO_RNCR:
			t->RNCR = value;
			break;
		case A_PIO_PTCR:
			t->PTCR = value;
			break;
		case A_PIO_ABCDSR0:
			t->ABCDSR0 = value;
			break;
		case A_PIO_ABCDSR1:
			t->ABCDSR1 = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM4E16E PIO write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsam4e16e_pio_ops = {
	.read = atsam4e16e_pio_read,
	.write = atsam4e16e_pio_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsam4e16e_pio_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAM4E16EPIO *t = ATSAM4E16E_PIO(obj);
	memory_region_init_io(&(t->iomem), obj, &atsam4e16e_pio_ops, t, "atsam4e16e-pio", ATSAM4E16E_PIO_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void atsam4e16e_pio_realize(DeviceState *dev, Error **errp) {
	ATSAM4E16EPIO *t = ATSAM4E16E_PIO(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), atsam4e16e_pio_can_receive, atsam4e16e_pio_receive,
		NULL, NULL, t, NULL, true);

}

static void atsam4e16e_pio_reset_enter(Object *obj, ResetType type) {
	ATSAM4E16EPIO *t = ATSAM4E16E_PIO(obj);
	atsam4e16e_pio_register_reset(t);
}

static const VMStateDescription atsam4e16e_pio_vmstate = {
	.name = "atsam4e16e-pio",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(PER, ATSAM4E16EPIO),
		VMSTATE_UINT32(PDR, ATSAM4E16EPIO),
		VMSTATE_UINT32(PSR, ATSAM4E16EPIO),
		VMSTATE_UINT32(OER, ATSAM4E16EPIO),
		VMSTATE_UINT32(ODR, ATSAM4E16EPIO),
		VMSTATE_UINT32(OSR, ATSAM4E16EPIO),
		VMSTATE_UINT32(IFER, ATSAM4E16EPIO),
		VMSTATE_UINT32(IFDR, ATSAM4E16EPIO),
		VMSTATE_UINT32(IFSR, ATSAM4E16EPIO),
		VMSTATE_UINT32(SODR, ATSAM4E16EPIO),
		VMSTATE_UINT32(CODR, ATSAM4E16EPIO),
		VMSTATE_UINT32(ODSR, ATSAM4E16EPIO),
		VMSTATE_UINT32(PDSR, ATSAM4E16EPIO),
		VMSTATE_UINT32(IER, ATSAM4E16EPIO),
		VMSTATE_UINT32(IDR, ATSAM4E16EPIO),
		VMSTATE_UINT32(IMR, ATSAM4E16EPIO),
		VMSTATE_UINT32(ISR, ATSAM4E16EPIO),
		VMSTATE_UINT32(MDER, ATSAM4E16EPIO),
		VMSTATE_UINT32(MDDR, ATSAM4E16EPIO),
		VMSTATE_UINT32(MDSR, ATSAM4E16EPIO),
		VMSTATE_UINT32(PUDR, ATSAM4E16EPIO),
		VMSTATE_UINT32(PUER, ATSAM4E16EPIO),
		VMSTATE_UINT32(PUSR, ATSAM4E16EPIO),
		VMSTATE_UINT32(IFSCDR, ATSAM4E16EPIO),
		VMSTATE_UINT32(IFSCER, ATSAM4E16EPIO),
		VMSTATE_UINT32(IFSCSR, ATSAM4E16EPIO),
		VMSTATE_UINT32(SCDR, ATSAM4E16EPIO),
		VMSTATE_UINT32(PPDDR, ATSAM4E16EPIO),
		VMSTATE_UINT32(PPDER, ATSAM4E16EPIO),
		VMSTATE_UINT32(PPDSR, ATSAM4E16EPIO),
		VMSTATE_UINT32(OWER, ATSAM4E16EPIO),
		VMSTATE_UINT32(OWDR, ATSAM4E16EPIO),
		VMSTATE_UINT32(OWSR, ATSAM4E16EPIO),
		VMSTATE_UINT32(AIMER, ATSAM4E16EPIO),
		VMSTATE_UINT32(AIMDR, ATSAM4E16EPIO),
		VMSTATE_UINT32(AIMMR, ATSAM4E16EPIO),
		VMSTATE_UINT32(ESR, ATSAM4E16EPIO),
		VMSTATE_UINT32(LSR, ATSAM4E16EPIO),
		VMSTATE_UINT32(ELSR, ATSAM4E16EPIO),
		VMSTATE_UINT32(FELLSR, ATSAM4E16EPIO),
		VMSTATE_UINT32(REHLSR, ATSAM4E16EPIO),
		VMSTATE_UINT32(FRLHSR, ATSAM4E16EPIO),
		VMSTATE_UINT32(LOCKSR, ATSAM4E16EPIO),
		VMSTATE_UINT32(WPMR, ATSAM4E16EPIO),
		VMSTATE_UINT32(WPSR, ATSAM4E16EPIO),
		VMSTATE_UINT32(SCHMITT, ATSAM4E16EPIO),
		VMSTATE_UINT32(DELAYR, ATSAM4E16EPIO),
		VMSTATE_UINT32(PCMR, ATSAM4E16EPIO),
		VMSTATE_UINT32(PCIER, ATSAM4E16EPIO),
		VMSTATE_UINT32(PCIDR, ATSAM4E16EPIO),
		VMSTATE_UINT32(PCIMR, ATSAM4E16EPIO),
		VMSTATE_UINT32(PCISR, ATSAM4E16EPIO),
		VMSTATE_UINT32(PCRHR, ATSAM4E16EPIO),
		VMSTATE_UINT32(RPR, ATSAM4E16EPIO),
		VMSTATE_UINT32(RCR, ATSAM4E16EPIO),
		VMSTATE_UINT32(RNPR, ATSAM4E16EPIO),
		VMSTATE_UINT32(RNCR, ATSAM4E16EPIO),
		VMSTATE_UINT32(PTCR, ATSAM4E16EPIO),
		VMSTATE_UINT32(PTSR, ATSAM4E16EPIO),
		VMSTATE_UINT32(ABCDSR0, ATSAM4E16EPIO),
		VMSTATE_UINT32(ABCDSR1, ATSAM4E16EPIO),

		VMSTATE_END_OF_LIST()
	}
};

static Property atsam4e16e_pio_properties[] = {
	DEFINE_PROP_CHR("chardev", ATSAM4E16EPIO, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void atsam4e16e_pio_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsam4e16e_pio_vmstate;
	dc->realize = atsam4e16e_pio_realize;
	rc->phases.enter = atsam4e16e_pio_reset_enter;
	device_class_set_props(dc, atsam4e16e_pio_properties);

}

static const TypeInfo atsam4e16e_pio_info = {
	.name = TYPE_ATSAM4E16E_PIO,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAM4E16EPIO),
	.instance_init = atsam4e16e_pio_init,
	.class_init = atsam4e16e_pio_class_init,
};

static void atsam4e16e_pio_register_types(void) {
	type_register_static(&atsam4e16e_pio_info);
}

type_init(atsam4e16e_pio_register_types);

#define TYPE_ATSAM4E16E_EFC "atsam4e16e-efc"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAM4E16EEFC, ATSAM4E16E_EFC)

struct ATSAM4E16EEFC {
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
#define ATSAM4E16E_EFC_SIZE				0x10

REG32(EFC_FMR, 0x0)
	FIELD(EFC_FMR, FRDY, 0, 1)
	FIELD(EFC_FMR, FWS, 8, 4)
	FIELD(EFC_FMR, SCOD, 16, 1)
	FIELD(EFC_FMR, FAM, 24, 1)
	FIELD(EFC_FMR, CLOE, 26, 1)
REG32(EFC_FCR, 0x4)
	FIELD(EFC_FCR, FCMD, 0, 8)
	FIELD(EFC_FCR, FARG, 8, 16)
	FIELD(EFC_FCR, FKEY, 24, 8)
REG32(EFC_FSR, 0x8)
	FIELD(EFC_FSR, FRDY, 0, 1)
	FIELD(EFC_FSR, FCMDE, 1, 1)
	FIELD(EFC_FSR, FLOCKE, 2, 1)
	FIELD(EFC_FSR, FLERR, 3, 1)
REG32(EFC_FRR, 0xc)
	FIELD(EFC_FRR, FVALUE, 0, 32)


static void atsam4e16e_efc_register_reset(ATSAM4E16EEFC *t) {
	t->FMR = 0x4000000;
	t->FSR = 0x1;
	t->FRR = 0x0;

}

static uint64_t atsam4e16e_efc_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAM4E16EEFC *t = ATSAM4E16E_EFC(opaque);
	uint64_t ret;

	switch (offset) {
		case A_EFC_FMR:
			ret = t->FMR;
			break;
		case A_EFC_FSR:
			ret = t->FSR;
			break;
		case A_EFC_FRR:
			ret = t->FRR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM4E16E EFC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsam4e16e_efc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAM4E16EEFC *t = ATSAM4E16E_EFC(opaque);


	switch (offset) {
		case A_EFC_FMR ... A_EFC_FMR + 3:
			value = value << ((offset - A_EFC_FMR) << 3);
			offset = A_EFC_FMR;
			break;
		case A_EFC_FCR ... A_EFC_FCR + 3:
			value = value << ((offset - A_EFC_FCR) << 3);
			offset = A_EFC_FCR;
			break;

		default: break;
	}

	switch (offset) {
		case A_EFC_FMR:
			t->FMR = value;
			break;
		case A_EFC_FCR:
			t->FCR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM4E16E EFC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsam4e16e_efc_ops = {
	.read = atsam4e16e_efc_read,
	.write = atsam4e16e_efc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsam4e16e_efc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAM4E16EEFC *t = ATSAM4E16E_EFC(obj);
	memory_region_init_io(&(t->iomem), obj, &atsam4e16e_efc_ops, t, "atsam4e16e-efc", ATSAM4E16E_EFC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void atsam4e16e_efc_realize(DeviceState *dev, Error **errp) {
	return;

}

static void atsam4e16e_efc_reset_enter(Object *obj, ResetType type) {
	ATSAM4E16EEFC *t = ATSAM4E16E_EFC(obj);
	atsam4e16e_efc_register_reset(t);
}

static const VMStateDescription atsam4e16e_efc_vmstate = {
	.name = "atsam4e16e-efc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(FMR, ATSAM4E16EEFC),
		VMSTATE_UINT32(FCR, ATSAM4E16EEFC),
		VMSTATE_UINT32(FSR, ATSAM4E16EEFC),
		VMSTATE_UINT32(FRR, ATSAM4E16EEFC),

		VMSTATE_END_OF_LIST()
	}
};

static void atsam4e16e_efc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsam4e16e_efc_vmstate;
	dc->realize = atsam4e16e_efc_realize;
	rc->phases.enter = atsam4e16e_efc_reset_enter;

}

static const TypeInfo atsam4e16e_efc_info = {
	.name = TYPE_ATSAM4E16E_EFC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAM4E16EEFC),
	.instance_init = atsam4e16e_efc_init,
	.class_init = atsam4e16e_efc_class_init,
};

static void atsam4e16e_efc_register_types(void) {
	type_register_static(&atsam4e16e_efc_info);
}

type_init(atsam4e16e_efc_register_types);

#define TYPE_ATSAM4E16E_WDT "atsam4e16e-wdt"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAM4E16EWDT, ATSAM4E16E_WDT)

struct ATSAM4E16EWDT {
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
#define ATSAM4E16E_WDT_SIZE				0xc

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


static void atsam4e16e_wdt_register_reset(ATSAM4E16EWDT *t) {
	t->MR = 0x3fff2fff;
	t->SR = 0x0;

}

static uint64_t atsam4e16e_wdt_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAM4E16EWDT *t = ATSAM4E16E_WDT(opaque);
	uint64_t ret;

	switch (offset) {
		case A_WDT_MR:
			ret = t->MR;
			break;
		case A_WDT_SR:
			ret = t->SR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM4E16E WDT read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsam4e16e_wdt_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAM4E16EWDT *t = ATSAM4E16E_WDT(opaque);


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
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM4E16E WDT write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsam4e16e_wdt_ops = {
	.read = atsam4e16e_wdt_read,
	.write = atsam4e16e_wdt_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsam4e16e_wdt_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAM4E16EWDT *t = ATSAM4E16E_WDT(obj);
	memory_region_init_io(&(t->iomem), obj, &atsam4e16e_wdt_ops, t, "atsam4e16e-wdt", ATSAM4E16E_WDT_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void atsam4e16e_wdt_realize(DeviceState *dev, Error **errp) {
	return;

}

static void atsam4e16e_wdt_reset_enter(Object *obj, ResetType type) {
	ATSAM4E16EWDT *t = ATSAM4E16E_WDT(obj);
	atsam4e16e_wdt_register_reset(t);
}

static const VMStateDescription atsam4e16e_wdt_vmstate = {
	.name = "atsam4e16e-wdt",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR, ATSAM4E16EWDT),
		VMSTATE_UINT32(MR, ATSAM4E16EWDT),
		VMSTATE_UINT32(SR, ATSAM4E16EWDT),

		VMSTATE_END_OF_LIST()
	}
};

static void atsam4e16e_wdt_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsam4e16e_wdt_vmstate;
	dc->realize = atsam4e16e_wdt_realize;
	rc->phases.enter = atsam4e16e_wdt_reset_enter;

}

static const TypeInfo atsam4e16e_wdt_info = {
	.name = TYPE_ATSAM4E16E_WDT,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAM4E16EWDT),
	.instance_init = atsam4e16e_wdt_init,
	.class_init = atsam4e16e_wdt_class_init,
};

static void atsam4e16e_wdt_register_types(void) {
	type_register_static(&atsam4e16e_wdt_info);
}

type_init(atsam4e16e_wdt_register_types);

#define TYPE_ATSAM4E16E_SPI "atsam4e16e-spi"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAM4E16ESPI, ATSAM4E16E_SPI)

struct ATSAM4E16ESPI {
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
#define ATSAM4E16E_SPI_SIZE				0x128

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
	FIELD(SPI_SR, ENDRX, 4, 1)
	FIELD(SPI_SR, ENDTX, 5, 1)
	FIELD(SPI_SR, RXBUFF, 6, 1)
	FIELD(SPI_SR, TXBUFE, 7, 1)
	FIELD(SPI_SR, NSSR, 8, 1)
	FIELD(SPI_SR, TXEMPTY, 9, 1)
	FIELD(SPI_SR, UNDES, 10, 1)
	FIELD(SPI_SR, SPIENS, 16, 1)
REG32(SPI_IER, 0x14)
	FIELD(SPI_IER, RDRF, 0, 1)
	FIELD(SPI_IER, TDRE, 1, 1)
	FIELD(SPI_IER, MODF, 2, 1)
	FIELD(SPI_IER, OVRES, 3, 1)
	FIELD(SPI_IER, ENDRX, 4, 1)
	FIELD(SPI_IER, ENDTX, 5, 1)
	FIELD(SPI_IER, RXBUFF, 6, 1)
	FIELD(SPI_IER, TXBUFE, 7, 1)
	FIELD(SPI_IER, NSSR, 8, 1)
	FIELD(SPI_IER, TXEMPTY, 9, 1)
	FIELD(SPI_IER, UNDES, 10, 1)
REG32(SPI_IDR, 0x18)
	FIELD(SPI_IDR, RDRF, 0, 1)
	FIELD(SPI_IDR, TDRE, 1, 1)
	FIELD(SPI_IDR, MODF, 2, 1)
	FIELD(SPI_IDR, OVRES, 3, 1)
	FIELD(SPI_IDR, ENDRX, 4, 1)
	FIELD(SPI_IDR, ENDTX, 5, 1)
	FIELD(SPI_IDR, RXBUFF, 6, 1)
	FIELD(SPI_IDR, TXBUFE, 7, 1)
	FIELD(SPI_IDR, NSSR, 8, 1)
	FIELD(SPI_IDR, TXEMPTY, 9, 1)
	FIELD(SPI_IDR, UNDES, 10, 1)
REG32(SPI_IMR, 0x1c)
	FIELD(SPI_IMR, RDRF, 0, 1)
	FIELD(SPI_IMR, TDRE, 1, 1)
	FIELD(SPI_IMR, MODF, 2, 1)
	FIELD(SPI_IMR, OVRES, 3, 1)
	FIELD(SPI_IMR, ENDRX, 4, 1)
	FIELD(SPI_IMR, ENDTX, 5, 1)
	FIELD(SPI_IMR, RXBUFF, 6, 1)
	FIELD(SPI_IMR, TXBUFE, 7, 1)
	FIELD(SPI_IMR, NSSR, 8, 1)
	FIELD(SPI_IMR, TXEMPTY, 9, 1)
	FIELD(SPI_IMR, UNDES, 10, 1)
REG32(SPI_WPMR, 0xe4)
	FIELD(SPI_WPMR, WPEN, 0, 1)
	FIELD(SPI_WPMR, WPKEY, 8, 24)
REG32(SPI_WPSR, 0xe8)
	FIELD(SPI_WPSR, WPVS, 0, 1)
	FIELD(SPI_WPSR, WPVSRC, 8, 8)
REG32(SPI_RPR, 0x100)
	FIELD(SPI_RPR, RXPTR, 0, 32)
REG32(SPI_RCR, 0x104)
	FIELD(SPI_RCR, RXCTR, 0, 16)
REG32(SPI_TPR, 0x108)
	FIELD(SPI_TPR, TXPTR, 0, 32)
REG32(SPI_TCR, 0x10c)
	FIELD(SPI_TCR, TXCTR, 0, 16)
REG32(SPI_RNPR, 0x110)
	FIELD(SPI_RNPR, RXNPTR, 0, 32)
REG32(SPI_RNCR, 0x114)
	FIELD(SPI_RNCR, RXNCTR, 0, 16)
REG32(SPI_TNPR, 0x118)
	FIELD(SPI_TNPR, TXNPTR, 0, 32)
REG32(SPI_TNCR, 0x11c)
	FIELD(SPI_TNCR, TXNCTR, 0, 16)
REG32(SPI_PTCR, 0x120)
	FIELD(SPI_PTCR, RXTEN, 0, 1)
	FIELD(SPI_PTCR, RXTDIS, 1, 1)
	FIELD(SPI_PTCR, TXTEN, 8, 1)
	FIELD(SPI_PTCR, TXTDIS, 9, 1)
REG32(SPI_PTSR, 0x124)
	FIELD(SPI_PTSR, RXTEN, 0, 1)
	FIELD(SPI_PTSR, TXTEN, 8, 1)
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


static int atsam4e16e_spi_can_receive(void *opaque) {
	return 1;
}

static void atsam4e16e_spi_receive(void *opaque, const uint8_t *buf, int size) {
	ATSAM4E16ESPI *t = ATSAM4E16E_SPI(opaque);

	t->RDR = *buf;
	t->SR |= 0x1;


}

static gboolean atsam4e16e_spi_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	ATSAM4E16ESPI *t = ATSAM4E16E_SPI(opaque);
	int ret;

	t->watch_tag = 0;
	t->SR &= (~(0x2));



	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->TDR), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, atsam4e16e_spi_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->SR |= 0x2;



	return FALSE;
}

static void atsam4e16e_spi_register_reset(ATSAM4E16ESPI *t) {
	t->MR = 0x0;
	t->RDR = 0x0;
	t->SR = 0xf0;
	t->IMR = 0x0;
	t->WPMR = 0x0;
	t->WPSR = 0x0;
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

static uint64_t atsam4e16e_spi_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAM4E16ESPI *t = ATSAM4E16E_SPI(opaque);
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
		case A_SPI_RPR:
			ret = t->RPR;
			break;
		case A_SPI_RCR:
			ret = t->RCR;
			break;
		case A_SPI_TPR:
			ret = t->TPR;
			break;
		case A_SPI_TCR:
			ret = t->TCR;
			break;
		case A_SPI_RNPR:
			ret = t->RNPR;
			break;
		case A_SPI_RNCR:
			ret = t->RNCR;
			break;
		case A_SPI_TNPR:
			ret = t->TNPR;
			break;
		case A_SPI_TNCR:
			ret = t->TNCR;
			break;
		case A_SPI_PTSR:
			ret = t->PTSR;
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
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM4E16E SPI read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsam4e16e_spi_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAM4E16ESPI *t = ATSAM4E16E_SPI(opaque);


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
		case A_SPI_RPR ... A_SPI_RPR + 3:
			value = value << ((offset - A_SPI_RPR) << 3);
			offset = A_SPI_RPR;
			break;
		case A_SPI_RCR ... A_SPI_RCR + 3:
			value = value << ((offset - A_SPI_RCR) << 3);
			offset = A_SPI_RCR;
			break;
		case A_SPI_TPR ... A_SPI_TPR + 3:
			value = value << ((offset - A_SPI_TPR) << 3);
			offset = A_SPI_TPR;
			break;
		case A_SPI_TCR ... A_SPI_TCR + 3:
			value = value << ((offset - A_SPI_TCR) << 3);
			offset = A_SPI_TCR;
			break;
		case A_SPI_RNPR ... A_SPI_RNPR + 3:
			value = value << ((offset - A_SPI_RNPR) << 3);
			offset = A_SPI_RNPR;
			break;
		case A_SPI_RNCR ... A_SPI_RNCR + 3:
			value = value << ((offset - A_SPI_RNCR) << 3);
			offset = A_SPI_RNCR;
			break;
		case A_SPI_TNPR ... A_SPI_TNPR + 3:
			value = value << ((offset - A_SPI_TNPR) << 3);
			offset = A_SPI_TNPR;
			break;
		case A_SPI_TNCR ... A_SPI_TNCR + 3:
			value = value << ((offset - A_SPI_TNCR) << 3);
			offset = A_SPI_TNCR;
			break;
		case A_SPI_PTCR ... A_SPI_PTCR + 3:
			value = value << ((offset - A_SPI_PTCR) << 3);
			offset = A_SPI_PTCR;
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
			atsam4e16e_spi_transmit(NULL, G_IO_OUT, t);
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
		case A_SPI_RPR:
			t->RPR = value;
			break;
		case A_SPI_RCR:
			t->RCR = value;
			break;
		case A_SPI_TPR:
			t->TPR = value;
			break;
		case A_SPI_TCR:
			t->TCR = value;
			break;
		case A_SPI_RNPR:
			t->RNPR = value;
			break;
		case A_SPI_RNCR:
			t->RNCR = value;
			break;
		case A_SPI_TNPR:
			t->TNPR = value;
			break;
		case A_SPI_TNCR:
			t->TNCR = value;
			break;
		case A_SPI_PTCR:
			t->PTCR = value;
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
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM4E16E SPI write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsam4e16e_spi_ops = {
	.read = atsam4e16e_spi_read,
	.write = atsam4e16e_spi_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsam4e16e_spi_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAM4E16ESPI *t = ATSAM4E16E_SPI(obj);
	memory_region_init_io(&(t->iomem), obj, &atsam4e16e_spi_ops, t, "atsam4e16e-spi", ATSAM4E16E_SPI_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void atsam4e16e_spi_realize(DeviceState *dev, Error **errp) {
	ATSAM4E16ESPI *t = ATSAM4E16E_SPI(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), atsam4e16e_spi_can_receive, atsam4e16e_spi_receive,
		NULL, NULL, t, NULL, true);

}

static void atsam4e16e_spi_reset_enter(Object *obj, ResetType type) {
	ATSAM4E16ESPI *t = ATSAM4E16E_SPI(obj);
	atsam4e16e_spi_register_reset(t);
}

static const VMStateDescription atsam4e16e_spi_vmstate = {
	.name = "atsam4e16e-spi",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR, ATSAM4E16ESPI),
		VMSTATE_UINT32(MR, ATSAM4E16ESPI),
		VMSTATE_UINT32(RDR, ATSAM4E16ESPI),
		VMSTATE_UINT32(TDR, ATSAM4E16ESPI),
		VMSTATE_UINT32(SR, ATSAM4E16ESPI),
		VMSTATE_UINT32(IER, ATSAM4E16ESPI),
		VMSTATE_UINT32(IDR, ATSAM4E16ESPI),
		VMSTATE_UINT32(IMR, ATSAM4E16ESPI),
		VMSTATE_UINT32(WPMR, ATSAM4E16ESPI),
		VMSTATE_UINT32(WPSR, ATSAM4E16ESPI),
		VMSTATE_UINT32(RPR, ATSAM4E16ESPI),
		VMSTATE_UINT32(RCR, ATSAM4E16ESPI),
		VMSTATE_UINT32(TPR, ATSAM4E16ESPI),
		VMSTATE_UINT32(TCR, ATSAM4E16ESPI),
		VMSTATE_UINT32(RNPR, ATSAM4E16ESPI),
		VMSTATE_UINT32(RNCR, ATSAM4E16ESPI),
		VMSTATE_UINT32(TNPR, ATSAM4E16ESPI),
		VMSTATE_UINT32(TNCR, ATSAM4E16ESPI),
		VMSTATE_UINT32(PTCR, ATSAM4E16ESPI),
		VMSTATE_UINT32(PTSR, ATSAM4E16ESPI),
		VMSTATE_UINT32(CSR0, ATSAM4E16ESPI),
		VMSTATE_UINT32(CSR1, ATSAM4E16ESPI),
		VMSTATE_UINT32(CSR2, ATSAM4E16ESPI),
		VMSTATE_UINT32(CSR3, ATSAM4E16ESPI),

		VMSTATE_END_OF_LIST()
	}
};

static Property atsam4e16e_spi_properties[] = {
	DEFINE_PROP_CHR("chardev", ATSAM4E16ESPI, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void atsam4e16e_spi_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsam4e16e_spi_vmstate;
	dc->realize = atsam4e16e_spi_realize;
	rc->phases.enter = atsam4e16e_spi_reset_enter;
	device_class_set_props(dc, atsam4e16e_spi_properties);

}

static const TypeInfo atsam4e16e_spi_info = {
	.name = TYPE_ATSAM4E16E_SPI,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAM4E16ESPI),
	.instance_init = atsam4e16e_spi_init,
	.class_init = atsam4e16e_spi_class_init,
};

static void atsam4e16e_spi_register_types(void) {
	type_register_static(&atsam4e16e_spi_info);
}

type_init(atsam4e16e_spi_register_types);

#define TYPE_ATSAM4E16E_TWI "atsam4e16e-twi"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAM4E16ETWI, ATSAM4E16E_TWI)

struct ATSAM4E16ETWI {
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
	uint32_t WPMR;
	uint32_t WPSR;
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
#define ATSAM4E16E_TWI_SIZE				0x128

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
REG32(TWI_WPMR, 0xe4)
	FIELD(TWI_WPMR, WPEN, 0, 1)
	FIELD(TWI_WPMR, WPKEY, 8, 24)
REG32(TWI_WPSR, 0xe8)
	FIELD(TWI_WPSR, WPVS, 0, 1)
	FIELD(TWI_WPSR, WPVSRC, 8, 24)
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


static int atsam4e16e_twi_can_receive(void *opaque) {
	return 1;
}

static void atsam4e16e_twi_receive(void *opaque, const uint8_t *buf, int size) {
	ATSAM4E16ETWI *t = ATSAM4E16E_TWI(opaque);

	t->RHR = *buf;
	t->WPSR = *buf;


}

static gboolean atsam4e16e_twi_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	ATSAM4E16ETWI *t = ATSAM4E16E_TWI(opaque);
	int ret;

	t->watch_tag = 0;
	t->SR &= (~(0x4));
	t->SR &= (~(0x1));
	t->SR |= 0x100;



	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->THR), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, atsam4e16e_twi_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->SR &= (~(0x100));
	t->SR |= 0x4;
	t->SR |= 0x1;



	return FALSE;
}

static void atsam4e16e_twi_register_reset(ATSAM4E16ETWI *t) {
	t->MMR = 0x0;
	t->SMR = 0x0;
	t->IADR = 0x0;
	t->CWGR = 0x0;
	t->SR = 0xf009;
	t->IMR = 0x0;
	t->RHR = 0x0;
	t->WPMR = 0x0;
	t->WPSR = 0x0;
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
	t->SR &= (~(0x100));
	t->SR |= 0x4;
	t->SR |= 0x1;

}

static uint64_t atsam4e16e_twi_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAM4E16ETWI *t = ATSAM4E16E_TWI(opaque);
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
			qemu_chr_fe_accept_input(&(t->chr));
			break;
		case A_TWI_WPMR:
			ret = t->WPMR;
			break;
		case A_TWI_WPSR:
			ret = t->WPSR;
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
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM4E16E TWI read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsam4e16e_twi_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAM4E16ETWI *t = ATSAM4E16E_TWI(opaque);


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
		case A_TWI_WPMR ... A_TWI_WPMR + 3:
			value = value << ((offset - A_TWI_WPMR) << 3);
			offset = A_TWI_WPMR;
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
			atsam4e16e_twi_transmit(NULL, G_IO_OUT, t);
			break;
		case A_TWI_WPMR:
			t->WPMR = value;
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
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM4E16E TWI write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsam4e16e_twi_ops = {
	.read = atsam4e16e_twi_read,
	.write = atsam4e16e_twi_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsam4e16e_twi_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAM4E16ETWI *t = ATSAM4E16E_TWI(obj);
	memory_region_init_io(&(t->iomem), obj, &atsam4e16e_twi_ops, t, "atsam4e16e-twi", ATSAM4E16E_TWI_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void atsam4e16e_twi_realize(DeviceState *dev, Error **errp) {
	ATSAM4E16ETWI *t = ATSAM4E16E_TWI(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), atsam4e16e_twi_can_receive, atsam4e16e_twi_receive,
		NULL, NULL, t, NULL, true);

}

static void atsam4e16e_twi_reset_enter(Object *obj, ResetType type) {
	ATSAM4E16ETWI *t = ATSAM4E16E_TWI(obj);
	atsam4e16e_twi_register_reset(t);
}

static const VMStateDescription atsam4e16e_twi_vmstate = {
	.name = "atsam4e16e-twi",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR, ATSAM4E16ETWI),
		VMSTATE_UINT32(MMR, ATSAM4E16ETWI),
		VMSTATE_UINT32(SMR, ATSAM4E16ETWI),
		VMSTATE_UINT32(IADR, ATSAM4E16ETWI),
		VMSTATE_UINT32(CWGR, ATSAM4E16ETWI),
		VMSTATE_UINT32(SR, ATSAM4E16ETWI),
		VMSTATE_UINT32(IER, ATSAM4E16ETWI),
		VMSTATE_UINT32(IDR, ATSAM4E16ETWI),
		VMSTATE_UINT32(IMR, ATSAM4E16ETWI),
		VMSTATE_UINT32(RHR, ATSAM4E16ETWI),
		VMSTATE_UINT32(THR, ATSAM4E16ETWI),
		VMSTATE_UINT32(WPMR, ATSAM4E16ETWI),
		VMSTATE_UINT32(WPSR, ATSAM4E16ETWI),
		VMSTATE_UINT32(RPR, ATSAM4E16ETWI),
		VMSTATE_UINT32(RCR, ATSAM4E16ETWI),
		VMSTATE_UINT32(TPR, ATSAM4E16ETWI),
		VMSTATE_UINT32(TCR, ATSAM4E16ETWI),
		VMSTATE_UINT32(RNPR, ATSAM4E16ETWI),
		VMSTATE_UINT32(RNCR, ATSAM4E16ETWI),
		VMSTATE_UINT32(TNPR, ATSAM4E16ETWI),
		VMSTATE_UINT32(TNCR, ATSAM4E16ETWI),
		VMSTATE_UINT32(PTCR, ATSAM4E16ETWI),
		VMSTATE_UINT32(PTSR, ATSAM4E16ETWI),

		VMSTATE_END_OF_LIST()
	}
};

static Property atsam4e16e_twi_properties[] = {
	DEFINE_PROP_CHR("chardev", ATSAM4E16ETWI, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void atsam4e16e_twi_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsam4e16e_twi_vmstate;
	dc->realize = atsam4e16e_twi_realize;
	rc->phases.enter = atsam4e16e_twi_reset_enter;
	device_class_set_props(dc, atsam4e16e_twi_properties);

}

static const TypeInfo atsam4e16e_twi_info = {
	.name = TYPE_ATSAM4E16E_TWI,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAM4E16ETWI),
	.instance_init = atsam4e16e_twi_init,
	.class_init = atsam4e16e_twi_class_init,
};

static void atsam4e16e_twi_register_types(void) {
	type_register_static(&atsam4e16e_twi_info);
}

type_init(atsam4e16e_twi_register_types);

#define TYPE_ATSAM4E16E_DACC "atsam4e16e-dacc"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAM4E16EDACC, ATSAM4E16E_DACC)

struct ATSAM4E16EDACC {
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
#define ATSAM4E16E_DACC_SIZE				0x128

REG32(DACC_CR, 0x0)
	FIELD(DACC_CR, SWRST, 0, 1)
REG32(DACC_MR, 0x4)
	FIELD(DACC_MR, TRGEN, 0, 1)
	FIELD(DACC_MR, TRGSEL, 1, 3)
	FIELD(DACC_MR, WORD, 4, 1)
	FIELD(DACC_MR, ONE, 8, 1)
	FIELD(DACC_MR, USER_SEL, 16, 2)
	FIELD(DACC_MR, TAG, 20, 1)
	FIELD(DACC_MR, MAXS, 21, 1)
	FIELD(DACC_MR, CLKDIV, 22, 1)
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
	FIELD(DACC_WPSR, WPVS, 0, 1)
	FIELD(DACC_WPSR, WPVSRC, 8, 8)
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


static int atsam4e16e_dacc_can_receive(void *opaque) {
	return 1;
}

static void atsam4e16e_dacc_receive(void *opaque, const uint8_t *buf, int size) {
	ATSAM4E16EDACC *t = ATSAM4E16E_DACC(opaque);

	(void)t;


}

static gboolean atsam4e16e_dacc_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	ATSAM4E16EDACC *t = ATSAM4E16E_DACC(opaque);
	int ret;

	t->watch_tag = 0;
	t->ISR &= (~(0x2));



	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->CDR), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, atsam4e16e_dacc_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->ISR |= 0x2;



	return FALSE;
}

static void atsam4e16e_dacc_register_reset(ATSAM4E16EDACC *t) {
	t->MR = 0x0;
	t->CHSR = 0x0;
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

static uint64_t atsam4e16e_dacc_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAM4E16EDACC *t = ATSAM4E16E_DACC(opaque);
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
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM4E16E DACC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsam4e16e_dacc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAM4E16EDACC *t = ATSAM4E16E_DACC(opaque);


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
			atsam4e16e_dacc_transmit(NULL, G_IO_OUT, t);
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
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM4E16E DACC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsam4e16e_dacc_ops = {
	.read = atsam4e16e_dacc_read,
	.write = atsam4e16e_dacc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsam4e16e_dacc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAM4E16EDACC *t = ATSAM4E16E_DACC(obj);
	memory_region_init_io(&(t->iomem), obj, &atsam4e16e_dacc_ops, t, "atsam4e16e-dacc", ATSAM4E16E_DACC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void atsam4e16e_dacc_realize(DeviceState *dev, Error **errp) {
	ATSAM4E16EDACC *t = ATSAM4E16E_DACC(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), atsam4e16e_dacc_can_receive, atsam4e16e_dacc_receive,
		NULL, NULL, t, NULL, true);

}

static void atsam4e16e_dacc_reset_enter(Object *obj, ResetType type) {
	ATSAM4E16EDACC *t = ATSAM4E16E_DACC(obj);
	atsam4e16e_dacc_register_reset(t);
}

static const VMStateDescription atsam4e16e_dacc_vmstate = {
	.name = "atsam4e16e-dacc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR, ATSAM4E16EDACC),
		VMSTATE_UINT32(MR, ATSAM4E16EDACC),
		VMSTATE_UINT32(CHER, ATSAM4E16EDACC),
		VMSTATE_UINT32(CHDR, ATSAM4E16EDACC),
		VMSTATE_UINT32(CHSR, ATSAM4E16EDACC),
		VMSTATE_UINT32(CDR, ATSAM4E16EDACC),
		VMSTATE_UINT32(IER, ATSAM4E16EDACC),
		VMSTATE_UINT32(IDR, ATSAM4E16EDACC),
		VMSTATE_UINT32(IMR, ATSAM4E16EDACC),
		VMSTATE_UINT32(ISR, ATSAM4E16EDACC),
		VMSTATE_UINT32(ACR, ATSAM4E16EDACC),
		VMSTATE_UINT32(WPMR, ATSAM4E16EDACC),
		VMSTATE_UINT32(WPSR, ATSAM4E16EDACC),
		VMSTATE_UINT32(TPR, ATSAM4E16EDACC),
		VMSTATE_UINT32(TCR, ATSAM4E16EDACC),
		VMSTATE_UINT32(TNPR, ATSAM4E16EDACC),
		VMSTATE_UINT32(TNCR, ATSAM4E16EDACC),
		VMSTATE_UINT32(PTCR, ATSAM4E16EDACC),
		VMSTATE_UINT32(PTSR, ATSAM4E16EDACC),

		VMSTATE_END_OF_LIST()
	}
};

static Property atsam4e16e_dacc_properties[] = {
	DEFINE_PROP_CHR("chardev", ATSAM4E16EDACC, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void atsam4e16e_dacc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsam4e16e_dacc_vmstate;
	dc->realize = atsam4e16e_dacc_realize;
	rc->phases.enter = atsam4e16e_dacc_reset_enter;
	device_class_set_props(dc, atsam4e16e_dacc_properties);

}

static const TypeInfo atsam4e16e_dacc_info = {
	.name = TYPE_ATSAM4E16E_DACC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAM4E16EDACC),
	.instance_init = atsam4e16e_dacc_init,
	.class_init = atsam4e16e_dacc_class_init,
};

static void atsam4e16e_dacc_register_types(void) {
	type_register_static(&atsam4e16e_dacc_info);
}

type_init(atsam4e16e_dacc_register_types);

#define TYPE_ATSAM4E16E_MACHINE MACHINE_TYPE_NAME("atsam4e16e")
OBJECT_DECLARE_TYPE(ATSAM4E16EMachineState, ATSAM4E16EMachineClass, ATSAM4E16E_MACHINE)

struct ATSAM4E16EMachineClass {
	MachineClass parent;
};

struct ATSAM4E16EMachineState {
	MachineState parent;
	ARMv7MState armv7m;
};

static void atsam4e16e_periph_init(MachineState *machine) {
	ATSAM4E16EMachineState *sms = ATSAM4E16E_MACHINE(machine);
	ATSAM4E16EUSART *p0 = g_new(ATSAM4E16EUSART, 1);
	object_initialize_child(OBJECT(sms), "USART0", p0, TYPE_ATSAM4E16E_USART);
	qdev_prop_set_chr(DEVICE(p0), "chardev", qemu_chr_new("soc-usart0", "chardev:usart0", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p0), &error_fatal);
	p0->base = 0x400a0000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p0), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 14));
	sysbus_mmio_map(SYS_BUS_DEVICE(p0), 0, 0x400a0000);

	ATSAM4E16EUSART *p1 = g_new(ATSAM4E16EUSART, 1);
	object_initialize_child(OBJECT(sms), "USART1", p1, TYPE_ATSAM4E16E_USART);
	qdev_prop_set_chr(DEVICE(p1), "chardev", qemu_chr_new("soc-usart1", "chardev:usart1", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p1), &error_fatal);
	p1->base = 0x400a4000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p1), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 15));
	sysbus_mmio_map(SYS_BUS_DEVICE(p1), 0, 0x400a4000);

	ATSAM4E16EUART *p2 = g_new(ATSAM4E16EUART, 1);
	object_initialize_child(OBJECT(sms), "UART0", p2, TYPE_ATSAM4E16E_UART);
	qdev_prop_set_chr(DEVICE(p2), "chardev", qemu_chr_new("soc-uart0", "chardev:uart0", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p2), &error_fatal);
	p2->base = 0x400e0600;
	sysbus_connect_irq(SYS_BUS_DEVICE(p2), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 7));
	sysbus_mmio_map(SYS_BUS_DEVICE(p2), 0, 0x400e0600);

	ATSAM4E16EUART *p3 = g_new(ATSAM4E16EUART, 1);
	object_initialize_child(OBJECT(sms), "UART1", p3, TYPE_ATSAM4E16E_UART);
	qdev_prop_set_chr(DEVICE(p3), "chardev", qemu_chr_new("soc-uart1", "chardev:uart1", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p3), &error_fatal);
	p3->base = 0x40060600;
	sysbus_connect_irq(SYS_BUS_DEVICE(p3), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 45));
	sysbus_mmio_map(SYS_BUS_DEVICE(p3), 0, 0x40060600);

	ATSAM4E16EPIO *p4 = g_new(ATSAM4E16EPIO, 1);
	object_initialize_child(OBJECT(sms), "PIOA", p4, TYPE_ATSAM4E16E_PIO);
	qdev_prop_set_chr(DEVICE(p4), "chardev", qemu_chr_new("soc-pioa", "chardev:pioa", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p4), &error_fatal);
	p4->base = 0x400e0e00;
	sysbus_connect_irq(SYS_BUS_DEVICE(p4), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 9));
	sysbus_mmio_map(SYS_BUS_DEVICE(p4), 0, 0x400e0e00);

	ATSAM4E16EPIO *p5 = g_new(ATSAM4E16EPIO, 1);
	object_initialize_child(OBJECT(sms), "PIOB", p5, TYPE_ATSAM4E16E_PIO);
	qdev_prop_set_chr(DEVICE(p5), "chardev", qemu_chr_new("soc-piob", "chardev:piob", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p5), &error_fatal);
	p5->base = 0x400e1000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p5), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 10));
	sysbus_mmio_map(SYS_BUS_DEVICE(p5), 0, 0x400e1000);

	ATSAM4E16EPIO *p6 = g_new(ATSAM4E16EPIO, 1);
	object_initialize_child(OBJECT(sms), "PIOC", p6, TYPE_ATSAM4E16E_PIO);
	qdev_prop_set_chr(DEVICE(p6), "chardev", qemu_chr_new("soc-pioc", "chardev:pioc", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p6), &error_fatal);
	p6->base = 0x400e1200;
	sysbus_connect_irq(SYS_BUS_DEVICE(p6), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 11));
	sysbus_mmio_map(SYS_BUS_DEVICE(p6), 0, 0x400e1200);

	ATSAM4E16EPIO *p7 = g_new(ATSAM4E16EPIO, 1);
	object_initialize_child(OBJECT(sms), "PIOD", p7, TYPE_ATSAM4E16E_PIO);
	qdev_prop_set_chr(DEVICE(p7), "chardev", qemu_chr_new("soc-piod", "chardev:piod", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p7), &error_fatal);
	p7->base = 0x400e1400;
	sysbus_connect_irq(SYS_BUS_DEVICE(p7), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 12));
	sysbus_mmio_map(SYS_BUS_DEVICE(p7), 0, 0x400e1400);

	ATSAM4E16EPIO *p8 = g_new(ATSAM4E16EPIO, 1);
	object_initialize_child(OBJECT(sms), "PIOE", p8, TYPE_ATSAM4E16E_PIO);
	qdev_prop_set_chr(DEVICE(p8), "chardev", qemu_chr_new("soc-pioe", "chardev:pioe", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p8), &error_fatal);
	p8->base = 0x400e1600;
	sysbus_connect_irq(SYS_BUS_DEVICE(p8), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 13));
	sysbus_mmio_map(SYS_BUS_DEVICE(p8), 0, 0x400e1600);

	ATSAM4E16EPMC *p9 = g_new(ATSAM4E16EPMC, 1);
	object_initialize_child(OBJECT(sms), "PMC", p9, TYPE_ATSAM4E16E_PMC);
	sysbus_realize(SYS_BUS_DEVICE(p9), &error_fatal);
	p9->base = 0x400e0400;
	sysbus_connect_irq(SYS_BUS_DEVICE(p9), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 5));
	sysbus_mmio_map(SYS_BUS_DEVICE(p9), 0, 0x400e0400);

	ATSAM4E16ESUPC *p10 = g_new(ATSAM4E16ESUPC, 1);
	object_initialize_child(OBJECT(sms), "SUPC", p10, TYPE_ATSAM4E16E_SUPC);
	sysbus_realize(SYS_BUS_DEVICE(p10), &error_fatal);
	p10->base = 0x400e1810;
	sysbus_mmio_map(SYS_BUS_DEVICE(p10), 0, 0x400e1810);

	ATSAM4E16EEFC *p11 = g_new(ATSAM4E16EEFC, 1);
	object_initialize_child(OBJECT(sms), "EFC", p11, TYPE_ATSAM4E16E_EFC);
	sysbus_realize(SYS_BUS_DEVICE(p11), &error_fatal);
	p11->base = 0x400e0a00;
	sysbus_connect_irq(SYS_BUS_DEVICE(p11), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 6));
	sysbus_mmio_map(SYS_BUS_DEVICE(p11), 0, 0x400e0a00);

	ATSAM4E16EWDT *p12 = g_new(ATSAM4E16EWDT, 1);
	object_initialize_child(OBJECT(sms), "WDT", p12, TYPE_ATSAM4E16E_WDT);
	sysbus_realize(SYS_BUS_DEVICE(p12), &error_fatal);
	p12->base = 0x400e1850;
	sysbus_mmio_map(SYS_BUS_DEVICE(p12), 0, 0x400e1850);

	ATSAM4E16ESPI *p13 = g_new(ATSAM4E16ESPI, 1);
	object_initialize_child(OBJECT(sms), "SPI", p13, TYPE_ATSAM4E16E_SPI);
	qdev_prop_set_chr(DEVICE(p13), "chardev", qemu_chr_new("soc-spi", "chardev:spi", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p13), &error_fatal);
	p13->base = 0x40088000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p13), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 19));
	sysbus_mmio_map(SYS_BUS_DEVICE(p13), 0, 0x40088000);

	ATSAM4E16EDACC *p14 = g_new(ATSAM4E16EDACC, 1);
	object_initialize_child(OBJECT(sms), "DACC", p14, TYPE_ATSAM4E16E_DACC);
	qdev_prop_set_chr(DEVICE(p14), "chardev", qemu_chr_new("soc-dacc", "chardev:dacc", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p14), &error_fatal);
	p14->base = 0x400b8000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p14), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 32));
	sysbus_mmio_map(SYS_BUS_DEVICE(p14), 0, 0x400b8000);

	ATSAM4E16ETWI *p15 = g_new(ATSAM4E16ETWI, 1);
	object_initialize_child(OBJECT(sms), "TWI0", p15, TYPE_ATSAM4E16E_TWI);
	qdev_prop_set_chr(DEVICE(p15), "chardev", qemu_chr_new("soc-twi0", "chardev:twi0", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p15), &error_fatal);
	p15->base = 0x400a8000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p15), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 17));
	sysbus_mmio_map(SYS_BUS_DEVICE(p15), 0, 0x400a8000);


}

static void atsam4e16e_common_init(MachineState *machine) {
	ATSAM4E16EMachineState *sms = ATSAM4E16E_MACHINE(machine);
	MemoryRegion *sysmem = get_system_memory();
	MemoryRegion *mem;
	Clock *cpuclk = clock_new(OBJECT(sms), "SYSCLK");
	clock_set_hz(cpuclk, 120000000);
	Clock *refclk = clock_new(OBJECT(sms), "REFCLK");
	clock_set_hz(refclk, 120000000);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_rom(mem, NULL, "main-flash", 0x100000, &error_fatal);
	memory_region_add_subregion(sysmem, 0x400000, mem);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_ram(mem, NULL, "sram", 0x40000, &error_fatal);
	memory_region_add_subregion(sysmem, 0x20000000, mem);


	object_initialize_child(OBJECT(sms), "armv7m", &(sms->armv7m), TYPE_ARMV7M);
	qdev_prop_set_uint32(DEVICE(&(sms->armv7m)),"num-irq", 256);
	qdev_prop_set_string(DEVICE(&(sms->armv7m)), "cpu-type", machine->cpu_type);
	qdev_prop_set_bit(DEVICE(&(sms->armv7m)), "enable-bitband", true);
	qdev_connect_clock_in(DEVICE(&(sms->armv7m)), "cpuclk", cpuclk);
	qdev_connect_clock_in(DEVICE(&(sms->armv7m)), "refclk", refclk);
	qdev_prop_set_uint32(DEVICE(&(sms->armv7m)), "init-nsvtor", 0x400000);
	object_property_set_link(OBJECT(&sms->armv7m), "memory", OBJECT(sysmem), &error_abort);
	sysbus_realize(SYS_BUS_DEVICE(&sms->armv7m), &error_fatal);

	atsam4e16e_periph_init(machine);

	armv7m_load_kernel(ARM_CPU(first_cpu), machine->kernel_filename, 0x400000, 0x100000);
}

static void atsam4e16e_class_init(ObjectClass *oc, void *data) {
	MachineClass *mc = MACHINE_CLASS(oc);
	mc->desc = "atsam4e16e";
	mc->init = atsam4e16e_common_init;
	mc->max_cpus = 1;
	mc->default_cpus = 1;
	mc->default_cpu_type = ARM_CPU_TYPE_NAME("cortex-m4");
}

static const TypeInfo atsam4e16e_info = {
	.name = TYPE_ATSAM4E16E_MACHINE,
	.parent = TYPE_MACHINE,
	.instance_size = sizeof(ATSAM4E16EMachineState),
	.class_size = sizeof(ATSAM4E16EMachineClass),
	.class_init = atsam4e16e_class_init
};

static void atsam4e16e_machine_init(void) {
	type_register_static(&atsam4e16e_info);
}

type_init(atsam4e16e_machine_init);
