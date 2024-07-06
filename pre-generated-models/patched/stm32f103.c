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


#define TYPE_STM32F1_DMA "stm32f1-dma"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F1DMA, STM32F1_DMA)

struct STM32F1DMA {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[7];

	/*registers*/
	uint32_t ISR;
	uint32_t IFCR;
	uint32_t CCR1;
	uint32_t CNDTR1;
	uint32_t CPAR1;
	uint32_t CMAR1;
	uint32_t CCR2;
	uint32_t CNDTR2;
	uint32_t CPAR2;
	uint32_t CMAR2;
	uint32_t CCR3;
	uint32_t CNDTR3;
	uint32_t CPAR3;
	uint32_t CMAR3;
	uint32_t CCR4;
	uint32_t CNDTR4;
	uint32_t CPAR4;
	uint32_t CMAR4;
	uint32_t CCR5;
	uint32_t CNDTR5;
	uint32_t CPAR5;
	uint32_t CMAR5;
	uint32_t CCR6;
	uint32_t CNDTR6;
	uint32_t CPAR6;
	uint32_t CMAR6;
	uint32_t CCR7;
	uint32_t CNDTR7;
	uint32_t CPAR7;
	uint32_t CMAR7;
	/* dma channel enable flags*/
	uint8_t channel_enabled[7];
	/* base */
	uint32_t base;

};
#define STM32F1_DMA_SIZE				0x90

REG32(DMA_ISR, 0x0)
	FIELD(DMA_ISR, GIF1, 0, 1)
	FIELD(DMA_ISR, TCIF1, 1, 1)
	FIELD(DMA_ISR, HTIF1, 2, 1)
	FIELD(DMA_ISR, TEIF1, 3, 1)
	FIELD(DMA_ISR, GIF2, 4, 1)
	FIELD(DMA_ISR, TCIF2, 5, 1)
	FIELD(DMA_ISR, HTIF2, 6, 1)
	FIELD(DMA_ISR, TEIF2, 7, 1)
	FIELD(DMA_ISR, GIF3, 8, 1)
	FIELD(DMA_ISR, TCIF3, 9, 1)
	FIELD(DMA_ISR, HTIF3, 10, 1)
	FIELD(DMA_ISR, TEIF3, 11, 1)
	FIELD(DMA_ISR, GIF4, 12, 1)
	FIELD(DMA_ISR, TCIF4, 13, 1)
	FIELD(DMA_ISR, HTIF4, 14, 1)
	FIELD(DMA_ISR, TEIF4, 15, 1)
	FIELD(DMA_ISR, GIF5, 16, 1)
	FIELD(DMA_ISR, TCIF5, 17, 1)
	FIELD(DMA_ISR, HTIF5, 18, 1)
	FIELD(DMA_ISR, TEIF5, 19, 1)
	FIELD(DMA_ISR, GIF6, 20, 1)
	FIELD(DMA_ISR, TCIF6, 21, 1)
	FIELD(DMA_ISR, HTIF6, 22, 1)
	FIELD(DMA_ISR, TEIF6, 23, 1)
	FIELD(DMA_ISR, GIF7, 24, 1)
	FIELD(DMA_ISR, TCIF7, 25, 1)
	FIELD(DMA_ISR, HTIF7, 26, 1)
	FIELD(DMA_ISR, TEIF7, 27, 1)
REG32(DMA_IFCR, 0x4)
	FIELD(DMA_IFCR, CGIF1, 0, 1)
	FIELD(DMA_IFCR, CGIF2, 4, 1)
	FIELD(DMA_IFCR, CGIF3, 8, 1)
	FIELD(DMA_IFCR, CGIF4, 12, 1)
	FIELD(DMA_IFCR, CGIF5, 16, 1)
	FIELD(DMA_IFCR, CGIF6, 20, 1)
	FIELD(DMA_IFCR, CGIF7, 24, 1)
	FIELD(DMA_IFCR, CTCIF1, 1, 1)
	FIELD(DMA_IFCR, CTCIF2, 5, 1)
	FIELD(DMA_IFCR, CTCIF3, 9, 1)
	FIELD(DMA_IFCR, CTCIF4, 13, 1)
	FIELD(DMA_IFCR, CTCIF5, 17, 1)
	FIELD(DMA_IFCR, CTCIF6, 21, 1)
	FIELD(DMA_IFCR, CTCIF7, 25, 1)
	FIELD(DMA_IFCR, CHTIF1, 2, 1)
	FIELD(DMA_IFCR, CHTIF2, 6, 1)
	FIELD(DMA_IFCR, CHTIF3, 10, 1)
	FIELD(DMA_IFCR, CHTIF4, 14, 1)
	FIELD(DMA_IFCR, CHTIF5, 18, 1)
	FIELD(DMA_IFCR, CHTIF6, 22, 1)
	FIELD(DMA_IFCR, CHTIF7, 26, 1)
	FIELD(DMA_IFCR, CTEIF1, 3, 1)
	FIELD(DMA_IFCR, CTEIF2, 7, 1)
	FIELD(DMA_IFCR, CTEIF3, 11, 1)
	FIELD(DMA_IFCR, CTEIF4, 15, 1)
	FIELD(DMA_IFCR, CTEIF5, 19, 1)
	FIELD(DMA_IFCR, CTEIF6, 23, 1)
	FIELD(DMA_IFCR, CTEIF7, 27, 1)
REG32(DMA_CCR1, 0x8)
	FIELD(DMA_CCR1, EN, 0, 1)
	FIELD(DMA_CCR1, TCIE, 1, 1)
	FIELD(DMA_CCR1, HTIE, 2, 1)
	FIELD(DMA_CCR1, TEIE, 3, 1)
	FIELD(DMA_CCR1, DIR, 4, 1)
	FIELD(DMA_CCR1, CIRC, 5, 1)
	FIELD(DMA_CCR1, PINC, 6, 1)
	FIELD(DMA_CCR1, MINC, 7, 1)
	FIELD(DMA_CCR1, PSIZE, 8, 2)
	FIELD(DMA_CCR1, MSIZE, 10, 2)
	FIELD(DMA_CCR1, PL, 12, 2)
	FIELD(DMA_CCR1, MEM2MEM, 14, 1)
REG32(DMA_CNDTR1, 0xc)
	FIELD(DMA_CNDTR1, NDT, 0, 16)
REG32(DMA_CPAR1, 0x10)
	FIELD(DMA_CPAR1, PA, 0, 32)
REG32(DMA_CMAR1, 0x14)
	FIELD(DMA_CMAR1, MA, 0, 32)
REG32(DMA_CCR2, 0x1c)
	FIELD(DMA_CCR2, EN, 0, 1)
	FIELD(DMA_CCR2, TCIE, 1, 1)
	FIELD(DMA_CCR2, HTIE, 2, 1)
	FIELD(DMA_CCR2, TEIE, 3, 1)
	FIELD(DMA_CCR2, DIR, 4, 1)
	FIELD(DMA_CCR2, CIRC, 5, 1)
	FIELD(DMA_CCR2, PINC, 6, 1)
	FIELD(DMA_CCR2, MINC, 7, 1)
	FIELD(DMA_CCR2, PSIZE, 8, 2)
	FIELD(DMA_CCR2, MSIZE, 10, 2)
	FIELD(DMA_CCR2, PL, 12, 2)
	FIELD(DMA_CCR2, MEM2MEM, 14, 1)
REG32(DMA_CNDTR2, 0x20)
	FIELD(DMA_CNDTR2, NDT, 0, 16)
REG32(DMA_CPAR2, 0x24)
	FIELD(DMA_CPAR2, PA, 0, 32)
REG32(DMA_CMAR2, 0x28)
	FIELD(DMA_CMAR2, MA, 0, 32)
REG32(DMA_CCR3, 0x30)
	FIELD(DMA_CCR3, EN, 0, 1)
	FIELD(DMA_CCR3, TCIE, 1, 1)
	FIELD(DMA_CCR3, HTIE, 2, 1)
	FIELD(DMA_CCR3, TEIE, 3, 1)
	FIELD(DMA_CCR3, DIR, 4, 1)
	FIELD(DMA_CCR3, CIRC, 5, 1)
	FIELD(DMA_CCR3, PINC, 6, 1)
	FIELD(DMA_CCR3, MINC, 7, 1)
	FIELD(DMA_CCR3, PSIZE, 8, 2)
	FIELD(DMA_CCR3, MSIZE, 10, 2)
	FIELD(DMA_CCR3, PL, 12, 2)
	FIELD(DMA_CCR3, MEM2MEM, 14, 1)
REG32(DMA_CNDTR3, 0x34)
	FIELD(DMA_CNDTR3, NDT, 0, 16)
REG32(DMA_CPAR3, 0x38)
	FIELD(DMA_CPAR3, PA, 0, 32)
REG32(DMA_CMAR3, 0x3c)
	FIELD(DMA_CMAR3, MA, 0, 32)
REG32(DMA_CCR4, 0x44)
	FIELD(DMA_CCR4, EN, 0, 1)
	FIELD(DMA_CCR4, TCIE, 1, 1)
	FIELD(DMA_CCR4, HTIE, 2, 1)
	FIELD(DMA_CCR4, TEIE, 3, 1)
	FIELD(DMA_CCR4, DIR, 4, 1)
	FIELD(DMA_CCR4, CIRC, 5, 1)
	FIELD(DMA_CCR4, PINC, 6, 1)
	FIELD(DMA_CCR4, MINC, 7, 1)
	FIELD(DMA_CCR4, PSIZE, 8, 2)
	FIELD(DMA_CCR4, MSIZE, 10, 2)
	FIELD(DMA_CCR4, PL, 12, 2)
	FIELD(DMA_CCR4, MEM2MEM, 14, 1)
REG32(DMA_CNDTR4, 0x48)
	FIELD(DMA_CNDTR4, NDT, 0, 16)
REG32(DMA_CPAR4, 0x4c)
	FIELD(DMA_CPAR4, PA, 0, 32)
REG32(DMA_CMAR4, 0x50)
	FIELD(DMA_CMAR4, MA, 0, 32)
REG32(DMA_CCR5, 0x58)
	FIELD(DMA_CCR5, EN, 0, 1)
	FIELD(DMA_CCR5, TCIE, 1, 1)
	FIELD(DMA_CCR5, HTIE, 2, 1)
	FIELD(DMA_CCR5, TEIE, 3, 1)
	FIELD(DMA_CCR5, DIR, 4, 1)
	FIELD(DMA_CCR5, CIRC, 5, 1)
	FIELD(DMA_CCR5, PINC, 6, 1)
	FIELD(DMA_CCR5, MINC, 7, 1)
	FIELD(DMA_CCR5, PSIZE, 8, 2)
	FIELD(DMA_CCR5, MSIZE, 10, 2)
	FIELD(DMA_CCR5, PL, 12, 2)
	FIELD(DMA_CCR5, MEM2MEM, 14, 1)
REG32(DMA_CNDTR5, 0x5c)
	FIELD(DMA_CNDTR5, NDT, 0, 16)
REG32(DMA_CPAR5, 0x60)
	FIELD(DMA_CPAR5, PA, 0, 32)
REG32(DMA_CMAR5, 0x64)
	FIELD(DMA_CMAR5, MA, 0, 32)
REG32(DMA_CCR6, 0x6c)
	FIELD(DMA_CCR6, EN, 0, 1)
	FIELD(DMA_CCR6, TCIE, 1, 1)
	FIELD(DMA_CCR6, HTIE, 2, 1)
	FIELD(DMA_CCR6, TEIE, 3, 1)
	FIELD(DMA_CCR6, DIR, 4, 1)
	FIELD(DMA_CCR6, CIRC, 5, 1)
	FIELD(DMA_CCR6, PINC, 6, 1)
	FIELD(DMA_CCR6, MINC, 7, 1)
	FIELD(DMA_CCR6, PSIZE, 8, 2)
	FIELD(DMA_CCR6, MSIZE, 10, 2)
	FIELD(DMA_CCR6, PL, 12, 2)
	FIELD(DMA_CCR6, MEM2MEM, 14, 1)
REG32(DMA_CNDTR6, 0x70)
	FIELD(DMA_CNDTR6, NDT, 0, 16)
REG32(DMA_CPAR6, 0x74)
	FIELD(DMA_CPAR6, PA, 0, 32)
REG32(DMA_CMAR6, 0x78)
	FIELD(DMA_CMAR6, MA, 0, 32)
REG32(DMA_CCR7, 0x80)
	FIELD(DMA_CCR7, EN, 0, 1)
	FIELD(DMA_CCR7, TCIE, 1, 1)
	FIELD(DMA_CCR7, HTIE, 2, 1)
	FIELD(DMA_CCR7, TEIE, 3, 1)
	FIELD(DMA_CCR7, DIR, 4, 1)
	FIELD(DMA_CCR7, CIRC, 5, 1)
	FIELD(DMA_CCR7, PINC, 6, 1)
	FIELD(DMA_CCR7, MINC, 7, 1)
	FIELD(DMA_CCR7, PSIZE, 8, 2)
	FIELD(DMA_CCR7, MSIZE, 10, 2)
	FIELD(DMA_CCR7, PL, 12, 2)
	FIELD(DMA_CCR7, MEM2MEM, 14, 1)
REG32(DMA_CNDTR7, 0x84)
	FIELD(DMA_CNDTR7, NDT, 0, 16)
REG32(DMA_CPAR7, 0x88)
	FIELD(DMA_CPAR7, PA, 0, 32)
REG32(DMA_CMAR7, 0x8c)
	FIELD(DMA_CMAR7, MA, 0, 32)


__attribute__((unused))
static void stm32f1_dma_update(STM32F1DMA *t, int channel_idx, int level) {
	qemu_set_irq(t->irq[channel_idx], level);
}

static void stm32f1_dma_receive(struct STM32F1DMA *t, uint32_t addr, uint8_t data) {
	int do_update = 0;
	int channel_idx = -1;
	if (((t->CCR1 & 0x1)) && (t->CPAR1 == addr || t->CMAR1 == addr)) {
		channel_idx = 0;
	}  else if (((t->CCR2 & 0x1)) && (t->CPAR2 == addr || t->CMAR2 == addr)) {
		channel_idx = 1;
	}  else if (((t->CCR3 & 0x1)) && (t->CPAR3 == addr || t->CMAR3 == addr)) {
		channel_idx = 2;
	}  else if (((t->CCR4 & 0x1)) && (t->CPAR4 == addr || t->CMAR4 == addr)) {
		channel_idx = 3;
	}  else if (((t->CCR5 & 0x1)) && (t->CPAR5 == addr || t->CMAR5 == addr)) {
		channel_idx = 4;
	}  else if (((t->CCR6 & 0x1)) && (t->CPAR6 == addr || t->CMAR6 == addr)) {
		channel_idx = 5;
	}  else if (((t->CCR7 & 0x1)) && (t->CPAR7 == addr || t->CMAR7 == addr)) {
		channel_idx = 6;
	} 
	switch (channel_idx) {

		case 0: {
			if (t->CNDTR1) {
				if ((((!(t->CCR1 & 0x800)) && (!(t->CCR1 & 0x10)) && (!(t->CCR1 & 0x100)) && (t->CCR1 & 0x4000)) || ((!(t->CCR1 & 0x200)) && (!(t->CCR1 & 0x10)) && (!(t->CCR1 & 0x400)) && (t->CCR1 & 0x4000)) || ((!(t->CCR1 & 0x10)) && (!(t->CCR1 & 0x100)) && (!(t->CCR1 & 0x400)) && (t->CCR1 & 0x4000)) || ((!(t->CCR1 & 0x800)) && (!(t->CCR1 & 0x200)) && (!(t->CCR1 & 0x10)) && (t->CCR1 & 0x4000)))) {
					cpu_physical_memory_write(t->CPAR1, &data, 1);
					t->CPAR1 += 1;
				} else {
					cpu_physical_memory_write(t->CMAR1, &data, 1);
					t->CMAR1 += 1;
				}
				t->CNDTR1 -= 1;
			}
			if (t->CNDTR1 == 0) {
				t->ISR &= (~(0x4));
				t->ISR |= 0x2;
				t->CCR1 &= (~(0x4));
				t->CCR1 |= 0x2;

				do_update = 1;
			}
			break;
		}

		case 1: {
			if (t->CNDTR2) {
				if ((((!(t->CCR2 & 0x200)) && (!(t->CCR2 & 0x10)) && (t->CCR2 & 0x4000) && (!(t->CCR2 & 0x800))) || ((!(t->CCR2 & 0x200)) && (!(t->CCR2 & 0x400)) && (!(t->CCR2 & 0x10)) && (t->CCR2 & 0x4000)) || ((!(t->CCR2 & 0x10)) && (!(t->CCR2 & 0x100)) && (t->CCR2 & 0x4000) && (!(t->CCR2 & 0x800))) || ((!(t->CCR2 & 0x400)) && (!(t->CCR2 & 0x10)) && (!(t->CCR2 & 0x100)) && (t->CCR2 & 0x4000)))) {
					cpu_physical_memory_write(t->CPAR2, &data, 1);
					t->CPAR2 += 1;
				} else {
					cpu_physical_memory_write(t->CMAR2, &data, 1);
					t->CMAR2 += 1;
				}
				t->CNDTR2 -= 1;
			}
			if (t->CNDTR2 == 0) {
				t->CCR2 |= 0x2;
				t->ISR |= 0x20;
				t->CCR2 &= (~(0x4));
				t->ISR &= (~(0x40));

				do_update = 1;
			}
			break;
		}

		case 2: {
			if (t->CNDTR3) {
				if ((((t->CCR3 & 0x4000) && (!(t->CCR3 & 0x200)) && (!(t->CCR3 & 0x800)) && (!(t->CCR3 & 0x10))) || ((!(t->CCR3 & 0x400)) && (!(t->CCR3 & 0x10)) && (!(t->CCR3 & 0x200)) && (t->CCR3 & 0x4000)) || ((!(t->CCR3 & 0x100)) && (t->CCR3 & 0x4000) && (!(t->CCR3 & 0x800)) && (!(t->CCR3 & 0x10))) || ((!(t->CCR3 & 0x100)) && (t->CCR3 & 0x4000) && (!(t->CCR3 & 0x400)) && (!(t->CCR3 & 0x10))))) {
					cpu_physical_memory_write(t->CPAR3, &data, 1);
					t->CPAR3 += 1;
				} else {
					cpu_physical_memory_write(t->CMAR3, &data, 1);
					t->CMAR3 += 1;
				}
				t->CNDTR3 -= 1;
			}
			if (t->CNDTR3 == 0) {
				t->CCR3 |= 0x2;
				t->ISR |= 0x200;
				t->ISR &= (~(0x400));
				t->CCR3 &= (~(0x4));

				do_update = 1;
			}
			break;
		}

		case 3: {
			if (t->CNDTR4) {
				if ((((!(t->CCR4 & 0x10)) && (t->CCR4 & 0x4000) && (!(t->CCR4 & 0x100)) && (!(t->CCR4 & 0x800))) || ((!(t->CCR4 & 0x10)) && (t->CCR4 & 0x4000) && (!(t->CCR4 & 0x400)) && (!(t->CCR4 & 0x100))) || ((!(t->CCR4 & 0x200)) && (!(t->CCR4 & 0x10)) && (t->CCR4 & 0x4000) && (!(t->CCR4 & 0x400))) || ((!(t->CCR4 & 0x200)) && (!(t->CCR4 & 0x10)) && (t->CCR4 & 0x4000) && (!(t->CCR4 & 0x800))))) {
					cpu_physical_memory_write(t->CPAR4, &data, 1);
					t->CPAR4 += 1;
				} else {
					cpu_physical_memory_write(t->CMAR4, &data, 1);
					t->CMAR4 += 1;
				}
				t->CNDTR4 -= 1;
			}
			if (t->CNDTR4 == 0) {
				t->ISR &= (~(0x4000));
				t->CCR4 &= (~(0x4));
				t->ISR |= 0x2000;
				t->CCR4 |= 0x2;

				do_update = 1;
			}
			break;
		}

		case 4: {
			if (t->CNDTR5) {
				if ((((!(t->CCR5 & 0x200)) && (t->CCR5 & 0x4000) && (!(t->CCR5 & 0x800)) && (!(t->CCR5 & 0x10))) || ((!(t->CCR5 & 0x200)) && (!(t->CCR5 & 0x400)) && (t->CCR5 & 0x4000) && (!(t->CCR5 & 0x10))) || ((t->CCR5 & 0x4000) && (!(t->CCR5 & 0x100)) && (!(t->CCR5 & 0x800)) && (!(t->CCR5 & 0x10))) || ((!(t->CCR5 & 0x400)) && (t->CCR5 & 0x4000) && (!(t->CCR5 & 0x100)) && (!(t->CCR5 & 0x10))))) {
					cpu_physical_memory_write(t->CPAR5, &data, 1);
					t->CPAR5 += 1;
				} else {
					cpu_physical_memory_write(t->CMAR5, &data, 1);
					t->CMAR5 += 1;
				}
				t->CNDTR5 -= 1;
			}
			if (t->CNDTR5 == 0) {
				t->ISR &= (~(0x40000));
				t->ISR |= 0x20000;
				t->CCR5 |= 0x2;
				t->CCR5 &= (~(0x4));

				do_update = 1;
			}
			break;
		}

		case 5: {
			if (t->CNDTR6) {
				if ((((!(t->CCR6 & 0x10)) && (!(t->CCR6 & 0x200)) && (t->CCR6 & 0x4000) && (!(t->CCR6 & 0x400))) || ((!(t->CCR6 & 0x10)) && (!(t->CCR6 & 0x200)) && (!(t->CCR6 & 0x800)) && (t->CCR6 & 0x4000)) || ((!(t->CCR6 & 0x10)) && (t->CCR6 & 0x4000) && (!(t->CCR6 & 0x400)) && (!(t->CCR6 & 0x100))) || ((!(t->CCR6 & 0x10)) && (!(t->CCR6 & 0x800)) && (t->CCR6 & 0x4000) && (!(t->CCR6 & 0x100))))) {
					cpu_physical_memory_write(t->CPAR6, &data, 1);
					t->CPAR6 += 1;
				} else {
					cpu_physical_memory_write(t->CMAR6, &data, 1);
					t->CMAR6 += 1;
				}
				t->CNDTR6 -= 1;
			}
			if (t->CNDTR6 == 0) {
				t->ISR &= (~(0x400000));
				t->CCR6 &= (~(0x4));
				t->CCR6 |= 0x2;
				t->ISR |= 0x200000;

				do_update = 1;
			}
			break;
		}

		case 6: {
			if (t->CNDTR7) {
				if ((((!(t->CCR7 & 0x200)) && (!(t->CCR7 & 0x400)) && (!(t->CCR7 & 0x10)) && (t->CCR7 & 0x4000)) || ((!(t->CCR7 & 0x800)) && (!(t->CCR7 & 0x200)) && (!(t->CCR7 & 0x10)) && (t->CCR7 & 0x4000)) || ((!(t->CCR7 & 0x400)) && (!(t->CCR7 & 0x10)) && (t->CCR7 & 0x4000) && (!(t->CCR7 & 0x100))) || ((!(t->CCR7 & 0x800)) && (!(t->CCR7 & 0x10)) && (t->CCR7 & 0x4000) && (!(t->CCR7 & 0x100))))) {
					cpu_physical_memory_write(t->CPAR7, &data, 1);
					t->CPAR7 += 1;
				} else {
					cpu_physical_memory_write(t->CMAR7, &data, 1);
					t->CMAR7 += 1;
				}
				t->CNDTR7 -= 1;
			}
			if (t->CNDTR7 == 0) {
				t->ISR |= 0x2000000;
				t->CCR7 &= (~(0x4));
				t->CCR7 |= 0x2;
				t->ISR &= (~(0x4000000));

				do_update = 1;
			}
			break;
		}

		default: break;
	}
	if (do_update) {
		stm32f1_dma_update(t, channel_idx, 1);
		stm32f1_dma_update(t, channel_idx, 0);
	}
}

