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


#define TYPE_STM32L0_DMA "stm32l0-dma"
OBJECT_DECLARE_SIMPLE_TYPE(STM32L0DMA, STM32L0_DMA)

struct STM32L0DMA {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[3];

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
	uint32_t CSELR;
	/* dma channel enable flags*/
	uint8_t channel_enabled[7];
	/* base */
	uint32_t base;

};
#define STM32L0_DMA_SIZE				0xac

REG32(DMA_ISR, 0x0)
	FIELD(DMA_ISR, TEIF7, 27, 1)
	FIELD(DMA_ISR, HTIF7, 26, 1)
	FIELD(DMA_ISR, TCIF7, 25, 1)
	FIELD(DMA_ISR, GIF7, 24, 1)
	FIELD(DMA_ISR, TEIF6, 23, 1)
	FIELD(DMA_ISR, HTIF6, 22, 1)
	FIELD(DMA_ISR, TCIF6, 21, 1)
	FIELD(DMA_ISR, GIF6, 20, 1)
	FIELD(DMA_ISR, TEIF5, 19, 1)
	FIELD(DMA_ISR, HTIF5, 18, 1)
	FIELD(DMA_ISR, TCIF5, 17, 1)
	FIELD(DMA_ISR, GIF5, 16, 1)
	FIELD(DMA_ISR, TEIF4, 15, 1)
	FIELD(DMA_ISR, HTIF4, 14, 1)
	FIELD(DMA_ISR, TCIF4, 13, 1)
	FIELD(DMA_ISR, GIF4, 12, 1)
	FIELD(DMA_ISR, TEIF3, 11, 1)
	FIELD(DMA_ISR, HTIF3, 10, 1)
	FIELD(DMA_ISR, TCIF3, 9, 1)
	FIELD(DMA_ISR, GIF3, 8, 1)
	FIELD(DMA_ISR, TEIF2, 7, 1)
	FIELD(DMA_ISR, HTIF2, 6, 1)
	FIELD(DMA_ISR, TCIF2, 5, 1)
	FIELD(DMA_ISR, GIF2, 4, 1)
	FIELD(DMA_ISR, TEIF1, 3, 1)
	FIELD(DMA_ISR, HTIF1, 2, 1)
	FIELD(DMA_ISR, TCIF1, 1, 1)
	FIELD(DMA_ISR, GIF1, 0, 1)
REG32(DMA_IFCR, 0x4)
	FIELD(DMA_IFCR, CTEIF7, 27, 1)
	FIELD(DMA_IFCR, CHTIF7, 26, 1)
	FIELD(DMA_IFCR, CTCIF7, 25, 1)
	FIELD(DMA_IFCR, CGIF7, 24, 1)
	FIELD(DMA_IFCR, CTEIF6, 23, 1)
	FIELD(DMA_IFCR, CHTIF6, 22, 1)
	FIELD(DMA_IFCR, CTCIF6, 21, 1)
	FIELD(DMA_IFCR, CGIF6, 20, 1)
	FIELD(DMA_IFCR, CTEIF5, 19, 1)
	FIELD(DMA_IFCR, CHTIF5, 18, 1)
	FIELD(DMA_IFCR, CTCIF5, 17, 1)
	FIELD(DMA_IFCR, CGIF5, 16, 1)
	FIELD(DMA_IFCR, CTEIF4, 15, 1)
	FIELD(DMA_IFCR, CHTIF4, 14, 1)
	FIELD(DMA_IFCR, CTCIF4, 13, 1)
	FIELD(DMA_IFCR, CGIF4, 12, 1)
	FIELD(DMA_IFCR, CTEIF3, 11, 1)
	FIELD(DMA_IFCR, CHTIF3, 10, 1)
	FIELD(DMA_IFCR, CTCIF3, 9, 1)
	FIELD(DMA_IFCR, CGIF3, 8, 1)
	FIELD(DMA_IFCR, CTEIF2, 7, 1)
	FIELD(DMA_IFCR, CHTIF2, 6, 1)
	FIELD(DMA_IFCR, CTCIF2, 5, 1)
	FIELD(DMA_IFCR, CGIF2, 4, 1)
	FIELD(DMA_IFCR, CTEIF1, 3, 1)
	FIELD(DMA_IFCR, CHTIF1, 2, 1)
	FIELD(DMA_IFCR, CTCIF1, 1, 1)
	FIELD(DMA_IFCR, CGIF1, 0, 1)
REG32(DMA_CCR1, 0x8)
	FIELD(DMA_CCR1, MEM2MEM, 14, 1)
	FIELD(DMA_CCR1, PL, 12, 2)
	FIELD(DMA_CCR1, MSIZE, 10, 2)
	FIELD(DMA_CCR1, PSIZE, 8, 2)
	FIELD(DMA_CCR1, MINC, 7, 1)
	FIELD(DMA_CCR1, PINC, 6, 1)
	FIELD(DMA_CCR1, CIRC, 5, 1)
	FIELD(DMA_CCR1, DIR, 4, 1)
	FIELD(DMA_CCR1, TEIE, 3, 1)
	FIELD(DMA_CCR1, HTIE, 2, 1)
	FIELD(DMA_CCR1, TCIE, 1, 1)
	FIELD(DMA_CCR1, EN, 0, 1)
REG32(DMA_CNDTR1, 0xc)
	FIELD(DMA_CNDTR1, NDT, 0, 16)
REG32(DMA_CPAR1, 0x10)
	FIELD(DMA_CPAR1, PA, 0, 32)
REG32(DMA_CMAR1, 0x14)
	FIELD(DMA_CMAR1, MA, 0, 32)
REG32(DMA_CCR2, 0x1c)
	FIELD(DMA_CCR2, MEM2MEM, 14, 1)
	FIELD(DMA_CCR2, PL, 12, 2)
	FIELD(DMA_CCR2, MSIZE, 10, 2)
	FIELD(DMA_CCR2, PSIZE, 8, 2)
	FIELD(DMA_CCR2, MINC, 7, 1)
	FIELD(DMA_CCR2, PINC, 6, 1)
	FIELD(DMA_CCR2, CIRC, 5, 1)
	FIELD(DMA_CCR2, DIR, 4, 1)
	FIELD(DMA_CCR2, TEIE, 3, 1)
	FIELD(DMA_CCR2, HTIE, 2, 1)
	FIELD(DMA_CCR2, TCIE, 1, 1)
	FIELD(DMA_CCR2, EN, 0, 1)
REG32(DMA_CNDTR2, 0x20)
	FIELD(DMA_CNDTR2, NDT, 0, 16)
REG32(DMA_CPAR2, 0x24)
	FIELD(DMA_CPAR2, PA, 0, 32)
REG32(DMA_CMAR2, 0x28)
	FIELD(DMA_CMAR2, MA, 0, 32)
REG32(DMA_CCR3, 0x30)
	FIELD(DMA_CCR3, MEM2MEM, 14, 1)
	FIELD(DMA_CCR3, PL, 12, 2)
	FIELD(DMA_CCR3, MSIZE, 10, 2)
	FIELD(DMA_CCR3, PSIZE, 8, 2)
	FIELD(DMA_CCR3, MINC, 7, 1)
	FIELD(DMA_CCR3, PINC, 6, 1)
	FIELD(DMA_CCR3, CIRC, 5, 1)
	FIELD(DMA_CCR3, DIR, 4, 1)
	FIELD(DMA_CCR3, TEIE, 3, 1)
	FIELD(DMA_CCR3, HTIE, 2, 1)
	FIELD(DMA_CCR3, TCIE, 1, 1)
	FIELD(DMA_CCR3, EN, 0, 1)
REG32(DMA_CNDTR3, 0x34)
	FIELD(DMA_CNDTR3, NDT, 0, 16)
REG32(DMA_CPAR3, 0x38)
	FIELD(DMA_CPAR3, PA, 0, 32)
REG32(DMA_CMAR3, 0x3c)
	FIELD(DMA_CMAR3, MA, 0, 32)
REG32(DMA_CCR4, 0x44)
	FIELD(DMA_CCR4, MEM2MEM, 14, 1)
	FIELD(DMA_CCR4, PL, 12, 2)
	FIELD(DMA_CCR4, MSIZE, 10, 2)
	FIELD(DMA_CCR4, PSIZE, 8, 2)
	FIELD(DMA_CCR4, MINC, 7, 1)
	FIELD(DMA_CCR4, PINC, 6, 1)
	FIELD(DMA_CCR4, CIRC, 5, 1)
	FIELD(DMA_CCR4, DIR, 4, 1)
	FIELD(DMA_CCR4, TEIE, 3, 1)
	FIELD(DMA_CCR4, HTIE, 2, 1)
	FIELD(DMA_CCR4, TCIE, 1, 1)
	FIELD(DMA_CCR4, EN, 0, 1)
REG32(DMA_CNDTR4, 0x48)
	FIELD(DMA_CNDTR4, NDT, 0, 16)
REG32(DMA_CPAR4, 0x4c)
	FIELD(DMA_CPAR4, PA, 0, 32)
REG32(DMA_CMAR4, 0x50)
	FIELD(DMA_CMAR4, MA, 0, 32)
REG32(DMA_CCR5, 0x58)
	FIELD(DMA_CCR5, MEM2MEM, 14, 1)
	FIELD(DMA_CCR5, PL, 12, 2)
	FIELD(DMA_CCR5, MSIZE, 10, 2)
	FIELD(DMA_CCR5, PSIZE, 8, 2)
	FIELD(DMA_CCR5, MINC, 7, 1)
	FIELD(DMA_CCR5, PINC, 6, 1)
	FIELD(DMA_CCR5, CIRC, 5, 1)
	FIELD(DMA_CCR5, DIR, 4, 1)
	FIELD(DMA_CCR5, TEIE, 3, 1)
	FIELD(DMA_CCR5, HTIE, 2, 1)
	FIELD(DMA_CCR5, TCIE, 1, 1)
	FIELD(DMA_CCR5, EN, 0, 1)
REG32(DMA_CNDTR5, 0x5c)
	FIELD(DMA_CNDTR5, NDT, 0, 16)
REG32(DMA_CPAR5, 0x60)
	FIELD(DMA_CPAR5, PA, 0, 32)
REG32(DMA_CMAR5, 0x64)
	FIELD(DMA_CMAR5, MA, 0, 32)
REG32(DMA_CCR6, 0x6c)
	FIELD(DMA_CCR6, MEM2MEM, 14, 1)
	FIELD(DMA_CCR6, PL, 12, 2)
	FIELD(DMA_CCR6, MSIZE, 10, 2)
	FIELD(DMA_CCR6, PSIZE, 8, 2)
	FIELD(DMA_CCR6, MINC, 7, 1)
	FIELD(DMA_CCR6, PINC, 6, 1)
	FIELD(DMA_CCR6, CIRC, 5, 1)
	FIELD(DMA_CCR6, DIR, 4, 1)
	FIELD(DMA_CCR6, TEIE, 3, 1)
	FIELD(DMA_CCR6, HTIE, 2, 1)
	FIELD(DMA_CCR6, TCIE, 1, 1)
	FIELD(DMA_CCR6, EN, 0, 1)
REG32(DMA_CNDTR6, 0x70)
	FIELD(DMA_CNDTR6, NDT, 0, 16)
REG32(DMA_CPAR6, 0x74)
	FIELD(DMA_CPAR6, PA, 0, 32)
REG32(DMA_CMAR6, 0x78)
	FIELD(DMA_CMAR6, MA, 0, 32)
REG32(DMA_CCR7, 0x80)
	FIELD(DMA_CCR7, MEM2MEM, 14, 1)
	FIELD(DMA_CCR7, PL, 12, 2)
	FIELD(DMA_CCR7, MSIZE, 10, 2)
	FIELD(DMA_CCR7, PSIZE, 8, 2)
	FIELD(DMA_CCR7, MINC, 7, 1)
	FIELD(DMA_CCR7, PINC, 6, 1)
	FIELD(DMA_CCR7, CIRC, 5, 1)
	FIELD(DMA_CCR7, DIR, 4, 1)
	FIELD(DMA_CCR7, TEIE, 3, 1)
	FIELD(DMA_CCR7, HTIE, 2, 1)
	FIELD(DMA_CCR7, TCIE, 1, 1)
	FIELD(DMA_CCR7, EN, 0, 1)
REG32(DMA_CNDTR7, 0x84)
	FIELD(DMA_CNDTR7, NDT, 0, 16)
REG32(DMA_CPAR7, 0x88)
	FIELD(DMA_CPAR7, PA, 0, 32)
REG32(DMA_CMAR7, 0x8c)
	FIELD(DMA_CMAR7, MA, 0, 32)
REG32(DMA_CSELR, 0xa8)
	FIELD(DMA_CSELR, C7S, 24, 4)
	FIELD(DMA_CSELR, C6S, 20, 4)
	FIELD(DMA_CSELR, C5S, 16, 4)
	FIELD(DMA_CSELR, C4S, 12, 4)
	FIELD(DMA_CSELR, C3S, 8, 4)
	FIELD(DMA_CSELR, C2S, 4, 4)
	FIELD(DMA_CSELR, C1S, 0, 4)


__attribute__((unused))
static void stm32l0_dma_update(STM32L0DMA *t, int channel_idx, int level) {
	qemu_set_irq(t->irq[channel_idx], level);
}

static void stm32l0_dma_receive(struct STM32L0DMA *t, uint32_t addr, uint8_t data) {
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
				if ((((!(t->CCR1 & 0x800)) && (!(t->CCR1 & 0x10)) && (t->CCR1 & 0x4000) && (!(t->CCR1 & 0x100))) || ((!(t->CCR1 & 0x800)) && (!(t->CCR1 & 0x200)) && (!(t->CCR1 & 0x10)) && (t->CCR1 & 0x4000)) || ((!(t->CCR1 & 0x200)) && (!(t->CCR1 & 0x10)) && (!(t->CCR1 & 0x400)) && (t->CCR1 & 0x4000)) || ((!(t->CCR1 & 0x10)) && (!(t->CCR1 & 0x400)) && (t->CCR1 & 0x4000) && (!(t->CCR1 & 0x100))))) {
					cpu_physical_memory_write(t->CPAR1, &data, 1);
					t->CPAR1 += 1;
				} else {
					cpu_physical_memory_write(t->CMAR1, &data, 1);
					t->CMAR1 += 1;
				}
				t->CNDTR1 -= 1;
			}
			if (t->CNDTR1 == 0) {
				t->CCR1 |= 0x2;
				t->ISR &= (~(0x4));
				t->CCR1 &= (~(0x4));
				t->ISR |= 0x2;

				do_update = 1;
			}
			break;
		}

		case 1: {
			if (t->CNDTR2) {
				if ((((!(t->CCR2 & 0x400)) && (!(t->CCR2 & 0x10)) && (!(t->CCR2 & 0x200)) && (t->CCR2 & 0x4000)) || ((!(t->CCR2 & 0x800)) && (!(t->CCR2 & 0x10)) && (!(t->CCR2 & 0x200)) && (t->CCR2 & 0x4000)) || ((!(t->CCR2 & 0x800)) && (!(t->CCR2 & 0x100)) && (!(t->CCR2 & 0x10)) && (t->CCR2 & 0x4000)) || ((!(t->CCR2 & 0x400)) && (!(t->CCR2 & 0x100)) && (!(t->CCR2 & 0x10)) && (t->CCR2 & 0x4000)))) {
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
				t->CCR2 &= (~(0x4));
				t->ISR |= 0x20;
				t->ISR &= (~(0x40));

				do_update = 1;
			}
			break;
		}

		case 2: {
			if (t->CNDTR3) {
				if ((((t->CCR3 & 0x4000) && (!(t->CCR3 & 0x400)) && (!(t->CCR3 & 0x100)) && (!(t->CCR3 & 0x10))) || ((t->CCR3 & 0x4000) && (!(t->CCR3 & 0x800)) && (!(t->CCR3 & 0x100)) && (!(t->CCR3 & 0x10))) || ((t->CCR3 & 0x4000) && (!(t->CCR3 & 0x800)) && (!(t->CCR3 & 0x200)) && (!(t->CCR3 & 0x10))) || ((t->CCR3 & 0x4000) && (!(t->CCR3 & 0x400)) && (!(t->CCR3 & 0x200)) && (!(t->CCR3 & 0x10))))) {
					cpu_physical_memory_write(t->CPAR3, &data, 1);
					t->CPAR3 += 1;
				} else {
					cpu_physical_memory_write(t->CMAR3, &data, 1);
					t->CMAR3 += 1;
				}
				t->CNDTR3 -= 1;
			}
			if (t->CNDTR3 == 0) {
				t->ISR &= (~(0x400));
				t->CCR3 &= (~(0x4));
				t->CCR3 |= 0x2;
				t->ISR |= 0x200;

				do_update = 1;
			}
			break;
		}

		case 3: {
			if (t->CNDTR4) {
				if ((((!(t->CCR4 & 0x10)) && (!(t->CCR4 & 0x400)) && (t->CCR4 & 0x4000) && (!(t->CCR4 & 0x100))) || ((!(t->CCR4 & 0x10)) && (!(t->CCR4 & 0x200)) && (!(t->CCR4 & 0x800)) && (t->CCR4 & 0x4000)) || ((!(t->CCR4 & 0x10)) && (!(t->CCR4 & 0x400)) && (!(t->CCR4 & 0x200)) && (t->CCR4 & 0x4000)) || ((!(t->CCR4 & 0x10)) && (!(t->CCR4 & 0x800)) && (t->CCR4 & 0x4000) && (!(t->CCR4 & 0x100))))) {
					cpu_physical_memory_write(t->CPAR4, &data, 1);
					t->CPAR4 += 1;
				} else {
					cpu_physical_memory_write(t->CMAR4, &data, 1);
					t->CMAR4 += 1;
				}
				t->CNDTR4 -= 1;
			}
			if (t->CNDTR4 == 0) {
				t->ISR |= 0x2000;
				t->CCR4 |= 0x2;
				t->ISR &= (~(0x4000));
				t->CCR4 &= (~(0x4));

				do_update = 1;
			}
			break;
		}

		case 4: {
			if (t->CNDTR5) {
				if ((((!(t->CCR5 & 0x100)) && (!(t->CCR5 & 0x400)) && (!(t->CCR5 & 0x10)) && (t->CCR5 & 0x4000)) || ((!(t->CCR5 & 0x100)) && (!(t->CCR5 & 0x10)) && (!(t->CCR5 & 0x800)) && (t->CCR5 & 0x4000)) || ((!(t->CCR5 & 0x200)) && (!(t->CCR5 & 0x400)) && (!(t->CCR5 & 0x10)) && (t->CCR5 & 0x4000)) || ((!(t->CCR5 & 0x200)) && (!(t->CCR5 & 0x10)) && (!(t->CCR5 & 0x800)) && (t->CCR5 & 0x4000)))) {
					cpu_physical_memory_write(t->CPAR5, &data, 1);
					t->CPAR5 += 1;
				} else {
					cpu_physical_memory_write(t->CMAR5, &data, 1);
					t->CMAR5 += 1;
				}
				t->CNDTR5 -= 1;
			}
			if (t->CNDTR5 == 0) {
				t->CCR5 |= 0x2;
				t->ISR |= 0x20000;
				t->CCR5 &= (~(0x4));
				t->ISR &= (~(0x40000));

				do_update = 1;
			}
			break;
		}

		case 5: {
			if (t->CNDTR6) {
				if ((((!(t->CCR6 & 0x200)) && (!(t->CCR6 & 0x800)) && (t->CCR6 & 0x4000) && (!(t->CCR6 & 0x10))) || ((!(t->CCR6 & 0x200)) && (t->CCR6 & 0x4000) && (!(t->CCR6 & 0x400)) && (!(t->CCR6 & 0x10))) || ((!(t->CCR6 & 0x100)) && (t->CCR6 & 0x4000) && (!(t->CCR6 & 0x400)) && (!(t->CCR6 & 0x10))) || ((!(t->CCR6 & 0x100)) && (!(t->CCR6 & 0x800)) && (t->CCR6 & 0x4000) && (!(t->CCR6 & 0x10))))) {
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
				t->ISR |= 0x200000;
				t->CCR6 &= (~(0x4));
				t->CCR6 |= 0x2;

				do_update = 1;
			}
			break;
		}

		case 6: {
			if (t->CNDTR7) {
				if ((((!(t->CCR7 & 0x400)) && (!(t->CCR7 & 0x10)) && (!(t->CCR7 & 0x100)) && (t->CCR7 & 0x4000)) || ((!(t->CCR7 & 0x800)) && (!(t->CCR7 & 0x200)) && (!(t->CCR7 & 0x10)) && (t->CCR7 & 0x4000)) || ((!(t->CCR7 & 0x800)) && (!(t->CCR7 & 0x10)) && (!(t->CCR7 & 0x100)) && (t->CCR7 & 0x4000)) || ((!(t->CCR7 & 0x400)) && (!(t->CCR7 & 0x200)) && (!(t->CCR7 & 0x10)) && (t->CCR7 & 0x4000)))) {
					cpu_physical_memory_write(t->CPAR7, &data, 1);
					t->CPAR7 += 1;
				} else {
					cpu_physical_memory_write(t->CMAR7, &data, 1);
					t->CMAR7 += 1;
				}
				t->CNDTR7 -= 1;
			}
			if (t->CNDTR7 == 0) {
				t->CCR7 &= (~(0x4));
				t->ISR |= 0x2000000;
				t->CCR7 |= 0x2;
				t->ISR &= (~(0x4000000));

				do_update = 1;
			}
			break;
		}

		default: break;
	}
	if (do_update) {
		stm32l0_dma_update(t, channel_idx, 1);
		stm32l0_dma_update(t, channel_idx, 0);
	}
}

