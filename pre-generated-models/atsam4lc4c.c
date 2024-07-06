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


#define TYPE_ATSAM4LC4C_USART "atsam4lc4c-usart"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAM4LC4CUSART, ATSAM4LC4C_USART)

struct ATSAM4LC4CUSART {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t BRGR;
	uint32_t CR;
	uint32_t CSR;
	uint32_t FIDI;
	uint32_t IDR;
	uint32_t IER;
	uint32_t IFR;
	uint32_t IMR;
	uint32_t LINBRR;
	uint32_t LINIR;
	uint32_t LINMR;
	uint32_t MAN;
	uint32_t MR;
	uint32_t NER;
	uint32_t RHR;
	uint32_t RTOR;
	uint32_t THR;
	uint32_t TTGR;
	uint32_t VERSION;
	uint32_t WPMR;
	uint32_t WPSR;
	/* chardev backend */
	CharBackend chr;
	guint watch_tag;

	/* base */
	uint32_t base;

};
#define ATSAM4LC4C_USART_SIZE				0x100

REG32(USART_BRGR, 0x20)
	FIELD(USART_BRGR, CD, 0, 16)
	FIELD(USART_BRGR, FP, 16, 3)
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
	FIELD(USART_CR, LINABT, 20, 1)
	FIELD(USART_CR, LINWKUP, 21, 1)
	FIELD(USART_CR, FCS, 18, 1)
	FIELD(USART_CR, RCS, 19, 1)
REG32(USART_CSR, 0x14)
	FIELD(USART_CSR, RXRDY, 0, 1)
	FIELD(USART_CSR, TXRDY, 1, 1)
	FIELD(USART_CSR, RXBRK, 2, 1)
	FIELD(USART_CSR, OVRE, 5, 1)
	FIELD(USART_CSR, FRAME, 6, 1)
	FIELD(USART_CSR, PARE, 7, 1)
	FIELD(USART_CSR, TIMEOUT, 8, 1)
	FIELD(USART_CSR, TXEMPTY, 9, 1)
	FIELD(USART_CSR, ITER, 10, 1)
	FIELD(USART_CSR, TXBUFE, 11, 1)
	FIELD(USART_CSR, RXBUFF, 12, 1)
	FIELD(USART_CSR, NACK, 13, 1)
	FIELD(USART_CSR, LINID, 14, 1)
	FIELD(USART_CSR, LINTC, 15, 1)
	FIELD(USART_CSR, RIIC, 16, 1)
	FIELD(USART_CSR, DSRIC, 17, 1)
	FIELD(USART_CSR, DCDIC, 18, 1)
	FIELD(USART_CSR, CTSIC, 19, 1)
	FIELD(USART_CSR, RI, 20, 1)
	FIELD(USART_CSR, DSR, 21, 1)
	FIELD(USART_CSR, DCD, 22, 1)
	FIELD(USART_CSR, LINBLS, 23, 1)
	FIELD(USART_CSR, LINBE, 25, 1)
	FIELD(USART_CSR, LINISFE, 26, 1)
	FIELD(USART_CSR, LINIPE, 27, 1)
	FIELD(USART_CSR, LINCE, 28, 1)
	FIELD(USART_CSR, LINSNRE, 29, 1)
	FIELD(USART_CSR, LINSTE, 30, 1)
	FIELD(USART_CSR, LINHTE, 31, 1)
	FIELD(USART_CSR, UNRE, 10, 1)
	FIELD(USART_CSR, CTS, 23, 1)
	FIELD(USART_CSR, MANERR, 24, 1)
REG32(USART_FIDI, 0x40)
	FIELD(USART_FIDI, FI_DI_RATIO, 0, 11)
REG32(USART_IDR, 0xc)
	FIELD(USART_IDR, RXRDY, 0, 1)
	FIELD(USART_IDR, TXRDY, 1, 1)
	FIELD(USART_IDR, RXBRK, 2, 1)
	FIELD(USART_IDR, OVRE, 5, 1)
	FIELD(USART_IDR, FRAME, 6, 1)
	FIELD(USART_IDR, PARE, 7, 1)
	FIELD(USART_IDR, TIMEOUT, 8, 1)
	FIELD(USART_IDR, TXEMPTY, 9, 1)
	FIELD(USART_IDR, ITER, 10, 1)
	FIELD(USART_IDR, TXBUFE, 11, 1)
	FIELD(USART_IDR, RXBUFF, 12, 1)
	FIELD(USART_IDR, NACK, 13, 1)
	FIELD(USART_IDR, LINID, 14, 1)
	FIELD(USART_IDR, LINTC, 15, 1)
	FIELD(USART_IDR, RIIC, 16, 1)
	FIELD(USART_IDR, DSRIC, 17, 1)
	FIELD(USART_IDR, DCDIC, 18, 1)
	FIELD(USART_IDR, CTSIC, 19, 1)
	FIELD(USART_IDR, LINBE, 25, 1)
	FIELD(USART_IDR, LINISFE, 26, 1)
	FIELD(USART_IDR, LINIPE, 27, 1)
	FIELD(USART_IDR, LINCE, 28, 1)
	FIELD(USART_IDR, LINSNRE, 29, 1)
	FIELD(USART_IDR, LINSTE, 30, 1)
	FIELD(USART_IDR, LINHTE, 31, 1)
	FIELD(USART_IDR, UNRE, 10, 1)
	FIELD(USART_IDR, MANE, 20, 1)
	FIELD(USART_IDR, MANEA, 24, 1)
REG32(USART_IER, 0x8)
	FIELD(USART_IER, RXRDY, 0, 1)
	FIELD(USART_IER, TXRDY, 1, 1)
	FIELD(USART_IER, RXBRK, 2, 1)
	FIELD(USART_IER, OVRE, 5, 1)
	FIELD(USART_IER, FRAME, 6, 1)
	FIELD(USART_IER, PARE, 7, 1)
	FIELD(USART_IER, TIMEOUT, 8, 1)
	FIELD(USART_IER, TXEMPTY, 9, 1)
	FIELD(USART_IER, ITER, 10, 1)
	FIELD(USART_IER, TXBUFE, 11, 1)
	FIELD(USART_IER, RXBUFF, 12, 1)
	FIELD(USART_IER, NACK, 13, 1)
	FIELD(USART_IER, LINID, 14, 1)
	FIELD(USART_IER, LINTC, 15, 1)
	FIELD(USART_IER, RIIC, 16, 1)
	FIELD(USART_IER, DSRIC, 17, 1)
	FIELD(USART_IER, DCDIC, 18, 1)
	FIELD(USART_IER, CTSIC, 19, 1)
	FIELD(USART_IER, LINBE, 25, 1)
	FIELD(USART_IER, LINISFE, 26, 1)
	FIELD(USART_IER, LINIPE, 27, 1)
	FIELD(USART_IER, LINCE, 28, 1)
	FIELD(USART_IER, LINSNRE, 29, 1)
	FIELD(USART_IER, LINSTE, 30, 1)
	FIELD(USART_IER, LINHTE, 31, 1)
	FIELD(USART_IER, UNRE, 10, 1)
	FIELD(USART_IER, MANE, 20, 1)
	FIELD(USART_IER, MANEA, 24, 1)
REG32(USART_IFR, 0x4c)
	FIELD(USART_IFR, IRDA_FILTER, 0, 8)
REG32(USART_IMR, 0x10)
	FIELD(USART_IMR, RXRDY, 0, 1)
	FIELD(USART_IMR, TXRDY, 1, 1)
	FIELD(USART_IMR, RXBRK, 2, 1)
	FIELD(USART_IMR, OVRE, 5, 1)
	FIELD(USART_IMR, FRAME, 6, 1)
	FIELD(USART_IMR, PARE, 7, 1)
	FIELD(USART_IMR, TIMEOUT, 8, 1)
	FIELD(USART_IMR, TXEMPTY, 9, 1)
	FIELD(USART_IMR, ITER, 10, 1)
	FIELD(USART_IMR, TXBUFE, 11, 1)
	FIELD(USART_IMR, RXBUFF, 12, 1)
	FIELD(USART_IMR, NACK, 13, 1)
	FIELD(USART_IMR, LINID, 14, 1)
	FIELD(USART_IMR, LINTC, 15, 1)
	FIELD(USART_IMR, RIIC, 16, 1)
	FIELD(USART_IMR, DSRIC, 17, 1)
	FIELD(USART_IMR, DCDIC, 18, 1)
	FIELD(USART_IMR, CTSIC, 19, 1)
	FIELD(USART_IMR, LINBE, 25, 1)
	FIELD(USART_IMR, LINISFE, 26, 1)
	FIELD(USART_IMR, LINIPE, 27, 1)
	FIELD(USART_IMR, LINCE, 28, 1)
	FIELD(USART_IMR, LINSNRE, 29, 1)
	FIELD(USART_IMR, LINSTE, 30, 1)
	FIELD(USART_IMR, LINHTE, 31, 1)
	FIELD(USART_IMR, UNRE, 10, 1)
	FIELD(USART_IMR, MANE, 20, 1)
	FIELD(USART_IMR, MANEA, 24, 1)
REG32(USART_LINBRR, 0x5c)
	FIELD(USART_LINBRR, LINCD, 0, 16)
	FIELD(USART_LINBRR, LINFP, 16, 3)
REG32(USART_LINIR, 0x58)
	FIELD(USART_LINIR, IDCHR, 0, 8)
REG32(USART_LINMR, 0x54)
	FIELD(USART_LINMR, NACT, 0, 2)
	FIELD(USART_LINMR, PARDIS, 2, 1)
	FIELD(USART_LINMR, CHKDIS, 3, 1)
	FIELD(USART_LINMR, CHKTYP, 4, 1)
	FIELD(USART_LINMR, DLM, 5, 1)
	FIELD(USART_LINMR, FSDIS, 6, 1)
	FIELD(USART_LINMR, WKUPTYP, 7, 1)
	FIELD(USART_LINMR, DLC, 8, 8)
	FIELD(USART_LINMR, PDCM, 16, 1)
	FIELD(USART_LINMR, SYNCDIS, 17, 1)
REG32(USART_MAN, 0x50)
	FIELD(USART_MAN, TX_PL, 0, 4)
	FIELD(USART_MAN, TX_PP, 8, 2)
	FIELD(USART_MAN, TX_MPOL, 12, 1)
	FIELD(USART_MAN, RX_PL, 16, 4)
	FIELD(USART_MAN, RX_PP, 24, 2)
	FIELD(USART_MAN, RX_MPOL, 28, 1)
	FIELD(USART_MAN, DRIFT, 30, 1)
REG32(USART_MR, 0x4)
	FIELD(USART_MR, MODE, 0, 4)
	FIELD(USART_MR, USCLKS, 4, 2)
	FIELD(USART_MR, CHRL, 6, 2)
	FIELD(USART_MR, CPHA, 8, 1)
	FIELD(USART_MR, PAR, 9, 3)
	FIELD(USART_MR, NBSTOP, 12, 2)
	FIELD(USART_MR, CHMODE, 14, 2)
	FIELD(USART_MR, CPOL, 16, 1)
	FIELD(USART_MR, MODE9, 17, 1)
	FIELD(USART_MR, CLKO, 18, 1)
	FIELD(USART_MR, OVER, 19, 1)
	FIELD(USART_MR, INACK, 20, 1)
	FIELD(USART_MR, DSNACK, 21, 1)
	FIELD(USART_MR, INVDATA, 23, 1)
	FIELD(USART_MR, MAX_ITERATION, 24, 3)
	FIELD(USART_MR, FILTER, 28, 1)
	FIELD(USART_MR, SYNC, 8, 1)
	FIELD(USART_MR, MSBF, 16, 1)
	FIELD(USART_MR, VAR_SYNC, 22, 1)
	FIELD(USART_MR, MAN, 29, 1)
	FIELD(USART_MR, MODSYNC, 30, 1)
	FIELD(USART_MR, ONEBIT, 31, 1)
REG32(USART_NER, 0x44)
	FIELD(USART_NER, NB_ERRORS, 0, 8)
REG32(USART_RHR, 0x18)
	FIELD(USART_RHR, RXCHR, 0, 9)
	FIELD(USART_RHR, RXSYNH, 15, 1)
REG32(USART_RTOR, 0x24)
	FIELD(USART_RTOR, TO, 0, 17)
REG32(USART_THR, 0x1c)
	FIELD(USART_THR, TXCHR, 0, 9)
	FIELD(USART_THR, TXSYNH, 15, 1)
REG32(USART_TTGR, 0x28)
	FIELD(USART_TTGR, TG, 0, 8)
REG32(USART_VERSION, 0xfc)
	FIELD(USART_VERSION, VERSION, 0, 12)
	FIELD(USART_VERSION, MFN, 16, 4)
REG32(USART_WPMR, 0xe4)
	FIELD(USART_WPMR, WPEN, 0, 1)
	FIELD(USART_WPMR, WPKEY, 8, 24)
REG32(USART_WPSR, 0xe8)
	FIELD(USART_WPSR, WPV, 0, 1)
	FIELD(USART_WPSR, WPVSRC, 8, 16)


__attribute__((unused))
static void atsam4lc4c_usart_update(ATSAM4LC4CUSART *t) {
	int conditions = (((t->IMR & 0x1) && (t->CSR & 0x1)) || ((t->IMR & 0x2) && (t->CSR & 0x2)) || ((t->CSR & 0x200) && (t->IMR & 0x200)));
	qemu_set_irq(t->irq[0], conditions);

}

static int atsam4lc4c_usart_can_receive(void *opaque) {
	return 1;
}

static void atsam4lc4c_usart_receive(void *opaque, const uint8_t *buf, int size) {
	ATSAM4LC4CUSART *t = ATSAM4LC4C_USART(opaque);

	t->RHR = *buf;
	t->CSR |= 0x1;

	atsam4lc4c_usart_update(t);
}

static gboolean atsam4lc4c_usart_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	ATSAM4LC4CUSART *t = ATSAM4LC4C_USART(opaque);
	int ret;

	t->watch_tag = 0;
	t->CSR &= (~(0x2));

	atsam4lc4c_usart_update(t);

	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->THR), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, atsam4lc4c_usart_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->CSR |= 0x2;

	atsam4lc4c_usart_update(t);

	return FALSE;
}

static void atsam4lc4c_usart_register_reset(ATSAM4LC4CUSART *t) {
	t->FIDI = 0x174;
	t->MAN = 0x30011004;
	t->VERSION = 0x602;
	t->CSR |= 0x2;

}

static uint64_t atsam4lc4c_usart_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAM4LC4CUSART *t = ATSAM4LC4C_USART(opaque);
	uint64_t ret;

	switch (offset) {
		case A_USART_BRGR:
			ret = t->BRGR;
			break;
		case A_USART_CSR:
			ret = t->CSR;
			break;
		case A_USART_FIDI:
			ret = t->FIDI;
			break;
		case A_USART_IFR:
			ret = t->IFR;
			break;
		case A_USART_IMR:
			ret = t->IMR;
			break;
		case A_USART_LINBRR:
			ret = t->LINBRR;
			break;
		case A_USART_LINIR:
			ret = t->LINIR;
			break;
		case A_USART_LINMR:
			ret = t->LINMR;
			break;
		case A_USART_MAN:
			ret = t->MAN;
			break;
		case A_USART_MR:
			ret = t->MR;
			break;
		case A_USART_NER:
			ret = t->NER;
			break;
		case A_USART_RHR:
			ret = t->RHR;
			t->CSR &= (~(0x1));
			qemu_chr_fe_accept_input(&(t->chr));
			atsam4lc4c_usart_update(t);
			break;
		case A_USART_RTOR:
			ret = t->RTOR;
			break;
		case A_USART_TTGR:
			ret = t->TTGR;
			break;
		case A_USART_VERSION:
			ret = t->VERSION;
			break;
		case A_USART_WPMR:
			ret = t->WPMR;
			break;
		case A_USART_WPSR:
			ret = t->WPSR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM4LC4C USART read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsam4lc4c_usart_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAM4LC4CUSART *t = ATSAM4LC4C_USART(opaque);


	switch (offset) {
		case A_USART_BRGR ... A_USART_BRGR + 3:
			value = value << ((offset - A_USART_BRGR) << 3);
			offset = A_USART_BRGR;
			break;
		case A_USART_CR ... A_USART_CR + 3:
			value = value << ((offset - A_USART_CR) << 3);
			offset = A_USART_CR;
			break;
		case A_USART_FIDI ... A_USART_FIDI + 3:
			value = value << ((offset - A_USART_FIDI) << 3);
			offset = A_USART_FIDI;
			break;
		case A_USART_IDR ... A_USART_IDR + 3:
			value = value << ((offset - A_USART_IDR) << 3);
			offset = A_USART_IDR;
			break;
		case A_USART_IER ... A_USART_IER + 3:
			value = value << ((offset - A_USART_IER) << 3);
			offset = A_USART_IER;
			break;
		case A_USART_IFR ... A_USART_IFR + 3:
			value = value << ((offset - A_USART_IFR) << 3);
			offset = A_USART_IFR;
			break;
		case A_USART_LINIR ... A_USART_LINIR + 3:
			value = value << ((offset - A_USART_LINIR) << 3);
			offset = A_USART_LINIR;
			break;
		case A_USART_LINMR ... A_USART_LINMR + 3:
			value = value << ((offset - A_USART_LINMR) << 3);
			offset = A_USART_LINMR;
			break;
		case A_USART_MAN ... A_USART_MAN + 3:
			value = value << ((offset - A_USART_MAN) << 3);
			offset = A_USART_MAN;
			break;
		case A_USART_MR ... A_USART_MR + 3:
			value = value << ((offset - A_USART_MR) << 3);
			offset = A_USART_MR;
			break;
		case A_USART_RTOR ... A_USART_RTOR + 3:
			value = value << ((offset - A_USART_RTOR) << 3);
			offset = A_USART_RTOR;
			break;
		case A_USART_THR ... A_USART_THR + 3:
			value = value << ((offset - A_USART_THR) << 3);
			offset = A_USART_THR;
			break;
		case A_USART_TTGR ... A_USART_TTGR + 3:
			value = value << ((offset - A_USART_TTGR) << 3);
			offset = A_USART_TTGR;
			break;
		case A_USART_WPMR ... A_USART_WPMR + 3:
			value = value << ((offset - A_USART_WPMR) << 3);
			offset = A_USART_WPMR;
			break;

		default: break;
	}

	switch (offset) {
		case A_USART_BRGR:
			t->BRGR = value;
			break;
		case A_USART_CR:
			t->CR = value;
			break;
		case A_USART_FIDI:
			t->FIDI = value;
			break;
		case A_USART_IDR:
			t->IDR = value;
			break;
		case A_USART_IER:
			t->IER = value;
			break;
		case A_USART_IFR:
			t->IFR = value;
			break;
		case A_USART_LINIR:
			t->LINIR = value;
			break;
		case A_USART_LINMR:
			t->LINMR = value;
			break;
		case A_USART_MAN:
			t->MAN = value;
			break;
		case A_USART_MR:
			t->MR = value;
			break;
		case A_USART_RTOR:
			t->RTOR = value;
			break;
		case A_USART_THR:
			t->THR = value;
			atsam4lc4c_usart_transmit(NULL, G_IO_OUT, t);
			if (((!(value & 0x100)) && (!(value & 0x200)) && (!(value & 0x400)) && (!(value & 0x800)) && (!(value & 0x1000)) && (!(value & 0x2000)) && (!(value & 0x4000)) && (!(value & 0x8000)) && (!(value & 0x10000)) && (!(value & 0x20000)) && (!(value & 0x40000)) && (!(value & 0x80000)) && (!(value & 0x100000)) && (!(value & 0x200000)) && (!(value & 0x400000)) && (!(value & 0x800000)) && (!(value & 0x1000000)) && (!(value & 0x2000000)) && (!(value & 0x4000000)) && (!(value & 0x8000000)) && (!(value & 0x10000000)) && (!(value & 0x20000000)) && (!(value & 0x40000000)) && (!(value & 0x80000000)))) {
				t->CSR |= 0x2;
			}
			atsam4lc4c_usart_update(t);
			break;
		case A_USART_TTGR:
			t->TTGR = value;
			break;
		case A_USART_WPMR:
			t->WPMR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM4LC4C USART write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsam4lc4c_usart_ops = {
	.read = atsam4lc4c_usart_read,
	.write = atsam4lc4c_usart_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsam4lc4c_usart_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAM4LC4CUSART *t = ATSAM4LC4C_USART(obj);
	memory_region_init_io(&(t->iomem), obj, &atsam4lc4c_usart_ops, t, "atsam4lc4c-usart", ATSAM4LC4C_USART_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void atsam4lc4c_usart_realize(DeviceState *dev, Error **errp) {
	ATSAM4LC4CUSART *t = ATSAM4LC4C_USART(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), atsam4lc4c_usart_can_receive, atsam4lc4c_usart_receive,
		NULL, NULL, t, NULL, true);

}

static void atsam4lc4c_usart_reset_enter(Object *obj, ResetType type) {
	ATSAM4LC4CUSART *t = ATSAM4LC4C_USART(obj);
	atsam4lc4c_usart_register_reset(t);
}

static const VMStateDescription atsam4lc4c_usart_vmstate = {
	.name = "atsam4lc4c-usart",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(BRGR, ATSAM4LC4CUSART),
		VMSTATE_UINT32(CR, ATSAM4LC4CUSART),
		VMSTATE_UINT32(CR, ATSAM4LC4CUSART),
		VMSTATE_UINT32(CR, ATSAM4LC4CUSART),
		VMSTATE_UINT32(CSR, ATSAM4LC4CUSART),
		VMSTATE_UINT32(CSR, ATSAM4LC4CUSART),
		VMSTATE_UINT32(CSR, ATSAM4LC4CUSART),
		VMSTATE_UINT32(FIDI, ATSAM4LC4CUSART),
		VMSTATE_UINT32(IDR, ATSAM4LC4CUSART),
		VMSTATE_UINT32(IDR, ATSAM4LC4CUSART),
		VMSTATE_UINT32(IDR, ATSAM4LC4CUSART),
		VMSTATE_UINT32(IER, ATSAM4LC4CUSART),
		VMSTATE_UINT32(IER, ATSAM4LC4CUSART),
		VMSTATE_UINT32(IER, ATSAM4LC4CUSART),
		VMSTATE_UINT32(IFR, ATSAM4LC4CUSART),
		VMSTATE_UINT32(IMR, ATSAM4LC4CUSART),
		VMSTATE_UINT32(IMR, ATSAM4LC4CUSART),
		VMSTATE_UINT32(IMR, ATSAM4LC4CUSART),
		VMSTATE_UINT32(LINBRR, ATSAM4LC4CUSART),
		VMSTATE_UINT32(LINIR, ATSAM4LC4CUSART),
		VMSTATE_UINT32(LINMR, ATSAM4LC4CUSART),
		VMSTATE_UINT32(MAN, ATSAM4LC4CUSART),
		VMSTATE_UINT32(MR, ATSAM4LC4CUSART),
		VMSTATE_UINT32(MR, ATSAM4LC4CUSART),
		VMSTATE_UINT32(NER, ATSAM4LC4CUSART),
		VMSTATE_UINT32(RHR, ATSAM4LC4CUSART),
		VMSTATE_UINT32(RTOR, ATSAM4LC4CUSART),
		VMSTATE_UINT32(THR, ATSAM4LC4CUSART),
		VMSTATE_UINT32(TTGR, ATSAM4LC4CUSART),
		VMSTATE_UINT32(VERSION, ATSAM4LC4CUSART),
		VMSTATE_UINT32(WPMR, ATSAM4LC4CUSART),
		VMSTATE_UINT32(WPSR, ATSAM4LC4CUSART),

		VMSTATE_END_OF_LIST()
	}
};

static Property atsam4lc4c_usart_properties[] = {
	DEFINE_PROP_CHR("chardev", ATSAM4LC4CUSART, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void atsam4lc4c_usart_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsam4lc4c_usart_vmstate;
	dc->realize = atsam4lc4c_usart_realize;
	rc->phases.enter = atsam4lc4c_usart_reset_enter;
	device_class_set_props(dc, atsam4lc4c_usart_properties);

}

static const TypeInfo atsam4lc4c_usart_info = {
	.name = TYPE_ATSAM4LC4C_USART,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAM4LC4CUSART),
	.instance_init = atsam4lc4c_usart_init,
	.class_init = atsam4lc4c_usart_class_init,
};

static void atsam4lc4c_usart_register_types(void) {
	type_register_static(&atsam4lc4c_usart_info);
}

type_init(atsam4lc4c_usart_register_types);

#define TYPE_ATSAM4LC4C_HCACHE "atsam4lc4c-hcache"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAM4LC4CHCACHE, ATSAM4LC4C_HCACHE)

struct ATSAM4LC4CHCACHE {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/*registers*/
	uint32_t CTRL;
	uint32_t MAINT0;
	uint32_t MAINT1;
	uint32_t MCFG;
	uint32_t MCTRL;
	uint32_t MEN;
	uint32_t MSR;
	uint32_t SR;
	uint32_t VERSION;
	/* base */
	uint32_t base;

};
#define ATSAM4LC4C_HCACHE_SIZE				0x100

REG32(HCACHE_CTRL, 0x8)
	FIELD(HCACHE_CTRL, CEN, 0, 1)
REG32(HCACHE_MAINT0, 0x20)
	FIELD(HCACHE_MAINT0, INVALL, 0, 1)
REG32(HCACHE_MAINT1, 0x24)
	FIELD(HCACHE_MAINT1, INDEX, 4, 4)
REG32(HCACHE_MCFG, 0x28)
	FIELD(HCACHE_MCFG, MODE, 0, 2)
REG32(HCACHE_MCTRL, 0x30)
	FIELD(HCACHE_MCTRL, SWRST, 0, 1)
REG32(HCACHE_MEN, 0x2c)
	FIELD(HCACHE_MEN, MENABLE, 0, 1)
REG32(HCACHE_MSR, 0x34)
	FIELD(HCACHE_MSR, EVENTCNT, 0, 32)
REG32(HCACHE_SR, 0xc)
	FIELD(HCACHE_SR, CSTS, 0, 1)
REG32(HCACHE_VERSION, 0xfc)
	FIELD(HCACHE_VERSION, VERSION, 0, 12)
	FIELD(HCACHE_VERSION, MFN, 16, 4)


static void atsam4lc4c_hcache_register_reset(ATSAM4LC4CHCACHE *t) {
	t->VERSION = 0x101;

}

static uint64_t atsam4lc4c_hcache_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAM4LC4CHCACHE *t = ATSAM4LC4C_HCACHE(opaque);
	uint64_t ret;

	switch (offset) {
		case A_HCACHE_MCFG:
			ret = t->MCFG;
			break;
		case A_HCACHE_MSR:
			ret = t->MSR;
			break;
		case A_HCACHE_SR:
			ret = t->SR;
			break;
		case A_HCACHE_VERSION:
			ret = t->VERSION;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM4LC4C HCACHE read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsam4lc4c_hcache_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAM4LC4CHCACHE *t = ATSAM4LC4C_HCACHE(opaque);


	switch (offset) {
		case A_HCACHE_CTRL ... A_HCACHE_CTRL + 3:
			value = value << ((offset - A_HCACHE_CTRL) << 3);
			offset = A_HCACHE_CTRL;
			break;
		case A_HCACHE_MAINT0 ... A_HCACHE_MAINT0 + 3:
			value = value << ((offset - A_HCACHE_MAINT0) << 3);
			offset = A_HCACHE_MAINT0;
			break;
		case A_HCACHE_MAINT1 ... A_HCACHE_MAINT1 + 3:
			value = value << ((offset - A_HCACHE_MAINT1) << 3);
			offset = A_HCACHE_MAINT1;
			break;
		case A_HCACHE_MCFG ... A_HCACHE_MCFG + 3:
			value = value << ((offset - A_HCACHE_MCFG) << 3);
			offset = A_HCACHE_MCFG;
			break;
		case A_HCACHE_MCTRL ... A_HCACHE_MCTRL + 3:
			value = value << ((offset - A_HCACHE_MCTRL) << 3);
			offset = A_HCACHE_MCTRL;
			break;
		case A_HCACHE_MEN ... A_HCACHE_MEN + 3:
			value = value << ((offset - A_HCACHE_MEN) << 3);
			offset = A_HCACHE_MEN;
			break;

		default: break;
	}

	switch (offset) {
		case A_HCACHE_CTRL:
			t->CTRL = value;
			break;
		case A_HCACHE_MAINT0:
			t->MAINT0 = value;
			break;
		case A_HCACHE_MAINT1:
			t->MAINT1 = value;
			break;
		case A_HCACHE_MCFG:
			t->MCFG = value;
			break;
		case A_HCACHE_MCTRL:
			t->MCTRL = value;
			break;
		case A_HCACHE_MEN:
			t->MEN = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM4LC4C HCACHE write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsam4lc4c_hcache_ops = {
	.read = atsam4lc4c_hcache_read,
	.write = atsam4lc4c_hcache_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsam4lc4c_hcache_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAM4LC4CHCACHE *t = ATSAM4LC4C_HCACHE(obj);
	memory_region_init_io(&(t->iomem), obj, &atsam4lc4c_hcache_ops, t, "atsam4lc4c-hcache", ATSAM4LC4C_HCACHE_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void atsam4lc4c_hcache_realize(DeviceState *dev, Error **errp) {
	return;

}

static void atsam4lc4c_hcache_reset_enter(Object *obj, ResetType type) {
	ATSAM4LC4CHCACHE *t = ATSAM4LC4C_HCACHE(obj);
	atsam4lc4c_hcache_register_reset(t);
}

static const VMStateDescription atsam4lc4c_hcache_vmstate = {
	.name = "atsam4lc4c-hcache",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CTRL, ATSAM4LC4CHCACHE),
		VMSTATE_UINT32(MAINT0, ATSAM4LC4CHCACHE),
		VMSTATE_UINT32(MAINT1, ATSAM4LC4CHCACHE),
		VMSTATE_UINT32(MCFG, ATSAM4LC4CHCACHE),
		VMSTATE_UINT32(MCTRL, ATSAM4LC4CHCACHE),
		VMSTATE_UINT32(MEN, ATSAM4LC4CHCACHE),
		VMSTATE_UINT32(MSR, ATSAM4LC4CHCACHE),
		VMSTATE_UINT32(SR, ATSAM4LC4CHCACHE),
		VMSTATE_UINT32(VERSION, ATSAM4LC4CHCACHE),

		VMSTATE_END_OF_LIST()
	}
};

static void atsam4lc4c_hcache_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsam4lc4c_hcache_vmstate;
	dc->realize = atsam4lc4c_hcache_realize;
	rc->phases.enter = atsam4lc4c_hcache_reset_enter;

}

static const TypeInfo atsam4lc4c_hcache_info = {
	.name = TYPE_ATSAM4LC4C_HCACHE,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAM4LC4CHCACHE),
	.instance_init = atsam4lc4c_hcache_init,
	.class_init = atsam4lc4c_hcache_class_init,
};

static void atsam4lc4c_hcache_register_types(void) {
	type_register_static(&atsam4lc4c_hcache_info);
}

type_init(atsam4lc4c_hcache_register_types);

#define TYPE_ATSAM4LC4C_SCIF "atsam4lc4c-scif"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAM4LC4CSCIF, ATSAM4LC4C_SCIF)

struct ATSAM4LC4CSCIF {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t CSCR;
	uint32_t DFLLIFBVERSION;
	uint32_t DFLL0CONF;
	uint32_t DFLL0MUL;
	uint32_t DFLL0RATIO;
	uint32_t DFLL0SSG;
	uint32_t DFLL0STEP;
	uint32_t DFLL0SYNC;
	uint32_t DFLL0VAL;
	uint32_t FLOVERSION;
	uint32_t FPCR;
	uint32_t FPDIV;
	uint32_t FPMUL;
	uint32_t GCLKIFVERSION;
	uint32_t GCLKPRESCVERSION;
	uint32_t HRPCR;
	uint32_t ICR;
	uint32_t IDR;
	uint32_t IER;
	uint32_t IMR;
	uint32_t ISR;
	uint32_t OSCCTRL0;
	uint32_t OSCIFAVERSION;
	uint32_t PCLKSR;
	uint32_t PLLIFAVERSION;
	uint32_t PLL;
	uint32_t RCCR;
	uint32_t RCFASTCFG;
	uint32_t RCFASTSR;
	uint32_t RCFASTVERSION;
	uint32_t RCOSCIFAVERSION;
	uint32_t RC80MCR;
	uint32_t RC80MVERSION;
	uint32_t UNLOCK;
	uint32_t VERSION;
	uint32_t GCCTRL0;
	uint32_t GCCTRL1;
	uint32_t GCCTRL2;
	uint32_t GCCTRL3;
	uint32_t GCCTRL4;
	uint32_t GCCTRL5;
	uint32_t GCCTRL6;
	uint32_t GCCTRL7;
	uint32_t GCCTRL8;
	uint32_t GCCTRL9;
	uint32_t GCCTRL10;
	uint32_t GCCTRL11;
	/* base */
	uint32_t base;

};
#define ATSAM4LC4C_SCIF_SIZE				0x400

REG32(SCIF_CSCR, 0x1c)
REG32(SCIF_DFLLIFBVERSION, 0x3e8)
	FIELD(SCIF_DFLLIFBVERSION, VERSION, 0, 12)
	FIELD(SCIF_DFLLIFBVERSION, VARIANT, 16, 4)
REG32(SCIF_DFLL0CONF, 0x28)
	FIELD(SCIF_DFLL0CONF, EN, 0, 1)
	FIELD(SCIF_DFLL0CONF, MODE, 1, 1)
	FIELD(SCIF_DFLL0CONF, STABLE, 2, 1)
	FIELD(SCIF_DFLL0CONF, LLAW, 3, 1)
	FIELD(SCIF_DFLL0CONF, CCDIS, 5, 1)
	FIELD(SCIF_DFLL0CONF, QLDIS, 6, 1)
	FIELD(SCIF_DFLL0CONF, RANGE, 16, 2)
	FIELD(SCIF_DFLL0CONF, FCD, 23, 1)
	FIELD(SCIF_DFLL0CONF, CALIB, 24, 4)
REG32(SCIF_DFLL0MUL, 0x30)
	FIELD(SCIF_DFLL0MUL, MUL, 0, 16)
REG32(SCIF_DFLL0RATIO, 0x3c)
	FIELD(SCIF_DFLL0RATIO, RATIODIFF, 0, 16)
REG32(SCIF_DFLL0SSG, 0x38)
	FIELD(SCIF_DFLL0SSG, EN, 0, 1)
	FIELD(SCIF_DFLL0SSG, PRBS, 1, 1)
	FIELD(SCIF_DFLL0SSG, AMPLITUDE, 8, 5)
	FIELD(SCIF_DFLL0SSG, STEPSIZE, 16, 5)
REG32(SCIF_DFLL0STEP, 0x34)
	FIELD(SCIF_DFLL0STEP, FSTEP, 0, 8)
	FIELD(SCIF_DFLL0STEP, CSTEP, 16, 5)
REG32(SCIF_DFLL0SYNC, 0x40)
	FIELD(SCIF_DFLL0SYNC, SYNC, 0, 1)
REG32(SCIF_DFLL0VAL, 0x2c)
	FIELD(SCIF_DFLL0VAL, FINE, 0, 8)
	FIELD(SCIF_DFLL0VAL, COARSE, 16, 5)
REG32(SCIF_FLOVERSION, 0x3f0)
	FIELD(SCIF_FLOVERSION, VERSION, 0, 12)
	FIELD(SCIF_FLOVERSION, VARIANT, 16, 4)
REG32(SCIF_FPCR, 0x68)
	FIELD(SCIF_FPCR, FPEN, 0, 1)
	FIELD(SCIF_FPCR, CKSEL, 1, 3)
REG32(SCIF_FPDIV, 0x70)
	FIELD(SCIF_FPDIV, FPDIV, 0, 16)
REG32(SCIF_FPMUL, 0x6c)
	FIELD(SCIF_FPMUL, FPMUL, 0, 16)
REG32(SCIF_GCLKIFVERSION, 0x3f8)
	FIELD(SCIF_GCLKIFVERSION, VERSION, 0, 12)
	FIELD(SCIF_GCLKIFVERSION, VARIANT, 16, 4)
REG32(SCIF_GCLKPRESCVERSION, 0x3dc)
	FIELD(SCIF_GCLKPRESCVERSION, VERSION, 0, 12)
	FIELD(SCIF_GCLKPRESCVERSION, VARIANT, 16, 4)
REG32(SCIF_HRPCR, 0x64)
	FIELD(SCIF_HRPCR, HRPEN, 0, 1)
	FIELD(SCIF_HRPCR, CKSEL, 1, 3)
	FIELD(SCIF_HRPCR, HRCOUNT, 8, 24)
REG32(SCIF_ICR, 0x10)
	FIELD(SCIF_ICR, OSC0RDY, 0, 1)
	FIELD(SCIF_ICR, DFLL0LOCKC, 1, 1)
	FIELD(SCIF_ICR, DFLL0LOCKF, 2, 1)
	FIELD(SCIF_ICR, DFLL0RDY, 3, 1)
	FIELD(SCIF_ICR, DFLL0RCS, 4, 1)
	FIELD(SCIF_ICR, DFLL0OOB, 5, 1)
	FIELD(SCIF_ICR, PLL0LOCK, 6, 1)
	FIELD(SCIF_ICR, PLL0LOCKLOST, 7, 1)
	FIELD(SCIF_ICR, RCFASTLOCK, 13, 1)
	FIELD(SCIF_ICR, RCFASTLOCKLOST, 14, 1)
	FIELD(SCIF_ICR, AE, 31, 1)
REG32(SCIF_IDR, 0x4)
	FIELD(SCIF_IDR, OSC0RDY, 0, 1)
	FIELD(SCIF_IDR, DFLL0LOCKC, 1, 1)
	FIELD(SCIF_IDR, DFLL0LOCKF, 2, 1)
	FIELD(SCIF_IDR, DFLL0RDY, 3, 1)
	FIELD(SCIF_IDR, DFLL0RCS, 4, 1)
	FIELD(SCIF_IDR, DFLL0OOB, 5, 1)
	FIELD(SCIF_IDR, PLL0LOCK, 6, 1)
	FIELD(SCIF_IDR, PLL0LOCKLOST, 7, 1)
	FIELD(SCIF_IDR, RCFASTLOCK, 13, 1)
	FIELD(SCIF_IDR, RCFASTLOCKLOST, 14, 1)
	FIELD(SCIF_IDR, AE, 31, 1)
REG32(SCIF_IER, 0x0)
	FIELD(SCIF_IER, OSC0RDY, 0, 1)
	FIELD(SCIF_IER, DFLL0LOCKC, 1, 1)
	FIELD(SCIF_IER, DFLL0LOCKF, 2, 1)
	FIELD(SCIF_IER, DFLL0RDY, 3, 1)
	FIELD(SCIF_IER, DFLL0RCS, 4, 1)
	FIELD(SCIF_IER, DFLL0OOB, 5, 1)
	FIELD(SCIF_IER, PLL0LOCK, 6, 1)
	FIELD(SCIF_IER, PLL0LOCKLOST, 7, 1)
	FIELD(SCIF_IER, RCFASTLOCK, 13, 1)
	FIELD(SCIF_IER, RCFASTLOCKLOST, 14, 1)
	FIELD(SCIF_IER, AE, 31, 1)
REG32(SCIF_IMR, 0x8)
	FIELD(SCIF_IMR, OSC0RDY, 0, 1)
	FIELD(SCIF_IMR, DFLL0LOCKC, 1, 1)
	FIELD(SCIF_IMR, DFLL0LOCKF, 2, 1)
	FIELD(SCIF_IMR, DFLL0RDY, 3, 1)
	FIELD(SCIF_IMR, DFLL0RCS, 4, 1)
	FIELD(SCIF_IMR, DFLL0OOB, 5, 1)
	FIELD(SCIF_IMR, PLL0LOCK, 6, 1)
	FIELD(SCIF_IMR, PLL0LOCKLOST, 7, 1)
	FIELD(SCIF_IMR, RCFASTLOCK, 13, 1)
	FIELD(SCIF_IMR, RCFASTLOCKLOST, 14, 1)
	FIELD(SCIF_IMR, AE, 31, 1)
REG32(SCIF_ISR, 0xc)
	FIELD(SCIF_ISR, OSC0RDY, 0, 1)
	FIELD(SCIF_ISR, DFLL0LOCKC, 1, 1)
	FIELD(SCIF_ISR, DFLL0LOCKF, 2, 1)
	FIELD(SCIF_ISR, DFLL0RDY, 3, 1)
	FIELD(SCIF_ISR, DFLL0RCS, 4, 1)
	FIELD(SCIF_ISR, DFLL0OOB, 5, 1)
	FIELD(SCIF_ISR, PLL0LOCK, 6, 1)
	FIELD(SCIF_ISR, PLL0LOCKLOST, 7, 1)
	FIELD(SCIF_ISR, RCFASTLOCK, 13, 1)
	FIELD(SCIF_ISR, RCFASTLOCKLOST, 14, 1)
	FIELD(SCIF_ISR, AE, 31, 1)
REG32(SCIF_OSCCTRL0, 0x20)
	FIELD(SCIF_OSCCTRL0, MODE, 0, 1)
	FIELD(SCIF_OSCCTRL0, GAIN, 1, 2)
	FIELD(SCIF_OSCCTRL0, AGC, 3, 1)
	FIELD(SCIF_OSCCTRL0, STARTUP, 8, 4)
	FIELD(SCIF_OSCCTRL0, OSCEN, 16, 1)
REG32(SCIF_OSCIFAVERSION, 0x3e4)
	FIELD(SCIF_OSCIFAVERSION, VERSION, 0, 12)
	FIELD(SCIF_OSCIFAVERSION, VARIANT, 16, 4)
REG32(SCIF_PCLKSR, 0x14)
	FIELD(SCIF_PCLKSR, OSC0RDY, 0, 1)
	FIELD(SCIF_PCLKSR, DFLL0LOCKC, 1, 1)
	FIELD(SCIF_PCLKSR, DFLL0LOCKF, 2, 1)
	FIELD(SCIF_PCLKSR, DFLL0RDY, 3, 1)
	FIELD(SCIF_PCLKSR, DFLL0RCS, 4, 1)
	FIELD(SCIF_PCLKSR, DFLL0OOB, 5, 1)
	FIELD(SCIF_PCLKSR, PLL0LOCK, 6, 1)
	FIELD(SCIF_PCLKSR, PLL0LOCKLOST, 7, 1)
	FIELD(SCIF_PCLKSR, RCFASTLOCK, 13, 1)
	FIELD(SCIF_PCLKSR, RCFASTLOCKLOST, 14, 1)
REG32(SCIF_PLLIFAVERSION, 0x3e0)
	FIELD(SCIF_PLLIFAVERSION, VERSION, 0, 12)
	FIELD(SCIF_PLLIFAVERSION, VARIANT, 16, 4)
REG32(SCIF_PLL, 0x24)
	FIELD(SCIF_PLL, PLLEN, 0, 1)
	FIELD(SCIF_PLL, PLLOSC, 1, 2)
	FIELD(SCIF_PLL, PLLOPT, 3, 3)
	FIELD(SCIF_PLL, PLLDIV, 8, 4)
	FIELD(SCIF_PLL, PLLMUL, 16, 4)
	FIELD(SCIF_PLL, PLLCOUNT, 24, 6)
REG32(SCIF_RCCR, 0x44)
	FIELD(SCIF_RCCR, CALIB, 0, 10)
	FIELD(SCIF_RCCR, FCD, 16, 1)
REG32(SCIF_RCFASTCFG, 0x48)
	FIELD(SCIF_RCFASTCFG, EN, 0, 1)
	FIELD(SCIF_RCFASTCFG, TUNEEN, 1, 1)
	FIELD(SCIF_RCFASTCFG, JITMODE, 2, 1)
	FIELD(SCIF_RCFASTCFG, NBPERIODS, 4, 3)
	FIELD(SCIF_RCFASTCFG, FCD, 7, 1)
	FIELD(SCIF_RCFASTCFG, FRANGE, 8, 2)
	FIELD(SCIF_RCFASTCFG, LOCKMARGIN, 12, 4)
	FIELD(SCIF_RCFASTCFG, CALIB, 16, 7)
REG32(SCIF_RCFASTSR, 0x4c)
	FIELD(SCIF_RCFASTSR, CURTRIM, 0, 7)
	FIELD(SCIF_RCFASTSR, CNTERR, 16, 5)
	FIELD(SCIF_RCFASTSR, SIGN, 21, 1)
	FIELD(SCIF_RCFASTSR, LOCK, 24, 1)
	FIELD(SCIF_RCFASTSR, LOCKLOST, 25, 1)
	FIELD(SCIF_RCFASTSR, UPDATED, 31, 1)
REG32(SCIF_RCFASTVERSION, 0x3d8)
	FIELD(SCIF_RCFASTVERSION, VERSION, 0, 12)
	FIELD(SCIF_RCFASTVERSION, VARIANT, 16, 4)
REG32(SCIF_RCOSCIFAVERSION, 0x3ec)
	FIELD(SCIF_RCOSCIFAVERSION, VERSION, 0, 12)
	FIELD(SCIF_RCOSCIFAVERSION, VARIANT, 16, 4)
REG32(SCIF_RC80MCR, 0x50)
	FIELD(SCIF_RC80MCR, EN, 0, 1)
	FIELD(SCIF_RC80MCR, FCD, 7, 1)
	FIELD(SCIF_RC80MCR, CALIB, 16, 2)
REG32(SCIF_RC80MVERSION, 0x3f4)
	FIELD(SCIF_RC80MVERSION, VERSION, 0, 12)
	FIELD(SCIF_RC80MVERSION, VARIANT, 16, 4)
REG32(SCIF_UNLOCK, 0x18)
	FIELD(SCIF_UNLOCK, ADDR, 0, 10)
	FIELD(SCIF_UNLOCK, KEY, 24, 8)
REG32(SCIF_VERSION, 0x3fc)
	FIELD(SCIF_VERSION, VERSION, 0, 12)
	FIELD(SCIF_VERSION, VARIANT, 16, 4)
REG32(SCIF_GCCTRL0, 0x74)
	FIELD(SCIF_GCCTRL0, CEN, 0, 1)
	FIELD(SCIF_GCCTRL0, DIVEN, 1, 1)
	FIELD(SCIF_GCCTRL0, OSCSEL, 8, 5)
	FIELD(SCIF_GCCTRL0, DIV, 16, 16)
REG32(SCIF_GCCTRL1, 0x78)
	FIELD(SCIF_GCCTRL1, CEN, 0, 1)
	FIELD(SCIF_GCCTRL1, DIVEN, 1, 1)
	FIELD(SCIF_GCCTRL1, OSCSEL, 8, 5)
	FIELD(SCIF_GCCTRL1, DIV, 16, 16)
REG32(SCIF_GCCTRL2, 0x7c)
	FIELD(SCIF_GCCTRL2, CEN, 0, 1)
	FIELD(SCIF_GCCTRL2, DIVEN, 1, 1)
	FIELD(SCIF_GCCTRL2, OSCSEL, 8, 5)
	FIELD(SCIF_GCCTRL2, DIV, 16, 16)
REG32(SCIF_GCCTRL3, 0x80)
	FIELD(SCIF_GCCTRL3, CEN, 0, 1)
	FIELD(SCIF_GCCTRL3, DIVEN, 1, 1)
	FIELD(SCIF_GCCTRL3, OSCSEL, 8, 5)
	FIELD(SCIF_GCCTRL3, DIV, 16, 16)
REG32(SCIF_GCCTRL4, 0x84)
	FIELD(SCIF_GCCTRL4, CEN, 0, 1)
	FIELD(SCIF_GCCTRL4, DIVEN, 1, 1)
	FIELD(SCIF_GCCTRL4, OSCSEL, 8, 5)
	FIELD(SCIF_GCCTRL4, DIV, 16, 16)
REG32(SCIF_GCCTRL5, 0x88)
	FIELD(SCIF_GCCTRL5, CEN, 0, 1)
	FIELD(SCIF_GCCTRL5, DIVEN, 1, 1)
	FIELD(SCIF_GCCTRL5, OSCSEL, 8, 5)
	FIELD(SCIF_GCCTRL5, DIV, 16, 16)
REG32(SCIF_GCCTRL6, 0x8c)
	FIELD(SCIF_GCCTRL6, CEN, 0, 1)
	FIELD(SCIF_GCCTRL6, DIVEN, 1, 1)
	FIELD(SCIF_GCCTRL6, OSCSEL, 8, 5)
	FIELD(SCIF_GCCTRL6, DIV, 16, 16)
REG32(SCIF_GCCTRL7, 0x90)
	FIELD(SCIF_GCCTRL7, CEN, 0, 1)
	FIELD(SCIF_GCCTRL7, DIVEN, 1, 1)
	FIELD(SCIF_GCCTRL7, OSCSEL, 8, 5)
	FIELD(SCIF_GCCTRL7, DIV, 16, 16)
REG32(SCIF_GCCTRL8, 0x94)
	FIELD(SCIF_GCCTRL8, CEN, 0, 1)
	FIELD(SCIF_GCCTRL8, DIVEN, 1, 1)
	FIELD(SCIF_GCCTRL8, OSCSEL, 8, 5)
	FIELD(SCIF_GCCTRL8, DIV, 16, 16)
REG32(SCIF_GCCTRL9, 0x98)
	FIELD(SCIF_GCCTRL9, CEN, 0, 1)
	FIELD(SCIF_GCCTRL9, DIVEN, 1, 1)
	FIELD(SCIF_GCCTRL9, OSCSEL, 8, 5)
	FIELD(SCIF_GCCTRL9, DIV, 16, 16)
REG32(SCIF_GCCTRL10, 0x9c)
	FIELD(SCIF_GCCTRL10, CEN, 0, 1)
	FIELD(SCIF_GCCTRL10, DIVEN, 1, 1)
	FIELD(SCIF_GCCTRL10, OSCSEL, 8, 5)
	FIELD(SCIF_GCCTRL10, DIV, 16, 16)
REG32(SCIF_GCCTRL11, 0xa0)
	FIELD(SCIF_GCCTRL11, CEN, 0, 1)
	FIELD(SCIF_GCCTRL11, DIVEN, 1, 1)
	FIELD(SCIF_GCCTRL11, OSCSEL, 8, 5)
	FIELD(SCIF_GCCTRL11, DIV, 16, 16)


static void atsam4lc4c_scif_register_reset(ATSAM4LC4CSCIF *t) {
	t->VERSION = 0x130;

}

static uint64_t atsam4lc4c_scif_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAM4LC4CSCIF *t = ATSAM4LC4C_SCIF(opaque);
	uint64_t ret;

	switch (offset) {
		case A_SCIF_DFLLIFBVERSION:
			ret = t->DFLLIFBVERSION;
			break;
		case A_SCIF_DFLL0CONF:
			ret = t->DFLL0CONF;
			break;
		case A_SCIF_DFLL0MUL:
			ret = t->DFLL0MUL;
			break;
		case A_SCIF_DFLL0RATIO:
			ret = t->DFLL0RATIO;
			break;
		case A_SCIF_DFLL0STEP:
			ret = t->DFLL0STEP;
			break;
		case A_SCIF_DFLL0VAL:
			ret = t->DFLL0VAL;
			break;
		case A_SCIF_FLOVERSION:
			ret = t->FLOVERSION;
			break;
		case A_SCIF_FPCR:
			ret = t->FPCR;
			break;
		case A_SCIF_FPDIV:
			ret = t->FPDIV;
			break;
		case A_SCIF_FPMUL:
			ret = t->FPMUL;
			break;
		case A_SCIF_GCLKIFVERSION:
			ret = t->GCLKIFVERSION;
			break;
		case A_SCIF_GCLKPRESCVERSION:
			ret = t->GCLKPRESCVERSION;
			break;
		case A_SCIF_HRPCR:
			ret = t->HRPCR;
			break;
		case A_SCIF_IMR:
			ret = t->IMR;
			break;
		case A_SCIF_ISR:
			ret = t->ISR;
			break;
		case A_SCIF_OSCCTRL0:
			ret = t->OSCCTRL0;
			break;
		case A_SCIF_OSCIFAVERSION:
			ret = t->OSCIFAVERSION;
			break;
		case A_SCIF_PCLKSR:
			ret = t->PCLKSR;
			break;
		case A_SCIF_PLLIFAVERSION:
			ret = t->PLLIFAVERSION;
			break;
		case A_SCIF_PLL:
			ret = t->PLL;
			break;
		case A_SCIF_RCCR:
			ret = t->RCCR;
			break;
		case A_SCIF_RCFASTCFG:
			ret = t->RCFASTCFG;
			break;
		case A_SCIF_RCFASTSR:
			ret = t->RCFASTSR;
			break;
		case A_SCIF_RCFASTVERSION:
			ret = t->RCFASTVERSION;
			break;
		case A_SCIF_RCOSCIFAVERSION:
			ret = t->RCOSCIFAVERSION;
			break;
		case A_SCIF_RC80MCR:
			ret = t->RC80MCR;
			break;
		case A_SCIF_RC80MVERSION:
			ret = t->RC80MVERSION;
			break;
		case A_SCIF_VERSION:
			ret = t->VERSION;
			break;
		case A_SCIF_GCCTRL0:
			ret = t->GCCTRL0;
			break;
		case A_SCIF_GCCTRL1:
			ret = t->GCCTRL1;
			break;
		case A_SCIF_GCCTRL2:
			ret = t->GCCTRL2;
			break;
		case A_SCIF_GCCTRL3:
			ret = t->GCCTRL3;
			break;
		case A_SCIF_GCCTRL4:
			ret = t->GCCTRL4;
			break;
		case A_SCIF_GCCTRL5:
			ret = t->GCCTRL5;
			break;
		case A_SCIF_GCCTRL6:
			ret = t->GCCTRL6;
			break;
		case A_SCIF_GCCTRL7:
			ret = t->GCCTRL7;
			break;
		case A_SCIF_GCCTRL8:
			ret = t->GCCTRL8;
			break;
		case A_SCIF_GCCTRL9:
			ret = t->GCCTRL9;
			break;
		case A_SCIF_GCCTRL10:
			ret = t->GCCTRL10;
			break;
		case A_SCIF_GCCTRL11:
			ret = t->GCCTRL11;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM4LC4C SCIF read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsam4lc4c_scif_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAM4LC4CSCIF *t = ATSAM4LC4C_SCIF(opaque);


	switch (offset) {
		case A_SCIF_DFLL0CONF ... A_SCIF_DFLL0CONF + 3:
			value = value << ((offset - A_SCIF_DFLL0CONF) << 3);
			offset = A_SCIF_DFLL0CONF;
			break;
		case A_SCIF_DFLL0MUL ... A_SCIF_DFLL0MUL + 3:
			value = value << ((offset - A_SCIF_DFLL0MUL) << 3);
			offset = A_SCIF_DFLL0MUL;
			break;
		case A_SCIF_DFLL0SSG ... A_SCIF_DFLL0SSG + 3:
			value = value << ((offset - A_SCIF_DFLL0SSG) << 3);
			offset = A_SCIF_DFLL0SSG;
			break;
		case A_SCIF_DFLL0STEP ... A_SCIF_DFLL0STEP + 3:
			value = value << ((offset - A_SCIF_DFLL0STEP) << 3);
			offset = A_SCIF_DFLL0STEP;
			break;
		case A_SCIF_DFLL0SYNC ... A_SCIF_DFLL0SYNC + 3:
			value = value << ((offset - A_SCIF_DFLL0SYNC) << 3);
			offset = A_SCIF_DFLL0SYNC;
			break;
		case A_SCIF_DFLL0VAL ... A_SCIF_DFLL0VAL + 3:
			value = value << ((offset - A_SCIF_DFLL0VAL) << 3);
			offset = A_SCIF_DFLL0VAL;
			break;
		case A_SCIF_FPCR ... A_SCIF_FPCR + 3:
			value = value << ((offset - A_SCIF_FPCR) << 3);
			offset = A_SCIF_FPCR;
			break;
		case A_SCIF_FPDIV ... A_SCIF_FPDIV + 3:
			value = value << ((offset - A_SCIF_FPDIV) << 3);
			offset = A_SCIF_FPDIV;
			break;
		case A_SCIF_FPMUL ... A_SCIF_FPMUL + 3:
			value = value << ((offset - A_SCIF_FPMUL) << 3);
			offset = A_SCIF_FPMUL;
			break;
		case A_SCIF_HRPCR ... A_SCIF_HRPCR + 3:
			value = value << ((offset - A_SCIF_HRPCR) << 3);
			offset = A_SCIF_HRPCR;
			break;
		case A_SCIF_ICR ... A_SCIF_ICR + 3:
			value = value << ((offset - A_SCIF_ICR) << 3);
			offset = A_SCIF_ICR;
			break;
		case A_SCIF_IDR ... A_SCIF_IDR + 3:
			value = value << ((offset - A_SCIF_IDR) << 3);
			offset = A_SCIF_IDR;
			break;
		case A_SCIF_IER ... A_SCIF_IER + 3:
			value = value << ((offset - A_SCIF_IER) << 3);
			offset = A_SCIF_IER;
			break;
		case A_SCIF_OSCCTRL0 ... A_SCIF_OSCCTRL0 + 3:
			value = value << ((offset - A_SCIF_OSCCTRL0) << 3);
			offset = A_SCIF_OSCCTRL0;
			break;
		case A_SCIF_PLL ... A_SCIF_PLL + 3:
			value = value << ((offset - A_SCIF_PLL) << 3);
			offset = A_SCIF_PLL;
			break;
		case A_SCIF_RCCR ... A_SCIF_RCCR + 3:
			value = value << ((offset - A_SCIF_RCCR) << 3);
			offset = A_SCIF_RCCR;
			break;
		case A_SCIF_RCFASTCFG ... A_SCIF_RCFASTCFG + 3:
			value = value << ((offset - A_SCIF_RCFASTCFG) << 3);
			offset = A_SCIF_RCFASTCFG;
			break;
		case A_SCIF_RCFASTSR ... A_SCIF_RCFASTSR + 3:
			value = value << ((offset - A_SCIF_RCFASTSR) << 3);
			offset = A_SCIF_RCFASTSR;
			break;
		case A_SCIF_RC80MCR ... A_SCIF_RC80MCR + 3:
			value = value << ((offset - A_SCIF_RC80MCR) << 3);
			offset = A_SCIF_RC80MCR;
			break;
		case A_SCIF_UNLOCK ... A_SCIF_UNLOCK + 3:
			value = value << ((offset - A_SCIF_UNLOCK) << 3);
			offset = A_SCIF_UNLOCK;
			break;
		case A_SCIF_GCCTRL0 ... A_SCIF_GCCTRL0 + 3:
			value = value << ((offset - A_SCIF_GCCTRL0) << 3);
			offset = A_SCIF_GCCTRL0;
			break;
		case A_SCIF_GCCTRL1 ... A_SCIF_GCCTRL1 + 3:
			value = value << ((offset - A_SCIF_GCCTRL1) << 3);
			offset = A_SCIF_GCCTRL1;
			break;
		case A_SCIF_GCCTRL2 ... A_SCIF_GCCTRL2 + 3:
			value = value << ((offset - A_SCIF_GCCTRL2) << 3);
			offset = A_SCIF_GCCTRL2;
			break;
		case A_SCIF_GCCTRL3 ... A_SCIF_GCCTRL3 + 3:
			value = value << ((offset - A_SCIF_GCCTRL3) << 3);
			offset = A_SCIF_GCCTRL3;
			break;
		case A_SCIF_GCCTRL4 ... A_SCIF_GCCTRL4 + 3:
			value = value << ((offset - A_SCIF_GCCTRL4) << 3);
			offset = A_SCIF_GCCTRL4;
			break;
		case A_SCIF_GCCTRL5 ... A_SCIF_GCCTRL5 + 3:
			value = value << ((offset - A_SCIF_GCCTRL5) << 3);
			offset = A_SCIF_GCCTRL5;
			break;
		case A_SCIF_GCCTRL6 ... A_SCIF_GCCTRL6 + 3:
			value = value << ((offset - A_SCIF_GCCTRL6) << 3);
			offset = A_SCIF_GCCTRL6;
			break;
		case A_SCIF_GCCTRL7 ... A_SCIF_GCCTRL7 + 3:
			value = value << ((offset - A_SCIF_GCCTRL7) << 3);
			offset = A_SCIF_GCCTRL7;
			break;
		case A_SCIF_GCCTRL8 ... A_SCIF_GCCTRL8 + 3:
			value = value << ((offset - A_SCIF_GCCTRL8) << 3);
			offset = A_SCIF_GCCTRL8;
			break;
		case A_SCIF_GCCTRL9 ... A_SCIF_GCCTRL9 + 3:
			value = value << ((offset - A_SCIF_GCCTRL9) << 3);
			offset = A_SCIF_GCCTRL9;
			break;
		case A_SCIF_GCCTRL10 ... A_SCIF_GCCTRL10 + 3:
			value = value << ((offset - A_SCIF_GCCTRL10) << 3);
			offset = A_SCIF_GCCTRL10;
			break;
		case A_SCIF_GCCTRL11 ... A_SCIF_GCCTRL11 + 3:
			value = value << ((offset - A_SCIF_GCCTRL11) << 3);
			offset = A_SCIF_GCCTRL11;
			break;

		default: break;
	}

	switch (offset) {
		case A_SCIF_DFLL0CONF:
			t->DFLL0CONF = value;
			break;
		case A_SCIF_DFLL0MUL:
			t->DFLL0MUL = value;
			break;
		case A_SCIF_DFLL0SSG:
			t->DFLL0SSG = value;
			break;
		case A_SCIF_DFLL0STEP:
			t->DFLL0STEP = value;
			break;
		case A_SCIF_DFLL0SYNC:
			t->DFLL0SYNC = value;
			break;
		case A_SCIF_DFLL0VAL:
			t->DFLL0VAL = value;
			break;
		case A_SCIF_FPCR:
			t->FPCR = value;
			break;
		case A_SCIF_FPDIV:
			t->FPDIV = value;
			break;
		case A_SCIF_FPMUL:
			t->FPMUL = value;
			break;
		case A_SCIF_HRPCR:
			t->HRPCR = value;
			break;
		case A_SCIF_ICR:
			t->ICR = value;
			break;
		case A_SCIF_IDR:
			t->IDR = value;
			break;
		case A_SCIF_IER:
			t->IER = value;
			break;
		case A_SCIF_OSCCTRL0:
			t->OSCCTRL0 = value;
			if ((value == 0x10207)) {
				t->PCLKSR |= 0x1;
			}
			break;
		case A_SCIF_PLL:
			t->PLL = value;
			if ((value == 0x3f0f0401)) {
				t->PCLKSR |= 0x40;
			}
			break;
		case A_SCIF_RCCR:
			t->RCCR = value;
			break;
		case A_SCIF_RCFASTCFG:
			t->RCFASTCFG = value;
			break;
		case A_SCIF_RCFASTSR:
			t->RCFASTSR = value;
			break;
		case A_SCIF_RC80MCR:
			t->RC80MCR = value;
			break;
		case A_SCIF_UNLOCK:
			t->UNLOCK = value;
			break;
		case A_SCIF_GCCTRL0:
			t->GCCTRL0 = value;
			break;
		case A_SCIF_GCCTRL1:
			t->GCCTRL1 = value;
			break;
		case A_SCIF_GCCTRL2:
			t->GCCTRL2 = value;
			break;
		case A_SCIF_GCCTRL3:
			t->GCCTRL3 = value;
			break;
		case A_SCIF_GCCTRL4:
			t->GCCTRL4 = value;
			break;
		case A_SCIF_GCCTRL5:
			t->GCCTRL5 = value;
			break;
		case A_SCIF_GCCTRL6:
			t->GCCTRL6 = value;
			break;
		case A_SCIF_GCCTRL7:
			t->GCCTRL7 = value;
			break;
		case A_SCIF_GCCTRL8:
			t->GCCTRL8 = value;
			break;
		case A_SCIF_GCCTRL9:
			t->GCCTRL9 = value;
			break;
		case A_SCIF_GCCTRL10:
			t->GCCTRL10 = value;
			break;
		case A_SCIF_GCCTRL11:
			t->GCCTRL11 = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM4LC4C SCIF write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsam4lc4c_scif_ops = {
	.read = atsam4lc4c_scif_read,
	.write = atsam4lc4c_scif_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsam4lc4c_scif_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAM4LC4CSCIF *t = ATSAM4LC4C_SCIF(obj);
	memory_region_init_io(&(t->iomem), obj, &atsam4lc4c_scif_ops, t, "atsam4lc4c-scif", ATSAM4LC4C_SCIF_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void atsam4lc4c_scif_realize(DeviceState *dev, Error **errp) {
	return;

}

static void atsam4lc4c_scif_reset_enter(Object *obj, ResetType type) {
	ATSAM4LC4CSCIF *t = ATSAM4LC4C_SCIF(obj);
	atsam4lc4c_scif_register_reset(t);
}

static const VMStateDescription atsam4lc4c_scif_vmstate = {
	.name = "atsam4lc4c-scif",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CSCR, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(DFLLIFBVERSION, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(DFLL0CONF, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(DFLL0MUL, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(DFLL0RATIO, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(DFLL0SSG, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(DFLL0STEP, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(DFLL0SYNC, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(DFLL0VAL, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(FLOVERSION, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(FPCR, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(FPDIV, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(FPMUL, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(GCLKIFVERSION, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(GCLKPRESCVERSION, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(HRPCR, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(ICR, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(IDR, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(IER, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(IMR, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(ISR, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(OSCCTRL0, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(OSCIFAVERSION, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(PCLKSR, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(PLLIFAVERSION, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(PLL, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(RCCR, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(RCFASTCFG, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(RCFASTSR, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(RCFASTVERSION, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(RCOSCIFAVERSION, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(RC80MCR, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(RC80MVERSION, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(UNLOCK, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(VERSION, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(GCCTRL0, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(GCCTRL1, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(GCCTRL2, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(GCCTRL3, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(GCCTRL4, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(GCCTRL5, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(GCCTRL6, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(GCCTRL7, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(GCCTRL8, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(GCCTRL9, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(GCCTRL10, ATSAM4LC4CSCIF),
		VMSTATE_UINT32(GCCTRL11, ATSAM4LC4CSCIF),

		VMSTATE_END_OF_LIST()
	}
};

static void atsam4lc4c_scif_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsam4lc4c_scif_vmstate;
	dc->realize = atsam4lc4c_scif_realize;
	rc->phases.enter = atsam4lc4c_scif_reset_enter;

}

static const TypeInfo atsam4lc4c_scif_info = {
	.name = TYPE_ATSAM4LC4C_SCIF,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAM4LC4CSCIF),
	.instance_init = atsam4lc4c_scif_init,
	.class_init = atsam4lc4c_scif_class_init,
};

static void atsam4lc4c_scif_register_types(void) {
	type_register_static(&atsam4lc4c_scif_info);
}

type_init(atsam4lc4c_scif_register_types);

#define TYPE_ATSAM4LC4C_BSCIF "atsam4lc4c-bscif"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAM4LC4CBSCIF, ATSAM4LC4C_BSCIF)

struct ATSAM4LC4CBSCIF {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t BGCR;
	uint32_t BGCTRL;
	uint32_t BGREFIFBVERSION;
	uint32_t BGSR;
	uint32_t BODIFCVERSION;
	uint32_t BOD18CTRL;
	uint32_t BOD18LEVEL;
	uint32_t BOD33CTRL;
	uint32_t BOD33LEVEL;
	uint32_t BOD33SAMPLING;
	uint32_t BRIFBVERSION;
	uint32_t CSCR;
	uint32_t ICR;
	uint32_t IDR;
	uint32_t IER;
	uint32_t IMR;
	uint32_t ISR;
	uint32_t OSCCTRL32;
	uint32_t OSC32IFAVERSION;
	uint32_t PCLKSR;
	uint32_t RC1MCR;
	uint32_t RC32KCR;
	uint32_t RC32KIFBVERSION;
	uint32_t RC32KTUNE;
	uint32_t UNLOCK;
	uint32_t VERSION;
	uint32_t VREGCR;
	uint32_t VREGIFGVERSION;
	uint32_t VREGLPCSR;
	uint32_t VREGNCSR;
	uint32_t BR0;
	uint32_t BR1;
	uint32_t BR2;
	uint32_t BR3;
	/* base */
	uint32_t base;

};
#define ATSAM4LC4C_BSCIF_SIZE				0x400

REG32(BSCIF_BGCR, 0x5c)
REG32(BSCIF_BGCTRL, 0x60)
	FIELD(BSCIF_BGCTRL, ADCISEL, 0, 2)
	FIELD(BSCIF_BGCTRL, TSEN, 8, 1)
REG32(BSCIF_BGREFIFBVERSION, 0x3e8)
	FIELD(BSCIF_BGREFIFBVERSION, VERSION, 0, 12)
	FIELD(BSCIF_BGREFIFBVERSION, VARIANT, 16, 4)
REG32(BSCIF_BGSR, 0x64)
	FIELD(BSCIF_BGSR, BGBUFRDY, 0, 8)
	FIELD(BSCIF_BGSR, BGRDY, 16, 1)
	FIELD(BSCIF_BGSR, LPBGRDY, 17, 1)
	FIELD(BSCIF_BGSR, VREF, 18, 2)
REG32(BSCIF_BODIFCVERSION, 0x3f0)
	FIELD(BSCIF_BODIFCVERSION, VERSION, 0, 12)
	FIELD(BSCIF_BODIFCVERSION, VARIANT, 16, 4)
REG32(BSCIF_BOD18CTRL, 0x38)
	FIELD(BSCIF_BOD18CTRL, EN, 0, 1)
	FIELD(BSCIF_BOD18CTRL, HYST, 1, 1)
	FIELD(BSCIF_BOD18CTRL, ACTION, 8, 2)
	FIELD(BSCIF_BOD18CTRL, MODE, 16, 1)
	FIELD(BSCIF_BOD18CTRL, FCD, 30, 1)
	FIELD(BSCIF_BOD18CTRL, SFV, 31, 1)
REG32(BSCIF_BOD18LEVEL, 0x3c)
	FIELD(BSCIF_BOD18LEVEL, VAL, 0, 6)
	FIELD(BSCIF_BOD18LEVEL, RANGE, 31, 1)
REG32(BSCIF_BOD33CTRL, 0x2c)
	FIELD(BSCIF_BOD33CTRL, EN, 0, 1)
	FIELD(BSCIF_BOD33CTRL, HYST, 1, 1)
	FIELD(BSCIF_BOD33CTRL, ACTION, 8, 2)
	FIELD(BSCIF_BOD33CTRL, MODE, 16, 1)
	FIELD(BSCIF_BOD33CTRL, FCD, 30, 1)
	FIELD(BSCIF_BOD33CTRL, SFV, 31, 1)
REG32(BSCIF_BOD33LEVEL, 0x30)
	FIELD(BSCIF_BOD33LEVEL, VAL, 0, 6)
REG32(BSCIF_BOD33SAMPLING, 0x34)
	FIELD(BSCIF_BOD33SAMPLING, CEN, 0, 1)
	FIELD(BSCIF_BOD33SAMPLING, CSSEL, 1, 1)
	FIELD(BSCIF_BOD33SAMPLING, PSEL, 8, 4)
REG32(BSCIF_BRIFBVERSION, 0x3e4)
	FIELD(BSCIF_BRIFBVERSION, VERSION, 0, 12)
	FIELD(BSCIF_BRIFBVERSION, VARIANT, 16, 4)
REG32(BSCIF_CSCR, 0x1c)
REG32(BSCIF_ICR, 0x10)
	FIELD(BSCIF_ICR, OSC32RDY, 0, 1)
	FIELD(BSCIF_ICR, RC32KRDY, 1, 1)
	FIELD(BSCIF_ICR, RC32KLOCK, 2, 1)
	FIELD(BSCIF_ICR, RC32KREFE, 3, 1)
	FIELD(BSCIF_ICR, RC32KSAT, 4, 1)
	FIELD(BSCIF_ICR, BOD33DET, 5, 1)
	FIELD(BSCIF_ICR, BOD18DET, 6, 1)
	FIELD(BSCIF_ICR, BOD33SYNRDY, 7, 1)
	FIELD(BSCIF_ICR, BOD18SYNRDY, 8, 1)
	FIELD(BSCIF_ICR, SSWRDY, 9, 1)
	FIELD(BSCIF_ICR, VREGOK, 10, 1)
	FIELD(BSCIF_ICR, LPBGRDY, 12, 1)
	FIELD(BSCIF_ICR, AE, 31, 1)
REG32(BSCIF_IDR, 0x4)
	FIELD(BSCIF_IDR, OSC32RDY, 0, 1)
	FIELD(BSCIF_IDR, RC32KRDY, 1, 1)
	FIELD(BSCIF_IDR, RC32KLOCK, 2, 1)
	FIELD(BSCIF_IDR, RC32KREFE, 3, 1)
	FIELD(BSCIF_IDR, RC32KSAT, 4, 1)
	FIELD(BSCIF_IDR, BOD33DET, 5, 1)
	FIELD(BSCIF_IDR, BOD18DET, 6, 1)
	FIELD(BSCIF_IDR, BOD33SYNRDY, 7, 1)
	FIELD(BSCIF_IDR, BOD18SYNRDY, 8, 1)
	FIELD(BSCIF_IDR, SSWRDY, 9, 1)
	FIELD(BSCIF_IDR, VREGOK, 10, 1)
	FIELD(BSCIF_IDR, LPBGRDY, 12, 1)
	FIELD(BSCIF_IDR, AE, 31, 1)
REG32(BSCIF_IER, 0x0)
	FIELD(BSCIF_IER, OSC32RDY, 0, 1)
	FIELD(BSCIF_IER, RC32KRDY, 1, 1)
	FIELD(BSCIF_IER, RC32KLOCK, 2, 1)
	FIELD(BSCIF_IER, RC32KREFE, 3, 1)
	FIELD(BSCIF_IER, RC32KSAT, 4, 1)
	FIELD(BSCIF_IER, BOD33DET, 5, 1)
	FIELD(BSCIF_IER, BOD18DET, 6, 1)
	FIELD(BSCIF_IER, BOD33SYNRDY, 7, 1)
	FIELD(BSCIF_IER, BOD18SYNRDY, 8, 1)
	FIELD(BSCIF_IER, SSWRDY, 9, 1)
	FIELD(BSCIF_IER, VREGOK, 10, 1)
	FIELD(BSCIF_IER, LPBGRDY, 12, 1)
	FIELD(BSCIF_IER, AE, 31, 1)
REG32(BSCIF_IMR, 0x8)
	FIELD(BSCIF_IMR, OSC32RDY, 0, 1)
	FIELD(BSCIF_IMR, RC32KRDY, 1, 1)
	FIELD(BSCIF_IMR, RC32KLOCK, 2, 1)
	FIELD(BSCIF_IMR, RC32KREFE, 3, 1)
	FIELD(BSCIF_IMR, RC32KSAT, 4, 1)
	FIELD(BSCIF_IMR, BOD33DET, 5, 1)
	FIELD(BSCIF_IMR, BOD18DET, 6, 1)
	FIELD(BSCIF_IMR, BOD33SYNRDY, 7, 1)
	FIELD(BSCIF_IMR, BOD18SYNRDY, 8, 1)
	FIELD(BSCIF_IMR, SSWRDY, 9, 1)
	FIELD(BSCIF_IMR, VREGOK, 10, 1)
	FIELD(BSCIF_IMR, LPBGRDY, 12, 1)
	FIELD(BSCIF_IMR, AE, 31, 1)
REG32(BSCIF_ISR, 0xc)
	FIELD(BSCIF_ISR, OSC32RDY, 0, 1)
	FIELD(BSCIF_ISR, RC32KRDY, 1, 1)
	FIELD(BSCIF_ISR, RC32KLOCK, 2, 1)
	FIELD(BSCIF_ISR, RC32KREFE, 3, 1)
	FIELD(BSCIF_ISR, RC32KSAT, 4, 1)
	FIELD(BSCIF_ISR, BOD33DET, 5, 1)
	FIELD(BSCIF_ISR, BOD18DET, 6, 1)
	FIELD(BSCIF_ISR, BOD33SYNRDY, 7, 1)
	FIELD(BSCIF_ISR, BOD18SYNRDY, 8, 1)
	FIELD(BSCIF_ISR, SSWRDY, 9, 1)
	FIELD(BSCIF_ISR, VREGOK, 10, 1)
	FIELD(BSCIF_ISR, LPBGRDY, 12, 1)
	FIELD(BSCIF_ISR, AE, 31, 1)
REG32(BSCIF_OSCCTRL32, 0x20)
	FIELD(BSCIF_OSCCTRL32, OSC32EN, 0, 1)
	FIELD(BSCIF_OSCCTRL32, PINSEL, 1, 1)
	FIELD(BSCIF_OSCCTRL32, EN32K, 2, 1)
	FIELD(BSCIF_OSCCTRL32, EN1K, 3, 1)
	FIELD(BSCIF_OSCCTRL32, MODE, 8, 3)
	FIELD(BSCIF_OSCCTRL32, SELCURR, 12, 4)
	FIELD(BSCIF_OSCCTRL32, STARTUP, 16, 3)
REG32(BSCIF_OSC32IFAVERSION, 0x3f8)
	FIELD(BSCIF_OSC32IFAVERSION, VERSION, 0, 12)
	FIELD(BSCIF_OSC32IFAVERSION, VARIANT, 16, 4)
REG32(BSCIF_PCLKSR, 0x14)
	FIELD(BSCIF_PCLKSR, OSC32RDY, 0, 1)
	FIELD(BSCIF_PCLKSR, RC32KRDY, 1, 1)
	FIELD(BSCIF_PCLKSR, RC32KLOCK, 2, 1)
	FIELD(BSCIF_PCLKSR, RC32KREFE, 3, 1)
	FIELD(BSCIF_PCLKSR, RC32KSAT, 4, 1)
	FIELD(BSCIF_PCLKSR, BOD33DET, 5, 1)
	FIELD(BSCIF_PCLKSR, BOD18DET, 6, 1)
	FIELD(BSCIF_PCLKSR, BOD33SYNRDY, 7, 1)
	FIELD(BSCIF_PCLKSR, BOD18SYNRDY, 8, 1)
	FIELD(BSCIF_PCLKSR, SSWRDY, 9, 1)
	FIELD(BSCIF_PCLKSR, VREGOK, 10, 1)
	FIELD(BSCIF_PCLKSR, RC1MRDY, 11, 1)
	FIELD(BSCIF_PCLKSR, LPBGRDY, 12, 1)
REG32(BSCIF_RC1MCR, 0x58)
	FIELD(BSCIF_RC1MCR, CLKOE, 0, 1)
	FIELD(BSCIF_RC1MCR, FCD, 7, 1)
	FIELD(BSCIF_RC1MCR, CLKCAL, 8, 5)
REG32(BSCIF_RC32KCR, 0x24)
	FIELD(BSCIF_RC32KCR, EN, 0, 1)
	FIELD(BSCIF_RC32KCR, TCEN, 1, 1)
	FIELD(BSCIF_RC32KCR, EN32K, 2, 1)
	FIELD(BSCIF_RC32KCR, EN1K, 3, 1)
	FIELD(BSCIF_RC32KCR, MODE, 4, 1)
	FIELD(BSCIF_RC32KCR, REF, 5, 1)
	FIELD(BSCIF_RC32KCR, FCD, 7, 1)
REG32(BSCIF_RC32KIFBVERSION, 0x3f4)
	FIELD(BSCIF_RC32KIFBVERSION, VERSION, 0, 12)
	FIELD(BSCIF_RC32KIFBVERSION, VARIANT, 16, 4)
REG32(BSCIF_RC32KTUNE, 0x28)
	FIELD(BSCIF_RC32KTUNE, FINE, 0, 6)
	FIELD(BSCIF_RC32KTUNE, COARSE, 16, 7)
REG32(BSCIF_UNLOCK, 0x18)
	FIELD(BSCIF_UNLOCK, ADDR, 0, 10)
	FIELD(BSCIF_UNLOCK, KEY, 24, 8)
REG32(BSCIF_VERSION, 0x3fc)
	FIELD(BSCIF_VERSION, VERSION, 0, 12)
	FIELD(BSCIF_VERSION, VARIANT, 16, 4)
REG32(BSCIF_VREGCR, 0x44)
	FIELD(BSCIF_VREGCR, DIS, 0, 1)
	FIELD(BSCIF_VREGCR, SSG, 8, 1)
	FIELD(BSCIF_VREGCR, SSW, 9, 1)
	FIELD(BSCIF_VREGCR, SSWEVT, 10, 1)
	FIELD(BSCIF_VREGCR, SFV, 31, 1)
REG32(BSCIF_VREGIFGVERSION, 0x3ec)
	FIELD(BSCIF_VREGIFGVERSION, VERSION, 0, 12)
	FIELD(BSCIF_VREGIFGVERSION, VARIANT, 16, 4)
REG32(BSCIF_VREGLPCSR, 0x50)
REG32(BSCIF_VREGNCSR, 0x4c)
REG32(BSCIF_BR0, 0x78)
REG32(BSCIF_BR1, 0x7c)
REG32(BSCIF_BR2, 0x80)
REG32(BSCIF_BR3, 0x84)


static void atsam4lc4c_bscif_register_reset(ATSAM4LC4CBSCIF *t) {
	t->BGREFIFBVERSION = 0x110;
	t->BODIFCVERSION = 0x110;
	t->BRIFBVERSION = 0x100;
	t->OSCCTRL32 = 0x4;
	t->OSC32IFAVERSION = 0x200;
	t->RC1MCR = 0xf00;
	t->RC32KIFBVERSION = 0x100;
	t->VERSION = 0x100;
	t->VREGIFGVERSION = 0x110;

}

static uint64_t atsam4lc4c_bscif_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAM4LC4CBSCIF *t = ATSAM4LC4C_BSCIF(opaque);
	uint64_t ret;

	switch (offset) {
		case A_BSCIF_BGCTRL:
			ret = t->BGCTRL;
			break;
		case A_BSCIF_BGREFIFBVERSION:
			ret = t->BGREFIFBVERSION;
			break;
		case A_BSCIF_BGSR:
			ret = t->BGSR;
			break;
		case A_BSCIF_BODIFCVERSION:
			ret = t->BODIFCVERSION;
			break;
		case A_BSCIF_BOD18CTRL:
			ret = t->BOD18CTRL;
			break;
		case A_BSCIF_BOD18LEVEL:
			ret = t->BOD18LEVEL;
			break;
		case A_BSCIF_BOD33CTRL:
			ret = t->BOD33CTRL;
			break;
		case A_BSCIF_BOD33LEVEL:
			ret = t->BOD33LEVEL;
			break;
		case A_BSCIF_BOD33SAMPLING:
			ret = t->BOD33SAMPLING;
			break;
		case A_BSCIF_BRIFBVERSION:
			ret = t->BRIFBVERSION;
			break;
		case A_BSCIF_IMR:
			ret = t->IMR;
			break;
		case A_BSCIF_ISR:
			ret = t->ISR;
			break;
		case A_BSCIF_OSCCTRL32:
			ret = t->OSCCTRL32;
			break;
		case A_BSCIF_OSC32IFAVERSION:
			ret = t->OSC32IFAVERSION;
			break;
		case A_BSCIF_PCLKSR:
			ret = t->PCLKSR;
			break;
		case A_BSCIF_RC1MCR:
			ret = t->RC1MCR;
			break;
		case A_BSCIF_RC32KCR:
			ret = t->RC32KCR;
			break;
		case A_BSCIF_RC32KIFBVERSION:
			ret = t->RC32KIFBVERSION;
			break;
		case A_BSCIF_RC32KTUNE:
			ret = t->RC32KTUNE;
			break;
		case A_BSCIF_VERSION:
			ret = t->VERSION;
			break;
		case A_BSCIF_VREGCR:
			ret = t->VREGCR;
			break;
		case A_BSCIF_VREGIFGVERSION:
			ret = t->VREGIFGVERSION;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM4LC4C BSCIF read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsam4lc4c_bscif_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAM4LC4CBSCIF *t = ATSAM4LC4C_BSCIF(opaque);


	switch (offset) {
		case A_BSCIF_BGCTRL ... A_BSCIF_BGCTRL + 3:
			value = value << ((offset - A_BSCIF_BGCTRL) << 3);
			offset = A_BSCIF_BGCTRL;
			break;
		case A_BSCIF_BOD18CTRL ... A_BSCIF_BOD18CTRL + 3:
			value = value << ((offset - A_BSCIF_BOD18CTRL) << 3);
			offset = A_BSCIF_BOD18CTRL;
			break;
		case A_BSCIF_BOD18LEVEL ... A_BSCIF_BOD18LEVEL + 3:
			value = value << ((offset - A_BSCIF_BOD18LEVEL) << 3);
			offset = A_BSCIF_BOD18LEVEL;
			break;
		case A_BSCIF_BOD33CTRL ... A_BSCIF_BOD33CTRL + 3:
			value = value << ((offset - A_BSCIF_BOD33CTRL) << 3);
			offset = A_BSCIF_BOD33CTRL;
			break;
		case A_BSCIF_BOD33LEVEL ... A_BSCIF_BOD33LEVEL + 3:
			value = value << ((offset - A_BSCIF_BOD33LEVEL) << 3);
			offset = A_BSCIF_BOD33LEVEL;
			break;
		case A_BSCIF_BOD33SAMPLING ... A_BSCIF_BOD33SAMPLING + 3:
			value = value << ((offset - A_BSCIF_BOD33SAMPLING) << 3);
			offset = A_BSCIF_BOD33SAMPLING;
			break;
		case A_BSCIF_ICR ... A_BSCIF_ICR + 3:
			value = value << ((offset - A_BSCIF_ICR) << 3);
			offset = A_BSCIF_ICR;
			break;
		case A_BSCIF_IDR ... A_BSCIF_IDR + 3:
			value = value << ((offset - A_BSCIF_IDR) << 3);
			offset = A_BSCIF_IDR;
			break;
		case A_BSCIF_IER ... A_BSCIF_IER + 3:
			value = value << ((offset - A_BSCIF_IER) << 3);
			offset = A_BSCIF_IER;
			break;
		case A_BSCIF_OSCCTRL32 ... A_BSCIF_OSCCTRL32 + 3:
			value = value << ((offset - A_BSCIF_OSCCTRL32) << 3);
			offset = A_BSCIF_OSCCTRL32;
			break;
		case A_BSCIF_RC1MCR ... A_BSCIF_RC1MCR + 3:
			value = value << ((offset - A_BSCIF_RC1MCR) << 3);
			offset = A_BSCIF_RC1MCR;
			break;
		case A_BSCIF_RC32KCR ... A_BSCIF_RC32KCR + 3:
			value = value << ((offset - A_BSCIF_RC32KCR) << 3);
			offset = A_BSCIF_RC32KCR;
			break;
		case A_BSCIF_RC32KTUNE ... A_BSCIF_RC32KTUNE + 3:
			value = value << ((offset - A_BSCIF_RC32KTUNE) << 3);
			offset = A_BSCIF_RC32KTUNE;
			break;
		case A_BSCIF_UNLOCK ... A_BSCIF_UNLOCK + 3:
			value = value << ((offset - A_BSCIF_UNLOCK) << 3);
			offset = A_BSCIF_UNLOCK;
			break;
		case A_BSCIF_VREGCR ... A_BSCIF_VREGCR + 3:
			value = value << ((offset - A_BSCIF_VREGCR) << 3);
			offset = A_BSCIF_VREGCR;
			break;

		default: break;
	}

	switch (offset) {
		case A_BSCIF_BGCTRL:
			t->BGCTRL = value;
			break;
		case A_BSCIF_BOD18CTRL:
			t->BOD18CTRL = value;
			break;
		case A_BSCIF_BOD18LEVEL:
			t->BOD18LEVEL = value;
			break;
		case A_BSCIF_BOD33CTRL:
			t->BOD33CTRL = value;
			break;
		case A_BSCIF_BOD33LEVEL:
			t->BOD33LEVEL = value;
			break;
		case A_BSCIF_BOD33SAMPLING:
			t->BOD33SAMPLING = value;
			break;
		case A_BSCIF_ICR:
			t->ICR = value;
			break;
		case A_BSCIF_IDR:
			t->IDR = value;
			break;
		case A_BSCIF_IER:
			t->IER = value;
			break;
		case A_BSCIF_OSCCTRL32:
			t->OSCCTRL32 = value;
			break;
		case A_BSCIF_RC1MCR:
			t->RC1MCR = value;
			break;
		case A_BSCIF_RC32KCR:
			t->RC32KCR = value;
			break;
		case A_BSCIF_RC32KTUNE:
			t->RC32KTUNE = value;
			break;
		case A_BSCIF_UNLOCK:
			t->UNLOCK = value;
			break;
		case A_BSCIF_VREGCR:
			t->VREGCR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM4LC4C BSCIF write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsam4lc4c_bscif_ops = {
	.read = atsam4lc4c_bscif_read,
	.write = atsam4lc4c_bscif_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsam4lc4c_bscif_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAM4LC4CBSCIF *t = ATSAM4LC4C_BSCIF(obj);
	memory_region_init_io(&(t->iomem), obj, &atsam4lc4c_bscif_ops, t, "atsam4lc4c-bscif", ATSAM4LC4C_BSCIF_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void atsam4lc4c_bscif_realize(DeviceState *dev, Error **errp) {
	return;

}

static void atsam4lc4c_bscif_reset_enter(Object *obj, ResetType type) {
	ATSAM4LC4CBSCIF *t = ATSAM4LC4C_BSCIF(obj);
	atsam4lc4c_bscif_register_reset(t);
}

static const VMStateDescription atsam4lc4c_bscif_vmstate = {
	.name = "atsam4lc4c-bscif",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(BGCR, ATSAM4LC4CBSCIF),
		VMSTATE_UINT32(BGCTRL, ATSAM4LC4CBSCIF),
		VMSTATE_UINT32(BGREFIFBVERSION, ATSAM4LC4CBSCIF),
		VMSTATE_UINT32(BGSR, ATSAM4LC4CBSCIF),
		VMSTATE_UINT32(BODIFCVERSION, ATSAM4LC4CBSCIF),
		VMSTATE_UINT32(BOD18CTRL, ATSAM4LC4CBSCIF),
		VMSTATE_UINT32(BOD18LEVEL, ATSAM4LC4CBSCIF),
		VMSTATE_UINT32(BOD33CTRL, ATSAM4LC4CBSCIF),
		VMSTATE_UINT32(BOD33LEVEL, ATSAM4LC4CBSCIF),
		VMSTATE_UINT32(BOD33SAMPLING, ATSAM4LC4CBSCIF),
		VMSTATE_UINT32(BRIFBVERSION, ATSAM4LC4CBSCIF),
		VMSTATE_UINT32(CSCR, ATSAM4LC4CBSCIF),
		VMSTATE_UINT32(ICR, ATSAM4LC4CBSCIF),
		VMSTATE_UINT32(IDR, ATSAM4LC4CBSCIF),
		VMSTATE_UINT32(IER, ATSAM4LC4CBSCIF),
		VMSTATE_UINT32(IMR, ATSAM4LC4CBSCIF),
		VMSTATE_UINT32(ISR, ATSAM4LC4CBSCIF),
		VMSTATE_UINT32(OSCCTRL32, ATSAM4LC4CBSCIF),
		VMSTATE_UINT32(OSC32IFAVERSION, ATSAM4LC4CBSCIF),
		VMSTATE_UINT32(PCLKSR, ATSAM4LC4CBSCIF),
		VMSTATE_UINT32(RC1MCR, ATSAM4LC4CBSCIF),
		VMSTATE_UINT32(RC32KCR, ATSAM4LC4CBSCIF),
		VMSTATE_UINT32(RC32KIFBVERSION, ATSAM4LC4CBSCIF),
		VMSTATE_UINT32(RC32KTUNE, ATSAM4LC4CBSCIF),
		VMSTATE_UINT32(UNLOCK, ATSAM4LC4CBSCIF),
		VMSTATE_UINT32(VERSION, ATSAM4LC4CBSCIF),
		VMSTATE_UINT32(VREGCR, ATSAM4LC4CBSCIF),
		VMSTATE_UINT32(VREGIFGVERSION, ATSAM4LC4CBSCIF),
		VMSTATE_UINT32(VREGLPCSR, ATSAM4LC4CBSCIF),
		VMSTATE_UINT32(VREGNCSR, ATSAM4LC4CBSCIF),
		VMSTATE_UINT32(BR0, ATSAM4LC4CBSCIF),
		VMSTATE_UINT32(BR1, ATSAM4LC4CBSCIF),
		VMSTATE_UINT32(BR2, ATSAM4LC4CBSCIF),
		VMSTATE_UINT32(BR3, ATSAM4LC4CBSCIF),

		VMSTATE_END_OF_LIST()
	}
};

static void atsam4lc4c_bscif_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsam4lc4c_bscif_vmstate;
	dc->realize = atsam4lc4c_bscif_realize;
	rc->phases.enter = atsam4lc4c_bscif_reset_enter;

}

static const TypeInfo atsam4lc4c_bscif_info = {
	.name = TYPE_ATSAM4LC4C_BSCIF,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAM4LC4CBSCIF),
	.instance_init = atsam4lc4c_bscif_init,
	.class_init = atsam4lc4c_bscif_class_init,
};

static void atsam4lc4c_bscif_register_types(void) {
	type_register_static(&atsam4lc4c_bscif_info);
}

type_init(atsam4lc4c_bscif_register_types);

#define TYPE_ATSAM4LC4C_PM "atsam4lc4c-pm"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAM4LC4CPM, ATSAM4LC4C_PM)

struct ATSAM4LC4CPM {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t AWEN;
	uint32_t CFDCTRL;
	uint32_t CONFIG;
	uint32_t CPUMASK;
	uint32_t CPUSEL;
	uint32_t FASTSLEEP;
	uint32_t HSBMASK;
	uint32_t ICR;
	uint32_t IDR;
	uint32_t IER;
	uint32_t IMR;
	uint32_t ISR;
	uint32_t MCCTRL;
	uint32_t OBS;
	uint32_t PBADIVMASK;
	uint32_t PBAMASK;
	uint32_t PBASEL;
	uint32_t PBBMASK;
	uint32_t PBBSEL;
	uint32_t PBCMASK;
	uint32_t PBCSEL;
	uint32_t PBDMASK;
	uint32_t PBDSEL;
	uint32_t PPCR;
	uint32_t RCAUSE;
	uint32_t SR;
	uint32_t UNLOCK;
	uint32_t VERSION;
	uint32_t WCAUSE;
	/* base */
	uint32_t base;

};
#define ATSAM4LC4C_PM_SIZE				0x400

REG32(PM_AWEN, 0x188)
	FIELD(PM_AWEN, AWEN, 0, 32)
REG32(PM_CFDCTRL, 0x54)
	FIELD(PM_CFDCTRL, CFDEN, 0, 1)
	FIELD(PM_CFDCTRL, SFV, 31, 1)
REG32(PM_CONFIG, 0x3f8)
	FIELD(PM_CONFIG, PBA, 0, 1)
	FIELD(PM_CONFIG, PBB, 1, 1)
	FIELD(PM_CONFIG, PBC, 2, 1)
	FIELD(PM_CONFIG, PBD, 3, 1)
	FIELD(PM_CONFIG, HSBPEVC, 7, 1)
REG32(PM_CPUMASK, 0x20)
	FIELD(PM_CPUMASK, OCD_, 0, 1)
REG32(PM_CPUSEL, 0x4)
	FIELD(PM_CPUSEL, CPUSEL, 0, 3)
	FIELD(PM_CPUSEL, CPUDIV, 7, 1)
REG32(PM_FASTSLEEP, 0x194)
	FIELD(PM_FASTSLEEP, OSC, 0, 1)
	FIELD(PM_FASTSLEEP, PLL, 8, 1)
	FIELD(PM_FASTSLEEP, FASTRCOSC, 16, 5)
	FIELD(PM_FASTSLEEP, DFLL, 24, 1)
REG32(PM_HSBMASK, 0x24)
	FIELD(PM_HSBMASK, PDCA_, 0, 1)
	FIELD(PM_HSBMASK, HFLASHC_, 1, 1)
	FIELD(PM_HSBMASK, HRAMC1_, 2, 1)
	FIELD(PM_HSBMASK, USBC_, 3, 1)
	FIELD(PM_HSBMASK, CRCCU_, 4, 1)
	FIELD(PM_HSBMASK, HTOP0_, 5, 1)
	FIELD(PM_HSBMASK, HTOP1_, 6, 1)
	FIELD(PM_HSBMASK, HTOP2_, 7, 1)
	FIELD(PM_HSBMASK, HTOP3_, 8, 1)
	FIELD(PM_HSBMASK, AESA_, 9, 1)
REG32(PM_ICR, 0xd0)
	FIELD(PM_ICR, CFD, 0, 1)
	FIELD(PM_ICR, CKRDY, 5, 1)
	FIELD(PM_ICR, WAKE, 8, 1)
	FIELD(PM_ICR, AE, 31, 1)
REG32(PM_IDR, 0xc4)
	FIELD(PM_IDR, CFD, 0, 1)
	FIELD(PM_IDR, CKRDY, 5, 1)
	FIELD(PM_IDR, WAKE, 8, 1)
	FIELD(PM_IDR, AE, 31, 1)
REG32(PM_IER, 0xc0)
	FIELD(PM_IER, CFD, 0, 1)
	FIELD(PM_IER, CKRDY, 5, 1)
	FIELD(PM_IER, WAKE, 8, 1)
	FIELD(PM_IER, AE, 31, 1)
REG32(PM_IMR, 0xc8)
	FIELD(PM_IMR, CFD, 0, 1)
	FIELD(PM_IMR, CKRDY, 5, 1)
	FIELD(PM_IMR, WAKE, 8, 1)
	FIELD(PM_IMR, AE, 31, 1)
REG32(PM_ISR, 0xcc)
	FIELD(PM_ISR, CFD, 0, 1)
	FIELD(PM_ISR, CKRDY, 5, 1)
	FIELD(PM_ISR, WAKE, 8, 1)
	FIELD(PM_ISR, AE, 31, 1)
REG32(PM_MCCTRL, 0x0)
	FIELD(PM_MCCTRL, MCSEL, 0, 3)
REG32(PM_OBS, 0x190)
REG32(PM_PBADIVMASK, 0x40)
REG32(PM_PBAMASK, 0x28)
	FIELD(PM_PBAMASK, IISC_, 0, 1)
	FIELD(PM_PBAMASK, SPI_, 1, 1)
	FIELD(PM_PBAMASK, TC0_, 2, 1)
	FIELD(PM_PBAMASK, TC1_, 3, 1)
	FIELD(PM_PBAMASK, TWIM0_, 4, 1)
	FIELD(PM_PBAMASK, TWIS0_, 5, 1)
	FIELD(PM_PBAMASK, TWIM1_, 6, 1)
	FIELD(PM_PBAMASK, TWIS1_, 7, 1)
	FIELD(PM_PBAMASK, USART0_, 8, 1)
	FIELD(PM_PBAMASK, USART1_, 9, 1)
	FIELD(PM_PBAMASK, USART2_, 10, 1)
	FIELD(PM_PBAMASK, USART3_, 11, 1)
	FIELD(PM_PBAMASK, ADCIFE_, 12, 1)
	FIELD(PM_PBAMASK, DACC_, 13, 1)
	FIELD(PM_PBAMASK, ACIFC_, 14, 1)
	FIELD(PM_PBAMASK, GLOC_, 15, 1)
	FIELD(PM_PBAMASK, ABDACB_, 16, 1)
	FIELD(PM_PBAMASK, TRNG_, 17, 1)
	FIELD(PM_PBAMASK, PARC_, 18, 1)
	FIELD(PM_PBAMASK, CATB_, 19, 1)
	FIELD(PM_PBAMASK, TWIM2_, 21, 1)
	FIELD(PM_PBAMASK, TWIM3_, 22, 1)
	FIELD(PM_PBAMASK, LCDCA_, 23, 1)
REG32(PM_PBASEL, 0xc)
	FIELD(PM_PBASEL, PBSEL, 0, 3)
	FIELD(PM_PBASEL, PBDIV, 7, 1)
REG32(PM_PBBMASK, 0x2c)
	FIELD(PM_PBBMASK, HFLASHC_, 0, 1)
	FIELD(PM_PBBMASK, HCACHE_, 1, 1)
	FIELD(PM_PBBMASK, HMATRIX_, 2, 1)
	FIELD(PM_PBBMASK, PDCA_, 3, 1)
	FIELD(PM_PBBMASK, CRCCU_, 4, 1)
	FIELD(PM_PBBMASK, USBC_, 5, 1)
	FIELD(PM_PBBMASK, PEVC_, 6, 1)
REG32(PM_PBBSEL, 0x10)
	FIELD(PM_PBBSEL, PBSEL, 0, 3)
	FIELD(PM_PBBSEL, PBDIV, 7, 1)
REG32(PM_PBCMASK, 0x30)
	FIELD(PM_PBCMASK, PM_, 0, 1)
	FIELD(PM_PBCMASK, CHIPID_, 1, 1)
	FIELD(PM_PBCMASK, SCIF_, 2, 1)
	FIELD(PM_PBCMASK, FREQM_, 3, 1)
	FIELD(PM_PBCMASK, GPIO_, 4, 1)
REG32(PM_PBCSEL, 0x14)
	FIELD(PM_PBCSEL, PBSEL, 0, 3)
	FIELD(PM_PBCSEL, PBDIV, 7, 1)
REG32(PM_PBDMASK, 0x34)
	FIELD(PM_PBDMASK, BPM_, 0, 1)
	FIELD(PM_PBDMASK, BSCIF_, 1, 1)
	FIELD(PM_PBDMASK, AST_, 2, 1)
	FIELD(PM_PBDMASK, WDT_, 3, 1)
	FIELD(PM_PBDMASK, EIC_, 4, 1)
	FIELD(PM_PBDMASK, PICOUART_, 5, 1)
REG32(PM_PBDSEL, 0x18)
	FIELD(PM_PBDSEL, PBSEL, 0, 3)
	FIELD(PM_PBDSEL, PBDIV, 7, 1)
REG32(PM_PPCR, 0x160)
	FIELD(PM_PPCR, RSTPUN, 0, 1)
	FIELD(PM_PPCR, CATBRCMASK, 1, 1)
	FIELD(PM_PPCR, ACIFCRCMASK, 2, 1)
	FIELD(PM_PPCR, ASTRCMASK, 3, 1)
	FIELD(PM_PPCR, TWIS0RCMASK, 4, 1)
	FIELD(PM_PPCR, TWIS1RCMASK, 5, 1)
	FIELD(PM_PPCR, PEVCRCMASK, 6, 1)
	FIELD(PM_PPCR, ADCIFERCMASK, 7, 1)
	FIELD(PM_PPCR, VREGRCMASK, 8, 1)
	FIELD(PM_PPCR, FWBGREF, 9, 1)
	FIELD(PM_PPCR, FWBOD18, 10, 1)
REG32(PM_RCAUSE, 0x180)
	FIELD(PM_RCAUSE, POR, 0, 1)
	FIELD(PM_RCAUSE, BOD, 1, 1)
	FIELD(PM_RCAUSE, EXT, 2, 1)
	FIELD(PM_RCAUSE, WDT, 3, 1)
	FIELD(PM_RCAUSE, OCDRST, 8, 1)
	FIELD(PM_RCAUSE, POR33, 10, 1)
	FIELD(PM_RCAUSE, BOD33, 13, 1)
REG32(PM_SR, 0xd4)
	FIELD(PM_SR, CFD, 0, 1)
	FIELD(PM_SR, OCP, 1, 1)
	FIELD(PM_SR, CKRDY, 5, 1)
	FIELD(PM_SR, WAKE, 8, 1)
	FIELD(PM_SR, PERRDY, 28, 1)
	FIELD(PM_SR, AE, 31, 1)
REG32(PM_UNLOCK, 0x58)
	FIELD(PM_UNLOCK, ADDR, 0, 10)
	FIELD(PM_UNLOCK, KEY, 24, 8)
REG32(PM_VERSION, 0x3fc)
	FIELD(PM_VERSION, VERSION, 0, 12)
	FIELD(PM_VERSION, VARIANT, 16, 4)
REG32(PM_WCAUSE, 0x184)
	FIELD(PM_WCAUSE, TWI_SLAVE_0, 0, 1)
	FIELD(PM_WCAUSE, TWI_SLAVE_1, 1, 1)
	FIELD(PM_WCAUSE, USBC, 2, 1)
	FIELD(PM_WCAUSE, PSOK, 3, 1)
	FIELD(PM_WCAUSE, BOD18_IRQ, 4, 1)
	FIELD(PM_WCAUSE, BOD33_IRQ, 5, 1)
	FIELD(PM_WCAUSE, PICOUART, 6, 1)
	FIELD(PM_WCAUSE, LCDCA, 7, 1)
	FIELD(PM_WCAUSE, EIC, 16, 1)
	FIELD(PM_WCAUSE, AST, 17, 1)


static void atsam4lc4c_pm_register_reset(ATSAM4LC4CPM *t) {
	t->CONFIG = 0xf;
	t->CPUMASK = 0x1;
	t->HSBMASK = 0x1e2;
	t->PBBMASK = 0x1;
	t->PBCMASK = 0x1f;
	t->PBDMASK = 0x3f;
	t->PPCR = 0x1fe;
	t->VERSION = 0x441;

}

static uint64_t atsam4lc4c_pm_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAM4LC4CPM *t = ATSAM4LC4C_PM(opaque);
	uint64_t ret;

	switch (offset) {
		case A_PM_AWEN:
			ret = t->AWEN;
			break;
		case A_PM_CFDCTRL:
			ret = t->CFDCTRL;
			break;
		case A_PM_CONFIG:
			ret = t->CONFIG;
			break;
		case A_PM_CPUMASK:
			ret = t->CPUMASK;
			break;
		case A_PM_CPUSEL:
			ret = t->CPUSEL;
			break;
		case A_PM_FASTSLEEP:
			ret = t->FASTSLEEP;
			break;
		case A_PM_HSBMASK:
			ret = t->HSBMASK;
			break;
		case A_PM_IMR:
			ret = t->IMR;
			break;
		case A_PM_ISR:
			ret = t->ISR;
			break;
		case A_PM_MCCTRL:
			ret = t->MCCTRL;
			break;
		case A_PM_PBAMASK:
			ret = t->PBAMASK;
			break;
		case A_PM_PBASEL:
			ret = t->PBASEL;
			break;
		case A_PM_PBBMASK:
			ret = t->PBBMASK;
			break;
		case A_PM_PBBSEL:
			ret = t->PBBSEL;
			break;
		case A_PM_PBCMASK:
			ret = t->PBCMASK;
			break;
		case A_PM_PBCSEL:
			ret = t->PBCSEL;
			break;
		case A_PM_PBDMASK:
			ret = t->PBDMASK;
			break;
		case A_PM_PBDSEL:
			ret = t->PBDSEL;
			break;
		case A_PM_PPCR:
			ret = t->PPCR;
			break;
		case A_PM_RCAUSE:
			ret = t->RCAUSE;
			break;
		case A_PM_SR:
			ret = t->SR;
			break;
		case A_PM_VERSION:
			ret = t->VERSION;
			break;
		case A_PM_WCAUSE:
			ret = t->WCAUSE;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM4LC4C PM read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsam4lc4c_pm_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAM4LC4CPM *t = ATSAM4LC4C_PM(opaque);


	switch (offset) {
		case A_PM_AWEN ... A_PM_AWEN + 3:
			value = value << ((offset - A_PM_AWEN) << 3);
			offset = A_PM_AWEN;
			break;
		case A_PM_CFDCTRL ... A_PM_CFDCTRL + 3:
			value = value << ((offset - A_PM_CFDCTRL) << 3);
			offset = A_PM_CFDCTRL;
			break;
		case A_PM_CPUMASK ... A_PM_CPUMASK + 3:
			value = value << ((offset - A_PM_CPUMASK) << 3);
			offset = A_PM_CPUMASK;
			break;
		case A_PM_CPUSEL ... A_PM_CPUSEL + 3:
			value = value << ((offset - A_PM_CPUSEL) << 3);
			offset = A_PM_CPUSEL;
			break;
		case A_PM_FASTSLEEP ... A_PM_FASTSLEEP + 3:
			value = value << ((offset - A_PM_FASTSLEEP) << 3);
			offset = A_PM_FASTSLEEP;
			break;
		case A_PM_HSBMASK ... A_PM_HSBMASK + 3:
			value = value << ((offset - A_PM_HSBMASK) << 3);
			offset = A_PM_HSBMASK;
			break;
		case A_PM_ICR ... A_PM_ICR + 3:
			value = value << ((offset - A_PM_ICR) << 3);
			offset = A_PM_ICR;
			break;
		case A_PM_IDR ... A_PM_IDR + 3:
			value = value << ((offset - A_PM_IDR) << 3);
			offset = A_PM_IDR;
			break;
		case A_PM_IER ... A_PM_IER + 3:
			value = value << ((offset - A_PM_IER) << 3);
			offset = A_PM_IER;
			break;
		case A_PM_MCCTRL ... A_PM_MCCTRL + 3:
			value = value << ((offset - A_PM_MCCTRL) << 3);
			offset = A_PM_MCCTRL;
			break;
		case A_PM_PBAMASK ... A_PM_PBAMASK + 3:
			value = value << ((offset - A_PM_PBAMASK) << 3);
			offset = A_PM_PBAMASK;
			break;
		case A_PM_PBASEL ... A_PM_PBASEL + 3:
			value = value << ((offset - A_PM_PBASEL) << 3);
			offset = A_PM_PBASEL;
			break;
		case A_PM_PBBMASK ... A_PM_PBBMASK + 3:
			value = value << ((offset - A_PM_PBBMASK) << 3);
			offset = A_PM_PBBMASK;
			break;
		case A_PM_PBBSEL ... A_PM_PBBSEL + 3:
			value = value << ((offset - A_PM_PBBSEL) << 3);
			offset = A_PM_PBBSEL;
			break;
		case A_PM_PBCMASK ... A_PM_PBCMASK + 3:
			value = value << ((offset - A_PM_PBCMASK) << 3);
			offset = A_PM_PBCMASK;
			break;
		case A_PM_PBCSEL ... A_PM_PBCSEL + 3:
			value = value << ((offset - A_PM_PBCSEL) << 3);
			offset = A_PM_PBCSEL;
			break;
		case A_PM_PBDMASK ... A_PM_PBDMASK + 3:
			value = value << ((offset - A_PM_PBDMASK) << 3);
			offset = A_PM_PBDMASK;
			break;
		case A_PM_PBDSEL ... A_PM_PBDSEL + 3:
			value = value << ((offset - A_PM_PBDSEL) << 3);
			offset = A_PM_PBDSEL;
			break;
		case A_PM_PPCR ... A_PM_PPCR + 3:
			value = value << ((offset - A_PM_PPCR) << 3);
			offset = A_PM_PPCR;
			break;
		case A_PM_UNLOCK ... A_PM_UNLOCK + 3:
			value = value << ((offset - A_PM_UNLOCK) << 3);
			offset = A_PM_UNLOCK;
			break;

		default: break;
	}

	switch (offset) {
		case A_PM_AWEN:
			t->AWEN = value;
			break;
		case A_PM_CFDCTRL:
			t->CFDCTRL = value;
			break;
		case A_PM_CPUMASK:
			t->CPUMASK = value;
			break;
		case A_PM_CPUSEL:
			t->CPUSEL = value;
			break;
		case A_PM_FASTSLEEP:
			t->FASTSLEEP = value;
			break;
		case A_PM_HSBMASK:
			t->HSBMASK = value;
			break;
		case A_PM_ICR:
			t->ICR = value;
			break;
		case A_PM_IDR:
			t->IDR = value;
			break;
		case A_PM_IER:
			t->IER = value;
			break;
		case A_PM_MCCTRL:
			t->MCCTRL = value;
			break;
		case A_PM_PBAMASK:
			t->PBAMASK = value;
			break;
		case A_PM_PBASEL:
			t->PBASEL = value;
			break;
		case A_PM_PBBMASK:
			t->PBBMASK = value;
			break;
		case A_PM_PBBSEL:
			t->PBBSEL = value;
			break;
		case A_PM_PBCMASK:
			t->PBCMASK = value;
			break;
		case A_PM_PBCSEL:
			t->PBCSEL = value;
			break;
		case A_PM_PBDMASK:
			t->PBDMASK = value;
			break;
		case A_PM_PBDSEL:
			t->PBDSEL = value;
			break;
		case A_PM_PPCR:
			t->PPCR = value;
			break;
		case A_PM_UNLOCK:
			t->UNLOCK = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM4LC4C PM write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsam4lc4c_pm_ops = {
	.read = atsam4lc4c_pm_read,
	.write = atsam4lc4c_pm_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsam4lc4c_pm_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAM4LC4CPM *t = ATSAM4LC4C_PM(obj);
	memory_region_init_io(&(t->iomem), obj, &atsam4lc4c_pm_ops, t, "atsam4lc4c-pm", ATSAM4LC4C_PM_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void atsam4lc4c_pm_realize(DeviceState *dev, Error **errp) {
	return;

}

static void atsam4lc4c_pm_reset_enter(Object *obj, ResetType type) {
	ATSAM4LC4CPM *t = ATSAM4LC4C_PM(obj);
	atsam4lc4c_pm_register_reset(t);
}

static const VMStateDescription atsam4lc4c_pm_vmstate = {
	.name = "atsam4lc4c-pm",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(AWEN, ATSAM4LC4CPM),
		VMSTATE_UINT32(CFDCTRL, ATSAM4LC4CPM),
		VMSTATE_UINT32(CONFIG, ATSAM4LC4CPM),
		VMSTATE_UINT32(CPUMASK, ATSAM4LC4CPM),
		VMSTATE_UINT32(CPUSEL, ATSAM4LC4CPM),
		VMSTATE_UINT32(FASTSLEEP, ATSAM4LC4CPM),
		VMSTATE_UINT32(HSBMASK, ATSAM4LC4CPM),
		VMSTATE_UINT32(ICR, ATSAM4LC4CPM),
		VMSTATE_UINT32(IDR, ATSAM4LC4CPM),
		VMSTATE_UINT32(IER, ATSAM4LC4CPM),
		VMSTATE_UINT32(IMR, ATSAM4LC4CPM),
		VMSTATE_UINT32(ISR, ATSAM4LC4CPM),
		VMSTATE_UINT32(MCCTRL, ATSAM4LC4CPM),
		VMSTATE_UINT32(OBS, ATSAM4LC4CPM),
		VMSTATE_UINT32(PBADIVMASK, ATSAM4LC4CPM),
		VMSTATE_UINT32(PBAMASK, ATSAM4LC4CPM),
		VMSTATE_UINT32(PBASEL, ATSAM4LC4CPM),
		VMSTATE_UINT32(PBBMASK, ATSAM4LC4CPM),
		VMSTATE_UINT32(PBBSEL, ATSAM4LC4CPM),
		VMSTATE_UINT32(PBCMASK, ATSAM4LC4CPM),
		VMSTATE_UINT32(PBCSEL, ATSAM4LC4CPM),
		VMSTATE_UINT32(PBDMASK, ATSAM4LC4CPM),
		VMSTATE_UINT32(PBDSEL, ATSAM4LC4CPM),
		VMSTATE_UINT32(PPCR, ATSAM4LC4CPM),
		VMSTATE_UINT32(RCAUSE, ATSAM4LC4CPM),
		VMSTATE_UINT32(SR, ATSAM4LC4CPM),
		VMSTATE_UINT32(UNLOCK, ATSAM4LC4CPM),
		VMSTATE_UINT32(VERSION, ATSAM4LC4CPM),
		VMSTATE_UINT32(WCAUSE, ATSAM4LC4CPM),

		VMSTATE_END_OF_LIST()
	}
};

static void atsam4lc4c_pm_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsam4lc4c_pm_vmstate;
	dc->realize = atsam4lc4c_pm_realize;
	rc->phases.enter = atsam4lc4c_pm_reset_enter;

}

static const TypeInfo atsam4lc4c_pm_info = {
	.name = TYPE_ATSAM4LC4C_PM,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAM4LC4CPM),
	.instance_init = atsam4lc4c_pm_init,
	.class_init = atsam4lc4c_pm_class_init,
};

static void atsam4lc4c_pm_register_types(void) {
	type_register_static(&atsam4lc4c_pm_info);
}

type_init(atsam4lc4c_pm_register_types);

#define TYPE_ATSAM4LC4C_HFLASHC "atsam4lc4c-hflashc"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAM4LC4CHFLASHC, ATSAM4LC4C_HFLASHC)

struct ATSAM4LC4CHFLASHC {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t FCMD;
	uint32_t FCR;
	uint32_t FGPFRHI;
	uint32_t FGPFRLO;
	uint32_t FPR;
	uint32_t FSR;
	uint32_t VERSION;
	/* base */
	uint32_t base;

};
#define ATSAM4LC4C_HFLASHC_SIZE				0x1c

REG32(HFLASHC_FCMD, 0x4)
	FIELD(HFLASHC_FCMD, CMD, 0, 6)
	FIELD(HFLASHC_FCMD, PAGEN, 8, 16)
	FIELD(HFLASHC_FCMD, KEY, 24, 8)
REG32(HFLASHC_FCR, 0x0)
	FIELD(HFLASHC_FCR, FRDY, 0, 1)
	FIELD(HFLASHC_FCR, LOCKE, 2, 1)
	FIELD(HFLASHC_FCR, PROGE, 3, 1)
	FIELD(HFLASHC_FCR, FWS, 6, 1)
	FIELD(HFLASHC_FCR, WS1OPT, 7, 1)
REG32(HFLASHC_FGPFRHI, 0x14)
	FIELD(HFLASHC_FGPFRHI, GPF32, 0, 1)
	FIELD(HFLASHC_FGPFRHI, GPF33, 1, 1)
	FIELD(HFLASHC_FGPFRHI, GPF34, 2, 1)
	FIELD(HFLASHC_FGPFRHI, GPF35, 3, 1)
	FIELD(HFLASHC_FGPFRHI, GPF36, 4, 1)
	FIELD(HFLASHC_FGPFRHI, GPF37, 5, 1)
	FIELD(HFLASHC_FGPFRHI, GPF38, 6, 1)
	FIELD(HFLASHC_FGPFRHI, GPF39, 7, 1)
	FIELD(HFLASHC_FGPFRHI, GPF40, 8, 1)
	FIELD(HFLASHC_FGPFRHI, GPF41, 9, 1)
	FIELD(HFLASHC_FGPFRHI, GPF42, 10, 1)
	FIELD(HFLASHC_FGPFRHI, GPF43, 11, 1)
	FIELD(HFLASHC_FGPFRHI, GPF44, 12, 1)
	FIELD(HFLASHC_FGPFRHI, GPF45, 13, 1)
	FIELD(HFLASHC_FGPFRHI, GPF46, 14, 1)
	FIELD(HFLASHC_FGPFRHI, GPF47, 15, 1)
	FIELD(HFLASHC_FGPFRHI, GPF48, 16, 1)
	FIELD(HFLASHC_FGPFRHI, GPF49, 17, 1)
	FIELD(HFLASHC_FGPFRHI, GPF50, 18, 1)
	FIELD(HFLASHC_FGPFRHI, GPF51, 19, 1)
	FIELD(HFLASHC_FGPFRHI, GPF52, 20, 1)
	FIELD(HFLASHC_FGPFRHI, GPF53, 21, 1)
	FIELD(HFLASHC_FGPFRHI, GPF54, 22, 1)
	FIELD(HFLASHC_FGPFRHI, GPF55, 23, 1)
	FIELD(HFLASHC_FGPFRHI, GPF56, 24, 1)
	FIELD(HFLASHC_FGPFRHI, GPF57, 25, 1)
	FIELD(HFLASHC_FGPFRHI, GPF58, 26, 1)
	FIELD(HFLASHC_FGPFRHI, GPF59, 27, 1)
	FIELD(HFLASHC_FGPFRHI, GPF60, 28, 1)
	FIELD(HFLASHC_FGPFRHI, GPF61, 29, 1)
	FIELD(HFLASHC_FGPFRHI, GPF62, 30, 1)
	FIELD(HFLASHC_FGPFRHI, GPF63, 31, 1)
REG32(HFLASHC_FGPFRLO, 0x18)
	FIELD(HFLASHC_FGPFRLO, LOCK0, 0, 1)
	FIELD(HFLASHC_FGPFRLO, LOCK1, 1, 1)
	FIELD(HFLASHC_FGPFRLO, LOCK2, 2, 1)
	FIELD(HFLASHC_FGPFRLO, LOCK3, 3, 1)
	FIELD(HFLASHC_FGPFRLO, LOCK4, 4, 1)
	FIELD(HFLASHC_FGPFRLO, LOCK5, 5, 1)
	FIELD(HFLASHC_FGPFRLO, LOCK6, 6, 1)
	FIELD(HFLASHC_FGPFRLO, LOCK7, 7, 1)
	FIELD(HFLASHC_FGPFRLO, LOCK8, 8, 1)
	FIELD(HFLASHC_FGPFRLO, LOCK9, 9, 1)
	FIELD(HFLASHC_FGPFRLO, LOCK10, 10, 1)
	FIELD(HFLASHC_FGPFRLO, LOCK11, 11, 1)
	FIELD(HFLASHC_FGPFRLO, LOCK12, 12, 1)
	FIELD(HFLASHC_FGPFRLO, LOCK13, 13, 1)
	FIELD(HFLASHC_FGPFRLO, LOCK14, 14, 1)
	FIELD(HFLASHC_FGPFRLO, LOCK15, 15, 1)
	FIELD(HFLASHC_FGPFRLO, GPF16, 16, 1)
	FIELD(HFLASHC_FGPFRLO, GPF17, 17, 1)
	FIELD(HFLASHC_FGPFRLO, GPF18, 18, 1)
	FIELD(HFLASHC_FGPFRLO, GPF19, 19, 1)
	FIELD(HFLASHC_FGPFRLO, GPF20, 20, 1)
	FIELD(HFLASHC_FGPFRLO, GPF21, 21, 1)
	FIELD(HFLASHC_FGPFRLO, GPF22, 22, 1)
	FIELD(HFLASHC_FGPFRLO, GPF23, 23, 1)
	FIELD(HFLASHC_FGPFRLO, GPF24, 24, 1)
	FIELD(HFLASHC_FGPFRLO, GPF25, 25, 1)
	FIELD(HFLASHC_FGPFRLO, GPF26, 26, 1)
	FIELD(HFLASHC_FGPFRLO, GPF27, 27, 1)
	FIELD(HFLASHC_FGPFRLO, GPF28, 28, 1)
	FIELD(HFLASHC_FGPFRLO, GPF29, 29, 1)
	FIELD(HFLASHC_FGPFRLO, GPF30, 30, 1)
	FIELD(HFLASHC_FGPFRLO, GPF31, 31, 1)
REG32(HFLASHC_FPR, 0xc)
	FIELD(HFLASHC_FPR, FSZ, 0, 4)
	FIELD(HFLASHC_FPR, PSZ, 8, 3)
REG32(HFLASHC_FSR, 0x8)
	FIELD(HFLASHC_FSR, FRDY, 0, 1)
	FIELD(HFLASHC_FSR, LOCKE, 2, 1)
	FIELD(HFLASHC_FSR, PROGE, 3, 1)
	FIELD(HFLASHC_FSR, SECURITY, 4, 1)
	FIELD(HFLASHC_FSR, QPRR, 5, 1)
	FIELD(HFLASHC_FSR, HSMODE, 6, 1)
	FIELD(HFLASHC_FSR, ECCERR, 8, 2)
	FIELD(HFLASHC_FSR, LOCK0, 16, 1)
	FIELD(HFLASHC_FSR, LOCK1, 17, 1)
	FIELD(HFLASHC_FSR, LOCK2, 18, 1)
	FIELD(HFLASHC_FSR, LOCK3, 19, 1)
	FIELD(HFLASHC_FSR, LOCK4, 20, 1)
	FIELD(HFLASHC_FSR, LOCK5, 21, 1)
	FIELD(HFLASHC_FSR, LOCK6, 22, 1)
	FIELD(HFLASHC_FSR, LOCK7, 23, 1)
	FIELD(HFLASHC_FSR, LOCK8, 24, 1)
	FIELD(HFLASHC_FSR, LOCK9, 25, 1)
	FIELD(HFLASHC_FSR, LOCK10, 26, 1)
	FIELD(HFLASHC_FSR, LOCK11, 27, 1)
	FIELD(HFLASHC_FSR, LOCK12, 28, 1)
	FIELD(HFLASHC_FSR, LOCK13, 29, 1)
	FIELD(HFLASHC_FSR, LOCK14, 30, 1)
	FIELD(HFLASHC_FSR, LOCK15, 31, 1)
REG32(HFLASHC_VERSION, 0x10)
	FIELD(HFLASHC_VERSION, VERSION, 0, 12)
	FIELD(HFLASHC_VERSION, VARIANT, 16, 4)


static void atsam4lc4c_hflashc_register_reset(ATSAM4LC4CHFLASHC *t) {
	t->VERSION = 0x110;

}

static uint64_t atsam4lc4c_hflashc_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAM4LC4CHFLASHC *t = ATSAM4LC4C_HFLASHC(opaque);
	uint64_t ret;

	switch (offset) {
		case A_HFLASHC_FCMD:
			ret = t->FCMD;
			break;
		case A_HFLASHC_FCR:
			ret = t->FCR;
			break;
		case A_HFLASHC_FGPFRHI:
			ret = t->FGPFRHI;
			break;
		case A_HFLASHC_FGPFRLO:
			ret = t->FGPFRLO;
			break;
		case A_HFLASHC_FPR:
			ret = t->FPR;
			break;
		case A_HFLASHC_FSR:
			ret = t->FSR;
			break;
		case A_HFLASHC_VERSION:
			ret = t->VERSION;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM4LC4C HFLASHC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsam4lc4c_hflashc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAM4LC4CHFLASHC *t = ATSAM4LC4C_HFLASHC(opaque);


	switch (offset) {
		case A_HFLASHC_FCMD ... A_HFLASHC_FCMD + 3:
			value = value << ((offset - A_HFLASHC_FCMD) << 3);
			offset = A_HFLASHC_FCMD;
			break;
		case A_HFLASHC_FCR ... A_HFLASHC_FCR + 3:
			value = value << ((offset - A_HFLASHC_FCR) << 3);
			offset = A_HFLASHC_FCR;
			break;

		default: break;
	}

	switch (offset) {
		case A_HFLASHC_FCMD:
			t->FCMD = value;
			break;
		case A_HFLASHC_FCR:
			t->FCR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM4LC4C HFLASHC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsam4lc4c_hflashc_ops = {
	.read = atsam4lc4c_hflashc_read,
	.write = atsam4lc4c_hflashc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsam4lc4c_hflashc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAM4LC4CHFLASHC *t = ATSAM4LC4C_HFLASHC(obj);
	memory_region_init_io(&(t->iomem), obj, &atsam4lc4c_hflashc_ops, t, "atsam4lc4c-hflashc", ATSAM4LC4C_HFLASHC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void atsam4lc4c_hflashc_realize(DeviceState *dev, Error **errp) {
	return;

}

static void atsam4lc4c_hflashc_reset_enter(Object *obj, ResetType type) {
	ATSAM4LC4CHFLASHC *t = ATSAM4LC4C_HFLASHC(obj);
	atsam4lc4c_hflashc_register_reset(t);
}

static const VMStateDescription atsam4lc4c_hflashc_vmstate = {
	.name = "atsam4lc4c-hflashc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(FCMD, ATSAM4LC4CHFLASHC),
		VMSTATE_UINT32(FCR, ATSAM4LC4CHFLASHC),
		VMSTATE_UINT32(FGPFRHI, ATSAM4LC4CHFLASHC),
		VMSTATE_UINT32(FGPFRLO, ATSAM4LC4CHFLASHC),
		VMSTATE_UINT32(FPR, ATSAM4LC4CHFLASHC),
		VMSTATE_UINT32(FSR, ATSAM4LC4CHFLASHC),
		VMSTATE_UINT32(VERSION, ATSAM4LC4CHFLASHC),

		VMSTATE_END_OF_LIST()
	}
};

static void atsam4lc4c_hflashc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsam4lc4c_hflashc_vmstate;
	dc->realize = atsam4lc4c_hflashc_realize;
	rc->phases.enter = atsam4lc4c_hflashc_reset_enter;

}

static const TypeInfo atsam4lc4c_hflashc_info = {
	.name = TYPE_ATSAM4LC4C_HFLASHC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAM4LC4CHFLASHC),
	.instance_init = atsam4lc4c_hflashc_init,
	.class_init = atsam4lc4c_hflashc_class_init,
};

static void atsam4lc4c_hflashc_register_types(void) {
	type_register_static(&atsam4lc4c_hflashc_info);
}

type_init(atsam4lc4c_hflashc_register_types);

#define TYPE_ATSAM4LC4C_GPIO "atsam4lc4c-gpio"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAM4LC4CGPIO, ATSAM4LC4C_GPIO)

struct ATSAM4LC4CGPIO {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[12];

	/*registers*/
	uint32_t ASR0;
	uint32_t ASR1;
	uint32_t ASR2;
	uint32_t EVERC0;
	uint32_t EVERC1;
	uint32_t EVERC2;
	uint32_t EVERS0;
	uint32_t EVERS1;
	uint32_t EVERS2;
	uint32_t EVERT0;
	uint32_t EVERT1;
	uint32_t EVERT2;
	uint32_t EVER0;
	uint32_t EVER1;
	uint32_t EVER2;
	uint32_t GFERC0;
	uint32_t GFERC1;
	uint32_t GFERC2;
	uint32_t GFERS0;
	uint32_t GFERS1;
	uint32_t GFERS2;
	uint32_t GFERT0;
	uint32_t GFERT1;
	uint32_t GFERT2;
	uint32_t GFER0;
	uint32_t GFER1;
	uint32_t GFER2;
	uint32_t GPERC0;
	uint32_t GPERC1;
	uint32_t GPERC2;
	uint32_t GPERS0;
	uint32_t GPERS1;
	uint32_t GPERS2;
	uint32_t GPERT0;
	uint32_t GPERT1;
	uint32_t GPERT2;
	uint32_t GPER0;
	uint32_t GPER1;
	uint32_t GPER2;
	uint32_t IERC0;
	uint32_t IERC1;
	uint32_t IERC2;
	uint32_t IERS0;
	uint32_t IERS1;
	uint32_t IERS2;
	uint32_t IERT0;
	uint32_t IERT1;
	uint32_t IERT2;
	uint32_t IER0;
	uint32_t IER1;
	uint32_t IER2;
	uint32_t IFRC0;
	uint32_t IFRC1;
	uint32_t IFRC2;
	uint32_t IFR0;
	uint32_t IFR1;
	uint32_t IFR2;
	uint32_t IMR0C0;
	uint32_t IMR0C1;
	uint32_t IMR0C2;
	uint32_t IMR0S0;
	uint32_t IMR0S1;
	uint32_t IMR0S2;
	uint32_t IMR0T0;
	uint32_t IMR0T1;
	uint32_t IMR0T2;
	uint32_t IMR1C0;
	uint32_t IMR1C1;
	uint32_t IMR1C2;
	uint32_t IMR1S0;
	uint32_t IMR1S1;
	uint32_t IMR1S2;
	uint32_t IMR1T0;
	uint32_t IMR1T1;
	uint32_t IMR1T2;
	uint32_t IMR00;
	uint32_t IMR01;
	uint32_t IMR02;
	uint32_t IMR10;
	uint32_t IMR11;
	uint32_t IMR12;
	uint32_t LOCKC0;
	uint32_t LOCKC1;
	uint32_t LOCKC2;
	uint32_t LOCKS0;
	uint32_t LOCKS1;
	uint32_t LOCKS2;
	uint32_t LOCKT0;
	uint32_t LOCKT1;
	uint32_t LOCKT2;
	uint32_t LOCK0;
	uint32_t LOCK1;
	uint32_t LOCK2;
	uint32_t ODCR0C0;
	uint32_t ODCR0C1;
	uint32_t ODCR0C2;
	uint32_t ODCR0S0;
	uint32_t ODCR0S1;
	uint32_t ODCR0S2;
	uint32_t ODCR0T0;
	uint32_t ODCR0T1;
	uint32_t ODCR0T2;
	uint32_t ODCR1C0;
	uint32_t ODCR1C1;
	uint32_t ODCR1C2;
	uint32_t ODCR1S0;
	uint32_t ODCR1S1;
	uint32_t ODCR1S2;
	uint32_t ODCR1T0;
	uint32_t ODCR1T1;
	uint32_t ODCR1T2;
	uint32_t ODCR00;
	uint32_t ODCR01;
	uint32_t ODCR02;
	uint32_t ODCR10;
	uint32_t ODCR11;
	uint32_t ODCR12;
	uint32_t ODERC0;
	uint32_t ODERC1;
	uint32_t ODERC2;
	uint32_t ODERS0;
	uint32_t ODERS1;
	uint32_t ODERS2;
	uint32_t ODERT0;
	uint32_t ODERT1;
	uint32_t ODERT2;
	uint32_t ODER0;
	uint32_t ODER1;
	uint32_t ODER2;
	uint32_t ODMERC0;
	uint32_t ODMERC1;
	uint32_t ODMERC2;
	uint32_t ODMERS0;
	uint32_t ODMERS1;
	uint32_t ODMERS2;
	uint32_t ODMERT0;
	uint32_t ODMERT1;
	uint32_t ODMERT2;
	uint32_t ODMER0;
	uint32_t ODMER1;
	uint32_t ODMER2;
	uint32_t OSRR0C0;
	uint32_t OSRR0C1;
	uint32_t OSRR0C2;
	uint32_t OSRR0S0;
	uint32_t OSRR0S1;
	uint32_t OSRR0S2;
	uint32_t OSRR0T0;
	uint32_t OSRR0T1;
	uint32_t OSRR0T2;
	uint32_t OSRR00;
	uint32_t OSRR01;
	uint32_t OSRR02;
	uint32_t OVRC0;
	uint32_t OVRC1;
	uint32_t OVRC2;
	uint32_t OVRS0;
	uint32_t OVRS1;
	uint32_t OVRS2;
	uint32_t OVRT0;
	uint32_t OVRT1;
	uint32_t OVRT2;
	uint32_t OVR0;
	uint32_t OVR1;
	uint32_t OVR2;
	uint32_t PARAMETER0;
	uint32_t PARAMETER1;
	uint32_t PARAMETER2;
	uint32_t PDERC0;
	uint32_t PDERC1;
	uint32_t PDERC2;
	uint32_t PDERS0;
	uint32_t PDERS1;
	uint32_t PDERS2;
	uint32_t PDERT0;
	uint32_t PDERT1;
	uint32_t PDERT2;
	uint32_t PDER0;
	uint32_t PDER1;
	uint32_t PDER2;
	uint32_t PMR0C0;
	uint32_t PMR0C1;
	uint32_t PMR0C2;
	uint32_t PMR0S0;
	uint32_t PMR0S1;
	uint32_t PMR0S2;
	uint32_t PMR0T0;
	uint32_t PMR0T1;
	uint32_t PMR0T2;
	uint32_t PMR1C0;
	uint32_t PMR1C1;
	uint32_t PMR1C2;
	uint32_t PMR1S0;
	uint32_t PMR1S1;
	uint32_t PMR1S2;
	uint32_t PMR1T0;
	uint32_t PMR1T1;
	uint32_t PMR1T2;
	uint32_t PMR2C0;
	uint32_t PMR2C1;
	uint32_t PMR2C2;
	uint32_t PMR2S0;
	uint32_t PMR2S1;
	uint32_t PMR2S2;
	uint32_t PMR2T0;
	uint32_t PMR2T1;
	uint32_t PMR2T2;
	uint32_t PMR00;
	uint32_t PMR01;
	uint32_t PMR02;
	uint32_t PMR10;
	uint32_t PMR11;
	uint32_t PMR12;
	uint32_t PMR20;
	uint32_t PMR21;
	uint32_t PMR22;
	uint32_t PUERC0;
	uint32_t PUERC1;
	uint32_t PUERC2;
	uint32_t PUERS0;
	uint32_t PUERS1;
	uint32_t PUERS2;
	uint32_t PUERT0;
	uint32_t PUERT1;
	uint32_t PUERT2;
	uint32_t PUER0;
	uint32_t PUER1;
	uint32_t PUER2;
	uint32_t PVR0;
	uint32_t PVR1;
	uint32_t PVR2;
	uint32_t STERC0;
	uint32_t STERC1;
	uint32_t STERC2;
	uint32_t STERS0;
	uint32_t STERS1;
	uint32_t STERS2;
	uint32_t STERT0;
	uint32_t STERT1;
	uint32_t STERT2;
	uint32_t STER0;
	uint32_t STER1;
	uint32_t STER2;
	uint32_t UNLOCK0;
	uint32_t UNLOCK1;
	uint32_t UNLOCK2;
	uint32_t VERSION0;
	uint32_t VERSION1;
	uint32_t VERSION2;
	/* base */
	uint32_t base;

};
#define ATSAM4LC4C_GPIO_SIZE				0x600

REG32(GPIO_ASR0, 0x1e4)
	FIELD(GPIO_ASR0, AR, 0, 1)
REG32(GPIO_ASR1, 0x3e4)
	FIELD(GPIO_ASR1, AR, 0, 1)
REG32(GPIO_ASR2, 0x5e4)
	FIELD(GPIO_ASR2, AR, 0, 1)
REG32(GPIO_EVERC0, 0x188)
	FIELD(GPIO_EVERC0, P0, 0, 1)
	FIELD(GPIO_EVERC0, P1, 1, 1)
	FIELD(GPIO_EVERC0, P2, 2, 1)
	FIELD(GPIO_EVERC0, P3, 3, 1)
	FIELD(GPIO_EVERC0, P4, 4, 1)
	FIELD(GPIO_EVERC0, P5, 5, 1)
	FIELD(GPIO_EVERC0, P6, 6, 1)
	FIELD(GPIO_EVERC0, P7, 7, 1)
	FIELD(GPIO_EVERC0, P8, 8, 1)
	FIELD(GPIO_EVERC0, P9, 9, 1)
	FIELD(GPIO_EVERC0, P10, 10, 1)
	FIELD(GPIO_EVERC0, P11, 11, 1)
	FIELD(GPIO_EVERC0, P12, 12, 1)
	FIELD(GPIO_EVERC0, P13, 13, 1)
	FIELD(GPIO_EVERC0, P14, 14, 1)
	FIELD(GPIO_EVERC0, P15, 15, 1)
	FIELD(GPIO_EVERC0, P16, 16, 1)
	FIELD(GPIO_EVERC0, P17, 17, 1)
	FIELD(GPIO_EVERC0, P18, 18, 1)
	FIELD(GPIO_EVERC0, P19, 19, 1)
	FIELD(GPIO_EVERC0, P20, 20, 1)
	FIELD(GPIO_EVERC0, P21, 21, 1)
	FIELD(GPIO_EVERC0, P22, 22, 1)
	FIELD(GPIO_EVERC0, P23, 23, 1)
	FIELD(GPIO_EVERC0, P24, 24, 1)
	FIELD(GPIO_EVERC0, P25, 25, 1)
	FIELD(GPIO_EVERC0, P26, 26, 1)
	FIELD(GPIO_EVERC0, P27, 27, 1)
	FIELD(GPIO_EVERC0, P28, 28, 1)
	FIELD(GPIO_EVERC0, P29, 29, 1)
	FIELD(GPIO_EVERC0, P30, 30, 1)
	FIELD(GPIO_EVERC0, P31, 31, 1)
REG32(GPIO_EVERC1, 0x388)
	FIELD(GPIO_EVERC1, P0, 0, 1)
	FIELD(GPIO_EVERC1, P1, 1, 1)
	FIELD(GPIO_EVERC1, P2, 2, 1)
	FIELD(GPIO_EVERC1, P3, 3, 1)
	FIELD(GPIO_EVERC1, P4, 4, 1)
	FIELD(GPIO_EVERC1, P5, 5, 1)
	FIELD(GPIO_EVERC1, P6, 6, 1)
	FIELD(GPIO_EVERC1, P7, 7, 1)
	FIELD(GPIO_EVERC1, P8, 8, 1)
	FIELD(GPIO_EVERC1, P9, 9, 1)
	FIELD(GPIO_EVERC1, P10, 10, 1)
	FIELD(GPIO_EVERC1, P11, 11, 1)
	FIELD(GPIO_EVERC1, P12, 12, 1)
	FIELD(GPIO_EVERC1, P13, 13, 1)
	FIELD(GPIO_EVERC1, P14, 14, 1)
	FIELD(GPIO_EVERC1, P15, 15, 1)
	FIELD(GPIO_EVERC1, P16, 16, 1)
	FIELD(GPIO_EVERC1, P17, 17, 1)
	FIELD(GPIO_EVERC1, P18, 18, 1)
	FIELD(GPIO_EVERC1, P19, 19, 1)
	FIELD(GPIO_EVERC1, P20, 20, 1)
	FIELD(GPIO_EVERC1, P21, 21, 1)
	FIELD(GPIO_EVERC1, P22, 22, 1)
	FIELD(GPIO_EVERC1, P23, 23, 1)
	FIELD(GPIO_EVERC1, P24, 24, 1)
	FIELD(GPIO_EVERC1, P25, 25, 1)
	FIELD(GPIO_EVERC1, P26, 26, 1)
	FIELD(GPIO_EVERC1, P27, 27, 1)
	FIELD(GPIO_EVERC1, P28, 28, 1)
	FIELD(GPIO_EVERC1, P29, 29, 1)
	FIELD(GPIO_EVERC1, P30, 30, 1)
	FIELD(GPIO_EVERC1, P31, 31, 1)
REG32(GPIO_EVERC2, 0x588)
	FIELD(GPIO_EVERC2, P0, 0, 1)
	FIELD(GPIO_EVERC2, P1, 1, 1)
	FIELD(GPIO_EVERC2, P2, 2, 1)
	FIELD(GPIO_EVERC2, P3, 3, 1)
	FIELD(GPIO_EVERC2, P4, 4, 1)
	FIELD(GPIO_EVERC2, P5, 5, 1)
	FIELD(GPIO_EVERC2, P6, 6, 1)
	FIELD(GPIO_EVERC2, P7, 7, 1)
	FIELD(GPIO_EVERC2, P8, 8, 1)
	FIELD(GPIO_EVERC2, P9, 9, 1)
	FIELD(GPIO_EVERC2, P10, 10, 1)
	FIELD(GPIO_EVERC2, P11, 11, 1)
	FIELD(GPIO_EVERC2, P12, 12, 1)
	FIELD(GPIO_EVERC2, P13, 13, 1)
	FIELD(GPIO_EVERC2, P14, 14, 1)
	FIELD(GPIO_EVERC2, P15, 15, 1)
	FIELD(GPIO_EVERC2, P16, 16, 1)
	FIELD(GPIO_EVERC2, P17, 17, 1)
	FIELD(GPIO_EVERC2, P18, 18, 1)
	FIELD(GPIO_EVERC2, P19, 19, 1)
	FIELD(GPIO_EVERC2, P20, 20, 1)
	FIELD(GPIO_EVERC2, P21, 21, 1)
	FIELD(GPIO_EVERC2, P22, 22, 1)
	FIELD(GPIO_EVERC2, P23, 23, 1)
	FIELD(GPIO_EVERC2, P24, 24, 1)
	FIELD(GPIO_EVERC2, P25, 25, 1)
	FIELD(GPIO_EVERC2, P26, 26, 1)
	FIELD(GPIO_EVERC2, P27, 27, 1)
	FIELD(GPIO_EVERC2, P28, 28, 1)
	FIELD(GPIO_EVERC2, P29, 29, 1)
	FIELD(GPIO_EVERC2, P30, 30, 1)
	FIELD(GPIO_EVERC2, P31, 31, 1)
REG32(GPIO_EVERS0, 0x184)
	FIELD(GPIO_EVERS0, P0, 0, 1)
	FIELD(GPIO_EVERS0, P1, 1, 1)
	FIELD(GPIO_EVERS0, P2, 2, 1)
	FIELD(GPIO_EVERS0, P3, 3, 1)
	FIELD(GPIO_EVERS0, P4, 4, 1)
	FIELD(GPIO_EVERS0, P5, 5, 1)
	FIELD(GPIO_EVERS0, P6, 6, 1)
	FIELD(GPIO_EVERS0, P7, 7, 1)
	FIELD(GPIO_EVERS0, P8, 8, 1)
	FIELD(GPIO_EVERS0, P9, 9, 1)
	FIELD(GPIO_EVERS0, P10, 10, 1)
	FIELD(GPIO_EVERS0, P11, 11, 1)
	FIELD(GPIO_EVERS0, P12, 12, 1)
	FIELD(GPIO_EVERS0, P13, 13, 1)
	FIELD(GPIO_EVERS0, P14, 14, 1)
	FIELD(GPIO_EVERS0, P15, 15, 1)
	FIELD(GPIO_EVERS0, P16, 16, 1)
	FIELD(GPIO_EVERS0, P17, 17, 1)
	FIELD(GPIO_EVERS0, P18, 18, 1)
	FIELD(GPIO_EVERS0, P19, 19, 1)
	FIELD(GPIO_EVERS0, P20, 20, 1)
	FIELD(GPIO_EVERS0, P21, 21, 1)
	FIELD(GPIO_EVERS0, P22, 22, 1)
	FIELD(GPIO_EVERS0, P23, 23, 1)
	FIELD(GPIO_EVERS0, P24, 24, 1)
	FIELD(GPIO_EVERS0, P25, 25, 1)
	FIELD(GPIO_EVERS0, P26, 26, 1)
	FIELD(GPIO_EVERS0, P27, 27, 1)
	FIELD(GPIO_EVERS0, P28, 28, 1)
	FIELD(GPIO_EVERS0, P29, 29, 1)
	FIELD(GPIO_EVERS0, P30, 30, 1)
	FIELD(GPIO_EVERS0, P31, 31, 1)
REG32(GPIO_EVERS1, 0x384)
	FIELD(GPIO_EVERS1, P0, 0, 1)
	FIELD(GPIO_EVERS1, P1, 1, 1)
	FIELD(GPIO_EVERS1, P2, 2, 1)
	FIELD(GPIO_EVERS1, P3, 3, 1)
	FIELD(GPIO_EVERS1, P4, 4, 1)
	FIELD(GPIO_EVERS1, P5, 5, 1)
	FIELD(GPIO_EVERS1, P6, 6, 1)
	FIELD(GPIO_EVERS1, P7, 7, 1)
	FIELD(GPIO_EVERS1, P8, 8, 1)
	FIELD(GPIO_EVERS1, P9, 9, 1)
	FIELD(GPIO_EVERS1, P10, 10, 1)
	FIELD(GPIO_EVERS1, P11, 11, 1)
	FIELD(GPIO_EVERS1, P12, 12, 1)
	FIELD(GPIO_EVERS1, P13, 13, 1)
	FIELD(GPIO_EVERS1, P14, 14, 1)
	FIELD(GPIO_EVERS1, P15, 15, 1)
	FIELD(GPIO_EVERS1, P16, 16, 1)
	FIELD(GPIO_EVERS1, P17, 17, 1)
	FIELD(GPIO_EVERS1, P18, 18, 1)
	FIELD(GPIO_EVERS1, P19, 19, 1)
	FIELD(GPIO_EVERS1, P20, 20, 1)
	FIELD(GPIO_EVERS1, P21, 21, 1)
	FIELD(GPIO_EVERS1, P22, 22, 1)
	FIELD(GPIO_EVERS1, P23, 23, 1)
	FIELD(GPIO_EVERS1, P24, 24, 1)
	FIELD(GPIO_EVERS1, P25, 25, 1)
	FIELD(GPIO_EVERS1, P26, 26, 1)
	FIELD(GPIO_EVERS1, P27, 27, 1)
	FIELD(GPIO_EVERS1, P28, 28, 1)
	FIELD(GPIO_EVERS1, P29, 29, 1)
	FIELD(GPIO_EVERS1, P30, 30, 1)
	FIELD(GPIO_EVERS1, P31, 31, 1)
REG32(GPIO_EVERS2, 0x584)
	FIELD(GPIO_EVERS2, P0, 0, 1)
	FIELD(GPIO_EVERS2, P1, 1, 1)
	FIELD(GPIO_EVERS2, P2, 2, 1)
	FIELD(GPIO_EVERS2, P3, 3, 1)
	FIELD(GPIO_EVERS2, P4, 4, 1)
	FIELD(GPIO_EVERS2, P5, 5, 1)
	FIELD(GPIO_EVERS2, P6, 6, 1)
	FIELD(GPIO_EVERS2, P7, 7, 1)
	FIELD(GPIO_EVERS2, P8, 8, 1)
	FIELD(GPIO_EVERS2, P9, 9, 1)
	FIELD(GPIO_EVERS2, P10, 10, 1)
	FIELD(GPIO_EVERS2, P11, 11, 1)
	FIELD(GPIO_EVERS2, P12, 12, 1)
	FIELD(GPIO_EVERS2, P13, 13, 1)
	FIELD(GPIO_EVERS2, P14, 14, 1)
	FIELD(GPIO_EVERS2, P15, 15, 1)
	FIELD(GPIO_EVERS2, P16, 16, 1)
	FIELD(GPIO_EVERS2, P17, 17, 1)
	FIELD(GPIO_EVERS2, P18, 18, 1)
	FIELD(GPIO_EVERS2, P19, 19, 1)
	FIELD(GPIO_EVERS2, P20, 20, 1)
	FIELD(GPIO_EVERS2, P21, 21, 1)
	FIELD(GPIO_EVERS2, P22, 22, 1)
	FIELD(GPIO_EVERS2, P23, 23, 1)
	FIELD(GPIO_EVERS2, P24, 24, 1)
	FIELD(GPIO_EVERS2, P25, 25, 1)
	FIELD(GPIO_EVERS2, P26, 26, 1)
	FIELD(GPIO_EVERS2, P27, 27, 1)
	FIELD(GPIO_EVERS2, P28, 28, 1)
	FIELD(GPIO_EVERS2, P29, 29, 1)
	FIELD(GPIO_EVERS2, P30, 30, 1)
	FIELD(GPIO_EVERS2, P31, 31, 1)
REG32(GPIO_EVERT0, 0x18c)
	FIELD(GPIO_EVERT0, P0, 0, 1)
	FIELD(GPIO_EVERT0, P1, 1, 1)
	FIELD(GPIO_EVERT0, P2, 2, 1)
	FIELD(GPIO_EVERT0, P3, 3, 1)
	FIELD(GPIO_EVERT0, P4, 4, 1)
	FIELD(GPIO_EVERT0, P5, 5, 1)
	FIELD(GPIO_EVERT0, P6, 6, 1)
	FIELD(GPIO_EVERT0, P7, 7, 1)
	FIELD(GPIO_EVERT0, P8, 8, 1)
	FIELD(GPIO_EVERT0, P9, 9, 1)
	FIELD(GPIO_EVERT0, P10, 10, 1)
	FIELD(GPIO_EVERT0, P11, 11, 1)
	FIELD(GPIO_EVERT0, P12, 12, 1)
	FIELD(GPIO_EVERT0, P13, 13, 1)
	FIELD(GPIO_EVERT0, P14, 14, 1)
	FIELD(GPIO_EVERT0, P15, 15, 1)
	FIELD(GPIO_EVERT0, P16, 16, 1)
	FIELD(GPIO_EVERT0, P17, 17, 1)
	FIELD(GPIO_EVERT0, P18, 18, 1)
	FIELD(GPIO_EVERT0, P19, 19, 1)
	FIELD(GPIO_EVERT0, P20, 20, 1)
	FIELD(GPIO_EVERT0, P21, 21, 1)
	FIELD(GPIO_EVERT0, P22, 22, 1)
	FIELD(GPIO_EVERT0, P23, 23, 1)
	FIELD(GPIO_EVERT0, P24, 24, 1)
	FIELD(GPIO_EVERT0, P25, 25, 1)
	FIELD(GPIO_EVERT0, P26, 26, 1)
	FIELD(GPIO_EVERT0, P27, 27, 1)
	FIELD(GPIO_EVERT0, P28, 28, 1)
	FIELD(GPIO_EVERT0, P29, 29, 1)
	FIELD(GPIO_EVERT0, P30, 30, 1)
	FIELD(GPIO_EVERT0, P31, 31, 1)
REG32(GPIO_EVERT1, 0x38c)
	FIELD(GPIO_EVERT1, P0, 0, 1)
	FIELD(GPIO_EVERT1, P1, 1, 1)
	FIELD(GPIO_EVERT1, P2, 2, 1)
	FIELD(GPIO_EVERT1, P3, 3, 1)
	FIELD(GPIO_EVERT1, P4, 4, 1)
	FIELD(GPIO_EVERT1, P5, 5, 1)
	FIELD(GPIO_EVERT1, P6, 6, 1)
	FIELD(GPIO_EVERT1, P7, 7, 1)
	FIELD(GPIO_EVERT1, P8, 8, 1)
	FIELD(GPIO_EVERT1, P9, 9, 1)
	FIELD(GPIO_EVERT1, P10, 10, 1)
	FIELD(GPIO_EVERT1, P11, 11, 1)
	FIELD(GPIO_EVERT1, P12, 12, 1)
	FIELD(GPIO_EVERT1, P13, 13, 1)
	FIELD(GPIO_EVERT1, P14, 14, 1)
	FIELD(GPIO_EVERT1, P15, 15, 1)
	FIELD(GPIO_EVERT1, P16, 16, 1)
	FIELD(GPIO_EVERT1, P17, 17, 1)
	FIELD(GPIO_EVERT1, P18, 18, 1)
	FIELD(GPIO_EVERT1, P19, 19, 1)
	FIELD(GPIO_EVERT1, P20, 20, 1)
	FIELD(GPIO_EVERT1, P21, 21, 1)
	FIELD(GPIO_EVERT1, P22, 22, 1)
	FIELD(GPIO_EVERT1, P23, 23, 1)
	FIELD(GPIO_EVERT1, P24, 24, 1)
	FIELD(GPIO_EVERT1, P25, 25, 1)
	FIELD(GPIO_EVERT1, P26, 26, 1)
	FIELD(GPIO_EVERT1, P27, 27, 1)
	FIELD(GPIO_EVERT1, P28, 28, 1)
	FIELD(GPIO_EVERT1, P29, 29, 1)
	FIELD(GPIO_EVERT1, P30, 30, 1)
	FIELD(GPIO_EVERT1, P31, 31, 1)
REG32(GPIO_EVERT2, 0x58c)
	FIELD(GPIO_EVERT2, P0, 0, 1)
	FIELD(GPIO_EVERT2, P1, 1, 1)
	FIELD(GPIO_EVERT2, P2, 2, 1)
	FIELD(GPIO_EVERT2, P3, 3, 1)
	FIELD(GPIO_EVERT2, P4, 4, 1)
	FIELD(GPIO_EVERT2, P5, 5, 1)
	FIELD(GPIO_EVERT2, P6, 6, 1)
	FIELD(GPIO_EVERT2, P7, 7, 1)
	FIELD(GPIO_EVERT2, P8, 8, 1)
	FIELD(GPIO_EVERT2, P9, 9, 1)
	FIELD(GPIO_EVERT2, P10, 10, 1)
	FIELD(GPIO_EVERT2, P11, 11, 1)
	FIELD(GPIO_EVERT2, P12, 12, 1)
	FIELD(GPIO_EVERT2, P13, 13, 1)
	FIELD(GPIO_EVERT2, P14, 14, 1)
	FIELD(GPIO_EVERT2, P15, 15, 1)
	FIELD(GPIO_EVERT2, P16, 16, 1)
	FIELD(GPIO_EVERT2, P17, 17, 1)
	FIELD(GPIO_EVERT2, P18, 18, 1)
	FIELD(GPIO_EVERT2, P19, 19, 1)
	FIELD(GPIO_EVERT2, P20, 20, 1)
	FIELD(GPIO_EVERT2, P21, 21, 1)
	FIELD(GPIO_EVERT2, P22, 22, 1)
	FIELD(GPIO_EVERT2, P23, 23, 1)
	FIELD(GPIO_EVERT2, P24, 24, 1)
	FIELD(GPIO_EVERT2, P25, 25, 1)
	FIELD(GPIO_EVERT2, P26, 26, 1)
	FIELD(GPIO_EVERT2, P27, 27, 1)
	FIELD(GPIO_EVERT2, P28, 28, 1)
	FIELD(GPIO_EVERT2, P29, 29, 1)
	FIELD(GPIO_EVERT2, P30, 30, 1)
	FIELD(GPIO_EVERT2, P31, 31, 1)
REG32(GPIO_EVER0, 0x180)
	FIELD(GPIO_EVER0, P0, 0, 1)
	FIELD(GPIO_EVER0, P1, 1, 1)
	FIELD(GPIO_EVER0, P2, 2, 1)
	FIELD(GPIO_EVER0, P3, 3, 1)
	FIELD(GPIO_EVER0, P4, 4, 1)
	FIELD(GPIO_EVER0, P5, 5, 1)
	FIELD(GPIO_EVER0, P6, 6, 1)
	FIELD(GPIO_EVER0, P7, 7, 1)
	FIELD(GPIO_EVER0, P8, 8, 1)
	FIELD(GPIO_EVER0, P9, 9, 1)
	FIELD(GPIO_EVER0, P10, 10, 1)
	FIELD(GPIO_EVER0, P11, 11, 1)
	FIELD(GPIO_EVER0, P12, 12, 1)
	FIELD(GPIO_EVER0, P13, 13, 1)
	FIELD(GPIO_EVER0, P14, 14, 1)
	FIELD(GPIO_EVER0, P15, 15, 1)
	FIELD(GPIO_EVER0, P16, 16, 1)
	FIELD(GPIO_EVER0, P17, 17, 1)
	FIELD(GPIO_EVER0, P18, 18, 1)
	FIELD(GPIO_EVER0, P19, 19, 1)
	FIELD(GPIO_EVER0, P20, 20, 1)
	FIELD(GPIO_EVER0, P21, 21, 1)
	FIELD(GPIO_EVER0, P22, 22, 1)
	FIELD(GPIO_EVER0, P23, 23, 1)
	FIELD(GPIO_EVER0, P24, 24, 1)
	FIELD(GPIO_EVER0, P25, 25, 1)
	FIELD(GPIO_EVER0, P26, 26, 1)
	FIELD(GPIO_EVER0, P27, 27, 1)
	FIELD(GPIO_EVER0, P28, 28, 1)
	FIELD(GPIO_EVER0, P29, 29, 1)
	FIELD(GPIO_EVER0, P30, 30, 1)
	FIELD(GPIO_EVER0, P31, 31, 1)
REG32(GPIO_EVER1, 0x380)
	FIELD(GPIO_EVER1, P0, 0, 1)
	FIELD(GPIO_EVER1, P1, 1, 1)
	FIELD(GPIO_EVER1, P2, 2, 1)
	FIELD(GPIO_EVER1, P3, 3, 1)
	FIELD(GPIO_EVER1, P4, 4, 1)
	FIELD(GPIO_EVER1, P5, 5, 1)
	FIELD(GPIO_EVER1, P6, 6, 1)
	FIELD(GPIO_EVER1, P7, 7, 1)
	FIELD(GPIO_EVER1, P8, 8, 1)
	FIELD(GPIO_EVER1, P9, 9, 1)
	FIELD(GPIO_EVER1, P10, 10, 1)
	FIELD(GPIO_EVER1, P11, 11, 1)
	FIELD(GPIO_EVER1, P12, 12, 1)
	FIELD(GPIO_EVER1, P13, 13, 1)
	FIELD(GPIO_EVER1, P14, 14, 1)
	FIELD(GPIO_EVER1, P15, 15, 1)
	FIELD(GPIO_EVER1, P16, 16, 1)
	FIELD(GPIO_EVER1, P17, 17, 1)
	FIELD(GPIO_EVER1, P18, 18, 1)
	FIELD(GPIO_EVER1, P19, 19, 1)
	FIELD(GPIO_EVER1, P20, 20, 1)
	FIELD(GPIO_EVER1, P21, 21, 1)
	FIELD(GPIO_EVER1, P22, 22, 1)
	FIELD(GPIO_EVER1, P23, 23, 1)
	FIELD(GPIO_EVER1, P24, 24, 1)
	FIELD(GPIO_EVER1, P25, 25, 1)
	FIELD(GPIO_EVER1, P26, 26, 1)
	FIELD(GPIO_EVER1, P27, 27, 1)
	FIELD(GPIO_EVER1, P28, 28, 1)
	FIELD(GPIO_EVER1, P29, 29, 1)
	FIELD(GPIO_EVER1, P30, 30, 1)
	FIELD(GPIO_EVER1, P31, 31, 1)
REG32(GPIO_EVER2, 0x580)
	FIELD(GPIO_EVER2, P0, 0, 1)
	FIELD(GPIO_EVER2, P1, 1, 1)
	FIELD(GPIO_EVER2, P2, 2, 1)
	FIELD(GPIO_EVER2, P3, 3, 1)
	FIELD(GPIO_EVER2, P4, 4, 1)
	FIELD(GPIO_EVER2, P5, 5, 1)
	FIELD(GPIO_EVER2, P6, 6, 1)
	FIELD(GPIO_EVER2, P7, 7, 1)
	FIELD(GPIO_EVER2, P8, 8, 1)
	FIELD(GPIO_EVER2, P9, 9, 1)
	FIELD(GPIO_EVER2, P10, 10, 1)
	FIELD(GPIO_EVER2, P11, 11, 1)
	FIELD(GPIO_EVER2, P12, 12, 1)
	FIELD(GPIO_EVER2, P13, 13, 1)
	FIELD(GPIO_EVER2, P14, 14, 1)
	FIELD(GPIO_EVER2, P15, 15, 1)
	FIELD(GPIO_EVER2, P16, 16, 1)
	FIELD(GPIO_EVER2, P17, 17, 1)
	FIELD(GPIO_EVER2, P18, 18, 1)
	FIELD(GPIO_EVER2, P19, 19, 1)
	FIELD(GPIO_EVER2, P20, 20, 1)
	FIELD(GPIO_EVER2, P21, 21, 1)
	FIELD(GPIO_EVER2, P22, 22, 1)
	FIELD(GPIO_EVER2, P23, 23, 1)
	FIELD(GPIO_EVER2, P24, 24, 1)
	FIELD(GPIO_EVER2, P25, 25, 1)
	FIELD(GPIO_EVER2, P26, 26, 1)
	FIELD(GPIO_EVER2, P27, 27, 1)
	FIELD(GPIO_EVER2, P28, 28, 1)
	FIELD(GPIO_EVER2, P29, 29, 1)
	FIELD(GPIO_EVER2, P30, 30, 1)
	FIELD(GPIO_EVER2, P31, 31, 1)
REG32(GPIO_GFERC0, 0xc8)
	FIELD(GPIO_GFERC0, P0, 0, 1)
	FIELD(GPIO_GFERC0, P1, 1, 1)
	FIELD(GPIO_GFERC0, P2, 2, 1)
	FIELD(GPIO_GFERC0, P3, 3, 1)
	FIELD(GPIO_GFERC0, P4, 4, 1)
	FIELD(GPIO_GFERC0, P5, 5, 1)
	FIELD(GPIO_GFERC0, P6, 6, 1)
	FIELD(GPIO_GFERC0, P7, 7, 1)
	FIELD(GPIO_GFERC0, P8, 8, 1)
	FIELD(GPIO_GFERC0, P9, 9, 1)
	FIELD(GPIO_GFERC0, P10, 10, 1)
	FIELD(GPIO_GFERC0, P11, 11, 1)
	FIELD(GPIO_GFERC0, P12, 12, 1)
	FIELD(GPIO_GFERC0, P13, 13, 1)
	FIELD(GPIO_GFERC0, P14, 14, 1)
	FIELD(GPIO_GFERC0, P15, 15, 1)
	FIELD(GPIO_GFERC0, P16, 16, 1)
	FIELD(GPIO_GFERC0, P17, 17, 1)
	FIELD(GPIO_GFERC0, P18, 18, 1)
	FIELD(GPIO_GFERC0, P19, 19, 1)
	FIELD(GPIO_GFERC0, P20, 20, 1)
	FIELD(GPIO_GFERC0, P21, 21, 1)
	FIELD(GPIO_GFERC0, P22, 22, 1)
	FIELD(GPIO_GFERC0, P23, 23, 1)
	FIELD(GPIO_GFERC0, P24, 24, 1)
	FIELD(GPIO_GFERC0, P25, 25, 1)
	FIELD(GPIO_GFERC0, P26, 26, 1)
	FIELD(GPIO_GFERC0, P27, 27, 1)
	FIELD(GPIO_GFERC0, P28, 28, 1)
	FIELD(GPIO_GFERC0, P29, 29, 1)
	FIELD(GPIO_GFERC0, P30, 30, 1)
	FIELD(GPIO_GFERC0, P31, 31, 1)
REG32(GPIO_GFERC1, 0x2c8)
	FIELD(GPIO_GFERC1, P0, 0, 1)
	FIELD(GPIO_GFERC1, P1, 1, 1)
	FIELD(GPIO_GFERC1, P2, 2, 1)
	FIELD(GPIO_GFERC1, P3, 3, 1)
	FIELD(GPIO_GFERC1, P4, 4, 1)
	FIELD(GPIO_GFERC1, P5, 5, 1)
	FIELD(GPIO_GFERC1, P6, 6, 1)
	FIELD(GPIO_GFERC1, P7, 7, 1)
	FIELD(GPIO_GFERC1, P8, 8, 1)
	FIELD(GPIO_GFERC1, P9, 9, 1)
	FIELD(GPIO_GFERC1, P10, 10, 1)
	FIELD(GPIO_GFERC1, P11, 11, 1)
	FIELD(GPIO_GFERC1, P12, 12, 1)
	FIELD(GPIO_GFERC1, P13, 13, 1)
	FIELD(GPIO_GFERC1, P14, 14, 1)
	FIELD(GPIO_GFERC1, P15, 15, 1)
	FIELD(GPIO_GFERC1, P16, 16, 1)
	FIELD(GPIO_GFERC1, P17, 17, 1)
	FIELD(GPIO_GFERC1, P18, 18, 1)
	FIELD(GPIO_GFERC1, P19, 19, 1)
	FIELD(GPIO_GFERC1, P20, 20, 1)
	FIELD(GPIO_GFERC1, P21, 21, 1)
	FIELD(GPIO_GFERC1, P22, 22, 1)
	FIELD(GPIO_GFERC1, P23, 23, 1)
	FIELD(GPIO_GFERC1, P24, 24, 1)
	FIELD(GPIO_GFERC1, P25, 25, 1)
	FIELD(GPIO_GFERC1, P26, 26, 1)
	FIELD(GPIO_GFERC1, P27, 27, 1)
	FIELD(GPIO_GFERC1, P28, 28, 1)
	FIELD(GPIO_GFERC1, P29, 29, 1)
	FIELD(GPIO_GFERC1, P30, 30, 1)
	FIELD(GPIO_GFERC1, P31, 31, 1)
REG32(GPIO_GFERC2, 0x4c8)
	FIELD(GPIO_GFERC2, P0, 0, 1)
	FIELD(GPIO_GFERC2, P1, 1, 1)
	FIELD(GPIO_GFERC2, P2, 2, 1)
	FIELD(GPIO_GFERC2, P3, 3, 1)
	FIELD(GPIO_GFERC2, P4, 4, 1)
	FIELD(GPIO_GFERC2, P5, 5, 1)
	FIELD(GPIO_GFERC2, P6, 6, 1)
	FIELD(GPIO_GFERC2, P7, 7, 1)
	FIELD(GPIO_GFERC2, P8, 8, 1)
	FIELD(GPIO_GFERC2, P9, 9, 1)
	FIELD(GPIO_GFERC2, P10, 10, 1)
	FIELD(GPIO_GFERC2, P11, 11, 1)
	FIELD(GPIO_GFERC2, P12, 12, 1)
	FIELD(GPIO_GFERC2, P13, 13, 1)
	FIELD(GPIO_GFERC2, P14, 14, 1)
	FIELD(GPIO_GFERC2, P15, 15, 1)
	FIELD(GPIO_GFERC2, P16, 16, 1)
	FIELD(GPIO_GFERC2, P17, 17, 1)
	FIELD(GPIO_GFERC2, P18, 18, 1)
	FIELD(GPIO_GFERC2, P19, 19, 1)
	FIELD(GPIO_GFERC2, P20, 20, 1)
	FIELD(GPIO_GFERC2, P21, 21, 1)
	FIELD(GPIO_GFERC2, P22, 22, 1)
	FIELD(GPIO_GFERC2, P23, 23, 1)
	FIELD(GPIO_GFERC2, P24, 24, 1)
	FIELD(GPIO_GFERC2, P25, 25, 1)
	FIELD(GPIO_GFERC2, P26, 26, 1)
	FIELD(GPIO_GFERC2, P27, 27, 1)
	FIELD(GPIO_GFERC2, P28, 28, 1)
	FIELD(GPIO_GFERC2, P29, 29, 1)
	FIELD(GPIO_GFERC2, P30, 30, 1)
	FIELD(GPIO_GFERC2, P31, 31, 1)
REG32(GPIO_GFERS0, 0xc4)
	FIELD(GPIO_GFERS0, P0, 0, 1)
	FIELD(GPIO_GFERS0, P1, 1, 1)
	FIELD(GPIO_GFERS0, P2, 2, 1)
	FIELD(GPIO_GFERS0, P3, 3, 1)
	FIELD(GPIO_GFERS0, P4, 4, 1)
	FIELD(GPIO_GFERS0, P5, 5, 1)
	FIELD(GPIO_GFERS0, P6, 6, 1)
	FIELD(GPIO_GFERS0, P7, 7, 1)
	FIELD(GPIO_GFERS0, P8, 8, 1)
	FIELD(GPIO_GFERS0, P9, 9, 1)
	FIELD(GPIO_GFERS0, P10, 10, 1)
	FIELD(GPIO_GFERS0, P11, 11, 1)
	FIELD(GPIO_GFERS0, P12, 12, 1)
	FIELD(GPIO_GFERS0, P13, 13, 1)
	FIELD(GPIO_GFERS0, P14, 14, 1)
	FIELD(GPIO_GFERS0, P15, 15, 1)
	FIELD(GPIO_GFERS0, P16, 16, 1)
	FIELD(GPIO_GFERS0, P17, 17, 1)
	FIELD(GPIO_GFERS0, P18, 18, 1)
	FIELD(GPIO_GFERS0, P19, 19, 1)
	FIELD(GPIO_GFERS0, P20, 20, 1)
	FIELD(GPIO_GFERS0, P21, 21, 1)
	FIELD(GPIO_GFERS0, P22, 22, 1)
	FIELD(GPIO_GFERS0, P23, 23, 1)
	FIELD(GPIO_GFERS0, P24, 24, 1)
	FIELD(GPIO_GFERS0, P25, 25, 1)
	FIELD(GPIO_GFERS0, P26, 26, 1)
	FIELD(GPIO_GFERS0, P27, 27, 1)
	FIELD(GPIO_GFERS0, P28, 28, 1)
	FIELD(GPIO_GFERS0, P29, 29, 1)
	FIELD(GPIO_GFERS0, P30, 30, 1)
	FIELD(GPIO_GFERS0, P31, 31, 1)
REG32(GPIO_GFERS1, 0x2c4)
	FIELD(GPIO_GFERS1, P0, 0, 1)
	FIELD(GPIO_GFERS1, P1, 1, 1)
	FIELD(GPIO_GFERS1, P2, 2, 1)
	FIELD(GPIO_GFERS1, P3, 3, 1)
	FIELD(GPIO_GFERS1, P4, 4, 1)
	FIELD(GPIO_GFERS1, P5, 5, 1)
	FIELD(GPIO_GFERS1, P6, 6, 1)
	FIELD(GPIO_GFERS1, P7, 7, 1)
	FIELD(GPIO_GFERS1, P8, 8, 1)
	FIELD(GPIO_GFERS1, P9, 9, 1)
	FIELD(GPIO_GFERS1, P10, 10, 1)
	FIELD(GPIO_GFERS1, P11, 11, 1)
	FIELD(GPIO_GFERS1, P12, 12, 1)
	FIELD(GPIO_GFERS1, P13, 13, 1)
	FIELD(GPIO_GFERS1, P14, 14, 1)
	FIELD(GPIO_GFERS1, P15, 15, 1)
	FIELD(GPIO_GFERS1, P16, 16, 1)
	FIELD(GPIO_GFERS1, P17, 17, 1)
	FIELD(GPIO_GFERS1, P18, 18, 1)
	FIELD(GPIO_GFERS1, P19, 19, 1)
	FIELD(GPIO_GFERS1, P20, 20, 1)
	FIELD(GPIO_GFERS1, P21, 21, 1)
	FIELD(GPIO_GFERS1, P22, 22, 1)
	FIELD(GPIO_GFERS1, P23, 23, 1)
	FIELD(GPIO_GFERS1, P24, 24, 1)
	FIELD(GPIO_GFERS1, P25, 25, 1)
	FIELD(GPIO_GFERS1, P26, 26, 1)
	FIELD(GPIO_GFERS1, P27, 27, 1)
	FIELD(GPIO_GFERS1, P28, 28, 1)
	FIELD(GPIO_GFERS1, P29, 29, 1)
	FIELD(GPIO_GFERS1, P30, 30, 1)
	FIELD(GPIO_GFERS1, P31, 31, 1)
REG32(GPIO_GFERS2, 0x4c4)
	FIELD(GPIO_GFERS2, P0, 0, 1)
	FIELD(GPIO_GFERS2, P1, 1, 1)
	FIELD(GPIO_GFERS2, P2, 2, 1)
	FIELD(GPIO_GFERS2, P3, 3, 1)
	FIELD(GPIO_GFERS2, P4, 4, 1)
	FIELD(GPIO_GFERS2, P5, 5, 1)
	FIELD(GPIO_GFERS2, P6, 6, 1)
	FIELD(GPIO_GFERS2, P7, 7, 1)
	FIELD(GPIO_GFERS2, P8, 8, 1)
	FIELD(GPIO_GFERS2, P9, 9, 1)
	FIELD(GPIO_GFERS2, P10, 10, 1)
	FIELD(GPIO_GFERS2, P11, 11, 1)
	FIELD(GPIO_GFERS2, P12, 12, 1)
	FIELD(GPIO_GFERS2, P13, 13, 1)
	FIELD(GPIO_GFERS2, P14, 14, 1)
	FIELD(GPIO_GFERS2, P15, 15, 1)
	FIELD(GPIO_GFERS2, P16, 16, 1)
	FIELD(GPIO_GFERS2, P17, 17, 1)
	FIELD(GPIO_GFERS2, P18, 18, 1)
	FIELD(GPIO_GFERS2, P19, 19, 1)
	FIELD(GPIO_GFERS2, P20, 20, 1)
	FIELD(GPIO_GFERS2, P21, 21, 1)
	FIELD(GPIO_GFERS2, P22, 22, 1)
	FIELD(GPIO_GFERS2, P23, 23, 1)
	FIELD(GPIO_GFERS2, P24, 24, 1)
	FIELD(GPIO_GFERS2, P25, 25, 1)
	FIELD(GPIO_GFERS2, P26, 26, 1)
	FIELD(GPIO_GFERS2, P27, 27, 1)
	FIELD(GPIO_GFERS2, P28, 28, 1)
	FIELD(GPIO_GFERS2, P29, 29, 1)
	FIELD(GPIO_GFERS2, P30, 30, 1)
	FIELD(GPIO_GFERS2, P31, 31, 1)
REG32(GPIO_GFERT0, 0xcc)
	FIELD(GPIO_GFERT0, P0, 0, 1)
	FIELD(GPIO_GFERT0, P1, 1, 1)
	FIELD(GPIO_GFERT0, P2, 2, 1)
	FIELD(GPIO_GFERT0, P3, 3, 1)
	FIELD(GPIO_GFERT0, P4, 4, 1)
	FIELD(GPIO_GFERT0, P5, 5, 1)
	FIELD(GPIO_GFERT0, P6, 6, 1)
	FIELD(GPIO_GFERT0, P7, 7, 1)
	FIELD(GPIO_GFERT0, P8, 8, 1)
	FIELD(GPIO_GFERT0, P9, 9, 1)
	FIELD(GPIO_GFERT0, P10, 10, 1)
	FIELD(GPIO_GFERT0, P11, 11, 1)
	FIELD(GPIO_GFERT0, P12, 12, 1)
	FIELD(GPIO_GFERT0, P13, 13, 1)
	FIELD(GPIO_GFERT0, P14, 14, 1)
	FIELD(GPIO_GFERT0, P15, 15, 1)
	FIELD(GPIO_GFERT0, P16, 16, 1)
	FIELD(GPIO_GFERT0, P17, 17, 1)
	FIELD(GPIO_GFERT0, P18, 18, 1)
	FIELD(GPIO_GFERT0, P19, 19, 1)
	FIELD(GPIO_GFERT0, P20, 20, 1)
	FIELD(GPIO_GFERT0, P21, 21, 1)
	FIELD(GPIO_GFERT0, P22, 22, 1)
	FIELD(GPIO_GFERT0, P23, 23, 1)
	FIELD(GPIO_GFERT0, P24, 24, 1)
	FIELD(GPIO_GFERT0, P25, 25, 1)
	FIELD(GPIO_GFERT0, P26, 26, 1)
	FIELD(GPIO_GFERT0, P27, 27, 1)
	FIELD(GPIO_GFERT0, P28, 28, 1)
	FIELD(GPIO_GFERT0, P29, 29, 1)
	FIELD(GPIO_GFERT0, P30, 30, 1)
	FIELD(GPIO_GFERT0, P31, 31, 1)
REG32(GPIO_GFERT1, 0x2cc)
	FIELD(GPIO_GFERT1, P0, 0, 1)
	FIELD(GPIO_GFERT1, P1, 1, 1)
	FIELD(GPIO_GFERT1, P2, 2, 1)
	FIELD(GPIO_GFERT1, P3, 3, 1)
	FIELD(GPIO_GFERT1, P4, 4, 1)
	FIELD(GPIO_GFERT1, P5, 5, 1)
	FIELD(GPIO_GFERT1, P6, 6, 1)
	FIELD(GPIO_GFERT1, P7, 7, 1)
	FIELD(GPIO_GFERT1, P8, 8, 1)
	FIELD(GPIO_GFERT1, P9, 9, 1)
	FIELD(GPIO_GFERT1, P10, 10, 1)
	FIELD(GPIO_GFERT1, P11, 11, 1)
	FIELD(GPIO_GFERT1, P12, 12, 1)
	FIELD(GPIO_GFERT1, P13, 13, 1)
	FIELD(GPIO_GFERT1, P14, 14, 1)
	FIELD(GPIO_GFERT1, P15, 15, 1)
	FIELD(GPIO_GFERT1, P16, 16, 1)
	FIELD(GPIO_GFERT1, P17, 17, 1)
	FIELD(GPIO_GFERT1, P18, 18, 1)
	FIELD(GPIO_GFERT1, P19, 19, 1)
	FIELD(GPIO_GFERT1, P20, 20, 1)
	FIELD(GPIO_GFERT1, P21, 21, 1)
	FIELD(GPIO_GFERT1, P22, 22, 1)
	FIELD(GPIO_GFERT1, P23, 23, 1)
	FIELD(GPIO_GFERT1, P24, 24, 1)
	FIELD(GPIO_GFERT1, P25, 25, 1)
	FIELD(GPIO_GFERT1, P26, 26, 1)
	FIELD(GPIO_GFERT1, P27, 27, 1)
	FIELD(GPIO_GFERT1, P28, 28, 1)
	FIELD(GPIO_GFERT1, P29, 29, 1)
	FIELD(GPIO_GFERT1, P30, 30, 1)
	FIELD(GPIO_GFERT1, P31, 31, 1)
REG32(GPIO_GFERT2, 0x4cc)
	FIELD(GPIO_GFERT2, P0, 0, 1)
	FIELD(GPIO_GFERT2, P1, 1, 1)
	FIELD(GPIO_GFERT2, P2, 2, 1)
	FIELD(GPIO_GFERT2, P3, 3, 1)
	FIELD(GPIO_GFERT2, P4, 4, 1)
	FIELD(GPIO_GFERT2, P5, 5, 1)
	FIELD(GPIO_GFERT2, P6, 6, 1)
	FIELD(GPIO_GFERT2, P7, 7, 1)
	FIELD(GPIO_GFERT2, P8, 8, 1)
	FIELD(GPIO_GFERT2, P9, 9, 1)
	FIELD(GPIO_GFERT2, P10, 10, 1)
	FIELD(GPIO_GFERT2, P11, 11, 1)
	FIELD(GPIO_GFERT2, P12, 12, 1)
	FIELD(GPIO_GFERT2, P13, 13, 1)
	FIELD(GPIO_GFERT2, P14, 14, 1)
	FIELD(GPIO_GFERT2, P15, 15, 1)
	FIELD(GPIO_GFERT2, P16, 16, 1)
	FIELD(GPIO_GFERT2, P17, 17, 1)
	FIELD(GPIO_GFERT2, P18, 18, 1)
	FIELD(GPIO_GFERT2, P19, 19, 1)
	FIELD(GPIO_GFERT2, P20, 20, 1)
	FIELD(GPIO_GFERT2, P21, 21, 1)
	FIELD(GPIO_GFERT2, P22, 22, 1)
	FIELD(GPIO_GFERT2, P23, 23, 1)
	FIELD(GPIO_GFERT2, P24, 24, 1)
	FIELD(GPIO_GFERT2, P25, 25, 1)
	FIELD(GPIO_GFERT2, P26, 26, 1)
	FIELD(GPIO_GFERT2, P27, 27, 1)
	FIELD(GPIO_GFERT2, P28, 28, 1)
	FIELD(GPIO_GFERT2, P29, 29, 1)
	FIELD(GPIO_GFERT2, P30, 30, 1)
	FIELD(GPIO_GFERT2, P31, 31, 1)
REG32(GPIO_GFER0, 0xc0)
	FIELD(GPIO_GFER0, P0, 0, 1)
	FIELD(GPIO_GFER0, P1, 1, 1)
	FIELD(GPIO_GFER0, P2, 2, 1)
	FIELD(GPIO_GFER0, P3, 3, 1)
	FIELD(GPIO_GFER0, P4, 4, 1)
	FIELD(GPIO_GFER0, P5, 5, 1)
	FIELD(GPIO_GFER0, P6, 6, 1)
	FIELD(GPIO_GFER0, P7, 7, 1)
	FIELD(GPIO_GFER0, P8, 8, 1)
	FIELD(GPIO_GFER0, P9, 9, 1)
	FIELD(GPIO_GFER0, P10, 10, 1)
	FIELD(GPIO_GFER0, P11, 11, 1)
	FIELD(GPIO_GFER0, P12, 12, 1)
	FIELD(GPIO_GFER0, P13, 13, 1)
	FIELD(GPIO_GFER0, P14, 14, 1)
	FIELD(GPIO_GFER0, P15, 15, 1)
	FIELD(GPIO_GFER0, P16, 16, 1)
	FIELD(GPIO_GFER0, P17, 17, 1)
	FIELD(GPIO_GFER0, P18, 18, 1)
	FIELD(GPIO_GFER0, P19, 19, 1)
	FIELD(GPIO_GFER0, P20, 20, 1)
	FIELD(GPIO_GFER0, P21, 21, 1)
	FIELD(GPIO_GFER0, P22, 22, 1)
	FIELD(GPIO_GFER0, P23, 23, 1)
	FIELD(GPIO_GFER0, P24, 24, 1)
	FIELD(GPIO_GFER0, P25, 25, 1)
	FIELD(GPIO_GFER0, P26, 26, 1)
	FIELD(GPIO_GFER0, P27, 27, 1)
	FIELD(GPIO_GFER0, P28, 28, 1)
	FIELD(GPIO_GFER0, P29, 29, 1)
	FIELD(GPIO_GFER0, P30, 30, 1)
	FIELD(GPIO_GFER0, P31, 31, 1)
REG32(GPIO_GFER1, 0x2c0)
	FIELD(GPIO_GFER1, P0, 0, 1)
	FIELD(GPIO_GFER1, P1, 1, 1)
	FIELD(GPIO_GFER1, P2, 2, 1)
	FIELD(GPIO_GFER1, P3, 3, 1)
	FIELD(GPIO_GFER1, P4, 4, 1)
	FIELD(GPIO_GFER1, P5, 5, 1)
	FIELD(GPIO_GFER1, P6, 6, 1)
	FIELD(GPIO_GFER1, P7, 7, 1)
	FIELD(GPIO_GFER1, P8, 8, 1)
	FIELD(GPIO_GFER1, P9, 9, 1)
	FIELD(GPIO_GFER1, P10, 10, 1)
	FIELD(GPIO_GFER1, P11, 11, 1)
	FIELD(GPIO_GFER1, P12, 12, 1)
	FIELD(GPIO_GFER1, P13, 13, 1)
	FIELD(GPIO_GFER1, P14, 14, 1)
	FIELD(GPIO_GFER1, P15, 15, 1)
	FIELD(GPIO_GFER1, P16, 16, 1)
	FIELD(GPIO_GFER1, P17, 17, 1)
	FIELD(GPIO_GFER1, P18, 18, 1)
	FIELD(GPIO_GFER1, P19, 19, 1)
	FIELD(GPIO_GFER1, P20, 20, 1)
	FIELD(GPIO_GFER1, P21, 21, 1)
	FIELD(GPIO_GFER1, P22, 22, 1)
	FIELD(GPIO_GFER1, P23, 23, 1)
	FIELD(GPIO_GFER1, P24, 24, 1)
	FIELD(GPIO_GFER1, P25, 25, 1)
	FIELD(GPIO_GFER1, P26, 26, 1)
	FIELD(GPIO_GFER1, P27, 27, 1)
	FIELD(GPIO_GFER1, P28, 28, 1)
	FIELD(GPIO_GFER1, P29, 29, 1)
	FIELD(GPIO_GFER1, P30, 30, 1)
	FIELD(GPIO_GFER1, P31, 31, 1)
REG32(GPIO_GFER2, 0x4c0)
	FIELD(GPIO_GFER2, P0, 0, 1)
	FIELD(GPIO_GFER2, P1, 1, 1)
	FIELD(GPIO_GFER2, P2, 2, 1)
	FIELD(GPIO_GFER2, P3, 3, 1)
	FIELD(GPIO_GFER2, P4, 4, 1)
	FIELD(GPIO_GFER2, P5, 5, 1)
	FIELD(GPIO_GFER2, P6, 6, 1)
	FIELD(GPIO_GFER2, P7, 7, 1)
	FIELD(GPIO_GFER2, P8, 8, 1)
	FIELD(GPIO_GFER2, P9, 9, 1)
	FIELD(GPIO_GFER2, P10, 10, 1)
	FIELD(GPIO_GFER2, P11, 11, 1)
	FIELD(GPIO_GFER2, P12, 12, 1)
	FIELD(GPIO_GFER2, P13, 13, 1)
	FIELD(GPIO_GFER2, P14, 14, 1)
	FIELD(GPIO_GFER2, P15, 15, 1)
	FIELD(GPIO_GFER2, P16, 16, 1)
	FIELD(GPIO_GFER2, P17, 17, 1)
	FIELD(GPIO_GFER2, P18, 18, 1)
	FIELD(GPIO_GFER2, P19, 19, 1)
	FIELD(GPIO_GFER2, P20, 20, 1)
	FIELD(GPIO_GFER2, P21, 21, 1)
	FIELD(GPIO_GFER2, P22, 22, 1)
	FIELD(GPIO_GFER2, P23, 23, 1)
	FIELD(GPIO_GFER2, P24, 24, 1)
	FIELD(GPIO_GFER2, P25, 25, 1)
	FIELD(GPIO_GFER2, P26, 26, 1)
	FIELD(GPIO_GFER2, P27, 27, 1)
	FIELD(GPIO_GFER2, P28, 28, 1)
	FIELD(GPIO_GFER2, P29, 29, 1)
	FIELD(GPIO_GFER2, P30, 30, 1)
	FIELD(GPIO_GFER2, P31, 31, 1)
REG32(GPIO_GPERC0, 0x8)
	FIELD(GPIO_GPERC0, P0, 0, 1)
	FIELD(GPIO_GPERC0, P1, 1, 1)
	FIELD(GPIO_GPERC0, P2, 2, 1)
	FIELD(GPIO_GPERC0, P3, 3, 1)
	FIELD(GPIO_GPERC0, P4, 4, 1)
	FIELD(GPIO_GPERC0, P5, 5, 1)
	FIELD(GPIO_GPERC0, P6, 6, 1)
	FIELD(GPIO_GPERC0, P7, 7, 1)
	FIELD(GPIO_GPERC0, P8, 8, 1)
	FIELD(GPIO_GPERC0, P9, 9, 1)
	FIELD(GPIO_GPERC0, P10, 10, 1)
	FIELD(GPIO_GPERC0, P11, 11, 1)
	FIELD(GPIO_GPERC0, P12, 12, 1)
	FIELD(GPIO_GPERC0, P13, 13, 1)
	FIELD(GPIO_GPERC0, P14, 14, 1)
	FIELD(GPIO_GPERC0, P15, 15, 1)
	FIELD(GPIO_GPERC0, P16, 16, 1)
	FIELD(GPIO_GPERC0, P17, 17, 1)
	FIELD(GPIO_GPERC0, P18, 18, 1)
	FIELD(GPIO_GPERC0, P19, 19, 1)
	FIELD(GPIO_GPERC0, P20, 20, 1)
	FIELD(GPIO_GPERC0, P21, 21, 1)
	FIELD(GPIO_GPERC0, P22, 22, 1)
	FIELD(GPIO_GPERC0, P23, 23, 1)
	FIELD(GPIO_GPERC0, P24, 24, 1)
	FIELD(GPIO_GPERC0, P25, 25, 1)
	FIELD(GPIO_GPERC0, P26, 26, 1)
	FIELD(GPIO_GPERC0, P27, 27, 1)
	FIELD(GPIO_GPERC0, P28, 28, 1)
	FIELD(GPIO_GPERC0, P29, 29, 1)
	FIELD(GPIO_GPERC0, P30, 30, 1)
	FIELD(GPIO_GPERC0, P31, 31, 1)
REG32(GPIO_GPERC1, 0x208)
	FIELD(GPIO_GPERC1, P0, 0, 1)
	FIELD(GPIO_GPERC1, P1, 1, 1)
	FIELD(GPIO_GPERC1, P2, 2, 1)
	FIELD(GPIO_GPERC1, P3, 3, 1)
	FIELD(GPIO_GPERC1, P4, 4, 1)
	FIELD(GPIO_GPERC1, P5, 5, 1)
	FIELD(GPIO_GPERC1, P6, 6, 1)
	FIELD(GPIO_GPERC1, P7, 7, 1)
	FIELD(GPIO_GPERC1, P8, 8, 1)
	FIELD(GPIO_GPERC1, P9, 9, 1)
	FIELD(GPIO_GPERC1, P10, 10, 1)
	FIELD(GPIO_GPERC1, P11, 11, 1)
	FIELD(GPIO_GPERC1, P12, 12, 1)
	FIELD(GPIO_GPERC1, P13, 13, 1)
	FIELD(GPIO_GPERC1, P14, 14, 1)
	FIELD(GPIO_GPERC1, P15, 15, 1)
	FIELD(GPIO_GPERC1, P16, 16, 1)
	FIELD(GPIO_GPERC1, P17, 17, 1)
	FIELD(GPIO_GPERC1, P18, 18, 1)
	FIELD(GPIO_GPERC1, P19, 19, 1)
	FIELD(GPIO_GPERC1, P20, 20, 1)
	FIELD(GPIO_GPERC1, P21, 21, 1)
	FIELD(GPIO_GPERC1, P22, 22, 1)
	FIELD(GPIO_GPERC1, P23, 23, 1)
	FIELD(GPIO_GPERC1, P24, 24, 1)
	FIELD(GPIO_GPERC1, P25, 25, 1)
	FIELD(GPIO_GPERC1, P26, 26, 1)
	FIELD(GPIO_GPERC1, P27, 27, 1)
	FIELD(GPIO_GPERC1, P28, 28, 1)
	FIELD(GPIO_GPERC1, P29, 29, 1)
	FIELD(GPIO_GPERC1, P30, 30, 1)
	FIELD(GPIO_GPERC1, P31, 31, 1)
REG32(GPIO_GPERC2, 0x408)
	FIELD(GPIO_GPERC2, P0, 0, 1)
	FIELD(GPIO_GPERC2, P1, 1, 1)
	FIELD(GPIO_GPERC2, P2, 2, 1)
	FIELD(GPIO_GPERC2, P3, 3, 1)
	FIELD(GPIO_GPERC2, P4, 4, 1)
	FIELD(GPIO_GPERC2, P5, 5, 1)
	FIELD(GPIO_GPERC2, P6, 6, 1)
	FIELD(GPIO_GPERC2, P7, 7, 1)
	FIELD(GPIO_GPERC2, P8, 8, 1)
	FIELD(GPIO_GPERC2, P9, 9, 1)
	FIELD(GPIO_GPERC2, P10, 10, 1)
	FIELD(GPIO_GPERC2, P11, 11, 1)
	FIELD(GPIO_GPERC2, P12, 12, 1)
	FIELD(GPIO_GPERC2, P13, 13, 1)
	FIELD(GPIO_GPERC2, P14, 14, 1)
	FIELD(GPIO_GPERC2, P15, 15, 1)
	FIELD(GPIO_GPERC2, P16, 16, 1)
	FIELD(GPIO_GPERC2, P17, 17, 1)
	FIELD(GPIO_GPERC2, P18, 18, 1)
	FIELD(GPIO_GPERC2, P19, 19, 1)
	FIELD(GPIO_GPERC2, P20, 20, 1)
	FIELD(GPIO_GPERC2, P21, 21, 1)
	FIELD(GPIO_GPERC2, P22, 22, 1)
	FIELD(GPIO_GPERC2, P23, 23, 1)
	FIELD(GPIO_GPERC2, P24, 24, 1)
	FIELD(GPIO_GPERC2, P25, 25, 1)
	FIELD(GPIO_GPERC2, P26, 26, 1)
	FIELD(GPIO_GPERC2, P27, 27, 1)
	FIELD(GPIO_GPERC2, P28, 28, 1)
	FIELD(GPIO_GPERC2, P29, 29, 1)
	FIELD(GPIO_GPERC2, P30, 30, 1)
	FIELD(GPIO_GPERC2, P31, 31, 1)
REG32(GPIO_GPERS0, 0x4)
	FIELD(GPIO_GPERS0, P0, 0, 1)
	FIELD(GPIO_GPERS0, P1, 1, 1)
	FIELD(GPIO_GPERS0, P2, 2, 1)
	FIELD(GPIO_GPERS0, P3, 3, 1)
	FIELD(GPIO_GPERS0, P4, 4, 1)
	FIELD(GPIO_GPERS0, P5, 5, 1)
	FIELD(GPIO_GPERS0, P6, 6, 1)
	FIELD(GPIO_GPERS0, P7, 7, 1)
	FIELD(GPIO_GPERS0, P8, 8, 1)
	FIELD(GPIO_GPERS0, P9, 9, 1)
	FIELD(GPIO_GPERS0, P10, 10, 1)
	FIELD(GPIO_GPERS0, P11, 11, 1)
	FIELD(GPIO_GPERS0, P12, 12, 1)
	FIELD(GPIO_GPERS0, P13, 13, 1)
	FIELD(GPIO_GPERS0, P14, 14, 1)
	FIELD(GPIO_GPERS0, P15, 15, 1)
	FIELD(GPIO_GPERS0, P16, 16, 1)
	FIELD(GPIO_GPERS0, P17, 17, 1)
	FIELD(GPIO_GPERS0, P18, 18, 1)
	FIELD(GPIO_GPERS0, P19, 19, 1)
	FIELD(GPIO_GPERS0, P20, 20, 1)
	FIELD(GPIO_GPERS0, P21, 21, 1)
	FIELD(GPIO_GPERS0, P22, 22, 1)
	FIELD(GPIO_GPERS0, P23, 23, 1)
	FIELD(GPIO_GPERS0, P24, 24, 1)
	FIELD(GPIO_GPERS0, P25, 25, 1)
	FIELD(GPIO_GPERS0, P26, 26, 1)
	FIELD(GPIO_GPERS0, P27, 27, 1)
	FIELD(GPIO_GPERS0, P28, 28, 1)
	FIELD(GPIO_GPERS0, P29, 29, 1)
	FIELD(GPIO_GPERS0, P30, 30, 1)
	FIELD(GPIO_GPERS0, P31, 31, 1)
REG32(GPIO_GPERS1, 0x204)
	FIELD(GPIO_GPERS1, P0, 0, 1)
	FIELD(GPIO_GPERS1, P1, 1, 1)
	FIELD(GPIO_GPERS1, P2, 2, 1)
	FIELD(GPIO_GPERS1, P3, 3, 1)
	FIELD(GPIO_GPERS1, P4, 4, 1)
	FIELD(GPIO_GPERS1, P5, 5, 1)
	FIELD(GPIO_GPERS1, P6, 6, 1)
	FIELD(GPIO_GPERS1, P7, 7, 1)
	FIELD(GPIO_GPERS1, P8, 8, 1)
	FIELD(GPIO_GPERS1, P9, 9, 1)
	FIELD(GPIO_GPERS1, P10, 10, 1)
	FIELD(GPIO_GPERS1, P11, 11, 1)
	FIELD(GPIO_GPERS1, P12, 12, 1)
	FIELD(GPIO_GPERS1, P13, 13, 1)
	FIELD(GPIO_GPERS1, P14, 14, 1)
	FIELD(GPIO_GPERS1, P15, 15, 1)
	FIELD(GPIO_GPERS1, P16, 16, 1)
	FIELD(GPIO_GPERS1, P17, 17, 1)
	FIELD(GPIO_GPERS1, P18, 18, 1)
	FIELD(GPIO_GPERS1, P19, 19, 1)
	FIELD(GPIO_GPERS1, P20, 20, 1)
	FIELD(GPIO_GPERS1, P21, 21, 1)
	FIELD(GPIO_GPERS1, P22, 22, 1)
	FIELD(GPIO_GPERS1, P23, 23, 1)
	FIELD(GPIO_GPERS1, P24, 24, 1)
	FIELD(GPIO_GPERS1, P25, 25, 1)
	FIELD(GPIO_GPERS1, P26, 26, 1)
	FIELD(GPIO_GPERS1, P27, 27, 1)
	FIELD(GPIO_GPERS1, P28, 28, 1)
	FIELD(GPIO_GPERS1, P29, 29, 1)
	FIELD(GPIO_GPERS1, P30, 30, 1)
	FIELD(GPIO_GPERS1, P31, 31, 1)
REG32(GPIO_GPERS2, 0x404)
	FIELD(GPIO_GPERS2, P0, 0, 1)
	FIELD(GPIO_GPERS2, P1, 1, 1)
	FIELD(GPIO_GPERS2, P2, 2, 1)
	FIELD(GPIO_GPERS2, P3, 3, 1)
	FIELD(GPIO_GPERS2, P4, 4, 1)
	FIELD(GPIO_GPERS2, P5, 5, 1)
	FIELD(GPIO_GPERS2, P6, 6, 1)
	FIELD(GPIO_GPERS2, P7, 7, 1)
	FIELD(GPIO_GPERS2, P8, 8, 1)
	FIELD(GPIO_GPERS2, P9, 9, 1)
	FIELD(GPIO_GPERS2, P10, 10, 1)
	FIELD(GPIO_GPERS2, P11, 11, 1)
	FIELD(GPIO_GPERS2, P12, 12, 1)
	FIELD(GPIO_GPERS2, P13, 13, 1)
	FIELD(GPIO_GPERS2, P14, 14, 1)
	FIELD(GPIO_GPERS2, P15, 15, 1)
	FIELD(GPIO_GPERS2, P16, 16, 1)
	FIELD(GPIO_GPERS2, P17, 17, 1)
	FIELD(GPIO_GPERS2, P18, 18, 1)
	FIELD(GPIO_GPERS2, P19, 19, 1)
	FIELD(GPIO_GPERS2, P20, 20, 1)
	FIELD(GPIO_GPERS2, P21, 21, 1)
	FIELD(GPIO_GPERS2, P22, 22, 1)
	FIELD(GPIO_GPERS2, P23, 23, 1)
	FIELD(GPIO_GPERS2, P24, 24, 1)
	FIELD(GPIO_GPERS2, P25, 25, 1)
	FIELD(GPIO_GPERS2, P26, 26, 1)
	FIELD(GPIO_GPERS2, P27, 27, 1)
	FIELD(GPIO_GPERS2, P28, 28, 1)
	FIELD(GPIO_GPERS2, P29, 29, 1)
	FIELD(GPIO_GPERS2, P30, 30, 1)
	FIELD(GPIO_GPERS2, P31, 31, 1)
REG32(GPIO_GPERT0, 0xc)
	FIELD(GPIO_GPERT0, P0, 0, 1)
	FIELD(GPIO_GPERT0, P1, 1, 1)
	FIELD(GPIO_GPERT0, P2, 2, 1)
	FIELD(GPIO_GPERT0, P3, 3, 1)
	FIELD(GPIO_GPERT0, P4, 4, 1)
	FIELD(GPIO_GPERT0, P5, 5, 1)
	FIELD(GPIO_GPERT0, P6, 6, 1)
	FIELD(GPIO_GPERT0, P7, 7, 1)
	FIELD(GPIO_GPERT0, P8, 8, 1)
	FIELD(GPIO_GPERT0, P9, 9, 1)
	FIELD(GPIO_GPERT0, P10, 10, 1)
	FIELD(GPIO_GPERT0, P11, 11, 1)
	FIELD(GPIO_GPERT0, P12, 12, 1)
	FIELD(GPIO_GPERT0, P13, 13, 1)
	FIELD(GPIO_GPERT0, P14, 14, 1)
	FIELD(GPIO_GPERT0, P15, 15, 1)
	FIELD(GPIO_GPERT0, P16, 16, 1)
	FIELD(GPIO_GPERT0, P17, 17, 1)
	FIELD(GPIO_GPERT0, P18, 18, 1)
	FIELD(GPIO_GPERT0, P19, 19, 1)
	FIELD(GPIO_GPERT0, P20, 20, 1)
	FIELD(GPIO_GPERT0, P21, 21, 1)
	FIELD(GPIO_GPERT0, P22, 22, 1)
	FIELD(GPIO_GPERT0, P23, 23, 1)
	FIELD(GPIO_GPERT0, P24, 24, 1)
	FIELD(GPIO_GPERT0, P25, 25, 1)
	FIELD(GPIO_GPERT0, P26, 26, 1)
	FIELD(GPIO_GPERT0, P27, 27, 1)
	FIELD(GPIO_GPERT0, P28, 28, 1)
	FIELD(GPIO_GPERT0, P29, 29, 1)
	FIELD(GPIO_GPERT0, P30, 30, 1)
	FIELD(GPIO_GPERT0, P31, 31, 1)
REG32(GPIO_GPERT1, 0x20c)
	FIELD(GPIO_GPERT1, P0, 0, 1)
	FIELD(GPIO_GPERT1, P1, 1, 1)
	FIELD(GPIO_GPERT1, P2, 2, 1)
	FIELD(GPIO_GPERT1, P3, 3, 1)
	FIELD(GPIO_GPERT1, P4, 4, 1)
	FIELD(GPIO_GPERT1, P5, 5, 1)
	FIELD(GPIO_GPERT1, P6, 6, 1)
	FIELD(GPIO_GPERT1, P7, 7, 1)
	FIELD(GPIO_GPERT1, P8, 8, 1)
	FIELD(GPIO_GPERT1, P9, 9, 1)
	FIELD(GPIO_GPERT1, P10, 10, 1)
	FIELD(GPIO_GPERT1, P11, 11, 1)
	FIELD(GPIO_GPERT1, P12, 12, 1)
	FIELD(GPIO_GPERT1, P13, 13, 1)
	FIELD(GPIO_GPERT1, P14, 14, 1)
	FIELD(GPIO_GPERT1, P15, 15, 1)
	FIELD(GPIO_GPERT1, P16, 16, 1)
	FIELD(GPIO_GPERT1, P17, 17, 1)
	FIELD(GPIO_GPERT1, P18, 18, 1)
	FIELD(GPIO_GPERT1, P19, 19, 1)
	FIELD(GPIO_GPERT1, P20, 20, 1)
	FIELD(GPIO_GPERT1, P21, 21, 1)
	FIELD(GPIO_GPERT1, P22, 22, 1)
	FIELD(GPIO_GPERT1, P23, 23, 1)
	FIELD(GPIO_GPERT1, P24, 24, 1)
	FIELD(GPIO_GPERT1, P25, 25, 1)
	FIELD(GPIO_GPERT1, P26, 26, 1)
	FIELD(GPIO_GPERT1, P27, 27, 1)
	FIELD(GPIO_GPERT1, P28, 28, 1)
	FIELD(GPIO_GPERT1, P29, 29, 1)
	FIELD(GPIO_GPERT1, P30, 30, 1)
	FIELD(GPIO_GPERT1, P31, 31, 1)
REG32(GPIO_GPERT2, 0x40c)
	FIELD(GPIO_GPERT2, P0, 0, 1)
	FIELD(GPIO_GPERT2, P1, 1, 1)
	FIELD(GPIO_GPERT2, P2, 2, 1)
	FIELD(GPIO_GPERT2, P3, 3, 1)
	FIELD(GPIO_GPERT2, P4, 4, 1)
	FIELD(GPIO_GPERT2, P5, 5, 1)
	FIELD(GPIO_GPERT2, P6, 6, 1)
	FIELD(GPIO_GPERT2, P7, 7, 1)
	FIELD(GPIO_GPERT2, P8, 8, 1)
	FIELD(GPIO_GPERT2, P9, 9, 1)
	FIELD(GPIO_GPERT2, P10, 10, 1)
	FIELD(GPIO_GPERT2, P11, 11, 1)
	FIELD(GPIO_GPERT2, P12, 12, 1)
	FIELD(GPIO_GPERT2, P13, 13, 1)
	FIELD(GPIO_GPERT2, P14, 14, 1)
	FIELD(GPIO_GPERT2, P15, 15, 1)
	FIELD(GPIO_GPERT2, P16, 16, 1)
	FIELD(GPIO_GPERT2, P17, 17, 1)
	FIELD(GPIO_GPERT2, P18, 18, 1)
	FIELD(GPIO_GPERT2, P19, 19, 1)
	FIELD(GPIO_GPERT2, P20, 20, 1)
	FIELD(GPIO_GPERT2, P21, 21, 1)
	FIELD(GPIO_GPERT2, P22, 22, 1)
	FIELD(GPIO_GPERT2, P23, 23, 1)
	FIELD(GPIO_GPERT2, P24, 24, 1)
	FIELD(GPIO_GPERT2, P25, 25, 1)
	FIELD(GPIO_GPERT2, P26, 26, 1)
	FIELD(GPIO_GPERT2, P27, 27, 1)
	FIELD(GPIO_GPERT2, P28, 28, 1)
	FIELD(GPIO_GPERT2, P29, 29, 1)
	FIELD(GPIO_GPERT2, P30, 30, 1)
	FIELD(GPIO_GPERT2, P31, 31, 1)
REG32(GPIO_GPER0, 0x0)
	FIELD(GPIO_GPER0, P0, 0, 1)
	FIELD(GPIO_GPER0, P1, 1, 1)
	FIELD(GPIO_GPER0, P2, 2, 1)
	FIELD(GPIO_GPER0, P3, 3, 1)
	FIELD(GPIO_GPER0, P4, 4, 1)
	FIELD(GPIO_GPER0, P5, 5, 1)
	FIELD(GPIO_GPER0, P6, 6, 1)
	FIELD(GPIO_GPER0, P7, 7, 1)
	FIELD(GPIO_GPER0, P8, 8, 1)
	FIELD(GPIO_GPER0, P9, 9, 1)
	FIELD(GPIO_GPER0, P10, 10, 1)
	FIELD(GPIO_GPER0, P11, 11, 1)
	FIELD(GPIO_GPER0, P12, 12, 1)
	FIELD(GPIO_GPER0, P13, 13, 1)
	FIELD(GPIO_GPER0, P14, 14, 1)
	FIELD(GPIO_GPER0, P15, 15, 1)
	FIELD(GPIO_GPER0, P16, 16, 1)
	FIELD(GPIO_GPER0, P17, 17, 1)
	FIELD(GPIO_GPER0, P18, 18, 1)
	FIELD(GPIO_GPER0, P19, 19, 1)
	FIELD(GPIO_GPER0, P20, 20, 1)
	FIELD(GPIO_GPER0, P21, 21, 1)
	FIELD(GPIO_GPER0, P22, 22, 1)
	FIELD(GPIO_GPER0, P23, 23, 1)
	FIELD(GPIO_GPER0, P24, 24, 1)
	FIELD(GPIO_GPER0, P25, 25, 1)
	FIELD(GPIO_GPER0, P26, 26, 1)
	FIELD(GPIO_GPER0, P27, 27, 1)
	FIELD(GPIO_GPER0, P28, 28, 1)
	FIELD(GPIO_GPER0, P29, 29, 1)
	FIELD(GPIO_GPER0, P30, 30, 1)
	FIELD(GPIO_GPER0, P31, 31, 1)
REG32(GPIO_GPER1, 0x200)
	FIELD(GPIO_GPER1, P0, 0, 1)
	FIELD(GPIO_GPER1, P1, 1, 1)
	FIELD(GPIO_GPER1, P2, 2, 1)
	FIELD(GPIO_GPER1, P3, 3, 1)
	FIELD(GPIO_GPER1, P4, 4, 1)
	FIELD(GPIO_GPER1, P5, 5, 1)
	FIELD(GPIO_GPER1, P6, 6, 1)
	FIELD(GPIO_GPER1, P7, 7, 1)
	FIELD(GPIO_GPER1, P8, 8, 1)
	FIELD(GPIO_GPER1, P9, 9, 1)
	FIELD(GPIO_GPER1, P10, 10, 1)
	FIELD(GPIO_GPER1, P11, 11, 1)
	FIELD(GPIO_GPER1, P12, 12, 1)
	FIELD(GPIO_GPER1, P13, 13, 1)
	FIELD(GPIO_GPER1, P14, 14, 1)
	FIELD(GPIO_GPER1, P15, 15, 1)
	FIELD(GPIO_GPER1, P16, 16, 1)
	FIELD(GPIO_GPER1, P17, 17, 1)
	FIELD(GPIO_GPER1, P18, 18, 1)
	FIELD(GPIO_GPER1, P19, 19, 1)
	FIELD(GPIO_GPER1, P20, 20, 1)
	FIELD(GPIO_GPER1, P21, 21, 1)
	FIELD(GPIO_GPER1, P22, 22, 1)
	FIELD(GPIO_GPER1, P23, 23, 1)
	FIELD(GPIO_GPER1, P24, 24, 1)
	FIELD(GPIO_GPER1, P25, 25, 1)
	FIELD(GPIO_GPER1, P26, 26, 1)
	FIELD(GPIO_GPER1, P27, 27, 1)
	FIELD(GPIO_GPER1, P28, 28, 1)
	FIELD(GPIO_GPER1, P29, 29, 1)
	FIELD(GPIO_GPER1, P30, 30, 1)
	FIELD(GPIO_GPER1, P31, 31, 1)
REG32(GPIO_GPER2, 0x400)
	FIELD(GPIO_GPER2, P0, 0, 1)
	FIELD(GPIO_GPER2, P1, 1, 1)
	FIELD(GPIO_GPER2, P2, 2, 1)
	FIELD(GPIO_GPER2, P3, 3, 1)
	FIELD(GPIO_GPER2, P4, 4, 1)
	FIELD(GPIO_GPER2, P5, 5, 1)
	FIELD(GPIO_GPER2, P6, 6, 1)
	FIELD(GPIO_GPER2, P7, 7, 1)
	FIELD(GPIO_GPER2, P8, 8, 1)
	FIELD(GPIO_GPER2, P9, 9, 1)
	FIELD(GPIO_GPER2, P10, 10, 1)
	FIELD(GPIO_GPER2, P11, 11, 1)
	FIELD(GPIO_GPER2, P12, 12, 1)
	FIELD(GPIO_GPER2, P13, 13, 1)
	FIELD(GPIO_GPER2, P14, 14, 1)
	FIELD(GPIO_GPER2, P15, 15, 1)
	FIELD(GPIO_GPER2, P16, 16, 1)
	FIELD(GPIO_GPER2, P17, 17, 1)
	FIELD(GPIO_GPER2, P18, 18, 1)
	FIELD(GPIO_GPER2, P19, 19, 1)
	FIELD(GPIO_GPER2, P20, 20, 1)
	FIELD(GPIO_GPER2, P21, 21, 1)
	FIELD(GPIO_GPER2, P22, 22, 1)
	FIELD(GPIO_GPER2, P23, 23, 1)
	FIELD(GPIO_GPER2, P24, 24, 1)
	FIELD(GPIO_GPER2, P25, 25, 1)
	FIELD(GPIO_GPER2, P26, 26, 1)
	FIELD(GPIO_GPER2, P27, 27, 1)
	FIELD(GPIO_GPER2, P28, 28, 1)
	FIELD(GPIO_GPER2, P29, 29, 1)
	FIELD(GPIO_GPER2, P30, 30, 1)
	FIELD(GPIO_GPER2, P31, 31, 1)
REG32(GPIO_IERC0, 0x98)
	FIELD(GPIO_IERC0, P0, 0, 1)
	FIELD(GPIO_IERC0, P1, 1, 1)
	FIELD(GPIO_IERC0, P2, 2, 1)
	FIELD(GPIO_IERC0, P3, 3, 1)
	FIELD(GPIO_IERC0, P4, 4, 1)
	FIELD(GPIO_IERC0, P5, 5, 1)
	FIELD(GPIO_IERC0, P6, 6, 1)
	FIELD(GPIO_IERC0, P7, 7, 1)
	FIELD(GPIO_IERC0, P8, 8, 1)
	FIELD(GPIO_IERC0, P9, 9, 1)
	FIELD(GPIO_IERC0, P10, 10, 1)
	FIELD(GPIO_IERC0, P11, 11, 1)
	FIELD(GPIO_IERC0, P12, 12, 1)
	FIELD(GPIO_IERC0, P13, 13, 1)
	FIELD(GPIO_IERC0, P14, 14, 1)
	FIELD(GPIO_IERC0, P15, 15, 1)
	FIELD(GPIO_IERC0, P16, 16, 1)
	FIELD(GPIO_IERC0, P17, 17, 1)
	FIELD(GPIO_IERC0, P18, 18, 1)
	FIELD(GPIO_IERC0, P19, 19, 1)
	FIELD(GPIO_IERC0, P20, 20, 1)
	FIELD(GPIO_IERC0, P21, 21, 1)
	FIELD(GPIO_IERC0, P22, 22, 1)
	FIELD(GPIO_IERC0, P23, 23, 1)
	FIELD(GPIO_IERC0, P24, 24, 1)
	FIELD(GPIO_IERC0, P25, 25, 1)
	FIELD(GPIO_IERC0, P26, 26, 1)
	FIELD(GPIO_IERC0, P27, 27, 1)
	FIELD(GPIO_IERC0, P28, 28, 1)
	FIELD(GPIO_IERC0, P29, 29, 1)
	FIELD(GPIO_IERC0, P30, 30, 1)
	FIELD(GPIO_IERC0, P31, 31, 1)
REG32(GPIO_IERC1, 0x298)
	FIELD(GPIO_IERC1, P0, 0, 1)
	FIELD(GPIO_IERC1, P1, 1, 1)
	FIELD(GPIO_IERC1, P2, 2, 1)
	FIELD(GPIO_IERC1, P3, 3, 1)
	FIELD(GPIO_IERC1, P4, 4, 1)
	FIELD(GPIO_IERC1, P5, 5, 1)
	FIELD(GPIO_IERC1, P6, 6, 1)
	FIELD(GPIO_IERC1, P7, 7, 1)
	FIELD(GPIO_IERC1, P8, 8, 1)
	FIELD(GPIO_IERC1, P9, 9, 1)
	FIELD(GPIO_IERC1, P10, 10, 1)
	FIELD(GPIO_IERC1, P11, 11, 1)
	FIELD(GPIO_IERC1, P12, 12, 1)
	FIELD(GPIO_IERC1, P13, 13, 1)
	FIELD(GPIO_IERC1, P14, 14, 1)
	FIELD(GPIO_IERC1, P15, 15, 1)
	FIELD(GPIO_IERC1, P16, 16, 1)
	FIELD(GPIO_IERC1, P17, 17, 1)
	FIELD(GPIO_IERC1, P18, 18, 1)
	FIELD(GPIO_IERC1, P19, 19, 1)
	FIELD(GPIO_IERC1, P20, 20, 1)
	FIELD(GPIO_IERC1, P21, 21, 1)
	FIELD(GPIO_IERC1, P22, 22, 1)
	FIELD(GPIO_IERC1, P23, 23, 1)
	FIELD(GPIO_IERC1, P24, 24, 1)
	FIELD(GPIO_IERC1, P25, 25, 1)
	FIELD(GPIO_IERC1, P26, 26, 1)
	FIELD(GPIO_IERC1, P27, 27, 1)
	FIELD(GPIO_IERC1, P28, 28, 1)
	FIELD(GPIO_IERC1, P29, 29, 1)
	FIELD(GPIO_IERC1, P30, 30, 1)
	FIELD(GPIO_IERC1, P31, 31, 1)
REG32(GPIO_IERC2, 0x498)
	FIELD(GPIO_IERC2, P0, 0, 1)
	FIELD(GPIO_IERC2, P1, 1, 1)
	FIELD(GPIO_IERC2, P2, 2, 1)
	FIELD(GPIO_IERC2, P3, 3, 1)
	FIELD(GPIO_IERC2, P4, 4, 1)
	FIELD(GPIO_IERC2, P5, 5, 1)
	FIELD(GPIO_IERC2, P6, 6, 1)
	FIELD(GPIO_IERC2, P7, 7, 1)
	FIELD(GPIO_IERC2, P8, 8, 1)
	FIELD(GPIO_IERC2, P9, 9, 1)
	FIELD(GPIO_IERC2, P10, 10, 1)
	FIELD(GPIO_IERC2, P11, 11, 1)
	FIELD(GPIO_IERC2, P12, 12, 1)
	FIELD(GPIO_IERC2, P13, 13, 1)
	FIELD(GPIO_IERC2, P14, 14, 1)
	FIELD(GPIO_IERC2, P15, 15, 1)
	FIELD(GPIO_IERC2, P16, 16, 1)
	FIELD(GPIO_IERC2, P17, 17, 1)
	FIELD(GPIO_IERC2, P18, 18, 1)
	FIELD(GPIO_IERC2, P19, 19, 1)
	FIELD(GPIO_IERC2, P20, 20, 1)
	FIELD(GPIO_IERC2, P21, 21, 1)
	FIELD(GPIO_IERC2, P22, 22, 1)
	FIELD(GPIO_IERC2, P23, 23, 1)
	FIELD(GPIO_IERC2, P24, 24, 1)
	FIELD(GPIO_IERC2, P25, 25, 1)
	FIELD(GPIO_IERC2, P26, 26, 1)
	FIELD(GPIO_IERC2, P27, 27, 1)
	FIELD(GPIO_IERC2, P28, 28, 1)
	FIELD(GPIO_IERC2, P29, 29, 1)
	FIELD(GPIO_IERC2, P30, 30, 1)
	FIELD(GPIO_IERC2, P31, 31, 1)
REG32(GPIO_IERS0, 0x94)
	FIELD(GPIO_IERS0, P0, 0, 1)
	FIELD(GPIO_IERS0, P1, 1, 1)
	FIELD(GPIO_IERS0, P2, 2, 1)
	FIELD(GPIO_IERS0, P3, 3, 1)
	FIELD(GPIO_IERS0, P4, 4, 1)
	FIELD(GPIO_IERS0, P5, 5, 1)
	FIELD(GPIO_IERS0, P6, 6, 1)
	FIELD(GPIO_IERS0, P7, 7, 1)
	FIELD(GPIO_IERS0, P8, 8, 1)
	FIELD(GPIO_IERS0, P9, 9, 1)
	FIELD(GPIO_IERS0, P10, 10, 1)
	FIELD(GPIO_IERS0, P11, 11, 1)
	FIELD(GPIO_IERS0, P12, 12, 1)
	FIELD(GPIO_IERS0, P13, 13, 1)
	FIELD(GPIO_IERS0, P14, 14, 1)
	FIELD(GPIO_IERS0, P15, 15, 1)
	FIELD(GPIO_IERS0, P16, 16, 1)
	FIELD(GPIO_IERS0, P17, 17, 1)
	FIELD(GPIO_IERS0, P18, 18, 1)
	FIELD(GPIO_IERS0, P19, 19, 1)
	FIELD(GPIO_IERS0, P20, 20, 1)
	FIELD(GPIO_IERS0, P21, 21, 1)
	FIELD(GPIO_IERS0, P22, 22, 1)
	FIELD(GPIO_IERS0, P23, 23, 1)
	FIELD(GPIO_IERS0, P24, 24, 1)
	FIELD(GPIO_IERS0, P25, 25, 1)
	FIELD(GPIO_IERS0, P26, 26, 1)
	FIELD(GPIO_IERS0, P27, 27, 1)
	FIELD(GPIO_IERS0, P28, 28, 1)
	FIELD(GPIO_IERS0, P29, 29, 1)
	FIELD(GPIO_IERS0, P30, 30, 1)
	FIELD(GPIO_IERS0, P31, 31, 1)
REG32(GPIO_IERS1, 0x294)
	FIELD(GPIO_IERS1, P0, 0, 1)
	FIELD(GPIO_IERS1, P1, 1, 1)
	FIELD(GPIO_IERS1, P2, 2, 1)
	FIELD(GPIO_IERS1, P3, 3, 1)
	FIELD(GPIO_IERS1, P4, 4, 1)
	FIELD(GPIO_IERS1, P5, 5, 1)
	FIELD(GPIO_IERS1, P6, 6, 1)
	FIELD(GPIO_IERS1, P7, 7, 1)
	FIELD(GPIO_IERS1, P8, 8, 1)
	FIELD(GPIO_IERS1, P9, 9, 1)
	FIELD(GPIO_IERS1, P10, 10, 1)
	FIELD(GPIO_IERS1, P11, 11, 1)
	FIELD(GPIO_IERS1, P12, 12, 1)
	FIELD(GPIO_IERS1, P13, 13, 1)
	FIELD(GPIO_IERS1, P14, 14, 1)
	FIELD(GPIO_IERS1, P15, 15, 1)
	FIELD(GPIO_IERS1, P16, 16, 1)
	FIELD(GPIO_IERS1, P17, 17, 1)
	FIELD(GPIO_IERS1, P18, 18, 1)
	FIELD(GPIO_IERS1, P19, 19, 1)
	FIELD(GPIO_IERS1, P20, 20, 1)
	FIELD(GPIO_IERS1, P21, 21, 1)
	FIELD(GPIO_IERS1, P22, 22, 1)
	FIELD(GPIO_IERS1, P23, 23, 1)
	FIELD(GPIO_IERS1, P24, 24, 1)
	FIELD(GPIO_IERS1, P25, 25, 1)
	FIELD(GPIO_IERS1, P26, 26, 1)
	FIELD(GPIO_IERS1, P27, 27, 1)
	FIELD(GPIO_IERS1, P28, 28, 1)
	FIELD(GPIO_IERS1, P29, 29, 1)
	FIELD(GPIO_IERS1, P30, 30, 1)
	FIELD(GPIO_IERS1, P31, 31, 1)
REG32(GPIO_IERS2, 0x494)
	FIELD(GPIO_IERS2, P0, 0, 1)
	FIELD(GPIO_IERS2, P1, 1, 1)
	FIELD(GPIO_IERS2, P2, 2, 1)
	FIELD(GPIO_IERS2, P3, 3, 1)
	FIELD(GPIO_IERS2, P4, 4, 1)
	FIELD(GPIO_IERS2, P5, 5, 1)
	FIELD(GPIO_IERS2, P6, 6, 1)
	FIELD(GPIO_IERS2, P7, 7, 1)
	FIELD(GPIO_IERS2, P8, 8, 1)
	FIELD(GPIO_IERS2, P9, 9, 1)
	FIELD(GPIO_IERS2, P10, 10, 1)
	FIELD(GPIO_IERS2, P11, 11, 1)
	FIELD(GPIO_IERS2, P12, 12, 1)
	FIELD(GPIO_IERS2, P13, 13, 1)
	FIELD(GPIO_IERS2, P14, 14, 1)
	FIELD(GPIO_IERS2, P15, 15, 1)
	FIELD(GPIO_IERS2, P16, 16, 1)
	FIELD(GPIO_IERS2, P17, 17, 1)
	FIELD(GPIO_IERS2, P18, 18, 1)
	FIELD(GPIO_IERS2, P19, 19, 1)
	FIELD(GPIO_IERS2, P20, 20, 1)
	FIELD(GPIO_IERS2, P21, 21, 1)
	FIELD(GPIO_IERS2, P22, 22, 1)
	FIELD(GPIO_IERS2, P23, 23, 1)
	FIELD(GPIO_IERS2, P24, 24, 1)
	FIELD(GPIO_IERS2, P25, 25, 1)
	FIELD(GPIO_IERS2, P26, 26, 1)
	FIELD(GPIO_IERS2, P27, 27, 1)
	FIELD(GPIO_IERS2, P28, 28, 1)
	FIELD(GPIO_IERS2, P29, 29, 1)
	FIELD(GPIO_IERS2, P30, 30, 1)
	FIELD(GPIO_IERS2, P31, 31, 1)
REG32(GPIO_IERT0, 0x9c)
	FIELD(GPIO_IERT0, P0, 0, 1)
	FIELD(GPIO_IERT0, P1, 1, 1)
	FIELD(GPIO_IERT0, P2, 2, 1)
	FIELD(GPIO_IERT0, P3, 3, 1)
	FIELD(GPIO_IERT0, P4, 4, 1)
	FIELD(GPIO_IERT0, P5, 5, 1)
	FIELD(GPIO_IERT0, P6, 6, 1)
	FIELD(GPIO_IERT0, P7, 7, 1)
	FIELD(GPIO_IERT0, P8, 8, 1)
	FIELD(GPIO_IERT0, P9, 9, 1)
	FIELD(GPIO_IERT0, P10, 10, 1)
	FIELD(GPIO_IERT0, P11, 11, 1)
	FIELD(GPIO_IERT0, P12, 12, 1)
	FIELD(GPIO_IERT0, P13, 13, 1)
	FIELD(GPIO_IERT0, P14, 14, 1)
	FIELD(GPIO_IERT0, P15, 15, 1)
	FIELD(GPIO_IERT0, P16, 16, 1)
	FIELD(GPIO_IERT0, P17, 17, 1)
	FIELD(GPIO_IERT0, P18, 18, 1)
	FIELD(GPIO_IERT0, P19, 19, 1)
	FIELD(GPIO_IERT0, P20, 20, 1)
	FIELD(GPIO_IERT0, P21, 21, 1)
	FIELD(GPIO_IERT0, P22, 22, 1)
	FIELD(GPIO_IERT0, P23, 23, 1)
	FIELD(GPIO_IERT0, P24, 24, 1)
	FIELD(GPIO_IERT0, P25, 25, 1)
	FIELD(GPIO_IERT0, P26, 26, 1)
	FIELD(GPIO_IERT0, P27, 27, 1)
	FIELD(GPIO_IERT0, P28, 28, 1)
	FIELD(GPIO_IERT0, P29, 29, 1)
	FIELD(GPIO_IERT0, P30, 30, 1)
	FIELD(GPIO_IERT0, P31, 31, 1)
REG32(GPIO_IERT1, 0x29c)
	FIELD(GPIO_IERT1, P0, 0, 1)
	FIELD(GPIO_IERT1, P1, 1, 1)
	FIELD(GPIO_IERT1, P2, 2, 1)
	FIELD(GPIO_IERT1, P3, 3, 1)
	FIELD(GPIO_IERT1, P4, 4, 1)
	FIELD(GPIO_IERT1, P5, 5, 1)
	FIELD(GPIO_IERT1, P6, 6, 1)
	FIELD(GPIO_IERT1, P7, 7, 1)
	FIELD(GPIO_IERT1, P8, 8, 1)
	FIELD(GPIO_IERT1, P9, 9, 1)
	FIELD(GPIO_IERT1, P10, 10, 1)
	FIELD(GPIO_IERT1, P11, 11, 1)
	FIELD(GPIO_IERT1, P12, 12, 1)
	FIELD(GPIO_IERT1, P13, 13, 1)
	FIELD(GPIO_IERT1, P14, 14, 1)
	FIELD(GPIO_IERT1, P15, 15, 1)
	FIELD(GPIO_IERT1, P16, 16, 1)
	FIELD(GPIO_IERT1, P17, 17, 1)
	FIELD(GPIO_IERT1, P18, 18, 1)
	FIELD(GPIO_IERT1, P19, 19, 1)
	FIELD(GPIO_IERT1, P20, 20, 1)
	FIELD(GPIO_IERT1, P21, 21, 1)
	FIELD(GPIO_IERT1, P22, 22, 1)
	FIELD(GPIO_IERT1, P23, 23, 1)
	FIELD(GPIO_IERT1, P24, 24, 1)
	FIELD(GPIO_IERT1, P25, 25, 1)
	FIELD(GPIO_IERT1, P26, 26, 1)
	FIELD(GPIO_IERT1, P27, 27, 1)
	FIELD(GPIO_IERT1, P28, 28, 1)
	FIELD(GPIO_IERT1, P29, 29, 1)
	FIELD(GPIO_IERT1, P30, 30, 1)
	FIELD(GPIO_IERT1, P31, 31, 1)
REG32(GPIO_IERT2, 0x49c)
	FIELD(GPIO_IERT2, P0, 0, 1)
	FIELD(GPIO_IERT2, P1, 1, 1)
	FIELD(GPIO_IERT2, P2, 2, 1)
	FIELD(GPIO_IERT2, P3, 3, 1)
	FIELD(GPIO_IERT2, P4, 4, 1)
	FIELD(GPIO_IERT2, P5, 5, 1)
	FIELD(GPIO_IERT2, P6, 6, 1)
	FIELD(GPIO_IERT2, P7, 7, 1)
	FIELD(GPIO_IERT2, P8, 8, 1)
	FIELD(GPIO_IERT2, P9, 9, 1)
	FIELD(GPIO_IERT2, P10, 10, 1)
	FIELD(GPIO_IERT2, P11, 11, 1)
	FIELD(GPIO_IERT2, P12, 12, 1)
	FIELD(GPIO_IERT2, P13, 13, 1)
	FIELD(GPIO_IERT2, P14, 14, 1)
	FIELD(GPIO_IERT2, P15, 15, 1)
	FIELD(GPIO_IERT2, P16, 16, 1)
	FIELD(GPIO_IERT2, P17, 17, 1)
	FIELD(GPIO_IERT2, P18, 18, 1)
	FIELD(GPIO_IERT2, P19, 19, 1)
	FIELD(GPIO_IERT2, P20, 20, 1)
	FIELD(GPIO_IERT2, P21, 21, 1)
	FIELD(GPIO_IERT2, P22, 22, 1)
	FIELD(GPIO_IERT2, P23, 23, 1)
	FIELD(GPIO_IERT2, P24, 24, 1)
	FIELD(GPIO_IERT2, P25, 25, 1)
	FIELD(GPIO_IERT2, P26, 26, 1)
	FIELD(GPIO_IERT2, P27, 27, 1)
	FIELD(GPIO_IERT2, P28, 28, 1)
	FIELD(GPIO_IERT2, P29, 29, 1)
	FIELD(GPIO_IERT2, P30, 30, 1)
	FIELD(GPIO_IERT2, P31, 31, 1)
REG32(GPIO_IER0, 0x90)
	FIELD(GPIO_IER0, P0, 0, 1)
	FIELD(GPIO_IER0, P1, 1, 1)
	FIELD(GPIO_IER0, P2, 2, 1)
	FIELD(GPIO_IER0, P3, 3, 1)
	FIELD(GPIO_IER0, P4, 4, 1)
	FIELD(GPIO_IER0, P5, 5, 1)
	FIELD(GPIO_IER0, P6, 6, 1)
	FIELD(GPIO_IER0, P7, 7, 1)
	FIELD(GPIO_IER0, P8, 8, 1)
	FIELD(GPIO_IER0, P9, 9, 1)
	FIELD(GPIO_IER0, P10, 10, 1)
	FIELD(GPIO_IER0, P11, 11, 1)
	FIELD(GPIO_IER0, P12, 12, 1)
	FIELD(GPIO_IER0, P13, 13, 1)
	FIELD(GPIO_IER0, P14, 14, 1)
	FIELD(GPIO_IER0, P15, 15, 1)
	FIELD(GPIO_IER0, P16, 16, 1)
	FIELD(GPIO_IER0, P17, 17, 1)
	FIELD(GPIO_IER0, P18, 18, 1)
	FIELD(GPIO_IER0, P19, 19, 1)
	FIELD(GPIO_IER0, P20, 20, 1)
	FIELD(GPIO_IER0, P21, 21, 1)
	FIELD(GPIO_IER0, P22, 22, 1)
	FIELD(GPIO_IER0, P23, 23, 1)
	FIELD(GPIO_IER0, P24, 24, 1)
	FIELD(GPIO_IER0, P25, 25, 1)
	FIELD(GPIO_IER0, P26, 26, 1)
	FIELD(GPIO_IER0, P27, 27, 1)
	FIELD(GPIO_IER0, P28, 28, 1)
	FIELD(GPIO_IER0, P29, 29, 1)
	FIELD(GPIO_IER0, P30, 30, 1)
	FIELD(GPIO_IER0, P31, 31, 1)
REG32(GPIO_IER1, 0x290)
	FIELD(GPIO_IER1, P0, 0, 1)
	FIELD(GPIO_IER1, P1, 1, 1)
	FIELD(GPIO_IER1, P2, 2, 1)
	FIELD(GPIO_IER1, P3, 3, 1)
	FIELD(GPIO_IER1, P4, 4, 1)
	FIELD(GPIO_IER1, P5, 5, 1)
	FIELD(GPIO_IER1, P6, 6, 1)
	FIELD(GPIO_IER1, P7, 7, 1)
	FIELD(GPIO_IER1, P8, 8, 1)
	FIELD(GPIO_IER1, P9, 9, 1)
	FIELD(GPIO_IER1, P10, 10, 1)
	FIELD(GPIO_IER1, P11, 11, 1)
	FIELD(GPIO_IER1, P12, 12, 1)
	FIELD(GPIO_IER1, P13, 13, 1)
	FIELD(GPIO_IER1, P14, 14, 1)
	FIELD(GPIO_IER1, P15, 15, 1)
	FIELD(GPIO_IER1, P16, 16, 1)
	FIELD(GPIO_IER1, P17, 17, 1)
	FIELD(GPIO_IER1, P18, 18, 1)
	FIELD(GPIO_IER1, P19, 19, 1)
	FIELD(GPIO_IER1, P20, 20, 1)
	FIELD(GPIO_IER1, P21, 21, 1)
	FIELD(GPIO_IER1, P22, 22, 1)
	FIELD(GPIO_IER1, P23, 23, 1)
	FIELD(GPIO_IER1, P24, 24, 1)
	FIELD(GPIO_IER1, P25, 25, 1)
	FIELD(GPIO_IER1, P26, 26, 1)
	FIELD(GPIO_IER1, P27, 27, 1)
	FIELD(GPIO_IER1, P28, 28, 1)
	FIELD(GPIO_IER1, P29, 29, 1)
	FIELD(GPIO_IER1, P30, 30, 1)
	FIELD(GPIO_IER1, P31, 31, 1)
REG32(GPIO_IER2, 0x490)
	FIELD(GPIO_IER2, P0, 0, 1)
	FIELD(GPIO_IER2, P1, 1, 1)
	FIELD(GPIO_IER2, P2, 2, 1)
	FIELD(GPIO_IER2, P3, 3, 1)
	FIELD(GPIO_IER2, P4, 4, 1)
	FIELD(GPIO_IER2, P5, 5, 1)
	FIELD(GPIO_IER2, P6, 6, 1)
	FIELD(GPIO_IER2, P7, 7, 1)
	FIELD(GPIO_IER2, P8, 8, 1)
	FIELD(GPIO_IER2, P9, 9, 1)
	FIELD(GPIO_IER2, P10, 10, 1)
	FIELD(GPIO_IER2, P11, 11, 1)
	FIELD(GPIO_IER2, P12, 12, 1)
	FIELD(GPIO_IER2, P13, 13, 1)
	FIELD(GPIO_IER2, P14, 14, 1)
	FIELD(GPIO_IER2, P15, 15, 1)
	FIELD(GPIO_IER2, P16, 16, 1)
	FIELD(GPIO_IER2, P17, 17, 1)
	FIELD(GPIO_IER2, P18, 18, 1)
	FIELD(GPIO_IER2, P19, 19, 1)
	FIELD(GPIO_IER2, P20, 20, 1)
	FIELD(GPIO_IER2, P21, 21, 1)
	FIELD(GPIO_IER2, P22, 22, 1)
	FIELD(GPIO_IER2, P23, 23, 1)
	FIELD(GPIO_IER2, P24, 24, 1)
	FIELD(GPIO_IER2, P25, 25, 1)
	FIELD(GPIO_IER2, P26, 26, 1)
	FIELD(GPIO_IER2, P27, 27, 1)
	FIELD(GPIO_IER2, P28, 28, 1)
	FIELD(GPIO_IER2, P29, 29, 1)
	FIELD(GPIO_IER2, P30, 30, 1)
	FIELD(GPIO_IER2, P31, 31, 1)
REG32(GPIO_IFRC0, 0xd8)
	FIELD(GPIO_IFRC0, P0, 0, 1)
	FIELD(GPIO_IFRC0, P1, 1, 1)
	FIELD(GPIO_IFRC0, P2, 2, 1)
	FIELD(GPIO_IFRC0, P3, 3, 1)
	FIELD(GPIO_IFRC0, P4, 4, 1)
	FIELD(GPIO_IFRC0, P5, 5, 1)
	FIELD(GPIO_IFRC0, P6, 6, 1)
	FIELD(GPIO_IFRC0, P7, 7, 1)
	FIELD(GPIO_IFRC0, P8, 8, 1)
	FIELD(GPIO_IFRC0, P9, 9, 1)
	FIELD(GPIO_IFRC0, P10, 10, 1)
	FIELD(GPIO_IFRC0, P11, 11, 1)
	FIELD(GPIO_IFRC0, P12, 12, 1)
	FIELD(GPIO_IFRC0, P13, 13, 1)
	FIELD(GPIO_IFRC0, P14, 14, 1)
	FIELD(GPIO_IFRC0, P15, 15, 1)
	FIELD(GPIO_IFRC0, P16, 16, 1)
	FIELD(GPIO_IFRC0, P17, 17, 1)
	FIELD(GPIO_IFRC0, P18, 18, 1)
	FIELD(GPIO_IFRC0, P19, 19, 1)
	FIELD(GPIO_IFRC0, P20, 20, 1)
	FIELD(GPIO_IFRC0, P21, 21, 1)
	FIELD(GPIO_IFRC0, P22, 22, 1)
	FIELD(GPIO_IFRC0, P23, 23, 1)
	FIELD(GPIO_IFRC0, P24, 24, 1)
	FIELD(GPIO_IFRC0, P25, 25, 1)
	FIELD(GPIO_IFRC0, P26, 26, 1)
	FIELD(GPIO_IFRC0, P27, 27, 1)
	FIELD(GPIO_IFRC0, P28, 28, 1)
	FIELD(GPIO_IFRC0, P29, 29, 1)
	FIELD(GPIO_IFRC0, P30, 30, 1)
	FIELD(GPIO_IFRC0, P31, 31, 1)
REG32(GPIO_IFRC1, 0x2d8)
	FIELD(GPIO_IFRC1, P0, 0, 1)
	FIELD(GPIO_IFRC1, P1, 1, 1)
	FIELD(GPIO_IFRC1, P2, 2, 1)
	FIELD(GPIO_IFRC1, P3, 3, 1)
	FIELD(GPIO_IFRC1, P4, 4, 1)
	FIELD(GPIO_IFRC1, P5, 5, 1)
	FIELD(GPIO_IFRC1, P6, 6, 1)
	FIELD(GPIO_IFRC1, P7, 7, 1)
	FIELD(GPIO_IFRC1, P8, 8, 1)
	FIELD(GPIO_IFRC1, P9, 9, 1)
	FIELD(GPIO_IFRC1, P10, 10, 1)
	FIELD(GPIO_IFRC1, P11, 11, 1)
	FIELD(GPIO_IFRC1, P12, 12, 1)
	FIELD(GPIO_IFRC1, P13, 13, 1)
	FIELD(GPIO_IFRC1, P14, 14, 1)
	FIELD(GPIO_IFRC1, P15, 15, 1)
	FIELD(GPIO_IFRC1, P16, 16, 1)
	FIELD(GPIO_IFRC1, P17, 17, 1)
	FIELD(GPIO_IFRC1, P18, 18, 1)
	FIELD(GPIO_IFRC1, P19, 19, 1)
	FIELD(GPIO_IFRC1, P20, 20, 1)
	FIELD(GPIO_IFRC1, P21, 21, 1)
	FIELD(GPIO_IFRC1, P22, 22, 1)
	FIELD(GPIO_IFRC1, P23, 23, 1)
	FIELD(GPIO_IFRC1, P24, 24, 1)
	FIELD(GPIO_IFRC1, P25, 25, 1)
	FIELD(GPIO_IFRC1, P26, 26, 1)
	FIELD(GPIO_IFRC1, P27, 27, 1)
	FIELD(GPIO_IFRC1, P28, 28, 1)
	FIELD(GPIO_IFRC1, P29, 29, 1)
	FIELD(GPIO_IFRC1, P30, 30, 1)
	FIELD(GPIO_IFRC1, P31, 31, 1)
REG32(GPIO_IFRC2, 0x4d8)
	FIELD(GPIO_IFRC2, P0, 0, 1)
	FIELD(GPIO_IFRC2, P1, 1, 1)
	FIELD(GPIO_IFRC2, P2, 2, 1)
	FIELD(GPIO_IFRC2, P3, 3, 1)
	FIELD(GPIO_IFRC2, P4, 4, 1)
	FIELD(GPIO_IFRC2, P5, 5, 1)
	FIELD(GPIO_IFRC2, P6, 6, 1)
	FIELD(GPIO_IFRC2, P7, 7, 1)
	FIELD(GPIO_IFRC2, P8, 8, 1)
	FIELD(GPIO_IFRC2, P9, 9, 1)
	FIELD(GPIO_IFRC2, P10, 10, 1)
	FIELD(GPIO_IFRC2, P11, 11, 1)
	FIELD(GPIO_IFRC2, P12, 12, 1)
	FIELD(GPIO_IFRC2, P13, 13, 1)
	FIELD(GPIO_IFRC2, P14, 14, 1)
	FIELD(GPIO_IFRC2, P15, 15, 1)
	FIELD(GPIO_IFRC2, P16, 16, 1)
	FIELD(GPIO_IFRC2, P17, 17, 1)
	FIELD(GPIO_IFRC2, P18, 18, 1)
	FIELD(GPIO_IFRC2, P19, 19, 1)
	FIELD(GPIO_IFRC2, P20, 20, 1)
	FIELD(GPIO_IFRC2, P21, 21, 1)
	FIELD(GPIO_IFRC2, P22, 22, 1)
	FIELD(GPIO_IFRC2, P23, 23, 1)
	FIELD(GPIO_IFRC2, P24, 24, 1)
	FIELD(GPIO_IFRC2, P25, 25, 1)
	FIELD(GPIO_IFRC2, P26, 26, 1)
	FIELD(GPIO_IFRC2, P27, 27, 1)
	FIELD(GPIO_IFRC2, P28, 28, 1)
	FIELD(GPIO_IFRC2, P29, 29, 1)
	FIELD(GPIO_IFRC2, P30, 30, 1)
	FIELD(GPIO_IFRC2, P31, 31, 1)
REG32(GPIO_IFR0, 0xd0)
	FIELD(GPIO_IFR0, P0, 0, 1)
	FIELD(GPIO_IFR0, P1, 1, 1)
	FIELD(GPIO_IFR0, P2, 2, 1)
	FIELD(GPIO_IFR0, P3, 3, 1)
	FIELD(GPIO_IFR0, P4, 4, 1)
	FIELD(GPIO_IFR0, P5, 5, 1)
	FIELD(GPIO_IFR0, P6, 6, 1)
	FIELD(GPIO_IFR0, P7, 7, 1)
	FIELD(GPIO_IFR0, P8, 8, 1)
	FIELD(GPIO_IFR0, P9, 9, 1)
	FIELD(GPIO_IFR0, P10, 10, 1)
	FIELD(GPIO_IFR0, P11, 11, 1)
	FIELD(GPIO_IFR0, P12, 12, 1)
	FIELD(GPIO_IFR0, P13, 13, 1)
	FIELD(GPIO_IFR0, P14, 14, 1)
	FIELD(GPIO_IFR0, P15, 15, 1)
	FIELD(GPIO_IFR0, P16, 16, 1)
	FIELD(GPIO_IFR0, P17, 17, 1)
	FIELD(GPIO_IFR0, P18, 18, 1)
	FIELD(GPIO_IFR0, P19, 19, 1)
	FIELD(GPIO_IFR0, P20, 20, 1)
	FIELD(GPIO_IFR0, P21, 21, 1)
	FIELD(GPIO_IFR0, P22, 22, 1)
	FIELD(GPIO_IFR0, P23, 23, 1)
	FIELD(GPIO_IFR0, P24, 24, 1)
	FIELD(GPIO_IFR0, P25, 25, 1)
	FIELD(GPIO_IFR0, P26, 26, 1)
	FIELD(GPIO_IFR0, P27, 27, 1)
	FIELD(GPIO_IFR0, P28, 28, 1)
	FIELD(GPIO_IFR0, P29, 29, 1)
	FIELD(GPIO_IFR0, P30, 30, 1)
	FIELD(GPIO_IFR0, P31, 31, 1)
REG32(GPIO_IFR1, 0x2d0)
	FIELD(GPIO_IFR1, P0, 0, 1)
	FIELD(GPIO_IFR1, P1, 1, 1)
	FIELD(GPIO_IFR1, P2, 2, 1)
	FIELD(GPIO_IFR1, P3, 3, 1)
	FIELD(GPIO_IFR1, P4, 4, 1)
	FIELD(GPIO_IFR1, P5, 5, 1)
	FIELD(GPIO_IFR1, P6, 6, 1)
	FIELD(GPIO_IFR1, P7, 7, 1)
	FIELD(GPIO_IFR1, P8, 8, 1)
	FIELD(GPIO_IFR1, P9, 9, 1)
	FIELD(GPIO_IFR1, P10, 10, 1)
	FIELD(GPIO_IFR1, P11, 11, 1)
	FIELD(GPIO_IFR1, P12, 12, 1)
	FIELD(GPIO_IFR1, P13, 13, 1)
	FIELD(GPIO_IFR1, P14, 14, 1)
	FIELD(GPIO_IFR1, P15, 15, 1)
	FIELD(GPIO_IFR1, P16, 16, 1)
	FIELD(GPIO_IFR1, P17, 17, 1)
	FIELD(GPIO_IFR1, P18, 18, 1)
	FIELD(GPIO_IFR1, P19, 19, 1)
	FIELD(GPIO_IFR1, P20, 20, 1)
	FIELD(GPIO_IFR1, P21, 21, 1)
	FIELD(GPIO_IFR1, P22, 22, 1)
	FIELD(GPIO_IFR1, P23, 23, 1)
	FIELD(GPIO_IFR1, P24, 24, 1)
	FIELD(GPIO_IFR1, P25, 25, 1)
	FIELD(GPIO_IFR1, P26, 26, 1)
	FIELD(GPIO_IFR1, P27, 27, 1)
	FIELD(GPIO_IFR1, P28, 28, 1)
	FIELD(GPIO_IFR1, P29, 29, 1)
	FIELD(GPIO_IFR1, P30, 30, 1)
	FIELD(GPIO_IFR1, P31, 31, 1)
REG32(GPIO_IFR2, 0x4d0)
	FIELD(GPIO_IFR2, P0, 0, 1)
	FIELD(GPIO_IFR2, P1, 1, 1)
	FIELD(GPIO_IFR2, P2, 2, 1)
	FIELD(GPIO_IFR2, P3, 3, 1)
	FIELD(GPIO_IFR2, P4, 4, 1)
	FIELD(GPIO_IFR2, P5, 5, 1)
	FIELD(GPIO_IFR2, P6, 6, 1)
	FIELD(GPIO_IFR2, P7, 7, 1)
	FIELD(GPIO_IFR2, P8, 8, 1)
	FIELD(GPIO_IFR2, P9, 9, 1)
	FIELD(GPIO_IFR2, P10, 10, 1)
	FIELD(GPIO_IFR2, P11, 11, 1)
	FIELD(GPIO_IFR2, P12, 12, 1)
	FIELD(GPIO_IFR2, P13, 13, 1)
	FIELD(GPIO_IFR2, P14, 14, 1)
	FIELD(GPIO_IFR2, P15, 15, 1)
	FIELD(GPIO_IFR2, P16, 16, 1)
	FIELD(GPIO_IFR2, P17, 17, 1)
	FIELD(GPIO_IFR2, P18, 18, 1)
	FIELD(GPIO_IFR2, P19, 19, 1)
	FIELD(GPIO_IFR2, P20, 20, 1)
	FIELD(GPIO_IFR2, P21, 21, 1)
	FIELD(GPIO_IFR2, P22, 22, 1)
	FIELD(GPIO_IFR2, P23, 23, 1)
	FIELD(GPIO_IFR2, P24, 24, 1)
	FIELD(GPIO_IFR2, P25, 25, 1)
	FIELD(GPIO_IFR2, P26, 26, 1)
	FIELD(GPIO_IFR2, P27, 27, 1)
	FIELD(GPIO_IFR2, P28, 28, 1)
	FIELD(GPIO_IFR2, P29, 29, 1)
	FIELD(GPIO_IFR2, P30, 30, 1)
	FIELD(GPIO_IFR2, P31, 31, 1)
REG32(GPIO_IMR0C0, 0xa8)
	FIELD(GPIO_IMR0C0, P0, 0, 1)
	FIELD(GPIO_IMR0C0, P1, 1, 1)
	FIELD(GPIO_IMR0C0, P2, 2, 1)
	FIELD(GPIO_IMR0C0, P3, 3, 1)
	FIELD(GPIO_IMR0C0, P4, 4, 1)
	FIELD(GPIO_IMR0C0, P5, 5, 1)
	FIELD(GPIO_IMR0C0, P6, 6, 1)
	FIELD(GPIO_IMR0C0, P7, 7, 1)
	FIELD(GPIO_IMR0C0, P8, 8, 1)
	FIELD(GPIO_IMR0C0, P9, 9, 1)
	FIELD(GPIO_IMR0C0, P10, 10, 1)
	FIELD(GPIO_IMR0C0, P11, 11, 1)
	FIELD(GPIO_IMR0C0, P12, 12, 1)
	FIELD(GPIO_IMR0C0, P13, 13, 1)
	FIELD(GPIO_IMR0C0, P14, 14, 1)
	FIELD(GPIO_IMR0C0, P15, 15, 1)
	FIELD(GPIO_IMR0C0, P16, 16, 1)
	FIELD(GPIO_IMR0C0, P17, 17, 1)
	FIELD(GPIO_IMR0C0, P18, 18, 1)
	FIELD(GPIO_IMR0C0, P19, 19, 1)
	FIELD(GPIO_IMR0C0, P20, 20, 1)
	FIELD(GPIO_IMR0C0, P21, 21, 1)
	FIELD(GPIO_IMR0C0, P22, 22, 1)
	FIELD(GPIO_IMR0C0, P23, 23, 1)
	FIELD(GPIO_IMR0C0, P24, 24, 1)
	FIELD(GPIO_IMR0C0, P25, 25, 1)
	FIELD(GPIO_IMR0C0, P26, 26, 1)
	FIELD(GPIO_IMR0C0, P27, 27, 1)
	FIELD(GPIO_IMR0C0, P28, 28, 1)
	FIELD(GPIO_IMR0C0, P29, 29, 1)
	FIELD(GPIO_IMR0C0, P30, 30, 1)
	FIELD(GPIO_IMR0C0, P31, 31, 1)
REG32(GPIO_IMR0C1, 0x2a8)
	FIELD(GPIO_IMR0C1, P0, 0, 1)
	FIELD(GPIO_IMR0C1, P1, 1, 1)
	FIELD(GPIO_IMR0C1, P2, 2, 1)
	FIELD(GPIO_IMR0C1, P3, 3, 1)
	FIELD(GPIO_IMR0C1, P4, 4, 1)
	FIELD(GPIO_IMR0C1, P5, 5, 1)
	FIELD(GPIO_IMR0C1, P6, 6, 1)
	FIELD(GPIO_IMR0C1, P7, 7, 1)
	FIELD(GPIO_IMR0C1, P8, 8, 1)
	FIELD(GPIO_IMR0C1, P9, 9, 1)
	FIELD(GPIO_IMR0C1, P10, 10, 1)
	FIELD(GPIO_IMR0C1, P11, 11, 1)
	FIELD(GPIO_IMR0C1, P12, 12, 1)
	FIELD(GPIO_IMR0C1, P13, 13, 1)
	FIELD(GPIO_IMR0C1, P14, 14, 1)
	FIELD(GPIO_IMR0C1, P15, 15, 1)
	FIELD(GPIO_IMR0C1, P16, 16, 1)
	FIELD(GPIO_IMR0C1, P17, 17, 1)
	FIELD(GPIO_IMR0C1, P18, 18, 1)
	FIELD(GPIO_IMR0C1, P19, 19, 1)
	FIELD(GPIO_IMR0C1, P20, 20, 1)
	FIELD(GPIO_IMR0C1, P21, 21, 1)
	FIELD(GPIO_IMR0C1, P22, 22, 1)
	FIELD(GPIO_IMR0C1, P23, 23, 1)
	FIELD(GPIO_IMR0C1, P24, 24, 1)
	FIELD(GPIO_IMR0C1, P25, 25, 1)
	FIELD(GPIO_IMR0C1, P26, 26, 1)
	FIELD(GPIO_IMR0C1, P27, 27, 1)
	FIELD(GPIO_IMR0C1, P28, 28, 1)
	FIELD(GPIO_IMR0C1, P29, 29, 1)
	FIELD(GPIO_IMR0C1, P30, 30, 1)
	FIELD(GPIO_IMR0C1, P31, 31, 1)
REG32(GPIO_IMR0C2, 0x4a8)
	FIELD(GPIO_IMR0C2, P0, 0, 1)
	FIELD(GPIO_IMR0C2, P1, 1, 1)
	FIELD(GPIO_IMR0C2, P2, 2, 1)
	FIELD(GPIO_IMR0C2, P3, 3, 1)
	FIELD(GPIO_IMR0C2, P4, 4, 1)
	FIELD(GPIO_IMR0C2, P5, 5, 1)
	FIELD(GPIO_IMR0C2, P6, 6, 1)
	FIELD(GPIO_IMR0C2, P7, 7, 1)
	FIELD(GPIO_IMR0C2, P8, 8, 1)
	FIELD(GPIO_IMR0C2, P9, 9, 1)
	FIELD(GPIO_IMR0C2, P10, 10, 1)
	FIELD(GPIO_IMR0C2, P11, 11, 1)
	FIELD(GPIO_IMR0C2, P12, 12, 1)
	FIELD(GPIO_IMR0C2, P13, 13, 1)
	FIELD(GPIO_IMR0C2, P14, 14, 1)
	FIELD(GPIO_IMR0C2, P15, 15, 1)
	FIELD(GPIO_IMR0C2, P16, 16, 1)
	FIELD(GPIO_IMR0C2, P17, 17, 1)
	FIELD(GPIO_IMR0C2, P18, 18, 1)
	FIELD(GPIO_IMR0C2, P19, 19, 1)
	FIELD(GPIO_IMR0C2, P20, 20, 1)
	FIELD(GPIO_IMR0C2, P21, 21, 1)
	FIELD(GPIO_IMR0C2, P22, 22, 1)
	FIELD(GPIO_IMR0C2, P23, 23, 1)
	FIELD(GPIO_IMR0C2, P24, 24, 1)
	FIELD(GPIO_IMR0C2, P25, 25, 1)
	FIELD(GPIO_IMR0C2, P26, 26, 1)
	FIELD(GPIO_IMR0C2, P27, 27, 1)
	FIELD(GPIO_IMR0C2, P28, 28, 1)
	FIELD(GPIO_IMR0C2, P29, 29, 1)
	FIELD(GPIO_IMR0C2, P30, 30, 1)
	FIELD(GPIO_IMR0C2, P31, 31, 1)
REG32(GPIO_IMR0S0, 0xa4)
	FIELD(GPIO_IMR0S0, P0, 0, 1)
	FIELD(GPIO_IMR0S0, P1, 1, 1)
	FIELD(GPIO_IMR0S0, P2, 2, 1)
	FIELD(GPIO_IMR0S0, P3, 3, 1)
	FIELD(GPIO_IMR0S0, P4, 4, 1)
	FIELD(GPIO_IMR0S0, P5, 5, 1)
	FIELD(GPIO_IMR0S0, P6, 6, 1)
	FIELD(GPIO_IMR0S0, P7, 7, 1)
	FIELD(GPIO_IMR0S0, P8, 8, 1)
	FIELD(GPIO_IMR0S0, P9, 9, 1)
	FIELD(GPIO_IMR0S0, P10, 10, 1)
	FIELD(GPIO_IMR0S0, P11, 11, 1)
	FIELD(GPIO_IMR0S0, P12, 12, 1)
	FIELD(GPIO_IMR0S0, P13, 13, 1)
	FIELD(GPIO_IMR0S0, P14, 14, 1)
	FIELD(GPIO_IMR0S0, P15, 15, 1)
	FIELD(GPIO_IMR0S0, P16, 16, 1)
	FIELD(GPIO_IMR0S0, P17, 17, 1)
	FIELD(GPIO_IMR0S0, P18, 18, 1)
	FIELD(GPIO_IMR0S0, P19, 19, 1)
	FIELD(GPIO_IMR0S0, P20, 20, 1)
	FIELD(GPIO_IMR0S0, P21, 21, 1)
	FIELD(GPIO_IMR0S0, P22, 22, 1)
	FIELD(GPIO_IMR0S0, P23, 23, 1)
	FIELD(GPIO_IMR0S0, P24, 24, 1)
	FIELD(GPIO_IMR0S0, P25, 25, 1)
	FIELD(GPIO_IMR0S0, P26, 26, 1)
	FIELD(GPIO_IMR0S0, P27, 27, 1)
	FIELD(GPIO_IMR0S0, P28, 28, 1)
	FIELD(GPIO_IMR0S0, P29, 29, 1)
	FIELD(GPIO_IMR0S0, P30, 30, 1)
	FIELD(GPIO_IMR0S0, P31, 31, 1)
REG32(GPIO_IMR0S1, 0x2a4)
	FIELD(GPIO_IMR0S1, P0, 0, 1)
	FIELD(GPIO_IMR0S1, P1, 1, 1)
	FIELD(GPIO_IMR0S1, P2, 2, 1)
	FIELD(GPIO_IMR0S1, P3, 3, 1)
	FIELD(GPIO_IMR0S1, P4, 4, 1)
	FIELD(GPIO_IMR0S1, P5, 5, 1)
	FIELD(GPIO_IMR0S1, P6, 6, 1)
	FIELD(GPIO_IMR0S1, P7, 7, 1)
	FIELD(GPIO_IMR0S1, P8, 8, 1)
	FIELD(GPIO_IMR0S1, P9, 9, 1)
	FIELD(GPIO_IMR0S1, P10, 10, 1)
	FIELD(GPIO_IMR0S1, P11, 11, 1)
	FIELD(GPIO_IMR0S1, P12, 12, 1)
	FIELD(GPIO_IMR0S1, P13, 13, 1)
	FIELD(GPIO_IMR0S1, P14, 14, 1)
	FIELD(GPIO_IMR0S1, P15, 15, 1)
	FIELD(GPIO_IMR0S1, P16, 16, 1)
	FIELD(GPIO_IMR0S1, P17, 17, 1)
	FIELD(GPIO_IMR0S1, P18, 18, 1)
	FIELD(GPIO_IMR0S1, P19, 19, 1)
	FIELD(GPIO_IMR0S1, P20, 20, 1)
	FIELD(GPIO_IMR0S1, P21, 21, 1)
	FIELD(GPIO_IMR0S1, P22, 22, 1)
	FIELD(GPIO_IMR0S1, P23, 23, 1)
	FIELD(GPIO_IMR0S1, P24, 24, 1)
	FIELD(GPIO_IMR0S1, P25, 25, 1)
	FIELD(GPIO_IMR0S1, P26, 26, 1)
	FIELD(GPIO_IMR0S1, P27, 27, 1)
	FIELD(GPIO_IMR0S1, P28, 28, 1)
	FIELD(GPIO_IMR0S1, P29, 29, 1)
	FIELD(GPIO_IMR0S1, P30, 30, 1)
	FIELD(GPIO_IMR0S1, P31, 31, 1)
REG32(GPIO_IMR0S2, 0x4a4)
	FIELD(GPIO_IMR0S2, P0, 0, 1)
	FIELD(GPIO_IMR0S2, P1, 1, 1)
	FIELD(GPIO_IMR0S2, P2, 2, 1)
	FIELD(GPIO_IMR0S2, P3, 3, 1)
	FIELD(GPIO_IMR0S2, P4, 4, 1)
	FIELD(GPIO_IMR0S2, P5, 5, 1)
	FIELD(GPIO_IMR0S2, P6, 6, 1)
	FIELD(GPIO_IMR0S2, P7, 7, 1)
	FIELD(GPIO_IMR0S2, P8, 8, 1)
	FIELD(GPIO_IMR0S2, P9, 9, 1)
	FIELD(GPIO_IMR0S2, P10, 10, 1)
	FIELD(GPIO_IMR0S2, P11, 11, 1)
	FIELD(GPIO_IMR0S2, P12, 12, 1)
	FIELD(GPIO_IMR0S2, P13, 13, 1)
	FIELD(GPIO_IMR0S2, P14, 14, 1)
	FIELD(GPIO_IMR0S2, P15, 15, 1)
	FIELD(GPIO_IMR0S2, P16, 16, 1)
	FIELD(GPIO_IMR0S2, P17, 17, 1)
	FIELD(GPIO_IMR0S2, P18, 18, 1)
	FIELD(GPIO_IMR0S2, P19, 19, 1)
	FIELD(GPIO_IMR0S2, P20, 20, 1)
	FIELD(GPIO_IMR0S2, P21, 21, 1)
	FIELD(GPIO_IMR0S2, P22, 22, 1)
	FIELD(GPIO_IMR0S2, P23, 23, 1)
	FIELD(GPIO_IMR0S2, P24, 24, 1)
	FIELD(GPIO_IMR0S2, P25, 25, 1)
	FIELD(GPIO_IMR0S2, P26, 26, 1)
	FIELD(GPIO_IMR0S2, P27, 27, 1)
	FIELD(GPIO_IMR0S2, P28, 28, 1)
	FIELD(GPIO_IMR0S2, P29, 29, 1)
	FIELD(GPIO_IMR0S2, P30, 30, 1)
	FIELD(GPIO_IMR0S2, P31, 31, 1)
REG32(GPIO_IMR0T0, 0xac)
	FIELD(GPIO_IMR0T0, P0, 0, 1)
	FIELD(GPIO_IMR0T0, P1, 1, 1)
	FIELD(GPIO_IMR0T0, P2, 2, 1)
	FIELD(GPIO_IMR0T0, P3, 3, 1)
	FIELD(GPIO_IMR0T0, P4, 4, 1)
	FIELD(GPIO_IMR0T0, P5, 5, 1)
	FIELD(GPIO_IMR0T0, P6, 6, 1)
	FIELD(GPIO_IMR0T0, P7, 7, 1)
	FIELD(GPIO_IMR0T0, P8, 8, 1)
	FIELD(GPIO_IMR0T0, P9, 9, 1)
	FIELD(GPIO_IMR0T0, P10, 10, 1)
	FIELD(GPIO_IMR0T0, P11, 11, 1)
	FIELD(GPIO_IMR0T0, P12, 12, 1)
	FIELD(GPIO_IMR0T0, P13, 13, 1)
	FIELD(GPIO_IMR0T0, P14, 14, 1)
	FIELD(GPIO_IMR0T0, P15, 15, 1)
	FIELD(GPIO_IMR0T0, P16, 16, 1)
	FIELD(GPIO_IMR0T0, P17, 17, 1)
	FIELD(GPIO_IMR0T0, P18, 18, 1)
	FIELD(GPIO_IMR0T0, P19, 19, 1)
	FIELD(GPIO_IMR0T0, P20, 20, 1)
	FIELD(GPIO_IMR0T0, P21, 21, 1)
	FIELD(GPIO_IMR0T0, P22, 22, 1)
	FIELD(GPIO_IMR0T0, P23, 23, 1)
	FIELD(GPIO_IMR0T0, P24, 24, 1)
	FIELD(GPIO_IMR0T0, P25, 25, 1)
	FIELD(GPIO_IMR0T0, P26, 26, 1)
	FIELD(GPIO_IMR0T0, P27, 27, 1)
	FIELD(GPIO_IMR0T0, P28, 28, 1)
	FIELD(GPIO_IMR0T0, P29, 29, 1)
	FIELD(GPIO_IMR0T0, P30, 30, 1)
	FIELD(GPIO_IMR0T0, P31, 31, 1)
REG32(GPIO_IMR0T1, 0x2ac)
	FIELD(GPIO_IMR0T1, P0, 0, 1)
	FIELD(GPIO_IMR0T1, P1, 1, 1)
	FIELD(GPIO_IMR0T1, P2, 2, 1)
	FIELD(GPIO_IMR0T1, P3, 3, 1)
	FIELD(GPIO_IMR0T1, P4, 4, 1)
	FIELD(GPIO_IMR0T1, P5, 5, 1)
	FIELD(GPIO_IMR0T1, P6, 6, 1)
	FIELD(GPIO_IMR0T1, P7, 7, 1)
	FIELD(GPIO_IMR0T1, P8, 8, 1)
	FIELD(GPIO_IMR0T1, P9, 9, 1)
	FIELD(GPIO_IMR0T1, P10, 10, 1)
	FIELD(GPIO_IMR0T1, P11, 11, 1)
	FIELD(GPIO_IMR0T1, P12, 12, 1)
	FIELD(GPIO_IMR0T1, P13, 13, 1)
	FIELD(GPIO_IMR0T1, P14, 14, 1)
	FIELD(GPIO_IMR0T1, P15, 15, 1)
	FIELD(GPIO_IMR0T1, P16, 16, 1)
	FIELD(GPIO_IMR0T1, P17, 17, 1)
	FIELD(GPIO_IMR0T1, P18, 18, 1)
	FIELD(GPIO_IMR0T1, P19, 19, 1)
	FIELD(GPIO_IMR0T1, P20, 20, 1)
	FIELD(GPIO_IMR0T1, P21, 21, 1)
	FIELD(GPIO_IMR0T1, P22, 22, 1)
	FIELD(GPIO_IMR0T1, P23, 23, 1)
	FIELD(GPIO_IMR0T1, P24, 24, 1)
	FIELD(GPIO_IMR0T1, P25, 25, 1)
	FIELD(GPIO_IMR0T1, P26, 26, 1)
	FIELD(GPIO_IMR0T1, P27, 27, 1)
	FIELD(GPIO_IMR0T1, P28, 28, 1)
	FIELD(GPIO_IMR0T1, P29, 29, 1)
	FIELD(GPIO_IMR0T1, P30, 30, 1)
	FIELD(GPIO_IMR0T1, P31, 31, 1)
REG32(GPIO_IMR0T2, 0x4ac)
	FIELD(GPIO_IMR0T2, P0, 0, 1)
	FIELD(GPIO_IMR0T2, P1, 1, 1)
	FIELD(GPIO_IMR0T2, P2, 2, 1)
	FIELD(GPIO_IMR0T2, P3, 3, 1)
	FIELD(GPIO_IMR0T2, P4, 4, 1)
	FIELD(GPIO_IMR0T2, P5, 5, 1)
	FIELD(GPIO_IMR0T2, P6, 6, 1)
	FIELD(GPIO_IMR0T2, P7, 7, 1)
	FIELD(GPIO_IMR0T2, P8, 8, 1)
	FIELD(GPIO_IMR0T2, P9, 9, 1)
	FIELD(GPIO_IMR0T2, P10, 10, 1)
	FIELD(GPIO_IMR0T2, P11, 11, 1)
	FIELD(GPIO_IMR0T2, P12, 12, 1)
	FIELD(GPIO_IMR0T2, P13, 13, 1)
	FIELD(GPIO_IMR0T2, P14, 14, 1)
	FIELD(GPIO_IMR0T2, P15, 15, 1)
	FIELD(GPIO_IMR0T2, P16, 16, 1)
	FIELD(GPIO_IMR0T2, P17, 17, 1)
	FIELD(GPIO_IMR0T2, P18, 18, 1)
	FIELD(GPIO_IMR0T2, P19, 19, 1)
	FIELD(GPIO_IMR0T2, P20, 20, 1)
	FIELD(GPIO_IMR0T2, P21, 21, 1)
	FIELD(GPIO_IMR0T2, P22, 22, 1)
	FIELD(GPIO_IMR0T2, P23, 23, 1)
	FIELD(GPIO_IMR0T2, P24, 24, 1)
	FIELD(GPIO_IMR0T2, P25, 25, 1)
	FIELD(GPIO_IMR0T2, P26, 26, 1)
	FIELD(GPIO_IMR0T2, P27, 27, 1)
	FIELD(GPIO_IMR0T2, P28, 28, 1)
	FIELD(GPIO_IMR0T2, P29, 29, 1)
	FIELD(GPIO_IMR0T2, P30, 30, 1)
	FIELD(GPIO_IMR0T2, P31, 31, 1)
REG32(GPIO_IMR1C0, 0xb8)
	FIELD(GPIO_IMR1C0, P0, 0, 1)
	FIELD(GPIO_IMR1C0, P1, 1, 1)
	FIELD(GPIO_IMR1C0, P2, 2, 1)
	FIELD(GPIO_IMR1C0, P3, 3, 1)
	FIELD(GPIO_IMR1C0, P4, 4, 1)
	FIELD(GPIO_IMR1C0, P5, 5, 1)
	FIELD(GPIO_IMR1C0, P6, 6, 1)
	FIELD(GPIO_IMR1C0, P7, 7, 1)
	FIELD(GPIO_IMR1C0, P8, 8, 1)
	FIELD(GPIO_IMR1C0, P9, 9, 1)
	FIELD(GPIO_IMR1C0, P10, 10, 1)
	FIELD(GPIO_IMR1C0, P11, 11, 1)
	FIELD(GPIO_IMR1C0, P12, 12, 1)
	FIELD(GPIO_IMR1C0, P13, 13, 1)
	FIELD(GPIO_IMR1C0, P14, 14, 1)
	FIELD(GPIO_IMR1C0, P15, 15, 1)
	FIELD(GPIO_IMR1C0, P16, 16, 1)
	FIELD(GPIO_IMR1C0, P17, 17, 1)
	FIELD(GPIO_IMR1C0, P18, 18, 1)
	FIELD(GPIO_IMR1C0, P19, 19, 1)
	FIELD(GPIO_IMR1C0, P20, 20, 1)
	FIELD(GPIO_IMR1C0, P21, 21, 1)
	FIELD(GPIO_IMR1C0, P22, 22, 1)
	FIELD(GPIO_IMR1C0, P23, 23, 1)
	FIELD(GPIO_IMR1C0, P24, 24, 1)
	FIELD(GPIO_IMR1C0, P25, 25, 1)
	FIELD(GPIO_IMR1C0, P26, 26, 1)
	FIELD(GPIO_IMR1C0, P27, 27, 1)
	FIELD(GPIO_IMR1C0, P28, 28, 1)
	FIELD(GPIO_IMR1C0, P29, 29, 1)
	FIELD(GPIO_IMR1C0, P30, 30, 1)
	FIELD(GPIO_IMR1C0, P31, 31, 1)
REG32(GPIO_IMR1C1, 0x2b8)
	FIELD(GPIO_IMR1C1, P0, 0, 1)
	FIELD(GPIO_IMR1C1, P1, 1, 1)
	FIELD(GPIO_IMR1C1, P2, 2, 1)
	FIELD(GPIO_IMR1C1, P3, 3, 1)
	FIELD(GPIO_IMR1C1, P4, 4, 1)
	FIELD(GPIO_IMR1C1, P5, 5, 1)
	FIELD(GPIO_IMR1C1, P6, 6, 1)
	FIELD(GPIO_IMR1C1, P7, 7, 1)
	FIELD(GPIO_IMR1C1, P8, 8, 1)
	FIELD(GPIO_IMR1C1, P9, 9, 1)
	FIELD(GPIO_IMR1C1, P10, 10, 1)
	FIELD(GPIO_IMR1C1, P11, 11, 1)
	FIELD(GPIO_IMR1C1, P12, 12, 1)
	FIELD(GPIO_IMR1C1, P13, 13, 1)
	FIELD(GPIO_IMR1C1, P14, 14, 1)
	FIELD(GPIO_IMR1C1, P15, 15, 1)
	FIELD(GPIO_IMR1C1, P16, 16, 1)
	FIELD(GPIO_IMR1C1, P17, 17, 1)
	FIELD(GPIO_IMR1C1, P18, 18, 1)
	FIELD(GPIO_IMR1C1, P19, 19, 1)
	FIELD(GPIO_IMR1C1, P20, 20, 1)
	FIELD(GPIO_IMR1C1, P21, 21, 1)
	FIELD(GPIO_IMR1C1, P22, 22, 1)
	FIELD(GPIO_IMR1C1, P23, 23, 1)
	FIELD(GPIO_IMR1C1, P24, 24, 1)
	FIELD(GPIO_IMR1C1, P25, 25, 1)
	FIELD(GPIO_IMR1C1, P26, 26, 1)
	FIELD(GPIO_IMR1C1, P27, 27, 1)
	FIELD(GPIO_IMR1C1, P28, 28, 1)
	FIELD(GPIO_IMR1C1, P29, 29, 1)
	FIELD(GPIO_IMR1C1, P30, 30, 1)
	FIELD(GPIO_IMR1C1, P31, 31, 1)
REG32(GPIO_IMR1C2, 0x4b8)
	FIELD(GPIO_IMR1C2, P0, 0, 1)
	FIELD(GPIO_IMR1C2, P1, 1, 1)
	FIELD(GPIO_IMR1C2, P2, 2, 1)
	FIELD(GPIO_IMR1C2, P3, 3, 1)
	FIELD(GPIO_IMR1C2, P4, 4, 1)
	FIELD(GPIO_IMR1C2, P5, 5, 1)
	FIELD(GPIO_IMR1C2, P6, 6, 1)
	FIELD(GPIO_IMR1C2, P7, 7, 1)
	FIELD(GPIO_IMR1C2, P8, 8, 1)
	FIELD(GPIO_IMR1C2, P9, 9, 1)
	FIELD(GPIO_IMR1C2, P10, 10, 1)
	FIELD(GPIO_IMR1C2, P11, 11, 1)
	FIELD(GPIO_IMR1C2, P12, 12, 1)
	FIELD(GPIO_IMR1C2, P13, 13, 1)
	FIELD(GPIO_IMR1C2, P14, 14, 1)
	FIELD(GPIO_IMR1C2, P15, 15, 1)
	FIELD(GPIO_IMR1C2, P16, 16, 1)
	FIELD(GPIO_IMR1C2, P17, 17, 1)
	FIELD(GPIO_IMR1C2, P18, 18, 1)
	FIELD(GPIO_IMR1C2, P19, 19, 1)
	FIELD(GPIO_IMR1C2, P20, 20, 1)
	FIELD(GPIO_IMR1C2, P21, 21, 1)
	FIELD(GPIO_IMR1C2, P22, 22, 1)
	FIELD(GPIO_IMR1C2, P23, 23, 1)
	FIELD(GPIO_IMR1C2, P24, 24, 1)
	FIELD(GPIO_IMR1C2, P25, 25, 1)
	FIELD(GPIO_IMR1C2, P26, 26, 1)
	FIELD(GPIO_IMR1C2, P27, 27, 1)
	FIELD(GPIO_IMR1C2, P28, 28, 1)
	FIELD(GPIO_IMR1C2, P29, 29, 1)
	FIELD(GPIO_IMR1C2, P30, 30, 1)
	FIELD(GPIO_IMR1C2, P31, 31, 1)
REG32(GPIO_IMR1S0, 0xb4)
	FIELD(GPIO_IMR1S0, P0, 0, 1)
	FIELD(GPIO_IMR1S0, P1, 1, 1)
	FIELD(GPIO_IMR1S0, P2, 2, 1)
	FIELD(GPIO_IMR1S0, P3, 3, 1)
	FIELD(GPIO_IMR1S0, P4, 4, 1)
	FIELD(GPIO_IMR1S0, P5, 5, 1)
	FIELD(GPIO_IMR1S0, P6, 6, 1)
	FIELD(GPIO_IMR1S0, P7, 7, 1)
	FIELD(GPIO_IMR1S0, P8, 8, 1)
	FIELD(GPIO_IMR1S0, P9, 9, 1)
	FIELD(GPIO_IMR1S0, P10, 10, 1)
	FIELD(GPIO_IMR1S0, P11, 11, 1)
	FIELD(GPIO_IMR1S0, P12, 12, 1)
	FIELD(GPIO_IMR1S0, P13, 13, 1)
	FIELD(GPIO_IMR1S0, P14, 14, 1)
	FIELD(GPIO_IMR1S0, P15, 15, 1)
	FIELD(GPIO_IMR1S0, P16, 16, 1)
	FIELD(GPIO_IMR1S0, P17, 17, 1)
	FIELD(GPIO_IMR1S0, P18, 18, 1)
	FIELD(GPIO_IMR1S0, P19, 19, 1)
	FIELD(GPIO_IMR1S0, P20, 20, 1)
	FIELD(GPIO_IMR1S0, P21, 21, 1)
	FIELD(GPIO_IMR1S0, P22, 22, 1)
	FIELD(GPIO_IMR1S0, P23, 23, 1)
	FIELD(GPIO_IMR1S0, P24, 24, 1)
	FIELD(GPIO_IMR1S0, P25, 25, 1)
	FIELD(GPIO_IMR1S0, P26, 26, 1)
	FIELD(GPIO_IMR1S0, P27, 27, 1)
	FIELD(GPIO_IMR1S0, P28, 28, 1)
	FIELD(GPIO_IMR1S0, P29, 29, 1)
	FIELD(GPIO_IMR1S0, P30, 30, 1)
	FIELD(GPIO_IMR1S0, P31, 31, 1)
REG32(GPIO_IMR1S1, 0x2b4)
	FIELD(GPIO_IMR1S1, P0, 0, 1)
	FIELD(GPIO_IMR1S1, P1, 1, 1)
	FIELD(GPIO_IMR1S1, P2, 2, 1)
	FIELD(GPIO_IMR1S1, P3, 3, 1)
	FIELD(GPIO_IMR1S1, P4, 4, 1)
	FIELD(GPIO_IMR1S1, P5, 5, 1)
	FIELD(GPIO_IMR1S1, P6, 6, 1)
	FIELD(GPIO_IMR1S1, P7, 7, 1)
	FIELD(GPIO_IMR1S1, P8, 8, 1)
	FIELD(GPIO_IMR1S1, P9, 9, 1)
	FIELD(GPIO_IMR1S1, P10, 10, 1)
	FIELD(GPIO_IMR1S1, P11, 11, 1)
	FIELD(GPIO_IMR1S1, P12, 12, 1)
	FIELD(GPIO_IMR1S1, P13, 13, 1)
	FIELD(GPIO_IMR1S1, P14, 14, 1)
	FIELD(GPIO_IMR1S1, P15, 15, 1)
	FIELD(GPIO_IMR1S1, P16, 16, 1)
	FIELD(GPIO_IMR1S1, P17, 17, 1)
	FIELD(GPIO_IMR1S1, P18, 18, 1)
	FIELD(GPIO_IMR1S1, P19, 19, 1)
	FIELD(GPIO_IMR1S1, P20, 20, 1)
	FIELD(GPIO_IMR1S1, P21, 21, 1)
	FIELD(GPIO_IMR1S1, P22, 22, 1)
	FIELD(GPIO_IMR1S1, P23, 23, 1)
	FIELD(GPIO_IMR1S1, P24, 24, 1)
	FIELD(GPIO_IMR1S1, P25, 25, 1)
	FIELD(GPIO_IMR1S1, P26, 26, 1)
	FIELD(GPIO_IMR1S1, P27, 27, 1)
	FIELD(GPIO_IMR1S1, P28, 28, 1)
	FIELD(GPIO_IMR1S1, P29, 29, 1)
	FIELD(GPIO_IMR1S1, P30, 30, 1)
	FIELD(GPIO_IMR1S1, P31, 31, 1)
REG32(GPIO_IMR1S2, 0x4b4)
	FIELD(GPIO_IMR1S2, P0, 0, 1)
	FIELD(GPIO_IMR1S2, P1, 1, 1)
	FIELD(GPIO_IMR1S2, P2, 2, 1)
	FIELD(GPIO_IMR1S2, P3, 3, 1)
	FIELD(GPIO_IMR1S2, P4, 4, 1)
	FIELD(GPIO_IMR1S2, P5, 5, 1)
	FIELD(GPIO_IMR1S2, P6, 6, 1)
	FIELD(GPIO_IMR1S2, P7, 7, 1)
	FIELD(GPIO_IMR1S2, P8, 8, 1)
	FIELD(GPIO_IMR1S2, P9, 9, 1)
	FIELD(GPIO_IMR1S2, P10, 10, 1)
	FIELD(GPIO_IMR1S2, P11, 11, 1)
	FIELD(GPIO_IMR1S2, P12, 12, 1)
	FIELD(GPIO_IMR1S2, P13, 13, 1)
	FIELD(GPIO_IMR1S2, P14, 14, 1)
	FIELD(GPIO_IMR1S2, P15, 15, 1)
	FIELD(GPIO_IMR1S2, P16, 16, 1)
	FIELD(GPIO_IMR1S2, P17, 17, 1)
	FIELD(GPIO_IMR1S2, P18, 18, 1)
	FIELD(GPIO_IMR1S2, P19, 19, 1)
	FIELD(GPIO_IMR1S2, P20, 20, 1)
	FIELD(GPIO_IMR1S2, P21, 21, 1)
	FIELD(GPIO_IMR1S2, P22, 22, 1)
	FIELD(GPIO_IMR1S2, P23, 23, 1)
	FIELD(GPIO_IMR1S2, P24, 24, 1)
	FIELD(GPIO_IMR1S2, P25, 25, 1)
	FIELD(GPIO_IMR1S2, P26, 26, 1)
	FIELD(GPIO_IMR1S2, P27, 27, 1)
	FIELD(GPIO_IMR1S2, P28, 28, 1)
	FIELD(GPIO_IMR1S2, P29, 29, 1)
	FIELD(GPIO_IMR1S2, P30, 30, 1)
	FIELD(GPIO_IMR1S2, P31, 31, 1)
REG32(GPIO_IMR1T0, 0xbc)
	FIELD(GPIO_IMR1T0, P0, 0, 1)
	FIELD(GPIO_IMR1T0, P1, 1, 1)
	FIELD(GPIO_IMR1T0, P2, 2, 1)
	FIELD(GPIO_IMR1T0, P3, 3, 1)
	FIELD(GPIO_IMR1T0, P4, 4, 1)
	FIELD(GPIO_IMR1T0, P5, 5, 1)
	FIELD(GPIO_IMR1T0, P6, 6, 1)
	FIELD(GPIO_IMR1T0, P7, 7, 1)
	FIELD(GPIO_IMR1T0, P8, 8, 1)
	FIELD(GPIO_IMR1T0, P9, 9, 1)
	FIELD(GPIO_IMR1T0, P10, 10, 1)
	FIELD(GPIO_IMR1T0, P11, 11, 1)
	FIELD(GPIO_IMR1T0, P12, 12, 1)
	FIELD(GPIO_IMR1T0, P13, 13, 1)
	FIELD(GPIO_IMR1T0, P14, 14, 1)
	FIELD(GPIO_IMR1T0, P15, 15, 1)
	FIELD(GPIO_IMR1T0, P16, 16, 1)
	FIELD(GPIO_IMR1T0, P17, 17, 1)
	FIELD(GPIO_IMR1T0, P18, 18, 1)
	FIELD(GPIO_IMR1T0, P19, 19, 1)
	FIELD(GPIO_IMR1T0, P20, 20, 1)
	FIELD(GPIO_IMR1T0, P21, 21, 1)
	FIELD(GPIO_IMR1T0, P22, 22, 1)
	FIELD(GPIO_IMR1T0, P23, 23, 1)
	FIELD(GPIO_IMR1T0, P24, 24, 1)
	FIELD(GPIO_IMR1T0, P25, 25, 1)
	FIELD(GPIO_IMR1T0, P26, 26, 1)
	FIELD(GPIO_IMR1T0, P27, 27, 1)
	FIELD(GPIO_IMR1T0, P28, 28, 1)
	FIELD(GPIO_IMR1T0, P29, 29, 1)
	FIELD(GPIO_IMR1T0, P30, 30, 1)
	FIELD(GPIO_IMR1T0, P31, 31, 1)
REG32(GPIO_IMR1T1, 0x2bc)
	FIELD(GPIO_IMR1T1, P0, 0, 1)
	FIELD(GPIO_IMR1T1, P1, 1, 1)
	FIELD(GPIO_IMR1T1, P2, 2, 1)
	FIELD(GPIO_IMR1T1, P3, 3, 1)
	FIELD(GPIO_IMR1T1, P4, 4, 1)
	FIELD(GPIO_IMR1T1, P5, 5, 1)
	FIELD(GPIO_IMR1T1, P6, 6, 1)
	FIELD(GPIO_IMR1T1, P7, 7, 1)
	FIELD(GPIO_IMR1T1, P8, 8, 1)
	FIELD(GPIO_IMR1T1, P9, 9, 1)
	FIELD(GPIO_IMR1T1, P10, 10, 1)
	FIELD(GPIO_IMR1T1, P11, 11, 1)
	FIELD(GPIO_IMR1T1, P12, 12, 1)
	FIELD(GPIO_IMR1T1, P13, 13, 1)
	FIELD(GPIO_IMR1T1, P14, 14, 1)
	FIELD(GPIO_IMR1T1, P15, 15, 1)
	FIELD(GPIO_IMR1T1, P16, 16, 1)
	FIELD(GPIO_IMR1T1, P17, 17, 1)
	FIELD(GPIO_IMR1T1, P18, 18, 1)
	FIELD(GPIO_IMR1T1, P19, 19, 1)
	FIELD(GPIO_IMR1T1, P20, 20, 1)
	FIELD(GPIO_IMR1T1, P21, 21, 1)
	FIELD(GPIO_IMR1T1, P22, 22, 1)
	FIELD(GPIO_IMR1T1, P23, 23, 1)
	FIELD(GPIO_IMR1T1, P24, 24, 1)
	FIELD(GPIO_IMR1T1, P25, 25, 1)
	FIELD(GPIO_IMR1T1, P26, 26, 1)
	FIELD(GPIO_IMR1T1, P27, 27, 1)
	FIELD(GPIO_IMR1T1, P28, 28, 1)
	FIELD(GPIO_IMR1T1, P29, 29, 1)
	FIELD(GPIO_IMR1T1, P30, 30, 1)
	FIELD(GPIO_IMR1T1, P31, 31, 1)
REG32(GPIO_IMR1T2, 0x4bc)
	FIELD(GPIO_IMR1T2, P0, 0, 1)
	FIELD(GPIO_IMR1T2, P1, 1, 1)
	FIELD(GPIO_IMR1T2, P2, 2, 1)
	FIELD(GPIO_IMR1T2, P3, 3, 1)
	FIELD(GPIO_IMR1T2, P4, 4, 1)
	FIELD(GPIO_IMR1T2, P5, 5, 1)
	FIELD(GPIO_IMR1T2, P6, 6, 1)
	FIELD(GPIO_IMR1T2, P7, 7, 1)
	FIELD(GPIO_IMR1T2, P8, 8, 1)
	FIELD(GPIO_IMR1T2, P9, 9, 1)
	FIELD(GPIO_IMR1T2, P10, 10, 1)
	FIELD(GPIO_IMR1T2, P11, 11, 1)
	FIELD(GPIO_IMR1T2, P12, 12, 1)
	FIELD(GPIO_IMR1T2, P13, 13, 1)
	FIELD(GPIO_IMR1T2, P14, 14, 1)
	FIELD(GPIO_IMR1T2, P15, 15, 1)
	FIELD(GPIO_IMR1T2, P16, 16, 1)
	FIELD(GPIO_IMR1T2, P17, 17, 1)
	FIELD(GPIO_IMR1T2, P18, 18, 1)
	FIELD(GPIO_IMR1T2, P19, 19, 1)
	FIELD(GPIO_IMR1T2, P20, 20, 1)
	FIELD(GPIO_IMR1T2, P21, 21, 1)
	FIELD(GPIO_IMR1T2, P22, 22, 1)
	FIELD(GPIO_IMR1T2, P23, 23, 1)
	FIELD(GPIO_IMR1T2, P24, 24, 1)
	FIELD(GPIO_IMR1T2, P25, 25, 1)
	FIELD(GPIO_IMR1T2, P26, 26, 1)
	FIELD(GPIO_IMR1T2, P27, 27, 1)
	FIELD(GPIO_IMR1T2, P28, 28, 1)
	FIELD(GPIO_IMR1T2, P29, 29, 1)
	FIELD(GPIO_IMR1T2, P30, 30, 1)
	FIELD(GPIO_IMR1T2, P31, 31, 1)
REG32(GPIO_IMR00, 0xa0)
	FIELD(GPIO_IMR00, P0, 0, 1)
	FIELD(GPIO_IMR00, P1, 1, 1)
	FIELD(GPIO_IMR00, P2, 2, 1)
	FIELD(GPIO_IMR00, P3, 3, 1)
	FIELD(GPIO_IMR00, P4, 4, 1)
	FIELD(GPIO_IMR00, P5, 5, 1)
	FIELD(GPIO_IMR00, P6, 6, 1)
	FIELD(GPIO_IMR00, P7, 7, 1)
	FIELD(GPIO_IMR00, P8, 8, 1)
	FIELD(GPIO_IMR00, P9, 9, 1)
	FIELD(GPIO_IMR00, P10, 10, 1)
	FIELD(GPIO_IMR00, P11, 11, 1)
	FIELD(GPIO_IMR00, P12, 12, 1)
	FIELD(GPIO_IMR00, P13, 13, 1)
	FIELD(GPIO_IMR00, P14, 14, 1)
	FIELD(GPIO_IMR00, P15, 15, 1)
	FIELD(GPIO_IMR00, P16, 16, 1)
	FIELD(GPIO_IMR00, P17, 17, 1)
	FIELD(GPIO_IMR00, P18, 18, 1)
	FIELD(GPIO_IMR00, P19, 19, 1)
	FIELD(GPIO_IMR00, P20, 20, 1)
	FIELD(GPIO_IMR00, P21, 21, 1)
	FIELD(GPIO_IMR00, P22, 22, 1)
	FIELD(GPIO_IMR00, P23, 23, 1)
	FIELD(GPIO_IMR00, P24, 24, 1)
	FIELD(GPIO_IMR00, P25, 25, 1)
	FIELD(GPIO_IMR00, P26, 26, 1)
	FIELD(GPIO_IMR00, P27, 27, 1)
	FIELD(GPIO_IMR00, P28, 28, 1)
	FIELD(GPIO_IMR00, P29, 29, 1)
	FIELD(GPIO_IMR00, P30, 30, 1)
	FIELD(GPIO_IMR00, P31, 31, 1)
REG32(GPIO_IMR01, 0x2a0)
	FIELD(GPIO_IMR01, P0, 0, 1)
	FIELD(GPIO_IMR01, P1, 1, 1)
	FIELD(GPIO_IMR01, P2, 2, 1)
	FIELD(GPIO_IMR01, P3, 3, 1)
	FIELD(GPIO_IMR01, P4, 4, 1)
	FIELD(GPIO_IMR01, P5, 5, 1)
	FIELD(GPIO_IMR01, P6, 6, 1)
	FIELD(GPIO_IMR01, P7, 7, 1)
	FIELD(GPIO_IMR01, P8, 8, 1)
	FIELD(GPIO_IMR01, P9, 9, 1)
	FIELD(GPIO_IMR01, P10, 10, 1)
	FIELD(GPIO_IMR01, P11, 11, 1)
	FIELD(GPIO_IMR01, P12, 12, 1)
	FIELD(GPIO_IMR01, P13, 13, 1)
	FIELD(GPIO_IMR01, P14, 14, 1)
	FIELD(GPIO_IMR01, P15, 15, 1)
	FIELD(GPIO_IMR01, P16, 16, 1)
	FIELD(GPIO_IMR01, P17, 17, 1)
	FIELD(GPIO_IMR01, P18, 18, 1)
	FIELD(GPIO_IMR01, P19, 19, 1)
	FIELD(GPIO_IMR01, P20, 20, 1)
	FIELD(GPIO_IMR01, P21, 21, 1)
	FIELD(GPIO_IMR01, P22, 22, 1)
	FIELD(GPIO_IMR01, P23, 23, 1)
	FIELD(GPIO_IMR01, P24, 24, 1)
	FIELD(GPIO_IMR01, P25, 25, 1)
	FIELD(GPIO_IMR01, P26, 26, 1)
	FIELD(GPIO_IMR01, P27, 27, 1)
	FIELD(GPIO_IMR01, P28, 28, 1)
	FIELD(GPIO_IMR01, P29, 29, 1)
	FIELD(GPIO_IMR01, P30, 30, 1)
	FIELD(GPIO_IMR01, P31, 31, 1)
REG32(GPIO_IMR02, 0x4a0)
	FIELD(GPIO_IMR02, P0, 0, 1)
	FIELD(GPIO_IMR02, P1, 1, 1)
	FIELD(GPIO_IMR02, P2, 2, 1)
	FIELD(GPIO_IMR02, P3, 3, 1)
	FIELD(GPIO_IMR02, P4, 4, 1)
	FIELD(GPIO_IMR02, P5, 5, 1)
	FIELD(GPIO_IMR02, P6, 6, 1)
	FIELD(GPIO_IMR02, P7, 7, 1)
	FIELD(GPIO_IMR02, P8, 8, 1)
	FIELD(GPIO_IMR02, P9, 9, 1)
	FIELD(GPIO_IMR02, P10, 10, 1)
	FIELD(GPIO_IMR02, P11, 11, 1)
	FIELD(GPIO_IMR02, P12, 12, 1)
	FIELD(GPIO_IMR02, P13, 13, 1)
	FIELD(GPIO_IMR02, P14, 14, 1)
	FIELD(GPIO_IMR02, P15, 15, 1)
	FIELD(GPIO_IMR02, P16, 16, 1)
	FIELD(GPIO_IMR02, P17, 17, 1)
	FIELD(GPIO_IMR02, P18, 18, 1)
	FIELD(GPIO_IMR02, P19, 19, 1)
	FIELD(GPIO_IMR02, P20, 20, 1)
	FIELD(GPIO_IMR02, P21, 21, 1)
	FIELD(GPIO_IMR02, P22, 22, 1)
	FIELD(GPIO_IMR02, P23, 23, 1)
	FIELD(GPIO_IMR02, P24, 24, 1)
	FIELD(GPIO_IMR02, P25, 25, 1)
	FIELD(GPIO_IMR02, P26, 26, 1)
	FIELD(GPIO_IMR02, P27, 27, 1)
	FIELD(GPIO_IMR02, P28, 28, 1)
	FIELD(GPIO_IMR02, P29, 29, 1)
	FIELD(GPIO_IMR02, P30, 30, 1)
	FIELD(GPIO_IMR02, P31, 31, 1)
REG32(GPIO_IMR10, 0xb0)
	FIELD(GPIO_IMR10, P0, 0, 1)
	FIELD(GPIO_IMR10, P1, 1, 1)
	FIELD(GPIO_IMR10, P2, 2, 1)
	FIELD(GPIO_IMR10, P3, 3, 1)
	FIELD(GPIO_IMR10, P4, 4, 1)
	FIELD(GPIO_IMR10, P5, 5, 1)
	FIELD(GPIO_IMR10, P6, 6, 1)
	FIELD(GPIO_IMR10, P7, 7, 1)
	FIELD(GPIO_IMR10, P8, 8, 1)
	FIELD(GPIO_IMR10, P9, 9, 1)
	FIELD(GPIO_IMR10, P10, 10, 1)
	FIELD(GPIO_IMR10, P11, 11, 1)
	FIELD(GPIO_IMR10, P12, 12, 1)
	FIELD(GPIO_IMR10, P13, 13, 1)
	FIELD(GPIO_IMR10, P14, 14, 1)
	FIELD(GPIO_IMR10, P15, 15, 1)
	FIELD(GPIO_IMR10, P16, 16, 1)
	FIELD(GPIO_IMR10, P17, 17, 1)
	FIELD(GPIO_IMR10, P18, 18, 1)
	FIELD(GPIO_IMR10, P19, 19, 1)
	FIELD(GPIO_IMR10, P20, 20, 1)
	FIELD(GPIO_IMR10, P21, 21, 1)
	FIELD(GPIO_IMR10, P22, 22, 1)
	FIELD(GPIO_IMR10, P23, 23, 1)
	FIELD(GPIO_IMR10, P24, 24, 1)
	FIELD(GPIO_IMR10, P25, 25, 1)
	FIELD(GPIO_IMR10, P26, 26, 1)
	FIELD(GPIO_IMR10, P27, 27, 1)
	FIELD(GPIO_IMR10, P28, 28, 1)
	FIELD(GPIO_IMR10, P29, 29, 1)
	FIELD(GPIO_IMR10, P30, 30, 1)
	FIELD(GPIO_IMR10, P31, 31, 1)
REG32(GPIO_IMR11, 0x2b0)
	FIELD(GPIO_IMR11, P0, 0, 1)
	FIELD(GPIO_IMR11, P1, 1, 1)
	FIELD(GPIO_IMR11, P2, 2, 1)
	FIELD(GPIO_IMR11, P3, 3, 1)
	FIELD(GPIO_IMR11, P4, 4, 1)
	FIELD(GPIO_IMR11, P5, 5, 1)
	FIELD(GPIO_IMR11, P6, 6, 1)
	FIELD(GPIO_IMR11, P7, 7, 1)
	FIELD(GPIO_IMR11, P8, 8, 1)
	FIELD(GPIO_IMR11, P9, 9, 1)
	FIELD(GPIO_IMR11, P10, 10, 1)
	FIELD(GPIO_IMR11, P11, 11, 1)
	FIELD(GPIO_IMR11, P12, 12, 1)
	FIELD(GPIO_IMR11, P13, 13, 1)
	FIELD(GPIO_IMR11, P14, 14, 1)
	FIELD(GPIO_IMR11, P15, 15, 1)
	FIELD(GPIO_IMR11, P16, 16, 1)
	FIELD(GPIO_IMR11, P17, 17, 1)
	FIELD(GPIO_IMR11, P18, 18, 1)
	FIELD(GPIO_IMR11, P19, 19, 1)
	FIELD(GPIO_IMR11, P20, 20, 1)
	FIELD(GPIO_IMR11, P21, 21, 1)
	FIELD(GPIO_IMR11, P22, 22, 1)
	FIELD(GPIO_IMR11, P23, 23, 1)
	FIELD(GPIO_IMR11, P24, 24, 1)
	FIELD(GPIO_IMR11, P25, 25, 1)
	FIELD(GPIO_IMR11, P26, 26, 1)
	FIELD(GPIO_IMR11, P27, 27, 1)
	FIELD(GPIO_IMR11, P28, 28, 1)
	FIELD(GPIO_IMR11, P29, 29, 1)
	FIELD(GPIO_IMR11, P30, 30, 1)
	FIELD(GPIO_IMR11, P31, 31, 1)
REG32(GPIO_IMR12, 0x4b0)
	FIELD(GPIO_IMR12, P0, 0, 1)
	FIELD(GPIO_IMR12, P1, 1, 1)
	FIELD(GPIO_IMR12, P2, 2, 1)
	FIELD(GPIO_IMR12, P3, 3, 1)
	FIELD(GPIO_IMR12, P4, 4, 1)
	FIELD(GPIO_IMR12, P5, 5, 1)
	FIELD(GPIO_IMR12, P6, 6, 1)
	FIELD(GPIO_IMR12, P7, 7, 1)
	FIELD(GPIO_IMR12, P8, 8, 1)
	FIELD(GPIO_IMR12, P9, 9, 1)
	FIELD(GPIO_IMR12, P10, 10, 1)
	FIELD(GPIO_IMR12, P11, 11, 1)
	FIELD(GPIO_IMR12, P12, 12, 1)
	FIELD(GPIO_IMR12, P13, 13, 1)
	FIELD(GPIO_IMR12, P14, 14, 1)
	FIELD(GPIO_IMR12, P15, 15, 1)
	FIELD(GPIO_IMR12, P16, 16, 1)
	FIELD(GPIO_IMR12, P17, 17, 1)
	FIELD(GPIO_IMR12, P18, 18, 1)
	FIELD(GPIO_IMR12, P19, 19, 1)
	FIELD(GPIO_IMR12, P20, 20, 1)
	FIELD(GPIO_IMR12, P21, 21, 1)
	FIELD(GPIO_IMR12, P22, 22, 1)
	FIELD(GPIO_IMR12, P23, 23, 1)
	FIELD(GPIO_IMR12, P24, 24, 1)
	FIELD(GPIO_IMR12, P25, 25, 1)
	FIELD(GPIO_IMR12, P26, 26, 1)
	FIELD(GPIO_IMR12, P27, 27, 1)
	FIELD(GPIO_IMR12, P28, 28, 1)
	FIELD(GPIO_IMR12, P29, 29, 1)
	FIELD(GPIO_IMR12, P30, 30, 1)
	FIELD(GPIO_IMR12, P31, 31, 1)
REG32(GPIO_LOCKC0, 0x1a8)
	FIELD(GPIO_LOCKC0, P0, 0, 1)
	FIELD(GPIO_LOCKC0, P1, 1, 1)
	FIELD(GPIO_LOCKC0, P2, 2, 1)
	FIELD(GPIO_LOCKC0, P3, 3, 1)
	FIELD(GPIO_LOCKC0, P4, 4, 1)
	FIELD(GPIO_LOCKC0, P5, 5, 1)
	FIELD(GPIO_LOCKC0, P6, 6, 1)
	FIELD(GPIO_LOCKC0, P7, 7, 1)
	FIELD(GPIO_LOCKC0, P8, 8, 1)
	FIELD(GPIO_LOCKC0, P9, 9, 1)
	FIELD(GPIO_LOCKC0, P10, 10, 1)
	FIELD(GPIO_LOCKC0, P11, 11, 1)
	FIELD(GPIO_LOCKC0, P12, 12, 1)
	FIELD(GPIO_LOCKC0, P13, 13, 1)
	FIELD(GPIO_LOCKC0, P14, 14, 1)
	FIELD(GPIO_LOCKC0, P15, 15, 1)
	FIELD(GPIO_LOCKC0, P16, 16, 1)
	FIELD(GPIO_LOCKC0, P17, 17, 1)
	FIELD(GPIO_LOCKC0, P18, 18, 1)
	FIELD(GPIO_LOCKC0, P19, 19, 1)
	FIELD(GPIO_LOCKC0, P20, 20, 1)
	FIELD(GPIO_LOCKC0, P21, 21, 1)
	FIELD(GPIO_LOCKC0, P22, 22, 1)
	FIELD(GPIO_LOCKC0, P23, 23, 1)
	FIELD(GPIO_LOCKC0, P24, 24, 1)
	FIELD(GPIO_LOCKC0, P25, 25, 1)
	FIELD(GPIO_LOCKC0, P26, 26, 1)
	FIELD(GPIO_LOCKC0, P27, 27, 1)
	FIELD(GPIO_LOCKC0, P28, 28, 1)
	FIELD(GPIO_LOCKC0, P29, 29, 1)
	FIELD(GPIO_LOCKC0, P30, 30, 1)
	FIELD(GPIO_LOCKC0, P31, 31, 1)
REG32(GPIO_LOCKC1, 0x3a8)
	FIELD(GPIO_LOCKC1, P0, 0, 1)
	FIELD(GPIO_LOCKC1, P1, 1, 1)
	FIELD(GPIO_LOCKC1, P2, 2, 1)
	FIELD(GPIO_LOCKC1, P3, 3, 1)
	FIELD(GPIO_LOCKC1, P4, 4, 1)
	FIELD(GPIO_LOCKC1, P5, 5, 1)
	FIELD(GPIO_LOCKC1, P6, 6, 1)
	FIELD(GPIO_LOCKC1, P7, 7, 1)
	FIELD(GPIO_LOCKC1, P8, 8, 1)
	FIELD(GPIO_LOCKC1, P9, 9, 1)
	FIELD(GPIO_LOCKC1, P10, 10, 1)
	FIELD(GPIO_LOCKC1, P11, 11, 1)
	FIELD(GPIO_LOCKC1, P12, 12, 1)
	FIELD(GPIO_LOCKC1, P13, 13, 1)
	FIELD(GPIO_LOCKC1, P14, 14, 1)
	FIELD(GPIO_LOCKC1, P15, 15, 1)
	FIELD(GPIO_LOCKC1, P16, 16, 1)
	FIELD(GPIO_LOCKC1, P17, 17, 1)
	FIELD(GPIO_LOCKC1, P18, 18, 1)
	FIELD(GPIO_LOCKC1, P19, 19, 1)
	FIELD(GPIO_LOCKC1, P20, 20, 1)
	FIELD(GPIO_LOCKC1, P21, 21, 1)
	FIELD(GPIO_LOCKC1, P22, 22, 1)
	FIELD(GPIO_LOCKC1, P23, 23, 1)
	FIELD(GPIO_LOCKC1, P24, 24, 1)
	FIELD(GPIO_LOCKC1, P25, 25, 1)
	FIELD(GPIO_LOCKC1, P26, 26, 1)
	FIELD(GPIO_LOCKC1, P27, 27, 1)
	FIELD(GPIO_LOCKC1, P28, 28, 1)
	FIELD(GPIO_LOCKC1, P29, 29, 1)
	FIELD(GPIO_LOCKC1, P30, 30, 1)
	FIELD(GPIO_LOCKC1, P31, 31, 1)
REG32(GPIO_LOCKC2, 0x5a8)
	FIELD(GPIO_LOCKC2, P0, 0, 1)
	FIELD(GPIO_LOCKC2, P1, 1, 1)
	FIELD(GPIO_LOCKC2, P2, 2, 1)
	FIELD(GPIO_LOCKC2, P3, 3, 1)
	FIELD(GPIO_LOCKC2, P4, 4, 1)
	FIELD(GPIO_LOCKC2, P5, 5, 1)
	FIELD(GPIO_LOCKC2, P6, 6, 1)
	FIELD(GPIO_LOCKC2, P7, 7, 1)
	FIELD(GPIO_LOCKC2, P8, 8, 1)
	FIELD(GPIO_LOCKC2, P9, 9, 1)
	FIELD(GPIO_LOCKC2, P10, 10, 1)
	FIELD(GPIO_LOCKC2, P11, 11, 1)
	FIELD(GPIO_LOCKC2, P12, 12, 1)
	FIELD(GPIO_LOCKC2, P13, 13, 1)
	FIELD(GPIO_LOCKC2, P14, 14, 1)
	FIELD(GPIO_LOCKC2, P15, 15, 1)
	FIELD(GPIO_LOCKC2, P16, 16, 1)
	FIELD(GPIO_LOCKC2, P17, 17, 1)
	FIELD(GPIO_LOCKC2, P18, 18, 1)
	FIELD(GPIO_LOCKC2, P19, 19, 1)
	FIELD(GPIO_LOCKC2, P20, 20, 1)
	FIELD(GPIO_LOCKC2, P21, 21, 1)
	FIELD(GPIO_LOCKC2, P22, 22, 1)
	FIELD(GPIO_LOCKC2, P23, 23, 1)
	FIELD(GPIO_LOCKC2, P24, 24, 1)
	FIELD(GPIO_LOCKC2, P25, 25, 1)
	FIELD(GPIO_LOCKC2, P26, 26, 1)
	FIELD(GPIO_LOCKC2, P27, 27, 1)
	FIELD(GPIO_LOCKC2, P28, 28, 1)
	FIELD(GPIO_LOCKC2, P29, 29, 1)
	FIELD(GPIO_LOCKC2, P30, 30, 1)
	FIELD(GPIO_LOCKC2, P31, 31, 1)
REG32(GPIO_LOCKS0, 0x1a4)
	FIELD(GPIO_LOCKS0, P0, 0, 1)
	FIELD(GPIO_LOCKS0, P1, 1, 1)
	FIELD(GPIO_LOCKS0, P2, 2, 1)
	FIELD(GPIO_LOCKS0, P3, 3, 1)
	FIELD(GPIO_LOCKS0, P4, 4, 1)
	FIELD(GPIO_LOCKS0, P5, 5, 1)
	FIELD(GPIO_LOCKS0, P6, 6, 1)
	FIELD(GPIO_LOCKS0, P7, 7, 1)
	FIELD(GPIO_LOCKS0, P8, 8, 1)
	FIELD(GPIO_LOCKS0, P9, 9, 1)
	FIELD(GPIO_LOCKS0, P10, 10, 1)
	FIELD(GPIO_LOCKS0, P11, 11, 1)
	FIELD(GPIO_LOCKS0, P12, 12, 1)
	FIELD(GPIO_LOCKS0, P13, 13, 1)
	FIELD(GPIO_LOCKS0, P14, 14, 1)
	FIELD(GPIO_LOCKS0, P15, 15, 1)
	FIELD(GPIO_LOCKS0, P16, 16, 1)
	FIELD(GPIO_LOCKS0, P17, 17, 1)
	FIELD(GPIO_LOCKS0, P18, 18, 1)
	FIELD(GPIO_LOCKS0, P19, 19, 1)
	FIELD(GPIO_LOCKS0, P20, 20, 1)
	FIELD(GPIO_LOCKS0, P21, 21, 1)
	FIELD(GPIO_LOCKS0, P22, 22, 1)
	FIELD(GPIO_LOCKS0, P23, 23, 1)
	FIELD(GPIO_LOCKS0, P24, 24, 1)
	FIELD(GPIO_LOCKS0, P25, 25, 1)
	FIELD(GPIO_LOCKS0, P26, 26, 1)
	FIELD(GPIO_LOCKS0, P27, 27, 1)
	FIELD(GPIO_LOCKS0, P28, 28, 1)
	FIELD(GPIO_LOCKS0, P29, 29, 1)
	FIELD(GPIO_LOCKS0, P30, 30, 1)
	FIELD(GPIO_LOCKS0, P31, 31, 1)
REG32(GPIO_LOCKS1, 0x3a4)
	FIELD(GPIO_LOCKS1, P0, 0, 1)
	FIELD(GPIO_LOCKS1, P1, 1, 1)
	FIELD(GPIO_LOCKS1, P2, 2, 1)
	FIELD(GPIO_LOCKS1, P3, 3, 1)
	FIELD(GPIO_LOCKS1, P4, 4, 1)
	FIELD(GPIO_LOCKS1, P5, 5, 1)
	FIELD(GPIO_LOCKS1, P6, 6, 1)
	FIELD(GPIO_LOCKS1, P7, 7, 1)
	FIELD(GPIO_LOCKS1, P8, 8, 1)
	FIELD(GPIO_LOCKS1, P9, 9, 1)
	FIELD(GPIO_LOCKS1, P10, 10, 1)
	FIELD(GPIO_LOCKS1, P11, 11, 1)
	FIELD(GPIO_LOCKS1, P12, 12, 1)
	FIELD(GPIO_LOCKS1, P13, 13, 1)
	FIELD(GPIO_LOCKS1, P14, 14, 1)
	FIELD(GPIO_LOCKS1, P15, 15, 1)
	FIELD(GPIO_LOCKS1, P16, 16, 1)
	FIELD(GPIO_LOCKS1, P17, 17, 1)
	FIELD(GPIO_LOCKS1, P18, 18, 1)
	FIELD(GPIO_LOCKS1, P19, 19, 1)
	FIELD(GPIO_LOCKS1, P20, 20, 1)
	FIELD(GPIO_LOCKS1, P21, 21, 1)
	FIELD(GPIO_LOCKS1, P22, 22, 1)
	FIELD(GPIO_LOCKS1, P23, 23, 1)
	FIELD(GPIO_LOCKS1, P24, 24, 1)
	FIELD(GPIO_LOCKS1, P25, 25, 1)
	FIELD(GPIO_LOCKS1, P26, 26, 1)
	FIELD(GPIO_LOCKS1, P27, 27, 1)
	FIELD(GPIO_LOCKS1, P28, 28, 1)
	FIELD(GPIO_LOCKS1, P29, 29, 1)
	FIELD(GPIO_LOCKS1, P30, 30, 1)
	FIELD(GPIO_LOCKS1, P31, 31, 1)
REG32(GPIO_LOCKS2, 0x5a4)
	FIELD(GPIO_LOCKS2, P0, 0, 1)
	FIELD(GPIO_LOCKS2, P1, 1, 1)
	FIELD(GPIO_LOCKS2, P2, 2, 1)
	FIELD(GPIO_LOCKS2, P3, 3, 1)
	FIELD(GPIO_LOCKS2, P4, 4, 1)
	FIELD(GPIO_LOCKS2, P5, 5, 1)
	FIELD(GPIO_LOCKS2, P6, 6, 1)
	FIELD(GPIO_LOCKS2, P7, 7, 1)
	FIELD(GPIO_LOCKS2, P8, 8, 1)
	FIELD(GPIO_LOCKS2, P9, 9, 1)
	FIELD(GPIO_LOCKS2, P10, 10, 1)
	FIELD(GPIO_LOCKS2, P11, 11, 1)
	FIELD(GPIO_LOCKS2, P12, 12, 1)
	FIELD(GPIO_LOCKS2, P13, 13, 1)
	FIELD(GPIO_LOCKS2, P14, 14, 1)
	FIELD(GPIO_LOCKS2, P15, 15, 1)
	FIELD(GPIO_LOCKS2, P16, 16, 1)
	FIELD(GPIO_LOCKS2, P17, 17, 1)
	FIELD(GPIO_LOCKS2, P18, 18, 1)
	FIELD(GPIO_LOCKS2, P19, 19, 1)
	FIELD(GPIO_LOCKS2, P20, 20, 1)
	FIELD(GPIO_LOCKS2, P21, 21, 1)
	FIELD(GPIO_LOCKS2, P22, 22, 1)
	FIELD(GPIO_LOCKS2, P23, 23, 1)
	FIELD(GPIO_LOCKS2, P24, 24, 1)
	FIELD(GPIO_LOCKS2, P25, 25, 1)
	FIELD(GPIO_LOCKS2, P26, 26, 1)
	FIELD(GPIO_LOCKS2, P27, 27, 1)
	FIELD(GPIO_LOCKS2, P28, 28, 1)
	FIELD(GPIO_LOCKS2, P29, 29, 1)
	FIELD(GPIO_LOCKS2, P30, 30, 1)
	FIELD(GPIO_LOCKS2, P31, 31, 1)
REG32(GPIO_LOCKT0, 0x1ac)
	FIELD(GPIO_LOCKT0, P0, 0, 1)
	FIELD(GPIO_LOCKT0, P1, 1, 1)
	FIELD(GPIO_LOCKT0, P2, 2, 1)
	FIELD(GPIO_LOCKT0, P3, 3, 1)
	FIELD(GPIO_LOCKT0, P4, 4, 1)
	FIELD(GPIO_LOCKT0, P5, 5, 1)
	FIELD(GPIO_LOCKT0, P6, 6, 1)
	FIELD(GPIO_LOCKT0, P7, 7, 1)
	FIELD(GPIO_LOCKT0, P8, 8, 1)
	FIELD(GPIO_LOCKT0, P9, 9, 1)
	FIELD(GPIO_LOCKT0, P10, 10, 1)
	FIELD(GPIO_LOCKT0, P11, 11, 1)
	FIELD(GPIO_LOCKT0, P12, 12, 1)
	FIELD(GPIO_LOCKT0, P13, 13, 1)
	FIELD(GPIO_LOCKT0, P14, 14, 1)
	FIELD(GPIO_LOCKT0, P15, 15, 1)
	FIELD(GPIO_LOCKT0, P16, 16, 1)
	FIELD(GPIO_LOCKT0, P17, 17, 1)
	FIELD(GPIO_LOCKT0, P18, 18, 1)
	FIELD(GPIO_LOCKT0, P19, 19, 1)
	FIELD(GPIO_LOCKT0, P20, 20, 1)
	FIELD(GPIO_LOCKT0, P21, 21, 1)
	FIELD(GPIO_LOCKT0, P22, 22, 1)
	FIELD(GPIO_LOCKT0, P23, 23, 1)
	FIELD(GPIO_LOCKT0, P24, 24, 1)
	FIELD(GPIO_LOCKT0, P25, 25, 1)
	FIELD(GPIO_LOCKT0, P26, 26, 1)
	FIELD(GPIO_LOCKT0, P27, 27, 1)
	FIELD(GPIO_LOCKT0, P28, 28, 1)
	FIELD(GPIO_LOCKT0, P29, 29, 1)
	FIELD(GPIO_LOCKT0, P30, 30, 1)
	FIELD(GPIO_LOCKT0, P31, 31, 1)
REG32(GPIO_LOCKT1, 0x3ac)
	FIELD(GPIO_LOCKT1, P0, 0, 1)
	FIELD(GPIO_LOCKT1, P1, 1, 1)
	FIELD(GPIO_LOCKT1, P2, 2, 1)
	FIELD(GPIO_LOCKT1, P3, 3, 1)
	FIELD(GPIO_LOCKT1, P4, 4, 1)
	FIELD(GPIO_LOCKT1, P5, 5, 1)
	FIELD(GPIO_LOCKT1, P6, 6, 1)
	FIELD(GPIO_LOCKT1, P7, 7, 1)
	FIELD(GPIO_LOCKT1, P8, 8, 1)
	FIELD(GPIO_LOCKT1, P9, 9, 1)
	FIELD(GPIO_LOCKT1, P10, 10, 1)
	FIELD(GPIO_LOCKT1, P11, 11, 1)
	FIELD(GPIO_LOCKT1, P12, 12, 1)
	FIELD(GPIO_LOCKT1, P13, 13, 1)
	FIELD(GPIO_LOCKT1, P14, 14, 1)
	FIELD(GPIO_LOCKT1, P15, 15, 1)
	FIELD(GPIO_LOCKT1, P16, 16, 1)
	FIELD(GPIO_LOCKT1, P17, 17, 1)
	FIELD(GPIO_LOCKT1, P18, 18, 1)
	FIELD(GPIO_LOCKT1, P19, 19, 1)
	FIELD(GPIO_LOCKT1, P20, 20, 1)
	FIELD(GPIO_LOCKT1, P21, 21, 1)
	FIELD(GPIO_LOCKT1, P22, 22, 1)
	FIELD(GPIO_LOCKT1, P23, 23, 1)
	FIELD(GPIO_LOCKT1, P24, 24, 1)
	FIELD(GPIO_LOCKT1, P25, 25, 1)
	FIELD(GPIO_LOCKT1, P26, 26, 1)
	FIELD(GPIO_LOCKT1, P27, 27, 1)
	FIELD(GPIO_LOCKT1, P28, 28, 1)
	FIELD(GPIO_LOCKT1, P29, 29, 1)
	FIELD(GPIO_LOCKT1, P30, 30, 1)
	FIELD(GPIO_LOCKT1, P31, 31, 1)
REG32(GPIO_LOCKT2, 0x5ac)
	FIELD(GPIO_LOCKT2, P0, 0, 1)
	FIELD(GPIO_LOCKT2, P1, 1, 1)
	FIELD(GPIO_LOCKT2, P2, 2, 1)
	FIELD(GPIO_LOCKT2, P3, 3, 1)
	FIELD(GPIO_LOCKT2, P4, 4, 1)
	FIELD(GPIO_LOCKT2, P5, 5, 1)
	FIELD(GPIO_LOCKT2, P6, 6, 1)
	FIELD(GPIO_LOCKT2, P7, 7, 1)
	FIELD(GPIO_LOCKT2, P8, 8, 1)
	FIELD(GPIO_LOCKT2, P9, 9, 1)
	FIELD(GPIO_LOCKT2, P10, 10, 1)
	FIELD(GPIO_LOCKT2, P11, 11, 1)
	FIELD(GPIO_LOCKT2, P12, 12, 1)
	FIELD(GPIO_LOCKT2, P13, 13, 1)
	FIELD(GPIO_LOCKT2, P14, 14, 1)
	FIELD(GPIO_LOCKT2, P15, 15, 1)
	FIELD(GPIO_LOCKT2, P16, 16, 1)
	FIELD(GPIO_LOCKT2, P17, 17, 1)
	FIELD(GPIO_LOCKT2, P18, 18, 1)
	FIELD(GPIO_LOCKT2, P19, 19, 1)
	FIELD(GPIO_LOCKT2, P20, 20, 1)
	FIELD(GPIO_LOCKT2, P21, 21, 1)
	FIELD(GPIO_LOCKT2, P22, 22, 1)
	FIELD(GPIO_LOCKT2, P23, 23, 1)
	FIELD(GPIO_LOCKT2, P24, 24, 1)
	FIELD(GPIO_LOCKT2, P25, 25, 1)
	FIELD(GPIO_LOCKT2, P26, 26, 1)
	FIELD(GPIO_LOCKT2, P27, 27, 1)
	FIELD(GPIO_LOCKT2, P28, 28, 1)
	FIELD(GPIO_LOCKT2, P29, 29, 1)
	FIELD(GPIO_LOCKT2, P30, 30, 1)
	FIELD(GPIO_LOCKT2, P31, 31, 1)
REG32(GPIO_LOCK0, 0x1a0)
	FIELD(GPIO_LOCK0, P0, 0, 1)
	FIELD(GPIO_LOCK0, P1, 1, 1)
	FIELD(GPIO_LOCK0, P2, 2, 1)
	FIELD(GPIO_LOCK0, P3, 3, 1)
	FIELD(GPIO_LOCK0, P4, 4, 1)
	FIELD(GPIO_LOCK0, P5, 5, 1)
	FIELD(GPIO_LOCK0, P6, 6, 1)
	FIELD(GPIO_LOCK0, P7, 7, 1)
	FIELD(GPIO_LOCK0, P8, 8, 1)
	FIELD(GPIO_LOCK0, P9, 9, 1)
	FIELD(GPIO_LOCK0, P10, 10, 1)
	FIELD(GPIO_LOCK0, P11, 11, 1)
	FIELD(GPIO_LOCK0, P12, 12, 1)
	FIELD(GPIO_LOCK0, P13, 13, 1)
	FIELD(GPIO_LOCK0, P14, 14, 1)
	FIELD(GPIO_LOCK0, P15, 15, 1)
	FIELD(GPIO_LOCK0, P16, 16, 1)
	FIELD(GPIO_LOCK0, P17, 17, 1)
	FIELD(GPIO_LOCK0, P18, 18, 1)
	FIELD(GPIO_LOCK0, P19, 19, 1)
	FIELD(GPIO_LOCK0, P20, 20, 1)
	FIELD(GPIO_LOCK0, P21, 21, 1)
	FIELD(GPIO_LOCK0, P22, 22, 1)
	FIELD(GPIO_LOCK0, P23, 23, 1)
	FIELD(GPIO_LOCK0, P24, 24, 1)
	FIELD(GPIO_LOCK0, P25, 25, 1)
	FIELD(GPIO_LOCK0, P26, 26, 1)
	FIELD(GPIO_LOCK0, P27, 27, 1)
	FIELD(GPIO_LOCK0, P28, 28, 1)
	FIELD(GPIO_LOCK0, P29, 29, 1)
	FIELD(GPIO_LOCK0, P30, 30, 1)
	FIELD(GPIO_LOCK0, P31, 31, 1)
REG32(GPIO_LOCK1, 0x3a0)
	FIELD(GPIO_LOCK1, P0, 0, 1)
	FIELD(GPIO_LOCK1, P1, 1, 1)
	FIELD(GPIO_LOCK1, P2, 2, 1)
	FIELD(GPIO_LOCK1, P3, 3, 1)
	FIELD(GPIO_LOCK1, P4, 4, 1)
	FIELD(GPIO_LOCK1, P5, 5, 1)
	FIELD(GPIO_LOCK1, P6, 6, 1)
	FIELD(GPIO_LOCK1, P7, 7, 1)
	FIELD(GPIO_LOCK1, P8, 8, 1)
	FIELD(GPIO_LOCK1, P9, 9, 1)
	FIELD(GPIO_LOCK1, P10, 10, 1)
	FIELD(GPIO_LOCK1, P11, 11, 1)
	FIELD(GPIO_LOCK1, P12, 12, 1)
	FIELD(GPIO_LOCK1, P13, 13, 1)
	FIELD(GPIO_LOCK1, P14, 14, 1)
	FIELD(GPIO_LOCK1, P15, 15, 1)
	FIELD(GPIO_LOCK1, P16, 16, 1)
	FIELD(GPIO_LOCK1, P17, 17, 1)
	FIELD(GPIO_LOCK1, P18, 18, 1)
	FIELD(GPIO_LOCK1, P19, 19, 1)
	FIELD(GPIO_LOCK1, P20, 20, 1)
	FIELD(GPIO_LOCK1, P21, 21, 1)
	FIELD(GPIO_LOCK1, P22, 22, 1)
	FIELD(GPIO_LOCK1, P23, 23, 1)
	FIELD(GPIO_LOCK1, P24, 24, 1)
	FIELD(GPIO_LOCK1, P25, 25, 1)
	FIELD(GPIO_LOCK1, P26, 26, 1)
	FIELD(GPIO_LOCK1, P27, 27, 1)
	FIELD(GPIO_LOCK1, P28, 28, 1)
	FIELD(GPIO_LOCK1, P29, 29, 1)
	FIELD(GPIO_LOCK1, P30, 30, 1)
	FIELD(GPIO_LOCK1, P31, 31, 1)
REG32(GPIO_LOCK2, 0x5a0)
	FIELD(GPIO_LOCK2, P0, 0, 1)
	FIELD(GPIO_LOCK2, P1, 1, 1)
	FIELD(GPIO_LOCK2, P2, 2, 1)
	FIELD(GPIO_LOCK2, P3, 3, 1)
	FIELD(GPIO_LOCK2, P4, 4, 1)
	FIELD(GPIO_LOCK2, P5, 5, 1)
	FIELD(GPIO_LOCK2, P6, 6, 1)
	FIELD(GPIO_LOCK2, P7, 7, 1)
	FIELD(GPIO_LOCK2, P8, 8, 1)
	FIELD(GPIO_LOCK2, P9, 9, 1)
	FIELD(GPIO_LOCK2, P10, 10, 1)
	FIELD(GPIO_LOCK2, P11, 11, 1)
	FIELD(GPIO_LOCK2, P12, 12, 1)
	FIELD(GPIO_LOCK2, P13, 13, 1)
	FIELD(GPIO_LOCK2, P14, 14, 1)
	FIELD(GPIO_LOCK2, P15, 15, 1)
	FIELD(GPIO_LOCK2, P16, 16, 1)
	FIELD(GPIO_LOCK2, P17, 17, 1)
	FIELD(GPIO_LOCK2, P18, 18, 1)
	FIELD(GPIO_LOCK2, P19, 19, 1)
	FIELD(GPIO_LOCK2, P20, 20, 1)
	FIELD(GPIO_LOCK2, P21, 21, 1)
	FIELD(GPIO_LOCK2, P22, 22, 1)
	FIELD(GPIO_LOCK2, P23, 23, 1)
	FIELD(GPIO_LOCK2, P24, 24, 1)
	FIELD(GPIO_LOCK2, P25, 25, 1)
	FIELD(GPIO_LOCK2, P26, 26, 1)
	FIELD(GPIO_LOCK2, P27, 27, 1)
	FIELD(GPIO_LOCK2, P28, 28, 1)
	FIELD(GPIO_LOCK2, P29, 29, 1)
	FIELD(GPIO_LOCK2, P30, 30, 1)
	FIELD(GPIO_LOCK2, P31, 31, 1)
REG32(GPIO_ODCR0C0, 0x108)
	FIELD(GPIO_ODCR0C0, P0, 0, 1)
	FIELD(GPIO_ODCR0C0, P1, 1, 1)
	FIELD(GPIO_ODCR0C0, P2, 2, 1)
	FIELD(GPIO_ODCR0C0, P3, 3, 1)
	FIELD(GPIO_ODCR0C0, P4, 4, 1)
	FIELD(GPIO_ODCR0C0, P5, 5, 1)
	FIELD(GPIO_ODCR0C0, P6, 6, 1)
	FIELD(GPIO_ODCR0C0, P7, 7, 1)
	FIELD(GPIO_ODCR0C0, P8, 8, 1)
	FIELD(GPIO_ODCR0C0, P9, 9, 1)
	FIELD(GPIO_ODCR0C0, P10, 10, 1)
	FIELD(GPIO_ODCR0C0, P11, 11, 1)
	FIELD(GPIO_ODCR0C0, P12, 12, 1)
	FIELD(GPIO_ODCR0C0, P13, 13, 1)
	FIELD(GPIO_ODCR0C0, P14, 14, 1)
	FIELD(GPIO_ODCR0C0, P15, 15, 1)
	FIELD(GPIO_ODCR0C0, P16, 16, 1)
	FIELD(GPIO_ODCR0C0, P17, 17, 1)
	FIELD(GPIO_ODCR0C0, P18, 18, 1)
	FIELD(GPIO_ODCR0C0, P19, 19, 1)
	FIELD(GPIO_ODCR0C0, P20, 20, 1)
	FIELD(GPIO_ODCR0C0, P21, 21, 1)
	FIELD(GPIO_ODCR0C0, P22, 22, 1)
	FIELD(GPIO_ODCR0C0, P23, 23, 1)
	FIELD(GPIO_ODCR0C0, P24, 24, 1)
	FIELD(GPIO_ODCR0C0, P25, 25, 1)
	FIELD(GPIO_ODCR0C0, P26, 26, 1)
	FIELD(GPIO_ODCR0C0, P27, 27, 1)
	FIELD(GPIO_ODCR0C0, P28, 28, 1)
	FIELD(GPIO_ODCR0C0, P29, 29, 1)
	FIELD(GPIO_ODCR0C0, P30, 30, 1)
	FIELD(GPIO_ODCR0C0, P31, 31, 1)
REG32(GPIO_ODCR0C1, 0x308)
	FIELD(GPIO_ODCR0C1, P0, 0, 1)
	FIELD(GPIO_ODCR0C1, P1, 1, 1)
	FIELD(GPIO_ODCR0C1, P2, 2, 1)
	FIELD(GPIO_ODCR0C1, P3, 3, 1)
	FIELD(GPIO_ODCR0C1, P4, 4, 1)
	FIELD(GPIO_ODCR0C1, P5, 5, 1)
	FIELD(GPIO_ODCR0C1, P6, 6, 1)
	FIELD(GPIO_ODCR0C1, P7, 7, 1)
	FIELD(GPIO_ODCR0C1, P8, 8, 1)
	FIELD(GPIO_ODCR0C1, P9, 9, 1)
	FIELD(GPIO_ODCR0C1, P10, 10, 1)
	FIELD(GPIO_ODCR0C1, P11, 11, 1)
	FIELD(GPIO_ODCR0C1, P12, 12, 1)
	FIELD(GPIO_ODCR0C1, P13, 13, 1)
	FIELD(GPIO_ODCR0C1, P14, 14, 1)
	FIELD(GPIO_ODCR0C1, P15, 15, 1)
	FIELD(GPIO_ODCR0C1, P16, 16, 1)
	FIELD(GPIO_ODCR0C1, P17, 17, 1)
	FIELD(GPIO_ODCR0C1, P18, 18, 1)
	FIELD(GPIO_ODCR0C1, P19, 19, 1)
	FIELD(GPIO_ODCR0C1, P20, 20, 1)
	FIELD(GPIO_ODCR0C1, P21, 21, 1)
	FIELD(GPIO_ODCR0C1, P22, 22, 1)
	FIELD(GPIO_ODCR0C1, P23, 23, 1)
	FIELD(GPIO_ODCR0C1, P24, 24, 1)
	FIELD(GPIO_ODCR0C1, P25, 25, 1)
	FIELD(GPIO_ODCR0C1, P26, 26, 1)
	FIELD(GPIO_ODCR0C1, P27, 27, 1)
	FIELD(GPIO_ODCR0C1, P28, 28, 1)
	FIELD(GPIO_ODCR0C1, P29, 29, 1)
	FIELD(GPIO_ODCR0C1, P30, 30, 1)
	FIELD(GPIO_ODCR0C1, P31, 31, 1)
REG32(GPIO_ODCR0C2, 0x508)
	FIELD(GPIO_ODCR0C2, P0, 0, 1)
	FIELD(GPIO_ODCR0C2, P1, 1, 1)
	FIELD(GPIO_ODCR0C2, P2, 2, 1)
	FIELD(GPIO_ODCR0C2, P3, 3, 1)
	FIELD(GPIO_ODCR0C2, P4, 4, 1)
	FIELD(GPIO_ODCR0C2, P5, 5, 1)
	FIELD(GPIO_ODCR0C2, P6, 6, 1)
	FIELD(GPIO_ODCR0C2, P7, 7, 1)
	FIELD(GPIO_ODCR0C2, P8, 8, 1)
	FIELD(GPIO_ODCR0C2, P9, 9, 1)
	FIELD(GPIO_ODCR0C2, P10, 10, 1)
	FIELD(GPIO_ODCR0C2, P11, 11, 1)
	FIELD(GPIO_ODCR0C2, P12, 12, 1)
	FIELD(GPIO_ODCR0C2, P13, 13, 1)
	FIELD(GPIO_ODCR0C2, P14, 14, 1)
	FIELD(GPIO_ODCR0C2, P15, 15, 1)
	FIELD(GPIO_ODCR0C2, P16, 16, 1)
	FIELD(GPIO_ODCR0C2, P17, 17, 1)
	FIELD(GPIO_ODCR0C2, P18, 18, 1)
	FIELD(GPIO_ODCR0C2, P19, 19, 1)
	FIELD(GPIO_ODCR0C2, P20, 20, 1)
	FIELD(GPIO_ODCR0C2, P21, 21, 1)
	FIELD(GPIO_ODCR0C2, P22, 22, 1)
	FIELD(GPIO_ODCR0C2, P23, 23, 1)
	FIELD(GPIO_ODCR0C2, P24, 24, 1)
	FIELD(GPIO_ODCR0C2, P25, 25, 1)
	FIELD(GPIO_ODCR0C2, P26, 26, 1)
	FIELD(GPIO_ODCR0C2, P27, 27, 1)
	FIELD(GPIO_ODCR0C2, P28, 28, 1)
	FIELD(GPIO_ODCR0C2, P29, 29, 1)
	FIELD(GPIO_ODCR0C2, P30, 30, 1)
	FIELD(GPIO_ODCR0C2, P31, 31, 1)
REG32(GPIO_ODCR0S0, 0x104)
	FIELD(GPIO_ODCR0S0, P0, 0, 1)
	FIELD(GPIO_ODCR0S0, P1, 1, 1)
	FIELD(GPIO_ODCR0S0, P2, 2, 1)
	FIELD(GPIO_ODCR0S0, P3, 3, 1)
	FIELD(GPIO_ODCR0S0, P4, 4, 1)
	FIELD(GPIO_ODCR0S0, P5, 5, 1)
	FIELD(GPIO_ODCR0S0, P6, 6, 1)
	FIELD(GPIO_ODCR0S0, P7, 7, 1)
	FIELD(GPIO_ODCR0S0, P8, 8, 1)
	FIELD(GPIO_ODCR0S0, P9, 9, 1)
	FIELD(GPIO_ODCR0S0, P10, 10, 1)
	FIELD(GPIO_ODCR0S0, P11, 11, 1)
	FIELD(GPIO_ODCR0S0, P12, 12, 1)
	FIELD(GPIO_ODCR0S0, P13, 13, 1)
	FIELD(GPIO_ODCR0S0, P14, 14, 1)
	FIELD(GPIO_ODCR0S0, P15, 15, 1)
	FIELD(GPIO_ODCR0S0, P16, 16, 1)
	FIELD(GPIO_ODCR0S0, P17, 17, 1)
	FIELD(GPIO_ODCR0S0, P18, 18, 1)
	FIELD(GPIO_ODCR0S0, P19, 19, 1)
	FIELD(GPIO_ODCR0S0, P20, 20, 1)
	FIELD(GPIO_ODCR0S0, P21, 21, 1)
	FIELD(GPIO_ODCR0S0, P22, 22, 1)
	FIELD(GPIO_ODCR0S0, P23, 23, 1)
	FIELD(GPIO_ODCR0S0, P24, 24, 1)
	FIELD(GPIO_ODCR0S0, P25, 25, 1)
	FIELD(GPIO_ODCR0S0, P26, 26, 1)
	FIELD(GPIO_ODCR0S0, P27, 27, 1)
	FIELD(GPIO_ODCR0S0, P28, 28, 1)
	FIELD(GPIO_ODCR0S0, P29, 29, 1)
	FIELD(GPIO_ODCR0S0, P30, 30, 1)
	FIELD(GPIO_ODCR0S0, P31, 31, 1)
REG32(GPIO_ODCR0S1, 0x304)
	FIELD(GPIO_ODCR0S1, P0, 0, 1)
	FIELD(GPIO_ODCR0S1, P1, 1, 1)
	FIELD(GPIO_ODCR0S1, P2, 2, 1)
	FIELD(GPIO_ODCR0S1, P3, 3, 1)
	FIELD(GPIO_ODCR0S1, P4, 4, 1)
	FIELD(GPIO_ODCR0S1, P5, 5, 1)
	FIELD(GPIO_ODCR0S1, P6, 6, 1)
	FIELD(GPIO_ODCR0S1, P7, 7, 1)
	FIELD(GPIO_ODCR0S1, P8, 8, 1)
	FIELD(GPIO_ODCR0S1, P9, 9, 1)
	FIELD(GPIO_ODCR0S1, P10, 10, 1)
	FIELD(GPIO_ODCR0S1, P11, 11, 1)
	FIELD(GPIO_ODCR0S1, P12, 12, 1)
	FIELD(GPIO_ODCR0S1, P13, 13, 1)
	FIELD(GPIO_ODCR0S1, P14, 14, 1)
	FIELD(GPIO_ODCR0S1, P15, 15, 1)
	FIELD(GPIO_ODCR0S1, P16, 16, 1)
	FIELD(GPIO_ODCR0S1, P17, 17, 1)
	FIELD(GPIO_ODCR0S1, P18, 18, 1)
	FIELD(GPIO_ODCR0S1, P19, 19, 1)
	FIELD(GPIO_ODCR0S1, P20, 20, 1)
	FIELD(GPIO_ODCR0S1, P21, 21, 1)
	FIELD(GPIO_ODCR0S1, P22, 22, 1)
	FIELD(GPIO_ODCR0S1, P23, 23, 1)
	FIELD(GPIO_ODCR0S1, P24, 24, 1)
	FIELD(GPIO_ODCR0S1, P25, 25, 1)
	FIELD(GPIO_ODCR0S1, P26, 26, 1)
	FIELD(GPIO_ODCR0S1, P27, 27, 1)
	FIELD(GPIO_ODCR0S1, P28, 28, 1)
	FIELD(GPIO_ODCR0S1, P29, 29, 1)
	FIELD(GPIO_ODCR0S1, P30, 30, 1)
	FIELD(GPIO_ODCR0S1, P31, 31, 1)
REG32(GPIO_ODCR0S2, 0x504)
	FIELD(GPIO_ODCR0S2, P0, 0, 1)
	FIELD(GPIO_ODCR0S2, P1, 1, 1)
	FIELD(GPIO_ODCR0S2, P2, 2, 1)
	FIELD(GPIO_ODCR0S2, P3, 3, 1)
	FIELD(GPIO_ODCR0S2, P4, 4, 1)
	FIELD(GPIO_ODCR0S2, P5, 5, 1)
	FIELD(GPIO_ODCR0S2, P6, 6, 1)
	FIELD(GPIO_ODCR0S2, P7, 7, 1)
	FIELD(GPIO_ODCR0S2, P8, 8, 1)
	FIELD(GPIO_ODCR0S2, P9, 9, 1)
	FIELD(GPIO_ODCR0S2, P10, 10, 1)
	FIELD(GPIO_ODCR0S2, P11, 11, 1)
	FIELD(GPIO_ODCR0S2, P12, 12, 1)
	FIELD(GPIO_ODCR0S2, P13, 13, 1)
	FIELD(GPIO_ODCR0S2, P14, 14, 1)
	FIELD(GPIO_ODCR0S2, P15, 15, 1)
	FIELD(GPIO_ODCR0S2, P16, 16, 1)
	FIELD(GPIO_ODCR0S2, P17, 17, 1)
	FIELD(GPIO_ODCR0S2, P18, 18, 1)
	FIELD(GPIO_ODCR0S2, P19, 19, 1)
	FIELD(GPIO_ODCR0S2, P20, 20, 1)
	FIELD(GPIO_ODCR0S2, P21, 21, 1)
	FIELD(GPIO_ODCR0S2, P22, 22, 1)
	FIELD(GPIO_ODCR0S2, P23, 23, 1)
	FIELD(GPIO_ODCR0S2, P24, 24, 1)
	FIELD(GPIO_ODCR0S2, P25, 25, 1)
	FIELD(GPIO_ODCR0S2, P26, 26, 1)
	FIELD(GPIO_ODCR0S2, P27, 27, 1)
	FIELD(GPIO_ODCR0S2, P28, 28, 1)
	FIELD(GPIO_ODCR0S2, P29, 29, 1)
	FIELD(GPIO_ODCR0S2, P30, 30, 1)
	FIELD(GPIO_ODCR0S2, P31, 31, 1)
REG32(GPIO_ODCR0T0, 0x10c)
	FIELD(GPIO_ODCR0T0, P0, 0, 1)
	FIELD(GPIO_ODCR0T0, P1, 1, 1)
	FIELD(GPIO_ODCR0T0, P2, 2, 1)
	FIELD(GPIO_ODCR0T0, P3, 3, 1)
	FIELD(GPIO_ODCR0T0, P4, 4, 1)
	FIELD(GPIO_ODCR0T0, P5, 5, 1)
	FIELD(GPIO_ODCR0T0, P6, 6, 1)
	FIELD(GPIO_ODCR0T0, P7, 7, 1)
	FIELD(GPIO_ODCR0T0, P8, 8, 1)
	FIELD(GPIO_ODCR0T0, P9, 9, 1)
	FIELD(GPIO_ODCR0T0, P10, 10, 1)
	FIELD(GPIO_ODCR0T0, P11, 11, 1)
	FIELD(GPIO_ODCR0T0, P12, 12, 1)
	FIELD(GPIO_ODCR0T0, P13, 13, 1)
	FIELD(GPIO_ODCR0T0, P14, 14, 1)
	FIELD(GPIO_ODCR0T0, P15, 15, 1)
	FIELD(GPIO_ODCR0T0, P16, 16, 1)
	FIELD(GPIO_ODCR0T0, P17, 17, 1)
	FIELD(GPIO_ODCR0T0, P18, 18, 1)
	FIELD(GPIO_ODCR0T0, P19, 19, 1)
	FIELD(GPIO_ODCR0T0, P20, 20, 1)
	FIELD(GPIO_ODCR0T0, P21, 21, 1)
	FIELD(GPIO_ODCR0T0, P22, 22, 1)
	FIELD(GPIO_ODCR0T0, P23, 23, 1)
	FIELD(GPIO_ODCR0T0, P24, 24, 1)
	FIELD(GPIO_ODCR0T0, P25, 25, 1)
	FIELD(GPIO_ODCR0T0, P26, 26, 1)
	FIELD(GPIO_ODCR0T0, P27, 27, 1)
	FIELD(GPIO_ODCR0T0, P28, 28, 1)
	FIELD(GPIO_ODCR0T0, P29, 29, 1)
	FIELD(GPIO_ODCR0T0, P30, 30, 1)
	FIELD(GPIO_ODCR0T0, P31, 31, 1)
REG32(GPIO_ODCR0T1, 0x30c)
	FIELD(GPIO_ODCR0T1, P0, 0, 1)
	FIELD(GPIO_ODCR0T1, P1, 1, 1)
	FIELD(GPIO_ODCR0T1, P2, 2, 1)
	FIELD(GPIO_ODCR0T1, P3, 3, 1)
	FIELD(GPIO_ODCR0T1, P4, 4, 1)
	FIELD(GPIO_ODCR0T1, P5, 5, 1)
	FIELD(GPIO_ODCR0T1, P6, 6, 1)
	FIELD(GPIO_ODCR0T1, P7, 7, 1)
	FIELD(GPIO_ODCR0T1, P8, 8, 1)
	FIELD(GPIO_ODCR0T1, P9, 9, 1)
	FIELD(GPIO_ODCR0T1, P10, 10, 1)
	FIELD(GPIO_ODCR0T1, P11, 11, 1)
	FIELD(GPIO_ODCR0T1, P12, 12, 1)
	FIELD(GPIO_ODCR0T1, P13, 13, 1)
	FIELD(GPIO_ODCR0T1, P14, 14, 1)
	FIELD(GPIO_ODCR0T1, P15, 15, 1)
	FIELD(GPIO_ODCR0T1, P16, 16, 1)
	FIELD(GPIO_ODCR0T1, P17, 17, 1)
	FIELD(GPIO_ODCR0T1, P18, 18, 1)
	FIELD(GPIO_ODCR0T1, P19, 19, 1)
	FIELD(GPIO_ODCR0T1, P20, 20, 1)
	FIELD(GPIO_ODCR0T1, P21, 21, 1)
	FIELD(GPIO_ODCR0T1, P22, 22, 1)
	FIELD(GPIO_ODCR0T1, P23, 23, 1)
	FIELD(GPIO_ODCR0T1, P24, 24, 1)
	FIELD(GPIO_ODCR0T1, P25, 25, 1)
	FIELD(GPIO_ODCR0T1, P26, 26, 1)
	FIELD(GPIO_ODCR0T1, P27, 27, 1)
	FIELD(GPIO_ODCR0T1, P28, 28, 1)
	FIELD(GPIO_ODCR0T1, P29, 29, 1)
	FIELD(GPIO_ODCR0T1, P30, 30, 1)
	FIELD(GPIO_ODCR0T1, P31, 31, 1)
REG32(GPIO_ODCR0T2, 0x50c)
	FIELD(GPIO_ODCR0T2, P0, 0, 1)
	FIELD(GPIO_ODCR0T2, P1, 1, 1)
	FIELD(GPIO_ODCR0T2, P2, 2, 1)
	FIELD(GPIO_ODCR0T2, P3, 3, 1)
	FIELD(GPIO_ODCR0T2, P4, 4, 1)
	FIELD(GPIO_ODCR0T2, P5, 5, 1)
	FIELD(GPIO_ODCR0T2, P6, 6, 1)
	FIELD(GPIO_ODCR0T2, P7, 7, 1)
	FIELD(GPIO_ODCR0T2, P8, 8, 1)
	FIELD(GPIO_ODCR0T2, P9, 9, 1)
	FIELD(GPIO_ODCR0T2, P10, 10, 1)
	FIELD(GPIO_ODCR0T2, P11, 11, 1)
	FIELD(GPIO_ODCR0T2, P12, 12, 1)
	FIELD(GPIO_ODCR0T2, P13, 13, 1)
	FIELD(GPIO_ODCR0T2, P14, 14, 1)
	FIELD(GPIO_ODCR0T2, P15, 15, 1)
	FIELD(GPIO_ODCR0T2, P16, 16, 1)
	FIELD(GPIO_ODCR0T2, P17, 17, 1)
	FIELD(GPIO_ODCR0T2, P18, 18, 1)
	FIELD(GPIO_ODCR0T2, P19, 19, 1)
	FIELD(GPIO_ODCR0T2, P20, 20, 1)
	FIELD(GPIO_ODCR0T2, P21, 21, 1)
	FIELD(GPIO_ODCR0T2, P22, 22, 1)
	FIELD(GPIO_ODCR0T2, P23, 23, 1)
	FIELD(GPIO_ODCR0T2, P24, 24, 1)
	FIELD(GPIO_ODCR0T2, P25, 25, 1)
	FIELD(GPIO_ODCR0T2, P26, 26, 1)
	FIELD(GPIO_ODCR0T2, P27, 27, 1)
	FIELD(GPIO_ODCR0T2, P28, 28, 1)
	FIELD(GPIO_ODCR0T2, P29, 29, 1)
	FIELD(GPIO_ODCR0T2, P30, 30, 1)
	FIELD(GPIO_ODCR0T2, P31, 31, 1)
REG32(GPIO_ODCR1C0, 0x118)
	FIELD(GPIO_ODCR1C0, P0, 0, 1)
	FIELD(GPIO_ODCR1C0, P1, 1, 1)
	FIELD(GPIO_ODCR1C0, P2, 2, 1)
	FIELD(GPIO_ODCR1C0, P3, 3, 1)
	FIELD(GPIO_ODCR1C0, P4, 4, 1)
	FIELD(GPIO_ODCR1C0, P5, 5, 1)
	FIELD(GPIO_ODCR1C0, P6, 6, 1)
	FIELD(GPIO_ODCR1C0, P7, 7, 1)
	FIELD(GPIO_ODCR1C0, P8, 8, 1)
	FIELD(GPIO_ODCR1C0, P9, 9, 1)
	FIELD(GPIO_ODCR1C0, P10, 10, 1)
	FIELD(GPIO_ODCR1C0, P11, 11, 1)
	FIELD(GPIO_ODCR1C0, P12, 12, 1)
	FIELD(GPIO_ODCR1C0, P13, 13, 1)
	FIELD(GPIO_ODCR1C0, P14, 14, 1)
	FIELD(GPIO_ODCR1C0, P15, 15, 1)
	FIELD(GPIO_ODCR1C0, P16, 16, 1)
	FIELD(GPIO_ODCR1C0, P17, 17, 1)
	FIELD(GPIO_ODCR1C0, P18, 18, 1)
	FIELD(GPIO_ODCR1C0, P19, 19, 1)
	FIELD(GPIO_ODCR1C0, P20, 20, 1)
	FIELD(GPIO_ODCR1C0, P21, 21, 1)
	FIELD(GPIO_ODCR1C0, P22, 22, 1)
	FIELD(GPIO_ODCR1C0, P23, 23, 1)
	FIELD(GPIO_ODCR1C0, P24, 24, 1)
	FIELD(GPIO_ODCR1C0, P25, 25, 1)
	FIELD(GPIO_ODCR1C0, P26, 26, 1)
	FIELD(GPIO_ODCR1C0, P27, 27, 1)
	FIELD(GPIO_ODCR1C0, P28, 28, 1)
	FIELD(GPIO_ODCR1C0, P29, 29, 1)
	FIELD(GPIO_ODCR1C0, P30, 30, 1)
	FIELD(GPIO_ODCR1C0, P31, 31, 1)
REG32(GPIO_ODCR1C1, 0x318)
	FIELD(GPIO_ODCR1C1, P0, 0, 1)
	FIELD(GPIO_ODCR1C1, P1, 1, 1)
	FIELD(GPIO_ODCR1C1, P2, 2, 1)
	FIELD(GPIO_ODCR1C1, P3, 3, 1)
	FIELD(GPIO_ODCR1C1, P4, 4, 1)
	FIELD(GPIO_ODCR1C1, P5, 5, 1)
	FIELD(GPIO_ODCR1C1, P6, 6, 1)
	FIELD(GPIO_ODCR1C1, P7, 7, 1)
	FIELD(GPIO_ODCR1C1, P8, 8, 1)
	FIELD(GPIO_ODCR1C1, P9, 9, 1)
	FIELD(GPIO_ODCR1C1, P10, 10, 1)
	FIELD(GPIO_ODCR1C1, P11, 11, 1)
	FIELD(GPIO_ODCR1C1, P12, 12, 1)
	FIELD(GPIO_ODCR1C1, P13, 13, 1)
	FIELD(GPIO_ODCR1C1, P14, 14, 1)
	FIELD(GPIO_ODCR1C1, P15, 15, 1)
	FIELD(GPIO_ODCR1C1, P16, 16, 1)
	FIELD(GPIO_ODCR1C1, P17, 17, 1)
	FIELD(GPIO_ODCR1C1, P18, 18, 1)
	FIELD(GPIO_ODCR1C1, P19, 19, 1)
	FIELD(GPIO_ODCR1C1, P20, 20, 1)
	FIELD(GPIO_ODCR1C1, P21, 21, 1)
	FIELD(GPIO_ODCR1C1, P22, 22, 1)
	FIELD(GPIO_ODCR1C1, P23, 23, 1)
	FIELD(GPIO_ODCR1C1, P24, 24, 1)
	FIELD(GPIO_ODCR1C1, P25, 25, 1)
	FIELD(GPIO_ODCR1C1, P26, 26, 1)
	FIELD(GPIO_ODCR1C1, P27, 27, 1)
	FIELD(GPIO_ODCR1C1, P28, 28, 1)
	FIELD(GPIO_ODCR1C1, P29, 29, 1)
	FIELD(GPIO_ODCR1C1, P30, 30, 1)
	FIELD(GPIO_ODCR1C1, P31, 31, 1)
REG32(GPIO_ODCR1C2, 0x518)
	FIELD(GPIO_ODCR1C2, P0, 0, 1)
	FIELD(GPIO_ODCR1C2, P1, 1, 1)
	FIELD(GPIO_ODCR1C2, P2, 2, 1)
	FIELD(GPIO_ODCR1C2, P3, 3, 1)
	FIELD(GPIO_ODCR1C2, P4, 4, 1)
	FIELD(GPIO_ODCR1C2, P5, 5, 1)
	FIELD(GPIO_ODCR1C2, P6, 6, 1)
	FIELD(GPIO_ODCR1C2, P7, 7, 1)
	FIELD(GPIO_ODCR1C2, P8, 8, 1)
	FIELD(GPIO_ODCR1C2, P9, 9, 1)
	FIELD(GPIO_ODCR1C2, P10, 10, 1)
	FIELD(GPIO_ODCR1C2, P11, 11, 1)
	FIELD(GPIO_ODCR1C2, P12, 12, 1)
	FIELD(GPIO_ODCR1C2, P13, 13, 1)
	FIELD(GPIO_ODCR1C2, P14, 14, 1)
	FIELD(GPIO_ODCR1C2, P15, 15, 1)
	FIELD(GPIO_ODCR1C2, P16, 16, 1)
	FIELD(GPIO_ODCR1C2, P17, 17, 1)
	FIELD(GPIO_ODCR1C2, P18, 18, 1)
	FIELD(GPIO_ODCR1C2, P19, 19, 1)
	FIELD(GPIO_ODCR1C2, P20, 20, 1)
	FIELD(GPIO_ODCR1C2, P21, 21, 1)
	FIELD(GPIO_ODCR1C2, P22, 22, 1)
	FIELD(GPIO_ODCR1C2, P23, 23, 1)
	FIELD(GPIO_ODCR1C2, P24, 24, 1)
	FIELD(GPIO_ODCR1C2, P25, 25, 1)
	FIELD(GPIO_ODCR1C2, P26, 26, 1)
	FIELD(GPIO_ODCR1C2, P27, 27, 1)
	FIELD(GPIO_ODCR1C2, P28, 28, 1)
	FIELD(GPIO_ODCR1C2, P29, 29, 1)
	FIELD(GPIO_ODCR1C2, P30, 30, 1)
	FIELD(GPIO_ODCR1C2, P31, 31, 1)
REG32(GPIO_ODCR1S0, 0x114)
	FIELD(GPIO_ODCR1S0, P0, 0, 1)
	FIELD(GPIO_ODCR1S0, P1, 1, 1)
	FIELD(GPIO_ODCR1S0, P2, 2, 1)
	FIELD(GPIO_ODCR1S0, P3, 3, 1)
	FIELD(GPIO_ODCR1S0, P4, 4, 1)
	FIELD(GPIO_ODCR1S0, P5, 5, 1)
	FIELD(GPIO_ODCR1S0, P6, 6, 1)
	FIELD(GPIO_ODCR1S0, P7, 7, 1)
	FIELD(GPIO_ODCR1S0, P8, 8, 1)
	FIELD(GPIO_ODCR1S0, P9, 9, 1)
	FIELD(GPIO_ODCR1S0, P10, 10, 1)
	FIELD(GPIO_ODCR1S0, P11, 11, 1)
	FIELD(GPIO_ODCR1S0, P12, 12, 1)
	FIELD(GPIO_ODCR1S0, P13, 13, 1)
	FIELD(GPIO_ODCR1S0, P14, 14, 1)
	FIELD(GPIO_ODCR1S0, P15, 15, 1)
	FIELD(GPIO_ODCR1S0, P16, 16, 1)
	FIELD(GPIO_ODCR1S0, P17, 17, 1)
	FIELD(GPIO_ODCR1S0, P18, 18, 1)
	FIELD(GPIO_ODCR1S0, P19, 19, 1)
	FIELD(GPIO_ODCR1S0, P20, 20, 1)
	FIELD(GPIO_ODCR1S0, P21, 21, 1)
	FIELD(GPIO_ODCR1S0, P22, 22, 1)
	FIELD(GPIO_ODCR1S0, P23, 23, 1)
	FIELD(GPIO_ODCR1S0, P24, 24, 1)
	FIELD(GPIO_ODCR1S0, P25, 25, 1)
	FIELD(GPIO_ODCR1S0, P26, 26, 1)
	FIELD(GPIO_ODCR1S0, P27, 27, 1)
	FIELD(GPIO_ODCR1S0, P28, 28, 1)
	FIELD(GPIO_ODCR1S0, P29, 29, 1)
	FIELD(GPIO_ODCR1S0, P30, 30, 1)
	FIELD(GPIO_ODCR1S0, P31, 31, 1)
REG32(GPIO_ODCR1S1, 0x314)
	FIELD(GPIO_ODCR1S1, P0, 0, 1)
	FIELD(GPIO_ODCR1S1, P1, 1, 1)
	FIELD(GPIO_ODCR1S1, P2, 2, 1)
	FIELD(GPIO_ODCR1S1, P3, 3, 1)
	FIELD(GPIO_ODCR1S1, P4, 4, 1)
	FIELD(GPIO_ODCR1S1, P5, 5, 1)
	FIELD(GPIO_ODCR1S1, P6, 6, 1)
	FIELD(GPIO_ODCR1S1, P7, 7, 1)
	FIELD(GPIO_ODCR1S1, P8, 8, 1)
	FIELD(GPIO_ODCR1S1, P9, 9, 1)
	FIELD(GPIO_ODCR1S1, P10, 10, 1)
	FIELD(GPIO_ODCR1S1, P11, 11, 1)
	FIELD(GPIO_ODCR1S1, P12, 12, 1)
	FIELD(GPIO_ODCR1S1, P13, 13, 1)
	FIELD(GPIO_ODCR1S1, P14, 14, 1)
	FIELD(GPIO_ODCR1S1, P15, 15, 1)
	FIELD(GPIO_ODCR1S1, P16, 16, 1)
	FIELD(GPIO_ODCR1S1, P17, 17, 1)
	FIELD(GPIO_ODCR1S1, P18, 18, 1)
	FIELD(GPIO_ODCR1S1, P19, 19, 1)
	FIELD(GPIO_ODCR1S1, P20, 20, 1)
	FIELD(GPIO_ODCR1S1, P21, 21, 1)
	FIELD(GPIO_ODCR1S1, P22, 22, 1)
	FIELD(GPIO_ODCR1S1, P23, 23, 1)
	FIELD(GPIO_ODCR1S1, P24, 24, 1)
	FIELD(GPIO_ODCR1S1, P25, 25, 1)
	FIELD(GPIO_ODCR1S1, P26, 26, 1)
	FIELD(GPIO_ODCR1S1, P27, 27, 1)
	FIELD(GPIO_ODCR1S1, P28, 28, 1)
	FIELD(GPIO_ODCR1S1, P29, 29, 1)
	FIELD(GPIO_ODCR1S1, P30, 30, 1)
	FIELD(GPIO_ODCR1S1, P31, 31, 1)
REG32(GPIO_ODCR1S2, 0x514)
	FIELD(GPIO_ODCR1S2, P0, 0, 1)
	FIELD(GPIO_ODCR1S2, P1, 1, 1)
	FIELD(GPIO_ODCR1S2, P2, 2, 1)
	FIELD(GPIO_ODCR1S2, P3, 3, 1)
	FIELD(GPIO_ODCR1S2, P4, 4, 1)
	FIELD(GPIO_ODCR1S2, P5, 5, 1)
	FIELD(GPIO_ODCR1S2, P6, 6, 1)
	FIELD(GPIO_ODCR1S2, P7, 7, 1)
	FIELD(GPIO_ODCR1S2, P8, 8, 1)
	FIELD(GPIO_ODCR1S2, P9, 9, 1)
	FIELD(GPIO_ODCR1S2, P10, 10, 1)
	FIELD(GPIO_ODCR1S2, P11, 11, 1)
	FIELD(GPIO_ODCR1S2, P12, 12, 1)
	FIELD(GPIO_ODCR1S2, P13, 13, 1)
	FIELD(GPIO_ODCR1S2, P14, 14, 1)
	FIELD(GPIO_ODCR1S2, P15, 15, 1)
	FIELD(GPIO_ODCR1S2, P16, 16, 1)
	FIELD(GPIO_ODCR1S2, P17, 17, 1)
	FIELD(GPIO_ODCR1S2, P18, 18, 1)
	FIELD(GPIO_ODCR1S2, P19, 19, 1)
	FIELD(GPIO_ODCR1S2, P20, 20, 1)
	FIELD(GPIO_ODCR1S2, P21, 21, 1)
	FIELD(GPIO_ODCR1S2, P22, 22, 1)
	FIELD(GPIO_ODCR1S2, P23, 23, 1)
	FIELD(GPIO_ODCR1S2, P24, 24, 1)
	FIELD(GPIO_ODCR1S2, P25, 25, 1)
	FIELD(GPIO_ODCR1S2, P26, 26, 1)
	FIELD(GPIO_ODCR1S2, P27, 27, 1)
	FIELD(GPIO_ODCR1S2, P28, 28, 1)
	FIELD(GPIO_ODCR1S2, P29, 29, 1)
	FIELD(GPIO_ODCR1S2, P30, 30, 1)
	FIELD(GPIO_ODCR1S2, P31, 31, 1)
REG32(GPIO_ODCR1T0, 0x11c)
	FIELD(GPIO_ODCR1T0, P0, 0, 1)
	FIELD(GPIO_ODCR1T0, P1, 1, 1)
	FIELD(GPIO_ODCR1T0, P2, 2, 1)
	FIELD(GPIO_ODCR1T0, P3, 3, 1)
	FIELD(GPIO_ODCR1T0, P4, 4, 1)
	FIELD(GPIO_ODCR1T0, P5, 5, 1)
	FIELD(GPIO_ODCR1T0, P6, 6, 1)
	FIELD(GPIO_ODCR1T0, P7, 7, 1)
	FIELD(GPIO_ODCR1T0, P8, 8, 1)
	FIELD(GPIO_ODCR1T0, P9, 9, 1)
	FIELD(GPIO_ODCR1T0, P10, 10, 1)
	FIELD(GPIO_ODCR1T0, P11, 11, 1)
	FIELD(GPIO_ODCR1T0, P12, 12, 1)
	FIELD(GPIO_ODCR1T0, P13, 13, 1)
	FIELD(GPIO_ODCR1T0, P14, 14, 1)
	FIELD(GPIO_ODCR1T0, P15, 15, 1)
	FIELD(GPIO_ODCR1T0, P16, 16, 1)
	FIELD(GPIO_ODCR1T0, P17, 17, 1)
	FIELD(GPIO_ODCR1T0, P18, 18, 1)
	FIELD(GPIO_ODCR1T0, P19, 19, 1)
	FIELD(GPIO_ODCR1T0, P20, 20, 1)
	FIELD(GPIO_ODCR1T0, P21, 21, 1)
	FIELD(GPIO_ODCR1T0, P22, 22, 1)
	FIELD(GPIO_ODCR1T0, P23, 23, 1)
	FIELD(GPIO_ODCR1T0, P24, 24, 1)
	FIELD(GPIO_ODCR1T0, P25, 25, 1)
	FIELD(GPIO_ODCR1T0, P26, 26, 1)
	FIELD(GPIO_ODCR1T0, P27, 27, 1)
	FIELD(GPIO_ODCR1T0, P28, 28, 1)
	FIELD(GPIO_ODCR1T0, P29, 29, 1)
	FIELD(GPIO_ODCR1T0, P30, 30, 1)
	FIELD(GPIO_ODCR1T0, P31, 31, 1)
REG32(GPIO_ODCR1T1, 0x31c)
	FIELD(GPIO_ODCR1T1, P0, 0, 1)
	FIELD(GPIO_ODCR1T1, P1, 1, 1)
	FIELD(GPIO_ODCR1T1, P2, 2, 1)
	FIELD(GPIO_ODCR1T1, P3, 3, 1)
	FIELD(GPIO_ODCR1T1, P4, 4, 1)
	FIELD(GPIO_ODCR1T1, P5, 5, 1)
	FIELD(GPIO_ODCR1T1, P6, 6, 1)
	FIELD(GPIO_ODCR1T1, P7, 7, 1)
	FIELD(GPIO_ODCR1T1, P8, 8, 1)
	FIELD(GPIO_ODCR1T1, P9, 9, 1)
	FIELD(GPIO_ODCR1T1, P10, 10, 1)
	FIELD(GPIO_ODCR1T1, P11, 11, 1)
	FIELD(GPIO_ODCR1T1, P12, 12, 1)
	FIELD(GPIO_ODCR1T1, P13, 13, 1)
	FIELD(GPIO_ODCR1T1, P14, 14, 1)
	FIELD(GPIO_ODCR1T1, P15, 15, 1)
	FIELD(GPIO_ODCR1T1, P16, 16, 1)
	FIELD(GPIO_ODCR1T1, P17, 17, 1)
	FIELD(GPIO_ODCR1T1, P18, 18, 1)
	FIELD(GPIO_ODCR1T1, P19, 19, 1)
	FIELD(GPIO_ODCR1T1, P20, 20, 1)
	FIELD(GPIO_ODCR1T1, P21, 21, 1)
	FIELD(GPIO_ODCR1T1, P22, 22, 1)
	FIELD(GPIO_ODCR1T1, P23, 23, 1)
	FIELD(GPIO_ODCR1T1, P24, 24, 1)
	FIELD(GPIO_ODCR1T1, P25, 25, 1)
	FIELD(GPIO_ODCR1T1, P26, 26, 1)
	FIELD(GPIO_ODCR1T1, P27, 27, 1)
	FIELD(GPIO_ODCR1T1, P28, 28, 1)
	FIELD(GPIO_ODCR1T1, P29, 29, 1)
	FIELD(GPIO_ODCR1T1, P30, 30, 1)
	FIELD(GPIO_ODCR1T1, P31, 31, 1)
REG32(GPIO_ODCR1T2, 0x51c)
	FIELD(GPIO_ODCR1T2, P0, 0, 1)
	FIELD(GPIO_ODCR1T2, P1, 1, 1)
	FIELD(GPIO_ODCR1T2, P2, 2, 1)
	FIELD(GPIO_ODCR1T2, P3, 3, 1)
	FIELD(GPIO_ODCR1T2, P4, 4, 1)
	FIELD(GPIO_ODCR1T2, P5, 5, 1)
	FIELD(GPIO_ODCR1T2, P6, 6, 1)
	FIELD(GPIO_ODCR1T2, P7, 7, 1)
	FIELD(GPIO_ODCR1T2, P8, 8, 1)
	FIELD(GPIO_ODCR1T2, P9, 9, 1)
	FIELD(GPIO_ODCR1T2, P10, 10, 1)
	FIELD(GPIO_ODCR1T2, P11, 11, 1)
	FIELD(GPIO_ODCR1T2, P12, 12, 1)
	FIELD(GPIO_ODCR1T2, P13, 13, 1)
	FIELD(GPIO_ODCR1T2, P14, 14, 1)
	FIELD(GPIO_ODCR1T2, P15, 15, 1)
	FIELD(GPIO_ODCR1T2, P16, 16, 1)
	FIELD(GPIO_ODCR1T2, P17, 17, 1)
	FIELD(GPIO_ODCR1T2, P18, 18, 1)
	FIELD(GPIO_ODCR1T2, P19, 19, 1)
	FIELD(GPIO_ODCR1T2, P20, 20, 1)
	FIELD(GPIO_ODCR1T2, P21, 21, 1)
	FIELD(GPIO_ODCR1T2, P22, 22, 1)
	FIELD(GPIO_ODCR1T2, P23, 23, 1)
	FIELD(GPIO_ODCR1T2, P24, 24, 1)
	FIELD(GPIO_ODCR1T2, P25, 25, 1)
	FIELD(GPIO_ODCR1T2, P26, 26, 1)
	FIELD(GPIO_ODCR1T2, P27, 27, 1)
	FIELD(GPIO_ODCR1T2, P28, 28, 1)
	FIELD(GPIO_ODCR1T2, P29, 29, 1)
	FIELD(GPIO_ODCR1T2, P30, 30, 1)
	FIELD(GPIO_ODCR1T2, P31, 31, 1)
REG32(GPIO_ODCR00, 0x100)
	FIELD(GPIO_ODCR00, P0, 0, 1)
	FIELD(GPIO_ODCR00, P1, 1, 1)
	FIELD(GPIO_ODCR00, P2, 2, 1)
	FIELD(GPIO_ODCR00, P3, 3, 1)
	FIELD(GPIO_ODCR00, P4, 4, 1)
	FIELD(GPIO_ODCR00, P5, 5, 1)
	FIELD(GPIO_ODCR00, P6, 6, 1)
	FIELD(GPIO_ODCR00, P7, 7, 1)
	FIELD(GPIO_ODCR00, P8, 8, 1)
	FIELD(GPIO_ODCR00, P9, 9, 1)
	FIELD(GPIO_ODCR00, P10, 10, 1)
	FIELD(GPIO_ODCR00, P11, 11, 1)
	FIELD(GPIO_ODCR00, P12, 12, 1)
	FIELD(GPIO_ODCR00, P13, 13, 1)
	FIELD(GPIO_ODCR00, P14, 14, 1)
	FIELD(GPIO_ODCR00, P15, 15, 1)
	FIELD(GPIO_ODCR00, P16, 16, 1)
	FIELD(GPIO_ODCR00, P17, 17, 1)
	FIELD(GPIO_ODCR00, P18, 18, 1)
	FIELD(GPIO_ODCR00, P19, 19, 1)
	FIELD(GPIO_ODCR00, P20, 20, 1)
	FIELD(GPIO_ODCR00, P21, 21, 1)
	FIELD(GPIO_ODCR00, P22, 22, 1)
	FIELD(GPIO_ODCR00, P23, 23, 1)
	FIELD(GPIO_ODCR00, P24, 24, 1)
	FIELD(GPIO_ODCR00, P25, 25, 1)
	FIELD(GPIO_ODCR00, P26, 26, 1)
	FIELD(GPIO_ODCR00, P27, 27, 1)
	FIELD(GPIO_ODCR00, P28, 28, 1)
	FIELD(GPIO_ODCR00, P29, 29, 1)
	FIELD(GPIO_ODCR00, P30, 30, 1)
	FIELD(GPIO_ODCR00, P31, 31, 1)
REG32(GPIO_ODCR01, 0x300)
	FIELD(GPIO_ODCR01, P0, 0, 1)
	FIELD(GPIO_ODCR01, P1, 1, 1)
	FIELD(GPIO_ODCR01, P2, 2, 1)
	FIELD(GPIO_ODCR01, P3, 3, 1)
	FIELD(GPIO_ODCR01, P4, 4, 1)
	FIELD(GPIO_ODCR01, P5, 5, 1)
	FIELD(GPIO_ODCR01, P6, 6, 1)
	FIELD(GPIO_ODCR01, P7, 7, 1)
	FIELD(GPIO_ODCR01, P8, 8, 1)
	FIELD(GPIO_ODCR01, P9, 9, 1)
	FIELD(GPIO_ODCR01, P10, 10, 1)
	FIELD(GPIO_ODCR01, P11, 11, 1)
	FIELD(GPIO_ODCR01, P12, 12, 1)
	FIELD(GPIO_ODCR01, P13, 13, 1)
	FIELD(GPIO_ODCR01, P14, 14, 1)
	FIELD(GPIO_ODCR01, P15, 15, 1)
	FIELD(GPIO_ODCR01, P16, 16, 1)
	FIELD(GPIO_ODCR01, P17, 17, 1)
	FIELD(GPIO_ODCR01, P18, 18, 1)
	FIELD(GPIO_ODCR01, P19, 19, 1)
	FIELD(GPIO_ODCR01, P20, 20, 1)
	FIELD(GPIO_ODCR01, P21, 21, 1)
	FIELD(GPIO_ODCR01, P22, 22, 1)
	FIELD(GPIO_ODCR01, P23, 23, 1)
	FIELD(GPIO_ODCR01, P24, 24, 1)
	FIELD(GPIO_ODCR01, P25, 25, 1)
	FIELD(GPIO_ODCR01, P26, 26, 1)
	FIELD(GPIO_ODCR01, P27, 27, 1)
	FIELD(GPIO_ODCR01, P28, 28, 1)
	FIELD(GPIO_ODCR01, P29, 29, 1)
	FIELD(GPIO_ODCR01, P30, 30, 1)
	FIELD(GPIO_ODCR01, P31, 31, 1)
REG32(GPIO_ODCR02, 0x500)
	FIELD(GPIO_ODCR02, P0, 0, 1)
	FIELD(GPIO_ODCR02, P1, 1, 1)
	FIELD(GPIO_ODCR02, P2, 2, 1)
	FIELD(GPIO_ODCR02, P3, 3, 1)
	FIELD(GPIO_ODCR02, P4, 4, 1)
	FIELD(GPIO_ODCR02, P5, 5, 1)
	FIELD(GPIO_ODCR02, P6, 6, 1)
	FIELD(GPIO_ODCR02, P7, 7, 1)
	FIELD(GPIO_ODCR02, P8, 8, 1)
	FIELD(GPIO_ODCR02, P9, 9, 1)
	FIELD(GPIO_ODCR02, P10, 10, 1)
	FIELD(GPIO_ODCR02, P11, 11, 1)
	FIELD(GPIO_ODCR02, P12, 12, 1)
	FIELD(GPIO_ODCR02, P13, 13, 1)
	FIELD(GPIO_ODCR02, P14, 14, 1)
	FIELD(GPIO_ODCR02, P15, 15, 1)
	FIELD(GPIO_ODCR02, P16, 16, 1)
	FIELD(GPIO_ODCR02, P17, 17, 1)
	FIELD(GPIO_ODCR02, P18, 18, 1)
	FIELD(GPIO_ODCR02, P19, 19, 1)
	FIELD(GPIO_ODCR02, P20, 20, 1)
	FIELD(GPIO_ODCR02, P21, 21, 1)
	FIELD(GPIO_ODCR02, P22, 22, 1)
	FIELD(GPIO_ODCR02, P23, 23, 1)
	FIELD(GPIO_ODCR02, P24, 24, 1)
	FIELD(GPIO_ODCR02, P25, 25, 1)
	FIELD(GPIO_ODCR02, P26, 26, 1)
	FIELD(GPIO_ODCR02, P27, 27, 1)
	FIELD(GPIO_ODCR02, P28, 28, 1)
	FIELD(GPIO_ODCR02, P29, 29, 1)
	FIELD(GPIO_ODCR02, P30, 30, 1)
	FIELD(GPIO_ODCR02, P31, 31, 1)
REG32(GPIO_ODCR10, 0x110)
	FIELD(GPIO_ODCR10, P0, 0, 1)
	FIELD(GPIO_ODCR10, P1, 1, 1)
	FIELD(GPIO_ODCR10, P2, 2, 1)
	FIELD(GPIO_ODCR10, P3, 3, 1)
	FIELD(GPIO_ODCR10, P4, 4, 1)
	FIELD(GPIO_ODCR10, P5, 5, 1)
	FIELD(GPIO_ODCR10, P6, 6, 1)
	FIELD(GPIO_ODCR10, P7, 7, 1)
	FIELD(GPIO_ODCR10, P8, 8, 1)
	FIELD(GPIO_ODCR10, P9, 9, 1)
	FIELD(GPIO_ODCR10, P10, 10, 1)
	FIELD(GPIO_ODCR10, P11, 11, 1)
	FIELD(GPIO_ODCR10, P12, 12, 1)
	FIELD(GPIO_ODCR10, P13, 13, 1)
	FIELD(GPIO_ODCR10, P14, 14, 1)
	FIELD(GPIO_ODCR10, P15, 15, 1)
	FIELD(GPIO_ODCR10, P16, 16, 1)
	FIELD(GPIO_ODCR10, P17, 17, 1)
	FIELD(GPIO_ODCR10, P18, 18, 1)
	FIELD(GPIO_ODCR10, P19, 19, 1)
	FIELD(GPIO_ODCR10, P20, 20, 1)
	FIELD(GPIO_ODCR10, P21, 21, 1)
	FIELD(GPIO_ODCR10, P22, 22, 1)
	FIELD(GPIO_ODCR10, P23, 23, 1)
	FIELD(GPIO_ODCR10, P24, 24, 1)
	FIELD(GPIO_ODCR10, P25, 25, 1)
	FIELD(GPIO_ODCR10, P26, 26, 1)
	FIELD(GPIO_ODCR10, P27, 27, 1)
	FIELD(GPIO_ODCR10, P28, 28, 1)
	FIELD(GPIO_ODCR10, P29, 29, 1)
	FIELD(GPIO_ODCR10, P30, 30, 1)
	FIELD(GPIO_ODCR10, P31, 31, 1)
REG32(GPIO_ODCR11, 0x310)
	FIELD(GPIO_ODCR11, P0, 0, 1)
	FIELD(GPIO_ODCR11, P1, 1, 1)
	FIELD(GPIO_ODCR11, P2, 2, 1)
	FIELD(GPIO_ODCR11, P3, 3, 1)
	FIELD(GPIO_ODCR11, P4, 4, 1)
	FIELD(GPIO_ODCR11, P5, 5, 1)
	FIELD(GPIO_ODCR11, P6, 6, 1)
	FIELD(GPIO_ODCR11, P7, 7, 1)
	FIELD(GPIO_ODCR11, P8, 8, 1)
	FIELD(GPIO_ODCR11, P9, 9, 1)
	FIELD(GPIO_ODCR11, P10, 10, 1)
	FIELD(GPIO_ODCR11, P11, 11, 1)
	FIELD(GPIO_ODCR11, P12, 12, 1)
	FIELD(GPIO_ODCR11, P13, 13, 1)
	FIELD(GPIO_ODCR11, P14, 14, 1)
	FIELD(GPIO_ODCR11, P15, 15, 1)
	FIELD(GPIO_ODCR11, P16, 16, 1)
	FIELD(GPIO_ODCR11, P17, 17, 1)
	FIELD(GPIO_ODCR11, P18, 18, 1)
	FIELD(GPIO_ODCR11, P19, 19, 1)
	FIELD(GPIO_ODCR11, P20, 20, 1)
	FIELD(GPIO_ODCR11, P21, 21, 1)
	FIELD(GPIO_ODCR11, P22, 22, 1)
	FIELD(GPIO_ODCR11, P23, 23, 1)
	FIELD(GPIO_ODCR11, P24, 24, 1)
	FIELD(GPIO_ODCR11, P25, 25, 1)
	FIELD(GPIO_ODCR11, P26, 26, 1)
	FIELD(GPIO_ODCR11, P27, 27, 1)
	FIELD(GPIO_ODCR11, P28, 28, 1)
	FIELD(GPIO_ODCR11, P29, 29, 1)
	FIELD(GPIO_ODCR11, P30, 30, 1)
	FIELD(GPIO_ODCR11, P31, 31, 1)
REG32(GPIO_ODCR12, 0x510)
	FIELD(GPIO_ODCR12, P0, 0, 1)
	FIELD(GPIO_ODCR12, P1, 1, 1)
	FIELD(GPIO_ODCR12, P2, 2, 1)
	FIELD(GPIO_ODCR12, P3, 3, 1)
	FIELD(GPIO_ODCR12, P4, 4, 1)
	FIELD(GPIO_ODCR12, P5, 5, 1)
	FIELD(GPIO_ODCR12, P6, 6, 1)
	FIELD(GPIO_ODCR12, P7, 7, 1)
	FIELD(GPIO_ODCR12, P8, 8, 1)
	FIELD(GPIO_ODCR12, P9, 9, 1)
	FIELD(GPIO_ODCR12, P10, 10, 1)
	FIELD(GPIO_ODCR12, P11, 11, 1)
	FIELD(GPIO_ODCR12, P12, 12, 1)
	FIELD(GPIO_ODCR12, P13, 13, 1)
	FIELD(GPIO_ODCR12, P14, 14, 1)
	FIELD(GPIO_ODCR12, P15, 15, 1)
	FIELD(GPIO_ODCR12, P16, 16, 1)
	FIELD(GPIO_ODCR12, P17, 17, 1)
	FIELD(GPIO_ODCR12, P18, 18, 1)
	FIELD(GPIO_ODCR12, P19, 19, 1)
	FIELD(GPIO_ODCR12, P20, 20, 1)
	FIELD(GPIO_ODCR12, P21, 21, 1)
	FIELD(GPIO_ODCR12, P22, 22, 1)
	FIELD(GPIO_ODCR12, P23, 23, 1)
	FIELD(GPIO_ODCR12, P24, 24, 1)
	FIELD(GPIO_ODCR12, P25, 25, 1)
	FIELD(GPIO_ODCR12, P26, 26, 1)
	FIELD(GPIO_ODCR12, P27, 27, 1)
	FIELD(GPIO_ODCR12, P28, 28, 1)
	FIELD(GPIO_ODCR12, P29, 29, 1)
	FIELD(GPIO_ODCR12, P30, 30, 1)
	FIELD(GPIO_ODCR12, P31, 31, 1)
REG32(GPIO_ODERC0, 0x48)
	FIELD(GPIO_ODERC0, P0, 0, 1)
	FIELD(GPIO_ODERC0, P1, 1, 1)
	FIELD(GPIO_ODERC0, P2, 2, 1)
	FIELD(GPIO_ODERC0, P3, 3, 1)
	FIELD(GPIO_ODERC0, P4, 4, 1)
	FIELD(GPIO_ODERC0, P5, 5, 1)
	FIELD(GPIO_ODERC0, P6, 6, 1)
	FIELD(GPIO_ODERC0, P7, 7, 1)
	FIELD(GPIO_ODERC0, P8, 8, 1)
	FIELD(GPIO_ODERC0, P9, 9, 1)
	FIELD(GPIO_ODERC0, P10, 10, 1)
	FIELD(GPIO_ODERC0, P11, 11, 1)
	FIELD(GPIO_ODERC0, P12, 12, 1)
	FIELD(GPIO_ODERC0, P13, 13, 1)
	FIELD(GPIO_ODERC0, P14, 14, 1)
	FIELD(GPIO_ODERC0, P15, 15, 1)
	FIELD(GPIO_ODERC0, P16, 16, 1)
	FIELD(GPIO_ODERC0, P17, 17, 1)
	FIELD(GPIO_ODERC0, P18, 18, 1)
	FIELD(GPIO_ODERC0, P19, 19, 1)
	FIELD(GPIO_ODERC0, P20, 20, 1)
	FIELD(GPIO_ODERC0, P21, 21, 1)
	FIELD(GPIO_ODERC0, P22, 22, 1)
	FIELD(GPIO_ODERC0, P23, 23, 1)
	FIELD(GPIO_ODERC0, P24, 24, 1)
	FIELD(GPIO_ODERC0, P25, 25, 1)
	FIELD(GPIO_ODERC0, P26, 26, 1)
	FIELD(GPIO_ODERC0, P27, 27, 1)
	FIELD(GPIO_ODERC0, P28, 28, 1)
	FIELD(GPIO_ODERC0, P29, 29, 1)
	FIELD(GPIO_ODERC0, P30, 30, 1)
	FIELD(GPIO_ODERC0, P31, 31, 1)
REG32(GPIO_ODERC1, 0x248)
	FIELD(GPIO_ODERC1, P0, 0, 1)
	FIELD(GPIO_ODERC1, P1, 1, 1)
	FIELD(GPIO_ODERC1, P2, 2, 1)
	FIELD(GPIO_ODERC1, P3, 3, 1)
	FIELD(GPIO_ODERC1, P4, 4, 1)
	FIELD(GPIO_ODERC1, P5, 5, 1)
	FIELD(GPIO_ODERC1, P6, 6, 1)
	FIELD(GPIO_ODERC1, P7, 7, 1)
	FIELD(GPIO_ODERC1, P8, 8, 1)
	FIELD(GPIO_ODERC1, P9, 9, 1)
	FIELD(GPIO_ODERC1, P10, 10, 1)
	FIELD(GPIO_ODERC1, P11, 11, 1)
	FIELD(GPIO_ODERC1, P12, 12, 1)
	FIELD(GPIO_ODERC1, P13, 13, 1)
	FIELD(GPIO_ODERC1, P14, 14, 1)
	FIELD(GPIO_ODERC1, P15, 15, 1)
	FIELD(GPIO_ODERC1, P16, 16, 1)
	FIELD(GPIO_ODERC1, P17, 17, 1)
	FIELD(GPIO_ODERC1, P18, 18, 1)
	FIELD(GPIO_ODERC1, P19, 19, 1)
	FIELD(GPIO_ODERC1, P20, 20, 1)
	FIELD(GPIO_ODERC1, P21, 21, 1)
	FIELD(GPIO_ODERC1, P22, 22, 1)
	FIELD(GPIO_ODERC1, P23, 23, 1)
	FIELD(GPIO_ODERC1, P24, 24, 1)
	FIELD(GPIO_ODERC1, P25, 25, 1)
	FIELD(GPIO_ODERC1, P26, 26, 1)
	FIELD(GPIO_ODERC1, P27, 27, 1)
	FIELD(GPIO_ODERC1, P28, 28, 1)
	FIELD(GPIO_ODERC1, P29, 29, 1)
	FIELD(GPIO_ODERC1, P30, 30, 1)
	FIELD(GPIO_ODERC1, P31, 31, 1)
REG32(GPIO_ODERC2, 0x448)
	FIELD(GPIO_ODERC2, P0, 0, 1)
	FIELD(GPIO_ODERC2, P1, 1, 1)
	FIELD(GPIO_ODERC2, P2, 2, 1)
	FIELD(GPIO_ODERC2, P3, 3, 1)
	FIELD(GPIO_ODERC2, P4, 4, 1)
	FIELD(GPIO_ODERC2, P5, 5, 1)
	FIELD(GPIO_ODERC2, P6, 6, 1)
	FIELD(GPIO_ODERC2, P7, 7, 1)
	FIELD(GPIO_ODERC2, P8, 8, 1)
	FIELD(GPIO_ODERC2, P9, 9, 1)
	FIELD(GPIO_ODERC2, P10, 10, 1)
	FIELD(GPIO_ODERC2, P11, 11, 1)
	FIELD(GPIO_ODERC2, P12, 12, 1)
	FIELD(GPIO_ODERC2, P13, 13, 1)
	FIELD(GPIO_ODERC2, P14, 14, 1)
	FIELD(GPIO_ODERC2, P15, 15, 1)
	FIELD(GPIO_ODERC2, P16, 16, 1)
	FIELD(GPIO_ODERC2, P17, 17, 1)
	FIELD(GPIO_ODERC2, P18, 18, 1)
	FIELD(GPIO_ODERC2, P19, 19, 1)
	FIELD(GPIO_ODERC2, P20, 20, 1)
	FIELD(GPIO_ODERC2, P21, 21, 1)
	FIELD(GPIO_ODERC2, P22, 22, 1)
	FIELD(GPIO_ODERC2, P23, 23, 1)
	FIELD(GPIO_ODERC2, P24, 24, 1)
	FIELD(GPIO_ODERC2, P25, 25, 1)
	FIELD(GPIO_ODERC2, P26, 26, 1)
	FIELD(GPIO_ODERC2, P27, 27, 1)
	FIELD(GPIO_ODERC2, P28, 28, 1)
	FIELD(GPIO_ODERC2, P29, 29, 1)
	FIELD(GPIO_ODERC2, P30, 30, 1)
	FIELD(GPIO_ODERC2, P31, 31, 1)
REG32(GPIO_ODERS0, 0x44)
	FIELD(GPIO_ODERS0, P0, 0, 1)
	FIELD(GPIO_ODERS0, P1, 1, 1)
	FIELD(GPIO_ODERS0, P2, 2, 1)
	FIELD(GPIO_ODERS0, P3, 3, 1)
	FIELD(GPIO_ODERS0, P4, 4, 1)
	FIELD(GPIO_ODERS0, P5, 5, 1)
	FIELD(GPIO_ODERS0, P6, 6, 1)
	FIELD(GPIO_ODERS0, P7, 7, 1)
	FIELD(GPIO_ODERS0, P8, 8, 1)
	FIELD(GPIO_ODERS0, P9, 9, 1)
	FIELD(GPIO_ODERS0, P10, 10, 1)
	FIELD(GPIO_ODERS0, P11, 11, 1)
	FIELD(GPIO_ODERS0, P12, 12, 1)
	FIELD(GPIO_ODERS0, P13, 13, 1)
	FIELD(GPIO_ODERS0, P14, 14, 1)
	FIELD(GPIO_ODERS0, P15, 15, 1)
	FIELD(GPIO_ODERS0, P16, 16, 1)
	FIELD(GPIO_ODERS0, P17, 17, 1)
	FIELD(GPIO_ODERS0, P18, 18, 1)
	FIELD(GPIO_ODERS0, P19, 19, 1)
	FIELD(GPIO_ODERS0, P20, 20, 1)
	FIELD(GPIO_ODERS0, P21, 21, 1)
	FIELD(GPIO_ODERS0, P22, 22, 1)
	FIELD(GPIO_ODERS0, P23, 23, 1)
	FIELD(GPIO_ODERS0, P24, 24, 1)
	FIELD(GPIO_ODERS0, P25, 25, 1)
	FIELD(GPIO_ODERS0, P26, 26, 1)
	FIELD(GPIO_ODERS0, P27, 27, 1)
	FIELD(GPIO_ODERS0, P28, 28, 1)
	FIELD(GPIO_ODERS0, P29, 29, 1)
	FIELD(GPIO_ODERS0, P30, 30, 1)
	FIELD(GPIO_ODERS0, P31, 31, 1)
REG32(GPIO_ODERS1, 0x244)
	FIELD(GPIO_ODERS1, P0, 0, 1)
	FIELD(GPIO_ODERS1, P1, 1, 1)
	FIELD(GPIO_ODERS1, P2, 2, 1)
	FIELD(GPIO_ODERS1, P3, 3, 1)
	FIELD(GPIO_ODERS1, P4, 4, 1)
	FIELD(GPIO_ODERS1, P5, 5, 1)
	FIELD(GPIO_ODERS1, P6, 6, 1)
	FIELD(GPIO_ODERS1, P7, 7, 1)
	FIELD(GPIO_ODERS1, P8, 8, 1)
	FIELD(GPIO_ODERS1, P9, 9, 1)
	FIELD(GPIO_ODERS1, P10, 10, 1)
	FIELD(GPIO_ODERS1, P11, 11, 1)
	FIELD(GPIO_ODERS1, P12, 12, 1)
	FIELD(GPIO_ODERS1, P13, 13, 1)
	FIELD(GPIO_ODERS1, P14, 14, 1)
	FIELD(GPIO_ODERS1, P15, 15, 1)
	FIELD(GPIO_ODERS1, P16, 16, 1)
	FIELD(GPIO_ODERS1, P17, 17, 1)
	FIELD(GPIO_ODERS1, P18, 18, 1)
	FIELD(GPIO_ODERS1, P19, 19, 1)
	FIELD(GPIO_ODERS1, P20, 20, 1)
	FIELD(GPIO_ODERS1, P21, 21, 1)
	FIELD(GPIO_ODERS1, P22, 22, 1)
	FIELD(GPIO_ODERS1, P23, 23, 1)
	FIELD(GPIO_ODERS1, P24, 24, 1)
	FIELD(GPIO_ODERS1, P25, 25, 1)
	FIELD(GPIO_ODERS1, P26, 26, 1)
	FIELD(GPIO_ODERS1, P27, 27, 1)
	FIELD(GPIO_ODERS1, P28, 28, 1)
	FIELD(GPIO_ODERS1, P29, 29, 1)
	FIELD(GPIO_ODERS1, P30, 30, 1)
	FIELD(GPIO_ODERS1, P31, 31, 1)
REG32(GPIO_ODERS2, 0x444)
	FIELD(GPIO_ODERS2, P0, 0, 1)
	FIELD(GPIO_ODERS2, P1, 1, 1)
	FIELD(GPIO_ODERS2, P2, 2, 1)
	FIELD(GPIO_ODERS2, P3, 3, 1)
	FIELD(GPIO_ODERS2, P4, 4, 1)
	FIELD(GPIO_ODERS2, P5, 5, 1)
	FIELD(GPIO_ODERS2, P6, 6, 1)
	FIELD(GPIO_ODERS2, P7, 7, 1)
	FIELD(GPIO_ODERS2, P8, 8, 1)
	FIELD(GPIO_ODERS2, P9, 9, 1)
	FIELD(GPIO_ODERS2, P10, 10, 1)
	FIELD(GPIO_ODERS2, P11, 11, 1)
	FIELD(GPIO_ODERS2, P12, 12, 1)
	FIELD(GPIO_ODERS2, P13, 13, 1)
	FIELD(GPIO_ODERS2, P14, 14, 1)
	FIELD(GPIO_ODERS2, P15, 15, 1)
	FIELD(GPIO_ODERS2, P16, 16, 1)
	FIELD(GPIO_ODERS2, P17, 17, 1)
	FIELD(GPIO_ODERS2, P18, 18, 1)
	FIELD(GPIO_ODERS2, P19, 19, 1)
	FIELD(GPIO_ODERS2, P20, 20, 1)
	FIELD(GPIO_ODERS2, P21, 21, 1)
	FIELD(GPIO_ODERS2, P22, 22, 1)
	FIELD(GPIO_ODERS2, P23, 23, 1)
	FIELD(GPIO_ODERS2, P24, 24, 1)
	FIELD(GPIO_ODERS2, P25, 25, 1)
	FIELD(GPIO_ODERS2, P26, 26, 1)
	FIELD(GPIO_ODERS2, P27, 27, 1)
	FIELD(GPIO_ODERS2, P28, 28, 1)
	FIELD(GPIO_ODERS2, P29, 29, 1)
	FIELD(GPIO_ODERS2, P30, 30, 1)
	FIELD(GPIO_ODERS2, P31, 31, 1)
REG32(GPIO_ODERT0, 0x4c)
	FIELD(GPIO_ODERT0, P0, 0, 1)
	FIELD(GPIO_ODERT0, P1, 1, 1)
	FIELD(GPIO_ODERT0, P2, 2, 1)
	FIELD(GPIO_ODERT0, P3, 3, 1)
	FIELD(GPIO_ODERT0, P4, 4, 1)
	FIELD(GPIO_ODERT0, P5, 5, 1)
	FIELD(GPIO_ODERT0, P6, 6, 1)
	FIELD(GPIO_ODERT0, P7, 7, 1)
	FIELD(GPIO_ODERT0, P8, 8, 1)
	FIELD(GPIO_ODERT0, P9, 9, 1)
	FIELD(GPIO_ODERT0, P10, 10, 1)
	FIELD(GPIO_ODERT0, P11, 11, 1)
	FIELD(GPIO_ODERT0, P12, 12, 1)
	FIELD(GPIO_ODERT0, P13, 13, 1)
	FIELD(GPIO_ODERT0, P14, 14, 1)
	FIELD(GPIO_ODERT0, P15, 15, 1)
	FIELD(GPIO_ODERT0, P16, 16, 1)
	FIELD(GPIO_ODERT0, P17, 17, 1)
	FIELD(GPIO_ODERT0, P18, 18, 1)
	FIELD(GPIO_ODERT0, P19, 19, 1)
	FIELD(GPIO_ODERT0, P20, 20, 1)
	FIELD(GPIO_ODERT0, P21, 21, 1)
	FIELD(GPIO_ODERT0, P22, 22, 1)
	FIELD(GPIO_ODERT0, P23, 23, 1)
	FIELD(GPIO_ODERT0, P24, 24, 1)
	FIELD(GPIO_ODERT0, P25, 25, 1)
	FIELD(GPIO_ODERT0, P26, 26, 1)
	FIELD(GPIO_ODERT0, P27, 27, 1)
	FIELD(GPIO_ODERT0, P28, 28, 1)
	FIELD(GPIO_ODERT0, P29, 29, 1)
	FIELD(GPIO_ODERT0, P30, 30, 1)
	FIELD(GPIO_ODERT0, P31, 31, 1)
REG32(GPIO_ODERT1, 0x24c)
	FIELD(GPIO_ODERT1, P0, 0, 1)
	FIELD(GPIO_ODERT1, P1, 1, 1)
	FIELD(GPIO_ODERT1, P2, 2, 1)
	FIELD(GPIO_ODERT1, P3, 3, 1)
	FIELD(GPIO_ODERT1, P4, 4, 1)
	FIELD(GPIO_ODERT1, P5, 5, 1)
	FIELD(GPIO_ODERT1, P6, 6, 1)
	FIELD(GPIO_ODERT1, P7, 7, 1)
	FIELD(GPIO_ODERT1, P8, 8, 1)
	FIELD(GPIO_ODERT1, P9, 9, 1)
	FIELD(GPIO_ODERT1, P10, 10, 1)
	FIELD(GPIO_ODERT1, P11, 11, 1)
	FIELD(GPIO_ODERT1, P12, 12, 1)
	FIELD(GPIO_ODERT1, P13, 13, 1)
	FIELD(GPIO_ODERT1, P14, 14, 1)
	FIELD(GPIO_ODERT1, P15, 15, 1)
	FIELD(GPIO_ODERT1, P16, 16, 1)
	FIELD(GPIO_ODERT1, P17, 17, 1)
	FIELD(GPIO_ODERT1, P18, 18, 1)
	FIELD(GPIO_ODERT1, P19, 19, 1)
	FIELD(GPIO_ODERT1, P20, 20, 1)
	FIELD(GPIO_ODERT1, P21, 21, 1)
	FIELD(GPIO_ODERT1, P22, 22, 1)
	FIELD(GPIO_ODERT1, P23, 23, 1)
	FIELD(GPIO_ODERT1, P24, 24, 1)
	FIELD(GPIO_ODERT1, P25, 25, 1)
	FIELD(GPIO_ODERT1, P26, 26, 1)
	FIELD(GPIO_ODERT1, P27, 27, 1)
	FIELD(GPIO_ODERT1, P28, 28, 1)
	FIELD(GPIO_ODERT1, P29, 29, 1)
	FIELD(GPIO_ODERT1, P30, 30, 1)
	FIELD(GPIO_ODERT1, P31, 31, 1)
REG32(GPIO_ODERT2, 0x44c)
	FIELD(GPIO_ODERT2, P0, 0, 1)
	FIELD(GPIO_ODERT2, P1, 1, 1)
	FIELD(GPIO_ODERT2, P2, 2, 1)
	FIELD(GPIO_ODERT2, P3, 3, 1)
	FIELD(GPIO_ODERT2, P4, 4, 1)
	FIELD(GPIO_ODERT2, P5, 5, 1)
	FIELD(GPIO_ODERT2, P6, 6, 1)
	FIELD(GPIO_ODERT2, P7, 7, 1)
	FIELD(GPIO_ODERT2, P8, 8, 1)
	FIELD(GPIO_ODERT2, P9, 9, 1)
	FIELD(GPIO_ODERT2, P10, 10, 1)
	FIELD(GPIO_ODERT2, P11, 11, 1)
	FIELD(GPIO_ODERT2, P12, 12, 1)
	FIELD(GPIO_ODERT2, P13, 13, 1)
	FIELD(GPIO_ODERT2, P14, 14, 1)
	FIELD(GPIO_ODERT2, P15, 15, 1)
	FIELD(GPIO_ODERT2, P16, 16, 1)
	FIELD(GPIO_ODERT2, P17, 17, 1)
	FIELD(GPIO_ODERT2, P18, 18, 1)
	FIELD(GPIO_ODERT2, P19, 19, 1)
	FIELD(GPIO_ODERT2, P20, 20, 1)
	FIELD(GPIO_ODERT2, P21, 21, 1)
	FIELD(GPIO_ODERT2, P22, 22, 1)
	FIELD(GPIO_ODERT2, P23, 23, 1)
	FIELD(GPIO_ODERT2, P24, 24, 1)
	FIELD(GPIO_ODERT2, P25, 25, 1)
	FIELD(GPIO_ODERT2, P26, 26, 1)
	FIELD(GPIO_ODERT2, P27, 27, 1)
	FIELD(GPIO_ODERT2, P28, 28, 1)
	FIELD(GPIO_ODERT2, P29, 29, 1)
	FIELD(GPIO_ODERT2, P30, 30, 1)
	FIELD(GPIO_ODERT2, P31, 31, 1)
REG32(GPIO_ODER0, 0x40)
	FIELD(GPIO_ODER0, P0, 0, 1)
	FIELD(GPIO_ODER0, P1, 1, 1)
	FIELD(GPIO_ODER0, P2, 2, 1)
	FIELD(GPIO_ODER0, P3, 3, 1)
	FIELD(GPIO_ODER0, P4, 4, 1)
	FIELD(GPIO_ODER0, P5, 5, 1)
	FIELD(GPIO_ODER0, P6, 6, 1)
	FIELD(GPIO_ODER0, P7, 7, 1)
	FIELD(GPIO_ODER0, P8, 8, 1)
	FIELD(GPIO_ODER0, P9, 9, 1)
	FIELD(GPIO_ODER0, P10, 10, 1)
	FIELD(GPIO_ODER0, P11, 11, 1)
	FIELD(GPIO_ODER0, P12, 12, 1)
	FIELD(GPIO_ODER0, P13, 13, 1)
	FIELD(GPIO_ODER0, P14, 14, 1)
	FIELD(GPIO_ODER0, P15, 15, 1)
	FIELD(GPIO_ODER0, P16, 16, 1)
	FIELD(GPIO_ODER0, P17, 17, 1)
	FIELD(GPIO_ODER0, P18, 18, 1)
	FIELD(GPIO_ODER0, P19, 19, 1)
	FIELD(GPIO_ODER0, P20, 20, 1)
	FIELD(GPIO_ODER0, P21, 21, 1)
	FIELD(GPIO_ODER0, P22, 22, 1)
	FIELD(GPIO_ODER0, P23, 23, 1)
	FIELD(GPIO_ODER0, P24, 24, 1)
	FIELD(GPIO_ODER0, P25, 25, 1)
	FIELD(GPIO_ODER0, P26, 26, 1)
	FIELD(GPIO_ODER0, P27, 27, 1)
	FIELD(GPIO_ODER0, P28, 28, 1)
	FIELD(GPIO_ODER0, P29, 29, 1)
	FIELD(GPIO_ODER0, P30, 30, 1)
	FIELD(GPIO_ODER0, P31, 31, 1)
REG32(GPIO_ODER1, 0x240)
	FIELD(GPIO_ODER1, P0, 0, 1)
	FIELD(GPIO_ODER1, P1, 1, 1)
	FIELD(GPIO_ODER1, P2, 2, 1)
	FIELD(GPIO_ODER1, P3, 3, 1)
	FIELD(GPIO_ODER1, P4, 4, 1)
	FIELD(GPIO_ODER1, P5, 5, 1)
	FIELD(GPIO_ODER1, P6, 6, 1)
	FIELD(GPIO_ODER1, P7, 7, 1)
	FIELD(GPIO_ODER1, P8, 8, 1)
	FIELD(GPIO_ODER1, P9, 9, 1)
	FIELD(GPIO_ODER1, P10, 10, 1)
	FIELD(GPIO_ODER1, P11, 11, 1)
	FIELD(GPIO_ODER1, P12, 12, 1)
	FIELD(GPIO_ODER1, P13, 13, 1)
	FIELD(GPIO_ODER1, P14, 14, 1)
	FIELD(GPIO_ODER1, P15, 15, 1)
	FIELD(GPIO_ODER1, P16, 16, 1)
	FIELD(GPIO_ODER1, P17, 17, 1)
	FIELD(GPIO_ODER1, P18, 18, 1)
	FIELD(GPIO_ODER1, P19, 19, 1)
	FIELD(GPIO_ODER1, P20, 20, 1)
	FIELD(GPIO_ODER1, P21, 21, 1)
	FIELD(GPIO_ODER1, P22, 22, 1)
	FIELD(GPIO_ODER1, P23, 23, 1)
	FIELD(GPIO_ODER1, P24, 24, 1)
	FIELD(GPIO_ODER1, P25, 25, 1)
	FIELD(GPIO_ODER1, P26, 26, 1)
	FIELD(GPIO_ODER1, P27, 27, 1)
	FIELD(GPIO_ODER1, P28, 28, 1)
	FIELD(GPIO_ODER1, P29, 29, 1)
	FIELD(GPIO_ODER1, P30, 30, 1)
	FIELD(GPIO_ODER1, P31, 31, 1)
REG32(GPIO_ODER2, 0x440)
	FIELD(GPIO_ODER2, P0, 0, 1)
	FIELD(GPIO_ODER2, P1, 1, 1)
	FIELD(GPIO_ODER2, P2, 2, 1)
	FIELD(GPIO_ODER2, P3, 3, 1)
	FIELD(GPIO_ODER2, P4, 4, 1)
	FIELD(GPIO_ODER2, P5, 5, 1)
	FIELD(GPIO_ODER2, P6, 6, 1)
	FIELD(GPIO_ODER2, P7, 7, 1)
	FIELD(GPIO_ODER2, P8, 8, 1)
	FIELD(GPIO_ODER2, P9, 9, 1)
	FIELD(GPIO_ODER2, P10, 10, 1)
	FIELD(GPIO_ODER2, P11, 11, 1)
	FIELD(GPIO_ODER2, P12, 12, 1)
	FIELD(GPIO_ODER2, P13, 13, 1)
	FIELD(GPIO_ODER2, P14, 14, 1)
	FIELD(GPIO_ODER2, P15, 15, 1)
	FIELD(GPIO_ODER2, P16, 16, 1)
	FIELD(GPIO_ODER2, P17, 17, 1)
	FIELD(GPIO_ODER2, P18, 18, 1)
	FIELD(GPIO_ODER2, P19, 19, 1)
	FIELD(GPIO_ODER2, P20, 20, 1)
	FIELD(GPIO_ODER2, P21, 21, 1)
	FIELD(GPIO_ODER2, P22, 22, 1)
	FIELD(GPIO_ODER2, P23, 23, 1)
	FIELD(GPIO_ODER2, P24, 24, 1)
	FIELD(GPIO_ODER2, P25, 25, 1)
	FIELD(GPIO_ODER2, P26, 26, 1)
	FIELD(GPIO_ODER2, P27, 27, 1)
	FIELD(GPIO_ODER2, P28, 28, 1)
	FIELD(GPIO_ODER2, P29, 29, 1)
	FIELD(GPIO_ODER2, P30, 30, 1)
	FIELD(GPIO_ODER2, P31, 31, 1)
REG32(GPIO_ODMERC0, 0xe8)
	FIELD(GPIO_ODMERC0, P0, 0, 1)
	FIELD(GPIO_ODMERC0, P1, 1, 1)
	FIELD(GPIO_ODMERC0, P2, 2, 1)
	FIELD(GPIO_ODMERC0, P3, 3, 1)
	FIELD(GPIO_ODMERC0, P4, 4, 1)
	FIELD(GPIO_ODMERC0, P5, 5, 1)
	FIELD(GPIO_ODMERC0, P6, 6, 1)
	FIELD(GPIO_ODMERC0, P7, 7, 1)
	FIELD(GPIO_ODMERC0, P8, 8, 1)
	FIELD(GPIO_ODMERC0, P9, 9, 1)
	FIELD(GPIO_ODMERC0, P10, 10, 1)
	FIELD(GPIO_ODMERC0, P11, 11, 1)
	FIELD(GPIO_ODMERC0, P12, 12, 1)
	FIELD(GPIO_ODMERC0, P13, 13, 1)
	FIELD(GPIO_ODMERC0, P14, 14, 1)
	FIELD(GPIO_ODMERC0, P15, 15, 1)
	FIELD(GPIO_ODMERC0, P16, 16, 1)
	FIELD(GPIO_ODMERC0, P17, 17, 1)
	FIELD(GPIO_ODMERC0, P18, 18, 1)
	FIELD(GPIO_ODMERC0, P19, 19, 1)
	FIELD(GPIO_ODMERC0, P20, 20, 1)
	FIELD(GPIO_ODMERC0, P21, 21, 1)
	FIELD(GPIO_ODMERC0, P22, 22, 1)
	FIELD(GPIO_ODMERC0, P23, 23, 1)
	FIELD(GPIO_ODMERC0, P24, 24, 1)
	FIELD(GPIO_ODMERC0, P25, 25, 1)
	FIELD(GPIO_ODMERC0, P26, 26, 1)
	FIELD(GPIO_ODMERC0, P27, 27, 1)
	FIELD(GPIO_ODMERC0, P28, 28, 1)
	FIELD(GPIO_ODMERC0, P29, 29, 1)
	FIELD(GPIO_ODMERC0, P30, 30, 1)
	FIELD(GPIO_ODMERC0, P31, 31, 1)
REG32(GPIO_ODMERC1, 0x2e8)
	FIELD(GPIO_ODMERC1, P0, 0, 1)
	FIELD(GPIO_ODMERC1, P1, 1, 1)
	FIELD(GPIO_ODMERC1, P2, 2, 1)
	FIELD(GPIO_ODMERC1, P3, 3, 1)
	FIELD(GPIO_ODMERC1, P4, 4, 1)
	FIELD(GPIO_ODMERC1, P5, 5, 1)
	FIELD(GPIO_ODMERC1, P6, 6, 1)
	FIELD(GPIO_ODMERC1, P7, 7, 1)
	FIELD(GPIO_ODMERC1, P8, 8, 1)
	FIELD(GPIO_ODMERC1, P9, 9, 1)
	FIELD(GPIO_ODMERC1, P10, 10, 1)
	FIELD(GPIO_ODMERC1, P11, 11, 1)
	FIELD(GPIO_ODMERC1, P12, 12, 1)
	FIELD(GPIO_ODMERC1, P13, 13, 1)
	FIELD(GPIO_ODMERC1, P14, 14, 1)
	FIELD(GPIO_ODMERC1, P15, 15, 1)
	FIELD(GPIO_ODMERC1, P16, 16, 1)
	FIELD(GPIO_ODMERC1, P17, 17, 1)
	FIELD(GPIO_ODMERC1, P18, 18, 1)
	FIELD(GPIO_ODMERC1, P19, 19, 1)
	FIELD(GPIO_ODMERC1, P20, 20, 1)
	FIELD(GPIO_ODMERC1, P21, 21, 1)
	FIELD(GPIO_ODMERC1, P22, 22, 1)
	FIELD(GPIO_ODMERC1, P23, 23, 1)
	FIELD(GPIO_ODMERC1, P24, 24, 1)
	FIELD(GPIO_ODMERC1, P25, 25, 1)
	FIELD(GPIO_ODMERC1, P26, 26, 1)
	FIELD(GPIO_ODMERC1, P27, 27, 1)
	FIELD(GPIO_ODMERC1, P28, 28, 1)
	FIELD(GPIO_ODMERC1, P29, 29, 1)
	FIELD(GPIO_ODMERC1, P30, 30, 1)
	FIELD(GPIO_ODMERC1, P31, 31, 1)
REG32(GPIO_ODMERC2, 0x4e8)
	FIELD(GPIO_ODMERC2, P0, 0, 1)
	FIELD(GPIO_ODMERC2, P1, 1, 1)
	FIELD(GPIO_ODMERC2, P2, 2, 1)
	FIELD(GPIO_ODMERC2, P3, 3, 1)
	FIELD(GPIO_ODMERC2, P4, 4, 1)
	FIELD(GPIO_ODMERC2, P5, 5, 1)
	FIELD(GPIO_ODMERC2, P6, 6, 1)
	FIELD(GPIO_ODMERC2, P7, 7, 1)
	FIELD(GPIO_ODMERC2, P8, 8, 1)
	FIELD(GPIO_ODMERC2, P9, 9, 1)
	FIELD(GPIO_ODMERC2, P10, 10, 1)
	FIELD(GPIO_ODMERC2, P11, 11, 1)
	FIELD(GPIO_ODMERC2, P12, 12, 1)
	FIELD(GPIO_ODMERC2, P13, 13, 1)
	FIELD(GPIO_ODMERC2, P14, 14, 1)
	FIELD(GPIO_ODMERC2, P15, 15, 1)
	FIELD(GPIO_ODMERC2, P16, 16, 1)
	FIELD(GPIO_ODMERC2, P17, 17, 1)
	FIELD(GPIO_ODMERC2, P18, 18, 1)
	FIELD(GPIO_ODMERC2, P19, 19, 1)
	FIELD(GPIO_ODMERC2, P20, 20, 1)
	FIELD(GPIO_ODMERC2, P21, 21, 1)
	FIELD(GPIO_ODMERC2, P22, 22, 1)
	FIELD(GPIO_ODMERC2, P23, 23, 1)
	FIELD(GPIO_ODMERC2, P24, 24, 1)
	FIELD(GPIO_ODMERC2, P25, 25, 1)
	FIELD(GPIO_ODMERC2, P26, 26, 1)
	FIELD(GPIO_ODMERC2, P27, 27, 1)
	FIELD(GPIO_ODMERC2, P28, 28, 1)
	FIELD(GPIO_ODMERC2, P29, 29, 1)
	FIELD(GPIO_ODMERC2, P30, 30, 1)
	FIELD(GPIO_ODMERC2, P31, 31, 1)
REG32(GPIO_ODMERS0, 0xe4)
	FIELD(GPIO_ODMERS0, P0, 0, 1)
	FIELD(GPIO_ODMERS0, P1, 1, 1)
	FIELD(GPIO_ODMERS0, P2, 2, 1)
	FIELD(GPIO_ODMERS0, P3, 3, 1)
	FIELD(GPIO_ODMERS0, P4, 4, 1)
	FIELD(GPIO_ODMERS0, P5, 5, 1)
	FIELD(GPIO_ODMERS0, P6, 6, 1)
	FIELD(GPIO_ODMERS0, P7, 7, 1)
	FIELD(GPIO_ODMERS0, P8, 8, 1)
	FIELD(GPIO_ODMERS0, P9, 9, 1)
	FIELD(GPIO_ODMERS0, P10, 10, 1)
	FIELD(GPIO_ODMERS0, P11, 11, 1)
	FIELD(GPIO_ODMERS0, P12, 12, 1)
	FIELD(GPIO_ODMERS0, P13, 13, 1)
	FIELD(GPIO_ODMERS0, P14, 14, 1)
	FIELD(GPIO_ODMERS0, P15, 15, 1)
	FIELD(GPIO_ODMERS0, P16, 16, 1)
	FIELD(GPIO_ODMERS0, P17, 17, 1)
	FIELD(GPIO_ODMERS0, P18, 18, 1)
	FIELD(GPIO_ODMERS0, P19, 19, 1)
	FIELD(GPIO_ODMERS0, P20, 20, 1)
	FIELD(GPIO_ODMERS0, P21, 21, 1)
	FIELD(GPIO_ODMERS0, P22, 22, 1)
	FIELD(GPIO_ODMERS0, P23, 23, 1)
	FIELD(GPIO_ODMERS0, P24, 24, 1)
	FIELD(GPIO_ODMERS0, P25, 25, 1)
	FIELD(GPIO_ODMERS0, P26, 26, 1)
	FIELD(GPIO_ODMERS0, P27, 27, 1)
	FIELD(GPIO_ODMERS0, P28, 28, 1)
	FIELD(GPIO_ODMERS0, P29, 29, 1)
	FIELD(GPIO_ODMERS0, P30, 30, 1)
	FIELD(GPIO_ODMERS0, P31, 31, 1)
REG32(GPIO_ODMERS1, 0x2e4)
	FIELD(GPIO_ODMERS1, P0, 0, 1)
	FIELD(GPIO_ODMERS1, P1, 1, 1)
	FIELD(GPIO_ODMERS1, P2, 2, 1)
	FIELD(GPIO_ODMERS1, P3, 3, 1)
	FIELD(GPIO_ODMERS1, P4, 4, 1)
	FIELD(GPIO_ODMERS1, P5, 5, 1)
	FIELD(GPIO_ODMERS1, P6, 6, 1)
	FIELD(GPIO_ODMERS1, P7, 7, 1)
	FIELD(GPIO_ODMERS1, P8, 8, 1)
	FIELD(GPIO_ODMERS1, P9, 9, 1)
	FIELD(GPIO_ODMERS1, P10, 10, 1)
	FIELD(GPIO_ODMERS1, P11, 11, 1)
	FIELD(GPIO_ODMERS1, P12, 12, 1)
	FIELD(GPIO_ODMERS1, P13, 13, 1)
	FIELD(GPIO_ODMERS1, P14, 14, 1)
	FIELD(GPIO_ODMERS1, P15, 15, 1)
	FIELD(GPIO_ODMERS1, P16, 16, 1)
	FIELD(GPIO_ODMERS1, P17, 17, 1)
	FIELD(GPIO_ODMERS1, P18, 18, 1)
	FIELD(GPIO_ODMERS1, P19, 19, 1)
	FIELD(GPIO_ODMERS1, P20, 20, 1)
	FIELD(GPIO_ODMERS1, P21, 21, 1)
	FIELD(GPIO_ODMERS1, P22, 22, 1)
	FIELD(GPIO_ODMERS1, P23, 23, 1)
	FIELD(GPIO_ODMERS1, P24, 24, 1)
	FIELD(GPIO_ODMERS1, P25, 25, 1)
	FIELD(GPIO_ODMERS1, P26, 26, 1)
	FIELD(GPIO_ODMERS1, P27, 27, 1)
	FIELD(GPIO_ODMERS1, P28, 28, 1)
	FIELD(GPIO_ODMERS1, P29, 29, 1)
	FIELD(GPIO_ODMERS1, P30, 30, 1)
	FIELD(GPIO_ODMERS1, P31, 31, 1)
REG32(GPIO_ODMERS2, 0x4e4)
	FIELD(GPIO_ODMERS2, P0, 0, 1)
	FIELD(GPIO_ODMERS2, P1, 1, 1)
	FIELD(GPIO_ODMERS2, P2, 2, 1)
	FIELD(GPIO_ODMERS2, P3, 3, 1)
	FIELD(GPIO_ODMERS2, P4, 4, 1)
	FIELD(GPIO_ODMERS2, P5, 5, 1)
	FIELD(GPIO_ODMERS2, P6, 6, 1)
	FIELD(GPIO_ODMERS2, P7, 7, 1)
	FIELD(GPIO_ODMERS2, P8, 8, 1)
	FIELD(GPIO_ODMERS2, P9, 9, 1)
	FIELD(GPIO_ODMERS2, P10, 10, 1)
	FIELD(GPIO_ODMERS2, P11, 11, 1)
	FIELD(GPIO_ODMERS2, P12, 12, 1)
	FIELD(GPIO_ODMERS2, P13, 13, 1)
	FIELD(GPIO_ODMERS2, P14, 14, 1)
	FIELD(GPIO_ODMERS2, P15, 15, 1)
	FIELD(GPIO_ODMERS2, P16, 16, 1)
	FIELD(GPIO_ODMERS2, P17, 17, 1)
	FIELD(GPIO_ODMERS2, P18, 18, 1)
	FIELD(GPIO_ODMERS2, P19, 19, 1)
	FIELD(GPIO_ODMERS2, P20, 20, 1)
	FIELD(GPIO_ODMERS2, P21, 21, 1)
	FIELD(GPIO_ODMERS2, P22, 22, 1)
	FIELD(GPIO_ODMERS2, P23, 23, 1)
	FIELD(GPIO_ODMERS2, P24, 24, 1)
	FIELD(GPIO_ODMERS2, P25, 25, 1)
	FIELD(GPIO_ODMERS2, P26, 26, 1)
	FIELD(GPIO_ODMERS2, P27, 27, 1)
	FIELD(GPIO_ODMERS2, P28, 28, 1)
	FIELD(GPIO_ODMERS2, P29, 29, 1)
	FIELD(GPIO_ODMERS2, P30, 30, 1)
	FIELD(GPIO_ODMERS2, P31, 31, 1)
REG32(GPIO_ODMERT0, 0xec)
	FIELD(GPIO_ODMERT0, P0, 0, 1)
	FIELD(GPIO_ODMERT0, P1, 1, 1)
	FIELD(GPIO_ODMERT0, P2, 2, 1)
	FIELD(GPIO_ODMERT0, P3, 3, 1)
	FIELD(GPIO_ODMERT0, P4, 4, 1)
	FIELD(GPIO_ODMERT0, P5, 5, 1)
	FIELD(GPIO_ODMERT0, P6, 6, 1)
	FIELD(GPIO_ODMERT0, P7, 7, 1)
	FIELD(GPIO_ODMERT0, P8, 8, 1)
	FIELD(GPIO_ODMERT0, P9, 9, 1)
	FIELD(GPIO_ODMERT0, P10, 10, 1)
	FIELD(GPIO_ODMERT0, P11, 11, 1)
	FIELD(GPIO_ODMERT0, P12, 12, 1)
	FIELD(GPIO_ODMERT0, P13, 13, 1)
	FIELD(GPIO_ODMERT0, P14, 14, 1)
	FIELD(GPIO_ODMERT0, P15, 15, 1)
	FIELD(GPIO_ODMERT0, P16, 16, 1)
	FIELD(GPIO_ODMERT0, P17, 17, 1)
	FIELD(GPIO_ODMERT0, P18, 18, 1)
	FIELD(GPIO_ODMERT0, P19, 19, 1)
	FIELD(GPIO_ODMERT0, P20, 20, 1)
	FIELD(GPIO_ODMERT0, P21, 21, 1)
	FIELD(GPIO_ODMERT0, P22, 22, 1)
	FIELD(GPIO_ODMERT0, P23, 23, 1)
	FIELD(GPIO_ODMERT0, P24, 24, 1)
	FIELD(GPIO_ODMERT0, P25, 25, 1)
	FIELD(GPIO_ODMERT0, P26, 26, 1)
	FIELD(GPIO_ODMERT0, P27, 27, 1)
	FIELD(GPIO_ODMERT0, P28, 28, 1)
	FIELD(GPIO_ODMERT0, P29, 29, 1)
	FIELD(GPIO_ODMERT0, P30, 30, 1)
	FIELD(GPIO_ODMERT0, P31, 31, 1)
REG32(GPIO_ODMERT1, 0x2ec)
	FIELD(GPIO_ODMERT1, P0, 0, 1)
	FIELD(GPIO_ODMERT1, P1, 1, 1)
	FIELD(GPIO_ODMERT1, P2, 2, 1)
	FIELD(GPIO_ODMERT1, P3, 3, 1)
	FIELD(GPIO_ODMERT1, P4, 4, 1)
	FIELD(GPIO_ODMERT1, P5, 5, 1)
	FIELD(GPIO_ODMERT1, P6, 6, 1)
	FIELD(GPIO_ODMERT1, P7, 7, 1)
	FIELD(GPIO_ODMERT1, P8, 8, 1)
	FIELD(GPIO_ODMERT1, P9, 9, 1)
	FIELD(GPIO_ODMERT1, P10, 10, 1)
	FIELD(GPIO_ODMERT1, P11, 11, 1)
	FIELD(GPIO_ODMERT1, P12, 12, 1)
	FIELD(GPIO_ODMERT1, P13, 13, 1)
	FIELD(GPIO_ODMERT1, P14, 14, 1)
	FIELD(GPIO_ODMERT1, P15, 15, 1)
	FIELD(GPIO_ODMERT1, P16, 16, 1)
	FIELD(GPIO_ODMERT1, P17, 17, 1)
	FIELD(GPIO_ODMERT1, P18, 18, 1)
	FIELD(GPIO_ODMERT1, P19, 19, 1)
	FIELD(GPIO_ODMERT1, P20, 20, 1)
	FIELD(GPIO_ODMERT1, P21, 21, 1)
	FIELD(GPIO_ODMERT1, P22, 22, 1)
	FIELD(GPIO_ODMERT1, P23, 23, 1)
	FIELD(GPIO_ODMERT1, P24, 24, 1)
	FIELD(GPIO_ODMERT1, P25, 25, 1)
	FIELD(GPIO_ODMERT1, P26, 26, 1)
	FIELD(GPIO_ODMERT1, P27, 27, 1)
	FIELD(GPIO_ODMERT1, P28, 28, 1)
	FIELD(GPIO_ODMERT1, P29, 29, 1)
	FIELD(GPIO_ODMERT1, P30, 30, 1)
	FIELD(GPIO_ODMERT1, P31, 31, 1)
REG32(GPIO_ODMERT2, 0x4ec)
	FIELD(GPIO_ODMERT2, P0, 0, 1)
	FIELD(GPIO_ODMERT2, P1, 1, 1)
	FIELD(GPIO_ODMERT2, P2, 2, 1)
	FIELD(GPIO_ODMERT2, P3, 3, 1)
	FIELD(GPIO_ODMERT2, P4, 4, 1)
	FIELD(GPIO_ODMERT2, P5, 5, 1)
	FIELD(GPIO_ODMERT2, P6, 6, 1)
	FIELD(GPIO_ODMERT2, P7, 7, 1)
	FIELD(GPIO_ODMERT2, P8, 8, 1)
	FIELD(GPIO_ODMERT2, P9, 9, 1)
	FIELD(GPIO_ODMERT2, P10, 10, 1)
	FIELD(GPIO_ODMERT2, P11, 11, 1)
	FIELD(GPIO_ODMERT2, P12, 12, 1)
	FIELD(GPIO_ODMERT2, P13, 13, 1)
	FIELD(GPIO_ODMERT2, P14, 14, 1)
	FIELD(GPIO_ODMERT2, P15, 15, 1)
	FIELD(GPIO_ODMERT2, P16, 16, 1)
	FIELD(GPIO_ODMERT2, P17, 17, 1)
	FIELD(GPIO_ODMERT2, P18, 18, 1)
	FIELD(GPIO_ODMERT2, P19, 19, 1)
	FIELD(GPIO_ODMERT2, P20, 20, 1)
	FIELD(GPIO_ODMERT2, P21, 21, 1)
	FIELD(GPIO_ODMERT2, P22, 22, 1)
	FIELD(GPIO_ODMERT2, P23, 23, 1)
	FIELD(GPIO_ODMERT2, P24, 24, 1)
	FIELD(GPIO_ODMERT2, P25, 25, 1)
	FIELD(GPIO_ODMERT2, P26, 26, 1)
	FIELD(GPIO_ODMERT2, P27, 27, 1)
	FIELD(GPIO_ODMERT2, P28, 28, 1)
	FIELD(GPIO_ODMERT2, P29, 29, 1)
	FIELD(GPIO_ODMERT2, P30, 30, 1)
	FIELD(GPIO_ODMERT2, P31, 31, 1)
REG32(GPIO_ODMER0, 0xe0)
	FIELD(GPIO_ODMER0, P0, 0, 1)
	FIELD(GPIO_ODMER0, P1, 1, 1)
	FIELD(GPIO_ODMER0, P2, 2, 1)
	FIELD(GPIO_ODMER0, P3, 3, 1)
	FIELD(GPIO_ODMER0, P4, 4, 1)
	FIELD(GPIO_ODMER0, P5, 5, 1)
	FIELD(GPIO_ODMER0, P6, 6, 1)
	FIELD(GPIO_ODMER0, P7, 7, 1)
	FIELD(GPIO_ODMER0, P8, 8, 1)
	FIELD(GPIO_ODMER0, P9, 9, 1)
	FIELD(GPIO_ODMER0, P10, 10, 1)
	FIELD(GPIO_ODMER0, P11, 11, 1)
	FIELD(GPIO_ODMER0, P12, 12, 1)
	FIELD(GPIO_ODMER0, P13, 13, 1)
	FIELD(GPIO_ODMER0, P14, 14, 1)
	FIELD(GPIO_ODMER0, P15, 15, 1)
	FIELD(GPIO_ODMER0, P16, 16, 1)
	FIELD(GPIO_ODMER0, P17, 17, 1)
	FIELD(GPIO_ODMER0, P18, 18, 1)
	FIELD(GPIO_ODMER0, P19, 19, 1)
	FIELD(GPIO_ODMER0, P20, 20, 1)
	FIELD(GPIO_ODMER0, P21, 21, 1)
	FIELD(GPIO_ODMER0, P22, 22, 1)
	FIELD(GPIO_ODMER0, P23, 23, 1)
	FIELD(GPIO_ODMER0, P24, 24, 1)
	FIELD(GPIO_ODMER0, P25, 25, 1)
	FIELD(GPIO_ODMER0, P26, 26, 1)
	FIELD(GPIO_ODMER0, P27, 27, 1)
	FIELD(GPIO_ODMER0, P28, 28, 1)
	FIELD(GPIO_ODMER0, P29, 29, 1)
	FIELD(GPIO_ODMER0, P30, 30, 1)
	FIELD(GPIO_ODMER0, P31, 31, 1)
REG32(GPIO_ODMER1, 0x2e0)
	FIELD(GPIO_ODMER1, P0, 0, 1)
	FIELD(GPIO_ODMER1, P1, 1, 1)
	FIELD(GPIO_ODMER1, P2, 2, 1)
	FIELD(GPIO_ODMER1, P3, 3, 1)
	FIELD(GPIO_ODMER1, P4, 4, 1)
	FIELD(GPIO_ODMER1, P5, 5, 1)
	FIELD(GPIO_ODMER1, P6, 6, 1)
	FIELD(GPIO_ODMER1, P7, 7, 1)
	FIELD(GPIO_ODMER1, P8, 8, 1)
	FIELD(GPIO_ODMER1, P9, 9, 1)
	FIELD(GPIO_ODMER1, P10, 10, 1)
	FIELD(GPIO_ODMER1, P11, 11, 1)
	FIELD(GPIO_ODMER1, P12, 12, 1)
	FIELD(GPIO_ODMER1, P13, 13, 1)
	FIELD(GPIO_ODMER1, P14, 14, 1)
	FIELD(GPIO_ODMER1, P15, 15, 1)
	FIELD(GPIO_ODMER1, P16, 16, 1)
	FIELD(GPIO_ODMER1, P17, 17, 1)
	FIELD(GPIO_ODMER1, P18, 18, 1)
	FIELD(GPIO_ODMER1, P19, 19, 1)
	FIELD(GPIO_ODMER1, P20, 20, 1)
	FIELD(GPIO_ODMER1, P21, 21, 1)
	FIELD(GPIO_ODMER1, P22, 22, 1)
	FIELD(GPIO_ODMER1, P23, 23, 1)
	FIELD(GPIO_ODMER1, P24, 24, 1)
	FIELD(GPIO_ODMER1, P25, 25, 1)
	FIELD(GPIO_ODMER1, P26, 26, 1)
	FIELD(GPIO_ODMER1, P27, 27, 1)
	FIELD(GPIO_ODMER1, P28, 28, 1)
	FIELD(GPIO_ODMER1, P29, 29, 1)
	FIELD(GPIO_ODMER1, P30, 30, 1)
	FIELD(GPIO_ODMER1, P31, 31, 1)
REG32(GPIO_ODMER2, 0x4e0)
	FIELD(GPIO_ODMER2, P0, 0, 1)
	FIELD(GPIO_ODMER2, P1, 1, 1)
	FIELD(GPIO_ODMER2, P2, 2, 1)
	FIELD(GPIO_ODMER2, P3, 3, 1)
	FIELD(GPIO_ODMER2, P4, 4, 1)
	FIELD(GPIO_ODMER2, P5, 5, 1)
	FIELD(GPIO_ODMER2, P6, 6, 1)
	FIELD(GPIO_ODMER2, P7, 7, 1)
	FIELD(GPIO_ODMER2, P8, 8, 1)
	FIELD(GPIO_ODMER2, P9, 9, 1)
	FIELD(GPIO_ODMER2, P10, 10, 1)
	FIELD(GPIO_ODMER2, P11, 11, 1)
	FIELD(GPIO_ODMER2, P12, 12, 1)
	FIELD(GPIO_ODMER2, P13, 13, 1)
	FIELD(GPIO_ODMER2, P14, 14, 1)
	FIELD(GPIO_ODMER2, P15, 15, 1)
	FIELD(GPIO_ODMER2, P16, 16, 1)
	FIELD(GPIO_ODMER2, P17, 17, 1)
	FIELD(GPIO_ODMER2, P18, 18, 1)
	FIELD(GPIO_ODMER2, P19, 19, 1)
	FIELD(GPIO_ODMER2, P20, 20, 1)
	FIELD(GPIO_ODMER2, P21, 21, 1)
	FIELD(GPIO_ODMER2, P22, 22, 1)
	FIELD(GPIO_ODMER2, P23, 23, 1)
	FIELD(GPIO_ODMER2, P24, 24, 1)
	FIELD(GPIO_ODMER2, P25, 25, 1)
	FIELD(GPIO_ODMER2, P26, 26, 1)
	FIELD(GPIO_ODMER2, P27, 27, 1)
	FIELD(GPIO_ODMER2, P28, 28, 1)
	FIELD(GPIO_ODMER2, P29, 29, 1)
	FIELD(GPIO_ODMER2, P30, 30, 1)
	FIELD(GPIO_ODMER2, P31, 31, 1)
REG32(GPIO_OSRR0C0, 0x138)
	FIELD(GPIO_OSRR0C0, P0, 0, 1)
	FIELD(GPIO_OSRR0C0, P1, 1, 1)
	FIELD(GPIO_OSRR0C0, P2, 2, 1)
	FIELD(GPIO_OSRR0C0, P3, 3, 1)
	FIELD(GPIO_OSRR0C0, P4, 4, 1)
	FIELD(GPIO_OSRR0C0, P5, 5, 1)
	FIELD(GPIO_OSRR0C0, P6, 6, 1)
	FIELD(GPIO_OSRR0C0, P7, 7, 1)
	FIELD(GPIO_OSRR0C0, P8, 8, 1)
	FIELD(GPIO_OSRR0C0, P9, 9, 1)
	FIELD(GPIO_OSRR0C0, P10, 10, 1)
	FIELD(GPIO_OSRR0C0, P11, 11, 1)
	FIELD(GPIO_OSRR0C0, P12, 12, 1)
	FIELD(GPIO_OSRR0C0, P13, 13, 1)
	FIELD(GPIO_OSRR0C0, P14, 14, 1)
	FIELD(GPIO_OSRR0C0, P15, 15, 1)
	FIELD(GPIO_OSRR0C0, P16, 16, 1)
	FIELD(GPIO_OSRR0C0, P17, 17, 1)
	FIELD(GPIO_OSRR0C0, P18, 18, 1)
	FIELD(GPIO_OSRR0C0, P19, 19, 1)
	FIELD(GPIO_OSRR0C0, P20, 20, 1)
	FIELD(GPIO_OSRR0C0, P21, 21, 1)
	FIELD(GPIO_OSRR0C0, P22, 22, 1)
	FIELD(GPIO_OSRR0C0, P23, 23, 1)
	FIELD(GPIO_OSRR0C0, P24, 24, 1)
	FIELD(GPIO_OSRR0C0, P25, 25, 1)
	FIELD(GPIO_OSRR0C0, P26, 26, 1)
	FIELD(GPIO_OSRR0C0, P27, 27, 1)
	FIELD(GPIO_OSRR0C0, P28, 28, 1)
	FIELD(GPIO_OSRR0C0, P29, 29, 1)
	FIELD(GPIO_OSRR0C0, P30, 30, 1)
	FIELD(GPIO_OSRR0C0, P31, 31, 1)
REG32(GPIO_OSRR0C1, 0x338)
	FIELD(GPIO_OSRR0C1, P0, 0, 1)
	FIELD(GPIO_OSRR0C1, P1, 1, 1)
	FIELD(GPIO_OSRR0C1, P2, 2, 1)
	FIELD(GPIO_OSRR0C1, P3, 3, 1)
	FIELD(GPIO_OSRR0C1, P4, 4, 1)
	FIELD(GPIO_OSRR0C1, P5, 5, 1)
	FIELD(GPIO_OSRR0C1, P6, 6, 1)
	FIELD(GPIO_OSRR0C1, P7, 7, 1)
	FIELD(GPIO_OSRR0C1, P8, 8, 1)
	FIELD(GPIO_OSRR0C1, P9, 9, 1)
	FIELD(GPIO_OSRR0C1, P10, 10, 1)
	FIELD(GPIO_OSRR0C1, P11, 11, 1)
	FIELD(GPIO_OSRR0C1, P12, 12, 1)
	FIELD(GPIO_OSRR0C1, P13, 13, 1)
	FIELD(GPIO_OSRR0C1, P14, 14, 1)
	FIELD(GPIO_OSRR0C1, P15, 15, 1)
	FIELD(GPIO_OSRR0C1, P16, 16, 1)
	FIELD(GPIO_OSRR0C1, P17, 17, 1)
	FIELD(GPIO_OSRR0C1, P18, 18, 1)
	FIELD(GPIO_OSRR0C1, P19, 19, 1)
	FIELD(GPIO_OSRR0C1, P20, 20, 1)
	FIELD(GPIO_OSRR0C1, P21, 21, 1)
	FIELD(GPIO_OSRR0C1, P22, 22, 1)
	FIELD(GPIO_OSRR0C1, P23, 23, 1)
	FIELD(GPIO_OSRR0C1, P24, 24, 1)
	FIELD(GPIO_OSRR0C1, P25, 25, 1)
	FIELD(GPIO_OSRR0C1, P26, 26, 1)
	FIELD(GPIO_OSRR0C1, P27, 27, 1)
	FIELD(GPIO_OSRR0C1, P28, 28, 1)
	FIELD(GPIO_OSRR0C1, P29, 29, 1)
	FIELD(GPIO_OSRR0C1, P30, 30, 1)
	FIELD(GPIO_OSRR0C1, P31, 31, 1)
REG32(GPIO_OSRR0C2, 0x538)
	FIELD(GPIO_OSRR0C2, P0, 0, 1)
	FIELD(GPIO_OSRR0C2, P1, 1, 1)
	FIELD(GPIO_OSRR0C2, P2, 2, 1)
	FIELD(GPIO_OSRR0C2, P3, 3, 1)
	FIELD(GPIO_OSRR0C2, P4, 4, 1)
	FIELD(GPIO_OSRR0C2, P5, 5, 1)
	FIELD(GPIO_OSRR0C2, P6, 6, 1)
	FIELD(GPIO_OSRR0C2, P7, 7, 1)
	FIELD(GPIO_OSRR0C2, P8, 8, 1)
	FIELD(GPIO_OSRR0C2, P9, 9, 1)
	FIELD(GPIO_OSRR0C2, P10, 10, 1)
	FIELD(GPIO_OSRR0C2, P11, 11, 1)
	FIELD(GPIO_OSRR0C2, P12, 12, 1)
	FIELD(GPIO_OSRR0C2, P13, 13, 1)
	FIELD(GPIO_OSRR0C2, P14, 14, 1)
	FIELD(GPIO_OSRR0C2, P15, 15, 1)
	FIELD(GPIO_OSRR0C2, P16, 16, 1)
	FIELD(GPIO_OSRR0C2, P17, 17, 1)
	FIELD(GPIO_OSRR0C2, P18, 18, 1)
	FIELD(GPIO_OSRR0C2, P19, 19, 1)
	FIELD(GPIO_OSRR0C2, P20, 20, 1)
	FIELD(GPIO_OSRR0C2, P21, 21, 1)
	FIELD(GPIO_OSRR0C2, P22, 22, 1)
	FIELD(GPIO_OSRR0C2, P23, 23, 1)
	FIELD(GPIO_OSRR0C2, P24, 24, 1)
	FIELD(GPIO_OSRR0C2, P25, 25, 1)
	FIELD(GPIO_OSRR0C2, P26, 26, 1)
	FIELD(GPIO_OSRR0C2, P27, 27, 1)
	FIELD(GPIO_OSRR0C2, P28, 28, 1)
	FIELD(GPIO_OSRR0C2, P29, 29, 1)
	FIELD(GPIO_OSRR0C2, P30, 30, 1)
	FIELD(GPIO_OSRR0C2, P31, 31, 1)
REG32(GPIO_OSRR0S0, 0x134)
	FIELD(GPIO_OSRR0S0, P0, 0, 1)
	FIELD(GPIO_OSRR0S0, P1, 1, 1)
	FIELD(GPIO_OSRR0S0, P2, 2, 1)
	FIELD(GPIO_OSRR0S0, P3, 3, 1)
	FIELD(GPIO_OSRR0S0, P4, 4, 1)
	FIELD(GPIO_OSRR0S0, P5, 5, 1)
	FIELD(GPIO_OSRR0S0, P6, 6, 1)
	FIELD(GPIO_OSRR0S0, P7, 7, 1)
	FIELD(GPIO_OSRR0S0, P8, 8, 1)
	FIELD(GPIO_OSRR0S0, P9, 9, 1)
	FIELD(GPIO_OSRR0S0, P10, 10, 1)
	FIELD(GPIO_OSRR0S0, P11, 11, 1)
	FIELD(GPIO_OSRR0S0, P12, 12, 1)
	FIELD(GPIO_OSRR0S0, P13, 13, 1)
	FIELD(GPIO_OSRR0S0, P14, 14, 1)
	FIELD(GPIO_OSRR0S0, P15, 15, 1)
	FIELD(GPIO_OSRR0S0, P16, 16, 1)
	FIELD(GPIO_OSRR0S0, P17, 17, 1)
	FIELD(GPIO_OSRR0S0, P18, 18, 1)
	FIELD(GPIO_OSRR0S0, P19, 19, 1)
	FIELD(GPIO_OSRR0S0, P20, 20, 1)
	FIELD(GPIO_OSRR0S0, P21, 21, 1)
	FIELD(GPIO_OSRR0S0, P22, 22, 1)
	FIELD(GPIO_OSRR0S0, P23, 23, 1)
	FIELD(GPIO_OSRR0S0, P24, 24, 1)
	FIELD(GPIO_OSRR0S0, P25, 25, 1)
	FIELD(GPIO_OSRR0S0, P26, 26, 1)
	FIELD(GPIO_OSRR0S0, P27, 27, 1)
	FIELD(GPIO_OSRR0S0, P28, 28, 1)
	FIELD(GPIO_OSRR0S0, P29, 29, 1)
	FIELD(GPIO_OSRR0S0, P30, 30, 1)
	FIELD(GPIO_OSRR0S0, P31, 31, 1)
REG32(GPIO_OSRR0S1, 0x334)
	FIELD(GPIO_OSRR0S1, P0, 0, 1)
	FIELD(GPIO_OSRR0S1, P1, 1, 1)
	FIELD(GPIO_OSRR0S1, P2, 2, 1)
	FIELD(GPIO_OSRR0S1, P3, 3, 1)
	FIELD(GPIO_OSRR0S1, P4, 4, 1)
	FIELD(GPIO_OSRR0S1, P5, 5, 1)
	FIELD(GPIO_OSRR0S1, P6, 6, 1)
	FIELD(GPIO_OSRR0S1, P7, 7, 1)
	FIELD(GPIO_OSRR0S1, P8, 8, 1)
	FIELD(GPIO_OSRR0S1, P9, 9, 1)
	FIELD(GPIO_OSRR0S1, P10, 10, 1)
	FIELD(GPIO_OSRR0S1, P11, 11, 1)
	FIELD(GPIO_OSRR0S1, P12, 12, 1)
	FIELD(GPIO_OSRR0S1, P13, 13, 1)
	FIELD(GPIO_OSRR0S1, P14, 14, 1)
	FIELD(GPIO_OSRR0S1, P15, 15, 1)
	FIELD(GPIO_OSRR0S1, P16, 16, 1)
	FIELD(GPIO_OSRR0S1, P17, 17, 1)
	FIELD(GPIO_OSRR0S1, P18, 18, 1)
	FIELD(GPIO_OSRR0S1, P19, 19, 1)
	FIELD(GPIO_OSRR0S1, P20, 20, 1)
	FIELD(GPIO_OSRR0S1, P21, 21, 1)
	FIELD(GPIO_OSRR0S1, P22, 22, 1)
	FIELD(GPIO_OSRR0S1, P23, 23, 1)
	FIELD(GPIO_OSRR0S1, P24, 24, 1)
	FIELD(GPIO_OSRR0S1, P25, 25, 1)
	FIELD(GPIO_OSRR0S1, P26, 26, 1)
	FIELD(GPIO_OSRR0S1, P27, 27, 1)
	FIELD(GPIO_OSRR0S1, P28, 28, 1)
	FIELD(GPIO_OSRR0S1, P29, 29, 1)
	FIELD(GPIO_OSRR0S1, P30, 30, 1)
	FIELD(GPIO_OSRR0S1, P31, 31, 1)
REG32(GPIO_OSRR0S2, 0x534)
	FIELD(GPIO_OSRR0S2, P0, 0, 1)
	FIELD(GPIO_OSRR0S2, P1, 1, 1)
	FIELD(GPIO_OSRR0S2, P2, 2, 1)
	FIELD(GPIO_OSRR0S2, P3, 3, 1)
	FIELD(GPIO_OSRR0S2, P4, 4, 1)
	FIELD(GPIO_OSRR0S2, P5, 5, 1)
	FIELD(GPIO_OSRR0S2, P6, 6, 1)
	FIELD(GPIO_OSRR0S2, P7, 7, 1)
	FIELD(GPIO_OSRR0S2, P8, 8, 1)
	FIELD(GPIO_OSRR0S2, P9, 9, 1)
	FIELD(GPIO_OSRR0S2, P10, 10, 1)
	FIELD(GPIO_OSRR0S2, P11, 11, 1)
	FIELD(GPIO_OSRR0S2, P12, 12, 1)
	FIELD(GPIO_OSRR0S2, P13, 13, 1)
	FIELD(GPIO_OSRR0S2, P14, 14, 1)
	FIELD(GPIO_OSRR0S2, P15, 15, 1)
	FIELD(GPIO_OSRR0S2, P16, 16, 1)
	FIELD(GPIO_OSRR0S2, P17, 17, 1)
	FIELD(GPIO_OSRR0S2, P18, 18, 1)
	FIELD(GPIO_OSRR0S2, P19, 19, 1)
	FIELD(GPIO_OSRR0S2, P20, 20, 1)
	FIELD(GPIO_OSRR0S2, P21, 21, 1)
	FIELD(GPIO_OSRR0S2, P22, 22, 1)
	FIELD(GPIO_OSRR0S2, P23, 23, 1)
	FIELD(GPIO_OSRR0S2, P24, 24, 1)
	FIELD(GPIO_OSRR0S2, P25, 25, 1)
	FIELD(GPIO_OSRR0S2, P26, 26, 1)
	FIELD(GPIO_OSRR0S2, P27, 27, 1)
	FIELD(GPIO_OSRR0S2, P28, 28, 1)
	FIELD(GPIO_OSRR0S2, P29, 29, 1)
	FIELD(GPIO_OSRR0S2, P30, 30, 1)
	FIELD(GPIO_OSRR0S2, P31, 31, 1)
REG32(GPIO_OSRR0T0, 0x13c)
	FIELD(GPIO_OSRR0T0, P0, 0, 1)
	FIELD(GPIO_OSRR0T0, P1, 1, 1)
	FIELD(GPIO_OSRR0T0, P2, 2, 1)
	FIELD(GPIO_OSRR0T0, P3, 3, 1)
	FIELD(GPIO_OSRR0T0, P4, 4, 1)
	FIELD(GPIO_OSRR0T0, P5, 5, 1)
	FIELD(GPIO_OSRR0T0, P6, 6, 1)
	FIELD(GPIO_OSRR0T0, P7, 7, 1)
	FIELD(GPIO_OSRR0T0, P8, 8, 1)
	FIELD(GPIO_OSRR0T0, P9, 9, 1)
	FIELD(GPIO_OSRR0T0, P10, 10, 1)
	FIELD(GPIO_OSRR0T0, P11, 11, 1)
	FIELD(GPIO_OSRR0T0, P12, 12, 1)
	FIELD(GPIO_OSRR0T0, P13, 13, 1)
	FIELD(GPIO_OSRR0T0, P14, 14, 1)
	FIELD(GPIO_OSRR0T0, P15, 15, 1)
	FIELD(GPIO_OSRR0T0, P16, 16, 1)
	FIELD(GPIO_OSRR0T0, P17, 17, 1)
	FIELD(GPIO_OSRR0T0, P18, 18, 1)
	FIELD(GPIO_OSRR0T0, P19, 19, 1)
	FIELD(GPIO_OSRR0T0, P20, 20, 1)
	FIELD(GPIO_OSRR0T0, P21, 21, 1)
	FIELD(GPIO_OSRR0T0, P22, 22, 1)
	FIELD(GPIO_OSRR0T0, P23, 23, 1)
	FIELD(GPIO_OSRR0T0, P24, 24, 1)
	FIELD(GPIO_OSRR0T0, P25, 25, 1)
	FIELD(GPIO_OSRR0T0, P26, 26, 1)
	FIELD(GPIO_OSRR0T0, P27, 27, 1)
	FIELD(GPIO_OSRR0T0, P28, 28, 1)
	FIELD(GPIO_OSRR0T0, P29, 29, 1)
	FIELD(GPIO_OSRR0T0, P30, 30, 1)
	FIELD(GPIO_OSRR0T0, P31, 31, 1)
REG32(GPIO_OSRR0T1, 0x33c)
	FIELD(GPIO_OSRR0T1, P0, 0, 1)
	FIELD(GPIO_OSRR0T1, P1, 1, 1)
	FIELD(GPIO_OSRR0T1, P2, 2, 1)
	FIELD(GPIO_OSRR0T1, P3, 3, 1)
	FIELD(GPIO_OSRR0T1, P4, 4, 1)
	FIELD(GPIO_OSRR0T1, P5, 5, 1)
	FIELD(GPIO_OSRR0T1, P6, 6, 1)
	FIELD(GPIO_OSRR0T1, P7, 7, 1)
	FIELD(GPIO_OSRR0T1, P8, 8, 1)
	FIELD(GPIO_OSRR0T1, P9, 9, 1)
	FIELD(GPIO_OSRR0T1, P10, 10, 1)
	FIELD(GPIO_OSRR0T1, P11, 11, 1)
	FIELD(GPIO_OSRR0T1, P12, 12, 1)
	FIELD(GPIO_OSRR0T1, P13, 13, 1)
	FIELD(GPIO_OSRR0T1, P14, 14, 1)
	FIELD(GPIO_OSRR0T1, P15, 15, 1)
	FIELD(GPIO_OSRR0T1, P16, 16, 1)
	FIELD(GPIO_OSRR0T1, P17, 17, 1)
	FIELD(GPIO_OSRR0T1, P18, 18, 1)
	FIELD(GPIO_OSRR0T1, P19, 19, 1)
	FIELD(GPIO_OSRR0T1, P20, 20, 1)
	FIELD(GPIO_OSRR0T1, P21, 21, 1)
	FIELD(GPIO_OSRR0T1, P22, 22, 1)
	FIELD(GPIO_OSRR0T1, P23, 23, 1)
	FIELD(GPIO_OSRR0T1, P24, 24, 1)
	FIELD(GPIO_OSRR0T1, P25, 25, 1)
	FIELD(GPIO_OSRR0T1, P26, 26, 1)
	FIELD(GPIO_OSRR0T1, P27, 27, 1)
	FIELD(GPIO_OSRR0T1, P28, 28, 1)
	FIELD(GPIO_OSRR0T1, P29, 29, 1)
	FIELD(GPIO_OSRR0T1, P30, 30, 1)
	FIELD(GPIO_OSRR0T1, P31, 31, 1)
REG32(GPIO_OSRR0T2, 0x53c)
	FIELD(GPIO_OSRR0T2, P0, 0, 1)
	FIELD(GPIO_OSRR0T2, P1, 1, 1)
	FIELD(GPIO_OSRR0T2, P2, 2, 1)
	FIELD(GPIO_OSRR0T2, P3, 3, 1)
	FIELD(GPIO_OSRR0T2, P4, 4, 1)
	FIELD(GPIO_OSRR0T2, P5, 5, 1)
	FIELD(GPIO_OSRR0T2, P6, 6, 1)
	FIELD(GPIO_OSRR0T2, P7, 7, 1)
	FIELD(GPIO_OSRR0T2, P8, 8, 1)
	FIELD(GPIO_OSRR0T2, P9, 9, 1)
	FIELD(GPIO_OSRR0T2, P10, 10, 1)
	FIELD(GPIO_OSRR0T2, P11, 11, 1)
	FIELD(GPIO_OSRR0T2, P12, 12, 1)
	FIELD(GPIO_OSRR0T2, P13, 13, 1)
	FIELD(GPIO_OSRR0T2, P14, 14, 1)
	FIELD(GPIO_OSRR0T2, P15, 15, 1)
	FIELD(GPIO_OSRR0T2, P16, 16, 1)
	FIELD(GPIO_OSRR0T2, P17, 17, 1)
	FIELD(GPIO_OSRR0T2, P18, 18, 1)
	FIELD(GPIO_OSRR0T2, P19, 19, 1)
	FIELD(GPIO_OSRR0T2, P20, 20, 1)
	FIELD(GPIO_OSRR0T2, P21, 21, 1)
	FIELD(GPIO_OSRR0T2, P22, 22, 1)
	FIELD(GPIO_OSRR0T2, P23, 23, 1)
	FIELD(GPIO_OSRR0T2, P24, 24, 1)
	FIELD(GPIO_OSRR0T2, P25, 25, 1)
	FIELD(GPIO_OSRR0T2, P26, 26, 1)
	FIELD(GPIO_OSRR0T2, P27, 27, 1)
	FIELD(GPIO_OSRR0T2, P28, 28, 1)
	FIELD(GPIO_OSRR0T2, P29, 29, 1)
	FIELD(GPIO_OSRR0T2, P30, 30, 1)
	FIELD(GPIO_OSRR0T2, P31, 31, 1)
REG32(GPIO_OSRR00, 0x130)
	FIELD(GPIO_OSRR00, P0, 0, 1)
	FIELD(GPIO_OSRR00, P1, 1, 1)
	FIELD(GPIO_OSRR00, P2, 2, 1)
	FIELD(GPIO_OSRR00, P3, 3, 1)
	FIELD(GPIO_OSRR00, P4, 4, 1)
	FIELD(GPIO_OSRR00, P5, 5, 1)
	FIELD(GPIO_OSRR00, P6, 6, 1)
	FIELD(GPIO_OSRR00, P7, 7, 1)
	FIELD(GPIO_OSRR00, P8, 8, 1)
	FIELD(GPIO_OSRR00, P9, 9, 1)
	FIELD(GPIO_OSRR00, P10, 10, 1)
	FIELD(GPIO_OSRR00, P11, 11, 1)
	FIELD(GPIO_OSRR00, P12, 12, 1)
	FIELD(GPIO_OSRR00, P13, 13, 1)
	FIELD(GPIO_OSRR00, P14, 14, 1)
	FIELD(GPIO_OSRR00, P15, 15, 1)
	FIELD(GPIO_OSRR00, P16, 16, 1)
	FIELD(GPIO_OSRR00, P17, 17, 1)
	FIELD(GPIO_OSRR00, P18, 18, 1)
	FIELD(GPIO_OSRR00, P19, 19, 1)
	FIELD(GPIO_OSRR00, P20, 20, 1)
	FIELD(GPIO_OSRR00, P21, 21, 1)
	FIELD(GPIO_OSRR00, P22, 22, 1)
	FIELD(GPIO_OSRR00, P23, 23, 1)
	FIELD(GPIO_OSRR00, P24, 24, 1)
	FIELD(GPIO_OSRR00, P25, 25, 1)
	FIELD(GPIO_OSRR00, P26, 26, 1)
	FIELD(GPIO_OSRR00, P27, 27, 1)
	FIELD(GPIO_OSRR00, P28, 28, 1)
	FIELD(GPIO_OSRR00, P29, 29, 1)
	FIELD(GPIO_OSRR00, P30, 30, 1)
	FIELD(GPIO_OSRR00, P31, 31, 1)
REG32(GPIO_OSRR01, 0x330)
	FIELD(GPIO_OSRR01, P0, 0, 1)
	FIELD(GPIO_OSRR01, P1, 1, 1)
	FIELD(GPIO_OSRR01, P2, 2, 1)
	FIELD(GPIO_OSRR01, P3, 3, 1)
	FIELD(GPIO_OSRR01, P4, 4, 1)
	FIELD(GPIO_OSRR01, P5, 5, 1)
	FIELD(GPIO_OSRR01, P6, 6, 1)
	FIELD(GPIO_OSRR01, P7, 7, 1)
	FIELD(GPIO_OSRR01, P8, 8, 1)
	FIELD(GPIO_OSRR01, P9, 9, 1)
	FIELD(GPIO_OSRR01, P10, 10, 1)
	FIELD(GPIO_OSRR01, P11, 11, 1)
	FIELD(GPIO_OSRR01, P12, 12, 1)
	FIELD(GPIO_OSRR01, P13, 13, 1)
	FIELD(GPIO_OSRR01, P14, 14, 1)
	FIELD(GPIO_OSRR01, P15, 15, 1)
	FIELD(GPIO_OSRR01, P16, 16, 1)
	FIELD(GPIO_OSRR01, P17, 17, 1)
	FIELD(GPIO_OSRR01, P18, 18, 1)
	FIELD(GPIO_OSRR01, P19, 19, 1)
	FIELD(GPIO_OSRR01, P20, 20, 1)
	FIELD(GPIO_OSRR01, P21, 21, 1)
	FIELD(GPIO_OSRR01, P22, 22, 1)
	FIELD(GPIO_OSRR01, P23, 23, 1)
	FIELD(GPIO_OSRR01, P24, 24, 1)
	FIELD(GPIO_OSRR01, P25, 25, 1)
	FIELD(GPIO_OSRR01, P26, 26, 1)
	FIELD(GPIO_OSRR01, P27, 27, 1)
	FIELD(GPIO_OSRR01, P28, 28, 1)
	FIELD(GPIO_OSRR01, P29, 29, 1)
	FIELD(GPIO_OSRR01, P30, 30, 1)
	FIELD(GPIO_OSRR01, P31, 31, 1)
REG32(GPIO_OSRR02, 0x530)
	FIELD(GPIO_OSRR02, P0, 0, 1)
	FIELD(GPIO_OSRR02, P1, 1, 1)
	FIELD(GPIO_OSRR02, P2, 2, 1)
	FIELD(GPIO_OSRR02, P3, 3, 1)
	FIELD(GPIO_OSRR02, P4, 4, 1)
	FIELD(GPIO_OSRR02, P5, 5, 1)
	FIELD(GPIO_OSRR02, P6, 6, 1)
	FIELD(GPIO_OSRR02, P7, 7, 1)
	FIELD(GPIO_OSRR02, P8, 8, 1)
	FIELD(GPIO_OSRR02, P9, 9, 1)
	FIELD(GPIO_OSRR02, P10, 10, 1)
	FIELD(GPIO_OSRR02, P11, 11, 1)
	FIELD(GPIO_OSRR02, P12, 12, 1)
	FIELD(GPIO_OSRR02, P13, 13, 1)
	FIELD(GPIO_OSRR02, P14, 14, 1)
	FIELD(GPIO_OSRR02, P15, 15, 1)
	FIELD(GPIO_OSRR02, P16, 16, 1)
	FIELD(GPIO_OSRR02, P17, 17, 1)
	FIELD(GPIO_OSRR02, P18, 18, 1)
	FIELD(GPIO_OSRR02, P19, 19, 1)
	FIELD(GPIO_OSRR02, P20, 20, 1)
	FIELD(GPIO_OSRR02, P21, 21, 1)
	FIELD(GPIO_OSRR02, P22, 22, 1)
	FIELD(GPIO_OSRR02, P23, 23, 1)
	FIELD(GPIO_OSRR02, P24, 24, 1)
	FIELD(GPIO_OSRR02, P25, 25, 1)
	FIELD(GPIO_OSRR02, P26, 26, 1)
	FIELD(GPIO_OSRR02, P27, 27, 1)
	FIELD(GPIO_OSRR02, P28, 28, 1)
	FIELD(GPIO_OSRR02, P29, 29, 1)
	FIELD(GPIO_OSRR02, P30, 30, 1)
	FIELD(GPIO_OSRR02, P31, 31, 1)
REG32(GPIO_OVRC0, 0x58)
	FIELD(GPIO_OVRC0, P0, 0, 1)
	FIELD(GPIO_OVRC0, P1, 1, 1)
	FIELD(GPIO_OVRC0, P2, 2, 1)
	FIELD(GPIO_OVRC0, P3, 3, 1)
	FIELD(GPIO_OVRC0, P4, 4, 1)
	FIELD(GPIO_OVRC0, P5, 5, 1)
	FIELD(GPIO_OVRC0, P6, 6, 1)
	FIELD(GPIO_OVRC0, P7, 7, 1)
	FIELD(GPIO_OVRC0, P8, 8, 1)
	FIELD(GPIO_OVRC0, P9, 9, 1)
	FIELD(GPIO_OVRC0, P10, 10, 1)
	FIELD(GPIO_OVRC0, P11, 11, 1)
	FIELD(GPIO_OVRC0, P12, 12, 1)
	FIELD(GPIO_OVRC0, P13, 13, 1)
	FIELD(GPIO_OVRC0, P14, 14, 1)
	FIELD(GPIO_OVRC0, P15, 15, 1)
	FIELD(GPIO_OVRC0, P16, 16, 1)
	FIELD(GPIO_OVRC0, P17, 17, 1)
	FIELD(GPIO_OVRC0, P18, 18, 1)
	FIELD(GPIO_OVRC0, P19, 19, 1)
	FIELD(GPIO_OVRC0, P20, 20, 1)
	FIELD(GPIO_OVRC0, P21, 21, 1)
	FIELD(GPIO_OVRC0, P22, 22, 1)
	FIELD(GPIO_OVRC0, P23, 23, 1)
	FIELD(GPIO_OVRC0, P24, 24, 1)
	FIELD(GPIO_OVRC0, P25, 25, 1)
	FIELD(GPIO_OVRC0, P26, 26, 1)
	FIELD(GPIO_OVRC0, P27, 27, 1)
	FIELD(GPIO_OVRC0, P28, 28, 1)
	FIELD(GPIO_OVRC0, P29, 29, 1)
	FIELD(GPIO_OVRC0, P30, 30, 1)
	FIELD(GPIO_OVRC0, P31, 31, 1)
REG32(GPIO_OVRC1, 0x258)
	FIELD(GPIO_OVRC1, P0, 0, 1)
	FIELD(GPIO_OVRC1, P1, 1, 1)
	FIELD(GPIO_OVRC1, P2, 2, 1)
	FIELD(GPIO_OVRC1, P3, 3, 1)
	FIELD(GPIO_OVRC1, P4, 4, 1)
	FIELD(GPIO_OVRC1, P5, 5, 1)
	FIELD(GPIO_OVRC1, P6, 6, 1)
	FIELD(GPIO_OVRC1, P7, 7, 1)
	FIELD(GPIO_OVRC1, P8, 8, 1)
	FIELD(GPIO_OVRC1, P9, 9, 1)
	FIELD(GPIO_OVRC1, P10, 10, 1)
	FIELD(GPIO_OVRC1, P11, 11, 1)
	FIELD(GPIO_OVRC1, P12, 12, 1)
	FIELD(GPIO_OVRC1, P13, 13, 1)
	FIELD(GPIO_OVRC1, P14, 14, 1)
	FIELD(GPIO_OVRC1, P15, 15, 1)
	FIELD(GPIO_OVRC1, P16, 16, 1)
	FIELD(GPIO_OVRC1, P17, 17, 1)
	FIELD(GPIO_OVRC1, P18, 18, 1)
	FIELD(GPIO_OVRC1, P19, 19, 1)
	FIELD(GPIO_OVRC1, P20, 20, 1)
	FIELD(GPIO_OVRC1, P21, 21, 1)
	FIELD(GPIO_OVRC1, P22, 22, 1)
	FIELD(GPIO_OVRC1, P23, 23, 1)
	FIELD(GPIO_OVRC1, P24, 24, 1)
	FIELD(GPIO_OVRC1, P25, 25, 1)
	FIELD(GPIO_OVRC1, P26, 26, 1)
	FIELD(GPIO_OVRC1, P27, 27, 1)
	FIELD(GPIO_OVRC1, P28, 28, 1)
	FIELD(GPIO_OVRC1, P29, 29, 1)
	FIELD(GPIO_OVRC1, P30, 30, 1)
	FIELD(GPIO_OVRC1, P31, 31, 1)
REG32(GPIO_OVRC2, 0x458)
	FIELD(GPIO_OVRC2, P0, 0, 1)
	FIELD(GPIO_OVRC2, P1, 1, 1)
	FIELD(GPIO_OVRC2, P2, 2, 1)
	FIELD(GPIO_OVRC2, P3, 3, 1)
	FIELD(GPIO_OVRC2, P4, 4, 1)
	FIELD(GPIO_OVRC2, P5, 5, 1)
	FIELD(GPIO_OVRC2, P6, 6, 1)
	FIELD(GPIO_OVRC2, P7, 7, 1)
	FIELD(GPIO_OVRC2, P8, 8, 1)
	FIELD(GPIO_OVRC2, P9, 9, 1)
	FIELD(GPIO_OVRC2, P10, 10, 1)
	FIELD(GPIO_OVRC2, P11, 11, 1)
	FIELD(GPIO_OVRC2, P12, 12, 1)
	FIELD(GPIO_OVRC2, P13, 13, 1)
	FIELD(GPIO_OVRC2, P14, 14, 1)
	FIELD(GPIO_OVRC2, P15, 15, 1)
	FIELD(GPIO_OVRC2, P16, 16, 1)
	FIELD(GPIO_OVRC2, P17, 17, 1)
	FIELD(GPIO_OVRC2, P18, 18, 1)
	FIELD(GPIO_OVRC2, P19, 19, 1)
	FIELD(GPIO_OVRC2, P20, 20, 1)
	FIELD(GPIO_OVRC2, P21, 21, 1)
	FIELD(GPIO_OVRC2, P22, 22, 1)
	FIELD(GPIO_OVRC2, P23, 23, 1)
	FIELD(GPIO_OVRC2, P24, 24, 1)
	FIELD(GPIO_OVRC2, P25, 25, 1)
	FIELD(GPIO_OVRC2, P26, 26, 1)
	FIELD(GPIO_OVRC2, P27, 27, 1)
	FIELD(GPIO_OVRC2, P28, 28, 1)
	FIELD(GPIO_OVRC2, P29, 29, 1)
	FIELD(GPIO_OVRC2, P30, 30, 1)
	FIELD(GPIO_OVRC2, P31, 31, 1)
REG32(GPIO_OVRS0, 0x54)
	FIELD(GPIO_OVRS0, P0, 0, 1)
	FIELD(GPIO_OVRS0, P1, 1, 1)
	FIELD(GPIO_OVRS0, P2, 2, 1)
	FIELD(GPIO_OVRS0, P3, 3, 1)
	FIELD(GPIO_OVRS0, P4, 4, 1)
	FIELD(GPIO_OVRS0, P5, 5, 1)
	FIELD(GPIO_OVRS0, P6, 6, 1)
	FIELD(GPIO_OVRS0, P7, 7, 1)
	FIELD(GPIO_OVRS0, P8, 8, 1)
	FIELD(GPIO_OVRS0, P9, 9, 1)
	FIELD(GPIO_OVRS0, P10, 10, 1)
	FIELD(GPIO_OVRS0, P11, 11, 1)
	FIELD(GPIO_OVRS0, P12, 12, 1)
	FIELD(GPIO_OVRS0, P13, 13, 1)
	FIELD(GPIO_OVRS0, P14, 14, 1)
	FIELD(GPIO_OVRS0, P15, 15, 1)
	FIELD(GPIO_OVRS0, P16, 16, 1)
	FIELD(GPIO_OVRS0, P17, 17, 1)
	FIELD(GPIO_OVRS0, P18, 18, 1)
	FIELD(GPIO_OVRS0, P19, 19, 1)
	FIELD(GPIO_OVRS0, P20, 20, 1)
	FIELD(GPIO_OVRS0, P21, 21, 1)
	FIELD(GPIO_OVRS0, P22, 22, 1)
	FIELD(GPIO_OVRS0, P23, 23, 1)
	FIELD(GPIO_OVRS0, P24, 24, 1)
	FIELD(GPIO_OVRS0, P25, 25, 1)
	FIELD(GPIO_OVRS0, P26, 26, 1)
	FIELD(GPIO_OVRS0, P27, 27, 1)
	FIELD(GPIO_OVRS0, P28, 28, 1)
	FIELD(GPIO_OVRS0, P29, 29, 1)
	FIELD(GPIO_OVRS0, P30, 30, 1)
	FIELD(GPIO_OVRS0, P31, 31, 1)
REG32(GPIO_OVRS1, 0x254)
	FIELD(GPIO_OVRS1, P0, 0, 1)
	FIELD(GPIO_OVRS1, P1, 1, 1)
	FIELD(GPIO_OVRS1, P2, 2, 1)
	FIELD(GPIO_OVRS1, P3, 3, 1)
	FIELD(GPIO_OVRS1, P4, 4, 1)
	FIELD(GPIO_OVRS1, P5, 5, 1)
	FIELD(GPIO_OVRS1, P6, 6, 1)
	FIELD(GPIO_OVRS1, P7, 7, 1)
	FIELD(GPIO_OVRS1, P8, 8, 1)
	FIELD(GPIO_OVRS1, P9, 9, 1)
	FIELD(GPIO_OVRS1, P10, 10, 1)
	FIELD(GPIO_OVRS1, P11, 11, 1)
	FIELD(GPIO_OVRS1, P12, 12, 1)
	FIELD(GPIO_OVRS1, P13, 13, 1)
	FIELD(GPIO_OVRS1, P14, 14, 1)
	FIELD(GPIO_OVRS1, P15, 15, 1)
	FIELD(GPIO_OVRS1, P16, 16, 1)
	FIELD(GPIO_OVRS1, P17, 17, 1)
	FIELD(GPIO_OVRS1, P18, 18, 1)
	FIELD(GPIO_OVRS1, P19, 19, 1)
	FIELD(GPIO_OVRS1, P20, 20, 1)
	FIELD(GPIO_OVRS1, P21, 21, 1)
	FIELD(GPIO_OVRS1, P22, 22, 1)
	FIELD(GPIO_OVRS1, P23, 23, 1)
	FIELD(GPIO_OVRS1, P24, 24, 1)
	FIELD(GPIO_OVRS1, P25, 25, 1)
	FIELD(GPIO_OVRS1, P26, 26, 1)
	FIELD(GPIO_OVRS1, P27, 27, 1)
	FIELD(GPIO_OVRS1, P28, 28, 1)
	FIELD(GPIO_OVRS1, P29, 29, 1)
	FIELD(GPIO_OVRS1, P30, 30, 1)
	FIELD(GPIO_OVRS1, P31, 31, 1)
REG32(GPIO_OVRS2, 0x454)
	FIELD(GPIO_OVRS2, P0, 0, 1)
	FIELD(GPIO_OVRS2, P1, 1, 1)
	FIELD(GPIO_OVRS2, P2, 2, 1)
	FIELD(GPIO_OVRS2, P3, 3, 1)
	FIELD(GPIO_OVRS2, P4, 4, 1)
	FIELD(GPIO_OVRS2, P5, 5, 1)
	FIELD(GPIO_OVRS2, P6, 6, 1)
	FIELD(GPIO_OVRS2, P7, 7, 1)
	FIELD(GPIO_OVRS2, P8, 8, 1)
	FIELD(GPIO_OVRS2, P9, 9, 1)
	FIELD(GPIO_OVRS2, P10, 10, 1)
	FIELD(GPIO_OVRS2, P11, 11, 1)
	FIELD(GPIO_OVRS2, P12, 12, 1)
	FIELD(GPIO_OVRS2, P13, 13, 1)
	FIELD(GPIO_OVRS2, P14, 14, 1)
	FIELD(GPIO_OVRS2, P15, 15, 1)
	FIELD(GPIO_OVRS2, P16, 16, 1)
	FIELD(GPIO_OVRS2, P17, 17, 1)
	FIELD(GPIO_OVRS2, P18, 18, 1)
	FIELD(GPIO_OVRS2, P19, 19, 1)
	FIELD(GPIO_OVRS2, P20, 20, 1)
	FIELD(GPIO_OVRS2, P21, 21, 1)
	FIELD(GPIO_OVRS2, P22, 22, 1)
	FIELD(GPIO_OVRS2, P23, 23, 1)
	FIELD(GPIO_OVRS2, P24, 24, 1)
	FIELD(GPIO_OVRS2, P25, 25, 1)
	FIELD(GPIO_OVRS2, P26, 26, 1)
	FIELD(GPIO_OVRS2, P27, 27, 1)
	FIELD(GPIO_OVRS2, P28, 28, 1)
	FIELD(GPIO_OVRS2, P29, 29, 1)
	FIELD(GPIO_OVRS2, P30, 30, 1)
	FIELD(GPIO_OVRS2, P31, 31, 1)
REG32(GPIO_OVRT0, 0x5c)
	FIELD(GPIO_OVRT0, P0, 0, 1)
	FIELD(GPIO_OVRT0, P1, 1, 1)
	FIELD(GPIO_OVRT0, P2, 2, 1)
	FIELD(GPIO_OVRT0, P3, 3, 1)
	FIELD(GPIO_OVRT0, P4, 4, 1)
	FIELD(GPIO_OVRT0, P5, 5, 1)
	FIELD(GPIO_OVRT0, P6, 6, 1)
	FIELD(GPIO_OVRT0, P7, 7, 1)
	FIELD(GPIO_OVRT0, P8, 8, 1)
	FIELD(GPIO_OVRT0, P9, 9, 1)
	FIELD(GPIO_OVRT0, P10, 10, 1)
	FIELD(GPIO_OVRT0, P11, 11, 1)
	FIELD(GPIO_OVRT0, P12, 12, 1)
	FIELD(GPIO_OVRT0, P13, 13, 1)
	FIELD(GPIO_OVRT0, P14, 14, 1)
	FIELD(GPIO_OVRT0, P15, 15, 1)
	FIELD(GPIO_OVRT0, P16, 16, 1)
	FIELD(GPIO_OVRT0, P17, 17, 1)
	FIELD(GPIO_OVRT0, P18, 18, 1)
	FIELD(GPIO_OVRT0, P19, 19, 1)
	FIELD(GPIO_OVRT0, P20, 20, 1)
	FIELD(GPIO_OVRT0, P21, 21, 1)
	FIELD(GPIO_OVRT0, P22, 22, 1)
	FIELD(GPIO_OVRT0, P23, 23, 1)
	FIELD(GPIO_OVRT0, P24, 24, 1)
	FIELD(GPIO_OVRT0, P25, 25, 1)
	FIELD(GPIO_OVRT0, P26, 26, 1)
	FIELD(GPIO_OVRT0, P27, 27, 1)
	FIELD(GPIO_OVRT0, P28, 28, 1)
	FIELD(GPIO_OVRT0, P29, 29, 1)
	FIELD(GPIO_OVRT0, P30, 30, 1)
	FIELD(GPIO_OVRT0, P31, 31, 1)
REG32(GPIO_OVRT1, 0x25c)
	FIELD(GPIO_OVRT1, P0, 0, 1)
	FIELD(GPIO_OVRT1, P1, 1, 1)
	FIELD(GPIO_OVRT1, P2, 2, 1)
	FIELD(GPIO_OVRT1, P3, 3, 1)
	FIELD(GPIO_OVRT1, P4, 4, 1)
	FIELD(GPIO_OVRT1, P5, 5, 1)
	FIELD(GPIO_OVRT1, P6, 6, 1)
	FIELD(GPIO_OVRT1, P7, 7, 1)
	FIELD(GPIO_OVRT1, P8, 8, 1)
	FIELD(GPIO_OVRT1, P9, 9, 1)
	FIELD(GPIO_OVRT1, P10, 10, 1)
	FIELD(GPIO_OVRT1, P11, 11, 1)
	FIELD(GPIO_OVRT1, P12, 12, 1)
	FIELD(GPIO_OVRT1, P13, 13, 1)
	FIELD(GPIO_OVRT1, P14, 14, 1)
	FIELD(GPIO_OVRT1, P15, 15, 1)
	FIELD(GPIO_OVRT1, P16, 16, 1)
	FIELD(GPIO_OVRT1, P17, 17, 1)
	FIELD(GPIO_OVRT1, P18, 18, 1)
	FIELD(GPIO_OVRT1, P19, 19, 1)
	FIELD(GPIO_OVRT1, P20, 20, 1)
	FIELD(GPIO_OVRT1, P21, 21, 1)
	FIELD(GPIO_OVRT1, P22, 22, 1)
	FIELD(GPIO_OVRT1, P23, 23, 1)
	FIELD(GPIO_OVRT1, P24, 24, 1)
	FIELD(GPIO_OVRT1, P25, 25, 1)
	FIELD(GPIO_OVRT1, P26, 26, 1)
	FIELD(GPIO_OVRT1, P27, 27, 1)
	FIELD(GPIO_OVRT1, P28, 28, 1)
	FIELD(GPIO_OVRT1, P29, 29, 1)
	FIELD(GPIO_OVRT1, P30, 30, 1)
	FIELD(GPIO_OVRT1, P31, 31, 1)
REG32(GPIO_OVRT2, 0x45c)
	FIELD(GPIO_OVRT2, P0, 0, 1)
	FIELD(GPIO_OVRT2, P1, 1, 1)
	FIELD(GPIO_OVRT2, P2, 2, 1)
	FIELD(GPIO_OVRT2, P3, 3, 1)
	FIELD(GPIO_OVRT2, P4, 4, 1)
	FIELD(GPIO_OVRT2, P5, 5, 1)
	FIELD(GPIO_OVRT2, P6, 6, 1)
	FIELD(GPIO_OVRT2, P7, 7, 1)
	FIELD(GPIO_OVRT2, P8, 8, 1)
	FIELD(GPIO_OVRT2, P9, 9, 1)
	FIELD(GPIO_OVRT2, P10, 10, 1)
	FIELD(GPIO_OVRT2, P11, 11, 1)
	FIELD(GPIO_OVRT2, P12, 12, 1)
	FIELD(GPIO_OVRT2, P13, 13, 1)
	FIELD(GPIO_OVRT2, P14, 14, 1)
	FIELD(GPIO_OVRT2, P15, 15, 1)
	FIELD(GPIO_OVRT2, P16, 16, 1)
	FIELD(GPIO_OVRT2, P17, 17, 1)
	FIELD(GPIO_OVRT2, P18, 18, 1)
	FIELD(GPIO_OVRT2, P19, 19, 1)
	FIELD(GPIO_OVRT2, P20, 20, 1)
	FIELD(GPIO_OVRT2, P21, 21, 1)
	FIELD(GPIO_OVRT2, P22, 22, 1)
	FIELD(GPIO_OVRT2, P23, 23, 1)
	FIELD(GPIO_OVRT2, P24, 24, 1)
	FIELD(GPIO_OVRT2, P25, 25, 1)
	FIELD(GPIO_OVRT2, P26, 26, 1)
	FIELD(GPIO_OVRT2, P27, 27, 1)
	FIELD(GPIO_OVRT2, P28, 28, 1)
	FIELD(GPIO_OVRT2, P29, 29, 1)
	FIELD(GPIO_OVRT2, P30, 30, 1)
	FIELD(GPIO_OVRT2, P31, 31, 1)
REG32(GPIO_OVR0, 0x50)
	FIELD(GPIO_OVR0, P0, 0, 1)
	FIELD(GPIO_OVR0, P1, 1, 1)
	FIELD(GPIO_OVR0, P2, 2, 1)
	FIELD(GPIO_OVR0, P3, 3, 1)
	FIELD(GPIO_OVR0, P4, 4, 1)
	FIELD(GPIO_OVR0, P5, 5, 1)
	FIELD(GPIO_OVR0, P6, 6, 1)
	FIELD(GPIO_OVR0, P7, 7, 1)
	FIELD(GPIO_OVR0, P8, 8, 1)
	FIELD(GPIO_OVR0, P9, 9, 1)
	FIELD(GPIO_OVR0, P10, 10, 1)
	FIELD(GPIO_OVR0, P11, 11, 1)
	FIELD(GPIO_OVR0, P12, 12, 1)
	FIELD(GPIO_OVR0, P13, 13, 1)
	FIELD(GPIO_OVR0, P14, 14, 1)
	FIELD(GPIO_OVR0, P15, 15, 1)
	FIELD(GPIO_OVR0, P16, 16, 1)
	FIELD(GPIO_OVR0, P17, 17, 1)
	FIELD(GPIO_OVR0, P18, 18, 1)
	FIELD(GPIO_OVR0, P19, 19, 1)
	FIELD(GPIO_OVR0, P20, 20, 1)
	FIELD(GPIO_OVR0, P21, 21, 1)
	FIELD(GPIO_OVR0, P22, 22, 1)
	FIELD(GPIO_OVR0, P23, 23, 1)
	FIELD(GPIO_OVR0, P24, 24, 1)
	FIELD(GPIO_OVR0, P25, 25, 1)
	FIELD(GPIO_OVR0, P26, 26, 1)
	FIELD(GPIO_OVR0, P27, 27, 1)
	FIELD(GPIO_OVR0, P28, 28, 1)
	FIELD(GPIO_OVR0, P29, 29, 1)
	FIELD(GPIO_OVR0, P30, 30, 1)
	FIELD(GPIO_OVR0, P31, 31, 1)
REG32(GPIO_OVR1, 0x250)
	FIELD(GPIO_OVR1, P0, 0, 1)
	FIELD(GPIO_OVR1, P1, 1, 1)
	FIELD(GPIO_OVR1, P2, 2, 1)
	FIELD(GPIO_OVR1, P3, 3, 1)
	FIELD(GPIO_OVR1, P4, 4, 1)
	FIELD(GPIO_OVR1, P5, 5, 1)
	FIELD(GPIO_OVR1, P6, 6, 1)
	FIELD(GPIO_OVR1, P7, 7, 1)
	FIELD(GPIO_OVR1, P8, 8, 1)
	FIELD(GPIO_OVR1, P9, 9, 1)
	FIELD(GPIO_OVR1, P10, 10, 1)
	FIELD(GPIO_OVR1, P11, 11, 1)
	FIELD(GPIO_OVR1, P12, 12, 1)
	FIELD(GPIO_OVR1, P13, 13, 1)
	FIELD(GPIO_OVR1, P14, 14, 1)
	FIELD(GPIO_OVR1, P15, 15, 1)
	FIELD(GPIO_OVR1, P16, 16, 1)
	FIELD(GPIO_OVR1, P17, 17, 1)
	FIELD(GPIO_OVR1, P18, 18, 1)
	FIELD(GPIO_OVR1, P19, 19, 1)
	FIELD(GPIO_OVR1, P20, 20, 1)
	FIELD(GPIO_OVR1, P21, 21, 1)
	FIELD(GPIO_OVR1, P22, 22, 1)
	FIELD(GPIO_OVR1, P23, 23, 1)
	FIELD(GPIO_OVR1, P24, 24, 1)
	FIELD(GPIO_OVR1, P25, 25, 1)
	FIELD(GPIO_OVR1, P26, 26, 1)
	FIELD(GPIO_OVR1, P27, 27, 1)
	FIELD(GPIO_OVR1, P28, 28, 1)
	FIELD(GPIO_OVR1, P29, 29, 1)
	FIELD(GPIO_OVR1, P30, 30, 1)
	FIELD(GPIO_OVR1, P31, 31, 1)
REG32(GPIO_OVR2, 0x450)
	FIELD(GPIO_OVR2, P0, 0, 1)
	FIELD(GPIO_OVR2, P1, 1, 1)
	FIELD(GPIO_OVR2, P2, 2, 1)
	FIELD(GPIO_OVR2, P3, 3, 1)
	FIELD(GPIO_OVR2, P4, 4, 1)
	FIELD(GPIO_OVR2, P5, 5, 1)
	FIELD(GPIO_OVR2, P6, 6, 1)
	FIELD(GPIO_OVR2, P7, 7, 1)
	FIELD(GPIO_OVR2, P8, 8, 1)
	FIELD(GPIO_OVR2, P9, 9, 1)
	FIELD(GPIO_OVR2, P10, 10, 1)
	FIELD(GPIO_OVR2, P11, 11, 1)
	FIELD(GPIO_OVR2, P12, 12, 1)
	FIELD(GPIO_OVR2, P13, 13, 1)
	FIELD(GPIO_OVR2, P14, 14, 1)
	FIELD(GPIO_OVR2, P15, 15, 1)
	FIELD(GPIO_OVR2, P16, 16, 1)
	FIELD(GPIO_OVR2, P17, 17, 1)
	FIELD(GPIO_OVR2, P18, 18, 1)
	FIELD(GPIO_OVR2, P19, 19, 1)
	FIELD(GPIO_OVR2, P20, 20, 1)
	FIELD(GPIO_OVR2, P21, 21, 1)
	FIELD(GPIO_OVR2, P22, 22, 1)
	FIELD(GPIO_OVR2, P23, 23, 1)
	FIELD(GPIO_OVR2, P24, 24, 1)
	FIELD(GPIO_OVR2, P25, 25, 1)
	FIELD(GPIO_OVR2, P26, 26, 1)
	FIELD(GPIO_OVR2, P27, 27, 1)
	FIELD(GPIO_OVR2, P28, 28, 1)
	FIELD(GPIO_OVR2, P29, 29, 1)
	FIELD(GPIO_OVR2, P30, 30, 1)
	FIELD(GPIO_OVR2, P31, 31, 1)
REG32(GPIO_PARAMETER0, 0x1f8)
	FIELD(GPIO_PARAMETER0, PARAMETER, 0, 32)
REG32(GPIO_PARAMETER1, 0x3f8)
	FIELD(GPIO_PARAMETER1, PARAMETER, 0, 32)
REG32(GPIO_PARAMETER2, 0x5f8)
	FIELD(GPIO_PARAMETER2, PARAMETER, 0, 32)
REG32(GPIO_PDERC0, 0x88)
	FIELD(GPIO_PDERC0, P0, 0, 1)
	FIELD(GPIO_PDERC0, P1, 1, 1)
	FIELD(GPIO_PDERC0, P2, 2, 1)
	FIELD(GPIO_PDERC0, P3, 3, 1)
	FIELD(GPIO_PDERC0, P4, 4, 1)
	FIELD(GPIO_PDERC0, P5, 5, 1)
	FIELD(GPIO_PDERC0, P6, 6, 1)
	FIELD(GPIO_PDERC0, P7, 7, 1)
	FIELD(GPIO_PDERC0, P8, 8, 1)
	FIELD(GPIO_PDERC0, P9, 9, 1)
	FIELD(GPIO_PDERC0, P10, 10, 1)
	FIELD(GPIO_PDERC0, P11, 11, 1)
	FIELD(GPIO_PDERC0, P12, 12, 1)
	FIELD(GPIO_PDERC0, P13, 13, 1)
	FIELD(GPIO_PDERC0, P14, 14, 1)
	FIELD(GPIO_PDERC0, P15, 15, 1)
	FIELD(GPIO_PDERC0, P16, 16, 1)
	FIELD(GPIO_PDERC0, P17, 17, 1)
	FIELD(GPIO_PDERC0, P18, 18, 1)
	FIELD(GPIO_PDERC0, P19, 19, 1)
	FIELD(GPIO_PDERC0, P20, 20, 1)
	FIELD(GPIO_PDERC0, P21, 21, 1)
	FIELD(GPIO_PDERC0, P22, 22, 1)
	FIELD(GPIO_PDERC0, P23, 23, 1)
	FIELD(GPIO_PDERC0, P24, 24, 1)
	FIELD(GPIO_PDERC0, P25, 25, 1)
	FIELD(GPIO_PDERC0, P26, 26, 1)
	FIELD(GPIO_PDERC0, P27, 27, 1)
	FIELD(GPIO_PDERC0, P28, 28, 1)
	FIELD(GPIO_PDERC0, P29, 29, 1)
	FIELD(GPIO_PDERC0, P30, 30, 1)
	FIELD(GPIO_PDERC0, P31, 31, 1)
REG32(GPIO_PDERC1, 0x288)
	FIELD(GPIO_PDERC1, P0, 0, 1)
	FIELD(GPIO_PDERC1, P1, 1, 1)
	FIELD(GPIO_PDERC1, P2, 2, 1)
	FIELD(GPIO_PDERC1, P3, 3, 1)
	FIELD(GPIO_PDERC1, P4, 4, 1)
	FIELD(GPIO_PDERC1, P5, 5, 1)
	FIELD(GPIO_PDERC1, P6, 6, 1)
	FIELD(GPIO_PDERC1, P7, 7, 1)
	FIELD(GPIO_PDERC1, P8, 8, 1)
	FIELD(GPIO_PDERC1, P9, 9, 1)
	FIELD(GPIO_PDERC1, P10, 10, 1)
	FIELD(GPIO_PDERC1, P11, 11, 1)
	FIELD(GPIO_PDERC1, P12, 12, 1)
	FIELD(GPIO_PDERC1, P13, 13, 1)
	FIELD(GPIO_PDERC1, P14, 14, 1)
	FIELD(GPIO_PDERC1, P15, 15, 1)
	FIELD(GPIO_PDERC1, P16, 16, 1)
	FIELD(GPIO_PDERC1, P17, 17, 1)
	FIELD(GPIO_PDERC1, P18, 18, 1)
	FIELD(GPIO_PDERC1, P19, 19, 1)
	FIELD(GPIO_PDERC1, P20, 20, 1)
	FIELD(GPIO_PDERC1, P21, 21, 1)
	FIELD(GPIO_PDERC1, P22, 22, 1)
	FIELD(GPIO_PDERC1, P23, 23, 1)
	FIELD(GPIO_PDERC1, P24, 24, 1)
	FIELD(GPIO_PDERC1, P25, 25, 1)
	FIELD(GPIO_PDERC1, P26, 26, 1)
	FIELD(GPIO_PDERC1, P27, 27, 1)
	FIELD(GPIO_PDERC1, P28, 28, 1)
	FIELD(GPIO_PDERC1, P29, 29, 1)
	FIELD(GPIO_PDERC1, P30, 30, 1)
	FIELD(GPIO_PDERC1, P31, 31, 1)
REG32(GPIO_PDERC2, 0x488)
	FIELD(GPIO_PDERC2, P0, 0, 1)
	FIELD(GPIO_PDERC2, P1, 1, 1)
	FIELD(GPIO_PDERC2, P2, 2, 1)
	FIELD(GPIO_PDERC2, P3, 3, 1)
	FIELD(GPIO_PDERC2, P4, 4, 1)
	FIELD(GPIO_PDERC2, P5, 5, 1)
	FIELD(GPIO_PDERC2, P6, 6, 1)
	FIELD(GPIO_PDERC2, P7, 7, 1)
	FIELD(GPIO_PDERC2, P8, 8, 1)
	FIELD(GPIO_PDERC2, P9, 9, 1)
	FIELD(GPIO_PDERC2, P10, 10, 1)
	FIELD(GPIO_PDERC2, P11, 11, 1)
	FIELD(GPIO_PDERC2, P12, 12, 1)
	FIELD(GPIO_PDERC2, P13, 13, 1)
	FIELD(GPIO_PDERC2, P14, 14, 1)
	FIELD(GPIO_PDERC2, P15, 15, 1)
	FIELD(GPIO_PDERC2, P16, 16, 1)
	FIELD(GPIO_PDERC2, P17, 17, 1)
	FIELD(GPIO_PDERC2, P18, 18, 1)
	FIELD(GPIO_PDERC2, P19, 19, 1)
	FIELD(GPIO_PDERC2, P20, 20, 1)
	FIELD(GPIO_PDERC2, P21, 21, 1)
	FIELD(GPIO_PDERC2, P22, 22, 1)
	FIELD(GPIO_PDERC2, P23, 23, 1)
	FIELD(GPIO_PDERC2, P24, 24, 1)
	FIELD(GPIO_PDERC2, P25, 25, 1)
	FIELD(GPIO_PDERC2, P26, 26, 1)
	FIELD(GPIO_PDERC2, P27, 27, 1)
	FIELD(GPIO_PDERC2, P28, 28, 1)
	FIELD(GPIO_PDERC2, P29, 29, 1)
	FIELD(GPIO_PDERC2, P30, 30, 1)
	FIELD(GPIO_PDERC2, P31, 31, 1)
REG32(GPIO_PDERS0, 0x84)
	FIELD(GPIO_PDERS0, P0, 0, 1)
	FIELD(GPIO_PDERS0, P1, 1, 1)
	FIELD(GPIO_PDERS0, P2, 2, 1)
	FIELD(GPIO_PDERS0, P3, 3, 1)
	FIELD(GPIO_PDERS0, P4, 4, 1)
	FIELD(GPIO_PDERS0, P5, 5, 1)
	FIELD(GPIO_PDERS0, P6, 6, 1)
	FIELD(GPIO_PDERS0, P7, 7, 1)
	FIELD(GPIO_PDERS0, P8, 8, 1)
	FIELD(GPIO_PDERS0, P9, 9, 1)
	FIELD(GPIO_PDERS0, P10, 10, 1)
	FIELD(GPIO_PDERS0, P11, 11, 1)
	FIELD(GPIO_PDERS0, P12, 12, 1)
	FIELD(GPIO_PDERS0, P13, 13, 1)
	FIELD(GPIO_PDERS0, P14, 14, 1)
	FIELD(GPIO_PDERS0, P15, 15, 1)
	FIELD(GPIO_PDERS0, P16, 16, 1)
	FIELD(GPIO_PDERS0, P17, 17, 1)
	FIELD(GPIO_PDERS0, P18, 18, 1)
	FIELD(GPIO_PDERS0, P19, 19, 1)
	FIELD(GPIO_PDERS0, P20, 20, 1)
	FIELD(GPIO_PDERS0, P21, 21, 1)
	FIELD(GPIO_PDERS0, P22, 22, 1)
	FIELD(GPIO_PDERS0, P23, 23, 1)
	FIELD(GPIO_PDERS0, P24, 24, 1)
	FIELD(GPIO_PDERS0, P25, 25, 1)
	FIELD(GPIO_PDERS0, P26, 26, 1)
	FIELD(GPIO_PDERS0, P27, 27, 1)
	FIELD(GPIO_PDERS0, P28, 28, 1)
	FIELD(GPIO_PDERS0, P29, 29, 1)
	FIELD(GPIO_PDERS0, P30, 30, 1)
	FIELD(GPIO_PDERS0, P31, 31, 1)
REG32(GPIO_PDERS1, 0x284)
	FIELD(GPIO_PDERS1, P0, 0, 1)
	FIELD(GPIO_PDERS1, P1, 1, 1)
	FIELD(GPIO_PDERS1, P2, 2, 1)
	FIELD(GPIO_PDERS1, P3, 3, 1)
	FIELD(GPIO_PDERS1, P4, 4, 1)
	FIELD(GPIO_PDERS1, P5, 5, 1)
	FIELD(GPIO_PDERS1, P6, 6, 1)
	FIELD(GPIO_PDERS1, P7, 7, 1)
	FIELD(GPIO_PDERS1, P8, 8, 1)
	FIELD(GPIO_PDERS1, P9, 9, 1)
	FIELD(GPIO_PDERS1, P10, 10, 1)
	FIELD(GPIO_PDERS1, P11, 11, 1)
	FIELD(GPIO_PDERS1, P12, 12, 1)
	FIELD(GPIO_PDERS1, P13, 13, 1)
	FIELD(GPIO_PDERS1, P14, 14, 1)
	FIELD(GPIO_PDERS1, P15, 15, 1)
	FIELD(GPIO_PDERS1, P16, 16, 1)
	FIELD(GPIO_PDERS1, P17, 17, 1)
	FIELD(GPIO_PDERS1, P18, 18, 1)
	FIELD(GPIO_PDERS1, P19, 19, 1)
	FIELD(GPIO_PDERS1, P20, 20, 1)
	FIELD(GPIO_PDERS1, P21, 21, 1)
	FIELD(GPIO_PDERS1, P22, 22, 1)
	FIELD(GPIO_PDERS1, P23, 23, 1)
	FIELD(GPIO_PDERS1, P24, 24, 1)
	FIELD(GPIO_PDERS1, P25, 25, 1)
	FIELD(GPIO_PDERS1, P26, 26, 1)
	FIELD(GPIO_PDERS1, P27, 27, 1)
	FIELD(GPIO_PDERS1, P28, 28, 1)
	FIELD(GPIO_PDERS1, P29, 29, 1)
	FIELD(GPIO_PDERS1, P30, 30, 1)
	FIELD(GPIO_PDERS1, P31, 31, 1)
REG32(GPIO_PDERS2, 0x484)
	FIELD(GPIO_PDERS2, P0, 0, 1)
	FIELD(GPIO_PDERS2, P1, 1, 1)
	FIELD(GPIO_PDERS2, P2, 2, 1)
	FIELD(GPIO_PDERS2, P3, 3, 1)
	FIELD(GPIO_PDERS2, P4, 4, 1)
	FIELD(GPIO_PDERS2, P5, 5, 1)
	FIELD(GPIO_PDERS2, P6, 6, 1)
	FIELD(GPIO_PDERS2, P7, 7, 1)
	FIELD(GPIO_PDERS2, P8, 8, 1)
	FIELD(GPIO_PDERS2, P9, 9, 1)
	FIELD(GPIO_PDERS2, P10, 10, 1)
	FIELD(GPIO_PDERS2, P11, 11, 1)
	FIELD(GPIO_PDERS2, P12, 12, 1)
	FIELD(GPIO_PDERS2, P13, 13, 1)
	FIELD(GPIO_PDERS2, P14, 14, 1)
	FIELD(GPIO_PDERS2, P15, 15, 1)
	FIELD(GPIO_PDERS2, P16, 16, 1)
	FIELD(GPIO_PDERS2, P17, 17, 1)
	FIELD(GPIO_PDERS2, P18, 18, 1)
	FIELD(GPIO_PDERS2, P19, 19, 1)
	FIELD(GPIO_PDERS2, P20, 20, 1)
	FIELD(GPIO_PDERS2, P21, 21, 1)
	FIELD(GPIO_PDERS2, P22, 22, 1)
	FIELD(GPIO_PDERS2, P23, 23, 1)
	FIELD(GPIO_PDERS2, P24, 24, 1)
	FIELD(GPIO_PDERS2, P25, 25, 1)
	FIELD(GPIO_PDERS2, P26, 26, 1)
	FIELD(GPIO_PDERS2, P27, 27, 1)
	FIELD(GPIO_PDERS2, P28, 28, 1)
	FIELD(GPIO_PDERS2, P29, 29, 1)
	FIELD(GPIO_PDERS2, P30, 30, 1)
	FIELD(GPIO_PDERS2, P31, 31, 1)
REG32(GPIO_PDERT0, 0x8c)
	FIELD(GPIO_PDERT0, P0, 0, 1)
	FIELD(GPIO_PDERT0, P1, 1, 1)
	FIELD(GPIO_PDERT0, P2, 2, 1)
	FIELD(GPIO_PDERT0, P3, 3, 1)
	FIELD(GPIO_PDERT0, P4, 4, 1)
	FIELD(GPIO_PDERT0, P5, 5, 1)
	FIELD(GPIO_PDERT0, P6, 6, 1)
	FIELD(GPIO_PDERT0, P7, 7, 1)
	FIELD(GPIO_PDERT0, P8, 8, 1)
	FIELD(GPIO_PDERT0, P9, 9, 1)
	FIELD(GPIO_PDERT0, P10, 10, 1)
	FIELD(GPIO_PDERT0, P11, 11, 1)
	FIELD(GPIO_PDERT0, P12, 12, 1)
	FIELD(GPIO_PDERT0, P13, 13, 1)
	FIELD(GPIO_PDERT0, P14, 14, 1)
	FIELD(GPIO_PDERT0, P15, 15, 1)
	FIELD(GPIO_PDERT0, P16, 16, 1)
	FIELD(GPIO_PDERT0, P17, 17, 1)
	FIELD(GPIO_PDERT0, P18, 18, 1)
	FIELD(GPIO_PDERT0, P19, 19, 1)
	FIELD(GPIO_PDERT0, P20, 20, 1)
	FIELD(GPIO_PDERT0, P21, 21, 1)
	FIELD(GPIO_PDERT0, P22, 22, 1)
	FIELD(GPIO_PDERT0, P23, 23, 1)
	FIELD(GPIO_PDERT0, P24, 24, 1)
	FIELD(GPIO_PDERT0, P25, 25, 1)
	FIELD(GPIO_PDERT0, P26, 26, 1)
	FIELD(GPIO_PDERT0, P27, 27, 1)
	FIELD(GPIO_PDERT0, P28, 28, 1)
	FIELD(GPIO_PDERT0, P29, 29, 1)
	FIELD(GPIO_PDERT0, P30, 30, 1)
	FIELD(GPIO_PDERT0, P31, 31, 1)
REG32(GPIO_PDERT1, 0x28c)
	FIELD(GPIO_PDERT1, P0, 0, 1)
	FIELD(GPIO_PDERT1, P1, 1, 1)
	FIELD(GPIO_PDERT1, P2, 2, 1)
	FIELD(GPIO_PDERT1, P3, 3, 1)
	FIELD(GPIO_PDERT1, P4, 4, 1)
	FIELD(GPIO_PDERT1, P5, 5, 1)
	FIELD(GPIO_PDERT1, P6, 6, 1)
	FIELD(GPIO_PDERT1, P7, 7, 1)
	FIELD(GPIO_PDERT1, P8, 8, 1)
	FIELD(GPIO_PDERT1, P9, 9, 1)
	FIELD(GPIO_PDERT1, P10, 10, 1)
	FIELD(GPIO_PDERT1, P11, 11, 1)
	FIELD(GPIO_PDERT1, P12, 12, 1)
	FIELD(GPIO_PDERT1, P13, 13, 1)
	FIELD(GPIO_PDERT1, P14, 14, 1)
	FIELD(GPIO_PDERT1, P15, 15, 1)
	FIELD(GPIO_PDERT1, P16, 16, 1)
	FIELD(GPIO_PDERT1, P17, 17, 1)
	FIELD(GPIO_PDERT1, P18, 18, 1)
	FIELD(GPIO_PDERT1, P19, 19, 1)
	FIELD(GPIO_PDERT1, P20, 20, 1)
	FIELD(GPIO_PDERT1, P21, 21, 1)
	FIELD(GPIO_PDERT1, P22, 22, 1)
	FIELD(GPIO_PDERT1, P23, 23, 1)
	FIELD(GPIO_PDERT1, P24, 24, 1)
	FIELD(GPIO_PDERT1, P25, 25, 1)
	FIELD(GPIO_PDERT1, P26, 26, 1)
	FIELD(GPIO_PDERT1, P27, 27, 1)
	FIELD(GPIO_PDERT1, P28, 28, 1)
	FIELD(GPIO_PDERT1, P29, 29, 1)
	FIELD(GPIO_PDERT1, P30, 30, 1)
	FIELD(GPIO_PDERT1, P31, 31, 1)
REG32(GPIO_PDERT2, 0x48c)
	FIELD(GPIO_PDERT2, P0, 0, 1)
	FIELD(GPIO_PDERT2, P1, 1, 1)
	FIELD(GPIO_PDERT2, P2, 2, 1)
	FIELD(GPIO_PDERT2, P3, 3, 1)
	FIELD(GPIO_PDERT2, P4, 4, 1)
	FIELD(GPIO_PDERT2, P5, 5, 1)
	FIELD(GPIO_PDERT2, P6, 6, 1)
	FIELD(GPIO_PDERT2, P7, 7, 1)
	FIELD(GPIO_PDERT2, P8, 8, 1)
	FIELD(GPIO_PDERT2, P9, 9, 1)
	FIELD(GPIO_PDERT2, P10, 10, 1)
	FIELD(GPIO_PDERT2, P11, 11, 1)
	FIELD(GPIO_PDERT2, P12, 12, 1)
	FIELD(GPIO_PDERT2, P13, 13, 1)
	FIELD(GPIO_PDERT2, P14, 14, 1)
	FIELD(GPIO_PDERT2, P15, 15, 1)
	FIELD(GPIO_PDERT2, P16, 16, 1)
	FIELD(GPIO_PDERT2, P17, 17, 1)
	FIELD(GPIO_PDERT2, P18, 18, 1)
	FIELD(GPIO_PDERT2, P19, 19, 1)
	FIELD(GPIO_PDERT2, P20, 20, 1)
	FIELD(GPIO_PDERT2, P21, 21, 1)
	FIELD(GPIO_PDERT2, P22, 22, 1)
	FIELD(GPIO_PDERT2, P23, 23, 1)
	FIELD(GPIO_PDERT2, P24, 24, 1)
	FIELD(GPIO_PDERT2, P25, 25, 1)
	FIELD(GPIO_PDERT2, P26, 26, 1)
	FIELD(GPIO_PDERT2, P27, 27, 1)
	FIELD(GPIO_PDERT2, P28, 28, 1)
	FIELD(GPIO_PDERT2, P29, 29, 1)
	FIELD(GPIO_PDERT2, P30, 30, 1)
	FIELD(GPIO_PDERT2, P31, 31, 1)
REG32(GPIO_PDER0, 0x80)
	FIELD(GPIO_PDER0, P0, 0, 1)
	FIELD(GPIO_PDER0, P1, 1, 1)
	FIELD(GPIO_PDER0, P2, 2, 1)
	FIELD(GPIO_PDER0, P3, 3, 1)
	FIELD(GPIO_PDER0, P4, 4, 1)
	FIELD(GPIO_PDER0, P5, 5, 1)
	FIELD(GPIO_PDER0, P6, 6, 1)
	FIELD(GPIO_PDER0, P7, 7, 1)
	FIELD(GPIO_PDER0, P8, 8, 1)
	FIELD(GPIO_PDER0, P9, 9, 1)
	FIELD(GPIO_PDER0, P10, 10, 1)
	FIELD(GPIO_PDER0, P11, 11, 1)
	FIELD(GPIO_PDER0, P12, 12, 1)
	FIELD(GPIO_PDER0, P13, 13, 1)
	FIELD(GPIO_PDER0, P14, 14, 1)
	FIELD(GPIO_PDER0, P15, 15, 1)
	FIELD(GPIO_PDER0, P16, 16, 1)
	FIELD(GPIO_PDER0, P17, 17, 1)
	FIELD(GPIO_PDER0, P18, 18, 1)
	FIELD(GPIO_PDER0, P19, 19, 1)
	FIELD(GPIO_PDER0, P20, 20, 1)
	FIELD(GPIO_PDER0, P21, 21, 1)
	FIELD(GPIO_PDER0, P22, 22, 1)
	FIELD(GPIO_PDER0, P23, 23, 1)
	FIELD(GPIO_PDER0, P24, 24, 1)
	FIELD(GPIO_PDER0, P25, 25, 1)
	FIELD(GPIO_PDER0, P26, 26, 1)
	FIELD(GPIO_PDER0, P27, 27, 1)
	FIELD(GPIO_PDER0, P28, 28, 1)
	FIELD(GPIO_PDER0, P29, 29, 1)
	FIELD(GPIO_PDER0, P30, 30, 1)
	FIELD(GPIO_PDER0, P31, 31, 1)
REG32(GPIO_PDER1, 0x280)
	FIELD(GPIO_PDER1, P0, 0, 1)
	FIELD(GPIO_PDER1, P1, 1, 1)
	FIELD(GPIO_PDER1, P2, 2, 1)
	FIELD(GPIO_PDER1, P3, 3, 1)
	FIELD(GPIO_PDER1, P4, 4, 1)
	FIELD(GPIO_PDER1, P5, 5, 1)
	FIELD(GPIO_PDER1, P6, 6, 1)
	FIELD(GPIO_PDER1, P7, 7, 1)
	FIELD(GPIO_PDER1, P8, 8, 1)
	FIELD(GPIO_PDER1, P9, 9, 1)
	FIELD(GPIO_PDER1, P10, 10, 1)
	FIELD(GPIO_PDER1, P11, 11, 1)
	FIELD(GPIO_PDER1, P12, 12, 1)
	FIELD(GPIO_PDER1, P13, 13, 1)
	FIELD(GPIO_PDER1, P14, 14, 1)
	FIELD(GPIO_PDER1, P15, 15, 1)
	FIELD(GPIO_PDER1, P16, 16, 1)
	FIELD(GPIO_PDER1, P17, 17, 1)
	FIELD(GPIO_PDER1, P18, 18, 1)
	FIELD(GPIO_PDER1, P19, 19, 1)
	FIELD(GPIO_PDER1, P20, 20, 1)
	FIELD(GPIO_PDER1, P21, 21, 1)
	FIELD(GPIO_PDER1, P22, 22, 1)
	FIELD(GPIO_PDER1, P23, 23, 1)
	FIELD(GPIO_PDER1, P24, 24, 1)
	FIELD(GPIO_PDER1, P25, 25, 1)
	FIELD(GPIO_PDER1, P26, 26, 1)
	FIELD(GPIO_PDER1, P27, 27, 1)
	FIELD(GPIO_PDER1, P28, 28, 1)
	FIELD(GPIO_PDER1, P29, 29, 1)
	FIELD(GPIO_PDER1, P30, 30, 1)
	FIELD(GPIO_PDER1, P31, 31, 1)
REG32(GPIO_PDER2, 0x480)
	FIELD(GPIO_PDER2, P0, 0, 1)
	FIELD(GPIO_PDER2, P1, 1, 1)
	FIELD(GPIO_PDER2, P2, 2, 1)
	FIELD(GPIO_PDER2, P3, 3, 1)
	FIELD(GPIO_PDER2, P4, 4, 1)
	FIELD(GPIO_PDER2, P5, 5, 1)
	FIELD(GPIO_PDER2, P6, 6, 1)
	FIELD(GPIO_PDER2, P7, 7, 1)
	FIELD(GPIO_PDER2, P8, 8, 1)
	FIELD(GPIO_PDER2, P9, 9, 1)
	FIELD(GPIO_PDER2, P10, 10, 1)
	FIELD(GPIO_PDER2, P11, 11, 1)
	FIELD(GPIO_PDER2, P12, 12, 1)
	FIELD(GPIO_PDER2, P13, 13, 1)
	FIELD(GPIO_PDER2, P14, 14, 1)
	FIELD(GPIO_PDER2, P15, 15, 1)
	FIELD(GPIO_PDER2, P16, 16, 1)
	FIELD(GPIO_PDER2, P17, 17, 1)
	FIELD(GPIO_PDER2, P18, 18, 1)
	FIELD(GPIO_PDER2, P19, 19, 1)
	FIELD(GPIO_PDER2, P20, 20, 1)
	FIELD(GPIO_PDER2, P21, 21, 1)
	FIELD(GPIO_PDER2, P22, 22, 1)
	FIELD(GPIO_PDER2, P23, 23, 1)
	FIELD(GPIO_PDER2, P24, 24, 1)
	FIELD(GPIO_PDER2, P25, 25, 1)
	FIELD(GPIO_PDER2, P26, 26, 1)
	FIELD(GPIO_PDER2, P27, 27, 1)
	FIELD(GPIO_PDER2, P28, 28, 1)
	FIELD(GPIO_PDER2, P29, 29, 1)
	FIELD(GPIO_PDER2, P30, 30, 1)
	FIELD(GPIO_PDER2, P31, 31, 1)
REG32(GPIO_PMR0C0, 0x18)
	FIELD(GPIO_PMR0C0, P0, 0, 1)
	FIELD(GPIO_PMR0C0, P1, 1, 1)
	FIELD(GPIO_PMR0C0, P2, 2, 1)
	FIELD(GPIO_PMR0C0, P3, 3, 1)
	FIELD(GPIO_PMR0C0, P4, 4, 1)
	FIELD(GPIO_PMR0C0, P5, 5, 1)
	FIELD(GPIO_PMR0C0, P6, 6, 1)
	FIELD(GPIO_PMR0C0, P7, 7, 1)
	FIELD(GPIO_PMR0C0, P8, 8, 1)
	FIELD(GPIO_PMR0C0, P9, 9, 1)
	FIELD(GPIO_PMR0C0, P10, 10, 1)
	FIELD(GPIO_PMR0C0, P11, 11, 1)
	FIELD(GPIO_PMR0C0, P12, 12, 1)
	FIELD(GPIO_PMR0C0, P13, 13, 1)
	FIELD(GPIO_PMR0C0, P14, 14, 1)
	FIELD(GPIO_PMR0C0, P15, 15, 1)
	FIELD(GPIO_PMR0C0, P16, 16, 1)
	FIELD(GPIO_PMR0C0, P17, 17, 1)
	FIELD(GPIO_PMR0C0, P18, 18, 1)
	FIELD(GPIO_PMR0C0, P19, 19, 1)
	FIELD(GPIO_PMR0C0, P20, 20, 1)
	FIELD(GPIO_PMR0C0, P21, 21, 1)
	FIELD(GPIO_PMR0C0, P22, 22, 1)
	FIELD(GPIO_PMR0C0, P23, 23, 1)
	FIELD(GPIO_PMR0C0, P24, 24, 1)
	FIELD(GPIO_PMR0C0, P25, 25, 1)
	FIELD(GPIO_PMR0C0, P26, 26, 1)
	FIELD(GPIO_PMR0C0, P27, 27, 1)
	FIELD(GPIO_PMR0C0, P28, 28, 1)
	FIELD(GPIO_PMR0C0, P29, 29, 1)
	FIELD(GPIO_PMR0C0, P30, 30, 1)
	FIELD(GPIO_PMR0C0, P31, 31, 1)
REG32(GPIO_PMR0C1, 0x218)
	FIELD(GPIO_PMR0C1, P0, 0, 1)
	FIELD(GPIO_PMR0C1, P1, 1, 1)
	FIELD(GPIO_PMR0C1, P2, 2, 1)
	FIELD(GPIO_PMR0C1, P3, 3, 1)
	FIELD(GPIO_PMR0C1, P4, 4, 1)
	FIELD(GPIO_PMR0C1, P5, 5, 1)
	FIELD(GPIO_PMR0C1, P6, 6, 1)
	FIELD(GPIO_PMR0C1, P7, 7, 1)
	FIELD(GPIO_PMR0C1, P8, 8, 1)
	FIELD(GPIO_PMR0C1, P9, 9, 1)
	FIELD(GPIO_PMR0C1, P10, 10, 1)
	FIELD(GPIO_PMR0C1, P11, 11, 1)
	FIELD(GPIO_PMR0C1, P12, 12, 1)
	FIELD(GPIO_PMR0C1, P13, 13, 1)
	FIELD(GPIO_PMR0C1, P14, 14, 1)
	FIELD(GPIO_PMR0C1, P15, 15, 1)
	FIELD(GPIO_PMR0C1, P16, 16, 1)
	FIELD(GPIO_PMR0C1, P17, 17, 1)
	FIELD(GPIO_PMR0C1, P18, 18, 1)
	FIELD(GPIO_PMR0C1, P19, 19, 1)
	FIELD(GPIO_PMR0C1, P20, 20, 1)
	FIELD(GPIO_PMR0C1, P21, 21, 1)
	FIELD(GPIO_PMR0C1, P22, 22, 1)
	FIELD(GPIO_PMR0C1, P23, 23, 1)
	FIELD(GPIO_PMR0C1, P24, 24, 1)
	FIELD(GPIO_PMR0C1, P25, 25, 1)
	FIELD(GPIO_PMR0C1, P26, 26, 1)
	FIELD(GPIO_PMR0C1, P27, 27, 1)
	FIELD(GPIO_PMR0C1, P28, 28, 1)
	FIELD(GPIO_PMR0C1, P29, 29, 1)
	FIELD(GPIO_PMR0C1, P30, 30, 1)
	FIELD(GPIO_PMR0C1, P31, 31, 1)
REG32(GPIO_PMR0C2, 0x418)
	FIELD(GPIO_PMR0C2, P0, 0, 1)
	FIELD(GPIO_PMR0C2, P1, 1, 1)
	FIELD(GPIO_PMR0C2, P2, 2, 1)
	FIELD(GPIO_PMR0C2, P3, 3, 1)
	FIELD(GPIO_PMR0C2, P4, 4, 1)
	FIELD(GPIO_PMR0C2, P5, 5, 1)
	FIELD(GPIO_PMR0C2, P6, 6, 1)
	FIELD(GPIO_PMR0C2, P7, 7, 1)
	FIELD(GPIO_PMR0C2, P8, 8, 1)
	FIELD(GPIO_PMR0C2, P9, 9, 1)
	FIELD(GPIO_PMR0C2, P10, 10, 1)
	FIELD(GPIO_PMR0C2, P11, 11, 1)
	FIELD(GPIO_PMR0C2, P12, 12, 1)
	FIELD(GPIO_PMR0C2, P13, 13, 1)
	FIELD(GPIO_PMR0C2, P14, 14, 1)
	FIELD(GPIO_PMR0C2, P15, 15, 1)
	FIELD(GPIO_PMR0C2, P16, 16, 1)
	FIELD(GPIO_PMR0C2, P17, 17, 1)
	FIELD(GPIO_PMR0C2, P18, 18, 1)
	FIELD(GPIO_PMR0C2, P19, 19, 1)
	FIELD(GPIO_PMR0C2, P20, 20, 1)
	FIELD(GPIO_PMR0C2, P21, 21, 1)
	FIELD(GPIO_PMR0C2, P22, 22, 1)
	FIELD(GPIO_PMR0C2, P23, 23, 1)
	FIELD(GPIO_PMR0C2, P24, 24, 1)
	FIELD(GPIO_PMR0C2, P25, 25, 1)
	FIELD(GPIO_PMR0C2, P26, 26, 1)
	FIELD(GPIO_PMR0C2, P27, 27, 1)
	FIELD(GPIO_PMR0C2, P28, 28, 1)
	FIELD(GPIO_PMR0C2, P29, 29, 1)
	FIELD(GPIO_PMR0C2, P30, 30, 1)
	FIELD(GPIO_PMR0C2, P31, 31, 1)
REG32(GPIO_PMR0S0, 0x14)
	FIELD(GPIO_PMR0S0, P0, 0, 1)
	FIELD(GPIO_PMR0S0, P1, 1, 1)
	FIELD(GPIO_PMR0S0, P2, 2, 1)
	FIELD(GPIO_PMR0S0, P3, 3, 1)
	FIELD(GPIO_PMR0S0, P4, 4, 1)
	FIELD(GPIO_PMR0S0, P5, 5, 1)
	FIELD(GPIO_PMR0S0, P6, 6, 1)
	FIELD(GPIO_PMR0S0, P7, 7, 1)
	FIELD(GPIO_PMR0S0, P8, 8, 1)
	FIELD(GPIO_PMR0S0, P9, 9, 1)
	FIELD(GPIO_PMR0S0, P10, 10, 1)
	FIELD(GPIO_PMR0S0, P11, 11, 1)
	FIELD(GPIO_PMR0S0, P12, 12, 1)
	FIELD(GPIO_PMR0S0, P13, 13, 1)
	FIELD(GPIO_PMR0S0, P14, 14, 1)
	FIELD(GPIO_PMR0S0, P15, 15, 1)
	FIELD(GPIO_PMR0S0, P16, 16, 1)
	FIELD(GPIO_PMR0S0, P17, 17, 1)
	FIELD(GPIO_PMR0S0, P18, 18, 1)
	FIELD(GPIO_PMR0S0, P19, 19, 1)
	FIELD(GPIO_PMR0S0, P20, 20, 1)
	FIELD(GPIO_PMR0S0, P21, 21, 1)
	FIELD(GPIO_PMR0S0, P22, 22, 1)
	FIELD(GPIO_PMR0S0, P23, 23, 1)
	FIELD(GPIO_PMR0S0, P24, 24, 1)
	FIELD(GPIO_PMR0S0, P25, 25, 1)
	FIELD(GPIO_PMR0S0, P26, 26, 1)
	FIELD(GPIO_PMR0S0, P27, 27, 1)
	FIELD(GPIO_PMR0S0, P28, 28, 1)
	FIELD(GPIO_PMR0S0, P29, 29, 1)
	FIELD(GPIO_PMR0S0, P30, 30, 1)
	FIELD(GPIO_PMR0S0, P31, 31, 1)
REG32(GPIO_PMR0S1, 0x214)
	FIELD(GPIO_PMR0S1, P0, 0, 1)
	FIELD(GPIO_PMR0S1, P1, 1, 1)
	FIELD(GPIO_PMR0S1, P2, 2, 1)
	FIELD(GPIO_PMR0S1, P3, 3, 1)
	FIELD(GPIO_PMR0S1, P4, 4, 1)
	FIELD(GPIO_PMR0S1, P5, 5, 1)
	FIELD(GPIO_PMR0S1, P6, 6, 1)
	FIELD(GPIO_PMR0S1, P7, 7, 1)
	FIELD(GPIO_PMR0S1, P8, 8, 1)
	FIELD(GPIO_PMR0S1, P9, 9, 1)
	FIELD(GPIO_PMR0S1, P10, 10, 1)
	FIELD(GPIO_PMR0S1, P11, 11, 1)
	FIELD(GPIO_PMR0S1, P12, 12, 1)
	FIELD(GPIO_PMR0S1, P13, 13, 1)
	FIELD(GPIO_PMR0S1, P14, 14, 1)
	FIELD(GPIO_PMR0S1, P15, 15, 1)
	FIELD(GPIO_PMR0S1, P16, 16, 1)
	FIELD(GPIO_PMR0S1, P17, 17, 1)
	FIELD(GPIO_PMR0S1, P18, 18, 1)
	FIELD(GPIO_PMR0S1, P19, 19, 1)
	FIELD(GPIO_PMR0S1, P20, 20, 1)
	FIELD(GPIO_PMR0S1, P21, 21, 1)
	FIELD(GPIO_PMR0S1, P22, 22, 1)
	FIELD(GPIO_PMR0S1, P23, 23, 1)
	FIELD(GPIO_PMR0S1, P24, 24, 1)
	FIELD(GPIO_PMR0S1, P25, 25, 1)
	FIELD(GPIO_PMR0S1, P26, 26, 1)
	FIELD(GPIO_PMR0S1, P27, 27, 1)
	FIELD(GPIO_PMR0S1, P28, 28, 1)
	FIELD(GPIO_PMR0S1, P29, 29, 1)
	FIELD(GPIO_PMR0S1, P30, 30, 1)
	FIELD(GPIO_PMR0S1, P31, 31, 1)
REG32(GPIO_PMR0S2, 0x414)
	FIELD(GPIO_PMR0S2, P0, 0, 1)
	FIELD(GPIO_PMR0S2, P1, 1, 1)
	FIELD(GPIO_PMR0S2, P2, 2, 1)
	FIELD(GPIO_PMR0S2, P3, 3, 1)
	FIELD(GPIO_PMR0S2, P4, 4, 1)
	FIELD(GPIO_PMR0S2, P5, 5, 1)
	FIELD(GPIO_PMR0S2, P6, 6, 1)
	FIELD(GPIO_PMR0S2, P7, 7, 1)
	FIELD(GPIO_PMR0S2, P8, 8, 1)
	FIELD(GPIO_PMR0S2, P9, 9, 1)
	FIELD(GPIO_PMR0S2, P10, 10, 1)
	FIELD(GPIO_PMR0S2, P11, 11, 1)
	FIELD(GPIO_PMR0S2, P12, 12, 1)
	FIELD(GPIO_PMR0S2, P13, 13, 1)
	FIELD(GPIO_PMR0S2, P14, 14, 1)
	FIELD(GPIO_PMR0S2, P15, 15, 1)
	FIELD(GPIO_PMR0S2, P16, 16, 1)
	FIELD(GPIO_PMR0S2, P17, 17, 1)
	FIELD(GPIO_PMR0S2, P18, 18, 1)
	FIELD(GPIO_PMR0S2, P19, 19, 1)
	FIELD(GPIO_PMR0S2, P20, 20, 1)
	FIELD(GPIO_PMR0S2, P21, 21, 1)
	FIELD(GPIO_PMR0S2, P22, 22, 1)
	FIELD(GPIO_PMR0S2, P23, 23, 1)
	FIELD(GPIO_PMR0S2, P24, 24, 1)
	FIELD(GPIO_PMR0S2, P25, 25, 1)
	FIELD(GPIO_PMR0S2, P26, 26, 1)
	FIELD(GPIO_PMR0S2, P27, 27, 1)
	FIELD(GPIO_PMR0S2, P28, 28, 1)
	FIELD(GPIO_PMR0S2, P29, 29, 1)
	FIELD(GPIO_PMR0S2, P30, 30, 1)
	FIELD(GPIO_PMR0S2, P31, 31, 1)
REG32(GPIO_PMR0T0, 0x1c)
	FIELD(GPIO_PMR0T0, P0, 0, 1)
	FIELD(GPIO_PMR0T0, P1, 1, 1)
	FIELD(GPIO_PMR0T0, P2, 2, 1)
	FIELD(GPIO_PMR0T0, P3, 3, 1)
	FIELD(GPIO_PMR0T0, P4, 4, 1)
	FIELD(GPIO_PMR0T0, P5, 5, 1)
	FIELD(GPIO_PMR0T0, P6, 6, 1)
	FIELD(GPIO_PMR0T0, P7, 7, 1)
	FIELD(GPIO_PMR0T0, P8, 8, 1)
	FIELD(GPIO_PMR0T0, P9, 9, 1)
	FIELD(GPIO_PMR0T0, P10, 10, 1)
	FIELD(GPIO_PMR0T0, P11, 11, 1)
	FIELD(GPIO_PMR0T0, P12, 12, 1)
	FIELD(GPIO_PMR0T0, P13, 13, 1)
	FIELD(GPIO_PMR0T0, P14, 14, 1)
	FIELD(GPIO_PMR0T0, P15, 15, 1)
	FIELD(GPIO_PMR0T0, P16, 16, 1)
	FIELD(GPIO_PMR0T0, P17, 17, 1)
	FIELD(GPIO_PMR0T0, P18, 18, 1)
	FIELD(GPIO_PMR0T0, P19, 19, 1)
	FIELD(GPIO_PMR0T0, P20, 20, 1)
	FIELD(GPIO_PMR0T0, P21, 21, 1)
	FIELD(GPIO_PMR0T0, P22, 22, 1)
	FIELD(GPIO_PMR0T0, P23, 23, 1)
	FIELD(GPIO_PMR0T0, P24, 24, 1)
	FIELD(GPIO_PMR0T0, P25, 25, 1)
	FIELD(GPIO_PMR0T0, P26, 26, 1)
	FIELD(GPIO_PMR0T0, P27, 27, 1)
	FIELD(GPIO_PMR0T0, P28, 28, 1)
	FIELD(GPIO_PMR0T0, P29, 29, 1)
	FIELD(GPIO_PMR0T0, P30, 30, 1)
	FIELD(GPIO_PMR0T0, P31, 31, 1)
REG32(GPIO_PMR0T1, 0x21c)
	FIELD(GPIO_PMR0T1, P0, 0, 1)
	FIELD(GPIO_PMR0T1, P1, 1, 1)
	FIELD(GPIO_PMR0T1, P2, 2, 1)
	FIELD(GPIO_PMR0T1, P3, 3, 1)
	FIELD(GPIO_PMR0T1, P4, 4, 1)
	FIELD(GPIO_PMR0T1, P5, 5, 1)
	FIELD(GPIO_PMR0T1, P6, 6, 1)
	FIELD(GPIO_PMR0T1, P7, 7, 1)
	FIELD(GPIO_PMR0T1, P8, 8, 1)
	FIELD(GPIO_PMR0T1, P9, 9, 1)
	FIELD(GPIO_PMR0T1, P10, 10, 1)
	FIELD(GPIO_PMR0T1, P11, 11, 1)
	FIELD(GPIO_PMR0T1, P12, 12, 1)
	FIELD(GPIO_PMR0T1, P13, 13, 1)
	FIELD(GPIO_PMR0T1, P14, 14, 1)
	FIELD(GPIO_PMR0T1, P15, 15, 1)
	FIELD(GPIO_PMR0T1, P16, 16, 1)
	FIELD(GPIO_PMR0T1, P17, 17, 1)
	FIELD(GPIO_PMR0T1, P18, 18, 1)
	FIELD(GPIO_PMR0T1, P19, 19, 1)
	FIELD(GPIO_PMR0T1, P20, 20, 1)
	FIELD(GPIO_PMR0T1, P21, 21, 1)
	FIELD(GPIO_PMR0T1, P22, 22, 1)
	FIELD(GPIO_PMR0T1, P23, 23, 1)
	FIELD(GPIO_PMR0T1, P24, 24, 1)
	FIELD(GPIO_PMR0T1, P25, 25, 1)
	FIELD(GPIO_PMR0T1, P26, 26, 1)
	FIELD(GPIO_PMR0T1, P27, 27, 1)
	FIELD(GPIO_PMR0T1, P28, 28, 1)
	FIELD(GPIO_PMR0T1, P29, 29, 1)
	FIELD(GPIO_PMR0T1, P30, 30, 1)
	FIELD(GPIO_PMR0T1, P31, 31, 1)
REG32(GPIO_PMR0T2, 0x41c)
	FIELD(GPIO_PMR0T2, P0, 0, 1)
	FIELD(GPIO_PMR0T2, P1, 1, 1)
	FIELD(GPIO_PMR0T2, P2, 2, 1)
	FIELD(GPIO_PMR0T2, P3, 3, 1)
	FIELD(GPIO_PMR0T2, P4, 4, 1)
	FIELD(GPIO_PMR0T2, P5, 5, 1)
	FIELD(GPIO_PMR0T2, P6, 6, 1)
	FIELD(GPIO_PMR0T2, P7, 7, 1)
	FIELD(GPIO_PMR0T2, P8, 8, 1)
	FIELD(GPIO_PMR0T2, P9, 9, 1)
	FIELD(GPIO_PMR0T2, P10, 10, 1)
	FIELD(GPIO_PMR0T2, P11, 11, 1)
	FIELD(GPIO_PMR0T2, P12, 12, 1)
	FIELD(GPIO_PMR0T2, P13, 13, 1)
	FIELD(GPIO_PMR0T2, P14, 14, 1)
	FIELD(GPIO_PMR0T2, P15, 15, 1)
	FIELD(GPIO_PMR0T2, P16, 16, 1)
	FIELD(GPIO_PMR0T2, P17, 17, 1)
	FIELD(GPIO_PMR0T2, P18, 18, 1)
	FIELD(GPIO_PMR0T2, P19, 19, 1)
	FIELD(GPIO_PMR0T2, P20, 20, 1)
	FIELD(GPIO_PMR0T2, P21, 21, 1)
	FIELD(GPIO_PMR0T2, P22, 22, 1)
	FIELD(GPIO_PMR0T2, P23, 23, 1)
	FIELD(GPIO_PMR0T2, P24, 24, 1)
	FIELD(GPIO_PMR0T2, P25, 25, 1)
	FIELD(GPIO_PMR0T2, P26, 26, 1)
	FIELD(GPIO_PMR0T2, P27, 27, 1)
	FIELD(GPIO_PMR0T2, P28, 28, 1)
	FIELD(GPIO_PMR0T2, P29, 29, 1)
	FIELD(GPIO_PMR0T2, P30, 30, 1)
	FIELD(GPIO_PMR0T2, P31, 31, 1)
REG32(GPIO_PMR1C0, 0x28)
	FIELD(GPIO_PMR1C0, P0, 0, 1)
	FIELD(GPIO_PMR1C0, P1, 1, 1)
	FIELD(GPIO_PMR1C0, P2, 2, 1)
	FIELD(GPIO_PMR1C0, P3, 3, 1)
	FIELD(GPIO_PMR1C0, P4, 4, 1)
	FIELD(GPIO_PMR1C0, P5, 5, 1)
	FIELD(GPIO_PMR1C0, P6, 6, 1)
	FIELD(GPIO_PMR1C0, P7, 7, 1)
	FIELD(GPIO_PMR1C0, P8, 8, 1)
	FIELD(GPIO_PMR1C0, P9, 9, 1)
	FIELD(GPIO_PMR1C0, P10, 10, 1)
	FIELD(GPIO_PMR1C0, P11, 11, 1)
	FIELD(GPIO_PMR1C0, P12, 12, 1)
	FIELD(GPIO_PMR1C0, P13, 13, 1)
	FIELD(GPIO_PMR1C0, P14, 14, 1)
	FIELD(GPIO_PMR1C0, P15, 15, 1)
	FIELD(GPIO_PMR1C0, P16, 16, 1)
	FIELD(GPIO_PMR1C0, P17, 17, 1)
	FIELD(GPIO_PMR1C0, P18, 18, 1)
	FIELD(GPIO_PMR1C0, P19, 19, 1)
	FIELD(GPIO_PMR1C0, P20, 20, 1)
	FIELD(GPIO_PMR1C0, P21, 21, 1)
	FIELD(GPIO_PMR1C0, P22, 22, 1)
	FIELD(GPIO_PMR1C0, P23, 23, 1)
	FIELD(GPIO_PMR1C0, P24, 24, 1)
	FIELD(GPIO_PMR1C0, P25, 25, 1)
	FIELD(GPIO_PMR1C0, P26, 26, 1)
	FIELD(GPIO_PMR1C0, P27, 27, 1)
	FIELD(GPIO_PMR1C0, P28, 28, 1)
	FIELD(GPIO_PMR1C0, P29, 29, 1)
	FIELD(GPIO_PMR1C0, P30, 30, 1)
	FIELD(GPIO_PMR1C0, P31, 31, 1)
REG32(GPIO_PMR1C1, 0x228)
	FIELD(GPIO_PMR1C1, P0, 0, 1)
	FIELD(GPIO_PMR1C1, P1, 1, 1)
	FIELD(GPIO_PMR1C1, P2, 2, 1)
	FIELD(GPIO_PMR1C1, P3, 3, 1)
	FIELD(GPIO_PMR1C1, P4, 4, 1)
	FIELD(GPIO_PMR1C1, P5, 5, 1)
	FIELD(GPIO_PMR1C1, P6, 6, 1)
	FIELD(GPIO_PMR1C1, P7, 7, 1)
	FIELD(GPIO_PMR1C1, P8, 8, 1)
	FIELD(GPIO_PMR1C1, P9, 9, 1)
	FIELD(GPIO_PMR1C1, P10, 10, 1)
	FIELD(GPIO_PMR1C1, P11, 11, 1)
	FIELD(GPIO_PMR1C1, P12, 12, 1)
	FIELD(GPIO_PMR1C1, P13, 13, 1)
	FIELD(GPIO_PMR1C1, P14, 14, 1)
	FIELD(GPIO_PMR1C1, P15, 15, 1)
	FIELD(GPIO_PMR1C1, P16, 16, 1)
	FIELD(GPIO_PMR1C1, P17, 17, 1)
	FIELD(GPIO_PMR1C1, P18, 18, 1)
	FIELD(GPIO_PMR1C1, P19, 19, 1)
	FIELD(GPIO_PMR1C1, P20, 20, 1)
	FIELD(GPIO_PMR1C1, P21, 21, 1)
	FIELD(GPIO_PMR1C1, P22, 22, 1)
	FIELD(GPIO_PMR1C1, P23, 23, 1)
	FIELD(GPIO_PMR1C1, P24, 24, 1)
	FIELD(GPIO_PMR1C1, P25, 25, 1)
	FIELD(GPIO_PMR1C1, P26, 26, 1)
	FIELD(GPIO_PMR1C1, P27, 27, 1)
	FIELD(GPIO_PMR1C1, P28, 28, 1)
	FIELD(GPIO_PMR1C1, P29, 29, 1)
	FIELD(GPIO_PMR1C1, P30, 30, 1)
	FIELD(GPIO_PMR1C1, P31, 31, 1)
REG32(GPIO_PMR1C2, 0x428)
	FIELD(GPIO_PMR1C2, P0, 0, 1)
	FIELD(GPIO_PMR1C2, P1, 1, 1)
	FIELD(GPIO_PMR1C2, P2, 2, 1)
	FIELD(GPIO_PMR1C2, P3, 3, 1)
	FIELD(GPIO_PMR1C2, P4, 4, 1)
	FIELD(GPIO_PMR1C2, P5, 5, 1)
	FIELD(GPIO_PMR1C2, P6, 6, 1)
	FIELD(GPIO_PMR1C2, P7, 7, 1)
	FIELD(GPIO_PMR1C2, P8, 8, 1)
	FIELD(GPIO_PMR1C2, P9, 9, 1)
	FIELD(GPIO_PMR1C2, P10, 10, 1)
	FIELD(GPIO_PMR1C2, P11, 11, 1)
	FIELD(GPIO_PMR1C2, P12, 12, 1)
	FIELD(GPIO_PMR1C2, P13, 13, 1)
	FIELD(GPIO_PMR1C2, P14, 14, 1)
	FIELD(GPIO_PMR1C2, P15, 15, 1)
	FIELD(GPIO_PMR1C2, P16, 16, 1)
	FIELD(GPIO_PMR1C2, P17, 17, 1)
	FIELD(GPIO_PMR1C2, P18, 18, 1)
	FIELD(GPIO_PMR1C2, P19, 19, 1)
	FIELD(GPIO_PMR1C2, P20, 20, 1)
	FIELD(GPIO_PMR1C2, P21, 21, 1)
	FIELD(GPIO_PMR1C2, P22, 22, 1)
	FIELD(GPIO_PMR1C2, P23, 23, 1)
	FIELD(GPIO_PMR1C2, P24, 24, 1)
	FIELD(GPIO_PMR1C2, P25, 25, 1)
	FIELD(GPIO_PMR1C2, P26, 26, 1)
	FIELD(GPIO_PMR1C2, P27, 27, 1)
	FIELD(GPIO_PMR1C2, P28, 28, 1)
	FIELD(GPIO_PMR1C2, P29, 29, 1)
	FIELD(GPIO_PMR1C2, P30, 30, 1)
	FIELD(GPIO_PMR1C2, P31, 31, 1)
REG32(GPIO_PMR1S0, 0x24)
	FIELD(GPIO_PMR1S0, P0, 0, 1)
	FIELD(GPIO_PMR1S0, P1, 1, 1)
	FIELD(GPIO_PMR1S0, P2, 2, 1)
	FIELD(GPIO_PMR1S0, P3, 3, 1)
	FIELD(GPIO_PMR1S0, P4, 4, 1)
	FIELD(GPIO_PMR1S0, P5, 5, 1)
	FIELD(GPIO_PMR1S0, P6, 6, 1)
	FIELD(GPIO_PMR1S0, P7, 7, 1)
	FIELD(GPIO_PMR1S0, P8, 8, 1)
	FIELD(GPIO_PMR1S0, P9, 9, 1)
	FIELD(GPIO_PMR1S0, P10, 10, 1)
	FIELD(GPIO_PMR1S0, P11, 11, 1)
	FIELD(GPIO_PMR1S0, P12, 12, 1)
	FIELD(GPIO_PMR1S0, P13, 13, 1)
	FIELD(GPIO_PMR1S0, P14, 14, 1)
	FIELD(GPIO_PMR1S0, P15, 15, 1)
	FIELD(GPIO_PMR1S0, P16, 16, 1)
	FIELD(GPIO_PMR1S0, P17, 17, 1)
	FIELD(GPIO_PMR1S0, P18, 18, 1)
	FIELD(GPIO_PMR1S0, P19, 19, 1)
	FIELD(GPIO_PMR1S0, P20, 20, 1)
	FIELD(GPIO_PMR1S0, P21, 21, 1)
	FIELD(GPIO_PMR1S0, P22, 22, 1)
	FIELD(GPIO_PMR1S0, P23, 23, 1)
	FIELD(GPIO_PMR1S0, P24, 24, 1)
	FIELD(GPIO_PMR1S0, P25, 25, 1)
	FIELD(GPIO_PMR1S0, P26, 26, 1)
	FIELD(GPIO_PMR1S0, P27, 27, 1)
	FIELD(GPIO_PMR1S0, P28, 28, 1)
	FIELD(GPIO_PMR1S0, P29, 29, 1)
	FIELD(GPIO_PMR1S0, P30, 30, 1)
	FIELD(GPIO_PMR1S0, P31, 31, 1)
REG32(GPIO_PMR1S1, 0x224)
	FIELD(GPIO_PMR1S1, P0, 0, 1)
	FIELD(GPIO_PMR1S1, P1, 1, 1)
	FIELD(GPIO_PMR1S1, P2, 2, 1)
	FIELD(GPIO_PMR1S1, P3, 3, 1)
	FIELD(GPIO_PMR1S1, P4, 4, 1)
	FIELD(GPIO_PMR1S1, P5, 5, 1)
	FIELD(GPIO_PMR1S1, P6, 6, 1)
	FIELD(GPIO_PMR1S1, P7, 7, 1)
	FIELD(GPIO_PMR1S1, P8, 8, 1)
	FIELD(GPIO_PMR1S1, P9, 9, 1)
	FIELD(GPIO_PMR1S1, P10, 10, 1)
	FIELD(GPIO_PMR1S1, P11, 11, 1)
	FIELD(GPIO_PMR1S1, P12, 12, 1)
	FIELD(GPIO_PMR1S1, P13, 13, 1)
	FIELD(GPIO_PMR1S1, P14, 14, 1)
	FIELD(GPIO_PMR1S1, P15, 15, 1)
	FIELD(GPIO_PMR1S1, P16, 16, 1)
	FIELD(GPIO_PMR1S1, P17, 17, 1)
	FIELD(GPIO_PMR1S1, P18, 18, 1)
	FIELD(GPIO_PMR1S1, P19, 19, 1)
	FIELD(GPIO_PMR1S1, P20, 20, 1)
	FIELD(GPIO_PMR1S1, P21, 21, 1)
	FIELD(GPIO_PMR1S1, P22, 22, 1)
	FIELD(GPIO_PMR1S1, P23, 23, 1)
	FIELD(GPIO_PMR1S1, P24, 24, 1)
	FIELD(GPIO_PMR1S1, P25, 25, 1)
	FIELD(GPIO_PMR1S1, P26, 26, 1)
	FIELD(GPIO_PMR1S1, P27, 27, 1)
	FIELD(GPIO_PMR1S1, P28, 28, 1)
	FIELD(GPIO_PMR1S1, P29, 29, 1)
	FIELD(GPIO_PMR1S1, P30, 30, 1)
	FIELD(GPIO_PMR1S1, P31, 31, 1)
REG32(GPIO_PMR1S2, 0x424)
	FIELD(GPIO_PMR1S2, P0, 0, 1)
	FIELD(GPIO_PMR1S2, P1, 1, 1)
	FIELD(GPIO_PMR1S2, P2, 2, 1)
	FIELD(GPIO_PMR1S2, P3, 3, 1)
	FIELD(GPIO_PMR1S2, P4, 4, 1)
	FIELD(GPIO_PMR1S2, P5, 5, 1)
	FIELD(GPIO_PMR1S2, P6, 6, 1)
	FIELD(GPIO_PMR1S2, P7, 7, 1)
	FIELD(GPIO_PMR1S2, P8, 8, 1)
	FIELD(GPIO_PMR1S2, P9, 9, 1)
	FIELD(GPIO_PMR1S2, P10, 10, 1)
	FIELD(GPIO_PMR1S2, P11, 11, 1)
	FIELD(GPIO_PMR1S2, P12, 12, 1)
	FIELD(GPIO_PMR1S2, P13, 13, 1)
	FIELD(GPIO_PMR1S2, P14, 14, 1)
	FIELD(GPIO_PMR1S2, P15, 15, 1)
	FIELD(GPIO_PMR1S2, P16, 16, 1)
	FIELD(GPIO_PMR1S2, P17, 17, 1)
	FIELD(GPIO_PMR1S2, P18, 18, 1)
	FIELD(GPIO_PMR1S2, P19, 19, 1)
	FIELD(GPIO_PMR1S2, P20, 20, 1)
	FIELD(GPIO_PMR1S2, P21, 21, 1)
	FIELD(GPIO_PMR1S2, P22, 22, 1)
	FIELD(GPIO_PMR1S2, P23, 23, 1)
	FIELD(GPIO_PMR1S2, P24, 24, 1)
	FIELD(GPIO_PMR1S2, P25, 25, 1)
	FIELD(GPIO_PMR1S2, P26, 26, 1)
	FIELD(GPIO_PMR1S2, P27, 27, 1)
	FIELD(GPIO_PMR1S2, P28, 28, 1)
	FIELD(GPIO_PMR1S2, P29, 29, 1)
	FIELD(GPIO_PMR1S2, P30, 30, 1)
	FIELD(GPIO_PMR1S2, P31, 31, 1)
REG32(GPIO_PMR1T0, 0x2c)
	FIELD(GPIO_PMR1T0, P0, 0, 1)
	FIELD(GPIO_PMR1T0, P1, 1, 1)
	FIELD(GPIO_PMR1T0, P2, 2, 1)
	FIELD(GPIO_PMR1T0, P3, 3, 1)
	FIELD(GPIO_PMR1T0, P4, 4, 1)
	FIELD(GPIO_PMR1T0, P5, 5, 1)
	FIELD(GPIO_PMR1T0, P6, 6, 1)
	FIELD(GPIO_PMR1T0, P7, 7, 1)
	FIELD(GPIO_PMR1T0, P8, 8, 1)
	FIELD(GPIO_PMR1T0, P9, 9, 1)
	FIELD(GPIO_PMR1T0, P10, 10, 1)
	FIELD(GPIO_PMR1T0, P11, 11, 1)
	FIELD(GPIO_PMR1T0, P12, 12, 1)
	FIELD(GPIO_PMR1T0, P13, 13, 1)
	FIELD(GPIO_PMR1T0, P14, 14, 1)
	FIELD(GPIO_PMR1T0, P15, 15, 1)
	FIELD(GPIO_PMR1T0, P16, 16, 1)
	FIELD(GPIO_PMR1T0, P17, 17, 1)
	FIELD(GPIO_PMR1T0, P18, 18, 1)
	FIELD(GPIO_PMR1T0, P19, 19, 1)
	FIELD(GPIO_PMR1T0, P20, 20, 1)
	FIELD(GPIO_PMR1T0, P21, 21, 1)
	FIELD(GPIO_PMR1T0, P22, 22, 1)
	FIELD(GPIO_PMR1T0, P23, 23, 1)
	FIELD(GPIO_PMR1T0, P24, 24, 1)
	FIELD(GPIO_PMR1T0, P25, 25, 1)
	FIELD(GPIO_PMR1T0, P26, 26, 1)
	FIELD(GPIO_PMR1T0, P27, 27, 1)
	FIELD(GPIO_PMR1T0, P28, 28, 1)
	FIELD(GPIO_PMR1T0, P29, 29, 1)
	FIELD(GPIO_PMR1T0, P30, 30, 1)
	FIELD(GPIO_PMR1T0, P31, 31, 1)
REG32(GPIO_PMR1T1, 0x22c)
	FIELD(GPIO_PMR1T1, P0, 0, 1)
	FIELD(GPIO_PMR1T1, P1, 1, 1)
	FIELD(GPIO_PMR1T1, P2, 2, 1)
	FIELD(GPIO_PMR1T1, P3, 3, 1)
	FIELD(GPIO_PMR1T1, P4, 4, 1)
	FIELD(GPIO_PMR1T1, P5, 5, 1)
	FIELD(GPIO_PMR1T1, P6, 6, 1)
	FIELD(GPIO_PMR1T1, P7, 7, 1)
	FIELD(GPIO_PMR1T1, P8, 8, 1)
	FIELD(GPIO_PMR1T1, P9, 9, 1)
	FIELD(GPIO_PMR1T1, P10, 10, 1)
	FIELD(GPIO_PMR1T1, P11, 11, 1)
	FIELD(GPIO_PMR1T1, P12, 12, 1)
	FIELD(GPIO_PMR1T1, P13, 13, 1)
	FIELD(GPIO_PMR1T1, P14, 14, 1)
	FIELD(GPIO_PMR1T1, P15, 15, 1)
	FIELD(GPIO_PMR1T1, P16, 16, 1)
	FIELD(GPIO_PMR1T1, P17, 17, 1)
	FIELD(GPIO_PMR1T1, P18, 18, 1)
	FIELD(GPIO_PMR1T1, P19, 19, 1)
	FIELD(GPIO_PMR1T1, P20, 20, 1)
	FIELD(GPIO_PMR1T1, P21, 21, 1)
	FIELD(GPIO_PMR1T1, P22, 22, 1)
	FIELD(GPIO_PMR1T1, P23, 23, 1)
	FIELD(GPIO_PMR1T1, P24, 24, 1)
	FIELD(GPIO_PMR1T1, P25, 25, 1)
	FIELD(GPIO_PMR1T1, P26, 26, 1)
	FIELD(GPIO_PMR1T1, P27, 27, 1)
	FIELD(GPIO_PMR1T1, P28, 28, 1)
	FIELD(GPIO_PMR1T1, P29, 29, 1)
	FIELD(GPIO_PMR1T1, P30, 30, 1)
	FIELD(GPIO_PMR1T1, P31, 31, 1)
REG32(GPIO_PMR1T2, 0x42c)
	FIELD(GPIO_PMR1T2, P0, 0, 1)
	FIELD(GPIO_PMR1T2, P1, 1, 1)
	FIELD(GPIO_PMR1T2, P2, 2, 1)
	FIELD(GPIO_PMR1T2, P3, 3, 1)
	FIELD(GPIO_PMR1T2, P4, 4, 1)
	FIELD(GPIO_PMR1T2, P5, 5, 1)
	FIELD(GPIO_PMR1T2, P6, 6, 1)
	FIELD(GPIO_PMR1T2, P7, 7, 1)
	FIELD(GPIO_PMR1T2, P8, 8, 1)
	FIELD(GPIO_PMR1T2, P9, 9, 1)
	FIELD(GPIO_PMR1T2, P10, 10, 1)
	FIELD(GPIO_PMR1T2, P11, 11, 1)
	FIELD(GPIO_PMR1T2, P12, 12, 1)
	FIELD(GPIO_PMR1T2, P13, 13, 1)
	FIELD(GPIO_PMR1T2, P14, 14, 1)
	FIELD(GPIO_PMR1T2, P15, 15, 1)
	FIELD(GPIO_PMR1T2, P16, 16, 1)
	FIELD(GPIO_PMR1T2, P17, 17, 1)
	FIELD(GPIO_PMR1T2, P18, 18, 1)
	FIELD(GPIO_PMR1T2, P19, 19, 1)
	FIELD(GPIO_PMR1T2, P20, 20, 1)
	FIELD(GPIO_PMR1T2, P21, 21, 1)
	FIELD(GPIO_PMR1T2, P22, 22, 1)
	FIELD(GPIO_PMR1T2, P23, 23, 1)
	FIELD(GPIO_PMR1T2, P24, 24, 1)
	FIELD(GPIO_PMR1T2, P25, 25, 1)
	FIELD(GPIO_PMR1T2, P26, 26, 1)
	FIELD(GPIO_PMR1T2, P27, 27, 1)
	FIELD(GPIO_PMR1T2, P28, 28, 1)
	FIELD(GPIO_PMR1T2, P29, 29, 1)
	FIELD(GPIO_PMR1T2, P30, 30, 1)
	FIELD(GPIO_PMR1T2, P31, 31, 1)
REG32(GPIO_PMR2C0, 0x38)
	FIELD(GPIO_PMR2C0, P0, 0, 1)
	FIELD(GPIO_PMR2C0, P1, 1, 1)
	FIELD(GPIO_PMR2C0, P2, 2, 1)
	FIELD(GPIO_PMR2C0, P3, 3, 1)
	FIELD(GPIO_PMR2C0, P4, 4, 1)
	FIELD(GPIO_PMR2C0, P5, 5, 1)
	FIELD(GPIO_PMR2C0, P6, 6, 1)
	FIELD(GPIO_PMR2C0, P7, 7, 1)
	FIELD(GPIO_PMR2C0, P8, 8, 1)
	FIELD(GPIO_PMR2C0, P9, 9, 1)
	FIELD(GPIO_PMR2C0, P10, 10, 1)
	FIELD(GPIO_PMR2C0, P11, 11, 1)
	FIELD(GPIO_PMR2C0, P12, 12, 1)
	FIELD(GPIO_PMR2C0, P13, 13, 1)
	FIELD(GPIO_PMR2C0, P14, 14, 1)
	FIELD(GPIO_PMR2C0, P15, 15, 1)
	FIELD(GPIO_PMR2C0, P16, 16, 1)
	FIELD(GPIO_PMR2C0, P17, 17, 1)
	FIELD(GPIO_PMR2C0, P18, 18, 1)
	FIELD(GPIO_PMR2C0, P19, 19, 1)
	FIELD(GPIO_PMR2C0, P20, 20, 1)
	FIELD(GPIO_PMR2C0, P21, 21, 1)
	FIELD(GPIO_PMR2C0, P22, 22, 1)
	FIELD(GPIO_PMR2C0, P23, 23, 1)
	FIELD(GPIO_PMR2C0, P24, 24, 1)
	FIELD(GPIO_PMR2C0, P25, 25, 1)
	FIELD(GPIO_PMR2C0, P26, 26, 1)
	FIELD(GPIO_PMR2C0, P27, 27, 1)
	FIELD(GPIO_PMR2C0, P28, 28, 1)
	FIELD(GPIO_PMR2C0, P29, 29, 1)
	FIELD(GPIO_PMR2C0, P30, 30, 1)
	FIELD(GPIO_PMR2C0, P31, 31, 1)
REG32(GPIO_PMR2C1, 0x238)
	FIELD(GPIO_PMR2C1, P0, 0, 1)
	FIELD(GPIO_PMR2C1, P1, 1, 1)
	FIELD(GPIO_PMR2C1, P2, 2, 1)
	FIELD(GPIO_PMR2C1, P3, 3, 1)
	FIELD(GPIO_PMR2C1, P4, 4, 1)
	FIELD(GPIO_PMR2C1, P5, 5, 1)
	FIELD(GPIO_PMR2C1, P6, 6, 1)
	FIELD(GPIO_PMR2C1, P7, 7, 1)
	FIELD(GPIO_PMR2C1, P8, 8, 1)
	FIELD(GPIO_PMR2C1, P9, 9, 1)
	FIELD(GPIO_PMR2C1, P10, 10, 1)
	FIELD(GPIO_PMR2C1, P11, 11, 1)
	FIELD(GPIO_PMR2C1, P12, 12, 1)
	FIELD(GPIO_PMR2C1, P13, 13, 1)
	FIELD(GPIO_PMR2C1, P14, 14, 1)
	FIELD(GPIO_PMR2C1, P15, 15, 1)
	FIELD(GPIO_PMR2C1, P16, 16, 1)
	FIELD(GPIO_PMR2C1, P17, 17, 1)
	FIELD(GPIO_PMR2C1, P18, 18, 1)
	FIELD(GPIO_PMR2C1, P19, 19, 1)
	FIELD(GPIO_PMR2C1, P20, 20, 1)
	FIELD(GPIO_PMR2C1, P21, 21, 1)
	FIELD(GPIO_PMR2C1, P22, 22, 1)
	FIELD(GPIO_PMR2C1, P23, 23, 1)
	FIELD(GPIO_PMR2C1, P24, 24, 1)
	FIELD(GPIO_PMR2C1, P25, 25, 1)
	FIELD(GPIO_PMR2C1, P26, 26, 1)
	FIELD(GPIO_PMR2C1, P27, 27, 1)
	FIELD(GPIO_PMR2C1, P28, 28, 1)
	FIELD(GPIO_PMR2C1, P29, 29, 1)
	FIELD(GPIO_PMR2C1, P30, 30, 1)
	FIELD(GPIO_PMR2C1, P31, 31, 1)
REG32(GPIO_PMR2C2, 0x438)
	FIELD(GPIO_PMR2C2, P0, 0, 1)
	FIELD(GPIO_PMR2C2, P1, 1, 1)
	FIELD(GPIO_PMR2C2, P2, 2, 1)
	FIELD(GPIO_PMR2C2, P3, 3, 1)
	FIELD(GPIO_PMR2C2, P4, 4, 1)
	FIELD(GPIO_PMR2C2, P5, 5, 1)
	FIELD(GPIO_PMR2C2, P6, 6, 1)
	FIELD(GPIO_PMR2C2, P7, 7, 1)
	FIELD(GPIO_PMR2C2, P8, 8, 1)
	FIELD(GPIO_PMR2C2, P9, 9, 1)
	FIELD(GPIO_PMR2C2, P10, 10, 1)
	FIELD(GPIO_PMR2C2, P11, 11, 1)
	FIELD(GPIO_PMR2C2, P12, 12, 1)
	FIELD(GPIO_PMR2C2, P13, 13, 1)
	FIELD(GPIO_PMR2C2, P14, 14, 1)
	FIELD(GPIO_PMR2C2, P15, 15, 1)
	FIELD(GPIO_PMR2C2, P16, 16, 1)
	FIELD(GPIO_PMR2C2, P17, 17, 1)
	FIELD(GPIO_PMR2C2, P18, 18, 1)
	FIELD(GPIO_PMR2C2, P19, 19, 1)
	FIELD(GPIO_PMR2C2, P20, 20, 1)
	FIELD(GPIO_PMR2C2, P21, 21, 1)
	FIELD(GPIO_PMR2C2, P22, 22, 1)
	FIELD(GPIO_PMR2C2, P23, 23, 1)
	FIELD(GPIO_PMR2C2, P24, 24, 1)
	FIELD(GPIO_PMR2C2, P25, 25, 1)
	FIELD(GPIO_PMR2C2, P26, 26, 1)
	FIELD(GPIO_PMR2C2, P27, 27, 1)
	FIELD(GPIO_PMR2C2, P28, 28, 1)
	FIELD(GPIO_PMR2C2, P29, 29, 1)
	FIELD(GPIO_PMR2C2, P30, 30, 1)
	FIELD(GPIO_PMR2C2, P31, 31, 1)
REG32(GPIO_PMR2S0, 0x34)
	FIELD(GPIO_PMR2S0, P0, 0, 1)
	FIELD(GPIO_PMR2S0, P1, 1, 1)
	FIELD(GPIO_PMR2S0, P2, 2, 1)
	FIELD(GPIO_PMR2S0, P3, 3, 1)
	FIELD(GPIO_PMR2S0, P4, 4, 1)
	FIELD(GPIO_PMR2S0, P5, 5, 1)
	FIELD(GPIO_PMR2S0, P6, 6, 1)
	FIELD(GPIO_PMR2S0, P7, 7, 1)
	FIELD(GPIO_PMR2S0, P8, 8, 1)
	FIELD(GPIO_PMR2S0, P9, 9, 1)
	FIELD(GPIO_PMR2S0, P10, 10, 1)
	FIELD(GPIO_PMR2S0, P11, 11, 1)
	FIELD(GPIO_PMR2S0, P12, 12, 1)
	FIELD(GPIO_PMR2S0, P13, 13, 1)
	FIELD(GPIO_PMR2S0, P14, 14, 1)
	FIELD(GPIO_PMR2S0, P15, 15, 1)
	FIELD(GPIO_PMR2S0, P16, 16, 1)
	FIELD(GPIO_PMR2S0, P17, 17, 1)
	FIELD(GPIO_PMR2S0, P18, 18, 1)
	FIELD(GPIO_PMR2S0, P19, 19, 1)
	FIELD(GPIO_PMR2S0, P20, 20, 1)
	FIELD(GPIO_PMR2S0, P21, 21, 1)
	FIELD(GPIO_PMR2S0, P22, 22, 1)
	FIELD(GPIO_PMR2S0, P23, 23, 1)
	FIELD(GPIO_PMR2S0, P24, 24, 1)
	FIELD(GPIO_PMR2S0, P25, 25, 1)
	FIELD(GPIO_PMR2S0, P26, 26, 1)
	FIELD(GPIO_PMR2S0, P27, 27, 1)
	FIELD(GPIO_PMR2S0, P28, 28, 1)
	FIELD(GPIO_PMR2S0, P29, 29, 1)
	FIELD(GPIO_PMR2S0, P30, 30, 1)
	FIELD(GPIO_PMR2S0, P31, 31, 1)
REG32(GPIO_PMR2S1, 0x234)
	FIELD(GPIO_PMR2S1, P0, 0, 1)
	FIELD(GPIO_PMR2S1, P1, 1, 1)
	FIELD(GPIO_PMR2S1, P2, 2, 1)
	FIELD(GPIO_PMR2S1, P3, 3, 1)
	FIELD(GPIO_PMR2S1, P4, 4, 1)
	FIELD(GPIO_PMR2S1, P5, 5, 1)
	FIELD(GPIO_PMR2S1, P6, 6, 1)
	FIELD(GPIO_PMR2S1, P7, 7, 1)
	FIELD(GPIO_PMR2S1, P8, 8, 1)
	FIELD(GPIO_PMR2S1, P9, 9, 1)
	FIELD(GPIO_PMR2S1, P10, 10, 1)
	FIELD(GPIO_PMR2S1, P11, 11, 1)
	FIELD(GPIO_PMR2S1, P12, 12, 1)
	FIELD(GPIO_PMR2S1, P13, 13, 1)
	FIELD(GPIO_PMR2S1, P14, 14, 1)
	FIELD(GPIO_PMR2S1, P15, 15, 1)
	FIELD(GPIO_PMR2S1, P16, 16, 1)
	FIELD(GPIO_PMR2S1, P17, 17, 1)
	FIELD(GPIO_PMR2S1, P18, 18, 1)
	FIELD(GPIO_PMR2S1, P19, 19, 1)
	FIELD(GPIO_PMR2S1, P20, 20, 1)
	FIELD(GPIO_PMR2S1, P21, 21, 1)
	FIELD(GPIO_PMR2S1, P22, 22, 1)
	FIELD(GPIO_PMR2S1, P23, 23, 1)
	FIELD(GPIO_PMR2S1, P24, 24, 1)
	FIELD(GPIO_PMR2S1, P25, 25, 1)
	FIELD(GPIO_PMR2S1, P26, 26, 1)
	FIELD(GPIO_PMR2S1, P27, 27, 1)
	FIELD(GPIO_PMR2S1, P28, 28, 1)
	FIELD(GPIO_PMR2S1, P29, 29, 1)
	FIELD(GPIO_PMR2S1, P30, 30, 1)
	FIELD(GPIO_PMR2S1, P31, 31, 1)
REG32(GPIO_PMR2S2, 0x434)
	FIELD(GPIO_PMR2S2, P0, 0, 1)
	FIELD(GPIO_PMR2S2, P1, 1, 1)
	FIELD(GPIO_PMR2S2, P2, 2, 1)
	FIELD(GPIO_PMR2S2, P3, 3, 1)
	FIELD(GPIO_PMR2S2, P4, 4, 1)
	FIELD(GPIO_PMR2S2, P5, 5, 1)
	FIELD(GPIO_PMR2S2, P6, 6, 1)
	FIELD(GPIO_PMR2S2, P7, 7, 1)
	FIELD(GPIO_PMR2S2, P8, 8, 1)
	FIELD(GPIO_PMR2S2, P9, 9, 1)
	FIELD(GPIO_PMR2S2, P10, 10, 1)
	FIELD(GPIO_PMR2S2, P11, 11, 1)
	FIELD(GPIO_PMR2S2, P12, 12, 1)
	FIELD(GPIO_PMR2S2, P13, 13, 1)
	FIELD(GPIO_PMR2S2, P14, 14, 1)
	FIELD(GPIO_PMR2S2, P15, 15, 1)
	FIELD(GPIO_PMR2S2, P16, 16, 1)
	FIELD(GPIO_PMR2S2, P17, 17, 1)
	FIELD(GPIO_PMR2S2, P18, 18, 1)
	FIELD(GPIO_PMR2S2, P19, 19, 1)
	FIELD(GPIO_PMR2S2, P20, 20, 1)
	FIELD(GPIO_PMR2S2, P21, 21, 1)
	FIELD(GPIO_PMR2S2, P22, 22, 1)
	FIELD(GPIO_PMR2S2, P23, 23, 1)
	FIELD(GPIO_PMR2S2, P24, 24, 1)
	FIELD(GPIO_PMR2S2, P25, 25, 1)
	FIELD(GPIO_PMR2S2, P26, 26, 1)
	FIELD(GPIO_PMR2S2, P27, 27, 1)
	FIELD(GPIO_PMR2S2, P28, 28, 1)
	FIELD(GPIO_PMR2S2, P29, 29, 1)
	FIELD(GPIO_PMR2S2, P30, 30, 1)
	FIELD(GPIO_PMR2S2, P31, 31, 1)
REG32(GPIO_PMR2T0, 0x3c)
	FIELD(GPIO_PMR2T0, P0, 0, 1)
	FIELD(GPIO_PMR2T0, P1, 1, 1)
	FIELD(GPIO_PMR2T0, P2, 2, 1)
	FIELD(GPIO_PMR2T0, P3, 3, 1)
	FIELD(GPIO_PMR2T0, P4, 4, 1)
	FIELD(GPIO_PMR2T0, P5, 5, 1)
	FIELD(GPIO_PMR2T0, P6, 6, 1)
	FIELD(GPIO_PMR2T0, P7, 7, 1)
	FIELD(GPIO_PMR2T0, P8, 8, 1)
	FIELD(GPIO_PMR2T0, P9, 9, 1)
	FIELD(GPIO_PMR2T0, P10, 10, 1)
	FIELD(GPIO_PMR2T0, P11, 11, 1)
	FIELD(GPIO_PMR2T0, P12, 12, 1)
	FIELD(GPIO_PMR2T0, P13, 13, 1)
	FIELD(GPIO_PMR2T0, P14, 14, 1)
	FIELD(GPIO_PMR2T0, P15, 15, 1)
	FIELD(GPIO_PMR2T0, P16, 16, 1)
	FIELD(GPIO_PMR2T0, P17, 17, 1)
	FIELD(GPIO_PMR2T0, P18, 18, 1)
	FIELD(GPIO_PMR2T0, P19, 19, 1)
	FIELD(GPIO_PMR2T0, P20, 20, 1)
	FIELD(GPIO_PMR2T0, P21, 21, 1)
	FIELD(GPIO_PMR2T0, P22, 22, 1)
	FIELD(GPIO_PMR2T0, P23, 23, 1)
	FIELD(GPIO_PMR2T0, P24, 24, 1)
	FIELD(GPIO_PMR2T0, P25, 25, 1)
	FIELD(GPIO_PMR2T0, P26, 26, 1)
	FIELD(GPIO_PMR2T0, P27, 27, 1)
	FIELD(GPIO_PMR2T0, P28, 28, 1)
	FIELD(GPIO_PMR2T0, P29, 29, 1)
	FIELD(GPIO_PMR2T0, P30, 30, 1)
	FIELD(GPIO_PMR2T0, P31, 31, 1)
REG32(GPIO_PMR2T1, 0x23c)
	FIELD(GPIO_PMR2T1, P0, 0, 1)
	FIELD(GPIO_PMR2T1, P1, 1, 1)
	FIELD(GPIO_PMR2T1, P2, 2, 1)
	FIELD(GPIO_PMR2T1, P3, 3, 1)
	FIELD(GPIO_PMR2T1, P4, 4, 1)
	FIELD(GPIO_PMR2T1, P5, 5, 1)
	FIELD(GPIO_PMR2T1, P6, 6, 1)
	FIELD(GPIO_PMR2T1, P7, 7, 1)
	FIELD(GPIO_PMR2T1, P8, 8, 1)
	FIELD(GPIO_PMR2T1, P9, 9, 1)
	FIELD(GPIO_PMR2T1, P10, 10, 1)
	FIELD(GPIO_PMR2T1, P11, 11, 1)
	FIELD(GPIO_PMR2T1, P12, 12, 1)
	FIELD(GPIO_PMR2T1, P13, 13, 1)
	FIELD(GPIO_PMR2T1, P14, 14, 1)
	FIELD(GPIO_PMR2T1, P15, 15, 1)
	FIELD(GPIO_PMR2T1, P16, 16, 1)
	FIELD(GPIO_PMR2T1, P17, 17, 1)
	FIELD(GPIO_PMR2T1, P18, 18, 1)
	FIELD(GPIO_PMR2T1, P19, 19, 1)
	FIELD(GPIO_PMR2T1, P20, 20, 1)
	FIELD(GPIO_PMR2T1, P21, 21, 1)
	FIELD(GPIO_PMR2T1, P22, 22, 1)
	FIELD(GPIO_PMR2T1, P23, 23, 1)
	FIELD(GPIO_PMR2T1, P24, 24, 1)
	FIELD(GPIO_PMR2T1, P25, 25, 1)
	FIELD(GPIO_PMR2T1, P26, 26, 1)
	FIELD(GPIO_PMR2T1, P27, 27, 1)
	FIELD(GPIO_PMR2T1, P28, 28, 1)
	FIELD(GPIO_PMR2T1, P29, 29, 1)
	FIELD(GPIO_PMR2T1, P30, 30, 1)
	FIELD(GPIO_PMR2T1, P31, 31, 1)
REG32(GPIO_PMR2T2, 0x43c)
	FIELD(GPIO_PMR2T2, P0, 0, 1)
	FIELD(GPIO_PMR2T2, P1, 1, 1)
	FIELD(GPIO_PMR2T2, P2, 2, 1)
	FIELD(GPIO_PMR2T2, P3, 3, 1)
	FIELD(GPIO_PMR2T2, P4, 4, 1)
	FIELD(GPIO_PMR2T2, P5, 5, 1)
	FIELD(GPIO_PMR2T2, P6, 6, 1)
	FIELD(GPIO_PMR2T2, P7, 7, 1)
	FIELD(GPIO_PMR2T2, P8, 8, 1)
	FIELD(GPIO_PMR2T2, P9, 9, 1)
	FIELD(GPIO_PMR2T2, P10, 10, 1)
	FIELD(GPIO_PMR2T2, P11, 11, 1)
	FIELD(GPIO_PMR2T2, P12, 12, 1)
	FIELD(GPIO_PMR2T2, P13, 13, 1)
	FIELD(GPIO_PMR2T2, P14, 14, 1)
	FIELD(GPIO_PMR2T2, P15, 15, 1)
	FIELD(GPIO_PMR2T2, P16, 16, 1)
	FIELD(GPIO_PMR2T2, P17, 17, 1)
	FIELD(GPIO_PMR2T2, P18, 18, 1)
	FIELD(GPIO_PMR2T2, P19, 19, 1)
	FIELD(GPIO_PMR2T2, P20, 20, 1)
	FIELD(GPIO_PMR2T2, P21, 21, 1)
	FIELD(GPIO_PMR2T2, P22, 22, 1)
	FIELD(GPIO_PMR2T2, P23, 23, 1)
	FIELD(GPIO_PMR2T2, P24, 24, 1)
	FIELD(GPIO_PMR2T2, P25, 25, 1)
	FIELD(GPIO_PMR2T2, P26, 26, 1)
	FIELD(GPIO_PMR2T2, P27, 27, 1)
	FIELD(GPIO_PMR2T2, P28, 28, 1)
	FIELD(GPIO_PMR2T2, P29, 29, 1)
	FIELD(GPIO_PMR2T2, P30, 30, 1)
	FIELD(GPIO_PMR2T2, P31, 31, 1)
REG32(GPIO_PMR00, 0x10)
	FIELD(GPIO_PMR00, P0, 0, 1)
	FIELD(GPIO_PMR00, P1, 1, 1)
	FIELD(GPIO_PMR00, P2, 2, 1)
	FIELD(GPIO_PMR00, P3, 3, 1)
	FIELD(GPIO_PMR00, P4, 4, 1)
	FIELD(GPIO_PMR00, P5, 5, 1)
	FIELD(GPIO_PMR00, P6, 6, 1)
	FIELD(GPIO_PMR00, P7, 7, 1)
	FIELD(GPIO_PMR00, P8, 8, 1)
	FIELD(GPIO_PMR00, P9, 9, 1)
	FIELD(GPIO_PMR00, P10, 10, 1)
	FIELD(GPIO_PMR00, P11, 11, 1)
	FIELD(GPIO_PMR00, P12, 12, 1)
	FIELD(GPIO_PMR00, P13, 13, 1)
	FIELD(GPIO_PMR00, P14, 14, 1)
	FIELD(GPIO_PMR00, P15, 15, 1)
	FIELD(GPIO_PMR00, P16, 16, 1)
	FIELD(GPIO_PMR00, P17, 17, 1)
	FIELD(GPIO_PMR00, P18, 18, 1)
	FIELD(GPIO_PMR00, P19, 19, 1)
	FIELD(GPIO_PMR00, P20, 20, 1)
	FIELD(GPIO_PMR00, P21, 21, 1)
	FIELD(GPIO_PMR00, P22, 22, 1)
	FIELD(GPIO_PMR00, P23, 23, 1)
	FIELD(GPIO_PMR00, P24, 24, 1)
	FIELD(GPIO_PMR00, P25, 25, 1)
	FIELD(GPIO_PMR00, P26, 26, 1)
	FIELD(GPIO_PMR00, P27, 27, 1)
	FIELD(GPIO_PMR00, P28, 28, 1)
	FIELD(GPIO_PMR00, P29, 29, 1)
	FIELD(GPIO_PMR00, P30, 30, 1)
	FIELD(GPIO_PMR00, P31, 31, 1)
REG32(GPIO_PMR01, 0x210)
	FIELD(GPIO_PMR01, P0, 0, 1)
	FIELD(GPIO_PMR01, P1, 1, 1)
	FIELD(GPIO_PMR01, P2, 2, 1)
	FIELD(GPIO_PMR01, P3, 3, 1)
	FIELD(GPIO_PMR01, P4, 4, 1)
	FIELD(GPIO_PMR01, P5, 5, 1)
	FIELD(GPIO_PMR01, P6, 6, 1)
	FIELD(GPIO_PMR01, P7, 7, 1)
	FIELD(GPIO_PMR01, P8, 8, 1)
	FIELD(GPIO_PMR01, P9, 9, 1)
	FIELD(GPIO_PMR01, P10, 10, 1)
	FIELD(GPIO_PMR01, P11, 11, 1)
	FIELD(GPIO_PMR01, P12, 12, 1)
	FIELD(GPIO_PMR01, P13, 13, 1)
	FIELD(GPIO_PMR01, P14, 14, 1)
	FIELD(GPIO_PMR01, P15, 15, 1)
	FIELD(GPIO_PMR01, P16, 16, 1)
	FIELD(GPIO_PMR01, P17, 17, 1)
	FIELD(GPIO_PMR01, P18, 18, 1)
	FIELD(GPIO_PMR01, P19, 19, 1)
	FIELD(GPIO_PMR01, P20, 20, 1)
	FIELD(GPIO_PMR01, P21, 21, 1)
	FIELD(GPIO_PMR01, P22, 22, 1)
	FIELD(GPIO_PMR01, P23, 23, 1)
	FIELD(GPIO_PMR01, P24, 24, 1)
	FIELD(GPIO_PMR01, P25, 25, 1)
	FIELD(GPIO_PMR01, P26, 26, 1)
	FIELD(GPIO_PMR01, P27, 27, 1)
	FIELD(GPIO_PMR01, P28, 28, 1)
	FIELD(GPIO_PMR01, P29, 29, 1)
	FIELD(GPIO_PMR01, P30, 30, 1)
	FIELD(GPIO_PMR01, P31, 31, 1)
REG32(GPIO_PMR02, 0x410)
	FIELD(GPIO_PMR02, P0, 0, 1)
	FIELD(GPIO_PMR02, P1, 1, 1)
	FIELD(GPIO_PMR02, P2, 2, 1)
	FIELD(GPIO_PMR02, P3, 3, 1)
	FIELD(GPIO_PMR02, P4, 4, 1)
	FIELD(GPIO_PMR02, P5, 5, 1)
	FIELD(GPIO_PMR02, P6, 6, 1)
	FIELD(GPIO_PMR02, P7, 7, 1)
	FIELD(GPIO_PMR02, P8, 8, 1)
	FIELD(GPIO_PMR02, P9, 9, 1)
	FIELD(GPIO_PMR02, P10, 10, 1)
	FIELD(GPIO_PMR02, P11, 11, 1)
	FIELD(GPIO_PMR02, P12, 12, 1)
	FIELD(GPIO_PMR02, P13, 13, 1)
	FIELD(GPIO_PMR02, P14, 14, 1)
	FIELD(GPIO_PMR02, P15, 15, 1)
	FIELD(GPIO_PMR02, P16, 16, 1)
	FIELD(GPIO_PMR02, P17, 17, 1)
	FIELD(GPIO_PMR02, P18, 18, 1)
	FIELD(GPIO_PMR02, P19, 19, 1)
	FIELD(GPIO_PMR02, P20, 20, 1)
	FIELD(GPIO_PMR02, P21, 21, 1)
	FIELD(GPIO_PMR02, P22, 22, 1)
	FIELD(GPIO_PMR02, P23, 23, 1)
	FIELD(GPIO_PMR02, P24, 24, 1)
	FIELD(GPIO_PMR02, P25, 25, 1)
	FIELD(GPIO_PMR02, P26, 26, 1)
	FIELD(GPIO_PMR02, P27, 27, 1)
	FIELD(GPIO_PMR02, P28, 28, 1)
	FIELD(GPIO_PMR02, P29, 29, 1)
	FIELD(GPIO_PMR02, P30, 30, 1)
	FIELD(GPIO_PMR02, P31, 31, 1)
REG32(GPIO_PMR10, 0x20)
	FIELD(GPIO_PMR10, P0, 0, 1)
	FIELD(GPIO_PMR10, P1, 1, 1)
	FIELD(GPIO_PMR10, P2, 2, 1)
	FIELD(GPIO_PMR10, P3, 3, 1)
	FIELD(GPIO_PMR10, P4, 4, 1)
	FIELD(GPIO_PMR10, P5, 5, 1)
	FIELD(GPIO_PMR10, P6, 6, 1)
	FIELD(GPIO_PMR10, P7, 7, 1)
	FIELD(GPIO_PMR10, P8, 8, 1)
	FIELD(GPIO_PMR10, P9, 9, 1)
	FIELD(GPIO_PMR10, P10, 10, 1)
	FIELD(GPIO_PMR10, P11, 11, 1)
	FIELD(GPIO_PMR10, P12, 12, 1)
	FIELD(GPIO_PMR10, P13, 13, 1)
	FIELD(GPIO_PMR10, P14, 14, 1)
	FIELD(GPIO_PMR10, P15, 15, 1)
	FIELD(GPIO_PMR10, P16, 16, 1)
	FIELD(GPIO_PMR10, P17, 17, 1)
	FIELD(GPIO_PMR10, P18, 18, 1)
	FIELD(GPIO_PMR10, P19, 19, 1)
	FIELD(GPIO_PMR10, P20, 20, 1)
	FIELD(GPIO_PMR10, P21, 21, 1)
	FIELD(GPIO_PMR10, P22, 22, 1)
	FIELD(GPIO_PMR10, P23, 23, 1)
	FIELD(GPIO_PMR10, P24, 24, 1)
	FIELD(GPIO_PMR10, P25, 25, 1)
	FIELD(GPIO_PMR10, P26, 26, 1)
	FIELD(GPIO_PMR10, P27, 27, 1)
	FIELD(GPIO_PMR10, P28, 28, 1)
	FIELD(GPIO_PMR10, P29, 29, 1)
	FIELD(GPIO_PMR10, P30, 30, 1)
	FIELD(GPIO_PMR10, P31, 31, 1)
REG32(GPIO_PMR11, 0x220)
	FIELD(GPIO_PMR11, P0, 0, 1)
	FIELD(GPIO_PMR11, P1, 1, 1)
	FIELD(GPIO_PMR11, P2, 2, 1)
	FIELD(GPIO_PMR11, P3, 3, 1)
	FIELD(GPIO_PMR11, P4, 4, 1)
	FIELD(GPIO_PMR11, P5, 5, 1)
	FIELD(GPIO_PMR11, P6, 6, 1)
	FIELD(GPIO_PMR11, P7, 7, 1)
	FIELD(GPIO_PMR11, P8, 8, 1)
	FIELD(GPIO_PMR11, P9, 9, 1)
	FIELD(GPIO_PMR11, P10, 10, 1)
	FIELD(GPIO_PMR11, P11, 11, 1)
	FIELD(GPIO_PMR11, P12, 12, 1)
	FIELD(GPIO_PMR11, P13, 13, 1)
	FIELD(GPIO_PMR11, P14, 14, 1)
	FIELD(GPIO_PMR11, P15, 15, 1)
	FIELD(GPIO_PMR11, P16, 16, 1)
	FIELD(GPIO_PMR11, P17, 17, 1)
	FIELD(GPIO_PMR11, P18, 18, 1)
	FIELD(GPIO_PMR11, P19, 19, 1)
	FIELD(GPIO_PMR11, P20, 20, 1)
	FIELD(GPIO_PMR11, P21, 21, 1)
	FIELD(GPIO_PMR11, P22, 22, 1)
	FIELD(GPIO_PMR11, P23, 23, 1)
	FIELD(GPIO_PMR11, P24, 24, 1)
	FIELD(GPIO_PMR11, P25, 25, 1)
	FIELD(GPIO_PMR11, P26, 26, 1)
	FIELD(GPIO_PMR11, P27, 27, 1)
	FIELD(GPIO_PMR11, P28, 28, 1)
	FIELD(GPIO_PMR11, P29, 29, 1)
	FIELD(GPIO_PMR11, P30, 30, 1)
	FIELD(GPIO_PMR11, P31, 31, 1)
REG32(GPIO_PMR12, 0x420)
	FIELD(GPIO_PMR12, P0, 0, 1)
	FIELD(GPIO_PMR12, P1, 1, 1)
	FIELD(GPIO_PMR12, P2, 2, 1)
	FIELD(GPIO_PMR12, P3, 3, 1)
	FIELD(GPIO_PMR12, P4, 4, 1)
	FIELD(GPIO_PMR12, P5, 5, 1)
	FIELD(GPIO_PMR12, P6, 6, 1)
	FIELD(GPIO_PMR12, P7, 7, 1)
	FIELD(GPIO_PMR12, P8, 8, 1)
	FIELD(GPIO_PMR12, P9, 9, 1)
	FIELD(GPIO_PMR12, P10, 10, 1)
	FIELD(GPIO_PMR12, P11, 11, 1)
	FIELD(GPIO_PMR12, P12, 12, 1)
	FIELD(GPIO_PMR12, P13, 13, 1)
	FIELD(GPIO_PMR12, P14, 14, 1)
	FIELD(GPIO_PMR12, P15, 15, 1)
	FIELD(GPIO_PMR12, P16, 16, 1)
	FIELD(GPIO_PMR12, P17, 17, 1)
	FIELD(GPIO_PMR12, P18, 18, 1)
	FIELD(GPIO_PMR12, P19, 19, 1)
	FIELD(GPIO_PMR12, P20, 20, 1)
	FIELD(GPIO_PMR12, P21, 21, 1)
	FIELD(GPIO_PMR12, P22, 22, 1)
	FIELD(GPIO_PMR12, P23, 23, 1)
	FIELD(GPIO_PMR12, P24, 24, 1)
	FIELD(GPIO_PMR12, P25, 25, 1)
	FIELD(GPIO_PMR12, P26, 26, 1)
	FIELD(GPIO_PMR12, P27, 27, 1)
	FIELD(GPIO_PMR12, P28, 28, 1)
	FIELD(GPIO_PMR12, P29, 29, 1)
	FIELD(GPIO_PMR12, P30, 30, 1)
	FIELD(GPIO_PMR12, P31, 31, 1)
REG32(GPIO_PMR20, 0x30)
	FIELD(GPIO_PMR20, P0, 0, 1)
	FIELD(GPIO_PMR20, P1, 1, 1)
	FIELD(GPIO_PMR20, P2, 2, 1)
	FIELD(GPIO_PMR20, P3, 3, 1)
	FIELD(GPIO_PMR20, P4, 4, 1)
	FIELD(GPIO_PMR20, P5, 5, 1)
	FIELD(GPIO_PMR20, P6, 6, 1)
	FIELD(GPIO_PMR20, P7, 7, 1)
	FIELD(GPIO_PMR20, P8, 8, 1)
	FIELD(GPIO_PMR20, P9, 9, 1)
	FIELD(GPIO_PMR20, P10, 10, 1)
	FIELD(GPIO_PMR20, P11, 11, 1)
	FIELD(GPIO_PMR20, P12, 12, 1)
	FIELD(GPIO_PMR20, P13, 13, 1)
	FIELD(GPIO_PMR20, P14, 14, 1)
	FIELD(GPIO_PMR20, P15, 15, 1)
	FIELD(GPIO_PMR20, P16, 16, 1)
	FIELD(GPIO_PMR20, P17, 17, 1)
	FIELD(GPIO_PMR20, P18, 18, 1)
	FIELD(GPIO_PMR20, P19, 19, 1)
	FIELD(GPIO_PMR20, P20, 20, 1)
	FIELD(GPIO_PMR20, P21, 21, 1)
	FIELD(GPIO_PMR20, P22, 22, 1)
	FIELD(GPIO_PMR20, P23, 23, 1)
	FIELD(GPIO_PMR20, P24, 24, 1)
	FIELD(GPIO_PMR20, P25, 25, 1)
	FIELD(GPIO_PMR20, P26, 26, 1)
	FIELD(GPIO_PMR20, P27, 27, 1)
	FIELD(GPIO_PMR20, P28, 28, 1)
	FIELD(GPIO_PMR20, P29, 29, 1)
	FIELD(GPIO_PMR20, P30, 30, 1)
	FIELD(GPIO_PMR20, P31, 31, 1)
REG32(GPIO_PMR21, 0x230)
	FIELD(GPIO_PMR21, P0, 0, 1)
	FIELD(GPIO_PMR21, P1, 1, 1)
	FIELD(GPIO_PMR21, P2, 2, 1)
	FIELD(GPIO_PMR21, P3, 3, 1)
	FIELD(GPIO_PMR21, P4, 4, 1)
	FIELD(GPIO_PMR21, P5, 5, 1)
	FIELD(GPIO_PMR21, P6, 6, 1)
	FIELD(GPIO_PMR21, P7, 7, 1)
	FIELD(GPIO_PMR21, P8, 8, 1)
	FIELD(GPIO_PMR21, P9, 9, 1)
	FIELD(GPIO_PMR21, P10, 10, 1)
	FIELD(GPIO_PMR21, P11, 11, 1)
	FIELD(GPIO_PMR21, P12, 12, 1)
	FIELD(GPIO_PMR21, P13, 13, 1)
	FIELD(GPIO_PMR21, P14, 14, 1)
	FIELD(GPIO_PMR21, P15, 15, 1)
	FIELD(GPIO_PMR21, P16, 16, 1)
	FIELD(GPIO_PMR21, P17, 17, 1)
	FIELD(GPIO_PMR21, P18, 18, 1)
	FIELD(GPIO_PMR21, P19, 19, 1)
	FIELD(GPIO_PMR21, P20, 20, 1)
	FIELD(GPIO_PMR21, P21, 21, 1)
	FIELD(GPIO_PMR21, P22, 22, 1)
	FIELD(GPIO_PMR21, P23, 23, 1)
	FIELD(GPIO_PMR21, P24, 24, 1)
	FIELD(GPIO_PMR21, P25, 25, 1)
	FIELD(GPIO_PMR21, P26, 26, 1)
	FIELD(GPIO_PMR21, P27, 27, 1)
	FIELD(GPIO_PMR21, P28, 28, 1)
	FIELD(GPIO_PMR21, P29, 29, 1)
	FIELD(GPIO_PMR21, P30, 30, 1)
	FIELD(GPIO_PMR21, P31, 31, 1)
REG32(GPIO_PMR22, 0x430)
	FIELD(GPIO_PMR22, P0, 0, 1)
	FIELD(GPIO_PMR22, P1, 1, 1)
	FIELD(GPIO_PMR22, P2, 2, 1)
	FIELD(GPIO_PMR22, P3, 3, 1)
	FIELD(GPIO_PMR22, P4, 4, 1)
	FIELD(GPIO_PMR22, P5, 5, 1)
	FIELD(GPIO_PMR22, P6, 6, 1)
	FIELD(GPIO_PMR22, P7, 7, 1)
	FIELD(GPIO_PMR22, P8, 8, 1)
	FIELD(GPIO_PMR22, P9, 9, 1)
	FIELD(GPIO_PMR22, P10, 10, 1)
	FIELD(GPIO_PMR22, P11, 11, 1)
	FIELD(GPIO_PMR22, P12, 12, 1)
	FIELD(GPIO_PMR22, P13, 13, 1)
	FIELD(GPIO_PMR22, P14, 14, 1)
	FIELD(GPIO_PMR22, P15, 15, 1)
	FIELD(GPIO_PMR22, P16, 16, 1)
	FIELD(GPIO_PMR22, P17, 17, 1)
	FIELD(GPIO_PMR22, P18, 18, 1)
	FIELD(GPIO_PMR22, P19, 19, 1)
	FIELD(GPIO_PMR22, P20, 20, 1)
	FIELD(GPIO_PMR22, P21, 21, 1)
	FIELD(GPIO_PMR22, P22, 22, 1)
	FIELD(GPIO_PMR22, P23, 23, 1)
	FIELD(GPIO_PMR22, P24, 24, 1)
	FIELD(GPIO_PMR22, P25, 25, 1)
	FIELD(GPIO_PMR22, P26, 26, 1)
	FIELD(GPIO_PMR22, P27, 27, 1)
	FIELD(GPIO_PMR22, P28, 28, 1)
	FIELD(GPIO_PMR22, P29, 29, 1)
	FIELD(GPIO_PMR22, P30, 30, 1)
	FIELD(GPIO_PMR22, P31, 31, 1)
REG32(GPIO_PUERC0, 0x78)
	FIELD(GPIO_PUERC0, P0, 0, 1)
	FIELD(GPIO_PUERC0, P1, 1, 1)
	FIELD(GPIO_PUERC0, P2, 2, 1)
	FIELD(GPIO_PUERC0, P3, 3, 1)
	FIELD(GPIO_PUERC0, P4, 4, 1)
	FIELD(GPIO_PUERC0, P5, 5, 1)
	FIELD(GPIO_PUERC0, P6, 6, 1)
	FIELD(GPIO_PUERC0, P7, 7, 1)
	FIELD(GPIO_PUERC0, P8, 8, 1)
	FIELD(GPIO_PUERC0, P9, 9, 1)
	FIELD(GPIO_PUERC0, P10, 10, 1)
	FIELD(GPIO_PUERC0, P11, 11, 1)
	FIELD(GPIO_PUERC0, P12, 12, 1)
	FIELD(GPIO_PUERC0, P13, 13, 1)
	FIELD(GPIO_PUERC0, P14, 14, 1)
	FIELD(GPIO_PUERC0, P15, 15, 1)
	FIELD(GPIO_PUERC0, P16, 16, 1)
	FIELD(GPIO_PUERC0, P17, 17, 1)
	FIELD(GPIO_PUERC0, P18, 18, 1)
	FIELD(GPIO_PUERC0, P19, 19, 1)
	FIELD(GPIO_PUERC0, P20, 20, 1)
	FIELD(GPIO_PUERC0, P21, 21, 1)
	FIELD(GPIO_PUERC0, P22, 22, 1)
	FIELD(GPIO_PUERC0, P23, 23, 1)
	FIELD(GPIO_PUERC0, P24, 24, 1)
	FIELD(GPIO_PUERC0, P25, 25, 1)
	FIELD(GPIO_PUERC0, P26, 26, 1)
	FIELD(GPIO_PUERC0, P27, 27, 1)
	FIELD(GPIO_PUERC0, P28, 28, 1)
	FIELD(GPIO_PUERC0, P29, 29, 1)
	FIELD(GPIO_PUERC0, P30, 30, 1)
	FIELD(GPIO_PUERC0, P31, 31, 1)
REG32(GPIO_PUERC1, 0x278)
	FIELD(GPIO_PUERC1, P0, 0, 1)
	FIELD(GPIO_PUERC1, P1, 1, 1)
	FIELD(GPIO_PUERC1, P2, 2, 1)
	FIELD(GPIO_PUERC1, P3, 3, 1)
	FIELD(GPIO_PUERC1, P4, 4, 1)
	FIELD(GPIO_PUERC1, P5, 5, 1)
	FIELD(GPIO_PUERC1, P6, 6, 1)
	FIELD(GPIO_PUERC1, P7, 7, 1)
	FIELD(GPIO_PUERC1, P8, 8, 1)
	FIELD(GPIO_PUERC1, P9, 9, 1)
	FIELD(GPIO_PUERC1, P10, 10, 1)
	FIELD(GPIO_PUERC1, P11, 11, 1)
	FIELD(GPIO_PUERC1, P12, 12, 1)
	FIELD(GPIO_PUERC1, P13, 13, 1)
	FIELD(GPIO_PUERC1, P14, 14, 1)
	FIELD(GPIO_PUERC1, P15, 15, 1)
	FIELD(GPIO_PUERC1, P16, 16, 1)
	FIELD(GPIO_PUERC1, P17, 17, 1)
	FIELD(GPIO_PUERC1, P18, 18, 1)
	FIELD(GPIO_PUERC1, P19, 19, 1)
	FIELD(GPIO_PUERC1, P20, 20, 1)
	FIELD(GPIO_PUERC1, P21, 21, 1)
	FIELD(GPIO_PUERC1, P22, 22, 1)
	FIELD(GPIO_PUERC1, P23, 23, 1)
	FIELD(GPIO_PUERC1, P24, 24, 1)
	FIELD(GPIO_PUERC1, P25, 25, 1)
	FIELD(GPIO_PUERC1, P26, 26, 1)
	FIELD(GPIO_PUERC1, P27, 27, 1)
	FIELD(GPIO_PUERC1, P28, 28, 1)
	FIELD(GPIO_PUERC1, P29, 29, 1)
	FIELD(GPIO_PUERC1, P30, 30, 1)
	FIELD(GPIO_PUERC1, P31, 31, 1)
REG32(GPIO_PUERC2, 0x478)
	FIELD(GPIO_PUERC2, P0, 0, 1)
	FIELD(GPIO_PUERC2, P1, 1, 1)
	FIELD(GPIO_PUERC2, P2, 2, 1)
	FIELD(GPIO_PUERC2, P3, 3, 1)
	FIELD(GPIO_PUERC2, P4, 4, 1)
	FIELD(GPIO_PUERC2, P5, 5, 1)
	FIELD(GPIO_PUERC2, P6, 6, 1)
	FIELD(GPIO_PUERC2, P7, 7, 1)
	FIELD(GPIO_PUERC2, P8, 8, 1)
	FIELD(GPIO_PUERC2, P9, 9, 1)
	FIELD(GPIO_PUERC2, P10, 10, 1)
	FIELD(GPIO_PUERC2, P11, 11, 1)
	FIELD(GPIO_PUERC2, P12, 12, 1)
	FIELD(GPIO_PUERC2, P13, 13, 1)
	FIELD(GPIO_PUERC2, P14, 14, 1)
	FIELD(GPIO_PUERC2, P15, 15, 1)
	FIELD(GPIO_PUERC2, P16, 16, 1)
	FIELD(GPIO_PUERC2, P17, 17, 1)
	FIELD(GPIO_PUERC2, P18, 18, 1)
	FIELD(GPIO_PUERC2, P19, 19, 1)
	FIELD(GPIO_PUERC2, P20, 20, 1)
	FIELD(GPIO_PUERC2, P21, 21, 1)
	FIELD(GPIO_PUERC2, P22, 22, 1)
	FIELD(GPIO_PUERC2, P23, 23, 1)
	FIELD(GPIO_PUERC2, P24, 24, 1)
	FIELD(GPIO_PUERC2, P25, 25, 1)
	FIELD(GPIO_PUERC2, P26, 26, 1)
	FIELD(GPIO_PUERC2, P27, 27, 1)
	FIELD(GPIO_PUERC2, P28, 28, 1)
	FIELD(GPIO_PUERC2, P29, 29, 1)
	FIELD(GPIO_PUERC2, P30, 30, 1)
	FIELD(GPIO_PUERC2, P31, 31, 1)
REG32(GPIO_PUERS0, 0x74)
	FIELD(GPIO_PUERS0, P0, 0, 1)
	FIELD(GPIO_PUERS0, P1, 1, 1)
	FIELD(GPIO_PUERS0, P2, 2, 1)
	FIELD(GPIO_PUERS0, P3, 3, 1)
	FIELD(GPIO_PUERS0, P4, 4, 1)
	FIELD(GPIO_PUERS0, P5, 5, 1)
	FIELD(GPIO_PUERS0, P6, 6, 1)
	FIELD(GPIO_PUERS0, P7, 7, 1)
	FIELD(GPIO_PUERS0, P8, 8, 1)
	FIELD(GPIO_PUERS0, P9, 9, 1)
	FIELD(GPIO_PUERS0, P10, 10, 1)
	FIELD(GPIO_PUERS0, P11, 11, 1)
	FIELD(GPIO_PUERS0, P12, 12, 1)
	FIELD(GPIO_PUERS0, P13, 13, 1)
	FIELD(GPIO_PUERS0, P14, 14, 1)
	FIELD(GPIO_PUERS0, P15, 15, 1)
	FIELD(GPIO_PUERS0, P16, 16, 1)
	FIELD(GPIO_PUERS0, P17, 17, 1)
	FIELD(GPIO_PUERS0, P18, 18, 1)
	FIELD(GPIO_PUERS0, P19, 19, 1)
	FIELD(GPIO_PUERS0, P20, 20, 1)
	FIELD(GPIO_PUERS0, P21, 21, 1)
	FIELD(GPIO_PUERS0, P22, 22, 1)
	FIELD(GPIO_PUERS0, P23, 23, 1)
	FIELD(GPIO_PUERS0, P24, 24, 1)
	FIELD(GPIO_PUERS0, P25, 25, 1)
	FIELD(GPIO_PUERS0, P26, 26, 1)
	FIELD(GPIO_PUERS0, P27, 27, 1)
	FIELD(GPIO_PUERS0, P28, 28, 1)
	FIELD(GPIO_PUERS0, P29, 29, 1)
	FIELD(GPIO_PUERS0, P30, 30, 1)
	FIELD(GPIO_PUERS0, P31, 31, 1)
REG32(GPIO_PUERS1, 0x274)
	FIELD(GPIO_PUERS1, P0, 0, 1)
	FIELD(GPIO_PUERS1, P1, 1, 1)
	FIELD(GPIO_PUERS1, P2, 2, 1)
	FIELD(GPIO_PUERS1, P3, 3, 1)
	FIELD(GPIO_PUERS1, P4, 4, 1)
	FIELD(GPIO_PUERS1, P5, 5, 1)
	FIELD(GPIO_PUERS1, P6, 6, 1)
	FIELD(GPIO_PUERS1, P7, 7, 1)
	FIELD(GPIO_PUERS1, P8, 8, 1)
	FIELD(GPIO_PUERS1, P9, 9, 1)
	FIELD(GPIO_PUERS1, P10, 10, 1)
	FIELD(GPIO_PUERS1, P11, 11, 1)
	FIELD(GPIO_PUERS1, P12, 12, 1)
	FIELD(GPIO_PUERS1, P13, 13, 1)
	FIELD(GPIO_PUERS1, P14, 14, 1)
	FIELD(GPIO_PUERS1, P15, 15, 1)
	FIELD(GPIO_PUERS1, P16, 16, 1)
	FIELD(GPIO_PUERS1, P17, 17, 1)
	FIELD(GPIO_PUERS1, P18, 18, 1)
	FIELD(GPIO_PUERS1, P19, 19, 1)
	FIELD(GPIO_PUERS1, P20, 20, 1)
	FIELD(GPIO_PUERS1, P21, 21, 1)
	FIELD(GPIO_PUERS1, P22, 22, 1)
	FIELD(GPIO_PUERS1, P23, 23, 1)
	FIELD(GPIO_PUERS1, P24, 24, 1)
	FIELD(GPIO_PUERS1, P25, 25, 1)
	FIELD(GPIO_PUERS1, P26, 26, 1)
	FIELD(GPIO_PUERS1, P27, 27, 1)
	FIELD(GPIO_PUERS1, P28, 28, 1)
	FIELD(GPIO_PUERS1, P29, 29, 1)
	FIELD(GPIO_PUERS1, P30, 30, 1)
	FIELD(GPIO_PUERS1, P31, 31, 1)
REG32(GPIO_PUERS2, 0x474)
	FIELD(GPIO_PUERS2, P0, 0, 1)
	FIELD(GPIO_PUERS2, P1, 1, 1)
	FIELD(GPIO_PUERS2, P2, 2, 1)
	FIELD(GPIO_PUERS2, P3, 3, 1)
	FIELD(GPIO_PUERS2, P4, 4, 1)
	FIELD(GPIO_PUERS2, P5, 5, 1)
	FIELD(GPIO_PUERS2, P6, 6, 1)
	FIELD(GPIO_PUERS2, P7, 7, 1)
	FIELD(GPIO_PUERS2, P8, 8, 1)
	FIELD(GPIO_PUERS2, P9, 9, 1)
	FIELD(GPIO_PUERS2, P10, 10, 1)
	FIELD(GPIO_PUERS2, P11, 11, 1)
	FIELD(GPIO_PUERS2, P12, 12, 1)
	FIELD(GPIO_PUERS2, P13, 13, 1)
	FIELD(GPIO_PUERS2, P14, 14, 1)
	FIELD(GPIO_PUERS2, P15, 15, 1)
	FIELD(GPIO_PUERS2, P16, 16, 1)
	FIELD(GPIO_PUERS2, P17, 17, 1)
	FIELD(GPIO_PUERS2, P18, 18, 1)
	FIELD(GPIO_PUERS2, P19, 19, 1)
	FIELD(GPIO_PUERS2, P20, 20, 1)
	FIELD(GPIO_PUERS2, P21, 21, 1)
	FIELD(GPIO_PUERS2, P22, 22, 1)
	FIELD(GPIO_PUERS2, P23, 23, 1)
	FIELD(GPIO_PUERS2, P24, 24, 1)
	FIELD(GPIO_PUERS2, P25, 25, 1)
	FIELD(GPIO_PUERS2, P26, 26, 1)
	FIELD(GPIO_PUERS2, P27, 27, 1)
	FIELD(GPIO_PUERS2, P28, 28, 1)
	FIELD(GPIO_PUERS2, P29, 29, 1)
	FIELD(GPIO_PUERS2, P30, 30, 1)
	FIELD(GPIO_PUERS2, P31, 31, 1)
REG32(GPIO_PUERT0, 0x7c)
	FIELD(GPIO_PUERT0, P0, 0, 1)
	FIELD(GPIO_PUERT0, P1, 1, 1)
	FIELD(GPIO_PUERT0, P2, 2, 1)
	FIELD(GPIO_PUERT0, P3, 3, 1)
	FIELD(GPIO_PUERT0, P4, 4, 1)
	FIELD(GPIO_PUERT0, P5, 5, 1)
	FIELD(GPIO_PUERT0, P6, 6, 1)
	FIELD(GPIO_PUERT0, P7, 7, 1)
	FIELD(GPIO_PUERT0, P8, 8, 1)
	FIELD(GPIO_PUERT0, P9, 9, 1)
	FIELD(GPIO_PUERT0, P10, 10, 1)
	FIELD(GPIO_PUERT0, P11, 11, 1)
	FIELD(GPIO_PUERT0, P12, 12, 1)
	FIELD(GPIO_PUERT0, P13, 13, 1)
	FIELD(GPIO_PUERT0, P14, 14, 1)
	FIELD(GPIO_PUERT0, P15, 15, 1)
	FIELD(GPIO_PUERT0, P16, 16, 1)
	FIELD(GPIO_PUERT0, P17, 17, 1)
	FIELD(GPIO_PUERT0, P18, 18, 1)
	FIELD(GPIO_PUERT0, P19, 19, 1)
	FIELD(GPIO_PUERT0, P20, 20, 1)
	FIELD(GPIO_PUERT0, P21, 21, 1)
	FIELD(GPIO_PUERT0, P22, 22, 1)
	FIELD(GPIO_PUERT0, P23, 23, 1)
	FIELD(GPIO_PUERT0, P24, 24, 1)
	FIELD(GPIO_PUERT0, P25, 25, 1)
	FIELD(GPIO_PUERT0, P26, 26, 1)
	FIELD(GPIO_PUERT0, P27, 27, 1)
	FIELD(GPIO_PUERT0, P28, 28, 1)
	FIELD(GPIO_PUERT0, P29, 29, 1)
	FIELD(GPIO_PUERT0, P30, 30, 1)
	FIELD(GPIO_PUERT0, P31, 31, 1)
REG32(GPIO_PUERT1, 0x27c)
	FIELD(GPIO_PUERT1, P0, 0, 1)
	FIELD(GPIO_PUERT1, P1, 1, 1)
	FIELD(GPIO_PUERT1, P2, 2, 1)
	FIELD(GPIO_PUERT1, P3, 3, 1)
	FIELD(GPIO_PUERT1, P4, 4, 1)
	FIELD(GPIO_PUERT1, P5, 5, 1)
	FIELD(GPIO_PUERT1, P6, 6, 1)
	FIELD(GPIO_PUERT1, P7, 7, 1)
	FIELD(GPIO_PUERT1, P8, 8, 1)
	FIELD(GPIO_PUERT1, P9, 9, 1)
	FIELD(GPIO_PUERT1, P10, 10, 1)
	FIELD(GPIO_PUERT1, P11, 11, 1)
	FIELD(GPIO_PUERT1, P12, 12, 1)
	FIELD(GPIO_PUERT1, P13, 13, 1)
	FIELD(GPIO_PUERT1, P14, 14, 1)
	FIELD(GPIO_PUERT1, P15, 15, 1)
	FIELD(GPIO_PUERT1, P16, 16, 1)
	FIELD(GPIO_PUERT1, P17, 17, 1)
	FIELD(GPIO_PUERT1, P18, 18, 1)
	FIELD(GPIO_PUERT1, P19, 19, 1)
	FIELD(GPIO_PUERT1, P20, 20, 1)
	FIELD(GPIO_PUERT1, P21, 21, 1)
	FIELD(GPIO_PUERT1, P22, 22, 1)
	FIELD(GPIO_PUERT1, P23, 23, 1)
	FIELD(GPIO_PUERT1, P24, 24, 1)
	FIELD(GPIO_PUERT1, P25, 25, 1)
	FIELD(GPIO_PUERT1, P26, 26, 1)
	FIELD(GPIO_PUERT1, P27, 27, 1)
	FIELD(GPIO_PUERT1, P28, 28, 1)
	FIELD(GPIO_PUERT1, P29, 29, 1)
	FIELD(GPIO_PUERT1, P30, 30, 1)
	FIELD(GPIO_PUERT1, P31, 31, 1)
REG32(GPIO_PUERT2, 0x47c)
	FIELD(GPIO_PUERT2, P0, 0, 1)
	FIELD(GPIO_PUERT2, P1, 1, 1)
	FIELD(GPIO_PUERT2, P2, 2, 1)
	FIELD(GPIO_PUERT2, P3, 3, 1)
	FIELD(GPIO_PUERT2, P4, 4, 1)
	FIELD(GPIO_PUERT2, P5, 5, 1)
	FIELD(GPIO_PUERT2, P6, 6, 1)
	FIELD(GPIO_PUERT2, P7, 7, 1)
	FIELD(GPIO_PUERT2, P8, 8, 1)
	FIELD(GPIO_PUERT2, P9, 9, 1)
	FIELD(GPIO_PUERT2, P10, 10, 1)
	FIELD(GPIO_PUERT2, P11, 11, 1)
	FIELD(GPIO_PUERT2, P12, 12, 1)
	FIELD(GPIO_PUERT2, P13, 13, 1)
	FIELD(GPIO_PUERT2, P14, 14, 1)
	FIELD(GPIO_PUERT2, P15, 15, 1)
	FIELD(GPIO_PUERT2, P16, 16, 1)
	FIELD(GPIO_PUERT2, P17, 17, 1)
	FIELD(GPIO_PUERT2, P18, 18, 1)
	FIELD(GPIO_PUERT2, P19, 19, 1)
	FIELD(GPIO_PUERT2, P20, 20, 1)
	FIELD(GPIO_PUERT2, P21, 21, 1)
	FIELD(GPIO_PUERT2, P22, 22, 1)
	FIELD(GPIO_PUERT2, P23, 23, 1)
	FIELD(GPIO_PUERT2, P24, 24, 1)
	FIELD(GPIO_PUERT2, P25, 25, 1)
	FIELD(GPIO_PUERT2, P26, 26, 1)
	FIELD(GPIO_PUERT2, P27, 27, 1)
	FIELD(GPIO_PUERT2, P28, 28, 1)
	FIELD(GPIO_PUERT2, P29, 29, 1)
	FIELD(GPIO_PUERT2, P30, 30, 1)
	FIELD(GPIO_PUERT2, P31, 31, 1)
REG32(GPIO_PUER0, 0x70)
	FIELD(GPIO_PUER0, P0, 0, 1)
	FIELD(GPIO_PUER0, P1, 1, 1)
	FIELD(GPIO_PUER0, P2, 2, 1)
	FIELD(GPIO_PUER0, P3, 3, 1)
	FIELD(GPIO_PUER0, P4, 4, 1)
	FIELD(GPIO_PUER0, P5, 5, 1)
	FIELD(GPIO_PUER0, P6, 6, 1)
	FIELD(GPIO_PUER0, P7, 7, 1)
	FIELD(GPIO_PUER0, P8, 8, 1)
	FIELD(GPIO_PUER0, P9, 9, 1)
	FIELD(GPIO_PUER0, P10, 10, 1)
	FIELD(GPIO_PUER0, P11, 11, 1)
	FIELD(GPIO_PUER0, P12, 12, 1)
	FIELD(GPIO_PUER0, P13, 13, 1)
	FIELD(GPIO_PUER0, P14, 14, 1)
	FIELD(GPIO_PUER0, P15, 15, 1)
	FIELD(GPIO_PUER0, P16, 16, 1)
	FIELD(GPIO_PUER0, P17, 17, 1)
	FIELD(GPIO_PUER0, P18, 18, 1)
	FIELD(GPIO_PUER0, P19, 19, 1)
	FIELD(GPIO_PUER0, P20, 20, 1)
	FIELD(GPIO_PUER0, P21, 21, 1)
	FIELD(GPIO_PUER0, P22, 22, 1)
	FIELD(GPIO_PUER0, P23, 23, 1)
	FIELD(GPIO_PUER0, P24, 24, 1)
	FIELD(GPIO_PUER0, P25, 25, 1)
	FIELD(GPIO_PUER0, P26, 26, 1)
	FIELD(GPIO_PUER0, P27, 27, 1)
	FIELD(GPIO_PUER0, P28, 28, 1)
	FIELD(GPIO_PUER0, P29, 29, 1)
	FIELD(GPIO_PUER0, P30, 30, 1)
	FIELD(GPIO_PUER0, P31, 31, 1)
REG32(GPIO_PUER1, 0x270)
	FIELD(GPIO_PUER1, P0, 0, 1)
	FIELD(GPIO_PUER1, P1, 1, 1)
	FIELD(GPIO_PUER1, P2, 2, 1)
	FIELD(GPIO_PUER1, P3, 3, 1)
	FIELD(GPIO_PUER1, P4, 4, 1)
	FIELD(GPIO_PUER1, P5, 5, 1)
	FIELD(GPIO_PUER1, P6, 6, 1)
	FIELD(GPIO_PUER1, P7, 7, 1)
	FIELD(GPIO_PUER1, P8, 8, 1)
	FIELD(GPIO_PUER1, P9, 9, 1)
	FIELD(GPIO_PUER1, P10, 10, 1)
	FIELD(GPIO_PUER1, P11, 11, 1)
	FIELD(GPIO_PUER1, P12, 12, 1)
	FIELD(GPIO_PUER1, P13, 13, 1)
	FIELD(GPIO_PUER1, P14, 14, 1)
	FIELD(GPIO_PUER1, P15, 15, 1)
	FIELD(GPIO_PUER1, P16, 16, 1)
	FIELD(GPIO_PUER1, P17, 17, 1)
	FIELD(GPIO_PUER1, P18, 18, 1)
	FIELD(GPIO_PUER1, P19, 19, 1)
	FIELD(GPIO_PUER1, P20, 20, 1)
	FIELD(GPIO_PUER1, P21, 21, 1)
	FIELD(GPIO_PUER1, P22, 22, 1)
	FIELD(GPIO_PUER1, P23, 23, 1)
	FIELD(GPIO_PUER1, P24, 24, 1)
	FIELD(GPIO_PUER1, P25, 25, 1)
	FIELD(GPIO_PUER1, P26, 26, 1)
	FIELD(GPIO_PUER1, P27, 27, 1)
	FIELD(GPIO_PUER1, P28, 28, 1)
	FIELD(GPIO_PUER1, P29, 29, 1)
	FIELD(GPIO_PUER1, P30, 30, 1)
	FIELD(GPIO_PUER1, P31, 31, 1)
REG32(GPIO_PUER2, 0x470)
	FIELD(GPIO_PUER2, P0, 0, 1)
	FIELD(GPIO_PUER2, P1, 1, 1)
	FIELD(GPIO_PUER2, P2, 2, 1)
	FIELD(GPIO_PUER2, P3, 3, 1)
	FIELD(GPIO_PUER2, P4, 4, 1)
	FIELD(GPIO_PUER2, P5, 5, 1)
	FIELD(GPIO_PUER2, P6, 6, 1)
	FIELD(GPIO_PUER2, P7, 7, 1)
	FIELD(GPIO_PUER2, P8, 8, 1)
	FIELD(GPIO_PUER2, P9, 9, 1)
	FIELD(GPIO_PUER2, P10, 10, 1)
	FIELD(GPIO_PUER2, P11, 11, 1)
	FIELD(GPIO_PUER2, P12, 12, 1)
	FIELD(GPIO_PUER2, P13, 13, 1)
	FIELD(GPIO_PUER2, P14, 14, 1)
	FIELD(GPIO_PUER2, P15, 15, 1)
	FIELD(GPIO_PUER2, P16, 16, 1)
	FIELD(GPIO_PUER2, P17, 17, 1)
	FIELD(GPIO_PUER2, P18, 18, 1)
	FIELD(GPIO_PUER2, P19, 19, 1)
	FIELD(GPIO_PUER2, P20, 20, 1)
	FIELD(GPIO_PUER2, P21, 21, 1)
	FIELD(GPIO_PUER2, P22, 22, 1)
	FIELD(GPIO_PUER2, P23, 23, 1)
	FIELD(GPIO_PUER2, P24, 24, 1)
	FIELD(GPIO_PUER2, P25, 25, 1)
	FIELD(GPIO_PUER2, P26, 26, 1)
	FIELD(GPIO_PUER2, P27, 27, 1)
	FIELD(GPIO_PUER2, P28, 28, 1)
	FIELD(GPIO_PUER2, P29, 29, 1)
	FIELD(GPIO_PUER2, P30, 30, 1)
	FIELD(GPIO_PUER2, P31, 31, 1)
REG32(GPIO_PVR0, 0x60)
	FIELD(GPIO_PVR0, P0, 0, 1)
	FIELD(GPIO_PVR0, P1, 1, 1)
	FIELD(GPIO_PVR0, P2, 2, 1)
	FIELD(GPIO_PVR0, P3, 3, 1)
	FIELD(GPIO_PVR0, P4, 4, 1)
	FIELD(GPIO_PVR0, P5, 5, 1)
	FIELD(GPIO_PVR0, P6, 6, 1)
	FIELD(GPIO_PVR0, P7, 7, 1)
	FIELD(GPIO_PVR0, P8, 8, 1)
	FIELD(GPIO_PVR0, P9, 9, 1)
	FIELD(GPIO_PVR0, P10, 10, 1)
	FIELD(GPIO_PVR0, P11, 11, 1)
	FIELD(GPIO_PVR0, P12, 12, 1)
	FIELD(GPIO_PVR0, P13, 13, 1)
	FIELD(GPIO_PVR0, P14, 14, 1)
	FIELD(GPIO_PVR0, P15, 15, 1)
	FIELD(GPIO_PVR0, P16, 16, 1)
	FIELD(GPIO_PVR0, P17, 17, 1)
	FIELD(GPIO_PVR0, P18, 18, 1)
	FIELD(GPIO_PVR0, P19, 19, 1)
	FIELD(GPIO_PVR0, P20, 20, 1)
	FIELD(GPIO_PVR0, P21, 21, 1)
	FIELD(GPIO_PVR0, P22, 22, 1)
	FIELD(GPIO_PVR0, P23, 23, 1)
	FIELD(GPIO_PVR0, P24, 24, 1)
	FIELD(GPIO_PVR0, P25, 25, 1)
	FIELD(GPIO_PVR0, P26, 26, 1)
	FIELD(GPIO_PVR0, P27, 27, 1)
	FIELD(GPIO_PVR0, P28, 28, 1)
	FIELD(GPIO_PVR0, P29, 29, 1)
	FIELD(GPIO_PVR0, P30, 30, 1)
	FIELD(GPIO_PVR0, P31, 31, 1)
REG32(GPIO_PVR1, 0x260)
	FIELD(GPIO_PVR1, P0, 0, 1)
	FIELD(GPIO_PVR1, P1, 1, 1)
	FIELD(GPIO_PVR1, P2, 2, 1)
	FIELD(GPIO_PVR1, P3, 3, 1)
	FIELD(GPIO_PVR1, P4, 4, 1)
	FIELD(GPIO_PVR1, P5, 5, 1)
	FIELD(GPIO_PVR1, P6, 6, 1)
	FIELD(GPIO_PVR1, P7, 7, 1)
	FIELD(GPIO_PVR1, P8, 8, 1)
	FIELD(GPIO_PVR1, P9, 9, 1)
	FIELD(GPIO_PVR1, P10, 10, 1)
	FIELD(GPIO_PVR1, P11, 11, 1)
	FIELD(GPIO_PVR1, P12, 12, 1)
	FIELD(GPIO_PVR1, P13, 13, 1)
	FIELD(GPIO_PVR1, P14, 14, 1)
	FIELD(GPIO_PVR1, P15, 15, 1)
	FIELD(GPIO_PVR1, P16, 16, 1)
	FIELD(GPIO_PVR1, P17, 17, 1)
	FIELD(GPIO_PVR1, P18, 18, 1)
	FIELD(GPIO_PVR1, P19, 19, 1)
	FIELD(GPIO_PVR1, P20, 20, 1)
	FIELD(GPIO_PVR1, P21, 21, 1)
	FIELD(GPIO_PVR1, P22, 22, 1)
	FIELD(GPIO_PVR1, P23, 23, 1)
	FIELD(GPIO_PVR1, P24, 24, 1)
	FIELD(GPIO_PVR1, P25, 25, 1)
	FIELD(GPIO_PVR1, P26, 26, 1)
	FIELD(GPIO_PVR1, P27, 27, 1)
	FIELD(GPIO_PVR1, P28, 28, 1)
	FIELD(GPIO_PVR1, P29, 29, 1)
	FIELD(GPIO_PVR1, P30, 30, 1)
	FIELD(GPIO_PVR1, P31, 31, 1)
REG32(GPIO_PVR2, 0x460)
	FIELD(GPIO_PVR2, P0, 0, 1)
	FIELD(GPIO_PVR2, P1, 1, 1)
	FIELD(GPIO_PVR2, P2, 2, 1)
	FIELD(GPIO_PVR2, P3, 3, 1)
	FIELD(GPIO_PVR2, P4, 4, 1)
	FIELD(GPIO_PVR2, P5, 5, 1)
	FIELD(GPIO_PVR2, P6, 6, 1)
	FIELD(GPIO_PVR2, P7, 7, 1)
	FIELD(GPIO_PVR2, P8, 8, 1)
	FIELD(GPIO_PVR2, P9, 9, 1)
	FIELD(GPIO_PVR2, P10, 10, 1)
	FIELD(GPIO_PVR2, P11, 11, 1)
	FIELD(GPIO_PVR2, P12, 12, 1)
	FIELD(GPIO_PVR2, P13, 13, 1)
	FIELD(GPIO_PVR2, P14, 14, 1)
	FIELD(GPIO_PVR2, P15, 15, 1)
	FIELD(GPIO_PVR2, P16, 16, 1)
	FIELD(GPIO_PVR2, P17, 17, 1)
	FIELD(GPIO_PVR2, P18, 18, 1)
	FIELD(GPIO_PVR2, P19, 19, 1)
	FIELD(GPIO_PVR2, P20, 20, 1)
	FIELD(GPIO_PVR2, P21, 21, 1)
	FIELD(GPIO_PVR2, P22, 22, 1)
	FIELD(GPIO_PVR2, P23, 23, 1)
	FIELD(GPIO_PVR2, P24, 24, 1)
	FIELD(GPIO_PVR2, P25, 25, 1)
	FIELD(GPIO_PVR2, P26, 26, 1)
	FIELD(GPIO_PVR2, P27, 27, 1)
	FIELD(GPIO_PVR2, P28, 28, 1)
	FIELD(GPIO_PVR2, P29, 29, 1)
	FIELD(GPIO_PVR2, P30, 30, 1)
	FIELD(GPIO_PVR2, P31, 31, 1)
REG32(GPIO_STERC0, 0x168)
	FIELD(GPIO_STERC0, P0, 0, 1)
	FIELD(GPIO_STERC0, P1, 1, 1)
	FIELD(GPIO_STERC0, P2, 2, 1)
	FIELD(GPIO_STERC0, P3, 3, 1)
	FIELD(GPIO_STERC0, P4, 4, 1)
	FIELD(GPIO_STERC0, P5, 5, 1)
	FIELD(GPIO_STERC0, P6, 6, 1)
	FIELD(GPIO_STERC0, P7, 7, 1)
	FIELD(GPIO_STERC0, P8, 8, 1)
	FIELD(GPIO_STERC0, P9, 9, 1)
	FIELD(GPIO_STERC0, P10, 10, 1)
	FIELD(GPIO_STERC0, P11, 11, 1)
	FIELD(GPIO_STERC0, P12, 12, 1)
	FIELD(GPIO_STERC0, P13, 13, 1)
	FIELD(GPIO_STERC0, P14, 14, 1)
	FIELD(GPIO_STERC0, P15, 15, 1)
	FIELD(GPIO_STERC0, P16, 16, 1)
	FIELD(GPIO_STERC0, P17, 17, 1)
	FIELD(GPIO_STERC0, P18, 18, 1)
	FIELD(GPIO_STERC0, P19, 19, 1)
	FIELD(GPIO_STERC0, P20, 20, 1)
	FIELD(GPIO_STERC0, P21, 21, 1)
	FIELD(GPIO_STERC0, P22, 22, 1)
	FIELD(GPIO_STERC0, P23, 23, 1)
	FIELD(GPIO_STERC0, P24, 24, 1)
	FIELD(GPIO_STERC0, P25, 25, 1)
	FIELD(GPIO_STERC0, P26, 26, 1)
	FIELD(GPIO_STERC0, P27, 27, 1)
	FIELD(GPIO_STERC0, P28, 28, 1)
	FIELD(GPIO_STERC0, P29, 29, 1)
	FIELD(GPIO_STERC0, P30, 30, 1)
	FIELD(GPIO_STERC0, P31, 31, 1)
REG32(GPIO_STERC1, 0x368)
	FIELD(GPIO_STERC1, P0, 0, 1)
	FIELD(GPIO_STERC1, P1, 1, 1)
	FIELD(GPIO_STERC1, P2, 2, 1)
	FIELD(GPIO_STERC1, P3, 3, 1)
	FIELD(GPIO_STERC1, P4, 4, 1)
	FIELD(GPIO_STERC1, P5, 5, 1)
	FIELD(GPIO_STERC1, P6, 6, 1)
	FIELD(GPIO_STERC1, P7, 7, 1)
	FIELD(GPIO_STERC1, P8, 8, 1)
	FIELD(GPIO_STERC1, P9, 9, 1)
	FIELD(GPIO_STERC1, P10, 10, 1)
	FIELD(GPIO_STERC1, P11, 11, 1)
	FIELD(GPIO_STERC1, P12, 12, 1)
	FIELD(GPIO_STERC1, P13, 13, 1)
	FIELD(GPIO_STERC1, P14, 14, 1)
	FIELD(GPIO_STERC1, P15, 15, 1)
	FIELD(GPIO_STERC1, P16, 16, 1)
	FIELD(GPIO_STERC1, P17, 17, 1)
	FIELD(GPIO_STERC1, P18, 18, 1)
	FIELD(GPIO_STERC1, P19, 19, 1)
	FIELD(GPIO_STERC1, P20, 20, 1)
	FIELD(GPIO_STERC1, P21, 21, 1)
	FIELD(GPIO_STERC1, P22, 22, 1)
	FIELD(GPIO_STERC1, P23, 23, 1)
	FIELD(GPIO_STERC1, P24, 24, 1)
	FIELD(GPIO_STERC1, P25, 25, 1)
	FIELD(GPIO_STERC1, P26, 26, 1)
	FIELD(GPIO_STERC1, P27, 27, 1)
	FIELD(GPIO_STERC1, P28, 28, 1)
	FIELD(GPIO_STERC1, P29, 29, 1)
	FIELD(GPIO_STERC1, P30, 30, 1)
	FIELD(GPIO_STERC1, P31, 31, 1)
REG32(GPIO_STERC2, 0x568)
	FIELD(GPIO_STERC2, P0, 0, 1)
	FIELD(GPIO_STERC2, P1, 1, 1)
	FIELD(GPIO_STERC2, P2, 2, 1)
	FIELD(GPIO_STERC2, P3, 3, 1)
	FIELD(GPIO_STERC2, P4, 4, 1)
	FIELD(GPIO_STERC2, P5, 5, 1)
	FIELD(GPIO_STERC2, P6, 6, 1)
	FIELD(GPIO_STERC2, P7, 7, 1)
	FIELD(GPIO_STERC2, P8, 8, 1)
	FIELD(GPIO_STERC2, P9, 9, 1)
	FIELD(GPIO_STERC2, P10, 10, 1)
	FIELD(GPIO_STERC2, P11, 11, 1)
	FIELD(GPIO_STERC2, P12, 12, 1)
	FIELD(GPIO_STERC2, P13, 13, 1)
	FIELD(GPIO_STERC2, P14, 14, 1)
	FIELD(GPIO_STERC2, P15, 15, 1)
	FIELD(GPIO_STERC2, P16, 16, 1)
	FIELD(GPIO_STERC2, P17, 17, 1)
	FIELD(GPIO_STERC2, P18, 18, 1)
	FIELD(GPIO_STERC2, P19, 19, 1)
	FIELD(GPIO_STERC2, P20, 20, 1)
	FIELD(GPIO_STERC2, P21, 21, 1)
	FIELD(GPIO_STERC2, P22, 22, 1)
	FIELD(GPIO_STERC2, P23, 23, 1)
	FIELD(GPIO_STERC2, P24, 24, 1)
	FIELD(GPIO_STERC2, P25, 25, 1)
	FIELD(GPIO_STERC2, P26, 26, 1)
	FIELD(GPIO_STERC2, P27, 27, 1)
	FIELD(GPIO_STERC2, P28, 28, 1)
	FIELD(GPIO_STERC2, P29, 29, 1)
	FIELD(GPIO_STERC2, P30, 30, 1)
	FIELD(GPIO_STERC2, P31, 31, 1)
REG32(GPIO_STERS0, 0x164)
	FIELD(GPIO_STERS0, P0, 0, 1)
	FIELD(GPIO_STERS0, P1, 1, 1)
	FIELD(GPIO_STERS0, P2, 2, 1)
	FIELD(GPIO_STERS0, P3, 3, 1)
	FIELD(GPIO_STERS0, P4, 4, 1)
	FIELD(GPIO_STERS0, P5, 5, 1)
	FIELD(GPIO_STERS0, P6, 6, 1)
	FIELD(GPIO_STERS0, P7, 7, 1)
	FIELD(GPIO_STERS0, P8, 8, 1)
	FIELD(GPIO_STERS0, P9, 9, 1)
	FIELD(GPIO_STERS0, P10, 10, 1)
	FIELD(GPIO_STERS0, P11, 11, 1)
	FIELD(GPIO_STERS0, P12, 12, 1)
	FIELD(GPIO_STERS0, P13, 13, 1)
	FIELD(GPIO_STERS0, P14, 14, 1)
	FIELD(GPIO_STERS0, P15, 15, 1)
	FIELD(GPIO_STERS0, P16, 16, 1)
	FIELD(GPIO_STERS0, P17, 17, 1)
	FIELD(GPIO_STERS0, P18, 18, 1)
	FIELD(GPIO_STERS0, P19, 19, 1)
	FIELD(GPIO_STERS0, P20, 20, 1)
	FIELD(GPIO_STERS0, P21, 21, 1)
	FIELD(GPIO_STERS0, P22, 22, 1)
	FIELD(GPIO_STERS0, P23, 23, 1)
	FIELD(GPIO_STERS0, P24, 24, 1)
	FIELD(GPIO_STERS0, P25, 25, 1)
	FIELD(GPIO_STERS0, P26, 26, 1)
	FIELD(GPIO_STERS0, P27, 27, 1)
	FIELD(GPIO_STERS0, P28, 28, 1)
	FIELD(GPIO_STERS0, P29, 29, 1)
	FIELD(GPIO_STERS0, P30, 30, 1)
	FIELD(GPIO_STERS0, P31, 31, 1)
REG32(GPIO_STERS1, 0x364)
	FIELD(GPIO_STERS1, P0, 0, 1)
	FIELD(GPIO_STERS1, P1, 1, 1)
	FIELD(GPIO_STERS1, P2, 2, 1)
	FIELD(GPIO_STERS1, P3, 3, 1)
	FIELD(GPIO_STERS1, P4, 4, 1)
	FIELD(GPIO_STERS1, P5, 5, 1)
	FIELD(GPIO_STERS1, P6, 6, 1)
	FIELD(GPIO_STERS1, P7, 7, 1)
	FIELD(GPIO_STERS1, P8, 8, 1)
	FIELD(GPIO_STERS1, P9, 9, 1)
	FIELD(GPIO_STERS1, P10, 10, 1)
	FIELD(GPIO_STERS1, P11, 11, 1)
	FIELD(GPIO_STERS1, P12, 12, 1)
	FIELD(GPIO_STERS1, P13, 13, 1)
	FIELD(GPIO_STERS1, P14, 14, 1)
	FIELD(GPIO_STERS1, P15, 15, 1)
	FIELD(GPIO_STERS1, P16, 16, 1)
	FIELD(GPIO_STERS1, P17, 17, 1)
	FIELD(GPIO_STERS1, P18, 18, 1)
	FIELD(GPIO_STERS1, P19, 19, 1)
	FIELD(GPIO_STERS1, P20, 20, 1)
	FIELD(GPIO_STERS1, P21, 21, 1)
	FIELD(GPIO_STERS1, P22, 22, 1)
	FIELD(GPIO_STERS1, P23, 23, 1)
	FIELD(GPIO_STERS1, P24, 24, 1)
	FIELD(GPIO_STERS1, P25, 25, 1)
	FIELD(GPIO_STERS1, P26, 26, 1)
	FIELD(GPIO_STERS1, P27, 27, 1)
	FIELD(GPIO_STERS1, P28, 28, 1)
	FIELD(GPIO_STERS1, P29, 29, 1)
	FIELD(GPIO_STERS1, P30, 30, 1)
	FIELD(GPIO_STERS1, P31, 31, 1)
REG32(GPIO_STERS2, 0x564)
	FIELD(GPIO_STERS2, P0, 0, 1)
	FIELD(GPIO_STERS2, P1, 1, 1)
	FIELD(GPIO_STERS2, P2, 2, 1)
	FIELD(GPIO_STERS2, P3, 3, 1)
	FIELD(GPIO_STERS2, P4, 4, 1)
	FIELD(GPIO_STERS2, P5, 5, 1)
	FIELD(GPIO_STERS2, P6, 6, 1)
	FIELD(GPIO_STERS2, P7, 7, 1)
	FIELD(GPIO_STERS2, P8, 8, 1)
	FIELD(GPIO_STERS2, P9, 9, 1)
	FIELD(GPIO_STERS2, P10, 10, 1)
	FIELD(GPIO_STERS2, P11, 11, 1)
	FIELD(GPIO_STERS2, P12, 12, 1)
	FIELD(GPIO_STERS2, P13, 13, 1)
	FIELD(GPIO_STERS2, P14, 14, 1)
	FIELD(GPIO_STERS2, P15, 15, 1)
	FIELD(GPIO_STERS2, P16, 16, 1)
	FIELD(GPIO_STERS2, P17, 17, 1)
	FIELD(GPIO_STERS2, P18, 18, 1)
	FIELD(GPIO_STERS2, P19, 19, 1)
	FIELD(GPIO_STERS2, P20, 20, 1)
	FIELD(GPIO_STERS2, P21, 21, 1)
	FIELD(GPIO_STERS2, P22, 22, 1)
	FIELD(GPIO_STERS2, P23, 23, 1)
	FIELD(GPIO_STERS2, P24, 24, 1)
	FIELD(GPIO_STERS2, P25, 25, 1)
	FIELD(GPIO_STERS2, P26, 26, 1)
	FIELD(GPIO_STERS2, P27, 27, 1)
	FIELD(GPIO_STERS2, P28, 28, 1)
	FIELD(GPIO_STERS2, P29, 29, 1)
	FIELD(GPIO_STERS2, P30, 30, 1)
	FIELD(GPIO_STERS2, P31, 31, 1)
REG32(GPIO_STERT0, 0x16c)
	FIELD(GPIO_STERT0, P0, 0, 1)
	FIELD(GPIO_STERT0, P1, 1, 1)
	FIELD(GPIO_STERT0, P2, 2, 1)
	FIELD(GPIO_STERT0, P3, 3, 1)
	FIELD(GPIO_STERT0, P4, 4, 1)
	FIELD(GPIO_STERT0, P5, 5, 1)
	FIELD(GPIO_STERT0, P6, 6, 1)
	FIELD(GPIO_STERT0, P7, 7, 1)
	FIELD(GPIO_STERT0, P8, 8, 1)
	FIELD(GPIO_STERT0, P9, 9, 1)
	FIELD(GPIO_STERT0, P10, 10, 1)
	FIELD(GPIO_STERT0, P11, 11, 1)
	FIELD(GPIO_STERT0, P12, 12, 1)
	FIELD(GPIO_STERT0, P13, 13, 1)
	FIELD(GPIO_STERT0, P14, 14, 1)
	FIELD(GPIO_STERT0, P15, 15, 1)
	FIELD(GPIO_STERT0, P16, 16, 1)
	FIELD(GPIO_STERT0, P17, 17, 1)
	FIELD(GPIO_STERT0, P18, 18, 1)
	FIELD(GPIO_STERT0, P19, 19, 1)
	FIELD(GPIO_STERT0, P20, 20, 1)
	FIELD(GPIO_STERT0, P21, 21, 1)
	FIELD(GPIO_STERT0, P22, 22, 1)
	FIELD(GPIO_STERT0, P23, 23, 1)
	FIELD(GPIO_STERT0, P24, 24, 1)
	FIELD(GPIO_STERT0, P25, 25, 1)
	FIELD(GPIO_STERT0, P26, 26, 1)
	FIELD(GPIO_STERT0, P27, 27, 1)
	FIELD(GPIO_STERT0, P28, 28, 1)
	FIELD(GPIO_STERT0, P29, 29, 1)
	FIELD(GPIO_STERT0, P30, 30, 1)
	FIELD(GPIO_STERT0, P31, 31, 1)
REG32(GPIO_STERT1, 0x36c)
	FIELD(GPIO_STERT1, P0, 0, 1)
	FIELD(GPIO_STERT1, P1, 1, 1)
	FIELD(GPIO_STERT1, P2, 2, 1)
	FIELD(GPIO_STERT1, P3, 3, 1)
	FIELD(GPIO_STERT1, P4, 4, 1)
	FIELD(GPIO_STERT1, P5, 5, 1)
	FIELD(GPIO_STERT1, P6, 6, 1)
	FIELD(GPIO_STERT1, P7, 7, 1)
	FIELD(GPIO_STERT1, P8, 8, 1)
	FIELD(GPIO_STERT1, P9, 9, 1)
	FIELD(GPIO_STERT1, P10, 10, 1)
	FIELD(GPIO_STERT1, P11, 11, 1)
	FIELD(GPIO_STERT1, P12, 12, 1)
	FIELD(GPIO_STERT1, P13, 13, 1)
	FIELD(GPIO_STERT1, P14, 14, 1)
	FIELD(GPIO_STERT1, P15, 15, 1)
	FIELD(GPIO_STERT1, P16, 16, 1)
	FIELD(GPIO_STERT1, P17, 17, 1)
	FIELD(GPIO_STERT1, P18, 18, 1)
	FIELD(GPIO_STERT1, P19, 19, 1)
	FIELD(GPIO_STERT1, P20, 20, 1)
	FIELD(GPIO_STERT1, P21, 21, 1)
	FIELD(GPIO_STERT1, P22, 22, 1)
	FIELD(GPIO_STERT1, P23, 23, 1)
	FIELD(GPIO_STERT1, P24, 24, 1)
	FIELD(GPIO_STERT1, P25, 25, 1)
	FIELD(GPIO_STERT1, P26, 26, 1)
	FIELD(GPIO_STERT1, P27, 27, 1)
	FIELD(GPIO_STERT1, P28, 28, 1)
	FIELD(GPIO_STERT1, P29, 29, 1)
	FIELD(GPIO_STERT1, P30, 30, 1)
	FIELD(GPIO_STERT1, P31, 31, 1)
REG32(GPIO_STERT2, 0x56c)
	FIELD(GPIO_STERT2, P0, 0, 1)
	FIELD(GPIO_STERT2, P1, 1, 1)
	FIELD(GPIO_STERT2, P2, 2, 1)
	FIELD(GPIO_STERT2, P3, 3, 1)
	FIELD(GPIO_STERT2, P4, 4, 1)
	FIELD(GPIO_STERT2, P5, 5, 1)
	FIELD(GPIO_STERT2, P6, 6, 1)
	FIELD(GPIO_STERT2, P7, 7, 1)
	FIELD(GPIO_STERT2, P8, 8, 1)
	FIELD(GPIO_STERT2, P9, 9, 1)
	FIELD(GPIO_STERT2, P10, 10, 1)
	FIELD(GPIO_STERT2, P11, 11, 1)
	FIELD(GPIO_STERT2, P12, 12, 1)
	FIELD(GPIO_STERT2, P13, 13, 1)
	FIELD(GPIO_STERT2, P14, 14, 1)
	FIELD(GPIO_STERT2, P15, 15, 1)
	FIELD(GPIO_STERT2, P16, 16, 1)
	FIELD(GPIO_STERT2, P17, 17, 1)
	FIELD(GPIO_STERT2, P18, 18, 1)
	FIELD(GPIO_STERT2, P19, 19, 1)
	FIELD(GPIO_STERT2, P20, 20, 1)
	FIELD(GPIO_STERT2, P21, 21, 1)
	FIELD(GPIO_STERT2, P22, 22, 1)
	FIELD(GPIO_STERT2, P23, 23, 1)
	FIELD(GPIO_STERT2, P24, 24, 1)
	FIELD(GPIO_STERT2, P25, 25, 1)
	FIELD(GPIO_STERT2, P26, 26, 1)
	FIELD(GPIO_STERT2, P27, 27, 1)
	FIELD(GPIO_STERT2, P28, 28, 1)
	FIELD(GPIO_STERT2, P29, 29, 1)
	FIELD(GPIO_STERT2, P30, 30, 1)
	FIELD(GPIO_STERT2, P31, 31, 1)
REG32(GPIO_STER0, 0x160)
	FIELD(GPIO_STER0, P0, 0, 1)
	FIELD(GPIO_STER0, P1, 1, 1)
	FIELD(GPIO_STER0, P2, 2, 1)
	FIELD(GPIO_STER0, P3, 3, 1)
	FIELD(GPIO_STER0, P4, 4, 1)
	FIELD(GPIO_STER0, P5, 5, 1)
	FIELD(GPIO_STER0, P6, 6, 1)
	FIELD(GPIO_STER0, P7, 7, 1)
	FIELD(GPIO_STER0, P8, 8, 1)
	FIELD(GPIO_STER0, P9, 9, 1)
	FIELD(GPIO_STER0, P10, 10, 1)
	FIELD(GPIO_STER0, P11, 11, 1)
	FIELD(GPIO_STER0, P12, 12, 1)
	FIELD(GPIO_STER0, P13, 13, 1)
	FIELD(GPIO_STER0, P14, 14, 1)
	FIELD(GPIO_STER0, P15, 15, 1)
	FIELD(GPIO_STER0, P16, 16, 1)
	FIELD(GPIO_STER0, P17, 17, 1)
	FIELD(GPIO_STER0, P18, 18, 1)
	FIELD(GPIO_STER0, P19, 19, 1)
	FIELD(GPIO_STER0, P20, 20, 1)
	FIELD(GPIO_STER0, P21, 21, 1)
	FIELD(GPIO_STER0, P22, 22, 1)
	FIELD(GPIO_STER0, P23, 23, 1)
	FIELD(GPIO_STER0, P24, 24, 1)
	FIELD(GPIO_STER0, P25, 25, 1)
	FIELD(GPIO_STER0, P26, 26, 1)
	FIELD(GPIO_STER0, P27, 27, 1)
	FIELD(GPIO_STER0, P28, 28, 1)
	FIELD(GPIO_STER0, P29, 29, 1)
	FIELD(GPIO_STER0, P30, 30, 1)
	FIELD(GPIO_STER0, P31, 31, 1)
REG32(GPIO_STER1, 0x360)
	FIELD(GPIO_STER1, P0, 0, 1)
	FIELD(GPIO_STER1, P1, 1, 1)
	FIELD(GPIO_STER1, P2, 2, 1)
	FIELD(GPIO_STER1, P3, 3, 1)
	FIELD(GPIO_STER1, P4, 4, 1)
	FIELD(GPIO_STER1, P5, 5, 1)
	FIELD(GPIO_STER1, P6, 6, 1)
	FIELD(GPIO_STER1, P7, 7, 1)
	FIELD(GPIO_STER1, P8, 8, 1)
	FIELD(GPIO_STER1, P9, 9, 1)
	FIELD(GPIO_STER1, P10, 10, 1)
	FIELD(GPIO_STER1, P11, 11, 1)
	FIELD(GPIO_STER1, P12, 12, 1)
	FIELD(GPIO_STER1, P13, 13, 1)
	FIELD(GPIO_STER1, P14, 14, 1)
	FIELD(GPIO_STER1, P15, 15, 1)
	FIELD(GPIO_STER1, P16, 16, 1)
	FIELD(GPIO_STER1, P17, 17, 1)
	FIELD(GPIO_STER1, P18, 18, 1)
	FIELD(GPIO_STER1, P19, 19, 1)
	FIELD(GPIO_STER1, P20, 20, 1)
	FIELD(GPIO_STER1, P21, 21, 1)
	FIELD(GPIO_STER1, P22, 22, 1)
	FIELD(GPIO_STER1, P23, 23, 1)
	FIELD(GPIO_STER1, P24, 24, 1)
	FIELD(GPIO_STER1, P25, 25, 1)
	FIELD(GPIO_STER1, P26, 26, 1)
	FIELD(GPIO_STER1, P27, 27, 1)
	FIELD(GPIO_STER1, P28, 28, 1)
	FIELD(GPIO_STER1, P29, 29, 1)
	FIELD(GPIO_STER1, P30, 30, 1)
	FIELD(GPIO_STER1, P31, 31, 1)
REG32(GPIO_STER2, 0x560)
	FIELD(GPIO_STER2, P0, 0, 1)
	FIELD(GPIO_STER2, P1, 1, 1)
	FIELD(GPIO_STER2, P2, 2, 1)
	FIELD(GPIO_STER2, P3, 3, 1)
	FIELD(GPIO_STER2, P4, 4, 1)
	FIELD(GPIO_STER2, P5, 5, 1)
	FIELD(GPIO_STER2, P6, 6, 1)
	FIELD(GPIO_STER2, P7, 7, 1)
	FIELD(GPIO_STER2, P8, 8, 1)
	FIELD(GPIO_STER2, P9, 9, 1)
	FIELD(GPIO_STER2, P10, 10, 1)
	FIELD(GPIO_STER2, P11, 11, 1)
	FIELD(GPIO_STER2, P12, 12, 1)
	FIELD(GPIO_STER2, P13, 13, 1)
	FIELD(GPIO_STER2, P14, 14, 1)
	FIELD(GPIO_STER2, P15, 15, 1)
	FIELD(GPIO_STER2, P16, 16, 1)
	FIELD(GPIO_STER2, P17, 17, 1)
	FIELD(GPIO_STER2, P18, 18, 1)
	FIELD(GPIO_STER2, P19, 19, 1)
	FIELD(GPIO_STER2, P20, 20, 1)
	FIELD(GPIO_STER2, P21, 21, 1)
	FIELD(GPIO_STER2, P22, 22, 1)
	FIELD(GPIO_STER2, P23, 23, 1)
	FIELD(GPIO_STER2, P24, 24, 1)
	FIELD(GPIO_STER2, P25, 25, 1)
	FIELD(GPIO_STER2, P26, 26, 1)
	FIELD(GPIO_STER2, P27, 27, 1)
	FIELD(GPIO_STER2, P28, 28, 1)
	FIELD(GPIO_STER2, P29, 29, 1)
	FIELD(GPIO_STER2, P30, 30, 1)
	FIELD(GPIO_STER2, P31, 31, 1)
REG32(GPIO_UNLOCK0, 0x1e0)
	FIELD(GPIO_UNLOCK0, ADDR, 0, 10)
	FIELD(GPIO_UNLOCK0, KEY, 24, 8)
REG32(GPIO_UNLOCK1, 0x3e0)
	FIELD(GPIO_UNLOCK1, ADDR, 0, 10)
	FIELD(GPIO_UNLOCK1, KEY, 24, 8)
REG32(GPIO_UNLOCK2, 0x5e0)
	FIELD(GPIO_UNLOCK2, ADDR, 0, 10)
	FIELD(GPIO_UNLOCK2, KEY, 24, 8)
REG32(GPIO_VERSION0, 0x1fc)
	FIELD(GPIO_VERSION0, VERSION, 0, 12)
	FIELD(GPIO_VERSION0, VARIANT, 16, 4)
REG32(GPIO_VERSION1, 0x3fc)
	FIELD(GPIO_VERSION1, VERSION, 0, 12)
	FIELD(GPIO_VERSION1, VARIANT, 16, 4)
REG32(GPIO_VERSION2, 0x5fc)
	FIELD(GPIO_VERSION2, VERSION, 0, 12)
	FIELD(GPIO_VERSION2, VARIANT, 16, 4)


static void atsam4lc4c_gpio_register_reset(ATSAM4LC4CGPIO *t) {
	t->VERSION0 = 0x215;
	t->VERSION1 = 0x215;
	t->VERSION2 = 0x215;

}

static uint64_t atsam4lc4c_gpio_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAM4LC4CGPIO *t = ATSAM4LC4C_GPIO(opaque);
	uint64_t ret;

	switch (offset) {
		case A_GPIO_ASR0:
			ret = t->ASR0;
			break;
		case A_GPIO_ASR1:
			ret = t->ASR1;
			break;
		case A_GPIO_ASR2:
			ret = t->ASR2;
			break;
		case A_GPIO_EVER0:
			ret = t->EVER0;
			break;
		case A_GPIO_EVER1:
			ret = t->EVER1;
			break;
		case A_GPIO_EVER2:
			ret = t->EVER2;
			break;
		case A_GPIO_GFER0:
			ret = t->GFER0;
			break;
		case A_GPIO_GFER1:
			ret = t->GFER1;
			break;
		case A_GPIO_GFER2:
			ret = t->GFER2;
			break;
		case A_GPIO_GPER0:
			ret = t->GPER0;
			break;
		case A_GPIO_GPER1:
			ret = t->GPER1;
			break;
		case A_GPIO_GPER2:
			ret = t->GPER2;
			break;
		case A_GPIO_IER0:
			ret = t->IER0;
			break;
		case A_GPIO_IER1:
			ret = t->IER1;
			break;
		case A_GPIO_IER2:
			ret = t->IER2;
			break;
		case A_GPIO_IFR0:
			ret = t->IFR0;
			break;
		case A_GPIO_IFR1:
			ret = t->IFR1;
			break;
		case A_GPIO_IFR2:
			ret = t->IFR2;
			break;
		case A_GPIO_IMR00:
			ret = t->IMR00;
			break;
		case A_GPIO_IMR01:
			ret = t->IMR01;
			break;
		case A_GPIO_IMR02:
			ret = t->IMR02;
			break;
		case A_GPIO_IMR10:
			ret = t->IMR10;
			break;
		case A_GPIO_IMR11:
			ret = t->IMR11;
			break;
		case A_GPIO_IMR12:
			ret = t->IMR12;
			break;
		case A_GPIO_LOCK0:
			ret = t->LOCK0;
			break;
		case A_GPIO_LOCK1:
			ret = t->LOCK1;
			break;
		case A_GPIO_LOCK2:
			ret = t->LOCK2;
			break;
		case A_GPIO_ODCR0C0:
			ret = t->ODCR0C0;
			break;
		case A_GPIO_ODCR0C1:
			ret = t->ODCR0C1;
			break;
		case A_GPIO_ODCR0C2:
			ret = t->ODCR0C2;
			break;
		case A_GPIO_ODCR0S0:
			ret = t->ODCR0S0;
			break;
		case A_GPIO_ODCR0S1:
			ret = t->ODCR0S1;
			break;
		case A_GPIO_ODCR0S2:
			ret = t->ODCR0S2;
			break;
		case A_GPIO_ODCR0T0:
			ret = t->ODCR0T0;
			break;
		case A_GPIO_ODCR0T1:
			ret = t->ODCR0T1;
			break;
		case A_GPIO_ODCR0T2:
			ret = t->ODCR0T2;
			break;
		case A_GPIO_ODCR1C0:
			ret = t->ODCR1C0;
			break;
		case A_GPIO_ODCR1C1:
			ret = t->ODCR1C1;
			break;
		case A_GPIO_ODCR1C2:
			ret = t->ODCR1C2;
			break;
		case A_GPIO_ODCR1S0:
			ret = t->ODCR1S0;
			break;
		case A_GPIO_ODCR1S1:
			ret = t->ODCR1S1;
			break;
		case A_GPIO_ODCR1S2:
			ret = t->ODCR1S2;
			break;
		case A_GPIO_ODCR1T0:
			ret = t->ODCR1T0;
			break;
		case A_GPIO_ODCR1T1:
			ret = t->ODCR1T1;
			break;
		case A_GPIO_ODCR1T2:
			ret = t->ODCR1T2;
			break;
		case A_GPIO_ODCR00:
			ret = t->ODCR00;
			break;
		case A_GPIO_ODCR01:
			ret = t->ODCR01;
			break;
		case A_GPIO_ODCR02:
			ret = t->ODCR02;
			break;
		case A_GPIO_ODCR10:
			ret = t->ODCR10;
			break;
		case A_GPIO_ODCR11:
			ret = t->ODCR11;
			break;
		case A_GPIO_ODCR12:
			ret = t->ODCR12;
			break;
		case A_GPIO_ODER0:
			ret = t->ODER0;
			break;
		case A_GPIO_ODER1:
			ret = t->ODER1;
			break;
		case A_GPIO_ODER2:
			ret = t->ODER2;
			break;
		case A_GPIO_ODMER0:
			ret = t->ODMER0;
			break;
		case A_GPIO_ODMER1:
			ret = t->ODMER1;
			break;
		case A_GPIO_ODMER2:
			ret = t->ODMER2;
			break;
		case A_GPIO_OSRR0C0:
			ret = t->OSRR0C0;
			break;
		case A_GPIO_OSRR0C1:
			ret = t->OSRR0C1;
			break;
		case A_GPIO_OSRR0C2:
			ret = t->OSRR0C2;
			break;
		case A_GPIO_OSRR0S0:
			ret = t->OSRR0S0;
			break;
		case A_GPIO_OSRR0S1:
			ret = t->OSRR0S1;
			break;
		case A_GPIO_OSRR0S2:
			ret = t->OSRR0S2;
			break;
		case A_GPIO_OSRR0T0:
			ret = t->OSRR0T0;
			break;
		case A_GPIO_OSRR0T1:
			ret = t->OSRR0T1;
			break;
		case A_GPIO_OSRR0T2:
			ret = t->OSRR0T2;
			break;
		case A_GPIO_OSRR00:
			ret = t->OSRR00;
			break;
		case A_GPIO_OSRR01:
			ret = t->OSRR01;
			break;
		case A_GPIO_OSRR02:
			ret = t->OSRR02;
			break;
		case A_GPIO_OVR0:
			ret = t->OVR0;
			break;
		case A_GPIO_OVR1:
			ret = t->OVR1;
			break;
		case A_GPIO_OVR2:
			ret = t->OVR2;
			break;
		case A_GPIO_PARAMETER0:
			ret = t->PARAMETER0;
			break;
		case A_GPIO_PARAMETER1:
			ret = t->PARAMETER1;
			break;
		case A_GPIO_PARAMETER2:
			ret = t->PARAMETER2;
			break;
		case A_GPIO_PDER0:
			ret = t->PDER0;
			break;
		case A_GPIO_PDER1:
			ret = t->PDER1;
			break;
		case A_GPIO_PDER2:
			ret = t->PDER2;
			break;
		case A_GPIO_PMR00:
			ret = t->PMR00;
			break;
		case A_GPIO_PMR01:
			ret = t->PMR01;
			break;
		case A_GPIO_PMR02:
			ret = t->PMR02;
			break;
		case A_GPIO_PMR10:
			ret = t->PMR10;
			break;
		case A_GPIO_PMR11:
			ret = t->PMR11;
			break;
		case A_GPIO_PMR12:
			ret = t->PMR12;
			break;
		case A_GPIO_PMR20:
			ret = t->PMR20;
			break;
		case A_GPIO_PMR21:
			ret = t->PMR21;
			break;
		case A_GPIO_PMR22:
			ret = t->PMR22;
			break;
		case A_GPIO_PUER0:
			ret = t->PUER0;
			break;
		case A_GPIO_PUER1:
			ret = t->PUER1;
			break;
		case A_GPIO_PUER2:
			ret = t->PUER2;
			break;
		case A_GPIO_PVR0:
			ret = t->PVR0;
			break;
		case A_GPIO_PVR1:
			ret = t->PVR1;
			break;
		case A_GPIO_PVR2:
			ret = t->PVR2;
			break;
		case A_GPIO_STERC0:
			ret = t->STERC0;
			break;
		case A_GPIO_STERC1:
			ret = t->STERC1;
			break;
		case A_GPIO_STERC2:
			ret = t->STERC2;
			break;
		case A_GPIO_STERS0:
			ret = t->STERS0;
			break;
		case A_GPIO_STERS1:
			ret = t->STERS1;
			break;
		case A_GPIO_STERS2:
			ret = t->STERS2;
			break;
		case A_GPIO_STERT0:
			ret = t->STERT0;
			break;
		case A_GPIO_STERT1:
			ret = t->STERT1;
			break;
		case A_GPIO_STERT2:
			ret = t->STERT2;
			break;
		case A_GPIO_STER0:
			ret = t->STER0;
			break;
		case A_GPIO_STER1:
			ret = t->STER1;
			break;
		case A_GPIO_STER2:
			ret = t->STER2;
			break;
		case A_GPIO_VERSION0:
			ret = t->VERSION0;
			break;
		case A_GPIO_VERSION1:
			ret = t->VERSION1;
			break;
		case A_GPIO_VERSION2:
			ret = t->VERSION2;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM4LC4C GPIO read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsam4lc4c_gpio_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAM4LC4CGPIO *t = ATSAM4LC4C_GPIO(opaque);


	switch (offset) {
		case A_GPIO_ASR0 ... A_GPIO_ASR0 + 3:
			value = value << ((offset - A_GPIO_ASR0) << 3);
			offset = A_GPIO_ASR0;
			break;
		case A_GPIO_ASR1 ... A_GPIO_ASR1 + 3:
			value = value << ((offset - A_GPIO_ASR1) << 3);
			offset = A_GPIO_ASR1;
			break;
		case A_GPIO_ASR2 ... A_GPIO_ASR2 + 3:
			value = value << ((offset - A_GPIO_ASR2) << 3);
			offset = A_GPIO_ASR2;
			break;
		case A_GPIO_EVERC0 ... A_GPIO_EVERC0 + 3:
			value = value << ((offset - A_GPIO_EVERC0) << 3);
			offset = A_GPIO_EVERC0;
			break;
		case A_GPIO_EVERC1 ... A_GPIO_EVERC1 + 3:
			value = value << ((offset - A_GPIO_EVERC1) << 3);
			offset = A_GPIO_EVERC1;
			break;
		case A_GPIO_EVERC2 ... A_GPIO_EVERC2 + 3:
			value = value << ((offset - A_GPIO_EVERC2) << 3);
			offset = A_GPIO_EVERC2;
			break;
		case A_GPIO_EVERS0 ... A_GPIO_EVERS0 + 3:
			value = value << ((offset - A_GPIO_EVERS0) << 3);
			offset = A_GPIO_EVERS0;
			break;
		case A_GPIO_EVERS1 ... A_GPIO_EVERS1 + 3:
			value = value << ((offset - A_GPIO_EVERS1) << 3);
			offset = A_GPIO_EVERS1;
			break;
		case A_GPIO_EVERS2 ... A_GPIO_EVERS2 + 3:
			value = value << ((offset - A_GPIO_EVERS2) << 3);
			offset = A_GPIO_EVERS2;
			break;
		case A_GPIO_EVERT0 ... A_GPIO_EVERT0 + 3:
			value = value << ((offset - A_GPIO_EVERT0) << 3);
			offset = A_GPIO_EVERT0;
			break;
		case A_GPIO_EVERT1 ... A_GPIO_EVERT1 + 3:
			value = value << ((offset - A_GPIO_EVERT1) << 3);
			offset = A_GPIO_EVERT1;
			break;
		case A_GPIO_EVERT2 ... A_GPIO_EVERT2 + 3:
			value = value << ((offset - A_GPIO_EVERT2) << 3);
			offset = A_GPIO_EVERT2;
			break;
		case A_GPIO_EVER0 ... A_GPIO_EVER0 + 3:
			value = value << ((offset - A_GPIO_EVER0) << 3);
			offset = A_GPIO_EVER0;
			break;
		case A_GPIO_EVER1 ... A_GPIO_EVER1 + 3:
			value = value << ((offset - A_GPIO_EVER1) << 3);
			offset = A_GPIO_EVER1;
			break;
		case A_GPIO_EVER2 ... A_GPIO_EVER2 + 3:
			value = value << ((offset - A_GPIO_EVER2) << 3);
			offset = A_GPIO_EVER2;
			break;
		case A_GPIO_GFERC0 ... A_GPIO_GFERC0 + 3:
			value = value << ((offset - A_GPIO_GFERC0) << 3);
			offset = A_GPIO_GFERC0;
			break;
		case A_GPIO_GFERC1 ... A_GPIO_GFERC1 + 3:
			value = value << ((offset - A_GPIO_GFERC1) << 3);
			offset = A_GPIO_GFERC1;
			break;
		case A_GPIO_GFERC2 ... A_GPIO_GFERC2 + 3:
			value = value << ((offset - A_GPIO_GFERC2) << 3);
			offset = A_GPIO_GFERC2;
			break;
		case A_GPIO_GFERS0 ... A_GPIO_GFERS0 + 3:
			value = value << ((offset - A_GPIO_GFERS0) << 3);
			offset = A_GPIO_GFERS0;
			break;
		case A_GPIO_GFERS1 ... A_GPIO_GFERS1 + 3:
			value = value << ((offset - A_GPIO_GFERS1) << 3);
			offset = A_GPIO_GFERS1;
			break;
		case A_GPIO_GFERS2 ... A_GPIO_GFERS2 + 3:
			value = value << ((offset - A_GPIO_GFERS2) << 3);
			offset = A_GPIO_GFERS2;
			break;
		case A_GPIO_GFERT0 ... A_GPIO_GFERT0 + 3:
			value = value << ((offset - A_GPIO_GFERT0) << 3);
			offset = A_GPIO_GFERT0;
			break;
		case A_GPIO_GFERT1 ... A_GPIO_GFERT1 + 3:
			value = value << ((offset - A_GPIO_GFERT1) << 3);
			offset = A_GPIO_GFERT1;
			break;
		case A_GPIO_GFERT2 ... A_GPIO_GFERT2 + 3:
			value = value << ((offset - A_GPIO_GFERT2) << 3);
			offset = A_GPIO_GFERT2;
			break;
		case A_GPIO_GFER0 ... A_GPIO_GFER0 + 3:
			value = value << ((offset - A_GPIO_GFER0) << 3);
			offset = A_GPIO_GFER0;
			break;
		case A_GPIO_GFER1 ... A_GPIO_GFER1 + 3:
			value = value << ((offset - A_GPIO_GFER1) << 3);
			offset = A_GPIO_GFER1;
			break;
		case A_GPIO_GFER2 ... A_GPIO_GFER2 + 3:
			value = value << ((offset - A_GPIO_GFER2) << 3);
			offset = A_GPIO_GFER2;
			break;
		case A_GPIO_GPERC0 ... A_GPIO_GPERC0 + 3:
			value = value << ((offset - A_GPIO_GPERC0) << 3);
			offset = A_GPIO_GPERC0;
			break;
		case A_GPIO_GPERC1 ... A_GPIO_GPERC1 + 3:
			value = value << ((offset - A_GPIO_GPERC1) << 3);
			offset = A_GPIO_GPERC1;
			break;
		case A_GPIO_GPERC2 ... A_GPIO_GPERC2 + 3:
			value = value << ((offset - A_GPIO_GPERC2) << 3);
			offset = A_GPIO_GPERC2;
			break;
		case A_GPIO_GPERS0 ... A_GPIO_GPERS0 + 3:
			value = value << ((offset - A_GPIO_GPERS0) << 3);
			offset = A_GPIO_GPERS0;
			break;
		case A_GPIO_GPERS1 ... A_GPIO_GPERS1 + 3:
			value = value << ((offset - A_GPIO_GPERS1) << 3);
			offset = A_GPIO_GPERS1;
			break;
		case A_GPIO_GPERS2 ... A_GPIO_GPERS2 + 3:
			value = value << ((offset - A_GPIO_GPERS2) << 3);
			offset = A_GPIO_GPERS2;
			break;
		case A_GPIO_GPERT0 ... A_GPIO_GPERT0 + 3:
			value = value << ((offset - A_GPIO_GPERT0) << 3);
			offset = A_GPIO_GPERT0;
			break;
		case A_GPIO_GPERT1 ... A_GPIO_GPERT1 + 3:
			value = value << ((offset - A_GPIO_GPERT1) << 3);
			offset = A_GPIO_GPERT1;
			break;
		case A_GPIO_GPERT2 ... A_GPIO_GPERT2 + 3:
			value = value << ((offset - A_GPIO_GPERT2) << 3);
			offset = A_GPIO_GPERT2;
			break;
		case A_GPIO_GPER0 ... A_GPIO_GPER0 + 3:
			value = value << ((offset - A_GPIO_GPER0) << 3);
			offset = A_GPIO_GPER0;
			break;
		case A_GPIO_GPER1 ... A_GPIO_GPER1 + 3:
			value = value << ((offset - A_GPIO_GPER1) << 3);
			offset = A_GPIO_GPER1;
			break;
		case A_GPIO_GPER2 ... A_GPIO_GPER2 + 3:
			value = value << ((offset - A_GPIO_GPER2) << 3);
			offset = A_GPIO_GPER2;
			break;
		case A_GPIO_IERC0 ... A_GPIO_IERC0 + 3:
			value = value << ((offset - A_GPIO_IERC0) << 3);
			offset = A_GPIO_IERC0;
			break;
		case A_GPIO_IERC1 ... A_GPIO_IERC1 + 3:
			value = value << ((offset - A_GPIO_IERC1) << 3);
			offset = A_GPIO_IERC1;
			break;
		case A_GPIO_IERC2 ... A_GPIO_IERC2 + 3:
			value = value << ((offset - A_GPIO_IERC2) << 3);
			offset = A_GPIO_IERC2;
			break;
		case A_GPIO_IERS0 ... A_GPIO_IERS0 + 3:
			value = value << ((offset - A_GPIO_IERS0) << 3);
			offset = A_GPIO_IERS0;
			break;
		case A_GPIO_IERS1 ... A_GPIO_IERS1 + 3:
			value = value << ((offset - A_GPIO_IERS1) << 3);
			offset = A_GPIO_IERS1;
			break;
		case A_GPIO_IERS2 ... A_GPIO_IERS2 + 3:
			value = value << ((offset - A_GPIO_IERS2) << 3);
			offset = A_GPIO_IERS2;
			break;
		case A_GPIO_IERT0 ... A_GPIO_IERT0 + 3:
			value = value << ((offset - A_GPIO_IERT0) << 3);
			offset = A_GPIO_IERT0;
			break;
		case A_GPIO_IERT1 ... A_GPIO_IERT1 + 3:
			value = value << ((offset - A_GPIO_IERT1) << 3);
			offset = A_GPIO_IERT1;
			break;
		case A_GPIO_IERT2 ... A_GPIO_IERT2 + 3:
			value = value << ((offset - A_GPIO_IERT2) << 3);
			offset = A_GPIO_IERT2;
			break;
		case A_GPIO_IER0 ... A_GPIO_IER0 + 3:
			value = value << ((offset - A_GPIO_IER0) << 3);
			offset = A_GPIO_IER0;
			break;
		case A_GPIO_IER1 ... A_GPIO_IER1 + 3:
			value = value << ((offset - A_GPIO_IER1) << 3);
			offset = A_GPIO_IER1;
			break;
		case A_GPIO_IER2 ... A_GPIO_IER2 + 3:
			value = value << ((offset - A_GPIO_IER2) << 3);
			offset = A_GPIO_IER2;
			break;
		case A_GPIO_IFRC0 ... A_GPIO_IFRC0 + 3:
			value = value << ((offset - A_GPIO_IFRC0) << 3);
			offset = A_GPIO_IFRC0;
			break;
		case A_GPIO_IFRC1 ... A_GPIO_IFRC1 + 3:
			value = value << ((offset - A_GPIO_IFRC1) << 3);
			offset = A_GPIO_IFRC1;
			break;
		case A_GPIO_IFRC2 ... A_GPIO_IFRC2 + 3:
			value = value << ((offset - A_GPIO_IFRC2) << 3);
			offset = A_GPIO_IFRC2;
			break;
		case A_GPIO_IMR0C0 ... A_GPIO_IMR0C0 + 3:
			value = value << ((offset - A_GPIO_IMR0C0) << 3);
			offset = A_GPIO_IMR0C0;
			break;
		case A_GPIO_IMR0C1 ... A_GPIO_IMR0C1 + 3:
			value = value << ((offset - A_GPIO_IMR0C1) << 3);
			offset = A_GPIO_IMR0C1;
			break;
		case A_GPIO_IMR0C2 ... A_GPIO_IMR0C2 + 3:
			value = value << ((offset - A_GPIO_IMR0C2) << 3);
			offset = A_GPIO_IMR0C2;
			break;
		case A_GPIO_IMR0S0 ... A_GPIO_IMR0S0 + 3:
			value = value << ((offset - A_GPIO_IMR0S0) << 3);
			offset = A_GPIO_IMR0S0;
			break;
		case A_GPIO_IMR0S1 ... A_GPIO_IMR0S1 + 3:
			value = value << ((offset - A_GPIO_IMR0S1) << 3);
			offset = A_GPIO_IMR0S1;
			break;
		case A_GPIO_IMR0S2 ... A_GPIO_IMR0S2 + 3:
			value = value << ((offset - A_GPIO_IMR0S2) << 3);
			offset = A_GPIO_IMR0S2;
			break;
		case A_GPIO_IMR0T0 ... A_GPIO_IMR0T0 + 3:
			value = value << ((offset - A_GPIO_IMR0T0) << 3);
			offset = A_GPIO_IMR0T0;
			break;
		case A_GPIO_IMR0T1 ... A_GPIO_IMR0T1 + 3:
			value = value << ((offset - A_GPIO_IMR0T1) << 3);
			offset = A_GPIO_IMR0T1;
			break;
		case A_GPIO_IMR0T2 ... A_GPIO_IMR0T2 + 3:
			value = value << ((offset - A_GPIO_IMR0T2) << 3);
			offset = A_GPIO_IMR0T2;
			break;
		case A_GPIO_IMR1C0 ... A_GPIO_IMR1C0 + 3:
			value = value << ((offset - A_GPIO_IMR1C0) << 3);
			offset = A_GPIO_IMR1C0;
			break;
		case A_GPIO_IMR1C1 ... A_GPIO_IMR1C1 + 3:
			value = value << ((offset - A_GPIO_IMR1C1) << 3);
			offset = A_GPIO_IMR1C1;
			break;
		case A_GPIO_IMR1C2 ... A_GPIO_IMR1C2 + 3:
			value = value << ((offset - A_GPIO_IMR1C2) << 3);
			offset = A_GPIO_IMR1C2;
			break;
		case A_GPIO_IMR1S0 ... A_GPIO_IMR1S0 + 3:
			value = value << ((offset - A_GPIO_IMR1S0) << 3);
			offset = A_GPIO_IMR1S0;
			break;
		case A_GPIO_IMR1S1 ... A_GPIO_IMR1S1 + 3:
			value = value << ((offset - A_GPIO_IMR1S1) << 3);
			offset = A_GPIO_IMR1S1;
			break;
		case A_GPIO_IMR1S2 ... A_GPIO_IMR1S2 + 3:
			value = value << ((offset - A_GPIO_IMR1S2) << 3);
			offset = A_GPIO_IMR1S2;
			break;
		case A_GPIO_IMR1T0 ... A_GPIO_IMR1T0 + 3:
			value = value << ((offset - A_GPIO_IMR1T0) << 3);
			offset = A_GPIO_IMR1T0;
			break;
		case A_GPIO_IMR1T1 ... A_GPIO_IMR1T1 + 3:
			value = value << ((offset - A_GPIO_IMR1T1) << 3);
			offset = A_GPIO_IMR1T1;
			break;
		case A_GPIO_IMR1T2 ... A_GPIO_IMR1T2 + 3:
			value = value << ((offset - A_GPIO_IMR1T2) << 3);
			offset = A_GPIO_IMR1T2;
			break;
		case A_GPIO_IMR00 ... A_GPIO_IMR00 + 3:
			value = value << ((offset - A_GPIO_IMR00) << 3);
			offset = A_GPIO_IMR00;
			break;
		case A_GPIO_IMR01 ... A_GPIO_IMR01 + 3:
			value = value << ((offset - A_GPIO_IMR01) << 3);
			offset = A_GPIO_IMR01;
			break;
		case A_GPIO_IMR02 ... A_GPIO_IMR02 + 3:
			value = value << ((offset - A_GPIO_IMR02) << 3);
			offset = A_GPIO_IMR02;
			break;
		case A_GPIO_IMR10 ... A_GPIO_IMR10 + 3:
			value = value << ((offset - A_GPIO_IMR10) << 3);
			offset = A_GPIO_IMR10;
			break;
		case A_GPIO_IMR11 ... A_GPIO_IMR11 + 3:
			value = value << ((offset - A_GPIO_IMR11) << 3);
			offset = A_GPIO_IMR11;
			break;
		case A_GPIO_IMR12 ... A_GPIO_IMR12 + 3:
			value = value << ((offset - A_GPIO_IMR12) << 3);
			offset = A_GPIO_IMR12;
			break;
		case A_GPIO_LOCKC0 ... A_GPIO_LOCKC0 + 3:
			value = value << ((offset - A_GPIO_LOCKC0) << 3);
			offset = A_GPIO_LOCKC0;
			break;
		case A_GPIO_LOCKC1 ... A_GPIO_LOCKC1 + 3:
			value = value << ((offset - A_GPIO_LOCKC1) << 3);
			offset = A_GPIO_LOCKC1;
			break;
		case A_GPIO_LOCKC2 ... A_GPIO_LOCKC2 + 3:
			value = value << ((offset - A_GPIO_LOCKC2) << 3);
			offset = A_GPIO_LOCKC2;
			break;
		case A_GPIO_LOCKS0 ... A_GPIO_LOCKS0 + 3:
			value = value << ((offset - A_GPIO_LOCKS0) << 3);
			offset = A_GPIO_LOCKS0;
			break;
		case A_GPIO_LOCKS1 ... A_GPIO_LOCKS1 + 3:
			value = value << ((offset - A_GPIO_LOCKS1) << 3);
			offset = A_GPIO_LOCKS1;
			break;
		case A_GPIO_LOCKS2 ... A_GPIO_LOCKS2 + 3:
			value = value << ((offset - A_GPIO_LOCKS2) << 3);
			offset = A_GPIO_LOCKS2;
			break;
		case A_GPIO_LOCKT0 ... A_GPIO_LOCKT0 + 3:
			value = value << ((offset - A_GPIO_LOCKT0) << 3);
			offset = A_GPIO_LOCKT0;
			break;
		case A_GPIO_LOCKT1 ... A_GPIO_LOCKT1 + 3:
			value = value << ((offset - A_GPIO_LOCKT1) << 3);
			offset = A_GPIO_LOCKT1;
			break;
		case A_GPIO_LOCKT2 ... A_GPIO_LOCKT2 + 3:
			value = value << ((offset - A_GPIO_LOCKT2) << 3);
			offset = A_GPIO_LOCKT2;
			break;
		case A_GPIO_LOCK0 ... A_GPIO_LOCK0 + 3:
			value = value << ((offset - A_GPIO_LOCK0) << 3);
			offset = A_GPIO_LOCK0;
			break;
		case A_GPIO_LOCK1 ... A_GPIO_LOCK1 + 3:
			value = value << ((offset - A_GPIO_LOCK1) << 3);
			offset = A_GPIO_LOCK1;
			break;
		case A_GPIO_LOCK2 ... A_GPIO_LOCK2 + 3:
			value = value << ((offset - A_GPIO_LOCK2) << 3);
			offset = A_GPIO_LOCK2;
			break;
		case A_GPIO_ODCR0C0 ... A_GPIO_ODCR0C0 + 3:
			value = value << ((offset - A_GPIO_ODCR0C0) << 3);
			offset = A_GPIO_ODCR0C0;
			break;
		case A_GPIO_ODCR0C1 ... A_GPIO_ODCR0C1 + 3:
			value = value << ((offset - A_GPIO_ODCR0C1) << 3);
			offset = A_GPIO_ODCR0C1;
			break;
		case A_GPIO_ODCR0C2 ... A_GPIO_ODCR0C2 + 3:
			value = value << ((offset - A_GPIO_ODCR0C2) << 3);
			offset = A_GPIO_ODCR0C2;
			break;
		case A_GPIO_ODCR0S0 ... A_GPIO_ODCR0S0 + 3:
			value = value << ((offset - A_GPIO_ODCR0S0) << 3);
			offset = A_GPIO_ODCR0S0;
			break;
		case A_GPIO_ODCR0S1 ... A_GPIO_ODCR0S1 + 3:
			value = value << ((offset - A_GPIO_ODCR0S1) << 3);
			offset = A_GPIO_ODCR0S1;
			break;
		case A_GPIO_ODCR0S2 ... A_GPIO_ODCR0S2 + 3:
			value = value << ((offset - A_GPIO_ODCR0S2) << 3);
			offset = A_GPIO_ODCR0S2;
			break;
		case A_GPIO_ODCR0T0 ... A_GPIO_ODCR0T0 + 3:
			value = value << ((offset - A_GPIO_ODCR0T0) << 3);
			offset = A_GPIO_ODCR0T0;
			break;
		case A_GPIO_ODCR0T1 ... A_GPIO_ODCR0T1 + 3:
			value = value << ((offset - A_GPIO_ODCR0T1) << 3);
			offset = A_GPIO_ODCR0T1;
			break;
		case A_GPIO_ODCR0T2 ... A_GPIO_ODCR0T2 + 3:
			value = value << ((offset - A_GPIO_ODCR0T2) << 3);
			offset = A_GPIO_ODCR0T2;
			break;
		case A_GPIO_ODCR1C0 ... A_GPIO_ODCR1C0 + 3:
			value = value << ((offset - A_GPIO_ODCR1C0) << 3);
			offset = A_GPIO_ODCR1C0;
			break;
		case A_GPIO_ODCR1C1 ... A_GPIO_ODCR1C1 + 3:
			value = value << ((offset - A_GPIO_ODCR1C1) << 3);
			offset = A_GPIO_ODCR1C1;
			break;
		case A_GPIO_ODCR1C2 ... A_GPIO_ODCR1C2 + 3:
			value = value << ((offset - A_GPIO_ODCR1C2) << 3);
			offset = A_GPIO_ODCR1C2;
			break;
		case A_GPIO_ODCR1S0 ... A_GPIO_ODCR1S0 + 3:
			value = value << ((offset - A_GPIO_ODCR1S0) << 3);
			offset = A_GPIO_ODCR1S0;
			break;
		case A_GPIO_ODCR1S1 ... A_GPIO_ODCR1S1 + 3:
			value = value << ((offset - A_GPIO_ODCR1S1) << 3);
			offset = A_GPIO_ODCR1S1;
			break;
		case A_GPIO_ODCR1S2 ... A_GPIO_ODCR1S2 + 3:
			value = value << ((offset - A_GPIO_ODCR1S2) << 3);
			offset = A_GPIO_ODCR1S2;
			break;
		case A_GPIO_ODCR1T0 ... A_GPIO_ODCR1T0 + 3:
			value = value << ((offset - A_GPIO_ODCR1T0) << 3);
			offset = A_GPIO_ODCR1T0;
			break;
		case A_GPIO_ODCR1T1 ... A_GPIO_ODCR1T1 + 3:
			value = value << ((offset - A_GPIO_ODCR1T1) << 3);
			offset = A_GPIO_ODCR1T1;
			break;
		case A_GPIO_ODCR1T2 ... A_GPIO_ODCR1T2 + 3:
			value = value << ((offset - A_GPIO_ODCR1T2) << 3);
			offset = A_GPIO_ODCR1T2;
			break;
		case A_GPIO_ODCR00 ... A_GPIO_ODCR00 + 3:
			value = value << ((offset - A_GPIO_ODCR00) << 3);
			offset = A_GPIO_ODCR00;
			break;
		case A_GPIO_ODCR01 ... A_GPIO_ODCR01 + 3:
			value = value << ((offset - A_GPIO_ODCR01) << 3);
			offset = A_GPIO_ODCR01;
			break;
		case A_GPIO_ODCR02 ... A_GPIO_ODCR02 + 3:
			value = value << ((offset - A_GPIO_ODCR02) << 3);
			offset = A_GPIO_ODCR02;
			break;
		case A_GPIO_ODCR10 ... A_GPIO_ODCR10 + 3:
			value = value << ((offset - A_GPIO_ODCR10) << 3);
			offset = A_GPIO_ODCR10;
			break;
		case A_GPIO_ODCR11 ... A_GPIO_ODCR11 + 3:
			value = value << ((offset - A_GPIO_ODCR11) << 3);
			offset = A_GPIO_ODCR11;
			break;
		case A_GPIO_ODCR12 ... A_GPIO_ODCR12 + 3:
			value = value << ((offset - A_GPIO_ODCR12) << 3);
			offset = A_GPIO_ODCR12;
			break;
		case A_GPIO_ODERC0 ... A_GPIO_ODERC0 + 3:
			value = value << ((offset - A_GPIO_ODERC0) << 3);
			offset = A_GPIO_ODERC0;
			break;
		case A_GPIO_ODERC1 ... A_GPIO_ODERC1 + 3:
			value = value << ((offset - A_GPIO_ODERC1) << 3);
			offset = A_GPIO_ODERC1;
			break;
		case A_GPIO_ODERC2 ... A_GPIO_ODERC2 + 3:
			value = value << ((offset - A_GPIO_ODERC2) << 3);
			offset = A_GPIO_ODERC2;
			break;
		case A_GPIO_ODERS0 ... A_GPIO_ODERS0 + 3:
			value = value << ((offset - A_GPIO_ODERS0) << 3);
			offset = A_GPIO_ODERS0;
			break;
		case A_GPIO_ODERS1 ... A_GPIO_ODERS1 + 3:
			value = value << ((offset - A_GPIO_ODERS1) << 3);
			offset = A_GPIO_ODERS1;
			break;
		case A_GPIO_ODERS2 ... A_GPIO_ODERS2 + 3:
			value = value << ((offset - A_GPIO_ODERS2) << 3);
			offset = A_GPIO_ODERS2;
			break;
		case A_GPIO_ODERT0 ... A_GPIO_ODERT0 + 3:
			value = value << ((offset - A_GPIO_ODERT0) << 3);
			offset = A_GPIO_ODERT0;
			break;
		case A_GPIO_ODERT1 ... A_GPIO_ODERT1 + 3:
			value = value << ((offset - A_GPIO_ODERT1) << 3);
			offset = A_GPIO_ODERT1;
			break;
		case A_GPIO_ODERT2 ... A_GPIO_ODERT2 + 3:
			value = value << ((offset - A_GPIO_ODERT2) << 3);
			offset = A_GPIO_ODERT2;
			break;
		case A_GPIO_ODER0 ... A_GPIO_ODER0 + 3:
			value = value << ((offset - A_GPIO_ODER0) << 3);
			offset = A_GPIO_ODER0;
			break;
		case A_GPIO_ODER1 ... A_GPIO_ODER1 + 3:
			value = value << ((offset - A_GPIO_ODER1) << 3);
			offset = A_GPIO_ODER1;
			break;
		case A_GPIO_ODER2 ... A_GPIO_ODER2 + 3:
			value = value << ((offset - A_GPIO_ODER2) << 3);
			offset = A_GPIO_ODER2;
			break;
		case A_GPIO_ODMERC0 ... A_GPIO_ODMERC0 + 3:
			value = value << ((offset - A_GPIO_ODMERC0) << 3);
			offset = A_GPIO_ODMERC0;
			break;
		case A_GPIO_ODMERC1 ... A_GPIO_ODMERC1 + 3:
			value = value << ((offset - A_GPIO_ODMERC1) << 3);
			offset = A_GPIO_ODMERC1;
			break;
		case A_GPIO_ODMERC2 ... A_GPIO_ODMERC2 + 3:
			value = value << ((offset - A_GPIO_ODMERC2) << 3);
			offset = A_GPIO_ODMERC2;
			break;
		case A_GPIO_ODMERS0 ... A_GPIO_ODMERS0 + 3:
			value = value << ((offset - A_GPIO_ODMERS0) << 3);
			offset = A_GPIO_ODMERS0;
			break;
		case A_GPIO_ODMERS1 ... A_GPIO_ODMERS1 + 3:
			value = value << ((offset - A_GPIO_ODMERS1) << 3);
			offset = A_GPIO_ODMERS1;
			break;
		case A_GPIO_ODMERS2 ... A_GPIO_ODMERS2 + 3:
			value = value << ((offset - A_GPIO_ODMERS2) << 3);
			offset = A_GPIO_ODMERS2;
			break;
		case A_GPIO_ODMERT0 ... A_GPIO_ODMERT0 + 3:
			value = value << ((offset - A_GPIO_ODMERT0) << 3);
			offset = A_GPIO_ODMERT0;
			break;
		case A_GPIO_ODMERT1 ... A_GPIO_ODMERT1 + 3:
			value = value << ((offset - A_GPIO_ODMERT1) << 3);
			offset = A_GPIO_ODMERT1;
			break;
		case A_GPIO_ODMERT2 ... A_GPIO_ODMERT2 + 3:
			value = value << ((offset - A_GPIO_ODMERT2) << 3);
			offset = A_GPIO_ODMERT2;
			break;
		case A_GPIO_ODMER0 ... A_GPIO_ODMER0 + 3:
			value = value << ((offset - A_GPIO_ODMER0) << 3);
			offset = A_GPIO_ODMER0;
			break;
		case A_GPIO_ODMER1 ... A_GPIO_ODMER1 + 3:
			value = value << ((offset - A_GPIO_ODMER1) << 3);
			offset = A_GPIO_ODMER1;
			break;
		case A_GPIO_ODMER2 ... A_GPIO_ODMER2 + 3:
			value = value << ((offset - A_GPIO_ODMER2) << 3);
			offset = A_GPIO_ODMER2;
			break;
		case A_GPIO_OSRR0C0 ... A_GPIO_OSRR0C0 + 3:
			value = value << ((offset - A_GPIO_OSRR0C0) << 3);
			offset = A_GPIO_OSRR0C0;
			break;
		case A_GPIO_OSRR0C1 ... A_GPIO_OSRR0C1 + 3:
			value = value << ((offset - A_GPIO_OSRR0C1) << 3);
			offset = A_GPIO_OSRR0C1;
			break;
		case A_GPIO_OSRR0C2 ... A_GPIO_OSRR0C2 + 3:
			value = value << ((offset - A_GPIO_OSRR0C2) << 3);
			offset = A_GPIO_OSRR0C2;
			break;
		case A_GPIO_OSRR0S0 ... A_GPIO_OSRR0S0 + 3:
			value = value << ((offset - A_GPIO_OSRR0S0) << 3);
			offset = A_GPIO_OSRR0S0;
			break;
		case A_GPIO_OSRR0S1 ... A_GPIO_OSRR0S1 + 3:
			value = value << ((offset - A_GPIO_OSRR0S1) << 3);
			offset = A_GPIO_OSRR0S1;
			break;
		case A_GPIO_OSRR0S2 ... A_GPIO_OSRR0S2 + 3:
			value = value << ((offset - A_GPIO_OSRR0S2) << 3);
			offset = A_GPIO_OSRR0S2;
			break;
		case A_GPIO_OSRR0T0 ... A_GPIO_OSRR0T0 + 3:
			value = value << ((offset - A_GPIO_OSRR0T0) << 3);
			offset = A_GPIO_OSRR0T0;
			break;
		case A_GPIO_OSRR0T1 ... A_GPIO_OSRR0T1 + 3:
			value = value << ((offset - A_GPIO_OSRR0T1) << 3);
			offset = A_GPIO_OSRR0T1;
			break;
		case A_GPIO_OSRR0T2 ... A_GPIO_OSRR0T2 + 3:
			value = value << ((offset - A_GPIO_OSRR0T2) << 3);
			offset = A_GPIO_OSRR0T2;
			break;
		case A_GPIO_OSRR00 ... A_GPIO_OSRR00 + 3:
			value = value << ((offset - A_GPIO_OSRR00) << 3);
			offset = A_GPIO_OSRR00;
			break;
		case A_GPIO_OSRR01 ... A_GPIO_OSRR01 + 3:
			value = value << ((offset - A_GPIO_OSRR01) << 3);
			offset = A_GPIO_OSRR01;
			break;
		case A_GPIO_OSRR02 ... A_GPIO_OSRR02 + 3:
			value = value << ((offset - A_GPIO_OSRR02) << 3);
			offset = A_GPIO_OSRR02;
			break;
		case A_GPIO_OVRC0 ... A_GPIO_OVRC0 + 3:
			value = value << ((offset - A_GPIO_OVRC0) << 3);
			offset = A_GPIO_OVRC0;
			break;
		case A_GPIO_OVRC1 ... A_GPIO_OVRC1 + 3:
			value = value << ((offset - A_GPIO_OVRC1) << 3);
			offset = A_GPIO_OVRC1;
			break;
		case A_GPIO_OVRC2 ... A_GPIO_OVRC2 + 3:
			value = value << ((offset - A_GPIO_OVRC2) << 3);
			offset = A_GPIO_OVRC2;
			break;
		case A_GPIO_OVRS0 ... A_GPIO_OVRS0 + 3:
			value = value << ((offset - A_GPIO_OVRS0) << 3);
			offset = A_GPIO_OVRS0;
			break;
		case A_GPIO_OVRS1 ... A_GPIO_OVRS1 + 3:
			value = value << ((offset - A_GPIO_OVRS1) << 3);
			offset = A_GPIO_OVRS1;
			break;
		case A_GPIO_OVRS2 ... A_GPIO_OVRS2 + 3:
			value = value << ((offset - A_GPIO_OVRS2) << 3);
			offset = A_GPIO_OVRS2;
			break;
		case A_GPIO_OVRT0 ... A_GPIO_OVRT0 + 3:
			value = value << ((offset - A_GPIO_OVRT0) << 3);
			offset = A_GPIO_OVRT0;
			break;
		case A_GPIO_OVRT1 ... A_GPIO_OVRT1 + 3:
			value = value << ((offset - A_GPIO_OVRT1) << 3);
			offset = A_GPIO_OVRT1;
			break;
		case A_GPIO_OVRT2 ... A_GPIO_OVRT2 + 3:
			value = value << ((offset - A_GPIO_OVRT2) << 3);
			offset = A_GPIO_OVRT2;
			break;
		case A_GPIO_OVR0 ... A_GPIO_OVR0 + 3:
			value = value << ((offset - A_GPIO_OVR0) << 3);
			offset = A_GPIO_OVR0;
			break;
		case A_GPIO_OVR1 ... A_GPIO_OVR1 + 3:
			value = value << ((offset - A_GPIO_OVR1) << 3);
			offset = A_GPIO_OVR1;
			break;
		case A_GPIO_OVR2 ... A_GPIO_OVR2 + 3:
			value = value << ((offset - A_GPIO_OVR2) << 3);
			offset = A_GPIO_OVR2;
			break;
		case A_GPIO_PDERC0 ... A_GPIO_PDERC0 + 3:
			value = value << ((offset - A_GPIO_PDERC0) << 3);
			offset = A_GPIO_PDERC0;
			break;
		case A_GPIO_PDERC1 ... A_GPIO_PDERC1 + 3:
			value = value << ((offset - A_GPIO_PDERC1) << 3);
			offset = A_GPIO_PDERC1;
			break;
		case A_GPIO_PDERC2 ... A_GPIO_PDERC2 + 3:
			value = value << ((offset - A_GPIO_PDERC2) << 3);
			offset = A_GPIO_PDERC2;
			break;
		case A_GPIO_PDERS0 ... A_GPIO_PDERS0 + 3:
			value = value << ((offset - A_GPIO_PDERS0) << 3);
			offset = A_GPIO_PDERS0;
			break;
		case A_GPIO_PDERS1 ... A_GPIO_PDERS1 + 3:
			value = value << ((offset - A_GPIO_PDERS1) << 3);
			offset = A_GPIO_PDERS1;
			break;
		case A_GPIO_PDERS2 ... A_GPIO_PDERS2 + 3:
			value = value << ((offset - A_GPIO_PDERS2) << 3);
			offset = A_GPIO_PDERS2;
			break;
		case A_GPIO_PDERT0 ... A_GPIO_PDERT0 + 3:
			value = value << ((offset - A_GPIO_PDERT0) << 3);
			offset = A_GPIO_PDERT0;
			break;
		case A_GPIO_PDERT1 ... A_GPIO_PDERT1 + 3:
			value = value << ((offset - A_GPIO_PDERT1) << 3);
			offset = A_GPIO_PDERT1;
			break;
		case A_GPIO_PDERT2 ... A_GPIO_PDERT2 + 3:
			value = value << ((offset - A_GPIO_PDERT2) << 3);
			offset = A_GPIO_PDERT2;
			break;
		case A_GPIO_PDER0 ... A_GPIO_PDER0 + 3:
			value = value << ((offset - A_GPIO_PDER0) << 3);
			offset = A_GPIO_PDER0;
			break;
		case A_GPIO_PDER1 ... A_GPIO_PDER1 + 3:
			value = value << ((offset - A_GPIO_PDER1) << 3);
			offset = A_GPIO_PDER1;
			break;
		case A_GPIO_PDER2 ... A_GPIO_PDER2 + 3:
			value = value << ((offset - A_GPIO_PDER2) << 3);
			offset = A_GPIO_PDER2;
			break;
		case A_GPIO_PMR0C0 ... A_GPIO_PMR0C0 + 3:
			value = value << ((offset - A_GPIO_PMR0C0) << 3);
			offset = A_GPIO_PMR0C0;
			break;
		case A_GPIO_PMR0C1 ... A_GPIO_PMR0C1 + 3:
			value = value << ((offset - A_GPIO_PMR0C1) << 3);
			offset = A_GPIO_PMR0C1;
			break;
		case A_GPIO_PMR0C2 ... A_GPIO_PMR0C2 + 3:
			value = value << ((offset - A_GPIO_PMR0C2) << 3);
			offset = A_GPIO_PMR0C2;
			break;
		case A_GPIO_PMR0S0 ... A_GPIO_PMR0S0 + 3:
			value = value << ((offset - A_GPIO_PMR0S0) << 3);
			offset = A_GPIO_PMR0S0;
			break;
		case A_GPIO_PMR0S1 ... A_GPIO_PMR0S1 + 3:
			value = value << ((offset - A_GPIO_PMR0S1) << 3);
			offset = A_GPIO_PMR0S1;
			break;
		case A_GPIO_PMR0S2 ... A_GPIO_PMR0S2 + 3:
			value = value << ((offset - A_GPIO_PMR0S2) << 3);
			offset = A_GPIO_PMR0S2;
			break;
		case A_GPIO_PMR0T0 ... A_GPIO_PMR0T0 + 3:
			value = value << ((offset - A_GPIO_PMR0T0) << 3);
			offset = A_GPIO_PMR0T0;
			break;
		case A_GPIO_PMR0T1 ... A_GPIO_PMR0T1 + 3:
			value = value << ((offset - A_GPIO_PMR0T1) << 3);
			offset = A_GPIO_PMR0T1;
			break;
		case A_GPIO_PMR0T2 ... A_GPIO_PMR0T2 + 3:
			value = value << ((offset - A_GPIO_PMR0T2) << 3);
			offset = A_GPIO_PMR0T2;
			break;
		case A_GPIO_PMR1C0 ... A_GPIO_PMR1C0 + 3:
			value = value << ((offset - A_GPIO_PMR1C0) << 3);
			offset = A_GPIO_PMR1C0;
			break;
		case A_GPIO_PMR1C1 ... A_GPIO_PMR1C1 + 3:
			value = value << ((offset - A_GPIO_PMR1C1) << 3);
			offset = A_GPIO_PMR1C1;
			break;
		case A_GPIO_PMR1C2 ... A_GPIO_PMR1C2 + 3:
			value = value << ((offset - A_GPIO_PMR1C2) << 3);
			offset = A_GPIO_PMR1C2;
			break;
		case A_GPIO_PMR1S0 ... A_GPIO_PMR1S0 + 3:
			value = value << ((offset - A_GPIO_PMR1S0) << 3);
			offset = A_GPIO_PMR1S0;
			break;
		case A_GPIO_PMR1S1 ... A_GPIO_PMR1S1 + 3:
			value = value << ((offset - A_GPIO_PMR1S1) << 3);
			offset = A_GPIO_PMR1S1;
			break;
		case A_GPIO_PMR1S2 ... A_GPIO_PMR1S2 + 3:
			value = value << ((offset - A_GPIO_PMR1S2) << 3);
			offset = A_GPIO_PMR1S2;
			break;
		case A_GPIO_PMR1T0 ... A_GPIO_PMR1T0 + 3:
			value = value << ((offset - A_GPIO_PMR1T0) << 3);
			offset = A_GPIO_PMR1T0;
			break;
		case A_GPIO_PMR1T1 ... A_GPIO_PMR1T1 + 3:
			value = value << ((offset - A_GPIO_PMR1T1) << 3);
			offset = A_GPIO_PMR1T1;
			break;
		case A_GPIO_PMR1T2 ... A_GPIO_PMR1T2 + 3:
			value = value << ((offset - A_GPIO_PMR1T2) << 3);
			offset = A_GPIO_PMR1T2;
			break;
		case A_GPIO_PMR2C0 ... A_GPIO_PMR2C0 + 3:
			value = value << ((offset - A_GPIO_PMR2C0) << 3);
			offset = A_GPIO_PMR2C0;
			break;
		case A_GPIO_PMR2C1 ... A_GPIO_PMR2C1 + 3:
			value = value << ((offset - A_GPIO_PMR2C1) << 3);
			offset = A_GPIO_PMR2C1;
			break;
		case A_GPIO_PMR2C2 ... A_GPIO_PMR2C2 + 3:
			value = value << ((offset - A_GPIO_PMR2C2) << 3);
			offset = A_GPIO_PMR2C2;
			break;
		case A_GPIO_PMR2S0 ... A_GPIO_PMR2S0 + 3:
			value = value << ((offset - A_GPIO_PMR2S0) << 3);
			offset = A_GPIO_PMR2S0;
			break;
		case A_GPIO_PMR2S1 ... A_GPIO_PMR2S1 + 3:
			value = value << ((offset - A_GPIO_PMR2S1) << 3);
			offset = A_GPIO_PMR2S1;
			break;
		case A_GPIO_PMR2S2 ... A_GPIO_PMR2S2 + 3:
			value = value << ((offset - A_GPIO_PMR2S2) << 3);
			offset = A_GPIO_PMR2S2;
			break;
		case A_GPIO_PMR2T0 ... A_GPIO_PMR2T0 + 3:
			value = value << ((offset - A_GPIO_PMR2T0) << 3);
			offset = A_GPIO_PMR2T0;
			break;
		case A_GPIO_PMR2T1 ... A_GPIO_PMR2T1 + 3:
			value = value << ((offset - A_GPIO_PMR2T1) << 3);
			offset = A_GPIO_PMR2T1;
			break;
		case A_GPIO_PMR2T2 ... A_GPIO_PMR2T2 + 3:
			value = value << ((offset - A_GPIO_PMR2T2) << 3);
			offset = A_GPIO_PMR2T2;
			break;
		case A_GPIO_PMR00 ... A_GPIO_PMR00 + 3:
			value = value << ((offset - A_GPIO_PMR00) << 3);
			offset = A_GPIO_PMR00;
			break;
		case A_GPIO_PMR01 ... A_GPIO_PMR01 + 3:
			value = value << ((offset - A_GPIO_PMR01) << 3);
			offset = A_GPIO_PMR01;
			break;
		case A_GPIO_PMR02 ... A_GPIO_PMR02 + 3:
			value = value << ((offset - A_GPIO_PMR02) << 3);
			offset = A_GPIO_PMR02;
			break;
		case A_GPIO_PMR10 ... A_GPIO_PMR10 + 3:
			value = value << ((offset - A_GPIO_PMR10) << 3);
			offset = A_GPIO_PMR10;
			break;
		case A_GPIO_PMR11 ... A_GPIO_PMR11 + 3:
			value = value << ((offset - A_GPIO_PMR11) << 3);
			offset = A_GPIO_PMR11;
			break;
		case A_GPIO_PMR12 ... A_GPIO_PMR12 + 3:
			value = value << ((offset - A_GPIO_PMR12) << 3);
			offset = A_GPIO_PMR12;
			break;
		case A_GPIO_PMR20 ... A_GPIO_PMR20 + 3:
			value = value << ((offset - A_GPIO_PMR20) << 3);
			offset = A_GPIO_PMR20;
			break;
		case A_GPIO_PMR21 ... A_GPIO_PMR21 + 3:
			value = value << ((offset - A_GPIO_PMR21) << 3);
			offset = A_GPIO_PMR21;
			break;
		case A_GPIO_PMR22 ... A_GPIO_PMR22 + 3:
			value = value << ((offset - A_GPIO_PMR22) << 3);
			offset = A_GPIO_PMR22;
			break;
		case A_GPIO_PUERC0 ... A_GPIO_PUERC0 + 3:
			value = value << ((offset - A_GPIO_PUERC0) << 3);
			offset = A_GPIO_PUERC0;
			break;
		case A_GPIO_PUERC1 ... A_GPIO_PUERC1 + 3:
			value = value << ((offset - A_GPIO_PUERC1) << 3);
			offset = A_GPIO_PUERC1;
			break;
		case A_GPIO_PUERC2 ... A_GPIO_PUERC2 + 3:
			value = value << ((offset - A_GPIO_PUERC2) << 3);
			offset = A_GPIO_PUERC2;
			break;
		case A_GPIO_PUERS0 ... A_GPIO_PUERS0 + 3:
			value = value << ((offset - A_GPIO_PUERS0) << 3);
			offset = A_GPIO_PUERS0;
			break;
		case A_GPIO_PUERS1 ... A_GPIO_PUERS1 + 3:
			value = value << ((offset - A_GPIO_PUERS1) << 3);
			offset = A_GPIO_PUERS1;
			break;
		case A_GPIO_PUERS2 ... A_GPIO_PUERS2 + 3:
			value = value << ((offset - A_GPIO_PUERS2) << 3);
			offset = A_GPIO_PUERS2;
			break;
		case A_GPIO_PUERT0 ... A_GPIO_PUERT0 + 3:
			value = value << ((offset - A_GPIO_PUERT0) << 3);
			offset = A_GPIO_PUERT0;
			break;
		case A_GPIO_PUERT1 ... A_GPIO_PUERT1 + 3:
			value = value << ((offset - A_GPIO_PUERT1) << 3);
			offset = A_GPIO_PUERT1;
			break;
		case A_GPIO_PUERT2 ... A_GPIO_PUERT2 + 3:
			value = value << ((offset - A_GPIO_PUERT2) << 3);
			offset = A_GPIO_PUERT2;
			break;
		case A_GPIO_PUER0 ... A_GPIO_PUER0 + 3:
			value = value << ((offset - A_GPIO_PUER0) << 3);
			offset = A_GPIO_PUER0;
			break;
		case A_GPIO_PUER1 ... A_GPIO_PUER1 + 3:
			value = value << ((offset - A_GPIO_PUER1) << 3);
			offset = A_GPIO_PUER1;
			break;
		case A_GPIO_PUER2 ... A_GPIO_PUER2 + 3:
			value = value << ((offset - A_GPIO_PUER2) << 3);
			offset = A_GPIO_PUER2;
			break;
		case A_GPIO_STERC0 ... A_GPIO_STERC0 + 3:
			value = value << ((offset - A_GPIO_STERC0) << 3);
			offset = A_GPIO_STERC0;
			break;
		case A_GPIO_STERC1 ... A_GPIO_STERC1 + 3:
			value = value << ((offset - A_GPIO_STERC1) << 3);
			offset = A_GPIO_STERC1;
			break;
		case A_GPIO_STERC2 ... A_GPIO_STERC2 + 3:
			value = value << ((offset - A_GPIO_STERC2) << 3);
			offset = A_GPIO_STERC2;
			break;
		case A_GPIO_STERS0 ... A_GPIO_STERS0 + 3:
			value = value << ((offset - A_GPIO_STERS0) << 3);
			offset = A_GPIO_STERS0;
			break;
		case A_GPIO_STERS1 ... A_GPIO_STERS1 + 3:
			value = value << ((offset - A_GPIO_STERS1) << 3);
			offset = A_GPIO_STERS1;
			break;
		case A_GPIO_STERS2 ... A_GPIO_STERS2 + 3:
			value = value << ((offset - A_GPIO_STERS2) << 3);
			offset = A_GPIO_STERS2;
			break;
		case A_GPIO_STERT0 ... A_GPIO_STERT0 + 3:
			value = value << ((offset - A_GPIO_STERT0) << 3);
			offset = A_GPIO_STERT0;
			break;
		case A_GPIO_STERT1 ... A_GPIO_STERT1 + 3:
			value = value << ((offset - A_GPIO_STERT1) << 3);
			offset = A_GPIO_STERT1;
			break;
		case A_GPIO_STERT2 ... A_GPIO_STERT2 + 3:
			value = value << ((offset - A_GPIO_STERT2) << 3);
			offset = A_GPIO_STERT2;
			break;
		case A_GPIO_STER0 ... A_GPIO_STER0 + 3:
			value = value << ((offset - A_GPIO_STER0) << 3);
			offset = A_GPIO_STER0;
			break;
		case A_GPIO_STER1 ... A_GPIO_STER1 + 3:
			value = value << ((offset - A_GPIO_STER1) << 3);
			offset = A_GPIO_STER1;
			break;
		case A_GPIO_STER2 ... A_GPIO_STER2 + 3:
			value = value << ((offset - A_GPIO_STER2) << 3);
			offset = A_GPIO_STER2;
			break;
		case A_GPIO_UNLOCK0 ... A_GPIO_UNLOCK0 + 3:
			value = value << ((offset - A_GPIO_UNLOCK0) << 3);
			offset = A_GPIO_UNLOCK0;
			break;
		case A_GPIO_UNLOCK1 ... A_GPIO_UNLOCK1 + 3:
			value = value << ((offset - A_GPIO_UNLOCK1) << 3);
			offset = A_GPIO_UNLOCK1;
			break;
		case A_GPIO_UNLOCK2 ... A_GPIO_UNLOCK2 + 3:
			value = value << ((offset - A_GPIO_UNLOCK2) << 3);
			offset = A_GPIO_UNLOCK2;
			break;

		default: break;
	}

	switch (offset) {
		case A_GPIO_ASR0:
			t->ASR0 = value;
			break;
		case A_GPIO_ASR1:
			t->ASR1 = value;
			break;
		case A_GPIO_ASR2:
			t->ASR2 = value;
			break;
		case A_GPIO_EVERC0:
			t->EVERC0 = value;
			break;
		case A_GPIO_EVERC1:
			t->EVERC1 = value;
			break;
		case A_GPIO_EVERC2:
			t->EVERC2 = value;
			break;
		case A_GPIO_EVERS0:
			t->EVERS0 = value;
			break;
		case A_GPIO_EVERS1:
			t->EVERS1 = value;
			break;
		case A_GPIO_EVERS2:
			t->EVERS2 = value;
			break;
		case A_GPIO_EVERT0:
			t->EVERT0 = value;
			break;
		case A_GPIO_EVERT1:
			t->EVERT1 = value;
			break;
		case A_GPIO_EVERT2:
			t->EVERT2 = value;
			break;
		case A_GPIO_EVER0:
			t->EVER0 = value;
			break;
		case A_GPIO_EVER1:
			t->EVER1 = value;
			break;
		case A_GPIO_EVER2:
			t->EVER2 = value;
			break;
		case A_GPIO_GFERC0:
			t->GFERC0 = value;
			break;
		case A_GPIO_GFERC1:
			t->GFERC1 = value;
			break;
		case A_GPIO_GFERC2:
			t->GFERC2 = value;
			break;
		case A_GPIO_GFERS0:
			t->GFERS0 = value;
			break;
		case A_GPIO_GFERS1:
			t->GFERS1 = value;
			break;
		case A_GPIO_GFERS2:
			t->GFERS2 = value;
			break;
		case A_GPIO_GFERT0:
			t->GFERT0 = value;
			break;
		case A_GPIO_GFERT1:
			t->GFERT1 = value;
			break;
		case A_GPIO_GFERT2:
			t->GFERT2 = value;
			break;
		case A_GPIO_GFER0:
			t->GFER0 = value;
			break;
		case A_GPIO_GFER1:
			t->GFER1 = value;
			break;
		case A_GPIO_GFER2:
			t->GFER2 = value;
			break;
		case A_GPIO_GPERC0:
			t->GPERC0 = value;
			break;
		case A_GPIO_GPERC1:
			t->GPERC1 = value;
			break;
		case A_GPIO_GPERC2:
			t->GPERC2 = value;
			break;
		case A_GPIO_GPERS0:
			t->GPERS0 = value;
			break;
		case A_GPIO_GPERS1:
			t->GPERS1 = value;
			break;
		case A_GPIO_GPERS2:
			t->GPERS2 = value;
			break;
		case A_GPIO_GPERT0:
			t->GPERT0 = value;
			break;
		case A_GPIO_GPERT1:
			t->GPERT1 = value;
			break;
		case A_GPIO_GPERT2:
			t->GPERT2 = value;
			break;
		case A_GPIO_GPER0:
			t->GPER0 = value;
			break;
		case A_GPIO_GPER1:
			t->GPER1 = value;
			break;
		case A_GPIO_GPER2:
			t->GPER2 = value;
			break;
		case A_GPIO_IERC0:
			t->IERC0 = value;
			break;
		case A_GPIO_IERC1:
			t->IERC1 = value;
			break;
		case A_GPIO_IERC2:
			t->IERC2 = value;
			break;
		case A_GPIO_IERS0:
			t->IERS0 = value;
			break;
		case A_GPIO_IERS1:
			t->IERS1 = value;
			break;
		case A_GPIO_IERS2:
			t->IERS2 = value;
			break;
		case A_GPIO_IERT0:
			t->IERT0 = value;
			break;
		case A_GPIO_IERT1:
			t->IERT1 = value;
			break;
		case A_GPIO_IERT2:
			t->IERT2 = value;
			break;
		case A_GPIO_IER0:
			t->IER0 = value;
			break;
		case A_GPIO_IER1:
			t->IER1 = value;
			break;
		case A_GPIO_IER2:
			t->IER2 = value;
			break;
		case A_GPIO_IFRC0:
			t->IFRC0 = value;
			break;
		case A_GPIO_IFRC1:
			t->IFRC1 = value;
			break;
		case A_GPIO_IFRC2:
			t->IFRC2 = value;
			break;
		case A_GPIO_IMR0C0:
			t->IMR0C0 = value;
			break;
		case A_GPIO_IMR0C1:
			t->IMR0C1 = value;
			break;
		case A_GPIO_IMR0C2:
			t->IMR0C2 = value;
			break;
		case A_GPIO_IMR0S0:
			t->IMR0S0 = value;
			break;
		case A_GPIO_IMR0S1:
			t->IMR0S1 = value;
			break;
		case A_GPIO_IMR0S2:
			t->IMR0S2 = value;
			break;
		case A_GPIO_IMR0T0:
			t->IMR0T0 = value;
			break;
		case A_GPIO_IMR0T1:
			t->IMR0T1 = value;
			break;
		case A_GPIO_IMR0T2:
			t->IMR0T2 = value;
			break;
		case A_GPIO_IMR1C0:
			t->IMR1C0 = value;
			break;
		case A_GPIO_IMR1C1:
			t->IMR1C1 = value;
			break;
		case A_GPIO_IMR1C2:
			t->IMR1C2 = value;
			break;
		case A_GPIO_IMR1S0:
			t->IMR1S0 = value;
			break;
		case A_GPIO_IMR1S1:
			t->IMR1S1 = value;
			break;
		case A_GPIO_IMR1S2:
			t->IMR1S2 = value;
			break;
		case A_GPIO_IMR1T0:
			t->IMR1T0 = value;
			break;
		case A_GPIO_IMR1T1:
			t->IMR1T1 = value;
			break;
		case A_GPIO_IMR1T2:
			t->IMR1T2 = value;
			break;
		case A_GPIO_IMR00:
			t->IMR00 = value;
			break;
		case A_GPIO_IMR01:
			t->IMR01 = value;
			break;
		case A_GPIO_IMR02:
			t->IMR02 = value;
			break;
		case A_GPIO_IMR10:
			t->IMR10 = value;
			break;
		case A_GPIO_IMR11:
			t->IMR11 = value;
			break;
		case A_GPIO_IMR12:
			t->IMR12 = value;
			break;
		case A_GPIO_LOCKC0:
			t->LOCKC0 = value;
			break;
		case A_GPIO_LOCKC1:
			t->LOCKC1 = value;
			break;
		case A_GPIO_LOCKC2:
			t->LOCKC2 = value;
			break;
		case A_GPIO_LOCKS0:
			t->LOCKS0 = value;
			break;
		case A_GPIO_LOCKS1:
			t->LOCKS1 = value;
			break;
		case A_GPIO_LOCKS2:
			t->LOCKS2 = value;
			break;
		case A_GPIO_LOCKT0:
			t->LOCKT0 = value;
			break;
		case A_GPIO_LOCKT1:
			t->LOCKT1 = value;
			break;
		case A_GPIO_LOCKT2:
			t->LOCKT2 = value;
			break;
		case A_GPIO_LOCK0:
			t->LOCK0 = value;
			break;
		case A_GPIO_LOCK1:
			t->LOCK1 = value;
			break;
		case A_GPIO_LOCK2:
			t->LOCK2 = value;
			break;
		case A_GPIO_ODCR0C0:
			t->ODCR0C0 = value;
			break;
		case A_GPIO_ODCR0C1:
			t->ODCR0C1 = value;
			break;
		case A_GPIO_ODCR0C2:
			t->ODCR0C2 = value;
			break;
		case A_GPIO_ODCR0S0:
			t->ODCR0S0 = value;
			break;
		case A_GPIO_ODCR0S1:
			t->ODCR0S1 = value;
			break;
		case A_GPIO_ODCR0S2:
			t->ODCR0S2 = value;
			break;
		case A_GPIO_ODCR0T0:
			t->ODCR0T0 = value;
			break;
		case A_GPIO_ODCR0T1:
			t->ODCR0T1 = value;
			break;
		case A_GPIO_ODCR0T2:
			t->ODCR0T2 = value;
			break;
		case A_GPIO_ODCR1C0:
			t->ODCR1C0 = value;
			break;
		case A_GPIO_ODCR1C1:
			t->ODCR1C1 = value;
			break;
		case A_GPIO_ODCR1C2:
			t->ODCR1C2 = value;
			break;
		case A_GPIO_ODCR1S0:
			t->ODCR1S0 = value;
			break;
		case A_GPIO_ODCR1S1:
			t->ODCR1S1 = value;
			break;
		case A_GPIO_ODCR1S2:
			t->ODCR1S2 = value;
			break;
		case A_GPIO_ODCR1T0:
			t->ODCR1T0 = value;
			break;
		case A_GPIO_ODCR1T1:
			t->ODCR1T1 = value;
			break;
		case A_GPIO_ODCR1T2:
			t->ODCR1T2 = value;
			break;
		case A_GPIO_ODCR00:
			t->ODCR00 = value;
			break;
		case A_GPIO_ODCR01:
			t->ODCR01 = value;
			break;
		case A_GPIO_ODCR02:
			t->ODCR02 = value;
			break;
		case A_GPIO_ODCR10:
			t->ODCR10 = value;
			break;
		case A_GPIO_ODCR11:
			t->ODCR11 = value;
			break;
		case A_GPIO_ODCR12:
			t->ODCR12 = value;
			break;
		case A_GPIO_ODERC0:
			t->ODERC0 = value;
			break;
		case A_GPIO_ODERC1:
			t->ODERC1 = value;
			break;
		case A_GPIO_ODERC2:
			t->ODERC2 = value;
			break;
		case A_GPIO_ODERS0:
			t->ODERS0 = value;
			break;
		case A_GPIO_ODERS1:
			t->ODERS1 = value;
			break;
		case A_GPIO_ODERS2:
			t->ODERS2 = value;
			break;
		case A_GPIO_ODERT0:
			t->ODERT0 = value;
			break;
		case A_GPIO_ODERT1:
			t->ODERT1 = value;
			break;
		case A_GPIO_ODERT2:
			t->ODERT2 = value;
			break;
		case A_GPIO_ODER0:
			t->ODER0 = value;
			break;
		case A_GPIO_ODER1:
			t->ODER1 = value;
			break;
		case A_GPIO_ODER2:
			t->ODER2 = value;
			break;
		case A_GPIO_ODMERC0:
			t->ODMERC0 = value;
			break;
		case A_GPIO_ODMERC1:
			t->ODMERC1 = value;
			break;
		case A_GPIO_ODMERC2:
			t->ODMERC2 = value;
			break;
		case A_GPIO_ODMERS0:
			t->ODMERS0 = value;
			break;
		case A_GPIO_ODMERS1:
			t->ODMERS1 = value;
			break;
		case A_GPIO_ODMERS2:
			t->ODMERS2 = value;
			break;
		case A_GPIO_ODMERT0:
			t->ODMERT0 = value;
			break;
		case A_GPIO_ODMERT1:
			t->ODMERT1 = value;
			break;
		case A_GPIO_ODMERT2:
			t->ODMERT2 = value;
			break;
		case A_GPIO_ODMER0:
			t->ODMER0 = value;
			break;
		case A_GPIO_ODMER1:
			t->ODMER1 = value;
			break;
		case A_GPIO_ODMER2:
			t->ODMER2 = value;
			break;
		case A_GPIO_OSRR0C0:
			t->OSRR0C0 = value;
			break;
		case A_GPIO_OSRR0C1:
			t->OSRR0C1 = value;
			break;
		case A_GPIO_OSRR0C2:
			t->OSRR0C2 = value;
			break;
		case A_GPIO_OSRR0S0:
			t->OSRR0S0 = value;
			break;
		case A_GPIO_OSRR0S1:
			t->OSRR0S1 = value;
			break;
		case A_GPIO_OSRR0S2:
			t->OSRR0S2 = value;
			break;
		case A_GPIO_OSRR0T0:
			t->OSRR0T0 = value;
			break;
		case A_GPIO_OSRR0T1:
			t->OSRR0T1 = value;
			break;
		case A_GPIO_OSRR0T2:
			t->OSRR0T2 = value;
			break;
		case A_GPIO_OSRR00:
			t->OSRR00 = value;
			break;
		case A_GPIO_OSRR01:
			t->OSRR01 = value;
			break;
		case A_GPIO_OSRR02:
			t->OSRR02 = value;
			break;
		case A_GPIO_OVRC0:
			t->OVRC0 = value;
			break;
		case A_GPIO_OVRC1:
			t->OVRC1 = value;
			break;
		case A_GPIO_OVRC2:
			t->OVRC2 = value;
			break;
		case A_GPIO_OVRS0:
			t->OVRS0 = value;
			break;
		case A_GPIO_OVRS1:
			t->OVRS1 = value;
			break;
		case A_GPIO_OVRS2:
			t->OVRS2 = value;
			break;
		case A_GPIO_OVRT0:
			t->OVRT0 = value;
			break;
		case A_GPIO_OVRT1:
			t->OVRT1 = value;
			break;
		case A_GPIO_OVRT2:
			t->OVRT2 = value;
			break;
		case A_GPIO_OVR0:
			t->OVR0 = value;
			break;
		case A_GPIO_OVR1:
			t->OVR1 = value;
			break;
		case A_GPIO_OVR2:
			t->OVR2 = value;
			break;
		case A_GPIO_PDERC0:
			t->PDERC0 = value;
			break;
		case A_GPIO_PDERC1:
			t->PDERC1 = value;
			break;
		case A_GPIO_PDERC2:
			t->PDERC2 = value;
			break;
		case A_GPIO_PDERS0:
			t->PDERS0 = value;
			break;
		case A_GPIO_PDERS1:
			t->PDERS1 = value;
			break;
		case A_GPIO_PDERS2:
			t->PDERS2 = value;
			break;
		case A_GPIO_PDERT0:
			t->PDERT0 = value;
			break;
		case A_GPIO_PDERT1:
			t->PDERT1 = value;
			break;
		case A_GPIO_PDERT2:
			t->PDERT2 = value;
			break;
		case A_GPIO_PDER0:
			t->PDER0 = value;
			break;
		case A_GPIO_PDER1:
			t->PDER1 = value;
			break;
		case A_GPIO_PDER2:
			t->PDER2 = value;
			break;
		case A_GPIO_PMR0C0:
			t->PMR0C0 = value;
			break;
		case A_GPIO_PMR0C1:
			t->PMR0C1 = value;
			break;
		case A_GPIO_PMR0C2:
			t->PMR0C2 = value;
			break;
		case A_GPIO_PMR0S0:
			t->PMR0S0 = value;
			break;
		case A_GPIO_PMR0S1:
			t->PMR0S1 = value;
			break;
		case A_GPIO_PMR0S2:
			t->PMR0S2 = value;
			break;
		case A_GPIO_PMR0T0:
			t->PMR0T0 = value;
			break;
		case A_GPIO_PMR0T1:
			t->PMR0T1 = value;
			break;
		case A_GPIO_PMR0T2:
			t->PMR0T2 = value;
			break;
		case A_GPIO_PMR1C0:
			t->PMR1C0 = value;
			break;
		case A_GPIO_PMR1C1:
			t->PMR1C1 = value;
			break;
		case A_GPIO_PMR1C2:
			t->PMR1C2 = value;
			break;
		case A_GPIO_PMR1S0:
			t->PMR1S0 = value;
			break;
		case A_GPIO_PMR1S1:
			t->PMR1S1 = value;
			break;
		case A_GPIO_PMR1S2:
			t->PMR1S2 = value;
			break;
		case A_GPIO_PMR1T0:
			t->PMR1T0 = value;
			break;
		case A_GPIO_PMR1T1:
			t->PMR1T1 = value;
			break;
		case A_GPIO_PMR1T2:
			t->PMR1T2 = value;
			break;
		case A_GPIO_PMR2C0:
			t->PMR2C0 = value;
			break;
		case A_GPIO_PMR2C1:
			t->PMR2C1 = value;
			break;
		case A_GPIO_PMR2C2:
			t->PMR2C2 = value;
			break;
		case A_GPIO_PMR2S0:
			t->PMR2S0 = value;
			break;
		case A_GPIO_PMR2S1:
			t->PMR2S1 = value;
			break;
		case A_GPIO_PMR2S2:
			t->PMR2S2 = value;
			break;
		case A_GPIO_PMR2T0:
			t->PMR2T0 = value;
			break;
		case A_GPIO_PMR2T1:
			t->PMR2T1 = value;
			break;
		case A_GPIO_PMR2T2:
			t->PMR2T2 = value;
			break;
		case A_GPIO_PMR00:
			t->PMR00 = value;
			break;
		case A_GPIO_PMR01:
			t->PMR01 = value;
			break;
		case A_GPIO_PMR02:
			t->PMR02 = value;
			break;
		case A_GPIO_PMR10:
			t->PMR10 = value;
			break;
		case A_GPIO_PMR11:
			t->PMR11 = value;
			break;
		case A_GPIO_PMR12:
			t->PMR12 = value;
			break;
		case A_GPIO_PMR20:
			t->PMR20 = value;
			break;
		case A_GPIO_PMR21:
			t->PMR21 = value;
			break;
		case A_GPIO_PMR22:
			t->PMR22 = value;
			break;
		case A_GPIO_PUERC0:
			t->PUERC0 = value;
			break;
		case A_GPIO_PUERC1:
			t->PUERC1 = value;
			break;
		case A_GPIO_PUERC2:
			t->PUERC2 = value;
			break;
		case A_GPIO_PUERS0:
			t->PUERS0 = value;
			break;
		case A_GPIO_PUERS1:
			t->PUERS1 = value;
			break;
		case A_GPIO_PUERS2:
			t->PUERS2 = value;
			break;
		case A_GPIO_PUERT0:
			t->PUERT0 = value;
			break;
		case A_GPIO_PUERT1:
			t->PUERT1 = value;
			break;
		case A_GPIO_PUERT2:
			t->PUERT2 = value;
			break;
		case A_GPIO_PUER0:
			t->PUER0 = value;
			break;
		case A_GPIO_PUER1:
			t->PUER1 = value;
			break;
		case A_GPIO_PUER2:
			t->PUER2 = value;
			break;
		case A_GPIO_STERC0:
			t->STERC0 = value;
			break;
		case A_GPIO_STERC1:
			t->STERC1 = value;
			break;
		case A_GPIO_STERC2:
			t->STERC2 = value;
			break;
		case A_GPIO_STERS0:
			t->STERS0 = value;
			break;
		case A_GPIO_STERS1:
			t->STERS1 = value;
			break;
		case A_GPIO_STERS2:
			t->STERS2 = value;
			break;
		case A_GPIO_STERT0:
			t->STERT0 = value;
			break;
		case A_GPIO_STERT1:
			t->STERT1 = value;
			break;
		case A_GPIO_STERT2:
			t->STERT2 = value;
			break;
		case A_GPIO_STER0:
			t->STER0 = value;
			break;
		case A_GPIO_STER1:
			t->STER1 = value;
			break;
		case A_GPIO_STER2:
			t->STER2 = value;
			break;
		case A_GPIO_UNLOCK0:
			t->UNLOCK0 = value;
			break;
		case A_GPIO_UNLOCK1:
			t->UNLOCK1 = value;
			break;
		case A_GPIO_UNLOCK2:
			t->UNLOCK2 = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM4LC4C GPIO write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsam4lc4c_gpio_ops = {
	.read = atsam4lc4c_gpio_read,
	.write = atsam4lc4c_gpio_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsam4lc4c_gpio_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAM4LC4CGPIO *t = ATSAM4LC4C_GPIO(obj);
	memory_region_init_io(&(t->iomem), obj, &atsam4lc4c_gpio_ops, t, "atsam4lc4c-gpio", ATSAM4LC4C_GPIO_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 12; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void atsam4lc4c_gpio_realize(DeviceState *dev, Error **errp) {
	return;

}

static void atsam4lc4c_gpio_reset_enter(Object *obj, ResetType type) {
	ATSAM4LC4CGPIO *t = ATSAM4LC4C_GPIO(obj);
	atsam4lc4c_gpio_register_reset(t);
}

static const VMStateDescription atsam4lc4c_gpio_vmstate = {
	.name = "atsam4lc4c-gpio",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(ASR0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ASR1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ASR2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(EVERC0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(EVERC1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(EVERC2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(EVERS0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(EVERS1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(EVERS2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(EVERT0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(EVERT1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(EVERT2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(EVER0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(EVER1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(EVER2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(GFERC0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(GFERC1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(GFERC2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(GFERS0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(GFERS1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(GFERS2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(GFERT0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(GFERT1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(GFERT2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(GFER0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(GFER1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(GFER2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(GPERC0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(GPERC1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(GPERC2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(GPERS0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(GPERS1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(GPERS2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(GPERT0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(GPERT1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(GPERT2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(GPER0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(GPER1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(GPER2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IERC0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IERC1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IERC2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IERS0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IERS1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IERS2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IERT0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IERT1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IERT2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IER0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IER1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IER2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IFRC0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IFRC1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IFRC2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IFR0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IFR1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IFR2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IMR0C0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IMR0C1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IMR0C2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IMR0S0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IMR0S1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IMR0S2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IMR0T0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IMR0T1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IMR0T2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IMR1C0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IMR1C1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IMR1C2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IMR1S0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IMR1S1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IMR1S2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IMR1T0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IMR1T1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IMR1T2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IMR00, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IMR01, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IMR02, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IMR10, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IMR11, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(IMR12, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(LOCKC0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(LOCKC1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(LOCKC2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(LOCKS0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(LOCKS1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(LOCKS2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(LOCKT0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(LOCKT1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(LOCKT2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(LOCK0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(LOCK1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(LOCK2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODCR0C0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODCR0C1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODCR0C2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODCR0S0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODCR0S1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODCR0S2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODCR0T0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODCR0T1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODCR0T2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODCR1C0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODCR1C1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODCR1C2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODCR1S0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODCR1S1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODCR1S2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODCR1T0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODCR1T1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODCR1T2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODCR00, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODCR01, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODCR02, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODCR10, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODCR11, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODCR12, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODERC0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODERC1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODERC2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODERS0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODERS1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODERS2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODERT0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODERT1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODERT2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODER0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODER1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODER2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODMERC0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODMERC1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODMERC2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODMERS0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODMERS1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODMERS2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODMERT0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODMERT1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODMERT2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODMER0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODMER1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(ODMER2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(OSRR0C0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(OSRR0C1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(OSRR0C2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(OSRR0S0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(OSRR0S1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(OSRR0S2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(OSRR0T0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(OSRR0T1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(OSRR0T2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(OSRR00, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(OSRR01, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(OSRR02, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(OVRC0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(OVRC1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(OVRC2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(OVRS0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(OVRS1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(OVRS2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(OVRT0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(OVRT1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(OVRT2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(OVR0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(OVR1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(OVR2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PARAMETER0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PARAMETER1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PARAMETER2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PDERC0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PDERC1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PDERC2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PDERS0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PDERS1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PDERS2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PDERT0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PDERT1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PDERT2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PDER0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PDER1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PDER2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PMR0C0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PMR0C1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PMR0C2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PMR0S0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PMR0S1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PMR0S2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PMR0T0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PMR0T1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PMR0T2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PMR1C0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PMR1C1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PMR1C2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PMR1S0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PMR1S1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PMR1S2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PMR1T0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PMR1T1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PMR1T2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PMR2C0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PMR2C1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PMR2C2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PMR2S0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PMR2S1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PMR2S2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PMR2T0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PMR2T1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PMR2T2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PMR00, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PMR01, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PMR02, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PMR10, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PMR11, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PMR12, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PMR20, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PMR21, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PMR22, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PUERC0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PUERC1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PUERC2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PUERS0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PUERS1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PUERS2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PUERT0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PUERT1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PUERT2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PUER0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PUER1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PUER2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PVR0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PVR1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(PVR2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(STERC0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(STERC1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(STERC2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(STERS0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(STERS1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(STERS2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(STERT0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(STERT1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(STERT2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(STER0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(STER1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(STER2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(UNLOCK0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(UNLOCK1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(UNLOCK2, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(VERSION0, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(VERSION1, ATSAM4LC4CGPIO),
		VMSTATE_UINT32(VERSION2, ATSAM4LC4CGPIO),

		VMSTATE_END_OF_LIST()
	}
};

static void atsam4lc4c_gpio_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsam4lc4c_gpio_vmstate;
	dc->realize = atsam4lc4c_gpio_realize;
	rc->phases.enter = atsam4lc4c_gpio_reset_enter;

}

static const TypeInfo atsam4lc4c_gpio_info = {
	.name = TYPE_ATSAM4LC4C_GPIO,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAM4LC4CGPIO),
	.instance_init = atsam4lc4c_gpio_init,
	.class_init = atsam4lc4c_gpio_class_init,
};

static void atsam4lc4c_gpio_register_types(void) {
	type_register_static(&atsam4lc4c_gpio_info);
}

type_init(atsam4lc4c_gpio_register_types);

#define TYPE_ATSAM4LC4C_SPI "atsam4lc4c-spi"
OBJECT_DECLARE_SIMPLE_TYPE(ATSAM4LC4CSPI, ATSAM4LC4C_SPI)

struct ATSAM4LC4CSPI {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t CR;
	uint32_t FEATURES;
	uint32_t IDR;
	uint32_t IER;
	uint32_t IMR;
	uint32_t MR;
	uint32_t RDR;
	uint32_t SR;
	uint32_t TDR;
	uint32_t VERSION;
	uint32_t WPCR;
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
#define ATSAM4LC4C_SPI_SIZE				0x100

REG32(SPI_CR, 0x0)
	FIELD(SPI_CR, SPIEN, 0, 1)
	FIELD(SPI_CR, SPIDIS, 1, 1)
	FIELD(SPI_CR, SWRST, 7, 1)
	FIELD(SPI_CR, FLUSHFIFO, 8, 1)
	FIELD(SPI_CR, LASTXFER, 24, 1)
REG32(SPI_FEATURES, 0xf8)
	FIELD(SPI_FEATURES, NCS, 0, 4)
	FIELD(SPI_FEATURES, PCONF, 4, 1)
	FIELD(SPI_FEATURES, PPNCONF, 5, 1)
	FIELD(SPI_FEATURES, PHCONF, 6, 1)
	FIELD(SPI_FEATURES, PHZNCONF, 7, 1)
	FIELD(SPI_FEATURES, LENCONF, 8, 1)
	FIELD(SPI_FEATURES, LENNCONF, 9, 7)
	FIELD(SPI_FEATURES, EXTDEC, 16, 1)
	FIELD(SPI_FEATURES, CSNAATIMPL, 17, 1)
	FIELD(SPI_FEATURES, BRPBHSB, 18, 1)
	FIELD(SPI_FEATURES, FIFORIMPL, 19, 1)
	FIELD(SPI_FEATURES, SWPIMPL, 20, 1)
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
REG32(SPI_MR, 0x4)
	FIELD(SPI_MR, MSTR, 0, 1)
	FIELD(SPI_MR, PS, 1, 1)
	FIELD(SPI_MR, PCSDEC, 2, 1)
	FIELD(SPI_MR, MODFDIS, 4, 1)
	FIELD(SPI_MR, WDRBT, 5, 1)
	FIELD(SPI_MR, RXFIFOEN, 6, 1)
	FIELD(SPI_MR, LLB, 7, 1)
	FIELD(SPI_MR, PCS, 16, 4)
	FIELD(SPI_MR, DLYBCS, 24, 8)
REG32(SPI_RDR, 0x8)
	FIELD(SPI_RDR, RD, 0, 16)
	FIELD(SPI_RDR, PCS, 16, 4)
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
REG32(SPI_TDR, 0xc)
	FIELD(SPI_TDR, TD, 0, 16)
	FIELD(SPI_TDR, PCS, 16, 4)
	FIELD(SPI_TDR, LASTXFER, 24, 1)
REG32(SPI_VERSION, 0xfc)
	FIELD(SPI_VERSION, VERSION, 0, 12)
	FIELD(SPI_VERSION, MFN, 16, 3)
REG32(SPI_WPCR, 0xe4)
	FIELD(SPI_WPCR, WPEN, 0, 1)
	FIELD(SPI_WPCR, WPKEY, 8, 24)
REG32(SPI_WPSR, 0xe8)
	FIELD(SPI_WPSR, WPVS, 0, 3)
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


static int atsam4lc4c_spi_can_receive(void *opaque) {
	return 1;
}

static void atsam4lc4c_spi_receive(void *opaque, const uint8_t *buf, int size) {
	ATSAM4LC4CSPI *t = ATSAM4LC4C_SPI(opaque);

	t->RDR = *buf;
	t->SR |= 0x1;


}

static gboolean atsam4lc4c_spi_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	ATSAM4LC4CSPI *t = ATSAM4LC4C_SPI(opaque);
	int ret;

	t->watch_tag = 0;
	t->SR &= (~(0x2));



	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->TDR), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, atsam4lc4c_spi_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->SR |= 0x2;



	return FALSE;
}

static void atsam4lc4c_spi_register_reset(ATSAM4LC4CSPI *t) {
	t->SR = 0xf0;
	t->VERSION = 0x211;
	t->SR |= 0x2;

}

static uint64_t atsam4lc4c_spi_read(void *opaque, hwaddr offset, unsigned size) {
	ATSAM4LC4CSPI *t = ATSAM4LC4C_SPI(opaque);
	uint64_t ret;

	switch (offset) {
		case A_SPI_FEATURES:
			ret = t->FEATURES;
			break;
		case A_SPI_IMR:
			ret = t->IMR;
			break;
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
		case A_SPI_VERSION:
			ret = t->VERSION;
			break;
		case A_SPI_WPCR:
			ret = t->WPCR;
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
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM4LC4C SPI read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void atsam4lc4c_spi_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	ATSAM4LC4CSPI *t = ATSAM4LC4C_SPI(opaque);


	switch (offset) {
		case A_SPI_CR ... A_SPI_CR + 3:
			value = value << ((offset - A_SPI_CR) << 3);
			offset = A_SPI_CR;
			break;
		case A_SPI_IDR ... A_SPI_IDR + 3:
			value = value << ((offset - A_SPI_IDR) << 3);
			offset = A_SPI_IDR;
			break;
		case A_SPI_IER ... A_SPI_IER + 3:
			value = value << ((offset - A_SPI_IER) << 3);
			offset = A_SPI_IER;
			break;
		case A_SPI_MR ... A_SPI_MR + 3:
			value = value << ((offset - A_SPI_MR) << 3);
			offset = A_SPI_MR;
			break;
		case A_SPI_TDR ... A_SPI_TDR + 3:
			value = value << ((offset - A_SPI_TDR) << 3);
			offset = A_SPI_TDR;
			break;
		case A_SPI_WPCR ... A_SPI_WPCR + 3:
			value = value << ((offset - A_SPI_WPCR) << 3);
			offset = A_SPI_WPCR;
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
		case A_SPI_IDR:
			t->IDR = value;
			break;
		case A_SPI_IER:
			t->IER = value;
			break;
		case A_SPI_MR:
			t->MR = value;
			break;
		case A_SPI_TDR:
			t->TDR = value;
			atsam4lc4c_spi_transmit(NULL, G_IO_OUT, t);
			break;
		case A_SPI_WPCR:
			t->WPCR = value;
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
			qemu_log_mask(LOG_GUEST_ERROR, "ATSAM4LC4C SPI write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps atsam4lc4c_spi_ops = {
	.read = atsam4lc4c_spi_read,
	.write = atsam4lc4c_spi_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void atsam4lc4c_spi_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	ATSAM4LC4CSPI *t = ATSAM4LC4C_SPI(obj);
	memory_region_init_io(&(t->iomem), obj, &atsam4lc4c_spi_ops, t, "atsam4lc4c-spi", ATSAM4LC4C_SPI_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void atsam4lc4c_spi_realize(DeviceState *dev, Error **errp) {
	ATSAM4LC4CSPI *t = ATSAM4LC4C_SPI(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), atsam4lc4c_spi_can_receive, atsam4lc4c_spi_receive,
		NULL, NULL, t, NULL, true);

}

static void atsam4lc4c_spi_reset_enter(Object *obj, ResetType type) {
	ATSAM4LC4CSPI *t = ATSAM4LC4C_SPI(obj);
	atsam4lc4c_spi_register_reset(t);
}

static const VMStateDescription atsam4lc4c_spi_vmstate = {
	.name = "atsam4lc4c-spi",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR, ATSAM4LC4CSPI),
		VMSTATE_UINT32(FEATURES, ATSAM4LC4CSPI),
		VMSTATE_UINT32(IDR, ATSAM4LC4CSPI),
		VMSTATE_UINT32(IER, ATSAM4LC4CSPI),
		VMSTATE_UINT32(IMR, ATSAM4LC4CSPI),
		VMSTATE_UINT32(MR, ATSAM4LC4CSPI),
		VMSTATE_UINT32(RDR, ATSAM4LC4CSPI),
		VMSTATE_UINT32(SR, ATSAM4LC4CSPI),
		VMSTATE_UINT32(TDR, ATSAM4LC4CSPI),
		VMSTATE_UINT32(VERSION, ATSAM4LC4CSPI),
		VMSTATE_UINT32(WPCR, ATSAM4LC4CSPI),
		VMSTATE_UINT32(WPSR, ATSAM4LC4CSPI),
		VMSTATE_UINT32(CSR0, ATSAM4LC4CSPI),
		VMSTATE_UINT32(CSR1, ATSAM4LC4CSPI),
		VMSTATE_UINT32(CSR2, ATSAM4LC4CSPI),
		VMSTATE_UINT32(CSR3, ATSAM4LC4CSPI),

		VMSTATE_END_OF_LIST()
	}
};

static Property atsam4lc4c_spi_properties[] = {
	DEFINE_PROP_CHR("chardev", ATSAM4LC4CSPI, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void atsam4lc4c_spi_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &atsam4lc4c_spi_vmstate;
	dc->realize = atsam4lc4c_spi_realize;
	rc->phases.enter = atsam4lc4c_spi_reset_enter;
	device_class_set_props(dc, atsam4lc4c_spi_properties);

}

static const TypeInfo atsam4lc4c_spi_info = {
	.name = TYPE_ATSAM4LC4C_SPI,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(ATSAM4LC4CSPI),
	.instance_init = atsam4lc4c_spi_init,
	.class_init = atsam4lc4c_spi_class_init,
};

static void atsam4lc4c_spi_register_types(void) {
	type_register_static(&atsam4lc4c_spi_info);
}

type_init(atsam4lc4c_spi_register_types);

#define TYPE_ATSAM4LC4C_MACHINE MACHINE_TYPE_NAME("atsam4lc4c")
OBJECT_DECLARE_TYPE(ATSAM4LC4CMachineState, ATSAM4LC4CMachineClass, ATSAM4LC4C_MACHINE)

struct ATSAM4LC4CMachineClass {
	MachineClass parent;
};

struct ATSAM4LC4CMachineState {
	MachineState parent;
	ARMv7MState armv7m;
};

static void atsam4lc4c_periph_init(MachineState *machine) {
	ATSAM4LC4CMachineState *sms = ATSAM4LC4C_MACHINE(machine);
	ATSAM4LC4CUSART *p0 = g_new(ATSAM4LC4CUSART, 1);
	object_initialize_child(OBJECT(sms), "USART0", p0, TYPE_ATSAM4LC4C_USART);
	qdev_prop_set_chr(DEVICE(p0), "chardev", qemu_chr_new("soc-usart0", "chardev:usart0", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p0), &error_fatal);
	p0->base = 0x40024000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p0), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 65));
	sysbus_mmio_map(SYS_BUS_DEVICE(p0), 0, 0x40024000);

	ATSAM4LC4CUSART *p1 = g_new(ATSAM4LC4CUSART, 1);
	object_initialize_child(OBJECT(sms), "USART1", p1, TYPE_ATSAM4LC4C_USART);
	qdev_prop_set_chr(DEVICE(p1), "chardev", qemu_chr_new("soc-usart1", "chardev:usart1", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p1), &error_fatal);
	p1->base = 0x40028000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p1), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 66));
	sysbus_mmio_map(SYS_BUS_DEVICE(p1), 0, 0x40028000);

	ATSAM4LC4CUSART *p2 = g_new(ATSAM4LC4CUSART, 1);
	object_initialize_child(OBJECT(sms), "USART2", p2, TYPE_ATSAM4LC4C_USART);
	qdev_prop_set_chr(DEVICE(p2), "chardev", qemu_chr_new("soc-usart2", "chardev:usart2", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p2), &error_fatal);
	p2->base = 0x4002c000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p2), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 67));
	sysbus_mmio_map(SYS_BUS_DEVICE(p2), 0, 0x4002c000);

	ATSAM4LC4CUSART *p3 = g_new(ATSAM4LC4CUSART, 1);
	object_initialize_child(OBJECT(sms), "USART3", p3, TYPE_ATSAM4LC4C_USART);
	qdev_prop_set_chr(DEVICE(p3), "chardev", qemu_chr_new("soc-usart3", "chardev:usart3", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p3), &error_fatal);
	p3->base = 0x40030000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p3), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 68));
	sysbus_mmio_map(SYS_BUS_DEVICE(p3), 0, 0x40030000);

	ATSAM4LC4CGPIO *p4 = g_new(ATSAM4LC4CGPIO, 1);
	object_initialize_child(OBJECT(sms), "GPIO", p4, TYPE_ATSAM4LC4C_GPIO);
	sysbus_realize(SYS_BUS_DEVICE(p4), &error_fatal);
	p4->base = 0x400e1000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p4), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 25));
	sysbus_connect_irq(SYS_BUS_DEVICE(p4), 1, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 26));
	sysbus_connect_irq(SYS_BUS_DEVICE(p4), 2, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 27));
	sysbus_connect_irq(SYS_BUS_DEVICE(p4), 3, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 28));
	sysbus_connect_irq(SYS_BUS_DEVICE(p4), 4, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 29));
	sysbus_connect_irq(SYS_BUS_DEVICE(p4), 5, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 30));
	sysbus_connect_irq(SYS_BUS_DEVICE(p4), 6, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 31));
	sysbus_connect_irq(SYS_BUS_DEVICE(p4), 7, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 32));
	sysbus_connect_irq(SYS_BUS_DEVICE(p4), 8, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 33));
	sysbus_connect_irq(SYS_BUS_DEVICE(p4), 9, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 34));
	sysbus_connect_irq(SYS_BUS_DEVICE(p4), 10, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 35));
	sysbus_connect_irq(SYS_BUS_DEVICE(p4), 11, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 36));
	sysbus_mmio_map(SYS_BUS_DEVICE(p4), 0, 0x400e1000);

	ATSAM4LC4CHCACHE *p5 = g_new(ATSAM4LC4CHCACHE, 1);
	object_initialize_child(OBJECT(sms), "HCACHE", p5, TYPE_ATSAM4LC4C_HCACHE);
	sysbus_realize(SYS_BUS_DEVICE(p5), &error_fatal);
	p5->base = 0x400a0400;
	sysbus_mmio_map(SYS_BUS_DEVICE(p5), 0, 0x400a0400);

	ATSAM4LC4CHFLASHC *p6 = g_new(ATSAM4LC4CHFLASHC, 1);
	object_initialize_child(OBJECT(sms), "HFLASHC", p6, TYPE_ATSAM4LC4C_HFLASHC);
	sysbus_realize(SYS_BUS_DEVICE(p6), &error_fatal);
	p6->base = 0x400a0000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p6), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 0));
	sysbus_mmio_map(SYS_BUS_DEVICE(p6), 0, 0x400a0000);

	ATSAM4LC4CPM *p7 = g_new(ATSAM4LC4CPM, 1);
	object_initialize_child(OBJECT(sms), "PM", p7, TYPE_ATSAM4LC4C_PM);
	sysbus_realize(SYS_BUS_DEVICE(p7), &error_fatal);
	p7->base = 0x400e0000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p7), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 22));
	sysbus_mmio_map(SYS_BUS_DEVICE(p7), 0, 0x400e0000);

	ATSAM4LC4CSCIF *p8 = g_new(ATSAM4LC4CSCIF, 1);
	object_initialize_child(OBJECT(sms), "SCIF", p8, TYPE_ATSAM4LC4C_SCIF);
	sysbus_realize(SYS_BUS_DEVICE(p8), &error_fatal);
	p8->base = 0x400e0800;
	sysbus_connect_irq(SYS_BUS_DEVICE(p8), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 23));
	sysbus_mmio_map(SYS_BUS_DEVICE(p8), 0, 0x400e0800);

	ATSAM4LC4CBSCIF *p9 = g_new(ATSAM4LC4CBSCIF, 1);
	object_initialize_child(OBJECT(sms), "BSCIF", p9, TYPE_ATSAM4LC4C_BSCIF);
	sysbus_realize(SYS_BUS_DEVICE(p9), &error_fatal);
	p9->base = 0x400f0400;
	sysbus_connect_irq(SYS_BUS_DEVICE(p9), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 38));
	sysbus_mmio_map(SYS_BUS_DEVICE(p9), 0, 0x400f0400);

	ATSAM4LC4CSPI *p10 = g_new(ATSAM4LC4CSPI, 1);
	object_initialize_child(OBJECT(sms), "SPI", p10, TYPE_ATSAM4LC4C_SPI);
	qdev_prop_set_chr(DEVICE(p10), "chardev", qemu_chr_new("soc-spi", "chardev:spi", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p10), &error_fatal);
	p10->base = 0x40008000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p10), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 54));
	sysbus_mmio_map(SYS_BUS_DEVICE(p10), 0, 0x40008000);


}

static void atsam4lc4c_common_init(MachineState *machine) {
	ATSAM4LC4CMachineState *sms = ATSAM4LC4C_MACHINE(machine);
	MemoryRegion *sysmem = get_system_memory();
	MemoryRegion *mem;
	Clock *cpuclk = clock_new(OBJECT(sms), "SYSCLK");
	clock_set_hz(cpuclk, 120000000);
	Clock *refclk = clock_new(OBJECT(sms), "REFCLK");
	clock_set_hz(refclk, 120000000);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_rom(mem, NULL, "main-flash", 0x100000, &error_fatal);
	memory_region_add_subregion(sysmem, 0x0, mem);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_ram(mem, NULL, "sram", 0x40000, &error_fatal);
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

	atsam4lc4c_periph_init(machine);

	armv7m_load_kernel(ARM_CPU(first_cpu), machine->kernel_filename, 0x0, 0x100000);
}

static void atsam4lc4c_class_init(ObjectClass *oc, void *data) {
	MachineClass *mc = MACHINE_CLASS(oc);
	mc->desc = "atsam4lc4c";
	mc->init = atsam4lc4c_common_init;
	mc->max_cpus = 1;
	mc->default_cpus = 1;
	mc->default_cpu_type = ARM_CPU_TYPE_NAME("cortex-m4");
}

static const TypeInfo atsam4lc4c_info = {
	.name = TYPE_ATSAM4LC4C_MACHINE,
	.parent = TYPE_MACHINE,
	.instance_size = sizeof(ATSAM4LC4CMachineState),
	.class_size = sizeof(ATSAM4LC4CMachineClass),
	.class_init = atsam4lc4c_class_init
};

static void atsam4lc4c_machine_init(void) {
	type_register_static(&atsam4lc4c_info);
}

type_init(atsam4lc4c_machine_init);
