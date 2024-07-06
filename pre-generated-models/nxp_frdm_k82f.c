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


#define TYPE_FRDMK82F_LPUART "frdmk82f-lpuart"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMK82FLPUART, FRDMK82F_LPUART)

struct FRDMK82FLPUART {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t BAUD;
	uint32_t STAT;
	uint32_t CTRL;
	uint32_t DATA;
	uint32_t MATCH;
	uint32_t MODIR;
	uint32_t FIFO;
	uint32_t WATER;
	/* chardev backend */
	CharBackend chr;
	guint watch_tag;

	/* base */
	uint32_t base;

};
#define FRDMK82F_LPUART_SIZE				0x20

REG32(LPUART_BAUD, 0x0)
	FIELD(LPUART_BAUD, SBR, 0, 13)
	FIELD(LPUART_BAUD, SBNS, 13, 1)
	FIELD(LPUART_BAUD, RXEDGIE, 14, 1)
	FIELD(LPUART_BAUD, LBKDIE, 15, 1)
	FIELD(LPUART_BAUD, RESYNCDIS, 16, 1)
	FIELD(LPUART_BAUD, BOTHEDGE, 17, 1)
	FIELD(LPUART_BAUD, MATCFG, 18, 2)
	FIELD(LPUART_BAUD, RDMAE, 21, 1)
	FIELD(LPUART_BAUD, TDMAE, 23, 1)
	FIELD(LPUART_BAUD, OSR, 24, 5)
	FIELD(LPUART_BAUD, M10, 29, 1)
	FIELD(LPUART_BAUD, MAEN2, 30, 1)
	FIELD(LPUART_BAUD, MAEN1, 31, 1)
REG32(LPUART_STAT, 0x4)
	FIELD(LPUART_STAT, MA2F, 14, 1)
	FIELD(LPUART_STAT, MA1F, 15, 1)
	FIELD(LPUART_STAT, PF, 16, 1)
	FIELD(LPUART_STAT, FE, 17, 1)
	FIELD(LPUART_STAT, NF, 18, 1)
	FIELD(LPUART_STAT, OR, 19, 1)
	FIELD(LPUART_STAT, IDLE, 20, 1)
	FIELD(LPUART_STAT, RDRF, 21, 1)
	FIELD(LPUART_STAT, TC, 22, 1)
	FIELD(LPUART_STAT, TDRE, 23, 1)
	FIELD(LPUART_STAT, RAF, 24, 1)
	FIELD(LPUART_STAT, LBKDE, 25, 1)
	FIELD(LPUART_STAT, BRK13, 26, 1)
	FIELD(LPUART_STAT, RWUID, 27, 1)
	FIELD(LPUART_STAT, RXINV, 28, 1)
	FIELD(LPUART_STAT, MSBF, 29, 1)
	FIELD(LPUART_STAT, RXEDGIF, 30, 1)
	FIELD(LPUART_STAT, LBKDIF, 31, 1)
REG32(LPUART_CTRL, 0x8)
	FIELD(LPUART_CTRL, PT, 0, 1)
	FIELD(LPUART_CTRL, PE, 1, 1)
	FIELD(LPUART_CTRL, ILT, 2, 1)
	FIELD(LPUART_CTRL, WAKE, 3, 1)
	FIELD(LPUART_CTRL, M, 4, 1)
	FIELD(LPUART_CTRL, RSRC, 5, 1)
	FIELD(LPUART_CTRL, DOZEEN, 6, 1)
	FIELD(LPUART_CTRL, LOOPS, 7, 1)
	FIELD(LPUART_CTRL, IDLECFG, 8, 3)
	FIELD(LPUART_CTRL, MA2IE, 14, 1)
	FIELD(LPUART_CTRL, MA1IE, 15, 1)
	FIELD(LPUART_CTRL, SBK, 16, 1)
	FIELD(LPUART_CTRL, RWU, 17, 1)
	FIELD(LPUART_CTRL, RE, 18, 1)
	FIELD(LPUART_CTRL, TE, 19, 1)
	FIELD(LPUART_CTRL, ILIE, 20, 1)
	FIELD(LPUART_CTRL, RIE, 21, 1)
	FIELD(LPUART_CTRL, TCIE, 22, 1)
	FIELD(LPUART_CTRL, TIE, 23, 1)
	FIELD(LPUART_CTRL, PEIE, 24, 1)
	FIELD(LPUART_CTRL, FEIE, 25, 1)
	FIELD(LPUART_CTRL, NEIE, 26, 1)
	FIELD(LPUART_CTRL, ORIE, 27, 1)
	FIELD(LPUART_CTRL, TXINV, 28, 1)
	FIELD(LPUART_CTRL, TXDIR, 29, 1)
	FIELD(LPUART_CTRL, R9T8, 30, 1)
	FIELD(LPUART_CTRL, R8T9, 31, 1)
REG32(LPUART_DATA, 0xc)
	FIELD(LPUART_DATA, R0T0, 0, 1)
	FIELD(LPUART_DATA, R1T1, 1, 1)
	FIELD(LPUART_DATA, R2T2, 2, 1)
	FIELD(LPUART_DATA, R3T3, 3, 1)
	FIELD(LPUART_DATA, R4T4, 4, 1)
	FIELD(LPUART_DATA, R5T5, 5, 1)
	FIELD(LPUART_DATA, R6T6, 6, 1)
	FIELD(LPUART_DATA, R7T7, 7, 1)
	FIELD(LPUART_DATA, R8T8, 8, 1)
	FIELD(LPUART_DATA, R9T9, 9, 1)
	FIELD(LPUART_DATA, IDLINE, 11, 1)
	FIELD(LPUART_DATA, RXEMPT, 12, 1)
	FIELD(LPUART_DATA, FRETSC, 13, 1)
	FIELD(LPUART_DATA, PARITYE, 14, 1)
	FIELD(LPUART_DATA, NOISY, 15, 1)
REG32(LPUART_MATCH, 0x10)
	FIELD(LPUART_MATCH, MA1, 0, 10)
	FIELD(LPUART_MATCH, MA2, 16, 10)
REG32(LPUART_MODIR, 0x14)
	FIELD(LPUART_MODIR, TXCTSE, 0, 1)
	FIELD(LPUART_MODIR, TXRTSE, 1, 1)
	FIELD(LPUART_MODIR, TXRTSPOL, 2, 1)
	FIELD(LPUART_MODIR, RXRTSE, 3, 1)
	FIELD(LPUART_MODIR, TXCTSC, 4, 1)
	FIELD(LPUART_MODIR, TXCTSSRC, 5, 1)
	FIELD(LPUART_MODIR, RTSWATER, 8, 8)
	FIELD(LPUART_MODIR, TNP, 16, 2)
	FIELD(LPUART_MODIR, IREN, 18, 1)
REG32(LPUART_FIFO, 0x18)
	FIELD(LPUART_FIFO, RXFIFOSIZE, 0, 3)
	FIELD(LPUART_FIFO, RXFE, 3, 1)
	FIELD(LPUART_FIFO, TXFIFOSIZE, 4, 3)
	FIELD(LPUART_FIFO, TXFE, 7, 1)
	FIELD(LPUART_FIFO, RXUFE, 8, 1)
	FIELD(LPUART_FIFO, TXOFE, 9, 1)
	FIELD(LPUART_FIFO, RXIDEN, 10, 3)
	FIELD(LPUART_FIFO, RXFLUSH, 14, 1)
	FIELD(LPUART_FIFO, TXFLUSH, 15, 1)
	FIELD(LPUART_FIFO, RXUF, 16, 1)
	FIELD(LPUART_FIFO, TXOF, 17, 1)
	FIELD(LPUART_FIFO, RXEMPT, 22, 1)
	FIELD(LPUART_FIFO, TXEMPT, 23, 1)
REG32(LPUART_WATER, 0x1c)
	FIELD(LPUART_WATER, TXWATER, 0, 8)
	FIELD(LPUART_WATER, TXCOUNT, 8, 8)
	FIELD(LPUART_WATER, RXWATER, 16, 8)
	FIELD(LPUART_WATER, RXCOUNT, 24, 8)


__attribute__((unused))
static void frdmk82f_lpuart_update(FRDMK82FLPUART *t) {
	int conditions = ((t->STAT & 0x80000) || ((t->CTRL & 0x800000) && (t->STAT & 0x800000)) || ((t->CTRL & 0x100000) && (t->STAT & 0x100000)) || ((t->CTRL & 0x400000) && (t->STAT & 0x400000)) || ((t->CTRL & 0x200000) && (t->STAT & 0x200000)));
	qemu_set_irq(t->irq[0], conditions);

}

static int frdmk82f_lpuart_can_receive(void *opaque) {
	return 1;
}

static void frdmk82f_lpuart_receive(void *opaque, const uint8_t *buf, int size) {
	FRDMK82FLPUART *t = FRDMK82F_LPUART(opaque);

	t->DATA = *buf;
	t->STAT |= 0x200000;

	frdmk82f_lpuart_update(t);
}

static gboolean frdmk82f_lpuart_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	FRDMK82FLPUART *t = FRDMK82F_LPUART(opaque);
	int ret;

	t->watch_tag = 0;
	t->STAT &= (~(0x400000));
	t->STAT &= (~(0x800000));

	frdmk82f_lpuart_update(t);

	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->DATA), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, frdmk82f_lpuart_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->STAT |= 0x800000;
	t->STAT |= 0x400000;

	frdmk82f_lpuart_update(t);

	return FALSE;
}

static void frdmk82f_lpuart_register_reset(FRDMK82FLPUART *t) {
	t->BAUD = 0xf000004;
	t->STAT = 0xc00000;
	t->CTRL = 0x0;
	t->DATA = 0x1000;
	t->MATCH = 0x0;
	t->MODIR = 0x0;
	t->FIFO = 0x22;
	t->WATER = 0x0;
	t->STAT |= 0x800000;
	t->STAT |= 0x400000;

}

static uint64_t frdmk82f_lpuart_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMK82FLPUART *t = FRDMK82F_LPUART(opaque);
	uint64_t ret;

	switch (offset) {
		case A_LPUART_BAUD:
			ret = t->BAUD;
			break;
		case A_LPUART_STAT:
			ret = t->STAT;
			break;
		case A_LPUART_CTRL:
			ret = t->CTRL;
			break;
		case A_LPUART_DATA:
			ret = t->DATA;
			t->STAT &= (~(0x200000));
			qemu_chr_fe_accept_input(&(t->chr));
			frdmk82f_lpuart_update(t);
			break;
		case A_LPUART_MATCH:
			ret = t->MATCH;
			break;
		case A_LPUART_MODIR:
			ret = t->MODIR;
			break;
		case A_LPUART_FIFO:
			ret = t->FIFO;
			break;
		case A_LPUART_WATER:
			ret = t->WATER;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK82F LPUART read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmk82f_lpuart_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMK82FLPUART *t = FRDMK82F_LPUART(opaque);


	switch (offset) {
		case A_LPUART_BAUD ... A_LPUART_BAUD + 3:
			value = value << ((offset - A_LPUART_BAUD) << 3);
			offset = A_LPUART_BAUD;
			break;
		case A_LPUART_STAT ... A_LPUART_STAT + 3:
			value = value << ((offset - A_LPUART_STAT) << 3);
			offset = A_LPUART_STAT;
			break;
		case A_LPUART_CTRL ... A_LPUART_CTRL + 3:
			value = value << ((offset - A_LPUART_CTRL) << 3);
			offset = A_LPUART_CTRL;
			break;
		case A_LPUART_DATA ... A_LPUART_DATA + 3:
			value = value << ((offset - A_LPUART_DATA) << 3);
			offset = A_LPUART_DATA;
			break;
		case A_LPUART_MATCH ... A_LPUART_MATCH + 3:
			value = value << ((offset - A_LPUART_MATCH) << 3);
			offset = A_LPUART_MATCH;
			break;
		case A_LPUART_MODIR ... A_LPUART_MODIR + 3:
			value = value << ((offset - A_LPUART_MODIR) << 3);
			offset = A_LPUART_MODIR;
			break;
		case A_LPUART_FIFO ... A_LPUART_FIFO + 3:
			value = value << ((offset - A_LPUART_FIFO) << 3);
			offset = A_LPUART_FIFO;
			break;
		case A_LPUART_WATER ... A_LPUART_WATER + 3:
			value = value << ((offset - A_LPUART_WATER) << 3);
			offset = A_LPUART_WATER;
			break;

		default: break;
	}

	switch (offset) {
		case A_LPUART_BAUD:
			t->BAUD = value;
			break;
		case A_LPUART_STAT:
			t->STAT &= value;
			t->STAT |= 0x800000;
			t->STAT |= 0x400000;
			frdmk82f_lpuart_update(t);
			break;
		case A_LPUART_CTRL:
			t->CTRL = value;
			frdmk82f_lpuart_update(t);
			break;
		case A_LPUART_DATA:
			t->DATA = value;
			frdmk82f_lpuart_transmit(NULL, G_IO_OUT, t);
			break;
		case A_LPUART_MATCH:
			t->MATCH = value;
			break;
		case A_LPUART_MODIR:
			t->MODIR = value;
			break;
		case A_LPUART_FIFO:
			t->FIFO = value;
			break;
		case A_LPUART_WATER:
			t->WATER = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK82F LPUART write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmk82f_lpuart_ops = {
	.read = frdmk82f_lpuart_read,
	.write = frdmk82f_lpuart_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmk82f_lpuart_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMK82FLPUART *t = FRDMK82F_LPUART(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmk82f_lpuart_ops, t, "frdmk82f-lpuart", FRDMK82F_LPUART_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void frdmk82f_lpuart_realize(DeviceState *dev, Error **errp) {
	FRDMK82FLPUART *t = FRDMK82F_LPUART(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), frdmk82f_lpuart_can_receive, frdmk82f_lpuart_receive,
		NULL, NULL, t, NULL, true);

}

static void frdmk82f_lpuart_reset_enter(Object *obj, ResetType type) {
	FRDMK82FLPUART *t = FRDMK82F_LPUART(obj);
	frdmk82f_lpuart_register_reset(t);
}

static const VMStateDescription frdmk82f_lpuart_vmstate = {
	.name = "frdmk82f-lpuart",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(BAUD, FRDMK82FLPUART),
		VMSTATE_UINT32(STAT, FRDMK82FLPUART),
		VMSTATE_UINT32(CTRL, FRDMK82FLPUART),
		VMSTATE_UINT32(DATA, FRDMK82FLPUART),
		VMSTATE_UINT32(MATCH, FRDMK82FLPUART),
		VMSTATE_UINT32(MODIR, FRDMK82FLPUART),
		VMSTATE_UINT32(FIFO, FRDMK82FLPUART),
		VMSTATE_UINT32(WATER, FRDMK82FLPUART),

		VMSTATE_END_OF_LIST()
	}
};

static Property frdmk82f_lpuart_properties[] = {
	DEFINE_PROP_CHR("chardev", FRDMK82FLPUART, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void frdmk82f_lpuart_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmk82f_lpuart_vmstate;
	dc->realize = frdmk82f_lpuart_realize;
	rc->phases.enter = frdmk82f_lpuart_reset_enter;
	device_class_set_props(dc, frdmk82f_lpuart_properties);

}

static const TypeInfo frdmk82f_lpuart_info = {
	.name = TYPE_FRDMK82F_LPUART,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMK82FLPUART),
	.instance_init = frdmk82f_lpuart_init,
	.class_init = frdmk82f_lpuart_class_init,
};

static void frdmk82f_lpuart_register_types(void) {
	type_register_static(&frdmk82f_lpuart_info);
}

type_init(frdmk82f_lpuart_register_types);

#define TYPE_FRDMK82F_SPI "frdmk82f-spi"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMK82FSPI, FRDMK82F_SPI)

struct FRDMK82FSPI {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t MCR;
	uint32_t TCR;
	uint32_t CTAR_SLAVE;
	uint32_t SR;
	uint32_t RSER;
	uint32_t PUSHR;
	uint32_t PUSHR_SLAVE;
	uint32_t POPR;
	uint32_t CTAR0;
	uint32_t CTAR1;
	uint32_t TXFR0;
	uint32_t TXFR1;
	uint32_t TXFR2;
	uint32_t TXFR3;
	uint32_t RXFR0;
	uint32_t RXFR1;
	uint32_t RXFR2;
	uint32_t RXFR3;
	/* chardev backend */
	CharBackend chr;
	guint watch_tag;

	/* base */
	uint32_t base;

};
#define FRDMK82F_SPI_SIZE				0x8c

REG32(SPI_MCR, 0x0)
	FIELD(SPI_MCR, HALT, 0, 1)
	FIELD(SPI_MCR, SMPL_PT, 8, 2)
	FIELD(SPI_MCR, CLR_RXF, 10, 1)
	FIELD(SPI_MCR, CLR_TXF, 11, 1)
	FIELD(SPI_MCR, DIS_RXF, 12, 1)
	FIELD(SPI_MCR, DIS_TXF, 13, 1)
	FIELD(SPI_MCR, MDIS, 14, 1)
	FIELD(SPI_MCR, DOZE, 15, 1)
	FIELD(SPI_MCR, PCSIS0, 16, 1)
	FIELD(SPI_MCR, PCSIS1, 17, 1)
	FIELD(SPI_MCR, PCSIS2, 18, 1)
	FIELD(SPI_MCR, PCSIS3, 19, 1)
	FIELD(SPI_MCR, PCSIS4, 20, 1)
	FIELD(SPI_MCR, PCSIS5, 21, 1)
	FIELD(SPI_MCR, ROOE, 24, 1)
	FIELD(SPI_MCR, PCSSE, 25, 1)
	FIELD(SPI_MCR, MTFE, 26, 1)
	FIELD(SPI_MCR, FRZ, 27, 1)
	FIELD(SPI_MCR, DCONF, 28, 2)
	FIELD(SPI_MCR, CONT_SCKE, 30, 1)
	FIELD(SPI_MCR, MSTR, 31, 1)
REG32(SPI_TCR, 0x8)
	FIELD(SPI_TCR, SPI_TCNT, 16, 16)
REG32(SPI_CTAR_SLAVE, 0xc)
	FIELD(SPI_CTAR_SLAVE, CPHA, 25, 1)
	FIELD(SPI_CTAR_SLAVE, CPOL, 26, 1)
	FIELD(SPI_CTAR_SLAVE, FMSZ, 27, 4)
REG32(SPI_SR, 0x2c)
	FIELD(SPI_SR, POPNXTPTR, 0, 4)
	FIELD(SPI_SR, RXCTR, 4, 4)
	FIELD(SPI_SR, TXNXTPTR, 8, 4)
	FIELD(SPI_SR, TXCTR, 12, 4)
	FIELD(SPI_SR, RFDF, 17, 1)
	FIELD(SPI_SR, RFOF, 19, 1)
	FIELD(SPI_SR, TFFF, 25, 1)
	FIELD(SPI_SR, TFUF, 27, 1)
	FIELD(SPI_SR, EOQF, 28, 1)
	FIELD(SPI_SR, TXRXS, 30, 1)
	FIELD(SPI_SR, TCF, 31, 1)
REG32(SPI_RSER, 0x30)
	FIELD(SPI_RSER, RFDF_DIRS, 16, 1)
	FIELD(SPI_RSER, RFDF_RE, 17, 1)
	FIELD(SPI_RSER, RFOF_RE, 19, 1)
	FIELD(SPI_RSER, TFFF_DIRS, 24, 1)
	FIELD(SPI_RSER, TFFF_RE, 25, 1)
	FIELD(SPI_RSER, TFUF_RE, 27, 1)
	FIELD(SPI_RSER, EOQF_RE, 28, 1)
	FIELD(SPI_RSER, TCF_RE, 31, 1)
REG32(SPI_PUSHR, 0x34)
	FIELD(SPI_PUSHR, TXDATA, 0, 16)
	FIELD(SPI_PUSHR, PCS0, 16, 1)
	FIELD(SPI_PUSHR, PCS1, 17, 1)
	FIELD(SPI_PUSHR, PCS2, 18, 1)
	FIELD(SPI_PUSHR, PCS3, 19, 1)
	FIELD(SPI_PUSHR, PCS4, 20, 1)
	FIELD(SPI_PUSHR, PCS5, 21, 1)
	FIELD(SPI_PUSHR, CTCNT, 26, 1)
	FIELD(SPI_PUSHR, EOQ, 27, 1)
	FIELD(SPI_PUSHR, CTAS, 28, 3)
	FIELD(SPI_PUSHR, CONT, 31, 1)
REG32(SPI_PUSHR_SLAVE, 0x34)
	FIELD(SPI_PUSHR_SLAVE, TXDATA, 0, 16)
REG32(SPI_POPR, 0x38)
	FIELD(SPI_POPR, RXDATA, 0, 32)
REG32(SPI_CTAR0, 0xc)
	FIELD(SPI_CTAR0, BR, 0, 4)
	FIELD(SPI_CTAR0, DT, 4, 4)
	FIELD(SPI_CTAR0, ASC, 8, 4)
	FIELD(SPI_CTAR0, CSSCK, 12, 4)
	FIELD(SPI_CTAR0, PBR, 16, 2)
	FIELD(SPI_CTAR0, PDT, 18, 2)
	FIELD(SPI_CTAR0, PASC, 20, 2)
	FIELD(SPI_CTAR0, PCSSCK, 22, 2)
	FIELD(SPI_CTAR0, LSBFE, 24, 1)
	FIELD(SPI_CTAR0, CPHA, 25, 1)
	FIELD(SPI_CTAR0, CPOL, 26, 1)
	FIELD(SPI_CTAR0, FMSZ, 27, 4)
	FIELD(SPI_CTAR0, DBR, 31, 1)
REG32(SPI_CTAR1, 0x10)
	FIELD(SPI_CTAR1, BR, 0, 4)
	FIELD(SPI_CTAR1, DT, 4, 4)
	FIELD(SPI_CTAR1, ASC, 8, 4)
	FIELD(SPI_CTAR1, CSSCK, 12, 4)
	FIELD(SPI_CTAR1, PBR, 16, 2)
	FIELD(SPI_CTAR1, PDT, 18, 2)
	FIELD(SPI_CTAR1, PASC, 20, 2)
	FIELD(SPI_CTAR1, PCSSCK, 22, 2)
	FIELD(SPI_CTAR1, LSBFE, 24, 1)
	FIELD(SPI_CTAR1, CPHA, 25, 1)
	FIELD(SPI_CTAR1, CPOL, 26, 1)
	FIELD(SPI_CTAR1, FMSZ, 27, 4)
	FIELD(SPI_CTAR1, DBR, 31, 1)
REG32(SPI_TXFR0, 0x3c)
	FIELD(SPI_TXFR0, TXDATA, 0, 16)
	FIELD(SPI_TXFR0, TXCMD_TXDATA, 16, 16)
REG32(SPI_TXFR1, 0x40)
	FIELD(SPI_TXFR1, TXDATA, 0, 16)
	FIELD(SPI_TXFR1, TXCMD_TXDATA, 16, 16)
REG32(SPI_TXFR2, 0x44)
	FIELD(SPI_TXFR2, TXDATA, 0, 16)
	FIELD(SPI_TXFR2, TXCMD_TXDATA, 16, 16)
REG32(SPI_TXFR3, 0x48)
	FIELD(SPI_TXFR3, TXDATA, 0, 16)
	FIELD(SPI_TXFR3, TXCMD_TXDATA, 16, 16)
REG32(SPI_RXFR0, 0x7c)
	FIELD(SPI_RXFR0, RXDATA, 0, 32)
REG32(SPI_RXFR1, 0x80)
	FIELD(SPI_RXFR1, RXDATA, 0, 32)
REG32(SPI_RXFR2, 0x84)
	FIELD(SPI_RXFR2, RXDATA, 0, 32)
REG32(SPI_RXFR3, 0x88)
	FIELD(SPI_RXFR3, RXDATA, 0, 32)


static int frdmk82f_spi_can_receive(void *opaque) {
	return 1;
}

static void frdmk82f_spi_receive(void *opaque, const uint8_t *buf, int size) {
	FRDMK82FSPI *t = FRDMK82F_SPI(opaque);

	t->POPR = *buf;
	t->SR |= 0x20000;


}

static gboolean frdmk82f_spi_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	FRDMK82FSPI *t = FRDMK82F_SPI(opaque);
	int ret;

	t->watch_tag = 0;
	t->SR &= (~(0x2000000));
	t->SR &= (~(0x80000000));
	t->SR &= (~(0x20000));
	t->MCR |= 0x80000000;



	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->PUSHR), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, frdmk82f_spi_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->SR |= 0x20000;
	t->MCR &= (~(0x80000000));
	t->SR |= 0x80000000;
	t->SR |= 0x2000000;



	return FALSE;
}

static void frdmk82f_spi_register_reset(FRDMK82FSPI *t) {
	t->MCR = 0x4001;
	t->TCR = 0x0;
	t->CTAR_SLAVE = 0x78000000;
	t->SR = 0x2000000;
	t->RSER = 0x0;
	t->PUSHR = 0x0;
	t->PUSHR_SLAVE = 0x0;
	t->POPR = 0x0;
	t->CTAR0 = 0x78000000;
	t->CTAR1 = 0x78000000;
	t->TXFR0 = 0x0;
	t->TXFR1 = 0x0;
	t->TXFR2 = 0x0;
	t->TXFR3 = 0x0;
	t->RXFR0 = 0x0;
	t->RXFR1 = 0x0;
	t->RXFR2 = 0x0;
	t->RXFR3 = 0x0;
	t->SR |= 0x20000;
	t->MCR &= (~(0x80000000));
	t->SR |= 0x80000000;
	t->SR |= 0x2000000;

}

