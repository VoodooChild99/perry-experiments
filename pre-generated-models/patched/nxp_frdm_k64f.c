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


#define TYPE_FRDMK64F_UART0 "frdmk64f-uart0"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMK64FUART0, FRDMK64F_UART0)

struct FRDMK64FUART0 {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[3];

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
	uint8_t ED;
	uint8_t MODEM;
	uint8_t IR;
	uint8_t PFIFO;
	uint8_t CFIFO;
	uint8_t SFIFO;
	uint8_t TWFIFO;
	uint8_t TCFIFO;
	uint8_t RWFIFO;
	uint8_t RCFIFO;
	uint8_t C7816;
	uint8_t IE7816;
	uint8_t IS7816;
	uint8_t WP7816T0;
	uint8_t WP7816T1;
	uint8_t WN7816;
	uint8_t WF7816;
	uint8_t ET7816;
	uint8_t TL7816;
	/* chardev backend */
	CharBackend chr;
	guint watch_tag;

	/* base */
	uint32_t base;

};
#define FRDMK64F_UART0_SIZE				0x20

REG8(UART0_BDH, 0x0)
	FIELD(UART0_BDH, SBR, 0, 5)
	FIELD(UART0_BDH, SBNS, 5, 1)
	FIELD(UART0_BDH, RXEDGIE, 6, 1)
	FIELD(UART0_BDH, LBKDIE, 7, 1)
REG8(UART0_BDL, 0x1)
	FIELD(UART0_BDL, SBR, 0, 8)
REG8(UART0_C1, 0x2)
	FIELD(UART0_C1, PT, 0, 1)
	FIELD(UART0_C1, PE, 1, 1)
	FIELD(UART0_C1, ILT, 2, 1)
	FIELD(UART0_C1, WAKE, 3, 1)
	FIELD(UART0_C1, M, 4, 1)
	FIELD(UART0_C1, RSRC, 5, 1)
	FIELD(UART0_C1, UARTSWAI, 6, 1)
	FIELD(UART0_C1, LOOPS, 7, 1)
REG8(UART0_C2, 0x3)
	FIELD(UART0_C2, SBK, 0, 1)
	FIELD(UART0_C2, RWU, 1, 1)
	FIELD(UART0_C2, RE, 2, 1)
	FIELD(UART0_C2, TE, 3, 1)
	FIELD(UART0_C2, ILIE, 4, 1)
	FIELD(UART0_C2, RIE, 5, 1)
	FIELD(UART0_C2, TCIE, 6, 1)
	FIELD(UART0_C2, TIE, 7, 1)
REG8(UART0_S1, 0x4)
	FIELD(UART0_S1, PF, 0, 1)
	FIELD(UART0_S1, FE, 1, 1)
	FIELD(UART0_S1, NF, 2, 1)
	FIELD(UART0_S1, OR, 3, 1)
	FIELD(UART0_S1, IDLE, 4, 1)
	FIELD(UART0_S1, RDRF, 5, 1)
	FIELD(UART0_S1, TC, 6, 1)
	FIELD(UART0_S1, TDRE, 7, 1)
REG8(UART0_S2, 0x5)
	FIELD(UART0_S2, RAF, 0, 1)
	FIELD(UART0_S2, LBKDE, 1, 1)
	FIELD(UART0_S2, BRK13, 2, 1)
	FIELD(UART0_S2, RWUID, 3, 1)
	FIELD(UART0_S2, RXINV, 4, 1)
	FIELD(UART0_S2, MSBF, 5, 1)
	FIELD(UART0_S2, RXEDGIF, 6, 1)
	FIELD(UART0_S2, LBKDIF, 7, 1)
REG8(UART0_C3, 0x6)
	FIELD(UART0_C3, PEIE, 0, 1)
	FIELD(UART0_C3, FEIE, 1, 1)
	FIELD(UART0_C3, NEIE, 2, 1)
	FIELD(UART0_C3, ORIE, 3, 1)
	FIELD(UART0_C3, TXINV, 4, 1)
	FIELD(UART0_C3, TXDIR, 5, 1)
	FIELD(UART0_C3, T8, 6, 1)
	FIELD(UART0_C3, R8, 7, 1)
REG8(UART0_D, 0x7)
	FIELD(UART0_D, RT, 0, 8)
REG8(UART0_MA1, 0x8)
	FIELD(UART0_MA1, MA, 0, 8)
REG8(UART0_MA2, 0x9)
	FIELD(UART0_MA2, MA, 0, 8)
REG8(UART0_C4, 0xa)
	FIELD(UART0_C4, BRFA, 0, 5)
	FIELD(UART0_C4, M10, 5, 1)
	FIELD(UART0_C4, MAEN2, 6, 1)
	FIELD(UART0_C4, MAEN1, 7, 1)
REG8(UART0_C5, 0xb)
	FIELD(UART0_C5, LBKDDMAS, 3, 1)
	FIELD(UART0_C5, ILDMAS, 4, 1)
	FIELD(UART0_C5, RDMAS, 5, 1)
	FIELD(UART0_C5, TCDMAS, 6, 1)
	FIELD(UART0_C5, TDMAS, 7, 1)
REG8(UART0_ED, 0xc)
	FIELD(UART0_ED, PARITYE, 6, 1)
	FIELD(UART0_ED, NOISY, 7, 1)
REG8(UART0_MODEM, 0xd)
	FIELD(UART0_MODEM, TXCTSE, 0, 1)
	FIELD(UART0_MODEM, TXRTSE, 1, 1)
	FIELD(UART0_MODEM, TXRTSPOL, 2, 1)
	FIELD(UART0_MODEM, RXRTSE, 3, 1)
REG8(UART0_IR, 0xe)
	FIELD(UART0_IR, TNP, 0, 2)
	FIELD(UART0_IR, IREN, 2, 1)
REG8(UART0_PFIFO, 0x10)
	FIELD(UART0_PFIFO, RXFIFOSIZE, 0, 3)
	FIELD(UART0_PFIFO, RXFE, 3, 1)
	FIELD(UART0_PFIFO, TXFIFOSIZE, 4, 3)
	FIELD(UART0_PFIFO, TXFE, 7, 1)
REG8(UART0_CFIFO, 0x11)
	FIELD(UART0_CFIFO, RXUFE, 0, 1)
	FIELD(UART0_CFIFO, TXOFE, 1, 1)
	FIELD(UART0_CFIFO, RXOFE, 2, 1)
	FIELD(UART0_CFIFO, RXFLUSH, 6, 1)
	FIELD(UART0_CFIFO, TXFLUSH, 7, 1)
REG8(UART0_SFIFO, 0x12)
	FIELD(UART0_SFIFO, RXUF, 0, 1)
	FIELD(UART0_SFIFO, TXOF, 1, 1)
	FIELD(UART0_SFIFO, RXOF, 2, 1)
	FIELD(UART0_SFIFO, RXEMPT, 6, 1)
	FIELD(UART0_SFIFO, TXEMPT, 7, 1)
REG8(UART0_TWFIFO, 0x13)
	FIELD(UART0_TWFIFO, TXWATER, 0, 8)
REG8(UART0_TCFIFO, 0x14)
	FIELD(UART0_TCFIFO, TXCOUNT, 0, 8)
REG8(UART0_RWFIFO, 0x15)
	FIELD(UART0_RWFIFO, RXWATER, 0, 8)
REG8(UART0_RCFIFO, 0x16)
	FIELD(UART0_RCFIFO, RXCOUNT, 0, 8)
REG8(UART0_C7816, 0x18)
	FIELD(UART0_C7816, ISO_7816E, 0, 1)
	FIELD(UART0_C7816, TTYPE, 1, 1)
	FIELD(UART0_C7816, INIT, 2, 1)
	FIELD(UART0_C7816, ANACK, 3, 1)
	FIELD(UART0_C7816, ONACK, 4, 1)
REG8(UART0_IE7816, 0x19)
	FIELD(UART0_IE7816, RXTE, 0, 1)
	FIELD(UART0_IE7816, TXTE, 1, 1)
	FIELD(UART0_IE7816, GTVE, 2, 1)
	FIELD(UART0_IE7816, INITDE, 4, 1)
	FIELD(UART0_IE7816, BWTE, 5, 1)
	FIELD(UART0_IE7816, CWTE, 6, 1)
	FIELD(UART0_IE7816, WTE, 7, 1)
REG8(UART0_IS7816, 0x1a)
	FIELD(UART0_IS7816, RXT, 0, 1)
	FIELD(UART0_IS7816, TXT, 1, 1)
	FIELD(UART0_IS7816, GTV, 2, 1)
	FIELD(UART0_IS7816, INITD, 4, 1)
	FIELD(UART0_IS7816, BWT, 5, 1)
	FIELD(UART0_IS7816, CWT, 6, 1)
	FIELD(UART0_IS7816, WT, 7, 1)
REG8(UART0_WP7816T0, 0x1b)
	FIELD(UART0_WP7816T0, WI, 0, 8)
REG8(UART0_WP7816T1, 0x1b)
	FIELD(UART0_WP7816T1, BWI, 0, 4)
	FIELD(UART0_WP7816T1, CWI, 4, 4)
REG8(UART0_WN7816, 0x1c)
	FIELD(UART0_WN7816, GTN, 0, 8)
REG8(UART0_WF7816, 0x1d)
	FIELD(UART0_WF7816, GTFD, 0, 8)
REG8(UART0_ET7816, 0x1e)
	FIELD(UART0_ET7816, RXTHRESHOLD, 0, 4)
	FIELD(UART0_ET7816, TXTHRESHOLD, 4, 4)
REG8(UART0_TL7816, 0x1f)
	FIELD(UART0_TL7816, TLEN, 0, 8)


__attribute__((unused))
static void frdmk64f_uart0_update(FRDMK64FUART0 *t) {
	int conditions = ((t->S1 & 0x1) || (t->S1 & 0x2) || (t->S1 & 0x8) || (t->C2 & 0x10) || ((t->C2 & 0x40) && (t->S1 & 0x40)) || ((t->C2 & 0x80) && (t->S1 & 0x80)) || ((t->S1 & 0x20) && (t->C2 & 0x20)));
	qemu_set_irq(t->irq[1], conditions);

}

static int frdmk64f_uart0_can_receive(void *opaque) {
	return 1;
}

static void frdmk64f_uart0_receive(void *opaque, const uint8_t *buf, int size) {
	FRDMK64FUART0 *t = FRDMK64F_UART0(opaque);

	t->D = *buf;
	t->S1 |= 0x20;

	frdmk64f_uart0_update(t);
}

static gboolean frdmk64f_uart0_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	FRDMK64FUART0 *t = FRDMK64F_UART0(opaque);
	int ret;

	t->watch_tag = 0;
	t->S1 &= (~(0x40));
	t->S1 &= (~(0x80));

	frdmk64f_uart0_update(t);

	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->D), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, frdmk64f_uart0_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->S1 |= 0x40;
	t->S1 |= 0x80;

	frdmk64f_uart0_update(t);

	return FALSE;
}

static void frdmk64f_uart0_register_reset(FRDMK64FUART0 *t) {
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
	t->C4 = 0x0;
	t->C5 = 0x0;
	t->ED = 0x0;
	t->MODEM = 0x0;
	t->IR = 0x0;
	t->PFIFO = 0x0;
	t->CFIFO = 0x0;
	t->SFIFO = 0xc0;
	t->TWFIFO = 0x0;
	t->TCFIFO = 0x0;
	t->RWFIFO = 0x1;
	t->RCFIFO = 0x0;
	t->C7816 = 0x0;
	t->IE7816 = 0x0;
	t->IS7816 = 0x0;
	t->WP7816T0 = 0xa;
	t->WP7816T1 = 0xa;
	t->WN7816 = 0x0;
	t->WF7816 = 0x1;
	t->ET7816 = 0x0;
	t->TL7816 = 0x0;
	t->S1 |= 0x40;
	t->S1 |= 0x80;

}

static uint64_t frdmk64f_uart0_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMK64FUART0 *t = FRDMK64F_UART0(opaque);
	uint64_t ret;

	switch (offset) {
		case A_UART0_BDH:
			ret = t->BDH;
			break;
		case A_UART0_BDL:
			ret = t->BDL;
			break;
		case A_UART0_C1:
			ret = t->C1;
			break;
		case A_UART0_C2:
			ret = t->C2;
			break;
		case A_UART0_S1:
			ret = t->S1;
			break;
		case A_UART0_S2:
			ret = t->S2;
			break;
		case A_UART0_C3:
			ret = t->C3;
			break;
		case A_UART0_D:
			ret = t->D;
			t->S1 &= (~(0x20));
			qemu_chr_fe_accept_input(&(t->chr));
			frdmk64f_uart0_update(t);
			break;
		case A_UART0_MA1:
			ret = t->MA1;
			break;
		case A_UART0_MA2:
			ret = t->MA2;
			break;
		case A_UART0_C4:
			ret = t->C4;
			break;
		case A_UART0_C5:
			ret = t->C5;
			break;
		case A_UART0_ED:
			ret = t->ED;
			break;
		case A_UART0_MODEM:
			ret = t->MODEM;
			break;
		case A_UART0_IR:
			ret = t->IR;
			break;
		case A_UART0_PFIFO:
			ret = t->PFIFO;
			break;
		case A_UART0_CFIFO:
			ret = t->CFIFO;
			break;
		case A_UART0_SFIFO:
			ret = t->SFIFO;
			break;
		case A_UART0_TWFIFO:
			ret = t->TWFIFO;
			break;
		case A_UART0_TCFIFO:
			ret = t->TCFIFO;
			break;
		case A_UART0_RWFIFO:
			ret = t->RWFIFO;
			break;
		case A_UART0_RCFIFO:
			ret = t->RCFIFO;
			break;
		case A_UART0_C7816:
			ret = t->C7816;
			break;
		case A_UART0_IE7816:
			ret = t->IE7816;
			break;
		case A_UART0_IS7816:
			ret = t->IS7816;
			break;
		case A_UART0_WP7816T0:
			ret = t->WP7816T0;
			break;
		case A_UART0_WN7816:
			ret = t->WN7816;
			break;
		case A_UART0_WF7816:
			ret = t->WF7816;
			break;
		case A_UART0_ET7816:
			ret = t->ET7816;
			break;
		case A_UART0_TL7816:
			ret = t->TL7816;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK64F UART0 read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmk64f_uart0_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMK64FUART0 *t = FRDMK64F_UART0(opaque);


	switch (offset) {

		default: break;
	}

	switch (offset) {
		case A_UART0_BDH:
			t->BDH = value;
			break;
		case A_UART0_BDL:
			t->BDL = value;
			break;
		case A_UART0_C1:
			t->C1 = value;
			break;
		case A_UART0_C2:
			t->C2 = value;
			frdmk64f_uart0_update(t);
			break;
		case A_UART0_S2:
			t->S2 = value;
			break;
		case A_UART0_C3:
			t->C3 = value;
			break;
		case A_UART0_D:
			t->D = value;
			frdmk64f_uart0_transmit(NULL, G_IO_OUT, t);
			break;
		case A_UART0_MA1:
			t->MA1 = value;
			break;
		case A_UART0_MA2:
			t->MA2 = value;
			break;
		case A_UART0_C4:
			t->C4 = value;
			break;
		case A_UART0_C5:
			t->C5 = value;
			break;
		case A_UART0_MODEM:
			t->MODEM = value;
			break;
		case A_UART0_IR:
			t->IR = value;
			break;
		case A_UART0_PFIFO:
			t->PFIFO = value;
			break;
		case A_UART0_CFIFO:
			t->CFIFO = value;
			break;
		case A_UART0_SFIFO:
			t->SFIFO = value;
			break;
		case A_UART0_TWFIFO:
			t->TWFIFO = value;
			break;
		case A_UART0_RWFIFO:
			t->RWFIFO = value;
			break;
		case A_UART0_C7816:
			t->C7816 = value;
			break;
		case A_UART0_IE7816:
			t->IE7816 = value;
			break;
		case A_UART0_IS7816:
			t->IS7816 = value;
			break;
		case A_UART0_WP7816T0:
			t->WP7816T0 = value;
			break;
		case A_UART0_WN7816:
			t->WN7816 = value;
			break;
		case A_UART0_WF7816:
			t->WF7816 = value;
			break;
		case A_UART0_ET7816:
			t->ET7816 = value;
			break;
		case A_UART0_TL7816:
			t->TL7816 = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK64F UART0 write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmk64f_uart0_ops = {
	.read = frdmk64f_uart0_read,
	.write = frdmk64f_uart0_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmk64f_uart0_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMK64FUART0 *t = FRDMK64F_UART0(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmk64f_uart0_ops, t, "frdmk64f-uart0", FRDMK64F_UART0_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 3; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void frdmk64f_uart0_realize(DeviceState *dev, Error **errp) {
	FRDMK64FUART0 *t = FRDMK64F_UART0(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), frdmk64f_uart0_can_receive, frdmk64f_uart0_receive,
		NULL, NULL, t, NULL, true);

}

static void frdmk64f_uart0_reset_enter(Object *obj, ResetType type) {
	FRDMK64FUART0 *t = FRDMK64F_UART0(obj);
	frdmk64f_uart0_register_reset(t);
}

static const VMStateDescription frdmk64f_uart0_vmstate = {
	.name = "frdmk64f-uart0",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT8(BDH, FRDMK64FUART0),
		VMSTATE_UINT8(BDL, FRDMK64FUART0),
		VMSTATE_UINT8(C1, FRDMK64FUART0),
		VMSTATE_UINT8(C2, FRDMK64FUART0),
		VMSTATE_UINT8(S1, FRDMK64FUART0),
		VMSTATE_UINT8(S2, FRDMK64FUART0),
		VMSTATE_UINT8(C3, FRDMK64FUART0),
		VMSTATE_UINT8(D, FRDMK64FUART0),
		VMSTATE_UINT8(MA1, FRDMK64FUART0),
		VMSTATE_UINT8(MA2, FRDMK64FUART0),
		VMSTATE_UINT8(C4, FRDMK64FUART0),
		VMSTATE_UINT8(C5, FRDMK64FUART0),
		VMSTATE_UINT8(ED, FRDMK64FUART0),
		VMSTATE_UINT8(MODEM, FRDMK64FUART0),
		VMSTATE_UINT8(IR, FRDMK64FUART0),
		VMSTATE_UINT8(PFIFO, FRDMK64FUART0),
		VMSTATE_UINT8(CFIFO, FRDMK64FUART0),
		VMSTATE_UINT8(SFIFO, FRDMK64FUART0),
		VMSTATE_UINT8(TWFIFO, FRDMK64FUART0),
		VMSTATE_UINT8(TCFIFO, FRDMK64FUART0),
		VMSTATE_UINT8(RWFIFO, FRDMK64FUART0),
		VMSTATE_UINT8(RCFIFO, FRDMK64FUART0),
		VMSTATE_UINT8(C7816, FRDMK64FUART0),
		VMSTATE_UINT8(IE7816, FRDMK64FUART0),
		VMSTATE_UINT8(IS7816, FRDMK64FUART0),
		VMSTATE_UINT8(WP7816T0, FRDMK64FUART0),
		VMSTATE_UINT8(WP7816T1, FRDMK64FUART0),
		VMSTATE_UINT8(WN7816, FRDMK64FUART0),
		VMSTATE_UINT8(WF7816, FRDMK64FUART0),
		VMSTATE_UINT8(ET7816, FRDMK64FUART0),
		VMSTATE_UINT8(TL7816, FRDMK64FUART0),

		VMSTATE_END_OF_LIST()
	}
};

static Property frdmk64f_uart0_properties[] = {
	DEFINE_PROP_CHR("chardev", FRDMK64FUART0, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void frdmk64f_uart0_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmk64f_uart0_vmstate;
	dc->realize = frdmk64f_uart0_realize;
	rc->phases.enter = frdmk64f_uart0_reset_enter;
	device_class_set_props(dc, frdmk64f_uart0_properties);

}

static const TypeInfo frdmk64f_uart0_info = {
	.name = TYPE_FRDMK64F_UART0,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMK64FUART0),
	.instance_init = frdmk64f_uart0_init,
	.class_init = frdmk64f_uart0_class_init,
};

static void frdmk64f_uart0_register_types(void) {
	type_register_static(&frdmk64f_uart0_info);
}

type_init(frdmk64f_uart0_register_types);

#define TYPE_FRDMK64F_UART3 "frdmk64f-uart3"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMK64FUART3, FRDMK64F_UART3)

struct FRDMK64FUART3 {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[2];

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
	uint8_t ED;
	uint8_t MODEM;
	uint8_t IR;
	uint8_t PFIFO;
	uint8_t CFIFO;
	uint8_t SFIFO;
	uint8_t TWFIFO;
	uint8_t TCFIFO;
	uint8_t RWFIFO;
	uint8_t RCFIFO;
	/* chardev backend */
	CharBackend chr;
	guint watch_tag;

	/* base */
	uint32_t base;

};
#define FRDMK64F_UART3_SIZE				0x17

REG8(UART3_BDH, 0x0)
	FIELD(UART3_BDH, SBR, 0, 5)
	FIELD(UART3_BDH, SBNS, 5, 1)
	FIELD(UART3_BDH, RXEDGIE, 6, 1)
	FIELD(UART3_BDH, LBKDIE, 7, 1)
REG8(UART3_BDL, 0x1)
	FIELD(UART3_BDL, SBR, 0, 8)
REG8(UART3_C1, 0x2)
	FIELD(UART3_C1, PT, 0, 1)
	FIELD(UART3_C1, PE, 1, 1)
	FIELD(UART3_C1, ILT, 2, 1)
	FIELD(UART3_C1, WAKE, 3, 1)
	FIELD(UART3_C1, M, 4, 1)
	FIELD(UART3_C1, RSRC, 5, 1)
	FIELD(UART3_C1, UARTSWAI, 6, 1)
	FIELD(UART3_C1, LOOPS, 7, 1)
REG8(UART3_C2, 0x3)
	FIELD(UART3_C2, SBK, 0, 1)
	FIELD(UART3_C2, RWU, 1, 1)
	FIELD(UART3_C2, RE, 2, 1)
	FIELD(UART3_C2, TE, 3, 1)
	FIELD(UART3_C2, ILIE, 4, 1)
	FIELD(UART3_C2, RIE, 5, 1)
	FIELD(UART3_C2, TCIE, 6, 1)
	FIELD(UART3_C2, TIE, 7, 1)
REG8(UART3_S1, 0x4)
	FIELD(UART3_S1, PF, 0, 1)
	FIELD(UART3_S1, FE, 1, 1)
	FIELD(UART3_S1, NF, 2, 1)
	FIELD(UART3_S1, OR, 3, 1)
	FIELD(UART3_S1, IDLE, 4, 1)
	FIELD(UART3_S1, RDRF, 5, 1)
	FIELD(UART3_S1, TC, 6, 1)
	FIELD(UART3_S1, TDRE, 7, 1)
REG8(UART3_S2, 0x5)
	FIELD(UART3_S2, RAF, 0, 1)
	FIELD(UART3_S2, LBKDE, 1, 1)
	FIELD(UART3_S2, BRK13, 2, 1)
	FIELD(UART3_S2, RWUID, 3, 1)
	FIELD(UART3_S2, RXINV, 4, 1)
	FIELD(UART3_S2, MSBF, 5, 1)
	FIELD(UART3_S2, RXEDGIF, 6, 1)
	FIELD(UART3_S2, LBKDIF, 7, 1)
REG8(UART3_C3, 0x6)
	FIELD(UART3_C3, PEIE, 0, 1)
	FIELD(UART3_C3, FEIE, 1, 1)
	FIELD(UART3_C3, NEIE, 2, 1)
	FIELD(UART3_C3, ORIE, 3, 1)
	FIELD(UART3_C3, TXINV, 4, 1)
	FIELD(UART3_C3, TXDIR, 5, 1)
	FIELD(UART3_C3, T8, 6, 1)
	FIELD(UART3_C3, R8, 7, 1)
REG8(UART3_D, 0x7)
	FIELD(UART3_D, RT, 0, 8)
REG8(UART3_MA1, 0x8)
	FIELD(UART3_MA1, MA, 0, 8)
REG8(UART3_MA2, 0x9)
	FIELD(UART3_MA2, MA, 0, 8)
REG8(UART3_C4, 0xa)
	FIELD(UART3_C4, BRFA, 0, 5)
	FIELD(UART3_C4, M10, 5, 1)
	FIELD(UART3_C4, MAEN2, 6, 1)
	FIELD(UART3_C4, MAEN1, 7, 1)
REG8(UART3_C5, 0xb)
	FIELD(UART3_C5, LBKDDMAS, 3, 1)
	FIELD(UART3_C5, ILDMAS, 4, 1)
	FIELD(UART3_C5, RDMAS, 5, 1)
	FIELD(UART3_C5, TCDMAS, 6, 1)
	FIELD(UART3_C5, TDMAS, 7, 1)
REG8(UART3_ED, 0xc)
	FIELD(UART3_ED, PARITYE, 6, 1)
	FIELD(UART3_ED, NOISY, 7, 1)
REG8(UART3_MODEM, 0xd)
	FIELD(UART3_MODEM, TXCTSE, 0, 1)
	FIELD(UART3_MODEM, TXRTSE, 1, 1)
	FIELD(UART3_MODEM, TXRTSPOL, 2, 1)
	FIELD(UART3_MODEM, RXRTSE, 3, 1)
REG8(UART3_IR, 0xe)
	FIELD(UART3_IR, TNP, 0, 2)
	FIELD(UART3_IR, IREN, 2, 1)
REG8(UART3_PFIFO, 0x10)
	FIELD(UART3_PFIFO, RXFIFOSIZE, 0, 3)
	FIELD(UART3_PFIFO, RXFE, 3, 1)
	FIELD(UART3_PFIFO, TXFIFOSIZE, 4, 3)
	FIELD(UART3_PFIFO, TXFE, 7, 1)
REG8(UART3_CFIFO, 0x11)
	FIELD(UART3_CFIFO, RXUFE, 0, 1)
	FIELD(UART3_CFIFO, TXOFE, 1, 1)
	FIELD(UART3_CFIFO, RXOFE, 2, 1)
	FIELD(UART3_CFIFO, RXFLUSH, 6, 1)
	FIELD(UART3_CFIFO, TXFLUSH, 7, 1)
REG8(UART3_SFIFO, 0x12)
	FIELD(UART3_SFIFO, RXUF, 0, 1)
	FIELD(UART3_SFIFO, TXOF, 1, 1)
	FIELD(UART3_SFIFO, RXOF, 2, 1)
	FIELD(UART3_SFIFO, RXEMPT, 6, 1)
	FIELD(UART3_SFIFO, TXEMPT, 7, 1)
REG8(UART3_TWFIFO, 0x13)
	FIELD(UART3_TWFIFO, TXWATER, 0, 8)
REG8(UART3_TCFIFO, 0x14)
	FIELD(UART3_TCFIFO, TXCOUNT, 0, 8)
REG8(UART3_RWFIFO, 0x15)
	FIELD(UART3_RWFIFO, RXWATER, 0, 8)
REG8(UART3_RCFIFO, 0x16)
	FIELD(UART3_RCFIFO, RXCOUNT, 0, 8)


__attribute__((unused))
static void frdmk64f_uart3_update(FRDMK64FUART3 *t) {
	int conditions = ((t->S1 & 0x1) || (t->S1 & 0x2) || (t->S1 & 0x8) || (t->C2 & 0x10) || ((t->C2 & 0x40) && (t->S1 & 0x40)) || ((t->C2 & 0x80) && (t->S1 & 0x80)) || ((t->S1 & 0x20) && (t->C2 & 0x20)));
	qemu_set_irq(t->irq[0], conditions);

}

static int frdmk64f_uart3_can_receive(void *opaque) {
	return 1;
}

static void frdmk64f_uart3_receive(void *opaque, const uint8_t *buf, int size) {
	FRDMK64FUART3 *t = FRDMK64F_UART3(opaque);

	t->D = *buf;
	t->S1 |= 0x20;

	frdmk64f_uart3_update(t);
}

static gboolean frdmk64f_uart3_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	FRDMK64FUART3 *t = FRDMK64F_UART3(opaque);
	int ret;

	t->watch_tag = 0;
	t->S1 &= (~(0x40));
	t->S1 &= (~(0x80));

	frdmk64f_uart3_update(t);

	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->D), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, frdmk64f_uart3_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->S1 |= 0x40;
	t->S1 |= 0x80;

	frdmk64f_uart3_update(t);

	return FALSE;
}

