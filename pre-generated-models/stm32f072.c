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


#define TYPE_STM32F0_DMA "stm32f0-dma"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F0DMA, STM32F0_DMA)

struct STM32F0DMA {
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
	/* dma channel enable flags*/
	uint8_t channel_enabled[7];
	/* base */
	uint32_t base;

};
#define STM32F0_DMA_SIZE				0x90

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
	FIELD(DMA_IFCR, CTCIF1, 1, 1)
	FIELD(DMA_IFCR, CHTIF1, 2, 1)
	FIELD(DMA_IFCR, CTEIF1, 3, 1)
	FIELD(DMA_IFCR, CGIF2, 4, 1)
	FIELD(DMA_IFCR, CTCIF2, 5, 1)
	FIELD(DMA_IFCR, CHTIF2, 6, 1)
	FIELD(DMA_IFCR, CTEIF2, 7, 1)
	FIELD(DMA_IFCR, CGIF3, 8, 1)
	FIELD(DMA_IFCR, CTCIF3, 9, 1)
	FIELD(DMA_IFCR, CHTIF3, 10, 1)
	FIELD(DMA_IFCR, CTEIF3, 11, 1)
	FIELD(DMA_IFCR, CGIF4, 12, 1)
	FIELD(DMA_IFCR, CTCIF4, 13, 1)
	FIELD(DMA_IFCR, CHTIF4, 14, 1)
	FIELD(DMA_IFCR, CTEIF4, 15, 1)
	FIELD(DMA_IFCR, CGIF5, 16, 1)
	FIELD(DMA_IFCR, CTCIF5, 17, 1)
	FIELD(DMA_IFCR, CHTIF5, 18, 1)
	FIELD(DMA_IFCR, CTEIF5, 19, 1)
	FIELD(DMA_IFCR, CGIF6, 20, 1)
	FIELD(DMA_IFCR, CTCIF6, 21, 1)
	FIELD(DMA_IFCR, CHTIF6, 22, 1)
	FIELD(DMA_IFCR, CTEIF6, 23, 1)
	FIELD(DMA_IFCR, CGIF7, 24, 1)
	FIELD(DMA_IFCR, CTCIF7, 25, 1)
	FIELD(DMA_IFCR, CHTIF7, 26, 1)
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
static void stm32f0_dma_update(STM32F0DMA *t, int channel_idx, int level) {
	qemu_set_irq(t->irq[channel_idx], level);
}

static void stm32f0_dma_receive(struct STM32F0DMA *t, uint32_t addr, uint8_t data) {
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
				t->CCR1 |= 0x2;
				t->ISR |= 0x2;
				t->CCR1 &= (~(0x4));

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
				t->CCR2 &= (~(0x4));
				t->ISR &= (~(0x40));
				t->ISR |= 0x20;
				t->CCR2 |= 0x2;

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
				t->ISR |= 0x200;
				t->ISR &= (~(0x400));
				t->CCR3 &= (~(0x4));
				t->CCR3 |= 0x2;

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
				t->CCR4 &= (~(0x4));
				t->ISR |= 0x2000;
				t->ISR &= (~(0x4000));
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
				t->CCR5 &= (~(0x4));
				t->ISR &= (~(0x40000));
				t->CCR5 |= 0x2;
				t->ISR |= 0x20000;

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
				t->CCR6 |= 0x2;
				t->ISR |= 0x200000;
				t->CCR6 &= (~(0x4));
				t->ISR &= (~(0x400000));

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
				t->ISR &= (~(0x4000000));
				t->ISR |= 0x2000000;
				t->CCR7 |= 0x2;
				t->CCR7 &= (~(0x4));

				do_update = 1;
			}
			break;
		}

		default: break;
	}
	if (do_update) {
		stm32f0_dma_update(t, channel_idx, 1);
		stm32f0_dma_update(t, channel_idx, 0);
	}
}

static void stm32f0_dma_transmit(struct STM32F0DMA *t, int channel_idx) {
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
			t->CCR1 |= 0x2;
			t->ISR |= 0x2;
			t->CCR1 &= (~(0x4));

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
			t->CCR2 &= (~(0x4));
			t->ISR &= (~(0x40));
			t->ISR |= 0x20;
			t->CCR2 |= 0x2;

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
			t->ISR |= 0x200;
			t->ISR &= (~(0x400));
			t->CCR3 &= (~(0x4));
			t->CCR3 |= 0x2;

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
			t->CCR4 &= (~(0x4));
			t->ISR |= 0x2000;
			t->ISR &= (~(0x4000));
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
			t->CCR5 &= (~(0x4));
			t->ISR &= (~(0x40000));
			t->CCR5 |= 0x2;
			t->ISR |= 0x20000;

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
			t->CCR6 |= 0x2;
			t->ISR |= 0x200000;
			t->CCR6 &= (~(0x4));
			t->ISR &= (~(0x400000));

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
			t->ISR &= (~(0x4000000));
			t->ISR |= 0x2000000;
			t->CCR7 |= 0x2;
			t->CCR7 &= (~(0x4));

			break;
		}

		default: break;
	}
	stm32f0_dma_update(t, channel_idx, 1);
	stm32f0_dma_update(t, channel_idx, 0);
}