static void stm32l0_dma_transmit(struct STM32L0DMA *t, int channel_idx) {
	uint8_t data;
	switch (channel_idx) {

		case 0: {
			if (!t->channel_enabled[0]) {
				break;
			}
			if (t->CPAR1 < 0x40000000 && t->CMAR1 < 0x40000000) {
				for (int i = 0; i < t->CNDTR1; ++i) {
					if ((((!(t->CCR1 & 0x800)) && (!(t->CCR1 & 0x10)) && (t->CCR1 & 0x4000) && (!(t->CCR1 & 0x100))) || ((!(t->CCR1 & 0x800)) && (!(t->CCR1 & 0x200)) && (!(t->CCR1 & 0x10)) && (t->CCR1 & 0x4000)) || ((!(t->CCR1 & 0x200)) && (!(t->CCR1 & 0x10)) && (!(t->CCR1 & 0x400)) && (t->CCR1 & 0x4000)) || ((!(t->CCR1 & 0x10)) && (!(t->CCR1 & 0x400)) && (t->CCR1 & 0x4000) && (!(t->CCR1 & 0x100))))) {
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
					if ((((!(t->CCR1 & 0x800)) && (!(t->CCR1 & 0x10)) && (t->CCR1 & 0x4000) && (!(t->CCR1 & 0x100))) || ((!(t->CCR1 & 0x800)) && (!(t->CCR1 & 0x200)) && (!(t->CCR1 & 0x10)) && (t->CCR1 & 0x4000)) || ((!(t->CCR1 & 0x200)) && (!(t->CCR1 & 0x10)) && (!(t->CCR1 & 0x400)) && (t->CCR1 & 0x4000)) || ((!(t->CCR1 & 0x10)) && (!(t->CCR1 & 0x400)) && (t->CCR1 & 0x4000) && (!(t->CCR1 & 0x100))))) {
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
			t->CCR1 |= 0x2;
			t->ISR &= (~(0x4));
			t->CCR1 &= (~(0x4));
			t->ISR |= 0x2;

			break;
		}

		case 1: {
			if (!t->channel_enabled[1]) {
				break;
			}
			if (t->CPAR2 < 0x40000000 && t->CMAR2 < 0x40000000) {
				for (int i = 0; i < t->CNDTR2; ++i) {
					if ((((!(t->CCR2 & 0x400)) && (!(t->CCR2 & 0x10)) && (!(t->CCR2 & 0x200)) && (t->CCR2 & 0x4000)) || ((!(t->CCR2 & 0x800)) && (!(t->CCR2 & 0x10)) && (!(t->CCR2 & 0x200)) && (t->CCR2 & 0x4000)) || ((!(t->CCR2 & 0x800)) && (!(t->CCR2 & 0x100)) && (!(t->CCR2 & 0x10)) && (t->CCR2 & 0x4000)) || ((!(t->CCR2 & 0x400)) && (!(t->CCR2 & 0x100)) && (!(t->CCR2 & 0x10)) && (t->CCR2 & 0x4000)))) {
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
					if ((((!(t->CCR2 & 0x400)) && (!(t->CCR2 & 0x10)) && (!(t->CCR2 & 0x200)) && (t->CCR2 & 0x4000)) || ((!(t->CCR2 & 0x800)) && (!(t->CCR2 & 0x10)) && (!(t->CCR2 & 0x200)) && (t->CCR2 & 0x4000)) || ((!(t->CCR2 & 0x800)) && (!(t->CCR2 & 0x100)) && (!(t->CCR2 & 0x10)) && (t->CCR2 & 0x4000)) || ((!(t->CCR2 & 0x400)) && (!(t->CCR2 & 0x100)) && (!(t->CCR2 & 0x10)) && (t->CCR2 & 0x4000)))) {
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
			t->CCR2 &= (~(0x4));
			t->ISR |= 0x20;
			t->ISR &= (~(0x40));

			break;
		}

		case 2: {
			if (!t->channel_enabled[2]) {
				break;
			}
			if (t->CPAR3 < 0x40000000 && t->CMAR3 < 0x40000000) {
				for (int i = 0; i < t->CNDTR3; ++i) {
					if ((((t->CCR3 & 0x4000) && (!(t->CCR3 & 0x400)) && (!(t->CCR3 & 0x100)) && (!(t->CCR3 & 0x10))) || ((t->CCR3 & 0x4000) && (!(t->CCR3 & 0x800)) && (!(t->CCR3 & 0x100)) && (!(t->CCR3 & 0x10))) || ((t->CCR3 & 0x4000) && (!(t->CCR3 & 0x800)) && (!(t->CCR3 & 0x200)) && (!(t->CCR3 & 0x10))) || ((t->CCR3 & 0x4000) && (!(t->CCR3 & 0x400)) && (!(t->CCR3 & 0x200)) && (!(t->CCR3 & 0x10))))) {
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
					if ((((t->CCR3 & 0x4000) && (!(t->CCR3 & 0x400)) && (!(t->CCR3 & 0x100)) && (!(t->CCR3 & 0x10))) || ((t->CCR3 & 0x4000) && (!(t->CCR3 & 0x800)) && (!(t->CCR3 & 0x100)) && (!(t->CCR3 & 0x10))) || ((t->CCR3 & 0x4000) && (!(t->CCR3 & 0x800)) && (!(t->CCR3 & 0x200)) && (!(t->CCR3 & 0x10))) || ((t->CCR3 & 0x4000) && (!(t->CCR3 & 0x400)) && (!(t->CCR3 & 0x200)) && (!(t->CCR3 & 0x10))))) {
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
			t->ISR &= (~(0x400));
			t->CCR3 &= (~(0x4));
			t->CCR3 |= 0x2;
			t->ISR |= 0x200;

			break;
		}

		case 3: {
			if (!t->channel_enabled[3]) {
				break;
			}
			if (t->CPAR4 < 0x40000000 && t->CMAR4 < 0x40000000) {
				for (int i = 0; i < t->CNDTR4; ++i) {
					if ((((!(t->CCR4 & 0x10)) && (!(t->CCR4 & 0x400)) && (t->CCR4 & 0x4000) && (!(t->CCR4 & 0x100))) || ((!(t->CCR4 & 0x10)) && (!(t->CCR4 & 0x200)) && (!(t->CCR4 & 0x800)) && (t->CCR4 & 0x4000)) || ((!(t->CCR4 & 0x10)) && (!(t->CCR4 & 0x400)) && (!(t->CCR4 & 0x200)) && (t->CCR4 & 0x4000)) || ((!(t->CCR4 & 0x10)) && (!(t->CCR4 & 0x800)) && (t->CCR4 & 0x4000) && (!(t->CCR4 & 0x100))))) {
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
					if ((((!(t->CCR4 & 0x10)) && (!(t->CCR4 & 0x400)) && (t->CCR4 & 0x4000) && (!(t->CCR4 & 0x100))) || ((!(t->CCR4 & 0x10)) && (!(t->CCR4 & 0x200)) && (!(t->CCR4 & 0x800)) && (t->CCR4 & 0x4000)) || ((!(t->CCR4 & 0x10)) && (!(t->CCR4 & 0x400)) && (!(t->CCR4 & 0x200)) && (t->CCR4 & 0x4000)) || ((!(t->CCR4 & 0x10)) && (!(t->CCR4 & 0x800)) && (t->CCR4 & 0x4000) && (!(t->CCR4 & 0x100))))) {
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
			t->ISR |= 0x2000;
			t->CCR4 |= 0x2;
			t->ISR &= (~(0x4000));
			t->CCR4 &= (~(0x4));

			break;
		}

		case 4: {
			if (!t->channel_enabled[4]) {
				break;
			}
			if (t->CPAR5 < 0x40000000 && t->CMAR5 < 0x40000000) {
				for (int i = 0; i < t->CNDTR5; ++i) {
					if ((((!(t->CCR5 & 0x100)) && (!(t->CCR5 & 0x400)) && (!(t->CCR5 & 0x10)) && (t->CCR5 & 0x4000)) || ((!(t->CCR5 & 0x100)) && (!(t->CCR5 & 0x10)) && (!(t->CCR5 & 0x800)) && (t->CCR5 & 0x4000)) || ((!(t->CCR5 & 0x200)) && (!(t->CCR5 & 0x400)) && (!(t->CCR5 & 0x10)) && (t->CCR5 & 0x4000)) || ((!(t->CCR5 & 0x200)) && (!(t->CCR5 & 0x10)) && (!(t->CCR5 & 0x800)) && (t->CCR5 & 0x4000)))) {
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
					if ((((!(t->CCR5 & 0x100)) && (!(t->CCR5 & 0x400)) && (!(t->CCR5 & 0x10)) && (t->CCR5 & 0x4000)) || ((!(t->CCR5 & 0x100)) && (!(t->CCR5 & 0x10)) && (!(t->CCR5 & 0x800)) && (t->CCR5 & 0x4000)) || ((!(t->CCR5 & 0x200)) && (!(t->CCR5 & 0x400)) && (!(t->CCR5 & 0x10)) && (t->CCR5 & 0x4000)) || ((!(t->CCR5 & 0x200)) && (!(t->CCR5 & 0x10)) && (!(t->CCR5 & 0x800)) && (t->CCR5 & 0x4000)))) {
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
			t->CCR5 |= 0x2;
			t->ISR |= 0x20000;
			t->CCR5 &= (~(0x4));
			t->ISR &= (~(0x40000));

			break;
		}

		case 5: {
			if (!t->channel_enabled[5]) {
				break;
			}
			if (t->CPAR6 < 0x40000000 && t->CMAR6 < 0x40000000) {
				for (int i = 0; i < t->CNDTR6; ++i) {
					if ((((!(t->CCR6 & 0x200)) && (!(t->CCR6 & 0x800)) && (t->CCR6 & 0x4000) && (!(t->CCR6 & 0x10))) || ((!(t->CCR6 & 0x200)) && (t->CCR6 & 0x4000) && (!(t->CCR6 & 0x400)) && (!(t->CCR6 & 0x10))) || ((!(t->CCR6 & 0x100)) && (t->CCR6 & 0x4000) && (!(t->CCR6 & 0x400)) && (!(t->CCR6 & 0x10))) || ((!(t->CCR6 & 0x100)) && (!(t->CCR6 & 0x800)) && (t->CCR6 & 0x4000) && (!(t->CCR6 & 0x10))))) {
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
					if ((((!(t->CCR6 & 0x200)) && (!(t->CCR6 & 0x800)) && (t->CCR6 & 0x4000) && (!(t->CCR6 & 0x10))) || ((!(t->CCR6 & 0x200)) && (t->CCR6 & 0x4000) && (!(t->CCR6 & 0x400)) && (!(t->CCR6 & 0x10))) || ((!(t->CCR6 & 0x100)) && (t->CCR6 & 0x4000) && (!(t->CCR6 & 0x400)) && (!(t->CCR6 & 0x10))) || ((!(t->CCR6 & 0x100)) && (!(t->CCR6 & 0x800)) && (t->CCR6 & 0x4000) && (!(t->CCR6 & 0x10))))) {
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
			t->ISR |= 0x200000;
			t->CCR6 &= (~(0x4));
			t->CCR6 |= 0x2;

			break;
		}

		case 6: {
			if (!t->channel_enabled[6]) {
				break;
			}
			if (t->CPAR7 < 0x40000000 && t->CMAR7 < 0x40000000) {
				for (int i = 0; i < t->CNDTR7; ++i) {
					if ((((!(t->CCR7 & 0x400)) && (!(t->CCR7 & 0x10)) && (!(t->CCR7 & 0x100)) && (t->CCR7 & 0x4000)) || ((!(t->CCR7 & 0x800)) && (!(t->CCR7 & 0x200)) && (!(t->CCR7 & 0x10)) && (t->CCR7 & 0x4000)) || ((!(t->CCR7 & 0x800)) && (!(t->CCR7 & 0x10)) && (!(t->CCR7 & 0x100)) && (t->CCR7 & 0x4000)) || ((!(t->CCR7 & 0x400)) && (!(t->CCR7 & 0x200)) && (!(t->CCR7 & 0x10)) && (t->CCR7 & 0x4000)))) {
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
					if ((((!(t->CCR7 & 0x400)) && (!(t->CCR7 & 0x10)) && (!(t->CCR7 & 0x100)) && (t->CCR7 & 0x4000)) || ((!(t->CCR7 & 0x800)) && (!(t->CCR7 & 0x200)) && (!(t->CCR7 & 0x10)) && (t->CCR7 & 0x4000)) || ((!(t->CCR7 & 0x800)) && (!(t->CCR7 & 0x10)) && (!(t->CCR7 & 0x100)) && (t->CCR7 & 0x4000)) || ((!(t->CCR7 & 0x400)) && (!(t->CCR7 & 0x200)) && (!(t->CCR7 & 0x10)) && (t->CCR7 & 0x4000)))) {
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
			t->CCR7 &= (~(0x4));
			t->ISR |= 0x2000000;
			t->CCR7 |= 0x2;
			t->ISR &= (~(0x4000000));

			break;
		}

		default: break;
	}
	stm32l0_dma_update(t, channel_idx, 1);
	stm32l0_dma_update(t, channel_idx, 0);
}

static void stm32l0_dma_register_reset(STM32L0DMA *t) {
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
	t->CSELR = 0x0;

}

static uint64_t stm32l0_dma_read(void *opaque, hwaddr offset, unsigned size) {
	STM32L0DMA *t = STM32L0_DMA(opaque);
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
		case A_DMA_CSELR:
			ret = t->CSELR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32L0 DMA read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32l0_dma_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32L0DMA *t = STM32L0_DMA(opaque);


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
		case A_DMA_CSELR ... A_DMA_CSELR + 3:
			value = value << ((offset - A_DMA_CSELR) << 3);
			offset = A_DMA_CSELR;
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
					stm32l0_dma_transmit(t, 0);
				} else {
					if ((((!(t->CCR1 & 0x800)) && (!(t->CCR1 & 0x10)) && (t->CCR1 & 0x4000) && (!(t->CCR1 & 0x100))) || ((!(t->CCR1 & 0x800)) && (!(t->CCR1 & 0x200)) && (!(t->CCR1 & 0x10)) && (t->CCR1 & 0x4000)) || ((!(t->CCR1 & 0x200)) && (!(t->CCR1 & 0x10)) && (!(t->CCR1 & 0x400)) && (t->CCR1 & 0x4000)) || ((!(t->CCR1 & 0x10)) && (!(t->CCR1 & 0x400)) && (t->CCR1 & 0x4000) && (!(t->CCR1 & 0x100))))) {
						if (t->CPAR1 < 0x40000000) {
							stm32l0_dma_transmit(t, 0);
						}
					} else {
						if (t->CMAR1 < 0x40000000) {
							stm32l0_dma_transmit(t, 0);
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
					stm32l0_dma_transmit(t, 1);
				} else {
					if ((((!(t->CCR2 & 0x400)) && (!(t->CCR2 & 0x10)) && (!(t->CCR2 & 0x200)) && (t->CCR2 & 0x4000)) || ((!(t->CCR2 & 0x800)) && (!(t->CCR2 & 0x10)) && (!(t->CCR2 & 0x200)) && (t->CCR2 & 0x4000)) || ((!(t->CCR2 & 0x800)) && (!(t->CCR2 & 0x100)) && (!(t->CCR2 & 0x10)) && (t->CCR2 & 0x4000)) || ((!(t->CCR2 & 0x400)) && (!(t->CCR2 & 0x100)) && (!(t->CCR2 & 0x10)) && (t->CCR2 & 0x4000)))) {
						if (t->CPAR2 < 0x40000000) {
							stm32l0_dma_transmit(t, 1);
						}
					} else {
						if (t->CMAR2 < 0x40000000) {
							stm32l0_dma_transmit(t, 1);
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
					stm32l0_dma_transmit(t, 2);
				} else {
					if ((((t->CCR3 & 0x4000) && (!(t->CCR3 & 0x400)) && (!(t->CCR3 & 0x100)) && (!(t->CCR3 & 0x10))) || ((t->CCR3 & 0x4000) && (!(t->CCR3 & 0x800)) && (!(t->CCR3 & 0x100)) && (!(t->CCR3 & 0x10))) || ((t->CCR3 & 0x4000) && (!(t->CCR3 & 0x800)) && (!(t->CCR3 & 0x200)) && (!(t->CCR3 & 0x10))) || ((t->CCR3 & 0x4000) && (!(t->CCR3 & 0x400)) && (!(t->CCR3 & 0x200)) && (!(t->CCR3 & 0x10))))) {
						if (t->CPAR3 < 0x40000000) {
							stm32l0_dma_transmit(t, 2);
						}
					} else {
						if (t->CMAR3 < 0x40000000) {
							stm32l0_dma_transmit(t, 2);
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
					stm32l0_dma_transmit(t, 3);
				} else {
					if ((((!(t->CCR4 & 0x10)) && (!(t->CCR4 & 0x400)) && (t->CCR4 & 0x4000) && (!(t->CCR4 & 0x100))) || ((!(t->CCR4 & 0x10)) && (!(t->CCR4 & 0x200)) && (!(t->CCR4 & 0x800)) && (t->CCR4 & 0x4000)) || ((!(t->CCR4 & 0x10)) && (!(t->CCR4 & 0x400)) && (!(t->CCR4 & 0x200)) && (t->CCR4 & 0x4000)) || ((!(t->CCR4 & 0x10)) && (!(t->CCR4 & 0x800)) && (t->CCR4 & 0x4000) && (!(t->CCR4 & 0x100))))) {
						if (t->CPAR4 < 0x40000000) {
							stm32l0_dma_transmit(t, 3);
						}
					} else {
						if (t->CMAR4 < 0x40000000) {
							stm32l0_dma_transmit(t, 3);
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
					stm32l0_dma_transmit(t, 4);
				} else {
					if ((((!(t->CCR5 & 0x100)) && (!(t->CCR5 & 0x400)) && (!(t->CCR5 & 0x10)) && (t->CCR5 & 0x4000)) || ((!(t->CCR5 & 0x100)) && (!(t->CCR5 & 0x10)) && (!(t->CCR5 & 0x800)) && (t->CCR5 & 0x4000)) || ((!(t->CCR5 & 0x200)) && (!(t->CCR5 & 0x400)) && (!(t->CCR5 & 0x10)) && (t->CCR5 & 0x4000)) || ((!(t->CCR5 & 0x200)) && (!(t->CCR5 & 0x10)) && (!(t->CCR5 & 0x800)) && (t->CCR5 & 0x4000)))) {
						if (t->CPAR5 < 0x40000000) {
							stm32l0_dma_transmit(t, 4);
						}
					} else {
						if (t->CMAR5 < 0x40000000) {
							stm32l0_dma_transmit(t, 4);
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
					stm32l0_dma_transmit(t, 5);
				} else {
					if ((((!(t->CCR6 & 0x200)) && (!(t->CCR6 & 0x800)) && (t->CCR6 & 0x4000) && (!(t->CCR6 & 0x10))) || ((!(t->CCR6 & 0x200)) && (t->CCR6 & 0x4000) && (!(t->CCR6 & 0x400)) && (!(t->CCR6 & 0x10))) || ((!(t->CCR6 & 0x100)) && (t->CCR6 & 0x4000) && (!(t->CCR6 & 0x400)) && (!(t->CCR6 & 0x10))) || ((!(t->CCR6 & 0x100)) && (!(t->CCR6 & 0x800)) && (t->CCR6 & 0x4000) && (!(t->CCR6 & 0x10))))) {
						if (t->CPAR6 < 0x40000000) {
							stm32l0_dma_transmit(t, 5);
						}
					} else {
						if (t->CMAR6 < 0x40000000) {
							stm32l0_dma_transmit(t, 5);
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
					stm32l0_dma_transmit(t, 6);
				} else {
					if ((((!(t->CCR7 & 0x400)) && (!(t->CCR7 & 0x10)) && (!(t->CCR7 & 0x100)) && (t->CCR7 & 0x4000)) || ((!(t->CCR7 & 0x800)) && (!(t->CCR7 & 0x200)) && (!(t->CCR7 & 0x10)) && (t->CCR7 & 0x4000)) || ((!(t->CCR7 & 0x800)) && (!(t->CCR7 & 0x10)) && (!(t->CCR7 & 0x100)) && (t->CCR7 & 0x4000)) || ((!(t->CCR7 & 0x400)) && (!(t->CCR7 & 0x200)) && (!(t->CCR7 & 0x10)) && (t->CCR7 & 0x4000)))) {
						if (t->CPAR7 < 0x40000000) {
							stm32l0_dma_transmit(t, 6);
						}
					} else {
						if (t->CMAR7 < 0x40000000) {
							stm32l0_dma_transmit(t, 6);
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
		case A_DMA_CSELR:
			t->CSELR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32L0 DMA write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32l0_dma_ops = {
	.read = stm32l0_dma_read,
	.write = stm32l0_dma_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32l0_dma_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32L0DMA *t = STM32L0_DMA(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32l0_dma_ops, t, "stm32l0-dma", STM32L0_DMA_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 3; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32l0_dma_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32l0_dma_reset_enter(Object *obj, ResetType type) {
	STM32L0DMA *t = STM32L0_DMA(obj);
	stm32l0_dma_register_reset(t);
}

static const VMStateDescription stm32l0_dma_vmstate = {
	.name = "stm32l0-dma",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(ISR, STM32L0DMA),
		VMSTATE_UINT32(IFCR, STM32L0DMA),
		VMSTATE_UINT32(CCR1, STM32L0DMA),
		VMSTATE_UINT32(CNDTR1, STM32L0DMA),
		VMSTATE_UINT32(CPAR1, STM32L0DMA),
		VMSTATE_UINT32(CMAR1, STM32L0DMA),
		VMSTATE_UINT32(CCR2, STM32L0DMA),
		VMSTATE_UINT32(CNDTR2, STM32L0DMA),
		VMSTATE_UINT32(CPAR2, STM32L0DMA),
		VMSTATE_UINT32(CMAR2, STM32L0DMA),
		VMSTATE_UINT32(CCR3, STM32L0DMA),
		VMSTATE_UINT32(CNDTR3, STM32L0DMA),
		VMSTATE_UINT32(CPAR3, STM32L0DMA),
		VMSTATE_UINT32(CMAR3, STM32L0DMA),
		VMSTATE_UINT32(CCR4, STM32L0DMA),
		VMSTATE_UINT32(CNDTR4, STM32L0DMA),
		VMSTATE_UINT32(CPAR4, STM32L0DMA),
		VMSTATE_UINT32(CMAR4, STM32L0DMA),
		VMSTATE_UINT32(CCR5, STM32L0DMA),
		VMSTATE_UINT32(CNDTR5, STM32L0DMA),
		VMSTATE_UINT32(CPAR5, STM32L0DMA),
		VMSTATE_UINT32(CMAR5, STM32L0DMA),
		VMSTATE_UINT32(CCR6, STM32L0DMA),
		VMSTATE_UINT32(CNDTR6, STM32L0DMA),
		VMSTATE_UINT32(CPAR6, STM32L0DMA),
		VMSTATE_UINT32(CMAR6, STM32L0DMA),
		VMSTATE_UINT32(CCR7, STM32L0DMA),
		VMSTATE_UINT32(CNDTR7, STM32L0DMA),
		VMSTATE_UINT32(CPAR7, STM32L0DMA),
		VMSTATE_UINT32(CMAR7, STM32L0DMA),
		VMSTATE_UINT32(CSELR, STM32L0DMA),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32l0_dma_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32l0_dma_vmstate;
	dc->realize = stm32l0_dma_realize;
	rc->phases.enter = stm32l0_dma_reset_enter;

}

static const TypeInfo stm32l0_dma_info = {
	.name = TYPE_STM32L0_DMA,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32L0DMA),
	.instance_init = stm32l0_dma_init,
	.class_init = stm32l0_dma_class_init,
};

static void stm32l0_dma_register_types(void) {
	type_register_static(&stm32l0_dma_info);
}

type_init(stm32l0_dma_register_types);

#define TYPE_STM32L0_PWR "stm32l0-pwr"
OBJECT_DECLARE_SIMPLE_TYPE(STM32L0PWR, STM32L0_PWR)

struct STM32L0PWR {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/*registers*/
	uint32_t CR;
	uint32_t CSR;
	/* base */
	uint32_t base;

};
#define STM32L0_PWR_SIZE				0x8

REG32(PWR_CR, 0x0)
	FIELD(PWR_CR, LPDS, 0, 1)
	FIELD(PWR_CR, PDDS, 1, 1)
	FIELD(PWR_CR, CWUF, 2, 1)
	FIELD(PWR_CR, CSBF, 3, 1)
	FIELD(PWR_CR, PVDE, 4, 1)
	FIELD(PWR_CR, PLS, 5, 3)
	FIELD(PWR_CR, DBP, 8, 1)
	FIELD(PWR_CR, ULP, 9, 1)
	FIELD(PWR_CR, FWU, 10, 1)
	FIELD(PWR_CR, VOS, 11, 2)
	FIELD(PWR_CR, DS_EE_KOFF, 13, 1)
	FIELD(PWR_CR, LPRUN, 14, 1)
REG32(PWR_CSR, 0x4)
	FIELD(PWR_CSR, BRE, 9, 1)
	FIELD(PWR_CSR, EWUP, 8, 1)
	FIELD(PWR_CSR, BRR, 3, 1)
	FIELD(PWR_CSR, PVDO, 2, 1)
	FIELD(PWR_CSR, SBF, 1, 1)
	FIELD(PWR_CSR, WUF, 0, 1)
	FIELD(PWR_CSR, VOSF, 4, 1)
	FIELD(PWR_CSR, REGLPF, 5, 1)


static void stm32l0_pwr_register_reset(STM32L0PWR *t) {
	t->CR = 0x1000;
	t->CSR = 0x0;

}

static uint64_t stm32l0_pwr_read(void *opaque, hwaddr offset, unsigned size) {
	STM32L0PWR *t = STM32L0_PWR(opaque);
	uint64_t ret;

	switch (offset) {
		case A_PWR_CR:
			ret = t->CR;
			break;
		case A_PWR_CSR:
			ret = t->CSR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32L0 PWR read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32l0_pwr_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32L0PWR *t = STM32L0_PWR(opaque);


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
			qemu_log_mask(LOG_GUEST_ERROR, "STM32L0 PWR write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32l0_pwr_ops = {
	.read = stm32l0_pwr_read,
	.write = stm32l0_pwr_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32l0_pwr_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32L0PWR *t = STM32L0_PWR(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32l0_pwr_ops, t, "stm32l0-pwr", STM32L0_PWR_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void stm32l0_pwr_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32l0_pwr_reset_enter(Object *obj, ResetType type) {
	STM32L0PWR *t = STM32L0_PWR(obj);
	stm32l0_pwr_register_reset(t);
}

static const VMStateDescription stm32l0_pwr_vmstate = {
	.name = "stm32l0-pwr",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR, STM32L0PWR),
		VMSTATE_UINT32(CSR, STM32L0PWR),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32l0_pwr_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32l0_pwr_vmstate;
	dc->realize = stm32l0_pwr_realize;
	rc->phases.enter = stm32l0_pwr_reset_enter;

}

static const TypeInfo stm32l0_pwr_info = {
	.name = TYPE_STM32L0_PWR,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32L0PWR),
	.instance_init = stm32l0_pwr_init,
	.class_init = stm32l0_pwr_class_init,
};

static void stm32l0_pwr_register_types(void) {
	type_register_static(&stm32l0_pwr_info);
}

type_init(stm32l0_pwr_register_types);

#define TYPE_STM32L0_FLASH "stm32l0-flash"
OBJECT_DECLARE_SIMPLE_TYPE(STM32L0FLASH, STM32L0_FLASH)

struct STM32L0FLASH {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t ACR;
	uint32_t PECR;
	uint32_t PDKEYR;
	uint32_t PEKEYR;
	uint32_t PRGKEYR;
	uint32_t OPTKEYR;
	uint32_t SR;
	uint32_t OBR;
	uint32_t WRPR;
	/* base */
	uint32_t base;

};
#define STM32L0_FLASH_SIZE				0x24

REG32(FLASH_ACR, 0x0)
	FIELD(FLASH_ACR, LATENCY, 0, 1)
	FIELD(FLASH_ACR, PRFTEN, 1, 1)
	FIELD(FLASH_ACR, SLEEP_PD, 3, 1)
	FIELD(FLASH_ACR, RUN_PD, 4, 1)
	FIELD(FLASH_ACR, DESAB_BUF, 5, 1)
	FIELD(FLASH_ACR, PRE_READ, 6, 1)
REG32(FLASH_PECR, 0x4)
	FIELD(FLASH_PECR, PELOCK, 0, 1)
	FIELD(FLASH_PECR, PRGLOCK, 1, 1)
	FIELD(FLASH_PECR, OPTLOCK, 2, 1)
	FIELD(FLASH_PECR, PROG, 3, 1)
	FIELD(FLASH_PECR, DATA, 4, 1)
	FIELD(FLASH_PECR, FTDW, 8, 1)
	FIELD(FLASH_PECR, ERASE, 9, 1)
	FIELD(FLASH_PECR, FPRG, 10, 1)
	FIELD(FLASH_PECR, PARALLELBANK, 15, 1)
	FIELD(FLASH_PECR, EOPIE, 16, 1)
	FIELD(FLASH_PECR, ERRIE, 17, 1)
	FIELD(FLASH_PECR, OBL_LAUNCH, 18, 1)
REG32(FLASH_PDKEYR, 0x8)
	FIELD(FLASH_PDKEYR, PDKEYR, 0, 32)
REG32(FLASH_PEKEYR, 0xc)
	FIELD(FLASH_PEKEYR, PEKEYR, 0, 32)
REG32(FLASH_PRGKEYR, 0x10)
	FIELD(FLASH_PRGKEYR, PRGKEYR, 0, 32)
REG32(FLASH_OPTKEYR, 0x14)
	FIELD(FLASH_OPTKEYR, OPTKEYR, 0, 32)
REG32(FLASH_SR, 0x18)
	FIELD(FLASH_SR, BSY, 0, 1)
	FIELD(FLASH_SR, EOP, 1, 1)
	FIELD(FLASH_SR, ENDHV, 2, 1)
	FIELD(FLASH_SR, READY, 3, 1)
	FIELD(FLASH_SR, WRPERR, 8, 1)
	FIELD(FLASH_SR, PGAERR, 9, 1)
	FIELD(FLASH_SR, SIZERR, 10, 1)
	FIELD(FLASH_SR, OPTVERR, 11, 1)
	FIELD(FLASH_SR, RDERR, 14, 1)
	FIELD(FLASH_SR, NOTZEROERR, 16, 1)
	FIELD(FLASH_SR, FWWERR, 17, 1)
REG32(FLASH_OBR, 0x1c)
	FIELD(FLASH_OBR, RDPRT, 0, 8)
	FIELD(FLASH_OBR, BOR_LEV, 16, 4)
	FIELD(FLASH_OBR, SPRMOD, 8, 1)
REG32(FLASH_WRPR, 0x20)
	FIELD(FLASH_WRPR, WRP, 0, 16)


static void stm32l0_flash_register_reset(STM32L0FLASH *t) {
	t->ACR = 0x0;
	t->PECR = 0x7;
	t->PDKEYR = 0x0;
	t->PEKEYR = 0x0;
	t->PRGKEYR = 0x0;
	t->OPTKEYR = 0x0;
	t->SR = 0x4;
	t->OBR = 0xf80000;
	t->WRPR = 0x0;

}

static uint64_t stm32l0_flash_read(void *opaque, hwaddr offset, unsigned size) {
	STM32L0FLASH *t = STM32L0_FLASH(opaque);
	uint64_t ret;

	switch (offset) {
		case A_FLASH_ACR:
			ret = t->ACR;
			break;
		case A_FLASH_PECR:
			ret = t->PECR;
			break;
		case A_FLASH_SR:
			ret = t->SR;
			break;
		case A_FLASH_OBR:
			ret = t->OBR;
			break;
		case A_FLASH_WRPR:
			ret = t->WRPR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32L0 FLASH read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32l0_flash_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32L0FLASH *t = STM32L0_FLASH(opaque);


	switch (offset) {
		case A_FLASH_ACR ... A_FLASH_ACR + 3:
			value = value << ((offset - A_FLASH_ACR) << 3);
			offset = A_FLASH_ACR;
			break;
		case A_FLASH_PECR ... A_FLASH_PECR + 3:
			value = value << ((offset - A_FLASH_PECR) << 3);
			offset = A_FLASH_PECR;
			break;
		case A_FLASH_PDKEYR ... A_FLASH_PDKEYR + 3:
			value = value << ((offset - A_FLASH_PDKEYR) << 3);
			offset = A_FLASH_PDKEYR;
			break;
		case A_FLASH_PEKEYR ... A_FLASH_PEKEYR + 3:
			value = value << ((offset - A_FLASH_PEKEYR) << 3);
			offset = A_FLASH_PEKEYR;
			break;
		case A_FLASH_PRGKEYR ... A_FLASH_PRGKEYR + 3:
			value = value << ((offset - A_FLASH_PRGKEYR) << 3);
			offset = A_FLASH_PRGKEYR;
			break;
		case A_FLASH_OPTKEYR ... A_FLASH_OPTKEYR + 3:
			value = value << ((offset - A_FLASH_OPTKEYR) << 3);
			offset = A_FLASH_OPTKEYR;
			break;
		case A_FLASH_SR ... A_FLASH_SR + 3:
			value = value << ((offset - A_FLASH_SR) << 3);
			offset = A_FLASH_SR;
			break;
		case A_FLASH_WRPR ... A_FLASH_WRPR + 3:
			value = value << ((offset - A_FLASH_WRPR) << 3);
			offset = A_FLASH_WRPR;
			break;

		default: break;
	}

	switch (offset) {
		case A_FLASH_ACR:
			t->ACR = value;
			break;
		case A_FLASH_PECR:
			t->PECR = value;
			break;
		case A_FLASH_PDKEYR:
			t->PDKEYR = value;
			break;
		case A_FLASH_PEKEYR:
			t->PEKEYR = value;
			break;
		case A_FLASH_PRGKEYR:
			t->PRGKEYR = value;
			break;
		case A_FLASH_OPTKEYR:
			t->OPTKEYR = value;
			break;
		case A_FLASH_SR:
			t->SR = value;
			break;
		case A_FLASH_WRPR:
			t->WRPR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32L0 FLASH write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32l0_flash_ops = {
	.read = stm32l0_flash_read,
	.write = stm32l0_flash_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32l0_flash_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32L0FLASH *t = STM32L0_FLASH(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32l0_flash_ops, t, "stm32l0-flash", STM32L0_FLASH_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32l0_flash_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32l0_flash_reset_enter(Object *obj, ResetType type) {
	STM32L0FLASH *t = STM32L0_FLASH(obj);
	stm32l0_flash_register_reset(t);
}

static const VMStateDescription stm32l0_flash_vmstate = {
	.name = "stm32l0-flash",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(ACR, STM32L0FLASH),
		VMSTATE_UINT32(PECR, STM32L0FLASH),
		VMSTATE_UINT32(PDKEYR, STM32L0FLASH),
		VMSTATE_UINT32(PEKEYR, STM32L0FLASH),
		VMSTATE_UINT32(PRGKEYR, STM32L0FLASH),
		VMSTATE_UINT32(OPTKEYR, STM32L0FLASH),
		VMSTATE_UINT32(SR, STM32L0FLASH),
		VMSTATE_UINT32(OBR, STM32L0FLASH),
		VMSTATE_UINT32(WRPR, STM32L0FLASH),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32l0_flash_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32l0_flash_vmstate;
	dc->realize = stm32l0_flash_realize;
	rc->phases.enter = stm32l0_flash_reset_enter;

}

static const TypeInfo stm32l0_flash_info = {
	.name = TYPE_STM32L0_FLASH,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32L0FLASH),
	.instance_init = stm32l0_flash_init,
	.class_init = stm32l0_flash_class_init,
};

static void stm32l0_flash_register_types(void) {
	type_register_static(&stm32l0_flash_info);
}

type_init(stm32l0_flash_register_types);

#define TYPE_STM32L0_RCC "stm32l0-rcc"
OBJECT_DECLARE_SIMPLE_TYPE(STM32L0RCC, STM32L0_RCC)

struct STM32L0RCC {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t CR;
	uint32_t ICSCR;
	uint32_t CRRCR;
	uint32_t CFGR;
	uint32_t CIER;
	uint32_t CIFR;
	uint32_t CICR;
	uint32_t IOPRSTR;
	uint32_t AHBRSTR;
	uint32_t APB2RSTR;
	uint32_t APB1RSTR;
	uint32_t IOPENR;
	uint32_t AHBENR;
	uint32_t APB2ENR;
	uint32_t APB1ENR;
	uint32_t IOPSMEN;
	uint32_t AHBSMENR;
	uint32_t APB2SMENR;
	uint32_t APB1SMENR;
	uint32_t CCIPR;
	uint32_t CSR;
	/* base */
	uint32_t base;

};
#define STM32L0_RCC_SIZE				0x54

REG32(RCC_CR, 0x0)
	FIELD(RCC_CR, PLLRDY, 25, 1)
	FIELD(RCC_CR, PLLON, 24, 1)
	FIELD(RCC_CR, RTCPRE, 20, 2)
	FIELD(RCC_CR, CSSLSEON, 19, 1)
	FIELD(RCC_CR, HSEBYP, 18, 1)
	FIELD(RCC_CR, HSERDY, 17, 1)
	FIELD(RCC_CR, HSEON, 16, 1)
	FIELD(RCC_CR, MSIRDY, 9, 1)
	FIELD(RCC_CR, MSION, 8, 1)
	FIELD(RCC_CR, HSI16DIVF, 4, 1)
	FIELD(RCC_CR, HSI16DIVEN, 3, 1)
	FIELD(RCC_CR, HSI16RDYF, 2, 1)
	FIELD(RCC_CR, HSI16KERON, 1, 1)
	FIELD(RCC_CR, HSI16ON, 0, 1)
REG32(RCC_ICSCR, 0x4)
	FIELD(RCC_ICSCR, MSITRIM, 24, 8)
	FIELD(RCC_ICSCR, MSICAL, 16, 8)
	FIELD(RCC_ICSCR, MSIRANGE, 13, 3)
	FIELD(RCC_ICSCR, HSI16TRIM, 8, 5)
	FIELD(RCC_ICSCR, HSI16CAL, 0, 8)
REG32(RCC_CRRCR, 0x8)
	FIELD(RCC_CRRCR, HSI48CAL, 8, 8)
	FIELD(RCC_CRRCR, HSI48RDY, 1, 1)
	FIELD(RCC_CRRCR, HSI48ON, 0, 1)
REG32(RCC_CFGR, 0xc)
	FIELD(RCC_CFGR, MCOPRE, 28, 3)
	FIELD(RCC_CFGR, MCOSEL, 24, 3)
	FIELD(RCC_CFGR, PLLDIV, 22, 2)
	FIELD(RCC_CFGR, PLLMUL, 18, 4)
	FIELD(RCC_CFGR, PLLSRC, 16, 1)
	FIELD(RCC_CFGR, STOPWUCK, 15, 1)
	FIELD(RCC_CFGR, PPRE2, 11, 3)
	FIELD(RCC_CFGR, PPRE1, 8, 3)
	FIELD(RCC_CFGR, HPRE, 4, 4)
	FIELD(RCC_CFGR, SWS, 2, 2)
	FIELD(RCC_CFGR, SW, 0, 2)
REG32(RCC_CIER, 0x10)
	FIELD(RCC_CIER, CSSLSE, 7, 1)
	FIELD(RCC_CIER, HSI48RDYIE, 6, 1)
	FIELD(RCC_CIER, MSIRDYIE, 5, 1)
	FIELD(RCC_CIER, PLLRDYIE, 4, 1)
	FIELD(RCC_CIER, HSERDYIE, 3, 1)
	FIELD(RCC_CIER, HSI16RDYIE, 2, 1)
	FIELD(RCC_CIER, LSERDYIE, 1, 1)
	FIELD(RCC_CIER, LSIRDYIE, 0, 1)
REG32(RCC_CIFR, 0x14)
	FIELD(RCC_CIFR, CSSHSEF, 8, 1)
	FIELD(RCC_CIFR, CSSLSEF, 7, 1)
	FIELD(RCC_CIFR, HSI48RDYF, 6, 1)
	FIELD(RCC_CIFR, MSIRDYF, 5, 1)
	FIELD(RCC_CIFR, PLLRDYF, 4, 1)
	FIELD(RCC_CIFR, HSERDYF, 3, 1)
	FIELD(RCC_CIFR, HSI16RDYF, 2, 1)
	FIELD(RCC_CIFR, LSERDYF, 1, 1)
	FIELD(RCC_CIFR, LSIRDYF, 0, 1)
REG32(RCC_CICR, 0x18)
	FIELD(RCC_CICR, CSSHSEC, 8, 1)
	FIELD(RCC_CICR, CSSLSEC, 7, 1)
	FIELD(RCC_CICR, HSI48RDYC, 6, 1)
	FIELD(RCC_CICR, MSIRDYC, 5, 1)
	FIELD(RCC_CICR, PLLRDYC, 4, 1)
	FIELD(RCC_CICR, HSERDYC, 3, 1)
	FIELD(RCC_CICR, HSI16RDYC, 2, 1)
	FIELD(RCC_CICR, LSERDYC, 1, 1)
	FIELD(RCC_CICR, LSIRDYC, 0, 1)
REG32(RCC_IOPRSTR, 0x1c)
	FIELD(RCC_IOPRSTR, IOPHRST, 7, 1)
	FIELD(RCC_IOPRSTR, IOPDRST, 3, 1)
	FIELD(RCC_IOPRSTR, IOPCRST, 2, 1)
	FIELD(RCC_IOPRSTR, IOPBRST, 1, 1)
	FIELD(RCC_IOPRSTR, IOPARST, 0, 1)
REG32(RCC_AHBRSTR, 0x20)
	FIELD(RCC_AHBRSTR, CRYPRST, 24, 1)
	FIELD(RCC_AHBRSTR, RNGRST, 20, 1)
	FIELD(RCC_AHBRSTR, TOUCHRST, 16, 1)
	FIELD(RCC_AHBRSTR, CRCRST, 12, 1)
	FIELD(RCC_AHBRSTR, MIFRST, 8, 1)
	FIELD(RCC_AHBRSTR, DMARST, 0, 1)
REG32(RCC_APB2RSTR, 0x24)
	FIELD(RCC_APB2RSTR, DBGRST, 22, 1)
	FIELD(RCC_APB2RSTR, USART1RST, 14, 1)
	FIELD(RCC_APB2RSTR, SPI1RST, 12, 1)
	FIELD(RCC_APB2RSTR, ADCRST, 9, 1)
	FIELD(RCC_APB2RSTR, TM12RST, 5, 1)
	FIELD(RCC_APB2RSTR, TIM21RST, 2, 1)
	FIELD(RCC_APB2RSTR, SYSCFGRST, 0, 1)
REG32(RCC_APB1RSTR, 0x28)
	FIELD(RCC_APB1RSTR, LPTIM1RST, 31, 1)
	FIELD(RCC_APB1RSTR, DACRST, 29, 1)
	FIELD(RCC_APB1RSTR, PWRRST, 28, 1)
	FIELD(RCC_APB1RSTR, CRSRST, 27, 1)
	FIELD(RCC_APB1RSTR, USBRST, 23, 1)
	FIELD(RCC_APB1RSTR, I2C2RST, 22, 1)
	FIELD(RCC_APB1RSTR, I2C1RST, 21, 1)
	FIELD(RCC_APB1RSTR, LPUART1RST, 18, 1)
	FIELD(RCC_APB1RSTR, LPUART12RST, 17, 1)
	FIELD(RCC_APB1RSTR, SPI2RST, 14, 1)
	FIELD(RCC_APB1RSTR, WWDRST, 11, 1)
	FIELD(RCC_APB1RSTR, LCDRST, 9, 1)
	FIELD(RCC_APB1RSTR, TIM6RST, 4, 1)
	FIELD(RCC_APB1RSTR, TIM2RST, 0, 1)
REG32(RCC_IOPENR, 0x2c)
	FIELD(RCC_IOPENR, IOPHEN, 7, 1)
	FIELD(RCC_IOPENR, IOPDEN, 3, 1)
	FIELD(RCC_IOPENR, IOPCEN, 2, 1)
	FIELD(RCC_IOPENR, IOPBEN, 1, 1)
	FIELD(RCC_IOPENR, IOPAEN, 0, 1)
REG32(RCC_AHBENR, 0x30)
	FIELD(RCC_AHBENR, CRYPEN, 24, 1)
	FIELD(RCC_AHBENR, RNGEN, 20, 1)
	FIELD(RCC_AHBENR, TOUCHEN, 16, 1)
	FIELD(RCC_AHBENR, CRCEN, 12, 1)
	FIELD(RCC_AHBENR, MIFEN, 8, 1)
	FIELD(RCC_AHBENR, DMAEN, 0, 1)
REG32(RCC_APB2ENR, 0x34)
	FIELD(RCC_APB2ENR, DBGEN, 22, 1)
	FIELD(RCC_APB2ENR, USART1EN, 14, 1)
	FIELD(RCC_APB2ENR, SPI1EN, 12, 1)
	FIELD(RCC_APB2ENR, ADCEN, 9, 1)
	FIELD(RCC_APB2ENR, MIFIEN, 7, 1)
	FIELD(RCC_APB2ENR, TIM22EN, 5, 1)
	FIELD(RCC_APB2ENR, TIM21EN, 2, 1)
	FIELD(RCC_APB2ENR, SYSCFGEN, 0, 1)
REG32(RCC_APB1ENR, 0x38)
	FIELD(RCC_APB1ENR, LPTIM1EN, 31, 1)
	FIELD(RCC_APB1ENR, DACEN, 29, 1)
	FIELD(RCC_APB1ENR, PWREN, 28, 1)
	FIELD(RCC_APB1ENR, CRSEN, 27, 1)
	FIELD(RCC_APB1ENR, USBEN, 23, 1)
	FIELD(RCC_APB1ENR, I2C2EN, 22, 1)
	FIELD(RCC_APB1ENR, I2C1EN, 21, 1)
	FIELD(RCC_APB1ENR, LPUART1EN, 18, 1)
	FIELD(RCC_APB1ENR, USART2EN, 17, 1)
	FIELD(RCC_APB1ENR, SPI2EN, 14, 1)
	FIELD(RCC_APB1ENR, WWDGEN, 11, 1)
	FIELD(RCC_APB1ENR, LCDEN, 9, 1)
	FIELD(RCC_APB1ENR, TIM6EN, 4, 1)
	FIELD(RCC_APB1ENR, TIM2EN, 0, 1)
REG32(RCC_IOPSMEN, 0x3c)
	FIELD(RCC_IOPSMEN, IOPHSMEN, 7, 1)
	FIELD(RCC_IOPSMEN, IOPDSMEN, 3, 1)
	FIELD(RCC_IOPSMEN, IOPCSMEN, 2, 1)
	FIELD(RCC_IOPSMEN, IOPBSMEN, 1, 1)
	FIELD(RCC_IOPSMEN, IOPASMEN, 0, 1)
REG32(RCC_AHBSMENR, 0x40)
	FIELD(RCC_AHBSMENR, CRYPSMEN, 24, 1)
	FIELD(RCC_AHBSMENR, RNGSMEN, 20, 1)
	FIELD(RCC_AHBSMENR, TOUCHSMEN, 16, 1)
	FIELD(RCC_AHBSMENR, CRCSMEN, 12, 1)
	FIELD(RCC_AHBSMENR, SRAMSMEN, 9, 1)
	FIELD(RCC_AHBSMENR, MIFSMEN, 8, 1)
	FIELD(RCC_AHBSMENR, DMASMEN, 0, 1)
REG32(RCC_APB2SMENR, 0x44)
	FIELD(RCC_APB2SMENR, DBGSMEN, 22, 1)
	FIELD(RCC_APB2SMENR, USART1SMEN, 14, 1)
	FIELD(RCC_APB2SMENR, SPI1SMEN, 12, 1)
	FIELD(RCC_APB2SMENR, ADCSMEN, 9, 1)
	FIELD(RCC_APB2SMENR, TIM22SMEN, 5, 1)
	FIELD(RCC_APB2SMENR, TIM21SMEN, 2, 1)
	FIELD(RCC_APB2SMENR, SYSCFGSMEN, 0, 1)
REG32(RCC_APB1SMENR, 0x48)
	FIELD(RCC_APB1SMENR, LPTIM1SMEN, 31, 1)
	FIELD(RCC_APB1SMENR, DACSMEN, 29, 1)
	FIELD(RCC_APB1SMENR, PWRSMEN, 28, 1)
	FIELD(RCC_APB1SMENR, CRSSMEN, 27, 1)
	FIELD(RCC_APB1SMENR, USBSMEN, 23, 1)
	FIELD(RCC_APB1SMENR, I2C2SMEN, 22, 1)
	FIELD(RCC_APB1SMENR, I2C1SMEN, 21, 1)
	FIELD(RCC_APB1SMENR, LPUART1SMEN, 18, 1)
	FIELD(RCC_APB1SMENR, USART2SMEN, 17, 1)
	FIELD(RCC_APB1SMENR, SPI2SMEN, 14, 1)
	FIELD(RCC_APB1SMENR, WWDGSMEN, 11, 1)
	FIELD(RCC_APB1SMENR, LCDSMEN, 9, 1)
	FIELD(RCC_APB1SMENR, TIM6SMEN, 4, 1)
	FIELD(RCC_APB1SMENR, TIM2SMEN, 0, 1)
REG32(RCC_CCIPR, 0x4c)
	FIELD(RCC_CCIPR, HSI48MSEL, 26, 1)
	FIELD(RCC_CCIPR, LPTIM1SEL1, 19, 1)
	FIELD(RCC_CCIPR, LPTIM1SEL0, 18, 1)
	FIELD(RCC_CCIPR, I2C1SEL1, 13, 1)
	FIELD(RCC_CCIPR, I2C1SEL0, 12, 1)
	FIELD(RCC_CCIPR, LPUART1SEL1, 11, 1)
	FIELD(RCC_CCIPR, LPUART1SEL0, 10, 1)
	FIELD(RCC_CCIPR, USART2SEL1, 3, 1)
	FIELD(RCC_CCIPR, USART2SEL0, 2, 1)
	FIELD(RCC_CCIPR, USART1SEL1, 1, 1)
	FIELD(RCC_CCIPR, USART1SEL0, 0, 1)
REG32(RCC_CSR, 0x50)
	FIELD(RCC_CSR, LPWRSTF, 31, 1)
	FIELD(RCC_CSR, WWDGRSTF, 30, 1)
	FIELD(RCC_CSR, IWDGRSTF, 29, 1)
	FIELD(RCC_CSR, SFTRSTF, 28, 1)
	FIELD(RCC_CSR, PORRSTF, 27, 1)
	FIELD(RCC_CSR, PINRSTF, 26, 1)
	FIELD(RCC_CSR, OBLRSTF, 25, 1)
	FIELD(RCC_CSR, RMVF, 24, 1)
	FIELD(RCC_CSR, RTCRST, 19, 1)
	FIELD(RCC_CSR, RTCEN, 18, 1)
	FIELD(RCC_CSR, RTCSEL, 16, 2)
	FIELD(RCC_CSR, CSSLSED, 14, 1)
	FIELD(RCC_CSR, CSSLSEON, 13, 1)
	FIELD(RCC_CSR, LSEDRV, 11, 2)
	FIELD(RCC_CSR, LSEBYP, 10, 1)
	FIELD(RCC_CSR, LSERDY, 9, 1)
	FIELD(RCC_CSR, LSEON, 8, 1)
	FIELD(RCC_CSR, LSIRDY, 1, 1)
	FIELD(RCC_CSR, LSION, 0, 1)


__attribute__((unused))
static void stm32l0_rcc_update(STM32L0RCC *t) {
	int conditions = (t->CIFR & 0x100);
	qemu_set_irq(t->irq[0], conditions);

}

static void stm32l0_rcc_register_reset(STM32L0RCC *t) {
	t->CR = 0x300;
	t->ICSCR = 0xb000;
	t->CRRCR = 0x0;
	t->CFGR = 0x0;
	t->CIER = 0x0;
	t->CIFR = 0x0;
	t->CICR = 0x0;
	t->IOPRSTR = 0x0;
	t->AHBRSTR = 0x0;
	t->APB2RSTR = 0x0;
	t->APB1RSTR = 0x0;
	t->IOPENR = 0x0;
	t->AHBENR = 0x100;
	t->APB2ENR = 0x0;
	t->APB1ENR = 0x0;
	t->IOPSMEN = 0x8f;
	t->AHBSMENR = 0x1111301;
	t->APB2SMENR = 0x405225;
	t->APB1SMENR = 0xb8e64a11;
	t->CCIPR = 0x0;
	t->CSR = 0xc000000;

}

static uint64_t stm32l0_rcc_read(void *opaque, hwaddr offset, unsigned size) {
	STM32L0RCC *t = STM32L0_RCC(opaque);
	uint64_t ret;

	switch (offset) {
		case A_RCC_CR:
			ret = t->CR;
			break;
		case A_RCC_ICSCR:
			ret = t->ICSCR;
			break;
		case A_RCC_CRRCR:
			ret = t->CRRCR;
			break;
		case A_RCC_CFGR:
			ret = t->CFGR;
			break;
		case A_RCC_CIER:
			ret = t->CIER;
			break;
		case A_RCC_CIFR:
			ret = t->CIFR;
			break;
		case A_RCC_CICR:
			ret = t->CICR;
			break;
		case A_RCC_IOPRSTR:
			ret = t->IOPRSTR;
			break;
		case A_RCC_AHBRSTR:
			ret = t->AHBRSTR;
			break;
		case A_RCC_APB2RSTR:
			ret = t->APB2RSTR;
			break;
		case A_RCC_APB1RSTR:
			ret = t->APB1RSTR;
			break;
		case A_RCC_IOPENR:
			ret = t->IOPENR;
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
		case A_RCC_IOPSMEN:
			ret = t->IOPSMEN;
			break;
		case A_RCC_AHBSMENR:
			ret = t->AHBSMENR;
			break;
		case A_RCC_APB2SMENR:
			ret = t->APB2SMENR;
			break;
		case A_RCC_APB1SMENR:
			ret = t->APB1SMENR;
			break;
		case A_RCC_CCIPR:
			ret = t->CCIPR;
			break;
		case A_RCC_CSR:
			ret = t->CSR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32L0 RCC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32l0_rcc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32L0RCC *t = STM32L0_RCC(opaque);


	switch (offset) {
		case A_RCC_CR ... A_RCC_CR + 3:
			value = value << ((offset - A_RCC_CR) << 3);
			offset = A_RCC_CR;
			break;
		case A_RCC_ICSCR ... A_RCC_ICSCR + 3:
			value = value << ((offset - A_RCC_ICSCR) << 3);
			offset = A_RCC_ICSCR;
			break;
		case A_RCC_CRRCR ... A_RCC_CRRCR + 3:
			value = value << ((offset - A_RCC_CRRCR) << 3);
			offset = A_RCC_CRRCR;
			break;
		case A_RCC_CFGR ... A_RCC_CFGR + 3:
			value = value << ((offset - A_RCC_CFGR) << 3);
			offset = A_RCC_CFGR;
			break;
		case A_RCC_IOPRSTR ... A_RCC_IOPRSTR + 3:
			value = value << ((offset - A_RCC_IOPRSTR) << 3);
			offset = A_RCC_IOPRSTR;
			break;
		case A_RCC_AHBRSTR ... A_RCC_AHBRSTR + 3:
			value = value << ((offset - A_RCC_AHBRSTR) << 3);
			offset = A_RCC_AHBRSTR;
			break;
		case A_RCC_APB2RSTR ... A_RCC_APB2RSTR + 3:
			value = value << ((offset - A_RCC_APB2RSTR) << 3);
			offset = A_RCC_APB2RSTR;
			break;
		case A_RCC_APB1RSTR ... A_RCC_APB1RSTR + 3:
			value = value << ((offset - A_RCC_APB1RSTR) << 3);
			offset = A_RCC_APB1RSTR;
			break;
		case A_RCC_IOPENR ... A_RCC_IOPENR + 3:
			value = value << ((offset - A_RCC_IOPENR) << 3);
			offset = A_RCC_IOPENR;
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
		case A_RCC_IOPSMEN ... A_RCC_IOPSMEN + 3:
			value = value << ((offset - A_RCC_IOPSMEN) << 3);
			offset = A_RCC_IOPSMEN;
			break;
		case A_RCC_AHBSMENR ... A_RCC_AHBSMENR + 3:
			value = value << ((offset - A_RCC_AHBSMENR) << 3);
			offset = A_RCC_AHBSMENR;
			break;
		case A_RCC_APB2SMENR ... A_RCC_APB2SMENR + 3:
			value = value << ((offset - A_RCC_APB2SMENR) << 3);
			offset = A_RCC_APB2SMENR;
			break;
		case A_RCC_APB1SMENR ... A_RCC_APB1SMENR + 3:
			value = value << ((offset - A_RCC_APB1SMENR) << 3);
			offset = A_RCC_APB1SMENR;
			break;
		case A_RCC_CCIPR ... A_RCC_CCIPR + 3:
			value = value << ((offset - A_RCC_CCIPR) << 3);
			offset = A_RCC_CCIPR;
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
			if ((value & 0x10000)) {
				t->CR |= 0x20000;
			}
			if ((value & 0x1000000)) {
				t->CR |= 0x2000000;
			} else if ((!(value & 0x1000000))) {
				t->CR &= (~(0x2000000));
			}
			if ((!(value & 0x40000))) {
				t->CR &= (~(0x20000));
				t->CR &= (~(0x2000000));
			}
			if ((!(value & 0x1))) {
				t->CR &= (~(0x4));
			} else if ((value & 0x1)) {
				t->CR |= 0x4;
			}
			if ((!(value & 0x100))) {
				t->CR &= (~(0x200));
			} else if ((value & 0x100)) {
				t->CR |= 0x200;
			}
			break;
		case A_RCC_ICSCR:
			t->ICSCR = value;
			break;
		case A_RCC_CRRCR:
			t->CRRCR = value;
			if ((!(value & 0x1))) {
				t->CRRCR &= (~(0x2));
			}
			break;
		case A_RCC_CFGR:
			t->CFGR = value;
			if (((value & 0x1) && (!(value & 0x2)))) {
				t->CFGR &= (~(0x8));
				t->CFGR |= 0x4;
			} else if (((!(value & 0x1)) && (!(value & 0x2)))) {
				t->CFGR &= (~(0x4));
				t->CFGR &= (~(0x8));
			} else if (((value & 0x1) && (value & 0x2))) {
				t->CFGR |= 0x4;
				t->CFGR |= 0x8;
			} else if (((!(value & 0x1)) && (value & 0x2))) {
				t->CFGR &= (~(0x4));
				t->CFGR |= 0x8;
			}
			break;
		case A_RCC_IOPRSTR:
			t->IOPRSTR = value;
			break;
		case A_RCC_AHBRSTR:
			t->AHBRSTR = value;
			break;
		case A_RCC_APB2RSTR:
			t->APB2RSTR = value;
			break;
		case A_RCC_APB1RSTR:
			t->APB1RSTR = value;
			break;
		case A_RCC_IOPENR:
			t->IOPENR = value;
			break;
		case A_RCC_AHBENR:
			t->AHBENR = value;
			break;
		case A_RCC_APB2ENR:
			t->APB2ENR = value;
			if ((value & 0x1)) {
				t->CRRCR |= 0x2;
			}
			break;
		case A_RCC_APB1ENR:
			t->APB1ENR = value;
			break;
		case A_RCC_IOPSMEN:
			t->IOPSMEN = value;
			break;
		case A_RCC_AHBSMENR:
			t->AHBSMENR = value;
			break;
		case A_RCC_APB2SMENR:
			t->APB2SMENR = value;
			break;
		case A_RCC_APB1SMENR:
			t->APB1SMENR = value;
			break;
		case A_RCC_CCIPR:
			t->CCIPR = value;
			break;
		case A_RCC_CSR:
			t->CSR = value;
			if ((value & 0x100)) {
				t->CSR |= 0x200;
			}
			if ((value & 0x1)) {
				t->CSR |= 0x2;
			} else if ((!(value & 0x1))) {
				t->CSR &= (~(0x2));
			}
			if ((!(value & 0x400))) {
				t->CSR &= (~(0x200));
			}
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32L0 RCC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32l0_rcc_ops = {
	.read = stm32l0_rcc_read,
	.write = stm32l0_rcc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32l0_rcc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32L0RCC *t = STM32L0_RCC(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32l0_rcc_ops, t, "stm32l0-rcc", STM32L0_RCC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32l0_rcc_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32l0_rcc_reset_enter(Object *obj, ResetType type) {
	STM32L0RCC *t = STM32L0_RCC(obj);
	stm32l0_rcc_register_reset(t);
}

static const VMStateDescription stm32l0_rcc_vmstate = {
	.name = "stm32l0-rcc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR, STM32L0RCC),
		VMSTATE_UINT32(ICSCR, STM32L0RCC),
		VMSTATE_UINT32(CRRCR, STM32L0RCC),
		VMSTATE_UINT32(CFGR, STM32L0RCC),
		VMSTATE_UINT32(CIER, STM32L0RCC),
		VMSTATE_UINT32(CIFR, STM32L0RCC),
		VMSTATE_UINT32(CICR, STM32L0RCC),
		VMSTATE_UINT32(IOPRSTR, STM32L0RCC),
		VMSTATE_UINT32(AHBRSTR, STM32L0RCC),
		VMSTATE_UINT32(APB2RSTR, STM32L0RCC),
		VMSTATE_UINT32(APB1RSTR, STM32L0RCC),
		VMSTATE_UINT32(IOPENR, STM32L0RCC),
		VMSTATE_UINT32(AHBENR, STM32L0RCC),
		VMSTATE_UINT32(APB2ENR, STM32L0RCC),
		VMSTATE_UINT32(APB1ENR, STM32L0RCC),
		VMSTATE_UINT32(IOPSMEN, STM32L0RCC),
		VMSTATE_UINT32(AHBSMENR, STM32L0RCC),
		VMSTATE_UINT32(APB2SMENR, STM32L0RCC),
		VMSTATE_UINT32(APB1SMENR, STM32L0RCC),
		VMSTATE_UINT32(CCIPR, STM32L0RCC),
		VMSTATE_UINT32(CSR, STM32L0RCC),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32l0_rcc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32l0_rcc_vmstate;
	dc->realize = stm32l0_rcc_realize;
	rc->phases.enter = stm32l0_rcc_reset_enter;

}

static const TypeInfo stm32l0_rcc_info = {
	.name = TYPE_STM32L0_RCC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32L0RCC),
	.instance_init = stm32l0_rcc_init,
	.class_init = stm32l0_rcc_class_init,
};

static void stm32l0_rcc_register_types(void) {
	type_register_static(&stm32l0_rcc_info);
}

type_init(stm32l0_rcc_register_types);

#define TYPE_STM32L0_SYSCFG "stm32l0-syscfg"
OBJECT_DECLARE_SIMPLE_TYPE(STM32L0SYSCFG, STM32L0_SYSCFG)

struct STM32L0SYSCFG {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/*registers*/
	uint32_t CFGR1;
	uint32_t CFGR2;
	uint32_t EXTICR1;
	uint32_t EXTICR2;
	uint32_t EXTICR3;
	uint32_t EXTICR4;
	uint32_t CFGR3;
	/* base */
	uint32_t base;
	// PERRY PATCH
	ARMCPU *cpu;

};
#define STM32L0_SYSCFG_SIZE				0x24

REG32(SYSCFG_CFGR1, 0x0)
	FIELD(SYSCFG_CFGR1, BOOT_MODE, 8, 2)
	FIELD(SYSCFG_CFGR1, MEM_MODE, 0, 2)
REG32(SYSCFG_CFGR2, 0x4)
	FIELD(SYSCFG_CFGR2, I2C2_FMP, 13, 1)
	FIELD(SYSCFG_CFGR2, I2C1_FMP, 12, 1)
	FIELD(SYSCFG_CFGR2, I2C_PB9_FMP, 11, 1)
	FIELD(SYSCFG_CFGR2, I2C_PB8_FMP, 10, 1)
	FIELD(SYSCFG_CFGR2, I2C_PB7_FMP, 9, 1)
	FIELD(SYSCFG_CFGR2, I2C_PB6_FMP, 8, 1)
	FIELD(SYSCFG_CFGR2, CAPA, 1, 3)
	FIELD(SYSCFG_CFGR2, FWDISEN, 0, 1)
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
REG32(SYSCFG_CFGR3, 0x20)
	FIELD(SYSCFG_CFGR3, REF_LOCK, 31, 1)
	FIELD(SYSCFG_CFGR3, VREFINT_RDYF, 30, 1)
	FIELD(SYSCFG_CFGR3, VREFINT_COMP_RDYF, 29, 1)
	FIELD(SYSCFG_CFGR3, VREFINT_ADC_RDYF, 28, 1)
	FIELD(SYSCFG_CFGR3, SENSOR_ADC_RDYF, 27, 1)
	FIELD(SYSCFG_CFGR3, REF_RC48MHz_RDYF, 26, 1)
	FIELD(SYSCFG_CFGR3, ENREF_RC48MHz, 13, 1)
	FIELD(SYSCFG_CFGR3, ENBUF_VREFINT_COMP, 12, 1)
	FIELD(SYSCFG_CFGR3, ENBUF_SENSOR_ADC, 9, 1)
	FIELD(SYSCFG_CFGR3, ENBUF_BGAP_ADC, 8, 1)
	FIELD(SYSCFG_CFGR3, SEL_VREF_OUT, 4, 2)
	FIELD(SYSCFG_CFGR3, EN_BGAP, 0, 1)


static void stm32l0_syscfg_register_reset(STM32L0SYSCFG *t) {
	t->CFGR1 = 0x0;
	t->CFGR2 = 0x0;
	t->EXTICR1 = 0x0;
	t->EXTICR2 = 0x0;
	t->EXTICR3 = 0x0;
	t->EXTICR4 = 0x0;
	t->CFGR3 = 0x0;

}

static uint64_t stm32l0_syscfg_read(void *opaque, hwaddr offset, unsigned size) {
	STM32L0SYSCFG *t = STM32L0_SYSCFG(opaque);
	uint64_t ret;

	switch (offset) {
		case A_SYSCFG_CFGR1:
			ret = t->CFGR1;
			break;
		case A_SYSCFG_CFGR2:
			ret = t->CFGR2;
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
		case A_SYSCFG_CFGR3:
			ret = t->CFGR3;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32L0 SYSCFG read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32l0_syscfg_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32L0SYSCFG *t = STM32L0_SYSCFG(opaque);


	switch (offset) {
		case A_SYSCFG_CFGR1 ... A_SYSCFG_CFGR1 + 3:
			value = value << ((offset - A_SYSCFG_CFGR1) << 3);
			offset = A_SYSCFG_CFGR1;
			break;
		case A_SYSCFG_CFGR2 ... A_SYSCFG_CFGR2 + 3:
			value = value << ((offset - A_SYSCFG_CFGR2) << 3);
			offset = A_SYSCFG_CFGR2;
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
		case A_SYSCFG_CFGR3 ... A_SYSCFG_CFGR3 + 3:
			value = value << ((offset - A_SYSCFG_CFGR3) << 3);
			offset = A_SYSCFG_CFGR3;
			break;

		default: break;
	}

	switch (offset) {
		case A_SYSCFG_CFGR1:
			t->CFGR1 = value;
			// PERRY PATCH
			if ((value & 3) == 3) { t->cpu->env.v7m.vecbase[0] = 0x20000000; }
			break;
		case A_SYSCFG_CFGR2:
			t->CFGR2 = value;
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
		case A_SYSCFG_CFGR3:
			t->CFGR3 = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32L0 SYSCFG write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32l0_syscfg_ops = {
	.read = stm32l0_syscfg_read,
	.write = stm32l0_syscfg_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32l0_syscfg_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32L0SYSCFG *t = STM32L0_SYSCFG(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32l0_syscfg_ops, t, "stm32l0-syscfg", STM32L0_SYSCFG_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void stm32l0_syscfg_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32l0_syscfg_reset_enter(Object *obj, ResetType type) {
	STM32L0SYSCFG *t = STM32L0_SYSCFG(obj);
	stm32l0_syscfg_register_reset(t);
}

static const VMStateDescription stm32l0_syscfg_vmstate = {
	.name = "stm32l0-syscfg",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CFGR1, STM32L0SYSCFG),
		VMSTATE_UINT32(CFGR2, STM32L0SYSCFG),
		VMSTATE_UINT32(EXTICR1, STM32L0SYSCFG),
		VMSTATE_UINT32(EXTICR2, STM32L0SYSCFG),
		VMSTATE_UINT32(EXTICR3, STM32L0SYSCFG),
		VMSTATE_UINT32(EXTICR4, STM32L0SYSCFG),
		VMSTATE_UINT32(CFGR3, STM32L0SYSCFG),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32l0_syscfg_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32l0_syscfg_vmstate;
	dc->realize = stm32l0_syscfg_realize;
	rc->phases.enter = stm32l0_syscfg_reset_enter;

}

static const TypeInfo stm32l0_syscfg_info = {
	.name = TYPE_STM32L0_SYSCFG,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32L0SYSCFG),
	.instance_init = stm32l0_syscfg_init,
	.class_init = stm32l0_syscfg_class_init,
};

static void stm32l0_syscfg_register_types(void) {
	type_register_static(&stm32l0_syscfg_info);
}

type_init(stm32l0_syscfg_register_types);

#define TYPE_STM32L0_TIM2 "stm32l0-tim2"
OBJECT_DECLARE_SIMPLE_TYPE(STM32L0TIM2, STM32L0_TIM2)

struct STM32L0TIM2 {
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
	uint32_t OR;
	/* timer */
	QEMUTimer *timers[1];
	uint8_t enabled[1];

	uint32_t cur_period[1];

	uint8_t period_set[1];

	uint8_t period_reg_set[1];

	/* base */
	uint32_t base;

};
#define STM32L0_TIM2_SIZE				0x54

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
	FIELD(TIM2_DIER, COMDE, 13, 1)
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
	FIELD(TIM2_CCMR2_Output, OC4CE, 15, 1)
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
	FIELD(TIM2_CCER, CC4NP, 15, 1)
	FIELD(TIM2_CCER, CC4P, 13, 1)
	FIELD(TIM2_CCER, CC4E, 12, 1)
	FIELD(TIM2_CCER, CC3NP, 11, 1)
	FIELD(TIM2_CCER, CC3P, 9, 1)
	FIELD(TIM2_CCER, CC3E, 8, 1)
	FIELD(TIM2_CCER, CC2NP, 7, 1)
	FIELD(TIM2_CCER, CC2P, 5, 1)
	FIELD(TIM2_CCER, CC2E, 4, 1)
	FIELD(TIM2_CCER, CC1NP, 3, 1)
	FIELD(TIM2_CCER, CC1P, 1, 1)
	FIELD(TIM2_CCER, CC1E, 0, 1)
REG32(TIM2_CNT, 0x24)
	FIELD(TIM2_CNT, CNT_H, 16, 16)
	FIELD(TIM2_CNT, CNT_L, 0, 16)
REG32(TIM2_PSC, 0x28)
	FIELD(TIM2_PSC, PSC, 0, 16)
REG32(TIM2_ARR, 0x2c)
	FIELD(TIM2_ARR, ARR_H, 16, 16)
	FIELD(TIM2_ARR, ARR_L, 0, 16)
REG32(TIM2_CCR1, 0x34)
	FIELD(TIM2_CCR1, CCR1_H, 16, 16)
	FIELD(TIM2_CCR1, CCR1_L, 0, 16)
REG32(TIM2_CCR2, 0x38)
	FIELD(TIM2_CCR2, CCR2_H, 16, 16)
	FIELD(TIM2_CCR2, CCR2_L, 0, 16)
REG32(TIM2_CCR3, 0x3c)
	FIELD(TIM2_CCR3, CCR3_H, 16, 16)
	FIELD(TIM2_CCR3, CCR3_L, 0, 16)
REG32(TIM2_CCR4, 0x40)
	FIELD(TIM2_CCR4, CCR4_H, 16, 16)
	FIELD(TIM2_CCR4, CCR4_L, 0, 16)
REG32(TIM2_DCR, 0x48)
	FIELD(TIM2_DCR, DBL, 8, 5)
	FIELD(TIM2_DCR, DBA, 0, 5)
REG32(TIM2_DMAR, 0x4c)
	FIELD(TIM2_DMAR, DMAB, 0, 16)
REG32(TIM2_OR, 0x50)
	FIELD(TIM2_OR, ETR_RMP, 0, 3)
	FIELD(TIM2_OR, TI4_RMP, 3, 2)


__attribute__((unused))
static void stm32l0_tim2_update(STM32L0TIM2 *t) {
	int conditions = (((t->DIER & 0x10) && (t->SR & 0x10)) || ((t->SR & 0x1) && (t->DIER & 0x1)) || ((t->SR & 0x2) && (t->DIER & 0x2)) || ((t->DIER & 0x40) && (t->SR & 0x40)) || ((t->DIER & 0x4) && (t->SR & 0x4)) || ((t->SR & 0x8) && (t->DIER & 0x8)));
	qemu_set_irq(t->irq[0], conditions);

}

static void stm32l0_tim2_timer_callback_0(void *opaque) {
  static uint8_t period_reg_set_last = 0;
  STM32L0TIM2 *t = (STM32L0TIM2*)opaque;

  if (!period_reg_set_last && t->period_reg_set[0]) {
      period_reg_set_last = 1;
      t->CNT = 0;
  }
  t->CNT += 1;
  unsigned prd = t->period_set[0] ? t->cur_period[0] : t->ARR;
  if (t->CNT == prd) {
    t->cur_period[0] = t->ARR;
    t->CNT = 0;
    t->SR |= 0x4;t->SR |= 0x8;t->DIER |= 0x1;t->DIER |= 0x40;t->DIER |= 0x2;t->SR |= 0x10;t->DIER |= 0x8;t->DIER |= 0x10;t->DIER |= 0x4;t->SR |= 0x40;t->SR |= 0x1;t->SR |= 0x2;;
    qemu_set_irq(t->irq[0], 1);
    qemu_set_irq(t->irq[0], 0);
  }

  if (t->enabled[0]) {
    timer_mod(t->timers[0], qemu_clock_get_us(QEMU_CLOCK_VIRTUAL) + 1);
  }
}


static void stm32l0_tim2_register_reset(STM32L0TIM2 *t) {
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
	t->OR = 0x0;
	t->enabled[0] = 0;
	t->period_set[0] = 0;
	t->period_reg_set[0] = 0;

}

static uint64_t stm32l0_tim2_read(void *opaque, hwaddr offset, unsigned size) {
	STM32L0TIM2 *t = STM32L0_TIM2(opaque);
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
		case A_TIM2_OR:
			ret = t->OR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32L0 TIM2 read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32l0_tim2_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32L0TIM2 *t = STM32L0_TIM2(opaque);


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
		case A_TIM2_OR ... A_TIM2_OR + 3:
			value = value << ((offset - A_TIM2_OR) << 3);
			offset = A_TIM2_OR;
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
				t->timers[0] = timer_new(QEMU_CLOCK_VIRTUAL, SCALE_US, stm32l0_tim2_timer_callback_0, t);
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
			stm32l0_tim2_update(t);
			break;
		case A_TIM2_SR:
			t->SR = value;
			stm32l0_tim2_update(t);
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
		case A_TIM2_OR:
			t->OR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32L0 TIM2 write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32l0_tim2_ops = {
	.read = stm32l0_tim2_read,
	.write = stm32l0_tim2_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32l0_tim2_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32L0TIM2 *t = STM32L0_TIM2(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32l0_tim2_ops, t, "stm32l0-tim2", STM32L0_TIM2_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32l0_tim2_realize(DeviceState *dev, Error **errp) {
	STM32L0TIM2 *t = STM32L0_TIM2(dev);
	t->timers[0] = timer_new(QEMU_CLOCK_VIRTUAL, SCALE_US, stm32l0_tim2_timer_callback_0, t);

}

static void stm32l0_tim2_reset_enter(Object *obj, ResetType type) {
	STM32L0TIM2 *t = STM32L0_TIM2(obj);
	stm32l0_tim2_register_reset(t);
}

static const VMStateDescription stm32l0_tim2_vmstate = {
	.name = "stm32l0-tim2",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR1, STM32L0TIM2),
		VMSTATE_UINT32(CR2, STM32L0TIM2),
		VMSTATE_UINT32(SMCR, STM32L0TIM2),
		VMSTATE_UINT32(DIER, STM32L0TIM2),
		VMSTATE_UINT32(SR, STM32L0TIM2),
		VMSTATE_UINT32(EGR, STM32L0TIM2),
		VMSTATE_UINT32(CCMR1_Output, STM32L0TIM2),
		VMSTATE_UINT32(CCMR1_Input, STM32L0TIM2),
		VMSTATE_UINT32(CCMR2_Output, STM32L0TIM2),
		VMSTATE_UINT32(CCMR2_Input, STM32L0TIM2),
		VMSTATE_UINT32(CCER, STM32L0TIM2),
		VMSTATE_UINT32(CNT, STM32L0TIM2),
		VMSTATE_UINT32(PSC, STM32L0TIM2),
		VMSTATE_UINT32(ARR, STM32L0TIM2),
		VMSTATE_UINT32(CCR1, STM32L0TIM2),
		VMSTATE_UINT32(CCR2, STM32L0TIM2),
		VMSTATE_UINT32(CCR3, STM32L0TIM2),
		VMSTATE_UINT32(CCR4, STM32L0TIM2),
		VMSTATE_UINT32(DCR, STM32L0TIM2),
		VMSTATE_UINT32(DMAR, STM32L0TIM2),
		VMSTATE_UINT32(OR, STM32L0TIM2),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32l0_tim2_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32l0_tim2_vmstate;
	dc->realize = stm32l0_tim2_realize;
	rc->phases.enter = stm32l0_tim2_reset_enter;

}

static const TypeInfo stm32l0_tim2_info = {
	.name = TYPE_STM32L0_TIM2,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32L0TIM2),
	.instance_init = stm32l0_tim2_init,
	.class_init = stm32l0_tim2_class_init,
};

static void stm32l0_tim2_register_types(void) {
	type_register_static(&stm32l0_tim2_info);
}

type_init(stm32l0_tim2_register_types);

#define TYPE_STM32L0_USART "stm32l0-usart"
OBJECT_DECLARE_SIMPLE_TYPE(STM32L0USART, STM32L0_USART)

struct STM32L0USART {
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

	/* dma */
	STM32L0DMA *dma;
	/* base */
	uint32_t base;

};
#define STM32L0_USART_SIZE				0x2c

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
static void stm32l0_usart_update(STM32L0USART *t) {
	int conditions = (((t->CR3 & 0x1) && (t->ISR & 0x8) && (!(t->CR3 & 0x40))) || ((!(t->ISR & 0x8)) && (!(t->ISR & 0x2)) && (!(t->ISR & 0x10)) && (!(t->ISR & 0x4)) && (!(t->ISR & 0x800)) && (!(t->ISR & 0x1)) && (t->ISR & 0x80) && (t->CR1 & 0x80)) || ((t->CR3 & 0x1) && (t->ISR & 0x1) && (!(t->CR3 & 0x40))) || ((t->ISR & 0x4) && (t->CR3 & 0x1) && (!(t->CR3 & 0x40))) || ((t->CR3 & 0x1) && (!(t->CR3 & 0x40)) && (t->ISR & 0x2)) || ((!(t->ISR & 0x8)) && (!(t->ISR & 0x2)) && (!(t->CR1 & 0x10)) && (!(t->ISR & 0x4)) && (!(t->ISR & 0x800)) && (!(t->ISR & 0x1)) && (t->ISR & 0x80) && (t->CR1 & 0x80)) || ((t->ISR & 0x4) && (t->CR1 & 0x20) && (!(t->CR3 & 0x40))) || ((t->CR1 & 0x20) && (!(t->CR3 & 0x40)) && (t->ISR & 0x2)) || ((t->CR1 & 0x20) && (!(t->CR3 & 0x40)) && (t->ISR & 0x800)) || ((!(t->CR3 & 0x40)) && (t->CR1 & 0x20) && (t->ISR & 0x8)) || ((t->ISR & 0x4) && (!(t->CR3 & 0x40)) && (t->CR1 & 0x4000000)) || ((t->ISR & 0x8) && (!(t->CR3 & 0x40)) && (t->CR1 & 0x4000000)) || ((t->ISR & 0x4) && (!(t->CR3 & 0x40)) && (t->CR1 & 0x100)) || ((t->ISR & 0x8) && (!(t->CR3 & 0x40)) && (t->CR1 & 0x100)) || ((!(t->CR3 & 0x40)) && (t->ISR & 0x2) && (t->CR1 & 0x100)) || ((!(t->CR3 & 0x40)) && (t->ISR & 0x2) && (t->CR1 & 0x4000000)) || ((t->ISR & 0x1) && (t->CR1 & 0x20) && (!(t->CR3 & 0x40))) || ((t->ISR & 0x1) && (!(t->CR3 & 0x40)) && (t->CR1 & 0x4000000)) || ((t->ISR & 0x1) && (!(t->CR3 & 0x40)) && (t->CR1 & 0x100)) || ((!(t->ISR & 0x8)) && (!(t->ISR & 0x2)) && (!(t->CR1 & 0x10)) && (!(t->ISR & 0x4)) && (!(t->ISR & 0x800)) && (!(t->ISR & 0x1)) && (t->ISR & 0x40) && (t->CR1 & 0x40)) || ((!(t->ISR & 0x8)) && (!(t->ISR & 0x2)) && (!(t->ISR & 0x10)) && (!(t->ISR & 0x4)) && (!(t->ISR & 0x800)) && (!(t->ISR & 0x1)) && (t->ISR & 0x40) && (t->CR1 & 0x40)) || ((!(t->ISR & 0x8)) && (!(t->ISR & 0x2)) && (!(t->CR1 & 0x10)) && (!(t->ISR & 0x4)) && (!(t->ISR & 0x800)) && (!(t->ISR & 0x1)) && (t->ISR & 0x100000) && (t->CR3 & 0x400000)) || ((!(t->ISR & 0x8)) && (!(t->ISR & 0x2)) && (!(t->ISR & 0x4)) && (!(t->ISR & 0x800)) && (!(t->ISR & 0x1)) && (t->ISR & 0x80) && (t->CR1 & 0x80)) || ((!(t->CR1 & 0x20)) && (!(t->CR3 & 0x1)) && (!(t->CR1 & 0x100)) && (!(t->CR1 & 0x4000000)) && (t->ISR & 0x40) && (t->CR1 & 0x40)) || ((!(t->ISR & 0x8)) && (!(t->ISR & 0x2)) && (!(t->ISR & 0x4)) && (!(t->ISR & 0x800)) && (!(t->ISR & 0x1)) && (t->ISR & 0x40) && (t->CR1 & 0x40)) || ((!(t->CR1 & 0x20)) && (!(t->ISR & 0x10)) && (!(t->CR3 & 0x1)) && (!(t->CR1 & 0x100)) && (!(t->CR1 & 0x4000000)) && (t->ISR & 0x100000) && (t->CR3 & 0x400000)) || ((!(t->CR1 & 0x20)) && (!(t->ISR & 0x10)) && (!(t->CR3 & 0x1)) && (!(t->CR1 & 0x100)) && (!(t->CR1 & 0x4000000)) && (t->ISR & 0x40) && (t->CR1 & 0x40)) || ((!(t->ISR & 0x8)) && (!(t->ISR & 0x2)) && (!(t->ISR & 0x4)) && (!(t->ISR & 0x800)) && (!(t->ISR & 0x1)) && (t->ISR & 0x100000) && (t->CR3 & 0x400000)) || ((!(t->ISR & 0x8)) && (!(t->ISR & 0x2)) && (!(t->ISR & 0x10)) && (!(t->ISR & 0x4)) && (!(t->ISR & 0x800)) && (!(t->ISR & 0x1)) && (t->ISR & 0x100000) && (t->CR3 & 0x400000)) || ((!(t->CR1 & 0x20)) && (!(t->CR3 & 0x1)) && (!(t->CR1 & 0x100)) && (!(t->CR1 & 0x4000000)) && (t->ISR & 0x80) && (t->CR1 & 0x80)) || ((!(t->CR1 & 0x20)) && (!(t->CR1 & 0x10)) && (!(t->CR3 & 0x1)) && (!(t->CR1 & 0x100)) && (!(t->CR1 & 0x4000000)) && (t->ISR & 0x80) && (t->CR1 & 0x80)) || ((!(t->CR1 & 0x20)) && (!(t->CR1 & 0x10)) && (!(t->CR3 & 0x1)) && (!(t->CR1 & 0x100)) && (!(t->CR1 & 0x4000000)) && (t->ISR & 0x40) && (t->CR1 & 0x40)) || ((!(t->CR1 & 0x20)) && (!(t->ISR & 0x10)) && (!(t->CR3 & 0x1)) && (!(t->CR1 & 0x100)) && (!(t->CR1 & 0x4000000)) && (t->ISR & 0x80) && (t->CR1 & 0x80)) || ((t->CR3 & 0x1) && (!(t->CR3 & 0x40)) && (t->ISR & 0x800)) || ((!(t->CR3 & 0x40)) && (t->CR1 & 0x100) && (t->ISR & 0x800)) || ((!(t->CR3 & 0x40)) && (t->CR1 & 0x4000000) && (t->ISR & 0x800)) || ((!(t->CR1 & 0x20)) && (!(t->CR1 & 0x10)) && (!(t->CR3 & 0x1)) && (!(t->CR1 & 0x100)) && (!(t->CR1 & 0x4000000)) && (t->ISR & 0x100000) && (t->CR3 & 0x400000)) || ((!(t->CR3 & 0x40)) && (t->ISR & 0x10) && (t->CR1 & 0x10)) || ((!(t->CR1 & 0x20)) && (!(t->CR3 & 0x1)) && (!(t->CR1 & 0x100)) && (!(t->CR1 & 0x4000000)) && (t->ISR & 0x100000) && (t->CR3 & 0x400000)) || ((t->CR1 & 0x20) && (t->ISR & 0x20)));
	qemu_set_irq(t->irq[0], conditions);

}

static int stm32l0_usart_can_receive(void *opaque) {
	return 1;
}

static void stm32l0_usart_receive(void *opaque, const uint8_t *buf, int size) {
	STM32L0USART *t = STM32L0_USART(opaque);


	if ((t->CR3 & 0x40) && t->dma) {
		stm32l0_dma_receive(t->dma, t->base + 36, *buf);
	}
	t->RDR = *buf;
	t->ISR |= 0x20;

	stm32l0_usart_update(t);
}

static gboolean stm32l0_usart_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	STM32L0USART *t = STM32L0_USART(opaque);
	int ret;

	t->watch_tag = 0;
	t->ISR &= (~(0x80));
	t->ISR &= (~(0x40));

	stm32l0_usart_update(t);

	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->TDR), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, stm32l0_usart_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->ISR |= 0x80;
	t->ISR |= 0x40;

	stm32l0_usart_update(t);

	return FALSE;
}

static void stm32l0_usart_register_reset(STM32L0USART *t) {
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
	t->ISR |= 0x80;
	t->ISR |= 0x40;

}

static uint64_t stm32l0_usart_read(void *opaque, hwaddr offset, unsigned size) {
	STM32L0USART *t = STM32L0_USART(opaque);
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
			stm32l0_usart_update(t);
			break;
		case A_USART_TDR:
			ret = t->TDR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32L0 USART read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32l0_usart_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32L0USART *t = STM32L0_USART(opaque);


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
			stm32l0_usart_update(t);
			break;
		case A_USART_CR2:
			t->CR2 = value;
			break;
		case A_USART_CR3:
			t->CR3 = value;
			stm32l0_usart_update(t);
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
			stm32l0_usart_transmit(NULL, G_IO_OUT, t);
			if (((!(value & 0x100)) && (!(value & 0x200)) && (!(value & 0x400)) && (!(value & 0x800)) && (!(value & 0x1000)) && (!(value & 0x2000)) && (!(value & 0x4000)) && (!(value & 0x8000)) && (!(value & 0x10000)) && (!(value & 0x20000)) && (!(value & 0x40000)) && (!(value & 0x80000)) && (!(value & 0x100000)) && (!(value & 0x200000)) && (!(value & 0x400000)) && (!(value & 0x800000)) && (!(value & 0x1000000)) && (!(value & 0x2000000)) && (!(value & 0x4000000)) && (!(value & 0x8000000)) && (!(value & 0x10000000)) && (!(value & 0x20000000)) && (!(value & 0x40000000)) && (!(value & 0x80000000)))) {
				t->ISR |= 0x80;
				t->ISR |= 0x40;
			} else if (((!(value & 0x200)) && (!(value & 0x400)) && (!(value & 0x800)) && (!(value & 0x1000)) && (!(value & 0x2000)) && (!(value & 0x4000)) && (!(value & 0x8000)) && (!(value & 0x10000)) && (!(value & 0x20000)) && (!(value & 0x40000)) && (!(value & 0x80000)) && (!(value & 0x100000)) && (!(value & 0x200000)) && (!(value & 0x400000)) && (!(value & 0x800000)) && (!(value & 0x1000000)) && (!(value & 0x2000000)) && (!(value & 0x4000000)) && (!(value & 0x8000000)) && (!(value & 0x10000000)) && (!(value & 0x20000000)) && (!(value & 0x40000000)) && (!(value & 0x80000000)))) {
				t->ISR |= 0x80;
				t->ISR |= 0x40;
			}
			t->ISR |= 0x40;
			stm32l0_usart_update(t);
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32L0 USART write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32l0_usart_ops = {
	.read = stm32l0_usart_read,
	.write = stm32l0_usart_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32l0_usart_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32L0USART *t = STM32L0_USART(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32l0_usart_ops, t, "stm32l0-usart", STM32L0_USART_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32l0_usart_realize(DeviceState *dev, Error **errp) {
	STM32L0USART *t = STM32L0_USART(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), stm32l0_usart_can_receive, stm32l0_usart_receive,
		NULL, NULL, t, NULL, true);

}

static void stm32l0_usart_reset_enter(Object *obj, ResetType type) {
	STM32L0USART *t = STM32L0_USART(obj);
	stm32l0_usart_register_reset(t);
}

static const VMStateDescription stm32l0_usart_vmstate = {
	.name = "stm32l0-usart",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR1, STM32L0USART),
		VMSTATE_UINT32(CR2, STM32L0USART),
		VMSTATE_UINT32(CR3, STM32L0USART),
		VMSTATE_UINT32(BRR, STM32L0USART),
		VMSTATE_UINT32(GTPR, STM32L0USART),
		VMSTATE_UINT32(RTOR, STM32L0USART),
		VMSTATE_UINT32(RQR, STM32L0USART),
		VMSTATE_UINT32(ISR, STM32L0USART),
		VMSTATE_UINT32(ICR, STM32L0USART),
		VMSTATE_UINT32(RDR, STM32L0USART),
		VMSTATE_UINT32(TDR, STM32L0USART),

		VMSTATE_END_OF_LIST()
	}
};

static Property stm32l0_usart_properties[] = {
	DEFINE_PROP_CHR("chardev", STM32L0USART, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void stm32l0_usart_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32l0_usart_vmstate;
	dc->realize = stm32l0_usart_realize;
	rc->phases.enter = stm32l0_usart_reset_enter;
	device_class_set_props(dc, stm32l0_usart_properties);

}

static const TypeInfo stm32l0_usart_info = {
	.name = TYPE_STM32L0_USART,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32L0USART),
	.instance_init = stm32l0_usart_init,
	.class_init = stm32l0_usart_class_init,
};

static void stm32l0_usart_register_types(void) {
	type_register_static(&stm32l0_usart_info);
}

type_init(stm32l0_usart_register_types);

#define TYPE_STM32L0_GPIOA "stm32l0-gpioa"
OBJECT_DECLARE_SIMPLE_TYPE(STM32L0GPIOA, STM32L0_GPIOA)

struct STM32L0GPIOA {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/*registers*/
	uint32_t MODER;
	uint32_t OTYPER;
	uint32_t OSPEEDR;
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
#define STM32L0_GPIOA_SIZE				0x2c

REG32(GPIOA_MODER, 0x0)
	FIELD(GPIOA_MODER, MODE0, 0, 2)
	FIELD(GPIOA_MODER, MODE1, 2, 2)
	FIELD(GPIOA_MODER, MODE2, 4, 2)
	FIELD(GPIOA_MODER, MODE3, 6, 2)
	FIELD(GPIOA_MODER, MODE4, 8, 2)
	FIELD(GPIOA_MODER, MODE5, 10, 2)
	FIELD(GPIOA_MODER, MODE6, 12, 2)
	FIELD(GPIOA_MODER, MODE7, 14, 2)
	FIELD(GPIOA_MODER, MODE8, 16, 2)
	FIELD(GPIOA_MODER, MODE9, 18, 2)
	FIELD(GPIOA_MODER, MODE10, 20, 2)
	FIELD(GPIOA_MODER, MODE11, 22, 2)
	FIELD(GPIOA_MODER, MODE12, 24, 2)
	FIELD(GPIOA_MODER, MODE13, 26, 2)
	FIELD(GPIOA_MODER, MODE14, 28, 2)
	FIELD(GPIOA_MODER, MODE15, 30, 2)
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
REG32(GPIOA_OSPEEDR, 0x8)
	FIELD(GPIOA_OSPEEDR, OSPEED15, 30, 2)
	FIELD(GPIOA_OSPEEDR, OSPEED14, 28, 2)
	FIELD(GPIOA_OSPEEDR, OSPEED13, 26, 2)
	FIELD(GPIOA_OSPEEDR, OSPEED12, 24, 2)
	FIELD(GPIOA_OSPEEDR, OSPEED11, 22, 2)
	FIELD(GPIOA_OSPEEDR, OSPEED10, 20, 2)
	FIELD(GPIOA_OSPEEDR, OSPEED9, 18, 2)
	FIELD(GPIOA_OSPEEDR, OSPEED8, 16, 2)
	FIELD(GPIOA_OSPEEDR, OSPEED7, 14, 2)
	FIELD(GPIOA_OSPEEDR, OSPEED6, 12, 2)
	FIELD(GPIOA_OSPEEDR, OSPEED5, 10, 2)
	FIELD(GPIOA_OSPEEDR, OSPEED4, 8, 2)
	FIELD(GPIOA_OSPEEDR, OSPEED3, 6, 2)
	FIELD(GPIOA_OSPEEDR, OSPEED2, 4, 2)
	FIELD(GPIOA_OSPEEDR, OSPEED1, 2, 2)
	FIELD(GPIOA_OSPEEDR, OSPEED0, 0, 2)
REG32(GPIOA_PUPDR, 0xc)
	FIELD(GPIOA_PUPDR, PUPD15, 30, 2)
	FIELD(GPIOA_PUPDR, PUPD14, 28, 2)
	FIELD(GPIOA_PUPDR, PUPD13, 26, 2)
	FIELD(GPIOA_PUPDR, PUPD12, 24, 2)
	FIELD(GPIOA_PUPDR, PUPD11, 22, 2)
	FIELD(GPIOA_PUPDR, PUPD10, 20, 2)
	FIELD(GPIOA_PUPDR, PUPD9, 18, 2)
	FIELD(GPIOA_PUPDR, PUPD8, 16, 2)
	FIELD(GPIOA_PUPDR, PUPD7, 14, 2)
	FIELD(GPIOA_PUPDR, PUPD6, 12, 2)
	FIELD(GPIOA_PUPDR, PUPD5, 10, 2)
	FIELD(GPIOA_PUPDR, PUPD4, 8, 2)
	FIELD(GPIOA_PUPDR, PUPD3, 6, 2)
	FIELD(GPIOA_PUPDR, PUPD2, 4, 2)
	FIELD(GPIOA_PUPDR, PUPD1, 2, 2)
	FIELD(GPIOA_PUPDR, PUPD0, 0, 2)
REG32(GPIOA_IDR, 0x10)
	FIELD(GPIOA_IDR, ID15, 15, 1)
	FIELD(GPIOA_IDR, ID14, 14, 1)
	FIELD(GPIOA_IDR, ID13, 13, 1)
	FIELD(GPIOA_IDR, ID12, 12, 1)
	FIELD(GPIOA_IDR, ID11, 11, 1)
	FIELD(GPIOA_IDR, ID10, 10, 1)
	FIELD(GPIOA_IDR, ID9, 9, 1)
	FIELD(GPIOA_IDR, ID8, 8, 1)
	FIELD(GPIOA_IDR, ID7, 7, 1)
	FIELD(GPIOA_IDR, ID6, 6, 1)
	FIELD(GPIOA_IDR, ID5, 5, 1)
	FIELD(GPIOA_IDR, ID4, 4, 1)
	FIELD(GPIOA_IDR, ID3, 3, 1)
	FIELD(GPIOA_IDR, ID2, 2, 1)
	FIELD(GPIOA_IDR, ID1, 1, 1)
	FIELD(GPIOA_IDR, ID0, 0, 1)
REG32(GPIOA_ODR, 0x14)
	FIELD(GPIOA_ODR, OD15, 15, 1)
	FIELD(GPIOA_ODR, OD14, 14, 1)
	FIELD(GPIOA_ODR, OD13, 13, 1)
	FIELD(GPIOA_ODR, OD12, 12, 1)
	FIELD(GPIOA_ODR, OD11, 11, 1)
	FIELD(GPIOA_ODR, OD10, 10, 1)
	FIELD(GPIOA_ODR, OD9, 9, 1)
	FIELD(GPIOA_ODR, OD8, 8, 1)
	FIELD(GPIOA_ODR, OD7, 7, 1)
	FIELD(GPIOA_ODR, OD6, 6, 1)
	FIELD(GPIOA_ODR, OD5, 5, 1)
	FIELD(GPIOA_ODR, OD4, 4, 1)
	FIELD(GPIOA_ODR, OD3, 3, 1)
	FIELD(GPIOA_ODR, OD2, 2, 1)
	FIELD(GPIOA_ODR, OD1, 1, 1)
	FIELD(GPIOA_ODR, OD0, 0, 1)
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
	FIELD(GPIOA_AFRL, AFSEL7, 28, 4)
	FIELD(GPIOA_AFRL, AFSEL6, 24, 4)
	FIELD(GPIOA_AFRL, AFSEL5, 20, 4)
	FIELD(GPIOA_AFRL, AFSEL4, 16, 4)
	FIELD(GPIOA_AFRL, AFSEL3, 12, 4)
	FIELD(GPIOA_AFRL, AFSEL2, 8, 4)
	FIELD(GPIOA_AFRL, AFSEL1, 4, 4)
	FIELD(GPIOA_AFRL, AFSEL0, 0, 4)
REG32(GPIOA_AFRH, 0x24)
	FIELD(GPIOA_AFRH, AFSEL15, 28, 4)
	FIELD(GPIOA_AFRH, AFSEL14, 24, 4)
	FIELD(GPIOA_AFRH, AFSEL13, 20, 4)
	FIELD(GPIOA_AFRH, AFSEL12, 16, 4)
	FIELD(GPIOA_AFRH, AFSEL11, 12, 4)
	FIELD(GPIOA_AFRH, AFSEL10, 8, 4)
	FIELD(GPIOA_AFRH, AFSEL9, 4, 4)
	FIELD(GPIOA_AFRH, AFSEL8, 0, 4)
REG32(GPIOA_BRR, 0x28)
	FIELD(GPIOA_BRR, BR15, 15, 1)
	FIELD(GPIOA_BRR, BR14, 14, 1)
	FIELD(GPIOA_BRR, BR13, 13, 1)
	FIELD(GPIOA_BRR, BR12, 12, 1)
	FIELD(GPIOA_BRR, BR11, 11, 1)
	FIELD(GPIOA_BRR, BR10, 10, 1)
	FIELD(GPIOA_BRR, BR9, 9, 1)
	FIELD(GPIOA_BRR, BR8, 8, 1)
	FIELD(GPIOA_BRR, BR7, 7, 1)
	FIELD(GPIOA_BRR, BR6, 6, 1)
	FIELD(GPIOA_BRR, BR5, 5, 1)
	FIELD(GPIOA_BRR, BR4, 4, 1)
	FIELD(GPIOA_BRR, BR3, 3, 1)
	FIELD(GPIOA_BRR, BR2, 2, 1)
	FIELD(GPIOA_BRR, BR1, 1, 1)
	FIELD(GPIOA_BRR, BR0, 0, 1)


static void stm32l0_gpioa_register_reset(STM32L0GPIOA *t) {
	t->MODER = 0xebfffcff;
	t->OTYPER = 0x0;
	t->OSPEEDR = 0x0;
	t->PUPDR = 0x24000000;
	t->IDR = 0x0;
	t->ODR = 0x0;
	t->BSRR = 0x0;
	t->LCKR = 0x0;
	t->AFRL = 0x0;
	t->AFRH = 0x0;
	t->BRR = 0x0;

}

static uint64_t stm32l0_gpioa_read(void *opaque, hwaddr offset, unsigned size) {
	STM32L0GPIOA *t = STM32L0_GPIOA(opaque);
	uint64_t ret;

	switch (offset) {
		case A_GPIOA_MODER:
			ret = t->MODER;
			break;
		case A_GPIOA_OTYPER:
			ret = t->OTYPER;
			break;
		case A_GPIOA_OSPEEDR:
			ret = t->OSPEEDR;
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

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32L0 GPIOA read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32l0_gpioa_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32L0GPIOA *t = STM32L0_GPIOA(opaque);


	switch (offset) {
		case A_GPIOA_MODER ... A_GPIOA_MODER + 3:
			value = value << ((offset - A_GPIOA_MODER) << 3);
			offset = A_GPIOA_MODER;
			break;
		case A_GPIOA_OTYPER ... A_GPIOA_OTYPER + 3:
			value = value << ((offset - A_GPIOA_OTYPER) << 3);
			offset = A_GPIOA_OTYPER;
			break;
		case A_GPIOA_OSPEEDR ... A_GPIOA_OSPEEDR + 3:
			value = value << ((offset - A_GPIOA_OSPEEDR) << 3);
			offset = A_GPIOA_OSPEEDR;
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
		case A_GPIOA_OSPEEDR:
			t->OSPEEDR = value;
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
			qemu_log_mask(LOG_GUEST_ERROR, "STM32L0 GPIOA write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32l0_gpioa_ops = {
	.read = stm32l0_gpioa_read,
	.write = stm32l0_gpioa_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32l0_gpioa_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32L0GPIOA *t = STM32L0_GPIOA(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32l0_gpioa_ops, t, "stm32l0-gpioa", STM32L0_GPIOA_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void stm32l0_gpioa_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32l0_gpioa_reset_enter(Object *obj, ResetType type) {
	STM32L0GPIOA *t = STM32L0_GPIOA(obj);
	stm32l0_gpioa_register_reset(t);
}

static const VMStateDescription stm32l0_gpioa_vmstate = {
	.name = "stm32l0-gpioa",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(MODER, STM32L0GPIOA),
		VMSTATE_UINT32(OTYPER, STM32L0GPIOA),
		VMSTATE_UINT32(OSPEEDR, STM32L0GPIOA),
		VMSTATE_UINT32(PUPDR, STM32L0GPIOA),
		VMSTATE_UINT32(IDR, STM32L0GPIOA),
		VMSTATE_UINT32(ODR, STM32L0GPIOA),
		VMSTATE_UINT32(BSRR, STM32L0GPIOA),
		VMSTATE_UINT32(LCKR, STM32L0GPIOA),
		VMSTATE_UINT32(AFRL, STM32L0GPIOA),
		VMSTATE_UINT32(AFRH, STM32L0GPIOA),
		VMSTATE_UINT32(BRR, STM32L0GPIOA),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32l0_gpioa_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32l0_gpioa_vmstate;
	dc->realize = stm32l0_gpioa_realize;
	rc->phases.enter = stm32l0_gpioa_reset_enter;

}

static const TypeInfo stm32l0_gpioa_info = {
	.name = TYPE_STM32L0_GPIOA,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32L0GPIOA),
	.instance_init = stm32l0_gpioa_init,
	.class_init = stm32l0_gpioa_class_init,
};

static void stm32l0_gpioa_register_types(void) {
	type_register_static(&stm32l0_gpioa_info);
}

type_init(stm32l0_gpioa_register_types);

#define TYPE_STM32L0_GPIOB "stm32l0-gpiob"
OBJECT_DECLARE_SIMPLE_TYPE(STM32L0GPIOB, STM32L0_GPIOB)

struct STM32L0GPIOB {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/*registers*/
	uint32_t MODER;
	uint32_t OTYPER;
	uint32_t OSPEEDR;
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
#define STM32L0_GPIOB_SIZE				0x2c

REG32(GPIOB_MODER, 0x0)
	FIELD(GPIOB_MODER, MODE15, 30, 2)
	FIELD(GPIOB_MODER, MODE14, 28, 2)
	FIELD(GPIOB_MODER, MODE13, 26, 2)
	FIELD(GPIOB_MODER, MODE12, 24, 2)
	FIELD(GPIOB_MODER, MODE11, 22, 2)
	FIELD(GPIOB_MODER, MODE10, 20, 2)
	FIELD(GPIOB_MODER, MODE9, 18, 2)
	FIELD(GPIOB_MODER, MODE8, 16, 2)
	FIELD(GPIOB_MODER, MODE7, 14, 2)
	FIELD(GPIOB_MODER, MODE6, 12, 2)
	FIELD(GPIOB_MODER, MODE5, 10, 2)
	FIELD(GPIOB_MODER, MODE4, 8, 2)
	FIELD(GPIOB_MODER, MODE3, 6, 2)
	FIELD(GPIOB_MODER, MODE2, 4, 2)
	FIELD(GPIOB_MODER, MODE1, 2, 2)
	FIELD(GPIOB_MODER, MODE0, 0, 2)
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
REG32(GPIOB_OSPEEDR, 0x8)
	FIELD(GPIOB_OSPEEDR, OSPEED15, 30, 2)
	FIELD(GPIOB_OSPEEDR, OSPEED14, 28, 2)
	FIELD(GPIOB_OSPEEDR, OSPEED13, 26, 2)
	FIELD(GPIOB_OSPEEDR, OSPEED12, 24, 2)
	FIELD(GPIOB_OSPEEDR, OSPEED11, 22, 2)
	FIELD(GPIOB_OSPEEDR, OSPEED10, 20, 2)
	FIELD(GPIOB_OSPEEDR, OSPEED9, 18, 2)
	FIELD(GPIOB_OSPEEDR, OSPEED8, 16, 2)
	FIELD(GPIOB_OSPEEDR, OSPEED7, 14, 2)
	FIELD(GPIOB_OSPEEDR, OSPEED6, 12, 2)
	FIELD(GPIOB_OSPEEDR, OSPEED5, 10, 2)
	FIELD(GPIOB_OSPEEDR, OSPEED4, 8, 2)
	FIELD(GPIOB_OSPEEDR, OSPEED3, 6, 2)
	FIELD(GPIOB_OSPEEDR, OSPEED2, 4, 2)
	FIELD(GPIOB_OSPEEDR, OSPEED1, 2, 2)
	FIELD(GPIOB_OSPEEDR, OSPEED0, 0, 2)
REG32(GPIOB_PUPDR, 0xc)
	FIELD(GPIOB_PUPDR, PUPD15, 30, 2)
	FIELD(GPIOB_PUPDR, PUPD14, 28, 2)
	FIELD(GPIOB_PUPDR, PUPD13, 26, 2)
	FIELD(GPIOB_PUPDR, PUPD12, 24, 2)
	FIELD(GPIOB_PUPDR, PUPD11, 22, 2)
	FIELD(GPIOB_PUPDR, PUPD10, 20, 2)
	FIELD(GPIOB_PUPDR, PUPD9, 18, 2)
	FIELD(GPIOB_PUPDR, PUPD8, 16, 2)
	FIELD(GPIOB_PUPDR, PUPD7, 14, 2)
	FIELD(GPIOB_PUPDR, PUPD6, 12, 2)
	FIELD(GPIOB_PUPDR, PUPD5, 10, 2)
	FIELD(GPIOB_PUPDR, PUPD4, 8, 2)
	FIELD(GPIOB_PUPDR, PUPD3, 6, 2)
	FIELD(GPIOB_PUPDR, PUPD2, 4, 2)
	FIELD(GPIOB_PUPDR, PUPD1, 2, 2)
	FIELD(GPIOB_PUPDR, PUPD0, 0, 2)
REG32(GPIOB_IDR, 0x10)
	FIELD(GPIOB_IDR, ID15, 15, 1)
	FIELD(GPIOB_IDR, ID14, 14, 1)
	FIELD(GPIOB_IDR, ID13, 13, 1)
	FIELD(GPIOB_IDR, ID12, 12, 1)
	FIELD(GPIOB_IDR, ID11, 11, 1)
	FIELD(GPIOB_IDR, ID10, 10, 1)
	FIELD(GPIOB_IDR, ID9, 9, 1)
	FIELD(GPIOB_IDR, ID8, 8, 1)
	FIELD(GPIOB_IDR, ID7, 7, 1)
	FIELD(GPIOB_IDR, ID6, 6, 1)
	FIELD(GPIOB_IDR, ID5, 5, 1)
	FIELD(GPIOB_IDR, ID4, 4, 1)
	FIELD(GPIOB_IDR, ID3, 3, 1)
	FIELD(GPIOB_IDR, ID2, 2, 1)
	FIELD(GPIOB_IDR, ID1, 1, 1)
	FIELD(GPIOB_IDR, ID0, 0, 1)
REG32(GPIOB_ODR, 0x14)
	FIELD(GPIOB_ODR, OD15, 15, 1)
	FIELD(GPIOB_ODR, OD14, 14, 1)
	FIELD(GPIOB_ODR, OD13, 13, 1)
	FIELD(GPIOB_ODR, OD12, 12, 1)
	FIELD(GPIOB_ODR, OD11, 11, 1)
	FIELD(GPIOB_ODR, OD10, 10, 1)
	FIELD(GPIOB_ODR, OD9, 9, 1)
	FIELD(GPIOB_ODR, OD8, 8, 1)
	FIELD(GPIOB_ODR, OD7, 7, 1)
	FIELD(GPIOB_ODR, OD6, 6, 1)
	FIELD(GPIOB_ODR, OD5, 5, 1)
	FIELD(GPIOB_ODR, OD4, 4, 1)
	FIELD(GPIOB_ODR, OD3, 3, 1)
	FIELD(GPIOB_ODR, OD2, 2, 1)
	FIELD(GPIOB_ODR, OD1, 1, 1)
	FIELD(GPIOB_ODR, OD0, 0, 1)
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
	FIELD(GPIOB_AFRL, AFSEL7, 28, 4)
	FIELD(GPIOB_AFRL, AFSEL6, 24, 4)
	FIELD(GPIOB_AFRL, AFSEL5, 20, 4)
	FIELD(GPIOB_AFRL, AFSEL4, 16, 4)
	FIELD(GPIOB_AFRL, AFSEL3, 12, 4)
	FIELD(GPIOB_AFRL, AFSEL2, 8, 4)
	FIELD(GPIOB_AFRL, AFSEL1, 4, 4)
	FIELD(GPIOB_AFRL, AFSEL0, 0, 4)
REG32(GPIOB_AFRH, 0x24)
	FIELD(GPIOB_AFRH, AFSEL15, 28, 4)
	FIELD(GPIOB_AFRH, AFSEL14, 24, 4)
	FIELD(GPIOB_AFRH, AFSEL13, 20, 4)
	FIELD(GPIOB_AFRH, AFSEL12, 16, 4)
	FIELD(GPIOB_AFRH, AFSEL11, 12, 4)
	FIELD(GPIOB_AFRH, AFSEL10, 8, 4)
	FIELD(GPIOB_AFRH, AFSEL9, 4, 4)
	FIELD(GPIOB_AFRH, AFSEL8, 0, 4)
REG32(GPIOB_BRR, 0x28)
	FIELD(GPIOB_BRR, BR15, 15, 1)
	FIELD(GPIOB_BRR, BR14, 14, 1)
	FIELD(GPIOB_BRR, BR13, 13, 1)
	FIELD(GPIOB_BRR, BR12, 12, 1)
	FIELD(GPIOB_BRR, BR11, 11, 1)
	FIELD(GPIOB_BRR, BR10, 10, 1)
	FIELD(GPIOB_BRR, BR9, 9, 1)
	FIELD(GPIOB_BRR, BR8, 8, 1)
	FIELD(GPIOB_BRR, BR7, 7, 1)
	FIELD(GPIOB_BRR, BR6, 6, 1)
	FIELD(GPIOB_BRR, BR5, 5, 1)
	FIELD(GPIOB_BRR, BR4, 4, 1)
	FIELD(GPIOB_BRR, BR3, 3, 1)
	FIELD(GPIOB_BRR, BR2, 2, 1)
	FIELD(GPIOB_BRR, BR1, 1, 1)
	FIELD(GPIOB_BRR, BR0, 0, 1)


static void stm32l0_gpiob_register_reset(STM32L0GPIOB *t) {
	t->MODER = 0xffffffff;
	t->OTYPER = 0x0;
	t->OSPEEDR = 0x0;
	t->PUPDR = 0x0;
	t->IDR = 0x0;
	t->ODR = 0x0;
	t->BSRR = 0x0;
	t->LCKR = 0x0;
	t->AFRL = 0x0;
	t->AFRH = 0x0;
	t->BRR = 0x0;

}

static uint64_t stm32l0_gpiob_read(void *opaque, hwaddr offset, unsigned size) {
	STM32L0GPIOB *t = STM32L0_GPIOB(opaque);
	uint64_t ret;

	switch (offset) {
		case A_GPIOB_MODER:
			ret = t->MODER;
			break;
		case A_GPIOB_OTYPER:
			ret = t->OTYPER;
			break;
		case A_GPIOB_OSPEEDR:
			ret = t->OSPEEDR;
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

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32L0 GPIOB read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32l0_gpiob_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32L0GPIOB *t = STM32L0_GPIOB(opaque);


	switch (offset) {
		case A_GPIOB_MODER ... A_GPIOB_MODER + 3:
			value = value << ((offset - A_GPIOB_MODER) << 3);
			offset = A_GPIOB_MODER;
			break;
		case A_GPIOB_OTYPER ... A_GPIOB_OTYPER + 3:
			value = value << ((offset - A_GPIOB_OTYPER) << 3);
			offset = A_GPIOB_OTYPER;
			break;
		case A_GPIOB_OSPEEDR ... A_GPIOB_OSPEEDR + 3:
			value = value << ((offset - A_GPIOB_OSPEEDR) << 3);
			offset = A_GPIOB_OSPEEDR;
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
		case A_GPIOB_OSPEEDR:
			t->OSPEEDR = value;
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
			qemu_log_mask(LOG_GUEST_ERROR, "STM32L0 GPIOB write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32l0_gpiob_ops = {
	.read = stm32l0_gpiob_read,
	.write = stm32l0_gpiob_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32l0_gpiob_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32L0GPIOB *t = STM32L0_GPIOB(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32l0_gpiob_ops, t, "stm32l0-gpiob", STM32L0_GPIOB_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void stm32l0_gpiob_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32l0_gpiob_reset_enter(Object *obj, ResetType type) {
	STM32L0GPIOB *t = STM32L0_GPIOB(obj);
	stm32l0_gpiob_register_reset(t);
}

static const VMStateDescription stm32l0_gpiob_vmstate = {
	.name = "stm32l0-gpiob",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(MODER, STM32L0GPIOB),
		VMSTATE_UINT32(OTYPER, STM32L0GPIOB),
		VMSTATE_UINT32(OSPEEDR, STM32L0GPIOB),
		VMSTATE_UINT32(PUPDR, STM32L0GPIOB),
		VMSTATE_UINT32(IDR, STM32L0GPIOB),
		VMSTATE_UINT32(ODR, STM32L0GPIOB),
		VMSTATE_UINT32(BSRR, STM32L0GPIOB),
		VMSTATE_UINT32(LCKR, STM32L0GPIOB),
		VMSTATE_UINT32(AFRL, STM32L0GPIOB),
		VMSTATE_UINT32(AFRH, STM32L0GPIOB),
		VMSTATE_UINT32(BRR, STM32L0GPIOB),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32l0_gpiob_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32l0_gpiob_vmstate;
	dc->realize = stm32l0_gpiob_realize;
	rc->phases.enter = stm32l0_gpiob_reset_enter;

}

static const TypeInfo stm32l0_gpiob_info = {
	.name = TYPE_STM32L0_GPIOB,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32L0GPIOB),
	.instance_init = stm32l0_gpiob_init,
	.class_init = stm32l0_gpiob_class_init,
};

static void stm32l0_gpiob_register_types(void) {
	type_register_static(&stm32l0_gpiob_info);
}

type_init(stm32l0_gpiob_register_types);

#define TYPE_STM32L0_SPI "stm32l0-spi"
OBJECT_DECLARE_SIMPLE_TYPE(STM32L0SPI, STM32L0_SPI)

struct STM32L0SPI {
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
	STM32L0DMA *dma;
	/* base */
	uint32_t base;

};
#define STM32L0_SPI_SIZE				0x24

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
	FIELD(SPI_CR2, RXDMAEN, 0, 1)
	FIELD(SPI_CR2, TXDMAEN, 1, 1)
	FIELD(SPI_CR2, SSOE, 2, 1)
	FIELD(SPI_CR2, FRF, 4, 1)
	FIELD(SPI_CR2, ERRIE, 5, 1)
	FIELD(SPI_CR2, RXNEIE, 6, 1)
	FIELD(SPI_CR2, TXEIE, 7, 1)
REG32(SPI_SR, 0x8)
	FIELD(SPI_SR, RXNE, 0, 1)
	FIELD(SPI_SR, TXE, 1, 1)
	FIELD(SPI_SR, CHSIDE, 2, 1)
	FIELD(SPI_SR, UDR, 3, 1)
	FIELD(SPI_SR, CRCERR, 4, 1)
	FIELD(SPI_SR, MODF, 5, 1)
	FIELD(SPI_SR, OVR, 6, 1)
	FIELD(SPI_SR, BSY, 7, 1)
	FIELD(SPI_SR, TIFRFE, 8, 1)
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
static void stm32l0_spi_update(STM32L0SPI *t) {
	int conditions = (((t->SR & 0x20) && (!(t->CR2 & 0x1)) && (!(t->CR2 & 0x2)) && (t->CR2 & 0x20)) || ((t->SR & 0x100) && (!(t->CR2 & 0x1)) && (!(t->CR2 & 0x2)) && (t->CR2 & 0x20)) || ((t->SR & 0x40) && (!(t->CR2 & 0x1)) && (!(t->CR2 & 0x2)) && (t->CR2 & 0x20)) || ((t->CR2 & 0x80) && (t->SR & 0x2)) || ((!(t->SR & 0x40)) && (t->CR2 & 0x40)));
	qemu_set_irq(t->irq[0], conditions);

}

static int stm32l0_spi_can_receive(void *opaque) {
	return 1;
}

static void stm32l0_spi_receive(void *opaque, const uint8_t *buf, int size) {
	STM32L0SPI *t = STM32L0_SPI(opaque);


	if ((t->CR2 & 0x1) && t->dma) {
		stm32l0_dma_receive(t->dma, t->base + 12, *buf);
	}
	t->DR = *buf;
	t->SR |= 0x1;

	stm32l0_spi_update(t);
}

static gboolean stm32l0_spi_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	STM32L0SPI *t = STM32L0_SPI(opaque);
	int ret;

	t->watch_tag = 0;
	t->SR &= (~(0x1));
	t->SR &= (~(0x2));
	t->SR |= 0x80;

	stm32l0_spi_update(t);

	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->DR), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, stm32l0_spi_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->SR |= 0x1;
	t->SR |= 0x2;
	t->SR &= (~(0x80));

	stm32l0_spi_update(t);

	return FALSE;
}

static void stm32l0_spi_register_reset(STM32L0SPI *t) {
	t->CR1 = 0x0;
	t->CR2 = 0x0;
	t->SR = 0x2;
	t->DR = 0x0;
	t->CRCPR = 0x7;
	t->RXCRCR = 0x0;
	t->TXCRCR = 0x0;
	t->I2SCFGR = 0x0;
	t->I2SPR = 0x10;
	t->SR |= 0x1;
	t->SR |= 0x2;
	t->SR &= (~(0x80));

}

static uint64_t stm32l0_spi_read(void *opaque, hwaddr offset, unsigned size) {
	STM32L0SPI *t = STM32L0_SPI(opaque);
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
			stm32l0_spi_update(t);
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
			qemu_log_mask(LOG_GUEST_ERROR, "STM32L0 SPI read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32l0_spi_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32L0SPI *t = STM32L0_SPI(opaque);


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
				t->SR &= (~(0x1));
				t->SR &= (~(0x80));
			}
			t->SR &= (~(0x1));
			t->SR &= (~(0x80));
			stm32l0_spi_update(t);
			break;
		case A_SPI_CR2:
			t->CR2 = value;
			if (((!(value & 0x20)) && (!(value & 0x40)))) {
				t->SR &= (~(0x1));
				t->SR &= (~(0x80));
			} else if (((!(value & 0x20)) && (!(value & 0x80)))) {
				t->SR &= (~(0x80));
			} else if ((!(value & 0x20))) {
				t->SR |= 0x2;
				t->SR &= (~(0x80));
			}
			stm32l0_spi_update(t);
			break;
		case A_SPI_SR:
			t->SR &= value;
			t->SR |= 0x1;
			t->SR |= 0x2;
			t->SR &= (~(0x80));
			stm32l0_spi_update(t);
			break;
		case A_SPI_DR:
			t->DR = value;
			stm32l0_spi_transmit(NULL, G_IO_OUT, t);
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
			qemu_log_mask(LOG_GUEST_ERROR, "STM32L0 SPI write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32l0_spi_ops = {
	.read = stm32l0_spi_read,
	.write = stm32l0_spi_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32l0_spi_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32L0SPI *t = STM32L0_SPI(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32l0_spi_ops, t, "stm32l0-spi", STM32L0_SPI_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32l0_spi_realize(DeviceState *dev, Error **errp) {
	STM32L0SPI *t = STM32L0_SPI(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), stm32l0_spi_can_receive, stm32l0_spi_receive,
		NULL, NULL, t, NULL, true);

}

static void stm32l0_spi_reset_enter(Object *obj, ResetType type) {
	STM32L0SPI *t = STM32L0_SPI(obj);
	stm32l0_spi_register_reset(t);
}

static const VMStateDescription stm32l0_spi_vmstate = {
	.name = "stm32l0-spi",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR1, STM32L0SPI),
		VMSTATE_UINT32(CR2, STM32L0SPI),
		VMSTATE_UINT32(SR, STM32L0SPI),
		VMSTATE_UINT32(DR, STM32L0SPI),
		VMSTATE_UINT32(CRCPR, STM32L0SPI),
		VMSTATE_UINT32(RXCRCR, STM32L0SPI),
		VMSTATE_UINT32(TXCRCR, STM32L0SPI),
		VMSTATE_UINT32(I2SCFGR, STM32L0SPI),
		VMSTATE_UINT32(I2SPR, STM32L0SPI),

		VMSTATE_END_OF_LIST()
	}
};

static Property stm32l0_spi_properties[] = {
	DEFINE_PROP_CHR("chardev", STM32L0SPI, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void stm32l0_spi_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32l0_spi_vmstate;
	dc->realize = stm32l0_spi_realize;
	rc->phases.enter = stm32l0_spi_reset_enter;
	device_class_set_props(dc, stm32l0_spi_properties);

}

static const TypeInfo stm32l0_spi_info = {
	.name = TYPE_STM32L0_SPI,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32L0SPI),
	.instance_init = stm32l0_spi_init,
	.class_init = stm32l0_spi_class_init,
};

static void stm32l0_spi_register_types(void) {
	type_register_static(&stm32l0_spi_info);
}

type_init(stm32l0_spi_register_types);

#define TYPE_STM32L0_RTC "stm32l0-rtc"
OBJECT_DECLARE_SIMPLE_TYPE(STM32L0RTC, STM32L0_RTC)

struct STM32L0RTC {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t TR;
	uint32_t DR;
	uint32_t CR;
	uint32_t ISR;
	uint32_t PRER;
	uint32_t WUTR;
	uint32_t ALRMAR;
	uint32_t ALRMBR;
	uint32_t WPR;
	uint32_t SSR;
	uint32_t SHIFTR;
	uint32_t TSTR;
	uint32_t TSDR;
	uint32_t TSSSR;
	uint32_t CALR;
	uint32_t TAMPCR;
	uint32_t ALRMASSR;
	uint32_t ALRMBSSR;
	uint32_t OR;
	uint32_t BKP0R;
	uint32_t BKP1R;
	uint32_t BKP2R;
	uint32_t BKP3R;
	uint32_t BKP4R;
	/* base */
	uint32_t base;

};
#define STM32L0_RTC_SIZE				0x64

REG32(RTC_TR, 0x0)
	FIELD(RTC_TR, PM, 22, 1)
	FIELD(RTC_TR, HT, 20, 2)
	FIELD(RTC_TR, HU, 16, 4)
	FIELD(RTC_TR, MNT, 12, 3)
	FIELD(RTC_TR, MNU, 8, 4)
	FIELD(RTC_TR, ST, 4, 3)
	FIELD(RTC_TR, SU, 0, 4)
REG32(RTC_DR, 0x4)
	FIELD(RTC_DR, YT, 20, 4)
	FIELD(RTC_DR, YU, 16, 4)
	FIELD(RTC_DR, WDU, 13, 3)
	FIELD(RTC_DR, MT, 12, 1)
	FIELD(RTC_DR, MU, 8, 4)
	FIELD(RTC_DR, DT, 4, 2)
	FIELD(RTC_DR, DU, 0, 4)
REG32(RTC_CR, 0x8)
	FIELD(RTC_CR, COE, 23, 1)
	FIELD(RTC_CR, OSEL, 21, 2)
	FIELD(RTC_CR, POL, 20, 1)
	FIELD(RTC_CR, COSEL, 19, 1)
	FIELD(RTC_CR, BKP, 18, 1)
	FIELD(RTC_CR, SUB1H, 17, 1)
	FIELD(RTC_CR, ADD1H, 16, 1)
	FIELD(RTC_CR, TSIE, 15, 1)
	FIELD(RTC_CR, WUTIE, 14, 1)
	FIELD(RTC_CR, ALRBIE, 13, 1)
	FIELD(RTC_CR, ALRAIE, 12, 1)
	FIELD(RTC_CR, TSE, 11, 1)
	FIELD(RTC_CR, WUTE, 10, 1)
	FIELD(RTC_CR, ALRBE, 9, 1)
	FIELD(RTC_CR, ALRAE, 8, 1)
	FIELD(RTC_CR, FMT, 6, 1)
	FIELD(RTC_CR, BYPSHAD, 5, 1)
	FIELD(RTC_CR, REFCKON, 4, 1)
	FIELD(RTC_CR, TSEDGE, 3, 1)
	FIELD(RTC_CR, WUCKSEL, 0, 3)
REG32(RTC_ISR, 0xc)
	FIELD(RTC_ISR, TAMP2F, 14, 1)
	FIELD(RTC_ISR, TAMP1F, 13, 1)
	FIELD(RTC_ISR, TSOVF, 12, 1)
	FIELD(RTC_ISR, TSF, 11, 1)
	FIELD(RTC_ISR, WUTF, 10, 1)
	FIELD(RTC_ISR, ALRBF, 9, 1)
	FIELD(RTC_ISR, ALRAF, 8, 1)
	FIELD(RTC_ISR, INIT, 7, 1)
	FIELD(RTC_ISR, INITF, 6, 1)
	FIELD(RTC_ISR, RSF, 5, 1)
	FIELD(RTC_ISR, INITS, 4, 1)
	FIELD(RTC_ISR, SHPF, 3, 1)
	FIELD(RTC_ISR, WUTWF, 2, 1)
	FIELD(RTC_ISR, ALRBWF, 1, 1)
	FIELD(RTC_ISR, ALRAWF, 0, 1)
REG32(RTC_PRER, 0x10)
	FIELD(RTC_PRER, PREDIV_A, 16, 7)
	FIELD(RTC_PRER, PREDIV_S, 0, 16)
REG32(RTC_WUTR, 0x14)
	FIELD(RTC_WUTR, WUT, 0, 16)
REG32(RTC_ALRMAR, 0x1c)
	FIELD(RTC_ALRMAR, MSK4, 31, 1)
	FIELD(RTC_ALRMAR, WDSEL, 30, 1)
	FIELD(RTC_ALRMAR, DT, 28, 2)
	FIELD(RTC_ALRMAR, DU, 24, 4)
	FIELD(RTC_ALRMAR, MSK3, 23, 1)
	FIELD(RTC_ALRMAR, PM, 22, 1)
	FIELD(RTC_ALRMAR, HT, 20, 2)
	FIELD(RTC_ALRMAR, HU, 16, 4)
	FIELD(RTC_ALRMAR, MSK2, 15, 1)
	FIELD(RTC_ALRMAR, MNT, 12, 3)
	FIELD(RTC_ALRMAR, MNU, 8, 4)
	FIELD(RTC_ALRMAR, MSK1, 7, 1)
	FIELD(RTC_ALRMAR, ST, 4, 3)
	FIELD(RTC_ALRMAR, SU, 0, 4)
REG32(RTC_ALRMBR, 0x20)
	FIELD(RTC_ALRMBR, MSK4, 31, 1)
	FIELD(RTC_ALRMBR, WDSEL, 30, 1)
	FIELD(RTC_ALRMBR, DT, 28, 2)
	FIELD(RTC_ALRMBR, DU, 24, 4)
	FIELD(RTC_ALRMBR, MSK3, 23, 1)
	FIELD(RTC_ALRMBR, PM, 22, 1)
	FIELD(RTC_ALRMBR, HT, 20, 2)
	FIELD(RTC_ALRMBR, HU, 16, 4)
	FIELD(RTC_ALRMBR, MSK2, 15, 1)
	FIELD(RTC_ALRMBR, MNT, 12, 3)
	FIELD(RTC_ALRMBR, MNU, 8, 4)
	FIELD(RTC_ALRMBR, MSK1, 7, 1)
	FIELD(RTC_ALRMBR, ST, 4, 3)
	FIELD(RTC_ALRMBR, SU, 0, 4)
REG32(RTC_WPR, 0x24)
	FIELD(RTC_WPR, KEY, 0, 8)
REG32(RTC_SSR, 0x28)
	FIELD(RTC_SSR, SS, 0, 16)
REG32(RTC_SHIFTR, 0x2c)
	FIELD(RTC_SHIFTR, ADD1S, 31, 1)
	FIELD(RTC_SHIFTR, SUBFS, 0, 15)
REG32(RTC_TSTR, 0x30)
	FIELD(RTC_TSTR, PM, 22, 1)
	FIELD(RTC_TSTR, HT, 20, 2)
	FIELD(RTC_TSTR, HU, 16, 4)
	FIELD(RTC_TSTR, MNT, 12, 3)
	FIELD(RTC_TSTR, MNU, 8, 4)
	FIELD(RTC_TSTR, ST, 4, 3)
	FIELD(RTC_TSTR, SU, 0, 4)
REG32(RTC_TSDR, 0x34)
	FIELD(RTC_TSDR, WDU, 13, 3)
	FIELD(RTC_TSDR, MT, 12, 1)
	FIELD(RTC_TSDR, MU, 8, 4)
	FIELD(RTC_TSDR, DT, 4, 2)
	FIELD(RTC_TSDR, DU, 0, 4)
REG32(RTC_TSSSR, 0x38)
	FIELD(RTC_TSSSR, SS, 0, 16)
REG32(RTC_CALR, 0x3c)
	FIELD(RTC_CALR, CALP, 15, 1)
	FIELD(RTC_CALR, CALW8, 14, 1)
	FIELD(RTC_CALR, CALW16, 13, 1)
	FIELD(RTC_CALR, CALM, 0, 9)
REG32(RTC_TAMPCR, 0x40)
	FIELD(RTC_TAMPCR, TAMP2MF, 21, 1)
	FIELD(RTC_TAMPCR, TAMP2NOERASE, 20, 1)
	FIELD(RTC_TAMPCR, TAMP2IE, 19, 1)
	FIELD(RTC_TAMPCR, TAMP1MF, 18, 1)
	FIELD(RTC_TAMPCR, TAMP1NOERASE, 17, 1)
	FIELD(RTC_TAMPCR, TAMP1IE, 16, 1)
	FIELD(RTC_TAMPCR, TAMPPUDIS, 15, 1)
	FIELD(RTC_TAMPCR, TAMPPRCH, 13, 2)
	FIELD(RTC_TAMPCR, TAMPFLT, 11, 2)
	FIELD(RTC_TAMPCR, TAMPFREQ, 8, 3)
	FIELD(RTC_TAMPCR, TAMPTS, 7, 1)
	FIELD(RTC_TAMPCR, TAMP2_TRG, 4, 1)
	FIELD(RTC_TAMPCR, TAMP2E, 3, 1)
	FIELD(RTC_TAMPCR, TAMPIE, 2, 1)
	FIELD(RTC_TAMPCR, TAMP1TRG, 1, 1)
	FIELD(RTC_TAMPCR, TAMP1E, 0, 1)
REG32(RTC_ALRMASSR, 0x44)
	FIELD(RTC_ALRMASSR, MASKSS, 24, 4)
	FIELD(RTC_ALRMASSR, SS, 0, 15)
REG32(RTC_ALRMBSSR, 0x48)
	FIELD(RTC_ALRMBSSR, MASKSS, 24, 4)
	FIELD(RTC_ALRMBSSR, SS, 0, 15)
REG32(RTC_OR, 0x4c)
	FIELD(RTC_OR, RTC_OUT_RMP, 1, 1)
	FIELD(RTC_OR, RTC_ALARM_TYPE, 0, 1)
REG32(RTC_BKP0R, 0x50)
	FIELD(RTC_BKP0R, BKP, 0, 32)
REG32(RTC_BKP1R, 0x54)
	FIELD(RTC_BKP1R, BKP, 0, 32)
REG32(RTC_BKP2R, 0x58)
	FIELD(RTC_BKP2R, BKP, 0, 32)
REG32(RTC_BKP3R, 0x5c)
	FIELD(RTC_BKP3R, BKP, 0, 32)
REG32(RTC_BKP4R, 0x60)
	FIELD(RTC_BKP4R, BKP, 0, 32)


__attribute__((unused))
static void stm32l0_rtc_update(STM32L0RTC *t) {
	int conditions = ((t->CR & 0x1000) && (t->ISR & 0x100));
	qemu_set_irq(t->irq[0], conditions);

}

static void stm32l0_rtc_register_reset(STM32L0RTC *t) {
	t->TR = 0x0;
	t->DR = 0x0;
	t->CR = 0x0;
	t->ISR = 0x0;
	t->PRER = 0x0;
	t->WUTR = 0x0;
	t->ALRMAR = 0x0;
	t->ALRMBR = 0x0;
	t->WPR = 0x0;
	t->SSR = 0x0;
	t->SHIFTR = 0x0;
	t->TSTR = 0x0;
	t->TSDR = 0x0;
	t->TSSSR = 0x0;
	t->CALR = 0x0;
	t->TAMPCR = 0x0;
	t->ALRMASSR = 0x0;
	t->ALRMBSSR = 0x0;
	t->OR = 0x0;
	t->BKP0R = 0x0;
	t->BKP1R = 0x0;
	t->BKP2R = 0x0;
	t->BKP3R = 0x0;
	t->BKP4R = 0x0;

}

static uint64_t stm32l0_rtc_read(void *opaque, hwaddr offset, unsigned size) {
	STM32L0RTC *t = STM32L0_RTC(opaque);
	uint64_t ret;

	switch (offset) {
		case A_RTC_TR:
			ret = t->TR;
			break;
		case A_RTC_DR:
			ret = t->DR;
			break;
		case A_RTC_CR:
			ret = t->CR;
			break;
		case A_RTC_ISR:
			ret = t->ISR;
			break;
		case A_RTC_PRER:
			ret = t->PRER;
			break;
		case A_RTC_WUTR:
			ret = t->WUTR;
			break;
		case A_RTC_ALRMAR:
			ret = t->ALRMAR;
			break;
		case A_RTC_ALRMBR:
			ret = t->ALRMBR;
			break;
		case A_RTC_SSR:
			ret = t->SSR;
			break;
		case A_RTC_TSTR:
			ret = t->TSTR;
			break;
		case A_RTC_TSDR:
			ret = t->TSDR;
			break;
		case A_RTC_TSSSR:
			ret = t->TSSSR;
			break;
		case A_RTC_CALR:
			ret = t->CALR;
			break;
		case A_RTC_TAMPCR:
			ret = t->TAMPCR;
			break;
		case A_RTC_ALRMASSR:
			ret = t->ALRMASSR;
			break;
		case A_RTC_ALRMBSSR:
			ret = t->ALRMBSSR;
			break;
		case A_RTC_OR:
			ret = t->OR;
			break;
		case A_RTC_BKP0R:
			ret = t->BKP0R;
			break;
		case A_RTC_BKP1R:
			ret = t->BKP1R;
			break;
		case A_RTC_BKP2R:
			ret = t->BKP2R;
			break;
		case A_RTC_BKP3R:
			ret = t->BKP3R;
			break;
		case A_RTC_BKP4R:
			ret = t->BKP4R;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32L0 RTC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32l0_rtc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32L0RTC *t = STM32L0_RTC(opaque);


	switch (offset) {
		case A_RTC_TR ... A_RTC_TR + 3:
			value = value << ((offset - A_RTC_TR) << 3);
			offset = A_RTC_TR;
			break;
		case A_RTC_DR ... A_RTC_DR + 3:
			value = value << ((offset - A_RTC_DR) << 3);
			offset = A_RTC_DR;
			break;
		case A_RTC_CR ... A_RTC_CR + 3:
			value = value << ((offset - A_RTC_CR) << 3);
			offset = A_RTC_CR;
			break;
		case A_RTC_ISR ... A_RTC_ISR + 3:
			value = value << ((offset - A_RTC_ISR) << 3);
			offset = A_RTC_ISR;
			break;
		case A_RTC_PRER ... A_RTC_PRER + 3:
			value = value << ((offset - A_RTC_PRER) << 3);
			offset = A_RTC_PRER;
			break;
		case A_RTC_WUTR ... A_RTC_WUTR + 3:
			value = value << ((offset - A_RTC_WUTR) << 3);
			offset = A_RTC_WUTR;
			break;
		case A_RTC_ALRMAR ... A_RTC_ALRMAR + 3:
			value = value << ((offset - A_RTC_ALRMAR) << 3);
			offset = A_RTC_ALRMAR;
			break;
		case A_RTC_ALRMBR ... A_RTC_ALRMBR + 3:
			value = value << ((offset - A_RTC_ALRMBR) << 3);
			offset = A_RTC_ALRMBR;
			break;
		case A_RTC_WPR ... A_RTC_WPR + 3:
			value = value << ((offset - A_RTC_WPR) << 3);
			offset = A_RTC_WPR;
			break;
		case A_RTC_SHIFTR ... A_RTC_SHIFTR + 3:
			value = value << ((offset - A_RTC_SHIFTR) << 3);
			offset = A_RTC_SHIFTR;
			break;
		case A_RTC_CALR ... A_RTC_CALR + 3:
			value = value << ((offset - A_RTC_CALR) << 3);
			offset = A_RTC_CALR;
			break;
		case A_RTC_TAMPCR ... A_RTC_TAMPCR + 3:
			value = value << ((offset - A_RTC_TAMPCR) << 3);
			offset = A_RTC_TAMPCR;
			break;
		case A_RTC_ALRMASSR ... A_RTC_ALRMASSR + 3:
			value = value << ((offset - A_RTC_ALRMASSR) << 3);
			offset = A_RTC_ALRMASSR;
			break;
		case A_RTC_ALRMBSSR ... A_RTC_ALRMBSSR + 3:
			value = value << ((offset - A_RTC_ALRMBSSR) << 3);
			offset = A_RTC_ALRMBSSR;
			break;
		case A_RTC_OR ... A_RTC_OR + 3:
			value = value << ((offset - A_RTC_OR) << 3);
			offset = A_RTC_OR;
			break;
		case A_RTC_BKP0R ... A_RTC_BKP0R + 3:
			value = value << ((offset - A_RTC_BKP0R) << 3);
			offset = A_RTC_BKP0R;
			break;
		case A_RTC_BKP1R ... A_RTC_BKP1R + 3:
			value = value << ((offset - A_RTC_BKP1R) << 3);
			offset = A_RTC_BKP1R;
			break;
		case A_RTC_BKP2R ... A_RTC_BKP2R + 3:
			value = value << ((offset - A_RTC_BKP2R) << 3);
			offset = A_RTC_BKP2R;
			break;
		case A_RTC_BKP3R ... A_RTC_BKP3R + 3:
			value = value << ((offset - A_RTC_BKP3R) << 3);
			offset = A_RTC_BKP3R;
			break;
		case A_RTC_BKP4R ... A_RTC_BKP4R + 3:
			value = value << ((offset - A_RTC_BKP4R) << 3);
			offset = A_RTC_BKP4R;
			break;

		default: break;
	}

	switch (offset) {
		case A_RTC_TR:
			t->TR = value;
			break;
		case A_RTC_DR:
			t->DR = value;
			break;
		case A_RTC_CR:
			t->CR = value;
			if (((!(value & 0x8)) && (!(value & 0x10)) && (!(value & 0x20)) && (!(value & 0x40)) && (!(value & 0x80)) && (!(value & 0x100)) && (!(value & 0x200)) && (!(value & 0x400)) && (!(value & 0x800)) && (!(value & 0x1000)) && (!(value & 0x2000)) && (!(value & 0x4000)) && (!(value & 0x8000)) && (!(value & 0x10000)) && (!(value & 0x20000)) && (!(value & 0x40000)) && (!(value & 0x80000)) && (!(value & 0x100000)) && (!(value & 0x200000)) && (!(value & 0x400000)) && (!(value & 0x800000)) && (!(value & 0x1000000)) && (!(value & 0x2000000)) && (!(value & 0x4000000)) && (!(value & 0x8000000)) && (!(value & 0x10000000)) && (!(value & 0x20000000)) && (!(value & 0x40000000)) && (!(value & 0x80000000)))) {
				t->ISR |= 0x4;
			} else if ((!(value & 0x2000))) {
				t->ISR |= 0x2;
			}
			if ((!(value & 0x1000))) {
				t->ISR |= 0x1;
			}
			stm32l0_rtc_update(t);
			break;
		case A_RTC_ISR:
			t->ISR = value;
			if (((value & 0x1) && (value & 0x2) && (value & 0x4) && (value & 0x8) && (value & 0x10) && (value & 0x20) && (value & 0x40) && (value & 0x100) && (!(value & 0x200)) && (value & 0x400) && (value & 0x800) && (value & 0x1000) && (value & 0x2000) && (value & 0x4000) && (value & 0x8000) && (value & 0x10000) && (value & 0x20000) && (value & 0x40000) && (value & 0x80000) && (value & 0x100000) && (value & 0x200000) && (value & 0x400000) && (value & 0x800000) && (value & 0x1000000) && (value & 0x2000000) && (value & 0x4000000) && (value & 0x8000000) && (value & 0x10000000) && (value & 0x20000000) && (value & 0x40000000) && (value & 0x80000000))) {
				t->ISR |= 0x2;
			} else if (((value & 0x1) && (value & 0x2) && (value & 0x4) && (value & 0x8) && (value & 0x10) && (value & 0x20) && (value & 0x40) && (!(value & 0x100)) && (value & 0x200) && (value & 0x400) && (value & 0x800) && (value & 0x1000) && (value & 0x2000) && (value & 0x4000) && (value & 0x8000) && (value & 0x10000) && (value & 0x20000) && (value & 0x40000) && (value & 0x80000) && (value & 0x100000) && (value & 0x200000) && (value & 0x400000) && (value & 0x800000) && (value & 0x1000000) && (value & 0x2000000) && (value & 0x4000000) && (value & 0x8000000) && (value & 0x10000000) && (value & 0x20000000) && (value & 0x40000000) && (value & 0x80000000))) {
				t->ISR |= 0x1;
			} else if (((!(value & 0x20)) && (!(value & 0x80)))) {
				t->ISR |= 0x20;
			}
			if ((value == 0xffffffff)) {
				t->ISR |= 0x40;
			}
			stm32l0_rtc_update(t);
			break;
		case A_RTC_PRER:
			t->PRER = value;
			break;
		case A_RTC_WUTR:
			t->WUTR = value;
			break;
		case A_RTC_ALRMAR:
			t->ALRMAR = value;
			break;
		case A_RTC_ALRMBR:
			t->ALRMBR = value;
			break;
		case A_RTC_WPR:
			t->WPR = value;
			break;
		case A_RTC_SHIFTR:
			t->SHIFTR = value;
			break;
		case A_RTC_CALR:
			t->CALR = value;
			break;
		case A_RTC_TAMPCR:
			t->TAMPCR = value;
			break;
		case A_RTC_ALRMASSR:
			t->ALRMASSR = value;
			break;
		case A_RTC_ALRMBSSR:
			t->ALRMBSSR = value;
			break;
		case A_RTC_OR:
			t->OR = value;
			break;
		case A_RTC_BKP0R:
			t->BKP0R = value;
			break;
		case A_RTC_BKP1R:
			t->BKP1R = value;
			break;
		case A_RTC_BKP2R:
			t->BKP2R = value;
			break;
		case A_RTC_BKP3R:
			t->BKP3R = value;
			break;
		case A_RTC_BKP4R:
			t->BKP4R = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32L0 RTC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32l0_rtc_ops = {
	.read = stm32l0_rtc_read,
	.write = stm32l0_rtc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32l0_rtc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32L0RTC *t = STM32L0_RTC(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32l0_rtc_ops, t, "stm32l0-rtc", STM32L0_RTC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32l0_rtc_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32l0_rtc_reset_enter(Object *obj, ResetType type) {
	STM32L0RTC *t = STM32L0_RTC(obj);
	stm32l0_rtc_register_reset(t);
}

static const VMStateDescription stm32l0_rtc_vmstate = {
	.name = "stm32l0-rtc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(TR, STM32L0RTC),
		VMSTATE_UINT32(DR, STM32L0RTC),
		VMSTATE_UINT32(CR, STM32L0RTC),
		VMSTATE_UINT32(ISR, STM32L0RTC),
		VMSTATE_UINT32(PRER, STM32L0RTC),
		VMSTATE_UINT32(WUTR, STM32L0RTC),
		VMSTATE_UINT32(ALRMAR, STM32L0RTC),
		VMSTATE_UINT32(ALRMBR, STM32L0RTC),
		VMSTATE_UINT32(WPR, STM32L0RTC),
		VMSTATE_UINT32(SSR, STM32L0RTC),
		VMSTATE_UINT32(SHIFTR, STM32L0RTC),
		VMSTATE_UINT32(TSTR, STM32L0RTC),
		VMSTATE_UINT32(TSDR, STM32L0RTC),
		VMSTATE_UINT32(TSSSR, STM32L0RTC),
		VMSTATE_UINT32(CALR, STM32L0RTC),
		VMSTATE_UINT32(TAMPCR, STM32L0RTC),
		VMSTATE_UINT32(ALRMASSR, STM32L0RTC),
		VMSTATE_UINT32(ALRMBSSR, STM32L0RTC),
		VMSTATE_UINT32(OR, STM32L0RTC),
		VMSTATE_UINT32(BKP0R, STM32L0RTC),
		VMSTATE_UINT32(BKP1R, STM32L0RTC),
		VMSTATE_UINT32(BKP2R, STM32L0RTC),
		VMSTATE_UINT32(BKP3R, STM32L0RTC),
		VMSTATE_UINT32(BKP4R, STM32L0RTC),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32l0_rtc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32l0_rtc_vmstate;
	dc->realize = stm32l0_rtc_realize;
	rc->phases.enter = stm32l0_rtc_reset_enter;

}

static const TypeInfo stm32l0_rtc_info = {
	.name = TYPE_STM32L0_RTC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32L0RTC),
	.instance_init = stm32l0_rtc_init,
	.class_init = stm32l0_rtc_class_init,
};

static void stm32l0_rtc_register_types(void) {
	type_register_static(&stm32l0_rtc_info);
}

type_init(stm32l0_rtc_register_types);

#define TYPE_STM32L0_EXTI "stm32l0-exti"
OBJECT_DECLARE_SIMPLE_TYPE(STM32L0EXTI, STM32L0_EXTI)

struct STM32L0EXTI {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[4];

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
#define STM32L0_EXTI_SIZE				0x18

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
	FIELD(EXTI_IMR, MR19, 19, 1)
	FIELD(EXTI_IMR, MR21, 21, 1)
	FIELD(EXTI_IMR, MR22, 22, 1)
	FIELD(EXTI_IMR, MR23, 23, 1)
	FIELD(EXTI_IMR, MR25, 25, 1)
	FIELD(EXTI_IMR, MR27, 27, 1)
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
	FIELD(EXTI_EMR, MR19, 19, 1)
	FIELD(EXTI_EMR, MR21, 21, 1)
	FIELD(EXTI_EMR, MR22, 22, 1)
	FIELD(EXTI_EMR, MR23, 23, 1)
	FIELD(EXTI_EMR, MR25, 25, 1)
	FIELD(EXTI_EMR, MR27, 27, 1)
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
	FIELD(EXTI_RTSR, TR19, 19, 1)
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
	FIELD(EXTI_FTSR, TR19, 19, 1)
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
	FIELD(EXTI_SWIER, SWIER19, 19, 1)
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
	FIELD(EXTI_PR, PR19, 19, 1)


static int stm32l0_exti_can_receive(void *opaque) {
	return 1;
}

static void stm32l0_exti_receive(void *opaque, const uint8_t *buf, int size) {
	STM32L0EXTI *t = STM32L0_EXTI(opaque);

	(void)t;


}

static void stm32l0_exti_register_reset(STM32L0EXTI *t) {
	t->IMR = 0xff840000;
	t->EMR = 0x0;
	t->RTSR = 0x0;
	t->FTSR = 0x0;
	t->SWIER = 0x0;
	t->PR = 0x0;

}

static uint64_t stm32l0_exti_read(void *opaque, hwaddr offset, unsigned size) {
	STM32L0EXTI *t = STM32L0_EXTI(opaque);
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
			qemu_log_mask(LOG_GUEST_ERROR, "STM32L0 EXTI read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32l0_exti_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32L0EXTI *t = STM32L0_EXTI(opaque);


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
			t->PR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32L0 EXTI write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32l0_exti_ops = {
	.read = stm32l0_exti_read,
	.write = stm32l0_exti_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32l0_exti_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32L0EXTI *t = STM32L0_EXTI(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32l0_exti_ops, t, "stm32l0-exti", STM32L0_EXTI_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 4; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32l0_exti_realize(DeviceState *dev, Error **errp) {
	STM32L0EXTI *t = STM32L0_EXTI(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), stm32l0_exti_can_receive, stm32l0_exti_receive,
		NULL, NULL, t, NULL, true);

}

static void stm32l0_exti_reset_enter(Object *obj, ResetType type) {
	STM32L0EXTI *t = STM32L0_EXTI(obj);
	stm32l0_exti_register_reset(t);
}

static const VMStateDescription stm32l0_exti_vmstate = {
	.name = "stm32l0-exti",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(IMR, STM32L0EXTI),
		VMSTATE_UINT32(EMR, STM32L0EXTI),
		VMSTATE_UINT32(RTSR, STM32L0EXTI),
		VMSTATE_UINT32(FTSR, STM32L0EXTI),
		VMSTATE_UINT32(SWIER, STM32L0EXTI),
		VMSTATE_UINT32(PR, STM32L0EXTI),

		VMSTATE_END_OF_LIST()
	}
};

static Property stm32l0_exti_properties[] = {
	DEFINE_PROP_CHR("chardev", STM32L0EXTI, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void stm32l0_exti_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32l0_exti_vmstate;
	dc->realize = stm32l0_exti_realize;
	rc->phases.enter = stm32l0_exti_reset_enter;
	device_class_set_props(dc, stm32l0_exti_properties);

}

static const TypeInfo stm32l0_exti_info = {
	.name = TYPE_STM32L0_EXTI,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32L0EXTI),
	.instance_init = stm32l0_exti_init,
	.class_init = stm32l0_exti_class_init,
};

static void stm32l0_exti_register_types(void) {
	type_register_static(&stm32l0_exti_info);
}

type_init(stm32l0_exti_register_types);

#define TYPE_STM32L0_I2C "stm32l0-i2c"
OBJECT_DECLARE_SIMPLE_TYPE(STM32L0I2C, STM32L0_I2C)

struct STM32L0I2C {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

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

	/* dma */
	STM32L0DMA *dma;
	/* base */
	uint32_t base;

};
#define STM32L0_I2C_SIZE				0x2c

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


static int stm32l0_i2c_can_receive(void *opaque) {
	return 1;
}

static void stm32l0_i2c_receive(void *opaque, const uint8_t *buf, int size) {
	STM32L0I2C *t = STM32L0_I2C(opaque);


	if ((t->CR1 & 0x8000) && t->dma) {
		stm32l0_dma_receive(t->dma, t->base + 36, *buf);
	}
	t->RXDR = *buf;
	t->ISR &= (~(0x10));
	t->ISR &= (~(0x8000));
	t->ISR |= 0x10;
	t->ISR |= 0x8;
	t->ISR &= (~(0x20));
	t->ISR |= 0x20;
	t->ISR &= (~(0x10000));
	t->ISR |= 0x4;


}

static gboolean stm32l0_i2c_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	STM32L0I2C *t = STM32L0_I2C(opaque);
	int ret;

	t->watch_tag = 0;
	t->ISR &= (~(0x10));
	t->ISR &= (~(0x8));
	t->ISR &= (~(0x80));
	t->ISR |= 0x8000;
	t->ISR &= (~(0x20));
	t->ISR &= (~(0x40));
	t->ISR &= (~(0x10000));
	t->ISR &= (~(0x2));
	t->ISR |= 0x4;



	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->TXDR), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, stm32l0_i2c_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->ISR |= 0x10000;
	t->ISR |= 0x2;
	t->ISR &= (~(0x8000));
	t->ISR &= (~(0x4));
	t->ISR |= 0x10;
	t->ISR |= 0x40;
	t->ISR |= 0x8;
	t->ISR |= 0x80;
	t->ISR |= 0x20;



	return FALSE;
}

static void stm32l0_i2c_register_reset(STM32L0I2C *t) {
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
	t->ISR |= 0x10000;
	t->ISR |= 0x2;
	t->ISR &= (~(0x8000));
	t->ISR &= (~(0x4));
	t->ISR |= 0x10;
	t->ISR |= 0x40;
	t->ISR |= 0x8;
	t->ISR |= 0x80;
	t->ISR |= 0x20;

}

static uint64_t stm32l0_i2c_read(void *opaque, hwaddr offset, unsigned size) {
	STM32L0I2C *t = STM32L0_I2C(opaque);
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
			t->ISR |= 0x10000;
			t->ISR &= (~(0x10));
			t->ISR &= (~(0x8));
			t->ISR &= (~(0x4));
			t->ISR |= 0x10;
			t->ISR |= 0x8000;
			t->ISR &= (~(0x20));
			t->ISR |= 0x20;
			qemu_chr_fe_accept_input(&(t->chr));
			break;
		case A_I2C_TXDR:
			ret = t->TXDR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32L0 I2C read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32l0_i2c_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32L0I2C *t = STM32L0_I2C(opaque);


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
			if (((value & 0x400) && (value & 0x2000) && (!(value & 0x4000)) && (!(value & 0x10000)) && (!(value & 0x20000)) && (!(value & 0x40000)) && (!(value & 0x80000)) && (!(value & 0x100000)) && (!(value & 0x200000)) && (!(value & 0x400000)) && (!(value & 0x800000)) && (!(value & 0x1000000)) && (value & 0x2000000) && (value & 0x80000000))) {
				t->ISR |= 0x20;
			} else if (((!(value & 0x400)) && (value & 0x2000) && (!(value & 0x4000)) && (!(value & 0x10000)) && (!(value & 0x20000)) && (!(value & 0x40000)) && (!(value & 0x80000)) && (!(value & 0x100000)) && (!(value & 0x200000)) && (!(value & 0x400000)) && (!(value & 0x800000)) && (!(value & 0x1000000)) && (value & 0x2000000) && (value & 0x80000000))) {
				t->ISR |= 0x20;
			} else if (((!(value & 0x400)) && (value & 0x2000) && (!(value & 0x4000)) && (!(value & 0x40000)) && (!(value & 0x80000)) && (!(value & 0x100000)) && (!(value & 0x200000)) && (!(value & 0x400000)) && (!(value & 0x800000)) && (value & 0x1000000) && (!(value & 0x2000000)) && (value & 0x80000000))) {
				t->ISR |= 0x2;
			} else if (((!(value & 0x400)) && (value & 0x2000) && (!(value & 0x4000)) && (!(value & 0x40000)) && (!(value & 0x80000)) && (!(value & 0x100000)) && (!(value & 0x200000)) && (!(value & 0x400000)) && (!(value & 0x800000)) && (!(value & 0x1000000)) && (!(value & 0x2000000)) && (value & 0x80000000))) {
				t->ISR |= 0x2;
			} else if (((!(value & 0x400)) && (value & 0x2000) && (!(value & 0x4000)) && (!(value & 0x1000000)) && (value & 0x2000000) && (value & 0x80000000))) {
				t->ISR |= 0x2;
			} else if (((value & 0x400) && (value & 0x2000) && (!(value & 0x4000)) && (!(value & 0x1000000)) && (value & 0x2000000) && (value & 0x80000000))) {
				t->ISR |= 0x4;
			} else if (((!(value & 0x2000)) && (!(value & 0x4000)) && (!(value & 0x1000000)) && (value & 0x2000000))) {
				t->ISR |= 0x2;
			} else if ((value & 0x4000)) {
				t->ISR |= 0x20;
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
			if ((value == 0x20)) {
				t->ISR &= (~(0x8000));
			}
			break;
		case A_I2C_TXDR:
			t->TXDR = value;
			stm32l0_i2c_transmit(NULL, G_IO_OUT, t);
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32L0 I2C write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32l0_i2c_ops = {
	.read = stm32l0_i2c_read,
	.write = stm32l0_i2c_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32l0_i2c_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32L0I2C *t = STM32L0_I2C(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32l0_i2c_ops, t, "stm32l0-i2c", STM32L0_I2C_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32l0_i2c_realize(DeviceState *dev, Error **errp) {
	STM32L0I2C *t = STM32L0_I2C(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), stm32l0_i2c_can_receive, stm32l0_i2c_receive,
		NULL, NULL, t, NULL, true);

}

static void stm32l0_i2c_reset_enter(Object *obj, ResetType type) {
	STM32L0I2C *t = STM32L0_I2C(obj);
	stm32l0_i2c_register_reset(t);
}

static const VMStateDescription stm32l0_i2c_vmstate = {
	.name = "stm32l0-i2c",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR1, STM32L0I2C),
		VMSTATE_UINT32(CR2, STM32L0I2C),
		VMSTATE_UINT32(OAR1, STM32L0I2C),
		VMSTATE_UINT32(OAR2, STM32L0I2C),
		VMSTATE_UINT32(TIMINGR, STM32L0I2C),
		VMSTATE_UINT32(TIMEOUTR, STM32L0I2C),
		VMSTATE_UINT32(ISR, STM32L0I2C),
		VMSTATE_UINT32(ICR, STM32L0I2C),
		VMSTATE_UINT32(PECR, STM32L0I2C),
		VMSTATE_UINT32(RXDR, STM32L0I2C),
		VMSTATE_UINT32(TXDR, STM32L0I2C),

		VMSTATE_END_OF_LIST()
	}
};

static Property stm32l0_i2c_properties[] = {
	DEFINE_PROP_CHR("chardev", STM32L0I2C, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void stm32l0_i2c_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32l0_i2c_vmstate;
	dc->realize = stm32l0_i2c_realize;
	rc->phases.enter = stm32l0_i2c_reset_enter;
	device_class_set_props(dc, stm32l0_i2c_properties);

}

static const TypeInfo stm32l0_i2c_info = {
	.name = TYPE_STM32L0_I2C,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32L0I2C),
	.instance_init = stm32l0_i2c_init,
	.class_init = stm32l0_i2c_class_init,
};

static void stm32l0_i2c_register_types(void) {
	type_register_static(&stm32l0_i2c_info);
}

type_init(stm32l0_i2c_register_types);

#define TYPE_STM32L0_LPTIM "stm32l0-lptim"
OBJECT_DECLARE_SIMPLE_TYPE(STM32L0LPTIM, STM32L0_LPTIM)

struct STM32L0LPTIM {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t ISR;
	uint32_t ICR;
	uint32_t IER;
	uint32_t CFGR;
	uint32_t CR;
	uint32_t CMP;
	uint32_t ARR;
	uint32_t CNT;
	/* base */
	uint32_t base;

};
#define STM32L0_LPTIM_SIZE				0x20

REG32(LPTIM_ISR, 0x0)
	FIELD(LPTIM_ISR, DOWN, 6, 1)
	FIELD(LPTIM_ISR, UP, 5, 1)
	FIELD(LPTIM_ISR, ARROK, 4, 1)
	FIELD(LPTIM_ISR, CMPOK, 3, 1)
	FIELD(LPTIM_ISR, EXTTRIG, 2, 1)
	FIELD(LPTIM_ISR, ARRM, 1, 1)
	FIELD(LPTIM_ISR, CMPM, 0, 1)
REG32(LPTIM_ICR, 0x4)
	FIELD(LPTIM_ICR, DOWNCF, 6, 1)
	FIELD(LPTIM_ICR, UPCF, 5, 1)
	FIELD(LPTIM_ICR, ARROKCF, 4, 1)
	FIELD(LPTIM_ICR, CMPOKCF, 3, 1)
	FIELD(LPTIM_ICR, EXTTRIGCF, 2, 1)
	FIELD(LPTIM_ICR, ARRMCF, 1, 1)
	FIELD(LPTIM_ICR, CMPMCF, 0, 1)
REG32(LPTIM_IER, 0x8)
	FIELD(LPTIM_IER, DOWNIE, 6, 1)
	FIELD(LPTIM_IER, UPIE, 5, 1)
	FIELD(LPTIM_IER, ARROKIE, 4, 1)
	FIELD(LPTIM_IER, CMPOKIE, 3, 1)
	FIELD(LPTIM_IER, EXTTRIGIE, 2, 1)
	FIELD(LPTIM_IER, ARRMIE, 1, 1)
	FIELD(LPTIM_IER, CMPMIE, 0, 1)
REG32(LPTIM_CFGR, 0xc)
	FIELD(LPTIM_CFGR, ENC, 24, 1)
	FIELD(LPTIM_CFGR, COUNTMODE, 23, 1)
	FIELD(LPTIM_CFGR, PRELOAD, 22, 1)
	FIELD(LPTIM_CFGR, WAVPOL, 21, 1)
	FIELD(LPTIM_CFGR, WAVE, 20, 1)
	FIELD(LPTIM_CFGR, TIMOUT, 19, 1)
	FIELD(LPTIM_CFGR, TRIGEN, 17, 2)
	FIELD(LPTIM_CFGR, TRIGSEL, 13, 3)
	FIELD(LPTIM_CFGR, PRESC, 9, 3)
	FIELD(LPTIM_CFGR, TRGFLT, 6, 2)
	FIELD(LPTIM_CFGR, CKFLT, 3, 2)
	FIELD(LPTIM_CFGR, CKPOL, 1, 2)
	FIELD(LPTIM_CFGR, CKSEL, 0, 1)
REG32(LPTIM_CR, 0x10)
	FIELD(LPTIM_CR, CNTSTRT, 2, 1)
	FIELD(LPTIM_CR, SNGSTRT, 1, 1)
	FIELD(LPTIM_CR, ENABLE, 0, 1)
REG32(LPTIM_CMP, 0x14)
	FIELD(LPTIM_CMP, CMP, 0, 16)
REG32(LPTIM_ARR, 0x18)
	FIELD(LPTIM_ARR, ARR, 0, 16)
REG32(LPTIM_CNT, 0x1c)
	FIELD(LPTIM_CNT, CNT, 0, 16)


__attribute__((unused))
static void stm32l0_lptim_update(STM32L0LPTIM *t) {
	int conditions = (((t->IER & 0x20) && (t->ISR & 0x20)) || ((t->IER & 0x4) && (t->ISR & 0x4)) || ((t->IER & 0x40) && (t->ISR & 0x40)) || ((t->ISR & 0x8) && (t->IER & 0x8)) || ((t->ISR & 0x2) && (t->IER & 0x2)) || ((t->ISR & 0x1) && (t->IER & 0x1)) || ((t->IER & 0x10) && (t->ISR & 0x10)));
	qemu_set_irq(t->irq[0], conditions);

}

static void stm32l0_lptim_register_reset(STM32L0LPTIM *t) {
	t->ISR = 0x0;
	t->ICR = 0x0;
	t->IER = 0x0;
	t->CFGR = 0x0;
	t->CR = 0x0;
	t->CMP = 0x0;
	t->ARR = 0x1;
	t->CNT = 0x0;

}

static uint64_t stm32l0_lptim_read(void *opaque, hwaddr offset, unsigned size) {
	STM32L0LPTIM *t = STM32L0_LPTIM(opaque);
	uint64_t ret;

	switch (offset) {
		case A_LPTIM_ISR:
			ret = t->ISR;
			break;
		case A_LPTIM_IER:
			ret = t->IER;
			break;
		case A_LPTIM_CFGR:
			ret = t->CFGR;
			break;
		case A_LPTIM_CR:
			ret = t->CR;
			break;
		case A_LPTIM_CMP:
			ret = t->CMP;
			break;
		case A_LPTIM_ARR:
			ret = t->ARR;
			break;
		case A_LPTIM_CNT:
			ret = t->CNT;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32L0 LPTIM read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32l0_lptim_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32L0LPTIM *t = STM32L0_LPTIM(opaque);


	switch (offset) {
		case A_LPTIM_ICR ... A_LPTIM_ICR + 3:
			value = value << ((offset - A_LPTIM_ICR) << 3);
			offset = A_LPTIM_ICR;
			break;
		case A_LPTIM_IER ... A_LPTIM_IER + 3:
			value = value << ((offset - A_LPTIM_IER) << 3);
			offset = A_LPTIM_IER;
			break;
		case A_LPTIM_CFGR ... A_LPTIM_CFGR + 3:
			value = value << ((offset - A_LPTIM_CFGR) << 3);
			offset = A_LPTIM_CFGR;
			break;
		case A_LPTIM_CR ... A_LPTIM_CR + 3:
			value = value << ((offset - A_LPTIM_CR) << 3);
			offset = A_LPTIM_CR;
			break;
		case A_LPTIM_CMP ... A_LPTIM_CMP + 3:
			value = value << ((offset - A_LPTIM_CMP) << 3);
			offset = A_LPTIM_CMP;
			break;
		case A_LPTIM_ARR ... A_LPTIM_ARR + 3:
			value = value << ((offset - A_LPTIM_ARR) << 3);
			offset = A_LPTIM_ARR;
			break;

		default: break;
	}

	switch (offset) {
		case A_LPTIM_ICR:
			t->ICR = value;
			break;
		case A_LPTIM_IER:
			t->IER = value;
			stm32l0_lptim_update(t);
			break;
		case A_LPTIM_CFGR:
			t->CFGR = value;
			break;
		case A_LPTIM_CR:
			t->CR = value;
			break;
		case A_LPTIM_CMP:
			t->CMP = value;
			if (((!(value & 0x10000)) && (!(value & 0x20000)) && (!(value & 0x40000)) && (!(value & 0x80000)) && (!(value & 0x100000)) && (!(value & 0x200000)) && (!(value & 0x400000)) && (!(value & 0x800000)) && (!(value & 0x1000000)) && (!(value & 0x2000000)) && (!(value & 0x4000000)) && (!(value & 0x8000000)) && (!(value & 0x10000000)) && (!(value & 0x20000000)) && (!(value & 0x40000000)) && (!(value & 0x80000000)))) {
				t->ISR |= 0x8;
			}
			t->ISR |= 0x8;
			stm32l0_lptim_update(t);
			break;
		case A_LPTIM_ARR:
			t->ARR = value;
			if (((!(value & 0x10000)) && (!(value & 0x20000)) && (!(value & 0x40000)) && (!(value & 0x80000)) && (!(value & 0x100000)) && (!(value & 0x200000)) && (!(value & 0x400000)) && (!(value & 0x800000)) && (!(value & 0x1000000)) && (!(value & 0x2000000)) && (!(value & 0x4000000)) && (!(value & 0x8000000)) && (!(value & 0x10000000)) && (!(value & 0x20000000)) && (!(value & 0x40000000)) && (!(value & 0x80000000)))) {
				t->ISR |= 0x10;
			}
			t->ISR |= 0x10;
			stm32l0_lptim_update(t);
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32L0 LPTIM write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32l0_lptim_ops = {
	.read = stm32l0_lptim_read,
	.write = stm32l0_lptim_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32l0_lptim_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32L0LPTIM *t = STM32L0_LPTIM(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32l0_lptim_ops, t, "stm32l0-lptim", STM32L0_LPTIM_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32l0_lptim_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32l0_lptim_reset_enter(Object *obj, ResetType type) {
	STM32L0LPTIM *t = STM32L0_LPTIM(obj);
	stm32l0_lptim_register_reset(t);
}

static const VMStateDescription stm32l0_lptim_vmstate = {
	.name = "stm32l0-lptim",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(ISR, STM32L0LPTIM),
		VMSTATE_UINT32(ICR, STM32L0LPTIM),
		VMSTATE_UINT32(IER, STM32L0LPTIM),
		VMSTATE_UINT32(CFGR, STM32L0LPTIM),
		VMSTATE_UINT32(CR, STM32L0LPTIM),
		VMSTATE_UINT32(CMP, STM32L0LPTIM),
		VMSTATE_UINT32(ARR, STM32L0LPTIM),
		VMSTATE_UINT32(CNT, STM32L0LPTIM),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32l0_lptim_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32l0_lptim_vmstate;
	dc->realize = stm32l0_lptim_realize;
	rc->phases.enter = stm32l0_lptim_reset_enter;

}

static const TypeInfo stm32l0_lptim_info = {
	.name = TYPE_STM32L0_LPTIM,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32L0LPTIM),
	.instance_init = stm32l0_lptim_init,
	.class_init = stm32l0_lptim_class_init,
};

static void stm32l0_lptim_register_types(void) {
	type_register_static(&stm32l0_lptim_info);
}

type_init(stm32l0_lptim_register_types);

#define TYPE_STM32L0_DBGMCU "stm32l0-dbgmcu"
OBJECT_DECLARE_SIMPLE_TYPE(STM32L0DBGMCU, STM32L0_DBGMCU)

struct STM32L0DBGMCU {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/*registers*/
	uint32_t IDCODE;
	uint32_t CR;
	uint32_t APB1_FZ;
	uint32_t APB2_FZ;
	/* base */
	uint32_t base;

};
#define STM32L0_DBGMCU_SIZE				0x10

REG32(DBGMCU_IDCODE, 0x0)
	FIELD(DBGMCU_IDCODE, DEV_ID, 0, 12)
	FIELD(DBGMCU_IDCODE, REV_ID, 16, 16)
REG32(DBGMCU_CR, 0x4)
	FIELD(DBGMCU_CR, DBG_STOP, 1, 1)
	FIELD(DBGMCU_CR, DBG_STANDBY, 2, 1)
	FIELD(DBGMCU_CR, DBG_SLEEP, 0, 1)
REG32(DBGMCU_APB1_FZ, 0x8)
	FIELD(DBGMCU_APB1_FZ, DBG_TIMER2_STOP, 0, 1)
	FIELD(DBGMCU_APB1_FZ, DBG_TIMER6_STOP, 4, 1)
	FIELD(DBGMCU_APB1_FZ, DBG_RTC_STOP, 10, 1)
	FIELD(DBGMCU_APB1_FZ, DBG_WWDG_STOP, 11, 1)
	FIELD(DBGMCU_APB1_FZ, DBG_IWDG_STOP, 12, 1)
	FIELD(DBGMCU_APB1_FZ, DBG_I2C1_STOP, 21, 1)
	FIELD(DBGMCU_APB1_FZ, DBG_I2C2_STOP, 22, 1)
	FIELD(DBGMCU_APB1_FZ, DBG_I2C3_STOP, 30, 1)
	FIELD(DBGMCU_APB1_FZ, DBG_LPTIMER_STOP, 31, 1)
REG32(DBGMCU_APB2_FZ, 0xc)
	FIELD(DBGMCU_APB2_FZ, DBG_TIMER21_STOP, 2, 1)
	FIELD(DBGMCU_APB2_FZ, DBG_TIMER22_STOP, 5, 1)


static void stm32l0_dbgmcu_register_reset(STM32L0DBGMCU *t) {
	t->IDCODE = 0x0;
	t->CR = 0x0;
	t->APB1_FZ = 0x0;
	t->APB2_FZ = 0x0;

}

static uint64_t stm32l0_dbgmcu_read(void *opaque, hwaddr offset, unsigned size) {
	STM32L0DBGMCU *t = STM32L0_DBGMCU(opaque);
	uint64_t ret;

	switch (offset) {
		case A_DBGMCU_IDCODE:
			ret = t->IDCODE;
			break;
		case A_DBGMCU_CR:
			ret = t->CR;
			break;
		case A_DBGMCU_APB1_FZ:
			ret = t->APB1_FZ;
			break;
		case A_DBGMCU_APB2_FZ:
			ret = t->APB2_FZ;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32L0 DBGMCU read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32l0_dbgmcu_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32L0DBGMCU *t = STM32L0_DBGMCU(opaque);


	switch (offset) {
		case A_DBGMCU_CR ... A_DBGMCU_CR + 3:
			value = value << ((offset - A_DBGMCU_CR) << 3);
			offset = A_DBGMCU_CR;
			break;
		case A_DBGMCU_APB1_FZ ... A_DBGMCU_APB1_FZ + 3:
			value = value << ((offset - A_DBGMCU_APB1_FZ) << 3);
			offset = A_DBGMCU_APB1_FZ;
			break;
		case A_DBGMCU_APB2_FZ ... A_DBGMCU_APB2_FZ + 3:
			value = value << ((offset - A_DBGMCU_APB2_FZ) << 3);
			offset = A_DBGMCU_APB2_FZ;
			break;

		default: break;
	}

	switch (offset) {
		case A_DBGMCU_CR:
			t->CR = value;
			break;
		case A_DBGMCU_APB1_FZ:
			t->APB1_FZ = value;
			break;
		case A_DBGMCU_APB2_FZ:
			t->APB2_FZ = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32L0 DBGMCU write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32l0_dbgmcu_ops = {
	.read = stm32l0_dbgmcu_read,
	.write = stm32l0_dbgmcu_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32l0_dbgmcu_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32L0DBGMCU *t = STM32L0_DBGMCU(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32l0_dbgmcu_ops, t, "stm32l0-dbgmcu", STM32L0_DBGMCU_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void stm32l0_dbgmcu_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32l0_dbgmcu_reset_enter(Object *obj, ResetType type) {
	STM32L0DBGMCU *t = STM32L0_DBGMCU(obj);
	stm32l0_dbgmcu_register_reset(t);
}

static const VMStateDescription stm32l0_dbgmcu_vmstate = {
	.name = "stm32l0-dbgmcu",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(IDCODE, STM32L0DBGMCU),
		VMSTATE_UINT32(CR, STM32L0DBGMCU),
		VMSTATE_UINT32(APB1_FZ, STM32L0DBGMCU),
		VMSTATE_UINT32(APB2_FZ, STM32L0DBGMCU),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32l0_dbgmcu_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32l0_dbgmcu_vmstate;
	dc->realize = stm32l0_dbgmcu_realize;
	rc->phases.enter = stm32l0_dbgmcu_reset_enter;

}

static const TypeInfo stm32l0_dbgmcu_info = {
	.name = TYPE_STM32L0_DBGMCU,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32L0DBGMCU),
	.instance_init = stm32l0_dbgmcu_init,
	.class_init = stm32l0_dbgmcu_class_init,
};

static void stm32l0_dbgmcu_register_types(void) {
	type_register_static(&stm32l0_dbgmcu_info);
}

type_init(stm32l0_dbgmcu_register_types);

#define TYPE_STM32L0_CRC "stm32l0-crc"
OBJECT_DECLARE_SIMPLE_TYPE(STM32L0CRC, STM32L0_CRC)

struct STM32L0CRC {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/*registers*/
	uint32_t DR;
	uint32_t IDR;
	uint32_t CR;
	uint32_t INIT;
	uint32_t POL;
	/* base */
	uint32_t base;

};
#define STM32L0_CRC_SIZE				0x18

REG32(CRC_DR, 0x0)
	FIELD(CRC_DR, DR, 0, 32)
REG32(CRC_IDR, 0x4)
	FIELD(CRC_IDR, IDR, 0, 8)
REG32(CRC_CR, 0x8)
	FIELD(CRC_CR, REV_OUT, 7, 1)
	FIELD(CRC_CR, REV_IN, 5, 2)
	FIELD(CRC_CR, POLYSIZE, 3, 2)
	FIELD(CRC_CR, RESET, 0, 1)
REG32(CRC_INIT, 0x10)
	FIELD(CRC_INIT, CRC_INIT, 0, 32)
REG32(CRC_POL, 0x14)
	FIELD(CRC_POL, Polynomialcoefficients, 0, 32)


static void stm32l0_crc_register_reset(STM32L0CRC *t) {
	t->DR = 0xffffffff;
	t->IDR = 0x0;
	t->CR = 0x0;
	t->INIT = 0xffffffff;
	t->POL = 0x4c11db7;

}

static uint64_t stm32l0_crc_read(void *opaque, hwaddr offset, unsigned size) {
	STM32L0CRC *t = STM32L0_CRC(opaque);
	uint64_t ret;

	switch (offset) {
		case A_CRC_DR:
			ret = t->DR;
			break;
		case A_CRC_IDR:
			ret = t->IDR;
			break;
		case A_CRC_CR:
			ret = t->CR;
			break;
		case A_CRC_INIT:
			ret = t->INIT;
			break;
		case A_CRC_POL:
			ret = t->POL;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32L0 CRC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32l0_crc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32L0CRC *t = STM32L0_CRC(opaque);


	switch (offset) {
		case A_CRC_DR ... A_CRC_DR + 3:
			value = value << ((offset - A_CRC_DR) << 3);
			offset = A_CRC_DR;
			break;
		case A_CRC_IDR ... A_CRC_IDR + 3:
			value = value << ((offset - A_CRC_IDR) << 3);
			offset = A_CRC_IDR;
			break;
		case A_CRC_CR ... A_CRC_CR + 3:
			value = value << ((offset - A_CRC_CR) << 3);
			offset = A_CRC_CR;
			break;
		case A_CRC_INIT ... A_CRC_INIT + 3:
			value = value << ((offset - A_CRC_INIT) << 3);
			offset = A_CRC_INIT;
			break;
		case A_CRC_POL ... A_CRC_POL + 3:
			value = value << ((offset - A_CRC_POL) << 3);
			offset = A_CRC_POL;
			break;

		default: break;
	}

	switch (offset) {
		case A_CRC_DR:
			t->DR = value;
			break;
		case A_CRC_IDR:
			t->IDR = value;
			break;
		case A_CRC_CR:
			t->CR = value;
			break;
		case A_CRC_INIT:
			t->INIT = value;
			break;
		case A_CRC_POL:
			t->POL = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32L0 CRC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32l0_crc_ops = {
	.read = stm32l0_crc_read,
	.write = stm32l0_crc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32l0_crc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32L0CRC *t = STM32L0_CRC(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32l0_crc_ops, t, "stm32l0-crc", STM32L0_CRC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void stm32l0_crc_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32l0_crc_reset_enter(Object *obj, ResetType type) {
	STM32L0CRC *t = STM32L0_CRC(obj);
	stm32l0_crc_register_reset(t);
}

static const VMStateDescription stm32l0_crc_vmstate = {
	.name = "stm32l0-crc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(DR, STM32L0CRC),
		VMSTATE_UINT32(IDR, STM32L0CRC),
		VMSTATE_UINT32(CR, STM32L0CRC),
		VMSTATE_UINT32(INIT, STM32L0CRC),
		VMSTATE_UINT32(POL, STM32L0CRC),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32l0_crc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32l0_crc_vmstate;
	dc->realize = stm32l0_crc_realize;
	rc->phases.enter = stm32l0_crc_reset_enter;

}

static const TypeInfo stm32l0_crc_info = {
	.name = TYPE_STM32L0_CRC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32L0CRC),
	.instance_init = stm32l0_crc_init,
	.class_init = stm32l0_crc_class_init,
};

static void stm32l0_crc_register_types(void) {
	type_register_static(&stm32l0_crc_info);
}

type_init(stm32l0_crc_register_types);

#define TYPE_STM32L0_IWDG "stm32l0-iwdg"
OBJECT_DECLARE_SIMPLE_TYPE(STM32L0IWDG, STM32L0_IWDG)

struct STM32L0IWDG {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/*registers*/
	uint32_t KR;
	uint32_t PR;
	uint32_t RLR;
	uint32_t SR;
	uint32_t WINR;
	/* base */
	uint32_t base;

};
#define STM32L0_IWDG_SIZE				0x14

REG32(IWDG_KR, 0x0)
	FIELD(IWDG_KR, KEY, 0, 16)
REG32(IWDG_PR, 0x4)
	FIELD(IWDG_PR, PR, 0, 3)
REG32(IWDG_RLR, 0x8)
	FIELD(IWDG_RLR, RL, 0, 12)
REG32(IWDG_SR, 0xc)
	FIELD(IWDG_SR, WVU, 2, 1)
	FIELD(IWDG_SR, RVU, 1, 1)
	FIELD(IWDG_SR, PVU, 0, 1)
REG32(IWDG_WINR, 0x10)
	FIELD(IWDG_WINR, WIN, 0, 12)


static void stm32l0_iwdg_register_reset(STM32L0IWDG *t) {
	t->KR = 0x0;
	t->PR = 0x0;
	t->RLR = 0xfff;
	t->SR = 0x0;
	t->WINR = 0xfff;

}

static uint64_t stm32l0_iwdg_read(void *opaque, hwaddr offset, unsigned size) {
	STM32L0IWDG *t = STM32L0_IWDG(opaque);
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
		case A_IWDG_WINR:
			ret = t->WINR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32L0 IWDG read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32l0_iwdg_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32L0IWDG *t = STM32L0_IWDG(opaque);


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
		case A_IWDG_WINR ... A_IWDG_WINR + 3:
			value = value << ((offset - A_IWDG_WINR) << 3);
			offset = A_IWDG_WINR;
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
		case A_IWDG_WINR:
			t->WINR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32L0 IWDG write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32l0_iwdg_ops = {
	.read = stm32l0_iwdg_read,
	.write = stm32l0_iwdg_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32l0_iwdg_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32L0IWDG *t = STM32L0_IWDG(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32l0_iwdg_ops, t, "stm32l0-iwdg", STM32L0_IWDG_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void stm32l0_iwdg_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32l0_iwdg_reset_enter(Object *obj, ResetType type) {
	STM32L0IWDG *t = STM32L0_IWDG(obj);
	stm32l0_iwdg_register_reset(t);
}

static const VMStateDescription stm32l0_iwdg_vmstate = {
	.name = "stm32l0-iwdg",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(KR, STM32L0IWDG),
		VMSTATE_UINT32(PR, STM32L0IWDG),
		VMSTATE_UINT32(RLR, STM32L0IWDG),
		VMSTATE_UINT32(SR, STM32L0IWDG),
		VMSTATE_UINT32(WINR, STM32L0IWDG),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32l0_iwdg_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32l0_iwdg_vmstate;
	dc->realize = stm32l0_iwdg_realize;
	rc->phases.enter = stm32l0_iwdg_reset_enter;

}

static const TypeInfo stm32l0_iwdg_info = {
	.name = TYPE_STM32L0_IWDG,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32L0IWDG),
	.instance_init = stm32l0_iwdg_init,
	.class_init = stm32l0_iwdg_class_init,
};

static void stm32l0_iwdg_register_types(void) {
	type_register_static(&stm32l0_iwdg_info);
}

type_init(stm32l0_iwdg_register_types);

#define TYPE_STM32L073_MACHINE MACHINE_TYPE_NAME("stm32l073")
OBJECT_DECLARE_TYPE(STM32L073MachineState, STM32L073MachineClass, STM32L073_MACHINE)

struct STM32L073MachineClass {
	MachineClass parent;
};

struct STM32L073MachineState {
	MachineState parent;
	ARMv7MState armv7m;
};

static void stm32l073_periph_init(MachineState *machine) {
	STM32L073MachineState *sms = STM32L073_MACHINE(machine);
	STM32L0DMA *p0 = g_new(STM32L0DMA, 1);
	object_initialize_child(OBJECT(sms), "DMA1", p0, TYPE_STM32L0_DMA);
	sysbus_realize(SYS_BUS_DEVICE(p0), &error_fatal);
	p0->base = 0x40020000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p0), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 9));
	sysbus_connect_irq(SYS_BUS_DEVICE(p0), 1, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 10));
	sysbus_connect_irq(SYS_BUS_DEVICE(p0), 2, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 11));
	sysbus_mmio_map(SYS_BUS_DEVICE(p0), 0, 0x40020000);

	STM32L0TIM2 *p1 = g_new(STM32L0TIM2, 1);
	object_initialize_child(OBJECT(sms), "TIM3", p1, TYPE_STM32L0_TIM2);
	sysbus_realize(SYS_BUS_DEVICE(p1), &error_fatal);
	p1->base = 0x40000400;
	sysbus_connect_irq(SYS_BUS_DEVICE(p1), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 15));
	sysbus_mmio_map(SYS_BUS_DEVICE(p1), 0, 0x40000400);

	STM32L0USART *p2 = g_new(STM32L0USART, 1);
	object_initialize_child(OBJECT(sms), "USART1", p2, TYPE_STM32L0_USART);
	qdev_prop_set_chr(DEVICE(p2), "chardev", qemu_chr_new("soc-usart1", "chardev:usart1", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p2), &error_fatal);
	p2->base = 0x40013800;
	sysbus_connect_irq(SYS_BUS_DEVICE(p2), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 27));
	sysbus_mmio_map(SYS_BUS_DEVICE(p2), 0, 0x40013800);

	STM32L0USART *p3 = g_new(STM32L0USART, 1);
	object_initialize_child(OBJECT(sms), "USART2", p3, TYPE_STM32L0_USART);
	qdev_prop_set_chr(DEVICE(p3), "chardev", qemu_chr_new("soc-usart2", "chardev:usart2", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p3), &error_fatal);
	p3->base = 0x40004400;
	sysbus_connect_irq(SYS_BUS_DEVICE(p3), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 28));
	sysbus_mmio_map(SYS_BUS_DEVICE(p3), 0, 0x40004400);

	STM32L0USART *p4 = g_new(STM32L0USART, 1);
	object_initialize_child(OBJECT(sms), "LPUSART1", p4, TYPE_STM32L0_USART);
	qdev_prop_set_chr(DEVICE(p4), "chardev", qemu_chr_new("soc-lpusart1", "chardev:lpusart1", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p4), &error_fatal);
	p4->base = 0x40004800;
	sysbus_mmio_map(SYS_BUS_DEVICE(p4), 0, 0x40004800);

	STM32L0PWR *p5 = g_new(STM32L0PWR, 1);
	object_initialize_child(OBJECT(sms), "PWR", p5, TYPE_STM32L0_PWR);
	sysbus_realize(SYS_BUS_DEVICE(p5), &error_fatal);
	p5->base = 0x40007000;
	sysbus_mmio_map(SYS_BUS_DEVICE(p5), 0, 0x40007000);

	STM32L0RCC *p6 = g_new(STM32L0RCC, 1);
	object_initialize_child(OBJECT(sms), "RCC", p6, TYPE_STM32L0_RCC);
	sysbus_realize(SYS_BUS_DEVICE(p6), &error_fatal);
	p6->base = 0x40021000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p6), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 4));
	sysbus_mmio_map(SYS_BUS_DEVICE(p6), 0, 0x40021000);

	STM32L0SYSCFG *p7 = g_new(STM32L0SYSCFG, 1);
	object_initialize_child(OBJECT(sms), "SYSCFG", p7, TYPE_STM32L0_SYSCFG);
	sysbus_realize(SYS_BUS_DEVICE(p7), &error_fatal);
	p7->base = 0x40010000;
	sysbus_mmio_map(SYS_BUS_DEVICE(p7), 0, 0x40010000);
	// PERRY PATCH
	p7->cpu = ARM_CPU(first_cpu);

	STM32L0FLASH *p8 = g_new(STM32L0FLASH, 1);
	object_initialize_child(OBJECT(sms), "Flash", p8, TYPE_STM32L0_FLASH);
	sysbus_realize(SYS_BUS_DEVICE(p8), &error_fatal);
	p8->base = 0x40022000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p8), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 3));
	sysbus_mmio_map(SYS_BUS_DEVICE(p8), 0, 0x40022000);

	STM32L0GPIOA *p9 = g_new(STM32L0GPIOA, 1);
	object_initialize_child(OBJECT(sms), "GPIOA", p9, TYPE_STM32L0_GPIOA);
	sysbus_realize(SYS_BUS_DEVICE(p9), &error_fatal);
	p9->base = 0x50000000;
	sysbus_mmio_map(SYS_BUS_DEVICE(p9), 0, 0x50000000);

	STM32L0GPIOB *p10 = g_new(STM32L0GPIOB, 1);
	object_initialize_child(OBJECT(sms), "GPIOB", p10, TYPE_STM32L0_GPIOB);
	sysbus_realize(SYS_BUS_DEVICE(p10), &error_fatal);
	p10->base = 0x50000400;
	sysbus_mmio_map(SYS_BUS_DEVICE(p10), 0, 0x50000400);

	STM32L0GPIOB *p11 = g_new(STM32L0GPIOB, 1);
	object_initialize_child(OBJECT(sms), "GPIOC", p11, TYPE_STM32L0_GPIOB);
	sysbus_realize(SYS_BUS_DEVICE(p11), &error_fatal);
	p11->base = 0x50000800;
	sysbus_mmio_map(SYS_BUS_DEVICE(p11), 0, 0x50000800);

	STM32L0GPIOB *p12 = g_new(STM32L0GPIOB, 1);
	object_initialize_child(OBJECT(sms), "GPIOD", p12, TYPE_STM32L0_GPIOB);
	sysbus_realize(SYS_BUS_DEVICE(p12), &error_fatal);
	p12->base = 0x50000c00;
	sysbus_mmio_map(SYS_BUS_DEVICE(p12), 0, 0x50000c00);

	STM32L0GPIOB *p13 = g_new(STM32L0GPIOB, 1);
	object_initialize_child(OBJECT(sms), "GPIOE", p13, TYPE_STM32L0_GPIOB);
	sysbus_realize(SYS_BUS_DEVICE(p13), &error_fatal);
	p13->base = 0x50001000;
	sysbus_mmio_map(SYS_BUS_DEVICE(p13), 0, 0x50001000);

	STM32L0GPIOB *p14 = g_new(STM32L0GPIOB, 1);
	object_initialize_child(OBJECT(sms), "GPIOH", p14, TYPE_STM32L0_GPIOB);
	sysbus_realize(SYS_BUS_DEVICE(p14), &error_fatal);
	p14->base = 0x50001c00;
	sysbus_mmio_map(SYS_BUS_DEVICE(p14), 0, 0x50001c00);

	STM32L0SPI *p15 = g_new(STM32L0SPI, 1);
	object_initialize_child(OBJECT(sms), "SPI1", p15, TYPE_STM32L0_SPI);
	qdev_prop_set_chr(DEVICE(p15), "chardev", qemu_chr_new("soc-spi1", "chardev:spi1", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p15), &error_fatal);
	p15->base = 0x40013000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p15), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 25));
	sysbus_mmio_map(SYS_BUS_DEVICE(p15), 0, 0x40013000);

	STM32L0SPI *p16 = g_new(STM32L0SPI, 1);
	object_initialize_child(OBJECT(sms), "SPI2", p16, TYPE_STM32L0_SPI);
	qdev_prop_set_chr(DEVICE(p16), "chardev", qemu_chr_new("soc-spi2", "chardev:spi2", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p16), &error_fatal);
	p16->base = 0x40003800;
	sysbus_connect_irq(SYS_BUS_DEVICE(p16), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 26));
	sysbus_mmio_map(SYS_BUS_DEVICE(p16), 0, 0x40003800);

	STM32L0RTC *p17 = g_new(STM32L0RTC, 1);
	object_initialize_child(OBJECT(sms), "RTC", p17, TYPE_STM32L0_RTC);
	sysbus_realize(SYS_BUS_DEVICE(p17), &error_fatal);
	p17->base = 0x40002800;
	sysbus_connect_irq(SYS_BUS_DEVICE(p17), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 2));
	sysbus_mmio_map(SYS_BUS_DEVICE(p17), 0, 0x40002800);

	STM32L0EXTI *p18 = g_new(STM32L0EXTI, 1);
	object_initialize_child(OBJECT(sms), "EXTI", p18, TYPE_STM32L0_EXTI);
	qdev_prop_set_chr(DEVICE(p18), "chardev", qemu_chr_new("soc-exti", "chardev:exti", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p18), &error_fatal);
	p18->base = 0x40010400;
	sysbus_connect_irq(SYS_BUS_DEVICE(p18), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 1));
	sysbus_connect_irq(SYS_BUS_DEVICE(p18), 1, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 5));
	sysbus_connect_irq(SYS_BUS_DEVICE(p18), 2, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 6));
	sysbus_connect_irq(SYS_BUS_DEVICE(p18), 3, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 7));
	sysbus_mmio_map(SYS_BUS_DEVICE(p18), 0, 0x40010400);

	STM32L0I2C *p19 = g_new(STM32L0I2C, 1);
	object_initialize_child(OBJECT(sms), "I2C1", p19, TYPE_STM32L0_I2C);
	qdev_prop_set_chr(DEVICE(p19), "chardev", qemu_chr_new("soc-i2c1", "chardev:i2c1", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p19), &error_fatal);
	p19->base = 0x40005400;
	sysbus_connect_irq(SYS_BUS_DEVICE(p19), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 23));
	sysbus_mmio_map(SYS_BUS_DEVICE(p19), 0, 0x40005400);

	STM32L0I2C *p20 = g_new(STM32L0I2C, 1);
	object_initialize_child(OBJECT(sms), "I2C2", p20, TYPE_STM32L0_I2C);
	qdev_prop_set_chr(DEVICE(p20), "chardev", qemu_chr_new("soc-i2c2", "chardev:i2c2", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p20), &error_fatal);
	p20->base = 0x40005800;
	sysbus_connect_irq(SYS_BUS_DEVICE(p20), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 24));
	sysbus_mmio_map(SYS_BUS_DEVICE(p20), 0, 0x40005800);

	STM32L0LPTIM *p21 = g_new(STM32L0LPTIM, 1);
	object_initialize_child(OBJECT(sms), "LPTIM", p21, TYPE_STM32L0_LPTIM);
	sysbus_realize(SYS_BUS_DEVICE(p21), &error_fatal);
	p21->base = 0x40007c00;
	sysbus_connect_irq(SYS_BUS_DEVICE(p21), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 13));
	sysbus_mmio_map(SYS_BUS_DEVICE(p21), 0, 0x40007c00);

	STM32L0DBGMCU *p22 = g_new(STM32L0DBGMCU, 1);
	object_initialize_child(OBJECT(sms), "DBGMCU", p22, TYPE_STM32L0_DBGMCU);
	sysbus_realize(SYS_BUS_DEVICE(p22), &error_fatal);
	p22->base = 0x40015800;
	sysbus_mmio_map(SYS_BUS_DEVICE(p22), 0, 0x40015800);

	STM32L0CRC *p23 = g_new(STM32L0CRC, 1);
	object_initialize_child(OBJECT(sms), "CRC", p23, TYPE_STM32L0_CRC);
	sysbus_realize(SYS_BUS_DEVICE(p23), &error_fatal);
	p23->base = 0x40023000;
	sysbus_mmio_map(SYS_BUS_DEVICE(p23), 0, 0x40023000);

	STM32L0IWDG *p24 = g_new(STM32L0IWDG, 1);
	object_initialize_child(OBJECT(sms), "IWDG", p24, TYPE_STM32L0_IWDG);
	sysbus_realize(SYS_BUS_DEVICE(p24), &error_fatal);
	p24->base = 0x40003000;
	sysbus_mmio_map(SYS_BUS_DEVICE(p24), 0, 0x40003000);


}

static void stm32l073_common_init(MachineState *machine) {
	STM32L073MachineState *sms = STM32L073_MACHINE(machine);
	MemoryRegion *sysmem = get_system_memory();
	MemoryRegion *mem;
	Clock *cpuclk = clock_new(OBJECT(sms), "SYSCLK");
	clock_set_hz(cpuclk, 32000000);
	Clock *refclk = clock_new(OBJECT(sms), "REFCLK");
	clock_set_hz(refclk, 32000000);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_ram(mem, NULL, "wtf", 0x5000, &error_fatal);
	memory_region_add_subregion(sysmem, 0x0, mem);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_rom(mem, NULL, "main-flash", 0x30000, &error_fatal);
	memory_region_add_subregion(sysmem, 0x8000000, mem);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_rom(mem, NULL, "eeprom", 0x1800, &error_fatal);
	memory_region_add_subregion(sysmem, 0x8080000, mem);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_rom(mem, NULL, "system-memory", 0x2000, &error_fatal);
	memory_region_add_subregion(sysmem, 0x1ff00000, mem);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_rom(mem, NULL, "option-bytes", 0x80, &error_fatal);
	memory_region_add_subregion(sysmem, 0x1ff80000, mem);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_ram(mem, NULL, "sram", 0x20000, &error_fatal);
	memory_region_add_subregion(sysmem, 0x20000000, mem);


	object_initialize_child(OBJECT(sms), "armv7m", &(sms->armv7m), TYPE_ARMV7M);
	qdev_prop_set_uint32(DEVICE(&(sms->armv7m)),"num-irq", 47);
	qdev_prop_set_string(DEVICE(&(sms->armv7m)), "cpu-type", machine->cpu_type);
	qdev_prop_set_bit(DEVICE(&(sms->armv7m)), "enable-bitband", false);
	qdev_connect_clock_in(DEVICE(&(sms->armv7m)), "cpuclk", cpuclk);
	qdev_connect_clock_in(DEVICE(&(sms->armv7m)), "refclk", refclk);
	qdev_prop_set_uint32(DEVICE(&(sms->armv7m)), "init-nsvtor", 0x8000000);
	object_property_set_link(OBJECT(&sms->armv7m), "memory", OBJECT(sysmem), &error_abort);
	sysbus_realize(SYS_BUS_DEVICE(&sms->armv7m), &error_fatal);

	stm32l073_periph_init(machine);

	armv7m_load_kernel(ARM_CPU(first_cpu), machine->kernel_filename, 0x8000000, 0x30000);
}

static void stm32l073_class_init(ObjectClass *oc, void *data) {
	MachineClass *mc = MACHINE_CLASS(oc);
	mc->desc = "stm32l073";
	mc->init = stm32l073_common_init;
	mc->max_cpus = 1;
	mc->default_cpus = 1;
	mc->default_cpu_type = ARM_CPU_TYPE_NAME("cortex-m0");
}

static const TypeInfo stm32l073_info = {
	.name = TYPE_STM32L073_MACHINE,
	.parent = TYPE_MACHINE,
	.instance_size = sizeof(STM32L073MachineState),
	.class_size = sizeof(STM32L073MachineClass),
	.class_init = stm32l073_class_init
};

static void stm32l073_machine_init(void) {
	type_register_static(&stm32l073_info);
}

type_init(stm32l073_machine_init);