static uint64_t frdmk82f_spi_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMK82FSPI *t = FRDMK82F_SPI(opaque);
	uint64_t ret;

	switch (offset) {
		case A_SPI_MCR:
			ret = t->MCR;
			break;
		case A_SPI_TCR:
			ret = t->TCR;
			break;
		case A_SPI_CTAR_SLAVE:
			ret = t->CTAR_SLAVE;
			break;
		case A_SPI_SR:
			ret = t->SR;
			break;
		case A_SPI_RSER:
			ret = t->RSER;
			break;
		case A_SPI_PUSHR:
			ret = t->PUSHR;
			break;
		case A_SPI_POPR:
			ret = t->POPR;
			t->SR &= (~(0x20000));
			qemu_chr_fe_accept_input(&(t->chr));
			break;
		case A_SPI_CTAR1:
			ret = t->CTAR1;
			break;
		case A_SPI_TXFR0:
			ret = t->TXFR0;
			break;
		case A_SPI_TXFR1:
			ret = t->TXFR1;
			break;
		case A_SPI_TXFR2:
			ret = t->TXFR2;
			break;
		case A_SPI_TXFR3:
			ret = t->TXFR3;
			break;
		case A_SPI_RXFR0:
			ret = t->RXFR0;
			break;
		case A_SPI_RXFR1:
			ret = t->RXFR1;
			break;
		case A_SPI_RXFR2:
			ret = t->RXFR2;
			break;
		case A_SPI_RXFR3:
			ret = t->RXFR3;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK82F SPI read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmk82f_spi_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMK82FSPI *t = FRDMK82F_SPI(opaque);


	switch (offset) {
		case A_SPI_MCR ... A_SPI_MCR + 3:
			value = value << ((offset - A_SPI_MCR) << 3);
			offset = A_SPI_MCR;
			break;
		case A_SPI_TCR ... A_SPI_TCR + 3:
			value = value << ((offset - A_SPI_TCR) << 3);
			offset = A_SPI_TCR;
			break;
		case A_SPI_CTAR_SLAVE ... A_SPI_CTAR_SLAVE + 3:
			value = value << ((offset - A_SPI_CTAR_SLAVE) << 3);
			offset = A_SPI_CTAR_SLAVE;
			break;
		case A_SPI_SR ... A_SPI_SR + 3:
			value = value << ((offset - A_SPI_SR) << 3);
			offset = A_SPI_SR;
			break;
		case A_SPI_RSER ... A_SPI_RSER + 3:
			value = value << ((offset - A_SPI_RSER) << 3);
			offset = A_SPI_RSER;
			break;
		case A_SPI_PUSHR ... A_SPI_PUSHR + 3:
			value = value << ((offset - A_SPI_PUSHR) << 3);
			offset = A_SPI_PUSHR;
			break;
		case A_SPI_CTAR1 ... A_SPI_CTAR1 + 3:
			value = value << ((offset - A_SPI_CTAR1) << 3);
			offset = A_SPI_CTAR1;
			break;

		default: break;
	}

	switch (offset) {
		case A_SPI_MCR:
			t->MCR &= value;
			break;
		case A_SPI_TCR:
			t->TCR = value;
			break;
		case A_SPI_CTAR_SLAVE:
			t->CTAR_SLAVE = value;
			break;
		case A_SPI_SR:
			t->SR &= value;
			break;
		case A_SPI_RSER:
			t->RSER = value;
			break;
		case A_SPI_PUSHR:
			t->PUSHR = value;
			frdmk82f_spi_transmit(NULL, G_IO_OUT, t);
			break;
		case A_SPI_CTAR1:
			t->CTAR1 = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK82F SPI write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmk82f_spi_ops = {
	.read = frdmk82f_spi_read,
	.write = frdmk82f_spi_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmk82f_spi_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMK82FSPI *t = FRDMK82F_SPI(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmk82f_spi_ops, t, "frdmk82f-spi", FRDMK82F_SPI_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void frdmk82f_spi_realize(DeviceState *dev, Error **errp) {
	FRDMK82FSPI *t = FRDMK82F_SPI(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), frdmk82f_spi_can_receive, frdmk82f_spi_receive,
		NULL, NULL, t, NULL, true);

}

static void frdmk82f_spi_reset_enter(Object *obj, ResetType type) {
	FRDMK82FSPI *t = FRDMK82F_SPI(obj);
	frdmk82f_spi_register_reset(t);
}

static const VMStateDescription frdmk82f_spi_vmstate = {
	.name = "frdmk82f-spi",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(MCR, FRDMK82FSPI),
		VMSTATE_UINT32(TCR, FRDMK82FSPI),
		VMSTATE_UINT32(CTAR_SLAVE, FRDMK82FSPI),
		VMSTATE_UINT32(SR, FRDMK82FSPI),
		VMSTATE_UINT32(RSER, FRDMK82FSPI),
		VMSTATE_UINT32(PUSHR, FRDMK82FSPI),
		VMSTATE_UINT32(PUSHR_SLAVE, FRDMK82FSPI),
		VMSTATE_UINT32(POPR, FRDMK82FSPI),
		VMSTATE_UINT32(CTAR0, FRDMK82FSPI),
		VMSTATE_UINT32(CTAR1, FRDMK82FSPI),
		VMSTATE_UINT32(TXFR0, FRDMK82FSPI),
		VMSTATE_UINT32(TXFR1, FRDMK82FSPI),
		VMSTATE_UINT32(TXFR2, FRDMK82FSPI),
		VMSTATE_UINT32(TXFR3, FRDMK82FSPI),
		VMSTATE_UINT32(RXFR0, FRDMK82FSPI),
		VMSTATE_UINT32(RXFR1, FRDMK82FSPI),
		VMSTATE_UINT32(RXFR2, FRDMK82FSPI),
		VMSTATE_UINT32(RXFR3, FRDMK82FSPI),

		VMSTATE_END_OF_LIST()
	}
};

static Property frdmk82f_spi_properties[] = {
	DEFINE_PROP_CHR("chardev", FRDMK82FSPI, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void frdmk82f_spi_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmk82f_spi_vmstate;
	dc->realize = frdmk82f_spi_realize;
	rc->phases.enter = frdmk82f_spi_reset_enter;
	device_class_set_props(dc, frdmk82f_spi_properties);

}

static const TypeInfo frdmk82f_spi_info = {
	.name = TYPE_FRDMK82F_SPI,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMK82FSPI),
	.instance_init = frdmk82f_spi_init,
	.class_init = frdmk82f_spi_class_init,
};

static void frdmk82f_spi_register_types(void) {
	type_register_static(&frdmk82f_spi_info);
}

type_init(frdmk82f_spi_register_types);

#define TYPE_FRDMK82F_WDOG "frdmk82f-wdog"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMK82FWDOG, FRDMK82F_WDOG)

struct FRDMK82FWDOG {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint16_t STCTRLH;
	uint16_t STCTRLL;
	uint16_t TOVALH;
	uint16_t TOVALL;
	uint16_t WINH;
	uint16_t WINL;
	uint16_t REFRESH;
	uint16_t UNLOCK;
	uint16_t TMROUTH;
	uint16_t TMROUTL;
	uint16_t RSTCNT;
	uint16_t PRESC;
	/* base */
	uint32_t base;

};
#define FRDMK82F_WDOG_SIZE				0x18

REG16(WDOG_STCTRLH, 0x0)
	FIELD(WDOG_STCTRLH, WDOGEN, 0, 1)
	FIELD(WDOG_STCTRLH, CLKSRC, 1, 1)
	FIELD(WDOG_STCTRLH, IRQRSTEN, 2, 1)
	FIELD(WDOG_STCTRLH, WINEN, 3, 1)
	FIELD(WDOG_STCTRLH, ALLOWUPDATE, 4, 1)
	FIELD(WDOG_STCTRLH, DBGEN, 5, 1)
	FIELD(WDOG_STCTRLH, STOPEN, 6, 1)
	FIELD(WDOG_STCTRLH, WAITEN, 7, 1)
	FIELD(WDOG_STCTRLH, TESTWDOG, 10, 1)
	FIELD(WDOG_STCTRLH, TESTSEL, 11, 1)
	FIELD(WDOG_STCTRLH, BYTESEL, 12, 2)
	FIELD(WDOG_STCTRLH, DISTESTWDOG, 14, 1)
REG16(WDOG_STCTRLL, 0x2)
	FIELD(WDOG_STCTRLL, INTFLG, 15, 1)
REG16(WDOG_TOVALH, 0x4)
	FIELD(WDOG_TOVALH, TOVALHIGH, 0, 16)
REG16(WDOG_TOVALL, 0x6)
	FIELD(WDOG_TOVALL, TOVALLOW, 0, 16)
REG16(WDOG_WINH, 0x8)
	FIELD(WDOG_WINH, WINHIGH, 0, 16)
REG16(WDOG_WINL, 0xa)
	FIELD(WDOG_WINL, WINLOW, 0, 16)
REG16(WDOG_REFRESH, 0xc)
	FIELD(WDOG_REFRESH, WDOGREFRESH, 0, 16)
REG16(WDOG_UNLOCK, 0xe)
	FIELD(WDOG_UNLOCK, WDOGUNLOCK, 0, 16)
REG16(WDOG_TMROUTH, 0x10)
	FIELD(WDOG_TMROUTH, TIMEROUTHIGH, 0, 16)
REG16(WDOG_TMROUTL, 0x12)
	FIELD(WDOG_TMROUTL, TIMEROUTLOW, 0, 16)
REG16(WDOG_RSTCNT, 0x14)
	FIELD(WDOG_RSTCNT, RSTCNT, 0, 16)
REG16(WDOG_PRESC, 0x16)
	FIELD(WDOG_PRESC, PRESCVAL, 8, 3)


static void frdmk82f_wdog_register_reset(FRDMK82FWDOG *t) {
	t->STCTRLH = 0x1d3;
	t->STCTRLL = 0x1;
	t->TOVALH = 0x4c;
	t->TOVALL = 0x4b4c;
	t->WINH = 0x0;
	t->WINL = 0x10;
	t->REFRESH = 0xb480;
	t->UNLOCK = 0xd928;
	t->TMROUTH = 0x0;
	t->TMROUTL = 0x0;
	t->RSTCNT = 0x0;
	t->PRESC = 0x400;

}

static uint64_t frdmk82f_wdog_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMK82FWDOG *t = FRDMK82F_WDOG(opaque);
	uint64_t ret;

	switch (offset) {
		case A_WDOG_STCTRLH:
			ret = t->STCTRLH;
			break;
		case A_WDOG_STCTRLL:
			ret = t->STCTRLL;
			break;
		case A_WDOG_TOVALH:
			ret = t->TOVALH;
			break;
		case A_WDOG_TOVALL:
			ret = t->TOVALL;
			break;
		case A_WDOG_WINH:
			ret = t->WINH;
			break;
		case A_WDOG_WINL:
			ret = t->WINL;
			break;
		case A_WDOG_REFRESH:
			ret = t->REFRESH;
			break;
		case A_WDOG_UNLOCK:
			ret = t->UNLOCK;
			break;
		case A_WDOG_TMROUTH:
			ret = t->TMROUTH;
			break;
		case A_WDOG_TMROUTL:
			ret = t->TMROUTL;
			break;
		case A_WDOG_RSTCNT:
			ret = t->RSTCNT;
			break;
		case A_WDOG_PRESC:
			ret = t->PRESC;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK82F WDOG read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmk82f_wdog_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMK82FWDOG *t = FRDMK82F_WDOG(opaque);


	switch (offset) {
		case A_WDOG_STCTRLH ... A_WDOG_STCTRLH + 1:
			value = value << ((offset - A_WDOG_STCTRLH) << 3);
			offset = A_WDOG_STCTRLH;
			break;
		case A_WDOG_STCTRLL ... A_WDOG_STCTRLL + 1:
			value = value << ((offset - A_WDOG_STCTRLL) << 3);
			offset = A_WDOG_STCTRLL;
			break;
		case A_WDOG_TOVALH ... A_WDOG_TOVALH + 1:
			value = value << ((offset - A_WDOG_TOVALH) << 3);
			offset = A_WDOG_TOVALH;
			break;
		case A_WDOG_TOVALL ... A_WDOG_TOVALL + 1:
			value = value << ((offset - A_WDOG_TOVALL) << 3);
			offset = A_WDOG_TOVALL;
			break;
		case A_WDOG_WINH ... A_WDOG_WINH + 1:
			value = value << ((offset - A_WDOG_WINH) << 3);
			offset = A_WDOG_WINH;
			break;
		case A_WDOG_WINL ... A_WDOG_WINL + 1:
			value = value << ((offset - A_WDOG_WINL) << 3);
			offset = A_WDOG_WINL;
			break;
		case A_WDOG_REFRESH ... A_WDOG_REFRESH + 1:
			value = value << ((offset - A_WDOG_REFRESH) << 3);
			offset = A_WDOG_REFRESH;
			break;
		case A_WDOG_UNLOCK ... A_WDOG_UNLOCK + 1:
			value = value << ((offset - A_WDOG_UNLOCK) << 3);
			offset = A_WDOG_UNLOCK;
			break;
		case A_WDOG_TMROUTH ... A_WDOG_TMROUTH + 1:
			value = value << ((offset - A_WDOG_TMROUTH) << 3);
			offset = A_WDOG_TMROUTH;
			break;
		case A_WDOG_TMROUTL ... A_WDOG_TMROUTL + 1:
			value = value << ((offset - A_WDOG_TMROUTL) << 3);
			offset = A_WDOG_TMROUTL;
			break;
		case A_WDOG_RSTCNT ... A_WDOG_RSTCNT + 1:
			value = value << ((offset - A_WDOG_RSTCNT) << 3);
			offset = A_WDOG_RSTCNT;
			break;
		case A_WDOG_PRESC ... A_WDOG_PRESC + 1:
			value = value << ((offset - A_WDOG_PRESC) << 3);
			offset = A_WDOG_PRESC;
			break;

		default: break;
	}

	switch (offset) {
		case A_WDOG_STCTRLH:
			t->STCTRLH = value;
			break;
		case A_WDOG_STCTRLL:
			t->STCTRLL = value;
			break;
		case A_WDOG_TOVALH:
			t->TOVALH = value;
			break;
		case A_WDOG_TOVALL:
			t->TOVALL = value;
			break;
		case A_WDOG_WINH:
			t->WINH = value;
			break;
		case A_WDOG_WINL:
			t->WINL = value;
			break;
		case A_WDOG_REFRESH:
			t->REFRESH = value;
			break;
		case A_WDOG_UNLOCK:
			t->UNLOCK = value;
			break;
		case A_WDOG_TMROUTH:
			t->TMROUTH = value;
			break;
		case A_WDOG_TMROUTL:
			t->TMROUTL = value;
			break;
		case A_WDOG_RSTCNT:
			t->RSTCNT = value;
			break;
		case A_WDOG_PRESC:
			t->PRESC = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK82F WDOG write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmk82f_wdog_ops = {
	.read = frdmk82f_wdog_read,
	.write = frdmk82f_wdog_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmk82f_wdog_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMK82FWDOG *t = FRDMK82F_WDOG(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmk82f_wdog_ops, t, "frdmk82f-wdog", FRDMK82F_WDOG_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void frdmk82f_wdog_realize(DeviceState *dev, Error **errp) {
	return;

}

static void frdmk82f_wdog_reset_enter(Object *obj, ResetType type) {
	FRDMK82FWDOG *t = FRDMK82F_WDOG(obj);
	frdmk82f_wdog_register_reset(t);
}

static const VMStateDescription frdmk82f_wdog_vmstate = {
	.name = "frdmk82f-wdog",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT16(STCTRLH, FRDMK82FWDOG),
		VMSTATE_UINT16(STCTRLL, FRDMK82FWDOG),
		VMSTATE_UINT16(TOVALH, FRDMK82FWDOG),
		VMSTATE_UINT16(TOVALL, FRDMK82FWDOG),
		VMSTATE_UINT16(WINH, FRDMK82FWDOG),
		VMSTATE_UINT16(WINL, FRDMK82FWDOG),
		VMSTATE_UINT16(REFRESH, FRDMK82FWDOG),
		VMSTATE_UINT16(UNLOCK, FRDMK82FWDOG),
		VMSTATE_UINT16(TMROUTH, FRDMK82FWDOG),
		VMSTATE_UINT16(TMROUTL, FRDMK82FWDOG),
		VMSTATE_UINT16(RSTCNT, FRDMK82FWDOG),
		VMSTATE_UINT16(PRESC, FRDMK82FWDOG),

		VMSTATE_END_OF_LIST()
	}
};

static void frdmk82f_wdog_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmk82f_wdog_vmstate;
	dc->realize = frdmk82f_wdog_realize;
	rc->phases.enter = frdmk82f_wdog_reset_enter;

}

static const TypeInfo frdmk82f_wdog_info = {
	.name = TYPE_FRDMK82F_WDOG,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMK82FWDOG),
	.instance_init = frdmk82f_wdog_init,
	.class_init = frdmk82f_wdog_class_init,
};

static void frdmk82f_wdog_register_types(void) {
	type_register_static(&frdmk82f_wdog_info);
}

type_init(frdmk82f_wdog_register_types);

#define TYPE_FRDMK82F_SIM "frdmk82f-sim"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMK82FSIM, FRDMK82F_SIM)

struct FRDMK82FSIM {
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
	uint32_t SOPT8;
	uint32_t SOPT9;
	uint32_t SDID;
	uint32_t SCGC1;
	uint32_t SCGC2;
	uint32_t SCGC3;
	uint32_t SCGC4;
	uint32_t SCGC5;
	uint32_t SCGC6;
	uint32_t SCGC7;
	uint32_t CLKDIV1;
	uint32_t CLKDIV2;
	uint32_t FCFG1;
	uint32_t FCFG2;
	uint32_t UIDH;
	uint32_t UIDMH;
	uint32_t UIDML;
	uint32_t UIDL;
	uint32_t CLKDIV3;
	uint32_t CLKDIV4;
	/* base */
	uint32_t base;

};
#define FRDMK82F_SIM_SIZE				0x106c

REG32(SIM_SOPT1, 0x0)
	FIELD(SIM_SOPT1, RAMSIZE, 12, 4)
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
	FIELD(SIM_SOPT2, FBSL, 8, 2)
	FIELD(SIM_SOPT2, TRACECLKSEL, 12, 1)
	FIELD(SIM_SOPT2, PLLFLLSEL, 16, 2)
	FIELD(SIM_SOPT2, USBSRC, 18, 1)
	FIELD(SIM_SOPT2, FLEXIOSRC, 22, 2)
	FIELD(SIM_SOPT2, TPMSRC, 24, 2)
	FIELD(SIM_SOPT2, LPUARTSRC, 26, 2)
	FIELD(SIM_SOPT2, SDHCSRC, 28, 2)
	FIELD(SIM_SOPT2, EMVSIMSRC, 30, 2)
REG32(SIM_SOPT4, 0x100c)
	FIELD(SIM_SOPT4, FTM0FLT0, 0, 1)
	FIELD(SIM_SOPT4, FTM0FLT1, 1, 1)
	FIELD(SIM_SOPT4, FTM1FLT0, 4, 1)
	FIELD(SIM_SOPT4, FTM2FLT0, 8, 1)
	FIELD(SIM_SOPT4, FTM3FLT0, 12, 1)
	FIELD(SIM_SOPT4, FTM1CH0SRC, 18, 2)
	FIELD(SIM_SOPT4, FTM2CH0SRC, 20, 2)
	FIELD(SIM_SOPT4, FTM2CH1SRC, 22, 1)
	FIELD(SIM_SOPT4, FTM0CLKSEL, 24, 1)
	FIELD(SIM_SOPT4, FTM1CLKSEL, 25, 1)
	FIELD(SIM_SOPT4, FTM2CLKSEL, 26, 1)
	FIELD(SIM_SOPT4, FTM3CLKSEL, 27, 1)
	FIELD(SIM_SOPT4, FTM0TRG0SRC, 28, 1)
	FIELD(SIM_SOPT4, FTM0TRG1SRC, 29, 1)
	FIELD(SIM_SOPT4, FTM3TRG0SRC, 30, 1)
	FIELD(SIM_SOPT4, FTM3TRG1SRC, 31, 1)
REG32(SIM_SOPT5, 0x1010)
	FIELD(SIM_SOPT5, LPUART0TXSRC, 16, 2)
	FIELD(SIM_SOPT5, LPUART0RXSRC, 18, 2)
	FIELD(SIM_SOPT5, LPUART1TXSRC, 20, 2)
	FIELD(SIM_SOPT5, LPUART1RXSRC, 22, 2)
REG32(SIM_SOPT7, 0x1018)
	FIELD(SIM_SOPT7, ADC0TRGSEL, 0, 4)
	FIELD(SIM_SOPT7, ADC0PRETRGSEL, 4, 1)
	FIELD(SIM_SOPT7, ADC0ALTTRGEN, 7, 1)
REG32(SIM_SOPT8, 0x101c)
	FIELD(SIM_SOPT8, FTM0SYNCBIT, 0, 1)
	FIELD(SIM_SOPT8, FTM1SYNCBIT, 1, 1)
	FIELD(SIM_SOPT8, FTM2SYNCBIT, 2, 1)
	FIELD(SIM_SOPT8, FTM3SYNCBIT, 3, 1)
	FIELD(SIM_SOPT8, FTM0OCH0SRC, 16, 1)
	FIELD(SIM_SOPT8, FTM0OCH1SRC, 17, 1)
	FIELD(SIM_SOPT8, FTM0OCH2SRC, 18, 1)
	FIELD(SIM_SOPT8, FTM0OCH3SRC, 19, 1)
	FIELD(SIM_SOPT8, FTM0OCH4SRC, 20, 1)
	FIELD(SIM_SOPT8, FTM0OCH5SRC, 21, 1)
	FIELD(SIM_SOPT8, FTM0OCH6SRC, 22, 1)
	FIELD(SIM_SOPT8, FTM0OCH7SRC, 23, 1)
	FIELD(SIM_SOPT8, FTM3OCH0SRC, 24, 1)
	FIELD(SIM_SOPT8, FTM3OCH1SRC, 25, 1)
	FIELD(SIM_SOPT8, FTM3OCH2SRC, 26, 1)
	FIELD(SIM_SOPT8, FTM3OCH3SRC, 27, 1)
	FIELD(SIM_SOPT8, FTM3OCH4SRC, 28, 1)
	FIELD(SIM_SOPT8, FTM3OCH5SRC, 29, 1)
	FIELD(SIM_SOPT8, FTM3OCH6SRC, 30, 1)
	FIELD(SIM_SOPT8, FTM3OCH7SRC, 31, 1)
REG32(SIM_SOPT9, 0x1020)
	FIELD(SIM_SOPT9, TPM1CH0SRC, 18, 2)
	FIELD(SIM_SOPT9, TPM2CH0SRC, 20, 2)
	FIELD(SIM_SOPT9, TPM1CLKSEL, 25, 1)
	FIELD(SIM_SOPT9, TPM2CLKSEL, 26, 1)
REG32(SIM_SDID, 0x1024)
	FIELD(SIM_SDID, PINID, 0, 4)
	FIELD(SIM_SDID, FAMID, 4, 3)
	FIELD(SIM_SDID, DIEID, 7, 5)
	FIELD(SIM_SDID, REVID, 12, 4)
	FIELD(SIM_SDID, SERIESID, 20, 4)
	FIELD(SIM_SDID, SUBFAMID, 24, 4)
	FIELD(SIM_SDID, FAMILYID, 28, 4)
REG32(SIM_SCGC1, 0x1028)
	FIELD(SIM_SCGC1, I2C2, 6, 1)
	FIELD(SIM_SCGC1, I2C3, 7, 1)
REG32(SIM_SCGC2, 0x102c)
	FIELD(SIM_SCGC2, LPUART0, 4, 1)
	FIELD(SIM_SCGC2, LPUART1, 5, 1)
	FIELD(SIM_SCGC2, LPUART2, 6, 1)
	FIELD(SIM_SCGC2, LPUART3, 7, 1)
	FIELD(SIM_SCGC2, TPM1, 9, 1)
	FIELD(SIM_SCGC2, TPM2, 10, 1)
	FIELD(SIM_SCGC2, DAC0, 12, 1)
	FIELD(SIM_SCGC2, LTC, 17, 1)
	FIELD(SIM_SCGC2, EMVSIM0, 20, 1)
	FIELD(SIM_SCGC2, EMVSIM1, 21, 1)
	FIELD(SIM_SCGC2, LPUART4, 22, 1)
	FIELD(SIM_SCGC2, QSPI, 26, 1)
	FIELD(SIM_SCGC2, FLEXIO, 31, 1)
REG32(SIM_SCGC3, 0x1030)
	FIELD(SIM_SCGC3, TRNG, 0, 1)
	FIELD(SIM_SCGC3, SPI2, 12, 1)
	FIELD(SIM_SCGC3, SDHC, 17, 1)
	FIELD(SIM_SCGC3, FTM2, 24, 1)
	FIELD(SIM_SCGC3, FTM3, 25, 1)
REG32(SIM_SCGC4, 0x1034)
	FIELD(SIM_SCGC4, EWM, 1, 1)
	FIELD(SIM_SCGC4, CMT, 2, 1)
	FIELD(SIM_SCGC4, I2C0, 6, 1)
	FIELD(SIM_SCGC4, I2C1, 7, 1)
	FIELD(SIM_SCGC4, USBOTG, 18, 1)
	FIELD(SIM_SCGC4, CMP, 19, 1)
	FIELD(SIM_SCGC4, VREF, 20, 1)
REG32(SIM_SCGC5, 0x1038)
	FIELD(SIM_SCGC5, LPTMR, 0, 1)
	FIELD(SIM_SCGC5, LPTMR1, 4, 1)
	FIELD(SIM_SCGC5, TSI, 5, 1)
	FIELD(SIM_SCGC5, PORTA, 9, 1)
	FIELD(SIM_SCGC5, PORTB, 10, 1)
	FIELD(SIM_SCGC5, PORTC, 11, 1)
	FIELD(SIM_SCGC5, PORTD, 12, 1)
	FIELD(SIM_SCGC5, PORTE, 13, 1)
REG32(SIM_SCGC6, 0x103c)
	FIELD(SIM_SCGC6, FTF, 0, 1)
	FIELD(SIM_SCGC6, DMAMUX, 1, 1)
	FIELD(SIM_SCGC6, SPI0, 12, 1)
	FIELD(SIM_SCGC6, SPI1, 13, 1)
	FIELD(SIM_SCGC6, I2S, 15, 1)
	FIELD(SIM_SCGC6, CRC, 18, 1)
	FIELD(SIM_SCGC6, USBDCD, 21, 1)
	FIELD(SIM_SCGC6, PDB, 22, 1)
	FIELD(SIM_SCGC6, PIT, 23, 1)
	FIELD(SIM_SCGC6, FTM0, 24, 1)
	FIELD(SIM_SCGC6, FTM1, 25, 1)
	FIELD(SIM_SCGC6, FTM2, 26, 1)
	FIELD(SIM_SCGC6, ADC0, 27, 1)
	FIELD(SIM_SCGC6, RTC, 29, 1)
	FIELD(SIM_SCGC6, DAC0, 31, 1)
REG32(SIM_SCGC7, 0x1040)
	FIELD(SIM_SCGC7, FLEXBUS, 0, 1)
	FIELD(SIM_SCGC7, DMA, 1, 1)
	FIELD(SIM_SCGC7, MPU, 2, 1)
	FIELD(SIM_SCGC7, SDRAMC, 3, 1)
REG32(SIM_CLKDIV1, 0x1044)
	FIELD(SIM_CLKDIV1, OUTDIV4, 16, 4)
	FIELD(SIM_CLKDIV1, OUTDIV3, 20, 4)
	FIELD(SIM_CLKDIV1, OUTDIV2, 24, 4)
	FIELD(SIM_CLKDIV1, OUTDIV1, 28, 4)
REG32(SIM_CLKDIV2, 0x1048)
	FIELD(SIM_CLKDIV2, USBFRAC, 0, 1)
	FIELD(SIM_CLKDIV2, USBDIV, 1, 3)
REG32(SIM_FCFG1, 0x104c)
	FIELD(SIM_FCFG1, FLASHDIS, 0, 1)
	FIELD(SIM_FCFG1, FLASHDOZE, 1, 1)
	FIELD(SIM_FCFG1, PFSIZE, 24, 4)
REG32(SIM_FCFG2, 0x1050)
	FIELD(SIM_FCFG2, MAXADDR1, 16, 7)
	FIELD(SIM_FCFG2, MAXADDR0, 24, 7)
REG32(SIM_UIDH, 0x1054)
	FIELD(SIM_UIDH, UID, 0, 32)
REG32(SIM_UIDMH, 0x1058)
	FIELD(SIM_UIDMH, UID, 0, 32)
REG32(SIM_UIDML, 0x105c)
	FIELD(SIM_UIDML, UID, 0, 32)
REG32(SIM_UIDL, 0x1060)
	FIELD(SIM_UIDL, UID, 0, 32)
REG32(SIM_CLKDIV3, 0x1064)
	FIELD(SIM_CLKDIV3, PLLFLLFRAC, 0, 1)
	FIELD(SIM_CLKDIV3, PLLFLLDIV, 1, 3)
REG32(SIM_CLKDIV4, 0x1068)
	FIELD(SIM_CLKDIV4, TRACEFRAC, 0, 1)
	FIELD(SIM_CLKDIV4, TRACEDIV, 1, 3)


static void frdmk82f_sim_register_reset(FRDMK82FSIM *t) {
	t->SOPT1 = 0x80000000;
	t->SOPT1CFG = 0x0;
	t->SOPT2 = 0x1000;
	t->SOPT4 = 0x0;
	t->SOPT5 = 0x0;
	t->SOPT7 = 0x0;
	t->SOPT8 = 0x0;
	t->SOPT9 = 0x0;
	t->SDID = 0xb80;
	t->SCGC1 = 0x0;
	t->SCGC2 = 0x0;
	t->SCGC3 = 0x0;
	t->SCGC4 = 0xf0100030;
	t->SCGC5 = 0x40182;
	t->SCGC6 = 0x40000001;
	t->SCGC7 = 0x6;
	t->CLKDIV1 = 0x110000;
	t->CLKDIV2 = 0x0;
	t->FCFG1 = 0xf0f0f00;
	t->FCFG2 = 0x7fff0000;
	t->UIDH = 0x0;
	t->UIDMH = 0x0;
	t->UIDML = 0x0;
	t->UIDL = 0x0;
	t->CLKDIV3 = 0x0;
	t->CLKDIV4 = 0x0;

}

static uint64_t frdmk82f_sim_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMK82FSIM *t = FRDMK82F_SIM(opaque);
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
		case A_SIM_SOPT8:
			ret = t->SOPT8;
			break;
		case A_SIM_SOPT9:
			ret = t->SOPT9;
			break;
		case A_SIM_SDID:
			ret = t->SDID;
			break;
		case A_SIM_SCGC1:
			ret = t->SCGC1;
			break;
		case A_SIM_SCGC2:
			ret = t->SCGC2;
			break;
		case A_SIM_SCGC3:
			ret = t->SCGC3;
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
		case A_SIM_CLKDIV2:
			ret = t->CLKDIV2;
			break;
		case A_SIM_FCFG1:
			ret = t->FCFG1;
			break;
		case A_SIM_FCFG2:
			ret = t->FCFG2;
			break;
		case A_SIM_UIDH:
			ret = t->UIDH;
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
		case A_SIM_CLKDIV3:
			ret = t->CLKDIV3;
			break;
		case A_SIM_CLKDIV4:
			ret = t->CLKDIV4;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK82F SIM read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmk82f_sim_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMK82FSIM *t = FRDMK82F_SIM(opaque);


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
		case A_SIM_SOPT8 ... A_SIM_SOPT8 + 3:
			value = value << ((offset - A_SIM_SOPT8) << 3);
			offset = A_SIM_SOPT8;
			break;
		case A_SIM_SOPT9 ... A_SIM_SOPT9 + 3:
			value = value << ((offset - A_SIM_SOPT9) << 3);
			offset = A_SIM_SOPT9;
			break;
		case A_SIM_SCGC1 ... A_SIM_SCGC1 + 3:
			value = value << ((offset - A_SIM_SCGC1) << 3);
			offset = A_SIM_SCGC1;
			break;
		case A_SIM_SCGC2 ... A_SIM_SCGC2 + 3:
			value = value << ((offset - A_SIM_SCGC2) << 3);
			offset = A_SIM_SCGC2;
			break;
		case A_SIM_SCGC3 ... A_SIM_SCGC3 + 3:
			value = value << ((offset - A_SIM_SCGC3) << 3);
			offset = A_SIM_SCGC3;
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
		case A_SIM_CLKDIV2 ... A_SIM_CLKDIV2 + 3:
			value = value << ((offset - A_SIM_CLKDIV2) << 3);
			offset = A_SIM_CLKDIV2;
			break;
		case A_SIM_FCFG1 ... A_SIM_FCFG1 + 3:
			value = value << ((offset - A_SIM_FCFG1) << 3);
			offset = A_SIM_FCFG1;
			break;
		case A_SIM_CLKDIV3 ... A_SIM_CLKDIV3 + 3:
			value = value << ((offset - A_SIM_CLKDIV3) << 3);
			offset = A_SIM_CLKDIV3;
			break;
		case A_SIM_CLKDIV4 ... A_SIM_CLKDIV4 + 3:
			value = value << ((offset - A_SIM_CLKDIV4) << 3);
			offset = A_SIM_CLKDIV4;
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
		case A_SIM_SOPT8:
			t->SOPT8 = value;
			break;
		case A_SIM_SOPT9:
			t->SOPT9 = value;
			break;
		case A_SIM_SCGC1:
			t->SCGC1 = value;
			break;
		case A_SIM_SCGC2:
			t->SCGC2 = value;
			break;
		case A_SIM_SCGC3:
			t->SCGC3 = value;
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
		case A_SIM_CLKDIV2:
			t->CLKDIV2 = value;
			break;
		case A_SIM_FCFG1:
			t->FCFG1 = value;
			break;
		case A_SIM_CLKDIV3:
			t->CLKDIV3 = value;
			break;
		case A_SIM_CLKDIV4:
			t->CLKDIV4 = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK82F SIM write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmk82f_sim_ops = {
	.read = frdmk82f_sim_read,
	.write = frdmk82f_sim_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmk82f_sim_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMK82FSIM *t = FRDMK82F_SIM(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmk82f_sim_ops, t, "frdmk82f-sim", FRDMK82F_SIM_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void frdmk82f_sim_realize(DeviceState *dev, Error **errp) {
	return;

}

static void frdmk82f_sim_reset_enter(Object *obj, ResetType type) {
	FRDMK82FSIM *t = FRDMK82F_SIM(obj);
	frdmk82f_sim_register_reset(t);
}

static const VMStateDescription frdmk82f_sim_vmstate = {
	.name = "frdmk82f-sim",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(SOPT1, FRDMK82FSIM),
		VMSTATE_UINT32(SOPT1CFG, FRDMK82FSIM),
		VMSTATE_UINT32(SOPT2, FRDMK82FSIM),
		VMSTATE_UINT32(SOPT4, FRDMK82FSIM),
		VMSTATE_UINT32(SOPT5, FRDMK82FSIM),
		VMSTATE_UINT32(SOPT7, FRDMK82FSIM),
		VMSTATE_UINT32(SOPT8, FRDMK82FSIM),
		VMSTATE_UINT32(SOPT9, FRDMK82FSIM),
		VMSTATE_UINT32(SDID, FRDMK82FSIM),
		VMSTATE_UINT32(SCGC1, FRDMK82FSIM),
		VMSTATE_UINT32(SCGC2, FRDMK82FSIM),
		VMSTATE_UINT32(SCGC3, FRDMK82FSIM),
		VMSTATE_UINT32(SCGC4, FRDMK82FSIM),
		VMSTATE_UINT32(SCGC5, FRDMK82FSIM),
		VMSTATE_UINT32(SCGC6, FRDMK82FSIM),
		VMSTATE_UINT32(SCGC7, FRDMK82FSIM),
		VMSTATE_UINT32(CLKDIV1, FRDMK82FSIM),
		VMSTATE_UINT32(CLKDIV2, FRDMK82FSIM),
		VMSTATE_UINT32(FCFG1, FRDMK82FSIM),
		VMSTATE_UINT32(FCFG2, FRDMK82FSIM),
		VMSTATE_UINT32(UIDH, FRDMK82FSIM),
		VMSTATE_UINT32(UIDMH, FRDMK82FSIM),
		VMSTATE_UINT32(UIDML, FRDMK82FSIM),
		VMSTATE_UINT32(UIDL, FRDMK82FSIM),
		VMSTATE_UINT32(CLKDIV3, FRDMK82FSIM),
		VMSTATE_UINT32(CLKDIV4, FRDMK82FSIM),

		VMSTATE_END_OF_LIST()
	}
};

static void frdmk82f_sim_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmk82f_sim_vmstate;
	dc->realize = frdmk82f_sim_realize;
	rc->phases.enter = frdmk82f_sim_reset_enter;

}

static const TypeInfo frdmk82f_sim_info = {
	.name = TYPE_FRDMK82F_SIM,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMK82FSIM),
	.instance_init = frdmk82f_sim_init,
	.class_init = frdmk82f_sim_class_init,
};

static void frdmk82f_sim_register_types(void) {
	type_register_static(&frdmk82f_sim_info);
}

type_init(frdmk82f_sim_register_types);

#define TYPE_FRDMK82F_PMC "frdmk82f-pmc"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMK82FPMC, FRDMK82F_PMC)

struct FRDMK82FPMC {
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
	uint8_t HVDSC1;
	/* base */
	uint32_t base;

};
#define FRDMK82F_PMC_SIZE				0xc

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
REG8(PMC_HVDSC1, 0xb)
	FIELD(PMC_HVDSC1, HVDV, 0, 1)
	FIELD(PMC_HVDSC1, HVDRE, 4, 1)
	FIELD(PMC_HVDSC1, HVDIE, 5, 1)
	FIELD(PMC_HVDSC1, HVDACK, 6, 1)
	FIELD(PMC_HVDSC1, HVDF, 7, 1)


static void frdmk82f_pmc_register_reset(FRDMK82FPMC *t) {
	t->LVDSC1 = 0x10;
	t->LVDSC2 = 0x0;
	t->REGSC = 0x4;
	t->HVDSC1 = 0x1;

}

static uint64_t frdmk82f_pmc_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMK82FPMC *t = FRDMK82F_PMC(opaque);
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
		case A_PMC_HVDSC1:
			ret = t->HVDSC1;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK82F PMC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmk82f_pmc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMK82FPMC *t = FRDMK82F_PMC(opaque);


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
		case A_PMC_HVDSC1:
			t->HVDSC1 = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK82F PMC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmk82f_pmc_ops = {
	.read = frdmk82f_pmc_read,
	.write = frdmk82f_pmc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmk82f_pmc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMK82FPMC *t = FRDMK82F_PMC(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmk82f_pmc_ops, t, "frdmk82f-pmc", FRDMK82F_PMC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void frdmk82f_pmc_realize(DeviceState *dev, Error **errp) {
	return;

}

static void frdmk82f_pmc_reset_enter(Object *obj, ResetType type) {
	FRDMK82FPMC *t = FRDMK82F_PMC(obj);
	frdmk82f_pmc_register_reset(t);
}

static const VMStateDescription frdmk82f_pmc_vmstate = {
	.name = "frdmk82f-pmc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT8(LVDSC1, FRDMK82FPMC),
		VMSTATE_UINT8(LVDSC2, FRDMK82FPMC),
		VMSTATE_UINT8(REGSC, FRDMK82FPMC),
		VMSTATE_UINT8(HVDSC1, FRDMK82FPMC),

		VMSTATE_END_OF_LIST()
	}
};

static void frdmk82f_pmc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmk82f_pmc_vmstate;
	dc->realize = frdmk82f_pmc_realize;
	rc->phases.enter = frdmk82f_pmc_reset_enter;

}

static const TypeInfo frdmk82f_pmc_info = {
	.name = TYPE_FRDMK82F_PMC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMK82FPMC),
	.instance_init = frdmk82f_pmc_init,
	.class_init = frdmk82f_pmc_class_init,
};

static void frdmk82f_pmc_register_types(void) {
	type_register_static(&frdmk82f_pmc_info);
}

type_init(frdmk82f_pmc_register_types);

#define TYPE_FRDMK82F_OSC "frdmk82f-osc"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMK82FOSC, FRDMK82F_OSC)

struct FRDMK82FOSC {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/*registers*/
	uint8_t CR;
	uint8_t DIV;
	/* base */
	uint32_t base;

};
#define FRDMK82F_OSC_SIZE				0x3

REG8(OSC_CR, 0x0)
	FIELD(OSC_CR, SC16P, 0, 1)
	FIELD(OSC_CR, SC8P, 1, 1)
	FIELD(OSC_CR, SC4P, 2, 1)
	FIELD(OSC_CR, SC2P, 3, 1)
	FIELD(OSC_CR, EREFSTEN, 5, 1)
	FIELD(OSC_CR, ERCLKEN, 7, 1)
REG8(OSC_DIV, 0x2)
	FIELD(OSC_DIV, ERPS, 6, 2)


static void frdmk82f_osc_register_reset(FRDMK82FOSC *t) {
	t->CR = 0x0;
	t->DIV = 0x0;

}

static uint64_t frdmk82f_osc_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMK82FOSC *t = FRDMK82F_OSC(opaque);
	uint64_t ret;

	switch (offset) {
		case A_OSC_CR:
			ret = t->CR;
			break;
		case A_OSC_DIV:
			ret = t->DIV;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK82F OSC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmk82f_osc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMK82FOSC *t = FRDMK82F_OSC(opaque);


	switch (offset) {

		default: break;
	}

	switch (offset) {
		case A_OSC_CR:
			t->CR = value;
			break;
		case A_OSC_DIV:
			t->DIV = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK82F OSC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmk82f_osc_ops = {
	.read = frdmk82f_osc_read,
	.write = frdmk82f_osc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmk82f_osc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMK82FOSC *t = FRDMK82F_OSC(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmk82f_osc_ops, t, "frdmk82f-osc", FRDMK82F_OSC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void frdmk82f_osc_realize(DeviceState *dev, Error **errp) {
	return;

}

static void frdmk82f_osc_reset_enter(Object *obj, ResetType type) {
	FRDMK82FOSC *t = FRDMK82F_OSC(obj);
	frdmk82f_osc_register_reset(t);
}

static const VMStateDescription frdmk82f_osc_vmstate = {
	.name = "frdmk82f-osc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT8(CR, FRDMK82FOSC),
		VMSTATE_UINT8(DIV, FRDMK82FOSC),

		VMSTATE_END_OF_LIST()
	}
};

static void frdmk82f_osc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmk82f_osc_vmstate;
	dc->realize = frdmk82f_osc_realize;
	rc->phases.enter = frdmk82f_osc_reset_enter;

}

static const TypeInfo frdmk82f_osc_info = {
	.name = TYPE_FRDMK82F_OSC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMK82FOSC),
	.instance_init = frdmk82f_osc_init,
	.class_init = frdmk82f_osc_class_init,
};

static void frdmk82f_osc_register_types(void) {
	type_register_static(&frdmk82f_osc_info);
}

type_init(frdmk82f_osc_register_types);

#define TYPE_FRDMK82F_MCG "frdmk82f-mcg"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMK82FMCG, FRDMK82F_MCG)

struct FRDMK82FMCG {
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
	/* base */
	uint32_t base;

};
#define FRDMK82F_MCG_SIZE				0xe

REG8(MCG_C1, 0x0)
	FIELD(MCG_C1, IREFSTEN, 0, 1)
	FIELD(MCG_C1, IRCLKEN, 1, 1)
	FIELD(MCG_C1, IREFS, 2, 1)
	FIELD(MCG_C1, FRDIV, 3, 3)
	FIELD(MCG_C1, CLKS, 6, 2)
REG8(MCG_C2, 0x1)
	FIELD(MCG_C2, IRCS, 0, 1)
	FIELD(MCG_C2, LP, 1, 1)
	FIELD(MCG_C2, EREFS, 2, 1)
	FIELD(MCG_C2, HGO, 3, 1)
	FIELD(MCG_C2, RANGE, 4, 2)
	FIELD(MCG_C2, FCFTRIM, 6, 1)
	FIELD(MCG_C2, LOCRE0, 7, 1)
REG8(MCG_C3, 0x2)
	FIELD(MCG_C3, SCTRIM, 0, 8)
REG8(MCG_C4, 0x3)
	FIELD(MCG_C4, SCFTRIM, 0, 1)
	FIELD(MCG_C4, FCTRIM, 1, 4)
	FIELD(MCG_C4, DRST_DRS, 5, 2)
	FIELD(MCG_C4, DMX32, 7, 1)
REG8(MCG_C5, 0x4)
	FIELD(MCG_C5, PRDIV, 0, 3)
	FIELD(MCG_C5, PLLSTEN, 5, 1)
	FIELD(MCG_C5, PLLCLKEN, 6, 1)
REG8(MCG_C6, 0x5)
	FIELD(MCG_C6, VDIV, 0, 5)
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
	FIELD(MCG_C7, OSCSEL, 0, 2)
REG8(MCG_C8, 0xd)
	FIELD(MCG_C8, LOCS1, 0, 1)
	FIELD(MCG_C8, CME1, 5, 1)
	FIELD(MCG_C8, LOLRE, 6, 1)
	FIELD(MCG_C8, LOCRE1, 7, 1)


static void frdmk82f_mcg_register_reset(FRDMK82FMCG *t) {
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

}

static uint64_t frdmk82f_mcg_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMK82FMCG *t = FRDMK82F_MCG(opaque);
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

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK82F MCG read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmk82f_mcg_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMK82FMCG *t = FRDMK82F_MCG(opaque);


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
			if (((!(value & 0x1)) && (value & 0x20) && (!(value & 0x40)) && (value & 0x80))) {
				t->SC &= (~(0x80));
			} else if (((!(value & 0x1)) && (value & 0x20) && (value & 0x40) && (value & 0x80))) {
				t->SC &= (~(0x80));
			}
			break;
		case A_MCG_ATCVH:
			t->ATCVH = value;
			break;
		case A_MCG_ATCVL:
			t->ATCVL = value;
			break;
		case A_MCG_C7:
			t->C7 = value;
			break;
		case A_MCG_C8:
			t->C8 = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK82F MCG write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmk82f_mcg_ops = {
	.read = frdmk82f_mcg_read,
	.write = frdmk82f_mcg_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmk82f_mcg_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMK82FMCG *t = FRDMK82F_MCG(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmk82f_mcg_ops, t, "frdmk82f-mcg", FRDMK82F_MCG_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void frdmk82f_mcg_realize(DeviceState *dev, Error **errp) {
	return;

}

static void frdmk82f_mcg_reset_enter(Object *obj, ResetType type) {
	FRDMK82FMCG *t = FRDMK82F_MCG(obj);
	frdmk82f_mcg_register_reset(t);
}

static const VMStateDescription frdmk82f_mcg_vmstate = {
	.name = "frdmk82f-mcg",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT8(C1, FRDMK82FMCG),
		VMSTATE_UINT8(C2, FRDMK82FMCG),
		VMSTATE_UINT8(C3, FRDMK82FMCG),
		VMSTATE_UINT8(C4, FRDMK82FMCG),
		VMSTATE_UINT8(C5, FRDMK82FMCG),
		VMSTATE_UINT8(C6, FRDMK82FMCG),
		VMSTATE_UINT8(S, FRDMK82FMCG),
		VMSTATE_UINT8(SC, FRDMK82FMCG),
		VMSTATE_UINT8(ATCVH, FRDMK82FMCG),
		VMSTATE_UINT8(ATCVL, FRDMK82FMCG),
		VMSTATE_UINT8(C7, FRDMK82FMCG),
		VMSTATE_UINT8(C8, FRDMK82FMCG),

		VMSTATE_END_OF_LIST()
	}
};

static void frdmk82f_mcg_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmk82f_mcg_vmstate;
	dc->realize = frdmk82f_mcg_realize;
	rc->phases.enter = frdmk82f_mcg_reset_enter;

}

static const TypeInfo frdmk82f_mcg_info = {
	.name = TYPE_FRDMK82F_MCG,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMK82FMCG),
	.instance_init = frdmk82f_mcg_init,
	.class_init = frdmk82f_mcg_class_init,
};

static void frdmk82f_mcg_register_types(void) {
	type_register_static(&frdmk82f_mcg_info);
}

type_init(frdmk82f_mcg_register_types);

#define TYPE_FRDMK82F_RTC "frdmk82f-rtc"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMK82FRTC, FRDMK82F_RTC)

struct FRDMK82FRTC {
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
	uint32_t WAR;
	uint32_t RAR;
	/* base */
	uint32_t base;

};
#define FRDMK82F_RTC_SIZE				0x808

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
	FIELD(RTC_CR, WPS, 4, 1)
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
REG32(RTC_WAR, 0x800)
	FIELD(RTC_WAR, TSRW, 0, 1)
	FIELD(RTC_WAR, TPRW, 1, 1)
	FIELD(RTC_WAR, TARW, 2, 1)
	FIELD(RTC_WAR, TCRW, 3, 1)
	FIELD(RTC_WAR, CRW, 4, 1)
	FIELD(RTC_WAR, SRW, 5, 1)
	FIELD(RTC_WAR, LRW, 6, 1)
	FIELD(RTC_WAR, IERW, 7, 1)
REG32(RTC_RAR, 0x804)
	FIELD(RTC_RAR, TSRR, 0, 1)
	FIELD(RTC_RAR, TPRR, 1, 1)
	FIELD(RTC_RAR, TARR, 2, 1)
	FIELD(RTC_RAR, TCRR, 3, 1)
	FIELD(RTC_RAR, CRR, 4, 1)
	FIELD(RTC_RAR, SRR, 5, 1)
	FIELD(RTC_RAR, LRR, 6, 1)
	FIELD(RTC_RAR, IERR, 7, 1)


static void frdmk82f_rtc_register_reset(FRDMK82FRTC *t) {
	t->TSR = 0x0;
	t->TPR = 0x0;
	t->TAR = 0x0;
	t->TCR = 0x0;
	t->CR = 0x0;
	t->SR = 0x1;
	t->LR = 0xff;
	t->IER = 0x7;
	t->WAR = 0xff;
	t->RAR = 0xff;

}

static uint64_t frdmk82f_rtc_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMK82FRTC *t = FRDMK82F_RTC(opaque);
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
		case A_RTC_WAR:
			ret = t->WAR;
			break;
		case A_RTC_RAR:
			ret = t->RAR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK82F RTC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmk82f_rtc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMK82FRTC *t = FRDMK82F_RTC(opaque);


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
		case A_RTC_WAR ... A_RTC_WAR + 3:
			value = value << ((offset - A_RTC_WAR) << 3);
			offset = A_RTC_WAR;
			break;
		case A_RTC_RAR ... A_RTC_RAR + 3:
			value = value << ((offset - A_RTC_RAR) << 3);
			offset = A_RTC_RAR;
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
		case A_RTC_WAR:
			t->WAR = value;
			break;
		case A_RTC_RAR:
			t->RAR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK82F RTC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmk82f_rtc_ops = {
	.read = frdmk82f_rtc_read,
	.write = frdmk82f_rtc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmk82f_rtc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMK82FRTC *t = FRDMK82F_RTC(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmk82f_rtc_ops, t, "frdmk82f-rtc", FRDMK82F_RTC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 2; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void frdmk82f_rtc_realize(DeviceState *dev, Error **errp) {
	return;

}

static void frdmk82f_rtc_reset_enter(Object *obj, ResetType type) {
	FRDMK82FRTC *t = FRDMK82F_RTC(obj);
	frdmk82f_rtc_register_reset(t);
}

static const VMStateDescription frdmk82f_rtc_vmstate = {
	.name = "frdmk82f-rtc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(TSR, FRDMK82FRTC),
		VMSTATE_UINT32(TPR, FRDMK82FRTC),
		VMSTATE_UINT32(TAR, FRDMK82FRTC),
		VMSTATE_UINT32(TCR, FRDMK82FRTC),
		VMSTATE_UINT32(CR, FRDMK82FRTC),
		VMSTATE_UINT32(SR, FRDMK82FRTC),
		VMSTATE_UINT32(LR, FRDMK82FRTC),
		VMSTATE_UINT32(IER, FRDMK82FRTC),
		VMSTATE_UINT32(WAR, FRDMK82FRTC),
		VMSTATE_UINT32(RAR, FRDMK82FRTC),

		VMSTATE_END_OF_LIST()
	}
};

static void frdmk82f_rtc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmk82f_rtc_vmstate;
	dc->realize = frdmk82f_rtc_realize;
	rc->phases.enter = frdmk82f_rtc_reset_enter;

}

static const TypeInfo frdmk82f_rtc_info = {
	.name = TYPE_FRDMK82F_RTC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMK82FRTC),
	.instance_init = frdmk82f_rtc_init,
	.class_init = frdmk82f_rtc_class_init,
};

