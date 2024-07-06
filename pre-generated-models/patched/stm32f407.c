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


#define TYPE_STM32F4_DMA "stm32f4-dma"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F4DMA, STM32F4_DMA)

struct STM32F4DMA {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[8];

	/*registers*/
	uint32_t LISR;
	uint32_t HISR;
	uint32_t LIFCR;
	uint32_t HIFCR;
	uint32_t S0CR;
	uint32_t S0NDTR;
	uint32_t S0PAR;
	uint32_t S0M0AR;
	uint32_t S0M1AR;
	uint32_t S0FCR;
	uint32_t S1CR;
	uint32_t S1NDTR;
	uint32_t S1PAR;
	uint32_t S1M0AR;
	uint32_t S1M1AR;
	uint32_t S1FCR;
	uint32_t S2CR;
	uint32_t S2NDTR;
	uint32_t S2PAR;
	uint32_t S2M0AR;
	uint32_t S2M1AR;
	uint32_t S2FCR;
	uint32_t S3CR;
	uint32_t S3NDTR;
	uint32_t S3PAR;
	uint32_t S3M0AR;
	uint32_t S3M1AR;
	uint32_t S3FCR;
	uint32_t S4CR;
	uint32_t S4NDTR;
	uint32_t S4PAR;
	uint32_t S4M0AR;
	uint32_t S4M1AR;
	uint32_t S4FCR;
	uint32_t S5CR;
	uint32_t S5NDTR;
	uint32_t S5PAR;
	uint32_t S5M0AR;
	uint32_t S5M1AR;
	uint32_t S5FCR;
	uint32_t S6CR;
	uint32_t S6NDTR;
	uint32_t S6PAR;
	uint32_t S6M0AR;
	uint32_t S6M1AR;
	uint32_t S6FCR;
	uint32_t S7CR;
	uint32_t S7NDTR;
	uint32_t S7PAR;
	uint32_t S7M0AR;
	uint32_t S7M1AR;
	uint32_t S7FCR;
	/* base */
	uint32_t base;

};
#define STM32F4_DMA_SIZE				0xd0

REG32(DMA_LISR, 0x0)
	FIELD(DMA_LISR, TCIF3, 27, 1)
	FIELD(DMA_LISR, HTIF3, 26, 1)
	FIELD(DMA_LISR, TEIF3, 25, 1)
	FIELD(DMA_LISR, DMEIF3, 24, 1)
	FIELD(DMA_LISR, FEIF3, 22, 1)
	FIELD(DMA_LISR, TCIF2, 21, 1)
	FIELD(DMA_LISR, HTIF2, 20, 1)
	FIELD(DMA_LISR, TEIF2, 19, 1)
	FIELD(DMA_LISR, DMEIF2, 18, 1)
	FIELD(DMA_LISR, FEIF2, 16, 1)
	FIELD(DMA_LISR, TCIF1, 11, 1)
	FIELD(DMA_LISR, HTIF1, 10, 1)
	FIELD(DMA_LISR, TEIF1, 9, 1)
	FIELD(DMA_LISR, DMEIF1, 8, 1)
	FIELD(DMA_LISR, FEIF1, 6, 1)
	FIELD(DMA_LISR, TCIF0, 5, 1)
	FIELD(DMA_LISR, HTIF0, 4, 1)
	FIELD(DMA_LISR, TEIF0, 3, 1)
	FIELD(DMA_LISR, DMEIF0, 2, 1)
	FIELD(DMA_LISR, FEIF0, 0, 1)
REG32(DMA_HISR, 0x4)
	FIELD(DMA_HISR, TCIF7, 27, 1)
	FIELD(DMA_HISR, HTIF7, 26, 1)
	FIELD(DMA_HISR, TEIF7, 25, 1)
	FIELD(DMA_HISR, DMEIF7, 24, 1)
	FIELD(DMA_HISR, FEIF7, 22, 1)
	FIELD(DMA_HISR, TCIF6, 21, 1)
	FIELD(DMA_HISR, HTIF6, 20, 1)
	FIELD(DMA_HISR, TEIF6, 19, 1)
	FIELD(DMA_HISR, DMEIF6, 18, 1)
	FIELD(DMA_HISR, FEIF6, 16, 1)
	FIELD(DMA_HISR, TCIF5, 11, 1)
	FIELD(DMA_HISR, HTIF5, 10, 1)
	FIELD(DMA_HISR, TEIF5, 9, 1)
	FIELD(DMA_HISR, DMEIF5, 8, 1)
	FIELD(DMA_HISR, FEIF5, 6, 1)
	FIELD(DMA_HISR, TCIF4, 5, 1)
	FIELD(DMA_HISR, HTIF4, 4, 1)
	FIELD(DMA_HISR, TEIF4, 3, 1)
	FIELD(DMA_HISR, DMEIF4, 2, 1)
	FIELD(DMA_HISR, FEIF4, 0, 1)
REG32(DMA_LIFCR, 0x8)
	FIELD(DMA_LIFCR, CTCIF3, 27, 1)
	FIELD(DMA_LIFCR, CHTIF3, 26, 1)
	FIELD(DMA_LIFCR, CTEIF3, 25, 1)
	FIELD(DMA_LIFCR, CDMEIF3, 24, 1)
	FIELD(DMA_LIFCR, CFEIF3, 22, 1)
	FIELD(DMA_LIFCR, CTCIF2, 21, 1)
	FIELD(DMA_LIFCR, CHTIF2, 20, 1)
	FIELD(DMA_LIFCR, CTEIF2, 19, 1)
	FIELD(DMA_LIFCR, CDMEIF2, 18, 1)
	FIELD(DMA_LIFCR, CFEIF2, 16, 1)
	FIELD(DMA_LIFCR, CTCIF1, 11, 1)
	FIELD(DMA_LIFCR, CHTIF1, 10, 1)
	FIELD(DMA_LIFCR, CTEIF1, 9, 1)
	FIELD(DMA_LIFCR, CDMEIF1, 8, 1)
	FIELD(DMA_LIFCR, CFEIF1, 6, 1)
	FIELD(DMA_LIFCR, CTCIF0, 5, 1)
	FIELD(DMA_LIFCR, CHTIF0, 4, 1)
	FIELD(DMA_LIFCR, CTEIF0, 3, 1)
	FIELD(DMA_LIFCR, CDMEIF0, 2, 1)
	FIELD(DMA_LIFCR, CFEIF0, 0, 1)
REG32(DMA_HIFCR, 0xc)
	FIELD(DMA_HIFCR, CTCIF7, 27, 1)
	FIELD(DMA_HIFCR, CHTIF7, 26, 1)
	FIELD(DMA_HIFCR, CTEIF7, 25, 1)
	FIELD(DMA_HIFCR, CDMEIF7, 24, 1)
	FIELD(DMA_HIFCR, CFEIF7, 22, 1)
	FIELD(DMA_HIFCR, CTCIF6, 21, 1)
	FIELD(DMA_HIFCR, CHTIF6, 20, 1)
	FIELD(DMA_HIFCR, CTEIF6, 19, 1)
	FIELD(DMA_HIFCR, CDMEIF6, 18, 1)
	FIELD(DMA_HIFCR, CFEIF6, 16, 1)
	FIELD(DMA_HIFCR, CTCIF5, 11, 1)
	FIELD(DMA_HIFCR, CHTIF5, 10, 1)
	FIELD(DMA_HIFCR, CTEIF5, 9, 1)
	FIELD(DMA_HIFCR, CDMEIF5, 8, 1)
	FIELD(DMA_HIFCR, CFEIF5, 6, 1)
	FIELD(DMA_HIFCR, CTCIF4, 5, 1)
	FIELD(DMA_HIFCR, CHTIF4, 4, 1)
	FIELD(DMA_HIFCR, CTEIF4, 3, 1)
	FIELD(DMA_HIFCR, CDMEIF4, 2, 1)
	FIELD(DMA_HIFCR, CFEIF4, 0, 1)
REG32(DMA_S0CR, 0x10)
	FIELD(DMA_S0CR, CHSEL, 25, 3)
	FIELD(DMA_S0CR, MBURST, 23, 2)
	FIELD(DMA_S0CR, PBURST, 21, 2)
	FIELD(DMA_S0CR, CT, 19, 1)
	FIELD(DMA_S0CR, DBM, 18, 1)
	FIELD(DMA_S0CR, PL, 16, 2)
	FIELD(DMA_S0CR, PINCOS, 15, 1)
	FIELD(DMA_S0CR, MSIZE, 13, 2)
	FIELD(DMA_S0CR, PSIZE, 11, 2)
	FIELD(DMA_S0CR, MINC, 10, 1)
	FIELD(DMA_S0CR, PINC, 9, 1)
	FIELD(DMA_S0CR, CIRC, 8, 1)
	FIELD(DMA_S0CR, DIR, 6, 2)
	FIELD(DMA_S0CR, PFCTRL, 5, 1)
	FIELD(DMA_S0CR, TCIE, 4, 1)
	FIELD(DMA_S0CR, HTIE, 3, 1)
	FIELD(DMA_S0CR, TEIE, 2, 1)
	FIELD(DMA_S0CR, DMEIE, 1, 1)
	FIELD(DMA_S0CR, EN, 0, 1)
REG32(DMA_S0NDTR, 0x14)
	FIELD(DMA_S0NDTR, NDT, 0, 16)
REG32(DMA_S0PAR, 0x18)
	FIELD(DMA_S0PAR, PA, 0, 32)
REG32(DMA_S0M0AR, 0x1c)
	FIELD(DMA_S0M0AR, M0A, 0, 32)
REG32(DMA_S0M1AR, 0x20)
	FIELD(DMA_S0M1AR, M1A, 0, 32)
REG32(DMA_S0FCR, 0x24)
	FIELD(DMA_S0FCR, FEIE, 7, 1)
	FIELD(DMA_S0FCR, FS, 3, 3)
	FIELD(DMA_S0FCR, DMDIS, 2, 1)
	FIELD(DMA_S0FCR, FTH, 0, 2)
REG32(DMA_S1CR, 0x28)
	FIELD(DMA_S1CR, CHSEL, 25, 3)
	FIELD(DMA_S1CR, MBURST, 23, 2)
	FIELD(DMA_S1CR, PBURST, 21, 2)
	FIELD(DMA_S1CR, ACK, 20, 1)
	FIELD(DMA_S1CR, CT, 19, 1)
	FIELD(DMA_S1CR, DBM, 18, 1)
	FIELD(DMA_S1CR, PL, 16, 2)
	FIELD(DMA_S1CR, PINCOS, 15, 1)
	FIELD(DMA_S1CR, MSIZE, 13, 2)
	FIELD(DMA_S1CR, PSIZE, 11, 2)
	FIELD(DMA_S1CR, MINC, 10, 1)
	FIELD(DMA_S1CR, PINC, 9, 1)
	FIELD(DMA_S1CR, CIRC, 8, 1)
	FIELD(DMA_S1CR, DIR, 6, 2)
	FIELD(DMA_S1CR, PFCTRL, 5, 1)
	FIELD(DMA_S1CR, TCIE, 4, 1)
	FIELD(DMA_S1CR, HTIE, 3, 1)
	FIELD(DMA_S1CR, TEIE, 2, 1)
	FIELD(DMA_S1CR, DMEIE, 1, 1)
	FIELD(DMA_S1CR, EN, 0, 1)
REG32(DMA_S1NDTR, 0x2c)
	FIELD(DMA_S1NDTR, NDT, 0, 16)
REG32(DMA_S1PAR, 0x30)
	FIELD(DMA_S1PAR, PA, 0, 32)
REG32(DMA_S1M0AR, 0x34)
	FIELD(DMA_S1M0AR, M0A, 0, 32)
REG32(DMA_S1M1AR, 0x38)
	FIELD(DMA_S1M1AR, M1A, 0, 32)
REG32(DMA_S1FCR, 0x3c)
	FIELD(DMA_S1FCR, FEIE, 7, 1)
	FIELD(DMA_S1FCR, FS, 3, 3)
	FIELD(DMA_S1FCR, DMDIS, 2, 1)
	FIELD(DMA_S1FCR, FTH, 0, 2)
REG32(DMA_S2CR, 0x40)
	FIELD(DMA_S2CR, CHSEL, 25, 3)
	FIELD(DMA_S2CR, MBURST, 23, 2)
	FIELD(DMA_S2CR, PBURST, 21, 2)
	FIELD(DMA_S2CR, ACK, 20, 1)
	FIELD(DMA_S2CR, CT, 19, 1)
	FIELD(DMA_S2CR, DBM, 18, 1)
	FIELD(DMA_S2CR, PL, 16, 2)
	FIELD(DMA_S2CR, PINCOS, 15, 1)
	FIELD(DMA_S2CR, MSIZE, 13, 2)
	FIELD(DMA_S2CR, PSIZE, 11, 2)
	FIELD(DMA_S2CR, MINC, 10, 1)
	FIELD(DMA_S2CR, PINC, 9, 1)
	FIELD(DMA_S2CR, CIRC, 8, 1)
	FIELD(DMA_S2CR, DIR, 6, 2)
	FIELD(DMA_S2CR, PFCTRL, 5, 1)
	FIELD(DMA_S2CR, TCIE, 4, 1)
	FIELD(DMA_S2CR, HTIE, 3, 1)
	FIELD(DMA_S2CR, TEIE, 2, 1)
	FIELD(DMA_S2CR, DMEIE, 1, 1)
	FIELD(DMA_S2CR, EN, 0, 1)
REG32(DMA_S2NDTR, 0x44)
	FIELD(DMA_S2NDTR, NDT, 0, 16)
REG32(DMA_S2PAR, 0x48)
	FIELD(DMA_S2PAR, PA, 0, 32)
REG32(DMA_S2M0AR, 0x4c)
	FIELD(DMA_S2M0AR, M0A, 0, 32)
REG32(DMA_S2M1AR, 0x50)
	FIELD(DMA_S2M1AR, M1A, 0, 32)
REG32(DMA_S2FCR, 0x54)
	FIELD(DMA_S2FCR, FEIE, 7, 1)
	FIELD(DMA_S2FCR, FS, 3, 3)
	FIELD(DMA_S2FCR, DMDIS, 2, 1)
	FIELD(DMA_S2FCR, FTH, 0, 2)
REG32(DMA_S3CR, 0x58)
	FIELD(DMA_S3CR, CHSEL, 25, 3)
	FIELD(DMA_S3CR, MBURST, 23, 2)
	FIELD(DMA_S3CR, PBURST, 21, 2)
	FIELD(DMA_S3CR, ACK, 20, 1)
	FIELD(DMA_S3CR, CT, 19, 1)
	FIELD(DMA_S3CR, DBM, 18, 1)
	FIELD(DMA_S3CR, PL, 16, 2)
	FIELD(DMA_S3CR, PINCOS, 15, 1)
	FIELD(DMA_S3CR, MSIZE, 13, 2)
	FIELD(DMA_S3CR, PSIZE, 11, 2)
	FIELD(DMA_S3CR, MINC, 10, 1)
	FIELD(DMA_S3CR, PINC, 9, 1)
	FIELD(DMA_S3CR, CIRC, 8, 1)
	FIELD(DMA_S3CR, DIR, 6, 2)
	FIELD(DMA_S3CR, PFCTRL, 5, 1)
	FIELD(DMA_S3CR, TCIE, 4, 1)
	FIELD(DMA_S3CR, HTIE, 3, 1)
	FIELD(DMA_S3CR, TEIE, 2, 1)
	FIELD(DMA_S3CR, DMEIE, 1, 1)
	FIELD(DMA_S3CR, EN, 0, 1)
REG32(DMA_S3NDTR, 0x5c)
	FIELD(DMA_S3NDTR, NDT, 0, 16)
REG32(DMA_S3PAR, 0x60)
	FIELD(DMA_S3PAR, PA, 0, 32)
REG32(DMA_S3M0AR, 0x64)
	FIELD(DMA_S3M0AR, M0A, 0, 32)
REG32(DMA_S3M1AR, 0x68)
	FIELD(DMA_S3M1AR, M1A, 0, 32)
REG32(DMA_S3FCR, 0x6c)
	FIELD(DMA_S3FCR, FEIE, 7, 1)
	FIELD(DMA_S3FCR, FS, 3, 3)
	FIELD(DMA_S3FCR, DMDIS, 2, 1)
	FIELD(DMA_S3FCR, FTH, 0, 2)
REG32(DMA_S4CR, 0x70)
	FIELD(DMA_S4CR, CHSEL, 25, 3)
	FIELD(DMA_S4CR, MBURST, 23, 2)
	FIELD(DMA_S4CR, PBURST, 21, 2)
	FIELD(DMA_S4CR, ACK, 20, 1)
	FIELD(DMA_S4CR, CT, 19, 1)
	FIELD(DMA_S4CR, DBM, 18, 1)
	FIELD(DMA_S4CR, PL, 16, 2)
	FIELD(DMA_S4CR, PINCOS, 15, 1)
	FIELD(DMA_S4CR, MSIZE, 13, 2)
	FIELD(DMA_S4CR, PSIZE, 11, 2)
	FIELD(DMA_S4CR, MINC, 10, 1)
	FIELD(DMA_S4CR, PINC, 9, 1)
	FIELD(DMA_S4CR, CIRC, 8, 1)
	FIELD(DMA_S4CR, DIR, 6, 2)
	FIELD(DMA_S4CR, PFCTRL, 5, 1)
	FIELD(DMA_S4CR, TCIE, 4, 1)
	FIELD(DMA_S4CR, HTIE, 3, 1)
	FIELD(DMA_S4CR, TEIE, 2, 1)
	FIELD(DMA_S4CR, DMEIE, 1, 1)
	FIELD(DMA_S4CR, EN, 0, 1)
REG32(DMA_S4NDTR, 0x74)
	FIELD(DMA_S4NDTR, NDT, 0, 16)
REG32(DMA_S4PAR, 0x78)
	FIELD(DMA_S4PAR, PA, 0, 32)
REG32(DMA_S4M0AR, 0x7c)
	FIELD(DMA_S4M0AR, M0A, 0, 32)
REG32(DMA_S4M1AR, 0x80)
	FIELD(DMA_S4M1AR, M1A, 0, 32)
REG32(DMA_S4FCR, 0x84)
	FIELD(DMA_S4FCR, FEIE, 7, 1)
	FIELD(DMA_S4FCR, FS, 3, 3)
	FIELD(DMA_S4FCR, DMDIS, 2, 1)
	FIELD(DMA_S4FCR, FTH, 0, 2)
REG32(DMA_S5CR, 0x88)
	FIELD(DMA_S5CR, CHSEL, 25, 3)
	FIELD(DMA_S5CR, MBURST, 23, 2)
	FIELD(DMA_S5CR, PBURST, 21, 2)
	FIELD(DMA_S5CR, ACK, 20, 1)
	FIELD(DMA_S5CR, CT, 19, 1)
	FIELD(DMA_S5CR, DBM, 18, 1)
	FIELD(DMA_S5CR, PL, 16, 2)
	FIELD(DMA_S5CR, PINCOS, 15, 1)
	FIELD(DMA_S5CR, MSIZE, 13, 2)
	FIELD(DMA_S5CR, PSIZE, 11, 2)
	FIELD(DMA_S5CR, MINC, 10, 1)
	FIELD(DMA_S5CR, PINC, 9, 1)
	FIELD(DMA_S5CR, CIRC, 8, 1)
	FIELD(DMA_S5CR, DIR, 6, 2)
	FIELD(DMA_S5CR, PFCTRL, 5, 1)
	FIELD(DMA_S5CR, TCIE, 4, 1)
	FIELD(DMA_S5CR, HTIE, 3, 1)
	FIELD(DMA_S5CR, TEIE, 2, 1)
	FIELD(DMA_S5CR, DMEIE, 1, 1)
	FIELD(DMA_S5CR, EN, 0, 1)
REG32(DMA_S5NDTR, 0x8c)
	FIELD(DMA_S5NDTR, NDT, 0, 16)
REG32(DMA_S5PAR, 0x90)
	FIELD(DMA_S5PAR, PA, 0, 32)
REG32(DMA_S5M0AR, 0x94)
	FIELD(DMA_S5M0AR, M0A, 0, 32)
REG32(DMA_S5M1AR, 0x98)
	FIELD(DMA_S5M1AR, M1A, 0, 32)
REG32(DMA_S5FCR, 0x9c)
	FIELD(DMA_S5FCR, FEIE, 7, 1)
	FIELD(DMA_S5FCR, FS, 3, 3)
	FIELD(DMA_S5FCR, DMDIS, 2, 1)
	FIELD(DMA_S5FCR, FTH, 0, 2)
REG32(DMA_S6CR, 0xa0)
	FIELD(DMA_S6CR, CHSEL, 25, 3)
	FIELD(DMA_S6CR, MBURST, 23, 2)
	FIELD(DMA_S6CR, PBURST, 21, 2)
	FIELD(DMA_S6CR, ACK, 20, 1)
	FIELD(DMA_S6CR, CT, 19, 1)
	FIELD(DMA_S6CR, DBM, 18, 1)
	FIELD(DMA_S6CR, PL, 16, 2)
	FIELD(DMA_S6CR, PINCOS, 15, 1)
	FIELD(DMA_S6CR, MSIZE, 13, 2)
	FIELD(DMA_S6CR, PSIZE, 11, 2)
	FIELD(DMA_S6CR, MINC, 10, 1)
	FIELD(DMA_S6CR, PINC, 9, 1)
	FIELD(DMA_S6CR, CIRC, 8, 1)
	FIELD(DMA_S6CR, DIR, 6, 2)
	FIELD(DMA_S6CR, PFCTRL, 5, 1)
	FIELD(DMA_S6CR, TCIE, 4, 1)
	FIELD(DMA_S6CR, HTIE, 3, 1)
	FIELD(DMA_S6CR, TEIE, 2, 1)
	FIELD(DMA_S6CR, DMEIE, 1, 1)
	FIELD(DMA_S6CR, EN, 0, 1)
REG32(DMA_S6NDTR, 0xa4)
	FIELD(DMA_S6NDTR, NDT, 0, 16)
REG32(DMA_S6PAR, 0xa8)
	FIELD(DMA_S6PAR, PA, 0, 32)
REG32(DMA_S6M0AR, 0xac)
	FIELD(DMA_S6M0AR, M0A, 0, 32)
REG32(DMA_S6M1AR, 0xb0)
	FIELD(DMA_S6M1AR, M1A, 0, 32)
REG32(DMA_S6FCR, 0xb4)
	FIELD(DMA_S6FCR, FEIE, 7, 1)
	FIELD(DMA_S6FCR, FS, 3, 3)
	FIELD(DMA_S6FCR, DMDIS, 2, 1)
	FIELD(DMA_S6FCR, FTH, 0, 2)
REG32(DMA_S7CR, 0xb8)
	FIELD(DMA_S7CR, CHSEL, 25, 3)
	FIELD(DMA_S7CR, MBURST, 23, 2)
	FIELD(DMA_S7CR, PBURST, 21, 2)
	FIELD(DMA_S7CR, ACK, 20, 1)
	FIELD(DMA_S7CR, CT, 19, 1)
	FIELD(DMA_S7CR, DBM, 18, 1)
	FIELD(DMA_S7CR, PL, 16, 2)
	FIELD(DMA_S7CR, PINCOS, 15, 1)
	FIELD(DMA_S7CR, MSIZE, 13, 2)
	FIELD(DMA_S7CR, PSIZE, 11, 2)
	FIELD(DMA_S7CR, MINC, 10, 1)
	FIELD(DMA_S7CR, PINC, 9, 1)
	FIELD(DMA_S7CR, CIRC, 8, 1)
	FIELD(DMA_S7CR, DIR, 6, 2)
	FIELD(DMA_S7CR, PFCTRL, 5, 1)
	FIELD(DMA_S7CR, TCIE, 4, 1)
	FIELD(DMA_S7CR, HTIE, 3, 1)
	FIELD(DMA_S7CR, TEIE, 2, 1)
	FIELD(DMA_S7CR, DMEIE, 1, 1)
	FIELD(DMA_S7CR, EN, 0, 1)
REG32(DMA_S7NDTR, 0xbc)
	FIELD(DMA_S7NDTR, NDT, 0, 16)
REG32(DMA_S7PAR, 0xc0)
	FIELD(DMA_S7PAR, PA, 0, 32)
REG32(DMA_S7M0AR, 0xc4)
	FIELD(DMA_S7M0AR, M0A, 0, 32)
REG32(DMA_S7M1AR, 0xc8)
	FIELD(DMA_S7M1AR, M1A, 0, 32)
REG32(DMA_S7FCR, 0xcc)
	FIELD(DMA_S7FCR, FEIE, 7, 1)
	FIELD(DMA_S7FCR, FS, 3, 3)
	FIELD(DMA_S7FCR, DMDIS, 2, 1)
	FIELD(DMA_S7FCR, FTH, 0, 2)


static void stm32f4_dma_register_reset(STM32F4DMA *t) {
	t->LISR = 0x0;
	t->HISR = 0x0;
	t->LIFCR = 0x0;
	t->HIFCR = 0x0;
	t->S0CR = 0x0;
	t->S0NDTR = 0x0;
	t->S0PAR = 0x0;
	t->S0M0AR = 0x0;
	t->S0M1AR = 0x0;
	t->S0FCR = 0x21;
	t->S1CR = 0x0;
	t->S1NDTR = 0x0;
	t->S1PAR = 0x0;
	t->S1M0AR = 0x0;
	t->S1M1AR = 0x0;
	t->S1FCR = 0x21;
	t->S2CR = 0x0;
	t->S2NDTR = 0x0;
	t->S2PAR = 0x0;
	t->S2M0AR = 0x0;
	t->S2M1AR = 0x0;
	t->S2FCR = 0x21;
	t->S3CR = 0x0;
	t->S3NDTR = 0x0;
	t->S3PAR = 0x0;
	t->S3M0AR = 0x0;
	t->S3M1AR = 0x0;
	t->S3FCR = 0x21;
	t->S4CR = 0x0;
	t->S4NDTR = 0x0;
	t->S4PAR = 0x0;
	t->S4M0AR = 0x0;
	t->S4M1AR = 0x0;
	t->S4FCR = 0x21;
	t->S5CR = 0x0;
	t->S5NDTR = 0x0;
	t->S5PAR = 0x0;
	t->S5M0AR = 0x0;
	t->S5M1AR = 0x0;
	t->S5FCR = 0x21;
	t->S6CR = 0x0;
	t->S6NDTR = 0x0;
	t->S6PAR = 0x0;
	t->S6M0AR = 0x0;
	t->S6M1AR = 0x0;
	t->S6FCR = 0x21;
	t->S7CR = 0x0;
	t->S7NDTR = 0x0;
	t->S7PAR = 0x0;
	t->S7M0AR = 0x0;
	t->S7M1AR = 0x0;
	t->S7FCR = 0x21;

}