static void stm32f1_dma_transmit(struct STM32F1DMA *t, int channel_idx) {
	uint8_t data;
	switch (channel_idx) {

		case 0: {
			if (!t->channel_enabled[0]) {
				break;
			}
			if (t->CPAR1 < 0x40000000 && t->CMAR1 < 0x40000000) {
				for (int i = 0; i < t->CNDTR1; ++i) {
					if ((((!(t->CCR1 & 0x800)) && (!(t->CCR1 & 0x10)) && (!(t->CCR1 & 0x100)) && (t->CCR1 & 0x4000)) || ((!(t->CCR1 & 0x200)) && (!(t->CCR1 & 0x10)) && (!(t->CCR1 & 0x400)) && (t->CCR1 & 0x4000)) || ((!(t->CCR1 & 0x10)) && (!(t->CCR1 & 0x100)) && (!(t->CCR1 & 0x400)) && (t->CCR1 & 0x4000)) || ((!(t->CCR1 & 0x800)) && (!(t->CCR1 & 0x200)) && (!(t->CCR1 & 0x10)) && (t->CCR1 & 0x4000)))) {
						cpu_physical_memory_read(t->CPAR1, &data, 1);
						cpu_physical_memory_write(t->CMAR1, &data, 1);
					} else {
						cpu_physical_memory_read(t->CMAR1, &data, 1);
						cpu_physical_memory_write(t->CPAR1, &data, 1);
					}
						t->CPAR1 += 1;
						t->CMAR1 += 1;
				}
			} else {
				for (int i = 0; i < t->CNDTR1; ++i) {
					if ((((!(t->CCR1 & 0x800)) && (!(t->CCR1 & 0x10)) && (!(t->CCR1 & 0x100)) && (t->CCR1 & 0x4000)) || ((!(t->CCR1 & 0x200)) && (!(t->CCR1 & 0x10)) && (!(t->CCR1 & 0x400)) && (t->CCR1 & 0x4000)) || ((!(t->CCR1 & 0x10)) && (!(t->CCR1 & 0x100)) && (!(t->CCR1 & 0x400)) && (t->CCR1 & 0x4000)) || ((!(t->CCR1 & 0x800)) && (!(t->CCR1 & 0x200)) && (!(t->CCR1 & 0x10)) && (t->CCR1 & 0x4000)))) {
						cpu_physical_memory_read(t->CPAR1, &data, 1);
						cpu_physical_memory_write(t->CMAR1, &data, 1);
						t->CPAR1 += 1;
					} else {
						cpu_physical_memory_read(t->CMAR1, &data, 1);
						cpu_physical_memory_write(t->CPAR1, &data, 1);
						t->CMAR1 += 1;
					}
				}
			}
			t->ISR &= (~(0x4));
			t->ISR |= 0x2;
			t->CCR1 &= (~(0x4));
			t->CCR1 |= 0x2;

			break;
		}

		case 1: {
			if (!t->channel_enabled[1]) {
				break;
			}
			if (t->CPAR2 < 0x40000000 && t->CMAR2 < 0x40000000) {
				for (int i = 0; i < t->CNDTR2; ++i) {
					if ((((!(t->CCR2 & 0x200)) && (!(t->CCR2 & 0x10)) && (t->CCR2 & 0x4000) && (!(t->CCR2 & 0x800))) || ((!(t->CCR2 & 0x200)) && (!(t->CCR2 & 0x400)) && (!(t->CCR2 & 0x10)) && (t->CCR2 & 0x4000)) || ((!(t->CCR2 & 0x10)) && (!(t->CCR2 & 0x100)) && (t->CCR2 & 0x4000) && (!(t->CCR2 & 0x800))) || ((!(t->CCR2 & 0x400)) && (!(t->CCR2 & 0x10)) && (!(t->CCR2 & 0x100)) && (t->CCR2 & 0x4000)))) {
						cpu_physical_memory_read(t->CPAR2, &data, 1);
						cpu_physical_memory_write(t->CMAR2, &data, 1);
					} else {
						cpu_physical_memory_read(t->CMAR2, &data, 1);
						cpu_physical_memory_write(t->CPAR2, &data, 1);
					}
						t->CPAR2 += 1;
						t->CMAR2 += 1;
				}
			} else {
				for (int i = 0; i < t->CNDTR2; ++i) {
					if ((((!(t->CCR2 & 0x200)) && (!(t->CCR2 & 0x10)) && (t->CCR2 & 0x4000) && (!(t->CCR2 & 0x800))) || ((!(t->CCR2 & 0x200)) && (!(t->CCR2 & 0x400)) && (!(t->CCR2 & 0x10)) && (t->CCR2 & 0x4000)) || ((!(t->CCR2 & 0x10)) && (!(t->CCR2 & 0x100)) && (t->CCR2 & 0x4000) && (!(t->CCR2 & 0x800))) || ((!(t->CCR2 & 0x400)) && (!(t->CCR2 & 0x10)) && (!(t->CCR2 & 0x100)) && (t->CCR2 & 0x4000)))) {
						cpu_physical_memory_read(t->CPAR2, &data, 1);
						cpu_physical_memory_write(t->CMAR2, &data, 1);
						t->CPAR2 += 1;
					} else {
						cpu_physical_memory_read(t->CMAR2, &data, 1);
						cpu_physical_memory_write(t->CPAR2, &data, 1);
						t->CMAR2 += 1;
					}
				}
			}
			t->CCR2 |= 0x2;
			t->ISR |= 0x20;
			t->CCR2 &= (~(0x4));
			t->ISR &= (~(0x40));

			break;
		}

		case 2: {
			if (!t->channel_enabled[2]) {
				break;
			}
			if (t->CPAR3 < 0x40000000 && t->CMAR3 < 0x40000000) {
				for (int i = 0; i < t->CNDTR3; ++i) {
					if ((((t->CCR3 & 0x4000) && (!(t->CCR3 & 0x200)) && (!(t->CCR3 & 0x800)) && (!(t->CCR3 & 0x10))) || ((!(t->CCR3 & 0x400)) && (!(t->CCR3 & 0x10)) && (!(t->CCR3 & 0x200)) && (t->CCR3 & 0x4000)) || ((!(t->CCR3 & 0x100)) && (t->CCR3 & 0x4000) && (!(t->CCR3 & 0x800)) && (!(t->CCR3 & 0x10))) || ((!(t->CCR3 & 0x100)) && (t->CCR3 & 0x4000) && (!(t->CCR3 & 0x400)) && (!(t->CCR3 & 0x10))))) {
						cpu_physical_memory_read(t->CPAR3, &data, 1);
						cpu_physical_memory_write(t->CMAR3, &data, 1);
					} else {
						cpu_physical_memory_read(t->CMAR3, &data, 1);
						cpu_physical_memory_write(t->CPAR3, &data, 1);
					}
						t->CPAR3 += 1;
						t->CMAR3 += 1;
				}
			} else {
				for (int i = 0; i < t->CNDTR3; ++i) {
					if ((((t->CCR3 & 0x4000) && (!(t->CCR3 & 0x200)) && (!(t->CCR3 & 0x800)) && (!(t->CCR3 & 0x10))) || ((!(t->CCR3 & 0x400)) && (!(t->CCR3 & 0x10)) && (!(t->CCR3 & 0x200)) && (t->CCR3 & 0x4000)) || ((!(t->CCR3 & 0x100)) && (t->CCR3 & 0x4000) && (!(t->CCR3 & 0x800)) && (!(t->CCR3 & 0x10))) || ((!(t->CCR3 & 0x100)) && (t->CCR3 & 0x4000) && (!(t->CCR3 & 0x400)) && (!(t->CCR3 & 0x10))))) {
						cpu_physical_memory_read(t->CPAR3, &data, 1);
						cpu_physical_memory_write(t->CMAR3, &data, 1);
						t->CPAR3 += 1;
					} else {
						cpu_physical_memory_read(t->CMAR3, &data, 1);
						cpu_physical_memory_write(t->CPAR3, &data, 1);
						t->CMAR3 += 1;
					}
				}
			}
			t->CCR3 |= 0x2;
			t->ISR |= 0x200;
			t->ISR &= (~(0x400));
			t->CCR3 &= (~(0x4));

			break;
		}

		case 3: {
			if (!t->channel_enabled[3]) {
				break;
			}
			if (t->CPAR4 < 0x40000000 && t->CMAR4 < 0x40000000) {
				for (int i = 0; i < t->CNDTR4; ++i) {
					if ((((!(t->CCR4 & 0x10)) && (t->CCR4 & 0x4000) && (!(t->CCR4 & 0x100)) && (!(t->CCR4 & 0x800))) || ((!(t->CCR4 & 0x10)) && (t->CCR4 & 0x4000) && (!(t->CCR4 & 0x400)) && (!(t->CCR4 & 0x100))) || ((!(t->CCR4 & 0x200)) && (!(t->CCR4 & 0x10)) && (t->CCR4 & 0x4000) && (!(t->CCR4 & 0x400))) || ((!(t->CCR4 & 0x200)) && (!(t->CCR4 & 0x10)) && (t->CCR4 & 0x4000) && (!(t->CCR4 & 0x800))))) {
						cpu_physical_memory_read(t->CPAR4, &data, 1);
						cpu_physical_memory_write(t->CMAR4, &data, 1);
					} else {
						cpu_physical_memory_read(t->CMAR4, &data, 1);
						cpu_physical_memory_write(t->CPAR4, &data, 1);
					}
						t->CPAR4 += 1;
						t->CMAR4 += 1;
				}
			} else {
				for (int i = 0; i < t->CNDTR4; ++i) {
					if ((((!(t->CCR4 & 0x10)) && (t->CCR4 & 0x4000) && (!(t->CCR4 & 0x100)) && (!(t->CCR4 & 0x800))) || ((!(t->CCR4 & 0x10)) && (t->CCR4 & 0x4000) && (!(t->CCR4 & 0x400)) && (!(t->CCR4 & 0x100))) || ((!(t->CCR4 & 0x200)) && (!(t->CCR4 & 0x10)) && (t->CCR4 & 0x4000) && (!(t->CCR4 & 0x400))) || ((!(t->CCR4 & 0x200)) && (!(t->CCR4 & 0x10)) && (t->CCR4 & 0x4000) && (!(t->CCR4 & 0x800))))) {
						cpu_physical_memory_read(t->CPAR4, &data, 1);
						cpu_physical_memory_write(t->CMAR4, &data, 1);
						t->CPAR4 += 1;
					} else {
						cpu_physical_memory_read(t->CMAR4, &data, 1);
						cpu_physical_memory_write(t->CPAR4, &data, 1);
						t->CMAR4 += 1;
					}
				}
			}
			t->ISR &= (~(0x4000));
			t->CCR4 &= (~(0x4));
			t->ISR |= 0x2000;
			t->CCR4 |= 0x2;

			break;
		}

		case 4: {
			if (!t->channel_enabled[4]) {
				break;
			}
			if (t->CPAR5 < 0x40000000 && t->CMAR5 < 0x40000000) {
				for (int i = 0; i < t->CNDTR5; ++i) {
					if ((((!(t->CCR5 & 0x200)) && (t->CCR5 & 0x4000) && (!(t->CCR5 & 0x800)) && (!(t->CCR5 & 0x10))) || ((!(t->CCR5 & 0x200)) && (!(t->CCR5 & 0x400)) && (t->CCR5 & 0x4000) && (!(t->CCR5 & 0x10))) || ((t->CCR5 & 0x4000) && (!(t->CCR5 & 0x100)) && (!(t->CCR5 & 0x800)) && (!(t->CCR5 & 0x10))) || ((!(t->CCR5 & 0x400)) && (t->CCR5 & 0x4000) && (!(t->CCR5 & 0x100)) && (!(t->CCR5 & 0x10))))) {
						cpu_physical_memory_read(t->CPAR5, &data, 1);
						cpu_physical_memory_write(t->CMAR5, &data, 1);
					} else {
						cpu_physical_memory_read(t->CMAR5, &data, 1);
						cpu_physical_memory_write(t->CPAR5, &data, 1);
					}
						t->CPAR5 += 1;
						t->CMAR5 += 1;
				}
			} else {
				for (int i = 0; i < t->CNDTR5; ++i) {
					if ((((!(t->CCR5 & 0x200)) && (t->CCR5 & 0x4000) && (!(t->CCR5 & 0x800)) && (!(t->CCR5 & 0x10))) || ((!(t->CCR5 & 0x200)) && (!(t->CCR5 & 0x400)) && (t->CCR5 & 0x4000) && (!(t->CCR5 & 0x10))) || ((t->CCR5 & 0x4000) && (!(t->CCR5 & 0x100)) && (!(t->CCR5 & 0x800)) && (!(t->CCR5 & 0x10))) || ((!(t->CCR5 & 0x400)) && (t->CCR5 & 0x4000) && (!(t->CCR5 & 0x100)) && (!(t->CCR5 & 0x10))))) {
						cpu_physical_memory_read(t->CPAR5, &data, 1);
						cpu_physical_memory_write(t->CMAR5, &data, 1);
						t->CPAR5 += 1;
					} else {
						cpu_physical_memory_read(t->CMAR5, &data, 1);
						cpu_physical_memory_write(t->CPAR5, &data, 1);
						t->CMAR5 += 1;
					}
				}
			}
			t->ISR &= (~(0x40000));
			t->ISR |= 0x20000;
			t->CCR5 |= 0x2;
			t->CCR5 &= (~(0x4));

			break;
		}

		case 5: {
			if (!t->channel_enabled[5]) {
				break;
			}
			if (t->CPAR6 < 0x40000000 && t->CMAR6 < 0x40000000) {
				for (int i = 0; i < t->CNDTR6; ++i) {
					if ((((!(t->CCR6 & 0x10)) && (!(t->CCR6 & 0x200)) && (t->CCR6 & 0x4000) && (!(t->CCR6 & 0x400))) || ((!(t->CCR6 & 0x10)) && (!(t->CCR6 & 0x200)) && (!(t->CCR6 & 0x800)) && (t->CCR6 & 0x4000)) || ((!(t->CCR6 & 0x10)) && (t->CCR6 & 0x4000) && (!(t->CCR6 & 0x400)) && (!(t->CCR6 & 0x100))) || ((!(t->CCR6 & 0x10)) && (!(t->CCR6 & 0x800)) && (t->CCR6 & 0x4000) && (!(t->CCR6 & 0x100))))) {
						cpu_physical_memory_read(t->CPAR6, &data, 1);
						cpu_physical_memory_write(t->CMAR6, &data, 1);
					} else {
						cpu_physical_memory_read(t->CMAR6, &data, 1);
						cpu_physical_memory_write(t->CPAR6, &data, 1);
					}
						t->CPAR6 += 1;
						t->CMAR6 += 1;
				}
			} else {
				for (int i = 0; i < t->CNDTR6; ++i) {
					if ((((!(t->CCR6 & 0x10)) && (!(t->CCR6 & 0x200)) && (t->CCR6 & 0x4000) && (!(t->CCR6 & 0x400))) || ((!(t->CCR6 & 0x10)) && (!(t->CCR6 & 0x200)) && (!(t->CCR6 & 0x800)) && (t->CCR6 & 0x4000)) || ((!(t->CCR6 & 0x10)) && (t->CCR6 & 0x4000) && (!(t->CCR6 & 0x400)) && (!(t->CCR6 & 0x100))) || ((!(t->CCR6 & 0x10)) && (!(t->CCR6 & 0x800)) && (t->CCR6 & 0x4000) && (!(t->CCR6 & 0x100))))) {
						cpu_physical_memory_read(t->CPAR6, &data, 1);
						cpu_physical_memory_write(t->CMAR6, &data, 1);
						t->CPAR6 += 1;
					} else {
						cpu_physical_memory_read(t->CMAR6, &data, 1);
						cpu_physical_memory_write(t->CPAR6, &data, 1);
						t->CMAR6 += 1;
					}
				}
			}
			t->ISR &= (~(0x400000));
			t->CCR6 &= (~(0x4));
			t->CCR6 |= 0x2;
			t->ISR |= 0x200000;

			break;
		}

		case 6: {
			if (!t->channel_enabled[6]) {
				break;
			}
			if (t->CPAR7 < 0x40000000 && t->CMAR7 < 0x40000000) {
				for (int i = 0; i < t->CNDTR7; ++i) {
					if ((((!(t->CCR7 & 0x200)) && (!(t->CCR7 & 0x400)) && (!(t->CCR7 & 0x10)) && (t->CCR7 & 0x4000)) || ((!(t->CCR7 & 0x800)) && (!(t->CCR7 & 0x200)) && (!(t->CCR7 & 0x10)) && (t->CCR7 & 0x4000)) || ((!(t->CCR7 & 0x400)) && (!(t->CCR7 & 0x10)) && (t->CCR7 & 0x4000) && (!(t->CCR7 & 0x100))) || ((!(t->CCR7 & 0x800)) && (!(t->CCR7 & 0x10)) && (t->CCR7 & 0x4000) && (!(t->CCR7 & 0x100))))) {
						cpu_physical_memory_read(t->CPAR7, &data, 1);
						cpu_physical_memory_write(t->CMAR7, &data, 1);
					} else {
						cpu_physical_memory_read(t->CMAR7, &data, 1);
						cpu_physical_memory_write(t->CPAR7, &data, 1);
					}
						t->CPAR7 += 1;
						t->CMAR7 += 1;
				}
			} else {
				for (int i = 0; i < t->CNDTR7; ++i) {
					if ((((!(t->CCR7 & 0x200)) && (!(t->CCR7 & 0x400)) && (!(t->CCR7 & 0x10)) && (t->CCR7 & 0x4000)) || ((!(t->CCR7 & 0x800)) && (!(t->CCR7 & 0x200)) && (!(t->CCR7 & 0x10)) && (t->CCR7 & 0x4000)) || ((!(t->CCR7 & 0x400)) && (!(t->CCR7 & 0x10)) && (t->CCR7 & 0x4000) && (!(t->CCR7 & 0x100))) || ((!(t->CCR7 & 0x800)) && (!(t->CCR7 & 0x10)) && (t->CCR7 & 0x4000) && (!(t->CCR7 & 0x100))))) {
						cpu_physical_memory_read(t->CPAR7, &data, 1);
						cpu_physical_memory_write(t->CMAR7, &data, 1);
						t->CPAR7 += 1;
					} else {
						cpu_physical_memory_read(t->CMAR7, &data, 1);
						cpu_physical_memory_write(t->CPAR7, &data, 1);
						t->CMAR7 += 1;
					}
				}
			}
			t->ISR |= 0x2000000;
			t->CCR7 &= (~(0x4));
			t->CCR7 |= 0x2;
			t->ISR &= (~(0x4000000));

			break;
		}

		default: break;
	}
	stm32f1_dma_update(t, channel_idx, 1);
	stm32f1_dma_update(t, channel_idx, 0);
}

static void stm32f1_dma_register_reset(STM32F1DMA *t) {
	t->ISR = 0x0;
	t->IFCR = 0x0;
	t->CCR1 = 0x0;
	t->CNDTR1 = 0x0;
	t->CPAR1 = 0x0;
	t->CMAR1 = 0x0;
	t->CCR2 = 0x0;
	t->CNDTR2 = 0x0;
	t->CPAR2 = 0x0;
	t->CMAR2 = 0x0;
	t->CCR3 = 0x0;
	t->CNDTR3 = 0x0;
	t->CPAR3 = 0x0;
	t->CMAR3 = 0x0;
	t->CCR4 = 0x0;
	t->CNDTR4 = 0x0;
	t->CPAR4 = 0x0;
	t->CMAR4 = 0x0;
	t->CCR5 = 0x0;
	t->CNDTR5 = 0x0;
	t->CPAR5 = 0x0;
	t->CMAR5 = 0x0;
	t->CCR6 = 0x0;
	t->CNDTR6 = 0x0;
	t->CPAR6 = 0x0;
	t->CMAR6 = 0x0;
	t->CCR7 = 0x0;
	t->CNDTR7 = 0x0;
	t->CPAR7 = 0x0;
	t->CMAR7 = 0x0;

}