static void frdmk82f_rtc_register_types(void) {
	type_register_static(&frdmk82f_rtc_info);
}

type_init(frdmk82f_rtc_register_types);

#define TYPE_FRDMK82F_GPIO "frdmk82f-gpio"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMK82FGPIO, FRDMK82F_GPIO)

struct FRDMK82FGPIO {
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
#define FRDMK82F_GPIO_SIZE				0x18

REG32(GPIO_PDOR, 0x0)
	FIELD(GPIO_PDOR, PDO0, 0, 1)
	FIELD(GPIO_PDOR, PDO1, 1, 1)
	FIELD(GPIO_PDOR, PDO2, 2, 1)
	FIELD(GPIO_PDOR, PDO3, 3, 1)
	FIELD(GPIO_PDOR, PDO4, 4, 1)
	FIELD(GPIO_PDOR, PDO5, 5, 1)
	FIELD(GPIO_PDOR, PDO6, 6, 1)
	FIELD(GPIO_PDOR, PDO7, 7, 1)
	FIELD(GPIO_PDOR, PDO8, 8, 1)
	FIELD(GPIO_PDOR, PDO9, 9, 1)
	FIELD(GPIO_PDOR, PDO10, 10, 1)
	FIELD(GPIO_PDOR, PDO11, 11, 1)
	FIELD(GPIO_PDOR, PDO12, 12, 1)
	FIELD(GPIO_PDOR, PDO13, 13, 1)
	FIELD(GPIO_PDOR, PDO14, 14, 1)
	FIELD(GPIO_PDOR, PDO15, 15, 1)
	FIELD(GPIO_PDOR, PDO16, 16, 1)
	FIELD(GPIO_PDOR, PDO17, 17, 1)
	FIELD(GPIO_PDOR, PDO18, 18, 1)
	FIELD(GPIO_PDOR, PDO19, 19, 1)
	FIELD(GPIO_PDOR, PDO20, 20, 1)
	FIELD(GPIO_PDOR, PDO21, 21, 1)
	FIELD(GPIO_PDOR, PDO22, 22, 1)
	FIELD(GPIO_PDOR, PDO23, 23, 1)
	FIELD(GPIO_PDOR, PDO24, 24, 1)
	FIELD(GPIO_PDOR, PDO25, 25, 1)
	FIELD(GPIO_PDOR, PDO26, 26, 1)
	FIELD(GPIO_PDOR, PDO27, 27, 1)
	FIELD(GPIO_PDOR, PDO28, 28, 1)
	FIELD(GPIO_PDOR, PDO29, 29, 1)
	FIELD(GPIO_PDOR, PDO30, 30, 1)
	FIELD(GPIO_PDOR, PDO31, 31, 1)
REG32(GPIO_PSOR, 0x4)
	FIELD(GPIO_PSOR, PTSO0, 0, 1)
	FIELD(GPIO_PSOR, PTSO1, 1, 1)
	FIELD(GPIO_PSOR, PTSO2, 2, 1)
	FIELD(GPIO_PSOR, PTSO3, 3, 1)
	FIELD(GPIO_PSOR, PTSO4, 4, 1)
	FIELD(GPIO_PSOR, PTSO5, 5, 1)
	FIELD(GPIO_PSOR, PTSO6, 6, 1)
	FIELD(GPIO_PSOR, PTSO7, 7, 1)
	FIELD(GPIO_PSOR, PTSO8, 8, 1)
	FIELD(GPIO_PSOR, PTSO9, 9, 1)
	FIELD(GPIO_PSOR, PTSO10, 10, 1)
	FIELD(GPIO_PSOR, PTSO11, 11, 1)
	FIELD(GPIO_PSOR, PTSO12, 12, 1)
	FIELD(GPIO_PSOR, PTSO13, 13, 1)
	FIELD(GPIO_PSOR, PTSO14, 14, 1)
	FIELD(GPIO_PSOR, PTSO15, 15, 1)
	FIELD(GPIO_PSOR, PTSO16, 16, 1)
	FIELD(GPIO_PSOR, PTSO17, 17, 1)
	FIELD(GPIO_PSOR, PTSO18, 18, 1)
	FIELD(GPIO_PSOR, PTSO19, 19, 1)
	FIELD(GPIO_PSOR, PTSO20, 20, 1)
	FIELD(GPIO_PSOR, PTSO21, 21, 1)
	FIELD(GPIO_PSOR, PTSO22, 22, 1)
	FIELD(GPIO_PSOR, PTSO23, 23, 1)
	FIELD(GPIO_PSOR, PTSO24, 24, 1)
	FIELD(GPIO_PSOR, PTSO25, 25, 1)
	FIELD(GPIO_PSOR, PTSO26, 26, 1)
	FIELD(GPIO_PSOR, PTSO27, 27, 1)
	FIELD(GPIO_PSOR, PTSO28, 28, 1)
	FIELD(GPIO_PSOR, PTSO29, 29, 1)
	FIELD(GPIO_PSOR, PTSO30, 30, 1)
	FIELD(GPIO_PSOR, PTSO31, 31, 1)
REG32(GPIO_PCOR, 0x8)
	FIELD(GPIO_PCOR, PTCO0, 0, 1)
	FIELD(GPIO_PCOR, PTCO1, 1, 1)
	FIELD(GPIO_PCOR, PTCO2, 2, 1)
	FIELD(GPIO_PCOR, PTCO3, 3, 1)
	FIELD(GPIO_PCOR, PTCO4, 4, 1)
	FIELD(GPIO_PCOR, PTCO5, 5, 1)
	FIELD(GPIO_PCOR, PTCO6, 6, 1)
	FIELD(GPIO_PCOR, PTCO7, 7, 1)
	FIELD(GPIO_PCOR, PTCO8, 8, 1)
	FIELD(GPIO_PCOR, PTCO9, 9, 1)
	FIELD(GPIO_PCOR, PTCO10, 10, 1)
	FIELD(GPIO_PCOR, PTCO11, 11, 1)
	FIELD(GPIO_PCOR, PTCO12, 12, 1)
	FIELD(GPIO_PCOR, PTCO13, 13, 1)
	FIELD(GPIO_PCOR, PTCO14, 14, 1)
	FIELD(GPIO_PCOR, PTCO15, 15, 1)
	FIELD(GPIO_PCOR, PTCO16, 16, 1)
	FIELD(GPIO_PCOR, PTCO17, 17, 1)
	FIELD(GPIO_PCOR, PTCO18, 18, 1)
	FIELD(GPIO_PCOR, PTCO19, 19, 1)
	FIELD(GPIO_PCOR, PTCO20, 20, 1)
	FIELD(GPIO_PCOR, PTCO21, 21, 1)
	FIELD(GPIO_PCOR, PTCO22, 22, 1)
	FIELD(GPIO_PCOR, PTCO23, 23, 1)
	FIELD(GPIO_PCOR, PTCO24, 24, 1)
	FIELD(GPIO_PCOR, PTCO25, 25, 1)
	FIELD(GPIO_PCOR, PTCO26, 26, 1)
	FIELD(GPIO_PCOR, PTCO27, 27, 1)
	FIELD(GPIO_PCOR, PTCO28, 28, 1)
	FIELD(GPIO_PCOR, PTCO29, 29, 1)
	FIELD(GPIO_PCOR, PTCO30, 30, 1)
	FIELD(GPIO_PCOR, PTCO31, 31, 1)
REG32(GPIO_PTOR, 0xc)
	FIELD(GPIO_PTOR, PTTO0, 0, 1)
	FIELD(GPIO_PTOR, PTTO1, 1, 1)
	FIELD(GPIO_PTOR, PTTO2, 2, 1)
	FIELD(GPIO_PTOR, PTTO3, 3, 1)
	FIELD(GPIO_PTOR, PTTO4, 4, 1)
	FIELD(GPIO_PTOR, PTTO5, 5, 1)
	FIELD(GPIO_PTOR, PTTO6, 6, 1)
	FIELD(GPIO_PTOR, PTTO7, 7, 1)
	FIELD(GPIO_PTOR, PTTO8, 8, 1)
	FIELD(GPIO_PTOR, PTTO9, 9, 1)
	FIELD(GPIO_PTOR, PTTO10, 10, 1)
	FIELD(GPIO_PTOR, PTTO11, 11, 1)
	FIELD(GPIO_PTOR, PTTO12, 12, 1)
	FIELD(GPIO_PTOR, PTTO13, 13, 1)
	FIELD(GPIO_PTOR, PTTO14, 14, 1)
	FIELD(GPIO_PTOR, PTTO15, 15, 1)
	FIELD(GPIO_PTOR, PTTO16, 16, 1)
	FIELD(GPIO_PTOR, PTTO17, 17, 1)
	FIELD(GPIO_PTOR, PTTO18, 18, 1)
	FIELD(GPIO_PTOR, PTTO19, 19, 1)
	FIELD(GPIO_PTOR, PTTO20, 20, 1)
	FIELD(GPIO_PTOR, PTTO21, 21, 1)
	FIELD(GPIO_PTOR, PTTO22, 22, 1)
	FIELD(GPIO_PTOR, PTTO23, 23, 1)
	FIELD(GPIO_PTOR, PTTO24, 24, 1)
	FIELD(GPIO_PTOR, PTTO25, 25, 1)
	FIELD(GPIO_PTOR, PTTO26, 26, 1)
	FIELD(GPIO_PTOR, PTTO27, 27, 1)
	FIELD(GPIO_PTOR, PTTO28, 28, 1)
	FIELD(GPIO_PTOR, PTTO29, 29, 1)
	FIELD(GPIO_PTOR, PTTO30, 30, 1)
	FIELD(GPIO_PTOR, PTTO31, 31, 1)
REG32(GPIO_PDIR, 0x10)
	FIELD(GPIO_PDIR, PDI0, 0, 1)
	FIELD(GPIO_PDIR, PDI1, 1, 1)
	FIELD(GPIO_PDIR, PDI2, 2, 1)
	FIELD(GPIO_PDIR, PDI3, 3, 1)
	FIELD(GPIO_PDIR, PDI4, 4, 1)
	FIELD(GPIO_PDIR, PDI5, 5, 1)
	FIELD(GPIO_PDIR, PDI6, 6, 1)
	FIELD(GPIO_PDIR, PDI7, 7, 1)
	FIELD(GPIO_PDIR, PDI8, 8, 1)
	FIELD(GPIO_PDIR, PDI9, 9, 1)
	FIELD(GPIO_PDIR, PDI10, 10, 1)
	FIELD(GPIO_PDIR, PDI11, 11, 1)
	FIELD(GPIO_PDIR, PDI12, 12, 1)
	FIELD(GPIO_PDIR, PDI13, 13, 1)
	FIELD(GPIO_PDIR, PDI14, 14, 1)
	FIELD(GPIO_PDIR, PDI15, 15, 1)
	FIELD(GPIO_PDIR, PDI16, 16, 1)
	FIELD(GPIO_PDIR, PDI17, 17, 1)
	FIELD(GPIO_PDIR, PDI18, 18, 1)
	FIELD(GPIO_PDIR, PDI19, 19, 1)
	FIELD(GPIO_PDIR, PDI20, 20, 1)
	FIELD(GPIO_PDIR, PDI21, 21, 1)
	FIELD(GPIO_PDIR, PDI22, 22, 1)
	FIELD(GPIO_PDIR, PDI23, 23, 1)
	FIELD(GPIO_PDIR, PDI24, 24, 1)
	FIELD(GPIO_PDIR, PDI25, 25, 1)
	FIELD(GPIO_PDIR, PDI26, 26, 1)
	FIELD(GPIO_PDIR, PDI27, 27, 1)
	FIELD(GPIO_PDIR, PDI28, 28, 1)
	FIELD(GPIO_PDIR, PDI29, 29, 1)
	FIELD(GPIO_PDIR, PDI30, 30, 1)
	FIELD(GPIO_PDIR, PDI31, 31, 1)
REG32(GPIO_PDDR, 0x14)
	FIELD(GPIO_PDDR, PDD0, 0, 1)
	FIELD(GPIO_PDDR, PDD1, 1, 1)
	FIELD(GPIO_PDDR, PDD2, 2, 1)
	FIELD(GPIO_PDDR, PDD3, 3, 1)
	FIELD(GPIO_PDDR, PDD4, 4, 1)
	FIELD(GPIO_PDDR, PDD5, 5, 1)
	FIELD(GPIO_PDDR, PDD6, 6, 1)
	FIELD(GPIO_PDDR, PDD7, 7, 1)
	FIELD(GPIO_PDDR, PDD8, 8, 1)
	FIELD(GPIO_PDDR, PDD9, 9, 1)
	FIELD(GPIO_PDDR, PDD10, 10, 1)
	FIELD(GPIO_PDDR, PDD11, 11, 1)
	FIELD(GPIO_PDDR, PDD12, 12, 1)
	FIELD(GPIO_PDDR, PDD13, 13, 1)
	FIELD(GPIO_PDDR, PDD14, 14, 1)
	FIELD(GPIO_PDDR, PDD15, 15, 1)
	FIELD(GPIO_PDDR, PDD16, 16, 1)
	FIELD(GPIO_PDDR, PDD17, 17, 1)
	FIELD(GPIO_PDDR, PDD18, 18, 1)
	FIELD(GPIO_PDDR, PDD19, 19, 1)
	FIELD(GPIO_PDDR, PDD20, 20, 1)
	FIELD(GPIO_PDDR, PDD21, 21, 1)
	FIELD(GPIO_PDDR, PDD22, 22, 1)
	FIELD(GPIO_PDDR, PDD23, 23, 1)
	FIELD(GPIO_PDDR, PDD24, 24, 1)
	FIELD(GPIO_PDDR, PDD25, 25, 1)
	FIELD(GPIO_PDDR, PDD26, 26, 1)
	FIELD(GPIO_PDDR, PDD27, 27, 1)
	FIELD(GPIO_PDDR, PDD28, 28, 1)
	FIELD(GPIO_PDDR, PDD29, 29, 1)
	FIELD(GPIO_PDDR, PDD30, 30, 1)
	FIELD(GPIO_PDDR, PDD31, 31, 1)


static void frdmk82f_gpio_register_reset(FRDMK82FGPIO *t) {
	t->PDOR = 0x0;
	t->PSOR = 0x0;
	t->PCOR = 0x0;
	t->PTOR = 0x0;
	t->PDIR = 0x0;
	t->PDDR = 0x0;

}

static uint64_t frdmk82f_gpio_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMK82FGPIO *t = FRDMK82F_GPIO(opaque);
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
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK82F GPIO read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmk82f_gpio_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMK82FGPIO *t = FRDMK82F_GPIO(opaque);


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
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK82F GPIO write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmk82f_gpio_ops = {
	.read = frdmk82f_gpio_read,
	.write = frdmk82f_gpio_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmk82f_gpio_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMK82FGPIO *t = FRDMK82F_GPIO(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmk82f_gpio_ops, t, "frdmk82f-gpio", FRDMK82F_GPIO_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void frdmk82f_gpio_realize(DeviceState *dev, Error **errp) {
	return;

}

static void frdmk82f_gpio_reset_enter(Object *obj, ResetType type) {
	FRDMK82FGPIO *t = FRDMK82F_GPIO(obj);
	frdmk82f_gpio_register_reset(t);
}

static const VMStateDescription frdmk82f_gpio_vmstate = {
	.name = "frdmk82f-gpio",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(PDOR, FRDMK82FGPIO),
		VMSTATE_UINT32(PSOR, FRDMK82FGPIO),
		VMSTATE_UINT32(PCOR, FRDMK82FGPIO),
		VMSTATE_UINT32(PTOR, FRDMK82FGPIO),
		VMSTATE_UINT32(PDIR, FRDMK82FGPIO),
		VMSTATE_UINT32(PDDR, FRDMK82FGPIO),

		VMSTATE_END_OF_LIST()
	}
};

static void frdmk82f_gpio_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmk82f_gpio_vmstate;
	dc->realize = frdmk82f_gpio_realize;
	rc->phases.enter = frdmk82f_gpio_reset_enter;

}

static const TypeInfo frdmk82f_gpio_info = {
	.name = TYPE_FRDMK82F_GPIO,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMK82FGPIO),
	.instance_init = frdmk82f_gpio_init,
	.class_init = frdmk82f_gpio_class_init,
};

static void frdmk82f_gpio_register_types(void) {
	type_register_static(&frdmk82f_gpio_info);
}

type_init(frdmk82f_gpio_register_types);

#define TYPE_FRDMK82F_PORT "frdmk82f-port"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMK82FPORT, FRDMK82F_PORT)