static uint64_t stm32f4_dma_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F4DMA *t = STM32F4_DMA(opaque);
	uint64_t ret;

	switch (offset) {
		case A_DMA_LISR:
			ret = t->LISR;
			break;
		case A_DMA_HISR:
			ret = t->HISR;
			break;
		case A_DMA_LIFCR:
			ret = t->LIFCR;
			break;
		case A_DMA_HIFCR:
			ret = t->HIFCR;
			break;
		case A_DMA_S0CR:
			ret = t->S0CR;
			break;
		case A_DMA_S0NDTR:
			ret = t->S0NDTR;
			break;
		case A_DMA_S0PAR:
			ret = t->S0PAR;
			break;
		case A_DMA_S0M0AR:
			ret = t->S0M0AR;
			break;
		case A_DMA_S0M1AR:
			ret = t->S0M1AR;
			break;
		case A_DMA_S0FCR:
			ret = t->S0FCR;
			break;
		case A_DMA_S1CR:
			ret = t->S1CR;
			break;
		case A_DMA_S1NDTR:
			ret = t->S1NDTR;
			break;
		case A_DMA_S1PAR:
			ret = t->S1PAR;
			break;
		case A_DMA_S1M0AR:
			ret = t->S1M0AR;
			break;
		case A_DMA_S1M1AR:
			ret = t->S1M1AR;
			break;
		case A_DMA_S1FCR:
			ret = t->S1FCR;
			break;
		case A_DMA_S2CR:
			ret = t->S2CR;
			break;
		case A_DMA_S2NDTR:
			ret = t->S2NDTR;
			break;
		case A_DMA_S2PAR:
			ret = t->S2PAR;
			break;
		case A_DMA_S2M0AR:
			ret = t->S2M0AR;
			break;
		case A_DMA_S2M1AR:
			ret = t->S2M1AR;
			break;
		case A_DMA_S2FCR:
			ret = t->S2FCR;
			break;
		case A_DMA_S3CR:
			ret = t->S3CR;
			break;
		case A_DMA_S3NDTR:
			ret = t->S3NDTR;
			break;
		case A_DMA_S3PAR:
			ret = t->S3PAR;
			break;
		case A_DMA_S3M0AR:
			ret = t->S3M0AR;
			break;
		case A_DMA_S3M1AR:
			ret = t->S3M1AR;
			break;
		case A_DMA_S3FCR:
			ret = t->S3FCR;
			break;
		case A_DMA_S4CR:
			ret = t->S4CR;
			break;
		case A_DMA_S4NDTR:
			ret = t->S4NDTR;
			break;
		case A_DMA_S4PAR:
			ret = t->S4PAR;
			break;
		case A_DMA_S4M0AR:
			ret = t->S4M0AR;
			break;
		case A_DMA_S4M1AR:
			ret = t->S4M1AR;
			break;
		case A_DMA_S4FCR:
			ret = t->S4FCR;
			break;
		case A_DMA_S5CR:
			ret = t->S5CR;
			break;
		case A_DMA_S5NDTR:
			ret = t->S5NDTR;
			break;
		case A_DMA_S5PAR:
			ret = t->S5PAR;
			break;
		case A_DMA_S5M0AR:
			ret = t->S5M0AR;
			break;
		case A_DMA_S5M1AR:
			ret = t->S5M1AR;
			break;
		case A_DMA_S5FCR:
			ret = t->S5FCR;
			break;
		case A_DMA_S6CR:
			ret = t->S6CR;
			break;
		case A_DMA_S6NDTR:
			ret = t->S6NDTR;
			break;
		case A_DMA_S6PAR:
			ret = t->S6PAR;
			break;
		case A_DMA_S6M0AR:
			ret = t->S6M0AR;
			break;
		case A_DMA_S6M1AR:
			ret = t->S6M1AR;
			break;
		case A_DMA_S6FCR:
			ret = t->S6FCR;
			break;
		case A_DMA_S7CR:
			ret = t->S7CR;
			break;
		case A_DMA_S7NDTR:
			ret = t->S7NDTR;
			break;
		case A_DMA_S7PAR:
			ret = t->S7PAR;
			break;
		case A_DMA_S7M0AR:
			ret = t->S7M0AR;
			break;
		case A_DMA_S7M1AR:
			ret = t->S7M1AR;
			break;
		case A_DMA_S7FCR:
			ret = t->S7FCR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F4 DMA read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f4_dma_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F4DMA *t = STM32F4_DMA(opaque);


	switch (offset) {
		case A_DMA_LIFCR ... A_DMA_LIFCR + 3:
			value = value << ((offset - A_DMA_LIFCR) << 3);
			offset = A_DMA_LIFCR;
			break;
		case A_DMA_HIFCR ... A_DMA_HIFCR + 3:
			value = value << ((offset - A_DMA_HIFCR) << 3);
			offset = A_DMA_HIFCR;
			break;
		case A_DMA_S0CR ... A_DMA_S0CR + 3:
			value = value << ((offset - A_DMA_S0CR) << 3);
			offset = A_DMA_S0CR;
			break;
		case A_DMA_S0NDTR ... A_DMA_S0NDTR + 3:
			value = value << ((offset - A_DMA_S0NDTR) << 3);
			offset = A_DMA_S0NDTR;
			break;
		case A_DMA_S0PAR ... A_DMA_S0PAR + 3:
			value = value << ((offset - A_DMA_S0PAR) << 3);
			offset = A_DMA_S0PAR;
			break;
		case A_DMA_S0M0AR ... A_DMA_S0M0AR + 3:
			value = value << ((offset - A_DMA_S0M0AR) << 3);
			offset = A_DMA_S0M0AR;
			break;
		case A_DMA_S0M1AR ... A_DMA_S0M1AR + 3:
			value = value << ((offset - A_DMA_S0M1AR) << 3);
			offset = A_DMA_S0M1AR;
			break;
		case A_DMA_S0FCR ... A_DMA_S0FCR + 3:
			value = value << ((offset - A_DMA_S0FCR) << 3);
			offset = A_DMA_S0FCR;
			break;
		case A_DMA_S1CR ... A_DMA_S1CR + 3:
			value = value << ((offset - A_DMA_S1CR) << 3);
			offset = A_DMA_S1CR;
			break;
		case A_DMA_S1NDTR ... A_DMA_S1NDTR + 3:
			value = value << ((offset - A_DMA_S1NDTR) << 3);
			offset = A_DMA_S1NDTR;
			break;
		case A_DMA_S1PAR ... A_DMA_S1PAR + 3:
			value = value << ((offset - A_DMA_S1PAR) << 3);
			offset = A_DMA_S1PAR;
			break;
		case A_DMA_S1M0AR ... A_DMA_S1M0AR + 3:
			value = value << ((offset - A_DMA_S1M0AR) << 3);
			offset = A_DMA_S1M0AR;
			break;
		case A_DMA_S1M1AR ... A_DMA_S1M1AR + 3:
			value = value << ((offset - A_DMA_S1M1AR) << 3);
			offset = A_DMA_S1M1AR;
			break;
		case A_DMA_S1FCR ... A_DMA_S1FCR + 3:
			value = value << ((offset - A_DMA_S1FCR) << 3);
			offset = A_DMA_S1FCR;
			break;
		case A_DMA_S2CR ... A_DMA_S2CR + 3:
			value = value << ((offset - A_DMA_S2CR) << 3);
			offset = A_DMA_S2CR;
			break;
		case A_DMA_S2NDTR ... A_DMA_S2NDTR + 3:
			value = value << ((offset - A_DMA_S2NDTR) << 3);
			offset = A_DMA_S2NDTR;
			break;
		case A_DMA_S2PAR ... A_DMA_S2PAR + 3:
			value = value << ((offset - A_DMA_S2PAR) << 3);
			offset = A_DMA_S2PAR;
			break;
		case A_DMA_S2M0AR ... A_DMA_S2M0AR + 3:
			value = value << ((offset - A_DMA_S2M0AR) << 3);
			offset = A_DMA_S2M0AR;
			break;
		case A_DMA_S2M1AR ... A_DMA_S2M1AR + 3:
			value = value << ((offset - A_DMA_S2M1AR) << 3);
			offset = A_DMA_S2M1AR;
			break;
		case A_DMA_S2FCR ... A_DMA_S2FCR + 3:
			value = value << ((offset - A_DMA_S2FCR) << 3);
			offset = A_DMA_S2FCR;
			break;
		case A_DMA_S3CR ... A_DMA_S3CR + 3:
			value = value << ((offset - A_DMA_S3CR) << 3);
			offset = A_DMA_S3CR;
			break;
		case A_DMA_S3NDTR ... A_DMA_S3NDTR + 3:
			value = value << ((offset - A_DMA_S3NDTR) << 3);
			offset = A_DMA_S3NDTR;
			break;
		case A_DMA_S3PAR ... A_DMA_S3PAR + 3:
			value = value << ((offset - A_DMA_S3PAR) << 3);
			offset = A_DMA_S3PAR;
			break;
		case A_DMA_S3M0AR ... A_DMA_S3M0AR + 3:
			value = value << ((offset - A_DMA_S3M0AR) << 3);
			offset = A_DMA_S3M0AR;
			break;
		case A_DMA_S3M1AR ... A_DMA_S3M1AR + 3:
			value = value << ((offset - A_DMA_S3M1AR) << 3);
			offset = A_DMA_S3M1AR;
			break;
		case A_DMA_S3FCR ... A_DMA_S3FCR + 3:
			value = value << ((offset - A_DMA_S3FCR) << 3);
			offset = A_DMA_S3FCR;
			break;
		case A_DMA_S4CR ... A_DMA_S4CR + 3:
			value = value << ((offset - A_DMA_S4CR) << 3);
			offset = A_DMA_S4CR;
			break;
		case A_DMA_S4NDTR ... A_DMA_S4NDTR + 3:
			value = value << ((offset - A_DMA_S4NDTR) << 3);
			offset = A_DMA_S4NDTR;
			break;
		case A_DMA_S4PAR ... A_DMA_S4PAR + 3:
			value = value << ((offset - A_DMA_S4PAR) << 3);
			offset = A_DMA_S4PAR;
			break;
		case A_DMA_S4M0AR ... A_DMA_S4M0AR + 3:
			value = value << ((offset - A_DMA_S4M0AR) << 3);
			offset = A_DMA_S4M0AR;
			break;
		case A_DMA_S4M1AR ... A_DMA_S4M1AR + 3:
			value = value << ((offset - A_DMA_S4M1AR) << 3);
			offset = A_DMA_S4M1AR;
			break;
		case A_DMA_S4FCR ... A_DMA_S4FCR + 3:
			value = value << ((offset - A_DMA_S4FCR) << 3);
			offset = A_DMA_S4FCR;
			break;
		case A_DMA_S5CR ... A_DMA_S5CR + 3:
			value = value << ((offset - A_DMA_S5CR) << 3);
			offset = A_DMA_S5CR;
			break;
		case A_DMA_S5NDTR ... A_DMA_S5NDTR + 3:
			value = value << ((offset - A_DMA_S5NDTR) << 3);
			offset = A_DMA_S5NDTR;
			break;
		case A_DMA_S5PAR ... A_DMA_S5PAR + 3:
			value = value << ((offset - A_DMA_S5PAR) << 3);
			offset = A_DMA_S5PAR;
			break;
		case A_DMA_S5M0AR ... A_DMA_S5M0AR + 3:
			value = value << ((offset - A_DMA_S5M0AR) << 3);
			offset = A_DMA_S5M0AR;
			break;
		case A_DMA_S5M1AR ... A_DMA_S5M1AR + 3:
			value = value << ((offset - A_DMA_S5M1AR) << 3);
			offset = A_DMA_S5M1AR;
			break;
		case A_DMA_S5FCR ... A_DMA_S5FCR + 3:
			value = value << ((offset - A_DMA_S5FCR) << 3);
			offset = A_DMA_S5FCR;
			break;
		case A_DMA_S6CR ... A_DMA_S6CR + 3:
			value = value << ((offset - A_DMA_S6CR) << 3);
			offset = A_DMA_S6CR;
			break;
		case A_DMA_S6NDTR ... A_DMA_S6NDTR + 3:
			value = value << ((offset - A_DMA_S6NDTR) << 3);
			offset = A_DMA_S6NDTR;
			break;
		case A_DMA_S6PAR ... A_DMA_S6PAR + 3:
			value = value << ((offset - A_DMA_S6PAR) << 3);
			offset = A_DMA_S6PAR;
			break;
		case A_DMA_S6M0AR ... A_DMA_S6M0AR + 3:
			value = value << ((offset - A_DMA_S6M0AR) << 3);
			offset = A_DMA_S6M0AR;
			break;
		case A_DMA_S6M1AR ... A_DMA_S6M1AR + 3:
			value = value << ((offset - A_DMA_S6M1AR) << 3);
			offset = A_DMA_S6M1AR;
			break;
		case A_DMA_S6FCR ... A_DMA_S6FCR + 3:
			value = value << ((offset - A_DMA_S6FCR) << 3);
			offset = A_DMA_S6FCR;
			break;
		case A_DMA_S7CR ... A_DMA_S7CR + 3:
			value = value << ((offset - A_DMA_S7CR) << 3);
			offset = A_DMA_S7CR;
			break;
		case A_DMA_S7NDTR ... A_DMA_S7NDTR + 3:
			value = value << ((offset - A_DMA_S7NDTR) << 3);
			offset = A_DMA_S7NDTR;
			break;
		case A_DMA_S7PAR ... A_DMA_S7PAR + 3:
			value = value << ((offset - A_DMA_S7PAR) << 3);
			offset = A_DMA_S7PAR;
			break;
		case A_DMA_S7M0AR ... A_DMA_S7M0AR + 3:
			value = value << ((offset - A_DMA_S7M0AR) << 3);
			offset = A_DMA_S7M0AR;
			break;
		case A_DMA_S7M1AR ... A_DMA_S7M1AR + 3:
			value = value << ((offset - A_DMA_S7M1AR) << 3);
			offset = A_DMA_S7M1AR;
			break;
		case A_DMA_S7FCR ... A_DMA_S7FCR + 3:
			value = value << ((offset - A_DMA_S7FCR) << 3);
			offset = A_DMA_S7FCR;
			break;

		default: break;
	}

	switch (offset) {
		case A_DMA_LIFCR:
			t->LIFCR = value;
			break;
		case A_DMA_HIFCR:
			t->HIFCR = value;
			break;
		case A_DMA_S0CR:
			t->S0CR = value;
			break;
		case A_DMA_S0NDTR:
			t->S0NDTR = value;
			break;
		case A_DMA_S0PAR:
			t->S0PAR = value;
			break;
		case A_DMA_S0M0AR:
			t->S0M0AR = value;
			break;
		case A_DMA_S0M1AR:
			t->S0M1AR = value;
			break;
		case A_DMA_S0FCR:
			t->S0FCR = value;
			break;
		case A_DMA_S1CR:
			t->S1CR = value;
			break;
		case A_DMA_S1NDTR:
			t->S1NDTR = value;
			break;
		case A_DMA_S1PAR:
			t->S1PAR = value;
			break;
		case A_DMA_S1M0AR:
			t->S1M0AR = value;
			break;
		case A_DMA_S1M1AR:
			t->S1M1AR = value;
			break;
		case A_DMA_S1FCR:
			t->S1FCR = value;
			break;
		case A_DMA_S2CR:
			t->S2CR = value;
			break;
		case A_DMA_S2NDTR:
			t->S2NDTR = value;
			break;
		case A_DMA_S2PAR:
			t->S2PAR = value;
			break;
		case A_DMA_S2M0AR:
			t->S2M0AR = value;
			break;
		case A_DMA_S2M1AR:
			t->S2M1AR = value;
			break;
		case A_DMA_S2FCR:
			t->S2FCR = value;
			break;
		case A_DMA_S3CR:
			t->S3CR = value;
			break;
		case A_DMA_S3NDTR:
			t->S3NDTR = value;
			break;
		case A_DMA_S3PAR:
			t->S3PAR = value;
			break;
		case A_DMA_S3M0AR:
			t->S3M0AR = value;
			break;
		case A_DMA_S3M1AR:
			t->S3M1AR = value;
			break;
		case A_DMA_S3FCR:
			t->S3FCR = value;
			break;
		case A_DMA_S4CR:
			t->S4CR = value;
			break;
		case A_DMA_S4NDTR:
			t->S4NDTR = value;
			break;
		case A_DMA_S4PAR:
			t->S4PAR = value;
			break;
		case A_DMA_S4M0AR:
			t->S4M0AR = value;
			break;
		case A_DMA_S4M1AR:
			t->S4M1AR = value;
			break;
		case A_DMA_S4FCR:
			t->S4FCR = value;
			break;
		case A_DMA_S5CR:
			t->S5CR = value;
			break;
		case A_DMA_S5NDTR:
			t->S5NDTR = value;
			break;
		case A_DMA_S5PAR:
			t->S5PAR = value;
			break;
		case A_DMA_S5M0AR:
			t->S5M0AR = value;
			break;
		case A_DMA_S5M1AR:
			t->S5M1AR = value;
			break;
		case A_DMA_S5FCR:
			t->S5FCR = value;
			break;
		case A_DMA_S6CR:
			t->S6CR = value;
			break;
		case A_DMA_S6NDTR:
			t->S6NDTR = value;
			break;
		case A_DMA_S6PAR:
			t->S6PAR = value;
			break;
		case A_DMA_S6M0AR:
			t->S6M0AR = value;
			break;
		case A_DMA_S6M1AR:
			t->S6M1AR = value;
			break;
		case A_DMA_S6FCR:
			t->S6FCR = value;
			break;
		case A_DMA_S7CR:
			t->S7CR = value;
			break;
		case A_DMA_S7NDTR:
			t->S7NDTR = value;
			break;
		case A_DMA_S7PAR:
			t->S7PAR = value;
			break;
		case A_DMA_S7M0AR:
			t->S7M0AR = value;
			break;
		case A_DMA_S7M1AR:
			t->S7M1AR = value;
			break;
		case A_DMA_S7FCR:
			t->S7FCR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F4 DMA write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f4_dma_ops = {
	.read = stm32f4_dma_read,
	.write = stm32f4_dma_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f4_dma_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F4DMA *t = STM32F4_DMA(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f4_dma_ops, t, "stm32f4-dma", STM32F4_DMA_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 8; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32f4_dma_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32f4_dma_reset_enter(Object *obj, ResetType type) {
	STM32F4DMA *t = STM32F4_DMA(obj);
	stm32f4_dma_register_reset(t);
}

static const VMStateDescription stm32f4_dma_vmstate = {
	.name = "stm32f4-dma",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(LISR, STM32F4DMA),
		VMSTATE_UINT32(HISR, STM32F4DMA),
		VMSTATE_UINT32(LIFCR, STM32F4DMA),
		VMSTATE_UINT32(HIFCR, STM32F4DMA),
		VMSTATE_UINT32(S0CR, STM32F4DMA),
		VMSTATE_UINT32(S0NDTR, STM32F4DMA),
		VMSTATE_UINT32(S0PAR, STM32F4DMA),
		VMSTATE_UINT32(S0M0AR, STM32F4DMA),
		VMSTATE_UINT32(S0M1AR, STM32F4DMA),
		VMSTATE_UINT32(S0FCR, STM32F4DMA),
		VMSTATE_UINT32(S1CR, STM32F4DMA),
		VMSTATE_UINT32(S1NDTR, STM32F4DMA),
		VMSTATE_UINT32(S1PAR, STM32F4DMA),
		VMSTATE_UINT32(S1M0AR, STM32F4DMA),
		VMSTATE_UINT32(S1M1AR, STM32F4DMA),
		VMSTATE_UINT32(S1FCR, STM32F4DMA),
		VMSTATE_UINT32(S2CR, STM32F4DMA),
		VMSTATE_UINT32(S2NDTR, STM32F4DMA),
		VMSTATE_UINT32(S2PAR, STM32F4DMA),
		VMSTATE_UINT32(S2M0AR, STM32F4DMA),
		VMSTATE_UINT32(S2M1AR, STM32F4DMA),
		VMSTATE_UINT32(S2FCR, STM32F4DMA),
		VMSTATE_UINT32(S3CR, STM32F4DMA),
		VMSTATE_UINT32(S3NDTR, STM32F4DMA),
		VMSTATE_UINT32(S3PAR, STM32F4DMA),
		VMSTATE_UINT32(S3M0AR, STM32F4DMA),
		VMSTATE_UINT32(S3M1AR, STM32F4DMA),
		VMSTATE_UINT32(S3FCR, STM32F4DMA),
		VMSTATE_UINT32(S4CR, STM32F4DMA),
		VMSTATE_UINT32(S4NDTR, STM32F4DMA),
		VMSTATE_UINT32(S4PAR, STM32F4DMA),
		VMSTATE_UINT32(S4M0AR, STM32F4DMA),
		VMSTATE_UINT32(S4M1AR, STM32F4DMA),
		VMSTATE_UINT32(S4FCR, STM32F4DMA),
		VMSTATE_UINT32(S5CR, STM32F4DMA),
		VMSTATE_UINT32(S5NDTR, STM32F4DMA),
		VMSTATE_UINT32(S5PAR, STM32F4DMA),
		VMSTATE_UINT32(S5M0AR, STM32F4DMA),
		VMSTATE_UINT32(S5M1AR, STM32F4DMA),
		VMSTATE_UINT32(S5FCR, STM32F4DMA),
		VMSTATE_UINT32(S6CR, STM32F4DMA),
		VMSTATE_UINT32(S6NDTR, STM32F4DMA),
		VMSTATE_UINT32(S6PAR, STM32F4DMA),
		VMSTATE_UINT32(S6M0AR, STM32F4DMA),
		VMSTATE_UINT32(S6M1AR, STM32F4DMA),
		VMSTATE_UINT32(S6FCR, STM32F4DMA),
		VMSTATE_UINT32(S7CR, STM32F4DMA),
		VMSTATE_UINT32(S7NDTR, STM32F4DMA),
		VMSTATE_UINT32(S7PAR, STM32F4DMA),
		VMSTATE_UINT32(S7M0AR, STM32F4DMA),
		VMSTATE_UINT32(S7M1AR, STM32F4DMA),
		VMSTATE_UINT32(S7FCR, STM32F4DMA),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f4_dma_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f4_dma_vmstate;
	dc->realize = stm32f4_dma_realize;
	rc->phases.enter = stm32f4_dma_reset_enter;

}

static const TypeInfo stm32f4_dma_info = {
	.name = TYPE_STM32F4_DMA,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F4DMA),
	.instance_init = stm32f4_dma_init,
	.class_init = stm32f4_dma_class_init,
};

static void stm32f4_dma_register_types(void) {
	type_register_static(&stm32f4_dma_info);
}

type_init(stm32f4_dma_register_types);

#define TYPE_STM32F4_PWR "stm32f4-pwr"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F4PWR, STM32F4_PWR)

struct STM32F4PWR {
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
#define STM32F4_PWR_SIZE				0x8

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
	FIELD(PWR_CSR, WUF, 0, 1)
	FIELD(PWR_CSR, SBF, 1, 1)
	FIELD(PWR_CSR, PVDO, 2, 1)
	FIELD(PWR_CSR, BRR, 3, 1)
	FIELD(PWR_CSR, EWUP, 8, 1)
	FIELD(PWR_CSR, BRE, 9, 1)
	FIELD(PWR_CSR, VOSRDY, 14, 1)


static void stm32f4_pwr_register_reset(STM32F4PWR *t) {
	t->CR = 0x0;
	t->CSR = 0x0;

}

static uint64_t stm32f4_pwr_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F4PWR *t = STM32F4_PWR(opaque);
	uint64_t ret;

	switch (offset) {
		case A_PWR_CR:
			ret = t->CR;
			break;
		case A_PWR_CSR:
			ret = t->CSR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F4 PWR read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f4_pwr_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F4PWR *t = STM32F4_PWR(opaque);


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
			if ((value & 0x200)) {
				t->CSR |= 0x8;
			} else if ((!(value & 0x200))) {
				t->CSR &= (~(0x8));
			}
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F4 PWR write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f4_pwr_ops = {
	.read = stm32f4_pwr_read,
	.write = stm32f4_pwr_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f4_pwr_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F4PWR *t = STM32F4_PWR(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f4_pwr_ops, t, "stm32f4-pwr", STM32F4_PWR_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32f4_pwr_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32f4_pwr_reset_enter(Object *obj, ResetType type) {
	STM32F4PWR *t = STM32F4_PWR(obj);
	stm32f4_pwr_register_reset(t);
}

static const VMStateDescription stm32f4_pwr_vmstate = {
	.name = "stm32f4-pwr",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR, STM32F4PWR),
		VMSTATE_UINT32(CSR, STM32F4PWR),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f4_pwr_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f4_pwr_vmstate;
	dc->realize = stm32f4_pwr_realize;
	rc->phases.enter = stm32f4_pwr_reset_enter;

}

static const TypeInfo stm32f4_pwr_info = {
	.name = TYPE_STM32F4_PWR,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F4PWR),
	.instance_init = stm32f4_pwr_init,
	.class_init = stm32f4_pwr_class_init,
};

static void stm32f4_pwr_register_types(void) {
	type_register_static(&stm32f4_pwr_info);
}

type_init(stm32f4_pwr_register_types);

#define TYPE_STM32F4_SYSCFG "stm32f4-syscfg"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F4SYSCFG, STM32F4_SYSCFG)

struct STM32F4SYSCFG {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/*registers*/
	uint32_t MEMRM;
	uint32_t PMC;
	uint32_t EXTICR1;
	uint32_t EXTICR2;
	uint32_t EXTICR3;
	uint32_t EXTICR4;
	uint32_t CMPCR;
	/* base */
	uint32_t base;

};
#define STM32F4_SYSCFG_SIZE				0x24

REG32(SYSCFG_MEMRM, 0x0)
	FIELD(SYSCFG_MEMRM, MEM_MODE, 0, 2)
REG32(SYSCFG_PMC, 0x4)
	FIELD(SYSCFG_PMC, MII_RMII_SEL, 23, 1)
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
REG32(SYSCFG_CMPCR, 0x20)
	FIELD(SYSCFG_CMPCR, READY, 8, 1)
	FIELD(SYSCFG_CMPCR, CMP_PD, 0, 1)


static void stm32f4_syscfg_register_reset(STM32F4SYSCFG *t) {
	t->MEMRM = 0x0;
	t->PMC = 0x0;
	t->EXTICR1 = 0x0;
	t->EXTICR2 = 0x0;
	t->EXTICR3 = 0x0;
	t->EXTICR4 = 0x0;
	t->CMPCR = 0x0;

}

static uint64_t stm32f4_syscfg_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F4SYSCFG *t = STM32F4_SYSCFG(opaque);
	uint64_t ret;

	switch (offset) {
		case A_SYSCFG_MEMRM:
			ret = t->MEMRM;
			break;
		case A_SYSCFG_PMC:
			ret = t->PMC;
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
		case A_SYSCFG_CMPCR:
			ret = t->CMPCR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F4 SYSCFG read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f4_syscfg_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F4SYSCFG *t = STM32F4_SYSCFG(opaque);


	switch (offset) {
		case A_SYSCFG_MEMRM ... A_SYSCFG_MEMRM + 3:
			value = value << ((offset - A_SYSCFG_MEMRM) << 3);
			offset = A_SYSCFG_MEMRM;
			break;
		case A_SYSCFG_PMC ... A_SYSCFG_PMC + 3:
			value = value << ((offset - A_SYSCFG_PMC) << 3);
			offset = A_SYSCFG_PMC;
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

		default: break;
	}

	switch (offset) {
		case A_SYSCFG_MEMRM:
			t->MEMRM = value;
			break;
		case A_SYSCFG_PMC:
			t->PMC = value;
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

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F4 SYSCFG write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f4_syscfg_ops = {
	.read = stm32f4_syscfg_read,
	.write = stm32f4_syscfg_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f4_syscfg_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F4SYSCFG *t = STM32F4_SYSCFG(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f4_syscfg_ops, t, "stm32f4-syscfg", STM32F4_SYSCFG_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void stm32f4_syscfg_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32f4_syscfg_reset_enter(Object *obj, ResetType type) {
	STM32F4SYSCFG *t = STM32F4_SYSCFG(obj);
	stm32f4_syscfg_register_reset(t);
}

static const VMStateDescription stm32f4_syscfg_vmstate = {
	.name = "stm32f4-syscfg",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(MEMRM, STM32F4SYSCFG),
		VMSTATE_UINT32(PMC, STM32F4SYSCFG),
		VMSTATE_UINT32(EXTICR1, STM32F4SYSCFG),
		VMSTATE_UINT32(EXTICR2, STM32F4SYSCFG),
		VMSTATE_UINT32(EXTICR3, STM32F4SYSCFG),
		VMSTATE_UINT32(EXTICR4, STM32F4SYSCFG),
		VMSTATE_UINT32(CMPCR, STM32F4SYSCFG),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f4_syscfg_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f4_syscfg_vmstate;
	dc->realize = stm32f4_syscfg_realize;
	rc->phases.enter = stm32f4_syscfg_reset_enter;

}

static const TypeInfo stm32f4_syscfg_info = {
	.name = TYPE_STM32F4_SYSCFG,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F4SYSCFG),
	.instance_init = stm32f4_syscfg_init,
	.class_init = stm32f4_syscfg_class_init,
};

static void stm32f4_syscfg_register_types(void) {
	type_register_static(&stm32f4_syscfg_info);
}

type_init(stm32f4_syscfg_register_types);

#define TYPE_STM32F4_FLASH "stm32f4-flash"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F4FLASH, STM32F4_FLASH)

struct STM32F4FLASH {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/*registers*/
	uint32_t ACR;
	uint32_t KEYR;
	uint32_t OPTKEYR;
	uint32_t SR;
	uint32_t CR;
	uint32_t OPTCR;
	/* base */
	uint32_t base;

};
#define STM32F4_FLASH_SIZE				0x18

REG32(FLASH_ACR, 0x0)
	FIELD(FLASH_ACR, LATENCY, 0, 3)
	FIELD(FLASH_ACR, PRFTEN, 8, 1)
	FIELD(FLASH_ACR, ICEN, 9, 1)
	FIELD(FLASH_ACR, DCEN, 10, 1)
	FIELD(FLASH_ACR, ICRST, 11, 1)
	FIELD(FLASH_ACR, DCRST, 12, 1)
REG32(FLASH_KEYR, 0x4)
	FIELD(FLASH_KEYR, KEY, 0, 32)
REG32(FLASH_OPTKEYR, 0x8)
	FIELD(FLASH_OPTKEYR, OPTKEY, 0, 32)
REG32(FLASH_SR, 0xc)
	FIELD(FLASH_SR, EOP, 0, 1)
	FIELD(FLASH_SR, OPERR, 1, 1)
	FIELD(FLASH_SR, WRPERR, 4, 1)
	FIELD(FLASH_SR, PGAERR, 5, 1)
	FIELD(FLASH_SR, PGPERR, 6, 1)
	FIELD(FLASH_SR, PGSERR, 7, 1)
	FIELD(FLASH_SR, BSY, 16, 1)
REG32(FLASH_CR, 0x10)
	FIELD(FLASH_CR, PG, 0, 1)
	FIELD(FLASH_CR, SER, 1, 1)
	FIELD(FLASH_CR, MER, 2, 1)
	FIELD(FLASH_CR, SNB, 3, 4)
	FIELD(FLASH_CR, PSIZE, 8, 2)
	FIELD(FLASH_CR, STRT, 16, 1)
	FIELD(FLASH_CR, EOPIE, 24, 1)
	FIELD(FLASH_CR, ERRIE, 25, 1)
	FIELD(FLASH_CR, LOCK, 31, 1)
REG32(FLASH_OPTCR, 0x14)
	FIELD(FLASH_OPTCR, OPTLOCK, 0, 1)
	FIELD(FLASH_OPTCR, OPTSTRT, 1, 1)
	FIELD(FLASH_OPTCR, BOR_LEV, 2, 2)
	FIELD(FLASH_OPTCR, WDG_SW, 5, 1)
	FIELD(FLASH_OPTCR, nRST_STOP, 6, 1)
	FIELD(FLASH_OPTCR, nRST_STDBY, 7, 1)
	FIELD(FLASH_OPTCR, RDP, 8, 8)
	FIELD(FLASH_OPTCR, nWRP, 16, 12)


static void stm32f4_flash_register_reset(STM32F4FLASH *t) {
	t->ACR = 0x0;
	t->KEYR = 0x0;
	t->OPTKEYR = 0x0;
	t->SR = 0x0;
	t->CR = 0x80000000;
	t->OPTCR = 0x14;

}

static uint64_t stm32f4_flash_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F4FLASH *t = STM32F4_FLASH(opaque);
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
		case A_FLASH_OPTCR:
			ret = t->OPTCR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F4 FLASH read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f4_flash_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F4FLASH *t = STM32F4_FLASH(opaque);


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
		case A_FLASH_OPTCR ... A_FLASH_OPTCR + 3:
			value = value << ((offset - A_FLASH_OPTCR) << 3);
			offset = A_FLASH_OPTCR;
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
		case A_FLASH_OPTCR:
			t->OPTCR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F4 FLASH write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f4_flash_ops = {
	.read = stm32f4_flash_read,
	.write = stm32f4_flash_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f4_flash_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F4FLASH *t = STM32F4_FLASH(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f4_flash_ops, t, "stm32f4-flash", STM32F4_FLASH_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void stm32f4_flash_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32f4_flash_reset_enter(Object *obj, ResetType type) {
	STM32F4FLASH *t = STM32F4_FLASH(obj);
	stm32f4_flash_register_reset(t);
}

static const VMStateDescription stm32f4_flash_vmstate = {
	.name = "stm32f4-flash",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(ACR, STM32F4FLASH),
		VMSTATE_UINT32(KEYR, STM32F4FLASH),
		VMSTATE_UINT32(OPTKEYR, STM32F4FLASH),
		VMSTATE_UINT32(SR, STM32F4FLASH),
		VMSTATE_UINT32(CR, STM32F4FLASH),
		VMSTATE_UINT32(OPTCR, STM32F4FLASH),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f4_flash_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f4_flash_vmstate;
	dc->realize = stm32f4_flash_realize;
	rc->phases.enter = stm32f4_flash_reset_enter;

}

static const TypeInfo stm32f4_flash_info = {
	.name = TYPE_STM32F4_FLASH,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F4FLASH),
	.instance_init = stm32f4_flash_init,
	.class_init = stm32f4_flash_class_init,
};

static void stm32f4_flash_register_types(void) {
	type_register_static(&stm32f4_flash_info);
}

type_init(stm32f4_flash_register_types);

#define TYPE_STM32F4_RCC "stm32f4-rcc"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F4RCC, STM32F4_RCC)

struct STM32F4RCC {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t CR;
	uint32_t PLLCFGR;
	uint32_t CFGR;
	uint32_t CIR;
	uint32_t AHB1RSTR;
	uint32_t AHB2RSTR;
	uint32_t AHB3RSTR;
	uint32_t APB1RSTR;
	uint32_t APB2RSTR;
	uint32_t AHB1ENR;
	uint32_t AHB2ENR;
	uint32_t AHB3ENR;
	uint32_t APB1ENR;
	uint32_t APB2ENR;
	uint32_t AHB1LPENR;
	uint32_t AHB2LPENR;
	uint32_t AHB3LPENR;
	uint32_t APB1LPENR;
	uint32_t APB2LPENR;
	uint32_t BDCR;
	uint32_t CSR;
	uint32_t SSCGR;
	uint32_t PLLI2SCFGR;
	/* base */
	uint32_t base;

};
#define STM32F4_RCC_SIZE				0x88

REG32(RCC_CR, 0x0)
	FIELD(RCC_CR, PLLI2SRDY, 27, 1)
	FIELD(RCC_CR, PLLI2SON, 26, 1)
	FIELD(RCC_CR, PLLRDY, 25, 1)
	FIELD(RCC_CR, PLLON, 24, 1)
	FIELD(RCC_CR, CSSON, 19, 1)
	FIELD(RCC_CR, HSEBYP, 18, 1)
	FIELD(RCC_CR, HSERDY, 17, 1)
	FIELD(RCC_CR, HSEON, 16, 1)
	FIELD(RCC_CR, HSICAL, 8, 8)
	FIELD(RCC_CR, HSITRIM, 3, 5)
	FIELD(RCC_CR, HSIRDY, 1, 1)
	FIELD(RCC_CR, HSION, 0, 1)
REG32(RCC_PLLCFGR, 0x4)
	FIELD(RCC_PLLCFGR, PLLQ3, 27, 1)
	FIELD(RCC_PLLCFGR, PLLQ2, 26, 1)
	FIELD(RCC_PLLCFGR, PLLQ1, 25, 1)
	FIELD(RCC_PLLCFGR, PLLQ0, 24, 1)
	FIELD(RCC_PLLCFGR, PLLSRC, 22, 1)
	FIELD(RCC_PLLCFGR, PLLP1, 17, 1)
	FIELD(RCC_PLLCFGR, PLLP0, 16, 1)
	FIELD(RCC_PLLCFGR, PLLN8, 14, 1)
	FIELD(RCC_PLLCFGR, PLLN7, 13, 1)
	FIELD(RCC_PLLCFGR, PLLN6, 12, 1)
	FIELD(RCC_PLLCFGR, PLLN5, 11, 1)
	FIELD(RCC_PLLCFGR, PLLN4, 10, 1)
	FIELD(RCC_PLLCFGR, PLLN3, 9, 1)
	FIELD(RCC_PLLCFGR, PLLN2, 8, 1)
	FIELD(RCC_PLLCFGR, PLLN1, 7, 1)
	FIELD(RCC_PLLCFGR, PLLN0, 6, 1)
	FIELD(RCC_PLLCFGR, PLLM5, 5, 1)
	FIELD(RCC_PLLCFGR, PLLM4, 4, 1)
	FIELD(RCC_PLLCFGR, PLLM3, 3, 1)
	FIELD(RCC_PLLCFGR, PLLM2, 2, 1)
	FIELD(RCC_PLLCFGR, PLLM1, 1, 1)
	FIELD(RCC_PLLCFGR, PLLM0, 0, 1)
REG32(RCC_CFGR, 0x8)
	FIELD(RCC_CFGR, MCO2, 30, 2)
	FIELD(RCC_CFGR, MCO2PRE, 27, 3)
	FIELD(RCC_CFGR, MCO1PRE, 24, 3)
	FIELD(RCC_CFGR, I2SSRC, 23, 1)
	FIELD(RCC_CFGR, MCO1, 21, 2)
	FIELD(RCC_CFGR, RTCPRE, 16, 5)
	FIELD(RCC_CFGR, PPRE2, 13, 3)
	FIELD(RCC_CFGR, PPRE1, 10, 3)
	FIELD(RCC_CFGR, HPRE, 4, 4)
	FIELD(RCC_CFGR, SWS1, 3, 1)
	FIELD(RCC_CFGR, SWS0, 2, 1)
	FIELD(RCC_CFGR, SW1, 1, 1)
	FIELD(RCC_CFGR, SW0, 0, 1)
REG32(RCC_CIR, 0xc)
	FIELD(RCC_CIR, CSSC, 23, 1)
	FIELD(RCC_CIR, PLLI2SRDYC, 21, 1)
	FIELD(RCC_CIR, PLLRDYC, 20, 1)
	FIELD(RCC_CIR, HSERDYC, 19, 1)
	FIELD(RCC_CIR, HSIRDYC, 18, 1)
	FIELD(RCC_CIR, LSERDYC, 17, 1)
	FIELD(RCC_CIR, LSIRDYC, 16, 1)
	FIELD(RCC_CIR, PLLI2SRDYIE, 13, 1)
	FIELD(RCC_CIR, PLLRDYIE, 12, 1)
	FIELD(RCC_CIR, HSERDYIE, 11, 1)
	FIELD(RCC_CIR, HSIRDYIE, 10, 1)
	FIELD(RCC_CIR, LSERDYIE, 9, 1)
	FIELD(RCC_CIR, LSIRDYIE, 8, 1)
	FIELD(RCC_CIR, CSSF, 7, 1)
	FIELD(RCC_CIR, PLLI2SRDYF, 5, 1)
	FIELD(RCC_CIR, PLLRDYF, 4, 1)
	FIELD(RCC_CIR, HSERDYF, 3, 1)
	FIELD(RCC_CIR, HSIRDYF, 2, 1)
	FIELD(RCC_CIR, LSERDYF, 1, 1)
	FIELD(RCC_CIR, LSIRDYF, 0, 1)
REG32(RCC_AHB1RSTR, 0x10)
	FIELD(RCC_AHB1RSTR, OTGHSRST, 29, 1)
	FIELD(RCC_AHB1RSTR, ETHMACRST, 25, 1)
	FIELD(RCC_AHB1RSTR, DMA2RST, 22, 1)
	FIELD(RCC_AHB1RSTR, DMA1RST, 21, 1)
	FIELD(RCC_AHB1RSTR, CRCRST, 12, 1)
	FIELD(RCC_AHB1RSTR, GPIOIRST, 8, 1)
	FIELD(RCC_AHB1RSTR, GPIOHRST, 7, 1)
	FIELD(RCC_AHB1RSTR, GPIOGRST, 6, 1)
	FIELD(RCC_AHB1RSTR, GPIOFRST, 5, 1)
	FIELD(RCC_AHB1RSTR, GPIOERST, 4, 1)
	FIELD(RCC_AHB1RSTR, GPIODRST, 3, 1)
	FIELD(RCC_AHB1RSTR, GPIOCRST, 2, 1)
	FIELD(RCC_AHB1RSTR, GPIOBRST, 1, 1)
	FIELD(RCC_AHB1RSTR, GPIOARST, 0, 1)
REG32(RCC_AHB2RSTR, 0x14)
	FIELD(RCC_AHB2RSTR, OTGFSRST, 7, 1)
	FIELD(RCC_AHB2RSTR, RNGRST, 6, 1)
	FIELD(RCC_AHB2RSTR, DCMIRST, 0, 1)
REG32(RCC_AHB3RSTR, 0x18)
	FIELD(RCC_AHB3RSTR, FSMCRST, 0, 1)
REG32(RCC_APB1RSTR, 0x20)
	FIELD(RCC_APB1RSTR, DACRST, 29, 1)
	FIELD(RCC_APB1RSTR, PWRRST, 28, 1)
	FIELD(RCC_APB1RSTR, CAN2RST, 26, 1)
	FIELD(RCC_APB1RSTR, CAN1RST, 25, 1)
	FIELD(RCC_APB1RSTR, I2C3RST, 23, 1)
	FIELD(RCC_APB1RSTR, I2C2RST, 22, 1)
	FIELD(RCC_APB1RSTR, I2C1RST, 21, 1)
	FIELD(RCC_APB1RSTR, UART5RST, 20, 1)
	FIELD(RCC_APB1RSTR, UART4RST, 19, 1)
	FIELD(RCC_APB1RSTR, UART3RST, 18, 1)
	FIELD(RCC_APB1RSTR, UART2RST, 17, 1)
	FIELD(RCC_APB1RSTR, SPI3RST, 15, 1)
	FIELD(RCC_APB1RSTR, SPI2RST, 14, 1)
	FIELD(RCC_APB1RSTR, WWDGRST, 11, 1)
	FIELD(RCC_APB1RSTR, TIM14RST, 8, 1)
	FIELD(RCC_APB1RSTR, TIM13RST, 7, 1)
	FIELD(RCC_APB1RSTR, TIM12RST, 6, 1)
	FIELD(RCC_APB1RSTR, TIM7RST, 5, 1)
	FIELD(RCC_APB1RSTR, TIM6RST, 4, 1)
	FIELD(RCC_APB1RSTR, TIM5RST, 3, 1)
	FIELD(RCC_APB1RSTR, TIM4RST, 2, 1)
	FIELD(RCC_APB1RSTR, TIM3RST, 1, 1)
	FIELD(RCC_APB1RSTR, TIM2RST, 0, 1)
REG32(RCC_APB2RSTR, 0x24)
	FIELD(RCC_APB2RSTR, TIM11RST, 18, 1)
	FIELD(RCC_APB2RSTR, TIM10RST, 17, 1)
	FIELD(RCC_APB2RSTR, TIM9RST, 16, 1)
	FIELD(RCC_APB2RSTR, SYSCFGRST, 14, 1)
	FIELD(RCC_APB2RSTR, SPI1RST, 12, 1)
	FIELD(RCC_APB2RSTR, SDIORST, 11, 1)
	FIELD(RCC_APB2RSTR, ADCRST, 8, 1)
	FIELD(RCC_APB2RSTR, USART6RST, 5, 1)
	FIELD(RCC_APB2RSTR, USART1RST, 4, 1)
	FIELD(RCC_APB2RSTR, TIM8RST, 1, 1)
	FIELD(RCC_APB2RSTR, TIM1RST, 0, 1)
REG32(RCC_AHB1ENR, 0x30)
	FIELD(RCC_AHB1ENR, OTGHSULPIEN, 30, 1)
	FIELD(RCC_AHB1ENR, OTGHSEN, 29, 1)
	FIELD(RCC_AHB1ENR, ETHMACPTPEN, 28, 1)
	FIELD(RCC_AHB1ENR, ETHMACRXEN, 27, 1)
	FIELD(RCC_AHB1ENR, ETHMACTXEN, 26, 1)
	FIELD(RCC_AHB1ENR, ETHMACEN, 25, 1)
	FIELD(RCC_AHB1ENR, DMA2EN, 22, 1)
	FIELD(RCC_AHB1ENR, DMA1EN, 21, 1)
	FIELD(RCC_AHB1ENR, BKPSRAMEN, 18, 1)
	FIELD(RCC_AHB1ENR, CRCEN, 12, 1)
	FIELD(RCC_AHB1ENR, GPIOIEN, 8, 1)
	FIELD(RCC_AHB1ENR, GPIOHEN, 7, 1)
	FIELD(RCC_AHB1ENR, GPIOGEN, 6, 1)
	FIELD(RCC_AHB1ENR, GPIOFEN, 5, 1)
	FIELD(RCC_AHB1ENR, GPIOEEN, 4, 1)
	FIELD(RCC_AHB1ENR, GPIODEN, 3, 1)
	FIELD(RCC_AHB1ENR, GPIOCEN, 2, 1)
	FIELD(RCC_AHB1ENR, GPIOBEN, 1, 1)
	FIELD(RCC_AHB1ENR, GPIOAEN, 0, 1)
REG32(RCC_AHB2ENR, 0x34)
	FIELD(RCC_AHB2ENR, OTGFSEN, 7, 1)
	FIELD(RCC_AHB2ENR, RNGEN, 6, 1)
	FIELD(RCC_AHB2ENR, DCMIEN, 0, 1)
REG32(RCC_AHB3ENR, 0x38)
	FIELD(RCC_AHB3ENR, FSMCEN, 0, 1)
REG32(RCC_APB1ENR, 0x40)
	FIELD(RCC_APB1ENR, DACEN, 29, 1)
	FIELD(RCC_APB1ENR, PWREN, 28, 1)
	FIELD(RCC_APB1ENR, CAN2EN, 26, 1)
	FIELD(RCC_APB1ENR, CAN1EN, 25, 1)
	FIELD(RCC_APB1ENR, I2C3EN, 23, 1)
	FIELD(RCC_APB1ENR, I2C2EN, 22, 1)
	FIELD(RCC_APB1ENR, I2C1EN, 21, 1)
	FIELD(RCC_APB1ENR, UART5EN, 20, 1)
	FIELD(RCC_APB1ENR, UART4EN, 19, 1)
	FIELD(RCC_APB1ENR, USART3EN, 18, 1)
	FIELD(RCC_APB1ENR, USART2EN, 17, 1)
	FIELD(RCC_APB1ENR, SPI3EN, 15, 1)
	FIELD(RCC_APB1ENR, SPI2EN, 14, 1)
	FIELD(RCC_APB1ENR, WWDGEN, 11, 1)
	FIELD(RCC_APB1ENR, TIM14EN, 8, 1)
	FIELD(RCC_APB1ENR, TIM13EN, 7, 1)
	FIELD(RCC_APB1ENR, TIM12EN, 6, 1)
	FIELD(RCC_APB1ENR, TIM7EN, 5, 1)
	FIELD(RCC_APB1ENR, TIM6EN, 4, 1)
	FIELD(RCC_APB1ENR, TIM5EN, 3, 1)
	FIELD(RCC_APB1ENR, TIM4EN, 2, 1)
	FIELD(RCC_APB1ENR, TIM3EN, 1, 1)
	FIELD(RCC_APB1ENR, TIM2EN, 0, 1)
REG32(RCC_APB2ENR, 0x44)
	FIELD(RCC_APB2ENR, TIM11EN, 18, 1)
	FIELD(RCC_APB2ENR, TIM10EN, 17, 1)
	FIELD(RCC_APB2ENR, TIM9EN, 16, 1)
	FIELD(RCC_APB2ENR, SYSCFGEN, 14, 1)
	FIELD(RCC_APB2ENR, SPI1EN, 12, 1)
	FIELD(RCC_APB2ENR, SDIOEN, 11, 1)
	FIELD(RCC_APB2ENR, ADC3EN, 10, 1)
	FIELD(RCC_APB2ENR, ADC2EN, 9, 1)
	FIELD(RCC_APB2ENR, ADC1EN, 8, 1)
	FIELD(RCC_APB2ENR, USART6EN, 5, 1)
	FIELD(RCC_APB2ENR, USART1EN, 4, 1)
	FIELD(RCC_APB2ENR, TIM8EN, 1, 1)
	FIELD(RCC_APB2ENR, TIM1EN, 0, 1)
REG32(RCC_AHB1LPENR, 0x50)
	FIELD(RCC_AHB1LPENR, OTGHSULPILPEN, 30, 1)
	FIELD(RCC_AHB1LPENR, OTGHSLPEN, 29, 1)
	FIELD(RCC_AHB1LPENR, ETHMACPTPLPEN, 28, 1)
	FIELD(RCC_AHB1LPENR, ETHMACRXLPEN, 27, 1)
	FIELD(RCC_AHB1LPENR, ETHMACTXLPEN, 26, 1)
	FIELD(RCC_AHB1LPENR, ETHMACLPEN, 25, 1)
	FIELD(RCC_AHB1LPENR, DMA2LPEN, 22, 1)
	FIELD(RCC_AHB1LPENR, DMA1LPEN, 21, 1)
	FIELD(RCC_AHB1LPENR, BKPSRAMLPEN, 18, 1)
	FIELD(RCC_AHB1LPENR, SRAM2LPEN, 17, 1)
	FIELD(RCC_AHB1LPENR, SRAM1LPEN, 16, 1)
	FIELD(RCC_AHB1LPENR, FLITFLPEN, 15, 1)
	FIELD(RCC_AHB1LPENR, CRCLPEN, 12, 1)
	FIELD(RCC_AHB1LPENR, GPIOILPEN, 8, 1)
	FIELD(RCC_AHB1LPENR, GPIOHLPEN, 7, 1)
	FIELD(RCC_AHB1LPENR, GPIOGLPEN, 6, 1)
	FIELD(RCC_AHB1LPENR, GPIOFLPEN, 5, 1)
	FIELD(RCC_AHB1LPENR, GPIOELPEN, 4, 1)
	FIELD(RCC_AHB1LPENR, GPIODLPEN, 3, 1)
	FIELD(RCC_AHB1LPENR, GPIOCLPEN, 2, 1)
	FIELD(RCC_AHB1LPENR, GPIOBLPEN, 1, 1)
	FIELD(RCC_AHB1LPENR, GPIOALPEN, 0, 1)
REG32(RCC_AHB2LPENR, 0x54)
	FIELD(RCC_AHB2LPENR, OTGFSLPEN, 7, 1)
	FIELD(RCC_AHB2LPENR, RNGLPEN, 6, 1)
	FIELD(RCC_AHB2LPENR, DCMILPEN, 0, 1)
REG32(RCC_AHB3LPENR, 0x58)
	FIELD(RCC_AHB3LPENR, FSMCLPEN, 0, 1)
REG32(RCC_APB1LPENR, 0x60)
	FIELD(RCC_APB1LPENR, DACLPEN, 29, 1)
	FIELD(RCC_APB1LPENR, PWRLPEN, 28, 1)
	FIELD(RCC_APB1LPENR, CAN2LPEN, 26, 1)
	FIELD(RCC_APB1LPENR, CAN1LPEN, 25, 1)
	FIELD(RCC_APB1LPENR, I2C3LPEN, 23, 1)
	FIELD(RCC_APB1LPENR, I2C2LPEN, 22, 1)
	FIELD(RCC_APB1LPENR, I2C1LPEN, 21, 1)
	FIELD(RCC_APB1LPENR, UART5LPEN, 20, 1)
	FIELD(RCC_APB1LPENR, UART4LPEN, 19, 1)
	FIELD(RCC_APB1LPENR, USART3LPEN, 18, 1)
	FIELD(RCC_APB1LPENR, USART2LPEN, 17, 1)
	FIELD(RCC_APB1LPENR, SPI3LPEN, 15, 1)
	FIELD(RCC_APB1LPENR, SPI2LPEN, 14, 1)
	FIELD(RCC_APB1LPENR, WWDGLPEN, 11, 1)
	FIELD(RCC_APB1LPENR, TIM14LPEN, 8, 1)
	FIELD(RCC_APB1LPENR, TIM13LPEN, 7, 1)
	FIELD(RCC_APB1LPENR, TIM12LPEN, 6, 1)
	FIELD(RCC_APB1LPENR, TIM7LPEN, 5, 1)
	FIELD(RCC_APB1LPENR, TIM6LPEN, 4, 1)
	FIELD(RCC_APB1LPENR, TIM5LPEN, 3, 1)
	FIELD(RCC_APB1LPENR, TIM4LPEN, 2, 1)
	FIELD(RCC_APB1LPENR, TIM3LPEN, 1, 1)
	FIELD(RCC_APB1LPENR, TIM2LPEN, 0, 1)
REG32(RCC_APB2LPENR, 0x64)
	FIELD(RCC_APB2LPENR, TIM11LPEN, 18, 1)
	FIELD(RCC_APB2LPENR, TIM10LPEN, 17, 1)
	FIELD(RCC_APB2LPENR, TIM9LPEN, 16, 1)
	FIELD(RCC_APB2LPENR, SYSCFGLPEN, 14, 1)
	FIELD(RCC_APB2LPENR, SPI1LPEN, 12, 1)
	FIELD(RCC_APB2LPENR, SDIOLPEN, 11, 1)
	FIELD(RCC_APB2LPENR, ADC3LPEN, 10, 1)
	FIELD(RCC_APB2LPENR, ADC2LPEN, 9, 1)
	FIELD(RCC_APB2LPENR, ADC1LPEN, 8, 1)
	FIELD(RCC_APB2LPENR, USART6LPEN, 5, 1)
	FIELD(RCC_APB2LPENR, USART1LPEN, 4, 1)
	FIELD(RCC_APB2LPENR, TIM8LPEN, 1, 1)
	FIELD(RCC_APB2LPENR, TIM1LPEN, 0, 1)
REG32(RCC_BDCR, 0x70)
	FIELD(RCC_BDCR, BDRST, 16, 1)
	FIELD(RCC_BDCR, RTCEN, 15, 1)
	FIELD(RCC_BDCR, RTCSEL1, 9, 1)
	FIELD(RCC_BDCR, RTCSEL0, 8, 1)
	FIELD(RCC_BDCR, LSEBYP, 2, 1)
	FIELD(RCC_BDCR, LSERDY, 1, 1)
	FIELD(RCC_BDCR, LSEON, 0, 1)
REG32(RCC_CSR, 0x74)
	FIELD(RCC_CSR, LPWRRSTF, 31, 1)
	FIELD(RCC_CSR, WWDGRSTF, 30, 1)
	FIELD(RCC_CSR, WDGRSTF, 29, 1)
	FIELD(RCC_CSR, SFTRSTF, 28, 1)
	FIELD(RCC_CSR, PORRSTF, 27, 1)
	FIELD(RCC_CSR, PADRSTF, 26, 1)
	FIELD(RCC_CSR, BORRSTF, 25, 1)
	FIELD(RCC_CSR, RMVF, 24, 1)
	FIELD(RCC_CSR, LSIRDY, 1, 1)
	FIELD(RCC_CSR, LSION, 0, 1)
REG32(RCC_SSCGR, 0x80)
	FIELD(RCC_SSCGR, SSCGEN, 31, 1)
	FIELD(RCC_SSCGR, SPREADSEL, 30, 1)
	FIELD(RCC_SSCGR, INCSTEP, 13, 15)
	FIELD(RCC_SSCGR, MODPER, 0, 13)
REG32(RCC_PLLI2SCFGR, 0x84)
	FIELD(RCC_PLLI2SCFGR, PLLI2SRx, 28, 3)
	FIELD(RCC_PLLI2SCFGR, PLLI2SNx, 6, 9)


__attribute__((unused))
static void stm32f4_rcc_update(STM32F4RCC *t) {
	int conditions = (t->CIR & 0x80);
	qemu_set_irq(t->irq[0], conditions);

}

static void stm32f4_rcc_register_reset(STM32F4RCC *t) {
	t->CR = 0x83;
	t->PLLCFGR = 0x24003010;
	t->CFGR = 0x0;
	t->CIR = 0x0;
	t->AHB1RSTR = 0x0;
	t->AHB2RSTR = 0x0;
	t->AHB3RSTR = 0x0;
	t->APB1RSTR = 0x0;
	t->APB2RSTR = 0x0;
	t->AHB1ENR = 0x100000;
	t->AHB2ENR = 0x0;
	t->AHB3ENR = 0x0;
	t->APB1ENR = 0x0;
	t->APB2ENR = 0x0;
	t->AHB1LPENR = 0x7e6791ff;
	t->AHB2LPENR = 0xf1;
	t->AHB3LPENR = 0x1;
	t->APB1LPENR = 0x36fec9ff;
	t->APB2LPENR = 0x75f33;
	t->BDCR = 0x0;
	t->CSR = 0xe000000;
	t->SSCGR = 0x0;
	t->PLLI2SCFGR = 0x20003000;

}

static uint64_t stm32f4_rcc_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F4RCC *t = STM32F4_RCC(opaque);
	uint64_t ret;

	switch (offset) {
		case A_RCC_CR:
			ret = t->CR;
			break;
		case A_RCC_PLLCFGR:
			ret = t->PLLCFGR;
			break;
		case A_RCC_CFGR:
			ret = t->CFGR;
			break;
		case A_RCC_CIR:
			ret = t->CIR;
			break;
		case A_RCC_AHB1RSTR:
			ret = t->AHB1RSTR;
			break;
		case A_RCC_AHB2RSTR:
			ret = t->AHB2RSTR;
			break;
		case A_RCC_AHB3RSTR:
			ret = t->AHB3RSTR;
			break;
		case A_RCC_APB1RSTR:
			ret = t->APB1RSTR;
			break;
		case A_RCC_APB2RSTR:
			ret = t->APB2RSTR;
			break;
		case A_RCC_AHB1ENR:
			ret = t->AHB1ENR;
			break;
		case A_RCC_AHB2ENR:
			ret = t->AHB2ENR;
			break;
		case A_RCC_AHB3ENR:
			ret = t->AHB3ENR;
			break;
		case A_RCC_APB1ENR:
			ret = t->APB1ENR;
			break;
		case A_RCC_APB2ENR:
			ret = t->APB2ENR;
			break;
		case A_RCC_AHB1LPENR:
			ret = t->AHB1LPENR;
			break;
		case A_RCC_AHB2LPENR:
			ret = t->AHB2LPENR;
			break;
		case A_RCC_AHB3LPENR:
			ret = t->AHB3LPENR;
			break;
		case A_RCC_APB1LPENR:
			ret = t->APB1LPENR;
			break;
		case A_RCC_APB2LPENR:
			ret = t->APB2LPENR;
			break;
		case A_RCC_BDCR:
			ret = t->BDCR;
			break;
		case A_RCC_CSR:
			ret = t->CSR;
			break;
		case A_RCC_SSCGR:
			ret = t->SSCGR;
			break;
		case A_RCC_PLLI2SCFGR:
			ret = t->PLLI2SCFGR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F4 RCC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f4_rcc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F4RCC *t = STM32F4_RCC(opaque);


	switch (offset) {
		case A_RCC_CR ... A_RCC_CR + 3:
			value = value << ((offset - A_RCC_CR) << 3);
			offset = A_RCC_CR;
			break;
		case A_RCC_PLLCFGR ... A_RCC_PLLCFGR + 3:
			value = value << ((offset - A_RCC_PLLCFGR) << 3);
			offset = A_RCC_PLLCFGR;
			break;
		case A_RCC_CFGR ... A_RCC_CFGR + 3:
			value = value << ((offset - A_RCC_CFGR) << 3);
			offset = A_RCC_CFGR;
			break;
		case A_RCC_CIR ... A_RCC_CIR + 3:
			value = value << ((offset - A_RCC_CIR) << 3);
			offset = A_RCC_CIR;
			break;
		case A_RCC_AHB1RSTR ... A_RCC_AHB1RSTR + 3:
			value = value << ((offset - A_RCC_AHB1RSTR) << 3);
			offset = A_RCC_AHB1RSTR;
			break;
		case A_RCC_AHB2RSTR ... A_RCC_AHB2RSTR + 3:
			value = value << ((offset - A_RCC_AHB2RSTR) << 3);
			offset = A_RCC_AHB2RSTR;
			break;
		case A_RCC_AHB3RSTR ... A_RCC_AHB3RSTR + 3:
			value = value << ((offset - A_RCC_AHB3RSTR) << 3);
			offset = A_RCC_AHB3RSTR;
			break;
		case A_RCC_APB1RSTR ... A_RCC_APB1RSTR + 3:
			value = value << ((offset - A_RCC_APB1RSTR) << 3);
			offset = A_RCC_APB1RSTR;
			break;
		case A_RCC_APB2RSTR ... A_RCC_APB2RSTR + 3:
			value = value << ((offset - A_RCC_APB2RSTR) << 3);
			offset = A_RCC_APB2RSTR;
			break;
		case A_RCC_AHB1ENR ... A_RCC_AHB1ENR + 3:
			value = value << ((offset - A_RCC_AHB1ENR) << 3);
			offset = A_RCC_AHB1ENR;
			break;
		case A_RCC_AHB2ENR ... A_RCC_AHB2ENR + 3:
			value = value << ((offset - A_RCC_AHB2ENR) << 3);
			offset = A_RCC_AHB2ENR;
			break;
		case A_RCC_AHB3ENR ... A_RCC_AHB3ENR + 3:
			value = value << ((offset - A_RCC_AHB3ENR) << 3);
			offset = A_RCC_AHB3ENR;
			break;
		case A_RCC_APB1ENR ... A_RCC_APB1ENR + 3:
			value = value << ((offset - A_RCC_APB1ENR) << 3);
			offset = A_RCC_APB1ENR;
			break;
		case A_RCC_APB2ENR ... A_RCC_APB2ENR + 3:
			value = value << ((offset - A_RCC_APB2ENR) << 3);
			offset = A_RCC_APB2ENR;
			break;
		case A_RCC_AHB1LPENR ... A_RCC_AHB1LPENR + 3:
			value = value << ((offset - A_RCC_AHB1LPENR) << 3);
			offset = A_RCC_AHB1LPENR;
			break;
		case A_RCC_AHB2LPENR ... A_RCC_AHB2LPENR + 3:
			value = value << ((offset - A_RCC_AHB2LPENR) << 3);
			offset = A_RCC_AHB2LPENR;
			break;
		case A_RCC_AHB3LPENR ... A_RCC_AHB3LPENR + 3:
			value = value << ((offset - A_RCC_AHB3LPENR) << 3);
			offset = A_RCC_AHB3LPENR;
			break;
		case A_RCC_APB1LPENR ... A_RCC_APB1LPENR + 3:
			value = value << ((offset - A_RCC_APB1LPENR) << 3);
			offset = A_RCC_APB1LPENR;
			break;
		case A_RCC_APB2LPENR ... A_RCC_APB2LPENR + 3:
			value = value << ((offset - A_RCC_APB2LPENR) << 3);
			offset = A_RCC_APB2LPENR;
			break;
		case A_RCC_BDCR ... A_RCC_BDCR + 3:
			value = value << ((offset - A_RCC_BDCR) << 3);
			offset = A_RCC_BDCR;
			break;
		case A_RCC_CSR ... A_RCC_CSR + 3:
			value = value << ((offset - A_RCC_CSR) << 3);
			offset = A_RCC_CSR;
			break;
		case A_RCC_SSCGR ... A_RCC_SSCGR + 3:
			value = value << ((offset - A_RCC_SSCGR) << 3);
			offset = A_RCC_SSCGR;
			break;
		case A_RCC_PLLI2SCFGR ... A_RCC_PLLI2SCFGR + 3:
			value = value << ((offset - A_RCC_PLLI2SCFGR) << 3);
			offset = A_RCC_PLLI2SCFGR;
			break;

		default: break;
	}

	switch (offset) {
		case A_RCC_CR:
			t->CR = value;
			if ((value & 0x10000)) {
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
		case A_RCC_PLLCFGR:
			t->PLLCFGR = value;
			break;
		case A_RCC_CFGR:
			t->CFGR = value;
			if (((value & 0x1) && (!(value & 0x2)))) {
				t->CFGR |= 0x4;
				t->CFGR &= (~(0x8));
			} else if (((!(value & 0x1)) && (!(value & 0x2)))) {
				t->CFGR &= (~(0x8));
				t->CFGR &= (~(0x4));
			} else if ((value & 0x2)) {
				t->CFGR |= 0x8;
			}
			break;
		case A_RCC_CIR:
			t->CIR = value;
			stm32f4_rcc_update(t);
			break;
		case A_RCC_AHB1RSTR:
			t->AHB1RSTR = value;
			break;
		case A_RCC_AHB2RSTR:
			t->AHB2RSTR = value;
			break;
		case A_RCC_AHB3RSTR:
			t->AHB3RSTR = value;
			break;
		case A_RCC_APB1RSTR:
			t->APB1RSTR = value;
			break;
		case A_RCC_APB2RSTR:
			t->APB2RSTR = value;
			break;
		case A_RCC_AHB1ENR:
			t->AHB1ENR = value;
			break;
		case A_RCC_AHB2ENR:
			t->AHB2ENR = value;
			break;
		case A_RCC_AHB3ENR:
			t->AHB3ENR = value;
			break;
		case A_RCC_APB1ENR:
			t->APB1ENR = value;
			break;
		case A_RCC_APB2ENR:
			t->APB2ENR = value;
			break;
		case A_RCC_AHB1LPENR:
			t->AHB1LPENR = value;
			break;
		case A_RCC_AHB2LPENR:
			t->AHB2LPENR = value;
			break;
		case A_RCC_AHB3LPENR:
			t->AHB3LPENR = value;
			break;
		case A_RCC_APB1LPENR:
			t->APB1LPENR = value;
			break;
		case A_RCC_APB2LPENR:
			t->APB2LPENR = value;
			break;
		case A_RCC_BDCR:
			t->BDCR = value;
			if ((!(value & 0x4))) {
				t->BDCR &= (~(0x2));
			}
			if ((value & 0x1)) {
				t->BDCR |= 0x2;
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
		case A_RCC_SSCGR:
			t->SSCGR = value;
			break;
		case A_RCC_PLLI2SCFGR:
			t->PLLI2SCFGR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F4 RCC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f4_rcc_ops = {
	.read = stm32f4_rcc_read,
	.write = stm32f4_rcc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f4_rcc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F4RCC *t = STM32F4_RCC(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f4_rcc_ops, t, "stm32f4-rcc", STM32F4_RCC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32f4_rcc_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32f4_rcc_reset_enter(Object *obj, ResetType type) {
	STM32F4RCC *t = STM32F4_RCC(obj);
	stm32f4_rcc_register_reset(t);
}

static const VMStateDescription stm32f4_rcc_vmstate = {
	.name = "stm32f4-rcc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR, STM32F4RCC),
		VMSTATE_UINT32(PLLCFGR, STM32F4RCC),
		VMSTATE_UINT32(CFGR, STM32F4RCC),
		VMSTATE_UINT32(CIR, STM32F4RCC),
		VMSTATE_UINT32(AHB1RSTR, STM32F4RCC),
		VMSTATE_UINT32(AHB2RSTR, STM32F4RCC),
		VMSTATE_UINT32(AHB3RSTR, STM32F4RCC),
		VMSTATE_UINT32(APB1RSTR, STM32F4RCC),
		VMSTATE_UINT32(APB2RSTR, STM32F4RCC),
		VMSTATE_UINT32(AHB1ENR, STM32F4RCC),
		VMSTATE_UINT32(AHB2ENR, STM32F4RCC),
		VMSTATE_UINT32(AHB3ENR, STM32F4RCC),
		VMSTATE_UINT32(APB1ENR, STM32F4RCC),
		VMSTATE_UINT32(APB2ENR, STM32F4RCC),
		VMSTATE_UINT32(AHB1LPENR, STM32F4RCC),
		VMSTATE_UINT32(AHB2LPENR, STM32F4RCC),
		VMSTATE_UINT32(AHB3LPENR, STM32F4RCC),
		VMSTATE_UINT32(APB1LPENR, STM32F4RCC),
		VMSTATE_UINT32(APB2LPENR, STM32F4RCC),
		VMSTATE_UINT32(BDCR, STM32F4RCC),
		VMSTATE_UINT32(CSR, STM32F4RCC),
		VMSTATE_UINT32(SSCGR, STM32F4RCC),
		VMSTATE_UINT32(PLLI2SCFGR, STM32F4RCC),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f4_rcc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f4_rcc_vmstate;
	dc->realize = stm32f4_rcc_realize;
	rc->phases.enter = stm32f4_rcc_reset_enter;

}

static const TypeInfo stm32f4_rcc_info = {
	.name = TYPE_STM32F4_RCC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F4RCC),
	.instance_init = stm32f4_rcc_init,
	.class_init = stm32f4_rcc_class_init,
};

static void stm32f4_rcc_register_types(void) {
	type_register_static(&stm32f4_rcc_info);
}

type_init(stm32f4_rcc_register_types);

#define TYPE_STM32F4_TIM3 "stm32f4-tim3"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F4TIM3, STM32F4_TIM3)

struct STM32F4TIM3 {
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
#define STM32F4_TIM3_SIZE				0x50

REG32(TIM3_CR1, 0x0)
	FIELD(TIM3_CR1, CKD, 8, 2)
	FIELD(TIM3_CR1, ARPE, 7, 1)
	FIELD(TIM3_CR1, CMS, 5, 2)
	FIELD(TIM3_CR1, DIR, 4, 1)
	FIELD(TIM3_CR1, OPM, 3, 1)
	FIELD(TIM3_CR1, URS, 2, 1)
	FIELD(TIM3_CR1, UDIS, 1, 1)
	FIELD(TIM3_CR1, CEN, 0, 1)
REG32(TIM3_CR2, 0x4)
	FIELD(TIM3_CR2, TI1S, 7, 1)
	FIELD(TIM3_CR2, MMS, 4, 3)
	FIELD(TIM3_CR2, CCDS, 3, 1)
REG32(TIM3_SMCR, 0x8)
	FIELD(TIM3_SMCR, ETP, 15, 1)
	FIELD(TIM3_SMCR, ECE, 14, 1)
	FIELD(TIM3_SMCR, ETPS, 12, 2)
	FIELD(TIM3_SMCR, ETF, 8, 4)
	FIELD(TIM3_SMCR, MSM, 7, 1)
	FIELD(TIM3_SMCR, TS, 4, 3)
	FIELD(TIM3_SMCR, SMS, 0, 3)
REG32(TIM3_DIER, 0xc)
	FIELD(TIM3_DIER, TDE, 14, 1)
	FIELD(TIM3_DIER, CC4DE, 12, 1)
	FIELD(TIM3_DIER, CC3DE, 11, 1)
	FIELD(TIM3_DIER, CC2DE, 10, 1)
	FIELD(TIM3_DIER, CC1DE, 9, 1)
	FIELD(TIM3_DIER, UDE, 8, 1)
	FIELD(TIM3_DIER, TIE, 6, 1)
	FIELD(TIM3_DIER, CC4IE, 4, 1)
	FIELD(TIM3_DIER, CC3IE, 3, 1)
	FIELD(TIM3_DIER, CC2IE, 2, 1)
	FIELD(TIM3_DIER, CC1IE, 1, 1)
	FIELD(TIM3_DIER, UIE, 0, 1)
REG32(TIM3_SR, 0x10)
	FIELD(TIM3_SR, CC4OF, 12, 1)
	FIELD(TIM3_SR, CC3OF, 11, 1)
	FIELD(TIM3_SR, CC2OF, 10, 1)
	FIELD(TIM3_SR, CC1OF, 9, 1)
	FIELD(TIM3_SR, TIF, 6, 1)
	FIELD(TIM3_SR, CC4IF, 4, 1)
	FIELD(TIM3_SR, CC3IF, 3, 1)
	FIELD(TIM3_SR, CC2IF, 2, 1)
	FIELD(TIM3_SR, CC1IF, 1, 1)
	FIELD(TIM3_SR, UIF, 0, 1)
REG32(TIM3_EGR, 0x14)
	FIELD(TIM3_EGR, TG, 6, 1)
	FIELD(TIM3_EGR, CC4G, 4, 1)
	FIELD(TIM3_EGR, CC3G, 3, 1)
	FIELD(TIM3_EGR, CC2G, 2, 1)
	FIELD(TIM3_EGR, CC1G, 1, 1)
	FIELD(TIM3_EGR, UG, 0, 1)
REG32(TIM3_CCMR1_Output, 0x18)
	FIELD(TIM3_CCMR1_Output, OC2CE, 15, 1)
	FIELD(TIM3_CCMR1_Output, OC2M, 12, 3)
	FIELD(TIM3_CCMR1_Output, OC2PE, 11, 1)
	FIELD(TIM3_CCMR1_Output, OC2FE, 10, 1)
	FIELD(TIM3_CCMR1_Output, CC2S, 8, 2)
	FIELD(TIM3_CCMR1_Output, OC1CE, 7, 1)
	FIELD(TIM3_CCMR1_Output, OC1M, 4, 3)
	FIELD(TIM3_CCMR1_Output, OC1PE, 3, 1)
	FIELD(TIM3_CCMR1_Output, OC1FE, 2, 1)
	FIELD(TIM3_CCMR1_Output, CC1S, 0, 2)
REG32(TIM3_CCMR1_Input, 0x18)
	FIELD(TIM3_CCMR1_Input, IC2F, 12, 4)
	FIELD(TIM3_CCMR1_Input, IC2PSC, 10, 2)
	FIELD(TIM3_CCMR1_Input, CC2S, 8, 2)
	FIELD(TIM3_CCMR1_Input, IC1F, 4, 4)
	FIELD(TIM3_CCMR1_Input, IC1PSC, 2, 2)
	FIELD(TIM3_CCMR1_Input, CC1S, 0, 2)
REG32(TIM3_CCMR2_Output, 0x1c)
	FIELD(TIM3_CCMR2_Output, OC4CE, 15, 1)
	FIELD(TIM3_CCMR2_Output, OC4M, 12, 3)
	FIELD(TIM3_CCMR2_Output, OC4PE, 11, 1)
	FIELD(TIM3_CCMR2_Output, OC4FE, 10, 1)
	FIELD(TIM3_CCMR2_Output, CC4S, 8, 2)
	FIELD(TIM3_CCMR2_Output, OC3CE, 7, 1)
	FIELD(TIM3_CCMR2_Output, OC3M, 4, 3)
	FIELD(TIM3_CCMR2_Output, OC3PE, 3, 1)
	FIELD(TIM3_CCMR2_Output, OC3FE, 2, 1)
	FIELD(TIM3_CCMR2_Output, CC3S, 0, 2)
REG32(TIM3_CCMR2_Input, 0x1c)
	FIELD(TIM3_CCMR2_Input, IC4F, 12, 4)
	FIELD(TIM3_CCMR2_Input, IC4PSC, 10, 2)
	FIELD(TIM3_CCMR2_Input, CC4S, 8, 2)
	FIELD(TIM3_CCMR2_Input, IC3F, 4, 4)
	FIELD(TIM3_CCMR2_Input, IC3PSC, 2, 2)
	FIELD(TIM3_CCMR2_Input, CC3S, 0, 2)
REG32(TIM3_CCER, 0x20)
	FIELD(TIM3_CCER, CC4NP, 15, 1)
	FIELD(TIM3_CCER, CC4P, 13, 1)
	FIELD(TIM3_CCER, CC4E, 12, 1)
	FIELD(TIM3_CCER, CC3NP, 11, 1)
	FIELD(TIM3_CCER, CC3P, 9, 1)
	FIELD(TIM3_CCER, CC3E, 8, 1)
	FIELD(TIM3_CCER, CC2NP, 7, 1)
	FIELD(TIM3_CCER, CC2P, 5, 1)
	FIELD(TIM3_CCER, CC2E, 4, 1)
	FIELD(TIM3_CCER, CC1NP, 3, 1)
	FIELD(TIM3_CCER, CC1P, 1, 1)
	FIELD(TIM3_CCER, CC1E, 0, 1)
REG32(TIM3_CNT, 0x24)
	FIELD(TIM3_CNT, CNT_H, 16, 16)
	FIELD(TIM3_CNT, CNT_L, 0, 16)
REG32(TIM3_PSC, 0x28)
	FIELD(TIM3_PSC, PSC, 0, 16)
REG32(TIM3_ARR, 0x2c)
	FIELD(TIM3_ARR, ARR_H, 16, 16)
	FIELD(TIM3_ARR, ARR_L, 0, 16)
REG32(TIM3_CCR1, 0x34)
	FIELD(TIM3_CCR1, CCR1_H, 16, 16)
	FIELD(TIM3_CCR1, CCR1_L, 0, 16)
REG32(TIM3_CCR2, 0x38)
	FIELD(TIM3_CCR2, CCR2_H, 16, 16)
	FIELD(TIM3_CCR2, CCR2_L, 0, 16)
REG32(TIM3_CCR3, 0x3c)
	FIELD(TIM3_CCR3, CCR3_H, 16, 16)
	FIELD(TIM3_CCR3, CCR3_L, 0, 16)
REG32(TIM3_CCR4, 0x40)
	FIELD(TIM3_CCR4, CCR4_H, 16, 16)
	FIELD(TIM3_CCR4, CCR4_L, 0, 16)
REG32(TIM3_DCR, 0x48)
	FIELD(TIM3_DCR, DBL, 8, 5)
	FIELD(TIM3_DCR, DBA, 0, 5)
REG32(TIM3_DMAR, 0x4c)
	FIELD(TIM3_DMAR, DMAB, 0, 16)


__attribute__((unused))
static void stm32f4_tim3_update(STM32F4TIM3 *t) {
	int conditions = (((t->SR & 0x80) && (t->DIER & 0x80)) || ((t->DIER & 0x40) && (t->SR & 0x40)) || ((t->DIER & 0x20) && (t->SR & 0x20)) || ((t->DIER & 0x10) && (t->SR & 0x10)) || ((t->SR & 0x1) && (t->DIER & 0x1)) || ((t->SR & 0x8) && (t->DIER & 0x8)) || ((t->SR & 0x2) && (t->DIER & 0x2)) || ((t->SR & 0x4) && (t->DIER & 0x4)));
	qemu_set_irq(t->irq[0], conditions);

}

static void stm32f4_tim3_timer_callback_0(void *opaque) {
  static uint8_t period_reg_set_last = 0;
  STM32F4TIM3 *t = (STM32F4TIM3*)opaque;

  if (!period_reg_set_last && t->period_reg_set[0]) {
      period_reg_set_last = 1;
      t->CNT = 0;
  }
  t->CNT += 1;
  unsigned prd = t->period_set[0] ? t->cur_period[0] : t->ARR;
  if (t->CNT == prd) {
    t->cur_period[0] = t->ARR;
    t->CNT = 0;
    t->DIER |= 0x20;t->SR |= 0x80;t->SR |= 0x8;t->SR |= 0x2;t->DIER |= 0x80;t->SR |= 0x4;t->SR |= 0x1;t->DIER |= 0x10;t->SR |= 0x40;t->SR |= 0x20;t->DIER |= 0x8;t->DIER |= 0x4;t->DIER |= 0x2;t->DIER |= 0x1;t->DIER |= 0x40;t->SR |= 0x10;;
    qemu_set_irq(t->irq[0], 1);
    qemu_set_irq(t->irq[0], 0);
  }

  if (t->enabled[0]) {
    timer_mod(t->timers[0], qemu_clock_get_us(QEMU_CLOCK_VIRTUAL) + 1);
  }
}


static void stm32f4_tim3_register_reset(STM32F4TIM3 *t) {
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

static uint64_t stm32f4_tim3_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F4TIM3 *t = STM32F4_TIM3(opaque);
	uint64_t ret;

	switch (offset) {
		case A_TIM3_CR1:
			ret = t->CR1;
			break;
		case A_TIM3_CR2:
			ret = t->CR2;
			break;
		case A_TIM3_SMCR:
			ret = t->SMCR;
			break;
		case A_TIM3_DIER:
			ret = t->DIER;
			break;
		case A_TIM3_SR:
			ret = t->SR;
			break;
		case A_TIM3_CCMR1_Output:
			ret = t->CCMR1_Output;
			break;
		case A_TIM3_CCMR2_Output:
			ret = t->CCMR2_Output;
			break;
		case A_TIM3_CCER:
			ret = t->CCER;
			break;
		case A_TIM3_CNT:
			ret = t->CNT;
			break;
		case A_TIM3_PSC:
			ret = t->PSC;
			break;
		case A_TIM3_ARR:
			ret = t->ARR;
			break;
		case A_TIM3_CCR1:
			ret = t->CCR1;
			break;
		case A_TIM3_CCR2:
			ret = t->CCR2;
			break;
		case A_TIM3_CCR3:
			ret = t->CCR3;
			break;
		case A_TIM3_CCR4:
			ret = t->CCR4;
			break;
		case A_TIM3_DCR:
			ret = t->DCR;
			break;
		case A_TIM3_DMAR:
			ret = t->DMAR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F4 TIM3 read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f4_tim3_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F4TIM3 *t = STM32F4_TIM3(opaque);


	switch (offset) {
		case A_TIM3_CR1 ... A_TIM3_CR1 + 3:
			value = value << ((offset - A_TIM3_CR1) << 3);
			offset = A_TIM3_CR1;
			break;
		case A_TIM3_CR2 ... A_TIM3_CR2 + 3:
			value = value << ((offset - A_TIM3_CR2) << 3);
			offset = A_TIM3_CR2;
			break;
		case A_TIM3_SMCR ... A_TIM3_SMCR + 3:
			value = value << ((offset - A_TIM3_SMCR) << 3);
			offset = A_TIM3_SMCR;
			break;
		case A_TIM3_DIER ... A_TIM3_DIER + 3:
			value = value << ((offset - A_TIM3_DIER) << 3);
			offset = A_TIM3_DIER;
			break;
		case A_TIM3_SR ... A_TIM3_SR + 3:
			value = value << ((offset - A_TIM3_SR) << 3);
			offset = A_TIM3_SR;
			break;
		case A_TIM3_EGR ... A_TIM3_EGR + 3:
			value = value << ((offset - A_TIM3_EGR) << 3);
			offset = A_TIM3_EGR;
			break;
		case A_TIM3_CCMR1_Output ... A_TIM3_CCMR1_Output + 3:
			value = value << ((offset - A_TIM3_CCMR1_Output) << 3);
			offset = A_TIM3_CCMR1_Output;
			break;
		case A_TIM3_CCMR2_Output ... A_TIM3_CCMR2_Output + 3:
			value = value << ((offset - A_TIM3_CCMR2_Output) << 3);
			offset = A_TIM3_CCMR2_Output;
			break;
		case A_TIM3_CCER ... A_TIM3_CCER + 3:
			value = value << ((offset - A_TIM3_CCER) << 3);
			offset = A_TIM3_CCER;
			break;
		case A_TIM3_CNT ... A_TIM3_CNT + 3:
			value = value << ((offset - A_TIM3_CNT) << 3);
			offset = A_TIM3_CNT;
			break;
		case A_TIM3_PSC ... A_TIM3_PSC + 3:
			value = value << ((offset - A_TIM3_PSC) << 3);
			offset = A_TIM3_PSC;
			break;
		case A_TIM3_ARR ... A_TIM3_ARR + 3:
			value = value << ((offset - A_TIM3_ARR) << 3);
			offset = A_TIM3_ARR;
			break;
		case A_TIM3_CCR1 ... A_TIM3_CCR1 + 3:
			value = value << ((offset - A_TIM3_CCR1) << 3);
			offset = A_TIM3_CCR1;
			break;
		case A_TIM3_CCR2 ... A_TIM3_CCR2 + 3:
			value = value << ((offset - A_TIM3_CCR2) << 3);
			offset = A_TIM3_CCR2;
			break;
		case A_TIM3_CCR3 ... A_TIM3_CCR3 + 3:
			value = value << ((offset - A_TIM3_CCR3) << 3);
			offset = A_TIM3_CCR3;
			break;
		case A_TIM3_CCR4 ... A_TIM3_CCR4 + 3:
			value = value << ((offset - A_TIM3_CCR4) << 3);
			offset = A_TIM3_CCR4;
			break;
		case A_TIM3_DCR ... A_TIM3_DCR + 3:
			value = value << ((offset - A_TIM3_DCR) << 3);
			offset = A_TIM3_DCR;
			break;
		case A_TIM3_DMAR ... A_TIM3_DMAR + 3:
			value = value << ((offset - A_TIM3_DMAR) << 3);
			offset = A_TIM3_DMAR;
			break;

		default: break;
	}

	switch (offset) {
		case A_TIM3_CR1:
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
				t->timers[0] = timer_new(QEMU_CLOCK_VIRTUAL, SCALE_US, stm32f4_tim3_timer_callback_0, t);
			}
			break;
		case A_TIM3_CR2:
			t->CR2 = value;
			break;
		case A_TIM3_SMCR:
			t->SMCR = value;
			break;
		case A_TIM3_DIER:
			t->DIER = value;
			stm32f4_tim3_update(t);
			break;
		case A_TIM3_SR:
			t->SR = value;
			stm32f4_tim3_update(t);
			break;
		case A_TIM3_EGR:
			t->EGR = value;
			break;
		case A_TIM3_CCMR1_Output:
			t->CCMR1_Output = value;
			break;
		case A_TIM3_CCMR2_Output:
			t->CCMR2_Output = value;
			break;
		case A_TIM3_CCER:
			t->CCER = value;
			break;
		case A_TIM3_CNT:
			t->CNT = value;
			break;
		case A_TIM3_PSC:
			t->PSC = value;
			break;
		case A_TIM3_ARR:
			t->ARR = value;
			if (!t->period_reg_set[0]) {
				t->period_reg_set[0] = 1;
			}
			break;
		case A_TIM3_CCR1:
			t->CCR1 = value;
			break;
		case A_TIM3_CCR2:
			t->CCR2 = value;
			break;
		case A_TIM3_CCR3:
			t->CCR3 = value;
			break;
		case A_TIM3_CCR4:
			t->CCR4 = value;
			break;
		case A_TIM3_DCR:
			t->DCR = value;
			break;
		case A_TIM3_DMAR:
			t->DMAR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F4 TIM3 write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f4_tim3_ops = {
	.read = stm32f4_tim3_read,
	.write = stm32f4_tim3_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f4_tim3_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F4TIM3 *t = STM32F4_TIM3(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f4_tim3_ops, t, "stm32f4-tim3", STM32F4_TIM3_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32f4_tim3_realize(DeviceState *dev, Error **errp) {
	STM32F4TIM3 *t = STM32F4_TIM3(dev);
	t->timers[0] = timer_new(QEMU_CLOCK_VIRTUAL, SCALE_US, stm32f4_tim3_timer_callback_0, t);

}

static void stm32f4_tim3_reset_enter(Object *obj, ResetType type) {
	STM32F4TIM3 *t = STM32F4_TIM3(obj);
	stm32f4_tim3_register_reset(t);
}

static const VMStateDescription stm32f4_tim3_vmstate = {
	.name = "stm32f4-tim3",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR1, STM32F4TIM3),
		VMSTATE_UINT32(CR2, STM32F4TIM3),
		VMSTATE_UINT32(SMCR, STM32F4TIM3),
		VMSTATE_UINT32(DIER, STM32F4TIM3),
		VMSTATE_UINT32(SR, STM32F4TIM3),
		VMSTATE_UINT32(EGR, STM32F4TIM3),
		VMSTATE_UINT32(CCMR1_Output, STM32F4TIM3),
		VMSTATE_UINT32(CCMR1_Input, STM32F4TIM3),
		VMSTATE_UINT32(CCMR2_Output, STM32F4TIM3),
		VMSTATE_UINT32(CCMR2_Input, STM32F4TIM3),
		VMSTATE_UINT32(CCER, STM32F4TIM3),
		VMSTATE_UINT32(CNT, STM32F4TIM3),
		VMSTATE_UINT32(PSC, STM32F4TIM3),
		VMSTATE_UINT32(ARR, STM32F4TIM3),
		VMSTATE_UINT32(CCR1, STM32F4TIM3),
		VMSTATE_UINT32(CCR2, STM32F4TIM3),
		VMSTATE_UINT32(CCR3, STM32F4TIM3),
		VMSTATE_UINT32(CCR4, STM32F4TIM3),
		VMSTATE_UINT32(DCR, STM32F4TIM3),
		VMSTATE_UINT32(DMAR, STM32F4TIM3),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f4_tim3_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f4_tim3_vmstate;
	dc->realize = stm32f4_tim3_realize;
	rc->phases.enter = stm32f4_tim3_reset_enter;

}

static const TypeInfo stm32f4_tim3_info = {
	.name = TYPE_STM32F4_TIM3,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F4TIM3),
	.instance_init = stm32f4_tim3_init,
	.class_init = stm32f4_tim3_class_init,
};

static void stm32f4_tim3_register_types(void) {
	type_register_static(&stm32f4_tim3_info);
}

type_init(stm32f4_tim3_register_types);

#define TYPE_STM32F4_USART "stm32f4-usart"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F4USART, STM32F4_USART)

struct STM32F4USART {
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

	/* base */
	uint32_t base;

};
#define STM32F4_USART_SIZE				0x1c

REG32(USART_SR, 0x0)
	FIELD(USART_SR, CTS, 9, 1)
	FIELD(USART_SR, LBD, 8, 1)
	FIELD(USART_SR, TXE, 7, 1)
	FIELD(USART_SR, TC, 6, 1)
	FIELD(USART_SR, RXNE, 5, 1)
	FIELD(USART_SR, IDLE, 4, 1)
	FIELD(USART_SR, ORE, 3, 1)
	FIELD(USART_SR, NF, 2, 1)
	FIELD(USART_SR, FE, 1, 1)
	FIELD(USART_SR, PE, 0, 1)
REG32(USART_DR, 0x4)
	FIELD(USART_DR, DR, 0, 9)
REG32(USART_BRR, 0x8)
	FIELD(USART_BRR, DIV_Mantissa, 4, 12)
	FIELD(USART_BRR, DIV_Fraction, 0, 4)
REG32(USART_CR1, 0xc)
	FIELD(USART_CR1, OVER8, 15, 1)
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
REG32(USART_GTPR, 0x18)
	FIELD(USART_GTPR, GT, 8, 8)
	FIELD(USART_GTPR, PSC, 0, 8)


__attribute__((unused))
static void stm32f4_usart_update(STM32F4USART *t) {
	int conditions = (((t->CR1 & 0x20) && (t->SR & 0x20)) || ((!(t->CR3 & 0x1)) && (!(t->CR1 & 0x100)) && (!(t->CR1 & 0x20)) && (t->SR & 0x80) && (t->CR1 & 0x80)) || ((!(t->CR3 & 0x1)) && (!(t->CR1 & 0x100)) && (!(t->CR1 & 0x20)) && (t->SR & 0x40) && (t->CR1 & 0x40)) || ((!(t->SR & 0x4)) && (!(t->SR & 0x8)) && (!(t->SR & 0x2)) && (!(t->SR & 0x1)) && (t->SR & 0x40) && (t->CR1 & 0x40)) || ((!(t->CR3 & 0x1)) && (!(t->CR1 & 0x100)) && (!(t->SR & 0x10)) && (!(t->CR1 & 0x20)) && (t->SR & 0x80) && (t->CR1 & 0x80)) || ((!(t->SR & 0x4)) && (!(t->SR & 0x8)) && (!(t->SR & 0x2)) && (!(t->SR & 0x1)) && (t->SR & 0x80) && (t->CR1 & 0x80)) || ((!(t->CR3 & 0x1)) && (!(t->CR1 & 0x100)) && (!(t->SR & 0x10)) && (!(t->CR1 & 0x20)) && (t->SR & 0x40) && (t->CR1 & 0x40)) || ((t->SR & 0x1) && (!(t->CR3 & 0x40)) && (t->CR1 & 0x100)) || ((!(t->CR3 & 0x40)) && (t->CR1 & 0x100) && (t->SR & 0x2)) || ((!(t->CR3 & 0x40)) && (t->CR1 & 0x100) && (t->SR & 0x4)) || ((t->SR & 0x1) && (!(t->CR3 & 0x40)) && (t->CR3 & 0x1)) || ((!(t->CR3 & 0x40)) && (t->SR & 0x8) && (t->CR3 & 0x1)) || ((!(t->CR3 & 0x40)) && (t->SR & 0x8) && (t->CR1 & 0x20)) || ((!(t->CR3 & 0x40)) && (t->SR & 0x2) && (t->CR1 & 0x20)) || ((!(t->CR3 & 0x40)) && (t->SR & 0x4) && (t->CR1 & 0x20)) || ((t->SR & 0x1) && (!(t->CR3 & 0x40)) && (t->CR1 & 0x20)) || ((!(t->CR1 & 0x10)) && (!(t->SR & 0x4)) && (!(t->SR & 0x8)) && (!(t->SR & 0x2)) && (!(t->SR & 0x1)) && (t->SR & 0x80) && (t->CR1 & 0x80)) || ((!(t->CR3 & 0x1)) && (!(t->CR1 & 0x100)) && (!(t->CR1 & 0x10)) && (!(t->CR1 & 0x20)) && (t->SR & 0x40) && (t->CR1 & 0x40)) || ((!(t->CR3 & 0x40)) && (t->CR3 & 0x1) && (t->SR & 0x2)) || ((!(t->SR & 0x4)) && (!(t->SR & 0x8)) && (!(t->SR & 0x10)) && (!(t->SR & 0x2)) && (!(t->SR & 0x1)) && (t->SR & 0x40) && (t->CR1 & 0x40)) || ((!(t->CR1 & 0x10)) && (!(t->SR & 0x4)) && (!(t->SR & 0x8)) && (!(t->SR & 0x2)) && (!(t->SR & 0x1)) && (t->SR & 0x40) && (t->CR1 & 0x40)) || ((!(t->CR3 & 0x40)) && (t->SR & 0x8) && (t->CR1 & 0x100)) || ((!(t->CR3 & 0x1)) && (!(t->CR1 & 0x100)) && (!(t->CR1 & 0x10)) && (!(t->CR1 & 0x20)) && (t->SR & 0x80) && (t->CR1 & 0x80)) || ((t->SR & 0x10) && (!(t->CR3 & 0x40)) && (t->CR1 & 0x10)) || ((!(t->CR3 & 0x40)) && (t->CR3 & 0x1) && (t->SR & 0x4)) || ((!(t->SR & 0x4)) && (!(t->SR & 0x8)) && (!(t->SR & 0x10)) && (!(t->SR & 0x2)) && (!(t->SR & 0x1)) && (t->SR & 0x80) && (t->CR1 & 0x80)));
	qemu_set_irq(t->irq[0], conditions);

}

static int stm32f4_usart_can_receive(void *opaque) {
	return 1;
}

static void stm32f4_usart_receive(void *opaque, const uint8_t *buf, int size) {
	STM32F4USART *t = STM32F4_USART(opaque);

	t->DR = *buf;
	t->SR |= 0x20;

	stm32f4_usart_update(t);
}

static gboolean stm32f4_usart_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	STM32F4USART *t = STM32F4_USART(opaque);
	int ret;

	t->watch_tag = 0;
	t->SR &= (~(0x80));
	t->SR &= (~(0x40));

	stm32f4_usart_update(t);

	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->DR), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, stm32f4_usart_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->SR |= 0x80;
	t->SR |= 0x40;

	stm32f4_usart_update(t);

	return FALSE;
}

static void stm32f4_usart_register_reset(STM32F4USART *t) {
	t->SR = 0xc00000;
	t->DR = 0x0;
	t->BRR = 0x0;
	t->CR1 = 0x0;
	t->CR2 = 0x0;
	t->CR3 = 0x0;
	t->GTPR = 0x0;
	t->SR |= 0x80;
	t->SR |= 0x40;

}

static uint64_t stm32f4_usart_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F4USART *t = STM32F4_USART(opaque);
	uint64_t ret;

	switch (offset) {
		case A_USART_SR:
			ret = t->SR;
			break;
		case A_USART_DR:
			ret = t->DR;
			t->SR &= (~(0x20));
			qemu_chr_fe_accept_input(&(t->chr));
			stm32f4_usart_update(t);
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
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F4 USART read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f4_usart_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F4USART *t = STM32F4_USART(opaque);


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
			stm32f4_usart_update(t);
			break;
		case A_USART_DR:
			t->DR = value;
			stm32f4_usart_transmit(NULL, G_IO_OUT, t);
			break;
		case A_USART_BRR:
			t->BRR = value;
			break;
		case A_USART_CR1:
			t->CR1 = value;
			stm32f4_usart_update(t);
			break;
		case A_USART_CR2:
			t->CR2 = value;
			break;
		case A_USART_CR3:
			t->CR3 = value;
			stm32f4_usart_update(t);
			break;
		case A_USART_GTPR:
			t->GTPR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F4 USART write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f4_usart_ops = {
	.read = stm32f4_usart_read,
	.write = stm32f4_usart_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f4_usart_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F4USART *t = STM32F4_USART(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f4_usart_ops, t, "stm32f4-usart", STM32F4_USART_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32f4_usart_realize(DeviceState *dev, Error **errp) {
	STM32F4USART *t = STM32F4_USART(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), stm32f4_usart_can_receive, stm32f4_usart_receive,
		NULL, NULL, t, NULL, true);

}

static void stm32f4_usart_reset_enter(Object *obj, ResetType type) {
	STM32F4USART *t = STM32F4_USART(obj);
	stm32f4_usart_register_reset(t);
}

static const VMStateDescription stm32f4_usart_vmstate = {
	.name = "stm32f4-usart",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(SR, STM32F4USART),
		VMSTATE_UINT32(DR, STM32F4USART),
		VMSTATE_UINT32(BRR, STM32F4USART),
		VMSTATE_UINT32(CR1, STM32F4USART),
		VMSTATE_UINT32(CR2, STM32F4USART),
		VMSTATE_UINT32(CR3, STM32F4USART),
		VMSTATE_UINT32(GTPR, STM32F4USART),

		VMSTATE_END_OF_LIST()
	}
};

static Property stm32f4_usart_properties[] = {
	DEFINE_PROP_CHR("chardev", STM32F4USART, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void stm32f4_usart_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f4_usart_vmstate;
	dc->realize = stm32f4_usart_realize;
	rc->phases.enter = stm32f4_usart_reset_enter;
	device_class_set_props(dc, stm32f4_usart_properties);

}

static const TypeInfo stm32f4_usart_info = {
	.name = TYPE_STM32F4_USART,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F4USART),
	.instance_init = stm32f4_usart_init,
	.class_init = stm32f4_usart_class_init,
};

static void stm32f4_usart_register_types(void) {
	type_register_static(&stm32f4_usart_info);
}

type_init(stm32f4_usart_register_types);

#define TYPE_STM32F4_GPIOA "stm32f4-gpioa"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F4GPIOA, STM32F4_GPIOA)

struct STM32F4GPIOA {
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
	/* base */
	uint32_t base;

};
#define STM32F4_GPIOA_SIZE				0x28

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


static void stm32f4_gpioa_register_reset(STM32F4GPIOA *t) {
	t->MODER = 0xa8000000;
	t->OTYPER = 0x0;
	t->OSPEEDR = 0x0;
	t->PUPDR = 0x64000000;
	t->IDR = 0x0;
	t->ODR = 0x0;
	t->BSRR = 0x0;
	t->LCKR = 0x0;
	t->AFRL = 0x0;
	t->AFRH = 0x0;

}

static uint64_t stm32f4_gpioa_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F4GPIOA *t = STM32F4_GPIOA(opaque);
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
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F4 GPIOA read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f4_gpioa_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F4GPIOA *t = STM32F4_GPIOA(opaque);


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

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F4 GPIOA write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f4_gpioa_ops = {
	.read = stm32f4_gpioa_read,
	.write = stm32f4_gpioa_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f4_gpioa_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F4GPIOA *t = STM32F4_GPIOA(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f4_gpioa_ops, t, "stm32f4-gpioa", STM32F4_GPIOA_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void stm32f4_gpioa_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32f4_gpioa_reset_enter(Object *obj, ResetType type) {
	STM32F4GPIOA *t = STM32F4_GPIOA(obj);
	stm32f4_gpioa_register_reset(t);
}

static const VMStateDescription stm32f4_gpioa_vmstate = {
	.name = "stm32f4-gpioa",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(MODER, STM32F4GPIOA),
		VMSTATE_UINT32(OTYPER, STM32F4GPIOA),
		VMSTATE_UINT32(OSPEEDR, STM32F4GPIOA),
		VMSTATE_UINT32(PUPDR, STM32F4GPIOA),
		VMSTATE_UINT32(IDR, STM32F4GPIOA),
		VMSTATE_UINT32(ODR, STM32F4GPIOA),
		VMSTATE_UINT32(BSRR, STM32F4GPIOA),
		VMSTATE_UINT32(LCKR, STM32F4GPIOA),
		VMSTATE_UINT32(AFRL, STM32F4GPIOA),
		VMSTATE_UINT32(AFRH, STM32F4GPIOA),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f4_gpioa_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f4_gpioa_vmstate;
	dc->realize = stm32f4_gpioa_realize;
	rc->phases.enter = stm32f4_gpioa_reset_enter;

}

static const TypeInfo stm32f4_gpioa_info = {
	.name = TYPE_STM32F4_GPIOA,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F4GPIOA),
	.instance_init = stm32f4_gpioa_init,
	.class_init = stm32f4_gpioa_class_init,
};

static void stm32f4_gpioa_register_types(void) {
	type_register_static(&stm32f4_gpioa_info);
}

type_init(stm32f4_gpioa_register_types);

#define TYPE_STM32F4_GPIOB "stm32f4-gpiob"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F4GPIOB, STM32F4_GPIOB)

struct STM32F4GPIOB {
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
	/* base */
	uint32_t base;

};
#define STM32F4_GPIOB_SIZE				0x28

REG32(GPIOB_MODER, 0x0)
	FIELD(GPIOB_MODER, MODER15, 30, 2)
	FIELD(GPIOB_MODER, MODER14, 28, 2)
	FIELD(GPIOB_MODER, MODER13, 26, 2)
	FIELD(GPIOB_MODER, MODER12, 24, 2)
	FIELD(GPIOB_MODER, MODER11, 22, 2)
	FIELD(GPIOB_MODER, MODER10, 20, 2)
	FIELD(GPIOB_MODER, MODER9, 18, 2)
	FIELD(GPIOB_MODER, MODER8, 16, 2)
	FIELD(GPIOB_MODER, MODER7, 14, 2)
	FIELD(GPIOB_MODER, MODER6, 12, 2)
	FIELD(GPIOB_MODER, MODER5, 10, 2)
	FIELD(GPIOB_MODER, MODER4, 8, 2)
	FIELD(GPIOB_MODER, MODER3, 6, 2)
	FIELD(GPIOB_MODER, MODER2, 4, 2)
	FIELD(GPIOB_MODER, MODER1, 2, 2)
	FIELD(GPIOB_MODER, MODER0, 0, 2)
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
	FIELD(GPIOB_OSPEEDR, OSPEEDR15, 30, 2)
	FIELD(GPIOB_OSPEEDR, OSPEEDR14, 28, 2)
	FIELD(GPIOB_OSPEEDR, OSPEEDR13, 26, 2)
	FIELD(GPIOB_OSPEEDR, OSPEEDR12, 24, 2)
	FIELD(GPIOB_OSPEEDR, OSPEEDR11, 22, 2)
	FIELD(GPIOB_OSPEEDR, OSPEEDR10, 20, 2)
	FIELD(GPIOB_OSPEEDR, OSPEEDR9, 18, 2)
	FIELD(GPIOB_OSPEEDR, OSPEEDR8, 16, 2)
	FIELD(GPIOB_OSPEEDR, OSPEEDR7, 14, 2)
	FIELD(GPIOB_OSPEEDR, OSPEEDR6, 12, 2)
	FIELD(GPIOB_OSPEEDR, OSPEEDR5, 10, 2)
	FIELD(GPIOB_OSPEEDR, OSPEEDR4, 8, 2)
	FIELD(GPIOB_OSPEEDR, OSPEEDR3, 6, 2)
	FIELD(GPIOB_OSPEEDR, OSPEEDR2, 4, 2)
	FIELD(GPIOB_OSPEEDR, OSPEEDR1, 2, 2)
	FIELD(GPIOB_OSPEEDR, OSPEEDR0, 0, 2)
REG32(GPIOB_PUPDR, 0xc)
	FIELD(GPIOB_PUPDR, PUPDR15, 30, 2)
	FIELD(GPIOB_PUPDR, PUPDR14, 28, 2)
	FIELD(GPIOB_PUPDR, PUPDR13, 26, 2)
	FIELD(GPIOB_PUPDR, PUPDR12, 24, 2)
	FIELD(GPIOB_PUPDR, PUPDR11, 22, 2)
	FIELD(GPIOB_PUPDR, PUPDR10, 20, 2)
	FIELD(GPIOB_PUPDR, PUPDR9, 18, 2)
	FIELD(GPIOB_PUPDR, PUPDR8, 16, 2)
	FIELD(GPIOB_PUPDR, PUPDR7, 14, 2)
	FIELD(GPIOB_PUPDR, PUPDR6, 12, 2)
	FIELD(GPIOB_PUPDR, PUPDR5, 10, 2)
	FIELD(GPIOB_PUPDR, PUPDR4, 8, 2)
	FIELD(GPIOB_PUPDR, PUPDR3, 6, 2)
	FIELD(GPIOB_PUPDR, PUPDR2, 4, 2)
	FIELD(GPIOB_PUPDR, PUPDR1, 2, 2)
	FIELD(GPIOB_PUPDR, PUPDR0, 0, 2)
REG32(GPIOB_IDR, 0x10)
	FIELD(GPIOB_IDR, IDR15, 15, 1)
	FIELD(GPIOB_IDR, IDR14, 14, 1)
	FIELD(GPIOB_IDR, IDR13, 13, 1)
	FIELD(GPIOB_IDR, IDR12, 12, 1)
	FIELD(GPIOB_IDR, IDR11, 11, 1)
	FIELD(GPIOB_IDR, IDR10, 10, 1)
	FIELD(GPIOB_IDR, IDR9, 9, 1)
	FIELD(GPIOB_IDR, IDR8, 8, 1)
	FIELD(GPIOB_IDR, IDR7, 7, 1)
	FIELD(GPIOB_IDR, IDR6, 6, 1)
	FIELD(GPIOB_IDR, IDR5, 5, 1)
	FIELD(GPIOB_IDR, IDR4, 4, 1)
	FIELD(GPIOB_IDR, IDR3, 3, 1)
	FIELD(GPIOB_IDR, IDR2, 2, 1)
	FIELD(GPIOB_IDR, IDR1, 1, 1)
	FIELD(GPIOB_IDR, IDR0, 0, 1)
REG32(GPIOB_ODR, 0x14)
	FIELD(GPIOB_ODR, ODR15, 15, 1)
	FIELD(GPIOB_ODR, ODR14, 14, 1)
	FIELD(GPIOB_ODR, ODR13, 13, 1)
	FIELD(GPIOB_ODR, ODR12, 12, 1)
	FIELD(GPIOB_ODR, ODR11, 11, 1)
	FIELD(GPIOB_ODR, ODR10, 10, 1)
	FIELD(GPIOB_ODR, ODR9, 9, 1)
	FIELD(GPIOB_ODR, ODR8, 8, 1)
	FIELD(GPIOB_ODR, ODR7, 7, 1)
	FIELD(GPIOB_ODR, ODR6, 6, 1)
	FIELD(GPIOB_ODR, ODR5, 5, 1)
	FIELD(GPIOB_ODR, ODR4, 4, 1)
	FIELD(GPIOB_ODR, ODR3, 3, 1)
	FIELD(GPIOB_ODR, ODR2, 2, 1)
	FIELD(GPIOB_ODR, ODR1, 1, 1)
	FIELD(GPIOB_ODR, ODR0, 0, 1)
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
	FIELD(GPIOB_AFRL, AFRL7, 28, 4)
	FIELD(GPIOB_AFRL, AFRL6, 24, 4)
	FIELD(GPIOB_AFRL, AFRL5, 20, 4)
	FIELD(GPIOB_AFRL, AFRL4, 16, 4)
	FIELD(GPIOB_AFRL, AFRL3, 12, 4)
	FIELD(GPIOB_AFRL, AFRL2, 8, 4)
	FIELD(GPIOB_AFRL, AFRL1, 4, 4)
	FIELD(GPIOB_AFRL, AFRL0, 0, 4)
REG32(GPIOB_AFRH, 0x24)
	FIELD(GPIOB_AFRH, AFRH15, 28, 4)
	FIELD(GPIOB_AFRH, AFRH14, 24, 4)
	FIELD(GPIOB_AFRH, AFRH13, 20, 4)
	FIELD(GPIOB_AFRH, AFRH12, 16, 4)
	FIELD(GPIOB_AFRH, AFRH11, 12, 4)
	FIELD(GPIOB_AFRH, AFRH10, 8, 4)
	FIELD(GPIOB_AFRH, AFRH9, 4, 4)
	FIELD(GPIOB_AFRH, AFRH8, 0, 4)


static void stm32f4_gpiob_register_reset(STM32F4GPIOB *t) {
	t->MODER = 0x280;
	t->OTYPER = 0x0;
	t->OSPEEDR = 0xc0;
	t->PUPDR = 0x100;
	t->IDR = 0x0;
	t->ODR = 0x0;
	t->BSRR = 0x0;
	t->LCKR = 0x0;
	t->AFRL = 0x0;
	t->AFRH = 0x0;

}

static uint64_t stm32f4_gpiob_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F4GPIOB *t = STM32F4_GPIOB(opaque);
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
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F4 GPIOB read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f4_gpiob_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F4GPIOB *t = STM32F4_GPIOB(opaque);


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

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F4 GPIOB write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f4_gpiob_ops = {
	.read = stm32f4_gpiob_read,
	.write = stm32f4_gpiob_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f4_gpiob_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F4GPIOB *t = STM32F4_GPIOB(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f4_gpiob_ops, t, "stm32f4-gpiob", STM32F4_GPIOB_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void stm32f4_gpiob_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32f4_gpiob_reset_enter(Object *obj, ResetType type) {
	STM32F4GPIOB *t = STM32F4_GPIOB(obj);
	stm32f4_gpiob_register_reset(t);
}

static const VMStateDescription stm32f4_gpiob_vmstate = {
	.name = "stm32f4-gpiob",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(MODER, STM32F4GPIOB),
		VMSTATE_UINT32(OTYPER, STM32F4GPIOB),
		VMSTATE_UINT32(OSPEEDR, STM32F4GPIOB),
		VMSTATE_UINT32(PUPDR, STM32F4GPIOB),
		VMSTATE_UINT32(IDR, STM32F4GPIOB),
		VMSTATE_UINT32(ODR, STM32F4GPIOB),
		VMSTATE_UINT32(BSRR, STM32F4GPIOB),
		VMSTATE_UINT32(LCKR, STM32F4GPIOB),
		VMSTATE_UINT32(AFRL, STM32F4GPIOB),
		VMSTATE_UINT32(AFRH, STM32F4GPIOB),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f4_gpiob_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f4_gpiob_vmstate;
	dc->realize = stm32f4_gpiob_realize;
	rc->phases.enter = stm32f4_gpiob_reset_enter;

}

static const TypeInfo stm32f4_gpiob_info = {
	.name = TYPE_STM32F4_GPIOB,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F4GPIOB),
	.instance_init = stm32f4_gpiob_init,
	.class_init = stm32f4_gpiob_class_init,
};

static void stm32f4_gpiob_register_types(void) {
	type_register_static(&stm32f4_gpiob_info);
}

type_init(stm32f4_gpiob_register_types);

#define TYPE_STM32F4_GPIOI "stm32f4-gpioi"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F4GPIOI, STM32F4_GPIOI)

struct STM32F4GPIOI {
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
	/* base */
	uint32_t base;

};
#define STM32F4_GPIOI_SIZE				0x28

REG32(GPIOI_MODER, 0x0)
	FIELD(GPIOI_MODER, MODER15, 30, 2)
	FIELD(GPIOI_MODER, MODER14, 28, 2)
	FIELD(GPIOI_MODER, MODER13, 26, 2)
	FIELD(GPIOI_MODER, MODER12, 24, 2)
	FIELD(GPIOI_MODER, MODER11, 22, 2)
	FIELD(GPIOI_MODER, MODER10, 20, 2)
	FIELD(GPIOI_MODER, MODER9, 18, 2)
	FIELD(GPIOI_MODER, MODER8, 16, 2)
	FIELD(GPIOI_MODER, MODER7, 14, 2)
	FIELD(GPIOI_MODER, MODER6, 12, 2)
	FIELD(GPIOI_MODER, MODER5, 10, 2)
	FIELD(GPIOI_MODER, MODER4, 8, 2)
	FIELD(GPIOI_MODER, MODER3, 6, 2)
	FIELD(GPIOI_MODER, MODER2, 4, 2)
	FIELD(GPIOI_MODER, MODER1, 2, 2)
	FIELD(GPIOI_MODER, MODER0, 0, 2)
REG32(GPIOI_OTYPER, 0x4)
	FIELD(GPIOI_OTYPER, OT15, 15, 1)
	FIELD(GPIOI_OTYPER, OT14, 14, 1)
	FIELD(GPIOI_OTYPER, OT13, 13, 1)
	FIELD(GPIOI_OTYPER, OT12, 12, 1)
	FIELD(GPIOI_OTYPER, OT11, 11, 1)
	FIELD(GPIOI_OTYPER, OT10, 10, 1)
	FIELD(GPIOI_OTYPER, OT9, 9, 1)
	FIELD(GPIOI_OTYPER, OT8, 8, 1)
	FIELD(GPIOI_OTYPER, OT7, 7, 1)
	FIELD(GPIOI_OTYPER, OT6, 6, 1)
	FIELD(GPIOI_OTYPER, OT5, 5, 1)
	FIELD(GPIOI_OTYPER, OT4, 4, 1)
	FIELD(GPIOI_OTYPER, OT3, 3, 1)
	FIELD(GPIOI_OTYPER, OT2, 2, 1)
	FIELD(GPIOI_OTYPER, OT1, 1, 1)
	FIELD(GPIOI_OTYPER, OT0, 0, 1)
REG32(GPIOI_OSPEEDR, 0x8)
	FIELD(GPIOI_OSPEEDR, OSPEEDR15, 30, 2)
	FIELD(GPIOI_OSPEEDR, OSPEEDR14, 28, 2)
	FIELD(GPIOI_OSPEEDR, OSPEEDR13, 26, 2)
	FIELD(GPIOI_OSPEEDR, OSPEEDR12, 24, 2)
	FIELD(GPIOI_OSPEEDR, OSPEEDR11, 22, 2)
	FIELD(GPIOI_OSPEEDR, OSPEEDR10, 20, 2)
	FIELD(GPIOI_OSPEEDR, OSPEEDR9, 18, 2)
	FIELD(GPIOI_OSPEEDR, OSPEEDR8, 16, 2)
	FIELD(GPIOI_OSPEEDR, OSPEEDR7, 14, 2)
	FIELD(GPIOI_OSPEEDR, OSPEEDR6, 12, 2)
	FIELD(GPIOI_OSPEEDR, OSPEEDR5, 10, 2)
	FIELD(GPIOI_OSPEEDR, OSPEEDR4, 8, 2)
	FIELD(GPIOI_OSPEEDR, OSPEEDR3, 6, 2)
	FIELD(GPIOI_OSPEEDR, OSPEEDR2, 4, 2)
	FIELD(GPIOI_OSPEEDR, OSPEEDR1, 2, 2)
	FIELD(GPIOI_OSPEEDR, OSPEEDR0, 0, 2)
REG32(GPIOI_PUPDR, 0xc)
	FIELD(GPIOI_PUPDR, PUPDR15, 30, 2)
	FIELD(GPIOI_PUPDR, PUPDR14, 28, 2)
	FIELD(GPIOI_PUPDR, PUPDR13, 26, 2)
	FIELD(GPIOI_PUPDR, PUPDR12, 24, 2)
	FIELD(GPIOI_PUPDR, PUPDR11, 22, 2)
	FIELD(GPIOI_PUPDR, PUPDR10, 20, 2)
	FIELD(GPIOI_PUPDR, PUPDR9, 18, 2)
	FIELD(GPIOI_PUPDR, PUPDR8, 16, 2)
	FIELD(GPIOI_PUPDR, PUPDR7, 14, 2)
	FIELD(GPIOI_PUPDR, PUPDR6, 12, 2)
	FIELD(GPIOI_PUPDR, PUPDR5, 10, 2)
	FIELD(GPIOI_PUPDR, PUPDR4, 8, 2)
	FIELD(GPIOI_PUPDR, PUPDR3, 6, 2)
	FIELD(GPIOI_PUPDR, PUPDR2, 4, 2)
	FIELD(GPIOI_PUPDR, PUPDR1, 2, 2)
	FIELD(GPIOI_PUPDR, PUPDR0, 0, 2)
REG32(GPIOI_IDR, 0x10)
	FIELD(GPIOI_IDR, IDR15, 15, 1)
	FIELD(GPIOI_IDR, IDR14, 14, 1)
	FIELD(GPIOI_IDR, IDR13, 13, 1)
	FIELD(GPIOI_IDR, IDR12, 12, 1)
	FIELD(GPIOI_IDR, IDR11, 11, 1)
	FIELD(GPIOI_IDR, IDR10, 10, 1)
	FIELD(GPIOI_IDR, IDR9, 9, 1)
	FIELD(GPIOI_IDR, IDR8, 8, 1)
	FIELD(GPIOI_IDR, IDR7, 7, 1)
	FIELD(GPIOI_IDR, IDR6, 6, 1)
	FIELD(GPIOI_IDR, IDR5, 5, 1)
	FIELD(GPIOI_IDR, IDR4, 4, 1)
	FIELD(GPIOI_IDR, IDR3, 3, 1)
	FIELD(GPIOI_IDR, IDR2, 2, 1)
	FIELD(GPIOI_IDR, IDR1, 1, 1)
	FIELD(GPIOI_IDR, IDR0, 0, 1)
REG32(GPIOI_ODR, 0x14)
	FIELD(GPIOI_ODR, ODR15, 15, 1)
	FIELD(GPIOI_ODR, ODR14, 14, 1)
	FIELD(GPIOI_ODR, ODR13, 13, 1)
	FIELD(GPIOI_ODR, ODR12, 12, 1)
	FIELD(GPIOI_ODR, ODR11, 11, 1)
	FIELD(GPIOI_ODR, ODR10, 10, 1)
	FIELD(GPIOI_ODR, ODR9, 9, 1)
	FIELD(GPIOI_ODR, ODR8, 8, 1)
	FIELD(GPIOI_ODR, ODR7, 7, 1)
	FIELD(GPIOI_ODR, ODR6, 6, 1)
	FIELD(GPIOI_ODR, ODR5, 5, 1)
	FIELD(GPIOI_ODR, ODR4, 4, 1)
	FIELD(GPIOI_ODR, ODR3, 3, 1)
	FIELD(GPIOI_ODR, ODR2, 2, 1)
	FIELD(GPIOI_ODR, ODR1, 1, 1)
	FIELD(GPIOI_ODR, ODR0, 0, 1)
REG32(GPIOI_BSRR, 0x18)
	FIELD(GPIOI_BSRR, BR15, 31, 1)
	FIELD(GPIOI_BSRR, BR14, 30, 1)
	FIELD(GPIOI_BSRR, BR13, 29, 1)
	FIELD(GPIOI_BSRR, BR12, 28, 1)
	FIELD(GPIOI_BSRR, BR11, 27, 1)
	FIELD(GPIOI_BSRR, BR10, 26, 1)
	FIELD(GPIOI_BSRR, BR9, 25, 1)
	FIELD(GPIOI_BSRR, BR8, 24, 1)
	FIELD(GPIOI_BSRR, BR7, 23, 1)
	FIELD(GPIOI_BSRR, BR6, 22, 1)
	FIELD(GPIOI_BSRR, BR5, 21, 1)
	FIELD(GPIOI_BSRR, BR4, 20, 1)
	FIELD(GPIOI_BSRR, BR3, 19, 1)
	FIELD(GPIOI_BSRR, BR2, 18, 1)
	FIELD(GPIOI_BSRR, BR1, 17, 1)
	FIELD(GPIOI_BSRR, BR0, 16, 1)
	FIELD(GPIOI_BSRR, BS15, 15, 1)
	FIELD(GPIOI_BSRR, BS14, 14, 1)
	FIELD(GPIOI_BSRR, BS13, 13, 1)
	FIELD(GPIOI_BSRR, BS12, 12, 1)
	FIELD(GPIOI_BSRR, BS11, 11, 1)
	FIELD(GPIOI_BSRR, BS10, 10, 1)
	FIELD(GPIOI_BSRR, BS9, 9, 1)
	FIELD(GPIOI_BSRR, BS8, 8, 1)
	FIELD(GPIOI_BSRR, BS7, 7, 1)
	FIELD(GPIOI_BSRR, BS6, 6, 1)
	FIELD(GPIOI_BSRR, BS5, 5, 1)
	FIELD(GPIOI_BSRR, BS4, 4, 1)
	FIELD(GPIOI_BSRR, BS3, 3, 1)
	FIELD(GPIOI_BSRR, BS2, 2, 1)
	FIELD(GPIOI_BSRR, BS1, 1, 1)
	FIELD(GPIOI_BSRR, BS0, 0, 1)
REG32(GPIOI_LCKR, 0x1c)
	FIELD(GPIOI_LCKR, LCKK, 16, 1)
	FIELD(GPIOI_LCKR, LCK15, 15, 1)
	FIELD(GPIOI_LCKR, LCK14, 14, 1)
	FIELD(GPIOI_LCKR, LCK13, 13, 1)
	FIELD(GPIOI_LCKR, LCK12, 12, 1)
	FIELD(GPIOI_LCKR, LCK11, 11, 1)
	FIELD(GPIOI_LCKR, LCK10, 10, 1)
	FIELD(GPIOI_LCKR, LCK9, 9, 1)
	FIELD(GPIOI_LCKR, LCK8, 8, 1)
	FIELD(GPIOI_LCKR, LCK7, 7, 1)
	FIELD(GPIOI_LCKR, LCK6, 6, 1)
	FIELD(GPIOI_LCKR, LCK5, 5, 1)
	FIELD(GPIOI_LCKR, LCK4, 4, 1)
	FIELD(GPIOI_LCKR, LCK3, 3, 1)
	FIELD(GPIOI_LCKR, LCK2, 2, 1)
	FIELD(GPIOI_LCKR, LCK1, 1, 1)
	FIELD(GPIOI_LCKR, LCK0, 0, 1)
REG32(GPIOI_AFRL, 0x20)
	FIELD(GPIOI_AFRL, AFRL7, 28, 4)
	FIELD(GPIOI_AFRL, AFRL6, 24, 4)
	FIELD(GPIOI_AFRL, AFRL5, 20, 4)
	FIELD(GPIOI_AFRL, AFRL4, 16, 4)
	FIELD(GPIOI_AFRL, AFRL3, 12, 4)
	FIELD(GPIOI_AFRL, AFRL2, 8, 4)
	FIELD(GPIOI_AFRL, AFRL1, 4, 4)
	FIELD(GPIOI_AFRL, AFRL0, 0, 4)
REG32(GPIOI_AFRH, 0x24)
	FIELD(GPIOI_AFRH, AFRH15, 28, 4)
	FIELD(GPIOI_AFRH, AFRH14, 24, 4)
	FIELD(GPIOI_AFRH, AFRH13, 20, 4)
	FIELD(GPIOI_AFRH, AFRH12, 16, 4)
	FIELD(GPIOI_AFRH, AFRH11, 12, 4)
	FIELD(GPIOI_AFRH, AFRH10, 8, 4)
	FIELD(GPIOI_AFRH, AFRH9, 4, 4)
	FIELD(GPIOI_AFRH, AFRH8, 0, 4)


static void stm32f4_gpioi_register_reset(STM32F4GPIOI *t) {
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

}

static uint64_t stm32f4_gpioi_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F4GPIOI *t = STM32F4_GPIOI(opaque);
	uint64_t ret;

	switch (offset) {
		case A_GPIOI_MODER:
			ret = t->MODER;
			break;
		case A_GPIOI_OTYPER:
			ret = t->OTYPER;
			break;
		case A_GPIOI_OSPEEDR:
			ret = t->OSPEEDR;
			break;
		case A_GPIOI_PUPDR:
			ret = t->PUPDR;
			break;
		case A_GPIOI_IDR:
			ret = t->IDR;
			break;
		case A_GPIOI_ODR:
			ret = t->ODR;
			break;
		case A_GPIOI_LCKR:
			ret = t->LCKR;
			break;
		case A_GPIOI_AFRL:
			ret = t->AFRL;
			break;
		case A_GPIOI_AFRH:
			ret = t->AFRH;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F4 GPIOI read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f4_gpioi_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F4GPIOI *t = STM32F4_GPIOI(opaque);


	switch (offset) {
		case A_GPIOI_MODER ... A_GPIOI_MODER + 3:
			value = value << ((offset - A_GPIOI_MODER) << 3);
			offset = A_GPIOI_MODER;
			break;
		case A_GPIOI_OTYPER ... A_GPIOI_OTYPER + 3:
			value = value << ((offset - A_GPIOI_OTYPER) << 3);
			offset = A_GPIOI_OTYPER;
			break;
		case A_GPIOI_OSPEEDR ... A_GPIOI_OSPEEDR + 3:
			value = value << ((offset - A_GPIOI_OSPEEDR) << 3);
			offset = A_GPIOI_OSPEEDR;
			break;
		case A_GPIOI_PUPDR ... A_GPIOI_PUPDR + 3:
			value = value << ((offset - A_GPIOI_PUPDR) << 3);
			offset = A_GPIOI_PUPDR;
			break;
		case A_GPIOI_ODR ... A_GPIOI_ODR + 3:
			value = value << ((offset - A_GPIOI_ODR) << 3);
			offset = A_GPIOI_ODR;
			break;
		case A_GPIOI_BSRR ... A_GPIOI_BSRR + 3:
			value = value << ((offset - A_GPIOI_BSRR) << 3);
			offset = A_GPIOI_BSRR;
			break;
		case A_GPIOI_LCKR ... A_GPIOI_LCKR + 3:
			value = value << ((offset - A_GPIOI_LCKR) << 3);
			offset = A_GPIOI_LCKR;
			break;
		case A_GPIOI_AFRL ... A_GPIOI_AFRL + 3:
			value = value << ((offset - A_GPIOI_AFRL) << 3);
			offset = A_GPIOI_AFRL;
			break;
		case A_GPIOI_AFRH ... A_GPIOI_AFRH + 3:
			value = value << ((offset - A_GPIOI_AFRH) << 3);
			offset = A_GPIOI_AFRH;
			break;

		default: break;
	}

	switch (offset) {
		case A_GPIOI_MODER:
			t->MODER = value;
			break;
		case A_GPIOI_OTYPER:
			t->OTYPER = value;
			break;
		case A_GPIOI_OSPEEDR:
			t->OSPEEDR = value;
			break;
		case A_GPIOI_PUPDR:
			t->PUPDR = value;
			break;
		case A_GPIOI_ODR:
			t->ODR = value;
			break;
		case A_GPIOI_BSRR:
			t->BSRR = value;
			break;
		case A_GPIOI_LCKR:
			t->LCKR = value;
			break;
		case A_GPIOI_AFRL:
			t->AFRL = value;
			break;
		case A_GPIOI_AFRH:
			t->AFRH = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F4 GPIOI write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f4_gpioi_ops = {
	.read = stm32f4_gpioi_read,
	.write = stm32f4_gpioi_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f4_gpioi_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F4GPIOI *t = STM32F4_GPIOI(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f4_gpioi_ops, t, "stm32f4-gpioi", STM32F4_GPIOI_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void stm32f4_gpioi_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32f4_gpioi_reset_enter(Object *obj, ResetType type) {
	STM32F4GPIOI *t = STM32F4_GPIOI(obj);
	stm32f4_gpioi_register_reset(t);
}

static const VMStateDescription stm32f4_gpioi_vmstate = {
	.name = "stm32f4-gpioi",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(MODER, STM32F4GPIOI),
		VMSTATE_UINT32(OTYPER, STM32F4GPIOI),
		VMSTATE_UINT32(OSPEEDR, STM32F4GPIOI),
		VMSTATE_UINT32(PUPDR, STM32F4GPIOI),
		VMSTATE_UINT32(IDR, STM32F4GPIOI),
		VMSTATE_UINT32(ODR, STM32F4GPIOI),
		VMSTATE_UINT32(BSRR, STM32F4GPIOI),
		VMSTATE_UINT32(LCKR, STM32F4GPIOI),
		VMSTATE_UINT32(AFRL, STM32F4GPIOI),
		VMSTATE_UINT32(AFRH, STM32F4GPIOI),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f4_gpioi_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f4_gpioi_vmstate;
	dc->realize = stm32f4_gpioi_realize;
	rc->phases.enter = stm32f4_gpioi_reset_enter;

}

static const TypeInfo stm32f4_gpioi_info = {
	.name = TYPE_STM32F4_GPIOI,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F4GPIOI),
	.instance_init = stm32f4_gpioi_init,
	.class_init = stm32f4_gpioi_class_init,
};

static void stm32f4_gpioi_register_types(void) {
	type_register_static(&stm32f4_gpioi_info);
}

type_init(stm32f4_gpioi_register_types);

#define TYPE_STM32F4_RNG "stm32f4-rng"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F4RNG, STM32F4_RNG)

struct STM32F4RNG {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t CR;
	uint32_t SR;
	uint32_t DR;
	/* base */
	uint32_t base;

};
#define STM32F4_RNG_SIZE				0xc

REG32(RNG_CR, 0x0)
	FIELD(RNG_CR, IE, 3, 1)
	FIELD(RNG_CR, RNGEN, 2, 1)
REG32(RNG_SR, 0x4)
	FIELD(RNG_SR, SEIS, 6, 1)
	FIELD(RNG_SR, CEIS, 5, 1)
	FIELD(RNG_SR, SECS, 2, 1)
	FIELD(RNG_SR, CECS, 1, 1)
	FIELD(RNG_SR, DRDY, 0, 1)
REG32(RNG_DR, 0x8)
	FIELD(RNG_DR, RNDATA, 0, 32)


__attribute__((unused))
static void stm32f4_rng_update(STM32F4RNG *t) {
	int conditions = ((t->SR & 0x20) || (t->SR & 0x40) || (t->SR & 0x1));
	qemu_set_irq(t->irq[0], conditions);

}

static void stm32f4_rng_register_reset(STM32F4RNG *t) {
	t->CR = 0x0;
	t->SR = 0x0;
	t->DR = 0x0;

}

static uint64_t stm32f4_rng_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F4RNG *t = STM32F4_RNG(opaque);
	uint64_t ret;

	switch (offset) {
		case A_RNG_CR:
			ret = t->CR;
			break;
		case A_RNG_SR:
			ret = t->SR;
			break;
		case A_RNG_DR:
			ret = t->DR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F4 RNG read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f4_rng_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F4RNG *t = STM32F4_RNG(opaque);


	switch (offset) {
		case A_RNG_CR ... A_RNG_CR + 3:
			value = value << ((offset - A_RNG_CR) << 3);
			offset = A_RNG_CR;
			break;
		case A_RNG_SR ... A_RNG_SR + 3:
			value = value << ((offset - A_RNG_SR) << 3);
			offset = A_RNG_SR;
			break;

		default: break;
	}

	switch (offset) {
		case A_RNG_CR:
			t->CR = value;
			break;
		case A_RNG_SR:
			t->SR = value;
			stm32f4_rng_update(t);
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F4 RNG write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f4_rng_ops = {
	.read = stm32f4_rng_read,
	.write = stm32f4_rng_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f4_rng_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F4RNG *t = STM32F4_RNG(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f4_rng_ops, t, "stm32f4-rng", STM32F4_RNG_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32f4_rng_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32f4_rng_reset_enter(Object *obj, ResetType type) {
	STM32F4RNG *t = STM32F4_RNG(obj);
	stm32f4_rng_register_reset(t);
}

static const VMStateDescription stm32f4_rng_vmstate = {
	.name = "stm32f4-rng",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR, STM32F4RNG),
		VMSTATE_UINT32(SR, STM32F4RNG),
		VMSTATE_UINT32(DR, STM32F4RNG),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f4_rng_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f4_rng_vmstate;
	dc->realize = stm32f4_rng_realize;
	rc->phases.enter = stm32f4_rng_reset_enter;

}

static const TypeInfo stm32f4_rng_info = {
	.name = TYPE_STM32F4_RNG,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F4RNG),
	.instance_init = stm32f4_rng_init,
	.class_init = stm32f4_rng_class_init,
};

static void stm32f4_rng_register_types(void) {
	type_register_static(&stm32f4_rng_info);
}

type_init(stm32f4_rng_register_types);

#define TYPE_STM32F4_SPI "stm32f4-spi"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F4SPI, STM32F4_SPI)

struct STM32F4SPI {
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

	/* base */
	uint32_t base;

};
#define STM32F4_SPI_SIZE				0x24

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
	FIELD(SPI_CR2, FRF, 4, 1)
	FIELD(SPI_CR2, SSOE, 2, 1)
	FIELD(SPI_CR2, TXDMAEN, 1, 1)
	FIELD(SPI_CR2, RXDMAEN, 0, 1)
REG32(SPI_SR, 0x8)
	FIELD(SPI_SR, TIFRFE, 8, 1)
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
static void stm32f4_spi_update(STM32F4SPI *t) {
	int conditions = (((t->SR & 0x20) && (!(t->CR2 & 0x1)) && (!(t->CR2 & 0x2)) && (t->CR2 & 0x20)) || ((t->SR & 0x100) && (!(t->CR2 & 0x1)) && (!(t->CR2 & 0x2)) && (t->CR2 & 0x20)) || ((t->SR & 0x40) && (!(t->CR2 & 0x1)) && (!(t->CR2 & 0x2)) && (t->CR2 & 0x20)) || ((t->CR2 & 0x80) && (t->SR & 0x2)) || ((!(t->SR & 0x40)) && (t->CR2 & 0x40)));
	qemu_set_irq(t->irq[0], conditions);

}

static int stm32f4_spi_can_receive(void *opaque) {
	return 1;
}

static void stm32f4_spi_receive(void *opaque, const uint8_t *buf, int size) {
	STM32F4SPI *t = STM32F4_SPI(opaque);

	t->DR = *buf;
	t->SR |= 0x1;

	stm32f4_spi_update(t);
}

static gboolean stm32f4_spi_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	STM32F4SPI *t = STM32F4_SPI(opaque);
	int ret;

	t->watch_tag = 0;
	t->SR |= 0x80;
	t->SR &= (~(0x2));
	t->SR &= (~(0x1));

	stm32f4_spi_update(t);

	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->DR), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, stm32f4_spi_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->SR &= (~(0x80));
	t->SR |= 0x2;
	t->SR |= 0x1;

	stm32f4_spi_update(t);

	return FALSE;
}

static void stm32f4_spi_register_reset(STM32F4SPI *t) {
	t->CR1 = 0x0;
	t->CR2 = 0x0;
	t->SR = 0x2;
	t->DR = 0x0;
	t->CRCPR = 0x7;
	t->RXCRCR = 0x0;
	t->TXCRCR = 0x0;
	t->I2SCFGR = 0x0;
	t->I2SPR = 0xa;
	t->SR &= (~(0x80));
	t->SR |= 0x2;
	t->SR |= 0x1;

}

static uint64_t stm32f4_spi_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F4SPI *t = STM32F4_SPI(opaque);
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
			stm32f4_spi_update(t);
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
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F4 SPI read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f4_spi_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F4SPI *t = STM32F4_SPI(opaque);


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
			stm32f4_spi_update(t);
			break;
		case A_SPI_CR2:
			t->CR2 = value;
			if (((!(value & 0x20)) && (!(value & 0x40)))) {
				t->SR &= (~(0x80));
				t->SR &= (~(0x1));
			} else if (((!(value & 0x20)) && (!(value & 0x80)))) {
				t->SR &= (~(0x80));
			} else if ((!(value & 0x20))) {
				t->SR &= (~(0x80));
				t->SR |= 0x2;
			}
			stm32f4_spi_update(t);
			break;
		case A_SPI_SR:
			t->SR &= value;
			t->SR &= (~(0x80));
			t->SR |= 0x2;
			t->SR |= 0x1;
			stm32f4_spi_update(t);
			break;
		case A_SPI_DR:
			t->DR = value;
			stm32f4_spi_transmit(NULL, G_IO_OUT, t);
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
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F4 SPI write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f4_spi_ops = {
	.read = stm32f4_spi_read,
	.write = stm32f4_spi_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f4_spi_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F4SPI *t = STM32F4_SPI(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f4_spi_ops, t, "stm32f4-spi", STM32F4_SPI_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32f4_spi_realize(DeviceState *dev, Error **errp) {
	STM32F4SPI *t = STM32F4_SPI(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), stm32f4_spi_can_receive, stm32f4_spi_receive,
		NULL, NULL, t, NULL, true);

}

static void stm32f4_spi_reset_enter(Object *obj, ResetType type) {
	STM32F4SPI *t = STM32F4_SPI(obj);
	stm32f4_spi_register_reset(t);
}

static const VMStateDescription stm32f4_spi_vmstate = {
	.name = "stm32f4-spi",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR1, STM32F4SPI),
		VMSTATE_UINT32(CR2, STM32F4SPI),
		VMSTATE_UINT32(SR, STM32F4SPI),
		VMSTATE_UINT32(DR, STM32F4SPI),
		VMSTATE_UINT32(CRCPR, STM32F4SPI),
		VMSTATE_UINT32(RXCRCR, STM32F4SPI),
		VMSTATE_UINT32(TXCRCR, STM32F4SPI),
		VMSTATE_UINT32(I2SCFGR, STM32F4SPI),
		VMSTATE_UINT32(I2SPR, STM32F4SPI),

		VMSTATE_END_OF_LIST()
	}
};

static Property stm32f4_spi_properties[] = {
	DEFINE_PROP_CHR("chardev", STM32F4SPI, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void stm32f4_spi_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f4_spi_vmstate;
	dc->realize = stm32f4_spi_realize;
	rc->phases.enter = stm32f4_spi_reset_enter;
	device_class_set_props(dc, stm32f4_spi_properties);

}

static const TypeInfo stm32f4_spi_info = {
	.name = TYPE_STM32F4_SPI,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F4SPI),
	.instance_init = stm32f4_spi_init,
	.class_init = stm32f4_spi_class_init,
};

static void stm32f4_spi_register_types(void) {
	type_register_static(&stm32f4_spi_info);
}

type_init(stm32f4_spi_register_types);

#define TYPE_STM32F4_RTC "stm32f4-rtc"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F4RTC, STM32F4_RTC)

struct STM32F4RTC {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[2];

	/*registers*/
	uint32_t TR;
	uint32_t DR;
	uint32_t CR;
	uint32_t ISR;
	uint32_t PRER;
	uint32_t WUTR;
	uint32_t CALIBR;
	uint32_t ALRMAR;
	uint32_t ALRMBR;
	uint32_t WPR;
	uint32_t SSR;
	uint32_t SHIFTR;
	uint32_t TSTR;
	uint32_t TSDR;
	uint32_t TSSSR;
	uint32_t CALR;
	uint32_t TAFCR;
	uint32_t ALRMASSR;
	uint32_t ALRMBSSR;
	uint32_t BKP0R;
	uint32_t BKP1R;
	uint32_t BKP2R;
	uint32_t BKP3R;
	uint32_t BKP4R;
	uint32_t BKP5R;
	uint32_t BKP6R;
	uint32_t BKP7R;
	uint32_t BKP8R;
	uint32_t BKP9R;
	uint32_t BKP10R;
	uint32_t BKP11R;
	uint32_t BKP12R;
	uint32_t BKP13R;
	uint32_t BKP14R;
	uint32_t BKP15R;
	uint32_t BKP16R;
	uint32_t BKP17R;
	uint32_t BKP18R;
	uint32_t BKP19R;
	/* base */
	uint32_t base;

};
#define STM32F4_RTC_SIZE				0xa0

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
	FIELD(RTC_CR, DCE, 7, 1)
	FIELD(RTC_CR, FMT, 6, 1)
	FIELD(RTC_CR, REFCKON, 4, 1)
	FIELD(RTC_CR, TSEDGE, 3, 1)
	FIELD(RTC_CR, WCKSEL, 0, 3)
REG32(RTC_ISR, 0xc)
	FIELD(RTC_ISR, ALRAWF, 0, 1)
	FIELD(RTC_ISR, ALRBWF, 1, 1)
	FIELD(RTC_ISR, WUTWF, 2, 1)
	FIELD(RTC_ISR, SHPF, 3, 1)
	FIELD(RTC_ISR, INITS, 4, 1)
	FIELD(RTC_ISR, RSF, 5, 1)
	FIELD(RTC_ISR, INITF, 6, 1)
	FIELD(RTC_ISR, INIT, 7, 1)
	FIELD(RTC_ISR, ALRAF, 8, 1)
	FIELD(RTC_ISR, ALRBF, 9, 1)
	FIELD(RTC_ISR, WUTF, 10, 1)
	FIELD(RTC_ISR, TSF, 11, 1)
	FIELD(RTC_ISR, TSOVF, 12, 1)
	FIELD(RTC_ISR, TAMP1F, 13, 1)
	FIELD(RTC_ISR, TAMP2F, 14, 1)
	FIELD(RTC_ISR, RECALPF, 16, 1)
REG32(RTC_PRER, 0x10)
	FIELD(RTC_PRER, PREDIV_A, 16, 7)
	FIELD(RTC_PRER, PREDIV_S, 0, 15)
REG32(RTC_WUTR, 0x14)
	FIELD(RTC_WUTR, WUT, 0, 16)
REG32(RTC_CALIBR, 0x18)
	FIELD(RTC_CALIBR, DCS, 7, 1)
	FIELD(RTC_CALIBR, DC, 0, 5)
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
	FIELD(RTC_TSTR, ALARMOUTTYPE, 18, 1)
	FIELD(RTC_TSTR, TSINSEL, 17, 1)
	FIELD(RTC_TSTR, TAMP1INSEL, 16, 1)
	FIELD(RTC_TSTR, TAMPIE, 2, 1)
	FIELD(RTC_TSTR, TAMP1TRG, 1, 1)
	FIELD(RTC_TSTR, TAMP1E, 0, 1)
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
REG32(RTC_TAFCR, 0x40)
	FIELD(RTC_TAFCR, ALARMOUTTYPE, 18, 1)
	FIELD(RTC_TAFCR, TSINSEL, 17, 1)
	FIELD(RTC_TAFCR, TAMP1INSEL, 16, 1)
	FIELD(RTC_TAFCR, TAMPPUDIS, 15, 1)
	FIELD(RTC_TAFCR, TAMPPRCH, 13, 2)
	FIELD(RTC_TAFCR, TAMPFLT, 11, 2)
	FIELD(RTC_TAFCR, TAMPFREQ, 8, 3)
	FIELD(RTC_TAFCR, TAMPTS, 7, 1)
	FIELD(RTC_TAFCR, TAMP2TRG, 4, 1)
	FIELD(RTC_TAFCR, TAMP2E, 3, 1)
	FIELD(RTC_TAFCR, TAMPIE, 2, 1)
	FIELD(RTC_TAFCR, TAMP1TRG, 1, 1)
	FIELD(RTC_TAFCR, TAMP1E, 0, 1)
REG32(RTC_ALRMASSR, 0x44)
	FIELD(RTC_ALRMASSR, MASKSS, 24, 4)
	FIELD(RTC_ALRMASSR, SS, 0, 15)
REG32(RTC_ALRMBSSR, 0x48)
	FIELD(RTC_ALRMBSSR, MASKSS, 24, 4)
	FIELD(RTC_ALRMBSSR, SS, 0, 15)
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
REG32(RTC_BKP5R, 0x64)
	FIELD(RTC_BKP5R, BKP, 0, 32)
REG32(RTC_BKP6R, 0x68)
	FIELD(RTC_BKP6R, BKP, 0, 32)
REG32(RTC_BKP7R, 0x6c)
	FIELD(RTC_BKP7R, BKP, 0, 32)
REG32(RTC_BKP8R, 0x70)
	FIELD(RTC_BKP8R, BKP, 0, 32)
REG32(RTC_BKP9R, 0x74)
	FIELD(RTC_BKP9R, BKP, 0, 32)
REG32(RTC_BKP10R, 0x78)
	FIELD(RTC_BKP10R, BKP, 0, 32)
REG32(RTC_BKP11R, 0x7c)
	FIELD(RTC_BKP11R, BKP, 0, 32)
REG32(RTC_BKP12R, 0x80)
	FIELD(RTC_BKP12R, BKP, 0, 32)
REG32(RTC_BKP13R, 0x84)
	FIELD(RTC_BKP13R, BKP, 0, 32)
REG32(RTC_BKP14R, 0x88)
	FIELD(RTC_BKP14R, BKP, 0, 32)
REG32(RTC_BKP15R, 0x8c)
	FIELD(RTC_BKP15R, BKP, 0, 32)
REG32(RTC_BKP16R, 0x90)
	FIELD(RTC_BKP16R, BKP, 0, 32)
REG32(RTC_BKP17R, 0x94)
	FIELD(RTC_BKP17R, BKP, 0, 32)
REG32(RTC_BKP18R, 0x98)
	FIELD(RTC_BKP18R, BKP, 0, 32)
REG32(RTC_BKP19R, 0x9c)
	FIELD(RTC_BKP19R, BKP, 0, 32)


__attribute__((unused))
static void stm32f4_rtc_update(STM32F4RTC *t) {
	int conditions = (((t->CR & 0x1000) && (t->ISR & 0x100)) || ((t->CR & 0x2000) && (t->ISR & 0x200)));
	qemu_set_irq(t->irq[0], conditions);

}

static void stm32f4_rtc_register_reset(STM32F4RTC *t) {
	t->TR = 0x0;
	t->DR = 0x2101;
	t->CR = 0x0;
	t->ISR = 0x7;
	t->PRER = 0x7f00ff;
	t->WUTR = 0xffff;
	t->CALIBR = 0x0;
	t->ALRMAR = 0x0;
	t->ALRMBR = 0x0;
	t->WPR = 0x0;
	t->SSR = 0x0;
	t->SHIFTR = 0x0;
	t->TSTR = 0x0;
	t->TSDR = 0x0;
	t->TSSSR = 0x0;
	t->CALR = 0x0;
	t->TAFCR = 0x0;
	t->ALRMASSR = 0x0;
	t->ALRMBSSR = 0x0;
	t->BKP0R = 0x0;
	t->BKP1R = 0x0;
	t->BKP2R = 0x0;
	t->BKP3R = 0x0;
	t->BKP4R = 0x0;
	t->BKP5R = 0x0;
	t->BKP6R = 0x0;
	t->BKP7R = 0x0;
	t->BKP8R = 0x0;
	t->BKP9R = 0x0;
	t->BKP10R = 0x0;
	t->BKP11R = 0x0;
	t->BKP12R = 0x0;
	t->BKP13R = 0x0;
	t->BKP14R = 0x0;
	t->BKP15R = 0x0;
	t->BKP16R = 0x0;
	t->BKP17R = 0x0;
	t->BKP18R = 0x0;
	t->BKP19R = 0x0;

}

static uint64_t stm32f4_rtc_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F4RTC *t = STM32F4_RTC(opaque);
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
		case A_RTC_CALIBR:
			ret = t->CALIBR;
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
		case A_RTC_TAFCR:
			ret = t->TAFCR;
			break;
		case A_RTC_ALRMASSR:
			ret = t->ALRMASSR;
			break;
		case A_RTC_ALRMBSSR:
			ret = t->ALRMBSSR;
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
		case A_RTC_BKP5R:
			ret = t->BKP5R;
			break;
		case A_RTC_BKP6R:
			ret = t->BKP6R;
			break;
		case A_RTC_BKP7R:
			ret = t->BKP7R;
			break;
		case A_RTC_BKP8R:
			ret = t->BKP8R;
			break;
		case A_RTC_BKP9R:
			ret = t->BKP9R;
			break;
		case A_RTC_BKP10R:
			ret = t->BKP10R;
			break;
		case A_RTC_BKP11R:
			ret = t->BKP11R;
			break;
		case A_RTC_BKP12R:
			ret = t->BKP12R;
			break;
		case A_RTC_BKP13R:
			ret = t->BKP13R;
			break;
		case A_RTC_BKP14R:
			ret = t->BKP14R;
			break;
		case A_RTC_BKP15R:
			ret = t->BKP15R;
			break;
		case A_RTC_BKP16R:
			ret = t->BKP16R;
			break;
		case A_RTC_BKP17R:
			ret = t->BKP17R;
			break;
		case A_RTC_BKP18R:
			ret = t->BKP18R;
			break;
		case A_RTC_BKP19R:
			ret = t->BKP19R;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F4 RTC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f4_rtc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F4RTC *t = STM32F4_RTC(opaque);


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
		case A_RTC_CALIBR ... A_RTC_CALIBR + 3:
			value = value << ((offset - A_RTC_CALIBR) << 3);
			offset = A_RTC_CALIBR;
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
		case A_RTC_TAFCR ... A_RTC_TAFCR + 3:
			value = value << ((offset - A_RTC_TAFCR) << 3);
			offset = A_RTC_TAFCR;
			break;
		case A_RTC_ALRMASSR ... A_RTC_ALRMASSR + 3:
			value = value << ((offset - A_RTC_ALRMASSR) << 3);
			offset = A_RTC_ALRMASSR;
			break;
		case A_RTC_ALRMBSSR ... A_RTC_ALRMBSSR + 3:
			value = value << ((offset - A_RTC_ALRMBSSR) << 3);
			offset = A_RTC_ALRMBSSR;
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
		case A_RTC_BKP5R ... A_RTC_BKP5R + 3:
			value = value << ((offset - A_RTC_BKP5R) << 3);
			offset = A_RTC_BKP5R;
			break;
		case A_RTC_BKP6R ... A_RTC_BKP6R + 3:
			value = value << ((offset - A_RTC_BKP6R) << 3);
			offset = A_RTC_BKP6R;
			break;
		case A_RTC_BKP7R ... A_RTC_BKP7R + 3:
			value = value << ((offset - A_RTC_BKP7R) << 3);
			offset = A_RTC_BKP7R;
			break;
		case A_RTC_BKP8R ... A_RTC_BKP8R + 3:
			value = value << ((offset - A_RTC_BKP8R) << 3);
			offset = A_RTC_BKP8R;
			break;
		case A_RTC_BKP9R ... A_RTC_BKP9R + 3:
			value = value << ((offset - A_RTC_BKP9R) << 3);
			offset = A_RTC_BKP9R;
			break;
		case A_RTC_BKP10R ... A_RTC_BKP10R + 3:
			value = value << ((offset - A_RTC_BKP10R) << 3);
			offset = A_RTC_BKP10R;
			break;
		case A_RTC_BKP11R ... A_RTC_BKP11R + 3:
			value = value << ((offset - A_RTC_BKP11R) << 3);
			offset = A_RTC_BKP11R;
			break;
		case A_RTC_BKP12R ... A_RTC_BKP12R + 3:
			value = value << ((offset - A_RTC_BKP12R) << 3);
			offset = A_RTC_BKP12R;
			break;
		case A_RTC_BKP13R ... A_RTC_BKP13R + 3:
			value = value << ((offset - A_RTC_BKP13R) << 3);
			offset = A_RTC_BKP13R;
			break;
		case A_RTC_BKP14R ... A_RTC_BKP14R + 3:
			value = value << ((offset - A_RTC_BKP14R) << 3);
			offset = A_RTC_BKP14R;
			break;
		case A_RTC_BKP15R ... A_RTC_BKP15R + 3:
			value = value << ((offset - A_RTC_BKP15R) << 3);
			offset = A_RTC_BKP15R;
			break;
		case A_RTC_BKP16R ... A_RTC_BKP16R + 3:
			value = value << ((offset - A_RTC_BKP16R) << 3);
			offset = A_RTC_BKP16R;
			break;
		case A_RTC_BKP17R ... A_RTC_BKP17R + 3:
			value = value << ((offset - A_RTC_BKP17R) << 3);
			offset = A_RTC_BKP17R;
			break;
		case A_RTC_BKP18R ... A_RTC_BKP18R + 3:
			value = value << ((offset - A_RTC_BKP18R) << 3);
			offset = A_RTC_BKP18R;
			break;
		case A_RTC_BKP19R ... A_RTC_BKP19R + 3:
			value = value << ((offset - A_RTC_BKP19R) << 3);
			offset = A_RTC_BKP19R;
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
			stm32f4_rtc_update(t);
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
			stm32f4_rtc_update(t);
			break;
		case A_RTC_PRER:
			t->PRER = value;
			break;
		case A_RTC_WUTR:
			t->WUTR = value;
			break;
		case A_RTC_CALIBR:
			t->CALIBR = value;
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
		case A_RTC_TAFCR:
			t->TAFCR = value;
			break;
		case A_RTC_ALRMASSR:
			t->ALRMASSR = value;
			break;
		case A_RTC_ALRMBSSR:
			t->ALRMBSSR = value;
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
		case A_RTC_BKP5R:
			t->BKP5R = value;
			break;
		case A_RTC_BKP6R:
			t->BKP6R = value;
			break;
		case A_RTC_BKP7R:
			t->BKP7R = value;
			break;
		case A_RTC_BKP8R:
			t->BKP8R = value;
			break;
		case A_RTC_BKP9R:
			t->BKP9R = value;
			break;
		case A_RTC_BKP10R:
			t->BKP10R = value;
			break;
		case A_RTC_BKP11R:
			t->BKP11R = value;
			break;
		case A_RTC_BKP12R:
			t->BKP12R = value;
			break;
		case A_RTC_BKP13R:
			t->BKP13R = value;
			break;
		case A_RTC_BKP14R:
			t->BKP14R = value;
			break;
		case A_RTC_BKP15R:
			t->BKP15R = value;
			break;
		case A_RTC_BKP16R:
			t->BKP16R = value;
			break;
		case A_RTC_BKP17R:
			t->BKP17R = value;
			break;
		case A_RTC_BKP18R:
			t->BKP18R = value;
			break;
		case A_RTC_BKP19R:
			t->BKP19R = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F4 RTC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f4_rtc_ops = {
	.read = stm32f4_rtc_read,
	.write = stm32f4_rtc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f4_rtc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F4RTC *t = STM32F4_RTC(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f4_rtc_ops, t, "stm32f4-rtc", STM32F4_RTC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 2; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32f4_rtc_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32f4_rtc_reset_enter(Object *obj, ResetType type) {
	STM32F4RTC *t = STM32F4_RTC(obj);
	stm32f4_rtc_register_reset(t);
}

static const VMStateDescription stm32f4_rtc_vmstate = {
	.name = "stm32f4-rtc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(TR, STM32F4RTC),
		VMSTATE_UINT32(DR, STM32F4RTC),
		VMSTATE_UINT32(CR, STM32F4RTC),
		VMSTATE_UINT32(ISR, STM32F4RTC),
		VMSTATE_UINT32(PRER, STM32F4RTC),
		VMSTATE_UINT32(WUTR, STM32F4RTC),
		VMSTATE_UINT32(CALIBR, STM32F4RTC),
		VMSTATE_UINT32(ALRMAR, STM32F4RTC),
		VMSTATE_UINT32(ALRMBR, STM32F4RTC),
		VMSTATE_UINT32(WPR, STM32F4RTC),
		VMSTATE_UINT32(SSR, STM32F4RTC),
		VMSTATE_UINT32(SHIFTR, STM32F4RTC),
		VMSTATE_UINT32(TSTR, STM32F4RTC),
		VMSTATE_UINT32(TSDR, STM32F4RTC),
		VMSTATE_UINT32(TSSSR, STM32F4RTC),
		VMSTATE_UINT32(CALR, STM32F4RTC),
		VMSTATE_UINT32(TAFCR, STM32F4RTC),
		VMSTATE_UINT32(ALRMASSR, STM32F4RTC),
		VMSTATE_UINT32(ALRMBSSR, STM32F4RTC),
		VMSTATE_UINT32(BKP0R, STM32F4RTC),
		VMSTATE_UINT32(BKP1R, STM32F4RTC),
		VMSTATE_UINT32(BKP2R, STM32F4RTC),
		VMSTATE_UINT32(BKP3R, STM32F4RTC),
		VMSTATE_UINT32(BKP4R, STM32F4RTC),
		VMSTATE_UINT32(BKP5R, STM32F4RTC),
		VMSTATE_UINT32(BKP6R, STM32F4RTC),
		VMSTATE_UINT32(BKP7R, STM32F4RTC),
		VMSTATE_UINT32(BKP8R, STM32F4RTC),
		VMSTATE_UINT32(BKP9R, STM32F4RTC),
		VMSTATE_UINT32(BKP10R, STM32F4RTC),
		VMSTATE_UINT32(BKP11R, STM32F4RTC),
		VMSTATE_UINT32(BKP12R, STM32F4RTC),
		VMSTATE_UINT32(BKP13R, STM32F4RTC),
		VMSTATE_UINT32(BKP14R, STM32F4RTC),
		VMSTATE_UINT32(BKP15R, STM32F4RTC),
		VMSTATE_UINT32(BKP16R, STM32F4RTC),
		VMSTATE_UINT32(BKP17R, STM32F4RTC),
		VMSTATE_UINT32(BKP18R, STM32F4RTC),
		VMSTATE_UINT32(BKP19R, STM32F4RTC),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f4_rtc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f4_rtc_vmstate;
	dc->realize = stm32f4_rtc_realize;
	rc->phases.enter = stm32f4_rtc_reset_enter;

}

static const TypeInfo stm32f4_rtc_info = {
	.name = TYPE_STM32F4_RTC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F4RTC),
	.instance_init = stm32f4_rtc_init,
	.class_init = stm32f4_rtc_class_init,
};

static void stm32f4_rtc_register_types(void) {
	type_register_static(&stm32f4_rtc_info);
}

type_init(stm32f4_rtc_register_types);

#define TYPE_STM32F4_EXTI "stm32f4-exti"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F4EXTI, STM32F4_EXTI)

struct STM32F4EXTI {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[9];

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
#define STM32F4_EXTI_SIZE				0x18

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
	FIELD(EXTI_IMR, MR19, 19, 1)
	FIELD(EXTI_IMR, MR20, 20, 1)
	FIELD(EXTI_IMR, MR21, 21, 1)
	FIELD(EXTI_IMR, MR22, 22, 1)
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
	FIELD(EXTI_EMR, MR19, 19, 1)
	FIELD(EXTI_EMR, MR20, 20, 1)
	FIELD(EXTI_EMR, MR21, 21, 1)
	FIELD(EXTI_EMR, MR22, 22, 1)
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
	FIELD(EXTI_RTSR, TR19, 19, 1)
	FIELD(EXTI_RTSR, TR20, 20, 1)
	FIELD(EXTI_RTSR, TR21, 21, 1)
	FIELD(EXTI_RTSR, TR22, 22, 1)
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
	FIELD(EXTI_FTSR, TR19, 19, 1)
	FIELD(EXTI_FTSR, TR20, 20, 1)
	FIELD(EXTI_FTSR, TR21, 21, 1)
	FIELD(EXTI_FTSR, TR22, 22, 1)
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
	FIELD(EXTI_SWIER, SWIER19, 19, 1)
	FIELD(EXTI_SWIER, SWIER20, 20, 1)
	FIELD(EXTI_SWIER, SWIER21, 21, 1)
	FIELD(EXTI_SWIER, SWIER22, 22, 1)
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
	FIELD(EXTI_PR, PR19, 19, 1)
	FIELD(EXTI_PR, PR20, 20, 1)
	FIELD(EXTI_PR, PR21, 21, 1)
	FIELD(EXTI_PR, PR22, 22, 1)


static int stm32f4_exti_can_receive(void *opaque) {
	return 1;
}

static void stm32f4_exti_receive(void *opaque, const uint8_t *buf, int size) {
	STM32F4EXTI *t = STM32F4_EXTI(opaque);

	(void)t;


}

static void stm32f4_exti_register_reset(STM32F4EXTI *t) {
	t->IMR = 0x0;
	t->EMR = 0x0;
	t->RTSR = 0x0;
	t->FTSR = 0x0;
	t->SWIER = 0x0;
	t->PR = 0x0;

}

static uint64_t stm32f4_exti_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F4EXTI *t = STM32F4_EXTI(opaque);
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
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F4 EXTI read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f4_exti_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F4EXTI *t = STM32F4_EXTI(opaque);


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
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F4 EXTI write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f4_exti_ops = {
	.read = stm32f4_exti_read,
	.write = stm32f4_exti_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f4_exti_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F4EXTI *t = STM32F4_EXTI(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f4_exti_ops, t, "stm32f4-exti", STM32F4_EXTI_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 9; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32f4_exti_realize(DeviceState *dev, Error **errp) {
	STM32F4EXTI *t = STM32F4_EXTI(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), stm32f4_exti_can_receive, stm32f4_exti_receive,
		NULL, NULL, t, NULL, true);

}

static void stm32f4_exti_reset_enter(Object *obj, ResetType type) {
	STM32F4EXTI *t = STM32F4_EXTI(obj);
	stm32f4_exti_register_reset(t);
}

static const VMStateDescription stm32f4_exti_vmstate = {
	.name = "stm32f4-exti",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(IMR, STM32F4EXTI),
		VMSTATE_UINT32(EMR, STM32F4EXTI),
		VMSTATE_UINT32(RTSR, STM32F4EXTI),
		VMSTATE_UINT32(FTSR, STM32F4EXTI),
		VMSTATE_UINT32(SWIER, STM32F4EXTI),
		VMSTATE_UINT32(PR, STM32F4EXTI),

		VMSTATE_END_OF_LIST()
	}
};

static Property stm32f4_exti_properties[] = {
	DEFINE_PROP_CHR("chardev", STM32F4EXTI, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void stm32f4_exti_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f4_exti_vmstate;
	dc->realize = stm32f4_exti_realize;
	rc->phases.enter = stm32f4_exti_reset_enter;
	device_class_set_props(dc, stm32f4_exti_properties);

}

static const TypeInfo stm32f4_exti_info = {
	.name = TYPE_STM32F4_EXTI,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F4EXTI),
	.instance_init = stm32f4_exti_init,
	.class_init = stm32f4_exti_class_init,
};

static void stm32f4_exti_register_types(void) {
	type_register_static(&stm32f4_exti_info);
}

type_init(stm32f4_exti_register_types);

#define TYPE_STM32F4_I2C "stm32f4-i2c"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F4I2C, STM32F4_I2C)

struct STM32F4I2C {
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

	/* base */
	uint32_t base;

};
#define STM32F4_I2C_SIZE				0x24

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
static void stm32f4_i2c_update(STM32F4I2C *t) {
	int conditions = (((!(t->SR1 & 0x4)) && (!(t->CR2 & 0x200)) && (t->CR2 & 0x400) && (t->SR1 & 0x80)) || ((!(t->CR2 & 0x800)) && (t->CR2 & 0x100) && (t->SR1 & 0x200)) || ((!(t->CR2 & 0x800)) && (t->CR2 & 0x100) && (t->SR1 & 0x800)) || ((!(t->CR2 & 0x800)) && (t->SR1 & 0x100) && (t->CR2 & 0x100)) || ((!(t->CR2 & 0x800)) && (t->CR2 & 0x200) && (t->SR1 & 0x10)) || ((t->SR1 & 0x4) && (!(t->SR1 & 0x10)) && (t->CR2 & 0x200)) || ((!(t->SR1 & 0x4)) && (!(t->CR2 & 0x200)) && (t->CR2 & 0x400) && (t->SR1 & 0x40)) || ((t->CR2 & 0x200) && (t->SR1 & 0x1)) || ((!(t->SR1 & 0x4)) && (!(t->SR1 & 0x10)) && (t->CR2 & 0x400) && (t->SR1 & 0x40)) || ((!(t->SR1 & 0x4)) && (!(t->SR1 & 0x10)) && (t->CR2 & 0x400) && (t->SR1 & 0x80)) || ((t->SR1 & 0x400) && (t->CR2 & 0x100)) || ((t->CR2 & 0x200) && (t->SR1 & 0x8)) || ((t->SR1 & 0x2) && (t->CR2 & 0x200)));
	qemu_set_irq(t->irq[0], conditions);

}

static int stm32f4_i2c_can_receive(void *opaque) {
	return 1;
}

static void stm32f4_i2c_receive(void *opaque, const uint8_t *buf, int size) {
	STM32F4I2C *t = STM32F4_I2C(opaque);

	t->DR = *buf;
	t->SR1 |= 0x800;
	t->SR1 |= 0x10;
	t->SR1 |= 0x40;
	t->SR1 &= (~(0x4));
	t->SR1 |= 0x100;
	t->SR1 |= 0x1;
	t->SR1 &= (~(0x10));
	t->SR1 |= 0x200;
	t->SR1 |= 0x2;
	t->SR2 |= 0x2;
	t->SR1 &= (~(0x2));
	t->SR1 |= 0x4;
	t->SR2 &= (~(0x2));
	t->SR1 |= 0x400;

	stm32f4_i2c_update(t);
}

static gboolean stm32f4_i2c_transmit(void *do_not_use, GIOCondition cond, void *opaque) {
	STM32F4I2C *t = STM32F4_I2C(opaque);
	int ret;

	t->watch_tag = 0;
	t->SR1 |= 0x10;
	t->SR1 &= (~(0x80));
	t->SR1 &= (~(0x1));
	t->SR1 &= (~(0x4));
	t->SR1 &= (~(0x8));
	t->SR1 |= 0x2;
	t->SR2 |= 0x2;
	t->SR1 &= (~(0x2));
	t->SR1 |= 0x4;
	t->SR1 &= (~(0x400));
	t->SR2 &= (~(0x4));

	stm32f4_i2c_update(t);

	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->DR), 1);
	if (ret <= 0){
		t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP, stm32f4_i2c_transmit, t);
		if (!t->watch_tag) {
			goto buffer_drained;
		}

		 return FALSE;
	}

buffer_drained:
	t->SR2 |= 0x4;
	t->SR1 &= (~(0x4));
	t->SR1 |= 0x1;
	t->SR1 &= (~(0x10));
	t->SR1 |= 0x2;
	t->SR1 &= (~(0x2));
	t->SR1 |= 0x8;
	t->SR1 |= 0x4;
	t->SR2 &= (~(0x2));
	t->SR1 |= 0x80;
	t->SR1 |= 0x400;

	stm32f4_i2c_update(t);

	return FALSE;
}

static void stm32f4_i2c_register_reset(STM32F4I2C *t) {
	t->CR1 = 0x0;
	t->CR2 = 0x0;
	t->OAR1 = 0x0;
	t->OAR2 = 0x0;
	t->DR = 0x0;
	t->SR1 = 0x0;
	t->SR2 = 0x0;
	t->CCR = 0x0;
	t->TRISE = 0x2;
	t->SR2 |= 0x4;
	t->SR1 &= (~(0x4));
	t->SR1 |= 0x1;
	t->SR1 &= (~(0x10));
	t->SR1 |= 0x2;
	t->SR1 &= (~(0x2));
	t->SR1 |= 0x8;
	t->SR1 |= 0x4;
	t->SR2 &= (~(0x2));
	t->SR1 |= 0x80;
	t->SR1 |= 0x400;

}

static uint64_t stm32f4_i2c_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F4I2C *t = STM32F4_I2C(opaque);
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
			t->SR1 &= (~(0x200));
			t->SR1 |= 0x10;
			t->SR1 &= (~(0x100));
			t->SR1 &= (~(0x1));
			t->SR1 &= (~(0x800));
			t->SR1 &= (~(0x4));
			t->SR1 &= (~(0x10));
			t->SR1 |= 0x2;
			t->SR2 |= 0x2;
			t->SR1 &= (~(0x40));
			t->SR1 &= (~(0x2));
			t->SR1 |= 0x4;
			t->SR1 &= (~(0x400));
			t->SR2 &= (~(0x2));
			qemu_chr_fe_accept_input(&(t->chr));
			stm32f4_i2c_update(t);
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
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F4 I2C read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f4_i2c_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F4I2C *t = STM32F4_I2C(opaque);


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
			}
			stm32f4_i2c_update(t);
			break;
		case A_I2C_CR2:
			t->CR2 = value;
			stm32f4_i2c_update(t);
			break;
		case A_I2C_OAR1:
			t->OAR1 = value;
			break;
		case A_I2C_OAR2:
			t->OAR2 = value;
			break;
		case A_I2C_DR:
			t->DR = value;
			stm32f4_i2c_transmit(NULL, G_IO_OUT, t);
			break;
		case A_I2C_SR1:
			t->SR1 &= value;
			if ((value == 0xfffffbff)) {
				t->SR2 &= (~(0x2));
			}
			stm32f4_i2c_update(t);
			break;
		case A_I2C_CCR:
			t->CCR = value;
			break;
		case A_I2C_TRISE:
			t->TRISE = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F4 I2C write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f4_i2c_ops = {
	.read = stm32f4_i2c_read,
	.write = stm32f4_i2c_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f4_i2c_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F4I2C *t = STM32F4_I2C(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f4_i2c_ops, t, "stm32f4-i2c", STM32F4_I2C_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 2; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32f4_i2c_realize(DeviceState *dev, Error **errp) {
	STM32F4I2C *t = STM32F4_I2C(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), stm32f4_i2c_can_receive, stm32f4_i2c_receive,
		NULL, NULL, t, NULL, true);

}

static void stm32f4_i2c_reset_enter(Object *obj, ResetType type) {
	STM32F4I2C *t = STM32F4_I2C(obj);
	stm32f4_i2c_register_reset(t);
}

static const VMStateDescription stm32f4_i2c_vmstate = {
	.name = "stm32f4-i2c",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR1, STM32F4I2C),
		VMSTATE_UINT32(CR2, STM32F4I2C),
		VMSTATE_UINT32(OAR1, STM32F4I2C),
		VMSTATE_UINT32(OAR2, STM32F4I2C),
		VMSTATE_UINT32(DR, STM32F4I2C),
		VMSTATE_UINT32(SR1, STM32F4I2C),
		VMSTATE_UINT32(SR2, STM32F4I2C),
		VMSTATE_UINT32(CCR, STM32F4I2C),
		VMSTATE_UINT32(TRISE, STM32F4I2C),

		VMSTATE_END_OF_LIST()
	}
};

static Property stm32f4_i2c_properties[] = {
	DEFINE_PROP_CHR("chardev", STM32F4I2C, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void stm32f4_i2c_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f4_i2c_vmstate;
	dc->realize = stm32f4_i2c_realize;
	rc->phases.enter = stm32f4_i2c_reset_enter;
	device_class_set_props(dc, stm32f4_i2c_properties);

}

static const TypeInfo stm32f4_i2c_info = {
	.name = TYPE_STM32F4_I2C,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F4I2C),
	.instance_init = stm32f4_i2c_init,
	.class_init = stm32f4_i2c_class_init,
};

static void stm32f4_i2c_register_types(void) {
	type_register_static(&stm32f4_i2c_info);
}

type_init(stm32f4_i2c_register_types);

#define TYPE_STM32F4_ADC "stm32f4-adc"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F4ADC, STM32F4_ADC)

struct STM32F4ADC {
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
	/* base */
	uint32_t base;

};
#define STM32F4_ADC_SIZE				0x50

REG32(ADC_SR, 0x0)
	FIELD(ADC_SR, OVR, 5, 1)
	FIELD(ADC_SR, STRT, 4, 1)
	FIELD(ADC_SR, JSTRT, 3, 1)
	FIELD(ADC_SR, JEOC, 2, 1)
	FIELD(ADC_SR, EOC, 1, 1)
	FIELD(ADC_SR, AWD, 0, 1)
REG32(ADC_CR1, 0x4)
	FIELD(ADC_CR1, OVRIE, 26, 1)
	FIELD(ADC_CR1, RES, 24, 2)
	FIELD(ADC_CR1, AWDEN, 23, 1)
	FIELD(ADC_CR1, JAWDEN, 22, 1)
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
	FIELD(ADC_CR2, SWSTART, 30, 1)
	FIELD(ADC_CR2, EXTEN, 28, 2)
	FIELD(ADC_CR2, EXTSEL, 24, 4)
	FIELD(ADC_CR2, JSWSTART, 22, 1)
	FIELD(ADC_CR2, JEXTEN, 20, 2)
	FIELD(ADC_CR2, JEXTSEL, 16, 4)
	FIELD(ADC_CR2, ALIGN, 11, 1)
	FIELD(ADC_CR2, EOCS, 10, 1)
	FIELD(ADC_CR2, DDS, 9, 1)
	FIELD(ADC_CR2, DMA, 8, 1)
	FIELD(ADC_CR2, CONT, 1, 1)
	FIELD(ADC_CR2, ADON, 0, 1)
REG32(ADC_SMPR1, 0xc)
	FIELD(ADC_SMPR1, SMPx_x, 0, 32)
REG32(ADC_SMPR2, 0x10)
	FIELD(ADC_SMPR2, SMPx_x, 0, 32)
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


__attribute__((unused))
static void stm32f4_adc_update(STM32F4ADC *t) {
	int conditions = (((t->CR1 & 0x4000000) && (t->SR & 0x20)) || ((t->SR & 0x2) && (t->CR1 & 0x20)) || ((t->SR & 0x1) && (t->CR1 & 0x40)) || ((t->CR1 & 0x80) && (t->SR & 0x4)));
	qemu_set_irq(t->irq[0], conditions);

}

static void stm32f4_adc_register_reset(STM32F4ADC *t) {
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

static uint64_t stm32f4_adc_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F4ADC *t = STM32F4_ADC(opaque);
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
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F4 ADC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f4_adc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F4ADC *t = STM32F4_ADC(opaque);


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
			t->SR = value;
			stm32f4_adc_update(t);
			break;
		case A_ADC_CR1:
			t->CR1 = value;
			stm32f4_adc_update(t);
			break;
		case A_ADC_CR2:
			t->CR2 = value;
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
			t->SQR1 = value;
			break;
		case A_ADC_SQR2:
			t->SQR2 = value;
			break;
		case A_ADC_SQR3:
			t->SQR3 = value;
			break;
		case A_ADC_JSQR:
			t->JSQR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F4 ADC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f4_adc_ops = {
	.read = stm32f4_adc_read,
	.write = stm32f4_adc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f4_adc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F4ADC *t = STM32F4_ADC(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f4_adc_ops, t, "stm32f4-adc", STM32F4_ADC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32f4_adc_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32f4_adc_reset_enter(Object *obj, ResetType type) {
	STM32F4ADC *t = STM32F4_ADC(obj);
	stm32f4_adc_register_reset(t);
}

static const VMStateDescription stm32f4_adc_vmstate = {
	.name = "stm32f4-adc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(SR, STM32F4ADC),
		VMSTATE_UINT32(CR1, STM32F4ADC),
		VMSTATE_UINT32(CR2, STM32F4ADC),
		VMSTATE_UINT32(SMPR1, STM32F4ADC),
		VMSTATE_UINT32(SMPR2, STM32F4ADC),
		VMSTATE_UINT32(JOFR1, STM32F4ADC),
		VMSTATE_UINT32(JOFR2, STM32F4ADC),
		VMSTATE_UINT32(JOFR3, STM32F4ADC),
		VMSTATE_UINT32(JOFR4, STM32F4ADC),
		VMSTATE_UINT32(HTR, STM32F4ADC),
		VMSTATE_UINT32(LTR, STM32F4ADC),
		VMSTATE_UINT32(SQR1, STM32F4ADC),
		VMSTATE_UINT32(SQR2, STM32F4ADC),
		VMSTATE_UINT32(SQR3, STM32F4ADC),
		VMSTATE_UINT32(JSQR, STM32F4ADC),
		VMSTATE_UINT32(JDR1, STM32F4ADC),
		VMSTATE_UINT32(JDR2, STM32F4ADC),
		VMSTATE_UINT32(JDR3, STM32F4ADC),
		VMSTATE_UINT32(JDR4, STM32F4ADC),
		VMSTATE_UINT32(DR, STM32F4ADC),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f4_adc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f4_adc_vmstate;
	dc->realize = stm32f4_adc_realize;
	rc->phases.enter = stm32f4_adc_reset_enter;

}

static const TypeInfo stm32f4_adc_info = {
	.name = TYPE_STM32F4_ADC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F4ADC),
	.instance_init = stm32f4_adc_init,
	.class_init = stm32f4_adc_class_init,
};

static void stm32f4_adc_register_types(void) {
	type_register_static(&stm32f4_adc_info);
}

type_init(stm32f4_adc_register_types);

#define TYPE_STM32F407_MACHINE MACHINE_TYPE_NAME("stm32f407")
OBJECT_DECLARE_TYPE(STM32F407MachineState, STM32F407MachineClass, STM32F407_MACHINE)

struct STM32F407MachineClass {
	MachineClass parent;
};

struct STM32F407MachineState {
	MachineState parent;
	ARMv7MState armv7m;
};

static void stm32f407_periph_init(MachineState *machine) {
	STM32F407MachineState *sms = STM32F407_MACHINE(machine);
	STM32F4DMA *p0 = g_new(STM32F4DMA, 1);
	object_initialize_child(OBJECT(sms), "DMA1", p0, TYPE_STM32F4_DMA);
	sysbus_realize(SYS_BUS_DEVICE(p0), &error_fatal);
	p0->base = 0x40026000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p0), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 11));
	sysbus_connect_irq(SYS_BUS_DEVICE(p0), 1, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 12));
	sysbus_connect_irq(SYS_BUS_DEVICE(p0), 2, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 13));
	sysbus_connect_irq(SYS_BUS_DEVICE(p0), 3, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 14));
	sysbus_connect_irq(SYS_BUS_DEVICE(p0), 4, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 15));
	sysbus_connect_irq(SYS_BUS_DEVICE(p0), 5, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 16));
	sysbus_connect_irq(SYS_BUS_DEVICE(p0), 6, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 17));
	sysbus_connect_irq(SYS_BUS_DEVICE(p0), 7, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 47));
	sysbus_mmio_map(SYS_BUS_DEVICE(p0), 0, 0x40026000);

	STM32F4DMA *p1 = g_new(STM32F4DMA, 1);
	object_initialize_child(OBJECT(sms), "DMA2", p1, TYPE_STM32F4_DMA);
	sysbus_realize(SYS_BUS_DEVICE(p1), &error_fatal);
	p1->base = 0x40026400;
	sysbus_connect_irq(SYS_BUS_DEVICE(p1), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 56));
	sysbus_connect_irq(SYS_BUS_DEVICE(p1), 1, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 57));
	sysbus_connect_irq(SYS_BUS_DEVICE(p1), 2, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 58));
	sysbus_connect_irq(SYS_BUS_DEVICE(p1), 3, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 59));
	sysbus_connect_irq(SYS_BUS_DEVICE(p1), 4, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 60));
	sysbus_connect_irq(SYS_BUS_DEVICE(p1), 5, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 68));
	sysbus_connect_irq(SYS_BUS_DEVICE(p1), 6, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 69));
	sysbus_connect_irq(SYS_BUS_DEVICE(p1), 7, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 70));
	sysbus_mmio_map(SYS_BUS_DEVICE(p1), 0, 0x40026400);

	STM32F4TIM3 *p2 = g_new(STM32F4TIM3, 1);
	object_initialize_child(OBJECT(sms), "TIM2", p2, TYPE_STM32F4_TIM3);
	sysbus_realize(SYS_BUS_DEVICE(p2), &error_fatal);
	p2->base = 0x40000000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p2), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 28));
	sysbus_mmio_map(SYS_BUS_DEVICE(p2), 0, 0x40000000);

	STM32F4TIM3 *p3 = g_new(STM32F4TIM3, 1);
	object_initialize_child(OBJECT(sms), "TIM3", p3, TYPE_STM32F4_TIM3);
	sysbus_realize(SYS_BUS_DEVICE(p3), &error_fatal);
	p3->base = 0x40000400;
	sysbus_connect_irq(SYS_BUS_DEVICE(p3), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 29));
	sysbus_mmio_map(SYS_BUS_DEVICE(p3), 0, 0x40000400);

	STM32F4TIM3 *p4 = g_new(STM32F4TIM3, 1);
	object_initialize_child(OBJECT(sms), "TIM4", p4, TYPE_STM32F4_TIM3);
	sysbus_realize(SYS_BUS_DEVICE(p4), &error_fatal);
	p4->base = 0x40000800;
	sysbus_connect_irq(SYS_BUS_DEVICE(p4), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 30));
	sysbus_mmio_map(SYS_BUS_DEVICE(p4), 0, 0x40000800);

	STM32F4TIM3 *p5 = g_new(STM32F4TIM3, 1);
	object_initialize_child(OBJECT(sms), "TIM5", p5, TYPE_STM32F4_TIM3);
	sysbus_realize(SYS_BUS_DEVICE(p5), &error_fatal);
	p5->base = 0x40000c00;
	sysbus_connect_irq(SYS_BUS_DEVICE(p5), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 50));
	sysbus_mmio_map(SYS_BUS_DEVICE(p5), 0, 0x40000c00);

	STM32F4USART *p6 = g_new(STM32F4USART, 1);
	object_initialize_child(OBJECT(sms), "USART1", p6, TYPE_STM32F4_USART);
	qdev_prop_set_chr(DEVICE(p6), "chardev", qemu_chr_new("soc-usart1", "chardev:usart1", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p6), &error_fatal);
	p6->base = 0x40011000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p6), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 37));
	sysbus_mmio_map(SYS_BUS_DEVICE(p6), 0, 0x40011000);

	STM32F4USART *p7 = g_new(STM32F4USART, 1);
	object_initialize_child(OBJECT(sms), "USART2", p7, TYPE_STM32F4_USART);
	qdev_prop_set_chr(DEVICE(p7), "chardev", qemu_chr_new("soc-usart2", "chardev:usart2", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p7), &error_fatal);
	p7->base = 0x40004400;
	sysbus_connect_irq(SYS_BUS_DEVICE(p7), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 38));
	sysbus_mmio_map(SYS_BUS_DEVICE(p7), 0, 0x40004400);

	STM32F4USART *p8 = g_new(STM32F4USART, 1);
	object_initialize_child(OBJECT(sms), "USART3", p8, TYPE_STM32F4_USART);
	qdev_prop_set_chr(DEVICE(p8), "chardev", qemu_chr_new("soc-usart3", "chardev:usart3", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p8), &error_fatal);
	p8->base = 0x40004800;
	sysbus_connect_irq(SYS_BUS_DEVICE(p8), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 39));
	sysbus_mmio_map(SYS_BUS_DEVICE(p8), 0, 0x40004800);

	STM32F4USART *p9 = g_new(STM32F4USART, 1);
	object_initialize_child(OBJECT(sms), "USART6", p9, TYPE_STM32F4_USART);
	qdev_prop_set_chr(DEVICE(p9), "chardev", qemu_chr_new("soc-usart6", "chardev:usart6", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p9), &error_fatal);
	p9->base = 0x40011400;
	sysbus_connect_irq(SYS_BUS_DEVICE(p9), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 71));
	sysbus_mmio_map(SYS_BUS_DEVICE(p9), 0, 0x40011400);

	STM32F4PWR *p10 = g_new(STM32F4PWR, 1);
	object_initialize_child(OBJECT(sms), "PWR", p10, TYPE_STM32F4_PWR);
	sysbus_realize(SYS_BUS_DEVICE(p10), &error_fatal);
	p10->base = 0x40007000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p10), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 1));
	sysbus_mmio_map(SYS_BUS_DEVICE(p10), 0, 0x40007000);

	STM32F4RCC *p11 = g_new(STM32F4RCC, 1);
	object_initialize_child(OBJECT(sms), "RCC", p11, TYPE_STM32F4_RCC);
	sysbus_realize(SYS_BUS_DEVICE(p11), &error_fatal);
	p11->base = 0x40023800;
	sysbus_connect_irq(SYS_BUS_DEVICE(p11), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 5));
	sysbus_mmio_map(SYS_BUS_DEVICE(p11), 0, 0x40023800);

	STM32F4FLASH *p12 = g_new(STM32F4FLASH, 1);
	object_initialize_child(OBJECT(sms), "FLASH", p12, TYPE_STM32F4_FLASH);
	sysbus_realize(SYS_BUS_DEVICE(p12), &error_fatal);
	p12->base = 0x40023c00;
	sysbus_mmio_map(SYS_BUS_DEVICE(p12), 0, 0x40023c00);

	STM32F4GPIOA *p13 = g_new(STM32F4GPIOA, 1);
	object_initialize_child(OBJECT(sms), "GPIOA", p13, TYPE_STM32F4_GPIOA);
	sysbus_realize(SYS_BUS_DEVICE(p13), &error_fatal);
	p13->base = 0x40020000;
	sysbus_mmio_map(SYS_BUS_DEVICE(p13), 0, 0x40020000);

	STM32F4GPIOB *p14 = g_new(STM32F4GPIOB, 1);
	object_initialize_child(OBJECT(sms), "GPIOB", p14, TYPE_STM32F4_GPIOB);
	sysbus_realize(SYS_BUS_DEVICE(p14), &error_fatal);
	p14->base = 0x40020400;
	sysbus_mmio_map(SYS_BUS_DEVICE(p14), 0, 0x40020400);

	STM32F4GPIOI *p15 = g_new(STM32F4GPIOI, 1);
	object_initialize_child(OBJECT(sms), "GPIOC", p15, TYPE_STM32F4_GPIOI);
	sysbus_realize(SYS_BUS_DEVICE(p15), &error_fatal);
	p15->base = 0x40020800;
	sysbus_mmio_map(SYS_BUS_DEVICE(p15), 0, 0x40020800);

	STM32F4GPIOI *p16 = g_new(STM32F4GPIOI, 1);
	object_initialize_child(OBJECT(sms), "GPIOD", p16, TYPE_STM32F4_GPIOI);
	sysbus_realize(SYS_BUS_DEVICE(p16), &error_fatal);
	p16->base = 0x40020c00;
	sysbus_mmio_map(SYS_BUS_DEVICE(p16), 0, 0x40020c00);

	STM32F4GPIOI *p17 = g_new(STM32F4GPIOI, 1);
	object_initialize_child(OBJECT(sms), "GPIOE", p17, TYPE_STM32F4_GPIOI);
	sysbus_realize(SYS_BUS_DEVICE(p17), &error_fatal);
	p17->base = 0x40021000;
	sysbus_mmio_map(SYS_BUS_DEVICE(p17), 0, 0x40021000);

	STM32F4GPIOI *p18 = g_new(STM32F4GPIOI, 1);
	object_initialize_child(OBJECT(sms), "GPIOF", p18, TYPE_STM32F4_GPIOI);
	sysbus_realize(SYS_BUS_DEVICE(p18), &error_fatal);
	p18->base = 0x40021400;
	sysbus_mmio_map(SYS_BUS_DEVICE(p18), 0, 0x40021400);

	STM32F4GPIOI *p19 = g_new(STM32F4GPIOI, 1);
	object_initialize_child(OBJECT(sms), "GPIOG", p19, TYPE_STM32F4_GPIOI);
	sysbus_realize(SYS_BUS_DEVICE(p19), &error_fatal);
	p19->base = 0x40021800;
	sysbus_mmio_map(SYS_BUS_DEVICE(p19), 0, 0x40021800);

	STM32F4GPIOI *p20 = g_new(STM32F4GPIOI, 1);
	object_initialize_child(OBJECT(sms), "GPIOH", p20, TYPE_STM32F4_GPIOI);
	sysbus_realize(SYS_BUS_DEVICE(p20), &error_fatal);
	p20->base = 0x40021c00;
	sysbus_mmio_map(SYS_BUS_DEVICE(p20), 0, 0x40021c00);

	STM32F4GPIOI *p21 = g_new(STM32F4GPIOI, 1);
	object_initialize_child(OBJECT(sms), "GPIOI", p21, TYPE_STM32F4_GPIOI);
	sysbus_realize(SYS_BUS_DEVICE(p21), &error_fatal);
	p21->base = 0x40022000;
	sysbus_mmio_map(SYS_BUS_DEVICE(p21), 0, 0x40022000);

	STM32F4RNG *p22 = g_new(STM32F4RNG, 1);
	object_initialize_child(OBJECT(sms), "RNG", p22, TYPE_STM32F4_RNG);
	sysbus_realize(SYS_BUS_DEVICE(p22), &error_fatal);
	p22->base = 0x50060800;
	sysbus_connect_irq(SYS_BUS_DEVICE(p22), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 81));
	sysbus_mmio_map(SYS_BUS_DEVICE(p22), 0, 0x50060800);

	STM32F4SPI *p23 = g_new(STM32F4SPI, 1);
	object_initialize_child(OBJECT(sms), "SPI1", p23, TYPE_STM32F4_SPI);
	qdev_prop_set_chr(DEVICE(p23), "chardev", qemu_chr_new("soc-spi1", "chardev:spi1", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p23), &error_fatal);
	p23->base = 0x40013000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p23), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 35));
	sysbus_mmio_map(SYS_BUS_DEVICE(p23), 0, 0x40013000);

	STM32F4SPI *p24 = g_new(STM32F4SPI, 1);
	object_initialize_child(OBJECT(sms), "SPI2", p24, TYPE_STM32F4_SPI);
	qdev_prop_set_chr(DEVICE(p24), "chardev", qemu_chr_new("soc-spi2", "chardev:spi2", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p24), &error_fatal);
	p24->base = 0x40003800;
	sysbus_connect_irq(SYS_BUS_DEVICE(p24), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 36));
	sysbus_mmio_map(SYS_BUS_DEVICE(p24), 0, 0x40003800);

	STM32F4SPI *p25 = g_new(STM32F4SPI, 1);
	object_initialize_child(OBJECT(sms), "SPI3", p25, TYPE_STM32F4_SPI);
	qdev_prop_set_chr(DEVICE(p25), "chardev", qemu_chr_new("soc-spi3", "chardev:spi3", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p25), &error_fatal);
	p25->base = 0x40003c00;
	sysbus_connect_irq(SYS_BUS_DEVICE(p25), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 51));
	sysbus_mmio_map(SYS_BUS_DEVICE(p25), 0, 0x40003c00);

	STM32F4SYSCFG *p26 = g_new(STM32F4SYSCFG, 1);
	object_initialize_child(OBJECT(sms), "SYSCFG", p26, TYPE_STM32F4_SYSCFG);
	sysbus_realize(SYS_BUS_DEVICE(p26), &error_fatal);
	p26->base = 0x40013800;
	sysbus_mmio_map(SYS_BUS_DEVICE(p26), 0, 0x40013800);

	STM32F4RTC *p27 = g_new(STM32F4RTC, 1);
	object_initialize_child(OBJECT(sms), "RTC", p27, TYPE_STM32F4_RTC);
	sysbus_realize(SYS_BUS_DEVICE(p27), &error_fatal);
	p27->base = 0x40002800;
	sysbus_connect_irq(SYS_BUS_DEVICE(p27), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 3));
	sysbus_connect_irq(SYS_BUS_DEVICE(p27), 1, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 41));
	sysbus_mmio_map(SYS_BUS_DEVICE(p27), 0, 0x40002800);

	STM32F4EXTI *p28 = g_new(STM32F4EXTI, 1);
	object_initialize_child(OBJECT(sms), "EXTI", p28, TYPE_STM32F4_EXTI);
	qdev_prop_set_chr(DEVICE(p28), "chardev", qemu_chr_new("soc-exti", "chardev:exti", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p28), &error_fatal);
	p28->base = 0x40013c00;
	sysbus_connect_irq(SYS_BUS_DEVICE(p28), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 2));
	sysbus_connect_irq(SYS_BUS_DEVICE(p28), 1, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 6));
	sysbus_connect_irq(SYS_BUS_DEVICE(p28), 2, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 7));
	sysbus_connect_irq(SYS_BUS_DEVICE(p28), 3, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 8));
	sysbus_connect_irq(SYS_BUS_DEVICE(p28), 4, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 9));
	sysbus_connect_irq(SYS_BUS_DEVICE(p28), 5, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 10));
	sysbus_connect_irq(SYS_BUS_DEVICE(p28), 6, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 23));
	sysbus_connect_irq(SYS_BUS_DEVICE(p28), 7, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 23));
	sysbus_connect_irq(SYS_BUS_DEVICE(p28), 8, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 40));
	sysbus_mmio_map(SYS_BUS_DEVICE(p28), 0, 0x40013c00);

	STM32F4I2C *p29 = g_new(STM32F4I2C, 1);
	object_initialize_child(OBJECT(sms), "I2C1", p29, TYPE_STM32F4_I2C);
	qdev_prop_set_chr(DEVICE(p29), "chardev", qemu_chr_new("soc-i2c1", "chardev:i2c1", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p29), &error_fatal);
	p29->base = 0x40005400;
	sysbus_connect_irq(SYS_BUS_DEVICE(p29), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 31));
	sysbus_connect_irq(SYS_BUS_DEVICE(p29), 1, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 32));
	sysbus_mmio_map(SYS_BUS_DEVICE(p29), 0, 0x40005400);

	STM32F4I2C *p30 = g_new(STM32F4I2C, 1);
	object_initialize_child(OBJECT(sms), "I2C2", p30, TYPE_STM32F4_I2C);
	qdev_prop_set_chr(DEVICE(p30), "chardev", qemu_chr_new("soc-i2c2", "chardev:i2c2", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p30), &error_fatal);
	p30->base = 0x40005800;
	sysbus_connect_irq(SYS_BUS_DEVICE(p30), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 33));
	sysbus_connect_irq(SYS_BUS_DEVICE(p30), 1, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 34));
	sysbus_mmio_map(SYS_BUS_DEVICE(p30), 0, 0x40005800);

	STM32F4I2C *p31 = g_new(STM32F4I2C, 1);
	object_initialize_child(OBJECT(sms), "I2C3", p31, TYPE_STM32F4_I2C);
	qdev_prop_set_chr(DEVICE(p31), "chardev", qemu_chr_new("soc-i2c3", "chardev:i2c3", NULL));
	sysbus_realize(SYS_BUS_DEVICE(p31), &error_fatal);
	p31->base = 0x40005c00;
	sysbus_connect_irq(SYS_BUS_DEVICE(p31), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 72));
	sysbus_connect_irq(SYS_BUS_DEVICE(p31), 1, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 73));
	sysbus_mmio_map(SYS_BUS_DEVICE(p31), 0, 0x40005c00);

	STM32F4ADC *p32 = g_new(STM32F4ADC, 1);
	object_initialize_child(OBJECT(sms), "ADC1", p32, TYPE_STM32F4_ADC);
	sysbus_realize(SYS_BUS_DEVICE(p32), &error_fatal);
	p32->base = 0x40012000;
	sysbus_connect_irq(SYS_BUS_DEVICE(p32), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 18));
	sysbus_mmio_map(SYS_BUS_DEVICE(p32), 0, 0x40012000);

	STM32F4ADC *p33 = g_new(STM32F4ADC, 1);
	object_initialize_child(OBJECT(sms), "ADC2", p33, TYPE_STM32F4_ADC);
	sysbus_realize(SYS_BUS_DEVICE(p33), &error_fatal);
	p33->base = 0x40012100;
	sysbus_connect_irq(SYS_BUS_DEVICE(p33), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 18));
	sysbus_mmio_map(SYS_BUS_DEVICE(p33), 0, 0x40012100);

	STM32F4ADC *p34 = g_new(STM32F4ADC, 1);
	object_initialize_child(OBJECT(sms), "ADC3", p34, TYPE_STM32F4_ADC);
	sysbus_realize(SYS_BUS_DEVICE(p34), &error_fatal);
	p34->base = 0x40012200;
	sysbus_connect_irq(SYS_BUS_DEVICE(p34), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 18));
	sysbus_mmio_map(SYS_BUS_DEVICE(p34), 0, 0x40012200);


}