static uint64_t stm32f1_dma_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F1DMA *t = STM32F1_DMA(opaque);
	uint64_t ret;

	switch (offset) {
		case A_DMA_ISR:
			ret = t->ISR;
			break;
		case A_DMA_CCR1:
			ret = t->CCR1;
			break;
		case A_DMA_CNDTR1:
			ret = t->CNDTR1;
			break;
		case A_DMA_CPAR1:
			ret = t->CPAR1;
			break;
		case A_DMA_CMAR1:
			ret = t->CMAR1;
			break;
		case A_DMA_CCR2:
			ret = t->CCR2;
			break;
		case A_DMA_CNDTR2:
			ret = t->CNDTR2;
			break;
		case A_DMA_CPAR2:
			ret = t->CPAR2;
			break;
		case A_DMA_CMAR2:
			ret = t->CMAR2;
			break;
		case A_DMA_CCR3:
			ret = t->CCR3;
			break;
		case A_DMA_CNDTR3:
			ret = t->CNDTR3;
			break;
		case A_DMA_CPAR3:
			ret = t->CPAR3;
			break;
		case A_DMA_CMAR3:
			ret = t->CMAR3;
			break;
		case A_DMA_CCR4:
			ret = t->CCR4;
			break;
		case A_DMA_CNDTR4:
			ret = t->CNDTR4;
			break;
		case A_DMA_CPAR4:
			ret = t->CPAR4;
			break;
		case A_DMA_CMAR4:
			ret = t->CMAR4;
			break;
		case A_DMA_CCR5:
			ret = t->CCR5;
			break;
		case A_DMA_CNDTR5:
			ret = t->CNDTR5;
			break;
		case A_DMA_CPAR5:
			ret = t->CPAR5;
			break;
		case A_DMA_CMAR5:
			ret = t->CMAR5;
			break;
		case A_DMA_CCR6:
			ret = t->CCR6;
			break;
		case A_DMA_CNDTR6:
			ret = t->CNDTR6;
			break;
		case A_DMA_CPAR6:
			ret = t->CPAR6;
			break;
		case A_DMA_CMAR6:
			ret = t->CMAR6;
			break;
		case A_DMA_CCR7:
			ret = t->CCR7;
			break;
		case A_DMA_CNDTR7:
			ret = t->CNDTR7;
			break;
		case A_DMA_CPAR7:
			ret = t->CPAR7;
			break;
		case A_DMA_CMAR7:
			ret = t->CMAR7;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F1 DMA read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f1_dma_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F1DMA *t = STM32F1_DMA(opaque);


	switch (offset) {
		case A_DMA_IFCR ... A_DMA_IFCR + 3:
			value = value << ((offset - A_DMA_IFCR) << 3);
			offset = A_DMA_IFCR;
			break;
		case A_DMA_CCR1 ... A_DMA_CCR1 + 3:
			value = value << ((offset - A_DMA_CCR1) << 3);
			offset = A_DMA_CCR1;
			break;
		case A_DMA_CNDTR1 ... A_DMA_CNDTR1 + 3:
			value = value << ((offset - A_DMA_CNDTR1) << 3);
			offset = A_DMA_CNDTR1;
			break;
		case A_DMA_CPAR1 ... A_DMA_CPAR1 + 3:
			value = value << ((offset - A_DMA_CPAR1) << 3);
			offset = A_DMA_CPAR1;
			break;
		case A_DMA_CMAR1 ... A_DMA_CMAR1 + 3:
			value = value << ((offset - A_DMA_CMAR1) << 3);
			offset = A_DMA_CMAR1;
			break;
		case A_DMA_CCR2 ... A_DMA_CCR2 + 3:
			value = value << ((offset - A_DMA_CCR2) << 3);
			offset = A_DMA_CCR2;
			break;
		case A_DMA_CNDTR2 ... A_DMA_CNDTR2 + 3:
			value = value << ((offset - A_DMA_CNDTR2) << 3);
			offset = A_DMA_CNDTR2;
			break;
		case A_DMA_CPAR2 ... A_DMA_CPAR2 + 3:
			value = value << ((offset - A_DMA_CPAR2) << 3);
			offset = A_DMA_CPAR2;
			break;
		case A_DMA_CMAR2 ... A_DMA_CMAR2 + 3:
			value = value << ((offset - A_DMA_CMAR2) << 3);
			offset = A_DMA_CMAR2;
			break;
		case A_DMA_CCR3 ... A_DMA_CCR3 + 3:
			value = value << ((offset - A_DMA_CCR3) << 3);
			offset = A_DMA_CCR3;
			break;
		case A_DMA_CNDTR3 ... A_DMA_CNDTR3 + 3:
			value = value << ((offset - A_DMA_CNDTR3) << 3);
			offset = A_DMA_CNDTR3;
			break;
		case A_DMA_CPAR3 ... A_DMA_CPAR3 + 3:
			value = value << ((offset - A_DMA_CPAR3) << 3);
			offset = A_DMA_CPAR3;
			break;
		case A_DMA_CMAR3 ... A_DMA_CMAR3 + 3:
			value = value << ((offset - A_DMA_CMAR3) << 3);
			offset = A_DMA_CMAR3;
			break;
		case A_DMA_CCR4 ... A_DMA_CCR4 + 3:
			value = value << ((offset - A_DMA_CCR4) << 3);
			offset = A_DMA_CCR4;
			break;
		case A_DMA_CNDTR4 ... A_DMA_CNDTR4 + 3:
			value = value << ((offset - A_DMA_CNDTR4) << 3);
			offset = A_DMA_CNDTR4;
			break;
		case A_DMA_CPAR4 ... A_DMA_CPAR4 + 3:
			value = value << ((offset - A_DMA_CPAR4) << 3);
			offset = A_DMA_CPAR4;
			break;
		case A_DMA_CMAR4 ... A_DMA_CMAR4 + 3:
			value = value << ((offset - A_DMA_CMAR4) << 3);
			offset = A_DMA_CMAR4;
			break;
		case A_DMA_CCR5 ... A_DMA_CCR5 + 3:
			value = value << ((offset - A_DMA_CCR5) << 3);
			offset = A_DMA_CCR5;
			break;
		case A_DMA_CNDTR5 ... A_DMA_CNDTR5 + 3:
			value = value << ((offset - A_DMA_CNDTR5) << 3);
			offset = A_DMA_CNDTR5;
			break;
		case A_DMA_CPAR5 ... A_DMA_CPAR5 + 3:
			value = value << ((offset - A_DMA_CPAR5) << 3);
			offset = A_DMA_CPAR5;
			break;
		case A_DMA_CMAR5 ... A_DMA_CMAR5 + 3:
			value = value << ((offset - A_DMA_CMAR5) << 3);
			offset = A_DMA_CMAR5;
			break;
		case A_DMA_CCR6 ... A_DMA_CCR6 + 3:
			value = value << ((offset - A_DMA_CCR6) << 3);
			offset = A_DMA_CCR6;
			break;
		case A_DMA_CNDTR6 ... A_DMA_CNDTR6 + 3:
			value = value << ((offset - A_DMA_CNDTR6) << 3);
			offset = A_DMA_CNDTR6;
			break;
		case A_DMA_CPAR6 ... A_DMA_CPAR6 + 3:
			value = value << ((offset - A_DMA_CPAR6) << 3);
			offset = A_DMA_CPAR6;
			break;
		case A_DMA_CMAR6 ... A_DMA_CMAR6 + 3:
			value = value << ((offset - A_DMA_CMAR6) << 3);
			offset = A_DMA_CMAR6;
			break;
		case A_DMA_CCR7 ... A_DMA_CCR7 + 3:
			value = value << ((offset - A_DMA_CCR7) << 3);
			offset = A_DMA_CCR7;
			break;
		case A_DMA_CNDTR7 ... A_DMA_CNDTR7 + 3:
			value = value << ((offset - A_DMA_CNDTR7) << 3);
			offset = A_DMA_CNDTR7;
			break;
		case A_DMA_CPAR7 ... A_DMA_CPAR7 + 3:
			value = value << ((offset - A_DMA_CPAR7) << 3);
			offset = A_DMA_CPAR7;
			break;
		case A_DMA_CMAR7 ... A_DMA_CMAR7 + 3:
			value = value << ((offset - A_DMA_CMAR7) << 3);
			offset = A_DMA_CMAR7;
			break;

		default: break;
	}

	switch (offset) {
		case A_DMA_IFCR:
			t->IFCR = value;
			break;
		case A_DMA_CCR1:
			t->CCR1 = value;

			if (!t->channel_enabled[0] && (t->CCR1 & 0x1)) {
				t->channel_enabled[0] = 1;
				if (t->CPAR1 < 0x40000000 && t->CMAR1 < 0x40000000) {
					stm32f1_dma_transmit(t, 0);
				} else {
					if ((((!(t->CCR1 & 0x800)) && (!(t->CCR1 & 0x10)) && (!(t->CCR1 & 0x100)) && (t->CCR1 & 0x4000)) || ((!(t->CCR1 & 0x200)) && (!(t->CCR1 & 0x10)) && (!(t->CCR1 & 0x400)) && (t->CCR1 & 0x4000)) || ((!(t->CCR1 & 0x10)) && (!(t->CCR1 & 0x100)) && (!(t->CCR1 & 0x400)) && (t->CCR1 & 0x4000)) || ((!(t->CCR1 & 0x800)) && (!(t->CCR1 & 0x200)) && (!(t->CCR1 & 0x10)) && (t->CCR1 & 0x4000)))) {
						if (t->CPAR1 < 0x40000000) {
							stm32f1_dma_transmit(t, 0);
						}
					} else {
						if (t->CMAR1 < 0x40000000) {
							stm32f1_dma_transmit(t, 0);
						}
					}
				}
			}

			if ((!(t->CCR1 & 0x1))) {
				t->channel_enabled[0] = 0;
			}
			break;
		case A_DMA_CNDTR1:
			t->CNDTR1 = value;
			break;
		case A_DMA_CPAR1:
			t->CPAR1 = value;
			break;
		case A_DMA_CMAR1:
			t->CMAR1 = value;
			break;
		case A_DMA_CCR2:
			t->CCR2 = value;

			if (!t->channel_enabled[1] && (t->CCR2 & 0x1)) {
				t->channel_enabled[1] = 1;
				if (t->CPAR2 < 0x40000000 && t->CMAR2 < 0x40000000) {
					stm32f1_dma_transmit(t, 1);
				} else {
					if ((((!(t->CCR2 & 0x200)) && (!(t->CCR2 & 0x10)) && (t->CCR2 & 0x4000) && (!(t->CCR2 & 0x800))) || ((!(t->CCR2 & 0x200)) && (!(t->CCR2 & 0x400)) && (!(t->CCR2 & 0x10)) && (t->CCR2 & 0x4000)) || ((!(t->CCR2 & 0x10)) && (!(t->CCR2 & 0x100)) && (t->CCR2 & 0x4000) && (!(t->CCR2 & 0x800))) || ((!(t->CCR2 & 0x400)) && (!(t->CCR2 & 0x10)) && (!(t->CCR2 & 0x100)) && (t->CCR2 & 0x4000)))) {
						if (t->CPAR2 < 0x40000000) {
							stm32f1_dma_transmit(t, 1);
						}
					} else {
						if (t->CMAR2 < 0x40000000) {
							stm32f1_dma_transmit(t, 1);
						}
					}
				}
			}

			if ((!(t->CCR2 & 0x1))) {
				t->channel_enabled[1] = 0;
			}
			break;
		case A_DMA_CNDTR2:
			t->CNDTR2 = value;
			break;
		case A_DMA_CPAR2:
			t->CPAR2 = value;
			break;
		case A_DMA_CMAR2:
			t->CMAR2 = value;
			break;
		case A_DMA_CCR3:
			t->CCR3 = value;

			if (!t->channel_enabled[2] && (t->CCR3 & 0x1)) {
				t->channel_enabled[2] = 1;
				if (t->CPAR3 < 0x40000000 && t->CMAR3 < 0x40000000) {
					stm32f1_dma_transmit(t, 2);
				} else {
					if ((((t->CCR3 & 0x4000) && (!(t->CCR3 & 0x200)) && (!(t->CCR3 & 0x800)) && (!(t->CCR3 & 0x10))) || ((!(t->CCR3 & 0x400)) && (!(t->CCR3 & 0x10)) && (!(t->CCR3 & 0x200)) && (t->CCR3 & 0x4000)) || ((!(t->CCR3 & 0x100)) && (t->CCR3 & 0x4000) && (!(t->CCR3 & 0x800)) && (!(t->CCR3 & 0x10))) || ((!(t->CCR3 & 0x100)) && (t->CCR3 & 0x4000) && (!(t->CCR3 & 0x400)) && (!(t->CCR3 & 0x10))))) {
						if (t->CPAR3 < 0x40000000) {
							stm32f1_dma_transmit(t, 2);
						}
					} else {
						if (t->CMAR3 < 0x40000000) {
							stm32f1_dma_transmit(t, 2);
						}
					}
				}
			}

			if ((!(t->CCR3 & 0x1))) {
				t->channel_enabled[2] = 0;
			}
			break;
		case A_DMA_CNDTR3:
			t->CNDTR3 = value;
			break;
		case A_DMA_CPAR3:
			t->CPAR3 = value;
			break;
		case A_DMA_CMAR3:
			t->CMAR3 = value;
			break;
		case A_DMA_CCR4:
			t->CCR4 = value;

			if (!t->channel_enabled[3] && (t->CCR4 & 0x1)) {
				t->channel_enabled[3] = 1;
				if (t->CPAR4 < 0x40000000 && t->CMAR4 < 0x40000000) {
					stm32f1_dma_transmit(t, 3);
				} else {
					if ((((!(t->CCR4 & 0x10)) && (t->CCR4 & 0x4000) && (!(t->CCR4 & 0x100)) && (!(t->CCR4 & 0x800))) || ((!(t->CCR4 & 0x10)) && (t->CCR4 & 0x4000) && (!(t->CCR4 & 0x400)) && (!(t->CCR4 & 0x100))) || ((!(t->CCR4 & 0x200)) && (!(t->CCR4 & 0x10)) && (t->CCR4 & 0x4000) && (!(t->CCR4 & 0x400))) || ((!(t->CCR4 & 0x200)) && (!(t->CCR4 & 0x10)) && (t->CCR4 & 0x4000) && (!(t->CCR4 & 0x800))))) {
						if (t->CPAR4 < 0x40000000) {
							stm32f1_dma_transmit(t, 3);
						}
					} else {
						if (t->CMAR4 < 0x40000000) {
							stm32f1_dma_transmit(t, 3);
						}
					}
				}
			}

			if ((!(t->CCR4 & 0x1))) {
				t->channel_enabled[3] = 0;
			}
			break;
		case A_DMA_CNDTR4:
			t->CNDTR4 = value;
			break;
		case A_DMA_CPAR4:
			t->CPAR4 = value;
			break;
		case A_DMA_CMAR4:
			t->CMAR4 = value;
			break;
		case A_DMA_CCR5:
			t->CCR5 = value;

			if (!t->channel_enabled[4] && (t->CCR5 & 0x1)) {
				t->channel_enabled[4] = 1;
				if (t->CPAR5 < 0x40000000 && t->CMAR5 < 0x40000000) {
					stm32f1_dma_transmit(t, 4);
				} else {
					if ((((!(t->CCR5 & 0x200)) && (t->CCR5 & 0x4000) && (!(t->CCR5 & 0x800)) && (!(t->CCR5 & 0x10))) || ((!(t->CCR5 & 0x200)) && (!(t->CCR5 & 0x400)) && (t->CCR5 & 0x4000) && (!(t->CCR5 & 0x10))) || ((t->CCR5 & 0x4000) && (!(t->CCR5 & 0x100)) && (!(t->CCR5 & 0x800)) && (!(t->CCR5 & 0x10))) || ((!(t->CCR5 & 0x400)) && (t->CCR5 & 0x4000) && (!(t->CCR5 & 0x100)) && (!(t->CCR5 & 0x10))))) {
						if (t->CPAR5 < 0x40000000) {
							stm32f1_dma_transmit(t, 4);
						}
					} else {
						if (t->CMAR5 < 0x40000000) {
							stm32f1_dma_transmit(t, 4);
						}
					}
				}
			}

			if ((!(t->CCR5 & 0x1))) {
				t->channel_enabled[4] = 0;
			}
			break;
		case A_DMA_CNDTR5:
			t->CNDTR5 = value;
			break;
		case A_DMA_CPAR5:
			t->CPAR5 = value;
			break;
		case A_DMA_CMAR5:
			t->CMAR5 = value;
			break;
		case A_DMA_CCR6:
			t->CCR6 = value;

			if (!t->channel_enabled[5] && (t->CCR6 & 0x1)) {
				t->channel_enabled[5] = 1;
				if (t->CPAR6 < 0x40000000 && t->CMAR6 < 0x40000000) {
					stm32f1_dma_transmit(t, 5);
				} else {
					if ((((!(t->CCR6 & 0x10)) && (!(t->CCR6 & 0x200)) && (t->CCR6 & 0x4000) && (!(t->CCR6 & 0x400))) || ((!(t->CCR6 & 0x10)) && (!(t->CCR6 & 0x200)) && (!(t->CCR6 & 0x800)) && (t->CCR6 & 0x4000)) || ((!(t->CCR6 & 0x10)) && (t->CCR6 & 0x4000) && (!(t->CCR6 & 0x400)) && (!(t->CCR6 & 0x100))) || ((!(t->CCR6 & 0x10)) && (!(t->CCR6 & 0x800)) && (t->CCR6 & 0x4000) && (!(t->CCR6 & 0x100))))) {
						if (t->CPAR6 < 0x40000000) {
							stm32f1_dma_transmit(t, 5);
						}
					} else {
						if (t->CMAR6 < 0x40000000) {
							stm32f1_dma_transmit(t, 5);
						}
					}
				}
			}

			if ((!(t->CCR6 & 0x1))) {
				t->channel_enabled[5] = 0;
			}
			break;
		case A_DMA_CNDTR6:
			t->CNDTR6 = value;
			break;
		case A_DMA_CPAR6:
			t->CPAR6 = value;
			break;
		case A_DMA_CMAR6:
			t->CMAR6 = value;
			break;
		case A_DMA_CCR7:
			t->CCR7 = value;

			if (!t->channel_enabled[6] && (t->CCR7 & 0x1)) {
				t->channel_enabled[6] = 1;
				if (t->CPAR7 < 0x40000000 && t->CMAR7 < 0x40000000) {
					stm32f1_dma_transmit(t, 6);
				} else {
					if ((((!(t->CCR7 & 0x200)) && (!(t->CCR7 & 0x400)) && (!(t->CCR7 & 0x10)) && (t->CCR7 & 0x4000)) || ((!(t->CCR7 & 0x800)) && (!(t->CCR7 & 0x200)) && (!(t->CCR7 & 0x10)) && (t->CCR7 & 0x4000)) || ((!(t->CCR7 & 0x400)) && (!(t->CCR7 & 0x10)) && (t->CCR7 & 0x4000) && (!(t->CCR7 & 0x100))) || ((!(t->CCR7 & 0x800)) && (!(t->CCR7 & 0x10)) && (t->CCR7 & 0x4000) && (!(t->CCR7 & 0x100))))) {
						if (t->CPAR7 < 0x40000000) {
							stm32f1_dma_transmit(t, 6);
						}
					} else {
						if (t->CMAR7 < 0x40000000) {
							stm32f1_dma_transmit(t, 6);
						}
					}
				}
			}

			if ((!(t->CCR7 & 0x1))) {
				t->channel_enabled[6] = 0;
			}
			break;
		case A_DMA_CNDTR7:
			t->CNDTR7 = value;
			break;
		case A_DMA_CPAR7:
			t->CPAR7 = value;
			break;
		case A_DMA_CMAR7:
			t->CMAR7 = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F1 DMA write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f1_dma_ops = {
	.read = stm32f1_dma_read,
	.write = stm32f1_dma_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f1_dma_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F1DMA *t = STM32F1_DMA(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f1_dma_ops, t, "stm32f1-dma", STM32F1_DMA_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 7; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32f1_dma_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32f1_dma_reset_enter(Object *obj, ResetType type) {
	STM32F1DMA *t = STM32F1_DMA(obj);
	stm32f1_dma_register_reset(t);
}

static const VMStateDescription stm32f1_dma_vmstate = {
	.name = "stm32f1-dma",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(ISR, STM32F1DMA),
		VMSTATE_UINT32(IFCR, STM32F1DMA),
		VMSTATE_UINT32(CCR1, STM32F1DMA),
		VMSTATE_UINT32(CNDTR1, STM32F1DMA),
		VMSTATE_UINT32(CPAR1, STM32F1DMA),
		VMSTATE_UINT32(CMAR1, STM32F1DMA),
		VMSTATE_UINT32(CCR2, STM32F1DMA),
		VMSTATE_UINT32(CNDTR2, STM32F1DMA),
		VMSTATE_UINT32(CPAR2, STM32F1DMA),
		VMSTATE_UINT32(CMAR2, STM32F1DMA),
		VMSTATE_UINT32(CCR3, STM32F1DMA),
		VMSTATE_UINT32(CNDTR3, STM32F1DMA),
		VMSTATE_UINT32(CPAR3, STM32F1DMA),
		VMSTATE_UINT32(CMAR3, STM32F1DMA),
		VMSTATE_UINT32(CCR4, STM32F1DMA),
		VMSTATE_UINT32(CNDTR4, STM32F1DMA),
		VMSTATE_UINT32(CPAR4, STM32F1DMA),
		VMSTATE_UINT32(CMAR4, STM32F1DMA),
		VMSTATE_UINT32(CCR5, STM32F1DMA),
		VMSTATE_UINT32(CNDTR5, STM32F1DMA),
		VMSTATE_UINT32(CPAR5, STM32F1DMA),
		VMSTATE_UINT32(CMAR5, STM32F1DMA),
		VMSTATE_UINT32(CCR6, STM32F1DMA),
		VMSTATE_UINT32(CNDTR6, STM32F1DMA),
		VMSTATE_UINT32(CPAR6, STM32F1DMA),
		VMSTATE_UINT32(CMAR6, STM32F1DMA),
		VMSTATE_UINT32(CCR7, STM32F1DMA),
		VMSTATE_UINT32(CNDTR7, STM32F1DMA),
		VMSTATE_UINT32(CPAR7, STM32F1DMA),
		VMSTATE_UINT32(CMAR7, STM32F1DMA),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f1_dma_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f1_dma_vmstate;
	dc->realize = stm32f1_dma_realize;
	rc->phases.enter = stm32f1_dma_reset_enter;

}

static const TypeInfo stm32f1_dma_info = {
	.name = TYPE_STM32F1_DMA,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F1DMA),
	.instance_init = stm32f1_dma_init,
	.class_init = stm32f1_dma_class_init,
};

static void stm32f1_dma_register_types(void) {
	type_register_static(&stm32f1_dma_info);
}

type_init(stm32f1_dma_register_types);

#define TYPE_STM32F1_PWR "stm32f1-pwr"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F1PWR, STM32F1_PWR)

struct STM32F1PWR {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t CR;
	uint32_t CSR;
	/* base */
	uint32_t base;

};
#define STM32F1_PWR_SIZE				0x8

REG32(PWR_CR, 0x0)
	FIELD(PWR_CR, LPDS, 0, 1)
	FIELD(PWR_CR, PDDS, 1, 1)
	FIELD(PWR_CR, CWUF, 2, 1)
	FIELD(PWR_CR, CSBF, 3, 1)
	FIELD(PWR_CR, PVDE, 4, 1)
	FIELD(PWR_CR, PLS, 5, 3)
	FIELD(PWR_CR, DBP, 8, 1)
REG32(PWR_CSR, 0x4)
	FIELD(PWR_CSR, WUF, 0, 1)
	FIELD(PWR_CSR, SBF, 1, 1)
	FIELD(PWR_CSR, PVDO, 2, 1)
	FIELD(PWR_CSR, EWUP, 8, 1)


static void stm32f1_pwr_register_reset(STM32F1PWR *t) {
	t->CR = 0x0;
	t->CSR = 0x0;

}

static uint64_t stm32f1_pwr_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F1PWR *t = STM32F1_PWR(opaque);
	uint64_t ret;

	switch (offset) {
		case A_PWR_CR:
			ret = t->CR;
			break;
		case A_PWR_CSR:
			ret = t->CSR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F1 PWR read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f1_pwr_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F1PWR *t = STM32F1_PWR(opaque);


	switch (offset) {
		case A_PWR_CR ... A_PWR_CR + 3:
			value = value << ((offset - A_PWR_CR) << 3);
			offset = A_PWR_CR;
			break;
		case A_PWR_CSR ... A_PWR_CSR + 3:
			value = value << ((offset - A_PWR_CSR) << 3);
			offset = A_PWR_CSR;
			break;

		default: break;
	}

	switch (offset) {
		case A_PWR_CR:
			t->CR = value;
			break;
		case A_PWR_CSR:
			t->CSR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F1 PWR write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f1_pwr_ops = {
	.read = stm32f1_pwr_read,
	.write = stm32f1_pwr_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f1_pwr_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F1PWR *t = STM32F1_PWR(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f1_pwr_ops, t, "stm32f1-pwr", STM32F1_PWR_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32f1_pwr_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32f1_pwr_reset_enter(Object *obj, ResetType type) {
	STM32F1PWR *t = STM32F1_PWR(obj);
	stm32f1_pwr_register_reset(t);
}

static const VMStateDescription stm32f1_pwr_vmstate = {
	.name = "stm32f1-pwr",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR, STM32F1PWR),
		VMSTATE_UINT32(CSR, STM32F1PWR),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f1_pwr_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f1_pwr_vmstate;
	dc->realize = stm32f1_pwr_realize;
	rc->phases.enter = stm32f1_pwr_reset_enter;

}

static const TypeInfo stm32f1_pwr_info = {
	.name = TYPE_STM32F1_PWR,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F1PWR),
	.instance_init = stm32f1_pwr_init,
	.class_init = stm32f1_pwr_class_init,
};

static void stm32f1_pwr_register_types(void) {
	type_register_static(&stm32f1_pwr_info);
}

type_init(stm32f1_pwr_register_types);

#define TYPE_STM32F1_FLASH "stm32f1-flash"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F1FLASH, STM32F1_FLASH)

struct STM32F1FLASH {
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
	uint32_t AR;
	uint32_t OBR;
	uint32_t WRPR;
	/* base */
	uint32_t base;

};
#define STM32F1_FLASH_SIZE				0x24

REG32(FLASH_ACR, 0x0)
	FIELD(FLASH_ACR, LATENCY, 0, 3)
	FIELD(FLASH_ACR, HLFCYA, 3, 1)
	FIELD(FLASH_ACR, PRFTBE, 4, 1)
	FIELD(FLASH_ACR, PRFTBS, 5, 1)
REG32(FLASH_KEYR, 0x4)
	FIELD(FLASH_KEYR, KEY, 0, 32)
REG32(FLASH_OPTKEYR, 0x8)
	FIELD(FLASH_OPTKEYR, OPTKEY, 0, 32)
REG32(FLASH_SR, 0xc)
	FIELD(FLASH_SR, EOP, 5, 1)
	FIELD(FLASH_SR, WRPRTERR, 4, 1)
	FIELD(FLASH_SR, PGERR, 2, 1)
	FIELD(FLASH_SR, BSY, 0, 1)
REG32(FLASH_CR, 0x10)
	FIELD(FLASH_CR, PG, 0, 1)
	FIELD(FLASH_CR, PER, 1, 1)
	FIELD(FLASH_CR, MER, 2, 1)
	FIELD(FLASH_CR, OPTPG, 4, 1)
	FIELD(FLASH_CR, OPTER, 5, 1)
	FIELD(FLASH_CR, STRT, 6, 1)
	FIELD(FLASH_CR, LOCK, 7, 1)
	FIELD(FLASH_CR, OPTWRE, 9, 1)
	FIELD(FLASH_CR, ERRIE, 10, 1)
	FIELD(FLASH_CR, EOPIE, 12, 1)
REG32(FLASH_AR, 0x14)
	FIELD(FLASH_AR, FAR, 0, 32)
REG32(FLASH_OBR, 0x1c)
	FIELD(FLASH_OBR, OPTERR, 0, 1)
	FIELD(FLASH_OBR, RDPRT, 1, 1)
	FIELD(FLASH_OBR, WDG_SW, 2, 1)
	FIELD(FLASH_OBR, nRST_STOP, 3, 1)
	FIELD(FLASH_OBR, nRST_STDBY, 4, 1)
	FIELD(FLASH_OBR, Data0, 10, 8)
	FIELD(FLASH_OBR, Data1, 18, 8)
REG32(FLASH_WRPR, 0x20)
	FIELD(FLASH_WRPR, WRP, 0, 32)


static void stm32f1_flash_register_reset(STM32F1FLASH *t) {
	t->ACR = 0x30;
	t->KEYR = 0x0;
	t->OPTKEYR = 0x0;
	t->SR = 0x0;
	t->CR = 0x80;
	t->AR = 0x0;
	t->OBR = 0x3fffffc;
	t->WRPR = 0xffffffff;

}

static uint64_t stm32f1_flash_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F1FLASH *t = STM32F1_FLASH(opaque);
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
		case A_FLASH_OBR:
			ret = t->OBR;
			break;
		case A_FLASH_WRPR:
			ret = t->WRPR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F1 FLASH read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f1_flash_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F1FLASH *t = STM32F1_FLASH(opaque);


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
		case A_FLASH_AR ... A_FLASH_AR + 3:
			value = value << ((offset - A_FLASH_AR) << 3);
			offset = A_FLASH_AR;
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
			if ((value & 0x40)) {
				t->SR &= (~(0x1));
			}
			break;
		case A_FLASH_AR:
			t->AR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F1 FLASH write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f1_flash_ops = {
	.read = stm32f1_flash_read,
	.write = stm32f1_flash_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f1_flash_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F1FLASH *t = STM32F1_FLASH(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f1_flash_ops, t, "stm32f1-flash", STM32F1_FLASH_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32f1_flash_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32f1_flash_reset_enter(Object *obj, ResetType type) {
	STM32F1FLASH *t = STM32F1_FLASH(obj);
	stm32f1_flash_register_reset(t);
}

static const VMStateDescription stm32f1_flash_vmstate = {
	.name = "stm32f1-flash",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(ACR, STM32F1FLASH),
		VMSTATE_UINT32(KEYR, STM32F1FLASH),
		VMSTATE_UINT32(OPTKEYR, STM32F1FLASH),
		VMSTATE_UINT32(SR, STM32F1FLASH),
		VMSTATE_UINT32(CR, STM32F1FLASH),
		VMSTATE_UINT32(AR, STM32F1FLASH),
		VMSTATE_UINT32(OBR, STM32F1FLASH),
		VMSTATE_UINT32(WRPR, STM32F1FLASH),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f1_flash_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f1_flash_vmstate;
	dc->realize = stm32f1_flash_realize;
	rc->phases.enter = stm32f1_flash_reset_enter;

}

static const TypeInfo stm32f1_flash_info = {
	.name = TYPE_STM32F1_FLASH,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F1FLASH),
	.instance_init = stm32f1_flash_init,
	.class_init = stm32f1_flash_class_init,
};

static void stm32f1_flash_register_types(void) {
	type_register_static(&stm32f1_flash_info);
}

type_init(stm32f1_flash_register_types);

#define TYPE_STM32F1_RCC "stm32f1-rcc"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F1RCC, STM32F1_RCC)

struct STM32F1RCC {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t CR;
	uint32_t CFGR;
	uint32_t CIR;
	uint32_t APB2RSTR;
	uint32_t APB1RSTR;
	uint32_t AHBENR;
	uint32_t APB2ENR;
	uint32_t APB1ENR;
	uint32_t BDCR;
	uint32_t CSR;
	/* base */
	uint32_t base;

};
#define STM32F1_RCC_SIZE				0x28

REG32(RCC_CR, 0x0)
	FIELD(RCC_CR, HSION, 0, 1)
	FIELD(RCC_CR, HSIRDY, 1, 1)
	FIELD(RCC_CR, HSITRIM, 3, 5)
	FIELD(RCC_CR, HSICAL, 8, 8)
	FIELD(RCC_CR, HSEON, 16, 1)
	FIELD(RCC_CR, HSERDY, 17, 1)
	FIELD(RCC_CR, HSEBYP, 18, 1)
	FIELD(RCC_CR, CSSON, 19, 1)
	FIELD(RCC_CR, PLLON, 24, 1)
	FIELD(RCC_CR, PLLRDY, 25, 1)
REG32(RCC_CFGR, 0x4)
	FIELD(RCC_CFGR, SW, 0, 2)
	FIELD(RCC_CFGR, SWS, 2, 2)
	FIELD(RCC_CFGR, HPRE, 4, 4)
	FIELD(RCC_CFGR, PPRE1, 8, 3)
	FIELD(RCC_CFGR, PPRE2, 11, 3)
	FIELD(RCC_CFGR, ADCPRE, 14, 2)
	FIELD(RCC_CFGR, PLLSRC, 16, 1)
	FIELD(RCC_CFGR, PLLXTPRE, 17, 1)
	FIELD(RCC_CFGR, PLLMUL, 18, 4)
	FIELD(RCC_CFGR, OTGFSPRE, 22, 1)
	FIELD(RCC_CFGR, MCO, 24, 3)
REG32(RCC_CIR, 0x8)
	FIELD(RCC_CIR, LSIRDYF, 0, 1)
	FIELD(RCC_CIR, LSERDYF, 1, 1)
	FIELD(RCC_CIR, HSIRDYF, 2, 1)
	FIELD(RCC_CIR, HSERDYF, 3, 1)
	FIELD(RCC_CIR, PLLRDYF, 4, 1)
	FIELD(RCC_CIR, CSSF, 7, 1)
	FIELD(RCC_CIR, LSIRDYIE, 8, 1)
	FIELD(RCC_CIR, LSERDYIE, 9, 1)
	FIELD(RCC_CIR, HSIRDYIE, 10, 1)
	FIELD(RCC_CIR, HSERDYIE, 11, 1)
	FIELD(RCC_CIR, PLLRDYIE, 12, 1)
	FIELD(RCC_CIR, LSIRDYC, 16, 1)
	FIELD(RCC_CIR, LSERDYC, 17, 1)
	FIELD(RCC_CIR, HSIRDYC, 18, 1)
	FIELD(RCC_CIR, HSERDYC, 19, 1)
	FIELD(RCC_CIR, PLLRDYC, 20, 1)
	FIELD(RCC_CIR, CSSC, 23, 1)
REG32(RCC_APB2RSTR, 0xc)
	FIELD(RCC_APB2RSTR, AFIORST, 0, 1)
	FIELD(RCC_APB2RSTR, IOPARST, 2, 1)
	FIELD(RCC_APB2RSTR, IOPBRST, 3, 1)
	FIELD(RCC_APB2RSTR, IOPCRST, 4, 1)
	FIELD(RCC_APB2RSTR, IOPDRST, 5, 1)
	FIELD(RCC_APB2RSTR, IOPERST, 6, 1)
	FIELD(RCC_APB2RSTR, IOPFRST, 7, 1)
	FIELD(RCC_APB2RSTR, IOPGRST, 8, 1)
	FIELD(RCC_APB2RSTR, ADC1RST, 9, 1)
	FIELD(RCC_APB2RSTR, ADC2RST, 10, 1)
	FIELD(RCC_APB2RSTR, TIM1RST, 11, 1)
	FIELD(RCC_APB2RSTR, SPI1RST, 12, 1)
	FIELD(RCC_APB2RSTR, TIM8RST, 13, 1)
	FIELD(RCC_APB2RSTR, USART1RST, 14, 1)
	FIELD(RCC_APB2RSTR, ADC3RST, 15, 1)
	FIELD(RCC_APB2RSTR, TIM9RST, 19, 1)
	FIELD(RCC_APB2RSTR, TIM10RST, 20, 1)
	FIELD(RCC_APB2RSTR, TIM11RST, 21, 1)
REG32(RCC_APB1RSTR, 0x10)
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
	FIELD(RCC_APB1RSTR, USART2RST, 17, 1)
	FIELD(RCC_APB1RSTR, USART3RST, 18, 1)
	FIELD(RCC_APB1RSTR, UART4RST, 19, 1)
	FIELD(RCC_APB1RSTR, UART5RST, 20, 1)
	FIELD(RCC_APB1RSTR, I2C1RST, 21, 1)
	FIELD(RCC_APB1RSTR, I2C2RST, 22, 1)
	FIELD(RCC_APB1RSTR, USBRST, 23, 1)
	FIELD(RCC_APB1RSTR, CANRST, 25, 1)
	FIELD(RCC_APB1RSTR, BKPRST, 27, 1)
	FIELD(RCC_APB1RSTR, PWRRST, 28, 1)
	FIELD(RCC_APB1RSTR, DACRST, 29, 1)
REG32(RCC_AHBENR, 0x14)
	FIELD(RCC_AHBENR, DMA1EN, 0, 1)
	FIELD(RCC_AHBENR, DMA2EN, 1, 1)
	FIELD(RCC_AHBENR, SRAMEN, 2, 1)
	FIELD(RCC_AHBENR, FLITFEN, 4, 1)
	FIELD(RCC_AHBENR, CRCEN, 6, 1)
	FIELD(RCC_AHBENR, FSMCEN, 8, 1)
	FIELD(RCC_AHBENR, SDIOEN, 10, 1)
REG32(RCC_APB2ENR, 0x18)
	FIELD(RCC_APB2ENR, AFIOEN, 0, 1)
	FIELD(RCC_APB2ENR, IOPAEN, 2, 1)
	FIELD(RCC_APB2ENR, IOPBEN, 3, 1)
	FIELD(RCC_APB2ENR, IOPCEN, 4, 1)
	FIELD(RCC_APB2ENR, IOPDEN, 5, 1)
	FIELD(RCC_APB2ENR, IOPEEN, 6, 1)
	FIELD(RCC_APB2ENR, IOPFEN, 7, 1)
	FIELD(RCC_APB2ENR, IOPGEN, 8, 1)
	FIELD(RCC_APB2ENR, ADC1EN, 9, 1)
	FIELD(RCC_APB2ENR, ADC2EN, 10, 1)
	FIELD(RCC_APB2ENR, TIM1EN, 11, 1)
	FIELD(RCC_APB2ENR, SPI1EN, 12, 1)
	FIELD(RCC_APB2ENR, TIM8EN, 13, 1)
	FIELD(RCC_APB2ENR, USART1EN, 14, 1)
	FIELD(RCC_APB2ENR, ADC3EN, 15, 1)
	FIELD(RCC_APB2ENR, TIM9EN, 19, 1)
	FIELD(RCC_APB2ENR, TIM10EN, 20, 1)
	FIELD(RCC_APB2ENR, TIM11EN, 21, 1)
REG32(RCC_APB1ENR, 0x1c)
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
	FIELD(RCC_APB1ENR, USBEN, 23, 1)
	FIELD(RCC_APB1ENR, CANEN, 25, 1)
	FIELD(RCC_APB1ENR, BKPEN, 27, 1)
	FIELD(RCC_APB1ENR, PWREN, 28, 1)
	FIELD(RCC_APB1ENR, DACEN, 29, 1)
REG32(RCC_BDCR, 0x20)
	FIELD(RCC_BDCR, LSEON, 0, 1)
	FIELD(RCC_BDCR, LSERDY, 1, 1)
	FIELD(RCC_BDCR, LSEBYP, 2, 1)
	FIELD(RCC_BDCR, RTCSEL, 8, 2)
	FIELD(RCC_BDCR, RTCEN, 15, 1)
	FIELD(RCC_BDCR, BDRST, 16, 1)
REG32(RCC_CSR, 0x24)
	FIELD(RCC_CSR, LSION, 0, 1)
	FIELD(RCC_CSR, LSIRDY, 1, 1)
	FIELD(RCC_CSR, RMVF, 24, 1)
	FIELD(RCC_CSR, PINRSTF, 26, 1)
	FIELD(RCC_CSR, PORRSTF, 27, 1)
	FIELD(RCC_CSR, SFTRSTF, 28, 1)
	FIELD(RCC_CSR, IWDGRSTF, 29, 1)
	FIELD(RCC_CSR, WWDGRSTF, 30, 1)
	FIELD(RCC_CSR, LPWRRSTF, 31, 1)


__attribute__((unused))
static void stm32f1_rcc_update(STM32F1RCC *t) {
	int conditions = (t->CIR & 0x80);
	qemu_set_irq(t->irq[0], conditions);

}

static void stm32f1_rcc_register_reset(STM32F1RCC *t) {
	t->CR = 0x83;
	t->CFGR = 0x0;
	t->CIR = 0x0;
	t->APB2RSTR = 0x0;
	t->APB1RSTR = 0x0;
	t->AHBENR = 0x14;
	t->APB2ENR = 0x0;
	t->APB1ENR = 0x0;
	t->BDCR = 0x0;
	t->CSR = 0xc000000;

}

static uint64_t stm32f1_rcc_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F1RCC *t = STM32F1_RCC(opaque);
	uint64_t ret;

	switch (offset) {
		case A_RCC_CR:
			ret = t->CR;
			break;
		case A_RCC_CFGR:
			ret = t->CFGR;
			break;
		case A_RCC_CIR:
			ret = t->CIR;
			break;
		case A_RCC_APB2RSTR:
			ret = t->APB2RSTR;
			break;
		case A_RCC_APB1RSTR:
			ret = t->APB1RSTR;
			break;
		case A_RCC_AHBENR:
			ret = t->AHBENR;
			break;
		case A_RCC_APB2ENR:
			ret = t->APB2ENR;
			break;
		case A_RCC_APB1ENR:
			ret = t->APB1ENR;
			break;
		case A_RCC_BDCR:
			ret = t->BDCR;
			break;
		case A_RCC_CSR:
			ret = t->CSR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F1 RCC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f1_rcc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F1RCC *t = STM32F1_RCC(opaque);


	switch (offset) {
		case A_RCC_CR ... A_RCC_CR + 3:
			value = value << ((offset - A_RCC_CR) << 3);
			offset = A_RCC_CR;
			break;
		case A_RCC_CFGR ... A_RCC_CFGR + 3:
			value = value << ((offset - A_RCC_CFGR) << 3);
			offset = A_RCC_CFGR;
			break;
		case A_RCC_CIR ... A_RCC_CIR + 3:
			value = value << ((offset - A_RCC_CIR) << 3);
			offset = A_RCC_CIR;
			break;
		case A_RCC_APB2RSTR ... A_RCC_APB2RSTR + 3:
			value = value << ((offset - A_RCC_APB2RSTR) << 3);
			offset = A_RCC_APB2RSTR;
			break;
		case A_RCC_APB1RSTR ... A_RCC_APB1RSTR + 3:
			value = value << ((offset - A_RCC_APB1RSTR) << 3);
			offset = A_RCC_APB1RSTR;
			break;
		case A_RCC_AHBENR ... A_RCC_AHBENR + 3:
			value = value << ((offset - A_RCC_AHBENR) << 3);
			offset = A_RCC_AHBENR;
			break;
		case A_RCC_APB2ENR ... A_RCC_APB2ENR + 3:
			value = value << ((offset - A_RCC_APB2ENR) << 3);
			offset = A_RCC_APB2ENR;
			break;
		case A_RCC_APB1ENR ... A_RCC_APB1ENR + 3:
			value = value << ((offset - A_RCC_APB1ENR) << 3);
			offset = A_RCC_APB1ENR;
			break;
		case A_RCC_BDCR ... A_RCC_BDCR + 3:
			value = value << ((offset - A_RCC_BDCR) << 3);
			offset = A_RCC_BDCR;
			break;
		case A_RCC_CSR ... A_RCC_CSR + 3:
			value = value << ((offset - A_RCC_CSR) << 3);
			offset = A_RCC_CSR;
			break;

		default: break;
	}

	switch (offset) {
		case A_RCC_CR:
			t->CR = value;
			if (((!(value & 0x10000)) && (!(value & 0x80000)))) {
				t->CR &= (~(0x20000));
			} else if ((value & 0x10000)) {
				t->CR |= 0x20000;
			}
			if (((value & 0x1000000) && (!(value & 0x2000000)))) {
				t->CR |= 0x2000000;
			} else if ((value & 0x1000000)) {
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
			break;
		case A_RCC_CFGR:
			t->CFGR = value;
			if (((!(value & 0x1)) && (value & 0x2))) {
				t->CFGR |= 0x8;
				t->CFGR &= (~(0x4));
			} else if (((value & 0x1) && (!(value & 0x2)))) {
				t->CFGR |= 0x4;
				t->CFGR &= (~(0x8));
			} else if (((!(value & 0x1)) && (!(value & 0x2)))) {
				t->CFGR &= (~(0x4));
				t->CFGR &= (~(0x8));
			}
			if ((value == 0x0)) {
				t->CFGR &= (~(0x4));
				t->CFGR &= (~(0x8));
			}
			break;
		case A_RCC_CIR:
			t->CIR = value;
			stm32f1_rcc_update(t);
			break;
		case A_RCC_APB2RSTR:
			t->APB2RSTR = value;
			break;
		case A_RCC_APB1RSTR:
			t->APB1RSTR = value;
			break;
		case A_RCC_AHBENR:
			t->AHBENR = value;
			break;
		case A_RCC_APB2ENR:
			t->APB2ENR = value;
			break;
		case A_RCC_APB1ENR:
			t->APB1ENR = value;
			break;
		case A_RCC_BDCR:
			t->BDCR = value;
			if ((value & 0x1)) {
				t->BDCR |= 0x2;
			}
			if ((!(value & 0x4))) {
				t->BDCR &= (~(0x2));
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

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F1 RCC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f1_rcc_ops = {
	.read = stm32f1_rcc_read,
	.write = stm32f1_rcc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f1_rcc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F1RCC *t = STM32F1_RCC(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f1_rcc_ops, t, "stm32f1-rcc", STM32F1_RCC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32f1_rcc_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32f1_rcc_reset_enter(Object *obj, ResetType type) {
	STM32F1RCC *t = STM32F1_RCC(obj);
	stm32f1_rcc_register_reset(t);
}

static const VMStateDescription stm32f1_rcc_vmstate = {
	.name = "stm32f1-rcc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR, STM32F1RCC),
		VMSTATE_UINT32(CFGR, STM32F1RCC),
		VMSTATE_UINT32(CIR, STM32F1RCC),
		VMSTATE_UINT32(APB2RSTR, STM32F1RCC),
		VMSTATE_UINT32(APB1RSTR, STM32F1RCC),
		VMSTATE_UINT32(AHBENR, STM32F1RCC),
		VMSTATE_UINT32(APB2ENR, STM32F1RCC),
		VMSTATE_UINT32(APB1ENR, STM32F1RCC),
		VMSTATE_UINT32(BDCR, STM32F1RCC),
		VMSTATE_UINT32(CSR, STM32F1RCC),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f1_rcc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f1_rcc_vmstate;
	dc->realize = stm32f1_rcc_realize;
	rc->phases.enter = stm32f1_rcc_reset_enter;

}

static const TypeInfo stm32f1_rcc_info = {
	.name = TYPE_STM32F1_RCC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F1RCC),
	.instance_init = stm32f1_rcc_init,
	.class_init = stm32f1_rcc_class_init,
};

static void stm32f1_rcc_register_types(void) {
	type_register_static(&stm32f1_rcc_info);
}

type_init(stm32f1_rcc_register_types);

#define TYPE_STM32F1_TIM2 "stm32f1-tim2"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F1TIM2, STM32F1_TIM2)

struct STM32F1TIM2 {
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
	/* timer */
	QEMUTimer *timers[1];
	uint8_t enabled[1];

	uint32_t cur_period[1];

	uint8_t period_set[1];

	uint8_t period_reg_set[1];

	/* base */
	uint32_t base;

};
#define STM32F1_TIM2_SIZE				0x50

REG32(TIM2_CR1, 0x0)
	FIELD(TIM2_CR1, CKD, 8, 2)
	FIELD(TIM2_CR1, ARPE, 7, 1)
	FIELD(TIM2_CR1, CMS, 5, 2)
	FIELD(TIM2_CR1, DIR, 4, 1)
	FIELD(TIM2_CR1, OPM, 3, 1)
	FIELD(TIM2_CR1, URS, 2, 1)
	FIELD(TIM2_CR1, UDIS, 1, 1)
	FIELD(TIM2_CR1, CEN, 0, 1)
REG32(TIM2_CR2, 0x4)
	FIELD(TIM2_CR2, TI1S, 7, 1)
	FIELD(TIM2_CR2, MMS, 4, 3)
	FIELD(TIM2_CR2, CCDS, 3, 1)
REG32(TIM2_SMCR, 0x8)
	FIELD(TIM2_SMCR, ETP, 15, 1)
	FIELD(TIM2_SMCR, ECE, 14, 1)
	FIELD(TIM2_SMCR, ETPS, 12, 2)
	FIELD(TIM2_SMCR, ETF, 8, 4)
	FIELD(TIM2_SMCR, MSM, 7, 1)
	FIELD(TIM2_SMCR, TS, 4, 3)
	FIELD(TIM2_SMCR, SMS, 0, 3)
REG32(TIM2_DIER, 0xc)
	FIELD(TIM2_DIER, TDE, 14, 1)
	FIELD(TIM2_DIER, CC4DE, 12, 1)
	FIELD(TIM2_DIER, CC3DE, 11, 1)
	FIELD(TIM2_DIER, CC2DE, 10, 1)
	FIELD(TIM2_DIER, CC1DE, 9, 1)
	FIELD(TIM2_DIER, UDE, 8, 1)
	FIELD(TIM2_DIER, TIE, 6, 1)
	FIELD(TIM2_DIER, CC4IE, 4, 1)
	FIELD(TIM2_DIER, CC3IE, 3, 1)
	FIELD(TIM2_DIER, CC2IE, 2, 1)
	FIELD(TIM2_DIER, CC1IE, 1, 1)
	FIELD(TIM2_DIER, UIE, 0, 1)
REG32(TIM2_SR, 0x10)
	FIELD(TIM2_SR, CC4OF, 12, 1)
	FIELD(TIM2_SR, CC3OF, 11, 1)
	FIELD(TIM2_SR, CC2OF, 10, 1)
	FIELD(TIM2_SR, CC1OF, 9, 1)
	FIELD(TIM2_SR, TIF, 6, 1)
	FIELD(TIM2_SR, CC4IF, 4, 1)
	FIELD(TIM2_SR, CC3IF, 3, 1)
	FIELD(TIM2_SR, CC2IF, 2, 1)
	FIELD(TIM2_SR, CC1IF, 1, 1)
	FIELD(TIM2_SR, UIF, 0, 1)
REG32(TIM2_EGR, 0x14)
	FIELD(TIM2_EGR, TG, 6, 1)
	FIELD(TIM2_EGR, CC4G, 4, 1)
	FIELD(TIM2_EGR, CC3G, 3, 1)
	FIELD(TIM2_EGR, CC2G, 2, 1)
	FIELD(TIM2_EGR, CC1G, 1, 1)
	FIELD(TIM2_EGR, UG, 0, 1)
REG32(TIM2_CCMR1_Output, 0x18)
	FIELD(TIM2_CCMR1_Output, OC2CE, 15, 1)
	FIELD(TIM2_CCMR1_Output, OC2M, 12, 3)
	FIELD(TIM2_CCMR1_Output, OC2PE, 11, 1)
	FIELD(TIM2_CCMR1_Output, OC2FE, 10, 1)
	FIELD(TIM2_CCMR1_Output, CC2S, 8, 2)
	FIELD(TIM2_CCMR1_Output, OC1CE, 7, 1)
	FIELD(TIM2_CCMR1_Output, OC1M, 4, 3)
	FIELD(TIM2_CCMR1_Output, OC1PE, 3, 1)
	FIELD(TIM2_CCMR1_Output, OC1FE, 2, 1)
	FIELD(TIM2_CCMR1_Output, CC1S, 0, 2)
REG32(TIM2_CCMR1_Input, 0x18)
	FIELD(TIM2_CCMR1_Input, IC2F, 12, 4)
	FIELD(TIM2_CCMR1_Input, IC2PSC, 10, 2)
	FIELD(TIM2_CCMR1_Input, CC2S, 8, 2)
	FIELD(TIM2_CCMR1_Input, IC1F, 4, 4)
	FIELD(TIM2_CCMR1_Input, IC1PSC, 2, 2)
	FIELD(TIM2_CCMR1_Input, CC1S, 0, 2)
REG32(TIM2_CCMR2_Output, 0x1c)
	FIELD(TIM2_CCMR2_Output, O24CE, 15, 1)
	FIELD(TIM2_CCMR2_Output, OC4M, 12, 3)
	FIELD(TIM2_CCMR2_Output, OC4PE, 11, 1)
	FIELD(TIM2_CCMR2_Output, OC4FE, 10, 1)
	FIELD(TIM2_CCMR2_Output, CC4S, 8, 2)
	FIELD(TIM2_CCMR2_Output, OC3CE, 7, 1)
	FIELD(TIM2_CCMR2_Output, OC3M, 4, 3)
	FIELD(TIM2_CCMR2_Output, OC3PE, 3, 1)
	FIELD(TIM2_CCMR2_Output, OC3FE, 2, 1)
	FIELD(TIM2_CCMR2_Output, CC3S, 0, 2)
REG32(TIM2_CCMR2_Input, 0x1c)
	FIELD(TIM2_CCMR2_Input, IC4F, 12, 4)
	FIELD(TIM2_CCMR2_Input, IC4PSC, 10, 2)
	FIELD(TIM2_CCMR2_Input, CC4S, 8, 2)
	FIELD(TIM2_CCMR2_Input, IC3F, 4, 4)
	FIELD(TIM2_CCMR2_Input, IC3PSC, 2, 2)
	FIELD(TIM2_CCMR2_Input, CC3S, 0, 2)
REG32(TIM2_CCER, 0x20)
	FIELD(TIM2_CCER, CC4P, 13, 1)
	FIELD(TIM2_CCER, CC4E, 12, 1)
	FIELD(TIM2_CCER, CC3P, 9, 1)
	FIELD(TIM2_CCER, CC3E, 8, 1)
	FIELD(TIM2_CCER, CC2P, 5, 1)
	FIELD(TIM2_CCER, CC2E, 4, 1)
	FIELD(TIM2_CCER, CC1P, 1, 1)
	FIELD(TIM2_CCER, CC1E, 0, 1)
REG32(TIM2_CNT, 0x24)
	FIELD(TIM2_CNT, CNT, 0, 16)
REG32(TIM2_PSC, 0x28)
	FIELD(TIM2_PSC, PSC, 0, 16)
REG32(TIM2_ARR, 0x2c)
	FIELD(TIM2_ARR, ARR, 0, 16)
REG32(TIM2_CCR1, 0x34)
	FIELD(TIM2_CCR1, CCR1, 0, 16)
REG32(TIM2_CCR2, 0x38)
	FIELD(TIM2_CCR2, CCR2, 0, 16)
REG32(TIM2_CCR3, 0x3c)
	FIELD(TIM2_CCR3, CCR3, 0, 16)
REG32(TIM2_CCR4, 0x40)
	FIELD(TIM2_CCR4, CCR4, 0, 16)
REG32(TIM2_DCR, 0x48)
	FIELD(TIM2_DCR, DBL, 8, 5)
	FIELD(TIM2_DCR, DBA, 0, 5)
REG32(TIM2_DMAR, 0x4c)
	FIELD(TIM2_DMAR, DMAB, 0, 16)


__attribute__((unused))
static void stm32f1_tim2_update(STM32F1TIM2 *t) {
	int conditions = (((t->SR & 0x80) && (t->DIER & 0x80)) || ((t->DIER & 0x40) && (t->SR & 0x40)) || ((t->DIER & 0x20) && (t->SR & 0x20)) || ((t->DIER & 0x10) && (t->SR & 0x10)) || ((t->SR & 0x1) && (t->DIER & 0x1)) || ((t->SR & 0x8) && (t->DIER & 0x8)) || ((t->SR & 0x2) && (t->DIER & 0x2)) || ((t->SR & 0x4) && (t->DIER & 0x4)));
	qemu_set_irq(t->irq[0], conditions);

}

static void stm32f1_tim2_timer_callback_0(void *opaque) {
  static uint8_t period_reg_set_last = 0;
  STM32F1TIM2 *t = (STM32F1TIM2*)opaque;

  if (!period_reg_set_last && t->period_reg_set[0]) {
      period_reg_set_last = 1;
      t->CNT = 0;
  }
  t->CNT += 1;
  unsigned prd = t->period_set[0] ? t->cur_period[0] : t->ARR;
  if (t->CNT == prd) {
    t->cur_period[0] = t->ARR;
    t->CNT = 0;
    t->SR |= 0x10;t->DIER |= 0x1;t->DIER |= 0x2;t->SR |= 0x2;t->SR |= 0x1;t->SR |= 0x4;t->SR |= 0x8;t->DIER |= 0x20;t->SR |= 0x80;t->SR |= 0x20;t->DIER |= 0x4;t->SR |= 0x40;t->DIER |= 0x40;t->DIER |= 0x8;t->DIER |= 0x10;t->DIER |= 0x80;;
    qemu_set_irq(t->irq[0], 1);
    qemu_set_irq(t->irq[0], 0);
  }

  if (t->enabled[0]) {
    timer_mod(t->timers[0], qemu_clock_get_us(QEMU_CLOCK_VIRTUAL) + 1);
  }
}


static void stm32f1_tim2_register_reset(STM32F1TIM2 *t) {
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
	t->enabled[0] = 0;
	t->period_set[0] = 0;
	t->period_reg_set[0] = 0;

}

static uint64_t stm32f1_tim2_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F1TIM2 *t = STM32F1_TIM2(opaque);
	uint64_t ret;

	switch (offset) {
		case A_TIM2_CR1:
			ret = t->CR1;
			break;
		case A_TIM2_CR2:
			ret = t->CR2;
			break;
		case A_TIM2_SMCR:
			ret = t->SMCR;
			break;
		case A_TIM2_DIER:
			ret = t->DIER;
			break;
		case A_TIM2_SR:
			ret = t->SR;
			break;
		case A_TIM2_CCMR1_Output:
			ret = t->CCMR1_Output;
			break;
		case A_TIM2_CCMR2_Output:
			ret = t->CCMR2_Output;
			break;
		case A_TIM2_CCER:
			ret = t->CCER;
			break;
		case A_TIM2_CNT:
			ret = t->CNT;
			break;
		case A_TIM2_PSC:
			ret = t->PSC;
			break;
		case A_TIM2_ARR:
			ret = t->ARR;
			break;
		case A_TIM2_CCR1:
			ret = t->CCR1;
			break;
		case A_TIM2_CCR2:
			ret = t->CCR2;
			break;
		case A_TIM2_CCR3:
			ret = t->CCR3;
			break;
		case A_TIM2_CCR4:
			ret = t->CCR4;
			break;
		case A_TIM2_DCR:
			ret = t->DCR;
			break;
		case A_TIM2_DMAR:
			ret = t->DMAR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F1 TIM2 read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f1_tim2_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F1TIM2 *t = STM32F1_TIM2(opaque);


	switch (offset) {
		case A_TIM2_CR1 ... A_TIM2_CR1 + 3:
			value = value << ((offset - A_TIM2_CR1) << 3);
			offset = A_TIM2_CR1;
			break;
		case A_TIM2_CR2 ... A_TIM2_CR2 + 3:
			value = value << ((offset - A_TIM2_CR2) << 3);
			offset = A_TIM2_CR2;
			break;
		case A_TIM2_SMCR ... A_TIM2_SMCR + 3:
			value = value << ((offset - A_TIM2_SMCR) << 3);
			offset = A_TIM2_SMCR;
			break;
		case A_TIM2_DIER ... A_TIM2_DIER + 3:
			value = value << ((offset - A_TIM2_DIER) << 3);
			offset = A_TIM2_DIER;
			break;
		case A_TIM2_SR ... A_TIM2_SR + 3:
			value = value << ((offset - A_TIM2_SR) << 3);
			offset = A_TIM2_SR;
			break;
		case A_TIM2_EGR ... A_TIM2_EGR + 3:
			value = value << ((offset - A_TIM2_EGR) << 3);
			offset = A_TIM2_EGR;
			break;
		case A_TIM2_CCMR1_Output ... A_TIM2_CCMR1_Output + 3:
			value = value << ((offset - A_TIM2_CCMR1_Output) << 3);
			offset = A_TIM2_CCMR1_Output;
			break;
		case A_TIM2_CCMR2_Output ... A_TIM2_CCMR2_Output + 3:
			value = value << ((offset - A_TIM2_CCMR2_Output) << 3);
			offset = A_TIM2_CCMR2_Output;
			break;
		case A_TIM2_CCER ... A_TIM2_CCER + 3:
			value = value << ((offset - A_TIM2_CCER) << 3);
			offset = A_TIM2_CCER;
			break;
		case A_TIM2_CNT ... A_TIM2_CNT + 3:
			value = value << ((offset - A_TIM2_CNT) << 3);
			offset = A_TIM2_CNT;
			break;
		case A_TIM2_PSC ... A_TIM2_PSC + 3:
			value = value << ((offset - A_TIM2_PSC) << 3);
			offset = A_TIM2_PSC;
			break;
		case A_TIM2_ARR ... A_TIM2_ARR + 3:
			value = value << ((offset - A_TIM2_ARR) << 3);
			offset = A_TIM2_ARR;
			break;
		case A_TIM2_CCR1 ... A_TIM2_CCR1 + 3:
			value = value << ((offset - A_TIM2_CCR1) << 3);
			offset = A_TIM2_CCR1;
			break;
		case A_TIM2_CCR2 ... A_TIM2_CCR2 + 3:
			value = value << ((offset - A_TIM2_CCR2) << 3);
			offset = A_TIM2_CCR2;
			break;
		case A_TIM2_CCR3 ... A_TIM2_CCR3 + 3:
			value = value << ((offset - A_TIM2_CCR3) << 3);
			offset = A_TIM2_CCR3;
			break;
		case A_TIM2_CCR4 ... A_TIM2_CCR4 + 3:
			value = value << ((offset - A_TIM2_CCR4) << 3);
			offset = A_TIM2_CCR4;
			break;
		case A_TIM2_DCR ... A_TIM2_DCR + 3:
			value = value << ((offset - A_TIM2_DCR) << 3);
			offset = A_TIM2_DCR;
			break;
		case A_TIM2_DMAR ... A_TIM2_DMAR + 3:
			value = value << ((offset - A_TIM2_DMAR) << 3);
			offset = A_TIM2_DMAR;
			break;

		default: break;
	}

	switch (offset) {
		case A_TIM2_CR1:
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
				t->timers[0] = timer_new(QEMU_CLOCK_VIRTUAL, SCALE_US, stm32f1_tim2_timer_callback_0, t);
			}
			break;
		case A_TIM2_CR2:
			t->CR2 = value;
			break;
		case A_TIM2_SMCR:
			t->SMCR = value;
			break;
		case A_TIM2_DIER:
			t->DIER = value;
			stm32f1_tim2_update(t);
			break;
		case A_TIM2_SR:
			t->SR = value;
			stm32f1_tim2_update(t);
			break;
		case A_TIM2_EGR:
			t->EGR = value;
			break;
		case A_TIM2_CCMR1_Output:
			t->CCMR1_Output = value;
			break;
		case A_TIM2_CCMR2_Output:
			t->CCMR2_Output = value;
			break;
		case A_TIM2_CCER:
			t->CCER = value;
			break;
		case A_TIM2_CNT:
			t->CNT = value;
			break;
		case A_TIM2_PSC:
			t->PSC = value;
			break;
		case A_TIM2_ARR:
			t->ARR = value;
			if (!t->period_reg_set[0]) {
				t->period_reg_set[0] = 1;
			}
			break;
		case A_TIM2_CCR1:
			t->CCR1 = value;
			break;
		case A_TIM2_CCR2:
			t->CCR2 = value;
			break;
		case A_TIM2_CCR3:
			t->CCR3 = value;
			break;
		case A_TIM2_CCR4:
			t->CCR4 = value;
			break;
		case A_TIM2_DCR:
			t->DCR = value;
			break;
		case A_TIM2_DMAR:
			t->DMAR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F1 TIM2 write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f1_tim2_ops = {
	.read = stm32f1_tim2_read,
	.write = stm32f1_tim2_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f1_tim2_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F1TIM2 *t = STM32F1_TIM2(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f1_tim2_ops, t, "stm32f1-tim2", STM32F1_TIM2_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32f1_tim2_realize(DeviceState *dev, Error **errp) {
	STM32F1TIM2 *t = STM32F1_TIM2(dev);
	t->timers[0] = timer_new(QEMU_CLOCK_VIRTUAL, SCALE_US, stm32f1_tim2_timer_callback_0, t);

}

static void stm32f1_tim2_reset_enter(Object *obj, ResetType type) {
	STM32F1TIM2 *t = STM32F1_TIM2(obj);
	stm32f1_tim2_register_reset(t);
}

static const VMStateDescription stm32f1_tim2_vmstate = {
	.name = "stm32f1-tim2",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR1, STM32F1TIM2),
		VMSTATE_UINT32(CR2, STM32F1TIM2),
		VMSTATE_UINT32(SMCR, STM32F1TIM2),
		VMSTATE_UINT32(DIER, STM32F1TIM2),
		VMSTATE_UINT32(SR, STM32F1TIM2),
		VMSTATE_UINT32(EGR, STM32F1TIM2),
		VMSTATE_UINT32(CCMR1_Output, STM32F1TIM2),
		VMSTATE_UINT32(CCMR1_Input, STM32F1TIM2),
		VMSTATE_UINT32(CCMR2_Output, STM32F1TIM2),
		VMSTATE_UINT32(CCMR2_Input, STM32F1TIM2),
		VMSTATE_UINT32(CCER, STM32F1TIM2),
		VMSTATE_UINT32(CNT, STM32F1TIM2),
		VMSTATE_UINT32(PSC, STM32F1TIM2),
		VMSTATE_UINT32(ARR, STM32F1TIM2),
		VMSTATE_UINT32(CCR1, STM32F1TIM2),
		VMSTATE_UINT32(CCR2, STM32F1TIM2),
		VMSTATE_UINT32(CCR3, STM32F1TIM2),
		VMSTATE_UINT32(CCR4, STM32F1TIM2),
		VMSTATE_UINT32(DCR, STM32F1TIM2),
		VMSTATE_UINT32(DMAR, STM32F1TIM2),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f1_tim2_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f1_tim2_vmstate;
	dc->realize = stm32f1_tim2_realize;
	rc->phases.enter = stm32f1_tim2_reset_enter;

}

static const TypeInfo stm32f1_tim2_info = {
	.name = TYPE_STM32F1_TIM2,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F1TIM2),
	.instance_init = stm32f1_tim2_init,
	.class_init = stm32f1_tim2_class_init,
};

static void stm32f1_tim2_register_types(void) {
	type_register_static(&stm32f1_tim2_info);
}

type_init(stm32f1_tim2_register_types);

#define TYPE_STM32F1_TIM1 "stm32f1-tim1"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F1TIM1, STM32F1_TIM1)

struct STM32F1TIM1 {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[4];

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
	uint32_t RCR;
	uint32_t BDTR;
	/* timer */
	QEMUTimer *timers[1];
	uint8_t enabled[1];

	uint32_t cur_period[1];

	uint8_t period_set[1];

	uint8_t period_reg_set[1];

	/* base */
	uint32_t base;

};
#define STM32F1_TIM1_SIZE				0x50

REG32(TIM1_CR1, 0x0)
	FIELD(TIM1_CR1, CKD, 8, 2)
	FIELD(TIM1_CR1, ARPE, 7, 1)
	FIELD(TIM1_CR1, CMS, 5, 2)
	FIELD(TIM1_CR1, DIR, 4, 1)
	FIELD(TIM1_CR1, OPM, 3, 1)
	FIELD(TIM1_CR1, URS, 2, 1)
	FIELD(TIM1_CR1, UDIS, 1, 1)
	FIELD(TIM1_CR1, CEN, 0, 1)
REG32(TIM1_CR2, 0x4)
	FIELD(TIM1_CR2, OIS4, 14, 1)
	FIELD(TIM1_CR2, OIS3N, 13, 1)
	FIELD(TIM1_CR2, OIS3, 12, 1)
	FIELD(TIM1_CR2, OIS2N, 11, 1)
	FIELD(TIM1_CR2, OIS2, 10, 1)
	FIELD(TIM1_CR2, OIS1N, 9, 1)
	FIELD(TIM1_CR2, OIS1, 8, 1)
	FIELD(TIM1_CR2, TI1S, 7, 1)
	FIELD(TIM1_CR2, MMS, 4, 3)
	FIELD(TIM1_CR2, CCDS, 3, 1)
	FIELD(TIM1_CR2, CCUS, 2, 1)
	FIELD(TIM1_CR2, CCPC, 0, 1)
REG32(TIM1_SMCR, 0x8)
	FIELD(TIM1_SMCR, ETP, 15, 1)
	FIELD(TIM1_SMCR, ECE, 14, 1)
	FIELD(TIM1_SMCR, ETPS, 12, 2)
	FIELD(TIM1_SMCR, ETF, 8, 4)
	FIELD(TIM1_SMCR, MSM, 7, 1)
	FIELD(TIM1_SMCR, TS, 4, 3)
	FIELD(TIM1_SMCR, SMS, 0, 3)
REG32(TIM1_DIER, 0xc)
	FIELD(TIM1_DIER, TDE, 14, 1)
	FIELD(TIM1_DIER, COMDE, 13, 1)
	FIELD(TIM1_DIER, CC4DE, 12, 1)
	FIELD(TIM1_DIER, CC3DE, 11, 1)
	FIELD(TIM1_DIER, CC2DE, 10, 1)
	FIELD(TIM1_DIER, CC1DE, 9, 1)
	FIELD(TIM1_DIER, UDE, 8, 1)
	FIELD(TIM1_DIER, TIE, 6, 1)
	FIELD(TIM1_DIER, CC4IE, 4, 1)
	FIELD(TIM1_DIER, CC3IE, 3, 1)
	FIELD(TIM1_DIER, CC2IE, 2, 1)
	FIELD(TIM1_DIER, CC1IE, 1, 1)
	FIELD(TIM1_DIER, UIE, 0, 1)
	FIELD(TIM1_DIER, BIE, 7, 1)
	FIELD(TIM1_DIER, COMIE, 5, 1)
REG32(TIM1_SR, 0x10)
	FIELD(TIM1_SR, CC4OF, 12, 1)
	FIELD(TIM1_SR, CC3OF, 11, 1)
	FIELD(TIM1_SR, CC2OF, 10, 1)
	FIELD(TIM1_SR, CC1OF, 9, 1)
	FIELD(TIM1_SR, BIF, 7, 1)
	FIELD(TIM1_SR, TIF, 6, 1)
	FIELD(TIM1_SR, COMIF, 5, 1)
	FIELD(TIM1_SR, CC4IF, 4, 1)
	FIELD(TIM1_SR, CC3IF, 3, 1)
	FIELD(TIM1_SR, CC2IF, 2, 1)
	FIELD(TIM1_SR, CC1IF, 1, 1)
	FIELD(TIM1_SR, UIF, 0, 1)
REG32(TIM1_EGR, 0x14)
	FIELD(TIM1_EGR, BG, 7, 1)
	FIELD(TIM1_EGR, TG, 6, 1)
	FIELD(TIM1_EGR, COMG, 5, 1)
	FIELD(TIM1_EGR, CC4G, 4, 1)
	FIELD(TIM1_EGR, CC3G, 3, 1)
	FIELD(TIM1_EGR, CC2G, 2, 1)
	FIELD(TIM1_EGR, CC1G, 1, 1)
	FIELD(TIM1_EGR, UG, 0, 1)
REG32(TIM1_CCMR1_Output, 0x18)
	FIELD(TIM1_CCMR1_Output, OC2CE, 15, 1)
	FIELD(TIM1_CCMR1_Output, OC2M, 12, 3)
	FIELD(TIM1_CCMR1_Output, OC2PE, 11, 1)
	FIELD(TIM1_CCMR1_Output, OC2FE, 10, 1)
	FIELD(TIM1_CCMR1_Output, CC2S, 8, 2)
	FIELD(TIM1_CCMR1_Output, OC1CE, 7, 1)
	FIELD(TIM1_CCMR1_Output, OC1M, 4, 3)
	FIELD(TIM1_CCMR1_Output, OC1PE, 3, 1)
	FIELD(TIM1_CCMR1_Output, OC1FE, 2, 1)
	FIELD(TIM1_CCMR1_Output, CC1S, 0, 2)
REG32(TIM1_CCMR1_Input, 0x18)
	FIELD(TIM1_CCMR1_Input, IC2F, 12, 4)
	FIELD(TIM1_CCMR1_Input, IC2PCS, 10, 2)
	FIELD(TIM1_CCMR1_Input, CC2S, 8, 2)
	FIELD(TIM1_CCMR1_Input, IC1F, 4, 4)
	FIELD(TIM1_CCMR1_Input, ICPCS, 2, 2)
	FIELD(TIM1_CCMR1_Input, CC1S, 0, 2)
REG32(TIM1_CCMR2_Output, 0x1c)
	FIELD(TIM1_CCMR2_Output, OC4CE, 15, 1)
	FIELD(TIM1_CCMR2_Output, OC4M, 12, 3)
	FIELD(TIM1_CCMR2_Output, OC4PE, 11, 1)
	FIELD(TIM1_CCMR2_Output, OC4FE, 10, 1)
	FIELD(TIM1_CCMR2_Output, CC4S, 8, 2)
	FIELD(TIM1_CCMR2_Output, OC3CE, 7, 1)
	FIELD(TIM1_CCMR2_Output, OC3M, 4, 3)
	FIELD(TIM1_CCMR2_Output, OC3PE, 3, 1)
	FIELD(TIM1_CCMR2_Output, OC3FE, 2, 1)
	FIELD(TIM1_CCMR2_Output, CC3S, 0, 2)
REG32(TIM1_CCMR2_Input, 0x1c)
	FIELD(TIM1_CCMR2_Input, IC4F, 12, 4)
	FIELD(TIM1_CCMR2_Input, IC4PSC, 10, 2)
	FIELD(TIM1_CCMR2_Input, CC4S, 8, 2)
	FIELD(TIM1_CCMR2_Input, IC3F, 4, 4)
	FIELD(TIM1_CCMR2_Input, IC3PSC, 2, 2)
	FIELD(TIM1_CCMR2_Input, CC3S, 0, 2)
REG32(TIM1_CCER, 0x20)
	FIELD(TIM1_CCER, CC4P, 13, 1)
	FIELD(TIM1_CCER, CC4E, 12, 1)
	FIELD(TIM1_CCER, CC3NP, 11, 1)
	FIELD(TIM1_CCER, CC3NE, 10, 1)
	FIELD(TIM1_CCER, CC3P, 9, 1)
	FIELD(TIM1_CCER, CC3E, 8, 1)
	FIELD(TIM1_CCER, CC2NP, 7, 1)
	FIELD(TIM1_CCER, CC2NE, 6, 1)
	FIELD(TIM1_CCER, CC2P, 5, 1)
	FIELD(TIM1_CCER, CC2E, 4, 1)
	FIELD(TIM1_CCER, CC1NP, 3, 1)
	FIELD(TIM1_CCER, CC1NE, 2, 1)
	FIELD(TIM1_CCER, CC1P, 1, 1)
	FIELD(TIM1_CCER, CC1E, 0, 1)
REG32(TIM1_CNT, 0x24)
	FIELD(TIM1_CNT, CNT, 0, 16)
REG32(TIM1_PSC, 0x28)
	FIELD(TIM1_PSC, PSC, 0, 16)
REG32(TIM1_ARR, 0x2c)
	FIELD(TIM1_ARR, ARR, 0, 16)
REG32(TIM1_CCR1, 0x34)
	FIELD(TIM1_CCR1, CCR1, 0, 16)
REG32(TIM1_CCR2, 0x38)
	FIELD(TIM1_CCR2, CCR2, 0, 16)
REG32(TIM1_CCR3, 0x3c)
	FIELD(TIM1_CCR3, CCR3, 0, 16)
REG32(TIM1_CCR4, 0x40)
	FIELD(TIM1_CCR4, CCR4, 0, 16)
REG32(TIM1_DCR, 0x48)
	FIELD(TIM1_DCR, DBL, 8, 5)
	FIELD(TIM1_DCR, DBA, 0, 5)
REG32(TIM1_DMAR, 0x4c)
	FIELD(TIM1_DMAR, DMAB, 0, 16)
REG32(TIM1_RCR, 0x30)
	FIELD(TIM1_RCR, REP, 0, 8)
REG32(TIM1_BDTR, 0x44)
	FIELD(TIM1_BDTR, MOE, 15, 1)
	FIELD(TIM1_BDTR, AOE, 14, 1)
	FIELD(TIM1_BDTR, BKP, 13, 1)
	FIELD(TIM1_BDTR, BKE, 12, 1)
	FIELD(TIM1_BDTR, OSSR, 11, 1)
	FIELD(TIM1_BDTR, OSSI, 10, 1)
	FIELD(TIM1_BDTR, LOCK, 8, 2)
	FIELD(TIM1_BDTR, DTG, 0, 8)


__attribute__((unused))
static void stm32f1_tim1_update(STM32F1TIM1 *t) {
	int conditions = (((t->SR & 0x80) && (t->DIER & 0x80)) || ((t->DIER & 0x40) && (t->SR & 0x40)) || ((t->DIER & 0x20) && (t->SR & 0x20)) || ((t->DIER & 0x10) && (t->SR & 0x10)) || ((t->SR & 0x1) && (t->DIER & 0x1)) || ((t->SR & 0x8) && (t->DIER & 0x8)) || ((t->SR & 0x2) && (t->DIER & 0x2)) || ((t->SR & 0x4) && (t->DIER & 0x4)));
	qemu_set_irq(t->irq[0], conditions);

}

static void stm32f1_tim1_timer_callback_0(void *opaque) {
  static uint8_t period_reg_set_last = 0;
  STM32F1TIM1 *t = (STM32F1TIM1*)opaque;

  if (!period_reg_set_last && t->period_reg_set[0]) {
      period_reg_set_last = 1;
      t->CNT = 0;
  }
  t->CNT += 1;
  unsigned prd = t->period_set[0] ? t->cur_period[0] : t->ARR;
  if (t->CNT == prd) {
    t->cur_period[0] = t->ARR;
    t->CNT = 0;
    t->SR |= 0x10;t->DIER |= 0x1;t->DIER |= 0x2;t->SR |= 0x2;t->SR |= 0x1;t->SR |= 0x4;t->SR |= 0x8;t->DIER |= 0x20;t->SR |= 0x80;t->SR |= 0x20;t->DIER |= 0x4;t->SR |= 0x40;t->DIER |= 0x40;t->DIER |= 0x8;t->DIER |= 0x10;t->DIER |= 0x80;;
    qemu_set_irq(t->irq[0], 1);
    qemu_set_irq(t->irq[0], 0);
  }

  if (t->enabled[0]) {
    timer_mod(t->timers[0], qemu_clock_get_us(QEMU_CLOCK_VIRTUAL) + 1);
  }
}


static void stm32f1_tim1_register_reset(STM32F1TIM1 *t) {
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
	t->RCR = 0x0;
	t->BDTR = 0x0;
	t->enabled[0] = 0;
	t->period_set[0] = 0;
	t->period_reg_set[0] = 0;

}

static uint64_t stm32f1_tim1_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F1TIM1 *t = STM32F1_TIM1(opaque);
	uint64_t ret;

	switch (offset) {
		case A_TIM1_CR1:
			ret = t->CR1;
			break;
		case A_TIM1_CR2:
			ret = t->CR2;
			break;
		case A_TIM1_SMCR:
			ret = t->SMCR;
			break;
		case A_TIM1_DIER:
			ret = t->DIER;
			break;
		case A_TIM1_SR:
			ret = t->SR;
			break;
		case A_TIM1_CCMR1_Output:
			ret = t->CCMR1_Output;
			break;
		case A_TIM1_CCMR2_Output:
			ret = t->CCMR2_Output;
			break;
		case A_TIM1_CCER:
			ret = t->CCER;
			break;
		case A_TIM1_CNT:
			ret = t->CNT;
			break;
		case A_TIM1_PSC:
			ret = t->PSC;
			break;
		case A_TIM1_ARR:
			ret = t->ARR;
			break;
		case A_TIM1_CCR1:
			ret = t->CCR1;
			break;
		case A_TIM1_CCR2:
			ret = t->CCR2;
			break;
		case A_TIM1_CCR3:
			ret = t->CCR3;
			break;
		case A_TIM1_CCR4:
			ret = t->CCR4;
			break;
		case A_TIM1_DCR:
			ret = t->DCR;
			break;
		case A_TIM1_DMAR:
			ret = t->DMAR;
			break;
		case A_TIM1_RCR:
			ret = t->RCR;
			break;
		case A_TIM1_BDTR:
			ret = t->BDTR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F1 TIM1 read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f1_tim1_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F1TIM1 *t = STM32F1_TIM1(opaque);


	switch (offset) {
		case A_TIM1_CR1 ... A_TIM1_CR1 + 3:
			value = value << ((offset - A_TIM1_CR1) << 3);
			offset = A_TIM1_CR1;
			break;
		case A_TIM1_CR2 ... A_TIM1_CR2 + 3:
			value = value << ((offset - A_TIM1_CR2) << 3);
			offset = A_TIM1_CR2;
			break;
		case A_TIM1_SMCR ... A_TIM1_SMCR + 3:
			value = value << ((offset - A_TIM1_SMCR) << 3);
			offset = A_TIM1_SMCR;
			break;
		case A_TIM1_DIER ... A_TIM1_DIER + 3:
			value = value << ((offset - A_TIM1_DIER) << 3);
			offset = A_TIM1_DIER;
			break;
		case A_TIM1_SR ... A_TIM1_SR + 3:
			value = value << ((offset - A_TIM1_SR) << 3);
			offset = A_TIM1_SR;
			break;
		case A_TIM1_EGR ... A_TIM1_EGR + 3:
			value = value << ((offset - A_TIM1_EGR) << 3);
			offset = A_TIM1_EGR;
			break;
		case A_TIM1_CCMR1_Output ... A_TIM1_CCMR1_Output + 3:
			value = value << ((offset - A_TIM1_CCMR1_Output) << 3);
			offset = A_TIM1_CCMR1_Output;
			break;
		case A_TIM1_CCMR2_Output ... A_TIM1_CCMR2_Output + 3:
			value = value << ((offset - A_TIM1_CCMR2_Output) << 3);
			offset = A_TIM1_CCMR2_Output;
			break;
		case A_TIM1_CCER ... A_TIM1_CCER + 3:
			value = value << ((offset - A_TIM1_CCER) << 3);
			offset = A_TIM1_CCER;
			break;
		case A_TIM1_CNT ... A_TIM1_CNT + 3:
			value = value << ((offset - A_TIM1_CNT) << 3);
			offset = A_TIM1_CNT;
			break;
		case A_TIM1_PSC ... A_TIM1_PSC + 3:
			value = value << ((offset - A_TIM1_PSC) << 3);
			offset = A_TIM1_PSC;
			break;
		case A_TIM1_ARR ... A_TIM1_ARR + 3:
			value = value << ((offset - A_TIM1_ARR) << 3);
			offset = A_TIM1_ARR;
			break;
		case A_TIM1_CCR1 ... A_TIM1_CCR1 + 3:
			value = value << ((offset - A_TIM1_CCR1) << 3);
			offset = A_TIM1_CCR1;
			break;
		case A_TIM1_CCR2 ... A_TIM1_CCR2 + 3:
			value = value << ((offset - A_TIM1_CCR2) << 3);
			offset = A_TIM1_CCR2;
			break;
		case A_TIM1_CCR3 ... A_TIM1_CCR3 + 3:
			value = value << ((offset - A_TIM1_CCR3) << 3);
			offset = A_TIM1_CCR3;
			break;
		case A_TIM1_CCR4 ... A_TIM1_CCR4 + 3:
			value = value << ((offset - A_TIM1_CCR4) << 3);
			offset = A_TIM1_CCR4;
			break;
		case A_TIM1_DCR ... A_TIM1_DCR + 3:
			value = value << ((offset - A_TIM1_DCR) << 3);
			offset = A_TIM1_DCR;
			break;
		case A_TIM1_DMAR ... A_TIM1_DMAR + 3:
			value = value << ((offset - A_TIM1_DMAR) << 3);
			offset = A_TIM1_DMAR;
			break;
		case A_TIM1_RCR ... A_TIM1_RCR + 3:
			value = value << ((offset - A_TIM1_RCR) << 3);
			offset = A_TIM1_RCR;
			break;
		case A_TIM1_BDTR ... A_TIM1_BDTR + 3:
			value = value << ((offset - A_TIM1_BDTR) << 3);
			offset = A_TIM1_BDTR;
			break;

		default: break;
	}

	switch (offset) {
		case A_TIM1_CR1:
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
				t->timers[0] = timer_new(QEMU_CLOCK_VIRTUAL, SCALE_US, stm32f1_tim1_timer_callback_0, t);
			}
			break;
		case A_TIM1_CR2:
			t->CR2 = value;
			break;
		case A_TIM1_SMCR:
			t->SMCR = value;
			break;
		case A_TIM1_DIER:
			t->DIER = value;
			stm32f1_tim1_update(t);
			break;
		case A_TIM1_SR:
			t->SR = value;
			stm32f1_tim1_update(t);
			break;
		case A_TIM1_EGR:
			t->EGR = value;
			break;
		case A_TIM1_CCMR1_Output:
			t->CCMR1_Output = value;
			break;
		case A_TIM1_CCMR2_Output:
			t->CCMR2_Output = value;
			break;
		case A_TIM1_CCER:
			t->CCER = value;
			break;
		case A_TIM1_CNT:
			t->CNT = value;
			break;
		case A_TIM1_PSC:
			t->PSC = value;
			break;
		case A_TIM1_ARR:
			t->ARR = value;
			if (!t->period_reg_set[0]) {
				t->period_reg_set[0] = 1;
			}
			break;
		case A_TIM1_CCR1:
			t->CCR1 = value;
			break;
		case A_TIM1_CCR2:
			t->CCR2 = value;
			break;
		case A_TIM1_CCR3:
			t->CCR3 = value;
			break;
		case A_TIM1_CCR4:
			t->CCR4 = value;
			break;
		case A_TIM1_DCR:
			t->DCR = value;
			break;
		case A_TIM1_DMAR:
			t->DMAR = value;
			break;
		case A_TIM1_RCR:
			t->RCR = value;
			break;
		case A_TIM1_BDTR:
			t->BDTR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F1 TIM1 write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f1_tim1_ops = {
	.read = stm32f1_tim1_read,
	.write = stm32f1_tim1_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f1_tim1_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F1TIM1 *t = STM32F1_TIM1(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f1_tim1_ops, t, "stm32f1-tim1", STM32F1_TIM1_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 4; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32f1_tim1_realize(DeviceState *dev, Error **errp) {
	STM32F1TIM1 *t = STM32F1_TIM1(dev);
	t->timers[0] = timer_new(QEMU_CLOCK_VIRTUAL, SCALE_US, stm32f1_tim1_timer_callback_0, t);

}

static void stm32f1_tim1_reset_enter(Object *obj, ResetType type) {
	STM32F1TIM1 *t = STM32F1_TIM1(obj);
	stm32f1_tim1_register_reset(t);
}

static const VMStateDescription stm32f1_tim1_vmstate = {
	.name = "stm32f1-tim1",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR1, STM32F1TIM1),
		VMSTATE_UINT32(CR2, STM32F1TIM1),
		VMSTATE_UINT32(SMCR, STM32F1TIM1),
		VMSTATE_UINT32(DIER, STM32F1TIM1),
		VMSTATE_UINT32(SR, STM32F1TIM1),
		VMSTATE_UINT32(EGR, STM32F1TIM1),
		VMSTATE_UINT32(CCMR1_Output, STM32F1TIM1),
		VMSTATE_UINT32(CCMR1_Input, STM32F1TIM1),
		VMSTATE_UINT32(CCMR2_Output, STM32F1TIM1),
		VMSTATE_UINT32(CCMR2_Input, STM32F1TIM1),
		VMSTATE_UINT32(CCER, STM32F1TIM1),
		VMSTATE_UINT32(CNT, STM32F1TIM1),
		VMSTATE_UINT32(PSC, STM32F1TIM1),
		VMSTATE_UINT32(ARR, STM32F1TIM1),
		VMSTATE_UINT32(CCR1, STM32F1TIM1),
		VMSTATE_UINT32(CCR2, STM32F1TIM1),
		VMSTATE_UINT32(CCR3, STM32F1TIM1),
		VMSTATE_UINT32(CCR4, STM32F1TIM1),
		VMSTATE_UINT32(DCR, STM32F1TIM1),
		VMSTATE_UINT32(DMAR, STM32F1TIM1),
		VMSTATE_UINT32(RCR, STM32F1TIM1),
		VMSTATE_UINT32(BDTR, STM32F1TIM1),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f1_tim1_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f1_tim1_vmstate;
	dc->realize = stm32f1_tim1_realize;
	rc->phases.enter = stm32f1_tim1_reset_enter;

}

static const TypeInfo stm32f1_tim1_info = {
	.name = TYPE_STM32F1_TIM1,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F1TIM1),
	.instance_init = stm32f1_tim1_init,
	.class_init = stm32f1_tim1_class_init,
};

static void stm32f1_tim1_register_types(void) {
	type_register_static(&stm32f1_tim1_info);
}

type_init(stm32f1_tim1_register_types);

#define TYPE_STM32F1_AFIO "stm32f1-afio"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F1AFIO, STM32F1_AFIO)

struct STM32F1AFIO {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/*registers*/
	uint32_t EVCR;
	uint32_t MAPR;
	uint32_t EXTICR1;
	uint32_t EXTICR2;
	uint32_t EXTICR3;
	uint32_t EXTICR4;
	uint32_t MAPR2;
	/* base */
	uint32_t base;

};
#define STM32F1_AFIO_SIZE				0x20

REG32(AFIO_EVCR, 0x0)
	FIELD(AFIO_EVCR, PIN, 0, 4)
	FIELD(AFIO_EVCR, PORT, 4, 3)
	FIELD(AFIO_EVCR, EVOE, 7, 1)
REG32(AFIO_MAPR, 0x4)
	FIELD(AFIO_MAPR, SPI1_REMAP, 0, 1)
	FIELD(AFIO_MAPR, I2C1_REMAP, 1, 1)
	FIELD(AFIO_MAPR, USART1_REMAP, 2, 1)
	FIELD(AFIO_MAPR, USART2_REMAP, 3, 1)
	FIELD(AFIO_MAPR, USART3_REMAP, 4, 2)
	FIELD(AFIO_MAPR, TIM1_REMAP, 6, 2)
	FIELD(AFIO_MAPR, TIM2_REMAP, 8, 2)
	FIELD(AFIO_MAPR, TIM3_REMAP, 10, 2)
	FIELD(AFIO_MAPR, TIM4_REMAP, 12, 1)
	FIELD(AFIO_MAPR, CAN_REMAP, 13, 2)
	FIELD(AFIO_MAPR, PD01_REMAP, 15, 1)
	FIELD(AFIO_MAPR, TIM5CH4_IREMAP, 16, 1)
	FIELD(AFIO_MAPR, ADC1_ETRGINJ_REMAP, 17, 1)
	FIELD(AFIO_MAPR, ADC1_ETRGREG_REMAP, 18, 1)
	FIELD(AFIO_MAPR, ADC2_ETRGINJ_REMAP, 19, 1)
	FIELD(AFIO_MAPR, ADC2_ETRGREG_REMAP, 20, 1)
	FIELD(AFIO_MAPR, SWJ_CFG, 24, 3)
REG32(AFIO_EXTICR1, 0x8)
	FIELD(AFIO_EXTICR1, EXTI0, 0, 4)
	FIELD(AFIO_EXTICR1, EXTI1, 4, 4)
	FIELD(AFIO_EXTICR1, EXTI2, 8, 4)
	FIELD(AFIO_EXTICR1, EXTI3, 12, 4)
REG32(AFIO_EXTICR2, 0xc)
	FIELD(AFIO_EXTICR2, EXTI4, 0, 4)
	FIELD(AFIO_EXTICR2, EXTI5, 4, 4)
	FIELD(AFIO_EXTICR2, EXTI6, 8, 4)
	FIELD(AFIO_EXTICR2, EXTI7, 12, 4)
REG32(AFIO_EXTICR3, 0x10)
	FIELD(AFIO_EXTICR3, EXTI8, 0, 4)
	FIELD(AFIO_EXTICR3, EXTI9, 4, 4)
	FIELD(AFIO_EXTICR3, EXTI10, 8, 4)
	FIELD(AFIO_EXTICR3, EXTI11, 12, 4)
REG32(AFIO_EXTICR4, 0x14)
	FIELD(AFIO_EXTICR4, EXTI12, 0, 4)
	FIELD(AFIO_EXTICR4, EXTI13, 4, 4)
	FIELD(AFIO_EXTICR4, EXTI14, 8, 4)
	FIELD(AFIO_EXTICR4, EXTI15, 12, 4)
REG32(AFIO_MAPR2, 0x1c)
	FIELD(AFIO_MAPR2, TIM9_REMAP, 5, 1)
	FIELD(AFIO_MAPR2, TIM10_REMAP, 6, 1)
	FIELD(AFIO_MAPR2, TIM11_REMAP, 7, 1)
	FIELD(AFIO_MAPR2, TIM13_REMAP, 8, 1)
	FIELD(AFIO_MAPR2, TIM14_REMAP, 9, 1)
	FIELD(AFIO_MAPR2, FSMC_NADV, 10, 1)


static void stm32f1_afio_register_reset(STM32F1AFIO *t) {
	t->EVCR = 0x0;
	t->MAPR = 0x0;
	t->EXTICR1 = 0x0;
	t->EXTICR2 = 0x0;
	t->EXTICR3 = 0x0;
	t->EXTICR4 = 0x0;
	t->MAPR2 = 0x0;

}

static uint64_t stm32f1_afio_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F1AFIO *t = STM32F1_AFIO(opaque);
	uint64_t ret;

	switch (offset) {
		case A_AFIO_EVCR:
			ret = t->EVCR;
			break;
		case A_AFIO_MAPR:
			ret = t->MAPR;
			break;
		case A_AFIO_EXTICR1:
			ret = t->EXTICR1;
			break;
		case A_AFIO_EXTICR2:
			ret = t->EXTICR2;
			break;
		case A_AFIO_EXTICR3:
			ret = t->EXTICR3;
			break;
		case A_AFIO_EXTICR4:
			ret = t->EXTICR4;
			break;
		case A_AFIO_MAPR2:
			ret = t->MAPR2;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F1 AFIO read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f1_afio_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F1AFIO *t = STM32F1_AFIO(opaque);


	switch (offset) {
		case A_AFIO_EVCR ... A_AFIO_EVCR + 3:
			value = value << ((offset - A_AFIO_EVCR) << 3);
			offset = A_AFIO_EVCR;
			break;
		case A_AFIO_MAPR ... A_AFIO_MAPR + 3:
			value = value << ((offset - A_AFIO_MAPR) << 3);
			offset = A_AFIO_MAPR;
			break;
		case A_AFIO_EXTICR1 ... A_AFIO_EXTICR1 + 3:
			value = value << ((offset - A_AFIO_EXTICR1) << 3);
			offset = A_AFIO_EXTICR1;
			break;
		case A_AFIO_EXTICR2 ... A_AFIO_EXTICR2 + 3:
			value = value << ((offset - A_AFIO_EXTICR2) << 3);
			offset = A_AFIO_EXTICR2;
			break;
		case A_AFIO_EXTICR3 ... A_AFIO_EXTICR3 + 3:
			value = value << ((offset - A_AFIO_EXTICR3) << 3);
			offset = A_AFIO_EXTICR3;
			break;
		case A_AFIO_EXTICR4 ... A_AFIO_EXTICR4 + 3:
			value = value << ((offset - A_AFIO_EXTICR4) << 3);
			offset = A_AFIO_EXTICR4;
			break;
		case A_AFIO_MAPR2 ... A_AFIO_MAPR2 + 3:
			value = value << ((offset - A_AFIO_MAPR2) << 3);
			offset = A_AFIO_MAPR2;
			break;

		default: break;
	}

	switch (offset) {
		case A_AFIO_EVCR:
			t->EVCR = value;
			break;
		case A_AFIO_MAPR:
			t->MAPR = value;
			break;
		case A_AFIO_EXTICR1:
			t->EXTICR1 = value;
			break;
		case A_AFIO_EXTICR2:
			t->EXTICR2 = value;
			break;
		case A_AFIO_EXTICR3:
			t->EXTICR3 = value;
			break;
		case A_AFIO_EXTICR4:
			t->EXTICR4 = value;
			break;
		case A_AFIO_MAPR2:
			t->MAPR2 = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F1 AFIO write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f1_afio_ops = {
	.read = stm32f1_afio_read,
	.write = stm32f1_afio_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f1_afio_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F1AFIO *t = STM32F1_AFIO(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f1_afio_ops, t, "stm32f1-afio", STM32F1_AFIO_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void stm32f1_afio_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32f1_afio_reset_enter(Object *obj, ResetType type) {
	STM32F1AFIO *t = STM32F1_AFIO(obj);
	stm32f1_afio_register_reset(t);
}

static const VMStateDescription stm32f1_afio_vmstate = {
	.name = "stm32f1-afio",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(EVCR, STM32F1AFIO),
		VMSTATE_UINT32(MAPR, STM32F1AFIO),
		VMSTATE_UINT32(EXTICR1, STM32F1AFIO),
		VMSTATE_UINT32(EXTICR2, STM32F1AFIO),
		VMSTATE_UINT32(EXTICR3, STM32F1AFIO),
		VMSTATE_UINT32(EXTICR4, STM32F1AFIO),
		VMSTATE_UINT32(MAPR2, STM32F1AFIO),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f1_afio_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f1_afio_vmstate;
	dc->realize = stm32f1_afio_realize;
	rc->phases.enter = stm32f1_afio_reset_enter;

}

static const TypeInfo stm32f1_afio_info = {
	.name = TYPE_STM32F1_AFIO,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F1AFIO),
	.instance_init = stm32f1_afio_init,
	.class_init = stm32f1_afio_class_init,
};

static void stm32f1_afio_register_types(void) {
	type_register_static(&stm32f1_afio_info);
}

type_init(stm32f1_afio_register_types);

#define TYPE_STM32F1_EXTI "stm32f1-exti"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F1EXTI, STM32F1_EXTI)

struct STM32F1EXTI {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[8];

	/*registers*/
	uint32_t IMR;
	uint32_t EMR;
	uint32_t RTSR;
	uint32_t FTSR;
	uint32_t SWIER;
	uint32_t PR;
	/* chardev backend */
	CharBackend chr;
	/* base */
	uint32_t base;

};
#define STM32F1_EXTI_SIZE				0x18

REG32(EXTI_IMR, 0x0)
	FIELD(EXTI_IMR, MR0, 0, 1)
	FIELD(EXTI_IMR, MR1, 1, 1)
	FIELD(EXTI_IMR, MR2, 2, 1)
	FIELD(EXTI_IMR, MR3, 3, 1)
	FIELD(EXTI_IMR, MR4, 4, 1)
	FIELD(EXTI_IMR, MR5, 5, 1)
	FIELD(EXTI_IMR, MR6, 6, 1)
	FIELD(EXTI_IMR, MR7, 7, 1)
	FIELD(EXTI_IMR, MR8, 8, 1)
	FIELD(EXTI_IMR, MR9, 9, 1)
	FIELD(EXTI_IMR, MR10, 10, 1)
	FIELD(EXTI_IMR, MR11, 11, 1)
	FIELD(EXTI_IMR, MR12, 12, 1)
	FIELD(EXTI_IMR, MR13, 13, 1)
	FIELD(EXTI_IMR, MR14, 14, 1)
	FIELD(EXTI_IMR, MR15, 15, 1)
	FIELD(EXTI_IMR, MR16, 16, 1)
	FIELD(EXTI_IMR, MR17, 17, 1)
	FIELD(EXTI_IMR, MR18, 18, 1)
REG32(EXTI_EMR, 0x4)
	FIELD(EXTI_EMR, MR0, 0, 1)
	FIELD(EXTI_EMR, MR1, 1, 1)
	FIELD(EXTI_EMR, MR2, 2, 1)
	FIELD(EXTI_EMR, MR3, 3, 1)
	FIELD(EXTI_EMR, MR4, 4, 1)
	FIELD(EXTI_EMR, MR5, 5, 1)
	FIELD(EXTI_EMR, MR6, 6, 1)
	FIELD(EXTI_EMR, MR7, 7, 1)
	FIELD(EXTI_EMR, MR8, 8, 1)
	FIELD(EXTI_EMR, MR9, 9, 1)
	FIELD(EXTI_EMR, MR10, 10, 1)
	FIELD(EXTI_EMR, MR11, 11, 1)
	FIELD(EXTI_EMR, MR12, 12, 1)
	FIELD(EXTI_EMR, MR13, 13, 1)
	FIELD(EXTI_EMR, MR14, 14, 1)
	FIELD(EXTI_EMR, MR15, 15, 1)
	FIELD(EXTI_EMR, MR16, 16, 1)
	FIELD(EXTI_EMR, MR17, 17, 1)
	FIELD(EXTI_EMR, MR18, 18, 1)
REG32(EXTI_RTSR, 0x8)
	FIELD(EXTI_RTSR, TR0, 0, 1)
	FIELD(EXTI_RTSR, TR1, 1, 1)
	FIELD(EXTI_RTSR, TR2, 2, 1)
	FIELD(EXTI_RTSR, TR3, 3, 1)
	FIELD(EXTI_RTSR, TR4, 4, 1)
	FIELD(EXTI_RTSR, TR5, 5, 1)
	FIELD(EXTI_RTSR, TR6, 6, 1)
	FIELD(EXTI_RTSR, TR7, 7, 1)
	FIELD(EXTI_RTSR, TR8, 8, 1)
	FIELD(EXTI_RTSR, TR9, 9, 1)
	FIELD(EXTI_RTSR, TR10, 10, 1)
	FIELD(EXTI_RTSR, TR11, 11, 1)
	FIELD(EXTI_RTSR, TR12, 12, 1)
	FIELD(EXTI_RTSR, TR13, 13, 1)
	FIELD(EXTI_RTSR, TR14, 14, 1)
	FIELD(EXTI_RTSR, TR15, 15, 1)
	FIELD(EXTI_RTSR, TR16, 16, 1)
	FIELD(EXTI_RTSR, TR17, 17, 1)
	FIELD(EXTI_RTSR, TR18, 18, 1)
REG32(EXTI_FTSR, 0xc)
	FIELD(EXTI_FTSR, TR0, 0, 1)
	FIELD(EXTI_FTSR, TR1, 1, 1)
	FIELD(EXTI_FTSR, TR2, 2, 1)
	FIELD(EXTI_FTSR, TR3, 3, 1)
	FIELD(EXTI_FTSR, TR4, 4, 1)
	FIELD(EXTI_FTSR, TR5, 5, 1)
	FIELD(EXTI_FTSR, TR6, 6, 1)
	FIELD(EXTI_FTSR, TR7, 7, 1)
	FIELD(EXTI_FTSR, TR8, 8, 1)
	FIELD(EXTI_FTSR, TR9, 9, 1)
	FIELD(EXTI_FTSR, TR10, 10, 1)
	FIELD(EXTI_FTSR, TR11, 11, 1)
	FIELD(EXTI_FTSR, TR12, 12, 1)
	FIELD(EXTI_FTSR, TR13, 13, 1)
	FIELD(EXTI_FTSR, TR14, 14, 1)
	FIELD(EXTI_FTSR, TR15, 15, 1)
	FIELD(EXTI_FTSR, TR16, 16, 1)
	FIELD(EXTI_FTSR, TR17, 17, 1)
	FIELD(EXTI_FTSR, TR18, 18, 1)
REG32(EXTI_SWIER, 0x10)
	FIELD(EXTI_SWIER, SWIER0, 0, 1)
	FIELD(EXTI_SWIER, SWIER1, 1, 1)
	FIELD(EXTI_SWIER, SWIER2, 2, 1)
	FIELD(EXTI_SWIER, SWIER3, 3, 1)
	FIELD(EXTI_SWIER, SWIER4, 4, 1)
	FIELD(EXTI_SWIER, SWIER5, 5, 1)
	FIELD(EXTI_SWIER, SWIER6, 6, 1)
	FIELD(EXTI_SWIER, SWIER7, 7, 1)
	FIELD(EXTI_SWIER, SWIER8, 8, 1)
	FIELD(EXTI_SWIER, SWIER9, 9, 1)
	FIELD(EXTI_SWIER, SWIER10, 10, 1)
	FIELD(EXTI_SWIER, SWIER11, 11, 1)
	FIELD(EXTI_SWIER, SWIER12, 12, 1)
	FIELD(EXTI_SWIER, SWIER13, 13, 1)
	FIELD(EXTI_SWIER, SWIER14, 14, 1)
	FIELD(EXTI_SWIER, SWIER15, 15, 1)
	FIELD(EXTI_SWIER, SWIER16, 16, 1)
	FIELD(EXTI_SWIER, SWIER17, 17, 1)
	FIELD(EXTI_SWIER, SWIER18, 18, 1)
REG32(EXTI_PR, 0x14)
	FIELD(EXTI_PR, PR0, 0, 1)
	FIELD(EXTI_PR, PR1, 1, 1)
	FIELD(EXTI_PR, PR2, 2, 1)
	FIELD(EXTI_PR, PR3, 3, 1)
	FIELD(EXTI_PR, PR4, 4, 1)
	FIELD(EXTI_PR, PR5, 5, 1)
	FIELD(EXTI_PR, PR6, 6, 1)
	FIELD(EXTI_PR, PR7, 7, 1)
	FIELD(EXTI_PR, PR8, 8, 1)
	FIELD(EXTI_PR, PR9, 9, 1)
	FIELD(EXTI_PR, PR10, 10, 1)
	FIELD(EXTI_PR, PR11, 11, 1)
	FIELD(EXTI_PR, PR12, 12, 1)
	FIELD(EXTI_PR, PR13, 13, 1)
	FIELD(EXTI_PR, PR14, 14, 1)
	FIELD(EXTI_PR, PR15, 15, 1)
	FIELD(EXTI_PR, PR16, 16, 1)
	FIELD(EXTI_PR, PR17, 17, 1)
	FIELD(EXTI_PR, PR18, 18, 1)


__attribute__((unused))
static void stm32f1_exti_update(STM32F1EXTI *t) {
	int conditions = ((t->PR & 0x1000000) || (t->PR & 0x2000000) || (t->PR & 0x4000000) || (t->PR & 0x8000000) || (t->PR & 0x10000000) || (t->PR & 0x20000000) || (t->PR & 0x40000000) || (t->PR & 0x80000000) || (t->PR & 0x10000) || (t->PR & 0x20000) || (t->PR & 0x40000) || (t->PR & 0x80000) || (t->PR & 0x100000) || (t->PR & 0x200000) || (t->PR & 0x400000) || (t->PR & 0x800000) || (t->PR & 0x100) || (t->PR & 0x200) || (t->PR & 0x400) || (t->PR & 0x800) || (t->PR & 0x1000) || (t->PR & 0x2000) || (t->PR & 0x4000) || (t->PR & 0x8000) || (t->PR & 0x1) || (t->PR & 0x2) || (t->PR & 0x4) || (t->PR & 0x8) || (t->PR & 0x10) || (t->PR & 0x20) || (t->PR & 0x40) || (t->PR & 0x80));
	qemu_set_irq(t->irq[0], conditions);
	qemu_set_irq(t->irq[1], conditions);
	qemu_set_irq(t->irq[2], conditions);
	qemu_set_irq(t->irq[3], conditions);
	qemu_set_irq(t->irq[4], conditions);
	qemu_set_irq(t->irq[5], conditions);
	qemu_set_irq(t->irq[6], conditions);
	qemu_set_irq(t->irq[7], conditions);
	qemu_set_irq(t->irq[0], 0);
	qemu_set_irq(t->irq[1], 0);
	qemu_set_irq(t->irq[2], 0);
	qemu_set_irq(t->irq[3], 0);
	qemu_set_irq(t->irq[4], 0);
	qemu_set_irq(t->irq[5], 0);
	qemu_set_irq(t->irq[6], 0);
	qemu_set_irq(t->irq[7], 0);

}

static int stm32f1_exti_can_receive(void *opaque) {
	return 1;
}

static void stm32f1_exti_receive(void *opaque, const uint8_t *buf, int size) {
	STM32F1EXTI *t = STM32F1_EXTI(opaque);

	t->PR = (1 << (*buf % 32));

	stm32f1_exti_update(t);
}

static void stm32f1_exti_register_reset(STM32F1EXTI *t) {
	t->IMR = 0x0;
	t->EMR = 0x0;
	t->RTSR = 0x0;
	t->FTSR = 0x0;
	t->SWIER = 0x0;
	t->PR = 0x0;

}

static uint64_t stm32f1_exti_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F1EXTI *t = STM32F1_EXTI(opaque);
	uint64_t ret;

	switch (offset) {
		case A_EXTI_IMR:
			ret = t->IMR;
			break;
		case A_EXTI_EMR:
			ret = t->EMR;
			break;
		case A_EXTI_RTSR:
			ret = t->RTSR;
			break;
		case A_EXTI_FTSR:
			ret = t->FTSR;
			break;
		case A_EXTI_SWIER:
			ret = t->SWIER;
			break;
		case A_EXTI_PR:
			ret = t->PR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F1 EXTI read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f1_exti_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F1EXTI *t = STM32F1_EXTI(opaque);


	switch (offset) {
		case A_EXTI_IMR ... A_EXTI_IMR + 3:
			value = value << ((offset - A_EXTI_IMR) << 3);
			offset = A_EXTI_IMR;
			break;
		case A_EXTI_EMR ... A_EXTI_EMR + 3:
			value = value << ((offset - A_EXTI_EMR) << 3);
			offset = A_EXTI_EMR;
			break;
		case A_EXTI_RTSR ... A_EXTI_RTSR + 3:
			value = value << ((offset - A_EXTI_RTSR) << 3);
			offset = A_EXTI_RTSR;
			break;
		case A_EXTI_FTSR ... A_EXTI_FTSR + 3:
			value = value << ((offset - A_EXTI_FTSR) << 3);
			offset = A_EXTI_FTSR;
			break;
		case A_EXTI_SWIER ... A_EXTI_SWIER + 3:
			value = value << ((offset - A_EXTI_SWIER) << 3);
			offset = A_EXTI_SWIER;
			break;
		case A_EXTI_PR ... A_EXTI_PR + 3:
			value = value << ((offset - A_EXTI_PR) << 3);
			offset = A_EXTI_PR;
			break;

		default: break;
	}

	switch (offset) {
		case A_EXTI_IMR:
			t->IMR = value;
			break;
		case A_EXTI_EMR:
			t->EMR = value;
			break;
		case A_EXTI_RTSR:
			t->RTSR = value;
			break;
		case A_EXTI_FTSR:
			t->FTSR = value;
			break;
		case A_EXTI_SWIER:
			t->SWIER = value;
			break;
		case A_EXTI_PR:
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F1 EXTI write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f1_exti_ops = {
	.read = stm32f1_exti_read,
	.write = stm32f1_exti_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f1_exti_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F1EXTI *t = STM32F1_EXTI(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f1_exti_ops, t, "stm32f1-exti", STM32F1_EXTI_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 8; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32f1_exti_realize(DeviceState *dev, Error **errp) {
	STM32F1EXTI *t = STM32F1_EXTI(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), stm32f1_exti_can_receive, stm32f1_exti_receive,
		NULL, NULL, t, NULL, true);

}

static void stm32f1_exti_reset_enter(Object *obj, ResetType type) {
	STM32F1EXTI *t = STM32F1_EXTI(obj);
	stm32f1_exti_register_reset(t);
}

static const VMStateDescription stm32f1_exti_vmstate = {
	.name = "stm32f1-exti",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(IMR, STM32F1EXTI),
		VMSTATE_UINT32(EMR, STM32F1EXTI),
		VMSTATE_UINT32(RTSR, STM32F1EXTI),
		VMSTATE_UINT32(FTSR, STM32F1EXTI),
		VMSTATE_UINT32(SWIER, STM32F1EXTI),
		VMSTATE_UINT32(PR, STM32F1EXTI),

		VMSTATE_END_OF_LIST()
	}
};

static Property stm32f1_exti_properties[] = {
	DEFINE_PROP_CHR("chardev", STM32F1EXTI, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void stm32f1_exti_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f1_exti_vmstate;
	dc->realize = stm32f1_exti_realize;
	rc->phases.enter = stm32f1_exti_reset_enter;
	device_class_set_props(dc, stm32f1_exti_properties);

}

static const TypeInfo stm32f1_exti_info = {
	.name = TYPE_STM32F1_EXTI,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F1EXTI),
	.instance_init = stm32f1_exti_init,
	.class_init = stm32f1_exti_class_init,
};

static void stm32f1_exti_register_types(void) {
	type_register_static(&stm32f1_exti_info);
}

type_init(stm32f1_exti_register_types);

#define TYPE_STM32F1_USART "stm32f1-usart"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F1USART, STM32F1_USART)

struct STM32F1USART {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t SR;
	uint32_t DR;
	uint32_t BRR;
	uint32_t CR1;
	uint32_t CR2;
	uint32_t CR3;
	uint32_t GTPR;
	/* chardev backend */
	CharBackend chr;
	guint watch_tag;

	/* dma */
	STM32F1DMA *dma;
	/* base */
	uint32_t base;

};
#define STM32F1_USART_SIZE				0x1c

REG32(USART_SR, 0x0)
	FIELD(USART_SR, CTS, 9, 1)
	FIELD(USART_SR, LBD, 8, 1)
	FIELD(USART_SR, TXE, 7, 1)
	FIELD(USART_SR, TC, 6, 1)
	FIELD(USART_SR, RXNE, 5, 1)
	FIELD(USART_SR, IDLE, 4, 1)
	FIELD(USART_SR, ORE, 3, 1)
	FIELD(USART_SR, NE, 2, 1)
	FIELD(USART_SR, FE, 1, 1)
	FIELD(USART_SR, PE, 0, 1)
REG32(USART_DR, 0x4)
	FIELD(USART_DR, DR, 0, 9)
REG32(USART_BRR, 0x8)
	FIELD(USART_BRR, DIV_Mantissa, 4, 12)
	FIELD(USART_BRR, DIV_Fraction, 0, 4)
REG32(USART_CR1, 0xc)
	FIELD(USART_CR1, UE, 13, 1)
	FIELD(USART_CR1, M, 12, 1)
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
	FIELD(USART_CR1, RWU, 1, 1)
	FIELD(USART_CR1, SBK, 0, 1)
REG32(USART_CR2, 0x10)
	FIELD(USART_CR2, LINEN, 14, 1)
	FIELD(USART_CR2, STOP, 12, 2)
	FIELD(USART_CR2, CLKEN, 11, 1)
	FIELD(USART_CR2, CPOL, 10, 1)
	FIELD(USART_CR2, CPHA, 9, 1)
	FIELD(USART_CR2, LBCL, 8, 1)
	FIELD(USART_CR2, LBDIE, 6, 1)
	FIELD(USART_CR2, LBDL, 5, 1)
	FIELD(USART_CR2, ADD, 0, 4)
REG32(USART_CR3, 0x14)
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
REG32(USART_GTPR, 0x18)
	FIELD(USART_GTPR, GT, 8, 8)
	FIELD(USART_GTPR, PSC, 0, 8)


__attribute__((unused))
static void stm32f1_usart_update(STM32F1USART *t) {
	int conditions = (((t->CR1 & 0x20) && (t->SR & 0x20)) || ((!(t->CR3 & 0x1)) && (!(t->CR1 & 0x100)) && (!(t->CR1 & 0x20)) && (t->SR & 0x80) && (t->CR1 & 0x80)) || ((!(t->CR3 & 0x1)) && (!(t->CR1 & 0x100)) && (!(t->CR1 & 0x20)) && (t->SR & 0x40) && (t->CR1 & 0x40)) || ((!(t->SR & 0x4)) && (!(t->SR & 0x8)) && (!(t->SR & 0x2)) && (!(t->SR & 0x1)) && (t->SR & 0x40) && (t->CR1 & 0x40)) || ((!(t->CR3 & 0x1)) && (!(t->CR1 & 0x100)) && (!(t->SR & 0x10)) && (!(t->CR1 & 0x20)) && (t->SR & 0x80) && (t->CR1 & 0x80)) || ((!(t->SR & 0x4)) && (!(t->SR & 0x8)) && (!(t->SR & 0x2)) && (!(t->SR & 0x1)) && (t->SR & 0x80) && (t->CR1 & 0x80)) || ((!(t->CR3 & 0x1)) && (!(t->CR1 & 0x100)) && (!(t->SR & 0x10)) && (!(t->CR1 & 0x20)) && (t->SR & 0x40) && (t->CR1 & 0x40)) || ((t->SR & 0x1) && (!(t->CR3 & 0x40)) && (t->CR1 & 0x100)) || ((!(t->CR3 & 0x40)) && (t->CR1 & 0x100) && (t->SR & 0x2)) || ((!(t->CR3 & 0x40)) && (t->CR1 & 0x100) && (t->SR & 0x4)) || ((t->SR & 0x1) && (!(t->CR3 & 0x40)) && (t->CR3 & 0x1)) || ((!(t->CR3 & 0x40)) && (t->SR & 0x8) && (t->CR3 & 0x1)) || ((!(t->CR3 & 0x40)) && (t->SR & 0x8) && (t->CR1 & 0x20)) || ((!(t->CR3 & 0x40)) && (t->SR & 0x2) && (t->CR1 & 0x20)) || ((!(t->CR3 & 0x40)) && (t->SR & 0x4) && (t->CR1 & 0x20)) || ((t->SR & 0x1) && (!(t->CR3 & 0x40)) && (t->CR1 & 0x20)) || ((!(t->CR1 & 0x10)) && (!(t->SR & 0x4)) && (!(t->SR & 0x8)) && (!(t->SR & 0x2)) && (!(t->SR & 0x1)) && (t->SR & 0x80) && (t->CR1 & 0x80)) || ((!(t->CR3 & 0x1)) && (!(t->CR1 & 0x100)) && (!(t->CR1 & 0x10)) && (!(t->CR1 & 0x20)) && (t->SR & 0x40) && (t->CR1 & 0x40)) || ((!(t->CR3 & 0x40)) && (t->CR3 & 0x1) && (t->SR & 0x2)) || ((!(t->SR & 0x4)) && (!(t->SR & 0x8)) && (!(t->SR & 0x10)) && (!(t->SR & 0x2)) && (!(t->SR & 0x1)) && (t->SR & 0x40) && (t->CR1 & 0x40)) || ((!(t->CR1 & 0x10)) && (!(t->SR & 0x4)) && (!(t->SR & 0x8)) && (!(t->SR & 0x2)) && (!(t->SR & 0x1)) && (t->SR & 0x40) && (t->CR1 & 0x40)) || ((!(t->CR3 & 0x40)) && (t->SR & 0x8) && (t->CR1 & 0x100)) || ((!(t->CR3 & 0x1)) && (!(t->CR1 & 0x100)) && (!(t->CR1 & 0x10)) && (!(t->CR1 & 0x20)) && (t->SR & 0x80) && (t->CR1 & 0x80)) || ((t->SR & 0x10) && (!(t->CR3 & 0x40)) && (t->CR1 & 0x10)) || ((!(t->CR3 & 0x40)) && (t->CR3 & 0x1) && (t->SR & 0x4)) || ((!(t->SR & 0x4)) && (!(t->SR & 0x8)) && (!(t->SR & 0x10)) && (!(t->SR & 0x2)) && (!(t->SR & 0x1)) && (t->SR & 0x80) && (t->CR1 & 0x80)));
	qemu_set_irq(t->irq[0], conditions);

}

static int stm32f1_usart_can_receive(void *opaque) {
	return 1;
}

static void stm32f1_usart_receive(void *opaque, const uint8_t *buf, int size) {
	STM32F1USART *t = STM32F1_USART(opaque);


	if ((t->CR3 & 0x40) && t->dma) {
		stm32f1_dma_receive(t->dma, t->base + 4, *buf);
	}
	t->DR = *buf;
	t->SR |= 0x20;

	stm32f1_usart_update(t);
}

static gboolean stm32f1_usart_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	STM32F1USART *t = STM32F1_USART(opaque);
	int ret;

	t->watch_tag = 0;
	t->SR &= (~(0x40));
	t->SR &= (~(0x80));

	stm32f1_usart_update(t);

	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->DR), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, stm32f1_usart_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->SR |= 0x80;
	t->SR |= 0x40;

	stm32f1_usart_update(t);

	return FALSE;
}

static void stm32f1_usart_register_reset(STM32F1USART *t) {
	t->SR = 0xc0;
	t->DR = 0x0;
	t->BRR = 0x0;
	t->CR1 = 0x0;
	t->CR2 = 0x0;
	t->CR3 = 0x0;
	t->GTPR = 0x0;
	t->SR |= 0x80;
	t->SR |= 0x40;

}

static uint64_t stm32f1_usart_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F1USART *t = STM32F1_USART(opaque);
	uint64_t ret;

	switch (offset) {
		case A_USART_SR:
			ret = t->SR;
			break;
		case A_USART_DR:
			ret = t->DR;
			t->SR &= (~(0x20));
			qemu_chr_fe_accept_input(&(t->chr));
			stm32f1_usart_update(t);
			break;
		case A_USART_BRR:
			ret = t->BRR;
			break;
		case A_USART_CR1:
			ret = t->CR1;
			break;
		case A_USART_CR2:
			ret = t->CR2;
			break;
		case A_USART_CR3:
			ret = t->CR3;
			break;
		case A_USART_GTPR:
			ret = t->GTPR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F1 USART read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f1_usart_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F1USART *t = STM32F1_USART(opaque);


	switch (offset) {
		case A_USART_SR ... A_USART_SR + 3:
			value = value << ((offset - A_USART_SR) << 3);
			offset = A_USART_SR;
			break;
		case A_USART_DR ... A_USART_DR + 3:
			value = value << ((offset - A_USART_DR) << 3);
			offset = A_USART_DR;
			break;
		case A_USART_BRR ... A_USART_BRR + 3:
			value = value << ((offset - A_USART_BRR) << 3);
			offset = A_USART_BRR;
			break;
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
		case A_USART_GTPR ... A_USART_GTPR + 3:
			value = value << ((offset - A_USART_GTPR) << 3);
			offset = A_USART_GTPR;
			break;

		default: break;
	}

	switch (offset) {
		case A_USART_SR:
			t->SR &= value;
			t->SR |= 0x80;
			t->SR |= 0x40;
			stm32f1_usart_update(t);
			break;
		case A_USART_DR:
			t->DR = value;
			stm32f1_usart_transmit(NULL, G_IO_OUT, t);
			break;
		case A_USART_BRR:
			t->BRR = value;
			break;
		case A_USART_CR1:
			t->CR1 = value;
			stm32f1_usart_update(t);
			break;
		case A_USART_CR2:
			t->CR2 = value;
			break;
		case A_USART_CR3:
			t->CR3 = value;
			stm32f1_usart_update(t);
			break;
		case A_USART_GTPR:
			t->GTPR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F1 USART write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f1_usart_ops = {
	.read = stm32f1_usart_read,
	.write = stm32f1_usart_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f1_usart_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F1USART *t = STM32F1_USART(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f1_usart_ops, t, "stm32f1-usart", STM32F1_USART_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32f1_usart_realize(DeviceState *dev, Error **errp) {
	STM32F1USART *t = STM32F1_USART(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), stm32f1_usart_can_receive, stm32f1_usart_receive,
		NULL, NULL, t, NULL, true);

}

static void stm32f1_usart_reset_enter(Object *obj, ResetType type) {
	STM32F1USART *t = STM32F1_USART(obj);
	stm32f1_usart_register_reset(t);
}

static const VMStateDescription stm32f1_usart_vmstate = {
	.name = "stm32f1-usart",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(SR, STM32F1USART),
		VMSTATE_UINT32(DR, STM32F1USART),
		VMSTATE_UINT32(BRR, STM32F1USART),
		VMSTATE_UINT32(CR1, STM32F1USART),
		VMSTATE_UINT32(CR2, STM32F1USART),
		VMSTATE_UINT32(CR3, STM32F1USART),
		VMSTATE_UINT32(GTPR, STM32F1USART),

		VMSTATE_END_OF_LIST()
	}
};

static Property stm32f1_usart_properties[] = {
	DEFINE_PROP_CHR("chardev", STM32F1USART, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void stm32f1_usart_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f1_usart_vmstate;
	dc->realize = stm32f1_usart_realize;
	rc->phases.enter = stm32f1_usart_reset_enter;
	device_class_set_props(dc, stm32f1_usart_properties);

}

static const TypeInfo stm32f1_usart_info = {
	.name = TYPE_STM32F1_USART,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F1USART),
	.instance_init = stm32f1_usart_init,
	.class_init = stm32f1_usart_class_init,
};

static void stm32f1_usart_register_types(void) {
	type_register_static(&stm32f1_usart_info);
}

type_init(stm32f1_usart_register_types);

#define TYPE_STM32F1_GPIOA "stm32f1-gpioa"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F1GPIOA, STM32F1_GPIOA)

struct STM32F1GPIOA {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/*registers*/
	uint32_t CRL;
	uint32_t CRH;
	uint32_t IDR;
	uint32_t ODR;
	uint32_t BSRR;
	uint32_t BRR;
	uint32_t LCKR;
	/* base */
	uint32_t base;

};
#define STM32F1_GPIOA_SIZE				0x1c

REG32(GPIOA_CRL, 0x0)
	FIELD(GPIOA_CRL, MODE0, 0, 2)
	FIELD(GPIOA_CRL, CNF0, 2, 2)
	FIELD(GPIOA_CRL, MODE1, 4, 2)
	FIELD(GPIOA_CRL, CNF1, 6, 2)
	FIELD(GPIOA_CRL, MODE2, 8, 2)
	FIELD(GPIOA_CRL, CNF2, 10, 2)
	FIELD(GPIOA_CRL, MODE3, 12, 2)
	FIELD(GPIOA_CRL, CNF3, 14, 2)
	FIELD(GPIOA_CRL, MODE4, 16, 2)
	FIELD(GPIOA_CRL, CNF4, 18, 2)
	FIELD(GPIOA_CRL, MODE5, 20, 2)
	FIELD(GPIOA_CRL, CNF5, 22, 2)
	FIELD(GPIOA_CRL, MODE6, 24, 2)
	FIELD(GPIOA_CRL, CNF6, 26, 2)
	FIELD(GPIOA_CRL, MODE7, 28, 2)
	FIELD(GPIOA_CRL, CNF7, 30, 2)
REG32(GPIOA_CRH, 0x4)
	FIELD(GPIOA_CRH, MODE8, 0, 2)
	FIELD(GPIOA_CRH, CNF8, 2, 2)
	FIELD(GPIOA_CRH, MODE9, 4, 2)
	FIELD(GPIOA_CRH, CNF9, 6, 2)
	FIELD(GPIOA_CRH, MODE10, 8, 2)
	FIELD(GPIOA_CRH, CNF10, 10, 2)
	FIELD(GPIOA_CRH, MODE11, 12, 2)
	FIELD(GPIOA_CRH, CNF11, 14, 2)
	FIELD(GPIOA_CRH, MODE12, 16, 2)
	FIELD(GPIOA_CRH, CNF12, 18, 2)
	FIELD(GPIOA_CRH, MODE13, 20, 2)
	FIELD(GPIOA_CRH, CNF13, 22, 2)
	FIELD(GPIOA_CRH, MODE14, 24, 2)
	FIELD(GPIOA_CRH, CNF14, 26, 2)
	FIELD(GPIOA_CRH, MODE15, 28, 2)
	FIELD(GPIOA_CRH, CNF15, 30, 2)
REG32(GPIOA_IDR, 0x8)
	FIELD(GPIOA_IDR, IDR0, 0, 1)
	FIELD(GPIOA_IDR, IDR1, 1, 1)
	FIELD(GPIOA_IDR, IDR2, 2, 1)
	FIELD(GPIOA_IDR, IDR3, 3, 1)
	FIELD(GPIOA_IDR, IDR4, 4, 1)
	FIELD(GPIOA_IDR, IDR5, 5, 1)
	FIELD(GPIOA_IDR, IDR6, 6, 1)
	FIELD(GPIOA_IDR, IDR7, 7, 1)
	FIELD(GPIOA_IDR, IDR8, 8, 1)
	FIELD(GPIOA_IDR, IDR9, 9, 1)
	FIELD(GPIOA_IDR, IDR10, 10, 1)
	FIELD(GPIOA_IDR, IDR11, 11, 1)
	FIELD(GPIOA_IDR, IDR12, 12, 1)
	FIELD(GPIOA_IDR, IDR13, 13, 1)
	FIELD(GPIOA_IDR, IDR14, 14, 1)
	FIELD(GPIOA_IDR, IDR15, 15, 1)
REG32(GPIOA_ODR, 0xc)
	FIELD(GPIOA_ODR, ODR0, 0, 1)
	FIELD(GPIOA_ODR, ODR1, 1, 1)
	FIELD(GPIOA_ODR, ODR2, 2, 1)
	FIELD(GPIOA_ODR, ODR3, 3, 1)
	FIELD(GPIOA_ODR, ODR4, 4, 1)
	FIELD(GPIOA_ODR, ODR5, 5, 1)
	FIELD(GPIOA_ODR, ODR6, 6, 1)
	FIELD(GPIOA_ODR, ODR7, 7, 1)
	FIELD(GPIOA_ODR, ODR8, 8, 1)
	FIELD(GPIOA_ODR, ODR9, 9, 1)
	FIELD(GPIOA_ODR, ODR10, 10, 1)
	FIELD(GPIOA_ODR, ODR11, 11, 1)
	FIELD(GPIOA_ODR, ODR12, 12, 1)
	FIELD(GPIOA_ODR, ODR13, 13, 1)
	FIELD(GPIOA_ODR, ODR14, 14, 1)
	FIELD(GPIOA_ODR, ODR15, 15, 1)
REG32(GPIOA_BSRR, 0x10)
	FIELD(GPIOA_BSRR, BS0, 0, 1)
	FIELD(GPIOA_BSRR, BS1, 1, 1)
	FIELD(GPIOA_BSRR, BS2, 2, 1)
	FIELD(GPIOA_BSRR, BS3, 3, 1)
	FIELD(GPIOA_BSRR, BS4, 4, 1)
	FIELD(GPIOA_BSRR, BS5, 5, 1)
	FIELD(GPIOA_BSRR, BS6, 6, 1)
	FIELD(GPIOA_BSRR, BS7, 7, 1)
	FIELD(GPIOA_BSRR, BS8, 8, 1)
	FIELD(GPIOA_BSRR, BS9, 9, 1)
	FIELD(GPIOA_BSRR, BS10, 10, 1)
	FIELD(GPIOA_BSRR, BS11, 11, 1)
	FIELD(GPIOA_BSRR, BS12, 12, 1)
	FIELD(GPIOA_BSRR, BS13, 13, 1)
	FIELD(GPIOA_BSRR, BS14, 14, 1)
	FIELD(GPIOA_BSRR, BS15, 15, 1)
	FIELD(GPIOA_BSRR, BR0, 16, 1)
	FIELD(GPIOA_BSRR, BR1, 17, 1)
	FIELD(GPIOA_BSRR, BR2, 18, 1)
	FIELD(GPIOA_BSRR, BR3, 19, 1)
	FIELD(GPIOA_BSRR, BR4, 20, 1)
	FIELD(GPIOA_BSRR, BR5, 21, 1)
	FIELD(GPIOA_BSRR, BR6, 22, 1)
	FIELD(GPIOA_BSRR, BR7, 23, 1)
	FIELD(GPIOA_BSRR, BR8, 24, 1)
	FIELD(GPIOA_BSRR, BR9, 25, 1)
	FIELD(GPIOA_BSRR, BR10, 26, 1)
	FIELD(GPIOA_BSRR, BR11, 27, 1)
	FIELD(GPIOA_BSRR, BR12, 28, 1)
	FIELD(GPIOA_BSRR, BR13, 29, 1)
	FIELD(GPIOA_BSRR, BR14, 30, 1)
	FIELD(GPIOA_BSRR, BR15, 31, 1)
REG32(GPIOA_BRR, 0x14)
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
REG32(GPIOA_LCKR, 0x18)
	FIELD(GPIOA_LCKR, LCK0, 0, 1)
	FIELD(GPIOA_LCKR, LCK1, 1, 1)
	FIELD(GPIOA_LCKR, LCK2, 2, 1)
	FIELD(GPIOA_LCKR, LCK3, 3, 1)
	FIELD(GPIOA_LCKR, LCK4, 4, 1)
	FIELD(GPIOA_LCKR, LCK5, 5, 1)
	FIELD(GPIOA_LCKR, LCK6, 6, 1)
	FIELD(GPIOA_LCKR, LCK7, 7, 1)
	FIELD(GPIOA_LCKR, LCK8, 8, 1)
	FIELD(GPIOA_LCKR, LCK9, 9, 1)
	FIELD(GPIOA_LCKR, LCK10, 10, 1)
	FIELD(GPIOA_LCKR, LCK11, 11, 1)
	FIELD(GPIOA_LCKR, LCK12, 12, 1)
	FIELD(GPIOA_LCKR, LCK13, 13, 1)
	FIELD(GPIOA_LCKR, LCK14, 14, 1)
	FIELD(GPIOA_LCKR, LCK15, 15, 1)
	FIELD(GPIOA_LCKR, LCKK, 16, 1)


static void stm32f1_gpioa_register_reset(STM32F1GPIOA *t) {
	t->CRL = 0x44444444;
	t->CRH = 0x44444444;
	t->IDR = 0x0;
	t->ODR = 0x0;
	t->BSRR = 0x0;
	t->BRR = 0x0;
	t->LCKR = 0x0;

}

static uint64_t stm32f1_gpioa_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F1GPIOA *t = STM32F1_GPIOA(opaque);
	uint64_t ret;

	switch (offset) {
		case A_GPIOA_CRL:
			ret = t->CRL;
			break;
		case A_GPIOA_CRH:
			ret = t->CRH;
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

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F1 GPIOA read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f1_gpioa_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F1GPIOA *t = STM32F1_GPIOA(opaque);


	switch (offset) {
		case A_GPIOA_CRL ... A_GPIOA_CRL + 3:
			value = value << ((offset - A_GPIOA_CRL) << 3);
			offset = A_GPIOA_CRL;
			break;
		case A_GPIOA_CRH ... A_GPIOA_CRH + 3:
			value = value << ((offset - A_GPIOA_CRH) << 3);
			offset = A_GPIOA_CRH;
			break;
		case A_GPIOA_ODR ... A_GPIOA_ODR + 3:
			value = value << ((offset - A_GPIOA_ODR) << 3);
			offset = A_GPIOA_ODR;
			break;
		case A_GPIOA_BSRR ... A_GPIOA_BSRR + 3:
			value = value << ((offset - A_GPIOA_BSRR) << 3);
			offset = A_GPIOA_BSRR;
			break;
		case A_GPIOA_BRR ... A_GPIOA_BRR + 3:
			value = value << ((offset - A_GPIOA_BRR) << 3);
			offset = A_GPIOA_BRR;
			break;
		case A_GPIOA_LCKR ... A_GPIOA_LCKR + 3:
			value = value << ((offset - A_GPIOA_LCKR) << 3);
			offset = A_GPIOA_LCKR;
			break;

		default: break;
	}

	switch (offset) {
		case A_GPIOA_CRL:
			t->CRL = value;
			break;
		case A_GPIOA_CRH:
			t->CRH = value;
			break;
		case A_GPIOA_ODR:
			t->ODR = value;
			break;
		case A_GPIOA_BSRR:
			t->BSRR = value;
			break;
		case A_GPIOA_BRR:
			t->BRR = value;
			break;
		case A_GPIOA_LCKR:
			t->LCKR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F1 GPIOA write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f1_gpioa_ops = {
	.read = stm32f1_gpioa_read,
	.write = stm32f1_gpioa_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f1_gpioa_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F1GPIOA *t = STM32F1_GPIOA(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f1_gpioa_ops, t, "stm32f1-gpioa", STM32F1_GPIOA_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void stm32f1_gpioa_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32f1_gpioa_reset_enter(Object *obj, ResetType type) {
	STM32F1GPIOA *t = STM32F1_GPIOA(obj);
	stm32f1_gpioa_register_reset(t);
}

static const VMStateDescription stm32f1_gpioa_vmstate = {
	.name = "stm32f1-gpioa",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CRL, STM32F1GPIOA),
		VMSTATE_UINT32(CRH, STM32F1GPIOA),
		VMSTATE_UINT32(IDR, STM32F1GPIOA),
		VMSTATE_UINT32(ODR, STM32F1GPIOA),
		VMSTATE_UINT32(BSRR, STM32F1GPIOA),
		VMSTATE_UINT32(BRR, STM32F1GPIOA),
		VMSTATE_UINT32(LCKR, STM32F1GPIOA),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f1_gpioa_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f1_gpioa_vmstate;
	dc->realize = stm32f1_gpioa_realize;
	rc->phases.enter = stm32f1_gpioa_reset_enter;

}

static const TypeInfo stm32f1_gpioa_info = {
	.name = TYPE_STM32F1_GPIOA,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F1GPIOA),
	.instance_init = stm32f1_gpioa_init,
	.class_init = stm32f1_gpioa_class_init,
};

static void stm32f1_gpioa_register_types(void) {
	type_register_static(&stm32f1_gpioa_info);
}

type_init(stm32f1_gpioa_register_types);

#define TYPE_STM32F1_SPI "stm32f1-spi"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F1SPI, STM32F1_SPI)

struct STM32F1SPI {
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

	/* dma */
	STM32F1DMA *dma;
	/* base */
	uint32_t base;

};
#define STM32F1_SPI_SIZE				0x24

REG32(SPI_CR1, 0x0)
	FIELD(SPI_CR1, BIDIMODE, 15, 1)
	FIELD(SPI_CR1, BIDIOE, 14, 1)
	FIELD(SPI_CR1, CRCEN, 13, 1)
	FIELD(SPI_CR1, CRCNEXT, 12, 1)
	FIELD(SPI_CR1, DFF, 11, 1)
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
	FIELD(SPI_CR2, TXEIE, 7, 1)
	FIELD(SPI_CR2, RXNEIE, 6, 1)
	FIELD(SPI_CR2, ERRIE, 5, 1)
	FIELD(SPI_CR2, SSOE, 2, 1)
	FIELD(SPI_CR2, TXDMAEN, 1, 1)
	FIELD(SPI_CR2, RXDMAEN, 0, 1)
REG32(SPI_SR, 0x8)
	FIELD(SPI_SR, BSY, 7, 1)
	FIELD(SPI_SR, OVR, 6, 1)
	FIELD(SPI_SR, MODF, 5, 1)
	FIELD(SPI_SR, CRCERR, 4, 1)
	FIELD(SPI_SR, UDR, 3, 1)
	FIELD(SPI_SR, CHSIDE, 2, 1)
	FIELD(SPI_SR, TXE, 1, 1)
	FIELD(SPI_SR, RXNE, 0, 1)
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
REG32(SPI_I2SPR, 0x20)
	FIELD(SPI_I2SPR, MCKOE, 9, 1)
	FIELD(SPI_I2SPR, ODD, 8, 1)
	FIELD(SPI_I2SPR, I2SDIV, 0, 8)


__attribute__((unused))
static void stm32f1_spi_update(STM32F1SPI *t) {
	int conditions = (((t->SR & 0x2) && (t->CR2 & 0x80)) || ((!(t->CR2 & 0x2)) && (t->SR & 0x40) && (!(t->CR2 & 0x1)) && (t->CR2 & 0x20)) || ((!(t->CR2 & 0x2)) && (t->SR & 0x20) && (!(t->CR2 & 0x1)) && (t->CR2 & 0x20)) || ((!(t->SR & 0x40)) && (t->CR2 & 0x40)));
	qemu_set_irq(t->irq[0], conditions);

}

static int stm32f1_spi_can_receive(void *opaque) {
	return 1;
}

static void stm32f1_spi_receive(void *opaque, const uint8_t *buf, int size) {
	STM32F1SPI *t = STM32F1_SPI(opaque);


	if ((t->CR2 & 0x1) && t->dma) {
		stm32f1_dma_receive(t->dma, t->base + 12, *buf);
	}
	t->DR = *buf;
	t->SR |= 0x1;

	stm32f1_spi_update(t);
}

static gboolean stm32f1_spi_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	STM32F1SPI *t = STM32F1_SPI(opaque);
	int ret;

	t->watch_tag = 0;
	t->SR &= (~(0x2));
	t->SR |= 0x80;

	stm32f1_spi_update(t);

	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->DR), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, stm32f1_spi_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->SR |= 0x2;
	t->SR &= (~(0x80));

	stm32f1_spi_update(t);

	return FALSE;
}

static void stm32f1_spi_register_reset(STM32F1SPI *t) {
	t->CR1 = 0x0;
	t->CR2 = 0x0;
	t->SR = 0x2;
	t->DR = 0x0;
	t->CRCPR = 0x7;
	t->RXCRCR = 0x0;
	t->TXCRCR = 0x0;
	t->I2SCFGR = 0x0;
	t->I2SPR = 0xa;
	t->SR |= 0x2;
	t->SR &= (~(0x80));

}

static uint64_t stm32f1_spi_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F1SPI *t = STM32F1_SPI(opaque);
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
			stm32f1_spi_update(t);
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
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F1 SPI read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f1_spi_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F1SPI *t = STM32F1_SPI(opaque);


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
				t->SR &= (~(0x80));
				t->SR &= (~(0x1));
			}
			t->SR &= (~(0x80));
			t->SR &= (~(0x1));
			stm32f1_spi_update(t);
			break;
		case A_SPI_CR2:
			t->CR2 = value;
			if (((!(value & 0x20)) && (!(value & 0x40)))) {
				t->SR &= (~(0x80));
			} else if (((!(value & 0x20)) && (!(value & 0x80)))) {
				t->SR &= (~(0x80));
			} else if ((!(value & 0x20))) {
				t->SR |= 0x2;
				t->SR &= (~(0x80));
			}
			stm32f1_spi_update(t);
			break;
		case A_SPI_SR:
			t->SR &= value;
			t->SR |= 0x2;
			t->SR &= (~(0x80));
			stm32f1_spi_update(t);
			break;
		case A_SPI_DR:
			t->DR = value;
			stm32f1_spi_transmit(NULL, G_IO_OUT, t);
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
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F1 SPI write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f1_spi_ops = {
	.read = stm32f1_spi_read,
	.write = stm32f1_spi_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f1_spi_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F1SPI *t = STM32F1_SPI(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f1_spi_ops, t, "stm32f1-spi", STM32F1_SPI_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32f1_spi_realize(DeviceState *dev, Error **errp) {
	STM32F1SPI *t = STM32F1_SPI(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), stm32f1_spi_can_receive, stm32f1_spi_receive,
		NULL, NULL, t, NULL, true);

}

static void stm32f1_spi_reset_enter(Object *obj, ResetType type) {
	STM32F1SPI *t = STM32F1_SPI(obj);
	stm32f1_spi_register_reset(t);
}

static const VMStateDescription stm32f1_spi_vmstate = {
	.name = "stm32f1-spi",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR1, STM32F1SPI),
		VMSTATE_UINT32(CR2, STM32F1SPI),
		VMSTATE_UINT32(SR, STM32F1SPI),
		VMSTATE_UINT32(DR, STM32F1SPI),
		VMSTATE_UINT32(CRCPR, STM32F1SPI),
		VMSTATE_UINT32(RXCRCR, STM32F1SPI),
		VMSTATE_UINT32(TXCRCR, STM32F1SPI),
		VMSTATE_UINT32(I2SCFGR, STM32F1SPI),
		VMSTATE_UINT32(I2SPR, STM32F1SPI),

		VMSTATE_END_OF_LIST()
	}
};

static Property stm32f1_spi_properties[] = {
	DEFINE_PROP_CHR("chardev", STM32F1SPI, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void stm32f1_spi_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f1_spi_vmstate;
	dc->realize = stm32f1_spi_realize;
	rc->phases.enter = stm32f1_spi_reset_enter;
	device_class_set_props(dc, stm32f1_spi_properties);

}

static const TypeInfo stm32f1_spi_info = {
	.name = TYPE_STM32F1_SPI,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F1SPI),
	.instance_init = stm32f1_spi_init,
	.class_init = stm32f1_spi_class_init,
};

static void stm32f1_spi_register_types(void) {
	type_register_static(&stm32f1_spi_info);
}

type_init(stm32f1_spi_register_types);

#define TYPE_STM32F1_I2C "stm32f1-i2c"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F1I2C, STM32F1_I2C)

struct STM32F1I2C {
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
	uint32_t DR;
	uint32_t SR1;
	uint32_t SR2;
	uint32_t CCR;
	uint32_t TRISE;
	/* chardev backend */
	CharBackend chr;
	guint watch_tag;

	/* dma */
	STM32F1DMA *dma;
	/* base */
	uint32_t base;

};
#define STM32F1_I2C_SIZE				0x24

REG32(I2C_CR1, 0x0)
	FIELD(I2C_CR1, SWRST, 15, 1)
	FIELD(I2C_CR1, ALERT, 13, 1)
	FIELD(I2C_CR1, PEC, 12, 1)
	FIELD(I2C_CR1, POS, 11, 1)
	FIELD(I2C_CR1, ACK, 10, 1)
	FIELD(I2C_CR1, STOP, 9, 1)
	FIELD(I2C_CR1, START, 8, 1)
	FIELD(I2C_CR1, NOSTRETCH, 7, 1)
	FIELD(I2C_CR1, ENGC, 6, 1)
	FIELD(I2C_CR1, ENPEC, 5, 1)
	FIELD(I2C_CR1, ENARP, 4, 1)
	FIELD(I2C_CR1, SMBTYPE, 3, 1)
	FIELD(I2C_CR1, SMBUS, 1, 1)
	FIELD(I2C_CR1, PE, 0, 1)
REG32(I2C_CR2, 0x4)
	FIELD(I2C_CR2, LAST, 12, 1)
	FIELD(I2C_CR2, DMAEN, 11, 1)
	FIELD(I2C_CR2, ITBUFEN, 10, 1)
	FIELD(I2C_CR2, ITEVTEN, 9, 1)
	FIELD(I2C_CR2, ITERREN, 8, 1)
	FIELD(I2C_CR2, FREQ, 0, 6)
REG32(I2C_OAR1, 0x8)
	FIELD(I2C_OAR1, ADDMODE, 15, 1)
	FIELD(I2C_OAR1, ADD10, 8, 2)
	FIELD(I2C_OAR1, ADD7, 1, 7)
	FIELD(I2C_OAR1, ADD0, 0, 1)
REG32(I2C_OAR2, 0xc)
	FIELD(I2C_OAR2, ADD2, 1, 7)
	FIELD(I2C_OAR2, ENDUAL, 0, 1)
REG32(I2C_DR, 0x10)
	FIELD(I2C_DR, DR, 0, 8)
REG32(I2C_SR1, 0x14)
	FIELD(I2C_SR1, SMBALERT, 15, 1)
	FIELD(I2C_SR1, TIMEOUT, 14, 1)
	FIELD(I2C_SR1, PECERR, 12, 1)
	FIELD(I2C_SR1, OVR, 11, 1)
	FIELD(I2C_SR1, AF, 10, 1)
	FIELD(I2C_SR1, ARLO, 9, 1)
	FIELD(I2C_SR1, BERR, 8, 1)
	FIELD(I2C_SR1, TxE, 7, 1)
	FIELD(I2C_SR1, RxNE, 6, 1)
	FIELD(I2C_SR1, STOPF, 4, 1)
	FIELD(I2C_SR1, ADD10, 3, 1)
	FIELD(I2C_SR1, BTF, 2, 1)
	FIELD(I2C_SR1, ADDR, 1, 1)
	FIELD(I2C_SR1, SB, 0, 1)
REG32(I2C_SR2, 0x18)
	FIELD(I2C_SR2, PEC, 8, 8)
	FIELD(I2C_SR2, DUALF, 7, 1)
	FIELD(I2C_SR2, SMBHOST, 6, 1)
	FIELD(I2C_SR2, SMBDEFAULT, 5, 1)
	FIELD(I2C_SR2, GENCALL, 4, 1)
	FIELD(I2C_SR2, TRA, 2, 1)
	FIELD(I2C_SR2, BUSY, 1, 1)
	FIELD(I2C_SR2, MSL, 0, 1)
REG32(I2C_CCR, 0x1c)
	FIELD(I2C_CCR, F_S, 15, 1)
	FIELD(I2C_CCR, DUTY, 14, 1)
	FIELD(I2C_CCR, CCR, 0, 12)
REG32(I2C_TRISE, 0x20)
	FIELD(I2C_TRISE, TRISE, 0, 6)


__attribute__((unused))
static void stm32f1_i2c_update(STM32F1I2C *t) {
	int conditions = (((!(t->SR1 & 0x4)) && (!(t->CR2 & 0x200)) && (t->CR2 & 0x400) && (t->SR1 & 0x80)) || ((!(t->CR2 & 0x800)) && (t->CR2 & 0x100) && (t->SR1 & 0x200)) || ((!(t->CR2 & 0x800)) && (t->CR2 & 0x100) && (t->SR1 & 0x800)) || ((!(t->CR2 & 0x800)) && (t->SR1 & 0x100) && (t->CR2 & 0x100)) || ((!(t->CR2 & 0x800)) && (t->CR2 & 0x200) && (t->SR1 & 0x10)) || ((t->SR1 & 0x4) && (!(t->SR1 & 0x10)) && (t->CR2 & 0x200)) || ((!(t->SR1 & 0x4)) && (!(t->CR2 & 0x200)) && (t->CR2 & 0x400) && (t->SR1 & 0x40)) || ((t->CR2 & 0x200) && (t->SR1 & 0x1)) || ((!(t->SR1 & 0x4)) && (!(t->SR1 & 0x10)) && (t->CR2 & 0x400) && (t->SR1 & 0x40)) || ((!(t->SR1 & 0x4)) && (!(t->SR1 & 0x10)) && (t->CR2 & 0x400) && (t->SR1 & 0x80)) || ((t->SR1 & 0x400) && (t->CR2 & 0x100)) || ((t->CR2 & 0x200) && (t->SR1 & 0x8)) || ((t->SR1 & 0x2) && (t->CR2 & 0x200)));
	qemu_set_irq(t->irq[0], conditions);

}

static int stm32f1_i2c_can_receive(void *opaque) {
	return 1;
}

static void stm32f1_i2c_receive(void *opaque, const uint8_t *buf, int size) {
	STM32F1I2C *t = STM32F1_I2C(opaque);


	if ((t->CR2 & 0x800) && t->dma) {
		stm32f1_dma_receive(t->dma, t->base + 16, *buf);
	}
	t->DR = *buf;
	t->SR1 &= (~(0x2));
	// PERRY PATCH: REMOVED
	// t->SR1 |= 0x400;
	// PERRY PATCH: REMOVED
	// t->SR1 |= 0x800;
	t->SR1 |= 0x1;
	t->SR2 &= (~(0x2));
	t->SR1 |= 0x40;
	// PERRY PATCH: REMOVED
	// t->SR1 &= (~(0x4));
	t->SR1 |= 0x10;
	t->SR2 |= 0x2;
	t->SR1 &= (~(0x10));
	t->SR1 |= 0x2;
	// PERRY PATCH: REMOVED
	// t->SR1 |= 0x100;
	t->SR1 |= 0x4;
	// PERRY PATCH: REMOVED
	// t->SR1 |= 0x200;

	// PERRY PATCH
	t->SR2 &= (~(0x4));
	stm32f1_i2c_update(t);
}

static gboolean stm32f1_i2c_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	STM32F1I2C *t = STM32F1_I2C(opaque);
	int ret;

	t->watch_tag = 0;
	t->SR1 &= (~(0x2));
	t->SR1 &= (~(0x80));
	t->SR1 |= 0x4;
	t->SR1 &= (~(0x8));
	t->SR1 &= (~(0x4));
	t->SR2 &= (~(0x4));
	t->SR1 |= 0x10;
	t->SR1 |= 0x2;
	t->SR1 &= (~(0x1));
	t->SR1 &= (~(0x400));
	t->SR2 |= 0x2;

	stm32f1_i2c_update(t);

	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->DR), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, stm32f1_i2c_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->SR1 &= (~(0x2));
	t->SR1 |= 0x400;
	t->SR2 |= 0x4;
	t->SR1 |= 0x1;
	t->SR1 |= 0x80;
	t->SR2 &= (~(0x2));
	t->SR1 &= (~(0x4));
	t->SR1 &= (~(0x10));
	t->SR1 |= 0x2;
	t->SR1 |= 0x4;
	t->SR1 |= 0x8;

	stm32f1_i2c_update(t);

	return FALSE;
}

static void stm32f1_i2c_register_reset(STM32F1I2C *t) {
	t->CR1 = 0x0;
	t->CR2 = 0x0;
	t->OAR1 = 0x0;
	t->OAR2 = 0x0;
	t->DR = 0x0;
	t->SR1 = 0x0;
	t->SR2 = 0x0;
	t->CCR = 0x0;
	t->TRISE = 0x2;
	t->SR1 &= (~(0x2));
	t->SR1 |= 0x400;
	t->SR2 |= 0x4;
	t->SR1 |= 0x1;
	t->SR1 |= 0x80;
	t->SR2 &= (~(0x2));
	t->SR1 &= (~(0x4));
	t->SR1 &= (~(0x10));
	t->SR1 |= 0x2;
	t->SR1 |= 0x4;
	t->SR1 |= 0x8;

}

static uint64_t stm32f1_i2c_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F1I2C *t = STM32F1_I2C(opaque);
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
		case A_I2C_DR:
			ret = t->DR;
			t->SR1 &= (~(0x2));
			t->SR1 |= 0x4;
			t->SR1 &= (~(0x100));
			t->SR2 &= (~(0x2));
			t->SR1 &= (~(0x4));
			t->SR1 &= (~(0x40));
			t->SR1 |= 0x10;
			t->SR1 &= (~(0x10));
			t->SR1 |= 0x2;
			t->SR1 &= (~(0x800));
			t->SR1 &= (~(0x200));
			t->SR1 &= (~(0x1));
			t->SR1 &= (~(0x400));
			t->SR2 |= 0x2;
			qemu_chr_fe_accept_input(&(t->chr));
			stm32f1_i2c_update(t);
			break;
		case A_I2C_SR1:
			ret = t->SR1;
			break;
		case A_I2C_SR2:
			ret = t->SR2;
			break;
		case A_I2C_CCR:
			ret = t->CCR;
			break;
		case A_I2C_TRISE:
			ret = t->TRISE;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F1 I2C read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f1_i2c_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F1I2C *t = STM32F1_I2C(opaque);


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
		case A_I2C_DR ... A_I2C_DR + 3:
			value = value << ((offset - A_I2C_DR) << 3);
			offset = A_I2C_DR;
			break;
		case A_I2C_SR1 ... A_I2C_SR1 + 3:
			value = value << ((offset - A_I2C_SR1) << 3);
			offset = A_I2C_SR1;
			break;
		case A_I2C_CCR ... A_I2C_CCR + 3:
			value = value << ((offset - A_I2C_CCR) << 3);
			offset = A_I2C_CCR;
			break;
		case A_I2C_TRISE ... A_I2C_TRISE + 3:
			value = value << ((offset - A_I2C_TRISE) << 3);
			offset = A_I2C_TRISE;
			break;

		default: break;
	}

	switch (offset) {
		case A_I2C_CR1:
			t->CR1 = value;
			if ((value & 0x200)) {
				t->SR1 |= 0x40;
			}
			if ((value & 0x100)) {
				t->SR1 |= 0x1;
			}
			if ((value & 0x400)) {
				t->SR1 |= 0x2;
			} else if ((!(value & 0x400))) {
				t->SR1 |= 0x4;
			}
			stm32f1_i2c_update(t);
			break;
		case A_I2C_CR2:
			t->CR2 = value;
			stm32f1_i2c_update(t);
			break;
		case A_I2C_OAR1:
			t->OAR1 = value;
			break;
		case A_I2C_OAR2:
			t->OAR2 = value;
			break;
		case A_I2C_DR:
			t->DR = value;
			stm32f1_i2c_transmit(NULL, G_IO_OUT, t);
			break;
		case A_I2C_SR1:
			t->SR1 &= value;
			if ((value == 0xfffffbff)) {
				t->SR2 &= (~(0x2));
			}
			stm32f1_i2c_update(t);
			break;
		case A_I2C_CCR:
			t->CCR = value;
			break;
		case A_I2C_TRISE:
			t->TRISE = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F1 I2C write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f1_i2c_ops = {
	.read = stm32f1_i2c_read,
	.write = stm32f1_i2c_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f1_i2c_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F1I2C *t = STM32F1_I2C(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f1_i2c_ops, t, "stm32f1-i2c", STM32F1_I2C_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 2; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32f1_i2c_realize(DeviceState *dev, Error **errp) {
	STM32F1I2C *t = STM32F1_I2C(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), stm32f1_i2c_can_receive, stm32f1_i2c_receive,
		NULL, NULL, t, NULL, true);

}

static void stm32f1_i2c_reset_enter(Object *obj, ResetType type) {
	STM32F1I2C *t = STM32F1_I2C(obj);
	stm32f1_i2c_register_reset(t);
}

static const VMStateDescription stm32f1_i2c_vmstate = {
	.name = "stm32f1-i2c",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR1, STM32F1I2C),
		VMSTATE_UINT32(CR2, STM32F1I2C),
		VMSTATE_UINT32(OAR1, STM32F1I2C),
		VMSTATE_UINT32(OAR2, STM32F1I2C),
		VMSTATE_UINT32(DR, STM32F1I2C),
		VMSTATE_UINT32(SR1, STM32F1I2C),
		VMSTATE_UINT32(SR2, STM32F1I2C),
		VMSTATE_UINT32(CCR, STM32F1I2C),
		VMSTATE_UINT32(TRISE, STM32F1I2C),

		VMSTATE_END_OF_LIST()
	}
};

static Property stm32f1_i2c_properties[] = {
	DEFINE_PROP_CHR("chardev", STM32F1I2C, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void stm32f1_i2c_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f1_i2c_vmstate;
	dc->realize = stm32f1_i2c_realize;
	rc->phases.enter = stm32f1_i2c_reset_enter;
	device_class_set_props(dc, stm32f1_i2c_properties);

}

static const TypeInfo stm32f1_i2c_info = {
	.name = TYPE_STM32F1_I2C,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F1I2C),
	.instance_init = stm32f1_i2c_init,
	.class_init = stm32f1_i2c_class_init,
};

static void stm32f1_i2c_register_types(void) {
	type_register_static(&stm32f1_i2c_info);
}

type_init(stm32f1_i2c_register_types);

#define TYPE_STM32F1_ADC "stm32f1-adc"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F1ADC, STM32F1_ADC)

struct STM32F1ADC {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t SR;
	uint32_t CR1;
	uint32_t CR2;
	uint32_t SMPR1;
	uint32_t SMPR2;
	uint32_t JOFR1;
	uint32_t JOFR2;
	uint32_t JOFR3;
	uint32_t JOFR4;
	uint32_t HTR;
	uint32_t LTR;
	uint32_t SQR1;
	uint32_t SQR2;
	uint32_t SQR3;
	uint32_t JSQR;
	uint32_t JDR1;
	uint32_t JDR2;
	uint32_t JDR3;
	uint32_t JDR4;
	uint32_t DR;
	/* chardev backend */
	CharBackend chr;
	guint watch_tag;

	/* base */
	uint32_t base;

};
#define STM32F1_ADC_SIZE				0x50

REG32(ADC_SR, 0x0)
	FIELD(ADC_SR, STRT, 4, 1)
	FIELD(ADC_SR, JSTRT, 3, 1)
	FIELD(ADC_SR, JEOC, 2, 1)
	FIELD(ADC_SR, EOC, 1, 1)
	FIELD(ADC_SR, AWD, 0, 1)
REG32(ADC_CR1, 0x4)
	FIELD(ADC_CR1, AWDEN, 23, 1)
	FIELD(ADC_CR1, JAWDEN, 22, 1)
	FIELD(ADC_CR1, DUALMOD, 16, 4)
	FIELD(ADC_CR1, DISCNUM, 13, 3)
	FIELD(ADC_CR1, JDISCEN, 12, 1)
	FIELD(ADC_CR1, DISCEN, 11, 1)
	FIELD(ADC_CR1, JAUTO, 10, 1)
	FIELD(ADC_CR1, AWDSGL, 9, 1)
	FIELD(ADC_CR1, SCAN, 8, 1)
	FIELD(ADC_CR1, JEOCIE, 7, 1)
	FIELD(ADC_CR1, AWDIE, 6, 1)
	FIELD(ADC_CR1, EOCIE, 5, 1)
	FIELD(ADC_CR1, AWDCH, 0, 5)
REG32(ADC_CR2, 0x8)
	FIELD(ADC_CR2, TSVREFE, 23, 1)
	FIELD(ADC_CR2, SWSTART, 22, 1)
	FIELD(ADC_CR2, JSWSTART, 21, 1)
	FIELD(ADC_CR2, EXTTRIG, 20, 1)
	FIELD(ADC_CR2, EXTSEL, 17, 3)
	FIELD(ADC_CR2, JEXTTRIG, 15, 1)
	FIELD(ADC_CR2, JEXTSEL, 12, 3)
	FIELD(ADC_CR2, ALIGN, 11, 1)
	FIELD(ADC_CR2, DMA, 8, 1)
	FIELD(ADC_CR2, RSTCAL, 3, 1)
	FIELD(ADC_CR2, CAL, 2, 1)
	FIELD(ADC_CR2, CONT, 1, 1)
	FIELD(ADC_CR2, ADON, 0, 1)
REG32(ADC_SMPR1, 0xc)
	FIELD(ADC_SMPR1, SMP10, 0, 3)
	FIELD(ADC_SMPR1, SMP11, 3, 3)
	FIELD(ADC_SMPR1, SMP12, 6, 3)
	FIELD(ADC_SMPR1, SMP13, 9, 3)
	FIELD(ADC_SMPR1, SMP14, 12, 3)
	FIELD(ADC_SMPR1, SMP15, 15, 3)
	FIELD(ADC_SMPR1, SMP16, 18, 3)
	FIELD(ADC_SMPR1, SMP17, 21, 3)
REG32(ADC_SMPR2, 0x10)
	FIELD(ADC_SMPR2, SMP0, 0, 3)
	FIELD(ADC_SMPR2, SMP1, 3, 3)
	FIELD(ADC_SMPR2, SMP2, 6, 3)
	FIELD(ADC_SMPR2, SMP3, 9, 3)
	FIELD(ADC_SMPR2, SMP4, 12, 3)
	FIELD(ADC_SMPR2, SMP5, 15, 3)
	FIELD(ADC_SMPR2, SMP6, 18, 3)
	FIELD(ADC_SMPR2, SMP7, 21, 3)
	FIELD(ADC_SMPR2, SMP8, 24, 3)
	FIELD(ADC_SMPR2, SMP9, 27, 3)
REG32(ADC_JOFR1, 0x14)
	FIELD(ADC_JOFR1, JOFFSET1, 0, 12)
REG32(ADC_JOFR2, 0x18)
	FIELD(ADC_JOFR2, JOFFSET2, 0, 12)
REG32(ADC_JOFR3, 0x1c)
	FIELD(ADC_JOFR3, JOFFSET3, 0, 12)
REG32(ADC_JOFR4, 0x20)
	FIELD(ADC_JOFR4, JOFFSET4, 0, 12)
REG32(ADC_HTR, 0x24)
	FIELD(ADC_HTR, HT, 0, 12)
REG32(ADC_LTR, 0x28)
	FIELD(ADC_LTR, LT, 0, 12)
REG32(ADC_SQR1, 0x2c)
	FIELD(ADC_SQR1, L, 20, 4)
	FIELD(ADC_SQR1, SQ16, 15, 5)
	FIELD(ADC_SQR1, SQ15, 10, 5)
	FIELD(ADC_SQR1, SQ14, 5, 5)
	FIELD(ADC_SQR1, SQ13, 0, 5)
REG32(ADC_SQR2, 0x30)
	FIELD(ADC_SQR2, SQ12, 25, 5)
	FIELD(ADC_SQR2, SQ11, 20, 5)
	FIELD(ADC_SQR2, SQ10, 15, 5)
	FIELD(ADC_SQR2, SQ9, 10, 5)
	FIELD(ADC_SQR2, SQ8, 5, 5)
	FIELD(ADC_SQR2, SQ7, 0, 5)
REG32(ADC_SQR3, 0x34)
	FIELD(ADC_SQR3, SQ6, 25, 5)
	FIELD(ADC_SQR3, SQ5, 20, 5)
	FIELD(ADC_SQR3, SQ4, 15, 5)
	FIELD(ADC_SQR3, SQ3, 10, 5)
	FIELD(ADC_SQR3, SQ2, 5, 5)
	FIELD(ADC_SQR3, SQ1, 0, 5)
REG32(ADC_JSQR, 0x38)
	FIELD(ADC_JSQR, JL, 20, 2)
	FIELD(ADC_JSQR, JSQ4, 15, 5)
	FIELD(ADC_JSQR, JSQ3, 10, 5)
	FIELD(ADC_JSQR, JSQ2, 5, 5)
	FIELD(ADC_JSQR, JSQ1, 0, 5)
REG32(ADC_JDR1, 0x3c)
	FIELD(ADC_JDR1, JDATA, 0, 16)
REG32(ADC_JDR2, 0x40)
	FIELD(ADC_JDR2, JDATA, 0, 16)
REG32(ADC_JDR3, 0x44)
	FIELD(ADC_JDR3, JDATA, 0, 16)
REG32(ADC_JDR4, 0x48)
	FIELD(ADC_JDR4, JDATA, 0, 16)
REG32(ADC_DR, 0x4c)
	FIELD(ADC_DR, DATA, 0, 16)
	FIELD(ADC_DR, ADC2DATA, 16, 16)


__attribute__((unused))
static void stm32f1_adc_update(STM32F1ADC *t) {
	int conditions = (((t->CR1 & 0x40) && (t->SR & 0x1)) || ((t->CR1 & 0x80) && (t->SR & 0x4)) || ((t->CR1 & 0x20) && (t->SR & 0x2)));
	qemu_set_irq(t->irq[0], conditions);

}

static int stm32f1_adc_can_receive(void *opaque) {
	return 1;
}

static void stm32f1_adc_receive(void *opaque, const uint8_t *buf, int size) {
	STM32F1ADC *t = STM32F1_ADC(opaque);

	t->DR = *buf;
	t->SQR1 &= (~(0x100000));
	t->SR |= 0x2;
	t->SQR1 &= (~(0x200000));
	t->SQR1 &= (~(0x400000));
	t->SQR1 &= (~(0x800000));

	stm32f1_adc_update(t);
}

static void stm32f1_adc_register_reset(STM32F1ADC *t) {
	t->SR = 0x0;
	t->CR1 = 0x0;
	t->CR2 = 0x0;
	t->SMPR1 = 0x0;
	t->SMPR2 = 0x0;
	t->JOFR1 = 0x0;
	t->JOFR2 = 0x0;
	t->JOFR3 = 0x0;
	t->JOFR4 = 0x0;
	t->HTR = 0xfff;
	t->LTR = 0x0;
	t->SQR1 = 0x0;
	t->SQR2 = 0x0;
	t->SQR3 = 0x0;
	t->JSQR = 0x0;
	t->JDR1 = 0x0;
	t->JDR2 = 0x0;
	t->JDR3 = 0x0;
	t->JDR4 = 0x0;
	t->DR = 0x0;

}

static uint64_t stm32f1_adc_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F1ADC *t = STM32F1_ADC(opaque);
	uint64_t ret;

	switch (offset) {
		case A_ADC_SR:
			ret = t->SR;
			break;
		case A_ADC_CR1:
			ret = t->CR1;
			break;
		case A_ADC_CR2:
			ret = t->CR2;
			break;
		case A_ADC_SMPR1:
			ret = t->SMPR1;
			break;
		case A_ADC_SMPR2:
			ret = t->SMPR2;
			break;
		case A_ADC_JOFR1:
			ret = t->JOFR1;
			break;
		case A_ADC_JOFR2:
			ret = t->JOFR2;
			break;
		case A_ADC_JOFR3:
			ret = t->JOFR3;
			break;
		case A_ADC_JOFR4:
			ret = t->JOFR4;
			break;
		case A_ADC_HTR:
			ret = t->HTR;
			break;
		case A_ADC_LTR:
			ret = t->LTR;
			break;
		case A_ADC_SQR1:
			ret = t->SQR1;
			break;
		case A_ADC_SQR2:
			ret = t->SQR2;
			break;
		case A_ADC_SQR3:
			ret = t->SQR3;
			break;
		case A_ADC_JSQR:
			ret = t->JSQR;
			break;
		case A_ADC_JDR1:
			ret = t->JDR1;
			break;
		case A_ADC_JDR2:
			ret = t->JDR2;
			break;
		case A_ADC_JDR3:
			ret = t->JDR3;
			break;
		case A_ADC_JDR4:
			ret = t->JDR4;
			break;
		case A_ADC_DR:
			ret = t->DR;
			t->SQR1 |= 0x100000;
			t->SQR1 |= 0x800000;
			t->SR &= (~(0x2));
			t->SQR1 |= 0x200000;
			t->SQR1 |= 0x400000;
			qemu_chr_fe_accept_input(&(t->chr));
			stm32f1_adc_update(t);
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F1 ADC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f1_adc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F1ADC *t = STM32F1_ADC(opaque);


	switch (offset) {
		case A_ADC_SR ... A_ADC_SR + 3:
			value = value << ((offset - A_ADC_SR) << 3);
			offset = A_ADC_SR;
			break;
		case A_ADC_CR1 ... A_ADC_CR1 + 3:
			value = value << ((offset - A_ADC_CR1) << 3);
			offset = A_ADC_CR1;
			break;
		case A_ADC_CR2 ... A_ADC_CR2 + 3:
			value = value << ((offset - A_ADC_CR2) << 3);
			offset = A_ADC_CR2;
			break;
		case A_ADC_SMPR1 ... A_ADC_SMPR1 + 3:
			value = value << ((offset - A_ADC_SMPR1) << 3);
			offset = A_ADC_SMPR1;
			break;
		case A_ADC_SMPR2 ... A_ADC_SMPR2 + 3:
			value = value << ((offset - A_ADC_SMPR2) << 3);
			offset = A_ADC_SMPR2;
			break;
		case A_ADC_JOFR1 ... A_ADC_JOFR1 + 3:
			value = value << ((offset - A_ADC_JOFR1) << 3);
			offset = A_ADC_JOFR1;
			break;
		case A_ADC_JOFR2 ... A_ADC_JOFR2 + 3:
			value = value << ((offset - A_ADC_JOFR2) << 3);
			offset = A_ADC_JOFR2;
			break;
		case A_ADC_JOFR3 ... A_ADC_JOFR3 + 3:
			value = value << ((offset - A_ADC_JOFR3) << 3);
			offset = A_ADC_JOFR3;
			break;
		case A_ADC_JOFR4 ... A_ADC_JOFR4 + 3:
			value = value << ((offset - A_ADC_JOFR4) << 3);
			offset = A_ADC_JOFR4;
			break;
		case A_ADC_HTR ... A_ADC_HTR + 3:
			value = value << ((offset - A_ADC_HTR) << 3);
			offset = A_ADC_HTR;
			break;
		case A_ADC_LTR ... A_ADC_LTR + 3:
			value = value << ((offset - A_ADC_LTR) << 3);
			offset = A_ADC_LTR;
			break;
		case A_ADC_SQR1 ... A_ADC_SQR1 + 3:
			value = value << ((offset - A_ADC_SQR1) << 3);
			offset = A_ADC_SQR1;
			break;
		case A_ADC_SQR2 ... A_ADC_SQR2 + 3:
			value = value << ((offset - A_ADC_SQR2) << 3);
			offset = A_ADC_SQR2;
			break;
		case A_ADC_SQR3 ... A_ADC_SQR3 + 3:
			value = value << ((offset - A_ADC_SQR3) << 3);
			offset = A_ADC_SQR3;
			break;
		case A_ADC_JSQR ... A_ADC_JSQR + 3:
			value = value << ((offset - A_ADC_JSQR) << 3);
			offset = A_ADC_JSQR;
			break;

		default: break;
	}

	switch (offset) {
		case A_ADC_SR:
			t->SR &= value;
			stm32f1_adc_update(t);
			break;
		case A_ADC_CR1:
			t->CR1 = value;
			stm32f1_adc_update(t);
			break;
		case A_ADC_CR2:
			t->CR2 = value;
			if ((value & 0x1)) {
				t->CR2 |= 0x1;
			} else if ((!(value & 0x1))) {
				t->CR2 &= (~(0x1));
			}
			if ((value & 0x4)) {
				t->CR2 &= (~(0x4));
			}
			if ((value & 0x8)) {
				t->CR2 &= (~(0x8));
			}
			break;
		case A_ADC_SMPR1:
			t->SMPR1 = value;
			break;
		case A_ADC_SMPR2:
			t->SMPR2 = value;
			break;
		case A_ADC_JOFR1:
			t->JOFR1 = value;
			break;
		case A_ADC_JOFR2:
			t->JOFR2 = value;
			break;
		case A_ADC_JOFR3:
			t->JOFR3 = value;
			break;
		case A_ADC_JOFR4:
			t->JOFR4 = value;
			break;
		case A_ADC_HTR:
			t->HTR = value;
			break;
		case A_ADC_LTR:
			t->LTR = value;
			break;
		case A_ADC_SQR1:
			t->SQR1 &= value;
			if (((!(value & 0x100000)) && (!(value & 0x200000)) && (!(value & 0x400000)) && (!(value & 0x800000)))) {
				t->SR |= 0x2;
			}
			stm32f1_adc_update(t);
			break;
		case A_ADC_SQR2:
			t->SQR2 = value;
			break;
		case A_ADC_SQR3:
			t->SQR3 = value;
			break;
		case A_ADC_JSQR:
			t->JSQR = value;
			if (((!(value & 0x100000)) && (!(value & 0x200000)))) {
				t->SR |= 0x4;
			}
			stm32f1_adc_update(t);
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F1 ADC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f1_adc_ops = {
	.read = stm32f1_adc_read,
	.write = stm32f1_adc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f1_adc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F1ADC *t = STM32F1_ADC(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f1_adc_ops, t, "stm32f1-adc", STM32F1_ADC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32f1_adc_realize(DeviceState *dev, Error **errp) {
	STM32F1ADC *t = STM32F1_ADC(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), stm32f1_adc_can_receive, stm32f1_adc_receive,
		NULL, NULL, t, NULL, true);

}

static void stm32f1_adc_reset_enter(Object *obj, ResetType type) {
	STM32F1ADC *t = STM32F1_ADC(obj);
	stm32f1_adc_register_reset(t);
}

static const VMStateDescription stm32f1_adc_vmstate = {
	.name = "stm32f1-adc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(SR, STM32F1ADC),
		VMSTATE_UINT32(CR1, STM32F1ADC),
		VMSTATE_UINT32(CR2, STM32F1ADC),
		VMSTATE_UINT32(SMPR1, STM32F1ADC),
		VMSTATE_UINT32(SMPR2, STM32F1ADC),
		VMSTATE_UINT32(JOFR1, STM32F1ADC),
		VMSTATE_UINT32(JOFR2, STM32F1ADC),
		VMSTATE_UINT32(JOFR3, STM32F1ADC),
		VMSTATE_UINT32(JOFR4, STM32F1ADC),
		VMSTATE_UINT32(HTR, STM32F1ADC),
		VMSTATE_UINT32(LTR, STM32F1ADC),
		VMSTATE_UINT32(SQR1, STM32F1ADC),
		VMSTATE_UINT32(SQR2, STM32F1ADC),
		VMSTATE_UINT32(SQR3, STM32F1ADC),
		VMSTATE_UINT32(JSQR, STM32F1ADC),
		VMSTATE_UINT32(JDR1, STM32F1ADC),
		VMSTATE_UINT32(JDR2, STM32F1ADC),
		VMSTATE_UINT32(JDR3, STM32F1ADC),
		VMSTATE_UINT32(JDR4, STM32F1ADC),
		VMSTATE_UINT32(DR, STM32F1ADC),

		VMSTATE_END_OF_LIST()
	}
};

static Property stm32f1_adc_properties[] = {
	DEFINE_PROP_CHR("chardev", STM32F1ADC, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void stm32f1_adc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f1_adc_vmstate;
	dc->realize = stm32f1_adc_realize;
	rc->phases.enter = stm32f1_adc_reset_enter;
	device_class_set_props(dc, stm32f1_adc_properties);

}

static const TypeInfo stm32f1_adc_info = {
	.name = TYPE_STM32F1_ADC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F1ADC),
	.instance_init = stm32f1_adc_init,
	.class_init = stm32f1_adc_class_init,
};

static void stm32f1_adc_register_types(void) {
	type_register_static(&stm32f1_adc_info);
}

type_init(stm32f1_adc_register_types);

#define TYPE_STM32F1_IWDG "stm32f1-iwdg"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F1IWDG, STM32F1_IWDG)

struct STM32F1IWDG {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/*registers*/
	uint32_t KR;
	uint32_t PR;
	uint32_t RLR;
	uint32_t SR;
	/* base */
	uint32_t base;

};
#define STM32F1_IWDG_SIZE				0x10

REG32(IWDG_KR, 0x0)
	FIELD(IWDG_KR, KEY, 0, 16)
REG32(IWDG_PR, 0x4)
	FIELD(IWDG_PR, PR, 0, 3)
REG32(IWDG_RLR, 0x8)
	FIELD(IWDG_RLR, RL, 0, 12)
REG32(IWDG_SR, 0xc)
	FIELD(IWDG_SR, PVU, 0, 1)
	FIELD(IWDG_SR, RVU, 1, 1)


static void stm32f1_iwdg_register_reset(STM32F1IWDG *t) {
	t->KR = 0x0;
	t->PR = 0x0;
	t->RLR = 0xfff;
	t->SR = 0x0;

}

static uint64_t stm32f1_iwdg_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F1IWDG *t = STM32F1_IWDG(opaque);
	uint64_t ret;

	switch (offset) {
		case A_IWDG_PR:
			ret = t->PR;
			break;
		case A_IWDG_RLR:
			ret = t->RLR;
			break;
		case A_IWDG_SR:
			ret = t->SR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F1 IWDG read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f1_iwdg_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F1IWDG *t = STM32F1_IWDG(opaque);


	switch (offset) {
		case A_IWDG_KR ... A_IWDG_KR + 3:
			value = value << ((offset - A_IWDG_KR) << 3);
			offset = A_IWDG_KR;
			break;
		case A_IWDG_PR ... A_IWDG_PR + 3:
			value = value << ((offset - A_IWDG_PR) << 3);
			offset = A_IWDG_PR;
			break;
		case A_IWDG_RLR ... A_IWDG_RLR + 3:
			value = value << ((offset - A_IWDG_RLR) << 3);
			offset = A_IWDG_RLR;
			break;

		default: break;
	}

	switch (offset) {
		case A_IWDG_KR:
			t->KR = value;
			break;
		case A_IWDG_PR:
			t->PR = value;
			break;
		case A_IWDG_RLR:
			t->RLR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F1 IWDG write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f1_iwdg_ops = {
	.read = stm32f1_iwdg_read,
	.write = stm32f1_iwdg_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f1_iwdg_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F1IWDG *t = STM32F1_IWDG(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f1_iwdg_ops, t, "stm32f1-iwdg", STM32F1_IWDG_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void stm32f1_iwdg_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32f1_iwdg_reset_enter(Object *obj, ResetType type) {
	STM32F1IWDG *t = STM32F1_IWDG(obj);
	stm32f1_iwdg_register_reset(t);
}

static const VMStateDescription stm32f1_iwdg_vmstate = {
	.name = "stm32f1-iwdg",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(KR, STM32F1IWDG),
		VMSTATE_UINT32(PR, STM32F1IWDG),
		VMSTATE_UINT32(RLR, STM32F1IWDG),
		VMSTATE_UINT32(SR, STM32F1IWDG),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f1_iwdg_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f1_iwdg_vmstate;
	dc->realize = stm32f1_iwdg_realize;
	rc->phases.enter = stm32f1_iwdg_reset_enter;

}

static const TypeInfo stm32f1_iwdg_info = {
	.name = TYPE_STM32F1_IWDG,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F1IWDG),
	.instance_init = stm32f1_iwdg_init,
	.class_init = stm32f1_iwdg_class_init,
};

static void stm32f1_iwdg_register_types(void) {
	type_register_static(&stm32f1_iwdg_info);
}

type_init(stm32f1_iwdg_register_types);

#define TYPE_STM32F103_MACHINE MACHINE_TYPE_NAME("stm32f103")
OBJECT_DECLARE_TYPE(STM32F103MachineState, STM32F103MachineClass, STM32F103_MACHINE)

struct STM32F103MachineClass {
	MachineClass parent;
};

struct STM32F103MachineState {
	MachineState parent;
	ARMv7MState armv7m;
};

static void stm32f103_periph_init(MachineState *machine) {
	STM32F103MachineState *sms = STM32F103_MACHINE(machine);
	STM32F1DMA *p0 = g_new(STM32F1DMA, 1);
	object_initialize_child(OBJECT(sms), "DMA1", p0, TYPE_STM32F1_DMA);
	sysbus_realize(SYS_BUS_DEVICE(p0), &error_fatal);
	p0->base = 0x40020000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p0), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 11));
	sysbus_connect_irq(SYS_BUS_DEVICE(p0), 1, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 12));
	sysbus_connect_irq(SYS_BUS_DEVICE(p0), 2, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 13));
	sysbus_connect_irq(SYS_BUS_DEVICE(p0), 3, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 14));
	sysbus_connect_irq(SYS_BUS_DEVICE(p0), 4, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 15));
	sysbus_connect_irq(SYS_BUS_DEVICE(p0), 5, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 16));
	sysbus_connect_irq(SYS_BUS_DEVICE(p0), 6, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 17));
	sysbus_mmio_map(SYS_BUS_DEVICE(p0), 0, 0x40020000);

	STM32F1DMA *p1 = g_new(STM32F1DMA, 1);
	object_initialize_child(OBJECT(sms), "DMA2", p1, TYPE_STM32F1_DMA);
	sysbus_realize(SYS_BUS_DEVICE(p1), &error_fatal);
	p1->base = 0x40020400;
	sysbus_connect_irq(SYS_BUS_DEVICE(p1), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 56));
	sysbus_connect_irq(SYS_BUS_DEVICE(p1), 1, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 57));
	sysbus_connect_irq(SYS_BUS_DEVICE(p1), 2, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 58));
	sysbus_connect_irq(SYS_BUS_DEVICE(p1), 3, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 59));
	sysbus_mmio_map(SYS_BUS_DEVICE(p1), 0, 0x40020400);

	STM32F1TIM2 *p2 = g_new(STM32F1TIM2, 1);
	object_initialize_child(OBJECT(sms), "TIM2", p2, TYPE_STM32F1_TIM2);
	sysbus_realize(SYS_BUS_DEVICE(p2), &error_fatal);
	p2->base = 0x40000000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p2), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 28));
	sysbus_mmio_map(SYS_BUS_DEVICE(p2), 0, 0x40000000);

	STM32F1TIM2 *p3 = g_new(STM32F1TIM2, 1);
	object_initialize_child(OBJECT(sms), "TIM3", p3, TYPE_STM32F1_TIM2);
	sysbus_realize(SYS_BUS_DEVICE(p3), &error_fatal);
	p3->base = 0x40000400;
	sysbus_connect_irq(SYS_BUS_DEVICE(p3), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 29));
	sysbus_mmio_map(SYS_BUS_DEVICE(p3), 0, 0x40000400);

	STM32F1TIM2 *p4 = g_new(STM32F1TIM2, 1);
	object_initialize_child(OBJECT(sms), "TIM4", p4, TYPE_STM32F1_TIM2);
	sysbus_realize(SYS_BUS_DEVICE(p4), &error_fatal);
	p4->base = 0x40000800;
	sysbus_connect_irq(SYS_BUS_DEVICE(p4), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 30));
	sysbus_mmio_map(SYS_BUS_DEVICE(p4), 0, 0x40000800);

	STM32F1TIM2 *p5 = g_new(STM32F1TIM2, 1);
	object_initialize_child(OBJECT(sms), "TIM5", p5, TYPE_STM32F1_TIM2);
	sysbus_realize(SYS_BUS_DEVICE(p5), &error_fatal);
	p5->base = 0x40000c00;
	sysbus_connect_irq(SYS_BUS_DEVICE(p5), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 50));
	sysbus_mmio_map(SYS_BUS_DEVICE(p5), 0, 0x40000c00);

	STM32F1TIM1 *p6 = g_new(STM32F1TIM1, 1);
	object_initialize_child(OBJECT(sms), "TIM1", p6, TYPE_STM32F1_TIM1);
	sysbus_realize(SYS_BUS_DEVICE(p6), &error_fatal);
	p6->base = 0x40012c00;
	sysbus_connect_irq(SYS_BUS_DEVICE(p6), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 24));
	sysbus_connect_irq(SYS_BUS_DEVICE(p6), 1, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 25));
	sysbus_connect_irq(SYS_BUS_DEVICE(p6), 2, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 26));
	sysbus_connect_irq(SYS_BUS_DEVICE(p6), 3, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 27));
	sysbus_mmio_map(SYS_BUS_DEVICE(p6), 0, 0x40012c00);

	STM32F1ADC *p7 = g_new(STM32F1ADC, 1);
	object_initialize_child(OBJECT(sms), "ADC1", p7, TYPE_STM32F1_ADC);
	qdev_prop_set_chr(DEVICE(p7), "chardev", qemu_chr_new("soc-adc1", "chardev:adc1", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p7), &error_fatal);
	p7->base = 0x40012400;
	sysbus_connect_irq(SYS_BUS_DEVICE(p7), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 18));
	sysbus_mmio_map(SYS_BUS_DEVICE(p7), 0, 0x40012400);

	STM32F1ADC *p8 = g_new(STM32F1ADC, 1);
	object_initialize_child(OBJECT(sms), "ADC2", p8, TYPE_STM32F1_ADC);
	qdev_prop_set_chr(DEVICE(p8), "chardev", qemu_chr_new("soc-adc2", "chardev:adc2", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p8), &error_fatal);
	p8->base = 0x40012800;
	sysbus_connect_irq(SYS_BUS_DEVICE(p8), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 18));
	sysbus_mmio_map(SYS_BUS_DEVICE(p8), 0, 0x40012800);

	STM32F1USART *p9 = g_new(STM32F1USART, 1);
	object_initialize_child(OBJECT(sms), "USART1", p9, TYPE_STM32F1_USART);
	qdev_prop_set_chr(DEVICE(p9), "chardev", qemu_chr_new("soc-usart1", "chardev:usart1", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p9), &error_fatal);
	p9->base = 0x40013800;
	sysbus_connect_irq(SYS_BUS_DEVICE(p9), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 37));
	sysbus_mmio_map(SYS_BUS_DEVICE(p9), 0, 0x40013800);

	STM32F1USART *p10 = g_new(STM32F1USART, 1);
	object_initialize_child(OBJECT(sms), "USART2", p10, TYPE_STM32F1_USART);
	qdev_prop_set_chr(DEVICE(p10), "chardev", qemu_chr_new("soc-usart2", "chardev:usart2", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p10), &error_fatal);
	p10->base = 0x40004400;
	sysbus_connect_irq(SYS_BUS_DEVICE(p10), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 38));
	sysbus_mmio_map(SYS_BUS_DEVICE(p10), 0, 0x40004400);

	STM32F1PWR *p11 = g_new(STM32F1PWR, 1);
	object_initialize_child(OBJECT(sms), "PWR", p11, TYPE_STM32F1_PWR);
	sysbus_realize(SYS_BUS_DEVICE(p11), &error_fatal);
	p11->base = 0x40007000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p11), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 1));
	sysbus_mmio_map(SYS_BUS_DEVICE(p11), 0, 0x40007000);

	STM32F1RCC *p12 = g_new(STM32F1RCC, 1);
	object_initialize_child(OBJECT(sms), "RCC", p12, TYPE_STM32F1_RCC);
	sysbus_realize(SYS_BUS_DEVICE(p12), &error_fatal);
	p12->base = 0x40021000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p12), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 5));
	sysbus_mmio_map(SYS_BUS_DEVICE(p12), 0, 0x40021000);

	STM32F1FLASH *p13 = g_new(STM32F1FLASH, 1);
	object_initialize_child(OBJECT(sms), "FLASH", p13, TYPE_STM32F1_FLASH);
	sysbus_realize(SYS_BUS_DEVICE(p13), &error_fatal);
	p13->base = 0x40022000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p13), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 4));
	sysbus_mmio_map(SYS_BUS_DEVICE(p13), 0, 0x40022000);

	STM32F1AFIO *p14 = g_new(STM32F1AFIO, 1);
	object_initialize_child(OBJECT(sms), "AFIO", p14, TYPE_STM32F1_AFIO);
	sysbus_realize(SYS_BUS_DEVICE(p14), &error_fatal);
	p14->base = 0x40010000;
	sysbus_mmio_map(SYS_BUS_DEVICE(p14), 0, 0x40010000);

	STM32F1EXTI *p15 = g_new(STM32F1EXTI, 1);
	object_initialize_child(OBJECT(sms), "EXTI", p15, TYPE_STM32F1_EXTI);
	qdev_prop_set_chr(DEVICE(p15), "chardev", qemu_chr_new("soc-exti", "chardev:exti", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p15), &error_fatal);
	p15->base = 0x40010400;
	sysbus_connect_irq(SYS_BUS_DEVICE(p15), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 2));
	sysbus_connect_irq(SYS_BUS_DEVICE(p15), 1, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 6));
	sysbus_connect_irq(SYS_BUS_DEVICE(p15), 2, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 7));
	sysbus_connect_irq(SYS_BUS_DEVICE(p15), 3, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 8));
	sysbus_connect_irq(SYS_BUS_DEVICE(p15), 4, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 9));
	sysbus_connect_irq(SYS_BUS_DEVICE(p15), 5, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 10));
	sysbus_connect_irq(SYS_BUS_DEVICE(p15), 6, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 23));
	sysbus_connect_irq(SYS_BUS_DEVICE(p15), 7, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 40));
	sysbus_mmio_map(SYS_BUS_DEVICE(p15), 0, 0x40010400);

	STM32F1GPIOA *p16 = g_new(STM32F1GPIOA, 1);
	object_initialize_child(OBJECT(sms), "GPIOA", p16, TYPE_STM32F1_GPIOA);
	sysbus_realize(SYS_BUS_DEVICE(p16), &error_fatal);
	p16->base = 0x40010800;
	sysbus_mmio_map(SYS_BUS_DEVICE(p16), 0, 0x40010800);

	STM32F1GPIOA *p17 = g_new(STM32F1GPIOA, 1);
	object_initialize_child(OBJECT(sms), "GPIOB", p17, TYPE_STM32F1_GPIOA);
	sysbus_realize(SYS_BUS_DEVICE(p17), &error_fatal);
	p17->base = 0x40010c00;
	sysbus_mmio_map(SYS_BUS_DEVICE(p17), 0, 0x40010c00);

	STM32F1GPIOA *p18 = g_new(STM32F1GPIOA, 1);
	object_initialize_child(OBJECT(sms), "GPIOC", p18, TYPE_STM32F1_GPIOA);
	sysbus_realize(SYS_BUS_DEVICE(p18), &error_fatal);
	p18->base = 0x40011000;
	sysbus_mmio_map(SYS_BUS_DEVICE(p18), 0, 0x40011000);

	STM32F1GPIOA *p19 = g_new(STM32F1GPIOA, 1);
	object_initialize_child(OBJECT(sms), "GPIOD", p19, TYPE_STM32F1_GPIOA);
	sysbus_realize(SYS_BUS_DEVICE(p19), &error_fatal);
	p19->base = 0x40011400;
	sysbus_mmio_map(SYS_BUS_DEVICE(p19), 0, 0x40011400);

	STM32F1GPIOA *p20 = g_new(STM32F1GPIOA, 1);
	object_initialize_child(OBJECT(sms), "GPIOE", p20, TYPE_STM32F1_GPIOA);
	sysbus_realize(SYS_BUS_DEVICE(p20), &error_fatal);
	p20->base = 0x40011800;
	sysbus_mmio_map(SYS_BUS_DEVICE(p20), 0, 0x40011800);

	STM32F1GPIOA *p21 = g_new(STM32F1GPIOA, 1);
	object_initialize_child(OBJECT(sms), "GPIOF", p21, TYPE_STM32F1_GPIOA);
	sysbus_realize(SYS_BUS_DEVICE(p21), &error_fatal);
	p21->base = 0x40011c00;
	sysbus_mmio_map(SYS_BUS_DEVICE(p21), 0, 0x40011c00);

	STM32F1GPIOA *p22 = g_new(STM32F1GPIOA, 1);
	object_initialize_child(OBJECT(sms), "GPIOG", p22, TYPE_STM32F1_GPIOA);
	sysbus_realize(SYS_BUS_DEVICE(p22), &error_fatal);
	p22->base = 0x40012000;
	sysbus_mmio_map(SYS_BUS_DEVICE(p22), 0, 0x40012000);

	STM32F1SPI *p23 = g_new(STM32F1SPI, 1);
	object_initialize_child(OBJECT(sms), "SPI1", p23, TYPE_STM32F1_SPI);
	qdev_prop_set_chr(DEVICE(p23), "chardev", qemu_chr_new("soc-spi1", "chardev:spi1", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p23), &error_fatal);
	p23->base = 0x40013000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p23), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 35));
	sysbus_mmio_map(SYS_BUS_DEVICE(p23), 0, 0x40013000);

	STM32F1SPI *p24 = g_new(STM32F1SPI, 1);
	object_initialize_child(OBJECT(sms), "SPI2", p24, TYPE_STM32F1_SPI);
	qdev_prop_set_chr(DEVICE(p24), "chardev", qemu_chr_new("soc-spi2", "chardev:spi2", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p24), &error_fatal);
	p24->base = 0x40003800;
	sysbus_connect_irq(SYS_BUS_DEVICE(p24), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 36));
	sysbus_mmio_map(SYS_BUS_DEVICE(p24), 0, 0x40003800);

	STM32F1SPI *p25 = g_new(STM32F1SPI, 1);
	object_initialize_child(OBJECT(sms), "SPI3", p25, TYPE_STM32F1_SPI);
	qdev_prop_set_chr(DEVICE(p25), "chardev", qemu_chr_new("soc-spi3", "chardev:spi3", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p25), &error_fatal);
	p25->base = 0x40003c00;
	sysbus_connect_irq(SYS_BUS_DEVICE(p25), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 51));
	sysbus_mmio_map(SYS_BUS_DEVICE(p25), 0, 0x40003c00);

	STM32F1I2C *p26 = g_new(STM32F1I2C, 1);
	object_initialize_child(OBJECT(sms), "I2C1", p26, TYPE_STM32F1_I2C);
	qdev_prop_set_chr(DEVICE(p26), "chardev", qemu_chr_new("soc-i2c1", "chardev:i2c1", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p26), &error_fatal);
	p26->base = 0x40005400;
	sysbus_connect_irq(SYS_BUS_DEVICE(p26), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 31));
	sysbus_connect_irq(SYS_BUS_DEVICE(p26), 1, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 32));
	sysbus_mmio_map(SYS_BUS_DEVICE(p26), 0, 0x40005400);

	STM32F1I2C *p27 = g_new(STM32F1I2C, 1);
	object_initialize_child(OBJECT(sms), "I2C2", p27, TYPE_STM32F1_I2C);
	qdev_prop_set_chr(DEVICE(p27), "chardev", qemu_chr_new("soc-i2c2", "chardev:i2c2", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p27), &error_fatal);
	p27->base = 0x40005800;
	sysbus_connect_irq(SYS_BUS_DEVICE(p27), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 33));
	sysbus_connect_irq(SYS_BUS_DEVICE(p27), 1, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 34));
	sysbus_mmio_map(SYS_BUS_DEVICE(p27), 0, 0x40005800);

	STM32F1IWDG *p28 = g_new(STM32F1IWDG, 1);
	object_initialize_child(OBJECT(sms), "IWDG", p28, TYPE_STM32F1_IWDG);
	sysbus_realize(SYS_BUS_DEVICE(p28), &error_fatal);
	p28->base = 0x40003000;
	sysbus_mmio_map(SYS_BUS_DEVICE(p28), 0, 0x40003000);


}