struct FRDMK82FPORT {
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
#define FRDMK82F_PORT_SIZE				0xa4

REG32(PORT_PCR0, 0x0)
	FIELD(PORT_PCR0, PS, 0, 1)
	FIELD(PORT_PCR0, PE, 1, 1)
	FIELD(PORT_PCR0, SRE, 2, 1)
	FIELD(PORT_PCR0, PFE, 4, 1)
	FIELD(PORT_PCR0, ODE, 5, 1)
	FIELD(PORT_PCR0, DSE, 6, 1)
	FIELD(PORT_PCR0, MUX, 8, 3)
	FIELD(PORT_PCR0, LK, 15, 1)
	FIELD(PORT_PCR0, IRQC, 16, 4)
	FIELD(PORT_PCR0, ISF, 24, 1)
REG32(PORT_PCR1, 0x4)
	FIELD(PORT_PCR1, PS, 0, 1)
	FIELD(PORT_PCR1, PE, 1, 1)
	FIELD(PORT_PCR1, SRE, 2, 1)
	FIELD(PORT_PCR1, PFE, 4, 1)
	FIELD(PORT_PCR1, ODE, 5, 1)
	FIELD(PORT_PCR1, DSE, 6, 1)
	FIELD(PORT_PCR1, MUX, 8, 3)
	FIELD(PORT_PCR1, LK, 15, 1)
	FIELD(PORT_PCR1, IRQC, 16, 4)
	FIELD(PORT_PCR1, ISF, 24, 1)
REG32(PORT_PCR2, 0x8)
	FIELD(PORT_PCR2, PS, 0, 1)
	FIELD(PORT_PCR2, PE, 1, 1)
	FIELD(PORT_PCR2, SRE, 2, 1)
	FIELD(PORT_PCR2, PFE, 4, 1)
	FIELD(PORT_PCR2, ODE, 5, 1)
	FIELD(PORT_PCR2, DSE, 6, 1)
	FIELD(PORT_PCR2, MUX, 8, 3)
	FIELD(PORT_PCR2, LK, 15, 1)
	FIELD(PORT_PCR2, IRQC, 16, 4)
	FIELD(PORT_PCR2, ISF, 24, 1)
REG32(PORT_PCR3, 0xc)
	FIELD(PORT_PCR3, PS, 0, 1)
	FIELD(PORT_PCR3, PE, 1, 1)
	FIELD(PORT_PCR3, SRE, 2, 1)
	FIELD(PORT_PCR3, PFE, 4, 1)
	FIELD(PORT_PCR3, ODE, 5, 1)
	FIELD(PORT_PCR3, DSE, 6, 1)
	FIELD(PORT_PCR3, MUX, 8, 3)
	FIELD(PORT_PCR3, LK, 15, 1)
	FIELD(PORT_PCR3, IRQC, 16, 4)
	FIELD(PORT_PCR3, ISF, 24, 1)
REG32(PORT_PCR4, 0x10)
	FIELD(PORT_PCR4, PS, 0, 1)
	FIELD(PORT_PCR4, PE, 1, 1)
	FIELD(PORT_PCR4, SRE, 2, 1)
	FIELD(PORT_PCR4, PFE, 4, 1)
	FIELD(PORT_PCR4, ODE, 5, 1)
	FIELD(PORT_PCR4, DSE, 6, 1)
	FIELD(PORT_PCR4, MUX, 8, 3)
	FIELD(PORT_PCR4, LK, 15, 1)
	FIELD(PORT_PCR4, IRQC, 16, 4)
	FIELD(PORT_PCR4, ISF, 24, 1)
REG32(PORT_PCR5, 0x14)
	FIELD(PORT_PCR5, PS, 0, 1)
	FIELD(PORT_PCR5, PE, 1, 1)
	FIELD(PORT_PCR5, SRE, 2, 1)
	FIELD(PORT_PCR5, PFE, 4, 1)
	FIELD(PORT_PCR5, ODE, 5, 1)
	FIELD(PORT_PCR5, DSE, 6, 1)
	FIELD(PORT_PCR5, MUX, 8, 3)
	FIELD(PORT_PCR5, LK, 15, 1)
	FIELD(PORT_PCR5, IRQC, 16, 4)
	FIELD(PORT_PCR5, ISF, 24, 1)
REG32(PORT_PCR6, 0x18)
	FIELD(PORT_PCR6, PS, 0, 1)
	FIELD(PORT_PCR6, PE, 1, 1)
	FIELD(PORT_PCR6, SRE, 2, 1)
	FIELD(PORT_PCR6, PFE, 4, 1)
	FIELD(PORT_PCR6, ODE, 5, 1)
	FIELD(PORT_PCR6, DSE, 6, 1)
	FIELD(PORT_PCR6, MUX, 8, 3)
	FIELD(PORT_PCR6, LK, 15, 1)
	FIELD(PORT_PCR6, IRQC, 16, 4)
	FIELD(PORT_PCR6, ISF, 24, 1)
REG32(PORT_PCR7, 0x1c)
	FIELD(PORT_PCR7, PS, 0, 1)
	FIELD(PORT_PCR7, PE, 1, 1)
	FIELD(PORT_PCR7, SRE, 2, 1)
	FIELD(PORT_PCR7, PFE, 4, 1)
	FIELD(PORT_PCR7, ODE, 5, 1)
	FIELD(PORT_PCR7, DSE, 6, 1)
	FIELD(PORT_PCR7, MUX, 8, 3)
	FIELD(PORT_PCR7, LK, 15, 1)
	FIELD(PORT_PCR7, IRQC, 16, 4)
	FIELD(PORT_PCR7, ISF, 24, 1)
REG32(PORT_PCR8, 0x20)
	FIELD(PORT_PCR8, PS, 0, 1)
	FIELD(PORT_PCR8, PE, 1, 1)
	FIELD(PORT_PCR8, SRE, 2, 1)
	FIELD(PORT_PCR8, PFE, 4, 1)
	FIELD(PORT_PCR8, ODE, 5, 1)
	FIELD(PORT_PCR8, DSE, 6, 1)
	FIELD(PORT_PCR8, MUX, 8, 3)
	FIELD(PORT_PCR8, LK, 15, 1)
	FIELD(PORT_PCR8, IRQC, 16, 4)
	FIELD(PORT_PCR8, ISF, 24, 1)
REG32(PORT_PCR9, 0x24)
	FIELD(PORT_PCR9, PS, 0, 1)
	FIELD(PORT_PCR9, PE, 1, 1)
	FIELD(PORT_PCR9, SRE, 2, 1)
	FIELD(PORT_PCR9, PFE, 4, 1)
	FIELD(PORT_PCR9, ODE, 5, 1)
	FIELD(PORT_PCR9, DSE, 6, 1)
	FIELD(PORT_PCR9, MUX, 8, 3)
	FIELD(PORT_PCR9, LK, 15, 1)
	FIELD(PORT_PCR9, IRQC, 16, 4)
	FIELD(PORT_PCR9, ISF, 24, 1)
REG32(PORT_PCR10, 0x28)
	FIELD(PORT_PCR10, PS, 0, 1)
	FIELD(PORT_PCR10, PE, 1, 1)
	FIELD(PORT_PCR10, SRE, 2, 1)
	FIELD(PORT_PCR10, PFE, 4, 1)
	FIELD(PORT_PCR10, ODE, 5, 1)
	FIELD(PORT_PCR10, DSE, 6, 1)
	FIELD(PORT_PCR10, MUX, 8, 3)
	FIELD(PORT_PCR10, LK, 15, 1)
	FIELD(PORT_PCR10, IRQC, 16, 4)
	FIELD(PORT_PCR10, ISF, 24, 1)
REG32(PORT_PCR11, 0x2c)
	FIELD(PORT_PCR11, PS, 0, 1)
	FIELD(PORT_PCR11, PE, 1, 1)
	FIELD(PORT_PCR11, SRE, 2, 1)
	FIELD(PORT_PCR11, PFE, 4, 1)
	FIELD(PORT_PCR11, ODE, 5, 1)
	FIELD(PORT_PCR11, DSE, 6, 1)
	FIELD(PORT_PCR11, MUX, 8, 3)
	FIELD(PORT_PCR11, LK, 15, 1)
	FIELD(PORT_PCR11, IRQC, 16, 4)
	FIELD(PORT_PCR11, ISF, 24, 1)
REG32(PORT_PCR12, 0x30)
	FIELD(PORT_PCR12, PS, 0, 1)
	FIELD(PORT_PCR12, PE, 1, 1)
	FIELD(PORT_PCR12, SRE, 2, 1)
	FIELD(PORT_PCR12, PFE, 4, 1)
	FIELD(PORT_PCR12, ODE, 5, 1)
	FIELD(PORT_PCR12, DSE, 6, 1)
	FIELD(PORT_PCR12, MUX, 8, 3)
	FIELD(PORT_PCR12, LK, 15, 1)
	FIELD(PORT_PCR12, IRQC, 16, 4)
	FIELD(PORT_PCR12, ISF, 24, 1)
REG32(PORT_PCR13, 0x34)
	FIELD(PORT_PCR13, PS, 0, 1)
	FIELD(PORT_PCR13, PE, 1, 1)
	FIELD(PORT_PCR13, SRE, 2, 1)
	FIELD(PORT_PCR13, PFE, 4, 1)
	FIELD(PORT_PCR13, ODE, 5, 1)
	FIELD(PORT_PCR13, DSE, 6, 1)
	FIELD(PORT_PCR13, MUX, 8, 3)
	FIELD(PORT_PCR13, LK, 15, 1)
	FIELD(PORT_PCR13, IRQC, 16, 4)
	FIELD(PORT_PCR13, ISF, 24, 1)
REG32(PORT_PCR14, 0x38)
	FIELD(PORT_PCR14, PS, 0, 1)
	FIELD(PORT_PCR14, PE, 1, 1)
	FIELD(PORT_PCR14, SRE, 2, 1)
	FIELD(PORT_PCR14, PFE, 4, 1)
	FIELD(PORT_PCR14, ODE, 5, 1)
	FIELD(PORT_PCR14, DSE, 6, 1)
	FIELD(PORT_PCR14, MUX, 8, 3)
	FIELD(PORT_PCR14, LK, 15, 1)
	FIELD(PORT_PCR14, IRQC, 16, 4)
	FIELD(PORT_PCR14, ISF, 24, 1)
REG32(PORT_PCR15, 0x3c)
	FIELD(PORT_PCR15, PS, 0, 1)
	FIELD(PORT_PCR15, PE, 1, 1)
	FIELD(PORT_PCR15, SRE, 2, 1)
	FIELD(PORT_PCR15, PFE, 4, 1)
	FIELD(PORT_PCR15, ODE, 5, 1)
	FIELD(PORT_PCR15, DSE, 6, 1)
	FIELD(PORT_PCR15, MUX, 8, 3)
	FIELD(PORT_PCR15, LK, 15, 1)
	FIELD(PORT_PCR15, IRQC, 16, 4)
	FIELD(PORT_PCR15, ISF, 24, 1)
REG32(PORT_PCR16, 0x40)
	FIELD(PORT_PCR16, PS, 0, 1)
	FIELD(PORT_PCR16, PE, 1, 1)
	FIELD(PORT_PCR16, SRE, 2, 1)
	FIELD(PORT_PCR16, PFE, 4, 1)
	FIELD(PORT_PCR16, ODE, 5, 1)
	FIELD(PORT_PCR16, DSE, 6, 1)
	FIELD(PORT_PCR16, MUX, 8, 3)
	FIELD(PORT_PCR16, LK, 15, 1)
	FIELD(PORT_PCR16, IRQC, 16, 4)
	FIELD(PORT_PCR16, ISF, 24, 1)
REG32(PORT_PCR17, 0x44)
	FIELD(PORT_PCR17, PS, 0, 1)
	FIELD(PORT_PCR17, PE, 1, 1)
	FIELD(PORT_PCR17, SRE, 2, 1)
	FIELD(PORT_PCR17, PFE, 4, 1)
	FIELD(PORT_PCR17, ODE, 5, 1)
	FIELD(PORT_PCR17, DSE, 6, 1)
	FIELD(PORT_PCR17, MUX, 8, 3)
	FIELD(PORT_PCR17, LK, 15, 1)
	FIELD(PORT_PCR17, IRQC, 16, 4)
	FIELD(PORT_PCR17, ISF, 24, 1)
REG32(PORT_PCR18, 0x48)
	FIELD(PORT_PCR18, PS, 0, 1)
	FIELD(PORT_PCR18, PE, 1, 1)
	FIELD(PORT_PCR18, SRE, 2, 1)
	FIELD(PORT_PCR18, PFE, 4, 1)
	FIELD(PORT_PCR18, ODE, 5, 1)
	FIELD(PORT_PCR18, DSE, 6, 1)
	FIELD(PORT_PCR18, MUX, 8, 3)
	FIELD(PORT_PCR18, LK, 15, 1)
	FIELD(PORT_PCR18, IRQC, 16, 4)
	FIELD(PORT_PCR18, ISF, 24, 1)
REG32(PORT_PCR19, 0x4c)
	FIELD(PORT_PCR19, PS, 0, 1)
	FIELD(PORT_PCR19, PE, 1, 1)
	FIELD(PORT_PCR19, SRE, 2, 1)
	FIELD(PORT_PCR19, PFE, 4, 1)
	FIELD(PORT_PCR19, ODE, 5, 1)
	FIELD(PORT_PCR19, DSE, 6, 1)
	FIELD(PORT_PCR19, MUX, 8, 3)
	FIELD(PORT_PCR19, LK, 15, 1)
	FIELD(PORT_PCR19, IRQC, 16, 4)
	FIELD(PORT_PCR19, ISF, 24, 1)
REG32(PORT_PCR20, 0x50)
	FIELD(PORT_PCR20, PS, 0, 1)
	FIELD(PORT_PCR20, PE, 1, 1)
	FIELD(PORT_PCR20, SRE, 2, 1)
	FIELD(PORT_PCR20, PFE, 4, 1)
	FIELD(PORT_PCR20, ODE, 5, 1)
	FIELD(PORT_PCR20, DSE, 6, 1)
	FIELD(PORT_PCR20, MUX, 8, 3)
	FIELD(PORT_PCR20, LK, 15, 1)
	FIELD(PORT_PCR20, IRQC, 16, 4)
	FIELD(PORT_PCR20, ISF, 24, 1)
REG32(PORT_PCR21, 0x54)
	FIELD(PORT_PCR21, PS, 0, 1)
	FIELD(PORT_PCR21, PE, 1, 1)
	FIELD(PORT_PCR21, SRE, 2, 1)
	FIELD(PORT_PCR21, PFE, 4, 1)
	FIELD(PORT_PCR21, ODE, 5, 1)
	FIELD(PORT_PCR21, DSE, 6, 1)
	FIELD(PORT_PCR21, MUX, 8, 3)
	FIELD(PORT_PCR21, LK, 15, 1)
	FIELD(PORT_PCR21, IRQC, 16, 4)
	FIELD(PORT_PCR21, ISF, 24, 1)
REG32(PORT_PCR22, 0x58)
	FIELD(PORT_PCR22, PS, 0, 1)
	FIELD(PORT_PCR22, PE, 1, 1)
	FIELD(PORT_PCR22, SRE, 2, 1)
	FIELD(PORT_PCR22, PFE, 4, 1)
	FIELD(PORT_PCR22, ODE, 5, 1)
	FIELD(PORT_PCR22, DSE, 6, 1)
	FIELD(PORT_PCR22, MUX, 8, 3)
	FIELD(PORT_PCR22, LK, 15, 1)
	FIELD(PORT_PCR22, IRQC, 16, 4)
	FIELD(PORT_PCR22, ISF, 24, 1)
REG32(PORT_PCR23, 0x5c)
	FIELD(PORT_PCR23, PS, 0, 1)
	FIELD(PORT_PCR23, PE, 1, 1)
	FIELD(PORT_PCR23, SRE, 2, 1)
	FIELD(PORT_PCR23, PFE, 4, 1)
	FIELD(PORT_PCR23, ODE, 5, 1)
	FIELD(PORT_PCR23, DSE, 6, 1)
	FIELD(PORT_PCR23, MUX, 8, 3)
	FIELD(PORT_PCR23, LK, 15, 1)
	FIELD(PORT_PCR23, IRQC, 16, 4)
	FIELD(PORT_PCR23, ISF, 24, 1)
REG32(PORT_PCR24, 0x60)
	FIELD(PORT_PCR24, PS, 0, 1)
	FIELD(PORT_PCR24, PE, 1, 1)
	FIELD(PORT_PCR24, SRE, 2, 1)
	FIELD(PORT_PCR24, PFE, 4, 1)
	FIELD(PORT_PCR24, ODE, 5, 1)
	FIELD(PORT_PCR24, DSE, 6, 1)
	FIELD(PORT_PCR24, MUX, 8, 3)
	FIELD(PORT_PCR24, LK, 15, 1)
	FIELD(PORT_PCR24, IRQC, 16, 4)
	FIELD(PORT_PCR24, ISF, 24, 1)
REG32(PORT_PCR25, 0x64)
	FIELD(PORT_PCR25, PS, 0, 1)
	FIELD(PORT_PCR25, PE, 1, 1)
	FIELD(PORT_PCR25, SRE, 2, 1)
	FIELD(PORT_PCR25, PFE, 4, 1)
	FIELD(PORT_PCR25, ODE, 5, 1)
	FIELD(PORT_PCR25, DSE, 6, 1)
	FIELD(PORT_PCR25, MUX, 8, 3)
	FIELD(PORT_PCR25, LK, 15, 1)
	FIELD(PORT_PCR25, IRQC, 16, 4)
	FIELD(PORT_PCR25, ISF, 24, 1)
REG32(PORT_PCR26, 0x68)
	FIELD(PORT_PCR26, PS, 0, 1)
	FIELD(PORT_PCR26, PE, 1, 1)
	FIELD(PORT_PCR26, SRE, 2, 1)
	FIELD(PORT_PCR26, PFE, 4, 1)
	FIELD(PORT_PCR26, ODE, 5, 1)
	FIELD(PORT_PCR26, DSE, 6, 1)
	FIELD(PORT_PCR26, MUX, 8, 3)
	FIELD(PORT_PCR26, LK, 15, 1)
	FIELD(PORT_PCR26, IRQC, 16, 4)
	FIELD(PORT_PCR26, ISF, 24, 1)
REG32(PORT_PCR27, 0x6c)
	FIELD(PORT_PCR27, PS, 0, 1)
	FIELD(PORT_PCR27, PE, 1, 1)
	FIELD(PORT_PCR27, SRE, 2, 1)
	FIELD(PORT_PCR27, PFE, 4, 1)
	FIELD(PORT_PCR27, ODE, 5, 1)
	FIELD(PORT_PCR27, DSE, 6, 1)
	FIELD(PORT_PCR27, MUX, 8, 3)
	FIELD(PORT_PCR27, LK, 15, 1)
	FIELD(PORT_PCR27, IRQC, 16, 4)
	FIELD(PORT_PCR27, ISF, 24, 1)
REG32(PORT_PCR28, 0x70)
	FIELD(PORT_PCR28, PS, 0, 1)
	FIELD(PORT_PCR28, PE, 1, 1)
	FIELD(PORT_PCR28, SRE, 2, 1)
	FIELD(PORT_PCR28, PFE, 4, 1)
	FIELD(PORT_PCR28, ODE, 5, 1)
	FIELD(PORT_PCR28, DSE, 6, 1)
	FIELD(PORT_PCR28, MUX, 8, 3)
	FIELD(PORT_PCR28, LK, 15, 1)
	FIELD(PORT_PCR28, IRQC, 16, 4)
	FIELD(PORT_PCR28, ISF, 24, 1)
REG32(PORT_PCR29, 0x74)
	FIELD(PORT_PCR29, PS, 0, 1)
	FIELD(PORT_PCR29, PE, 1, 1)
	FIELD(PORT_PCR29, SRE, 2, 1)
	FIELD(PORT_PCR29, PFE, 4, 1)
	FIELD(PORT_PCR29, ODE, 5, 1)
	FIELD(PORT_PCR29, DSE, 6, 1)
	FIELD(PORT_PCR29, MUX, 8, 3)
	FIELD(PORT_PCR29, LK, 15, 1)
	FIELD(PORT_PCR29, IRQC, 16, 4)
	FIELD(PORT_PCR29, ISF, 24, 1)
REG32(PORT_PCR30, 0x78)
	FIELD(PORT_PCR30, PS, 0, 1)
	FIELD(PORT_PCR30, PE, 1, 1)
	FIELD(PORT_PCR30, SRE, 2, 1)
	FIELD(PORT_PCR30, PFE, 4, 1)
	FIELD(PORT_PCR30, ODE, 5, 1)
	FIELD(PORT_PCR30, DSE, 6, 1)
	FIELD(PORT_PCR30, MUX, 8, 3)
	FIELD(PORT_PCR30, LK, 15, 1)
	FIELD(PORT_PCR30, IRQC, 16, 4)
	FIELD(PORT_PCR30, ISF, 24, 1)
REG32(PORT_PCR31, 0x7c)
	FIELD(PORT_PCR31, PS, 0, 1)
	FIELD(PORT_PCR31, PE, 1, 1)
	FIELD(PORT_PCR31, SRE, 2, 1)
	FIELD(PORT_PCR31, PFE, 4, 1)
	FIELD(PORT_PCR31, ODE, 5, 1)
	FIELD(PORT_PCR31, DSE, 6, 1)
	FIELD(PORT_PCR31, MUX, 8, 3)
	FIELD(PORT_PCR31, LK, 15, 1)
	FIELD(PORT_PCR31, IRQC, 16, 4)
	FIELD(PORT_PCR31, ISF, 24, 1)
REG32(PORT_GPCLR, 0x80)
	FIELD(PORT_GPCLR, GPWD, 0, 16)
	FIELD(PORT_GPCLR, GPWE0, 16, 1)
	FIELD(PORT_GPCLR, GPWE1, 17, 1)
	FIELD(PORT_GPCLR, GPWE2, 18, 1)
	FIELD(PORT_GPCLR, GPWE3, 19, 1)
	FIELD(PORT_GPCLR, GPWE4, 20, 1)
	FIELD(PORT_GPCLR, GPWE5, 21, 1)
	FIELD(PORT_GPCLR, GPWE6, 22, 1)
	FIELD(PORT_GPCLR, GPWE7, 23, 1)
	FIELD(PORT_GPCLR, GPWE8, 24, 1)
	FIELD(PORT_GPCLR, GPWE9, 25, 1)
	FIELD(PORT_GPCLR, GPWE10, 26, 1)
	FIELD(PORT_GPCLR, GPWE11, 27, 1)
	FIELD(PORT_GPCLR, GPWE12, 28, 1)
	FIELD(PORT_GPCLR, GPWE13, 29, 1)
	FIELD(PORT_GPCLR, GPWE14, 30, 1)
	FIELD(PORT_GPCLR, GPWE15, 31, 1)
REG32(PORT_GPCHR, 0x84)
	FIELD(PORT_GPCHR, GPWD, 0, 16)
	FIELD(PORT_GPCHR, GPWE0, 16, 1)
	FIELD(PORT_GPCHR, GPWE1, 17, 1)
	FIELD(PORT_GPCHR, GPWE2, 18, 1)
	FIELD(PORT_GPCHR, GPWE3, 19, 1)
	FIELD(PORT_GPCHR, GPWE4, 20, 1)
	FIELD(PORT_GPCHR, GPWE5, 21, 1)
	FIELD(PORT_GPCHR, GPWE6, 22, 1)
	FIELD(PORT_GPCHR, GPWE7, 23, 1)
	FIELD(PORT_GPCHR, GPWE8, 24, 1)
	FIELD(PORT_GPCHR, GPWE9, 25, 1)
	FIELD(PORT_GPCHR, GPWE10, 26, 1)
	FIELD(PORT_GPCHR, GPWE11, 27, 1)
	FIELD(PORT_GPCHR, GPWE12, 28, 1)
	FIELD(PORT_GPCHR, GPWE13, 29, 1)
	FIELD(PORT_GPCHR, GPWE14, 30, 1)
	FIELD(PORT_GPCHR, GPWE15, 31, 1)
REG32(PORT_ISFR, 0xa0)
	FIELD(PORT_ISFR, ISF0, 0, 1)
	FIELD(PORT_ISFR, ISF1, 1, 1)
	FIELD(PORT_ISFR, ISF2, 2, 1)
	FIELD(PORT_ISFR, ISF3, 3, 1)
	FIELD(PORT_ISFR, ISF4, 4, 1)
	FIELD(PORT_ISFR, ISF5, 5, 1)
	FIELD(PORT_ISFR, ISF6, 6, 1)
	FIELD(PORT_ISFR, ISF7, 7, 1)
	FIELD(PORT_ISFR, ISF8, 8, 1)
	FIELD(PORT_ISFR, ISF9, 9, 1)
	FIELD(PORT_ISFR, ISF10, 10, 1)
	FIELD(PORT_ISFR, ISF11, 11, 1)
	FIELD(PORT_ISFR, ISF12, 12, 1)
	FIELD(PORT_ISFR, ISF13, 13, 1)
	FIELD(PORT_ISFR, ISF14, 14, 1)
	FIELD(PORT_ISFR, ISF15, 15, 1)
	FIELD(PORT_ISFR, ISF16, 16, 1)
	FIELD(PORT_ISFR, ISF17, 17, 1)
	FIELD(PORT_ISFR, ISF18, 18, 1)
	FIELD(PORT_ISFR, ISF19, 19, 1)
	FIELD(PORT_ISFR, ISF20, 20, 1)
	FIELD(PORT_ISFR, ISF21, 21, 1)
	FIELD(PORT_ISFR, ISF22, 22, 1)
	FIELD(PORT_ISFR, ISF23, 23, 1)
	FIELD(PORT_ISFR, ISF24, 24, 1)
	FIELD(PORT_ISFR, ISF25, 25, 1)
	FIELD(PORT_ISFR, ISF26, 26, 1)
	FIELD(PORT_ISFR, ISF27, 27, 1)
	FIELD(PORT_ISFR, ISF28, 28, 1)
	FIELD(PORT_ISFR, ISF29, 29, 1)
	FIELD(PORT_ISFR, ISF30, 30, 1)
	FIELD(PORT_ISFR, ISF31, 31, 1)


static int frdmk82f_port_can_receive(void *opaque) {
	return 1;
}

static void frdmk82f_port_receive(void *opaque, const uint8_t *buf, int size) {
	FRDMK82FPORT *t = FRDMK82F_PORT(opaque);

	(void)t;


}

static void frdmk82f_port_register_reset(FRDMK82FPORT *t) {
	t->PCR0 = 0x702;
	t->PCR1 = 0x703;
	t->PCR2 = 0x703;
	t->PCR3 = 0x703;
	t->PCR4 = 0x703;
	t->PCR5 = 0x1;
	t->PCR6 = 0x0;
	t->PCR7 = 0x0;
	t->PCR8 = 0x0;
	t->PCR9 = 0x0;
	t->PCR10 = 0x0;
	t->PCR11 = 0x0;
	t->PCR12 = 0x0;
	t->PCR13 = 0x0;
	t->PCR14 = 0x0;
	t->PCR15 = 0x0;
	t->PCR16 = 0x0;
	t->PCR17 = 0x0;
	t->PCR18 = 0x0;
	t->PCR19 = 0x0;
	t->PCR20 = 0x0;
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

static uint64_t frdmk82f_port_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMK82FPORT *t = FRDMK82F_PORT(opaque);
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
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK82F PORT read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmk82f_port_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMK82FPORT *t = FRDMK82F_PORT(opaque);


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
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK82F PORT write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmk82f_port_ops = {
	.read = frdmk82f_port_read,
	.write = frdmk82f_port_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmk82f_port_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMK82FPORT *t = FRDMK82F_PORT(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmk82f_port_ops, t, "frdmk82f-port", FRDMK82F_PORT_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void frdmk82f_port_realize(DeviceState *dev, Error **errp) {
	FRDMK82FPORT *t = FRDMK82F_PORT(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), frdmk82f_port_can_receive, frdmk82f_port_receive,
		NULL, NULL, t, NULL, true);

}

static void frdmk82f_port_reset_enter(Object *obj, ResetType type) {
	FRDMK82FPORT *t = FRDMK82F_PORT(obj);
	frdmk82f_port_register_reset(t);
}

static const VMStateDescription frdmk82f_port_vmstate = {
	.name = "frdmk82f-port",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(PCR0, FRDMK82FPORT),
		VMSTATE_UINT32(PCR1, FRDMK82FPORT),
		VMSTATE_UINT32(PCR2, FRDMK82FPORT),
		VMSTATE_UINT32(PCR3, FRDMK82FPORT),
		VMSTATE_UINT32(PCR4, FRDMK82FPORT),
		VMSTATE_UINT32(PCR5, FRDMK82FPORT),
		VMSTATE_UINT32(PCR6, FRDMK82FPORT),
		VMSTATE_UINT32(PCR7, FRDMK82FPORT),
		VMSTATE_UINT32(PCR8, FRDMK82FPORT),
		VMSTATE_UINT32(PCR9, FRDMK82FPORT),
		VMSTATE_UINT32(PCR10, FRDMK82FPORT),
		VMSTATE_UINT32(PCR11, FRDMK82FPORT),
		VMSTATE_UINT32(PCR12, FRDMK82FPORT),
		VMSTATE_UINT32(PCR13, FRDMK82FPORT),
		VMSTATE_UINT32(PCR14, FRDMK82FPORT),
		VMSTATE_UINT32(PCR15, FRDMK82FPORT),
		VMSTATE_UINT32(PCR16, FRDMK82FPORT),
		VMSTATE_UINT32(PCR17, FRDMK82FPORT),
		VMSTATE_UINT32(PCR18, FRDMK82FPORT),
		VMSTATE_UINT32(PCR19, FRDMK82FPORT),
		VMSTATE_UINT32(PCR20, FRDMK82FPORT),
		VMSTATE_UINT32(PCR21, FRDMK82FPORT),
		VMSTATE_UINT32(PCR22, FRDMK82FPORT),
		VMSTATE_UINT32(PCR23, FRDMK82FPORT),
		VMSTATE_UINT32(PCR24, FRDMK82FPORT),
		VMSTATE_UINT32(PCR25, FRDMK82FPORT),
		VMSTATE_UINT32(PCR26, FRDMK82FPORT),
		VMSTATE_UINT32(PCR27, FRDMK82FPORT),
		VMSTATE_UINT32(PCR28, FRDMK82FPORT),
		VMSTATE_UINT32(PCR29, FRDMK82FPORT),
		VMSTATE_UINT32(PCR30, FRDMK82FPORT),
		VMSTATE_UINT32(PCR31, FRDMK82FPORT),
		VMSTATE_UINT32(GPCLR, FRDMK82FPORT),
		VMSTATE_UINT32(GPCHR, FRDMK82FPORT),
		VMSTATE_UINT32(ISFR, FRDMK82FPORT),

		VMSTATE_END_OF_LIST()
	}
};

static Property frdmk82f_port_properties[] = {
	DEFINE_PROP_CHR("chardev", FRDMK82FPORT, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void frdmk82f_port_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmk82f_port_vmstate;
	dc->realize = frdmk82f_port_realize;
	rc->phases.enter = frdmk82f_port_reset_enter;
	device_class_set_props(dc, frdmk82f_port_properties);

}

static const TypeInfo frdmk82f_port_info = {
	.name = TYPE_FRDMK82F_PORT,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMK82FPORT),
	.instance_init = frdmk82f_port_init,
	.class_init = frdmk82f_port_class_init,
};

static void frdmk82f_port_register_types(void) {
	type_register_static(&frdmk82f_port_info);
}

type_init(frdmk82f_port_register_types);

#define TYPE_FRDMK82F_SYSMPU "frdmk82f-sysmpu"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMK82FSYSMPU, FRDMK82F_SYSMPU)

struct FRDMK82FSYSMPU {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/*registers*/
	uint32_t CESR;
	uint32_t EAR0;
	uint32_t EAR1;
	uint32_t EAR2;
	uint32_t EAR3;
	uint32_t EAR4;
	uint32_t EDR0;
	uint32_t EDR1;
	uint32_t EDR2;
	uint32_t EDR3;
	uint32_t EDR4;
	uint32_t RGD0_WORD0;
	uint32_t RGD1_WORD0;
	uint32_t RGD2_WORD0;
	uint32_t RGD3_WORD0;
	uint32_t RGD4_WORD0;
	uint32_t RGD5_WORD0;
	uint32_t RGD6_WORD0;
	uint32_t RGD7_WORD0;
	uint32_t RGD8_WORD0;
	uint32_t RGD9_WORD0;
	uint32_t RGD10_WORD0;
	uint32_t RGD11_WORD0;
	uint32_t RGD0_WORD1;
	uint32_t RGD1_WORD1;
	uint32_t RGD2_WORD1;
	uint32_t RGD3_WORD1;
	uint32_t RGD4_WORD1;
	uint32_t RGD5_WORD1;
	uint32_t RGD6_WORD1;
	uint32_t RGD7_WORD1;
	uint32_t RGD8_WORD1;
	uint32_t RGD9_WORD1;
	uint32_t RGD10_WORD1;
	uint32_t RGD11_WORD1;
	uint32_t RGD0_WORD2;
	uint32_t RGD1_WORD2;
	uint32_t RGD2_WORD2;
	uint32_t RGD3_WORD2;
	uint32_t RGD4_WORD2;
	uint32_t RGD5_WORD2;
	uint32_t RGD6_WORD2;
	uint32_t RGD7_WORD2;
	uint32_t RGD8_WORD2;
	uint32_t RGD9_WORD2;
	uint32_t RGD10_WORD2;
	uint32_t RGD11_WORD2;
	uint32_t RGD0_WORD3;
	uint32_t RGD1_WORD3;
	uint32_t RGD2_WORD3;
	uint32_t RGD3_WORD3;
	uint32_t RGD4_WORD3;
	uint32_t RGD5_WORD3;
	uint32_t RGD6_WORD3;
	uint32_t RGD7_WORD3;
	uint32_t RGD8_WORD3;
	uint32_t RGD9_WORD3;
	uint32_t RGD10_WORD3;
	uint32_t RGD11_WORD3;
	uint32_t RGDAAC0;
	uint32_t RGDAAC1;
	uint32_t RGDAAC2;
	uint32_t RGDAAC3;
	uint32_t RGDAAC4;
	uint32_t RGDAAC5;
	uint32_t RGDAAC6;
	uint32_t RGDAAC7;
	uint32_t RGDAAC8;
	uint32_t RGDAAC9;
	uint32_t RGDAAC10;
	uint32_t RGDAAC11;
	/* base */
	uint32_t base;

};
#define FRDMK82F_SYSMPU_SIZE				0x830

REG32(SYSMPU_CESR, 0x0)
	FIELD(SYSMPU_CESR, VLD, 0, 1)
	FIELD(SYSMPU_CESR, NRGD, 8, 4)
	FIELD(SYSMPU_CESR, NSP, 12, 4)
	FIELD(SYSMPU_CESR, HRL, 16, 4)
	FIELD(SYSMPU_CESR, SPERR, 27, 5)
REG32(SYSMPU_EAR0, 0x10)
	FIELD(SYSMPU_EAR0, EADDR, 0, 32)
REG32(SYSMPU_EAR1, 0x18)
	FIELD(SYSMPU_EAR1, EADDR, 0, 32)
REG32(SYSMPU_EAR2, 0x20)
	FIELD(SYSMPU_EAR2, EADDR, 0, 32)
REG32(SYSMPU_EAR3, 0x28)
	FIELD(SYSMPU_EAR3, EADDR, 0, 32)
REG32(SYSMPU_EAR4, 0x30)
	FIELD(SYSMPU_EAR4, EADDR, 0, 32)
REG32(SYSMPU_EDR0, 0x14)
	FIELD(SYSMPU_EDR0, ERW, 0, 1)
	FIELD(SYSMPU_EDR0, EATTR, 1, 3)
	FIELD(SYSMPU_EDR0, EMN, 4, 4)
	FIELD(SYSMPU_EDR0, EPID, 8, 8)
	FIELD(SYSMPU_EDR0, EACD, 16, 16)
REG32(SYSMPU_EDR1, 0x1c)
	FIELD(SYSMPU_EDR1, ERW, 0, 1)
	FIELD(SYSMPU_EDR1, EATTR, 1, 3)
	FIELD(SYSMPU_EDR1, EMN, 4, 4)
	FIELD(SYSMPU_EDR1, EPID, 8, 8)
	FIELD(SYSMPU_EDR1, EACD, 16, 16)
REG32(SYSMPU_EDR2, 0x24)
	FIELD(SYSMPU_EDR2, ERW, 0, 1)
	FIELD(SYSMPU_EDR2, EATTR, 1, 3)
	FIELD(SYSMPU_EDR2, EMN, 4, 4)
	FIELD(SYSMPU_EDR2, EPID, 8, 8)
	FIELD(SYSMPU_EDR2, EACD, 16, 16)
REG32(SYSMPU_EDR3, 0x2c)
	FIELD(SYSMPU_EDR3, ERW, 0, 1)
	FIELD(SYSMPU_EDR3, EATTR, 1, 3)
	FIELD(SYSMPU_EDR3, EMN, 4, 4)
	FIELD(SYSMPU_EDR3, EPID, 8, 8)
	FIELD(SYSMPU_EDR3, EACD, 16, 16)
REG32(SYSMPU_EDR4, 0x34)
	FIELD(SYSMPU_EDR4, ERW, 0, 1)
	FIELD(SYSMPU_EDR4, EATTR, 1, 3)
	FIELD(SYSMPU_EDR4, EMN, 4, 4)
	FIELD(SYSMPU_EDR4, EPID, 8, 8)
	FIELD(SYSMPU_EDR4, EACD, 16, 16)
REG32(SYSMPU_RGD0_WORD0, 0x400)
	FIELD(SYSMPU_RGD0_WORD0, SRTADDR, 5, 27)
REG32(SYSMPU_RGD1_WORD0, 0x410)
	FIELD(SYSMPU_RGD1_WORD0, SRTADDR, 5, 27)
REG32(SYSMPU_RGD2_WORD0, 0x420)
	FIELD(SYSMPU_RGD2_WORD0, SRTADDR, 5, 27)
REG32(SYSMPU_RGD3_WORD0, 0x430)
	FIELD(SYSMPU_RGD3_WORD0, SRTADDR, 5, 27)
REG32(SYSMPU_RGD4_WORD0, 0x440)
	FIELD(SYSMPU_RGD4_WORD0, SRTADDR, 5, 27)
REG32(SYSMPU_RGD5_WORD0, 0x450)
	FIELD(SYSMPU_RGD5_WORD0, SRTADDR, 5, 27)
REG32(SYSMPU_RGD6_WORD0, 0x460)
	FIELD(SYSMPU_RGD6_WORD0, SRTADDR, 5, 27)
REG32(SYSMPU_RGD7_WORD0, 0x470)
	FIELD(SYSMPU_RGD7_WORD0, SRTADDR, 5, 27)
REG32(SYSMPU_RGD8_WORD0, 0x480)
	FIELD(SYSMPU_RGD8_WORD0, SRTADDR, 5, 27)
REG32(SYSMPU_RGD9_WORD0, 0x490)
	FIELD(SYSMPU_RGD9_WORD0, SRTADDR, 5, 27)
REG32(SYSMPU_RGD10_WORD0, 0x4a0)
	FIELD(SYSMPU_RGD10_WORD0, SRTADDR, 5, 27)
REG32(SYSMPU_RGD11_WORD0, 0x4b0)
	FIELD(SYSMPU_RGD11_WORD0, SRTADDR, 5, 27)
REG32(SYSMPU_RGD0_WORD1, 0x404)
	FIELD(SYSMPU_RGD0_WORD1, ENDADDR, 5, 27)
REG32(SYSMPU_RGD1_WORD1, 0x414)
	FIELD(SYSMPU_RGD1_WORD1, ENDADDR, 5, 27)
REG32(SYSMPU_RGD2_WORD1, 0x424)
	FIELD(SYSMPU_RGD2_WORD1, ENDADDR, 5, 27)
REG32(SYSMPU_RGD3_WORD1, 0x434)
	FIELD(SYSMPU_RGD3_WORD1, ENDADDR, 5, 27)
REG32(SYSMPU_RGD4_WORD1, 0x444)
	FIELD(SYSMPU_RGD4_WORD1, ENDADDR, 5, 27)
REG32(SYSMPU_RGD5_WORD1, 0x454)
	FIELD(SYSMPU_RGD5_WORD1, ENDADDR, 5, 27)
REG32(SYSMPU_RGD6_WORD1, 0x464)
	FIELD(SYSMPU_RGD6_WORD1, ENDADDR, 5, 27)
REG32(SYSMPU_RGD7_WORD1, 0x474)
	FIELD(SYSMPU_RGD7_WORD1, ENDADDR, 5, 27)
REG32(SYSMPU_RGD8_WORD1, 0x484)
	FIELD(SYSMPU_RGD8_WORD1, ENDADDR, 5, 27)
REG32(SYSMPU_RGD9_WORD1, 0x494)
	FIELD(SYSMPU_RGD9_WORD1, ENDADDR, 5, 27)
REG32(SYSMPU_RGD10_WORD1, 0x4a4)
	FIELD(SYSMPU_RGD10_WORD1, ENDADDR, 5, 27)
REG32(SYSMPU_RGD11_WORD1, 0x4b4)
	FIELD(SYSMPU_RGD11_WORD1, ENDADDR, 5, 27)
REG32(SYSMPU_RGD0_WORD2, 0x408)
	FIELD(SYSMPU_RGD0_WORD2, M0UM, 0, 3)
	FIELD(SYSMPU_RGD0_WORD2, M0SM, 3, 2)
	FIELD(SYSMPU_RGD0_WORD2, M0PE, 5, 1)
	FIELD(SYSMPU_RGD0_WORD2, M1UM, 6, 3)
	FIELD(SYSMPU_RGD0_WORD2, M1SM, 9, 2)
	FIELD(SYSMPU_RGD0_WORD2, M1PE, 11, 1)
	FIELD(SYSMPU_RGD0_WORD2, M2UM, 12, 3)
	FIELD(SYSMPU_RGD0_WORD2, M2SM, 15, 2)
	FIELD(SYSMPU_RGD0_WORD2, M2PE, 17, 1)
	FIELD(SYSMPU_RGD0_WORD2, M3UM, 18, 3)
	FIELD(SYSMPU_RGD0_WORD2, M3SM, 21, 2)
	FIELD(SYSMPU_RGD0_WORD2, M3PE, 23, 1)
	FIELD(SYSMPU_RGD0_WORD2, M4WE, 24, 1)
	FIELD(SYSMPU_RGD0_WORD2, M4RE, 25, 1)
	FIELD(SYSMPU_RGD0_WORD2, M5WE, 26, 1)
	FIELD(SYSMPU_RGD0_WORD2, M5RE, 27, 1)
	FIELD(SYSMPU_RGD0_WORD2, M6WE, 28, 1)
	FIELD(SYSMPU_RGD0_WORD2, M6RE, 29, 1)
	FIELD(SYSMPU_RGD0_WORD2, M7WE, 30, 1)
	FIELD(SYSMPU_RGD0_WORD2, M7RE, 31, 1)
REG32(SYSMPU_RGD1_WORD2, 0x418)
	FIELD(SYSMPU_RGD1_WORD2, M0UM, 0, 3)
	FIELD(SYSMPU_RGD1_WORD2, M0SM, 3, 2)
	FIELD(SYSMPU_RGD1_WORD2, M0PE, 5, 1)
	FIELD(SYSMPU_RGD1_WORD2, M1UM, 6, 3)
	FIELD(SYSMPU_RGD1_WORD2, M1SM, 9, 2)
	FIELD(SYSMPU_RGD1_WORD2, M1PE, 11, 1)
	FIELD(SYSMPU_RGD1_WORD2, M2UM, 12, 3)
	FIELD(SYSMPU_RGD1_WORD2, M2SM, 15, 2)
	FIELD(SYSMPU_RGD1_WORD2, M2PE, 17, 1)
	FIELD(SYSMPU_RGD1_WORD2, M3UM, 18, 3)
	FIELD(SYSMPU_RGD1_WORD2, M3SM, 21, 2)
	FIELD(SYSMPU_RGD1_WORD2, M3PE, 23, 1)
	FIELD(SYSMPU_RGD1_WORD2, M4WE, 24, 1)
	FIELD(SYSMPU_RGD1_WORD2, M4RE, 25, 1)
	FIELD(SYSMPU_RGD1_WORD2, M5WE, 26, 1)
	FIELD(SYSMPU_RGD1_WORD2, M5RE, 27, 1)
	FIELD(SYSMPU_RGD1_WORD2, M6WE, 28, 1)
	FIELD(SYSMPU_RGD1_WORD2, M6RE, 29, 1)
	FIELD(SYSMPU_RGD1_WORD2, M7WE, 30, 1)
	FIELD(SYSMPU_RGD1_WORD2, M7RE, 31, 1)
REG32(SYSMPU_RGD2_WORD2, 0x428)
	FIELD(SYSMPU_RGD2_WORD2, M0UM, 0, 3)
	FIELD(SYSMPU_RGD2_WORD2, M0SM, 3, 2)
	FIELD(SYSMPU_RGD2_WORD2, M0PE, 5, 1)
	FIELD(SYSMPU_RGD2_WORD2, M1UM, 6, 3)
	FIELD(SYSMPU_RGD2_WORD2, M1SM, 9, 2)
	FIELD(SYSMPU_RGD2_WORD2, M1PE, 11, 1)
	FIELD(SYSMPU_RGD2_WORD2, M2UM, 12, 3)
	FIELD(SYSMPU_RGD2_WORD2, M2SM, 15, 2)
	FIELD(SYSMPU_RGD2_WORD2, M2PE, 17, 1)
	FIELD(SYSMPU_RGD2_WORD2, M3UM, 18, 3)
	FIELD(SYSMPU_RGD2_WORD2, M3SM, 21, 2)
	FIELD(SYSMPU_RGD2_WORD2, M3PE, 23, 1)
	FIELD(SYSMPU_RGD2_WORD2, M4WE, 24, 1)
	FIELD(SYSMPU_RGD2_WORD2, M4RE, 25, 1)
	FIELD(SYSMPU_RGD2_WORD2, M5WE, 26, 1)
	FIELD(SYSMPU_RGD2_WORD2, M5RE, 27, 1)
	FIELD(SYSMPU_RGD2_WORD2, M6WE, 28, 1)
	FIELD(SYSMPU_RGD2_WORD2, M6RE, 29, 1)
	FIELD(SYSMPU_RGD2_WORD2, M7WE, 30, 1)
	FIELD(SYSMPU_RGD2_WORD2, M7RE, 31, 1)
REG32(SYSMPU_RGD3_WORD2, 0x438)
	FIELD(SYSMPU_RGD3_WORD2, M0UM, 0, 3)
	FIELD(SYSMPU_RGD3_WORD2, M0SM, 3, 2)
	FIELD(SYSMPU_RGD3_WORD2, M0PE, 5, 1)
	FIELD(SYSMPU_RGD3_WORD2, M1UM, 6, 3)
	FIELD(SYSMPU_RGD3_WORD2, M1SM, 9, 2)
	FIELD(SYSMPU_RGD3_WORD2, M1PE, 11, 1)
	FIELD(SYSMPU_RGD3_WORD2, M2UM, 12, 3)
	FIELD(SYSMPU_RGD3_WORD2, M2SM, 15, 2)
	FIELD(SYSMPU_RGD3_WORD2, M2PE, 17, 1)
	FIELD(SYSMPU_RGD3_WORD2, M3UM, 18, 3)
	FIELD(SYSMPU_RGD3_WORD2, M3SM, 21, 2)
	FIELD(SYSMPU_RGD3_WORD2, M3PE, 23, 1)
	FIELD(SYSMPU_RGD3_WORD2, M4WE, 24, 1)
	FIELD(SYSMPU_RGD3_WORD2, M4RE, 25, 1)
	FIELD(SYSMPU_RGD3_WORD2, M5WE, 26, 1)
	FIELD(SYSMPU_RGD3_WORD2, M5RE, 27, 1)
	FIELD(SYSMPU_RGD3_WORD2, M6WE, 28, 1)
	FIELD(SYSMPU_RGD3_WORD2, M6RE, 29, 1)
	FIELD(SYSMPU_RGD3_WORD2, M7WE, 30, 1)
	FIELD(SYSMPU_RGD3_WORD2, M7RE, 31, 1)
REG32(SYSMPU_RGD4_WORD2, 0x448)
	FIELD(SYSMPU_RGD4_WORD2, M0UM, 0, 3)
	FIELD(SYSMPU_RGD4_WORD2, M0SM, 3, 2)
	FIELD(SYSMPU_RGD4_WORD2, M0PE, 5, 1)
	FIELD(SYSMPU_RGD4_WORD2, M1UM, 6, 3)
	FIELD(SYSMPU_RGD4_WORD2, M1SM, 9, 2)
	FIELD(SYSMPU_RGD4_WORD2, M1PE, 11, 1)
	FIELD(SYSMPU_RGD4_WORD2, M2UM, 12, 3)
	FIELD(SYSMPU_RGD4_WORD2, M2SM, 15, 2)
	FIELD(SYSMPU_RGD4_WORD2, M2PE, 17, 1)
	FIELD(SYSMPU_RGD4_WORD2, M3UM, 18, 3)
	FIELD(SYSMPU_RGD4_WORD2, M3SM, 21, 2)
	FIELD(SYSMPU_RGD4_WORD2, M3PE, 23, 1)
	FIELD(SYSMPU_RGD4_WORD2, M4WE, 24, 1)
	FIELD(SYSMPU_RGD4_WORD2, M4RE, 25, 1)
	FIELD(SYSMPU_RGD4_WORD2, M5WE, 26, 1)
	FIELD(SYSMPU_RGD4_WORD2, M5RE, 27, 1)
	FIELD(SYSMPU_RGD4_WORD2, M6WE, 28, 1)
	FIELD(SYSMPU_RGD4_WORD2, M6RE, 29, 1)
	FIELD(SYSMPU_RGD4_WORD2, M7WE, 30, 1)
	FIELD(SYSMPU_RGD4_WORD2, M7RE, 31, 1)
REG32(SYSMPU_RGD5_WORD2, 0x458)
	FIELD(SYSMPU_RGD5_WORD2, M0UM, 0, 3)
	FIELD(SYSMPU_RGD5_WORD2, M0SM, 3, 2)
	FIELD(SYSMPU_RGD5_WORD2, M0PE, 5, 1)
	FIELD(SYSMPU_RGD5_WORD2, M1UM, 6, 3)
	FIELD(SYSMPU_RGD5_WORD2, M1SM, 9, 2)
	FIELD(SYSMPU_RGD5_WORD2, M1PE, 11, 1)
	FIELD(SYSMPU_RGD5_WORD2, M2UM, 12, 3)
	FIELD(SYSMPU_RGD5_WORD2, M2SM, 15, 2)
	FIELD(SYSMPU_RGD5_WORD2, M2PE, 17, 1)
	FIELD(SYSMPU_RGD5_WORD2, M3UM, 18, 3)
	FIELD(SYSMPU_RGD5_WORD2, M3SM, 21, 2)
	FIELD(SYSMPU_RGD5_WORD2, M3PE, 23, 1)
	FIELD(SYSMPU_RGD5_WORD2, M4WE, 24, 1)
	FIELD(SYSMPU_RGD5_WORD2, M4RE, 25, 1)
	FIELD(SYSMPU_RGD5_WORD2, M5WE, 26, 1)
	FIELD(SYSMPU_RGD5_WORD2, M5RE, 27, 1)
	FIELD(SYSMPU_RGD5_WORD2, M6WE, 28, 1)
	FIELD(SYSMPU_RGD5_WORD2, M6RE, 29, 1)
	FIELD(SYSMPU_RGD5_WORD2, M7WE, 30, 1)
	FIELD(SYSMPU_RGD5_WORD2, M7RE, 31, 1)
REG32(SYSMPU_RGD6_WORD2, 0x468)
	FIELD(SYSMPU_RGD6_WORD2, M0UM, 0, 3)
	FIELD(SYSMPU_RGD6_WORD2, M0SM, 3, 2)
	FIELD(SYSMPU_RGD6_WORD2, M0PE, 5, 1)
	FIELD(SYSMPU_RGD6_WORD2, M1UM, 6, 3)
	FIELD(SYSMPU_RGD6_WORD2, M1SM, 9, 2)
	FIELD(SYSMPU_RGD6_WORD2, M1PE, 11, 1)
	FIELD(SYSMPU_RGD6_WORD2, M2UM, 12, 3)
	FIELD(SYSMPU_RGD6_WORD2, M2SM, 15, 2)
	FIELD(SYSMPU_RGD6_WORD2, M2PE, 17, 1)
	FIELD(SYSMPU_RGD6_WORD2, M3UM, 18, 3)
	FIELD(SYSMPU_RGD6_WORD2, M3SM, 21, 2)
	FIELD(SYSMPU_RGD6_WORD2, M3PE, 23, 1)
	FIELD(SYSMPU_RGD6_WORD2, M4WE, 24, 1)
	FIELD(SYSMPU_RGD6_WORD2, M4RE, 25, 1)
	FIELD(SYSMPU_RGD6_WORD2, M5WE, 26, 1)
	FIELD(SYSMPU_RGD6_WORD2, M5RE, 27, 1)
	FIELD(SYSMPU_RGD6_WORD2, M6WE, 28, 1)
	FIELD(SYSMPU_RGD6_WORD2, M6RE, 29, 1)
	FIELD(SYSMPU_RGD6_WORD2, M7WE, 30, 1)
	FIELD(SYSMPU_RGD6_WORD2, M7RE, 31, 1)
REG32(SYSMPU_RGD7_WORD2, 0x478)
	FIELD(SYSMPU_RGD7_WORD2, M0UM, 0, 3)
	FIELD(SYSMPU_RGD7_WORD2, M0SM, 3, 2)
	FIELD(SYSMPU_RGD7_WORD2, M0PE, 5, 1)
	FIELD(SYSMPU_RGD7_WORD2, M1UM, 6, 3)
	FIELD(SYSMPU_RGD7_WORD2, M1SM, 9, 2)
	FIELD(SYSMPU_RGD7_WORD2, M1PE, 11, 1)
	FIELD(SYSMPU_RGD7_WORD2, M2UM, 12, 3)
	FIELD(SYSMPU_RGD7_WORD2, M2SM, 15, 2)
	FIELD(SYSMPU_RGD7_WORD2, M2PE, 17, 1)
	FIELD(SYSMPU_RGD7_WORD2, M3UM, 18, 3)
	FIELD(SYSMPU_RGD7_WORD2, M3SM, 21, 2)
	FIELD(SYSMPU_RGD7_WORD2, M3PE, 23, 1)
	FIELD(SYSMPU_RGD7_WORD2, M4WE, 24, 1)
	FIELD(SYSMPU_RGD7_WORD2, M4RE, 25, 1)
	FIELD(SYSMPU_RGD7_WORD2, M5WE, 26, 1)
	FIELD(SYSMPU_RGD7_WORD2, M5RE, 27, 1)
	FIELD(SYSMPU_RGD7_WORD2, M6WE, 28, 1)
	FIELD(SYSMPU_RGD7_WORD2, M6RE, 29, 1)
	FIELD(SYSMPU_RGD7_WORD2, M7WE, 30, 1)
	FIELD(SYSMPU_RGD7_WORD2, M7RE, 31, 1)
REG32(SYSMPU_RGD8_WORD2, 0x488)
	FIELD(SYSMPU_RGD8_WORD2, M0UM, 0, 3)
	FIELD(SYSMPU_RGD8_WORD2, M0SM, 3, 2)
	FIELD(SYSMPU_RGD8_WORD2, M0PE, 5, 1)
	FIELD(SYSMPU_RGD8_WORD2, M1UM, 6, 3)
	FIELD(SYSMPU_RGD8_WORD2, M1SM, 9, 2)
	FIELD(SYSMPU_RGD8_WORD2, M1PE, 11, 1)
	FIELD(SYSMPU_RGD8_WORD2, M2UM, 12, 3)
	FIELD(SYSMPU_RGD8_WORD2, M2SM, 15, 2)
	FIELD(SYSMPU_RGD8_WORD2, M2PE, 17, 1)
	FIELD(SYSMPU_RGD8_WORD2, M3UM, 18, 3)
	FIELD(SYSMPU_RGD8_WORD2, M3SM, 21, 2)
	FIELD(SYSMPU_RGD8_WORD2, M3PE, 23, 1)
	FIELD(SYSMPU_RGD8_WORD2, M4WE, 24, 1)
	FIELD(SYSMPU_RGD8_WORD2, M4RE, 25, 1)
	FIELD(SYSMPU_RGD8_WORD2, M5WE, 26, 1)
	FIELD(SYSMPU_RGD8_WORD2, M5RE, 27, 1)
	FIELD(SYSMPU_RGD8_WORD2, M6WE, 28, 1)
	FIELD(SYSMPU_RGD8_WORD2, M6RE, 29, 1)
	FIELD(SYSMPU_RGD8_WORD2, M7WE, 30, 1)
	FIELD(SYSMPU_RGD8_WORD2, M7RE, 31, 1)
REG32(SYSMPU_RGD9_WORD2, 0x498)
	FIELD(SYSMPU_RGD9_WORD2, M0UM, 0, 3)
	FIELD(SYSMPU_RGD9_WORD2, M0SM, 3, 2)
	FIELD(SYSMPU_RGD9_WORD2, M0PE, 5, 1)
	FIELD(SYSMPU_RGD9_WORD2, M1UM, 6, 3)
	FIELD(SYSMPU_RGD9_WORD2, M1SM, 9, 2)
	FIELD(SYSMPU_RGD9_WORD2, M1PE, 11, 1)
	FIELD(SYSMPU_RGD9_WORD2, M2UM, 12, 3)
	FIELD(SYSMPU_RGD9_WORD2, M2SM, 15, 2)
	FIELD(SYSMPU_RGD9_WORD2, M2PE, 17, 1)
	FIELD(SYSMPU_RGD9_WORD2, M3UM, 18, 3)
	FIELD(SYSMPU_RGD9_WORD2, M3SM, 21, 2)
	FIELD(SYSMPU_RGD9_WORD2, M3PE, 23, 1)
	FIELD(SYSMPU_RGD9_WORD2, M4WE, 24, 1)
	FIELD(SYSMPU_RGD9_WORD2, M4RE, 25, 1)
	FIELD(SYSMPU_RGD9_WORD2, M5WE, 26, 1)
	FIELD(SYSMPU_RGD9_WORD2, M5RE, 27, 1)
	FIELD(SYSMPU_RGD9_WORD2, M6WE, 28, 1)
	FIELD(SYSMPU_RGD9_WORD2, M6RE, 29, 1)
	FIELD(SYSMPU_RGD9_WORD2, M7WE, 30, 1)
	FIELD(SYSMPU_RGD9_WORD2, M7RE, 31, 1)
REG32(SYSMPU_RGD10_WORD2, 0x4a8)
	FIELD(SYSMPU_RGD10_WORD2, M0UM, 0, 3)
	FIELD(SYSMPU_RGD10_WORD2, M0SM, 3, 2)
	FIELD(SYSMPU_RGD10_WORD2, M0PE, 5, 1)
	FIELD(SYSMPU_RGD10_WORD2, M1UM, 6, 3)
	FIELD(SYSMPU_RGD10_WORD2, M1SM, 9, 2)
	FIELD(SYSMPU_RGD10_WORD2, M1PE, 11, 1)
	FIELD(SYSMPU_RGD10_WORD2, M2UM, 12, 3)
	FIELD(SYSMPU_RGD10_WORD2, M2SM, 15, 2)
	FIELD(SYSMPU_RGD10_WORD2, M2PE, 17, 1)
	FIELD(SYSMPU_RGD10_WORD2, M3UM, 18, 3)
	FIELD(SYSMPU_RGD10_WORD2, M3SM, 21, 2)
	FIELD(SYSMPU_RGD10_WORD2, M3PE, 23, 1)
	FIELD(SYSMPU_RGD10_WORD2, M4WE, 24, 1)
	FIELD(SYSMPU_RGD10_WORD2, M4RE, 25, 1)
	FIELD(SYSMPU_RGD10_WORD2, M5WE, 26, 1)
	FIELD(SYSMPU_RGD10_WORD2, M5RE, 27, 1)
	FIELD(SYSMPU_RGD10_WORD2, M6WE, 28, 1)
	FIELD(SYSMPU_RGD10_WORD2, M6RE, 29, 1)
	FIELD(SYSMPU_RGD10_WORD2, M7WE, 30, 1)
	FIELD(SYSMPU_RGD10_WORD2, M7RE, 31, 1)
REG32(SYSMPU_RGD11_WORD2, 0x4b8)
	FIELD(SYSMPU_RGD11_WORD2, M0UM, 0, 3)
	FIELD(SYSMPU_RGD11_WORD2, M0SM, 3, 2)
	FIELD(SYSMPU_RGD11_WORD2, M0PE, 5, 1)
	FIELD(SYSMPU_RGD11_WORD2, M1UM, 6, 3)
	FIELD(SYSMPU_RGD11_WORD2, M1SM, 9, 2)
	FIELD(SYSMPU_RGD11_WORD2, M1PE, 11, 1)
	FIELD(SYSMPU_RGD11_WORD2, M2UM, 12, 3)
	FIELD(SYSMPU_RGD11_WORD2, M2SM, 15, 2)
	FIELD(SYSMPU_RGD11_WORD2, M2PE, 17, 1)
	FIELD(SYSMPU_RGD11_WORD2, M3UM, 18, 3)
	FIELD(SYSMPU_RGD11_WORD2, M3SM, 21, 2)
	FIELD(SYSMPU_RGD11_WORD2, M3PE, 23, 1)
	FIELD(SYSMPU_RGD11_WORD2, M4WE, 24, 1)
	FIELD(SYSMPU_RGD11_WORD2, M4RE, 25, 1)
	FIELD(SYSMPU_RGD11_WORD2, M5WE, 26, 1)
	FIELD(SYSMPU_RGD11_WORD2, M5RE, 27, 1)
	FIELD(SYSMPU_RGD11_WORD2, M6WE, 28, 1)
	FIELD(SYSMPU_RGD11_WORD2, M6RE, 29, 1)
	FIELD(SYSMPU_RGD11_WORD2, M7WE, 30, 1)
	FIELD(SYSMPU_RGD11_WORD2, M7RE, 31, 1)
REG32(SYSMPU_RGD0_WORD3, 0x40c)
	FIELD(SYSMPU_RGD0_WORD3, VLD, 0, 1)
	FIELD(SYSMPU_RGD0_WORD3, PIDMASK, 16, 8)
	FIELD(SYSMPU_RGD0_WORD3, PID, 24, 8)
REG32(SYSMPU_RGD1_WORD3, 0x41c)
	FIELD(SYSMPU_RGD1_WORD3, VLD, 0, 1)
	FIELD(SYSMPU_RGD1_WORD3, PIDMASK, 16, 8)
	FIELD(SYSMPU_RGD1_WORD3, PID, 24, 8)
REG32(SYSMPU_RGD2_WORD3, 0x42c)
	FIELD(SYSMPU_RGD2_WORD3, VLD, 0, 1)
	FIELD(SYSMPU_RGD2_WORD3, PIDMASK, 16, 8)
	FIELD(SYSMPU_RGD2_WORD3, PID, 24, 8)
REG32(SYSMPU_RGD3_WORD3, 0x43c)
	FIELD(SYSMPU_RGD3_WORD3, VLD, 0, 1)
	FIELD(SYSMPU_RGD3_WORD3, PIDMASK, 16, 8)
	FIELD(SYSMPU_RGD3_WORD3, PID, 24, 8)
REG32(SYSMPU_RGD4_WORD3, 0x44c)
	FIELD(SYSMPU_RGD4_WORD3, VLD, 0, 1)
	FIELD(SYSMPU_RGD4_WORD3, PIDMASK, 16, 8)
	FIELD(SYSMPU_RGD4_WORD3, PID, 24, 8)
REG32(SYSMPU_RGD5_WORD3, 0x45c)
	FIELD(SYSMPU_RGD5_WORD3, VLD, 0, 1)
	FIELD(SYSMPU_RGD5_WORD3, PIDMASK, 16, 8)
	FIELD(SYSMPU_RGD5_WORD3, PID, 24, 8)
REG32(SYSMPU_RGD6_WORD3, 0x46c)
	FIELD(SYSMPU_RGD6_WORD3, VLD, 0, 1)
	FIELD(SYSMPU_RGD6_WORD3, PIDMASK, 16, 8)
	FIELD(SYSMPU_RGD6_WORD3, PID, 24, 8)
REG32(SYSMPU_RGD7_WORD3, 0x47c)
	FIELD(SYSMPU_RGD7_WORD3, VLD, 0, 1)
	FIELD(SYSMPU_RGD7_WORD3, PIDMASK, 16, 8)
	FIELD(SYSMPU_RGD7_WORD3, PID, 24, 8)
REG32(SYSMPU_RGD8_WORD3, 0x48c)
	FIELD(SYSMPU_RGD8_WORD3, VLD, 0, 1)
	FIELD(SYSMPU_RGD8_WORD3, PIDMASK, 16, 8)
	FIELD(SYSMPU_RGD8_WORD3, PID, 24, 8)
REG32(SYSMPU_RGD9_WORD3, 0x49c)
	FIELD(SYSMPU_RGD9_WORD3, VLD, 0, 1)
	FIELD(SYSMPU_RGD9_WORD3, PIDMASK, 16, 8)
	FIELD(SYSMPU_RGD9_WORD3, PID, 24, 8)
REG32(SYSMPU_RGD10_WORD3, 0x4ac)
	FIELD(SYSMPU_RGD10_WORD3, VLD, 0, 1)
	FIELD(SYSMPU_RGD10_WORD3, PIDMASK, 16, 8)
	FIELD(SYSMPU_RGD10_WORD3, PID, 24, 8)
REG32(SYSMPU_RGD11_WORD3, 0x4bc)
	FIELD(SYSMPU_RGD11_WORD3, VLD, 0, 1)
	FIELD(SYSMPU_RGD11_WORD3, PIDMASK, 16, 8)
	FIELD(SYSMPU_RGD11_WORD3, PID, 24, 8)
REG32(SYSMPU_RGDAAC0, 0x800)
	FIELD(SYSMPU_RGDAAC0, M0UM, 0, 3)
	FIELD(SYSMPU_RGDAAC0, M0SM, 3, 2)
	FIELD(SYSMPU_RGDAAC0, M0PE, 5, 1)
	FIELD(SYSMPU_RGDAAC0, M1UM, 6, 3)
	FIELD(SYSMPU_RGDAAC0, M1SM, 9, 2)
	FIELD(SYSMPU_RGDAAC0, M1PE, 11, 1)
	FIELD(SYSMPU_RGDAAC0, M2UM, 12, 3)
	FIELD(SYSMPU_RGDAAC0, M2SM, 15, 2)
	FIELD(SYSMPU_RGDAAC0, M2PE, 17, 1)
	FIELD(SYSMPU_RGDAAC0, M3UM, 18, 3)
	FIELD(SYSMPU_RGDAAC0, M3SM, 21, 2)
	FIELD(SYSMPU_RGDAAC0, M3PE, 23, 1)
	FIELD(SYSMPU_RGDAAC0, M4WE, 24, 1)
	FIELD(SYSMPU_RGDAAC0, M4RE, 25, 1)
	FIELD(SYSMPU_RGDAAC0, M5WE, 26, 1)
	FIELD(SYSMPU_RGDAAC0, M5RE, 27, 1)
	FIELD(SYSMPU_RGDAAC0, M6WE, 28, 1)
	FIELD(SYSMPU_RGDAAC0, M6RE, 29, 1)
	FIELD(SYSMPU_RGDAAC0, M7WE, 30, 1)
	FIELD(SYSMPU_RGDAAC0, M7RE, 31, 1)
REG32(SYSMPU_RGDAAC1, 0x804)
	FIELD(SYSMPU_RGDAAC1, M0UM, 0, 3)
	FIELD(SYSMPU_RGDAAC1, M0SM, 3, 2)
	FIELD(SYSMPU_RGDAAC1, M0PE, 5, 1)
	FIELD(SYSMPU_RGDAAC1, M1UM, 6, 3)
	FIELD(SYSMPU_RGDAAC1, M1SM, 9, 2)
	FIELD(SYSMPU_RGDAAC1, M1PE, 11, 1)
	FIELD(SYSMPU_RGDAAC1, M2UM, 12, 3)
	FIELD(SYSMPU_RGDAAC1, M2SM, 15, 2)
	FIELD(SYSMPU_RGDAAC1, M2PE, 17, 1)
	FIELD(SYSMPU_RGDAAC1, M3UM, 18, 3)
	FIELD(SYSMPU_RGDAAC1, M3SM, 21, 2)
	FIELD(SYSMPU_RGDAAC1, M3PE, 23, 1)
	FIELD(SYSMPU_RGDAAC1, M4WE, 24, 1)
	FIELD(SYSMPU_RGDAAC1, M4RE, 25, 1)
	FIELD(SYSMPU_RGDAAC1, M5WE, 26, 1)
	FIELD(SYSMPU_RGDAAC1, M5RE, 27, 1)
	FIELD(SYSMPU_RGDAAC1, M6WE, 28, 1)
	FIELD(SYSMPU_RGDAAC1, M6RE, 29, 1)
	FIELD(SYSMPU_RGDAAC1, M7WE, 30, 1)
	FIELD(SYSMPU_RGDAAC1, M7RE, 31, 1)
REG32(SYSMPU_RGDAAC2, 0x808)
	FIELD(SYSMPU_RGDAAC2, M0UM, 0, 3)
	FIELD(SYSMPU_RGDAAC2, M0SM, 3, 2)
	FIELD(SYSMPU_RGDAAC2, M0PE, 5, 1)
	FIELD(SYSMPU_RGDAAC2, M1UM, 6, 3)
	FIELD(SYSMPU_RGDAAC2, M1SM, 9, 2)
	FIELD(SYSMPU_RGDAAC2, M1PE, 11, 1)
	FIELD(SYSMPU_RGDAAC2, M2UM, 12, 3)
	FIELD(SYSMPU_RGDAAC2, M2SM, 15, 2)
	FIELD(SYSMPU_RGDAAC2, M2PE, 17, 1)
	FIELD(SYSMPU_RGDAAC2, M3UM, 18, 3)
	FIELD(SYSMPU_RGDAAC2, M3SM, 21, 2)
	FIELD(SYSMPU_RGDAAC2, M3PE, 23, 1)
	FIELD(SYSMPU_RGDAAC2, M4WE, 24, 1)
	FIELD(SYSMPU_RGDAAC2, M4RE, 25, 1)
	FIELD(SYSMPU_RGDAAC2, M5WE, 26, 1)
	FIELD(SYSMPU_RGDAAC2, M5RE, 27, 1)
	FIELD(SYSMPU_RGDAAC2, M6WE, 28, 1)
	FIELD(SYSMPU_RGDAAC2, M6RE, 29, 1)
	FIELD(SYSMPU_RGDAAC2, M7WE, 30, 1)
	FIELD(SYSMPU_RGDAAC2, M7RE, 31, 1)
REG32(SYSMPU_RGDAAC3, 0x80c)
	FIELD(SYSMPU_RGDAAC3, M0UM, 0, 3)
	FIELD(SYSMPU_RGDAAC3, M0SM, 3, 2)
	FIELD(SYSMPU_RGDAAC3, M0PE, 5, 1)
	FIELD(SYSMPU_RGDAAC3, M1UM, 6, 3)
	FIELD(SYSMPU_RGDAAC3, M1SM, 9, 2)
	FIELD(SYSMPU_RGDAAC3, M1PE, 11, 1)
	FIELD(SYSMPU_RGDAAC3, M2UM, 12, 3)
	FIELD(SYSMPU_RGDAAC3, M2SM, 15, 2)
	FIELD(SYSMPU_RGDAAC3, M2PE, 17, 1)
	FIELD(SYSMPU_RGDAAC3, M3UM, 18, 3)
	FIELD(SYSMPU_RGDAAC3, M3SM, 21, 2)
	FIELD(SYSMPU_RGDAAC3, M3PE, 23, 1)
	FIELD(SYSMPU_RGDAAC3, M4WE, 24, 1)
	FIELD(SYSMPU_RGDAAC3, M4RE, 25, 1)
	FIELD(SYSMPU_RGDAAC3, M5WE, 26, 1)
	FIELD(SYSMPU_RGDAAC3, M5RE, 27, 1)
	FIELD(SYSMPU_RGDAAC3, M6WE, 28, 1)
	FIELD(SYSMPU_RGDAAC3, M6RE, 29, 1)
	FIELD(SYSMPU_RGDAAC3, M7WE, 30, 1)
	FIELD(SYSMPU_RGDAAC3, M7RE, 31, 1)
REG32(SYSMPU_RGDAAC4, 0x810)
	FIELD(SYSMPU_RGDAAC4, M0UM, 0, 3)
	FIELD(SYSMPU_RGDAAC4, M0SM, 3, 2)
	FIELD(SYSMPU_RGDAAC4, M0PE, 5, 1)
	FIELD(SYSMPU_RGDAAC4, M1UM, 6, 3)
	FIELD(SYSMPU_RGDAAC4, M1SM, 9, 2)
	FIELD(SYSMPU_RGDAAC4, M1PE, 11, 1)
	FIELD(SYSMPU_RGDAAC4, M2UM, 12, 3)
	FIELD(SYSMPU_RGDAAC4, M2SM, 15, 2)
	FIELD(SYSMPU_RGDAAC4, M2PE, 17, 1)
	FIELD(SYSMPU_RGDAAC4, M3UM, 18, 3)
	FIELD(SYSMPU_RGDAAC4, M3SM, 21, 2)
	FIELD(SYSMPU_RGDAAC4, M3PE, 23, 1)
	FIELD(SYSMPU_RGDAAC4, M4WE, 24, 1)
	FIELD(SYSMPU_RGDAAC4, M4RE, 25, 1)
	FIELD(SYSMPU_RGDAAC4, M5WE, 26, 1)
	FIELD(SYSMPU_RGDAAC4, M5RE, 27, 1)
	FIELD(SYSMPU_RGDAAC4, M6WE, 28, 1)
	FIELD(SYSMPU_RGDAAC4, M6RE, 29, 1)
	FIELD(SYSMPU_RGDAAC4, M7WE, 30, 1)
	FIELD(SYSMPU_RGDAAC4, M7RE, 31, 1)
REG32(SYSMPU_RGDAAC5, 0x814)
	FIELD(SYSMPU_RGDAAC5, M0UM, 0, 3)
	FIELD(SYSMPU_RGDAAC5, M0SM, 3, 2)
	FIELD(SYSMPU_RGDAAC5, M0PE, 5, 1)
	FIELD(SYSMPU_RGDAAC5, M1UM, 6, 3)
	FIELD(SYSMPU_RGDAAC5, M1SM, 9, 2)
	FIELD(SYSMPU_RGDAAC5, M1PE, 11, 1)
	FIELD(SYSMPU_RGDAAC5, M2UM, 12, 3)
	FIELD(SYSMPU_RGDAAC5, M2SM, 15, 2)
	FIELD(SYSMPU_RGDAAC5, M2PE, 17, 1)
	FIELD(SYSMPU_RGDAAC5, M3UM, 18, 3)
	FIELD(SYSMPU_RGDAAC5, M3SM, 21, 2)
	FIELD(SYSMPU_RGDAAC5, M3PE, 23, 1)
	FIELD(SYSMPU_RGDAAC5, M4WE, 24, 1)
	FIELD(SYSMPU_RGDAAC5, M4RE, 25, 1)
	FIELD(SYSMPU_RGDAAC5, M5WE, 26, 1)
	FIELD(SYSMPU_RGDAAC5, M5RE, 27, 1)
	FIELD(SYSMPU_RGDAAC5, M6WE, 28, 1)
	FIELD(SYSMPU_RGDAAC5, M6RE, 29, 1)
	FIELD(SYSMPU_RGDAAC5, M7WE, 30, 1)
	FIELD(SYSMPU_RGDAAC5, M7RE, 31, 1)
REG32(SYSMPU_RGDAAC6, 0x818)
	FIELD(SYSMPU_RGDAAC6, M0UM, 0, 3)
	FIELD(SYSMPU_RGDAAC6, M0SM, 3, 2)
	FIELD(SYSMPU_RGDAAC6, M0PE, 5, 1)
	FIELD(SYSMPU_RGDAAC6, M1UM, 6, 3)
	FIELD(SYSMPU_RGDAAC6, M1SM, 9, 2)
	FIELD(SYSMPU_RGDAAC6, M1PE, 11, 1)
	FIELD(SYSMPU_RGDAAC6, M2UM, 12, 3)
	FIELD(SYSMPU_RGDAAC6, M2SM, 15, 2)
	FIELD(SYSMPU_RGDAAC6, M2PE, 17, 1)
	FIELD(SYSMPU_RGDAAC6, M3UM, 18, 3)
	FIELD(SYSMPU_RGDAAC6, M3SM, 21, 2)
	FIELD(SYSMPU_RGDAAC6, M3PE, 23, 1)
	FIELD(SYSMPU_RGDAAC6, M4WE, 24, 1)
	FIELD(SYSMPU_RGDAAC6, M4RE, 25, 1)
	FIELD(SYSMPU_RGDAAC6, M5WE, 26, 1)
	FIELD(SYSMPU_RGDAAC6, M5RE, 27, 1)
	FIELD(SYSMPU_RGDAAC6, M6WE, 28, 1)
	FIELD(SYSMPU_RGDAAC6, M6RE, 29, 1)
	FIELD(SYSMPU_RGDAAC6, M7WE, 30, 1)
	FIELD(SYSMPU_RGDAAC6, M7RE, 31, 1)
REG32(SYSMPU_RGDAAC7, 0x81c)
	FIELD(SYSMPU_RGDAAC7, M0UM, 0, 3)
	FIELD(SYSMPU_RGDAAC7, M0SM, 3, 2)
	FIELD(SYSMPU_RGDAAC7, M0PE, 5, 1)
	FIELD(SYSMPU_RGDAAC7, M1UM, 6, 3)
	FIELD(SYSMPU_RGDAAC7, M1SM, 9, 2)
	FIELD(SYSMPU_RGDAAC7, M1PE, 11, 1)
	FIELD(SYSMPU_RGDAAC7, M2UM, 12, 3)
	FIELD(SYSMPU_RGDAAC7, M2SM, 15, 2)
	FIELD(SYSMPU_RGDAAC7, M2PE, 17, 1)
	FIELD(SYSMPU_RGDAAC7, M3UM, 18, 3)
	FIELD(SYSMPU_RGDAAC7, M3SM, 21, 2)
	FIELD(SYSMPU_RGDAAC7, M3PE, 23, 1)
	FIELD(SYSMPU_RGDAAC7, M4WE, 24, 1)
	FIELD(SYSMPU_RGDAAC7, M4RE, 25, 1)
	FIELD(SYSMPU_RGDAAC7, M5WE, 26, 1)
	FIELD(SYSMPU_RGDAAC7, M5RE, 27, 1)
	FIELD(SYSMPU_RGDAAC7, M6WE, 28, 1)
	FIELD(SYSMPU_RGDAAC7, M6RE, 29, 1)
	FIELD(SYSMPU_RGDAAC7, M7WE, 30, 1)
	FIELD(SYSMPU_RGDAAC7, M7RE, 31, 1)
REG32(SYSMPU_RGDAAC8, 0x820)
	FIELD(SYSMPU_RGDAAC8, M0UM, 0, 3)
	FIELD(SYSMPU_RGDAAC8, M0SM, 3, 2)
	FIELD(SYSMPU_RGDAAC8, M0PE, 5, 1)
	FIELD(SYSMPU_RGDAAC8, M1UM, 6, 3)
	FIELD(SYSMPU_RGDAAC8, M1SM, 9, 2)
	FIELD(SYSMPU_RGDAAC8, M1PE, 11, 1)
	FIELD(SYSMPU_RGDAAC8, M2UM, 12, 3)
	FIELD(SYSMPU_RGDAAC8, M2SM, 15, 2)
	FIELD(SYSMPU_RGDAAC8, M2PE, 17, 1)
	FIELD(SYSMPU_RGDAAC8, M3UM, 18, 3)
	FIELD(SYSMPU_RGDAAC8, M3SM, 21, 2)
	FIELD(SYSMPU_RGDAAC8, M3PE, 23, 1)
	FIELD(SYSMPU_RGDAAC8, M4WE, 24, 1)
	FIELD(SYSMPU_RGDAAC8, M4RE, 25, 1)
	FIELD(SYSMPU_RGDAAC8, M5WE, 26, 1)
	FIELD(SYSMPU_RGDAAC8, M5RE, 27, 1)
	FIELD(SYSMPU_RGDAAC8, M6WE, 28, 1)
	FIELD(SYSMPU_RGDAAC8, M6RE, 29, 1)
	FIELD(SYSMPU_RGDAAC8, M7WE, 30, 1)
	FIELD(SYSMPU_RGDAAC8, M7RE, 31, 1)
REG32(SYSMPU_RGDAAC9, 0x824)
	FIELD(SYSMPU_RGDAAC9, M0UM, 0, 3)
	FIELD(SYSMPU_RGDAAC9, M0SM, 3, 2)
	FIELD(SYSMPU_RGDAAC9, M0PE, 5, 1)
	FIELD(SYSMPU_RGDAAC9, M1UM, 6, 3)
	FIELD(SYSMPU_RGDAAC9, M1SM, 9, 2)
	FIELD(SYSMPU_RGDAAC9, M1PE, 11, 1)
	FIELD(SYSMPU_RGDAAC9, M2UM, 12, 3)
	FIELD(SYSMPU_RGDAAC9, M2SM, 15, 2)
	FIELD(SYSMPU_RGDAAC9, M2PE, 17, 1)
	FIELD(SYSMPU_RGDAAC9, M3UM, 18, 3)
	FIELD(SYSMPU_RGDAAC9, M3SM, 21, 2)
	FIELD(SYSMPU_RGDAAC9, M3PE, 23, 1)
	FIELD(SYSMPU_RGDAAC9, M4WE, 24, 1)
	FIELD(SYSMPU_RGDAAC9, M4RE, 25, 1)
	FIELD(SYSMPU_RGDAAC9, M5WE, 26, 1)
	FIELD(SYSMPU_RGDAAC9, M5RE, 27, 1)
	FIELD(SYSMPU_RGDAAC9, M6WE, 28, 1)
	FIELD(SYSMPU_RGDAAC9, M6RE, 29, 1)
	FIELD(SYSMPU_RGDAAC9, M7WE, 30, 1)
	FIELD(SYSMPU_RGDAAC9, M7RE, 31, 1)
REG32(SYSMPU_RGDAAC10, 0x828)
	FIELD(SYSMPU_RGDAAC10, M0UM, 0, 3)
	FIELD(SYSMPU_RGDAAC10, M0SM, 3, 2)
	FIELD(SYSMPU_RGDAAC10, M0PE, 5, 1)
	FIELD(SYSMPU_RGDAAC10, M1UM, 6, 3)
	FIELD(SYSMPU_RGDAAC10, M1SM, 9, 2)
	FIELD(SYSMPU_RGDAAC10, M1PE, 11, 1)
	FIELD(SYSMPU_RGDAAC10, M2UM, 12, 3)
	FIELD(SYSMPU_RGDAAC10, M2SM, 15, 2)
	FIELD(SYSMPU_RGDAAC10, M2PE, 17, 1)
	FIELD(SYSMPU_RGDAAC10, M3UM, 18, 3)
	FIELD(SYSMPU_RGDAAC10, M3SM, 21, 2)
	FIELD(SYSMPU_RGDAAC10, M3PE, 23, 1)
	FIELD(SYSMPU_RGDAAC10, M4WE, 24, 1)
	FIELD(SYSMPU_RGDAAC10, M4RE, 25, 1)
	FIELD(SYSMPU_RGDAAC10, M5WE, 26, 1)
	FIELD(SYSMPU_RGDAAC10, M5RE, 27, 1)
	FIELD(SYSMPU_RGDAAC10, M6WE, 28, 1)
	FIELD(SYSMPU_RGDAAC10, M6RE, 29, 1)
	FIELD(SYSMPU_RGDAAC10, M7WE, 30, 1)
	FIELD(SYSMPU_RGDAAC10, M7RE, 31, 1)
REG32(SYSMPU_RGDAAC11, 0x82c)
	FIELD(SYSMPU_RGDAAC11, M0UM, 0, 3)
	FIELD(SYSMPU_RGDAAC11, M0SM, 3, 2)
	FIELD(SYSMPU_RGDAAC11, M0PE, 5, 1)
	FIELD(SYSMPU_RGDAAC11, M1UM, 6, 3)
	FIELD(SYSMPU_RGDAAC11, M1SM, 9, 2)
	FIELD(SYSMPU_RGDAAC11, M1PE, 11, 1)
	FIELD(SYSMPU_RGDAAC11, M2UM, 12, 3)
	FIELD(SYSMPU_RGDAAC11, M2SM, 15, 2)
	FIELD(SYSMPU_RGDAAC11, M2PE, 17, 1)
	FIELD(SYSMPU_RGDAAC11, M3UM, 18, 3)
	FIELD(SYSMPU_RGDAAC11, M3SM, 21, 2)
	FIELD(SYSMPU_RGDAAC11, M3PE, 23, 1)
	FIELD(SYSMPU_RGDAAC11, M4WE, 24, 1)
	FIELD(SYSMPU_RGDAAC11, M4RE, 25, 1)
	FIELD(SYSMPU_RGDAAC11, M5WE, 26, 1)
	FIELD(SYSMPU_RGDAAC11, M5RE, 27, 1)
	FIELD(SYSMPU_RGDAAC11, M6WE, 28, 1)
	FIELD(SYSMPU_RGDAAC11, M6RE, 29, 1)
	FIELD(SYSMPU_RGDAAC11, M7WE, 30, 1)
	FIELD(SYSMPU_RGDAAC11, M7RE, 31, 1)


static void frdmk82f_sysmpu_register_reset(FRDMK82FSYSMPU *t) {
	t->CESR = 0x815101;
	t->EAR0 = 0x0;
	t->EAR1 = 0x0;
	t->EAR2 = 0x0;
	t->EAR3 = 0x0;
	t->EAR4 = 0x0;
	t->EDR0 = 0x0;
	t->EDR1 = 0x0;
	t->EDR2 = 0x0;
	t->EDR3 = 0x0;
	t->EDR4 = 0x0;
	t->RGD0_WORD0 = 0x0;
	t->RGD1_WORD0 = 0x0;
	t->RGD2_WORD0 = 0x0;
	t->RGD3_WORD0 = 0x0;
	t->RGD4_WORD0 = 0x0;
	t->RGD5_WORD0 = 0x0;
	t->RGD6_WORD0 = 0x0;
	t->RGD7_WORD0 = 0x0;
	t->RGD8_WORD0 = 0x0;
	t->RGD9_WORD0 = 0x0;
	t->RGD10_WORD0 = 0x0;
	t->RGD11_WORD0 = 0x0;
	t->RGD0_WORD1 = 0xffffffff;
	t->RGD1_WORD1 = 0xffffffff;
	t->RGD2_WORD1 = 0xffffffff;
	t->RGD3_WORD1 = 0xffffffff;
	t->RGD4_WORD1 = 0xffffffff;
	t->RGD5_WORD1 = 0xffffffff;
	t->RGD6_WORD1 = 0xffffffff;
	t->RGD7_WORD1 = 0xffffffff;
	t->RGD8_WORD1 = 0xffffffff;
	t->RGD9_WORD1 = 0xffffffff;
	t->RGD10_WORD1 = 0xffffffff;
	t->RGD11_WORD1 = 0xffffffff;
	t->RGD0_WORD2 = 0x61f7df;
	t->RGD1_WORD2 = 0x61f7df;
	t->RGD2_WORD2 = 0x61f7df;
	t->RGD3_WORD2 = 0x61f7df;
	t->RGD4_WORD2 = 0x61f7df;
	t->RGD5_WORD2 = 0x61f7df;
	t->RGD6_WORD2 = 0x61f7df;
	t->RGD7_WORD2 = 0x61f7df;
	t->RGD8_WORD2 = 0x61f7df;
	t->RGD9_WORD2 = 0x61f7df;
	t->RGD10_WORD2 = 0x61f7df;
	t->RGD11_WORD2 = 0x61f7df;
	t->RGD0_WORD3 = 0x1;
	t->RGD1_WORD3 = 0x1;
	t->RGD2_WORD3 = 0x1;
	t->RGD3_WORD3 = 0x1;
	t->RGD4_WORD3 = 0x1;
	t->RGD5_WORD3 = 0x1;
	t->RGD6_WORD3 = 0x1;
	t->RGD7_WORD3 = 0x1;
	t->RGD8_WORD3 = 0x1;
	t->RGD9_WORD3 = 0x1;
	t->RGD10_WORD3 = 0x1;
	t->RGD11_WORD3 = 0x1;
	t->RGDAAC0 = 0x61f7df;
	t->RGDAAC1 = 0x61f7df;
	t->RGDAAC2 = 0x61f7df;
	t->RGDAAC3 = 0x61f7df;
	t->RGDAAC4 = 0x61f7df;
	t->RGDAAC5 = 0x61f7df;
	t->RGDAAC6 = 0x61f7df;
	t->RGDAAC7 = 0x61f7df;
	t->RGDAAC8 = 0x61f7df;
	t->RGDAAC9 = 0x61f7df;
	t->RGDAAC10 = 0x61f7df;
	t->RGDAAC11 = 0x61f7df;

}

static uint64_t frdmk82f_sysmpu_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMK82FSYSMPU *t = FRDMK82F_SYSMPU(opaque);
	uint64_t ret;

	switch (offset) {
		case A_SYSMPU_CESR:
			ret = t->CESR;
			break;
		case A_SYSMPU_EAR0:
			ret = t->EAR0;
			break;
		case A_SYSMPU_EAR1:
			ret = t->EAR1;
			break;
		case A_SYSMPU_EAR2:
			ret = t->EAR2;
			break;
		case A_SYSMPU_EAR3:
			ret = t->EAR3;
			break;
		case A_SYSMPU_EAR4:
			ret = t->EAR4;
			break;
		case A_SYSMPU_EDR0:
			ret = t->EDR0;
			break;
		case A_SYSMPU_EDR1:
			ret = t->EDR1;
			break;
		case A_SYSMPU_EDR2:
			ret = t->EDR2;
			break;
		case A_SYSMPU_EDR3:
			ret = t->EDR3;
			break;
		case A_SYSMPU_EDR4:
			ret = t->EDR4;
			break;
		case A_SYSMPU_RGD0_WORD0:
			ret = t->RGD0_WORD0;
			break;
		case A_SYSMPU_RGD1_WORD0:
			ret = t->RGD1_WORD0;
			break;
		case A_SYSMPU_RGD2_WORD0:
			ret = t->RGD2_WORD0;
			break;
		case A_SYSMPU_RGD3_WORD0:
			ret = t->RGD3_WORD0;
			break;
		case A_SYSMPU_RGD4_WORD0:
			ret = t->RGD4_WORD0;
			break;
		case A_SYSMPU_RGD5_WORD0:
			ret = t->RGD5_WORD0;
			break;
		case A_SYSMPU_RGD6_WORD0:
			ret = t->RGD6_WORD0;
			break;
		case A_SYSMPU_RGD7_WORD0:
			ret = t->RGD7_WORD0;
			break;
		case A_SYSMPU_RGD8_WORD0:
			ret = t->RGD8_WORD0;
			break;
		case A_SYSMPU_RGD9_WORD0:
			ret = t->RGD9_WORD0;
			break;
		case A_SYSMPU_RGD10_WORD0:
			ret = t->RGD10_WORD0;
			break;
		case A_SYSMPU_RGD11_WORD0:
			ret = t->RGD11_WORD0;
			break;
		case A_SYSMPU_RGD0_WORD1:
			ret = t->RGD0_WORD1;
			break;
		case A_SYSMPU_RGD1_WORD1:
			ret = t->RGD1_WORD1;
			break;
		case A_SYSMPU_RGD2_WORD1:
			ret = t->RGD2_WORD1;
			break;
		case A_SYSMPU_RGD3_WORD1:
			ret = t->RGD3_WORD1;
			break;
		case A_SYSMPU_RGD4_WORD1:
			ret = t->RGD4_WORD1;
			break;
		case A_SYSMPU_RGD5_WORD1:
			ret = t->RGD5_WORD1;
			break;
		case A_SYSMPU_RGD6_WORD1:
			ret = t->RGD6_WORD1;
			break;
		case A_SYSMPU_RGD7_WORD1:
			ret = t->RGD7_WORD1;
			break;
		case A_SYSMPU_RGD8_WORD1:
			ret = t->RGD8_WORD1;
			break;
		case A_SYSMPU_RGD9_WORD1:
			ret = t->RGD9_WORD1;
			break;
		case A_SYSMPU_RGD10_WORD1:
			ret = t->RGD10_WORD1;
			break;
		case A_SYSMPU_RGD11_WORD1:
			ret = t->RGD11_WORD1;
			break;
		case A_SYSMPU_RGD0_WORD2:
			ret = t->RGD0_WORD2;
			break;
		case A_SYSMPU_RGD1_WORD2:
			ret = t->RGD1_WORD2;
			break;
		case A_SYSMPU_RGD2_WORD2:
			ret = t->RGD2_WORD2;
			break;
		case A_SYSMPU_RGD3_WORD2:
			ret = t->RGD3_WORD2;
			break;
		case A_SYSMPU_RGD4_WORD2:
			ret = t->RGD4_WORD2;
			break;
		case A_SYSMPU_RGD5_WORD2:
			ret = t->RGD5_WORD2;
			break;
		case A_SYSMPU_RGD6_WORD2:
			ret = t->RGD6_WORD2;
			break;
		case A_SYSMPU_RGD7_WORD2:
			ret = t->RGD7_WORD2;
			break;
		case A_SYSMPU_RGD8_WORD2:
			ret = t->RGD8_WORD2;
			break;
		case A_SYSMPU_RGD9_WORD2:
			ret = t->RGD9_WORD2;
			break;
		case A_SYSMPU_RGD10_WORD2:
			ret = t->RGD10_WORD2;
			break;
		case A_SYSMPU_RGD11_WORD2:
			ret = t->RGD11_WORD2;
			break;
		case A_SYSMPU_RGD0_WORD3:
			ret = t->RGD0_WORD3;
			break;
		case A_SYSMPU_RGD1_WORD3:
			ret = t->RGD1_WORD3;
			break;
		case A_SYSMPU_RGD2_WORD3:
			ret = t->RGD2_WORD3;
			break;
		case A_SYSMPU_RGD3_WORD3:
			ret = t->RGD3_WORD3;
			break;
		case A_SYSMPU_RGD4_WORD3:
			ret = t->RGD4_WORD3;
			break;
		case A_SYSMPU_RGD5_WORD3:
			ret = t->RGD5_WORD3;
			break;
		case A_SYSMPU_RGD6_WORD3:
			ret = t->RGD6_WORD3;
			break;
		case A_SYSMPU_RGD7_WORD3:
			ret = t->RGD7_WORD3;
			break;
		case A_SYSMPU_RGD8_WORD3:
			ret = t->RGD8_WORD3;
			break;
		case A_SYSMPU_RGD9_WORD3:
			ret = t->RGD9_WORD3;
			break;
		case A_SYSMPU_RGD10_WORD3:
			ret = t->RGD10_WORD3;
			break;
		case A_SYSMPU_RGD11_WORD3:
			ret = t->RGD11_WORD3;
			break;
		case A_SYSMPU_RGDAAC0:
			ret = t->RGDAAC0;
			break;
		case A_SYSMPU_RGDAAC1:
			ret = t->RGDAAC1;
			break;
		case A_SYSMPU_RGDAAC2:
			ret = t->RGDAAC2;
			break;
		case A_SYSMPU_RGDAAC3:
			ret = t->RGDAAC3;
			break;
		case A_SYSMPU_RGDAAC4:
			ret = t->RGDAAC4;
			break;
		case A_SYSMPU_RGDAAC5:
			ret = t->RGDAAC5;
			break;
		case A_SYSMPU_RGDAAC6:
			ret = t->RGDAAC6;
			break;
		case A_SYSMPU_RGDAAC7:
			ret = t->RGDAAC7;
			break;
		case A_SYSMPU_RGDAAC8:
			ret = t->RGDAAC8;
			break;
		case A_SYSMPU_RGDAAC9:
			ret = t->RGDAAC9;
			break;
		case A_SYSMPU_RGDAAC10:
			ret = t->RGDAAC10;
			break;
		case A_SYSMPU_RGDAAC11:
			ret = t->RGDAAC11;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK82F SYSMPU read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmk82f_sysmpu_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMK82FSYSMPU *t = FRDMK82F_SYSMPU(opaque);


	switch (offset) {
		case A_SYSMPU_CESR ... A_SYSMPU_CESR + 3:
			value = value << ((offset - A_SYSMPU_CESR) << 3);
			offset = A_SYSMPU_CESR;
			break;
		case A_SYSMPU_RGD0_WORD0 ... A_SYSMPU_RGD0_WORD0 + 3:
			value = value << ((offset - A_SYSMPU_RGD0_WORD0) << 3);
			offset = A_SYSMPU_RGD0_WORD0;
			break;
		case A_SYSMPU_RGD1_WORD0 ... A_SYSMPU_RGD1_WORD0 + 3:
			value = value << ((offset - A_SYSMPU_RGD1_WORD0) << 3);
			offset = A_SYSMPU_RGD1_WORD0;
			break;
		case A_SYSMPU_RGD2_WORD0 ... A_SYSMPU_RGD2_WORD0 + 3:
			value = value << ((offset - A_SYSMPU_RGD2_WORD0) << 3);
			offset = A_SYSMPU_RGD2_WORD0;
			break;
		case A_SYSMPU_RGD3_WORD0 ... A_SYSMPU_RGD3_WORD0 + 3:
			value = value << ((offset - A_SYSMPU_RGD3_WORD0) << 3);
			offset = A_SYSMPU_RGD3_WORD0;
			break;
		case A_SYSMPU_RGD4_WORD0 ... A_SYSMPU_RGD4_WORD0 + 3:
			value = value << ((offset - A_SYSMPU_RGD4_WORD0) << 3);
			offset = A_SYSMPU_RGD4_WORD0;
			break;
		case A_SYSMPU_RGD5_WORD0 ... A_SYSMPU_RGD5_WORD0 + 3:
			value = value << ((offset - A_SYSMPU_RGD5_WORD0) << 3);
			offset = A_SYSMPU_RGD5_WORD0;
			break;
		case A_SYSMPU_RGD6_WORD0 ... A_SYSMPU_RGD6_WORD0 + 3:
			value = value << ((offset - A_SYSMPU_RGD6_WORD0) << 3);
			offset = A_SYSMPU_RGD6_WORD0;
			break;
		case A_SYSMPU_RGD7_WORD0 ... A_SYSMPU_RGD7_WORD0 + 3:
			value = value << ((offset - A_SYSMPU_RGD7_WORD0) << 3);
			offset = A_SYSMPU_RGD7_WORD0;
			break;
		case A_SYSMPU_RGD8_WORD0 ... A_SYSMPU_RGD8_WORD0 + 3:
			value = value << ((offset - A_SYSMPU_RGD8_WORD0) << 3);
			offset = A_SYSMPU_RGD8_WORD0;
			break;
		case A_SYSMPU_RGD9_WORD0 ... A_SYSMPU_RGD9_WORD0 + 3:
			value = value << ((offset - A_SYSMPU_RGD9_WORD0) << 3);
			offset = A_SYSMPU_RGD9_WORD0;
			break;
		case A_SYSMPU_RGD10_WORD0 ... A_SYSMPU_RGD10_WORD0 + 3:
			value = value << ((offset - A_SYSMPU_RGD10_WORD0) << 3);
			offset = A_SYSMPU_RGD10_WORD0;
			break;
		case A_SYSMPU_RGD11_WORD0 ... A_SYSMPU_RGD11_WORD0 + 3:
			value = value << ((offset - A_SYSMPU_RGD11_WORD0) << 3);
			offset = A_SYSMPU_RGD11_WORD0;
			break;
		case A_SYSMPU_RGD0_WORD1 ... A_SYSMPU_RGD0_WORD1 + 3:
			value = value << ((offset - A_SYSMPU_RGD0_WORD1) << 3);
			offset = A_SYSMPU_RGD0_WORD1;
			break;
		case A_SYSMPU_RGD1_WORD1 ... A_SYSMPU_RGD1_WORD1 + 3:
			value = value << ((offset - A_SYSMPU_RGD1_WORD1) << 3);
			offset = A_SYSMPU_RGD1_WORD1;
			break;
		case A_SYSMPU_RGD2_WORD1 ... A_SYSMPU_RGD2_WORD1 + 3:
			value = value << ((offset - A_SYSMPU_RGD2_WORD1) << 3);
			offset = A_SYSMPU_RGD2_WORD1;
			break;
		case A_SYSMPU_RGD3_WORD1 ... A_SYSMPU_RGD3_WORD1 + 3:
			value = value << ((offset - A_SYSMPU_RGD3_WORD1) << 3);
			offset = A_SYSMPU_RGD3_WORD1;
			break;
		case A_SYSMPU_RGD4_WORD1 ... A_SYSMPU_RGD4_WORD1 + 3:
			value = value << ((offset - A_SYSMPU_RGD4_WORD1) << 3);
			offset = A_SYSMPU_RGD4_WORD1;
			break;
		case A_SYSMPU_RGD5_WORD1 ... A_SYSMPU_RGD5_WORD1 + 3:
			value = value << ((offset - A_SYSMPU_RGD5_WORD1) << 3);
			offset = A_SYSMPU_RGD5_WORD1;
			break;
		case A_SYSMPU_RGD6_WORD1 ... A_SYSMPU_RGD6_WORD1 + 3:
			value = value << ((offset - A_SYSMPU_RGD6_WORD1) << 3);
			offset = A_SYSMPU_RGD6_WORD1;
			break;
		case A_SYSMPU_RGD7_WORD1 ... A_SYSMPU_RGD7_WORD1 + 3:
			value = value << ((offset - A_SYSMPU_RGD7_WORD1) << 3);
			offset = A_SYSMPU_RGD7_WORD1;
			break;
		case A_SYSMPU_RGD8_WORD1 ... A_SYSMPU_RGD8_WORD1 + 3:
			value = value << ((offset - A_SYSMPU_RGD8_WORD1) << 3);
			offset = A_SYSMPU_RGD8_WORD1;
			break;
		case A_SYSMPU_RGD9_WORD1 ... A_SYSMPU_RGD9_WORD1 + 3:
			value = value << ((offset - A_SYSMPU_RGD9_WORD1) << 3);
			offset = A_SYSMPU_RGD9_WORD1;
			break;
		case A_SYSMPU_RGD10_WORD1 ... A_SYSMPU_RGD10_WORD1 + 3:
			value = value << ((offset - A_SYSMPU_RGD10_WORD1) << 3);
			offset = A_SYSMPU_RGD10_WORD1;
			break;
		case A_SYSMPU_RGD11_WORD1 ... A_SYSMPU_RGD11_WORD1 + 3:
			value = value << ((offset - A_SYSMPU_RGD11_WORD1) << 3);
			offset = A_SYSMPU_RGD11_WORD1;
			break;
		case A_SYSMPU_RGD0_WORD2 ... A_SYSMPU_RGD0_WORD2 + 3:
			value = value << ((offset - A_SYSMPU_RGD0_WORD2) << 3);
			offset = A_SYSMPU_RGD0_WORD2;
			break;
		case A_SYSMPU_RGD1_WORD2 ... A_SYSMPU_RGD1_WORD2 + 3:
			value = value << ((offset - A_SYSMPU_RGD1_WORD2) << 3);
			offset = A_SYSMPU_RGD1_WORD2;
			break;
		case A_SYSMPU_RGD2_WORD2 ... A_SYSMPU_RGD2_WORD2 + 3:
			value = value << ((offset - A_SYSMPU_RGD2_WORD2) << 3);
			offset = A_SYSMPU_RGD2_WORD2;
			break;
		case A_SYSMPU_RGD3_WORD2 ... A_SYSMPU_RGD3_WORD2 + 3:
			value = value << ((offset - A_SYSMPU_RGD3_WORD2) << 3);
			offset = A_SYSMPU_RGD3_WORD2;
			break;
		case A_SYSMPU_RGD4_WORD2 ... A_SYSMPU_RGD4_WORD2 + 3:
			value = value << ((offset - A_SYSMPU_RGD4_WORD2) << 3);
			offset = A_SYSMPU_RGD4_WORD2;
			break;
		case A_SYSMPU_RGD5_WORD2 ... A_SYSMPU_RGD5_WORD2 + 3:
			value = value << ((offset - A_SYSMPU_RGD5_WORD2) << 3);
			offset = A_SYSMPU_RGD5_WORD2;
			break;
		case A_SYSMPU_RGD6_WORD2 ... A_SYSMPU_RGD6_WORD2 + 3:
			value = value << ((offset - A_SYSMPU_RGD6_WORD2) << 3);
			offset = A_SYSMPU_RGD6_WORD2;
			break;
		case A_SYSMPU_RGD7_WORD2 ... A_SYSMPU_RGD7_WORD2 + 3:
			value = value << ((offset - A_SYSMPU_RGD7_WORD2) << 3);
			offset = A_SYSMPU_RGD7_WORD2;
			break;
		case A_SYSMPU_RGD8_WORD2 ... A_SYSMPU_RGD8_WORD2 + 3:
			value = value << ((offset - A_SYSMPU_RGD8_WORD2) << 3);
			offset = A_SYSMPU_RGD8_WORD2;
			break;
		case A_SYSMPU_RGD9_WORD2 ... A_SYSMPU_RGD9_WORD2 + 3:
			value = value << ((offset - A_SYSMPU_RGD9_WORD2) << 3);
			offset = A_SYSMPU_RGD9_WORD2;
			break;
		case A_SYSMPU_RGD10_WORD2 ... A_SYSMPU_RGD10_WORD2 + 3:
			value = value << ((offset - A_SYSMPU_RGD10_WORD2) << 3);
			offset = A_SYSMPU_RGD10_WORD2;
			break;
		case A_SYSMPU_RGD11_WORD2 ... A_SYSMPU_RGD11_WORD2 + 3:
			value = value << ((offset - A_SYSMPU_RGD11_WORD2) << 3);
			offset = A_SYSMPU_RGD11_WORD2;
			break;
		case A_SYSMPU_RGD0_WORD3 ... A_SYSMPU_RGD0_WORD3 + 3:
			value = value << ((offset - A_SYSMPU_RGD0_WORD3) << 3);
			offset = A_SYSMPU_RGD0_WORD3;
			break;
		case A_SYSMPU_RGD1_WORD3 ... A_SYSMPU_RGD1_WORD3 + 3:
			value = value << ((offset - A_SYSMPU_RGD1_WORD3) << 3);
			offset = A_SYSMPU_RGD1_WORD3;
			break;
		case A_SYSMPU_RGD2_WORD3 ... A_SYSMPU_RGD2_WORD3 + 3:
			value = value << ((offset - A_SYSMPU_RGD2_WORD3) << 3);
			offset = A_SYSMPU_RGD2_WORD3;
			break;
		case A_SYSMPU_RGD3_WORD3 ... A_SYSMPU_RGD3_WORD3 + 3:
			value = value << ((offset - A_SYSMPU_RGD3_WORD3) << 3);
			offset = A_SYSMPU_RGD3_WORD3;
			break;
		case A_SYSMPU_RGD4_WORD3 ... A_SYSMPU_RGD4_WORD3 + 3:
			value = value << ((offset - A_SYSMPU_RGD4_WORD3) << 3);
			offset = A_SYSMPU_RGD4_WORD3;
			break;
		case A_SYSMPU_RGD5_WORD3 ... A_SYSMPU_RGD5_WORD3 + 3:
			value = value << ((offset - A_SYSMPU_RGD5_WORD3) << 3);
			offset = A_SYSMPU_RGD5_WORD3;
			break;
		case A_SYSMPU_RGD6_WORD3 ... A_SYSMPU_RGD6_WORD3 + 3:
			value = value << ((offset - A_SYSMPU_RGD6_WORD3) << 3);
			offset = A_SYSMPU_RGD6_WORD3;
			break;
		case A_SYSMPU_RGD7_WORD3 ... A_SYSMPU_RGD7_WORD3 + 3:
			value = value << ((offset - A_SYSMPU_RGD7_WORD3) << 3);
			offset = A_SYSMPU_RGD7_WORD3;
			break;
		case A_SYSMPU_RGD8_WORD3 ... A_SYSMPU_RGD8_WORD3 + 3:
			value = value << ((offset - A_SYSMPU_RGD8_WORD3) << 3);
			offset = A_SYSMPU_RGD8_WORD3;
			break;
		case A_SYSMPU_RGD9_WORD3 ... A_SYSMPU_RGD9_WORD3 + 3:
			value = value << ((offset - A_SYSMPU_RGD9_WORD3) << 3);
			offset = A_SYSMPU_RGD9_WORD3;
			break;
		case A_SYSMPU_RGD10_WORD3 ... A_SYSMPU_RGD10_WORD3 + 3:
			value = value << ((offset - A_SYSMPU_RGD10_WORD3) << 3);
			offset = A_SYSMPU_RGD10_WORD3;
			break;
		case A_SYSMPU_RGD11_WORD3 ... A_SYSMPU_RGD11_WORD3 + 3:
			value = value << ((offset - A_SYSMPU_RGD11_WORD3) << 3);
			offset = A_SYSMPU_RGD11_WORD3;
			break;
		case A_SYSMPU_RGDAAC0 ... A_SYSMPU_RGDAAC0 + 3:
			value = value << ((offset - A_SYSMPU_RGDAAC0) << 3);
			offset = A_SYSMPU_RGDAAC0;
			break;
		case A_SYSMPU_RGDAAC1 ... A_SYSMPU_RGDAAC1 + 3:
			value = value << ((offset - A_SYSMPU_RGDAAC1) << 3);
			offset = A_SYSMPU_RGDAAC1;
			break;
		case A_SYSMPU_RGDAAC2 ... A_SYSMPU_RGDAAC2 + 3:
			value = value << ((offset - A_SYSMPU_RGDAAC2) << 3);
			offset = A_SYSMPU_RGDAAC2;
			break;
		case A_SYSMPU_RGDAAC3 ... A_SYSMPU_RGDAAC3 + 3:
			value = value << ((offset - A_SYSMPU_RGDAAC3) << 3);
			offset = A_SYSMPU_RGDAAC3;
			break;
		case A_SYSMPU_RGDAAC4 ... A_SYSMPU_RGDAAC4 + 3:
			value = value << ((offset - A_SYSMPU_RGDAAC4) << 3);
			offset = A_SYSMPU_RGDAAC4;
			break;
		case A_SYSMPU_RGDAAC5 ... A_SYSMPU_RGDAAC5 + 3:
			value = value << ((offset - A_SYSMPU_RGDAAC5) << 3);
			offset = A_SYSMPU_RGDAAC5;
			break;
		case A_SYSMPU_RGDAAC6 ... A_SYSMPU_RGDAAC6 + 3:
			value = value << ((offset - A_SYSMPU_RGDAAC6) << 3);
			offset = A_SYSMPU_RGDAAC6;
			break;
		case A_SYSMPU_RGDAAC7 ... A_SYSMPU_RGDAAC7 + 3:
			value = value << ((offset - A_SYSMPU_RGDAAC7) << 3);
			offset = A_SYSMPU_RGDAAC7;
			break;
		case A_SYSMPU_RGDAAC8 ... A_SYSMPU_RGDAAC8 + 3:
			value = value << ((offset - A_SYSMPU_RGDAAC8) << 3);
			offset = A_SYSMPU_RGDAAC8;
			break;
		case A_SYSMPU_RGDAAC9 ... A_SYSMPU_RGDAAC9 + 3:
			value = value << ((offset - A_SYSMPU_RGDAAC9) << 3);
			offset = A_SYSMPU_RGDAAC9;
			break;
		case A_SYSMPU_RGDAAC10 ... A_SYSMPU_RGDAAC10 + 3:
			value = value << ((offset - A_SYSMPU_RGDAAC10) << 3);
			offset = A_SYSMPU_RGDAAC10;
			break;
		case A_SYSMPU_RGDAAC11 ... A_SYSMPU_RGDAAC11 + 3:
			value = value << ((offset - A_SYSMPU_RGDAAC11) << 3);
			offset = A_SYSMPU_RGDAAC11;
			break;

		default: break;
	}

	switch (offset) {
		case A_SYSMPU_CESR:
			t->CESR = value;
			break;
		case A_SYSMPU_RGD0_WORD0:
			t->RGD0_WORD0 = value;
			break;
		case A_SYSMPU_RGD1_WORD0:
			t->RGD1_WORD0 = value;
			break;
		case A_SYSMPU_RGD2_WORD0:
			t->RGD2_WORD0 = value;
			break;
		case A_SYSMPU_RGD3_WORD0:
			t->RGD3_WORD0 = value;
			break;
		case A_SYSMPU_RGD4_WORD0:
			t->RGD4_WORD0 = value;
			break;
		case A_SYSMPU_RGD5_WORD0:
			t->RGD5_WORD0 = value;
			break;
		case A_SYSMPU_RGD6_WORD0:
			t->RGD6_WORD0 = value;
			break;
		case A_SYSMPU_RGD7_WORD0:
			t->RGD7_WORD0 = value;
			break;
		case A_SYSMPU_RGD8_WORD0:
			t->RGD8_WORD0 = value;
			break;
		case A_SYSMPU_RGD9_WORD0:
			t->RGD9_WORD0 = value;
			break;
		case A_SYSMPU_RGD10_WORD0:
			t->RGD10_WORD0 = value;
			break;
		case A_SYSMPU_RGD11_WORD0:
			t->RGD11_WORD0 = value;
			break;
		case A_SYSMPU_RGD0_WORD1:
			t->RGD0_WORD1 = value;
			break;
		case A_SYSMPU_RGD1_WORD1:
			t->RGD1_WORD1 = value;
			break;
		case A_SYSMPU_RGD2_WORD1:
			t->RGD2_WORD1 = value;
			break;
		case A_SYSMPU_RGD3_WORD1:
			t->RGD3_WORD1 = value;
			break;
		case A_SYSMPU_RGD4_WORD1:
			t->RGD4_WORD1 = value;
			break;
		case A_SYSMPU_RGD5_WORD1:
			t->RGD5_WORD1 = value;
			break;
		case A_SYSMPU_RGD6_WORD1:
			t->RGD6_WORD1 = value;
			break;
		case A_SYSMPU_RGD7_WORD1:
			t->RGD7_WORD1 = value;
			break;
		case A_SYSMPU_RGD8_WORD1:
			t->RGD8_WORD1 = value;
			break;
		case A_SYSMPU_RGD9_WORD1:
			t->RGD9_WORD1 = value;
			break;
		case A_SYSMPU_RGD10_WORD1:
			t->RGD10_WORD1 = value;
			break;
		case A_SYSMPU_RGD11_WORD1:
			t->RGD11_WORD1 = value;
			break;
		case A_SYSMPU_RGD0_WORD2:
			t->RGD0_WORD2 = value;
			break;
		case A_SYSMPU_RGD1_WORD2:
			t->RGD1_WORD2 = value;
			break;
		case A_SYSMPU_RGD2_WORD2:
			t->RGD2_WORD2 = value;
			break;
		case A_SYSMPU_RGD3_WORD2:
			t->RGD3_WORD2 = value;
			break;
		case A_SYSMPU_RGD4_WORD2:
			t->RGD4_WORD2 = value;
			break;
		case A_SYSMPU_RGD5_WORD2:
			t->RGD5_WORD2 = value;
			break;
		case A_SYSMPU_RGD6_WORD2:
			t->RGD6_WORD2 = value;
			break;
		case A_SYSMPU_RGD7_WORD2:
			t->RGD7_WORD2 = value;
			break;
		case A_SYSMPU_RGD8_WORD2:
			t->RGD8_WORD2 = value;
			break;
		case A_SYSMPU_RGD9_WORD2:
			t->RGD9_WORD2 = value;
			break;
		case A_SYSMPU_RGD10_WORD2:
			t->RGD10_WORD2 = value;
			break;
		case A_SYSMPU_RGD11_WORD2:
			t->RGD11_WORD2 = value;
			break;
		case A_SYSMPU_RGD0_WORD3:
			t->RGD0_WORD3 = value;
			break;
		case A_SYSMPU_RGD1_WORD3:
			t->RGD1_WORD3 = value;
			break;
		case A_SYSMPU_RGD2_WORD3:
			t->RGD2_WORD3 = value;
			break;
		case A_SYSMPU_RGD3_WORD3:
			t->RGD3_WORD3 = value;
			break;
		case A_SYSMPU_RGD4_WORD3:
			t->RGD4_WORD3 = value;
			break;
		case A_SYSMPU_RGD5_WORD3:
			t->RGD5_WORD3 = value;
			break;
		case A_SYSMPU_RGD6_WORD3:
			t->RGD6_WORD3 = value;
			break;
		case A_SYSMPU_RGD7_WORD3:
			t->RGD7_WORD3 = value;
			break;
		case A_SYSMPU_RGD8_WORD3:
			t->RGD8_WORD3 = value;
			break;
		case A_SYSMPU_RGD9_WORD3:
			t->RGD9_WORD3 = value;
			break;
		case A_SYSMPU_RGD10_WORD3:
			t->RGD10_WORD3 = value;
			break;
		case A_SYSMPU_RGD11_WORD3:
			t->RGD11_WORD3 = value;
			break;
		case A_SYSMPU_RGDAAC0:
			t->RGDAAC0 = value;
			break;
		case A_SYSMPU_RGDAAC1:
			t->RGDAAC1 = value;
			break;
		case A_SYSMPU_RGDAAC2:
			t->RGDAAC2 = value;
			break;
		case A_SYSMPU_RGDAAC3:
			t->RGDAAC3 = value;
			break;
		case A_SYSMPU_RGDAAC4:
			t->RGDAAC4 = value;
			break;
		case A_SYSMPU_RGDAAC5:
			t->RGDAAC5 = value;
			break;
		case A_SYSMPU_RGDAAC6:
			t->RGDAAC6 = value;
			break;
		case A_SYSMPU_RGDAAC7:
			t->RGDAAC7 = value;
			break;
		case A_SYSMPU_RGDAAC8:
			t->RGDAAC8 = value;
			break;
		case A_SYSMPU_RGDAAC9:
			t->RGDAAC9 = value;
			break;
		case A_SYSMPU_RGDAAC10:
			t->RGDAAC10 = value;
			break;
		case A_SYSMPU_RGDAAC11:
			t->RGDAAC11 = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK82F SYSMPU write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmk82f_sysmpu_ops = {
	.read = frdmk82f_sysmpu_read,
	.write = frdmk82f_sysmpu_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmk82f_sysmpu_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMK82FSYSMPU *t = FRDMK82F_SYSMPU(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmk82f_sysmpu_ops, t, "frdmk82f-sysmpu", FRDMK82F_SYSMPU_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void frdmk82f_sysmpu_realize(DeviceState *dev, Error **errp) {
	return;

}

static void frdmk82f_sysmpu_reset_enter(Object *obj, ResetType type) {
	FRDMK82FSYSMPU *t = FRDMK82F_SYSMPU(obj);
	frdmk82f_sysmpu_register_reset(t);
}

static const VMStateDescription frdmk82f_sysmpu_vmstate = {
	.name = "frdmk82f-sysmpu",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CESR, FRDMK82FSYSMPU),
		VMSTATE_UINT32(EAR0, FRDMK82FSYSMPU),
		VMSTATE_UINT32(EAR1, FRDMK82FSYSMPU),
		VMSTATE_UINT32(EAR2, FRDMK82FSYSMPU),
		VMSTATE_UINT32(EAR3, FRDMK82FSYSMPU),
		VMSTATE_UINT32(EAR4, FRDMK82FSYSMPU),
		VMSTATE_UINT32(EDR0, FRDMK82FSYSMPU),
		VMSTATE_UINT32(EDR1, FRDMK82FSYSMPU),
		VMSTATE_UINT32(EDR2, FRDMK82FSYSMPU),
		VMSTATE_UINT32(EDR3, FRDMK82FSYSMPU),
		VMSTATE_UINT32(EDR4, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD0_WORD0, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD1_WORD0, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD2_WORD0, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD3_WORD0, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD4_WORD0, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD5_WORD0, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD6_WORD0, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD7_WORD0, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD8_WORD0, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD9_WORD0, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD10_WORD0, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD11_WORD0, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD0_WORD1, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD1_WORD1, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD2_WORD1, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD3_WORD1, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD4_WORD1, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD5_WORD1, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD6_WORD1, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD7_WORD1, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD8_WORD1, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD9_WORD1, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD10_WORD1, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD11_WORD1, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD0_WORD2, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD1_WORD2, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD2_WORD2, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD3_WORD2, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD4_WORD2, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD5_WORD2, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD6_WORD2, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD7_WORD2, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD8_WORD2, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD9_WORD2, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD10_WORD2, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD11_WORD2, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD0_WORD3, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD1_WORD3, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD2_WORD3, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD3_WORD3, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD4_WORD3, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD5_WORD3, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD6_WORD3, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD7_WORD3, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD8_WORD3, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD9_WORD3, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD10_WORD3, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGD11_WORD3, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGDAAC0, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGDAAC1, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGDAAC2, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGDAAC3, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGDAAC4, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGDAAC5, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGDAAC6, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGDAAC7, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGDAAC8, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGDAAC9, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGDAAC10, FRDMK82FSYSMPU),
		VMSTATE_UINT32(RGDAAC11, FRDMK82FSYSMPU),

		VMSTATE_END_OF_LIST()
	}
};

static void frdmk82f_sysmpu_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmk82f_sysmpu_vmstate;
	dc->realize = frdmk82f_sysmpu_realize;
	rc->phases.enter = frdmk82f_sysmpu_reset_enter;

}

static const TypeInfo frdmk82f_sysmpu_info = {
	.name = TYPE_FRDMK82F_SYSMPU,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMK82FSYSMPU),
	.instance_init = frdmk82f_sysmpu_init,
	.class_init = frdmk82f_sysmpu_class_init,
};

static void frdmk82f_sysmpu_register_types(void) {
	type_register_static(&frdmk82f_sysmpu_info);
}

type_init(frdmk82f_sysmpu_register_types);

#define TYPE_FRDMK82F_PIT "frdmk82f-pit"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMK82FPIT, FRDMK82F_PIT)

struct FRDMK82FPIT {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[4];

	/*registers*/
	uint32_t MCR;
	uint32_t LTMR64H;
	uint32_t LTMR64L;
	uint32_t LDVAL0;
	uint32_t LDVAL1;
	uint32_t LDVAL2;
	uint32_t LDVAL3;
	uint32_t CVAL0;
	uint32_t CVAL1;
	uint32_t CVAL2;
	uint32_t CVAL3;
	uint32_t TCTRL0;
	uint32_t TCTRL1;
	uint32_t TCTRL2;
	uint32_t TCTRL3;
	uint32_t TFLG0;
	uint32_t TFLG1;
	uint32_t TFLG2;
	uint32_t TFLG3;
	/* base */
	uint32_t base;

};
#define FRDMK82F_PIT_SIZE				0x140

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
REG32(PIT_LDVAL2, 0x120)
	FIELD(PIT_LDVAL2, TSV, 0, 32)
REG32(PIT_LDVAL3, 0x130)
	FIELD(PIT_LDVAL3, TSV, 0, 32)
REG32(PIT_CVAL0, 0x104)
	FIELD(PIT_CVAL0, TVL, 0, 32)
REG32(PIT_CVAL1, 0x114)
	FIELD(PIT_CVAL1, TVL, 0, 32)
REG32(PIT_CVAL2, 0x124)
	FIELD(PIT_CVAL2, TVL, 0, 32)
REG32(PIT_CVAL3, 0x134)
	FIELD(PIT_CVAL3, TVL, 0, 32)
REG32(PIT_TCTRL0, 0x108)
	FIELD(PIT_TCTRL0, TEN, 0, 1)
	FIELD(PIT_TCTRL0, TIE, 1, 1)
	FIELD(PIT_TCTRL0, CHN, 2, 1)
REG32(PIT_TCTRL1, 0x118)
	FIELD(PIT_TCTRL1, TEN, 0, 1)
	FIELD(PIT_TCTRL1, TIE, 1, 1)
	FIELD(PIT_TCTRL1, CHN, 2, 1)
REG32(PIT_TCTRL2, 0x128)
	FIELD(PIT_TCTRL2, TEN, 0, 1)
	FIELD(PIT_TCTRL2, TIE, 1, 1)
	FIELD(PIT_TCTRL2, CHN, 2, 1)
REG32(PIT_TCTRL3, 0x138)
	FIELD(PIT_TCTRL3, TEN, 0, 1)
	FIELD(PIT_TCTRL3, TIE, 1, 1)
	FIELD(PIT_TCTRL3, CHN, 2, 1)
REG32(PIT_TFLG0, 0x10c)
	FIELD(PIT_TFLG0, TIF, 0, 1)
REG32(PIT_TFLG1, 0x11c)
	FIELD(PIT_TFLG1, TIF, 0, 1)
REG32(PIT_TFLG2, 0x12c)
	FIELD(PIT_TFLG2, TIF, 0, 1)
REG32(PIT_TFLG3, 0x13c)
	FIELD(PIT_TFLG3, TIF, 0, 1)


static void frdmk82f_pit_register_reset(FRDMK82FPIT *t) {
	t->MCR = 0x2;
	t->LTMR64H = 0x0;
	t->LTMR64L = 0x0;
	t->LDVAL0 = 0x0;
	t->LDVAL1 = 0x0;
	t->LDVAL2 = 0x0;
	t->LDVAL3 = 0x0;
	t->CVAL0 = 0x0;
	t->CVAL1 = 0x0;
	t->CVAL2 = 0x0;
	t->CVAL3 = 0x0;
	t->TCTRL0 = 0x0;
	t->TCTRL1 = 0x0;
	t->TCTRL2 = 0x0;
	t->TCTRL3 = 0x0;
	t->TFLG0 = 0x0;
	t->TFLG1 = 0x0;
	t->TFLG2 = 0x0;
	t->TFLG3 = 0x0;

}

static uint64_t frdmk82f_pit_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMK82FPIT *t = FRDMK82F_PIT(opaque);
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
		case A_PIT_LDVAL2:
			ret = t->LDVAL2;
			break;
		case A_PIT_LDVAL3:
			ret = t->LDVAL3;
			break;
		case A_PIT_CVAL0:
			ret = t->CVAL0;
			break;
		case A_PIT_CVAL1:
			ret = t->CVAL1;
			break;
		case A_PIT_CVAL2:
			ret = t->CVAL2;
			break;
		case A_PIT_CVAL3:
			ret = t->CVAL3;
			break;
		case A_PIT_TCTRL0:
			ret = t->TCTRL0;
			break;
		case A_PIT_TCTRL1:
			ret = t->TCTRL1;
			break;
		case A_PIT_TCTRL2:
			ret = t->TCTRL2;
			break;
		case A_PIT_TCTRL3:
			ret = t->TCTRL3;
			break;
		case A_PIT_TFLG0:
			ret = t->TFLG0;
			break;
		case A_PIT_TFLG1:
			ret = t->TFLG1;
			break;
		case A_PIT_TFLG2:
			ret = t->TFLG2;
			break;
		case A_PIT_TFLG3:
			ret = t->TFLG3;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK82F PIT read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmk82f_pit_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMK82FPIT *t = FRDMK82F_PIT(opaque);


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
		case A_PIT_LDVAL2 ... A_PIT_LDVAL2 + 3:
			value = value << ((offset - A_PIT_LDVAL2) << 3);
			offset = A_PIT_LDVAL2;
			break;
		case A_PIT_LDVAL3 ... A_PIT_LDVAL3 + 3:
			value = value << ((offset - A_PIT_LDVAL3) << 3);
			offset = A_PIT_LDVAL3;
			break;
		case A_PIT_TCTRL0 ... A_PIT_TCTRL0 + 3:
			value = value << ((offset - A_PIT_TCTRL0) << 3);
			offset = A_PIT_TCTRL0;
			break;
		case A_PIT_TCTRL1 ... A_PIT_TCTRL1 + 3:
			value = value << ((offset - A_PIT_TCTRL1) << 3);
			offset = A_PIT_TCTRL1;
			break;
		case A_PIT_TCTRL2 ... A_PIT_TCTRL2 + 3:
			value = value << ((offset - A_PIT_TCTRL2) << 3);
			offset = A_PIT_TCTRL2;
			break;
		case A_PIT_TCTRL3 ... A_PIT_TCTRL3 + 3:
			value = value << ((offset - A_PIT_TCTRL3) << 3);
			offset = A_PIT_TCTRL3;
			break;
		case A_PIT_TFLG0 ... A_PIT_TFLG0 + 3:
			value = value << ((offset - A_PIT_TFLG0) << 3);
			offset = A_PIT_TFLG0;
			break;
		case A_PIT_TFLG1 ... A_PIT_TFLG1 + 3:
			value = value << ((offset - A_PIT_TFLG1) << 3);
			offset = A_PIT_TFLG1;
			break;
		case A_PIT_TFLG2 ... A_PIT_TFLG2 + 3:
			value = value << ((offset - A_PIT_TFLG2) << 3);
			offset = A_PIT_TFLG2;
			break;
		case A_PIT_TFLG3 ... A_PIT_TFLG3 + 3:
			value = value << ((offset - A_PIT_TFLG3) << 3);
			offset = A_PIT_TFLG3;
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
		case A_PIT_LDVAL2:
			t->LDVAL2 = value;
			break;
		case A_PIT_LDVAL3:
			t->LDVAL3 = value;
			break;
		case A_PIT_TCTRL0:
			t->TCTRL0 = value;
			break;
		case A_PIT_TCTRL1:
			t->TCTRL1 = value;
			break;
		case A_PIT_TCTRL2:
			t->TCTRL2 = value;
			break;
		case A_PIT_TCTRL3:
			t->TCTRL3 = value;
			break;
		case A_PIT_TFLG0:
			t->TFLG0 = value;
			break;
		case A_PIT_TFLG1:
			t->TFLG1 = value;
			break;
		case A_PIT_TFLG2:
			t->TFLG2 = value;
			break;
		case A_PIT_TFLG3:
			t->TFLG3 = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK82F PIT write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmk82f_pit_ops = {
	.read = frdmk82f_pit_read,
	.write = frdmk82f_pit_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmk82f_pit_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMK82FPIT *t = FRDMK82F_PIT(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmk82f_pit_ops, t, "frdmk82f-pit", FRDMK82F_PIT_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 4; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void frdmk82f_pit_realize(DeviceState *dev, Error **errp) {
	return;

}

static void frdmk82f_pit_reset_enter(Object *obj, ResetType type) {
	FRDMK82FPIT *t = FRDMK82F_PIT(obj);
	frdmk82f_pit_register_reset(t);
}

static const VMStateDescription frdmk82f_pit_vmstate = {
	.name = "frdmk82f-pit",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(MCR, FRDMK82FPIT),
		VMSTATE_UINT32(LTMR64H, FRDMK82FPIT),
		VMSTATE_UINT32(LTMR64L, FRDMK82FPIT),
		VMSTATE_UINT32(LDVAL0, FRDMK82FPIT),
		VMSTATE_UINT32(LDVAL1, FRDMK82FPIT),
		VMSTATE_UINT32(LDVAL2, FRDMK82FPIT),
		VMSTATE_UINT32(LDVAL3, FRDMK82FPIT),
		VMSTATE_UINT32(CVAL0, FRDMK82FPIT),
		VMSTATE_UINT32(CVAL1, FRDMK82FPIT),
		VMSTATE_UINT32(CVAL2, FRDMK82FPIT),
		VMSTATE_UINT32(CVAL3, FRDMK82FPIT),
		VMSTATE_UINT32(TCTRL0, FRDMK82FPIT),
		VMSTATE_UINT32(TCTRL1, FRDMK82FPIT),
		VMSTATE_UINT32(TCTRL2, FRDMK82FPIT),
		VMSTATE_UINT32(TCTRL3, FRDMK82FPIT),
		VMSTATE_UINT32(TFLG0, FRDMK82FPIT),
		VMSTATE_UINT32(TFLG1, FRDMK82FPIT),
		VMSTATE_UINT32(TFLG2, FRDMK82FPIT),
		VMSTATE_UINT32(TFLG3, FRDMK82FPIT),

		VMSTATE_END_OF_LIST()
	}
};

static void frdmk82f_pit_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmk82f_pit_vmstate;
	dc->realize = frdmk82f_pit_realize;
	rc->phases.enter = frdmk82f_pit_reset_enter;

}

static const TypeInfo frdmk82f_pit_info = {
	.name = TYPE_FRDMK82F_PIT,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMK82FPIT),
	.instance_init = frdmk82f_pit_init,
	.class_init = frdmk82f_pit_class_init,
};

static void frdmk82f_pit_register_types(void) {
	type_register_static(&frdmk82f_pit_info);
}

type_init(frdmk82f_pit_register_types);

#define TYPE_FRDMK82F_ADC "frdmk82f-adc"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMK82FADC, FRDMK82F_ADC)

struct FRDMK82FADC {
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
#define FRDMK82F_ADC_SIZE				0x70

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


static void frdmk82f_adc_register_reset(FRDMK82FADC *t) {
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

static uint64_t frdmk82f_adc_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMK82FADC *t = FRDMK82F_ADC(opaque);
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
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK82F ADC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmk82f_adc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMK82FADC *t = FRDMK82F_ADC(opaque);


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
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK82F ADC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmk82f_adc_ops = {
	.read = frdmk82f_adc_read,
	.write = frdmk82f_adc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmk82f_adc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMK82FADC *t = FRDMK82F_ADC(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmk82f_adc_ops, t, "frdmk82f-adc", FRDMK82F_ADC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void frdmk82f_adc_realize(DeviceState *dev, Error **errp) {
	return;

}

static void frdmk82f_adc_reset_enter(Object *obj, ResetType type) {
	FRDMK82FADC *t = FRDMK82F_ADC(obj);
	frdmk82f_adc_register_reset(t);
}

static const VMStateDescription frdmk82f_adc_vmstate = {
	.name = "frdmk82f-adc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CFG1, FRDMK82FADC),
		VMSTATE_UINT32(CFG2, FRDMK82FADC),
		VMSTATE_UINT32(SC2, FRDMK82FADC),
		VMSTATE_UINT32(SC3, FRDMK82FADC),
		VMSTATE_UINT32(OFS, FRDMK82FADC),
		VMSTATE_UINT32(PG, FRDMK82FADC),
		VMSTATE_UINT32(MG, FRDMK82FADC),
		VMSTATE_UINT32(CLPD, FRDMK82FADC),
		VMSTATE_UINT32(CLPS, FRDMK82FADC),
		VMSTATE_UINT32(CLP4, FRDMK82FADC),
		VMSTATE_UINT32(CLP3, FRDMK82FADC),
		VMSTATE_UINT32(CLP2, FRDMK82FADC),
		VMSTATE_UINT32(CLP1, FRDMK82FADC),
		VMSTATE_UINT32(CLP0, FRDMK82FADC),
		VMSTATE_UINT32(CLMD, FRDMK82FADC),
		VMSTATE_UINT32(CLMS, FRDMK82FADC),
		VMSTATE_UINT32(CLM4, FRDMK82FADC),
		VMSTATE_UINT32(CLM3, FRDMK82FADC),
		VMSTATE_UINT32(CLM2, FRDMK82FADC),
		VMSTATE_UINT32(CLM1, FRDMK82FADC),
		VMSTATE_UINT32(CLM0, FRDMK82FADC),
		VMSTATE_UINT32(SC1A, FRDMK82FADC),
		VMSTATE_UINT32(SC1B, FRDMK82FADC),
		VMSTATE_UINT32(RA, FRDMK82FADC),
		VMSTATE_UINT32(RB, FRDMK82FADC),
		VMSTATE_UINT32(CV1, FRDMK82FADC),
		VMSTATE_UINT32(CV2, FRDMK82FADC),

		VMSTATE_END_OF_LIST()
	}
};

static void frdmk82f_adc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmk82f_adc_vmstate;
	dc->realize = frdmk82f_adc_realize;
	rc->phases.enter = frdmk82f_adc_reset_enter;

}

static const TypeInfo frdmk82f_adc_info = {
	.name = TYPE_FRDMK82F_ADC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMK82FADC),
	.instance_init = frdmk82f_adc_init,
	.class_init = frdmk82f_adc_class_init,
};

static void frdmk82f_adc_register_types(void) {
	type_register_static(&frdmk82f_adc_info);
}

type_init(frdmk82f_adc_register_types);

#define TYPE_FRDMK82F_DAC "frdmk82f-dac"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMK82FDAC, FRDMK82F_DAC)

struct FRDMK82FDAC {
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
	uint8_t DAT2L;
	uint8_t DAT3L;
	uint8_t DAT4L;
	uint8_t DAT5L;
	uint8_t DAT6L;
	uint8_t DAT7L;
	uint8_t DAT8L;
	uint8_t DAT9L;
	uint8_t DAT10L;
	uint8_t DAT11L;
	uint8_t DAT12L;
	uint8_t DAT13L;
	uint8_t DAT14L;
	uint8_t DAT15L;
	uint8_t DAT0H;
	uint8_t DAT1H;
	uint8_t DAT2H;
	uint8_t DAT3H;
	uint8_t DAT4H;
	uint8_t DAT5H;
	uint8_t DAT6H;
	uint8_t DAT7H;
	uint8_t DAT8H;
	uint8_t DAT9H;
	uint8_t DAT10H;
	uint8_t DAT11H;
	uint8_t DAT12H;
	uint8_t DAT13H;
	uint8_t DAT14H;
	uint8_t DAT15H;
	/* base */
	uint32_t base;

};
#define FRDMK82F_DAC_SIZE				0x24

REG8(DAC_SR, 0x20)
	FIELD(DAC_SR, DACBFRPBF, 0, 1)
	FIELD(DAC_SR, DACBFRPTF, 1, 1)
	FIELD(DAC_SR, DACBFWMF, 2, 1)
REG8(DAC_C0, 0x21)
	FIELD(DAC_C0, DACBBIEN, 0, 1)
	FIELD(DAC_C0, DACBTIEN, 1, 1)
	FIELD(DAC_C0, DACBWIEN, 2, 1)
	FIELD(DAC_C0, LPEN, 3, 1)
	FIELD(DAC_C0, DACSWTRG, 4, 1)
	FIELD(DAC_C0, DACTRGSEL, 5, 1)
	FIELD(DAC_C0, DACRFS, 6, 1)
	FIELD(DAC_C0, DACEN, 7, 1)
REG8(DAC_C1, 0x22)
	FIELD(DAC_C1, DACBFEN, 0, 1)
	FIELD(DAC_C1, DACBFMD, 1, 2)
	FIELD(DAC_C1, DACBFWM, 3, 2)
	FIELD(DAC_C1, DMAEN, 7, 1)
REG8(DAC_C2, 0x23)
	FIELD(DAC_C2, DACBFUP, 0, 4)
	FIELD(DAC_C2, DACBFRP, 4, 4)
REG8(DAC_DAT0L, 0x0)
	FIELD(DAC_DAT0L, DATA0, 0, 8)
REG8(DAC_DAT1L, 0x2)
	FIELD(DAC_DAT1L, DATA0, 0, 8)
REG8(DAC_DAT2L, 0x4)
	FIELD(DAC_DAT2L, DATA0, 0, 8)
REG8(DAC_DAT3L, 0x6)
	FIELD(DAC_DAT3L, DATA0, 0, 8)
REG8(DAC_DAT4L, 0x8)
	FIELD(DAC_DAT4L, DATA0, 0, 8)
REG8(DAC_DAT5L, 0xa)
	FIELD(DAC_DAT5L, DATA0, 0, 8)
REG8(DAC_DAT6L, 0xc)
	FIELD(DAC_DAT6L, DATA0, 0, 8)
REG8(DAC_DAT7L, 0xe)
	FIELD(DAC_DAT7L, DATA0, 0, 8)
REG8(DAC_DAT8L, 0x10)
	FIELD(DAC_DAT8L, DATA0, 0, 8)
REG8(DAC_DAT9L, 0x12)
	FIELD(DAC_DAT9L, DATA0, 0, 8)
REG8(DAC_DAT10L, 0x14)
	FIELD(DAC_DAT10L, DATA0, 0, 8)
REG8(DAC_DAT11L, 0x16)
	FIELD(DAC_DAT11L, DATA0, 0, 8)
REG8(DAC_DAT12L, 0x18)
	FIELD(DAC_DAT12L, DATA0, 0, 8)
REG8(DAC_DAT13L, 0x1a)
	FIELD(DAC_DAT13L, DATA0, 0, 8)
REG8(DAC_DAT14L, 0x1c)
	FIELD(DAC_DAT14L, DATA0, 0, 8)
REG8(DAC_DAT15L, 0x1e)
	FIELD(DAC_DAT15L, DATA0, 0, 8)
REG8(DAC_DAT0H, 0x1)
	FIELD(DAC_DAT0H, DATA1, 0, 4)
REG8(DAC_DAT1H, 0x3)
	FIELD(DAC_DAT1H, DATA1, 0, 4)
REG8(DAC_DAT2H, 0x5)
	FIELD(DAC_DAT2H, DATA1, 0, 4)
REG8(DAC_DAT3H, 0x7)
	FIELD(DAC_DAT3H, DATA1, 0, 4)
REG8(DAC_DAT4H, 0x9)
	FIELD(DAC_DAT4H, DATA1, 0, 4)
REG8(DAC_DAT5H, 0xb)
	FIELD(DAC_DAT5H, DATA1, 0, 4)
REG8(DAC_DAT6H, 0xd)
	FIELD(DAC_DAT6H, DATA1, 0, 4)
REG8(DAC_DAT7H, 0xf)
	FIELD(DAC_DAT7H, DATA1, 0, 4)
REG8(DAC_DAT8H, 0x11)
	FIELD(DAC_DAT8H, DATA1, 0, 4)
REG8(DAC_DAT9H, 0x13)
	FIELD(DAC_DAT9H, DATA1, 0, 4)
REG8(DAC_DAT10H, 0x15)
	FIELD(DAC_DAT10H, DATA1, 0, 4)
REG8(DAC_DAT11H, 0x17)
	FIELD(DAC_DAT11H, DATA1, 0, 4)
REG8(DAC_DAT12H, 0x19)
	FIELD(DAC_DAT12H, DATA1, 0, 4)
REG8(DAC_DAT13H, 0x1b)
	FIELD(DAC_DAT13H, DATA1, 0, 4)
REG8(DAC_DAT14H, 0x1d)
	FIELD(DAC_DAT14H, DATA1, 0, 4)
REG8(DAC_DAT15H, 0x1f)
	FIELD(DAC_DAT15H, DATA1, 0, 4)


static void frdmk82f_dac_register_reset(FRDMK82FDAC *t) {
	t->SR = 0x2;
	t->C0 = 0x0;
	t->C1 = 0x0;
	t->C2 = 0xf;
	t->DAT0L = 0x0;
	t->DAT1L = 0x0;
	t->DAT2L = 0x0;
	t->DAT3L = 0x0;
	t->DAT4L = 0x0;
	t->DAT5L = 0x0;
	t->DAT6L = 0x0;
	t->DAT7L = 0x0;
	t->DAT8L = 0x0;
	t->DAT9L = 0x0;
	t->DAT10L = 0x0;
	t->DAT11L = 0x0;
	t->DAT12L = 0x0;
	t->DAT13L = 0x0;
	t->DAT14L = 0x0;
	t->DAT15L = 0x0;
	t->DAT0H = 0x0;
	t->DAT1H = 0x0;
	t->DAT2H = 0x0;
	t->DAT3H = 0x0;
	t->DAT4H = 0x0;
	t->DAT5H = 0x0;
	t->DAT6H = 0x0;
	t->DAT7H = 0x0;
	t->DAT8H = 0x0;
	t->DAT9H = 0x0;
	t->DAT10H = 0x0;
	t->DAT11H = 0x0;
	t->DAT12H = 0x0;
	t->DAT13H = 0x0;
	t->DAT14H = 0x0;
	t->DAT15H = 0x0;

}

static uint64_t frdmk82f_dac_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMK82FDAC *t = FRDMK82F_DAC(opaque);
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
		case A_DAC_DAT2L:
			ret = t->DAT2L;
			break;
		case A_DAC_DAT3L:
			ret = t->DAT3L;
			break;
		case A_DAC_DAT4L:
			ret = t->DAT4L;
			break;
		case A_DAC_DAT5L:
			ret = t->DAT5L;
			break;
		case A_DAC_DAT6L:
			ret = t->DAT6L;
			break;
		case A_DAC_DAT7L:
			ret = t->DAT7L;
			break;
		case A_DAC_DAT8L:
			ret = t->DAT8L;
			break;
		case A_DAC_DAT9L:
			ret = t->DAT9L;
			break;
		case A_DAC_DAT10L:
			ret = t->DAT10L;
			break;
		case A_DAC_DAT11L:
			ret = t->DAT11L;
			break;
		case A_DAC_DAT12L:
			ret = t->DAT12L;
			break;
		case A_DAC_DAT13L:
			ret = t->DAT13L;
			break;
		case A_DAC_DAT14L:
			ret = t->DAT14L;
			break;
		case A_DAC_DAT15L:
			ret = t->DAT15L;
			break;
		case A_DAC_DAT0H:
			ret = t->DAT0H;
			break;
		case A_DAC_DAT1H:
			ret = t->DAT1H;
			break;
		case A_DAC_DAT2H:
			ret = t->DAT2H;
			break;
		case A_DAC_DAT3H:
			ret = t->DAT3H;
			break;
		case A_DAC_DAT4H:
			ret = t->DAT4H;
			break;
		case A_DAC_DAT5H:
			ret = t->DAT5H;
			break;
		case A_DAC_DAT6H:
			ret = t->DAT6H;
			break;
		case A_DAC_DAT7H:
			ret = t->DAT7H;
			break;
		case A_DAC_DAT8H:
			ret = t->DAT8H;
			break;
		case A_DAC_DAT9H:
			ret = t->DAT9H;
			break;
		case A_DAC_DAT10H:
			ret = t->DAT10H;
			break;
		case A_DAC_DAT11H:
			ret = t->DAT11H;
			break;
		case A_DAC_DAT12H:
			ret = t->DAT12H;
			break;
		case A_DAC_DAT13H:
			ret = t->DAT13H;
			break;
		case A_DAC_DAT14H:
			ret = t->DAT14H;
			break;
		case A_DAC_DAT15H:
			ret = t->DAT15H;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK82F DAC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmk82f_dac_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMK82FDAC *t = FRDMK82F_DAC(opaque);


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
		case A_DAC_DAT2L:
			t->DAT2L = value;
			break;
		case A_DAC_DAT3L:
			t->DAT3L = value;
			break;
		case A_DAC_DAT4L:
			t->DAT4L = value;
			break;
		case A_DAC_DAT5L:
			t->DAT5L = value;
			break;
		case A_DAC_DAT6L:
			t->DAT6L = value;
			break;
		case A_DAC_DAT7L:
			t->DAT7L = value;
			break;
		case A_DAC_DAT8L:
			t->DAT8L = value;
			break;
		case A_DAC_DAT9L:
			t->DAT9L = value;
			break;
		case A_DAC_DAT10L:
			t->DAT10L = value;
			break;
		case A_DAC_DAT11L:
			t->DAT11L = value;
			break;
		case A_DAC_DAT12L:
			t->DAT12L = value;
			break;
		case A_DAC_DAT13L:
			t->DAT13L = value;
			break;
		case A_DAC_DAT14L:
			t->DAT14L = value;
			break;
		case A_DAC_DAT15L:
			t->DAT15L = value;
			break;
		case A_DAC_DAT0H:
			t->DAT0H = value;
			break;
		case A_DAC_DAT1H:
			t->DAT1H = value;
			break;
		case A_DAC_DAT2H:
			t->DAT2H = value;
			break;
		case A_DAC_DAT3H:
			t->DAT3H = value;
			break;
		case A_DAC_DAT4H:
			t->DAT4H = value;
			break;
		case A_DAC_DAT5H:
			t->DAT5H = value;
			break;
		case A_DAC_DAT6H:
			t->DAT6H = value;
			break;
		case A_DAC_DAT7H:
			t->DAT7H = value;
			break;
		case A_DAC_DAT8H:
			t->DAT8H = value;
			break;
		case A_DAC_DAT9H:
			t->DAT9H = value;
			break;
		case A_DAC_DAT10H:
			t->DAT10H = value;
			break;
		case A_DAC_DAT11H:
			t->DAT11H = value;
			break;
		case A_DAC_DAT12H:
			t->DAT12H = value;
			break;
		case A_DAC_DAT13H:
			t->DAT13H = value;
			break;
		case A_DAC_DAT14H:
			t->DAT14H = value;
			break;
		case A_DAC_DAT15H:
			t->DAT15H = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK82F DAC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmk82f_dac_ops = {
	.read = frdmk82f_dac_read,
	.write = frdmk82f_dac_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmk82f_dac_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMK82FDAC *t = FRDMK82F_DAC(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmk82f_dac_ops, t, "frdmk82f-dac", FRDMK82F_DAC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void frdmk82f_dac_realize(DeviceState *dev, Error **errp) {
	return;

}

static void frdmk82f_dac_reset_enter(Object *obj, ResetType type) {
	FRDMK82FDAC *t = FRDMK82F_DAC(obj);
	frdmk82f_dac_register_reset(t);
}

static const VMStateDescription frdmk82f_dac_vmstate = {
	.name = "frdmk82f-dac",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT8(SR, FRDMK82FDAC),
		VMSTATE_UINT8(C0, FRDMK82FDAC),
		VMSTATE_UINT8(C1, FRDMK82FDAC),
		VMSTATE_UINT8(C2, FRDMK82FDAC),
		VMSTATE_UINT8(DAT0L, FRDMK82FDAC),
		VMSTATE_UINT8(DAT1L, FRDMK82FDAC),
		VMSTATE_UINT8(DAT2L, FRDMK82FDAC),
		VMSTATE_UINT8(DAT3L, FRDMK82FDAC),
		VMSTATE_UINT8(DAT4L, FRDMK82FDAC),
		VMSTATE_UINT8(DAT5L, FRDMK82FDAC),
		VMSTATE_UINT8(DAT6L, FRDMK82FDAC),
		VMSTATE_UINT8(DAT7L, FRDMK82FDAC),
		VMSTATE_UINT8(DAT8L, FRDMK82FDAC),
		VMSTATE_UINT8(DAT9L, FRDMK82FDAC),
		VMSTATE_UINT8(DAT10L, FRDMK82FDAC),
		VMSTATE_UINT8(DAT11L, FRDMK82FDAC),
		VMSTATE_UINT8(DAT12L, FRDMK82FDAC),
		VMSTATE_UINT8(DAT13L, FRDMK82FDAC),
		VMSTATE_UINT8(DAT14L, FRDMK82FDAC),
		VMSTATE_UINT8(DAT15L, FRDMK82FDAC),
		VMSTATE_UINT8(DAT0H, FRDMK82FDAC),
		VMSTATE_UINT8(DAT1H, FRDMK82FDAC),
		VMSTATE_UINT8(DAT2H, FRDMK82FDAC),
		VMSTATE_UINT8(DAT3H, FRDMK82FDAC),
		VMSTATE_UINT8(DAT4H, FRDMK82FDAC),
		VMSTATE_UINT8(DAT5H, FRDMK82FDAC),
		VMSTATE_UINT8(DAT6H, FRDMK82FDAC),
		VMSTATE_UINT8(DAT7H, FRDMK82FDAC),
		VMSTATE_UINT8(DAT8H, FRDMK82FDAC),
		VMSTATE_UINT8(DAT9H, FRDMK82FDAC),
		VMSTATE_UINT8(DAT10H, FRDMK82FDAC),
		VMSTATE_UINT8(DAT11H, FRDMK82FDAC),
		VMSTATE_UINT8(DAT12H, FRDMK82FDAC),
		VMSTATE_UINT8(DAT13H, FRDMK82FDAC),
		VMSTATE_UINT8(DAT14H, FRDMK82FDAC),
		VMSTATE_UINT8(DAT15H, FRDMK82FDAC),

		VMSTATE_END_OF_LIST()
	}
};

static void frdmk82f_dac_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmk82f_dac_vmstate;
	dc->realize = frdmk82f_dac_realize;
	rc->phases.enter = frdmk82f_dac_reset_enter;

}

static const TypeInfo frdmk82f_dac_info = {
	.name = TYPE_FRDMK82F_DAC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMK82FDAC),
	.instance_init = frdmk82f_dac_init,
	.class_init = frdmk82f_dac_class_init,
};

static void frdmk82f_dac_register_types(void) {
	type_register_static(&frdmk82f_dac_info);
}

type_init(frdmk82f_dac_register_types);

#define TYPE_FRDMK82F_I2C "frdmk82f-i2c"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMK82FI2C, FRDMK82F_I2C)

struct FRDMK82FI2C {
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
	uint8_t S2;
	/* chardev backend */
	CharBackend chr;
	guint watch_tag;