static void stm32f0_dma_register_reset(STM32F0DMA *t) {
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

static uint64_t stm32f0_dma_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F0DMA *t = STM32F0_DMA(opaque);
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
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F0 DMA read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f0_dma_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F0DMA *t = STM32F0_DMA(opaque);


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
					stm32f0_dma_transmit(t, 0);
				} else {
					if ((((!(t->CCR1 & 0x800)) && (!(t->CCR1 & 0x10)) && (!(t->CCR1 & 0x100)) && (t->CCR1 & 0x4000)) || ((!(t->CCR1 & 0x200)) && (!(t->CCR1 & 0x10)) && (!(t->CCR1 & 0x400)) && (t->CCR1 & 0x4000)) || ((!(t->CCR1 & 0x10)) && (!(t->CCR1 & 0x100)) && (!(t->CCR1 & 0x400)) && (t->CCR1 & 0x4000)) || ((!(t->CCR1 & 0x800)) && (!(t->CCR1 & 0x200)) && (!(t->CCR1 & 0x10)) && (t->CCR1 & 0x4000)))) {
						if (t->CPAR1 < 0x40000000) {
							stm32f0_dma_transmit(t, 0);
						}
					} else {
						if (t->CMAR1 < 0x40000000) {
							stm32f0_dma_transmit(t, 0);
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
					stm32f0_dma_transmit(t, 1);
				} else {
					if ((((!(t->CCR2 & 0x200)) && (!(t->CCR2 & 0x10)) && (t->CCR2 & 0x4000) && (!(t->CCR2 & 0x800))) || ((!(t->CCR2 & 0x200)) && (!(t->CCR2 & 0x400)) && (!(t->CCR2 & 0x10)) && (t->CCR2 & 0x4000)) || ((!(t->CCR2 & 0x10)) && (!(t->CCR2 & 0x100)) && (t->CCR2 & 0x4000) && (!(t->CCR2 & 0x800))) || ((!(t->CCR2 & 0x400)) && (!(t->CCR2 & 0x10)) && (!(t->CCR2 & 0x100)) && (t->CCR2 & 0x4000)))) {
						if (t->CPAR2 < 0x40000000) {
							stm32f0_dma_transmit(t, 1);
						}
					} else {
						if (t->CMAR2 < 0x40000000) {
							stm32f0_dma_transmit(t, 1);
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
					stm32f0_dma_transmit(t, 2);
				} else {
					if ((((t->CCR3 & 0x4000) && (!(t->CCR3 & 0x200)) && (!(t->CCR3 & 0x800)) && (!(t->CCR3 & 0x10))) || ((!(t->CCR3 & 0x400)) && (!(t->CCR3 & 0x10)) && (!(t->CCR3 & 0x200)) && (t->CCR3 & 0x4000)) || ((!(t->CCR3 & 0x100)) && (t->CCR3 & 0x4000) && (!(t->CCR3 & 0x800)) && (!(t->CCR3 & 0x10))) || ((!(t->CCR3 & 0x100)) && (t->CCR3 & 0x4000) && (!(t->CCR3 & 0x400)) && (!(t->CCR3 & 0x10))))) {
						if (t->CPAR3 < 0x40000000) {
							stm32f0_dma_transmit(t, 2);
						}
					} else {
						if (t->CMAR3 < 0x40000000) {
							stm32f0_dma_transmit(t, 2);
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
					stm32f0_dma_transmit(t, 3);
				} else {
					if ((((!(t->CCR4 & 0x10)) && (t->CCR4 & 0x4000) && (!(t->CCR4 & 0x100)) && (!(t->CCR4 & 0x800))) || ((!(t->CCR4 & 0x10)) && (t->CCR4 & 0x4000) && (!(t->CCR4 & 0x400)) && (!(t->CCR4 & 0x100))) || ((!(t->CCR4 & 0x200)) && (!(t->CCR4 & 0x10)) && (t->CCR4 & 0x4000) && (!(t->CCR4 & 0x400))) || ((!(t->CCR4 & 0x200)) && (!(t->CCR4 & 0x10)) && (t->CCR4 & 0x4000) && (!(t->CCR4 & 0x800))))) {
						if (t->CPAR4 < 0x40000000) {
							stm32f0_dma_transmit(t, 3);
						}
					} else {
						if (t->CMAR4 < 0x40000000) {
							stm32f0_dma_transmit(t, 3);
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
					stm32f0_dma_transmit(t, 4);
				} else {
					if ((((!(t->CCR5 & 0x200)) && (t->CCR5 & 0x4000) && (!(t->CCR5 & 0x800)) && (!(t->CCR5 & 0x10))) || ((!(t->CCR5 & 0x200)) && (!(t->CCR5 & 0x400)) && (t->CCR5 & 0x4000) && (!(t->CCR5 & 0x10))) || ((t->CCR5 & 0x4000) && (!(t->CCR5 & 0x100)) && (!(t->CCR5 & 0x800)) && (!(t->CCR5 & 0x10))) || ((!(t->CCR5 & 0x400)) && (t->CCR5 & 0x4000) && (!(t->CCR5 & 0x100)) && (!(t->CCR5 & 0x10))))) {
						if (t->CPAR5 < 0x40000000) {
							stm32f0_dma_transmit(t, 4);
						}
					} else {
						if (t->CMAR5 < 0x40000000) {
							stm32f0_dma_transmit(t, 4);
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
					stm32f0_dma_transmit(t, 5);
				} else {
					if ((((!(t->CCR6 & 0x10)) && (!(t->CCR6 & 0x200)) && (t->CCR6 & 0x4000) && (!(t->CCR6 & 0x400))) || ((!(t->CCR6 & 0x10)) && (!(t->CCR6 & 0x200)) && (!(t->CCR6 & 0x800)) && (t->CCR6 & 0x4000)) || ((!(t->CCR6 & 0x10)) && (t->CCR6 & 0x4000) && (!(t->CCR6 & 0x400)) && (!(t->CCR6 & 0x100))) || ((!(t->CCR6 & 0x10)) && (!(t->CCR6 & 0x800)) && (t->CCR6 & 0x4000) && (!(t->CCR6 & 0x100))))) {
						if (t->CPAR6 < 0x40000000) {
							stm32f0_dma_transmit(t, 5);
						}
					} else {
						if (t->CMAR6 < 0x40000000) {
							stm32f0_dma_transmit(t, 5);
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
					stm32f0_dma_transmit(t, 6);
				} else {
					if ((((!(t->CCR7 & 0x200)) && (!(t->CCR7 & 0x400)) && (!(t->CCR7 & 0x10)) && (t->CCR7 & 0x4000)) || ((!(t->CCR7 & 0x800)) && (!(t->CCR7 & 0x200)) && (!(t->CCR7 & 0x10)) && (t->CCR7 & 0x4000)) || ((!(t->CCR7 & 0x400)) && (!(t->CCR7 & 0x10)) && (t->CCR7 & 0x4000) && (!(t->CCR7 & 0x100))) || ((!(t->CCR7 & 0x800)) && (!(t->CCR7 & 0x10)) && (t->CCR7 & 0x4000) && (!(t->CCR7 & 0x100))))) {
						if (t->CPAR7 < 0x40000000) {
							stm32f0_dma_transmit(t, 6);
						}
					} else {
						if (t->CMAR7 < 0x40000000) {
							stm32f0_dma_transmit(t, 6);
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
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F0 DMA write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f0_dma_ops = {
	.read = stm32f0_dma_read,
	.write = stm32f0_dma_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f0_dma_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F0DMA *t = STM32F0_DMA(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f0_dma_ops, t, "stm32f0-dma", STM32F0_DMA_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 3; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32f0_dma_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32f0_dma_reset_enter(Object *obj, ResetType type) {
	STM32F0DMA *t = STM32F0_DMA(obj);
	stm32f0_dma_register_reset(t);
}

static const VMStateDescription stm32f0_dma_vmstate = {
	.name = "stm32f0-dma",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(ISR, STM32F0DMA),
		VMSTATE_UINT32(IFCR, STM32F0DMA),
		VMSTATE_UINT32(CCR1, STM32F0DMA),
		VMSTATE_UINT32(CNDTR1, STM32F0DMA),
		VMSTATE_UINT32(CPAR1, STM32F0DMA),
		VMSTATE_UINT32(CMAR1, STM32F0DMA),
		VMSTATE_UINT32(CCR2, STM32F0DMA),
		VMSTATE_UINT32(CNDTR2, STM32F0DMA),
		VMSTATE_UINT32(CPAR2, STM32F0DMA),
		VMSTATE_UINT32(CMAR2, STM32F0DMA),
		VMSTATE_UINT32(CCR3, STM32F0DMA),
		VMSTATE_UINT32(CNDTR3, STM32F0DMA),
		VMSTATE_UINT32(CPAR3, STM32F0DMA),
		VMSTATE_UINT32(CMAR3, STM32F0DMA),
		VMSTATE_UINT32(CCR4, STM32F0DMA),
		VMSTATE_UINT32(CNDTR4, STM32F0DMA),
		VMSTATE_UINT32(CPAR4, STM32F0DMA),
		VMSTATE_UINT32(CMAR4, STM32F0DMA),
		VMSTATE_UINT32(CCR5, STM32F0DMA),
		VMSTATE_UINT32(CNDTR5, STM32F0DMA),
		VMSTATE_UINT32(CPAR5, STM32F0DMA),
		VMSTATE_UINT32(CMAR5, STM32F0DMA),
		VMSTATE_UINT32(CCR6, STM32F0DMA),
		VMSTATE_UINT32(CNDTR6, STM32F0DMA),
		VMSTATE_UINT32(CPAR6, STM32F0DMA),
		VMSTATE_UINT32(CMAR6, STM32F0DMA),
		VMSTATE_UINT32(CCR7, STM32F0DMA),
		VMSTATE_UINT32(CNDTR7, STM32F0DMA),
		VMSTATE_UINT32(CPAR7, STM32F0DMA),
		VMSTATE_UINT32(CMAR7, STM32F0DMA),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f0_dma_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f0_dma_vmstate;
	dc->realize = stm32f0_dma_realize;
	rc->phases.enter = stm32f0_dma_reset_enter;

}

static const TypeInfo stm32f0_dma_info = {
	.name = TYPE_STM32F0_DMA,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F0DMA),
	.instance_init = stm32f0_dma_init,
	.class_init = stm32f0_dma_class_init,
};

static void stm32f0_dma_register_types(void) {
	type_register_static(&stm32f0_dma_info);
}

type_init(stm32f0_dma_register_types);

#define TYPE_STM32F0_PWR "stm32f0-pwr"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F0PWR, STM32F0_PWR)

struct STM32F0PWR {
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
#define STM32F0_PWR_SIZE				0x8

REG32(PWR_CR, 0x0)
	FIELD(PWR_CR, FPDS, 9, 1)
	FIELD(PWR_CR, DBP, 8, 1)
	FIELD(PWR_CR, PLS, 5, 3)
	FIELD(PWR_CR, PVDE, 4, 1)
	FIELD(PWR_CR, CSBF, 3, 1)
	FIELD(PWR_CR, CWUF, 2, 1)
	FIELD(PWR_CR, PDDS, 1, 1)
	FIELD(PWR_CR, LPDS, 0, 1)
REG32(PWR_CSR, 0x4)
	FIELD(PWR_CSR, BRE, 9, 1)
	FIELD(PWR_CSR, EWUP, 8, 1)
	FIELD(PWR_CSR, BRR, 3, 1)
	FIELD(PWR_CSR, PVDO, 2, 1)
	FIELD(PWR_CSR, SBF, 1, 1)
	FIELD(PWR_CSR, WUF, 0, 1)


static void stm32f0_pwr_register_reset(STM32F0PWR *t) {
	t->CR = 0x0;
	t->CSR = 0x0;

}

static uint64_t stm32f0_pwr_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F0PWR *t = STM32F0_PWR(opaque);
	uint64_t ret;

	switch (offset) {
		case A_PWR_CR:
			ret = t->CR;
			break;
		case A_PWR_CSR:
			ret = t->CSR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F0 PWR read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f0_pwr_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F0PWR *t = STM32F0_PWR(opaque);


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
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F0 PWR write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f0_pwr_ops = {
	.read = stm32f0_pwr_read,
	.write = stm32f0_pwr_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f0_pwr_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F0PWR *t = STM32F0_PWR(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f0_pwr_ops, t, "stm32f0-pwr", STM32F0_PWR_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void stm32f0_pwr_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32f0_pwr_reset_enter(Object *obj, ResetType type) {
	STM32F0PWR *t = STM32F0_PWR(obj);
	stm32f0_pwr_register_reset(t);
}

static const VMStateDescription stm32f0_pwr_vmstate = {
	.name = "stm32f0-pwr",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR, STM32F0PWR),
		VMSTATE_UINT32(CSR, STM32F0PWR),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f0_pwr_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f0_pwr_vmstate;
	dc->realize = stm32f0_pwr_realize;
	rc->phases.enter = stm32f0_pwr_reset_enter;

}

static const TypeInfo stm32f0_pwr_info = {
	.name = TYPE_STM32F0_PWR,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F0PWR),
	.instance_init = stm32f0_pwr_init,
	.class_init = stm32f0_pwr_class_init,
};

static void stm32f0_pwr_register_types(void) {
	type_register_static(&stm32f0_pwr_info);
}

type_init(stm32f0_pwr_register_types);

#define TYPE_STM32F0_FLASH "stm32f0-flash"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F0FLASH, STM32F0_FLASH)

struct STM32F0FLASH {
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
#define STM32F0_FLASH_SIZE				0x24

REG32(FLASH_ACR, 0x0)
	FIELD(FLASH_ACR, LATENCY, 0, 3)
	FIELD(FLASH_ACR, PRFTBE, 4, 1)
	FIELD(FLASH_ACR, PRFTBS, 5, 1)
REG32(FLASH_KEYR, 0x4)
	FIELD(FLASH_KEYR, FKEYR, 0, 32)
REG32(FLASH_OPTKEYR, 0x8)
	FIELD(FLASH_OPTKEYR, OPTKEYR, 0, 32)
REG32(FLASH_SR, 0xc)
	FIELD(FLASH_SR, EOP, 5, 1)
	FIELD(FLASH_SR, WRPRT, 4, 1)
	FIELD(FLASH_SR, PGERR, 2, 1)
	FIELD(FLASH_SR, BSY, 0, 1)
REG32(FLASH_CR, 0x10)
	FIELD(FLASH_CR, FORCE_OPTLOAD, 13, 1)
	FIELD(FLASH_CR, EOPIE, 12, 1)
	FIELD(FLASH_CR, ERRIE, 10, 1)
	FIELD(FLASH_CR, OPTWRE, 9, 1)
	FIELD(FLASH_CR, LOCK, 7, 1)
	FIELD(FLASH_CR, STRT, 6, 1)
	FIELD(FLASH_CR, OPTER, 5, 1)
	FIELD(FLASH_CR, OPTPG, 4, 1)
	FIELD(FLASH_CR, MER, 2, 1)
	FIELD(FLASH_CR, PER, 1, 1)
	FIELD(FLASH_CR, PG, 0, 1)
REG32(FLASH_AR, 0x14)
	FIELD(FLASH_AR, FAR, 0, 32)
REG32(FLASH_OBR, 0x1c)
	FIELD(FLASH_OBR, Data1, 24, 8)
	FIELD(FLASH_OBR, Data0, 16, 8)
	FIELD(FLASH_OBR, VDDA_MONITOR, 13, 1)
	FIELD(FLASH_OBR, BOOT1, 12, 1)
	FIELD(FLASH_OBR, nRST_STDBY, 10, 1)
	FIELD(FLASH_OBR, nRST_STOP, 9, 1)
	FIELD(FLASH_OBR, WDG_SW, 8, 1)
	FIELD(FLASH_OBR, LEVEL2_PROT, 2, 1)
	FIELD(FLASH_OBR, LEVEL1_PROT, 1, 1)
	FIELD(FLASH_OBR, OPTERR, 0, 1)
REG32(FLASH_WRPR, 0x20)
	FIELD(FLASH_WRPR, WRP, 0, 32)


static void stm32f0_flash_register_reset(STM32F0FLASH *t) {
	t->ACR = 0x30;
	t->KEYR = 0x0;
	t->OPTKEYR = 0x0;
	t->SR = 0x0;
	t->CR = 0x80;
	t->AR = 0x0;
	t->OBR = 0x3fffff2;
	t->WRPR = 0xffffffff;

}

static uint64_t stm32f0_flash_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F0FLASH *t = STM32F0_FLASH(opaque);
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
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F0 FLASH read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f0_flash_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F0FLASH *t = STM32F0_FLASH(opaque);


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
			break;
		case A_FLASH_AR:
			t->AR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F0 FLASH write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f0_flash_ops = {
	.read = stm32f0_flash_read,
	.write = stm32f0_flash_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f0_flash_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F0FLASH *t = STM32F0_FLASH(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f0_flash_ops, t, "stm32f0-flash", STM32F0_FLASH_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32f0_flash_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32f0_flash_reset_enter(Object *obj, ResetType type) {
	STM32F0FLASH *t = STM32F0_FLASH(obj);
	stm32f0_flash_register_reset(t);
}

static const VMStateDescription stm32f0_flash_vmstate = {
	.name = "stm32f0-flash",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(ACR, STM32F0FLASH),
		VMSTATE_UINT32(KEYR, STM32F0FLASH),
		VMSTATE_UINT32(OPTKEYR, STM32F0FLASH),
		VMSTATE_UINT32(SR, STM32F0FLASH),
		VMSTATE_UINT32(CR, STM32F0FLASH),
		VMSTATE_UINT32(AR, STM32F0FLASH),
		VMSTATE_UINT32(OBR, STM32F0FLASH),
		VMSTATE_UINT32(WRPR, STM32F0FLASH),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f0_flash_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f0_flash_vmstate;
	dc->realize = stm32f0_flash_realize;
	rc->phases.enter = stm32f0_flash_reset_enter;

}

static const TypeInfo stm32f0_flash_info = {
	.name = TYPE_STM32F0_FLASH,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F0FLASH),
	.instance_init = stm32f0_flash_init,
	.class_init = stm32f0_flash_class_init,
};

static void stm32f0_flash_register_types(void) {
	type_register_static(&stm32f0_flash_info);
}

type_init(stm32f0_flash_register_types);

#define TYPE_STM32F0_RCC "stm32f0-rcc"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F0RCC, STM32F0_RCC)

struct STM32F0RCC {
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
	uint32_t AHBRSTR;
	uint32_t CFGR2;
	uint32_t CFGR3;
	uint32_t CR2;
	/* base */
	uint32_t base;

};
#define STM32F0_RCC_SIZE				0x38

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
	FIELD(RCC_CFGR, PPRE, 8, 3)
	FIELD(RCC_CFGR, ADCPRE, 14, 1)
	FIELD(RCC_CFGR, PLLSRC, 15, 2)
	FIELD(RCC_CFGR, PLLXTPRE, 17, 1)
	FIELD(RCC_CFGR, PLLMUL, 18, 4)
	FIELD(RCC_CFGR, MCO, 24, 3)
	FIELD(RCC_CFGR, MCOPRE, 28, 3)
	FIELD(RCC_CFGR, PLLNODIV, 31, 1)
REG32(RCC_CIR, 0x8)
	FIELD(RCC_CIR, LSIRDYF, 0, 1)
	FIELD(RCC_CIR, LSERDYF, 1, 1)
	FIELD(RCC_CIR, HSIRDYF, 2, 1)
	FIELD(RCC_CIR, HSERDYF, 3, 1)
	FIELD(RCC_CIR, PLLRDYF, 4, 1)
	FIELD(RCC_CIR, HSI14RDYF, 5, 1)
	FIELD(RCC_CIR, HSI48RDYF, 6, 1)
	FIELD(RCC_CIR, CSSF, 7, 1)
	FIELD(RCC_CIR, LSIRDYIE, 8, 1)
	FIELD(RCC_CIR, LSERDYIE, 9, 1)
	FIELD(RCC_CIR, HSIRDYIE, 10, 1)
	FIELD(RCC_CIR, HSERDYIE, 11, 1)
	FIELD(RCC_CIR, PLLRDYIE, 12, 1)
	FIELD(RCC_CIR, HSI14RDYE, 13, 1)
	FIELD(RCC_CIR, HSI48RDYIE, 14, 1)
	FIELD(RCC_CIR, LSIRDYC, 16, 1)
	FIELD(RCC_CIR, LSERDYC, 17, 1)
	FIELD(RCC_CIR, HSIRDYC, 18, 1)
	FIELD(RCC_CIR, HSERDYC, 19, 1)
	FIELD(RCC_CIR, PLLRDYC, 20, 1)
	FIELD(RCC_CIR, HSI14RDYC, 21, 1)
	FIELD(RCC_CIR, HSI48RDYC, 22, 1)
	FIELD(RCC_CIR, CSSC, 23, 1)
REG32(RCC_APB2RSTR, 0xc)
	FIELD(RCC_APB2RSTR, SYSCFGRST, 0, 1)
	FIELD(RCC_APB2RSTR, ADCRST, 9, 1)
	FIELD(RCC_APB2RSTR, TIM1RST, 11, 1)
	FIELD(RCC_APB2RSTR, SPI1RST, 12, 1)
	FIELD(RCC_APB2RSTR, USART1RST, 14, 1)
	FIELD(RCC_APB2RSTR, TIM15RST, 16, 1)
	FIELD(RCC_APB2RSTR, TIM16RST, 17, 1)
	FIELD(RCC_APB2RSTR, TIM17RST, 18, 1)
	FIELD(RCC_APB2RSTR, DBGMCURST, 22, 1)
REG32(RCC_APB1RSTR, 0x10)
	FIELD(RCC_APB1RSTR, TIM2RST, 0, 1)
	FIELD(RCC_APB1RSTR, TIM3RST, 1, 1)
	FIELD(RCC_APB1RSTR, TIM6RST, 4, 1)
	FIELD(RCC_APB1RSTR, TIM7RST, 5, 1)
	FIELD(RCC_APB1RSTR, TIM14RST, 8, 1)
	FIELD(RCC_APB1RSTR, WWDGRST, 11, 1)
	FIELD(RCC_APB1RSTR, SPI2RST, 14, 1)
	FIELD(RCC_APB1RSTR, USART2RST, 17, 1)
	FIELD(RCC_APB1RSTR, USART3RST, 18, 1)
	FIELD(RCC_APB1RSTR, USART4RST, 19, 1)
	FIELD(RCC_APB1RSTR, I2C1RST, 21, 1)
	FIELD(RCC_APB1RSTR, I2C2RST, 22, 1)
	FIELD(RCC_APB1RSTR, USBRST, 23, 1)
	FIELD(RCC_APB1RSTR, CANRST, 25, 1)
	FIELD(RCC_APB1RSTR, CRSRST, 27, 1)
	FIELD(RCC_APB1RSTR, PWRRST, 28, 1)
	FIELD(RCC_APB1RSTR, DACRST, 29, 1)
	FIELD(RCC_APB1RSTR, CECRST, 30, 1)
REG32(RCC_AHBENR, 0x14)
	FIELD(RCC_AHBENR, DMAEN, 0, 1)
	FIELD(RCC_AHBENR, SRAMEN, 2, 1)
	FIELD(RCC_AHBENR, FLITFEN, 4, 1)
	FIELD(RCC_AHBENR, CRCEN, 6, 1)
	FIELD(RCC_AHBENR, IOPAEN, 17, 1)
	FIELD(RCC_AHBENR, IOPBEN, 18, 1)
	FIELD(RCC_AHBENR, IOPCEN, 19, 1)
	FIELD(RCC_AHBENR, IOPDEN, 20, 1)
	FIELD(RCC_AHBENR, IOPFEN, 22, 1)
	FIELD(RCC_AHBENR, TSCEN, 24, 1)
REG32(RCC_APB2ENR, 0x18)
	FIELD(RCC_APB2ENR, SYSCFGEN, 0, 1)
	FIELD(RCC_APB2ENR, ADCEN, 9, 1)
	FIELD(RCC_APB2ENR, TIM1EN, 11, 1)
	FIELD(RCC_APB2ENR, SPI1EN, 12, 1)
	FIELD(RCC_APB2ENR, USART1EN, 14, 1)
	FIELD(RCC_APB2ENR, TIM15EN, 16, 1)
	FIELD(RCC_APB2ENR, TIM16EN, 17, 1)
	FIELD(RCC_APB2ENR, TIM17EN, 18, 1)
	FIELD(RCC_APB2ENR, DBGMCUEN, 22, 1)
REG32(RCC_APB1ENR, 0x1c)
	FIELD(RCC_APB1ENR, TIM2EN, 0, 1)
	FIELD(RCC_APB1ENR, TIM3EN, 1, 1)
	FIELD(RCC_APB1ENR, TIM6EN, 4, 1)
	FIELD(RCC_APB1ENR, TIM7EN, 5, 1)
	FIELD(RCC_APB1ENR, TIM14EN, 8, 1)
	FIELD(RCC_APB1ENR, WWDGEN, 11, 1)
	FIELD(RCC_APB1ENR, SPI2EN, 14, 1)
	FIELD(RCC_APB1ENR, USART2EN, 17, 1)
	FIELD(RCC_APB1ENR, USART3EN, 18, 1)
	FIELD(RCC_APB1ENR, USART4EN, 19, 1)
	FIELD(RCC_APB1ENR, I2C1EN, 21, 1)
	FIELD(RCC_APB1ENR, I2C2EN, 22, 1)
	FIELD(RCC_APB1ENR, USBRST, 23, 1)
	FIELD(RCC_APB1ENR, CANEN, 25, 1)
	FIELD(RCC_APB1ENR, CRSEN, 27, 1)
	FIELD(RCC_APB1ENR, PWREN, 28, 1)
	FIELD(RCC_APB1ENR, DACEN, 29, 1)
	FIELD(RCC_APB1ENR, CECEN, 30, 1)
REG32(RCC_BDCR, 0x20)
	FIELD(RCC_BDCR, LSEON, 0, 1)
	FIELD(RCC_BDCR, LSERDY, 1, 1)
	FIELD(RCC_BDCR, LSEBYP, 2, 1)
	FIELD(RCC_BDCR, LSEDRV, 3, 2)
	FIELD(RCC_BDCR, RTCSEL, 8, 2)
	FIELD(RCC_BDCR, RTCEN, 15, 1)
	FIELD(RCC_BDCR, BDRST, 16, 1)
REG32(RCC_CSR, 0x24)
	FIELD(RCC_CSR, LSION, 0, 1)
	FIELD(RCC_CSR, LSIRDY, 1, 1)
	FIELD(RCC_CSR, RMVF, 24, 1)
	FIELD(RCC_CSR, OBLRSTF, 25, 1)
	FIELD(RCC_CSR, PINRSTF, 26, 1)
	FIELD(RCC_CSR, PORRSTF, 27, 1)
	FIELD(RCC_CSR, SFTRSTF, 28, 1)
	FIELD(RCC_CSR, IWDGRSTF, 29, 1)
	FIELD(RCC_CSR, WWDGRSTF, 30, 1)
	FIELD(RCC_CSR, LPWRRSTF, 31, 1)
REG32(RCC_AHBRSTR, 0x28)
	FIELD(RCC_AHBRSTR, IOPARST, 17, 1)
	FIELD(RCC_AHBRSTR, IOPBRST, 18, 1)
	FIELD(RCC_AHBRSTR, IOPCRST, 19, 1)
	FIELD(RCC_AHBRSTR, IOPDRST, 20, 1)
	FIELD(RCC_AHBRSTR, IOPFRST, 22, 1)
	FIELD(RCC_AHBRSTR, TSCRST, 24, 1)
REG32(RCC_CFGR2, 0x2c)
	FIELD(RCC_CFGR2, PREDIV, 0, 4)
REG32(RCC_CFGR3, 0x30)
	FIELD(RCC_CFGR3, USART1SW, 0, 2)
	FIELD(RCC_CFGR3, I2C1SW, 4, 1)
	FIELD(RCC_CFGR3, CECSW, 6, 1)
	FIELD(RCC_CFGR3, USBSW, 7, 1)
	FIELD(RCC_CFGR3, ADCSW, 8, 1)
	FIELD(RCC_CFGR3, USART2SW, 16, 2)
REG32(RCC_CR2, 0x34)
	FIELD(RCC_CR2, HSI14ON, 0, 1)
	FIELD(RCC_CR2, HSI14RDY, 1, 1)
	FIELD(RCC_CR2, HSI14DIS, 2, 1)
	FIELD(RCC_CR2, HSI14TRIM, 3, 5)
	FIELD(RCC_CR2, HSI14CAL, 8, 8)
	FIELD(RCC_CR2, HSI48ON, 16, 1)
	FIELD(RCC_CR2, HSI48RDY, 17, 1)
	FIELD(RCC_CR2, HSI48CAL, 24, 1)


__attribute__((unused))
static void stm32f0_rcc_update(STM32F0RCC *t) {
	int conditions = (t->CIR & 0x80);
	qemu_set_irq(t->irq[0], conditions);

}

static void stm32f0_rcc_register_reset(STM32F0RCC *t) {
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
	t->AHBRSTR = 0x0;
	t->CFGR2 = 0x0;
	t->CFGR3 = 0x0;
	t->CR2 = 0x80;

}

static uint64_t stm32f0_rcc_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F0RCC *t = STM32F0_RCC(opaque);
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
		case A_RCC_AHBRSTR:
			ret = t->AHBRSTR;
			break;
		case A_RCC_CFGR2:
			ret = t->CFGR2;
			break;
		case A_RCC_CFGR3:
			ret = t->CFGR3;
			break;
		case A_RCC_CR2:
			ret = t->CR2;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F0 RCC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f0_rcc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F0RCC *t = STM32F0_RCC(opaque);


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
		case A_RCC_AHBRSTR ... A_RCC_AHBRSTR + 3:
			value = value << ((offset - A_RCC_AHBRSTR) << 3);
			offset = A_RCC_AHBRSTR;
			break;
		case A_RCC_CFGR2 ... A_RCC_CFGR2 + 3:
			value = value << ((offset - A_RCC_CFGR2) << 3);
			offset = A_RCC_CFGR2;
			break;
		case A_RCC_CFGR3 ... A_RCC_CFGR3 + 3:
			value = value << ((offset - A_RCC_CFGR3) << 3);
			offset = A_RCC_CFGR3;
			break;
		case A_RCC_CR2 ... A_RCC_CR2 + 3:
			value = value << ((offset - A_RCC_CR2) << 3);
			offset = A_RCC_CR2;
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
			if (((value & 0x1) && (value & 0x80))) {
				t->CR |= 0x2;
			} else if ((!(value & 0x1))) {
				t->CR &= (~(0x2));
			} else if ((value & 0x1)) {
				t->CR |= 0x2;
			}
			break;
		case A_RCC_CFGR:
			t->CFGR = value;
			if (((!(value & 0x1)) && (!(value & 0x2)) && (!(value & 0x10)) && (!(value & 0x20)) && (!(value & 0x40)) && (!(value & 0x80)) && (!(value & 0x100)) && (!(value & 0x200)) && (!(value & 0x400)) && (!(value & 0x1000000)) && (!(value & 0x2000000)) && (!(value & 0x4000000)) && (!(value & 0x8000000)))) {
				t->CFGR &= (~(0x4));
				t->CFGR &= (~(0x8));
			} else if (((!(value & 0x1)) && (value & 0x2))) {
				t->CFGR &= (~(0x4));
				t->CFGR |= 0x8;
			} else if (((value & 0x1) && (value & 0x2))) {
				t->CFGR |= 0x8;
				t->CFGR |= 0x4;
			} else if (((value & 0x1) && (!(value & 0x2)))) {
				t->CFGR &= (~(0x8));
				t->CFGR |= 0x4;
			} else if (((!(value & 0x1)) && (!(value & 0x2)))) {
				t->CFGR &= (~(0x4));
				t->CFGR &= (~(0x8));
			}
			break;
		case A_RCC_CIR:
			t->CIR = value;
			stm32f0_rcc_update(t);
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
		case A_RCC_AHBRSTR:
			t->AHBRSTR = value;
			break;
		case A_RCC_CFGR2:
			t->CFGR2 = value;
			break;
		case A_RCC_CFGR3:
			t->CFGR3 = value;
			break;
		case A_RCC_CR2:
			t->CR2 = value;
			if ((value & 0x1)) {
				t->CR2 |= 0x2;
			} else if ((!(value & 0x1))) {
				t->CR2 &= (~(0x2));
			}
			if ((!(value & 0x10000))) {
				t->CR2 &= (~(0x10000));
			} else if ((value & 0x10000)) {
				t->CR2 |= 0x10000;
			}
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F0 RCC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f0_rcc_ops = {
	.read = stm32f0_rcc_read,
	.write = stm32f0_rcc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f0_rcc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F0RCC *t = STM32F0_RCC(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f0_rcc_ops, t, "stm32f0-rcc", STM32F0_RCC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32f0_rcc_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32f0_rcc_reset_enter(Object *obj, ResetType type) {
	STM32F0RCC *t = STM32F0_RCC(obj);
	stm32f0_rcc_register_reset(t);
}

static const VMStateDescription stm32f0_rcc_vmstate = {
	.name = "stm32f0-rcc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR, STM32F0RCC),
		VMSTATE_UINT32(CFGR, STM32F0RCC),
		VMSTATE_UINT32(CIR, STM32F0RCC),
		VMSTATE_UINT32(APB2RSTR, STM32F0RCC),
		VMSTATE_UINT32(APB1RSTR, STM32F0RCC),
		VMSTATE_UINT32(AHBENR, STM32F0RCC),
		VMSTATE_UINT32(APB2ENR, STM32F0RCC),
		VMSTATE_UINT32(APB1ENR, STM32F0RCC),
		VMSTATE_UINT32(BDCR, STM32F0RCC),
		VMSTATE_UINT32(CSR, STM32F0RCC),
		VMSTATE_UINT32(AHBRSTR, STM32F0RCC),
		VMSTATE_UINT32(CFGR2, STM32F0RCC),
		VMSTATE_UINT32(CFGR3, STM32F0RCC),
		VMSTATE_UINT32(CR2, STM32F0RCC),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f0_rcc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f0_rcc_vmstate;
	dc->realize = stm32f0_rcc_realize;
	rc->phases.enter = stm32f0_rcc_reset_enter;

}

static const TypeInfo stm32f0_rcc_info = {
	.name = TYPE_STM32F0_RCC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F0RCC),
	.instance_init = stm32f0_rcc_init,
	.class_init = stm32f0_rcc_class_init,
};

static void stm32f0_rcc_register_types(void) {
	type_register_static(&stm32f0_rcc_info);
}

type_init(stm32f0_rcc_register_types);

#define TYPE_STM32F0_SYSCFG "stm32f0-syscfg"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F0SYSCFG, STM32F0_SYSCFG)

struct STM32F0SYSCFG {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/*registers*/
	uint32_t CFGR1;
	uint32_t EXTICR1;
	uint32_t EXTICR2;
	uint32_t EXTICR3;
	uint32_t EXTICR4;
	uint32_t CFGR2;
	/* base */
	uint32_t base;

};
#define STM32F0_SYSCFG_SIZE				0x1c

REG32(SYSCFG_CFGR1, 0x0)
	FIELD(SYSCFG_CFGR1, MEM_MODE, 0, 2)
	FIELD(SYSCFG_CFGR1, ADC_DMA_RMP, 8, 1)
	FIELD(SYSCFG_CFGR1, USART1_TX_DMA_RMP, 9, 1)
	FIELD(SYSCFG_CFGR1, USART1_RX_DMA_RMP, 10, 1)
	FIELD(SYSCFG_CFGR1, TIM16_DMA_RMP, 11, 1)
	FIELD(SYSCFG_CFGR1, TIM17_DMA_RMP, 12, 1)
	FIELD(SYSCFG_CFGR1, I2C_PB6_FM, 16, 1)
	FIELD(SYSCFG_CFGR1, I2C_PB7_FM, 17, 1)
	FIELD(SYSCFG_CFGR1, I2C_PB8_FM, 18, 1)
	FIELD(SYSCFG_CFGR1, I2C_PB9_FM, 19, 1)
	FIELD(SYSCFG_CFGR1, I2C1_FM_plus, 20, 1)
	FIELD(SYSCFG_CFGR1, I2C2_FM_plus, 21, 1)
	FIELD(SYSCFG_CFGR1, SPI2_DMA_RMP, 24, 1)
	FIELD(SYSCFG_CFGR1, USART2_DMA_RMP, 25, 1)
	FIELD(SYSCFG_CFGR1, USART3_DMA_RMP, 26, 1)
	FIELD(SYSCFG_CFGR1, I2C1_DMA_RMP, 27, 1)
	FIELD(SYSCFG_CFGR1, TIM1_DMA_RMP, 28, 1)
	FIELD(SYSCFG_CFGR1, TIM2_DMA_RMP, 29, 1)
	FIELD(SYSCFG_CFGR1, TIM3_DMA_RMP, 30, 1)
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
REG32(SYSCFG_CFGR2, 0x18)
	FIELD(SYSCFG_CFGR2, SRAM_PEF, 8, 1)
	FIELD(SYSCFG_CFGR2, PVD_LOCK, 2, 1)
	FIELD(SYSCFG_CFGR2, SRAM_PARITY_LOCK, 1, 1)
	FIELD(SYSCFG_CFGR2, LOCUP_LOCK, 0, 1)


static void stm32f0_syscfg_register_reset(STM32F0SYSCFG *t) {
	t->CFGR1 = 0x0;
	t->EXTICR1 = 0x0;
	t->EXTICR2 = 0x0;
	t->EXTICR3 = 0x0;
	t->EXTICR4 = 0x0;
	t->CFGR2 = 0x0;

}

static uint64_t stm32f0_syscfg_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F0SYSCFG *t = STM32F0_SYSCFG(opaque);
	uint64_t ret;

	switch (offset) {
		case A_SYSCFG_CFGR1:
			ret = t->CFGR1;
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
		case A_SYSCFG_CFGR2:
			ret = t->CFGR2;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F0 SYSCFG read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f0_syscfg_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F0SYSCFG *t = STM32F0_SYSCFG(opaque);


	switch (offset) {
		case A_SYSCFG_CFGR1 ... A_SYSCFG_CFGR1 + 3:
			value = value << ((offset - A_SYSCFG_CFGR1) << 3);
			offset = A_SYSCFG_CFGR1;
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
		case A_SYSCFG_CFGR2 ... A_SYSCFG_CFGR2 + 3:
			value = value << ((offset - A_SYSCFG_CFGR2) << 3);
			offset = A_SYSCFG_CFGR2;
			break;

		default: break;
	}

	switch (offset) {
		case A_SYSCFG_CFGR1:
			t->CFGR1 = value;
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
		case A_SYSCFG_CFGR2:
			t->CFGR2 = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F0 SYSCFG write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f0_syscfg_ops = {
	.read = stm32f0_syscfg_read,
	.write = stm32f0_syscfg_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f0_syscfg_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F0SYSCFG *t = STM32F0_SYSCFG(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f0_syscfg_ops, t, "stm32f0-syscfg", STM32F0_SYSCFG_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void stm32f0_syscfg_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32f0_syscfg_reset_enter(Object *obj, ResetType type) {
	STM32F0SYSCFG *t = STM32F0_SYSCFG(obj);
	stm32f0_syscfg_register_reset(t);
}

static const VMStateDescription stm32f0_syscfg_vmstate = {
	.name = "stm32f0-syscfg",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CFGR1, STM32F0SYSCFG),
		VMSTATE_UINT32(EXTICR1, STM32F0SYSCFG),
		VMSTATE_UINT32(EXTICR2, STM32F0SYSCFG),
		VMSTATE_UINT32(EXTICR3, STM32F0SYSCFG),
		VMSTATE_UINT32(EXTICR4, STM32F0SYSCFG),
		VMSTATE_UINT32(CFGR2, STM32F0SYSCFG),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f0_syscfg_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f0_syscfg_vmstate;
	dc->realize = stm32f0_syscfg_realize;
	rc->phases.enter = stm32f0_syscfg_reset_enter;

}

static const TypeInfo stm32f0_syscfg_info = {
	.name = TYPE_STM32F0_SYSCFG,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F0SYSCFG),
	.instance_init = stm32f0_syscfg_init,
	.class_init = stm32f0_syscfg_class_init,
};

static void stm32f0_syscfg_register_types(void) {
	type_register_static(&stm32f0_syscfg_info);
}

type_init(stm32f0_syscfg_register_types);

#define TYPE_STM32F0_TIM2 "stm32f0-tim2"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F0TIM2, STM32F0_TIM2)

struct STM32F0TIM2 {
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
#define STM32F0_TIM2_SIZE				0x50

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
	FIELD(TIM2_DMAR, DMAR, 0, 16)


__attribute__((unused))
static void stm32f0_tim2_update(STM32F0TIM2 *t) {
	int conditions = (((t->SR & 0x80) && (t->DIER & 0x80)) || ((t->DIER & 0x40) && (t->SR & 0x40)) || ((t->DIER & 0x20) && (t->SR & 0x20)) || ((t->DIER & 0x10) && (t->SR & 0x10)) || ((t->SR & 0x1) && (t->DIER & 0x1)) || ((t->SR & 0x8) && (t->DIER & 0x8)) || ((t->SR & 0x2) && (t->DIER & 0x2)) || ((t->SR & 0x4) && (t->DIER & 0x4)));
	qemu_set_irq(t->irq[0], conditions);

}

static void stm32f0_tim2_timer_callback_0(void *opaque) {
  static uint8_t period_reg_set_last = 0;
  STM32F0TIM2 *t = (STM32F0TIM2*)opaque;

  if (!period_reg_set_last && t->period_reg_set[0]) {
      period_reg_set_last = 1;
      t->CNT = 0;
  }
  t->CNT += 1;
  unsigned prd = t->period_set[0] ? t->cur_period[0] : t->ARR;
  if (t->CNT == prd) {
    t->cur_period[0] = t->ARR;
    t->CNT = 0;
    t->SR |= 0x1;t->SR |= 0x10;t->SR |= 0x20;t->SR |= 0x4;t->DIER |= 0x40;t->SR |= 0x80;t->DIER |= 0x2;t->DIER |= 0x8;t->DIER |= 0x4;t->DIER |= 0x80;t->DIER |= 0x20;t->SR |= 0x40;t->SR |= 0x8;t->DIER |= 0x1;t->SR |= 0x2;t->DIER |= 0x10;;
    qemu_set_irq(t->irq[0], 1);
    qemu_set_irq(t->irq[0], 0);
  }

  if (t->enabled[0]) {
    timer_mod(t->timers[0], qemu_clock_get_us(QEMU_CLOCK_VIRTUAL) + 1);
  }
}


static void stm32f0_tim2_register_reset(STM32F0TIM2 *t) {
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

static uint64_t stm32f0_tim2_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F0TIM2 *t = STM32F0_TIM2(opaque);
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
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F0 TIM2 read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f0_tim2_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F0TIM2 *t = STM32F0_TIM2(opaque);


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
				t->timers[0] = timer_new(QEMU_CLOCK_VIRTUAL, SCALE_US, stm32f0_tim2_timer_callback_0, t);
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
			stm32f0_tim2_update(t);
			break;
		case A_TIM2_SR:
			t->SR = value;
			stm32f0_tim2_update(t);
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
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F0 TIM2 write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f0_tim2_ops = {
	.read = stm32f0_tim2_read,
	.write = stm32f0_tim2_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f0_tim2_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F0TIM2 *t = STM32F0_TIM2(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f0_tim2_ops, t, "stm32f0-tim2", STM32F0_TIM2_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32f0_tim2_realize(DeviceState *dev, Error **errp) {
	STM32F0TIM2 *t = STM32F0_TIM2(dev);
	t->timers[0] = timer_new(QEMU_CLOCK_VIRTUAL, SCALE_US, stm32f0_tim2_timer_callback_0, t);

}

static void stm32f0_tim2_reset_enter(Object *obj, ResetType type) {
	STM32F0TIM2 *t = STM32F0_TIM2(obj);
	stm32f0_tim2_register_reset(t);
}

static const VMStateDescription stm32f0_tim2_vmstate = {
	.name = "stm32f0-tim2",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR1, STM32F0TIM2),
		VMSTATE_UINT32(CR2, STM32F0TIM2),
		VMSTATE_UINT32(SMCR, STM32F0TIM2),
		VMSTATE_UINT32(DIER, STM32F0TIM2),
		VMSTATE_UINT32(SR, STM32F0TIM2),
		VMSTATE_UINT32(EGR, STM32F0TIM2),
		VMSTATE_UINT32(CCMR1_Output, STM32F0TIM2),
		VMSTATE_UINT32(CCMR1_Input, STM32F0TIM2),
		VMSTATE_UINT32(CCMR2_Output, STM32F0TIM2),
		VMSTATE_UINT32(CCMR2_Input, STM32F0TIM2),
		VMSTATE_UINT32(CCER, STM32F0TIM2),
		VMSTATE_UINT32(CNT, STM32F0TIM2),
		VMSTATE_UINT32(PSC, STM32F0TIM2),
		VMSTATE_UINT32(ARR, STM32F0TIM2),
		VMSTATE_UINT32(CCR1, STM32F0TIM2),
		VMSTATE_UINT32(CCR2, STM32F0TIM2),
		VMSTATE_UINT32(CCR3, STM32F0TIM2),
		VMSTATE_UINT32(CCR4, STM32F0TIM2),
		VMSTATE_UINT32(DCR, STM32F0TIM2),
		VMSTATE_UINT32(DMAR, STM32F0TIM2),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f0_tim2_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f0_tim2_vmstate;
	dc->realize = stm32f0_tim2_realize;
	rc->phases.enter = stm32f0_tim2_reset_enter;

}

static const TypeInfo stm32f0_tim2_info = {
	.name = TYPE_STM32F0_TIM2,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F0TIM2),
	.instance_init = stm32f0_tim2_init,
	.class_init = stm32f0_tim2_class_init,
};

static void stm32f0_tim2_register_types(void) {
	type_register_static(&stm32f0_tim2_info);
}

type_init(stm32f0_tim2_register_types);

#define TYPE_STM32F0_USART "stm32f0-usart"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F0USART, STM32F0_USART)

struct STM32F0USART {
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
	STM32F0DMA *dma;
	/* base */
	uint32_t base;

};
#define STM32F0_USART_SIZE				0x2c

REG32(USART_CR1, 0x0)
	FIELD(USART_CR1, UE, 0, 1)
	FIELD(USART_CR1, UESM, 1, 1)
	FIELD(USART_CR1, RE, 2, 1)
	FIELD(USART_CR1, TE, 3, 1)
	FIELD(USART_CR1, IDLEIE, 4, 1)
	FIELD(USART_CR1, RXNEIE, 5, 1)
	FIELD(USART_CR1, TCIE, 6, 1)
	FIELD(USART_CR1, TXEIE, 7, 1)
	FIELD(USART_CR1, PEIE, 8, 1)
	FIELD(USART_CR1, PS, 9, 1)
	FIELD(USART_CR1, PCE, 10, 1)
	FIELD(USART_CR1, WAKE, 11, 1)
	FIELD(USART_CR1, M, 12, 1)
	FIELD(USART_CR1, MME, 13, 1)
	FIELD(USART_CR1, CMIE, 14, 1)
	FIELD(USART_CR1, OVER8, 15, 1)
	FIELD(USART_CR1, DEDT, 16, 5)
	FIELD(USART_CR1, DEAT, 21, 5)
	FIELD(USART_CR1, RTOIE, 26, 1)
	FIELD(USART_CR1, EOBIE, 27, 1)
	FIELD(USART_CR1, M1, 28, 1)
REG32(USART_CR2, 0x4)
	FIELD(USART_CR2, ADD4, 28, 4)
	FIELD(USART_CR2, ADD0, 24, 4)
	FIELD(USART_CR2, RTOEN, 23, 1)
	FIELD(USART_CR2, ABRMOD, 21, 2)
	FIELD(USART_CR2, ABREN, 20, 1)
	FIELD(USART_CR2, MSBFIRST, 19, 1)
	FIELD(USART_CR2, DATAINV, 18, 1)
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
static void stm32f0_usart_update(STM32F0USART *t) {
	int conditions = (((!(t->CR1 & 0x20)) && (!(t->CR3 & 0x1)) && (!(t->CR1 & 0x100)) && (!(t->CR1 & 0x4000000)) && (t->ISR & 0x80) && (t->CR1 & 0x80)) || ((!(t->CR1 & 0x20)) && (!(t->CR3 & 0x1)) && (!(t->CR1 & 0x100)) && (!(t->CR1 & 0x4000000)) && (t->ISR & 0x40) && (t->CR1 & 0x40)) || ((t->CR1 & 0x20) && (t->ISR & 0x20)) || ((!(t->CR3 & 0x40)) && (t->ISR & 0x1) && (t->CR1 & 0x20)) || ((!(t->CR3 & 0x40)) && (t->CR1 & 0x20) && (t->ISR & 0x800)) || ((t->ISR & 0x4) && (!(t->CR3 & 0x40)) && (t->CR1 & 0x20)) || ((!(t->CR3 & 0x40)) && (t->ISR & 0x8) && (t->CR1 & 0x20)) || ((!(t->CR3 & 0x40)) && (t->ISR & 0x1) && (t->CR3 & 0x1)) || ((!(t->CR1 & 0x20)) && (!(t->CR3 & 0x1)) && (!(t->CR1 & 0x100)) && (!(t->CR1 & 0x4000000)) && (t->ISR & 0x100000) && (t->CR3 & 0x400000)) || ((!(t->CR3 & 0x40)) && (t->ISR & 0x1) && (t->CR1 & 0x4000000)) || ((!(t->CR1 & 0x10)) && (!(t->CR1 & 0x20)) && (!(t->CR3 & 0x1)) && (!(t->CR1 & 0x100)) && (!(t->CR1 & 0x4000000)) && (t->ISR & 0x80) && (t->CR1 & 0x80)) || ((!(t->CR1 & 0x10)) && (!(t->CR1 & 0x20)) && (!(t->CR3 & 0x1)) && (!(t->CR1 & 0x100)) && (!(t->CR1 & 0x4000000)) && (t->ISR & 0x100000) && (t->CR3 & 0x400000)) || ((!(t->CR1 & 0x10)) && (!(t->CR1 & 0x20)) && (!(t->CR3 & 0x1)) && (!(t->CR1 & 0x100)) && (!(t->CR1 & 0x4000000)) && (t->ISR & 0x40) && (t->CR1 & 0x40)) || ((!(t->CR3 & 0x40)) && (t->CR1 & 0x100) && (t->ISR & 0x800)) || ((!(t->CR3 & 0x40)) && (t->CR3 & 0x1) && (t->ISR & 0x8)) || ((!(t->CR3 & 0x40)) && (t->CR3 & 0x1) && (t->ISR & 0x800)) || ((t->ISR & 0x4) && (!(t->CR3 & 0x40)) && (t->CR3 & 0x1)) || ((!(t->CR3 & 0x40)) && (t->ISR & 0x8) && (t->CR1 & 0x100)) || ((!(t->CR3 & 0x40)) && (t->ISR & 0x8) && (t->CR1 & 0x4000000)) || ((t->ISR & 0x4) && (!(t->CR3 & 0x40)) && (t->CR1 & 0x100)) || ((t->ISR & 0x4) && (!(t->CR3 & 0x40)) && (t->CR1 & 0x4000000)) || ((!(t->CR3 & 0x40)) && (t->CR1 & 0x4000000) && (t->ISR & 0x800)) || ((!(t->CR3 & 0x40)) && (t->ISR & 0x2) && (t->CR1 & 0x20)) || ((!(t->CR3 & 0x40)) && (t->ISR & 0x2) && (t->CR1 & 0x100)) || ((!(t->CR3 & 0x40)) && (t->ISR & 0x2) && (t->CR1 & 0x4000000)) || ((!(t->ISR & 0x10)) && (!(t->CR1 & 0x20)) && (!(t->CR3 & 0x1)) && (!(t->CR1 & 0x100)) && (!(t->CR1 & 0x4000000)) && (t->ISR & 0x40) && (t->CR1 & 0x40)) || ((!(t->CR3 & 0x40)) && (t->CR3 & 0x1) && (t->ISR & 0x2)) || ((!(t->ISR & 0x10)) && (!(t->CR1 & 0x20)) && (!(t->CR3 & 0x1)) && (!(t->CR1 & 0x100)) && (!(t->CR1 & 0x4000000)) && (t->ISR & 0x100000) && (t->CR3 & 0x400000)) || ((!(t->ISR & 0x10)) && (!(t->CR1 & 0x20)) && (!(t->CR3 & 0x1)) && (!(t->CR1 & 0x100)) && (!(t->CR1 & 0x4000000)) && (t->ISR & 0x80) && (t->CR1 & 0x80)) || ((!(t->ISR & 0x2)) && (!(t->ISR & 0x8)) && (!(t->ISR & 0x4)) && (!(t->ISR & 0x800)) && (!(t->ISR & 0x1)) && (t->ISR & 0x80) && (t->CR1 & 0x80)) || ((!(t->ISR & 0x2)) && (!(t->ISR & 0x10)) && (!(t->ISR & 0x8)) && (!(t->ISR & 0x4)) && (!(t->ISR & 0x800)) && (!(t->ISR & 0x1)) && (t->ISR & 0x80) && (t->CR1 & 0x80)) || ((!(t->ISR & 0x2)) && (!(t->ISR & 0x8)) && (!(t->ISR & 0x4)) && (!(t->ISR & 0x800)) && (!(t->ISR & 0x1)) && (t->ISR & 0x40) && (t->CR1 & 0x40)) || ((!(t->ISR & 0x2)) && (!(t->ISR & 0x10)) && (!(t->ISR & 0x8)) && (!(t->ISR & 0x4)) && (!(t->ISR & 0x800)) && (!(t->ISR & 0x1)) && (t->ISR & 0x40) && (t->CR1 & 0x40)) || ((!(t->ISR & 0x2)) && (!(t->ISR & 0x8)) && (!(t->ISR & 0x4)) && (!(t->ISR & 0x800)) && (!(t->ISR & 0x1)) && (t->ISR & 0x100000) && (t->CR3 & 0x400000)) || ((!(t->ISR & 0x2)) && (!(t->CR1 & 0x10)) && (!(t->ISR & 0x8)) && (!(t->ISR & 0x4)) && (!(t->ISR & 0x800)) && (!(t->ISR & 0x1)) && (t->ISR & 0x100000) && (t->CR3 & 0x400000)) || ((!(t->ISR & 0x2)) && (!(t->CR1 & 0x10)) && (!(t->ISR & 0x8)) && (!(t->ISR & 0x4)) && (!(t->ISR & 0x800)) && (!(t->ISR & 0x1)) && (t->ISR & 0x40) && (t->CR1 & 0x40)) || ((!(t->ISR & 0x2)) && (!(t->ISR & 0x10)) && (!(t->ISR & 0x8)) && (!(t->ISR & 0x4)) && (!(t->ISR & 0x800)) && (!(t->ISR & 0x1)) && (t->ISR & 0x100000) && (t->CR3 & 0x400000)) || ((!(t->ISR & 0x2)) && (!(t->CR1 & 0x10)) && (!(t->ISR & 0x8)) && (!(t->ISR & 0x4)) && (!(t->ISR & 0x800)) && (!(t->ISR & 0x1)) && (t->ISR & 0x80) && (t->CR1 & 0x80)) || ((!(t->CR3 & 0x40)) && (t->ISR & 0x1) && (t->CR1 & 0x100)) || ((!(t->CR3 & 0x40)) && (t->CR1 & 0x10) && (t->ISR & 0x10)));
	qemu_set_irq(t->irq[0], conditions);

}

static int stm32f0_usart_can_receive(void *opaque) {
	return 1;
}

static void stm32f0_usart_receive(void *opaque, const uint8_t *buf, int size) {
	STM32F0USART *t = STM32F0_USART(opaque);


	if ((t->CR3 & 0x40) && t->dma) {
		stm32f0_dma_receive(t->dma, t->base + 36, *buf);
	}
	t->RDR = *buf;
	t->ISR |= 0x20;

	stm32f0_usart_update(t);
}

static gboolean stm32f0_usart_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	STM32F0USART *t = STM32F0_USART(opaque);
	int ret;

	t->watch_tag = 0;
	t->ISR &= (~(0x40));
	t->ISR &= (~(0x80));

	stm32f0_usart_update(t);

	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->TDR), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, stm32f0_usart_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->ISR |= 0x40;
	t->ISR |= 0x80;

	stm32f0_usart_update(t);

	return FALSE;
}

static void stm32f0_usart_register_reset(STM32F0USART *t) {
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
	t->ISR |= 0x40;
	t->ISR |= 0x80;

}

static uint64_t stm32f0_usart_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F0USART *t = STM32F0_USART(opaque);
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
		case A_USART_RQR:
			ret = t->RQR;
			break;
		case A_USART_ISR:
			ret = t->ISR;
			break;
		case A_USART_ICR:
			ret = t->ICR;
			break;
		case A_USART_RDR:
			ret = t->RDR;
			t->ISR &= (~(0x20));
			qemu_chr_fe_accept_input(&(t->chr));
			stm32f0_usart_update(t);
			break;
		case A_USART_TDR:
			ret = t->TDR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F0 USART read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f0_usart_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F0USART *t = STM32F0_USART(opaque);


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
			stm32f0_usart_update(t);
			break;
		case A_USART_CR2:
			t->CR2 = value;
			break;
		case A_USART_CR3:
			t->CR3 = value;
			stm32f0_usart_update(t);
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
			stm32f0_usart_transmit(NULL, G_IO_OUT, t);
			if (((!(value & 0x100)) && (!(value & 0x200)) && (!(value & 0x400)) && (!(value & 0x800)) && (!(value & 0x1000)) && (!(value & 0x2000)) && (!(value & 0x4000)) && (!(value & 0x8000)))) {
				t->ISR |= 0x40;
				t->ISR |= 0x80;
			} else if (((!(value & 0x200)) && (!(value & 0x400)) && (!(value & 0x800)) && (!(value & 0x1000)) && (!(value & 0x2000)) && (!(value & 0x4000)) && (!(value & 0x8000)))) {
				t->ISR |= 0x40;
				t->ISR |= 0x80;
			}
			t->ISR |= 0x40;
			stm32f0_usart_update(t);
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F0 USART write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f0_usart_ops = {
	.read = stm32f0_usart_read,
	.write = stm32f0_usart_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f0_usart_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F0USART *t = STM32F0_USART(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f0_usart_ops, t, "stm32f0-usart", STM32F0_USART_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32f0_usart_realize(DeviceState *dev, Error **errp) {
	STM32F0USART *t = STM32F0_USART(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), stm32f0_usart_can_receive, stm32f0_usart_receive,
		NULL, NULL, t, NULL, true);

}

static void stm32f0_usart_reset_enter(Object *obj, ResetType type) {
	STM32F0USART *t = STM32F0_USART(obj);
	stm32f0_usart_register_reset(t);
}

static const VMStateDescription stm32f0_usart_vmstate = {
	.name = "stm32f0-usart",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR1, STM32F0USART),
		VMSTATE_UINT32(CR2, STM32F0USART),
		VMSTATE_UINT32(CR3, STM32F0USART),
		VMSTATE_UINT32(BRR, STM32F0USART),
		VMSTATE_UINT32(GTPR, STM32F0USART),
		VMSTATE_UINT32(RTOR, STM32F0USART),
		VMSTATE_UINT32(RQR, STM32F0USART),
		VMSTATE_UINT32(ISR, STM32F0USART),
		VMSTATE_UINT32(ICR, STM32F0USART),
		VMSTATE_UINT32(RDR, STM32F0USART),
		VMSTATE_UINT32(TDR, STM32F0USART),

		VMSTATE_END_OF_LIST()
	}
};

static Property stm32f0_usart_properties[] = {
	DEFINE_PROP_CHR("chardev", STM32F0USART, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void stm32f0_usart_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f0_usart_vmstate;
	dc->realize = stm32f0_usart_realize;
	rc->phases.enter = stm32f0_usart_reset_enter;
	device_class_set_props(dc, stm32f0_usart_properties);

}

static const TypeInfo stm32f0_usart_info = {
	.name = TYPE_STM32F0_USART,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F0USART),
	.instance_init = stm32f0_usart_init,
	.class_init = stm32f0_usart_class_init,
};

static void stm32f0_usart_register_types(void) {
	type_register_static(&stm32f0_usart_info);
}

type_init(stm32f0_usart_register_types);

#define TYPE_STM32F0_GPIOA "stm32f0-gpioa"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F0GPIOA, STM32F0_GPIOA)

struct STM32F0GPIOA {
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
#define STM32F0_GPIOA_SIZE				0x2c

REG32(GPIOA_MODER, 0x0)
	FIELD(GPIOA_MODER, MODER15, 30, 2)
	FIELD(GPIOA_MODER, MODER14, 28, 2)
	FIELD(GPIOA_MODER, MODER13, 26, 2)
	FIELD(GPIOA_MODER, MODER12, 24, 2)
	FIELD(GPIOA_MODER, MODER11, 22, 2)
	FIELD(GPIOA_MODER, MODER10, 20, 2)
	FIELD(GPIOA_MODER, MODER9, 18, 2)
	FIELD(GPIOA_MODER, MODER8, 16, 2)
	FIELD(GPIOA_MODER, MODER7, 14, 2)
	FIELD(GPIOA_MODER, MODER6, 12, 2)
	FIELD(GPIOA_MODER, MODER5, 10, 2)
	FIELD(GPIOA_MODER, MODER4, 8, 2)
	FIELD(GPIOA_MODER, MODER3, 6, 2)
	FIELD(GPIOA_MODER, MODER2, 4, 2)
	FIELD(GPIOA_MODER, MODER1, 2, 2)
	FIELD(GPIOA_MODER, MODER0, 0, 2)
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
	FIELD(GPIOA_OSPEEDR, OSPEEDR15, 30, 2)
	FIELD(GPIOA_OSPEEDR, OSPEEDR14, 28, 2)
	FIELD(GPIOA_OSPEEDR, OSPEEDR13, 26, 2)
	FIELD(GPIOA_OSPEEDR, OSPEEDR12, 24, 2)
	FIELD(GPIOA_OSPEEDR, OSPEEDR11, 22, 2)
	FIELD(GPIOA_OSPEEDR, OSPEEDR10, 20, 2)
	FIELD(GPIOA_OSPEEDR, OSPEEDR9, 18, 2)
	FIELD(GPIOA_OSPEEDR, OSPEEDR8, 16, 2)
	FIELD(GPIOA_OSPEEDR, OSPEEDR7, 14, 2)
	FIELD(GPIOA_OSPEEDR, OSPEEDR6, 12, 2)
	FIELD(GPIOA_OSPEEDR, OSPEEDR5, 10, 2)
	FIELD(GPIOA_OSPEEDR, OSPEEDR4, 8, 2)
	FIELD(GPIOA_OSPEEDR, OSPEEDR3, 6, 2)
	FIELD(GPIOA_OSPEEDR, OSPEEDR2, 4, 2)
	FIELD(GPIOA_OSPEEDR, OSPEEDR1, 2, 2)
	FIELD(GPIOA_OSPEEDR, OSPEEDR0, 0, 2)
REG32(GPIOA_PUPDR, 0xc)
	FIELD(GPIOA_PUPDR, PUPDR15, 30, 2)
	FIELD(GPIOA_PUPDR, PUPDR14, 28, 2)
	FIELD(GPIOA_PUPDR, PUPDR13, 26, 2)
	FIELD(GPIOA_PUPDR, PUPDR12, 24, 2)
	FIELD(GPIOA_PUPDR, PUPDR11, 22, 2)
	FIELD(GPIOA_PUPDR, PUPDR10, 20, 2)
	FIELD(GPIOA_PUPDR, PUPDR9, 18, 2)
	FIELD(GPIOA_PUPDR, PUPDR8, 16, 2)
	FIELD(GPIOA_PUPDR, PUPDR7, 14, 2)
	FIELD(GPIOA_PUPDR, PUPDR6, 12, 2)
	FIELD(GPIOA_PUPDR, PUPDR5, 10, 2)
	FIELD(GPIOA_PUPDR, PUPDR4, 8, 2)
	FIELD(GPIOA_PUPDR, PUPDR3, 6, 2)
	FIELD(GPIOA_PUPDR, PUPDR2, 4, 2)
	FIELD(GPIOA_PUPDR, PUPDR1, 2, 2)
	FIELD(GPIOA_PUPDR, PUPDR0, 0, 2)
REG32(GPIOA_IDR, 0x10)
	FIELD(GPIOA_IDR, IDR15, 15, 1)
	FIELD(GPIOA_IDR, IDR14, 14, 1)
	FIELD(GPIOA_IDR, IDR13, 13, 1)
	FIELD(GPIOA_IDR, IDR12, 12, 1)
	FIELD(GPIOA_IDR, IDR11, 11, 1)
	FIELD(GPIOA_IDR, IDR10, 10, 1)
	FIELD(GPIOA_IDR, IDR9, 9, 1)
	FIELD(GPIOA_IDR, IDR8, 8, 1)
	FIELD(GPIOA_IDR, IDR7, 7, 1)
	FIELD(GPIOA_IDR, IDR6, 6, 1)
	FIELD(GPIOA_IDR, IDR5, 5, 1)
	FIELD(GPIOA_IDR, IDR4, 4, 1)
	FIELD(GPIOA_IDR, IDR3, 3, 1)
	FIELD(GPIOA_IDR, IDR2, 2, 1)
	FIELD(GPIOA_IDR, IDR1, 1, 1)
	FIELD(GPIOA_IDR, IDR0, 0, 1)
REG32(GPIOA_ODR, 0x14)
	FIELD(GPIOA_ODR, ODR15, 15, 1)
	FIELD(GPIOA_ODR, ODR14, 14, 1)
	FIELD(GPIOA_ODR, ODR13, 13, 1)
	FIELD(GPIOA_ODR, ODR12, 12, 1)
	FIELD(GPIOA_ODR, ODR11, 11, 1)
	FIELD(GPIOA_ODR, ODR10, 10, 1)
	FIELD(GPIOA_ODR, ODR9, 9, 1)
	FIELD(GPIOA_ODR, ODR8, 8, 1)
	FIELD(GPIOA_ODR, ODR7, 7, 1)
	FIELD(GPIOA_ODR, ODR6, 6, 1)
	FIELD(GPIOA_ODR, ODR5, 5, 1)
	FIELD(GPIOA_ODR, ODR4, 4, 1)
	FIELD(GPIOA_ODR, ODR3, 3, 1)
	FIELD(GPIOA_ODR, ODR2, 2, 1)
	FIELD(GPIOA_ODR, ODR1, 1, 1)
	FIELD(GPIOA_ODR, ODR0, 0, 1)
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
	FIELD(GPIOA_AFRL, AFRL7, 28, 4)
	FIELD(GPIOA_AFRL, AFRL6, 24, 4)
	FIELD(GPIOA_AFRL, AFRL5, 20, 4)
	FIELD(GPIOA_AFRL, AFRL4, 16, 4)
	FIELD(GPIOA_AFRL, AFRL3, 12, 4)
	FIELD(GPIOA_AFRL, AFRL2, 8, 4)
	FIELD(GPIOA_AFRL, AFRL1, 4, 4)
	FIELD(GPIOA_AFRL, AFRL0, 0, 4)
REG32(GPIOA_AFRH, 0x24)
	FIELD(GPIOA_AFRH, AFRH15, 28, 4)
	FIELD(GPIOA_AFRH, AFRH14, 24, 4)
	FIELD(GPIOA_AFRH, AFRH13, 20, 4)
	FIELD(GPIOA_AFRH, AFRH12, 16, 4)
	FIELD(GPIOA_AFRH, AFRH11, 12, 4)
	FIELD(GPIOA_AFRH, AFRH10, 8, 4)
	FIELD(GPIOA_AFRH, AFRH9, 4, 4)
	FIELD(GPIOA_AFRH, AFRH8, 0, 4)
REG32(GPIOA_BRR, 0x28)
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


static void stm32f0_gpioa_register_reset(STM32F0GPIOA *t) {
	t->MODER = 0x28000000;
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

static uint64_t stm32f0_gpioa_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F0GPIOA *t = STM32F0_GPIOA(opaque);
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
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F0 GPIOA read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f0_gpioa_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F0GPIOA *t = STM32F0_GPIOA(opaque);


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
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F0 GPIOA write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f0_gpioa_ops = {
	.read = stm32f0_gpioa_read,
	.write = stm32f0_gpioa_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f0_gpioa_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F0GPIOA *t = STM32F0_GPIOA(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f0_gpioa_ops, t, "stm32f0-gpioa", STM32F0_GPIOA_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void stm32f0_gpioa_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32f0_gpioa_reset_enter(Object *obj, ResetType type) {
	STM32F0GPIOA *t = STM32F0_GPIOA(obj);
	stm32f0_gpioa_register_reset(t);
}

static const VMStateDescription stm32f0_gpioa_vmstate = {
	.name = "stm32f0-gpioa",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(MODER, STM32F0GPIOA),
		VMSTATE_UINT32(OTYPER, STM32F0GPIOA),
		VMSTATE_UINT32(OSPEEDR, STM32F0GPIOA),
		VMSTATE_UINT32(PUPDR, STM32F0GPIOA),
		VMSTATE_UINT32(IDR, STM32F0GPIOA),
		VMSTATE_UINT32(ODR, STM32F0GPIOA),
		VMSTATE_UINT32(BSRR, STM32F0GPIOA),
		VMSTATE_UINT32(LCKR, STM32F0GPIOA),
		VMSTATE_UINT32(AFRL, STM32F0GPIOA),
		VMSTATE_UINT32(AFRH, STM32F0GPIOA),
		VMSTATE_UINT32(BRR, STM32F0GPIOA),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f0_gpioa_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f0_gpioa_vmstate;
	dc->realize = stm32f0_gpioa_realize;
	rc->phases.enter = stm32f0_gpioa_reset_enter;

}

static const TypeInfo stm32f0_gpioa_info = {
	.name = TYPE_STM32F0_GPIOA,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F0GPIOA),
	.instance_init = stm32f0_gpioa_init,
	.class_init = stm32f0_gpioa_class_init,
};

static void stm32f0_gpioa_register_types(void) {
	type_register_static(&stm32f0_gpioa_info);
}

type_init(stm32f0_gpioa_register_types);

#define TYPE_STM32F0_GPIOF "stm32f0-gpiof"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F0GPIOF, STM32F0_GPIOF)

struct STM32F0GPIOF {
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
#define STM32F0_GPIOF_SIZE				0x2c

REG32(GPIOF_MODER, 0x0)
	FIELD(GPIOF_MODER, MODER15, 30, 2)
	FIELD(GPIOF_MODER, MODER14, 28, 2)
	FIELD(GPIOF_MODER, MODER13, 26, 2)
	FIELD(GPIOF_MODER, MODER12, 24, 2)
	FIELD(GPIOF_MODER, MODER11, 22, 2)
	FIELD(GPIOF_MODER, MODER10, 20, 2)
	FIELD(GPIOF_MODER, MODER9, 18, 2)
	FIELD(GPIOF_MODER, MODER8, 16, 2)
	FIELD(GPIOF_MODER, MODER7, 14, 2)
	FIELD(GPIOF_MODER, MODER6, 12, 2)
	FIELD(GPIOF_MODER, MODER5, 10, 2)
	FIELD(GPIOF_MODER, MODER4, 8, 2)
	FIELD(GPIOF_MODER, MODER3, 6, 2)
	FIELD(GPIOF_MODER, MODER2, 4, 2)
	FIELD(GPIOF_MODER, MODER1, 2, 2)
	FIELD(GPIOF_MODER, MODER0, 0, 2)
REG32(GPIOF_OTYPER, 0x4)
	FIELD(GPIOF_OTYPER, OT15, 15, 1)
	FIELD(GPIOF_OTYPER, OT14, 14, 1)
	FIELD(GPIOF_OTYPER, OT13, 13, 1)
	FIELD(GPIOF_OTYPER, OT12, 12, 1)
	FIELD(GPIOF_OTYPER, OT11, 11, 1)
	FIELD(GPIOF_OTYPER, OT10, 10, 1)
	FIELD(GPIOF_OTYPER, OT9, 9, 1)
	FIELD(GPIOF_OTYPER, OT8, 8, 1)
	FIELD(GPIOF_OTYPER, OT7, 7, 1)
	FIELD(GPIOF_OTYPER, OT6, 6, 1)
	FIELD(GPIOF_OTYPER, OT5, 5, 1)
	FIELD(GPIOF_OTYPER, OT4, 4, 1)
	FIELD(GPIOF_OTYPER, OT3, 3, 1)
	FIELD(GPIOF_OTYPER, OT2, 2, 1)
	FIELD(GPIOF_OTYPER, OT1, 1, 1)
	FIELD(GPIOF_OTYPER, OT0, 0, 1)
REG32(GPIOF_OSPEEDR, 0x8)
	FIELD(GPIOF_OSPEEDR, OSPEEDR15, 30, 2)
	FIELD(GPIOF_OSPEEDR, OSPEEDR14, 28, 2)
	FIELD(GPIOF_OSPEEDR, OSPEEDR13, 26, 2)
	FIELD(GPIOF_OSPEEDR, OSPEEDR12, 24, 2)
	FIELD(GPIOF_OSPEEDR, OSPEEDR11, 22, 2)
	FIELD(GPIOF_OSPEEDR, OSPEEDR10, 20, 2)
	FIELD(GPIOF_OSPEEDR, OSPEEDR9, 18, 2)
	FIELD(GPIOF_OSPEEDR, OSPEEDR8, 16, 2)
	FIELD(GPIOF_OSPEEDR, OSPEEDR7, 14, 2)
	FIELD(GPIOF_OSPEEDR, OSPEEDR6, 12, 2)
	FIELD(GPIOF_OSPEEDR, OSPEEDR5, 10, 2)
	FIELD(GPIOF_OSPEEDR, OSPEEDR4, 8, 2)
	FIELD(GPIOF_OSPEEDR, OSPEEDR3, 6, 2)
	FIELD(GPIOF_OSPEEDR, OSPEEDR2, 4, 2)
	FIELD(GPIOF_OSPEEDR, OSPEEDR1, 2, 2)
	FIELD(GPIOF_OSPEEDR, OSPEEDR0, 0, 2)
REG32(GPIOF_PUPDR, 0xc)
	FIELD(GPIOF_PUPDR, PUPDR15, 30, 2)
	FIELD(GPIOF_PUPDR, PUPDR14, 28, 2)
	FIELD(GPIOF_PUPDR, PUPDR13, 26, 2)
	FIELD(GPIOF_PUPDR, PUPDR12, 24, 2)
	FIELD(GPIOF_PUPDR, PUPDR11, 22, 2)
	FIELD(GPIOF_PUPDR, PUPDR10, 20, 2)
	FIELD(GPIOF_PUPDR, PUPDR9, 18, 2)
	FIELD(GPIOF_PUPDR, PUPDR8, 16, 2)
	FIELD(GPIOF_PUPDR, PUPDR7, 14, 2)
	FIELD(GPIOF_PUPDR, PUPDR6, 12, 2)
	FIELD(GPIOF_PUPDR, PUPDR5, 10, 2)
	FIELD(GPIOF_PUPDR, PUPDR4, 8, 2)
	FIELD(GPIOF_PUPDR, PUPDR3, 6, 2)
	FIELD(GPIOF_PUPDR, PUPDR2, 4, 2)
	FIELD(GPIOF_PUPDR, PUPDR1, 2, 2)
	FIELD(GPIOF_PUPDR, PUPDR0, 0, 2)
REG32(GPIOF_IDR, 0x10)
	FIELD(GPIOF_IDR, IDR15, 15, 1)
	FIELD(GPIOF_IDR, IDR14, 14, 1)
	FIELD(GPIOF_IDR, IDR13, 13, 1)
	FIELD(GPIOF_IDR, IDR12, 12, 1)
	FIELD(GPIOF_IDR, IDR11, 11, 1)
	FIELD(GPIOF_IDR, IDR10, 10, 1)
	FIELD(GPIOF_IDR, IDR9, 9, 1)
	FIELD(GPIOF_IDR, IDR8, 8, 1)
	FIELD(GPIOF_IDR, IDR7, 7, 1)
	FIELD(GPIOF_IDR, IDR6, 6, 1)
	FIELD(GPIOF_IDR, IDR5, 5, 1)
	FIELD(GPIOF_IDR, IDR4, 4, 1)
	FIELD(GPIOF_IDR, IDR3, 3, 1)
	FIELD(GPIOF_IDR, IDR2, 2, 1)
	FIELD(GPIOF_IDR, IDR1, 1, 1)
	FIELD(GPIOF_IDR, IDR0, 0, 1)
REG32(GPIOF_ODR, 0x14)
	FIELD(GPIOF_ODR, ODR15, 15, 1)
	FIELD(GPIOF_ODR, ODR14, 14, 1)
	FIELD(GPIOF_ODR, ODR13, 13, 1)
	FIELD(GPIOF_ODR, ODR12, 12, 1)
	FIELD(GPIOF_ODR, ODR11, 11, 1)
	FIELD(GPIOF_ODR, ODR10, 10, 1)
	FIELD(GPIOF_ODR, ODR9, 9, 1)
	FIELD(GPIOF_ODR, ODR8, 8, 1)
	FIELD(GPIOF_ODR, ODR7, 7, 1)
	FIELD(GPIOF_ODR, ODR6, 6, 1)
	FIELD(GPIOF_ODR, ODR5, 5, 1)
	FIELD(GPIOF_ODR, ODR4, 4, 1)
	FIELD(GPIOF_ODR, ODR3, 3, 1)
	FIELD(GPIOF_ODR, ODR2, 2, 1)
	FIELD(GPIOF_ODR, ODR1, 1, 1)
	FIELD(GPIOF_ODR, ODR0, 0, 1)
REG32(GPIOF_BSRR, 0x18)
	FIELD(GPIOF_BSRR, BR15, 31, 1)
	FIELD(GPIOF_BSRR, BR14, 30, 1)
	FIELD(GPIOF_BSRR, BR13, 29, 1)
	FIELD(GPIOF_BSRR, BR12, 28, 1)
	FIELD(GPIOF_BSRR, BR11, 27, 1)
	FIELD(GPIOF_BSRR, BR10, 26, 1)
	FIELD(GPIOF_BSRR, BR9, 25, 1)
	FIELD(GPIOF_BSRR, BR8, 24, 1)
	FIELD(GPIOF_BSRR, BR7, 23, 1)
	FIELD(GPIOF_BSRR, BR6, 22, 1)
	FIELD(GPIOF_BSRR, BR5, 21, 1)
	FIELD(GPIOF_BSRR, BR4, 20, 1)
	FIELD(GPIOF_BSRR, BR3, 19, 1)
	FIELD(GPIOF_BSRR, BR2, 18, 1)
	FIELD(GPIOF_BSRR, BR1, 17, 1)
	FIELD(GPIOF_BSRR, BR0, 16, 1)
	FIELD(GPIOF_BSRR, BS15, 15, 1)
	FIELD(GPIOF_BSRR, BS14, 14, 1)
	FIELD(GPIOF_BSRR, BS13, 13, 1)
	FIELD(GPIOF_BSRR, BS12, 12, 1)
	FIELD(GPIOF_BSRR, BS11, 11, 1)
	FIELD(GPIOF_BSRR, BS10, 10, 1)
	FIELD(GPIOF_BSRR, BS9, 9, 1)
	FIELD(GPIOF_BSRR, BS8, 8, 1)
	FIELD(GPIOF_BSRR, BS7, 7, 1)
	FIELD(GPIOF_BSRR, BS6, 6, 1)
	FIELD(GPIOF_BSRR, BS5, 5, 1)
	FIELD(GPIOF_BSRR, BS4, 4, 1)
	FIELD(GPIOF_BSRR, BS3, 3, 1)
	FIELD(GPIOF_BSRR, BS2, 2, 1)
	FIELD(GPIOF_BSRR, BS1, 1, 1)
	FIELD(GPIOF_BSRR, BS0, 0, 1)
REG32(GPIOF_LCKR, 0x1c)
	FIELD(GPIOF_LCKR, LCKK, 16, 1)
	FIELD(GPIOF_LCKR, LCK15, 15, 1)
	FIELD(GPIOF_LCKR, LCK14, 14, 1)
	FIELD(GPIOF_LCKR, LCK13, 13, 1)
	FIELD(GPIOF_LCKR, LCK12, 12, 1)
	FIELD(GPIOF_LCKR, LCK11, 11, 1)
	FIELD(GPIOF_LCKR, LCK10, 10, 1)
	FIELD(GPIOF_LCKR, LCK9, 9, 1)
	FIELD(GPIOF_LCKR, LCK8, 8, 1)
	FIELD(GPIOF_LCKR, LCK7, 7, 1)
	FIELD(GPIOF_LCKR, LCK6, 6, 1)
	FIELD(GPIOF_LCKR, LCK5, 5, 1)
	FIELD(GPIOF_LCKR, LCK4, 4, 1)
	FIELD(GPIOF_LCKR, LCK3, 3, 1)
	FIELD(GPIOF_LCKR, LCK2, 2, 1)
	FIELD(GPIOF_LCKR, LCK1, 1, 1)
	FIELD(GPIOF_LCKR, LCK0, 0, 1)
REG32(GPIOF_AFRL, 0x20)
	FIELD(GPIOF_AFRL, AFRL7, 28, 4)
	FIELD(GPIOF_AFRL, AFRL6, 24, 4)
	FIELD(GPIOF_AFRL, AFRL5, 20, 4)
	FIELD(GPIOF_AFRL, AFRL4, 16, 4)
	FIELD(GPIOF_AFRL, AFRL3, 12, 4)
	FIELD(GPIOF_AFRL, AFRL2, 8, 4)
	FIELD(GPIOF_AFRL, AFRL1, 4, 4)
	FIELD(GPIOF_AFRL, AFRL0, 0, 4)
REG32(GPIOF_AFRH, 0x24)
	FIELD(GPIOF_AFRH, AFRH15, 28, 4)
	FIELD(GPIOF_AFRH, AFRH14, 24, 4)
	FIELD(GPIOF_AFRH, AFRH13, 20, 4)
	FIELD(GPIOF_AFRH, AFRH12, 16, 4)
	FIELD(GPIOF_AFRH, AFRH11, 12, 4)
	FIELD(GPIOF_AFRH, AFRH10, 8, 4)
	FIELD(GPIOF_AFRH, AFRH9, 4, 4)
	FIELD(GPIOF_AFRH, AFRH8, 0, 4)
REG32(GPIOF_BRR, 0x28)
	FIELD(GPIOF_BRR, BR0, 0, 1)
	FIELD(GPIOF_BRR, BR1, 1, 1)
	FIELD(GPIOF_BRR, BR2, 2, 1)
	FIELD(GPIOF_BRR, BR3, 3, 1)
	FIELD(GPIOF_BRR, BR4, 4, 1)
	FIELD(GPIOF_BRR, BR5, 5, 1)
	FIELD(GPIOF_BRR, BR6, 6, 1)
	FIELD(GPIOF_BRR, BR7, 7, 1)
	FIELD(GPIOF_BRR, BR8, 8, 1)
	FIELD(GPIOF_BRR, BR9, 9, 1)
	FIELD(GPIOF_BRR, BR10, 10, 1)
	FIELD(GPIOF_BRR, BR11, 11, 1)
	FIELD(GPIOF_BRR, BR12, 12, 1)
	FIELD(GPIOF_BRR, BR13, 13, 1)
	FIELD(GPIOF_BRR, BR14, 14, 1)
	FIELD(GPIOF_BRR, BR15, 15, 1)


static void stm32f0_gpiof_register_reset(STM32F0GPIOF *t) {
	t->MODER = 0x0;
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

static uint64_t stm32f0_gpiof_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F0GPIOF *t = STM32F0_GPIOF(opaque);
	uint64_t ret;

	switch (offset) {
		case A_GPIOF_MODER:
			ret = t->MODER;
			break;
		case A_GPIOF_OTYPER:
			ret = t->OTYPER;
			break;
		case A_GPIOF_OSPEEDR:
			ret = t->OSPEEDR;
			break;
		case A_GPIOF_PUPDR:
			ret = t->PUPDR;
			break;
		case A_GPIOF_IDR:
			ret = t->IDR;
			break;
		case A_GPIOF_ODR:
			ret = t->ODR;
			break;
		case A_GPIOF_LCKR:
			ret = t->LCKR;
			break;
		case A_GPIOF_AFRL:
			ret = t->AFRL;
			break;
		case A_GPIOF_AFRH:
			ret = t->AFRH;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F0 GPIOF read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f0_gpiof_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F0GPIOF *t = STM32F0_GPIOF(opaque);


	switch (offset) {
		case A_GPIOF_MODER ... A_GPIOF_MODER + 3:
			value = value << ((offset - A_GPIOF_MODER) << 3);
			offset = A_GPIOF_MODER;
			break;
		case A_GPIOF_OTYPER ... A_GPIOF_OTYPER + 3:
			value = value << ((offset - A_GPIOF_OTYPER) << 3);
			offset = A_GPIOF_OTYPER;
			break;
		case A_GPIOF_OSPEEDR ... A_GPIOF_OSPEEDR + 3:
			value = value << ((offset - A_GPIOF_OSPEEDR) << 3);
			offset = A_GPIOF_OSPEEDR;
			break;
		case A_GPIOF_PUPDR ... A_GPIOF_PUPDR + 3:
			value = value << ((offset - A_GPIOF_PUPDR) << 3);
			offset = A_GPIOF_PUPDR;
			break;
		case A_GPIOF_ODR ... A_GPIOF_ODR + 3:
			value = value << ((offset - A_GPIOF_ODR) << 3);
			offset = A_GPIOF_ODR;
			break;
		case A_GPIOF_BSRR ... A_GPIOF_BSRR + 3:
			value = value << ((offset - A_GPIOF_BSRR) << 3);
			offset = A_GPIOF_BSRR;
			break;
		case A_GPIOF_LCKR ... A_GPIOF_LCKR + 3:
			value = value << ((offset - A_GPIOF_LCKR) << 3);
			offset = A_GPIOF_LCKR;
			break;
		case A_GPIOF_AFRL ... A_GPIOF_AFRL + 3:
			value = value << ((offset - A_GPIOF_AFRL) << 3);
			offset = A_GPIOF_AFRL;
			break;
		case A_GPIOF_AFRH ... A_GPIOF_AFRH + 3:
			value = value << ((offset - A_GPIOF_AFRH) << 3);
			offset = A_GPIOF_AFRH;
			break;
		case A_GPIOF_BRR ... A_GPIOF_BRR + 3:
			value = value << ((offset - A_GPIOF_BRR) << 3);
			offset = A_GPIOF_BRR;
			break;

		default: break;
	}

	switch (offset) {
		case A_GPIOF_MODER:
			t->MODER = value;
			break;
		case A_GPIOF_OTYPER:
			t->OTYPER = value;
			break;
		case A_GPIOF_OSPEEDR:
			t->OSPEEDR = value;
			break;
		case A_GPIOF_PUPDR:
			t->PUPDR = value;
			break;
		case A_GPIOF_ODR:
			t->ODR = value;
			break;
		case A_GPIOF_BSRR:
			t->BSRR = value;
			break;
		case A_GPIOF_LCKR:
			t->LCKR = value;
			break;
		case A_GPIOF_AFRL:
			t->AFRL = value;
			break;
		case A_GPIOF_AFRH:
			t->AFRH = value;
			break;
		case A_GPIOF_BRR:
			t->BRR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F0 GPIOF write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f0_gpiof_ops = {
	.read = stm32f0_gpiof_read,
	.write = stm32f0_gpiof_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f0_gpiof_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F0GPIOF *t = STM32F0_GPIOF(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f0_gpiof_ops, t, "stm32f0-gpiof", STM32F0_GPIOF_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void stm32f0_gpiof_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32f0_gpiof_reset_enter(Object *obj, ResetType type) {
	STM32F0GPIOF *t = STM32F0_GPIOF(obj);
	stm32f0_gpiof_register_reset(t);
}

static const VMStateDescription stm32f0_gpiof_vmstate = {
	.name = "stm32f0-gpiof",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(MODER, STM32F0GPIOF),
		VMSTATE_UINT32(OTYPER, STM32F0GPIOF),
		VMSTATE_UINT32(OSPEEDR, STM32F0GPIOF),
		VMSTATE_UINT32(PUPDR, STM32F0GPIOF),
		VMSTATE_UINT32(IDR, STM32F0GPIOF),
		VMSTATE_UINT32(ODR, STM32F0GPIOF),
		VMSTATE_UINT32(BSRR, STM32F0GPIOF),
		VMSTATE_UINT32(LCKR, STM32F0GPIOF),
		VMSTATE_UINT32(AFRL, STM32F0GPIOF),
		VMSTATE_UINT32(AFRH, STM32F0GPIOF),
		VMSTATE_UINT32(BRR, STM32F0GPIOF),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f0_gpiof_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f0_gpiof_vmstate;
	dc->realize = stm32f0_gpiof_realize;
	rc->phases.enter = stm32f0_gpiof_reset_enter;

}

static const TypeInfo stm32f0_gpiof_info = {
	.name = TYPE_STM32F0_GPIOF,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F0GPIOF),
	.instance_init = stm32f0_gpiof_init,
	.class_init = stm32f0_gpiof_class_init,
};

static void stm32f0_gpiof_register_types(void) {
	type_register_static(&stm32f0_gpiof_info);
}

type_init(stm32f0_gpiof_register_types);

#define TYPE_STM32F0_SPI "stm32f0-spi"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F0SPI, STM32F0_SPI)

struct STM32F0SPI {
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
	STM32F0DMA *dma;
	/* base */
	uint32_t base;

};
#define STM32F0_SPI_SIZE				0x24

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
	FIELD(SPI_CR2, NSSP, 3, 1)
	FIELD(SPI_CR2, FRF, 4, 1)
	FIELD(SPI_CR2, ERRIE, 5, 1)
	FIELD(SPI_CR2, RXNEIE, 6, 1)
	FIELD(SPI_CR2, TXEIE, 7, 1)
	FIELD(SPI_CR2, DS, 8, 4)
	FIELD(SPI_CR2, FRXTH, 12, 1)
	FIELD(SPI_CR2, LDMA_RX, 13, 1)
	FIELD(SPI_CR2, LDMA_TX, 14, 1)
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
	FIELD(SPI_SR, FRLVL, 9, 2)
	FIELD(SPI_SR, FTLVL, 11, 2)
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
static void stm32f0_spi_update(STM32F0SPI *t) {
	int conditions = (((t->SR & 0x20) && (t->CR2 & 0x20) && (!(t->CR2 & 0x1)) && (!(t->CR2 & 0x2))) || ((!(t->SR & 0x40)) && (t->CR2 & 0x40) && (t->SR & 0x1)) || ((t->SR & 0x100) && (t->CR2 & 0x20) && (!(t->CR2 & 0x1)) && (!(t->CR2 & 0x2))) || ((t->CR2 & 0x20) && (!(t->CR2 & 0x1)) && (!(t->CR2 & 0x2)) && (t->SR & 0x40)) || ((t->SR & 0x2) && (t->CR2 & 0x80)));
	qemu_set_irq(t->irq[0], conditions);

}

static int stm32f0_spi_can_receive(void *opaque) {
	return 1;
}

static void stm32f0_spi_receive(void *opaque, const uint8_t *buf, int size) {
	STM32F0SPI *t = STM32F0_SPI(opaque);


	if ((t->CR2 & 0x1) && t->dma) {
		stm32f0_dma_receive(t->dma, t->base + 12, *buf);
	}
	t->DR = *buf;
	t->SR |= 0x1;

	stm32f0_spi_update(t);
}

static gboolean stm32f0_spi_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	STM32F0SPI *t = STM32F0_SPI(opaque);
	int ret;

	t->watch_tag = 0;
	t->SR |= 0x400;
	t->SR &= (~(0x2));
	t->SR |= 0x800;
	t->SR |= 0x1000;
	t->SR |= 0x200;
	t->SR |= 0x80;

	stm32f0_spi_update(t);

	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->DR), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, stm32f0_spi_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->SR &= (~(0x80));
	t->SR |= 0x2;
	t->SR &= (~(0x800));
	t->SR &= (~(0x400));
	t->SR &= (~(0x200));
	t->SR &= (~(0x1000));

	stm32f0_spi_update(t);

	return FALSE;
}

static void stm32f0_spi_register_reset(STM32F0SPI *t) {
	t->CR1 = 0x0;
	t->CR2 = 0x0;
	t->SR = 0x2;
	t->DR = 0x0;
	t->CRCPR = 0x7;
	t->RXCRCR = 0x0;
	t->TXCRCR = 0x0;
	t->I2SCFGR = 0x0;
	t->I2SPR = 0x10;
	t->SR &= (~(0x80));
	t->SR |= 0x2;
	t->SR &= (~(0x800));
	t->SR &= (~(0x400));
	t->SR &= (~(0x200));
	t->SR &= (~(0x1000));

}

static uint64_t stm32f0_spi_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F0SPI *t = STM32F0_SPI(opaque);
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
			stm32f0_spi_update(t);
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
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F0 SPI read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f0_spi_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F0SPI *t = STM32F0_SPI(opaque);


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
				t->SR &= (~(0x400));
				t->SR &= (~(0x200));
			}
			t->SR &= (~(0x80));
			stm32f0_spi_update(t);
			break;
		case A_SPI_CR2:
			t->CR2 = value;
			if (((!(value & 0x20)) && (!(value & 0x80)))) {
				t->SR &= (~(0x800));
				t->SR &= (~(0x1000));
			} else if (((!(value & 0x20)) && (!(value & 0x40)))) {
				t->SR &= (~(0x80));
			} else if ((!(value & 0x80))) {
				t->SR &= (~(0x800));
				t->SR &= (~(0x1000));
				t->CR2 &= (~(0x80));
			}
			if ((!(value & 0x40))) {
				t->SR &= (~(0x80));
				t->CR2 &= (~(0x40));
			}
			if ((!(value & 0x20))) {
				t->SR &= (~(0x800));
				t->SR &= (~(0x1000));
			}
			stm32f0_spi_update(t);
			break;
		case A_SPI_SR:
			t->SR &= value;
			t->SR &= (~(0x80));
			t->SR |= 0x2;
			t->SR &= (~(0x800));
			t->SR &= (~(0x400));
			t->SR &= (~(0x200));
			t->SR &= (~(0x1000));
			stm32f0_spi_update(t);
			break;
		case A_SPI_DR:
			t->DR = value;
			stm32f0_spi_transmit(NULL, G_IO_OUT, t);
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
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F0 SPI write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f0_spi_ops = {
	.read = stm32f0_spi_read,
	.write = stm32f0_spi_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f0_spi_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F0SPI *t = STM32F0_SPI(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f0_spi_ops, t, "stm32f0-spi", STM32F0_SPI_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32f0_spi_realize(DeviceState *dev, Error **errp) {
	STM32F0SPI *t = STM32F0_SPI(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), stm32f0_spi_can_receive, stm32f0_spi_receive,
		NULL, NULL, t, NULL, true);

}

static void stm32f0_spi_reset_enter(Object *obj, ResetType type) {
	STM32F0SPI *t = STM32F0_SPI(obj);
	stm32f0_spi_register_reset(t);
}

static const VMStateDescription stm32f0_spi_vmstate = {
	.name = "stm32f0-spi",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR1, STM32F0SPI),
		VMSTATE_UINT32(CR2, STM32F0SPI),
		VMSTATE_UINT32(SR, STM32F0SPI),
		VMSTATE_UINT32(DR, STM32F0SPI),
		VMSTATE_UINT32(CRCPR, STM32F0SPI),
		VMSTATE_UINT32(RXCRCR, STM32F0SPI),
		VMSTATE_UINT32(TXCRCR, STM32F0SPI),
		VMSTATE_UINT32(I2SCFGR, STM32F0SPI),
		VMSTATE_UINT32(I2SPR, STM32F0SPI),

		VMSTATE_END_OF_LIST()
	}
};

static Property stm32f0_spi_properties[] = {
	DEFINE_PROP_CHR("chardev", STM32F0SPI, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void stm32f0_spi_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f0_spi_vmstate;
	dc->realize = stm32f0_spi_realize;
	rc->phases.enter = stm32f0_spi_reset_enter;
	device_class_set_props(dc, stm32f0_spi_properties);

}

static const TypeInfo stm32f0_spi_info = {
	.name = TYPE_STM32F0_SPI,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F0SPI),
	.instance_init = stm32f0_spi_init,
	.class_init = stm32f0_spi_class_init,
};

static void stm32f0_spi_register_types(void) {
	type_register_static(&stm32f0_spi_info);
}

type_init(stm32f0_spi_register_types);

#define TYPE_STM32F0_I2C "stm32f0-i2c"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F0I2C, STM32F0_I2C)

struct STM32F0I2C {
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
	STM32F0DMA *dma;
	/* base */
	uint32_t base;

};
#define STM32F0_I2C_SIZE				0x2c

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
	FIELD(I2C_CR1, SWRST, 13, 1)
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
	FIELD(I2C_CR2, SADD8, 8, 2)
	FIELD(I2C_CR2, SADD1, 1, 7)
	FIELD(I2C_CR2, SADD0, 0, 1)
REG32(I2C_OAR1, 0x8)
	FIELD(I2C_OAR1, OA1_0, 0, 1)
	FIELD(I2C_OAR1, OA1_1, 1, 7)
	FIELD(I2C_OAR1, OA1_8, 8, 2)
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


static int stm32f0_i2c_can_receive(void *opaque) {
	return 1;
}

static void stm32f0_i2c_receive(void *opaque, const uint8_t *buf, int size) {
	STM32F0I2C *t = STM32F0_I2C(opaque);


	if ((t->CR1 & 0x8000) && t->dma) {
		stm32f0_dma_receive(t->dma, t->base + 36, *buf);
	}
	t->RXDR = *buf;
	t->ISR &= (~(0x20));
	t->ISR |= 0x8;
	t->ISR &= (~(0x10000));
	t->ISR |= 0x4;
	t->ISR |= 0x10;
	t->ISR &= (~(0x8000));
	t->ISR |= 0x20;
	t->ISR &= (~(0x10));


}

static gboolean stm32f0_i2c_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	STM32F0I2C *t = STM32F0_I2C(opaque);
	int ret;

	t->watch_tag = 0;
	t->ISR &= (~(0x80));
	t->ISR |= 0x8000;
	t->ISR &= (~(0x20));
	t->ISR &= (~(0x40));
	t->ISR &= (~(0x10000));
	t->ISR |= 0x4;
	t->ISR &= (~(0x2));
	t->ISR &= (~(0x10));
	t->ISR &= (~(0x8));



	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->TXDR), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, stm32f0_i2c_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->ISR &= (~(0x4));
	t->ISR |= 0x2;
	t->ISR |= 0x10000;
	t->ISR |= 0x8;
	t->ISR |= 0x40;
	t->ISR |= 0x80;
	t->ISR |= 0x20;
	t->ISR |= 0x10;
	t->ISR &= (~(0x8000));



	return FALSE;
}

static void stm32f0_i2c_register_reset(STM32F0I2C *t) {
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
	t->ISR &= (~(0x4));
	t->ISR |= 0x2;
	t->ISR |= 0x10000;
	t->ISR |= 0x8;
	t->ISR |= 0x40;
	t->ISR |= 0x80;
	t->ISR |= 0x20;
	t->ISR |= 0x10;
	t->ISR &= (~(0x8000));

}

static uint64_t stm32f0_i2c_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F0I2C *t = STM32F0_I2C(opaque);
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
			t->ISR &= (~(0x4));
			t->ISR |= 0x8000;
			t->ISR &= (~(0x20));
			t->ISR |= 0x10000;
			t->ISR |= 0x20;
			t->ISR |= 0x10;
			t->ISR &= (~(0x10));
			t->ISR &= (~(0x8));
			qemu_chr_fe_accept_input(&(t->chr));
			break;
		case A_I2C_TXDR:
			ret = t->TXDR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F0 I2C read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f0_i2c_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F0I2C *t = STM32F0_I2C(opaque);


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
			stm32f0_i2c_transmit(NULL, G_IO_OUT, t);
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F0 I2C write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f0_i2c_ops = {
	.read = stm32f0_i2c_read,
	.write = stm32f0_i2c_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f0_i2c_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F0I2C *t = STM32F0_I2C(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f0_i2c_ops, t, "stm32f0-i2c", STM32F0_I2C_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32f0_i2c_realize(DeviceState *dev, Error **errp) {
	STM32F0I2C *t = STM32F0_I2C(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), stm32f0_i2c_can_receive, stm32f0_i2c_receive,
		NULL, NULL, t, NULL, true);

}

static void stm32f0_i2c_reset_enter(Object *obj, ResetType type) {
	STM32F0I2C *t = STM32F0_I2C(obj);
	stm32f0_i2c_register_reset(t);
}

static const VMStateDescription stm32f0_i2c_vmstate = {
	.name = "stm32f0-i2c",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR1, STM32F0I2C),
		VMSTATE_UINT32(CR2, STM32F0I2C),
		VMSTATE_UINT32(OAR1, STM32F0I2C),
		VMSTATE_UINT32(OAR2, STM32F0I2C),
		VMSTATE_UINT32(TIMINGR, STM32F0I2C),
		VMSTATE_UINT32(TIMEOUTR, STM32F0I2C),
		VMSTATE_UINT32(ISR, STM32F0I2C),
		VMSTATE_UINT32(ICR, STM32F0I2C),
		VMSTATE_UINT32(PECR, STM32F0I2C),
		VMSTATE_UINT32(RXDR, STM32F0I2C),
		VMSTATE_UINT32(TXDR, STM32F0I2C),

		VMSTATE_END_OF_LIST()
	}
};

static Property stm32f0_i2c_properties[] = {
	DEFINE_PROP_CHR("chardev", STM32F0I2C, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void stm32f0_i2c_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f0_i2c_vmstate;
	dc->realize = stm32f0_i2c_realize;
	rc->phases.enter = stm32f0_i2c_reset_enter;
	device_class_set_props(dc, stm32f0_i2c_properties);

}

static const TypeInfo stm32f0_i2c_info = {
	.name = TYPE_STM32F0_I2C,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F0I2C),
	.instance_init = stm32f0_i2c_init,
	.class_init = stm32f0_i2c_class_init,
};

static void stm32f0_i2c_register_types(void) {
	type_register_static(&stm32f0_i2c_info);
}

type_init(stm32f0_i2c_register_types);

#define TYPE_STM32F072_MACHINE MACHINE_TYPE_NAME("stm32f072")
OBJECT_DECLARE_TYPE(STM32F072MachineState, STM32F072MachineClass, STM32F072_MACHINE)

struct STM32F072MachineClass {
	MachineClass parent;
};

struct STM32F072MachineState {
	MachineState parent;
	ARMv7MState armv7m;
};

static void stm32f072_periph_init(MachineState *machine) {
	STM32F072MachineState *sms = STM32F072_MACHINE(machine);
	STM32F0DMA *p0 = g_new(STM32F0DMA, 1);
	object_initialize_child(OBJECT(sms), "DMA", p0, TYPE_STM32F0_DMA);
	sysbus_realize(SYS_BUS_DEVICE(p0), &error_fatal);
	p0->base = 0x40020000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p0), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 9));
	sysbus_connect_irq(SYS_BUS_DEVICE(p0), 1, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 10));
	sysbus_connect_irq(SYS_BUS_DEVICE(p0), 2, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 11));
	sysbus_mmio_map(SYS_BUS_DEVICE(p0), 0, 0x40020000);

	STM32F0TIM2 *p1 = g_new(STM32F0TIM2, 1);
	object_initialize_child(OBJECT(sms), "TIM2", p1, TYPE_STM32F0_TIM2);
	sysbus_realize(SYS_BUS_DEVICE(p1), &error_fatal);
	p1->base = 0x40000000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p1), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 15));
	sysbus_mmio_map(SYS_BUS_DEVICE(p1), 0, 0x40000000);

	STM32F0TIM2 *p2 = g_new(STM32F0TIM2, 1);
	object_initialize_child(OBJECT(sms), "TIM3", p2, TYPE_STM32F0_TIM2);
	sysbus_realize(SYS_BUS_DEVICE(p2), &error_fatal);
	p2->base = 0x40000400;
	sysbus_connect_irq(SYS_BUS_DEVICE(p2), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 16));
	sysbus_mmio_map(SYS_BUS_DEVICE(p2), 0, 0x40000400);

	STM32F0USART *p3 = g_new(STM32F0USART, 1);
	object_initialize_child(OBJECT(sms), "USART1", p3, TYPE_STM32F0_USART);
	qdev_prop_set_chr(DEVICE(p3), "chardev", qemu_chr_new("soc-usart1", "chardev:usart1", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p3), &error_fatal);
	p3->base = 0x40013800;
	sysbus_connect_irq(SYS_BUS_DEVICE(p3), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 27));
	sysbus_mmio_map(SYS_BUS_DEVICE(p3), 0, 0x40013800);

	STM32F0USART *p4 = g_new(STM32F0USART, 1);
	object_initialize_child(OBJECT(sms), "USART2", p4, TYPE_STM32F0_USART);
	qdev_prop_set_chr(DEVICE(p4), "chardev", qemu_chr_new("soc-usart2", "chardev:usart2", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p4), &error_fatal);
	p4->base = 0x40004400;
	sysbus_connect_irq(SYS_BUS_DEVICE(p4), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 28));
	sysbus_mmio_map(SYS_BUS_DEVICE(p4), 0, 0x40004400);

	STM32F0USART *p5 = g_new(STM32F0USART, 1);
	object_initialize_child(OBJECT(sms), "USART3", p5, TYPE_STM32F0_USART);
	qdev_prop_set_chr(DEVICE(p5), "chardev", qemu_chr_new("soc-usart3", "chardev:usart3", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p5), &error_fatal);
	p5->base = 0x40004800;
	sysbus_connect_irq(SYS_BUS_DEVICE(p5), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 29));
	sysbus_mmio_map(SYS_BUS_DEVICE(p5), 0, 0x40004800);

	STM32F0PWR *p6 = g_new(STM32F0PWR, 1);
	object_initialize_child(OBJECT(sms), "PWR", p6, TYPE_STM32F0_PWR);
	sysbus_realize(SYS_BUS_DEVICE(p6), &error_fatal);
	p6->base = 0x40007000;
	sysbus_mmio_map(SYS_BUS_DEVICE(p6), 0, 0x40007000);

	STM32F0RCC *p7 = g_new(STM32F0RCC, 1);
	object_initialize_child(OBJECT(sms), "RCC", p7, TYPE_STM32F0_RCC);
	sysbus_realize(SYS_BUS_DEVICE(p7), &error_fatal);
	p7->base = 0x40021000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p7), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 4));
	sysbus_mmio_map(SYS_BUS_DEVICE(p7), 0, 0x40021000);

	STM32F0SYSCFG *p8 = g_new(STM32F0SYSCFG, 1);
	object_initialize_child(OBJECT(sms), "SYSCFG", p8, TYPE_STM32F0_SYSCFG);
	sysbus_realize(SYS_BUS_DEVICE(p8), &error_fatal);
	p8->base = 0x40010000;
	sysbus_mmio_map(SYS_BUS_DEVICE(p8), 0, 0x40010000);

	STM32F0FLASH *p9 = g_new(STM32F0FLASH, 1);
	object_initialize_child(OBJECT(sms), "Flash", p9, TYPE_STM32F0_FLASH);
	sysbus_realize(SYS_BUS_DEVICE(p9), &error_fatal);
	p9->base = 0x40022000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p9), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 3));
	sysbus_mmio_map(SYS_BUS_DEVICE(p9), 0, 0x40022000);

	STM32F0GPIOA *p10 = g_new(STM32F0GPIOA, 1);
	object_initialize_child(OBJECT(sms), "GPIOA", p10, TYPE_STM32F0_GPIOA);
	sysbus_realize(SYS_BUS_DEVICE(p10), &error_fatal);
	p10->base = 0x48000000;
	sysbus_mmio_map(SYS_BUS_DEVICE(p10), 0, 0x48000000);

	STM32F0GPIOF *p11 = g_new(STM32F0GPIOF, 1);
	object_initialize_child(OBJECT(sms), "GPIOB", p11, TYPE_STM32F0_GPIOF);
	sysbus_realize(SYS_BUS_DEVICE(p11), &error_fatal);
	p11->base = 0x48000400;
	sysbus_mmio_map(SYS_BUS_DEVICE(p11), 0, 0x48000400);

	STM32F0GPIOF *p12 = g_new(STM32F0GPIOF, 1);
	object_initialize_child(OBJECT(sms), "GPIOC", p12, TYPE_STM32F0_GPIOF);
	sysbus_realize(SYS_BUS_DEVICE(p12), &error_fatal);
	p12->base = 0x48000800;
	sysbus_mmio_map(SYS_BUS_DEVICE(p12), 0, 0x48000800);

	STM32F0GPIOF *p13 = g_new(STM32F0GPIOF, 1);
	object_initialize_child(OBJECT(sms), "GPIOD", p13, TYPE_STM32F0_GPIOF);
	sysbus_realize(SYS_BUS_DEVICE(p13), &error_fatal);
	p13->base = 0x48000c00;
	sysbus_mmio_map(SYS_BUS_DEVICE(p13), 0, 0x48000c00);

	STM32F0GPIOF *p14 = g_new(STM32F0GPIOF, 1);
	object_initialize_child(OBJECT(sms), "GPIOE", p14, TYPE_STM32F0_GPIOF);
	sysbus_realize(SYS_BUS_DEVICE(p14), &error_fatal);
	p14->base = 0x48001000;
	sysbus_mmio_map(SYS_BUS_DEVICE(p14), 0, 0x48001000);

	STM32F0GPIOF *p15 = g_new(STM32F0GPIOF, 1);
	object_initialize_child(OBJECT(sms), "GPIOF", p15, TYPE_STM32F0_GPIOF);
	sysbus_realize(SYS_BUS_DEVICE(p15), &error_fatal);
	p15->base = 0x48001400;
	sysbus_mmio_map(SYS_BUS_DEVICE(p15), 0, 0x48001400);

	STM32F0SPI *p16 = g_new(STM32F0SPI, 1);
	object_initialize_child(OBJECT(sms), "SPI1", p16, TYPE_STM32F0_SPI);
	qdev_prop_set_chr(DEVICE(p16), "chardev", qemu_chr_new("soc-spi1", "chardev:spi1", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p16), &error_fatal);
	p16->base = 0x40013000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p16), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 25));
	sysbus_mmio_map(SYS_BUS_DEVICE(p16), 0, 0x40013000);

	STM32F0SPI *p17 = g_new(STM32F0SPI, 1);
	object_initialize_child(OBJECT(sms), "SPI2", p17, TYPE_STM32F0_SPI);
	qdev_prop_set_chr(DEVICE(p17), "chardev", qemu_chr_new("soc-spi2", "chardev:spi2", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p17), &error_fatal);
	p17->base = 0x40003800;
	sysbus_connect_irq(SYS_BUS_DEVICE(p17), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 26));
	sysbus_mmio_map(SYS_BUS_DEVICE(p17), 0, 0x40003800);

	STM32F0I2C *p18 = g_new(STM32F0I2C, 1);
	object_initialize_child(OBJECT(sms), "I2C1", p18, TYPE_STM32F0_I2C);
	qdev_prop_set_chr(DEVICE(p18), "chardev", qemu_chr_new("soc-i2c1", "chardev:i2c1", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p18), &error_fatal);
	p18->base = 0x40005400;
	sysbus_connect_irq(SYS_BUS_DEVICE(p18), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 23));
	sysbus_mmio_map(SYS_BUS_DEVICE(p18), 0, 0x40005400);

	STM32F0I2C *p19 = g_new(STM32F0I2C, 1);
	object_initialize_child(OBJECT(sms), "I2C2", p19, TYPE_STM32F0_I2C);
	qdev_prop_set_chr(DEVICE(p19), "chardev", qemu_chr_new("soc-i2c2", "chardev:i2c2", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p19), &error_fatal);
	p19->base = 0x40005800;
	sysbus_connect_irq(SYS_BUS_DEVICE(p19), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 24));
	sysbus_mmio_map(SYS_BUS_DEVICE(p19), 0, 0x40005800);


}