static void frdmk64f_uart3_register_reset(FRDMK64FUART3 *t) {
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
	t->C4 = 0x0;
	t->C5 = 0x0;
	t->ED = 0x0;
	t->MODEM = 0x0;
	t->IR = 0x0;
	t->PFIFO = 0x0;
	t->CFIFO = 0x0;
	t->SFIFO = 0xc0;
	t->TWFIFO = 0x0;
	t->TCFIFO = 0x0;
	t->RWFIFO = 0x1;
	t->RCFIFO = 0x0;
	t->S1 |= 0x40;
	t->S1 |= 0x80;

}

static uint64_t frdmk64f_uart3_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMK64FUART3 *t = FRDMK64F_UART3(opaque);
	uint64_t ret;

	switch (offset) {
		case A_UART3_BDH:
			ret = t->BDH;
			break;
		case A_UART3_BDL:
			ret = t->BDL;
			break;
		case A_UART3_C1:
			ret = t->C1;
			break;
		case A_UART3_C2:
			ret = t->C2;
			break;
		case A_UART3_S1:
			ret = t->S1;
			break;
		case A_UART3_S2:
			ret = t->S2;
			break;
		case A_UART3_C3:
			ret = t->C3;
			break;
		case A_UART3_D:
			ret = t->D;
			t->S1 &= (~(0x20));
			qemu_chr_fe_accept_input(&(t->chr));
			frdmk64f_uart3_update(t);
			break;
		case A_UART3_MA1:
			ret = t->MA1;
			break;
		case A_UART3_MA2:
			ret = t->MA2;
			break;
		case A_UART3_C4:
			ret = t->C4;
			break;
		case A_UART3_C5:
			ret = t->C5;
			break;
		case A_UART3_ED:
			ret = t->ED;
			break;
		case A_UART3_MODEM:
			ret = t->MODEM;
			break;
		case A_UART3_IR:
			ret = t->IR;
			break;
		case A_UART3_PFIFO:
			ret = t->PFIFO;
			break;
		case A_UART3_CFIFO:
			ret = t->CFIFO;
			break;
		case A_UART3_SFIFO:
			ret = t->SFIFO;
			break;
		case A_UART3_TWFIFO:
			ret = t->TWFIFO;
			break;
		case A_UART3_TCFIFO:
			ret = t->TCFIFO;
			break;
		case A_UART3_RWFIFO:
			ret = t->RWFIFO;
			break;
		case A_UART3_RCFIFO:
			ret = t->RCFIFO;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK64F UART3 read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmk64f_uart3_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMK64FUART3 *t = FRDMK64F_UART3(opaque);


	switch (offset) {

		default: break;
	}

	switch (offset) {
		case A_UART3_BDH:
			t->BDH = value;
			break;
		case A_UART3_BDL:
			t->BDL = value;
			break;
		case A_UART3_C1:
			t->C1 = value;
			break;
		case A_UART3_C2:
			t->C2 = value;
			frdmk64f_uart3_update(t);
			break;
		case A_UART3_S2:
			t->S2 = value;
			break;
		case A_UART3_C3:
			t->C3 = value;
			break;
		case A_UART3_D:
			t->D = value;
			frdmk64f_uart3_transmit(NULL, G_IO_OUT, t);
			break;
		case A_UART3_MA1:
			t->MA1 = value;
			break;
		case A_UART3_MA2:
			t->MA2 = value;
			break;
		case A_UART3_C4:
			t->C4 = value;
			break;
		case A_UART3_C5:
			t->C5 = value;
			break;
		case A_UART3_MODEM:
			t->MODEM = value;
			break;
		case A_UART3_IR:
			t->IR = value;
			break;
		case A_UART3_PFIFO:
			t->PFIFO = value;
			break;
		case A_UART3_CFIFO:
			t->CFIFO = value;
			break;
		case A_UART3_SFIFO:
			t->SFIFO = value;
			break;
		case A_UART3_TWFIFO:
			t->TWFIFO = value;
			break;
		case A_UART3_RWFIFO:
			t->RWFIFO = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK64F UART3 write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmk64f_uart3_ops = {
	.read = frdmk64f_uart3_read,
	.write = frdmk64f_uart3_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmk64f_uart3_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMK64FUART3 *t = FRDMK64F_UART3(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmk64f_uart3_ops, t, "frdmk64f-uart3", FRDMK64F_UART3_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 2; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void frdmk64f_uart3_realize(DeviceState *dev, Error **errp) {
	FRDMK64FUART3 *t = FRDMK64F_UART3(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), frdmk64f_uart3_can_receive, frdmk64f_uart3_receive,
		NULL, NULL, t, NULL, true);

}

static void frdmk64f_uart3_reset_enter(Object *obj, ResetType type) {
	FRDMK64FUART3 *t = FRDMK64F_UART3(obj);
	frdmk64f_uart3_register_reset(t);
}

static const VMStateDescription frdmk64f_uart3_vmstate = {
	.name = "frdmk64f-uart3",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT8(BDH, FRDMK64FUART3),
		VMSTATE_UINT8(BDL, FRDMK64FUART3),
		VMSTATE_UINT8(C1, FRDMK64FUART3),
		VMSTATE_UINT8(C2, FRDMK64FUART3),
		VMSTATE_UINT8(S1, FRDMK64FUART3),
		VMSTATE_UINT8(S2, FRDMK64FUART3),
		VMSTATE_UINT8(C3, FRDMK64FUART3),
		VMSTATE_UINT8(D, FRDMK64FUART3),
		VMSTATE_UINT8(MA1, FRDMK64FUART3),
		VMSTATE_UINT8(MA2, FRDMK64FUART3),
		VMSTATE_UINT8(C4, FRDMK64FUART3),
		VMSTATE_UINT8(C5, FRDMK64FUART3),
		VMSTATE_UINT8(ED, FRDMK64FUART3),
		VMSTATE_UINT8(MODEM, FRDMK64FUART3),
		VMSTATE_UINT8(IR, FRDMK64FUART3),
		VMSTATE_UINT8(PFIFO, FRDMK64FUART3),
		VMSTATE_UINT8(CFIFO, FRDMK64FUART3),
		VMSTATE_UINT8(SFIFO, FRDMK64FUART3),
		VMSTATE_UINT8(TWFIFO, FRDMK64FUART3),
		VMSTATE_UINT8(TCFIFO, FRDMK64FUART3),
		VMSTATE_UINT8(RWFIFO, FRDMK64FUART3),
		VMSTATE_UINT8(RCFIFO, FRDMK64FUART3),

		VMSTATE_END_OF_LIST()
	}
};

static Property frdmk64f_uart3_properties[] = {
	DEFINE_PROP_CHR("chardev", FRDMK64FUART3, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void frdmk64f_uart3_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmk64f_uart3_vmstate;
	dc->realize = frdmk64f_uart3_realize;
	rc->phases.enter = frdmk64f_uart3_reset_enter;
	device_class_set_props(dc, frdmk64f_uart3_properties);

}

static const TypeInfo frdmk64f_uart3_info = {
	.name = TYPE_FRDMK64F_UART3,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMK64FUART3),
	.instance_init = frdmk64f_uart3_init,
	.class_init = frdmk64f_uart3_class_init,
};

static void frdmk64f_uart3_register_types(void) {
	type_register_static(&frdmk64f_uart3_info);
}

type_init(frdmk64f_uart3_register_types);

#define TYPE_FRDMK64F_SPI "frdmk64f-spi"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMK64FSPI, FRDMK64F_SPI)

struct FRDMK64FSPI {
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
#define FRDMK64F_SPI_SIZE				0x8c

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
	FIELD(SPI_CTAR_SLAVE, FMSZ, 27, 5)
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
	FIELD(SPI_PUSHR_SLAVE, TXDATA, 0, 32)
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


static int frdmk64f_spi_can_receive(void *opaque) {
	return 1;
}

static void frdmk64f_spi_receive(void *opaque, const uint8_t *buf, int size) {
	FRDMK64FSPI *t = FRDMK64F_SPI(opaque);

	t->POPR = *buf;
	t->SR |= 0x40;
	t->SR |= 0x80;
	t->SR |= 0x10;
	t->SR |= 0x20;
	t->SR |= 0x20000;


}

static gboolean frdmk64f_spi_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	FRDMK64FSPI *t = FRDMK64F_SPI(opaque);
	int ret;

	t->watch_tag = 0;
	t->SR &= (~(0x80000000));
	t->MCR |= 0x80000000;
	t->SR &= (~(0x20000));
	t->SR &= (~(0x2000000));



	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->PUSHR), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, frdmk64f_spi_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->MCR &= (~(0x80000000));
	t->SR |= 0x80000000;
	t->SR |= 0x2000000;
	t->SR |= 0x20000;



	return FALSE;
}

static void frdmk64f_spi_register_reset(FRDMK64FSPI *t) {
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
	t->MCR &= (~(0x80000000));
	t->SR |= 0x80000000;
	t->SR |= 0x2000000;
	t->SR |= 0x20000;

}

static uint64_t frdmk64f_spi_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMK64FSPI *t = FRDMK64F_SPI(opaque);
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
			t->SR &= (~(0x40));
			t->SR &= (~(0x20000));
			t->SR &= (~(0x10));
			t->SR &= (~(0x80));
			t->SR &= (~(0x20));
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
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK64F SPI read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmk64f_spi_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMK64FSPI *t = FRDMK64F_SPI(opaque);


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
			frdmk64f_spi_transmit(NULL, G_IO_OUT, t);
			break;
		case A_SPI_CTAR1:
			t->CTAR1 = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK64F SPI write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmk64f_spi_ops = {
	.read = frdmk64f_spi_read,
	.write = frdmk64f_spi_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmk64f_spi_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMK64FSPI *t = FRDMK64F_SPI(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmk64f_spi_ops, t, "frdmk64f-spi", FRDMK64F_SPI_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void frdmk64f_spi_realize(DeviceState *dev, Error **errp) {
	FRDMK64FSPI *t = FRDMK64F_SPI(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), frdmk64f_spi_can_receive, frdmk64f_spi_receive,
		NULL, NULL, t, NULL, true);

}

static void frdmk64f_spi_reset_enter(Object *obj, ResetType type) {
	FRDMK64FSPI *t = FRDMK64F_SPI(obj);
	frdmk64f_spi_register_reset(t);
}

static const VMStateDescription frdmk64f_spi_vmstate = {
	.name = "frdmk64f-spi",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(MCR, FRDMK64FSPI),
		VMSTATE_UINT32(TCR, FRDMK64FSPI),
		VMSTATE_UINT32(CTAR_SLAVE, FRDMK64FSPI),
		VMSTATE_UINT32(SR, FRDMK64FSPI),
		VMSTATE_UINT32(RSER, FRDMK64FSPI),
		VMSTATE_UINT32(PUSHR, FRDMK64FSPI),
		VMSTATE_UINT32(PUSHR_SLAVE, FRDMK64FSPI),
		VMSTATE_UINT32(POPR, FRDMK64FSPI),
		VMSTATE_UINT32(CTAR0, FRDMK64FSPI),
		VMSTATE_UINT32(CTAR1, FRDMK64FSPI),
		VMSTATE_UINT32(TXFR0, FRDMK64FSPI),
		VMSTATE_UINT32(TXFR1, FRDMK64FSPI),
		VMSTATE_UINT32(TXFR2, FRDMK64FSPI),
		VMSTATE_UINT32(TXFR3, FRDMK64FSPI),
		VMSTATE_UINT32(RXFR0, FRDMK64FSPI),
		VMSTATE_UINT32(RXFR1, FRDMK64FSPI),
		VMSTATE_UINT32(RXFR2, FRDMK64FSPI),
		VMSTATE_UINT32(RXFR3, FRDMK64FSPI),

		VMSTATE_END_OF_LIST()
	}
};

static Property frdmk64f_spi_properties[] = {
	DEFINE_PROP_CHR("chardev", FRDMK64FSPI, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void frdmk64f_spi_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmk64f_spi_vmstate;
	dc->realize = frdmk64f_spi_realize;
	rc->phases.enter = frdmk64f_spi_reset_enter;
	device_class_set_props(dc, frdmk64f_spi_properties);

}

static const TypeInfo frdmk64f_spi_info = {
	.name = TYPE_FRDMK64F_SPI,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMK64FSPI),
	.instance_init = frdmk64f_spi_init,
	.class_init = frdmk64f_spi_class_init,
};

static void frdmk64f_spi_register_types(void) {
	type_register_static(&frdmk64f_spi_info);
}

type_init(frdmk64f_spi_register_types);

#define TYPE_FRDMK64F_WDOG "frdmk64f-wdog"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMK64FWDOG, FRDMK64F_WDOG)

struct FRDMK64FWDOG {
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
#define FRDMK64F_WDOG_SIZE				0x18

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


static void frdmk64f_wdog_register_reset(FRDMK64FWDOG *t) {
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

static uint64_t frdmk64f_wdog_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMK64FWDOG *t = FRDMK64F_WDOG(opaque);
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
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK64F WDOG read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmk64f_wdog_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMK64FWDOG *t = FRDMK64F_WDOG(opaque);


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
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK64F WDOG write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmk64f_wdog_ops = {
	.read = frdmk64f_wdog_read,
	.write = frdmk64f_wdog_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmk64f_wdog_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMK64FWDOG *t = FRDMK64F_WDOG(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmk64f_wdog_ops, t, "frdmk64f-wdog", FRDMK64F_WDOG_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void frdmk64f_wdog_realize(DeviceState *dev, Error **errp) {
	return;

}

static void frdmk64f_wdog_reset_enter(Object *obj, ResetType type) {
	FRDMK64FWDOG *t = FRDMK64F_WDOG(obj);
	frdmk64f_wdog_register_reset(t);
}

static const VMStateDescription frdmk64f_wdog_vmstate = {
	.name = "frdmk64f-wdog",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT16(STCTRLH, FRDMK64FWDOG),
		VMSTATE_UINT16(STCTRLL, FRDMK64FWDOG),
		VMSTATE_UINT16(TOVALH, FRDMK64FWDOG),
		VMSTATE_UINT16(TOVALL, FRDMK64FWDOG),
		VMSTATE_UINT16(WINH, FRDMK64FWDOG),
		VMSTATE_UINT16(WINL, FRDMK64FWDOG),
		VMSTATE_UINT16(REFRESH, FRDMK64FWDOG),
		VMSTATE_UINT16(UNLOCK, FRDMK64FWDOG),
		VMSTATE_UINT16(TMROUTH, FRDMK64FWDOG),
		VMSTATE_UINT16(TMROUTL, FRDMK64FWDOG),
		VMSTATE_UINT16(RSTCNT, FRDMK64FWDOG),
		VMSTATE_UINT16(PRESC, FRDMK64FWDOG),

		VMSTATE_END_OF_LIST()
	}
};

static void frdmk64f_wdog_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmk64f_wdog_vmstate;
	dc->realize = frdmk64f_wdog_realize;
	rc->phases.enter = frdmk64f_wdog_reset_enter;

}

static const TypeInfo frdmk64f_wdog_info = {
	.name = TYPE_FRDMK64F_WDOG,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMK64FWDOG),
	.instance_init = frdmk64f_wdog_init,
	.class_init = frdmk64f_wdog_class_init,
};

static void frdmk64f_wdog_register_types(void) {
	type_register_static(&frdmk64f_wdog_info);
}

type_init(frdmk64f_wdog_register_types);

#define TYPE_FRDMK64F_SYSMPU "frdmk64f-sysmpu"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMK64FSYSMPU, FRDMK64F_SYSMPU)

struct FRDMK64FSYSMPU {
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
#define FRDMK64F_SYSMPU_SIZE				0x830

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


static void frdmk64f_sysmpu_register_reset(FRDMK64FSYSMPU *t) {
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

static uint64_t frdmk64f_sysmpu_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMK64FSYSMPU *t = FRDMK64F_SYSMPU(opaque);
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
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK64F SYSMPU read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmk64f_sysmpu_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMK64FSYSMPU *t = FRDMK64F_SYSMPU(opaque);


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
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK64F SYSMPU write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmk64f_sysmpu_ops = {
	.read = frdmk64f_sysmpu_read,
	.write = frdmk64f_sysmpu_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmk64f_sysmpu_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMK64FSYSMPU *t = FRDMK64F_SYSMPU(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmk64f_sysmpu_ops, t, "frdmk64f-sysmpu", FRDMK64F_SYSMPU_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void frdmk64f_sysmpu_realize(DeviceState *dev, Error **errp) {
	return;

}

static void frdmk64f_sysmpu_reset_enter(Object *obj, ResetType type) {
	FRDMK64FSYSMPU *t = FRDMK64F_SYSMPU(obj);
	frdmk64f_sysmpu_register_reset(t);
}

static const VMStateDescription frdmk64f_sysmpu_vmstate = {
	.name = "frdmk64f-sysmpu",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CESR, FRDMK64FSYSMPU),
		VMSTATE_UINT32(EAR0, FRDMK64FSYSMPU),
		VMSTATE_UINT32(EAR1, FRDMK64FSYSMPU),
		VMSTATE_UINT32(EAR2, FRDMK64FSYSMPU),
		VMSTATE_UINT32(EAR3, FRDMK64FSYSMPU),
		VMSTATE_UINT32(EAR4, FRDMK64FSYSMPU),
		VMSTATE_UINT32(EDR0, FRDMK64FSYSMPU),
		VMSTATE_UINT32(EDR1, FRDMK64FSYSMPU),
		VMSTATE_UINT32(EDR2, FRDMK64FSYSMPU),
		VMSTATE_UINT32(EDR3, FRDMK64FSYSMPU),
		VMSTATE_UINT32(EDR4, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD0_WORD0, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD1_WORD0, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD2_WORD0, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD3_WORD0, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD4_WORD0, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD5_WORD0, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD6_WORD0, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD7_WORD0, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD8_WORD0, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD9_WORD0, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD10_WORD0, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD11_WORD0, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD0_WORD1, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD1_WORD1, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD2_WORD1, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD3_WORD1, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD4_WORD1, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD5_WORD1, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD6_WORD1, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD7_WORD1, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD8_WORD1, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD9_WORD1, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD10_WORD1, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD11_WORD1, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD0_WORD2, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD1_WORD2, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD2_WORD2, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD3_WORD2, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD4_WORD2, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD5_WORD2, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD6_WORD2, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD7_WORD2, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD8_WORD2, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD9_WORD2, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD10_WORD2, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD11_WORD2, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD0_WORD3, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD1_WORD3, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD2_WORD3, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD3_WORD3, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD4_WORD3, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD5_WORD3, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD6_WORD3, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD7_WORD3, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD8_WORD3, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD9_WORD3, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD10_WORD3, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGD11_WORD3, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGDAAC0, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGDAAC1, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGDAAC2, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGDAAC3, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGDAAC4, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGDAAC5, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGDAAC6, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGDAAC7, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGDAAC8, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGDAAC9, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGDAAC10, FRDMK64FSYSMPU),
		VMSTATE_UINT32(RGDAAC11, FRDMK64FSYSMPU),

		VMSTATE_END_OF_LIST()
	}
};

static void frdmk64f_sysmpu_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmk64f_sysmpu_vmstate;
	dc->realize = frdmk64f_sysmpu_realize;
	rc->phases.enter = frdmk64f_sysmpu_reset_enter;

}

static const TypeInfo frdmk64f_sysmpu_info = {
	.name = TYPE_FRDMK64F_SYSMPU,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMK64FSYSMPU),
	.instance_init = frdmk64f_sysmpu_init,
	.class_init = frdmk64f_sysmpu_class_init,
};

static void frdmk64f_sysmpu_register_types(void) {
	type_register_static(&frdmk64f_sysmpu_info);
}

type_init(frdmk64f_sysmpu_register_types);

#define TYPE_FRDMK64F_SIM "frdmk64f-sim"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMK64FSIM, FRDMK64F_SIM)

struct FRDMK64FSIM {
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
	/* base */
	uint32_t base;

};
#define FRDMK64F_SIM_SIZE				0x1064

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
	FIELD(SIM_SOPT2, PTD7PAD, 11, 1)
	FIELD(SIM_SOPT2, TRACECLKSEL, 12, 1)
	FIELD(SIM_SOPT2, PLLFLLSEL, 16, 2)
	FIELD(SIM_SOPT2, USBSRC, 18, 1)
	FIELD(SIM_SOPT2, RMIISRC, 19, 1)
	FIELD(SIM_SOPT2, TIMESRC, 20, 2)
	FIELD(SIM_SOPT2, SDHCSRC, 28, 2)
REG32(SIM_SOPT4, 0x100c)
	FIELD(SIM_SOPT4, FTM0FLT0, 0, 1)
	FIELD(SIM_SOPT4, FTM0FLT1, 1, 1)
	FIELD(SIM_SOPT4, FTM0FLT2, 2, 1)
	FIELD(SIM_SOPT4, FTM1FLT0, 4, 1)
	FIELD(SIM_SOPT4, FTM2FLT0, 8, 1)
	FIELD(SIM_SOPT4, FTM3FLT0, 12, 1)
	FIELD(SIM_SOPT4, FTM1CH0SRC, 18, 2)
	FIELD(SIM_SOPT4, FTM2CH0SRC, 20, 2)
	FIELD(SIM_SOPT4, FTM0CLKSEL, 24, 1)
	FIELD(SIM_SOPT4, FTM1CLKSEL, 25, 1)
	FIELD(SIM_SOPT4, FTM2CLKSEL, 26, 1)
	FIELD(SIM_SOPT4, FTM3CLKSEL, 27, 1)
	FIELD(SIM_SOPT4, FTM0TRG0SRC, 28, 1)
	FIELD(SIM_SOPT4, FTM0TRG1SRC, 29, 1)
	FIELD(SIM_SOPT4, FTM3TRG0SRC, 30, 1)
	FIELD(SIM_SOPT4, FTM3TRG1SRC, 31, 1)
REG32(SIM_SOPT5, 0x1010)
	FIELD(SIM_SOPT5, UART0TXSRC, 0, 2)
	FIELD(SIM_SOPT5, UART0RXSRC, 2, 2)
	FIELD(SIM_SOPT5, UART1TXSRC, 4, 2)
	FIELD(SIM_SOPT5, UART1RXSRC, 6, 2)
REG32(SIM_SOPT7, 0x1018)
	FIELD(SIM_SOPT7, ADC0TRGSEL, 0, 4)
	FIELD(SIM_SOPT7, ADC0PRETRGSEL, 4, 1)
	FIELD(SIM_SOPT7, ADC0ALTTRGEN, 7, 1)
	FIELD(SIM_SOPT7, ADC1TRGSEL, 8, 4)
	FIELD(SIM_SOPT7, ADC1PRETRGSEL, 12, 1)
	FIELD(SIM_SOPT7, ADC1ALTTRGEN, 15, 1)
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
	FIELD(SIM_SCGC1, UART4, 10, 1)
	FIELD(SIM_SCGC1, UART5, 11, 1)
REG32(SIM_SCGC2, 0x102c)
	FIELD(SIM_SCGC2, ENET, 0, 1)
	FIELD(SIM_SCGC2, DAC0, 12, 1)
	FIELD(SIM_SCGC2, DAC1, 13, 1)
REG32(SIM_SCGC3, 0x1030)
	FIELD(SIM_SCGC3, RNGA, 0, 1)
	FIELD(SIM_SCGC3, SPI2, 12, 1)
	FIELD(SIM_SCGC3, SDHC, 17, 1)
	FIELD(SIM_SCGC3, FTM2, 24, 1)
	FIELD(SIM_SCGC3, FTM3, 25, 1)
	FIELD(SIM_SCGC3, ADC1, 27, 1)
REG32(SIM_SCGC4, 0x1034)
	FIELD(SIM_SCGC4, EWM, 1, 1)
	FIELD(SIM_SCGC4, CMT, 2, 1)
	FIELD(SIM_SCGC4, I2C0, 6, 1)
	FIELD(SIM_SCGC4, I2C1, 7, 1)
	FIELD(SIM_SCGC4, UART0, 10, 1)
	FIELD(SIM_SCGC4, UART1, 11, 1)
	FIELD(SIM_SCGC4, UART2, 12, 1)
	FIELD(SIM_SCGC4, UART3, 13, 1)
	FIELD(SIM_SCGC4, USBOTG, 18, 1)
	FIELD(SIM_SCGC4, CMP, 19, 1)
	FIELD(SIM_SCGC4, VREF, 20, 1)
REG32(SIM_SCGC5, 0x1038)
	FIELD(SIM_SCGC5, LPTMR, 0, 1)
	FIELD(SIM_SCGC5, PORTA, 9, 1)
	FIELD(SIM_SCGC5, PORTB, 10, 1)
	FIELD(SIM_SCGC5, PORTC, 11, 1)
	FIELD(SIM_SCGC5, PORTD, 12, 1)
	FIELD(SIM_SCGC5, PORTE, 13, 1)
REG32(SIM_SCGC6, 0x103c)
	FIELD(SIM_SCGC6, FTF, 0, 1)
	FIELD(SIM_SCGC6, DMAMUX, 1, 1)
	FIELD(SIM_SCGC6, FLEXCAN0, 4, 1)
	FIELD(SIM_SCGC6, RNGA, 9, 1)
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
	FIELD(SIM_FCFG1, DEPART, 8, 4)
	FIELD(SIM_FCFG1, EESIZE, 16, 4)
	FIELD(SIM_FCFG1, PFSIZE, 24, 4)
	FIELD(SIM_FCFG1, NVMSIZE, 28, 4)
REG32(SIM_FCFG2, 0x1050)
	FIELD(SIM_FCFG2, MAXADDR1, 16, 7)
	FIELD(SIM_FCFG2, PFLSH, 23, 1)
	FIELD(SIM_FCFG2, MAXADDR0, 24, 7)
REG32(SIM_UIDH, 0x1054)
	FIELD(SIM_UIDH, UID, 0, 32)
REG32(SIM_UIDMH, 0x1058)
	FIELD(SIM_UIDMH, UID, 0, 32)
REG32(SIM_UIDML, 0x105c)
	FIELD(SIM_UIDML, UID, 0, 32)
REG32(SIM_UIDL, 0x1060)
	FIELD(SIM_UIDL, UID, 0, 32)


static void frdmk64f_sim_register_reset(FRDMK64FSIM *t) {
	t->SOPT1 = 0x80000000;
	t->SOPT1CFG = 0x0;
	t->SOPT2 = 0x1000;
	t->SOPT4 = 0x0;
	t->SOPT5 = 0x0;
	t->SOPT7 = 0x0;
	t->SDID = 0x380;
	t->SCGC1 = 0x0;
	t->SCGC2 = 0x0;
	t->SCGC3 = 0x0;
	t->SCGC4 = 0xf0100030;
	t->SCGC5 = 0x40182;
	t->SCGC6 = 0x40000001;
	t->SCGC7 = 0x6;
	t->CLKDIV1 = 0x110000;
	t->CLKDIV2 = 0x0;
	t->FCFG1 = 0xff0f0f00;
	t->FCFG2 = 0x7f7f0000;
	t->UIDH = 0x0;
	t->UIDMH = 0x0;
	t->UIDML = 0x0;
	t->UIDL = 0x0;

}

static uint64_t frdmk64f_sim_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMK64FSIM *t = FRDMK64F_SIM(opaque);
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

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK64F SIM read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmk64f_sim_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMK64FSIM *t = FRDMK64F_SIM(opaque);


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

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK64F SIM write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmk64f_sim_ops = {
	.read = frdmk64f_sim_read,
	.write = frdmk64f_sim_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmk64f_sim_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMK64FSIM *t = FRDMK64F_SIM(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmk64f_sim_ops, t, "frdmk64f-sim", FRDMK64F_SIM_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void frdmk64f_sim_realize(DeviceState *dev, Error **errp) {
	return;

}

static void frdmk64f_sim_reset_enter(Object *obj, ResetType type) {
	FRDMK64FSIM *t = FRDMK64F_SIM(obj);
	frdmk64f_sim_register_reset(t);
}

static const VMStateDescription frdmk64f_sim_vmstate = {
	.name = "frdmk64f-sim",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(SOPT1, FRDMK64FSIM),
		VMSTATE_UINT32(SOPT1CFG, FRDMK64FSIM),
		VMSTATE_UINT32(SOPT2, FRDMK64FSIM),
		VMSTATE_UINT32(SOPT4, FRDMK64FSIM),
		VMSTATE_UINT32(SOPT5, FRDMK64FSIM),
		VMSTATE_UINT32(SOPT7, FRDMK64FSIM),
		VMSTATE_UINT32(SDID, FRDMK64FSIM),
		VMSTATE_UINT32(SCGC1, FRDMK64FSIM),
		VMSTATE_UINT32(SCGC2, FRDMK64FSIM),
		VMSTATE_UINT32(SCGC3, FRDMK64FSIM),
		VMSTATE_UINT32(SCGC4, FRDMK64FSIM),
		VMSTATE_UINT32(SCGC5, FRDMK64FSIM),
		VMSTATE_UINT32(SCGC6, FRDMK64FSIM),
		VMSTATE_UINT32(SCGC7, FRDMK64FSIM),
		VMSTATE_UINT32(CLKDIV1, FRDMK64FSIM),
		VMSTATE_UINT32(CLKDIV2, FRDMK64FSIM),
		VMSTATE_UINT32(FCFG1, FRDMK64FSIM),
		VMSTATE_UINT32(FCFG2, FRDMK64FSIM),
		VMSTATE_UINT32(UIDH, FRDMK64FSIM),
		VMSTATE_UINT32(UIDMH, FRDMK64FSIM),
		VMSTATE_UINT32(UIDML, FRDMK64FSIM),
		VMSTATE_UINT32(UIDL, FRDMK64FSIM),

		VMSTATE_END_OF_LIST()
	}
};

static void frdmk64f_sim_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmk64f_sim_vmstate;
	dc->realize = frdmk64f_sim_realize;
	rc->phases.enter = frdmk64f_sim_reset_enter;

}

static const TypeInfo frdmk64f_sim_info = {
	.name = TYPE_FRDMK64F_SIM,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMK64FSIM),
	.instance_init = frdmk64f_sim_init,
	.class_init = frdmk64f_sim_class_init,
};

static void frdmk64f_sim_register_types(void) {
	type_register_static(&frdmk64f_sim_info);
}

type_init(frdmk64f_sim_register_types);

#define TYPE_FRDMK64F_PMC "frdmk64f-pmc"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMK64FPMC, FRDMK64F_PMC)

struct FRDMK64FPMC {
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
#define FRDMK64F_PMC_SIZE				0x3

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


static void frdmk64f_pmc_register_reset(FRDMK64FPMC *t) {
	t->LVDSC1 = 0x10;
	t->LVDSC2 = 0x0;
	t->REGSC = 0x4;

}

static uint64_t frdmk64f_pmc_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMK64FPMC *t = FRDMK64F_PMC(opaque);
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
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK64F PMC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmk64f_pmc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMK64FPMC *t = FRDMK64F_PMC(opaque);


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
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK64F PMC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmk64f_pmc_ops = {
	.read = frdmk64f_pmc_read,
	.write = frdmk64f_pmc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmk64f_pmc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMK64FPMC *t = FRDMK64F_PMC(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmk64f_pmc_ops, t, "frdmk64f-pmc", FRDMK64F_PMC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void frdmk64f_pmc_realize(DeviceState *dev, Error **errp) {
	return;

}

static void frdmk64f_pmc_reset_enter(Object *obj, ResetType type) {
	FRDMK64FPMC *t = FRDMK64F_PMC(obj);
	frdmk64f_pmc_register_reset(t);
}

static const VMStateDescription frdmk64f_pmc_vmstate = {
	.name = "frdmk64f-pmc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT8(LVDSC1, FRDMK64FPMC),
		VMSTATE_UINT8(LVDSC2, FRDMK64FPMC),
		VMSTATE_UINT8(REGSC, FRDMK64FPMC),

		VMSTATE_END_OF_LIST()
	}
};

static void frdmk64f_pmc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmk64f_pmc_vmstate;
	dc->realize = frdmk64f_pmc_realize;
	rc->phases.enter = frdmk64f_pmc_reset_enter;

}

static const TypeInfo frdmk64f_pmc_info = {
	.name = TYPE_FRDMK64F_PMC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMK64FPMC),
	.instance_init = frdmk64f_pmc_init,
	.class_init = frdmk64f_pmc_class_init,
};

static void frdmk64f_pmc_register_types(void) {
	type_register_static(&frdmk64f_pmc_info);
}

type_init(frdmk64f_pmc_register_types);

#define TYPE_FRDMK64F_OSC "frdmk64f-osc"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMK64FOSC, FRDMK64F_OSC)

struct FRDMK64FOSC {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/*registers*/
	uint8_t CR;
	/* base */
	uint32_t base;

};
#define FRDMK64F_OSC_SIZE				0x1

REG8(OSC_CR, 0x0)
	FIELD(OSC_CR, SC16P, 0, 1)
	FIELD(OSC_CR, SC8P, 1, 1)
	FIELD(OSC_CR, SC4P, 2, 1)
	FIELD(OSC_CR, SC2P, 3, 1)
	FIELD(OSC_CR, EREFSTEN, 5, 1)
	FIELD(OSC_CR, ERCLKEN, 7, 1)


static void frdmk64f_osc_register_reset(FRDMK64FOSC *t) {
	t->CR = 0x0;

}

static uint64_t frdmk64f_osc_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMK64FOSC *t = FRDMK64F_OSC(opaque);
	uint64_t ret;

	switch (offset) {
		case A_OSC_CR:
			ret = t->CR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK64F OSC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmk64f_osc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMK64FOSC *t = FRDMK64F_OSC(opaque);


	switch (offset) {

		default: break;
	}

	switch (offset) {
		case A_OSC_CR:
			t->CR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK64F OSC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmk64f_osc_ops = {
	.read = frdmk64f_osc_read,
	.write = frdmk64f_osc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmk64f_osc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMK64FOSC *t = FRDMK64F_OSC(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmk64f_osc_ops, t, "frdmk64f-osc", FRDMK64F_OSC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void frdmk64f_osc_realize(DeviceState *dev, Error **errp) {
	return;

}

static void frdmk64f_osc_reset_enter(Object *obj, ResetType type) {
	FRDMK64FOSC *t = FRDMK64F_OSC(obj);
	frdmk64f_osc_register_reset(t);
}

static const VMStateDescription frdmk64f_osc_vmstate = {
	.name = "frdmk64f-osc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT8(CR, FRDMK64FOSC),

		VMSTATE_END_OF_LIST()
	}
};

static void frdmk64f_osc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmk64f_osc_vmstate;
	dc->realize = frdmk64f_osc_realize;
	rc->phases.enter = frdmk64f_osc_reset_enter;

}

static const TypeInfo frdmk64f_osc_info = {
	.name = TYPE_FRDMK64F_OSC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMK64FOSC),
	.instance_init = frdmk64f_osc_init,
	.class_init = frdmk64f_osc_class_init,
};

static void frdmk64f_osc_register_types(void) {
	type_register_static(&frdmk64f_osc_info);
}

type_init(frdmk64f_osc_register_types);

#define TYPE_FRDMK64F_MCG "frdmk64f-mcg"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMK64FMCG, FRDMK64F_MCG)

struct FRDMK64FMCG {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

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
#define FRDMK64F_MCG_SIZE				0xe

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
	FIELD(MCG_C7, OSCSEL, 0, 2)
REG8(MCG_C8, 0xd)
	FIELD(MCG_C8, LOCS1, 0, 1)
	FIELD(MCG_C8, CME1, 5, 1)
	FIELD(MCG_C8, LOLRE, 6, 1)
	FIELD(MCG_C8, LOCRE1, 7, 1)


static void frdmk64f_mcg_register_reset(FRDMK64FMCG *t) {
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

static uint64_t frdmk64f_mcg_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMK64FMCG *t = FRDMK64F_MCG(opaque);
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
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK64F MCG read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmk64f_mcg_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMK64FMCG *t = FRDMK64F_MCG(opaque);


	switch (offset) {

		default: break;
	}

	switch (offset) {
		case A_MCG_C1:
			t->C1 = value;
			if (((!(value & 0x4)) && (!(value & 0x40)) && (value & 0x80))) {
				t->S &= (~(0x4));
				t->S &= (~(0x10));
				t->S |= 0x8;
			} else if (((value & 0x4) && (!(value & 0x40)) && (!(value & 0x80)))) {
				t->S &= (~(0x4));
				t->S &= (~(0x8));
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
			if ((value & 0x40)) {
				t->S |= 0x20;
				t->S |= 0x40;
			} else if ((!(value & 0x40))) {
				t->S &= (~(0x20));
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
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK64F MCG write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmk64f_mcg_ops = {
	.read = frdmk64f_mcg_read,
	.write = frdmk64f_mcg_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmk64f_mcg_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMK64FMCG *t = FRDMK64F_MCG(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmk64f_mcg_ops, t, "frdmk64f-mcg", FRDMK64F_MCG_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void frdmk64f_mcg_realize(DeviceState *dev, Error **errp) {
	return;

}

static void frdmk64f_mcg_reset_enter(Object *obj, ResetType type) {
	FRDMK64FMCG *t = FRDMK64F_MCG(obj);
	frdmk64f_mcg_register_reset(t);
}

static const VMStateDescription frdmk64f_mcg_vmstate = {
	.name = "frdmk64f-mcg",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT8(C1, FRDMK64FMCG),
		VMSTATE_UINT8(C2, FRDMK64FMCG),
		VMSTATE_UINT8(C3, FRDMK64FMCG),
		VMSTATE_UINT8(C4, FRDMK64FMCG),
		VMSTATE_UINT8(C5, FRDMK64FMCG),
		VMSTATE_UINT8(C6, FRDMK64FMCG),
		VMSTATE_UINT8(S, FRDMK64FMCG),
		VMSTATE_UINT8(SC, FRDMK64FMCG),
		VMSTATE_UINT8(ATCVH, FRDMK64FMCG),
		VMSTATE_UINT8(ATCVL, FRDMK64FMCG),
		VMSTATE_UINT8(C7, FRDMK64FMCG),
		VMSTATE_UINT8(C8, FRDMK64FMCG),

		VMSTATE_END_OF_LIST()
	}
};

static void frdmk64f_mcg_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmk64f_mcg_vmstate;
	dc->realize = frdmk64f_mcg_realize;
	rc->phases.enter = frdmk64f_mcg_reset_enter;

}

static const TypeInfo frdmk64f_mcg_info = {
	.name = TYPE_FRDMK64F_MCG,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMK64FMCG),
	.instance_init = frdmk64f_mcg_init,
	.class_init = frdmk64f_mcg_class_init,
};

static void frdmk64f_mcg_register_types(void) {
	type_register_static(&frdmk64f_mcg_info);
}

type_init(frdmk64f_mcg_register_types);

#define TYPE_FRDMK64F_RTC "frdmk64f-rtc"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMK64FRTC, FRDMK64F_RTC)

struct FRDMK64FRTC {
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
#define FRDMK64F_RTC_SIZE				0x808

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


static void frdmk64f_rtc_register_reset(FRDMK64FRTC *t) {
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

static uint64_t frdmk64f_rtc_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMK64FRTC *t = FRDMK64F_RTC(opaque);
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
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK64F RTC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmk64f_rtc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMK64FRTC *t = FRDMK64F_RTC(opaque);


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
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK64F RTC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmk64f_rtc_ops = {
	.read = frdmk64f_rtc_read,
	.write = frdmk64f_rtc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmk64f_rtc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMK64FRTC *t = FRDMK64F_RTC(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmk64f_rtc_ops, t, "frdmk64f-rtc", FRDMK64F_RTC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 2; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void frdmk64f_rtc_realize(DeviceState *dev, Error **errp) {
	return;

}

static void frdmk64f_rtc_reset_enter(Object *obj, ResetType type) {
	FRDMK64FRTC *t = FRDMK64F_RTC(obj);
	frdmk64f_rtc_register_reset(t);
}

static const VMStateDescription frdmk64f_rtc_vmstate = {
	.name = "frdmk64f-rtc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(TSR, FRDMK64FRTC),
		VMSTATE_UINT32(TPR, FRDMK64FRTC),
		VMSTATE_UINT32(TAR, FRDMK64FRTC),
		VMSTATE_UINT32(TCR, FRDMK64FRTC),
		VMSTATE_UINT32(CR, FRDMK64FRTC),
		VMSTATE_UINT32(SR, FRDMK64FRTC),
		VMSTATE_UINT32(LR, FRDMK64FRTC),
		VMSTATE_UINT32(IER, FRDMK64FRTC),
		VMSTATE_UINT32(WAR, FRDMK64FRTC),
		VMSTATE_UINT32(RAR, FRDMK64FRTC),

		VMSTATE_END_OF_LIST()
	}
};

static void frdmk64f_rtc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmk64f_rtc_vmstate;
	dc->realize = frdmk64f_rtc_realize;
	rc->phases.enter = frdmk64f_rtc_reset_enter;

}

static const TypeInfo frdmk64f_rtc_info = {
	.name = TYPE_FRDMK64F_RTC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMK64FRTC),
	.instance_init = frdmk64f_rtc_init,
	.class_init = frdmk64f_rtc_class_init,
};

static void frdmk64f_rtc_register_types(void) {
	type_register_static(&frdmk64f_rtc_info);
}

type_init(frdmk64f_rtc_register_types);

#define TYPE_FRDMK64F_GPIO "frdmk64f-gpio"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMK64FGPIO, FRDMK64F_GPIO)

struct FRDMK64FGPIO {
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
#define FRDMK64F_GPIO_SIZE				0x18

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


static void frdmk64f_gpio_register_reset(FRDMK64FGPIO *t) {
	t->PDOR = 0x0;
	t->PSOR = 0x0;
	t->PCOR = 0x0;
	t->PTOR = 0x0;
	t->PDIR = 0x0;
	t->PDDR = 0x0;

}

static uint64_t frdmk64f_gpio_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMK64FGPIO *t = FRDMK64F_GPIO(opaque);
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
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK64F GPIO read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmk64f_gpio_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMK64FGPIO *t = FRDMK64F_GPIO(opaque);


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
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK64F GPIO write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmk64f_gpio_ops = {
	.read = frdmk64f_gpio_read,
	.write = frdmk64f_gpio_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmk64f_gpio_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMK64FGPIO *t = FRDMK64F_GPIO(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmk64f_gpio_ops, t, "frdmk64f-gpio", FRDMK64F_GPIO_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void frdmk64f_gpio_realize(DeviceState *dev, Error **errp) {
	return;

}

static void frdmk64f_gpio_reset_enter(Object *obj, ResetType type) {
	FRDMK64FGPIO *t = FRDMK64F_GPIO(obj);
	frdmk64f_gpio_register_reset(t);
}

static const VMStateDescription frdmk64f_gpio_vmstate = {
	.name = "frdmk64f-gpio",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(PDOR, FRDMK64FGPIO),
		VMSTATE_UINT32(PSOR, FRDMK64FGPIO),
		VMSTATE_UINT32(PCOR, FRDMK64FGPIO),
		VMSTATE_UINT32(PTOR, FRDMK64FGPIO),
		VMSTATE_UINT32(PDIR, FRDMK64FGPIO),
		VMSTATE_UINT32(PDDR, FRDMK64FGPIO),

		VMSTATE_END_OF_LIST()
	}
};

static void frdmk64f_gpio_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmk64f_gpio_vmstate;
	dc->realize = frdmk64f_gpio_realize;
	rc->phases.enter = frdmk64f_gpio_reset_enter;

}

static const TypeInfo frdmk64f_gpio_info = {
	.name = TYPE_FRDMK64F_GPIO,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMK64FGPIO),
	.instance_init = frdmk64f_gpio_init,
	.class_init = frdmk64f_gpio_class_init,
};

static void frdmk64f_gpio_register_types(void) {
	type_register_static(&frdmk64f_gpio_info);
}

type_init(frdmk64f_gpio_register_types);

#define TYPE_FRDMK64F_PORT "frdmk64f-port"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMK64FPORT, FRDMK64F_PORT)

struct FRDMK64FPORT {
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
#define FRDMK64F_PORT_SIZE				0xa4

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


__attribute__((unused))
static void frdmk64f_port_update(FRDMK64FPORT *t) {
	int conditions = ((t->ISFR & 0x800) || (t->ISFR & 0x2) || (t->ISFR & 0x8) || (t->ISFR & 0x4) || (t->ISFR & 0x1000) || (t->ISFR & 0x4000) || (t->ISFR & 0x10) || (t->ISFR & 0x2000) || (t->ISFR & 0x1) || (t->ISFR & 0x20) || (t->ISFR & 0x40) || (t->ISFR & 0x80) || (t->ISFR & 0x100) || (t->ISFR & 0x200) || (t->ISFR & 0x400) || (t->ISFR & 0x8000) || (t->ISFR & 0x10000) || (t->ISFR & 0x20000) || (t->ISFR & 0x40000) || (t->ISFR & 0x80000) || (t->ISFR & 0x100000) || (t->ISFR & 0x200000) || (t->ISFR & 0x400000) || (t->ISFR & 0x800000) || (t->ISFR & 0x1000000) || (t->ISFR & 0x2000000) || (t->ISFR & 0x4000000) || (t->ISFR & 0x8000000) || (t->ISFR & 0x10000000) || (t->ISFR & 0x20000000) || (t->ISFR & 0x40000000) || (t->ISFR & 0x80000000));
	qemu_set_irq(t->irq[0], conditions);
	qemu_set_irq(t->irq[0], 0);

}

static int frdmk64f_port_can_receive(void *opaque) {
	return 1;
}

static void frdmk64f_port_receive(void *opaque, const uint8_t *buf, int size) {
	FRDMK64FPORT *t = FRDMK64F_PORT(opaque);

	t->ISFR = (1 << (*buf % 32));

	frdmk64f_port_update(t);
}

static void frdmk64f_port_register_reset(FRDMK64FPORT *t) {
	t->PCR0 = 0x742;
	t->PCR1 = 0x743;
	t->PCR2 = 0x743;
	t->PCR3 = 0x743;
	t->PCR4 = 0x743;
	t->PCR5 = 0x41;
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

static uint64_t frdmk64f_port_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMK64FPORT *t = FRDMK64F_PORT(opaque);
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
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK64F PORT read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmk64f_port_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMK64FPORT *t = FRDMK64F_PORT(opaque);


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
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK64F PORT write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmk64f_port_ops = {
	.read = frdmk64f_port_read,
	.write = frdmk64f_port_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmk64f_port_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMK64FPORT *t = FRDMK64F_PORT(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmk64f_port_ops, t, "frdmk64f-port", FRDMK64F_PORT_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void frdmk64f_port_realize(DeviceState *dev, Error **errp) {
	FRDMK64FPORT *t = FRDMK64F_PORT(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), frdmk64f_port_can_receive, frdmk64f_port_receive,
		NULL, NULL, t, NULL, true);

}

static void frdmk64f_port_reset_enter(Object *obj, ResetType type) {
	FRDMK64FPORT *t = FRDMK64F_PORT(obj);
	frdmk64f_port_register_reset(t);
}

static const VMStateDescription frdmk64f_port_vmstate = {
	.name = "frdmk64f-port",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(PCR0, FRDMK64FPORT),
		VMSTATE_UINT32(PCR1, FRDMK64FPORT),
		VMSTATE_UINT32(PCR2, FRDMK64FPORT),
		VMSTATE_UINT32(PCR3, FRDMK64FPORT),
		VMSTATE_UINT32(PCR4, FRDMK64FPORT),
		VMSTATE_UINT32(PCR5, FRDMK64FPORT),
		VMSTATE_UINT32(PCR6, FRDMK64FPORT),
		VMSTATE_UINT32(PCR7, FRDMK64FPORT),
		VMSTATE_UINT32(PCR8, FRDMK64FPORT),
		VMSTATE_UINT32(PCR9, FRDMK64FPORT),
		VMSTATE_UINT32(PCR10, FRDMK64FPORT),
		VMSTATE_UINT32(PCR11, FRDMK64FPORT),
		VMSTATE_UINT32(PCR12, FRDMK64FPORT),
		VMSTATE_UINT32(PCR13, FRDMK64FPORT),
		VMSTATE_UINT32(PCR14, FRDMK64FPORT),
		VMSTATE_UINT32(PCR15, FRDMK64FPORT),
		VMSTATE_UINT32(PCR16, FRDMK64FPORT),
		VMSTATE_UINT32(PCR17, FRDMK64FPORT),
		VMSTATE_UINT32(PCR18, FRDMK64FPORT),
		VMSTATE_UINT32(PCR19, FRDMK64FPORT),
		VMSTATE_UINT32(PCR20, FRDMK64FPORT),
		VMSTATE_UINT32(PCR21, FRDMK64FPORT),
		VMSTATE_UINT32(PCR22, FRDMK64FPORT),
		VMSTATE_UINT32(PCR23, FRDMK64FPORT),
		VMSTATE_UINT32(PCR24, FRDMK64FPORT),
		VMSTATE_UINT32(PCR25, FRDMK64FPORT),
		VMSTATE_UINT32(PCR26, FRDMK64FPORT),
		VMSTATE_UINT32(PCR27, FRDMK64FPORT),
		VMSTATE_UINT32(PCR28, FRDMK64FPORT),
		VMSTATE_UINT32(PCR29, FRDMK64FPORT),
		VMSTATE_UINT32(PCR30, FRDMK64FPORT),
		VMSTATE_UINT32(PCR31, FRDMK64FPORT),
		VMSTATE_UINT32(GPCLR, FRDMK64FPORT),
		VMSTATE_UINT32(GPCHR, FRDMK64FPORT),
		VMSTATE_UINT32(ISFR, FRDMK64FPORT),

		VMSTATE_END_OF_LIST()
	}
};

static Property frdmk64f_port_properties[] = {
	DEFINE_PROP_CHR("chardev", FRDMK64FPORT, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void frdmk64f_port_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmk64f_port_vmstate;
	dc->realize = frdmk64f_port_realize;
	rc->phases.enter = frdmk64f_port_reset_enter;
	device_class_set_props(dc, frdmk64f_port_properties);

}

static const TypeInfo frdmk64f_port_info = {
	.name = TYPE_FRDMK64F_PORT,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMK64FPORT),
	.instance_init = frdmk64f_port_init,
	.class_init = frdmk64f_port_class_init,
};

static void frdmk64f_port_register_types(void) {
	type_register_static(&frdmk64f_port_info);
}

type_init(frdmk64f_port_register_types);

#define TYPE_FRDMK64F_LPTMR0 "frdmk64f-lptmr0"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMK64FLPTMR0, FRDMK64F_LPTMR0)

struct FRDMK64FLPTMR0 {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t CSR;
	uint32_t PSR;
	uint32_t CMR;
	uint32_t CNR;
	/* base */
	uint32_t base;

};
#define FRDMK64F_LPTMR0_SIZE				0x10

REG32(LPTMR0_CSR, 0x0)
	FIELD(LPTMR0_CSR, TEN, 0, 1)
	FIELD(LPTMR0_CSR, TMS, 1, 1)
	FIELD(LPTMR0_CSR, TFC, 2, 1)
	FIELD(LPTMR0_CSR, TPP, 3, 1)
	FIELD(LPTMR0_CSR, TPS, 4, 2)
	FIELD(LPTMR0_CSR, TIE, 6, 1)
	FIELD(LPTMR0_CSR, TCF, 7, 1)
REG32(LPTMR0_PSR, 0x4)
	FIELD(LPTMR0_PSR, PCS, 0, 2)
	FIELD(LPTMR0_PSR, PBYP, 2, 1)
	FIELD(LPTMR0_PSR, PRESCALE, 3, 4)
REG32(LPTMR0_CMR, 0x8)
	FIELD(LPTMR0_CMR, COMPARE, 0, 16)
REG32(LPTMR0_CNR, 0xc)
	FIELD(LPTMR0_CNR, COUNTER, 0, 16)


static void frdmk64f_lptmr0_register_reset(FRDMK64FLPTMR0 *t) {
	t->CSR = 0x0;
	t->PSR = 0x0;
	t->CMR = 0x0;
	t->CNR = 0x0;

}

static uint64_t frdmk64f_lptmr0_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMK64FLPTMR0 *t = FRDMK64F_LPTMR0(opaque);
	uint64_t ret;

	switch (offset) {
		case A_LPTMR0_CSR:
			ret = t->CSR;
			break;
		case A_LPTMR0_PSR:
			ret = t->PSR;
			break;
		case A_LPTMR0_CMR:
			ret = t->CMR;
			break;
		case A_LPTMR0_CNR:
			ret = t->CNR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK64F LPTMR0 read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmk64f_lptmr0_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMK64FLPTMR0 *t = FRDMK64F_LPTMR0(opaque);


	switch (offset) {
		case A_LPTMR0_CSR ... A_LPTMR0_CSR + 3:
			value = value << ((offset - A_LPTMR0_CSR) << 3);
			offset = A_LPTMR0_CSR;
			break;
		case A_LPTMR0_PSR ... A_LPTMR0_PSR + 3:
			value = value << ((offset - A_LPTMR0_PSR) << 3);
			offset = A_LPTMR0_PSR;
			break;
		case A_LPTMR0_CMR ... A_LPTMR0_CMR + 3:
			value = value << ((offset - A_LPTMR0_CMR) << 3);
			offset = A_LPTMR0_CMR;
			break;
		case A_LPTMR0_CNR ... A_LPTMR0_CNR + 3:
			value = value << ((offset - A_LPTMR0_CNR) << 3);
			offset = A_LPTMR0_CNR;
			break;

		default: break;
	}

	switch (offset) {
		case A_LPTMR0_CSR:
			t->CSR = value;
			break;
		case A_LPTMR0_PSR:
			t->PSR = value;
			break;
		case A_LPTMR0_CMR:
			t->CMR = value;
			break;
		case A_LPTMR0_CNR:
			t->CNR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK64F LPTMR0 write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmk64f_lptmr0_ops = {
	.read = frdmk64f_lptmr0_read,
	.write = frdmk64f_lptmr0_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmk64f_lptmr0_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMK64FLPTMR0 *t = FRDMK64F_LPTMR0(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmk64f_lptmr0_ops, t, "frdmk64f-lptmr0", FRDMK64F_LPTMR0_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void frdmk64f_lptmr0_realize(DeviceState *dev, Error **errp) {
	return;

}

static void frdmk64f_lptmr0_reset_enter(Object *obj, ResetType type) {
	FRDMK64FLPTMR0 *t = FRDMK64F_LPTMR0(obj);
	frdmk64f_lptmr0_register_reset(t);
}

static const VMStateDescription frdmk64f_lptmr0_vmstate = {
	.name = "frdmk64f-lptmr0",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CSR, FRDMK64FLPTMR0),
		VMSTATE_UINT32(PSR, FRDMK64FLPTMR0),
		VMSTATE_UINT32(CMR, FRDMK64FLPTMR0),
		VMSTATE_UINT32(CNR, FRDMK64FLPTMR0),

		VMSTATE_END_OF_LIST()
	}
};

static void frdmk64f_lptmr0_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmk64f_lptmr0_vmstate;
	dc->realize = frdmk64f_lptmr0_realize;
	rc->phases.enter = frdmk64f_lptmr0_reset_enter;

}

static const TypeInfo frdmk64f_lptmr0_info = {
	.name = TYPE_FRDMK64F_LPTMR0,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMK64FLPTMR0),
	.instance_init = frdmk64f_lptmr0_init,
	.class_init = frdmk64f_lptmr0_class_init,
};

static void frdmk64f_lptmr0_register_types(void) {
	type_register_static(&frdmk64f_lptmr0_info);
}

type_init(frdmk64f_lptmr0_register_types);

#define TYPE_FRDMK64F_SMC "frdmk64f-smc"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMK64FSMC, FRDMK64F_SMC)

struct FRDMK64FSMC {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/*registers*/
	uint8_t PMPROT;
	uint8_t PMCTRL;
	uint8_t VLLSCTRL;
	uint8_t PMSTAT;
	/* base */
	uint32_t base;

};
#define FRDMK64F_SMC_SIZE				0x4

REG8(SMC_PMPROT, 0x0)
	FIELD(SMC_PMPROT, AVLLS, 1, 1)
	FIELD(SMC_PMPROT, ALLS, 3, 1)
	FIELD(SMC_PMPROT, AVLP, 5, 1)
REG8(SMC_PMCTRL, 0x1)
	FIELD(SMC_PMCTRL, STOPM, 0, 3)
	FIELD(SMC_PMCTRL, STOPA, 3, 1)
	FIELD(SMC_PMCTRL, RUNM, 5, 2)
	FIELD(SMC_PMCTRL, LPWUI, 7, 1)
REG8(SMC_VLLSCTRL, 0x2)
	FIELD(SMC_VLLSCTRL, VLLSM, 0, 3)
	FIELD(SMC_VLLSCTRL, PORPO, 5, 1)
REG8(SMC_PMSTAT, 0x3)
	FIELD(SMC_PMSTAT, PMSTAT, 0, 7)


static void frdmk64f_smc_register_reset(FRDMK64FSMC *t) {
	t->PMPROT = 0x0;
	t->PMCTRL = 0x0;
	t->VLLSCTRL = 0x3;
	t->PMSTAT = 0x1;

}

static uint64_t frdmk64f_smc_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMK64FSMC *t = FRDMK64F_SMC(opaque);
	uint64_t ret;

	switch (offset) {
		case A_SMC_PMPROT:
			ret = t->PMPROT;
			break;
		case A_SMC_PMCTRL:
			ret = t->PMCTRL;
			break;
		case A_SMC_VLLSCTRL:
			ret = t->VLLSCTRL;
			break;
		case A_SMC_PMSTAT:
			ret = t->PMSTAT;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK64F SMC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmk64f_smc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMK64FSMC *t = FRDMK64F_SMC(opaque);


	switch (offset) {

		default: break;
	}

	switch (offset) {
		case A_SMC_PMPROT:
			t->PMPROT = value;
			break;
		case A_SMC_PMCTRL:
			t->PMCTRL = value;
			break;
		case A_SMC_VLLSCTRL:
			t->VLLSCTRL = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK64F SMC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmk64f_smc_ops = {
	.read = frdmk64f_smc_read,
	.write = frdmk64f_smc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmk64f_smc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMK64FSMC *t = FRDMK64F_SMC(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmk64f_smc_ops, t, "frdmk64f-smc", FRDMK64F_SMC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void frdmk64f_smc_realize(DeviceState *dev, Error **errp) {
	return;

}

static void frdmk64f_smc_reset_enter(Object *obj, ResetType type) {
	FRDMK64FSMC *t = FRDMK64F_SMC(obj);
	frdmk64f_smc_register_reset(t);
}

static const VMStateDescription frdmk64f_smc_vmstate = {
	.name = "frdmk64f-smc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT8(PMPROT, FRDMK64FSMC),
		VMSTATE_UINT8(PMCTRL, FRDMK64FSMC),
		VMSTATE_UINT8(VLLSCTRL, FRDMK64FSMC),
		VMSTATE_UINT8(PMSTAT, FRDMK64FSMC),

		VMSTATE_END_OF_LIST()
	}
};

static void frdmk64f_smc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmk64f_smc_vmstate;
	dc->realize = frdmk64f_smc_realize;
	rc->phases.enter = frdmk64f_smc_reset_enter;

}

static const TypeInfo frdmk64f_smc_info = {
	.name = TYPE_FRDMK64F_SMC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMK64FSMC),
	.instance_init = frdmk64f_smc_init,
	.class_init = frdmk64f_smc_class_init,
};

static void frdmk64f_smc_register_types(void) {
	type_register_static(&frdmk64f_smc_info);
}

type_init(frdmk64f_smc_register_types);

#define TYPE_FRDMK64F_PIT "frdmk64f-pit"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMK64FPIT, FRDMK64F_PIT)

struct FRDMK64FPIT {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[4];

	/*registers*/
	uint32_t MCR;
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
	/* timer */
	QEMUTimer *timers[4];
	uint8_t enabled[4];

	uint32_t cur_period[4];

	uint8_t period_set[4];

	uint8_t period_reg_set[4];

	/* base */
	uint32_t base;

};
#define FRDMK64F_PIT_SIZE				0x140

REG32(PIT_MCR, 0x0)
	FIELD(PIT_MCR, FRZ, 0, 1)
	FIELD(PIT_MCR, MDIS, 1, 1)
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


__attribute__((unused))
static void frdmk64f_pit_update(FRDMK64FPIT *t) {
	int conditions = ((t->TFLG2 & 0x20) || (t->TFLG2 & 0x4) || (t->TFLG2 & 0x2) || (t->TFLG2 & 0x40) || (t->TFLG2 & 0x10) || (t->TFLG2 & 0x200) || (t->TFLG2 & 0x1000) || (t->TFLG2 & 0x8) || (t->TFLG2 & 0x1) || (t->TFLG2 & 0x100) || (t->TFLG2 & 0x400) || (t->TFLG2 & 0x80) || (t->TFLG2 & 0x2000) || (t->TFLG2 & 0x800) || (t->TFLG2 & 0x4000) || (t->TFLG2 & 0x8000) || (t->TFLG2 & 0x10000) || (t->TFLG2 & 0x20000) || (t->TFLG2 & 0x40000) || (t->TFLG2 & 0x80000) || (t->TFLG2 & 0x100000) || (t->TFLG2 & 0x200000) || (t->TFLG2 & 0x400000) || (t->TFLG2 & 0x800000) || (t->TFLG2 & 0x1000000) || (t->TFLG2 & 0x2000000) || (t->TFLG2 & 0x4000000) || (t->TFLG2 & 0x8000000) || (t->TFLG2 & 0x10000000) || (t->TFLG2 & 0x20000000) || (t->TFLG2 & 0x40000000) || (t->TFLG2 & 0x80000000) || (t->TFLG0 & 0x1) || (t->TFLG0 & 0x2) || (t->TFLG0 & 0x4) || (t->TFLG0 & 0x8) || (t->TFLG0 & 0x10) || (t->TFLG0 & 0x20) || (t->TFLG0 & 0x40) || (t->TFLG0 & 0x80) || (t->TFLG0 & 0x100) || (t->TFLG0 & 0x200) || (t->TFLG0 & 0x400) || (t->TFLG0 & 0x800) || (t->TFLG0 & 0x1000) || (t->TFLG0 & 0x2000) || (t->TFLG0 & 0x4000) || (t->TFLG0 & 0x8000) || (t->TFLG0 & 0x10000) || (t->TFLG0 & 0x20000) || (t->TFLG0 & 0x40000) || (t->TFLG0 & 0x80000) || (t->TFLG0 & 0x100000) || (t->TFLG0 & 0x200000) || (t->TFLG0 & 0x400000) || (t->TFLG0 & 0x800000) || (t->TFLG0 & 0x1000000) || (t->TFLG0 & 0x2000000) || (t->TFLG0 & 0x4000000) || (t->TFLG0 & 0x8000000) || (t->TFLG0 & 0x10000000) || (t->TFLG0 & 0x20000000) || (t->TFLG0 & 0x40000000) || (t->TFLG0 & 0x80000000) || (t->TFLG1 & 0x1) || (t->TFLG1 & 0x2) || (t->TFLG1 & 0x4) || (t->TFLG1 & 0x8) || (t->TFLG1 & 0x10) || (t->TFLG1 & 0x20) || (t->TFLG1 & 0x40) || (t->TFLG1 & 0x80) || (t->TFLG1 & 0x100) || (t->TFLG1 & 0x200) || (t->TFLG1 & 0x400) || (t->TFLG1 & 0x800) || (t->TFLG1 & 0x1000) || (t->TFLG1 & 0x2000) || (t->TFLG1 & 0x4000) || (t->TFLG1 & 0x8000) || (t->TFLG1 & 0x10000) || (t->TFLG1 & 0x20000) || (t->TFLG1 & 0x40000) || (t->TFLG1 & 0x80000) || (t->TFLG1 & 0x100000) || (t->TFLG1 & 0x200000) || (t->TFLG1 & 0x400000) || (t->TFLG1 & 0x800000) || (t->TFLG1 & 0x1000000) || (t->TFLG1 & 0x2000000) || (t->TFLG1 & 0x4000000) || (t->TFLG1 & 0x8000000) || (t->TFLG1 & 0x10000000) || (t->TFLG1 & 0x20000000) || (t->TFLG1 & 0x40000000) || (t->TFLG1 & 0x80000000) || (t->TFLG3 & 0x1) || (t->TFLG3 & 0x2) || (t->TFLG3 & 0x4) || (t->TFLG3 & 0x8) || (t->TFLG3 & 0x10) || (t->TFLG3 & 0x20) || (t->TFLG3 & 0x40) || (t->TFLG3 & 0x80) || (t->TFLG3 & 0x100) || (t->TFLG3 & 0x200) || (t->TFLG3 & 0x400) || (t->TFLG3 & 0x800) || (t->TFLG3 & 0x1000) || (t->TFLG3 & 0x2000) || (t->TFLG3 & 0x4000) || (t->TFLG3 & 0x8000) || (t->TFLG3 & 0x10000) || (t->TFLG3 & 0x20000) || (t->TFLG3 & 0x40000) || (t->TFLG3 & 0x80000) || (t->TFLG3 & 0x100000) || (t->TFLG3 & 0x200000) || (t->TFLG3 & 0x400000) || (t->TFLG3 & 0x800000) || (t->TFLG3 & 0x1000000) || (t->TFLG3 & 0x2000000) || (t->TFLG3 & 0x4000000) || (t->TFLG3 & 0x8000000) || (t->TFLG3 & 0x10000000) || (t->TFLG3 & 0x20000000) || (t->TFLG3 & 0x40000000) || (t->TFLG3 & 0x80000000));
	qemu_set_irq(t->irq[0], conditions);

}

static void frdmk64f_pit_timer_callback_0(void *opaque) {
  static uint8_t period_reg_set_last = 0;
  FRDMK64FPIT *t = (FRDMK64FPIT*)opaque;

  if (!period_reg_set_last && t->period_reg_set[0]) {
      period_reg_set_last = 1;
      t->CVAL0 = 0;
  }
  t->CVAL0 += 1;
  unsigned prd = t->period_set[0] ? t->cur_period[0] : t->LDVAL0;
  if (t->CVAL0 == prd) {
    t->cur_period[0] = t->LDVAL0;
    t->CVAL0 = 0;
    t->TFLG0 |= 0x40000000;t->TFLG1 |= 0x40000;t->TFLG0 |= 0x40;t->TFLG1 |= 0x100;t->TFLG0 |= 0x1000000;t->TFLG2 |= 0x80000;t->TFLG2 |= 0x80;t->TFLG2 |= 0x40;t->TFLG3 |= 0x4000000;t->TFLG0 |= 0x200000;t->TFLG2 |= 0x200;t->TFLG3 |= 0x80000000;t->TFLG2 |= 0x100000;t->TFLG3 |= 0x200;t->TFLG3 |= 0x8;t->TFLG0 |= 0x200;t->TFLG1 |= 0x40;t->TFLG3 |= 0x10;t->TFLG0 |= 0x10000;t->TFLG0 |= 0x80000;t->TFLG0 |= 0x4000;t->TFLG0 |= 0x1;t->TFLG0 |= 0x2000000;t->TFLG3 |= 0x2;t->TFLG0 |= 0x8;t->TFLG1 |= 0x10;t->TFLG1 |= 0x8000000;t->TFLG1 |= 0x20000000;t->TFLG1 |= 0x1000000;t->TFLG2 |= 0x400;t->TFLG2 |= 0x1000;t->TFLG1 |= 0x4;t->TFLG2 |= 0x20;t->TFLG0 |= 0x20;t->TFLG3 |= 0x2000000;t->TFLG2 |= 0x10000000;t->TFLG1 |= 0x800000;t->TFLG2 |= 0x1000000;t->TFLG3 |= 0x200000;t->TFLG3 |= 0x40;t->TFLG2 |= 0x200000;t->TFLG1 |= 0x100000;t->TFLG3 |= 0x8000;t->TFLG0 |= 0x20000;t->TFLG3 |= 0x10000000;t->TFLG0 |= 0x40000;t->TFLG1 |= 0x1000;t->TFLG2 |= 0x10;t->TFLG2 |= 0x100;t->TFLG0 |= 0x2000;t->TFLG1 |= 0x2;t->TFLG2 |= 0x4000;t->TFLG1 |= 0x800;t->TFLG2 |= 0x800;t->TFLG0 |= 0x2;t->TFLG1 |= 0x80000;t->TFLG3 |= 0x4000;t->TFLG1 |= 0x4000000;t->TFLG1 |= 0x40000000;t->TFLG1 |= 0x200000;t->TFLG3 |= 0x20000;t->TFLG1 |= 0x8000;t->TFLG2 |= 0x800000;t->TFLG3 |= 0x100000;t->TFLG1 |= 0x80;t->TFLG2 |= 0x40000;t->TFLG0 |= 0x10;t->TFLG1 |= 0x4000;t->TFLG2 |= 0x1;t->TFLG3 |= 0x40000;t->TFLG3 |= 0x1;t->TFLG3 |= 0x1000;t->TFLG3 |= 0x80000;t->TFLG0 |= 0x400000;t->TFLG3 |= 0x800000;t->TFLG2 |= 0x4000000;t->TFLG3 |= 0x1000000;t->TFLG1 |= 0x200;t->TFLG3 |= 0x400000;t->TFLG0 |= 0x80;t->TFLG2 |= 0x2000;t->TFLG3 |= 0x100;t->TFLG2 |= 0x2;t->TFLG2 |= 0x8000000;t->TFLG1 |= 0x10000000;t->TFLG0 |= 0x800000;t->TFLG2 |= 0x80000000;t->TFLG0 |= 0x100000;t->TFLG3 |= 0x8000000;t->TFLG2 |= 0x40000000;t->TFLG2 |= 0x8;t->TFLG1 |= 0x1;t->TFLG0 |= 0x800;t->TFLG3 |= 0x40000000;t->TFLG1 |= 0x2000000;t->TFLG0 |= 0x10000000;t->TFLG0 |= 0x4000000;t->TFLG1 |= 0x20;t->TFLG3 |= 0x10000;t->TFLG2 |= 0x8000;t->TFLG1 |= 0x400000;t->TFLG0 |= 0x20000000;t->TFLG0 |= 0x100;t->TFLG2 |= 0x400000;t->TFLG2 |= 0x20000;t->TFLG1 |= 0x400;t->TFLG3 |= 0x20;t->TFLG2 |= 0x2000000;t->TFLG0 |= 0x4;t->TFLG1 |= 0x8;t->TFLG3 |= 0x4;t->TFLG0 |= 0x80000000;t->TFLG0 |= 0x400;t->TFLG3 |= 0x400;t->TFLG3 |= 0x80;t->TFLG2 |= 0x10000;t->TFLG3 |= 0x2000;t->TFLG1 |= 0x20000;t->TFLG2 |= 0x4;t->TFLG3 |= 0x20000000;t->TFLG1 |= 0x80000000;t->TFLG3 |= 0x800;t->TFLG1 |= 0x10000;t->TFLG0 |= 0x8000;t->TFLG0 |= 0x8000000;t->TFLG1 |= 0x2000;t->TFLG2 |= 0x20000000;t->TFLG0 |= 0x1000;;
    qemu_set_irq(t->irq[0], 1);
    qemu_set_irq(t->irq[0], 0);
  }

  if (t->enabled[0]) {
    timer_mod(t->timers[0], qemu_clock_get_us(QEMU_CLOCK_VIRTUAL) + 1);
  }
}


static void frdmk64f_pit_timer_callback_1(void *opaque) {
  static uint8_t period_reg_set_last = 0;
  FRDMK64FPIT *t = (FRDMK64FPIT*)opaque;

  if (!period_reg_set_last && t->period_reg_set[1]) {
      period_reg_set_last = 1;
      t->CVAL1 = 0;
  }
  t->CVAL1 += 1;
  unsigned prd = t->period_set[1] ? t->cur_period[1] : t->LDVAL1;
  if (t->CVAL1 == prd) {
    t->cur_period[1] = t->LDVAL1;
    t->CVAL1 = 0;
    t->TFLG0 |= 0x40000000;t->TFLG1 |= 0x40000;t->TFLG0 |= 0x40;t->TFLG1 |= 0x100;t->TFLG0 |= 0x1000000;t->TFLG2 |= 0x80000;t->TFLG2 |= 0x80;t->TFLG2 |= 0x40;t->TFLG3 |= 0x4000000;t->TFLG0 |= 0x200000;t->TFLG2 |= 0x200;t->TFLG3 |= 0x80000000;t->TFLG2 |= 0x100000;t->TFLG3 |= 0x200;t->TFLG3 |= 0x8;t->TFLG0 |= 0x200;t->TFLG1 |= 0x40;t->TFLG3 |= 0x10;t->TFLG0 |= 0x10000;t->TFLG0 |= 0x80000;t->TFLG0 |= 0x4000;t->TFLG0 |= 0x1;t->TFLG0 |= 0x2000000;t->TFLG3 |= 0x2;t->TFLG0 |= 0x8;t->TFLG1 |= 0x10;t->TFLG1 |= 0x8000000;t->TFLG1 |= 0x20000000;t->TFLG1 |= 0x1000000;t->TFLG2 |= 0x400;t->TFLG2 |= 0x1000;t->TFLG1 |= 0x4;t->TFLG2 |= 0x20;t->TFLG0 |= 0x20;t->TFLG3 |= 0x2000000;t->TFLG2 |= 0x10000000;t->TFLG1 |= 0x800000;t->TFLG2 |= 0x1000000;t->TFLG3 |= 0x200000;t->TFLG3 |= 0x40;t->TFLG2 |= 0x200000;t->TFLG1 |= 0x100000;t->TFLG3 |= 0x8000;t->TFLG0 |= 0x20000;t->TFLG3 |= 0x10000000;t->TFLG0 |= 0x40000;t->TFLG1 |= 0x1000;t->TFLG2 |= 0x10;t->TFLG2 |= 0x100;t->TFLG0 |= 0x2000;t->TFLG1 |= 0x2;t->TFLG2 |= 0x4000;t->TFLG1 |= 0x800;t->TFLG2 |= 0x800;t->TFLG0 |= 0x2;t->TFLG1 |= 0x80000;t->TFLG3 |= 0x4000;t->TFLG1 |= 0x4000000;t->TFLG1 |= 0x40000000;t->TFLG1 |= 0x200000;t->TFLG3 |= 0x20000;t->TFLG1 |= 0x8000;t->TFLG2 |= 0x800000;t->TFLG3 |= 0x100000;t->TFLG1 |= 0x80;t->TFLG2 |= 0x40000;t->TFLG0 |= 0x10;t->TFLG1 |= 0x4000;t->TFLG2 |= 0x1;t->TFLG3 |= 0x40000;t->TFLG3 |= 0x1;t->TFLG3 |= 0x1000;t->TFLG3 |= 0x80000;t->TFLG0 |= 0x400000;t->TFLG3 |= 0x800000;t->TFLG2 |= 0x4000000;t->TFLG3 |= 0x1000000;t->TFLG1 |= 0x200;t->TFLG3 |= 0x400000;t->TFLG0 |= 0x80;t->TFLG2 |= 0x2000;t->TFLG3 |= 0x100;t->TFLG2 |= 0x2;t->TFLG2 |= 0x8000000;t->TFLG1 |= 0x10000000;t->TFLG0 |= 0x800000;t->TFLG2 |= 0x80000000;t->TFLG0 |= 0x100000;t->TFLG3 |= 0x8000000;t->TFLG2 |= 0x40000000;t->TFLG2 |= 0x8;t->TFLG1 |= 0x1;t->TFLG0 |= 0x800;t->TFLG3 |= 0x40000000;t->TFLG1 |= 0x2000000;t->TFLG0 |= 0x10000000;t->TFLG0 |= 0x4000000;t->TFLG1 |= 0x20;t->TFLG3 |= 0x10000;t->TFLG2 |= 0x8000;t->TFLG1 |= 0x400000;t->TFLG0 |= 0x20000000;t->TFLG0 |= 0x100;t->TFLG2 |= 0x400000;t->TFLG2 |= 0x20000;t->TFLG1 |= 0x400;t->TFLG3 |= 0x20;t->TFLG2 |= 0x2000000;t->TFLG0 |= 0x4;t->TFLG1 |= 0x8;t->TFLG3 |= 0x4;t->TFLG0 |= 0x80000000;t->TFLG0 |= 0x400;t->TFLG3 |= 0x400;t->TFLG3 |= 0x80;t->TFLG2 |= 0x10000;t->TFLG3 |= 0x2000;t->TFLG1 |= 0x20000;t->TFLG2 |= 0x4;t->TFLG3 |= 0x20000000;t->TFLG1 |= 0x80000000;t->TFLG3 |= 0x800;t->TFLG1 |= 0x10000;t->TFLG0 |= 0x8000;t->TFLG0 |= 0x8000000;t->TFLG1 |= 0x2000;t->TFLG2 |= 0x20000000;t->TFLG0 |= 0x1000;;
    qemu_set_irq(t->irq[1], 1);
    qemu_set_irq(t->irq[1], 0);
  }

  if (t->enabled[1]) {
    timer_mod(t->timers[1], qemu_clock_get_us(QEMU_CLOCK_VIRTUAL) + 1);
  }
}


static void frdmk64f_pit_timer_callback_2(void *opaque) {
  static uint8_t period_reg_set_last = 0;
  FRDMK64FPIT *t = (FRDMK64FPIT*)opaque;

  if (!period_reg_set_last && t->period_reg_set[2]) {
      period_reg_set_last = 1;
      t->CVAL2 = 0;
  }
  t->CVAL2 += 1;
  unsigned prd = t->period_set[2] ? t->cur_period[2] : t->LDVAL2;
  if (t->CVAL2 == prd) {
    t->cur_period[2] = t->LDVAL2;
    t->CVAL2 = 0;
    t->TFLG0 |= 0x40000000;t->TFLG1 |= 0x40000;t->TFLG0 |= 0x40;t->TFLG1 |= 0x100;t->TFLG0 |= 0x1000000;t->TFLG2 |= 0x80000;t->TFLG2 |= 0x80;t->TFLG2 |= 0x40;t->TFLG3 |= 0x4000000;t->TFLG0 |= 0x200000;t->TFLG2 |= 0x200;t->TFLG3 |= 0x80000000;t->TFLG2 |= 0x100000;t->TFLG3 |= 0x200;t->TFLG3 |= 0x8;t->TFLG0 |= 0x200;t->TFLG1 |= 0x40;t->TFLG3 |= 0x10;t->TFLG0 |= 0x10000;t->TFLG0 |= 0x80000;t->TFLG0 |= 0x4000;t->TFLG0 |= 0x1;t->TFLG0 |= 0x2000000;t->TFLG3 |= 0x2;t->TFLG0 |= 0x8;t->TFLG1 |= 0x10;t->TFLG1 |= 0x8000000;t->TFLG1 |= 0x20000000;t->TFLG1 |= 0x1000000;t->TFLG2 |= 0x400;t->TFLG2 |= 0x1000;t->TFLG1 |= 0x4;t->TFLG2 |= 0x20;t->TFLG0 |= 0x20;t->TFLG3 |= 0x2000000;t->TFLG2 |= 0x10000000;t->TFLG1 |= 0x800000;t->TFLG2 |= 0x1000000;t->TFLG3 |= 0x200000;t->TFLG3 |= 0x40;t->TFLG2 |= 0x200000;t->TFLG1 |= 0x100000;t->TFLG3 |= 0x8000;t->TFLG0 |= 0x20000;t->TFLG3 |= 0x10000000;t->TFLG0 |= 0x40000;t->TFLG1 |= 0x1000;t->TFLG2 |= 0x10;t->TFLG2 |= 0x100;t->TFLG0 |= 0x2000;t->TFLG1 |= 0x2;t->TFLG2 |= 0x4000;t->TFLG1 |= 0x800;t->TFLG2 |= 0x800;t->TFLG0 |= 0x2;t->TFLG1 |= 0x80000;t->TFLG3 |= 0x4000;t->TFLG1 |= 0x4000000;t->TFLG1 |= 0x40000000;t->TFLG1 |= 0x200000;t->TFLG3 |= 0x20000;t->TFLG1 |= 0x8000;t->TFLG2 |= 0x800000;t->TFLG3 |= 0x100000;t->TFLG1 |= 0x80;t->TFLG2 |= 0x40000;t->TFLG0 |= 0x10;t->TFLG1 |= 0x4000;t->TFLG2 |= 0x1;t->TFLG3 |= 0x40000;t->TFLG3 |= 0x1;t->TFLG3 |= 0x1000;t->TFLG3 |= 0x80000;t->TFLG0 |= 0x400000;t->TFLG3 |= 0x800000;t->TFLG2 |= 0x4000000;t->TFLG3 |= 0x1000000;t->TFLG1 |= 0x200;t->TFLG3 |= 0x400000;t->TFLG0 |= 0x80;t->TFLG2 |= 0x2000;t->TFLG3 |= 0x100;t->TFLG2 |= 0x2;t->TFLG2 |= 0x8000000;t->TFLG1 |= 0x10000000;t->TFLG0 |= 0x800000;t->TFLG2 |= 0x80000000;t->TFLG0 |= 0x100000;t->TFLG3 |= 0x8000000;t->TFLG2 |= 0x40000000;t->TFLG2 |= 0x8;t->TFLG1 |= 0x1;t->TFLG0 |= 0x800;t->TFLG3 |= 0x40000000;t->TFLG1 |= 0x2000000;t->TFLG0 |= 0x10000000;t->TFLG0 |= 0x4000000;t->TFLG1 |= 0x20;t->TFLG3 |= 0x10000;t->TFLG2 |= 0x8000;t->TFLG1 |= 0x400000;t->TFLG0 |= 0x20000000;t->TFLG0 |= 0x100;t->TFLG2 |= 0x400000;t->TFLG2 |= 0x20000;t->TFLG1 |= 0x400;t->TFLG3 |= 0x20;t->TFLG2 |= 0x2000000;t->TFLG0 |= 0x4;t->TFLG1 |= 0x8;t->TFLG3 |= 0x4;t->TFLG0 |= 0x80000000;t->TFLG0 |= 0x400;t->TFLG3 |= 0x400;t->TFLG3 |= 0x80;t->TFLG2 |= 0x10000;t->TFLG3 |= 0x2000;t->TFLG1 |= 0x20000;t->TFLG2 |= 0x4;t->TFLG3 |= 0x20000000;t->TFLG1 |= 0x80000000;t->TFLG3 |= 0x800;t->TFLG1 |= 0x10000;t->TFLG0 |= 0x8000;t->TFLG0 |= 0x8000000;t->TFLG1 |= 0x2000;t->TFLG2 |= 0x20000000;t->TFLG0 |= 0x1000;;
    qemu_set_irq(t->irq[2], 1);
    qemu_set_irq(t->irq[2], 0);
  }

  if (t->enabled[2]) {
    timer_mod(t->timers[2], qemu_clock_get_us(QEMU_CLOCK_VIRTUAL) + 1);
  }
}


static void frdmk64f_pit_timer_callback_3(void *opaque) {
  static uint8_t period_reg_set_last = 0;
  FRDMK64FPIT *t = (FRDMK64FPIT*)opaque;

  if (!period_reg_set_last && t->period_reg_set[3]) {
      period_reg_set_last = 1;
      t->CVAL3 = 0;
  }
  t->CVAL3 += 1;
  unsigned prd = t->period_set[3] ? t->cur_period[3] : t->LDVAL3;
  if (t->CVAL3 == prd) {
    t->cur_period[3] = t->LDVAL3;
    t->CVAL3 = 0;
    t->TFLG0 |= 0x40000000;t->TFLG1 |= 0x40000;t->TFLG0 |= 0x40;t->TFLG1 |= 0x100;t->TFLG0 |= 0x1000000;t->TFLG2 |= 0x80000;t->TFLG2 |= 0x80;t->TFLG2 |= 0x40;t->TFLG3 |= 0x4000000;t->TFLG0 |= 0x200000;t->TFLG2 |= 0x200;t->TFLG3 |= 0x80000000;t->TFLG2 |= 0x100000;t->TFLG3 |= 0x200;t->TFLG3 |= 0x8;t->TFLG0 |= 0x200;t->TFLG1 |= 0x40;t->TFLG3 |= 0x10;t->TFLG0 |= 0x10000;t->TFLG0 |= 0x80000;t->TFLG0 |= 0x4000;t->TFLG0 |= 0x1;t->TFLG0 |= 0x2000000;t->TFLG3 |= 0x2;t->TFLG0 |= 0x8;t->TFLG1 |= 0x10;t->TFLG1 |= 0x8000000;t->TFLG1 |= 0x20000000;t->TFLG1 |= 0x1000000;t->TFLG2 |= 0x400;t->TFLG2 |= 0x1000;t->TFLG1 |= 0x4;t->TFLG2 |= 0x20;t->TFLG0 |= 0x20;t->TFLG3 |= 0x2000000;t->TFLG2 |= 0x10000000;t->TFLG1 |= 0x800000;t->TFLG2 |= 0x1000000;t->TFLG3 |= 0x200000;t->TFLG3 |= 0x40;t->TFLG2 |= 0x200000;t->TFLG1 |= 0x100000;t->TFLG3 |= 0x8000;t->TFLG0 |= 0x20000;t->TFLG3 |= 0x10000000;t->TFLG0 |= 0x40000;t->TFLG1 |= 0x1000;t->TFLG2 |= 0x10;t->TFLG2 |= 0x100;t->TFLG0 |= 0x2000;t->TFLG1 |= 0x2;t->TFLG2 |= 0x4000;t->TFLG1 |= 0x800;t->TFLG2 |= 0x800;t->TFLG0 |= 0x2;t->TFLG1 |= 0x80000;t->TFLG3 |= 0x4000;t->TFLG1 |= 0x4000000;t->TFLG1 |= 0x40000000;t->TFLG1 |= 0x200000;t->TFLG3 |= 0x20000;t->TFLG1 |= 0x8000;t->TFLG2 |= 0x800000;t->TFLG3 |= 0x100000;t->TFLG1 |= 0x80;t->TFLG2 |= 0x40000;t->TFLG0 |= 0x10;t->TFLG1 |= 0x4000;t->TFLG2 |= 0x1;t->TFLG3 |= 0x40000;t->TFLG3 |= 0x1;t->TFLG3 |= 0x1000;t->TFLG3 |= 0x80000;t->TFLG0 |= 0x400000;t->TFLG3 |= 0x800000;t->TFLG2 |= 0x4000000;t->TFLG3 |= 0x1000000;t->TFLG1 |= 0x200;t->TFLG3 |= 0x400000;t->TFLG0 |= 0x80;t->TFLG2 |= 0x2000;t->TFLG3 |= 0x100;t->TFLG2 |= 0x2;t->TFLG2 |= 0x8000000;t->TFLG1 |= 0x10000000;t->TFLG0 |= 0x800000;t->TFLG2 |= 0x80000000;t->TFLG0 |= 0x100000;t->TFLG3 |= 0x8000000;t->TFLG2 |= 0x40000000;t->TFLG2 |= 0x8;t->TFLG1 |= 0x1;t->TFLG0 |= 0x800;t->TFLG3 |= 0x40000000;t->TFLG1 |= 0x2000000;t->TFLG0 |= 0x10000000;t->TFLG0 |= 0x4000000;t->TFLG1 |= 0x20;t->TFLG3 |= 0x10000;t->TFLG2 |= 0x8000;t->TFLG1 |= 0x400000;t->TFLG0 |= 0x20000000;t->TFLG0 |= 0x100;t->TFLG2 |= 0x400000;t->TFLG2 |= 0x20000;t->TFLG1 |= 0x400;t->TFLG3 |= 0x20;t->TFLG2 |= 0x2000000;t->TFLG0 |= 0x4;t->TFLG1 |= 0x8;t->TFLG3 |= 0x4;t->TFLG0 |= 0x80000000;t->TFLG0 |= 0x400;t->TFLG3 |= 0x400;t->TFLG3 |= 0x80;t->TFLG2 |= 0x10000;t->TFLG3 |= 0x2000;t->TFLG1 |= 0x20000;t->TFLG2 |= 0x4;t->TFLG3 |= 0x20000000;t->TFLG1 |= 0x80000000;t->TFLG3 |= 0x800;t->TFLG1 |= 0x10000;t->TFLG0 |= 0x8000;t->TFLG0 |= 0x8000000;t->TFLG1 |= 0x2000;t->TFLG2 |= 0x20000000;t->TFLG0 |= 0x1000;;
    qemu_set_irq(t->irq[3], 1);
    qemu_set_irq(t->irq[3], 0);
  }

  if (t->enabled[3]) {
    timer_mod(t->timers[3], qemu_clock_get_us(QEMU_CLOCK_VIRTUAL) + 1);
  }
}


static void frdmk64f_pit_register_reset(FRDMK64FPIT *t) {
	t->MCR = 0x6;
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
	t->enabled[0] = 0;
	t->period_set[0] = 0;
	t->period_reg_set[0] = 0;
	t->enabled[1] = 0;
	t->period_set[1] = 0;
	t->period_reg_set[1] = 0;
	t->enabled[2] = 0;
	t->period_set[2] = 0;
	t->period_reg_set[2] = 0;
	t->enabled[3] = 0;
	t->period_set[3] = 0;
	t->period_reg_set[3] = 0;

}

static uint64_t frdmk64f_pit_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMK64FPIT *t = FRDMK64F_PIT(opaque);
	uint64_t ret;

	switch (offset) {
		case A_PIT_MCR:
			ret = t->MCR;
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
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK64F PIT read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmk64f_pit_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMK64FPIT *t = FRDMK64F_PIT(opaque);


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
			if (!t->period_reg_set[0]) {
				t->period_reg_set[0] = 1;
			}
			break;
		case A_PIT_LDVAL1:
			t->LDVAL1 = value;
			if (!t->period_reg_set[1]) {
				t->period_reg_set[1] = 1;
			}
			break;
		case A_PIT_LDVAL2:
			t->LDVAL2 = value;
			if (!t->period_reg_set[2]) {
				t->period_reg_set[2] = 1;
			}
			break;
		case A_PIT_LDVAL3:
			t->LDVAL3 = value;
			if (!t->period_reg_set[3]) {
				t->period_reg_set[3] = 1;
			}
			break;
		case A_PIT_TCTRL0:
			t->TCTRL0 = value;
			if ((t->TCTRL0 & 0x1)) {
				t->period_set[0] = t->period_reg_set[0];
				t->enabled[0] = 1;
				t->cur_period[0] = t->LDVAL0;
				timer_mod(t->timers[0], qemu_clock_get_us(QEMU_CLOCK_VIRTUAL) + 1);
			}
			if ((!(t->TCTRL0 & 0x1))) {
				t->enabled[0] = 0;
				timer_free(t->timers[0]);
				t->timers[0] = timer_new(QEMU_CLOCK_VIRTUAL, SCALE_US, frdmk64f_pit_timer_callback_0, t);
			}
			break;
		case A_PIT_TCTRL1:
			t->TCTRL1 = value;
			if ((t->TCTRL1 & 0x1)) {
				t->period_set[1] = t->period_reg_set[1];
				t->enabled[1] = 1;
				t->cur_period[1] = t->LDVAL1;
				timer_mod(t->timers[1], qemu_clock_get_us(QEMU_CLOCK_VIRTUAL) + 1);
			}
			if ((!(t->TCTRL1 & 0x1))) {
				t->enabled[1] = 0;
				timer_free(t->timers[1]);
				t->timers[1] = timer_new(QEMU_CLOCK_VIRTUAL, SCALE_US, frdmk64f_pit_timer_callback_1, t);
			}
			break;
		case A_PIT_TCTRL2:
			t->TCTRL2 = value;
			if ((t->TCTRL2 & 0x1)) {
				t->period_set[2] = t->period_reg_set[2];
				t->enabled[2] = 1;
				t->cur_period[2] = t->LDVAL2;
				timer_mod(t->timers[2], qemu_clock_get_us(QEMU_CLOCK_VIRTUAL) + 1);
			}
			if ((!(t->TCTRL2 & 0x1))) {
				t->enabled[2] = 0;
				timer_free(t->timers[2]);
				t->timers[2] = timer_new(QEMU_CLOCK_VIRTUAL, SCALE_US, frdmk64f_pit_timer_callback_2, t);
			}
			break;
		case A_PIT_TCTRL3:
			t->TCTRL3 = value;
			if ((t->TCTRL3 & 0x1)) {
				t->period_set[3] = t->period_reg_set[3];
				t->enabled[3] = 1;
				t->cur_period[3] = t->LDVAL3;
				timer_mod(t->timers[3], qemu_clock_get_us(QEMU_CLOCK_VIRTUAL) + 1);
			}
			if ((!(t->TCTRL3 & 0x1))) {
				t->enabled[3] = 0;
				timer_free(t->timers[3]);
				t->timers[3] = timer_new(QEMU_CLOCK_VIRTUAL, SCALE_US, frdmk64f_pit_timer_callback_3, t);
			}
			break;
		case A_PIT_TFLG0:
			t->TFLG0 = value;
			frdmk64f_pit_update(t);
			break;
		case A_PIT_TFLG1:
			t->TFLG1 = value;
			frdmk64f_pit_update(t);
			break;
		case A_PIT_TFLG2:
			t->TFLG2 = value;
			frdmk64f_pit_update(t);
			break;
		case A_PIT_TFLG3:
			t->TFLG3 = value;
			frdmk64f_pit_update(t);
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK64F PIT write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmk64f_pit_ops = {
	.read = frdmk64f_pit_read,
	.write = frdmk64f_pit_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmk64f_pit_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMK64FPIT *t = FRDMK64F_PIT(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmk64f_pit_ops, t, "frdmk64f-pit", FRDMK64F_PIT_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 4; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void frdmk64f_pit_realize(DeviceState *dev, Error **errp) {
	FRDMK64FPIT *t = FRDMK64F_PIT(dev);
	t->timers[0] = timer_new(QEMU_CLOCK_VIRTUAL, SCALE_US, frdmk64f_pit_timer_callback_0, t);
	t->timers[1] = timer_new(QEMU_CLOCK_VIRTUAL, SCALE_US, frdmk64f_pit_timer_callback_1, t);
	t->timers[2] = timer_new(QEMU_CLOCK_VIRTUAL, SCALE_US, frdmk64f_pit_timer_callback_2, t);
	t->timers[3] = timer_new(QEMU_CLOCK_VIRTUAL, SCALE_US, frdmk64f_pit_timer_callback_3, t);

}

static void frdmk64f_pit_reset_enter(Object *obj, ResetType type) {
	FRDMK64FPIT *t = FRDMK64F_PIT(obj);
	frdmk64f_pit_register_reset(t);
}

static const VMStateDescription frdmk64f_pit_vmstate = {
	.name = "frdmk64f-pit",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(MCR, FRDMK64FPIT),
		VMSTATE_UINT32(LDVAL0, FRDMK64FPIT),
		VMSTATE_UINT32(LDVAL1, FRDMK64FPIT),
		VMSTATE_UINT32(LDVAL2, FRDMK64FPIT),
		VMSTATE_UINT32(LDVAL3, FRDMK64FPIT),
		VMSTATE_UINT32(CVAL0, FRDMK64FPIT),
		VMSTATE_UINT32(CVAL1, FRDMK64FPIT),
		VMSTATE_UINT32(CVAL2, FRDMK64FPIT),
		VMSTATE_UINT32(CVAL3, FRDMK64FPIT),
		VMSTATE_UINT32(TCTRL0, FRDMK64FPIT),
		VMSTATE_UINT32(TCTRL1, FRDMK64FPIT),
		VMSTATE_UINT32(TCTRL2, FRDMK64FPIT),
		VMSTATE_UINT32(TCTRL3, FRDMK64FPIT),
		VMSTATE_UINT32(TFLG0, FRDMK64FPIT),
		VMSTATE_UINT32(TFLG1, FRDMK64FPIT),
		VMSTATE_UINT32(TFLG2, FRDMK64FPIT),
		VMSTATE_UINT32(TFLG3, FRDMK64FPIT),

		VMSTATE_END_OF_LIST()
	}
};

static void frdmk64f_pit_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmk64f_pit_vmstate;
	dc->realize = frdmk64f_pit_realize;
	rc->phases.enter = frdmk64f_pit_reset_enter;

}

static const TypeInfo frdmk64f_pit_info = {
	.name = TYPE_FRDMK64F_PIT,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMK64FPIT),
	.instance_init = frdmk64f_pit_init,
	.class_init = frdmk64f_pit_class_init,
};

static void frdmk64f_pit_register_types(void) {
	type_register_static(&frdmk64f_pit_info);
}

type_init(frdmk64f_pit_register_types);

#define TYPE_FRDMK64F_ADC "frdmk64f-adc"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMK64FADC, FRDMK64F_ADC)

struct FRDMK64FADC {
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
	/* chardev backend */
	CharBackend chr;
	guint watch_tag;

	/* base */
	uint32_t base;

};
#define FRDMK64F_ADC_SIZE				0x70

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


static int frdmk64f_adc_can_receive(void *opaque) {
	return 1;
}

static void frdmk64f_adc_receive(void *opaque, const uint8_t *buf, int size) {
	FRDMK64FADC *t = FRDMK64F_ADC(opaque);

	t->RA = *buf;
	t->SC1A |= 0x80;


}

static void frdmk64f_adc_register_reset(FRDMK64FADC *t) {
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

static uint64_t frdmk64f_adc_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMK64FADC *t = FRDMK64F_ADC(opaque);
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
			t->SC1A &= (~(0x80));
			qemu_chr_fe_accept_input(&(t->chr));
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
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK64F ADC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmk64f_adc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMK64FADC *t = FRDMK64F_ADC(opaque);


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
			if ((value & 0x80)) {
				t->SC3 &= (~(0x80));
				t->SC1A |= 0x80;
			}
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
			t->SC1A &= value;
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
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK64F ADC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmk64f_adc_ops = {
	.read = frdmk64f_adc_read,
	.write = frdmk64f_adc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmk64f_adc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMK64FADC *t = FRDMK64F_ADC(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmk64f_adc_ops, t, "frdmk64f-adc", FRDMK64F_ADC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void frdmk64f_adc_realize(DeviceState *dev, Error **errp) {
	FRDMK64FADC *t = FRDMK64F_ADC(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), frdmk64f_adc_can_receive, frdmk64f_adc_receive,
		NULL, NULL, t, NULL, true);

}

static void frdmk64f_adc_reset_enter(Object *obj, ResetType type) {
	FRDMK64FADC *t = FRDMK64F_ADC(obj);
	frdmk64f_adc_register_reset(t);
}

static const VMStateDescription frdmk64f_adc_vmstate = {
	.name = "frdmk64f-adc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CFG1, FRDMK64FADC),
		VMSTATE_UINT32(CFG2, FRDMK64FADC),
		VMSTATE_UINT32(SC2, FRDMK64FADC),
		VMSTATE_UINT32(SC3, FRDMK64FADC),
		VMSTATE_UINT32(OFS, FRDMK64FADC),
		VMSTATE_UINT32(PG, FRDMK64FADC),
		VMSTATE_UINT32(MG, FRDMK64FADC),
		VMSTATE_UINT32(CLPD, FRDMK64FADC),
		VMSTATE_UINT32(CLPS, FRDMK64FADC),
		VMSTATE_UINT32(CLP4, FRDMK64FADC),
		VMSTATE_UINT32(CLP3, FRDMK64FADC),
		VMSTATE_UINT32(CLP2, FRDMK64FADC),
		VMSTATE_UINT32(CLP1, FRDMK64FADC),
		VMSTATE_UINT32(CLP0, FRDMK64FADC),
		VMSTATE_UINT32(CLMD, FRDMK64FADC),
		VMSTATE_UINT32(CLMS, FRDMK64FADC),
		VMSTATE_UINT32(CLM4, FRDMK64FADC),
		VMSTATE_UINT32(CLM3, FRDMK64FADC),
		VMSTATE_UINT32(CLM2, FRDMK64FADC),
		VMSTATE_UINT32(CLM1, FRDMK64FADC),
		VMSTATE_UINT32(CLM0, FRDMK64FADC),
		VMSTATE_UINT32(SC1A, FRDMK64FADC),
		VMSTATE_UINT32(SC1B, FRDMK64FADC),
		VMSTATE_UINT32(RA, FRDMK64FADC),
		VMSTATE_UINT32(RB, FRDMK64FADC),
		VMSTATE_UINT32(CV1, FRDMK64FADC),
		VMSTATE_UINT32(CV2, FRDMK64FADC),

		VMSTATE_END_OF_LIST()
	}
};

static Property frdmk64f_adc_properties[] = {
	DEFINE_PROP_CHR("chardev", FRDMK64FADC, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void frdmk64f_adc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmk64f_adc_vmstate;
	dc->realize = frdmk64f_adc_realize;
	rc->phases.enter = frdmk64f_adc_reset_enter;
	device_class_set_props(dc, frdmk64f_adc_properties);

}

static const TypeInfo frdmk64f_adc_info = {
	.name = TYPE_FRDMK64F_ADC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMK64FADC),
	.instance_init = frdmk64f_adc_init,
	.class_init = frdmk64f_adc_class_init,
};

static void frdmk64f_adc_register_types(void) {
	type_register_static(&frdmk64f_adc_info);
}

type_init(frdmk64f_adc_register_types);

#define TYPE_FRDMK64F_DAC "frdmk64f-dac"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMK64FDAC, FRDMK64F_DAC)

struct FRDMK64FDAC {
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
#define FRDMK64F_DAC_SIZE				0x24

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


static void frdmk64f_dac_register_reset(FRDMK64FDAC *t) {
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

static uint64_t frdmk64f_dac_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMK64FDAC *t = FRDMK64F_DAC(opaque);
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
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK64F DAC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmk64f_dac_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMK64FDAC *t = FRDMK64F_DAC(opaque);


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
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK64F DAC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmk64f_dac_ops = {
	.read = frdmk64f_dac_read,
	.write = frdmk64f_dac_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmk64f_dac_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMK64FDAC *t = FRDMK64F_DAC(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmk64f_dac_ops, t, "frdmk64f-dac", FRDMK64F_DAC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void frdmk64f_dac_realize(DeviceState *dev, Error **errp) {
	return;

}

static void frdmk64f_dac_reset_enter(Object *obj, ResetType type) {
	FRDMK64FDAC *t = FRDMK64F_DAC(obj);
	frdmk64f_dac_register_reset(t);
}

static const VMStateDescription frdmk64f_dac_vmstate = {
	.name = "frdmk64f-dac",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT8(SR, FRDMK64FDAC),
		VMSTATE_UINT8(C0, FRDMK64FDAC),
		VMSTATE_UINT8(C1, FRDMK64FDAC),
		VMSTATE_UINT8(C2, FRDMK64FDAC),
		VMSTATE_UINT8(DAT0L, FRDMK64FDAC),
		VMSTATE_UINT8(DAT1L, FRDMK64FDAC),
		VMSTATE_UINT8(DAT2L, FRDMK64FDAC),
		VMSTATE_UINT8(DAT3L, FRDMK64FDAC),
		VMSTATE_UINT8(DAT4L, FRDMK64FDAC),
		VMSTATE_UINT8(DAT5L, FRDMK64FDAC),
		VMSTATE_UINT8(DAT6L, FRDMK64FDAC),
		VMSTATE_UINT8(DAT7L, FRDMK64FDAC),
		VMSTATE_UINT8(DAT8L, FRDMK64FDAC),
		VMSTATE_UINT8(DAT9L, FRDMK64FDAC),
		VMSTATE_UINT8(DAT10L, FRDMK64FDAC),
		VMSTATE_UINT8(DAT11L, FRDMK64FDAC),
		VMSTATE_UINT8(DAT12L, FRDMK64FDAC),
		VMSTATE_UINT8(DAT13L, FRDMK64FDAC),
		VMSTATE_UINT8(DAT14L, FRDMK64FDAC),
		VMSTATE_UINT8(DAT15L, FRDMK64FDAC),
		VMSTATE_UINT8(DAT0H, FRDMK64FDAC),
		VMSTATE_UINT8(DAT1H, FRDMK64FDAC),
		VMSTATE_UINT8(DAT2H, FRDMK64FDAC),
		VMSTATE_UINT8(DAT3H, FRDMK64FDAC),
		VMSTATE_UINT8(DAT4H, FRDMK64FDAC),
		VMSTATE_UINT8(DAT5H, FRDMK64FDAC),
		VMSTATE_UINT8(DAT6H, FRDMK64FDAC),
		VMSTATE_UINT8(DAT7H, FRDMK64FDAC),
		VMSTATE_UINT8(DAT8H, FRDMK64FDAC),
		VMSTATE_UINT8(DAT9H, FRDMK64FDAC),
		VMSTATE_UINT8(DAT10H, FRDMK64FDAC),
		VMSTATE_UINT8(DAT11H, FRDMK64FDAC),
		VMSTATE_UINT8(DAT12H, FRDMK64FDAC),
		VMSTATE_UINT8(DAT13H, FRDMK64FDAC),
		VMSTATE_UINT8(DAT14H, FRDMK64FDAC),
		VMSTATE_UINT8(DAT15H, FRDMK64FDAC),

		VMSTATE_END_OF_LIST()
	}
};

static void frdmk64f_dac_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmk64f_dac_vmstate;
	dc->realize = frdmk64f_dac_realize;
	rc->phases.enter = frdmk64f_dac_reset_enter;

}

static const TypeInfo frdmk64f_dac_info = {
	.name = TYPE_FRDMK64F_DAC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMK64FDAC),
	.instance_init = frdmk64f_dac_init,
	.class_init = frdmk64f_dac_class_init,
};

static void frdmk64f_dac_register_types(void) {
	type_register_static(&frdmk64f_dac_info);
}

type_init(frdmk64f_dac_register_types);

#define TYPE_FRDMK64F_I2C "frdmk64f-i2c"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMK64FI2C, FRDMK64F_I2C)

struct FRDMK64FI2C {
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
#define FRDMK64F_I2C_SIZE				0xc

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


__attribute__((unused))
static void frdmk64f_i2c_update(FRDMK64FI2C *t) {
	int conditions = ((!(t->S & 0x10)) && (!(t->S & 0x1)) && (t->C1 & 0x20));
	// PERRY PATCH
	conditions = conditions && (t->C1 & 0x40);
	qemu_set_irq(t->irq[0], conditions);

}

static int frdmk64f_i2c_can_receive(void *opaque) {
	return 1;
}

static void frdmk64f_i2c_receive(void *opaque, const uint8_t *buf, int size) {
	FRDMK64FI2C *t = FRDMK64F_I2C(opaque);

	t->D = *buf;
	t->S &= (~(0x10));
	t->FLT |= 0x10;
	t->S |= 0x40;
	t->S |= 0x2;
	t->S &= (~(0x1));
	t->S |= 0x80;

	frdmk64f_i2c_update(t);
}

static gboolean frdmk64f_i2c_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	FRDMK64FI2C *t = FRDMK64F_I2C(opaque);
	int ret;

	t->watch_tag = 0;
	t->S |= 0x1;
	t->S &= (~(0x80));
	t->S |= 0x10;
	t->S &= (~(0x2));
	t->S |= 0x20;

	frdmk64f_i2c_update(t);

	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->D), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, frdmk64f_i2c_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->S &= (~(0x10));
	t->S &= (~(0x20));
	t->S |= 0x2;
	t->S &= (~(0x1));
	t->S |= 0x80;

	frdmk64f_i2c_update(t);

	return FALSE;
}

static void frdmk64f_i2c_register_reset(FRDMK64FI2C *t) {
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
	t->S &= (~(0x10));
	t->S &= (~(0x20));
	t->S |= 0x2;
	t->S &= (~(0x1));
	t->S |= 0x80;

}

static uint64_t frdmk64f_i2c_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMK64FI2C *t = FRDMK64F_I2C(opaque);
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
			t->S |= 0x1;
			t->S &= (~(0x80));
			t->S |= 0x10;
			t->S &= (~(0x2));
			t->FLT &= (~(0x10));
			qemu_chr_fe_accept_input(&(t->chr));
			frdmk64f_i2c_update(t);
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
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK64F I2C read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmk64f_i2c_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMK64FI2C *t = FRDMK64F_I2C(opaque);


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
			} else if ((value & 0x20)) {
				t->S |= 0x2;
			} else if ((value & 0x8)) {
				t->S |= 0x2;
			}
			frdmk64f_i2c_update(t);
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
			frdmk64f_i2c_update(t);
			break;
		case A_I2C_D:
			t->D = value;
			frdmk64f_i2c_transmit(NULL, G_IO_OUT, t);
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

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK64F I2C write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmk64f_i2c_ops = {
	.read = frdmk64f_i2c_read,
	.write = frdmk64f_i2c_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmk64f_i2c_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMK64FI2C *t = FRDMK64F_I2C(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmk64f_i2c_ops, t, "frdmk64f-i2c", FRDMK64F_I2C_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void frdmk64f_i2c_realize(DeviceState *dev, Error **errp) {
	FRDMK64FI2C *t = FRDMK64F_I2C(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), frdmk64f_i2c_can_receive, frdmk64f_i2c_receive,
		NULL, NULL, t, NULL, true);

}

static void frdmk64f_i2c_reset_enter(Object *obj, ResetType type) {
	FRDMK64FI2C *t = FRDMK64F_I2C(obj);
	frdmk64f_i2c_register_reset(t);
}

static const VMStateDescription frdmk64f_i2c_vmstate = {
	.name = "frdmk64f-i2c",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT8(A1, FRDMK64FI2C),
		VMSTATE_UINT8(F, FRDMK64FI2C),
		VMSTATE_UINT8(C1, FRDMK64FI2C),
		VMSTATE_UINT8(S, FRDMK64FI2C),
		VMSTATE_UINT8(D, FRDMK64FI2C),
		VMSTATE_UINT8(C2, FRDMK64FI2C),
		VMSTATE_UINT8(FLT, FRDMK64FI2C),
		VMSTATE_UINT8(RA, FRDMK64FI2C),
		VMSTATE_UINT8(SMB, FRDMK64FI2C),
		VMSTATE_UINT8(A2, FRDMK64FI2C),
		VMSTATE_UINT8(SLTH, FRDMK64FI2C),
		VMSTATE_UINT8(SLTL, FRDMK64FI2C),

		VMSTATE_END_OF_LIST()
	}
};

static Property frdmk64f_i2c_properties[] = {
	DEFINE_PROP_CHR("chardev", FRDMK64FI2C, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void frdmk64f_i2c_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmk64f_i2c_vmstate;
	dc->realize = frdmk64f_i2c_realize;
	rc->phases.enter = frdmk64f_i2c_reset_enter;
	device_class_set_props(dc, frdmk64f_i2c_properties);

}

static const TypeInfo frdmk64f_i2c_info = {
	.name = TYPE_FRDMK64F_I2C,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMK64FI2C),
	.instance_init = frdmk64f_i2c_init,
	.class_init = frdmk64f_i2c_class_init,
};

static void frdmk64f_i2c_register_types(void) {
	type_register_static(&frdmk64f_i2c_info);
}

type_init(frdmk64f_i2c_register_types);

#define TYPE_FRDMK64F_FTM "frdmk64f-ftm"
OBJECT_DECLARE_SIMPLE_TYPE(FRDMK64FFTM, FRDMK64F_FTM)

struct FRDMK64FFTM {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t SC;
	uint32_t CNT;
	uint32_t MOD;
	uint32_t CNTIN;
	uint32_t STATUS;
	uint32_t MODE;
	uint32_t SYNC;
	uint32_t OUTINIT;
	uint32_t OUTMASK;
	uint32_t COMBINE;
	uint32_t DEADTIME;
	uint32_t EXTTRIG;
	uint32_t POL;
	uint32_t FMS;
	uint32_t FILTER;
	uint32_t FLTCTRL;
	uint32_t QDCTRL;
	uint32_t CONF;
	uint32_t FLTPOL;
	uint32_t SYNCONF;
	uint32_t INVCTRL;
	uint32_t SWOCTRL;
	uint32_t PWMLOAD;
	uint32_t C0SC;
	uint32_t C1SC;
	uint32_t C2SC;
	uint32_t C3SC;
	uint32_t C4SC;
	uint32_t C5SC;
	uint32_t C6SC;
	uint32_t C7SC;
	uint32_t C0V;
	uint32_t C1V;
	uint32_t C2V;
	uint32_t C3V;
	uint32_t C4V;
	uint32_t C5V;
	uint32_t C6V;
	uint32_t C7V;
	/* base */
	uint32_t base;

};
#define FRDMK64F_FTM_SIZE				0x9c

REG32(FTM_SC, 0x0)
	FIELD(FTM_SC, PS, 0, 3)
	FIELD(FTM_SC, CLKS, 3, 2)
	FIELD(FTM_SC, CPWMS, 5, 1)
	FIELD(FTM_SC, TOIE, 6, 1)
	FIELD(FTM_SC, TOF, 7, 1)
REG32(FTM_CNT, 0x4)
	FIELD(FTM_CNT, COUNT, 0, 16)
REG32(FTM_MOD, 0x8)
	FIELD(FTM_MOD, MOD, 0, 16)
REG32(FTM_CNTIN, 0x4c)
	FIELD(FTM_CNTIN, INIT, 0, 16)
REG32(FTM_STATUS, 0x50)
	FIELD(FTM_STATUS, CH0F, 0, 1)
	FIELD(FTM_STATUS, CH1F, 1, 1)
	FIELD(FTM_STATUS, CH2F, 2, 1)
	FIELD(FTM_STATUS, CH3F, 3, 1)
	FIELD(FTM_STATUS, CH4F, 4, 1)
	FIELD(FTM_STATUS, CH5F, 5, 1)
	FIELD(FTM_STATUS, CH6F, 6, 1)
	FIELD(FTM_STATUS, CH7F, 7, 1)
REG32(FTM_MODE, 0x54)
	FIELD(FTM_MODE, FTMEN, 0, 1)
	FIELD(FTM_MODE, INIT, 1, 1)
	FIELD(FTM_MODE, WPDIS, 2, 1)
	FIELD(FTM_MODE, PWMSYNC, 3, 1)
	FIELD(FTM_MODE, CAPTEST, 4, 1)
	FIELD(FTM_MODE, FAULTM, 5, 2)
	FIELD(FTM_MODE, FAULTIE, 7, 1)
REG32(FTM_SYNC, 0x58)
	FIELD(FTM_SYNC, CNTMIN, 0, 1)
	FIELD(FTM_SYNC, CNTMAX, 1, 1)
	FIELD(FTM_SYNC, REINIT, 2, 1)
	FIELD(FTM_SYNC, SYNCHOM, 3, 1)
	FIELD(FTM_SYNC, TRIG0, 4, 1)
	FIELD(FTM_SYNC, TRIG1, 5, 1)
	FIELD(FTM_SYNC, TRIG2, 6, 1)
	FIELD(FTM_SYNC, SWSYNC, 7, 1)
REG32(FTM_OUTINIT, 0x5c)
	FIELD(FTM_OUTINIT, CH0OI, 0, 1)
	FIELD(FTM_OUTINIT, CH1OI, 1, 1)
	FIELD(FTM_OUTINIT, CH2OI, 2, 1)
	FIELD(FTM_OUTINIT, CH3OI, 3, 1)
	FIELD(FTM_OUTINIT, CH4OI, 4, 1)
	FIELD(FTM_OUTINIT, CH5OI, 5, 1)
	FIELD(FTM_OUTINIT, CH6OI, 6, 1)
	FIELD(FTM_OUTINIT, CH7OI, 7, 1)
REG32(FTM_OUTMASK, 0x60)
	FIELD(FTM_OUTMASK, CH0OM, 0, 1)
	FIELD(FTM_OUTMASK, CH1OM, 1, 1)
	FIELD(FTM_OUTMASK, CH2OM, 2, 1)
	FIELD(FTM_OUTMASK, CH3OM, 3, 1)
	FIELD(FTM_OUTMASK, CH4OM, 4, 1)
	FIELD(FTM_OUTMASK, CH5OM, 5, 1)
	FIELD(FTM_OUTMASK, CH6OM, 6, 1)
	FIELD(FTM_OUTMASK, CH7OM, 7, 1)
REG32(FTM_COMBINE, 0x64)
	FIELD(FTM_COMBINE, COMBINE0, 0, 1)
	FIELD(FTM_COMBINE, COMP0, 1, 1)
	FIELD(FTM_COMBINE, DECAPEN0, 2, 1)
	FIELD(FTM_COMBINE, DECAP0, 3, 1)
	FIELD(FTM_COMBINE, DTEN0, 4, 1)
	FIELD(FTM_COMBINE, SYNCEN0, 5, 1)
	FIELD(FTM_COMBINE, FAULTEN0, 6, 1)
	FIELD(FTM_COMBINE, COMBINE1, 8, 1)
	FIELD(FTM_COMBINE, COMP1, 9, 1)
	FIELD(FTM_COMBINE, DECAPEN1, 10, 1)
	FIELD(FTM_COMBINE, DECAP1, 11, 1)
	FIELD(FTM_COMBINE, DTEN1, 12, 1)
	FIELD(FTM_COMBINE, SYNCEN1, 13, 1)
	FIELD(FTM_COMBINE, FAULTEN1, 14, 1)
	FIELD(FTM_COMBINE, COMBINE2, 16, 1)
	FIELD(FTM_COMBINE, COMP2, 17, 1)
	FIELD(FTM_COMBINE, DECAPEN2, 18, 1)
	FIELD(FTM_COMBINE, DECAP2, 19, 1)
	FIELD(FTM_COMBINE, DTEN2, 20, 1)
	FIELD(FTM_COMBINE, SYNCEN2, 21, 1)
	FIELD(FTM_COMBINE, FAULTEN2, 22, 1)
	FIELD(FTM_COMBINE, COMBINE3, 24, 1)
	FIELD(FTM_COMBINE, COMP3, 25, 1)
	FIELD(FTM_COMBINE, DECAPEN3, 26, 1)
	FIELD(FTM_COMBINE, DECAP3, 27, 1)
	FIELD(FTM_COMBINE, DTEN3, 28, 1)
	FIELD(FTM_COMBINE, SYNCEN3, 29, 1)
	FIELD(FTM_COMBINE, FAULTEN3, 30, 1)
REG32(FTM_DEADTIME, 0x68)
	FIELD(FTM_DEADTIME, DTVAL, 0, 6)
	FIELD(FTM_DEADTIME, DTPS, 6, 2)
REG32(FTM_EXTTRIG, 0x6c)
	FIELD(FTM_EXTTRIG, CH2TRIG, 0, 1)
	FIELD(FTM_EXTTRIG, CH3TRIG, 1, 1)
	FIELD(FTM_EXTTRIG, CH4TRIG, 2, 1)
	FIELD(FTM_EXTTRIG, CH5TRIG, 3, 1)
	FIELD(FTM_EXTTRIG, CH0TRIG, 4, 1)
	FIELD(FTM_EXTTRIG, CH1TRIG, 5, 1)
	FIELD(FTM_EXTTRIG, INITTRIGEN, 6, 1)
	FIELD(FTM_EXTTRIG, TRIGF, 7, 1)
REG32(FTM_POL, 0x70)
	FIELD(FTM_POL, POL0, 0, 1)
	FIELD(FTM_POL, POL1, 1, 1)
	FIELD(FTM_POL, POL2, 2, 1)
	FIELD(FTM_POL, POL3, 3, 1)
	FIELD(FTM_POL, POL4, 4, 1)
	FIELD(FTM_POL, POL5, 5, 1)
	FIELD(FTM_POL, POL6, 6, 1)
	FIELD(FTM_POL, POL7, 7, 1)
REG32(FTM_FMS, 0x74)
	FIELD(FTM_FMS, FAULTF0, 0, 1)
	FIELD(FTM_FMS, FAULTF1, 1, 1)
	FIELD(FTM_FMS, FAULTF2, 2, 1)
	FIELD(FTM_FMS, FAULTF3, 3, 1)
	FIELD(FTM_FMS, FAULTIN, 5, 1)
	FIELD(FTM_FMS, WPEN, 6, 1)
	FIELD(FTM_FMS, FAULTF, 7, 1)
REG32(FTM_FILTER, 0x78)
	FIELD(FTM_FILTER, CH0FVAL, 0, 4)
	FIELD(FTM_FILTER, CH1FVAL, 4, 4)
	FIELD(FTM_FILTER, CH2FVAL, 8, 4)
	FIELD(FTM_FILTER, CH3FVAL, 12, 4)
REG32(FTM_FLTCTRL, 0x7c)
	FIELD(FTM_FLTCTRL, FAULT0EN, 0, 1)
	FIELD(FTM_FLTCTRL, FAULT1EN, 1, 1)
	FIELD(FTM_FLTCTRL, FAULT2EN, 2, 1)
	FIELD(FTM_FLTCTRL, FAULT3EN, 3, 1)
	FIELD(FTM_FLTCTRL, FFLTR0EN, 4, 1)
	FIELD(FTM_FLTCTRL, FFLTR1EN, 5, 1)
	FIELD(FTM_FLTCTRL, FFLTR2EN, 6, 1)
	FIELD(FTM_FLTCTRL, FFLTR3EN, 7, 1)
	FIELD(FTM_FLTCTRL, FFVAL, 8, 4)
REG32(FTM_QDCTRL, 0x80)
	FIELD(FTM_QDCTRL, QUADEN, 0, 1)
	FIELD(FTM_QDCTRL, TOFDIR, 1, 1)
	FIELD(FTM_QDCTRL, QUADIR, 2, 1)
	FIELD(FTM_QDCTRL, QUADMODE, 3, 1)
	FIELD(FTM_QDCTRL, PHBPOL, 4, 1)
	FIELD(FTM_QDCTRL, PHAPOL, 5, 1)
	FIELD(FTM_QDCTRL, PHBFLTREN, 6, 1)
	FIELD(FTM_QDCTRL, PHAFLTREN, 7, 1)
REG32(FTM_CONF, 0x84)
	FIELD(FTM_CONF, NUMTOF, 0, 5)
	FIELD(FTM_CONF, BDMMODE, 6, 2)
	FIELD(FTM_CONF, GTBEEN, 9, 1)
	FIELD(FTM_CONF, GTBEOUT, 10, 1)
REG32(FTM_FLTPOL, 0x88)
	FIELD(FTM_FLTPOL, FLT0POL, 0, 1)
	FIELD(FTM_FLTPOL, FLT1POL, 1, 1)
	FIELD(FTM_FLTPOL, FLT2POL, 2, 1)
	FIELD(FTM_FLTPOL, FLT3POL, 3, 1)
REG32(FTM_SYNCONF, 0x8c)
	FIELD(FTM_SYNCONF, HWTRIGMODE, 0, 1)
	FIELD(FTM_SYNCONF, CNTINC, 2, 1)
	FIELD(FTM_SYNCONF, INVC, 4, 1)
	FIELD(FTM_SYNCONF, SWOC, 5, 1)
	FIELD(FTM_SYNCONF, SYNCMODE, 7, 1)
	FIELD(FTM_SYNCONF, SWRSTCNT, 8, 1)
	FIELD(FTM_SYNCONF, SWWRBUF, 9, 1)
	FIELD(FTM_SYNCONF, SWOM, 10, 1)
	FIELD(FTM_SYNCONF, SWINVC, 11, 1)
	FIELD(FTM_SYNCONF, SWSOC, 12, 1)
	FIELD(FTM_SYNCONF, HWRSTCNT, 16, 1)
	FIELD(FTM_SYNCONF, HWWRBUF, 17, 1)
	FIELD(FTM_SYNCONF, HWOM, 18, 1)
	FIELD(FTM_SYNCONF, HWINVC, 19, 1)
	FIELD(FTM_SYNCONF, HWSOC, 20, 1)
REG32(FTM_INVCTRL, 0x90)
	FIELD(FTM_INVCTRL, INV0EN, 0, 1)
	FIELD(FTM_INVCTRL, INV1EN, 1, 1)
	FIELD(FTM_INVCTRL, INV2EN, 2, 1)
	FIELD(FTM_INVCTRL, INV3EN, 3, 1)
REG32(FTM_SWOCTRL, 0x94)
	FIELD(FTM_SWOCTRL, CH0OC, 0, 1)
	FIELD(FTM_SWOCTRL, CH1OC, 1, 1)
	FIELD(FTM_SWOCTRL, CH2OC, 2, 1)
	FIELD(FTM_SWOCTRL, CH3OC, 3, 1)
	FIELD(FTM_SWOCTRL, CH4OC, 4, 1)
	FIELD(FTM_SWOCTRL, CH5OC, 5, 1)
	FIELD(FTM_SWOCTRL, CH6OC, 6, 1)
	FIELD(FTM_SWOCTRL, CH7OC, 7, 1)
	FIELD(FTM_SWOCTRL, CH0OCV, 8, 1)
	FIELD(FTM_SWOCTRL, CH1OCV, 9, 1)
	FIELD(FTM_SWOCTRL, CH2OCV, 10, 1)
	FIELD(FTM_SWOCTRL, CH3OCV, 11, 1)
	FIELD(FTM_SWOCTRL, CH4OCV, 12, 1)
	FIELD(FTM_SWOCTRL, CH5OCV, 13, 1)
	FIELD(FTM_SWOCTRL, CH6OCV, 14, 1)
	FIELD(FTM_SWOCTRL, CH7OCV, 15, 1)
REG32(FTM_PWMLOAD, 0x98)
	FIELD(FTM_PWMLOAD, CH0SEL, 0, 1)
	FIELD(FTM_PWMLOAD, CH1SEL, 1, 1)
	FIELD(FTM_PWMLOAD, CH2SEL, 2, 1)
	FIELD(FTM_PWMLOAD, CH3SEL, 3, 1)
	FIELD(FTM_PWMLOAD, CH4SEL, 4, 1)
	FIELD(FTM_PWMLOAD, CH5SEL, 5, 1)
	FIELD(FTM_PWMLOAD, CH6SEL, 6, 1)
	FIELD(FTM_PWMLOAD, CH7SEL, 7, 1)
	FIELD(FTM_PWMLOAD, LDOK, 9, 1)
REG32(FTM_C0SC, 0xc)
	FIELD(FTM_C0SC, DMA, 0, 1)
	FIELD(FTM_C0SC, ELSA, 2, 1)
	FIELD(FTM_C0SC, ELSB, 3, 1)
	FIELD(FTM_C0SC, MSA, 4, 1)
	FIELD(FTM_C0SC, MSB, 5, 1)
	FIELD(FTM_C0SC, CHIE, 6, 1)
	FIELD(FTM_C0SC, CHF, 7, 1)
REG32(FTM_C1SC, 0x14)
	FIELD(FTM_C1SC, DMA, 0, 1)
	FIELD(FTM_C1SC, ELSA, 2, 1)
	FIELD(FTM_C1SC, ELSB, 3, 1)
	FIELD(FTM_C1SC, MSA, 4, 1)
	FIELD(FTM_C1SC, MSB, 5, 1)
	FIELD(FTM_C1SC, CHIE, 6, 1)
	FIELD(FTM_C1SC, CHF, 7, 1)
REG32(FTM_C2SC, 0x1c)
	FIELD(FTM_C2SC, DMA, 0, 1)
	FIELD(FTM_C2SC, ELSA, 2, 1)
	FIELD(FTM_C2SC, ELSB, 3, 1)
	FIELD(FTM_C2SC, MSA, 4, 1)
	FIELD(FTM_C2SC, MSB, 5, 1)
	FIELD(FTM_C2SC, CHIE, 6, 1)
	FIELD(FTM_C2SC, CHF, 7, 1)
REG32(FTM_C3SC, 0x24)
	FIELD(FTM_C3SC, DMA, 0, 1)
	FIELD(FTM_C3SC, ELSA, 2, 1)
	FIELD(FTM_C3SC, ELSB, 3, 1)
	FIELD(FTM_C3SC, MSA, 4, 1)
	FIELD(FTM_C3SC, MSB, 5, 1)
	FIELD(FTM_C3SC, CHIE, 6, 1)
	FIELD(FTM_C3SC, CHF, 7, 1)
REG32(FTM_C4SC, 0x2c)
	FIELD(FTM_C4SC, DMA, 0, 1)
	FIELD(FTM_C4SC, ELSA, 2, 1)
	FIELD(FTM_C4SC, ELSB, 3, 1)
	FIELD(FTM_C4SC, MSA, 4, 1)
	FIELD(FTM_C4SC, MSB, 5, 1)
	FIELD(FTM_C4SC, CHIE, 6, 1)
	FIELD(FTM_C4SC, CHF, 7, 1)
REG32(FTM_C5SC, 0x34)
	FIELD(FTM_C5SC, DMA, 0, 1)
	FIELD(FTM_C5SC, ELSA, 2, 1)
	FIELD(FTM_C5SC, ELSB, 3, 1)
	FIELD(FTM_C5SC, MSA, 4, 1)
	FIELD(FTM_C5SC, MSB, 5, 1)
	FIELD(FTM_C5SC, CHIE, 6, 1)
	FIELD(FTM_C5SC, CHF, 7, 1)
REG32(FTM_C6SC, 0x3c)
	FIELD(FTM_C6SC, DMA, 0, 1)
	FIELD(FTM_C6SC, ELSA, 2, 1)
	FIELD(FTM_C6SC, ELSB, 3, 1)
	FIELD(FTM_C6SC, MSA, 4, 1)
	FIELD(FTM_C6SC, MSB, 5, 1)
	FIELD(FTM_C6SC, CHIE, 6, 1)
	FIELD(FTM_C6SC, CHF, 7, 1)
REG32(FTM_C7SC, 0x44)
	FIELD(FTM_C7SC, DMA, 0, 1)
	FIELD(FTM_C7SC, ELSA, 2, 1)
	FIELD(FTM_C7SC, ELSB, 3, 1)
	FIELD(FTM_C7SC, MSA, 4, 1)
	FIELD(FTM_C7SC, MSB, 5, 1)
	FIELD(FTM_C7SC, CHIE, 6, 1)
	FIELD(FTM_C7SC, CHF, 7, 1)
REG32(FTM_C0V, 0x10)
	FIELD(FTM_C0V, VAL, 0, 16)
REG32(FTM_C1V, 0x18)
	FIELD(FTM_C1V, VAL, 0, 16)
REG32(FTM_C2V, 0x20)
	FIELD(FTM_C2V, VAL, 0, 16)
REG32(FTM_C3V, 0x28)
	FIELD(FTM_C3V, VAL, 0, 16)
REG32(FTM_C4V, 0x30)
	FIELD(FTM_C4V, VAL, 0, 16)
REG32(FTM_C5V, 0x38)
	FIELD(FTM_C5V, VAL, 0, 16)
REG32(FTM_C6V, 0x40)
	FIELD(FTM_C6V, VAL, 0, 16)
REG32(FTM_C7V, 0x48)
	FIELD(FTM_C7V, VAL, 0, 16)


static void frdmk64f_ftm_register_reset(FRDMK64FFTM *t) {
	t->SC = 0x0;
	t->CNT = 0x0;
	t->MOD = 0x0;
	t->CNTIN = 0x0;
	t->STATUS = 0x0;
	t->MODE = 0x4;
	t->SYNC = 0x0;
	t->OUTINIT = 0x0;
	t->OUTMASK = 0x0;
	t->COMBINE = 0x0;
	t->DEADTIME = 0x0;
	t->EXTTRIG = 0x0;
	t->POL = 0x0;
	t->FMS = 0x0;
	t->FILTER = 0x0;
	t->FLTCTRL = 0x0;
	t->QDCTRL = 0x0;
	t->CONF = 0x0;
	t->FLTPOL = 0x0;
	t->SYNCONF = 0x0;
	t->INVCTRL = 0x0;
	t->SWOCTRL = 0x0;
	t->PWMLOAD = 0x0;
	t->C0SC = 0x0;
	t->C1SC = 0x0;
	t->C2SC = 0x0;
	t->C3SC = 0x0;
	t->C4SC = 0x0;
	t->C5SC = 0x0;
	t->C6SC = 0x0;
	t->C7SC = 0x0;
	t->C0V = 0x0;
	t->C1V = 0x0;
	t->C2V = 0x0;
	t->C3V = 0x0;
	t->C4V = 0x0;
	t->C5V = 0x0;
	t->C6V = 0x0;
	t->C7V = 0x0;

}

static uint64_t frdmk64f_ftm_read(void *opaque, hwaddr offset, unsigned size) {
	FRDMK64FFTM *t = FRDMK64F_FTM(opaque);
	uint64_t ret;

	switch (offset) {
		case A_FTM_SC:
			ret = t->SC;
			break;
		case A_FTM_CNT:
			ret = t->CNT;
			break;
		case A_FTM_MOD:
			ret = t->MOD;
			break;
		case A_FTM_CNTIN:
			ret = t->CNTIN;
			break;
		case A_FTM_STATUS:
			ret = t->STATUS;
			break;
		case A_FTM_MODE:
			ret = t->MODE;
			break;
		case A_FTM_SYNC:
			ret = t->SYNC;
			break;
		case A_FTM_OUTINIT:
			ret = t->OUTINIT;
			break;
		case A_FTM_OUTMASK:
			ret = t->OUTMASK;
			break;
		case A_FTM_COMBINE:
			ret = t->COMBINE;
			break;
		case A_FTM_DEADTIME:
			ret = t->DEADTIME;
			break;
		case A_FTM_EXTTRIG:
			ret = t->EXTTRIG;
			break;
		case A_FTM_POL:
			ret = t->POL;
			break;
		case A_FTM_FMS:
			ret = t->FMS;
			break;
		case A_FTM_FILTER:
			ret = t->FILTER;
			break;
		case A_FTM_FLTCTRL:
			ret = t->FLTCTRL;
			break;
		case A_FTM_QDCTRL:
			ret = t->QDCTRL;
			break;
		case A_FTM_CONF:
			ret = t->CONF;
			break;
		case A_FTM_FLTPOL:
			ret = t->FLTPOL;
			break;
		case A_FTM_SYNCONF:
			ret = t->SYNCONF;
			break;
		case A_FTM_INVCTRL:
			ret = t->INVCTRL;
			break;
		case A_FTM_SWOCTRL:
			ret = t->SWOCTRL;
			break;
		case A_FTM_PWMLOAD:
			ret = t->PWMLOAD;
			break;
		case A_FTM_C0SC:
			ret = t->C0SC;
			break;
		case A_FTM_C1SC:
			ret = t->C1SC;
			break;
		case A_FTM_C2SC:
			ret = t->C2SC;
			break;
		case A_FTM_C3SC:
			ret = t->C3SC;
			break;
		case A_FTM_C4SC:
			ret = t->C4SC;
			break;
		case A_FTM_C5SC:
			ret = t->C5SC;
			break;
		case A_FTM_C6SC:
			ret = t->C6SC;
			break;
		case A_FTM_C7SC:
			ret = t->C7SC;
			break;
		case A_FTM_C0V:
			ret = t->C0V;
			break;
		case A_FTM_C1V:
			ret = t->C1V;
			break;
		case A_FTM_C2V:
			ret = t->C2V;
			break;
		case A_FTM_C3V:
			ret = t->C3V;
			break;
		case A_FTM_C4V:
			ret = t->C4V;
			break;
		case A_FTM_C5V:
			ret = t->C5V;
			break;
		case A_FTM_C6V:
			ret = t->C6V;
			break;
		case A_FTM_C7V:
			ret = t->C7V;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK64F FTM read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void frdmk64f_ftm_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	FRDMK64FFTM *t = FRDMK64F_FTM(opaque);


	switch (offset) {
		case A_FTM_SC ... A_FTM_SC + 3:
			value = value << ((offset - A_FTM_SC) << 3);
			offset = A_FTM_SC;
			break;
		case A_FTM_CNT ... A_FTM_CNT + 3:
			value = value << ((offset - A_FTM_CNT) << 3);
			offset = A_FTM_CNT;
			break;
		case A_FTM_MOD ... A_FTM_MOD + 3:
			value = value << ((offset - A_FTM_MOD) << 3);
			offset = A_FTM_MOD;
			break;
		case A_FTM_CNTIN ... A_FTM_CNTIN + 3:
			value = value << ((offset - A_FTM_CNTIN) << 3);
			offset = A_FTM_CNTIN;
			break;
		case A_FTM_STATUS ... A_FTM_STATUS + 3:
			value = value << ((offset - A_FTM_STATUS) << 3);
			offset = A_FTM_STATUS;
			break;
		case A_FTM_MODE ... A_FTM_MODE + 3:
			value = value << ((offset - A_FTM_MODE) << 3);
			offset = A_FTM_MODE;
			break;
		case A_FTM_SYNC ... A_FTM_SYNC + 3:
			value = value << ((offset - A_FTM_SYNC) << 3);
			offset = A_FTM_SYNC;
			break;
		case A_FTM_OUTINIT ... A_FTM_OUTINIT + 3:
			value = value << ((offset - A_FTM_OUTINIT) << 3);
			offset = A_FTM_OUTINIT;
			break;
		case A_FTM_OUTMASK ... A_FTM_OUTMASK + 3:
			value = value << ((offset - A_FTM_OUTMASK) << 3);
			offset = A_FTM_OUTMASK;
			break;
		case A_FTM_COMBINE ... A_FTM_COMBINE + 3:
			value = value << ((offset - A_FTM_COMBINE) << 3);
			offset = A_FTM_COMBINE;
			break;
		case A_FTM_DEADTIME ... A_FTM_DEADTIME + 3:
			value = value << ((offset - A_FTM_DEADTIME) << 3);
			offset = A_FTM_DEADTIME;
			break;
		case A_FTM_EXTTRIG ... A_FTM_EXTTRIG + 3:
			value = value << ((offset - A_FTM_EXTTRIG) << 3);
			offset = A_FTM_EXTTRIG;
			break;
		case A_FTM_POL ... A_FTM_POL + 3:
			value = value << ((offset - A_FTM_POL) << 3);
			offset = A_FTM_POL;
			break;
		case A_FTM_FMS ... A_FTM_FMS + 3:
			value = value << ((offset - A_FTM_FMS) << 3);
			offset = A_FTM_FMS;
			break;
		case A_FTM_FILTER ... A_FTM_FILTER + 3:
			value = value << ((offset - A_FTM_FILTER) << 3);
			offset = A_FTM_FILTER;
			break;
		case A_FTM_FLTCTRL ... A_FTM_FLTCTRL + 3:
			value = value << ((offset - A_FTM_FLTCTRL) << 3);
			offset = A_FTM_FLTCTRL;
			break;
		case A_FTM_QDCTRL ... A_FTM_QDCTRL + 3:
			value = value << ((offset - A_FTM_QDCTRL) << 3);
			offset = A_FTM_QDCTRL;
			break;
		case A_FTM_CONF ... A_FTM_CONF + 3:
			value = value << ((offset - A_FTM_CONF) << 3);
			offset = A_FTM_CONF;
			break;
		case A_FTM_FLTPOL ... A_FTM_FLTPOL + 3:
			value = value << ((offset - A_FTM_FLTPOL) << 3);
			offset = A_FTM_FLTPOL;
			break;
		case A_FTM_SYNCONF ... A_FTM_SYNCONF + 3:
			value = value << ((offset - A_FTM_SYNCONF) << 3);
			offset = A_FTM_SYNCONF;
			break;
		case A_FTM_INVCTRL ... A_FTM_INVCTRL + 3:
			value = value << ((offset - A_FTM_INVCTRL) << 3);
			offset = A_FTM_INVCTRL;
			break;
		case A_FTM_SWOCTRL ... A_FTM_SWOCTRL + 3:
			value = value << ((offset - A_FTM_SWOCTRL) << 3);
			offset = A_FTM_SWOCTRL;
			break;
		case A_FTM_PWMLOAD ... A_FTM_PWMLOAD + 3:
			value = value << ((offset - A_FTM_PWMLOAD) << 3);
			offset = A_FTM_PWMLOAD;
			break;
		case A_FTM_C0SC ... A_FTM_C0SC + 3:
			value = value << ((offset - A_FTM_C0SC) << 3);
			offset = A_FTM_C0SC;
			break;
		case A_FTM_C1SC ... A_FTM_C1SC + 3:
			value = value << ((offset - A_FTM_C1SC) << 3);
			offset = A_FTM_C1SC;
			break;
		case A_FTM_C2SC ... A_FTM_C2SC + 3:
			value = value << ((offset - A_FTM_C2SC) << 3);
			offset = A_FTM_C2SC;
			break;
		case A_FTM_C3SC ... A_FTM_C3SC + 3:
			value = value << ((offset - A_FTM_C3SC) << 3);
			offset = A_FTM_C3SC;
			break;
		case A_FTM_C4SC ... A_FTM_C4SC + 3:
			value = value << ((offset - A_FTM_C4SC) << 3);
			offset = A_FTM_C4SC;
			break;
		case A_FTM_C5SC ... A_FTM_C5SC + 3:
			value = value << ((offset - A_FTM_C5SC) << 3);
			offset = A_FTM_C5SC;
			break;
		case A_FTM_C6SC ... A_FTM_C6SC + 3:
			value = value << ((offset - A_FTM_C6SC) << 3);
			offset = A_FTM_C6SC;
			break;
		case A_FTM_C7SC ... A_FTM_C7SC + 3:
			value = value << ((offset - A_FTM_C7SC) << 3);
			offset = A_FTM_C7SC;
			break;
		case A_FTM_C0V ... A_FTM_C0V + 3:
			value = value << ((offset - A_FTM_C0V) << 3);
			offset = A_FTM_C0V;
			break;
		case A_FTM_C1V ... A_FTM_C1V + 3:
			value = value << ((offset - A_FTM_C1V) << 3);
			offset = A_FTM_C1V;
			break;
		case A_FTM_C2V ... A_FTM_C2V + 3:
			value = value << ((offset - A_FTM_C2V) << 3);
			offset = A_FTM_C2V;
			break;
		case A_FTM_C3V ... A_FTM_C3V + 3:
			value = value << ((offset - A_FTM_C3V) << 3);
			offset = A_FTM_C3V;
			break;
		case A_FTM_C4V ... A_FTM_C4V + 3:
			value = value << ((offset - A_FTM_C4V) << 3);
			offset = A_FTM_C4V;
			break;
		case A_FTM_C5V ... A_FTM_C5V + 3:
			value = value << ((offset - A_FTM_C5V) << 3);
			offset = A_FTM_C5V;
			break;
		case A_FTM_C6V ... A_FTM_C6V + 3:
			value = value << ((offset - A_FTM_C6V) << 3);
			offset = A_FTM_C6V;
			break;
		case A_FTM_C7V ... A_FTM_C7V + 3:
			value = value << ((offset - A_FTM_C7V) << 3);
			offset = A_FTM_C7V;
			break;

		default: break;
	}

	switch (offset) {
		case A_FTM_SC:
			t->SC = value;
			break;
		case A_FTM_CNT:
			t->CNT = value;
			break;
		case A_FTM_MOD:
			t->MOD = value;
			break;
		case A_FTM_CNTIN:
			t->CNTIN = value;
			break;
		case A_FTM_STATUS:
			t->STATUS = value;
			break;
		case A_FTM_MODE:
			t->MODE = value;
			break;
		case A_FTM_SYNC:
			t->SYNC = value;
			break;
		case A_FTM_OUTINIT:
			t->OUTINIT = value;
			break;
		case A_FTM_OUTMASK:
			t->OUTMASK = value;
			break;
		case A_FTM_COMBINE:
			t->COMBINE = value;
			break;
		case A_FTM_DEADTIME:
			t->DEADTIME = value;
			break;
		case A_FTM_EXTTRIG:
			t->EXTTRIG = value;
			break;
		case A_FTM_POL:
			t->POL = value;
			break;
		case A_FTM_FMS:
			t->FMS = value;
			break;
		case A_FTM_FILTER:
			t->FILTER = value;
			break;
		case A_FTM_FLTCTRL:
			t->FLTCTRL = value;
			break;
		case A_FTM_QDCTRL:
			t->QDCTRL = value;
			break;
		case A_FTM_CONF:
			t->CONF = value;
			break;
		case A_FTM_FLTPOL:
			t->FLTPOL = value;
			break;
		case A_FTM_SYNCONF:
			t->SYNCONF = value;
			break;
		case A_FTM_INVCTRL:
			t->INVCTRL = value;
			break;
		case A_FTM_SWOCTRL:
			t->SWOCTRL = value;
			break;
		case A_FTM_PWMLOAD:
			t->PWMLOAD = value;
			break;
		case A_FTM_C0SC:
			t->C0SC = value;
			break;
		case A_FTM_C1SC:
			t->C1SC = value;
			break;
		case A_FTM_C2SC:
			t->C2SC = value;
			break;
		case A_FTM_C3SC:
			t->C3SC = value;
			break;
		case A_FTM_C4SC:
			t->C4SC = value;
			break;
		case A_FTM_C5SC:
			t->C5SC = value;
			break;
		case A_FTM_C6SC:
			t->C6SC = value;
			break;
		case A_FTM_C7SC:
			t->C7SC = value;
			break;
		case A_FTM_C0V:
			t->C0V = value;
			break;
		case A_FTM_C1V:
			t->C1V = value;
			break;
		case A_FTM_C2V:
			t->C2V = value;
			break;
		case A_FTM_C3V:
			t->C3V = value;
			break;
		case A_FTM_C4V:
			t->C4V = value;
			break;
		case A_FTM_C5V:
			t->C5V = value;
			break;
		case A_FTM_C6V:
			t->C6V = value;
			break;
		case A_FTM_C7V:
			t->C7V = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "FRDMK64F FTM write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps frdmk64f_ftm_ops = {
	.read = frdmk64f_ftm_read,
	.write = frdmk64f_ftm_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void frdmk64f_ftm_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	FRDMK64FFTM *t = FRDMK64F_FTM(obj);
	memory_region_init_io(&(t->iomem), obj, &frdmk64f_ftm_ops, t, "frdmk64f-ftm", FRDMK64F_FTM_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void frdmk64f_ftm_realize(DeviceState *dev, Error **errp) {
	return;

}

static void frdmk64f_ftm_reset_enter(Object *obj, ResetType type) {
	FRDMK64FFTM *t = FRDMK64F_FTM(obj);
	frdmk64f_ftm_register_reset(t);
}

static const VMStateDescription frdmk64f_ftm_vmstate = {
	.name = "frdmk64f-ftm",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(SC, FRDMK64FFTM),
		VMSTATE_UINT32(CNT, FRDMK64FFTM),
		VMSTATE_UINT32(MOD, FRDMK64FFTM),
		VMSTATE_UINT32(CNTIN, FRDMK64FFTM),
		VMSTATE_UINT32(STATUS, FRDMK64FFTM),
		VMSTATE_UINT32(MODE, FRDMK64FFTM),
		VMSTATE_UINT32(SYNC, FRDMK64FFTM),
		VMSTATE_UINT32(OUTINIT, FRDMK64FFTM),
		VMSTATE_UINT32(OUTMASK, FRDMK64FFTM),
		VMSTATE_UINT32(COMBINE, FRDMK64FFTM),
		VMSTATE_UINT32(DEADTIME, FRDMK64FFTM),
		VMSTATE_UINT32(EXTTRIG, FRDMK64FFTM),
		VMSTATE_UINT32(POL, FRDMK64FFTM),
		VMSTATE_UINT32(FMS, FRDMK64FFTM),
		VMSTATE_UINT32(FILTER, FRDMK64FFTM),
		VMSTATE_UINT32(FLTCTRL, FRDMK64FFTM),
		VMSTATE_UINT32(QDCTRL, FRDMK64FFTM),
		VMSTATE_UINT32(CONF, FRDMK64FFTM),
		VMSTATE_UINT32(FLTPOL, FRDMK64FFTM),
		VMSTATE_UINT32(SYNCONF, FRDMK64FFTM),
		VMSTATE_UINT32(INVCTRL, FRDMK64FFTM),
		VMSTATE_UINT32(SWOCTRL, FRDMK64FFTM),
		VMSTATE_UINT32(PWMLOAD, FRDMK64FFTM),
		VMSTATE_UINT32(C0SC, FRDMK64FFTM),
		VMSTATE_UINT32(C1SC, FRDMK64FFTM),
		VMSTATE_UINT32(C2SC, FRDMK64FFTM),
		VMSTATE_UINT32(C3SC, FRDMK64FFTM),
		VMSTATE_UINT32(C4SC, FRDMK64FFTM),
		VMSTATE_UINT32(C5SC, FRDMK64FFTM),
		VMSTATE_UINT32(C6SC, FRDMK64FFTM),
		VMSTATE_UINT32(C7SC, FRDMK64FFTM),
		VMSTATE_UINT32(C0V, FRDMK64FFTM),
		VMSTATE_UINT32(C1V, FRDMK64FFTM),
		VMSTATE_UINT32(C2V, FRDMK64FFTM),
		VMSTATE_UINT32(C3V, FRDMK64FFTM),
		VMSTATE_UINT32(C4V, FRDMK64FFTM),
		VMSTATE_UINT32(C5V, FRDMK64FFTM),
		VMSTATE_UINT32(C6V, FRDMK64FFTM),
		VMSTATE_UINT32(C7V, FRDMK64FFTM),

		VMSTATE_END_OF_LIST()
	}
};

static void frdmk64f_ftm_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &frdmk64f_ftm_vmstate;
	dc->realize = frdmk64f_ftm_realize;
	rc->phases.enter = frdmk64f_ftm_reset_enter;

}

static const TypeInfo frdmk64f_ftm_info = {
	.name = TYPE_FRDMK64F_FTM,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(FRDMK64FFTM),
	.instance_init = frdmk64f_ftm_init,
	.class_init = frdmk64f_ftm_class_init,
};

static void frdmk64f_ftm_register_types(void) {
	type_register_static(&frdmk64f_ftm_info);
}

type_init(frdmk64f_ftm_register_types);

#define TYPE_NXP_FRDM_K64F_MACHINE MACHINE_TYPE_NAME("nxp_frdm_k64f")
OBJECT_DECLARE_TYPE(NXP_FRDM_K64FMachineState, NXP_FRDM_K64FMachineClass, NXP_FRDM_K64F_MACHINE)

struct NXP_FRDM_K64FMachineClass {
	MachineClass parent;
};

struct NXP_FRDM_K64FMachineState {
	MachineState parent;
	ARMv7MState armv7m;
};

static void nxp_frdm_k64f_periph_init(MachineState *machine) {
	NXP_FRDM_K64FMachineState *sms = NXP_FRDM_K64F_MACHINE(machine);
	FRDMK64FUART0 *p0 = g_new(FRDMK64FUART0, 1);
	object_initialize_child(OBJECT(sms), "UART0", p0, TYPE_FRDMK64F_UART0);
	qdev_prop_set_chr(DEVICE(p0), "chardev", qemu_chr_new("soc-uart0", "chardev:uart0", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p0), &error_fatal);
	p0->base = 0x4006a000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p0), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 30));
	sysbus_connect_irq(SYS_BUS_DEVICE(p0), 1, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 31));
	sysbus_connect_irq(SYS_BUS_DEVICE(p0), 2, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 32));
	sysbus_mmio_map(SYS_BUS_DEVICE(p0), 0, 0x4006a000);

	FRDMK64FUART3 *p1 = g_new(FRDMK64FUART3, 1);
	object_initialize_child(OBJECT(sms), "UART3", p1, TYPE_FRDMK64F_UART3);
	qdev_prop_set_chr(DEVICE(p1), "chardev", qemu_chr_new("soc-uart3", "chardev:uart3", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p1), &error_fatal);
	p1->base = 0x4006d000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p1), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 37));
	sysbus_connect_irq(SYS_BUS_DEVICE(p1), 1, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 38));
	sysbus_mmio_map(SYS_BUS_DEVICE(p1), 0, 0x4006d000);

	FRDMK64FWDOG *p2 = g_new(FRDMK64FWDOG, 1);
	object_initialize_child(OBJECT(sms), "WDOG", p2, TYPE_FRDMK64F_WDOG);
	sysbus_realize(SYS_BUS_DEVICE(p2), &error_fatal);
	p2->base = 0x40052000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p2), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 22));
	sysbus_mmio_map(SYS_BUS_DEVICE(p2), 0, 0x40052000);

	FRDMK64FSYSMPU *p3 = g_new(FRDMK64FSYSMPU, 1);
	object_initialize_child(OBJECT(sms), "SYSMPU", p3, TYPE_FRDMK64F_SYSMPU);
	sysbus_realize(SYS_BUS_DEVICE(p3), &error_fatal);
	p3->base = 0x4000d000;
	sysbus_mmio_map(SYS_BUS_DEVICE(p3), 0, 0x4000d000);

	FRDMK64FSIM *p4 = g_new(FRDMK64FSIM, 1);
	object_initialize_child(OBJECT(sms), "SIM", p4, TYPE_FRDMK64F_SIM);
	sysbus_realize(SYS_BUS_DEVICE(p4), &error_fatal);
	p4->base = 0x40047000;
	sysbus_mmio_map(SYS_BUS_DEVICE(p4), 0, 0x40047000);

	FRDMK64FPMC *p5 = g_new(FRDMK64FPMC, 1);
	object_initialize_child(OBJECT(sms), "PMC", p5, TYPE_FRDMK64F_PMC);
	sysbus_realize(SYS_BUS_DEVICE(p5), &error_fatal);
	p5->base = 0x4007d000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p5), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 20));
	sysbus_mmio_map(SYS_BUS_DEVICE(p5), 0, 0x4007d000);

	FRDMK64FOSC *p6 = g_new(FRDMK64FOSC, 1);
	object_initialize_child(OBJECT(sms), "OSC", p6, TYPE_FRDMK64F_OSC);
	sysbus_realize(SYS_BUS_DEVICE(p6), &error_fatal);
	p6->base = 0x40065000;
	sysbus_mmio_map(SYS_BUS_DEVICE(p6), 0, 0x40065000);

	FRDMK64FMCG *p7 = g_new(FRDMK64FMCG, 1);
	object_initialize_child(OBJECT(sms), "MCG", p7, TYPE_FRDMK64F_MCG);
	sysbus_realize(SYS_BUS_DEVICE(p7), &error_fatal);
	p7->base = 0x40064000;
	sysbus_mmio_map(SYS_BUS_DEVICE(p7), 0, 0x40064000);

	FRDMK64FRTC *p8 = g_new(FRDMK64FRTC, 1);
	object_initialize_child(OBJECT(sms), "RTC", p8, TYPE_FRDMK64F_RTC);
	sysbus_realize(SYS_BUS_DEVICE(p8), &error_fatal);
	p8->base = 0x4003d000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p8), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 46));
	sysbus_connect_irq(SYS_BUS_DEVICE(p8), 1, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 47));
	sysbus_mmio_map(SYS_BUS_DEVICE(p8), 0, 0x4003d000);

	FRDMK64FGPIO *p9 = g_new(FRDMK64FGPIO, 1);
	object_initialize_child(OBJECT(sms), "GPIOA", p9, TYPE_FRDMK64F_GPIO);
	sysbus_realize(SYS_BUS_DEVICE(p9), &error_fatal);
	p9->base = 0x400ff000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p9), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 59));
	sysbus_mmio_map(SYS_BUS_DEVICE(p9), 0, 0x400ff000);

	FRDMK64FGPIO *p10 = g_new(FRDMK64FGPIO, 1);
	object_initialize_child(OBJECT(sms), "GPIOB", p10, TYPE_FRDMK64F_GPIO);
	sysbus_realize(SYS_BUS_DEVICE(p10), &error_fatal);
	p10->base = 0x400ff040;
	sysbus_connect_irq(SYS_BUS_DEVICE(p10), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 60));
	sysbus_mmio_map(SYS_BUS_DEVICE(p10), 0, 0x400ff040);

	FRDMK64FGPIO *p11 = g_new(FRDMK64FGPIO, 1);
	object_initialize_child(OBJECT(sms), "GPIOC", p11, TYPE_FRDMK64F_GPIO);
	sysbus_realize(SYS_BUS_DEVICE(p11), &error_fatal);
	p11->base = 0x400ff080;
	sysbus_connect_irq(SYS_BUS_DEVICE(p11), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 61));
	sysbus_mmio_map(SYS_BUS_DEVICE(p11), 0, 0x400ff080);

	FRDMK64FGPIO *p12 = g_new(FRDMK64FGPIO, 1);
	object_initialize_child(OBJECT(sms), "GPIOD", p12, TYPE_FRDMK64F_GPIO);
	sysbus_realize(SYS_BUS_DEVICE(p12), &error_fatal);
	p12->base = 0x400ff0c0;
	sysbus_connect_irq(SYS_BUS_DEVICE(p12), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 62));
	sysbus_mmio_map(SYS_BUS_DEVICE(p12), 0, 0x400ff0c0);

	FRDMK64FGPIO *p13 = g_new(FRDMK64FGPIO, 1);
	object_initialize_child(OBJECT(sms), "GPIOE", p13, TYPE_FRDMK64F_GPIO);
	sysbus_realize(SYS_BUS_DEVICE(p13), &error_fatal);
	p13->base = 0x400ff100;
	sysbus_connect_irq(SYS_BUS_DEVICE(p13), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 63));
	sysbus_mmio_map(SYS_BUS_DEVICE(p13), 0, 0x400ff100);

	FRDMK64FPORT *p14 = g_new(FRDMK64FPORT, 1);
	object_initialize_child(OBJECT(sms), "PORTA", p14, TYPE_FRDMK64F_PORT);
	qdev_prop_set_chr(DEVICE(p14), "chardev", qemu_chr_new("soc-porta", "chardev:porta", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p14), &error_fatal);
	p14->base = 0x40049000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p14), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 59));
	sysbus_mmio_map(SYS_BUS_DEVICE(p14), 0, 0x40049000);

	FRDMK64FPORT *p15 = g_new(FRDMK64FPORT, 1);
	object_initialize_child(OBJECT(sms), "PORTB", p15, TYPE_FRDMK64F_PORT);
	qdev_prop_set_chr(DEVICE(p15), "chardev", qemu_chr_new("soc-portb", "chardev:portb", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p15), &error_fatal);
	p15->base = 0x4004a000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p15), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 60));
	sysbus_mmio_map(SYS_BUS_DEVICE(p15), 0, 0x4004a000);

	FRDMK64FPORT *p16 = g_new(FRDMK64FPORT, 1);
	object_initialize_child(OBJECT(sms), "PORTC", p16, TYPE_FRDMK64F_PORT);
	qdev_prop_set_chr(DEVICE(p16), "chardev", qemu_chr_new("soc-portc", "chardev:portc", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p16), &error_fatal);
	p16->base = 0x4004b000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p16), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 61));
	sysbus_mmio_map(SYS_BUS_DEVICE(p16), 0, 0x4004b000);

	FRDMK64FPORT *p17 = g_new(FRDMK64FPORT, 1);
	object_initialize_child(OBJECT(sms), "PORTD", p17, TYPE_FRDMK64F_PORT);
	qdev_prop_set_chr(DEVICE(p17), "chardev", qemu_chr_new("soc-portd", "chardev:portd", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p17), &error_fatal);
	p17->base = 0x4004c000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p17), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 62));
	sysbus_mmio_map(SYS_BUS_DEVICE(p17), 0, 0x4004c000);

	FRDMK64FPORT *p18 = g_new(FRDMK64FPORT, 1);
	object_initialize_child(OBJECT(sms), "PORTE", p18, TYPE_FRDMK64F_PORT);
	qdev_prop_set_chr(DEVICE(p18), "chardev", qemu_chr_new("soc-porte", "chardev:porte", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p18), &error_fatal);
	p18->base = 0x4004d000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p18), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 63));
	sysbus_mmio_map(SYS_BUS_DEVICE(p18), 0, 0x4004d000);

	FRDMK64FLPTMR0 *p19 = g_new(FRDMK64FLPTMR0, 1);
	object_initialize_child(OBJECT(sms), "LPTMR0", p19, TYPE_FRDMK64F_LPTMR0);
	sysbus_realize(SYS_BUS_DEVICE(p19), &error_fatal);
	p19->base = 0x40040000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p19), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 58));
	sysbus_mmio_map(SYS_BUS_DEVICE(p19), 0, 0x40040000);

	FRDMK64FSMC *p20 = g_new(FRDMK64FSMC, 1);
	object_initialize_child(OBJECT(sms), "SMC", p20, TYPE_FRDMK64F_SMC);
	sysbus_realize(SYS_BUS_DEVICE(p20), &error_fatal);
	p20->base = 0x4007e000;
	sysbus_mmio_map(SYS_BUS_DEVICE(p20), 0, 0x4007e000);

	FRDMK64FADC *p21 = g_new(FRDMK64FADC, 1);
	object_initialize_child(OBJECT(sms), "ADC0", p21, TYPE_FRDMK64F_ADC);
	qdev_prop_set_chr(DEVICE(p21), "chardev", qemu_chr_new("soc-adc0", "chardev:adc0", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p21), &error_fatal);
	p21->base = 0x4003b000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p21), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 39));
	sysbus_mmio_map(SYS_BUS_DEVICE(p21), 0, 0x4003b000);

	FRDMK64FADC *p22 = g_new(FRDMK64FADC, 1);
	object_initialize_child(OBJECT(sms), "ADC1", p22, TYPE_FRDMK64F_ADC);
	qdev_prop_set_chr(DEVICE(p22), "chardev", qemu_chr_new("soc-adc1", "chardev:adc1", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p22), &error_fatal);
	p22->base = 0x400bb000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p22), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 73));
	sysbus_mmio_map(SYS_BUS_DEVICE(p22), 0, 0x400bb000);

	FRDMK64FDAC *p23 = g_new(FRDMK64FDAC, 1);
	object_initialize_child(OBJECT(sms), "DAC0", p23, TYPE_FRDMK64F_DAC);
	sysbus_realize(SYS_BUS_DEVICE(p23), &error_fatal);
	p23->base = 0x400cc000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p23), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 56));
	sysbus_mmio_map(SYS_BUS_DEVICE(p23), 0, 0x400cc000);

	FRDMK64FDAC *p24 = g_new(FRDMK64FDAC, 1);
	object_initialize_child(OBJECT(sms), "DAC1", p24, TYPE_FRDMK64F_DAC);
	sysbus_realize(SYS_BUS_DEVICE(p24), &error_fatal);
	p24->base = 0x400cd000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p24), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 72));
	sysbus_mmio_map(SYS_BUS_DEVICE(p24), 0, 0x400cd000);

	FRDMK64FSPI *p25 = g_new(FRDMK64FSPI, 1);
	object_initialize_child(OBJECT(sms), "SPI0", p25, TYPE_FRDMK64F_SPI);
	qdev_prop_set_chr(DEVICE(p25), "chardev", qemu_chr_new("soc-spi0", "chardev:spi0", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p25), &error_fatal);
	p25->base = 0x4002c000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p25), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 26));
	sysbus_mmio_map(SYS_BUS_DEVICE(p25), 0, 0x4002c000);

	FRDMK64FSPI *p26 = g_new(FRDMK64FSPI, 1);
	object_initialize_child(OBJECT(sms), "SPI1", p26, TYPE_FRDMK64F_SPI);
	qdev_prop_set_chr(DEVICE(p26), "chardev", qemu_chr_new("soc-spi1", "chardev:spi1", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p26), &error_fatal);
	p26->base = 0x4002d000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p26), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 27));
	sysbus_mmio_map(SYS_BUS_DEVICE(p26), 0, 0x4002d000);

	FRDMK64FSPI *p27 = g_new(FRDMK64FSPI, 1);
	object_initialize_child(OBJECT(sms), "SPI2", p27, TYPE_FRDMK64F_SPI);
	qdev_prop_set_chr(DEVICE(p27), "chardev", qemu_chr_new("soc-spi2", "chardev:spi2", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p27), &error_fatal);
	p27->base = 0x400ac000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p27), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 65));
	sysbus_mmio_map(SYS_BUS_DEVICE(p27), 0, 0x400ac000);

	FRDMK64FI2C *p28 = g_new(FRDMK64FI2C, 1);
	object_initialize_child(OBJECT(sms), "I2C0", p28, TYPE_FRDMK64F_I2C);
	qdev_prop_set_chr(DEVICE(p28), "chardev", qemu_chr_new("soc-i2c0", "chardev:i2c0", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p28), &error_fatal);
	p28->base = 0x40066000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p28), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 24));
	sysbus_mmio_map(SYS_BUS_DEVICE(p28), 0, 0x40066000);

	FRDMK64FI2C *p29 = g_new(FRDMK64FI2C, 1);
	object_initialize_child(OBJECT(sms), "I2C1", p29, TYPE_FRDMK64F_I2C);
	qdev_prop_set_chr(DEVICE(p29), "chardev", qemu_chr_new("soc-i2c1", "chardev:i2c1", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p29), &error_fatal);
	p29->base = 0x40067000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p29), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 25));
	sysbus_mmio_map(SYS_BUS_DEVICE(p29), 0, 0x40067000);

	FRDMK64FI2C *p30 = g_new(FRDMK64FI2C, 1);
	object_initialize_child(OBJECT(sms), "I2C2", p30, TYPE_FRDMK64F_I2C);
	qdev_prop_set_chr(DEVICE(p30), "chardev", qemu_chr_new("soc-i2c2", "chardev:i2c2", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p30), &error_fatal);
	p30->base = 0x400e6000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p30), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 74));
	sysbus_mmio_map(SYS_BUS_DEVICE(p30), 0, 0x400e6000);

	FRDMK64FFTM *p31 = g_new(FRDMK64FFTM, 1);
	object_initialize_child(OBJECT(sms), "FTM0", p31, TYPE_FRDMK64F_FTM);
	sysbus_realize(SYS_BUS_DEVICE(p31), &error_fatal);
	p31->base = 0x40038000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p31), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 42));
	sysbus_mmio_map(SYS_BUS_DEVICE(p31), 0, 0x40038000);

	FRDMK64FFTM *p32 = g_new(FRDMK64FFTM, 1);
	object_initialize_child(OBJECT(sms), "FTM1", p32, TYPE_FRDMK64F_FTM);
	sysbus_realize(SYS_BUS_DEVICE(p32), &error_fatal);
	p32->base = 0x40039000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p32), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 43));
	sysbus_mmio_map(SYS_BUS_DEVICE(p32), 0, 0x40039000);

	FRDMK64FFTM *p33 = g_new(FRDMK64FFTM, 1);
	object_initialize_child(OBJECT(sms), "FTM2", p33, TYPE_FRDMK64F_FTM);
	sysbus_realize(SYS_BUS_DEVICE(p33), &error_fatal);
	p33->base = 0x4003a000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p33), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 44));
	sysbus_mmio_map(SYS_BUS_DEVICE(p33), 0, 0x4003a000);

	FRDMK64FFTM *p34 = g_new(FRDMK64FFTM, 1);
	object_initialize_child(OBJECT(sms), "FTM3", p34, TYPE_FRDMK64F_FTM);
	sysbus_realize(SYS_BUS_DEVICE(p34), &error_fatal);
	p34->base = 0x400b9000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p34), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 71));
	sysbus_mmio_map(SYS_BUS_DEVICE(p34), 0, 0x400b9000);

	FRDMK64FPIT *p35 = g_new(FRDMK64FPIT, 1);
	object_initialize_child(OBJECT(sms), "PIT", p35, TYPE_FRDMK64F_PIT);
	sysbus_realize(SYS_BUS_DEVICE(p35), &error_fatal);
	p35->base = 0x40037000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p35), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 48));
	sysbus_connect_irq(SYS_BUS_DEVICE(p35), 1, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 49));
	sysbus_connect_irq(SYS_BUS_DEVICE(p35), 2, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 50));
	sysbus_connect_irq(SYS_BUS_DEVICE(p35), 3, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 51));
	sysbus_mmio_map(SYS_BUS_DEVICE(p35), 0, 0x40037000);


}