	/* base */
	uint32_t base;

};
#define FRDMK82F_I2C_SIZE				0xd

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
	FIELD(I2C_FLT, FLT, 0, 4)
	FIELD(I2C_FLT, STARTF, 4, 1)
	FIELD(I2C_FLT, SSIE, 5, 1)
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
REG8(I2C_S2, 0xc)
	FIELD(I2C_S2, EMPTY, 0, 1)
	FIELD(I2C_S2, ERROR, 1, 1)


__attribute__((unused))
static void frdmk82f_i2c_update(FRDMK82FI2C *t) {
	int conditions = ((!(t->S & 0x10)) && (t->C1 & 0x20) && (!(t->S & 0x1)));
	qemu_set_irq(t->irq[0], conditions);

}

static int frdmk82f_i2c_can_receive(void *opaque) {
	return 1;
}

static void frdmk82f_i2c_receive(void *opaque, const uint8_t *buf, int size) {
	FRDMK82FI2C *t = FRDMK82F_I2C(opaque);

	t->D = *buf;
	t->FLT |= 0x10;
	t->S &= (~(0x10));
	t->S &= (~(0x1));
	t->S |= 0x40;
	t->S |= 0x80;

	frdmk82f_i2c_update(t);
}

static gboolean frdmk82f_i2c_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	FRDMK82FI2C *t = FRDMK82F_I2C(opaque);
	int ret;

