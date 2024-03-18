# Specification Violation Bugs Found by Perry

* [STM32CubeF0](https://github.com/STMicroelectronics/STM32CubeF0/issues/18)
    * wrong `RCC_CR2_HSI48RDY_BitNumber`
* [mcux-sdk](https://github.com/nxp-mcuxpresso/mcux-sdk/issues/150)
    * `MCG_C4[DMX32]` and `MCG_C4[DRST_DRS]` are not updated in `CLOCK_SetFbeMode()`