static void stm32f072_common_init(MachineState *machine) {
	STM32F072MachineState *sms = STM32F072_MACHINE(machine);
	MemoryRegion *sysmem = get_system_memory();
	MemoryRegion *mem;
	Clock *cpuclk = clock_new(OBJECT(sms), "SYSCLK");
	clock_set_hz(cpuclk, 48000000);
	Clock *refclk = clock_new(OBJECT(sms), "REFCLK");
	clock_set_hz(refclk, 48000000);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_ram(mem, NULL, "wtf", 0x2000, &error_fatal);
	memory_region_add_subregion(sysmem, 0x0, mem);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_rom(mem, NULL, "main-flash", 0x80000, &error_fatal);
	memory_region_add_subregion(sysmem, 0x8000000, mem);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_rom(mem, NULL, "option-bytes", 0x3800, &error_fatal);
	memory_region_add_subregion(sysmem, 0x1fffc800, mem);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_ram(mem, NULL, "sram", 0x40000, &error_fatal);
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

	stm32f072_periph_init(machine);

	armv7m_load_kernel(ARM_CPU(first_cpu), machine->kernel_filename, 0x8000000, 0x80000);
}

static void stm32f072_class_init(ObjectClass *oc, void *data) {
	MachineClass *mc = MACHINE_CLASS(oc);
	mc->desc = "stm32f072";
	mc->init = stm32f072_common_init;
	mc->max_cpus = 1;
	mc->default_cpus = 1;
	mc->default_cpu_type = ARM_CPU_TYPE_NAME("cortex-m0");
}

static const TypeInfo stm32f072_info = {
	.name = TYPE_STM32F072_MACHINE,
	.parent = TYPE_MACHINE,
	.instance_size = sizeof(STM32F072MachineState),
	.class_size = sizeof(STM32F072MachineClass),
	.class_init = stm32f072_class_init
};

static void stm32f072_machine_init(void) {
	type_register_static(&stm32f072_info);
}

type_init(stm32f072_machine_init);