	t->watch_tag = 0;
	t->S |= 0x1;
	t->S2 &= (~(0x1));
	t->S |= 0x20;
	t->S &= (~(0x2));
	t->S &= (~(0x80));
	t->S |= 0x10;

	frdmk82f_i2c_update(t);

	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->D), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, frdmk82f_i2c_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->S &= (~(0x10));
	t->S2 |= 0x1;
	t->S &= (~(0x1));
	t->S |= 0x80;
	t->S |= 0x2;
	t->S &= (~(0x20));

	frdmk82f_i2c_update(t);

	return FALSE;
}

static void frdmk82f_i2c_register_reset(FRDMK82FI2C *t) {
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
	t->S2 = 0x1;
	t->S &= (~(0x10));
	t->S2 |= 0x1;
	t->S &= (~(0x1));
	t->S |= 0x80;
	t->S |= 0x2;
	t->S &= (~(0x20));

}

static uint64_t frdmk82f_i2c_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMK82FI2C *t = FRDMK82F_I2C(opaque);
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
			t->S |= 0x1;
			t->FLT &= (~(0x10));
			t->S &= (~(0x40));
			t->S &= (~(0x80));
			t->S |= 0x10;
			qemu_chr_fe_accept_input(&(t->chr));
			frdmk82f_i2c_update(t);
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
		case A_I2C_S2:
			ret = t->S2;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK82F I2C read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmk82f_i2c_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMK82FI2C *t = FRDMK82F_I2C(opaque);


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
			} else if (((value & 0x10) && (value & 0x20))) {
				t->S2 |= 0x1;
			} else if ((value & 0x20)) {
				t->S |= 0x2;
			} else if ((value & 0x8)) {
				t->S |= 0x2;
			}
			frdmk82f_i2c_update(t);
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
			if ((value == 0x2)) {
				t->S |= 0x40;
			}
			frdmk82f_i2c_update(t);
			break;
		case A_I2C_D:
			t->D = value;
			frdmk82f_i2c_transmit(NULL, G_IO_OUT, t);
			break;
		case A_I2C_C2:
			t->C2 = value;
			break;
		case A_I2C_FLT:
			t->FLT &= value;
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
		case A_I2C_S2:
			t->S2 &= value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK82F I2C write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmk82f_i2c_ops = {
	.read = frdmk82f_i2c_read,
	.write = frdmk82f_i2c_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmk82f_i2c_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMK82FI2C *t = FRDMK82F_I2C(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmk82f_i2c_ops, t, "frdmk82f-i2c", FRDMK82F_I2C_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void frdmk82f_i2c_realize(DeviceState *dev, Error **errp) {
	FRDMK82FI2C *t = FRDMK82F_I2C(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), frdmk82f_i2c_can_receive, frdmk82f_i2c_receive,
		NULL, NULL, t, NULL, true);

}

static void frdmk82f_i2c_reset_enter(Object *obj, ResetType type) {
	FRDMK82FI2C *t = FRDMK82F_I2C(obj);
	frdmk82f_i2c_register_reset(t);
}

static const VMStateDescription frdmk82f_i2c_vmstate = {
	.name = "frdmk82f-i2c",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT8(A1, FRDMK82FI2C),
		VMSTATE_UINT8(F, FRDMK82FI2C),
		VMSTATE_UINT8(C1, FRDMK82FI2C),
		VMSTATE_UINT8(S, FRDMK82FI2C),
		VMSTATE_UINT8(D, FRDMK82FI2C),
		VMSTATE_UINT8(C2, FRDMK82FI2C),
		VMSTATE_UINT8(FLT, FRDMK82FI2C),
		VMSTATE_UINT8(RA, FRDMK82FI2C),
		VMSTATE_UINT8(SMB, FRDMK82FI2C),
		VMSTATE_UINT8(A2, FRDMK82FI2C),
		VMSTATE_UINT8(SLTH, FRDMK82FI2C),
		VMSTATE_UINT8(SLTL, FRDMK82FI2C),
		VMSTATE_UINT8(S2, FRDMK82FI2C),

		VMSTATE_END_OF_LIST()
	}
};