static void stm32f103_common_init(MachineState *machine) {
	STM32F103MachineState *sms = STM32F103_MACHINE(machine);
	MemoryRegion *sysmem = get_system_memory();
	MemoryRegion *mem;
	Clock *cpuclk = clock_new(OBJECT(sms), "SYSCLK");
	clock_set_hz(cpuclk, 72000000);
	Clock *refclk = clock_new(OBJECT(sms), "REFCLK");
	clock_set_hz(refclk, 72000000);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_rom(mem, NULL, "main-flash", 0x80000, &error_fatal);
	memory_region_add_subregion(sysmem, 0x8000000, mem);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_rom(mem, NULL, "option-bytes", 0x810, &error_fatal);
	memory_region_add_subregion(sysmem, 0x1ffff000, mem);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_ram(mem, NULL, "sram", 0x14000, &error_fatal);
	memory_region_add_subregion(sysmem, 0x20000000, mem);


	object_initialize_child(OBJECT(sms), "armv7m", &(sms->armv7m), TYPE_ARMV7M);
	qdev_prop_set_uint32(DEVICE(&(sms->armv7m)),"num-irq", 256);
	qdev_prop_set_string(DEVICE(&(sms->armv7m)), "cpu-type", machine->cpu_type);
	qdev_prop_set_bit(DEVICE(&(sms->armv7m)), "enable-bitband", true);
	qdev_connect_clock_in(DEVICE(&(sms->armv7m)), "cpuclk", cpuclk);
	qdev_connect_clock_in(DEVICE(&(sms->armv7m)), "refclk", refclk);
	qdev_prop_set_uint32(DEVICE(&(sms->armv7m)), "init-nsvtor", 0x8000000);
	object_property_set_link(OBJECT(&sms->armv7m), "memory", OBJECT(sysmem), &error_abort);
	sysbus_realize(SYS_BUS_DEVICE(&sms->armv7m), &error_fatal);

	stm32f103_periph_init(machine);

	armv7m_load_kernel(ARM_CPU(first_cpu), machine->kernel_filename, 0x8000000, 0x80000);
}

static void stm32f103_class_init(ObjectClass *oc, void *data) {
	MachineClass *mc = MACHINE_CLASS(oc);
	mc->desc = "stm32f103";
	mc->init = stm32f103_common_init;
	mc->max_cpus = 1;
	mc->default_cpus = 1;
	mc->default_cpu_type = ARM_CPU_TYPE_NAME("cortex-m3");
}

static const TypeInfo stm32f103_info = {
	.name = TYPE_STM32F103_MACHINE,
	.parent = TYPE_MACHINE,
	.instance_size = sizeof(STM32F103MachineState),
	.class_size = sizeof(STM32F103MachineClass),
	.class_init = stm32f103_class_init
};

static void stm32f103_machine_init(void) {
	type_register_static(&stm32f103_info);
}

type_init(stm32f103_machine_init);