static void stm32f407_common_init(MachineState *machine) {
	STM32F407MachineState *sms = STM32F407_MACHINE(machine);
	MemoryRegion *sysmem = get_system_memory();
	MemoryRegion *mem;
	Clock *cpuclk = clock_new(OBJECT(sms), "SYSCLK");
	clock_set_hz(cpuclk, 168000000);
	Clock *refclk = clock_new(OBJECT(sms), "REFCLK");
	clock_set_hz(refclk, 168000000);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_ram(mem, NULL, "wtf", 0x100000, &error_fatal);
	memory_region_add_subregion(sysmem, 0x0, mem);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_rom(mem, NULL, "main-flash", 0x200000, &error_fatal);
	memory_region_add_subregion(sysmem, 0x8000000, mem);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_rom(mem, NULL, "option-bytes1", 0x10, &error_fatal);
	memory_region_add_subregion(sysmem, 0x1fffc000, mem);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_rom(mem, NULL, "option-bytes2", 0x10, &error_fatal);
	memory_region_add_subregion(sysmem, 0x1ffec000, mem);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_rom(mem, NULL, "system-memory", 0x7a10, &error_fatal);
	memory_region_add_subregion(sysmem, 0x1fff0000, mem);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_ram(mem, NULL, "sram", 0x200000, &error_fatal);
	memory_region_add_subregion(sysmem, 0x20000000, mem);

	mem = g_new(MemoryRegion, 1);
	memory_region_init_ram(mem, NULL, "BKPSRAM", 0x1000, &error_fatal);
	memory_region_add_subregion(sysmem, 0x40024000, mem);


	object_initialize_child(OBJECT(sms), "armv7m", &(sms->armv7m), TYPE_ARMV7M);
	qdev_prop_set_uint32(DEVICE(&(sms->armv7m)),"num-irq", 256);
	qdev_prop_set_string(DEVICE(&(sms->armv7m)), "cpu-type", machine->cpu_type);
	qdev_prop_set_bit(DEVICE(&(sms->armv7m)), "enable-bitband", true);
	qdev_connect_clock_in(DEVICE(&(sms->armv7m)), "cpuclk", cpuclk);
	qdev_connect_clock_in(DEVICE(&(sms->armv7m)), "refclk", refclk);
	qdev_prop_set_uint32(DEVICE(&(sms->armv7m)), "init-nsvtor", 0x8000000);
	object_property_set_link(OBJECT(&sms->armv7m), "memory", OBJECT(sysmem), &error_abort);
	sysbus_realize(SYS_BUS_DEVICE(&sms->armv7m), &error_fatal);

	stm32f407_periph_init(machine);

	armv7m_load_kernel(ARM_CPU(first_cpu), machine->kernel_filename, 0x8000000, 0x200000);
}

static void stm32f407_class_init(ObjectClass *oc, void *data) {
	MachineClass *mc = MACHINE_CLASS(oc);
	mc->desc = "stm32f407";
	mc->init = stm32f407_common_init;
	mc->max_cpus = 1;
	mc->default_cpus = 1;
	mc->default_cpu_type = ARM_CPU_TYPE_NAME("cortex-m4");
}

static const TypeInfo stm32f407_info = {
	.name = TYPE_STM32F407_MACHINE,
	.parent = TYPE_MACHINE,
	.instance_size = sizeof(STM32F407MachineState),
	.class_size = sizeof(STM32F407MachineClass),
	.class_init = stm32f407_class_init
};

static void stm32f407_machine_init(void) {
	type_register_static(&stm32f407_info);
}

type_init(stm32f407_machine_init);