static Property frdmk82f_i2c_properties[] = {
	DEFINE_PROP_CHR("chardev", FRDMK82FI2C, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void frdmk82f_i2c_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmk82f_i2c_vmstate;
	dc->realize = frdmk82f_i2c_realize;
	rc->phases.enter = frdmk82f_i2c_reset_enter;
	device_class_set_props(dc, frdmk82f_i2c_properties);

}

static const TypeInfo frdmk82f_i2c_info = {
	.name = TYPE_FRDMK82F_I2C,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMK82FI2C),
	.instance_init = frdmk82f_i2c_init,
	.class_init = frdmk82f_i2c_class_init,
};

static void frdmk82f_i2c_register_types(void) {
	type_register_static(&frdmk82f_i2c_info);
}

type_init(frdmk82f_i2c_register_types);

#define TYPE_NXP_FRDM_K82F_MACHINE MACHINE_TYPE_NAME("nxp_frdm_k82f")
OBJECT_DECLARE_TYPE(NXP_FRDM_K82FMachineState, NXP_FRDM_K82FMachineClass, NXP_FRDM_K82F_MACHINE)

struct NXP_FRDM_K82FMachineClass {
	MachineClass parent;
};

struct NXP_FRDM_K82FMachineState {
	MachineState parent;
	ARMv7MState armv7m;
};