static void nxp_frdm_k64f_common_init(MachineState *machine) {
	NXP_FRDM_K64FMachineState *sms = NXP_FRDM_K64F_MACHINE(machine);
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

	nxp_frdm_k64f_periph_init(machine);

	armv7m_load_kernel(ARM_CPU(first_cpu), machine->kernel_filename, 0x0, 0x8000000);
}

static void nxp_frdm_k64f_class_init(ObjectClass *oc, void *data) {
	MachineClass *mc = MACHINE_CLASS(oc);
	mc->desc = "nxp_frdm_k64f";
	mc->init = nxp_frdm_k64f_common_init;
	mc->max_cpus = 1;
	mc->default_cpus = 1;
	mc->default_cpu_type = ARM_CPU_TYPE_NAME("cortex-m4");
}

static const TypeInfo nxp_frdm_k64f_info = {
	.name = TYPE_NXP_FRDM_K64F_MACHINE,
	.parent = TYPE_MACHINE,
	.instance_size = sizeof(NXP_FRDM_K64FMachineState),
	.class_size = sizeof(NXP_FRDM_K64FMachineClass),
	.class_init = nxp_frdm_k64f_class_init
};

static void nxp_frdm_k64f_machine_init(void) {
	type_register_static(&nxp_frdm_k64f_info);
}

type_init(nxp_frdm_k64f_machine_init);