static void nxp_frdm_k82f_periph_init(MachineState *machine) {
	NXP_FRDM_K82FMachineState *sms = NXP_FRDM_K82F_MACHINE(machine);
	FRDMK82FLPUART *p0 = g_new(FRDMK82FLPUART, 1);
	object_initialize_child(OBJECT(sms), "LPUART0", p0, TYPE_FRDMK82F_LPUART);
	qdev_prop_set_chr(DEVICE(p0), "chardev", qemu_chr_new("soc-lpuart0", "chardev:lpuart0", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p0), &error_fatal);
	p0->base = 0x400c4000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p0), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 30));
	sysbus_mmio_map(SYS_BUS_DEVICE(p0), 0, 0x400c4000);

	FRDMK82FLPUART *p1 = g_new(FRDMK82FLPUART, 1);
	object_initialize_child(OBJECT(sms), "LPUART1", p1, TYPE_FRDMK82F_LPUART);
	qdev_prop_set_chr(DEVICE(p1), "chardev", qemu_chr_new("soc-lpuart1", "chardev:lpuart1", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p1), &error_fatal);
	p1->base = 0x400c5000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p1), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 31));
	sysbus_mmio_map(SYS_BUS_DEVICE(p1), 0, 0x400c5000);

	FRDMK82FLPUART *p2 = g_new(FRDMK82FLPUART, 1);
	object_initialize_child(OBJECT(sms), "LPUART2", p2, TYPE_FRDMK82F_LPUART);
	qdev_prop_set_chr(DEVICE(p2), "chardev", qemu_chr_new("soc-lpuart2", "chardev:lpuart2", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p2), &error_fatal);
	p2->base = 0x400c6000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p2), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 32));
	sysbus_mmio_map(SYS_BUS_DEVICE(p2), 0, 0x400c6000);

	FRDMK82FLPUART *p3 = g_new(FRDMK82FLPUART, 1);
	object_initialize_child(OBJECT(sms), "LPUART3", p3, TYPE_FRDMK82F_LPUART);
	qdev_prop_set_chr(DEVICE(p3), "chardev", qemu_chr_new("soc-lpuart3", "chardev:lpuart3", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p3), &error_fatal);
	p3->base = 0x400c7000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p3), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 33));
	sysbus_mmio_map(SYS_BUS_DEVICE(p3), 0, 0x400c7000);

	FRDMK82FLPUART *p4 = g_new(FRDMK82FLPUART, 1);
	object_initialize_child(OBJECT(sms), "LPUART4", p4, TYPE_FRDMK82F_LPUART);
	qdev_prop_set_chr(DEVICE(p4), "chardev", qemu_chr_new("soc-lpuart4", "chardev:lpuart4", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p4), &error_fatal);
	p4->base = 0x400d6000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p4), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 34));
	sysbus_mmio_map(SYS_BUS_DEVICE(p4), 0, 0x400d6000);

	FRDMK82FWDOG *p5 = g_new(FRDMK82FWDOG, 1);
	object_initialize_child(OBJECT(sms), "WDOG", p5, TYPE_FRDMK82F_WDOG);
	sysbus_realize(SYS_BUS_DEVICE(p5), &error_fatal);
	p5->base = 0x40052000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p5), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 22));
	sysbus_mmio_map(SYS_BUS_DEVICE(p5), 0, 0x40052000);

	FRDMK82FSIM *p6 = g_new(FRDMK82FSIM, 1);
	object_initialize_child(OBJECT(sms), "SIM", p6, TYPE_FRDMK82F_SIM);
	sysbus_realize(SYS_BUS_DEVICE(p6), &error_fatal);
	p6->base = 0x40047000;
	sysbus_mmio_map(SYS_BUS_DEVICE(p6), 0, 0x40047000);

	FRDMK82FPMC *p7 = g_new(FRDMK82FPMC, 1);
	object_initialize_child(OBJECT(sms), "PMC", p7, TYPE_FRDMK82F_PMC);
	sysbus_realize(SYS_BUS_DEVICE(p7), &error_fatal);
	p7->base = 0x4007d000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p7), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 20));
	sysbus_mmio_map(SYS_BUS_DEVICE(p7), 0, 0x4007d000);

	FRDMK82FOSC *p8 = g_new(FRDMK82FOSC, 1);
	object_initialize_child(OBJECT(sms), "OSC", p8, TYPE_FRDMK82F_OSC);
	sysbus_realize(SYS_BUS_DEVICE(p8), &error_fatal);
	p8->base = 0x40065000;
	sysbus_mmio_map(SYS_BUS_DEVICE(p8), 0, 0x40065000);

	FRDMK82FMCG *p9 = g_new(FRDMK82FMCG, 1);
	object_initialize_child(OBJECT(sms), "MCG", p9, TYPE_FRDMK82F_MCG);
	sysbus_realize(SYS_BUS_DEVICE(p9), &error_fatal);
	p9->base = 0x40064000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p9), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 57));
	sysbus_mmio_map(SYS_BUS_DEVICE(p9), 0, 0x40064000);

	FRDMK82FRTC *p10 = g_new(FRDMK82FRTC, 1);
	object_initialize_child(OBJECT(sms), "RTC", p10, TYPE_FRDMK82F_RTC);
	sysbus_realize(SYS_BUS_DEVICE(p10), &error_fatal);
	p10->base = 0x4003d000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p10), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 46));
	sysbus_connect_irq(SYS_BUS_DEVICE(p10), 1, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 47));
	sysbus_mmio_map(SYS_BUS_DEVICE(p10), 0, 0x4003d000);

	FRDMK82FGPIO *p11 = g_new(FRDMK82FGPIO, 1);
	object_initialize_child(OBJECT(sms), "GPIOA", p11, TYPE_FRDMK82F_GPIO);
	sysbus_realize(SYS_BUS_DEVICE(p11), &error_fatal);
	p11->base = 0x400ff000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p11), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 59));
	sysbus_mmio_map(SYS_BUS_DEVICE(p11), 0, 0x400ff000);

	FRDMK82FGPIO *p12 = g_new(FRDMK82FGPIO, 1);
	object_initialize_child(OBJECT(sms), "GPIOB", p12, TYPE_FRDMK82F_GPIO);
	sysbus_realize(SYS_BUS_DEVICE(p12), &error_fatal);
	p12->base = 0x400ff040;
	sysbus_connect_irq(SYS_BUS_DEVICE(p12), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 60));
	sysbus_mmio_map(SYS_BUS_DEVICE(p12), 0, 0x400ff040);

	FRDMK82FGPIO *p13 = g_new(FRDMK82FGPIO, 1);
	object_initialize_child(OBJECT(sms), "GPIOC", p13, TYPE_FRDMK82F_GPIO);
	sysbus_realize(SYS_BUS_DEVICE(p13), &error_fatal);
	p13->base = 0x400ff080;
	sysbus_connect_irq(SYS_BUS_DEVICE(p13), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 61));
	sysbus_mmio_map(SYS_BUS_DEVICE(p13), 0, 0x400ff080);

	FRDMK82FGPIO *p14 = g_new(FRDMK82FGPIO, 1);
	object_initialize_child(OBJECT(sms), "GPIOD", p14, TYPE_FRDMK82F_GPIO);
	sysbus_realize(SYS_BUS_DEVICE(p14), &error_fatal);
	p14->base = 0x400ff0c0;
	sysbus_connect_irq(SYS_BUS_DEVICE(p14), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 62));
	sysbus_mmio_map(SYS_BUS_DEVICE(p14), 0, 0x400ff0c0);

	FRDMK82FGPIO *p15 = g_new(FRDMK82FGPIO, 1);
	object_initialize_child(OBJECT(sms), "GPIOE", p15, TYPE_FRDMK82F_GPIO);
	sysbus_realize(SYS_BUS_DEVICE(p15), &error_fatal);
	p15->base = 0x400ff100;
	sysbus_connect_irq(SYS_BUS_DEVICE(p15), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 63));
	sysbus_mmio_map(SYS_BUS_DEVICE(p15), 0, 0x400ff100);

	FRDMK82FPORT *p16 = g_new(FRDMK82FPORT, 1);
	object_initialize_child(OBJECT(sms), "PORTA", p16, TYPE_FRDMK82F_PORT);
	qdev_prop_set_chr(DEVICE(p16), "chardev", qemu_chr_new("soc-porta", "chardev:porta", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p16), &error_fatal);
	p16->base = 0x40049000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p16), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 59));
	sysbus_mmio_map(SYS_BUS_DEVICE(p16), 0, 0x40049000);

	FRDMK82FPORT *p17 = g_new(FRDMK82FPORT, 1);
	object_initialize_child(OBJECT(sms), "PORTB", p17, TYPE_FRDMK82F_PORT);
	qdev_prop_set_chr(DEVICE(p17), "chardev", qemu_chr_new("soc-portb", "chardev:portb", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p17), &error_fatal);
	p17->base = 0x4004a000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p17), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 60));
	sysbus_mmio_map(SYS_BUS_DEVICE(p17), 0, 0x4004a000);

	FRDMK82FPORT *p18 = g_new(FRDMK82FPORT, 1);
	object_initialize_child(OBJECT(sms), "PORTC", p18, TYPE_FRDMK82F_PORT);
	qdev_prop_set_chr(DEVICE(p18), "chardev", qemu_chr_new("soc-portc", "chardev:portc", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p18), &error_fatal);
	p18->base = 0x4004b000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p18), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 61));
	sysbus_mmio_map(SYS_BUS_DEVICE(p18), 0, 0x4004b000);

	FRDMK82FPORT *p19 = g_new(FRDMK82FPORT, 1);
	object_initialize_child(OBJECT(sms), "PORTD", p19, TYPE_FRDMK82F_PORT);
	qdev_prop_set_chr(DEVICE(p19), "chardev", qemu_chr_new("soc-portd", "chardev:portd", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p19), &error_fatal);
	p19->base = 0x4004c000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p19), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 62));
	sysbus_mmio_map(SYS_BUS_DEVICE(p19), 0, 0x4004c000);

	FRDMK82FPORT *p20 = g_new(FRDMK82FPORT, 1);
	object_initialize_child(OBJECT(sms), "PORTE", p20, TYPE_FRDMK82F_PORT);
	qdev_prop_set_chr(DEVICE(p20), "chardev", qemu_chr_new("soc-porte", "chardev:porte", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p20), &error_fatal);
	p20->base = 0x4004d000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p20), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 63));
	sysbus_mmio_map(SYS_BUS_DEVICE(p20), 0, 0x4004d000);

	FRDMK82FSYSMPU *p21 = g_new(FRDMK82FSYSMPU, 1);
	object_initialize_child(OBJECT(sms), "SYSMPU", p21, TYPE_FRDMK82F_SYSMPU);
	sysbus_realize(SYS_BUS_DEVICE(p21), &error_fatal);
	p21->base = 0x4000d000;
	sysbus_mmio_map(SYS_BUS_DEVICE(p21), 0, 0x4000d000);

	FRDMK82FADC *p22 = g_new(FRDMK82FADC, 1);
	object_initialize_child(OBJECT(sms), "ADC0", p22, TYPE_FRDMK82F_ADC);
	sysbus_realize(SYS_BUS_DEVICE(p22), &error_fatal);
	p22->base = 0x4003b000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p22), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 39));
	sysbus_mmio_map(SYS_BUS_DEVICE(p22), 0, 0x4003b000);

	FRDMK82FDAC *p23 = g_new(FRDMK82FDAC, 1);
	object_initialize_child(OBJECT(sms), "DAC0", p23, TYPE_FRDMK82F_DAC);
	sysbus_realize(SYS_BUS_DEVICE(p23), &error_fatal);
	p23->base = 0x400cc000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p23), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 56));
	sysbus_mmio_map(SYS_BUS_DEVICE(p23), 0, 0x400cc000);

	FRDMK82FSPI *p24 = g_new(FRDMK82FSPI, 1);
	object_initialize_child(OBJECT(sms), "SPI0", p24, TYPE_FRDMK82F_SPI);
	qdev_prop_set_chr(DEVICE(p24), "chardev", qemu_chr_new("soc-spi0", "chardev:spi0", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p24), &error_fatal);
	p24->base = 0x4002c000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p24), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 26));
	sysbus_mmio_map(SYS_BUS_DEVICE(p24), 0, 0x4002c000);

	FRDMK82FI2C *p25 = g_new(FRDMK82FI2C, 1);
	object_initialize_child(OBJECT(sms), "I2C0", p25, TYPE_FRDMK82F_I2C);
	qdev_prop_set_chr(DEVICE(p25), "chardev", qemu_chr_new("soc-i2c0", "chardev:i2c0", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p25), &error_fatal);
	p25->base = 0x40066000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p25), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 24));
	sysbus_mmio_map(SYS_BUS_DEVICE(p25), 0, 0x40066000);

	FRDMK82FPIT *p26 = g_new(FRDMK82FPIT, 1);
	object_initialize_child(OBJECT(sms), "PIT0", p26, TYPE_FRDMK82F_PIT);
	sysbus_realize(SYS_BUS_DEVICE(p26), &error_fatal);
	p26->base = 0x40037000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p26), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 48));
	sysbus_connect_irq(SYS_BUS_DEVICE(p26), 1, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 49));
	sysbus_connect_irq(SYS_BUS_DEVICE(p26), 2, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 50));
	sysbus_connect_irq(SYS_BUS_DEVICE(p26), 3, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 51));
	sysbus_mmio_map(SYS_BUS_DEVICE(p26), 0, 0x40037000);


}

static void nxp_frdm_k82f_common_init(MachineState *machine) {
	NXP_FRDM_K82FMachineState *sms = NXP_FRDM_K82F_MACHINE(machine);
	MemoryRegion *sysmem = get_system_memory();
	MemoryRegion *mem;
	Clock *cpuclk = clock_new(OBJECT(sms), "SYSCLK");
	clock_set_hz(cpuclk, 120000000);
	Clock *refclk = clock_new(OBJECT(sms), "REFCLK");
	clock_set_hz(refclk, 120000000);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_rom(mem, NULL, "main-flash", 0x8000000, &error_fatal);
	memory_region_add_subregion(sysmem, 0x0, mem);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_ram(mem, NULL, "sram_l", 0x10000, &error_fatal);
	memory_region_add_subregion(sysmem, 0x1fff0000, mem);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_ram(mem, NULL, "sram_u", 0x30000, &error_fatal);
	memory_region_add_subregion(sysmem, 0x20000000, mem);


	object_initialize_child(OBJECT(sms), "armv7m", &(sms->armv7m), TYPE_ARMV7M);
	qdev_prop_set_uint32(DEVICE(&(sms->armv7m)),"num-irq", 256);
	qdev_prop_set_string(DEVICE(&(sms->armv7m)), "cpu-type", machine->cpu_type);
	qdev_prop_set_bit(DEVICE(&(sms->armv7m)), "enable-bitband", true);
	qdev_connect_clock_in(DEVICE(&(sms->armv7m)), "cpuclk", cpuclk);
	qdev_connect_clock_in(DEVICE(&(sms->armv7m)), "refclk", refclk);
	qdev_prop_set_uint32(DEVICE(&(sms->armv7m)), "init-nsvtor", 0x0);
	object_property_set_link(OBJECT(&sms->armv7m), "memory", OBJECT(sysmem), &error_abort);
	sysbus_realize(SYS_BUS_DEVICE(&sms->armv7m), &error_fatal);

	nxp_frdm_k82f_periph_init(machine);

	armv7m_load_kernel(ARM_CPU(first_cpu), machine->kernel_filename, 0x0, 0x8000000);
}

static void nxp_frdm_k82f_class_init(ObjectClass *oc, void *data) {
	MachineClass *mc = MACHINE_CLASS(oc);
	mc->desc = "nxp_frdm_k82f";
	mc->init = nxp_frdm_k82f_common_init;
	mc->max_cpus = 1;
	mc->default_cpus = 1;
	mc->default_cpu_type = ARM_CPU_TYPE_NAME("cortex-m4");
}

static const TypeInfo nxp_frdm_k82f_info = {
	.name = TYPE_NXP_FRDM_K82F_MACHINE,
	.parent = TYPE_MACHINE,
	.instance_size = sizeof(NXP_FRDM_K82FMachineState),
	.class_size = sizeof(NXP_FRDM_K82FMachineClass),
	.class_init = nxp_frdm_k82f_class_init
};

static void nxp_frdm_k82f_machine_init(void) {
	type_register_static(&nxp_frdm_k82f_info);
}

type_init(nxp_frdm_k82f_machine_init);
