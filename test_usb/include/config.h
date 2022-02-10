#ifndef _CONFIG_H_
#define _CONFIG_H_

//#include <stm32f103c6t6.h>
////==============  RTOS function  ==============
//#define RTOS_MAX_TASK           2
//#define RTOS_STACK_SIZE         256
//#define RTOS_SWITCH_TRIGGER()   asm("SVC #0")//SCB.ICSR.BITS.PENDSVSET = 1
//#include <system.h>
//#define RTOS_GETTIME()          System.MsTick
//#define RTOS_ENABLE_IRQ()       asm("CPSIE I");
//#define RTOS_DISABLE_IRQ()      asm("CPSID I");

//==============================================================================================================================================================
#define BUTTON_QUANTITY_1                       3
#define BUTTON_SAMPLE_TIMES_1                   5
#define BUTTON_SAMPLE_DIVIDER_1                 4
#define BUTTON_PRESS_DOWN_TIME_1                10
//==============================================================================================================================================================
#define LED_QUANTITY_1                          7
//==============================================================================================================================================================
#include <gpio.h>
extern volatile GPIO_BITBAND_TypeDef GPIOA_BITBAND;
#include <spi.h>
extern volatile SPI_TypeDef SPI1;
#include <dma.h>
extern volatile DMA_TypeDef DMA1;
#include <system.h>
extern reg8 FlashIC_CheckWriteEnableTimeout(reg8 Stage);
extern void SPI1_ReadDMA(unsigned char* RXBuffer, reg16 Size, void (*EndFunction)());
extern void (*SPI1_DMA_TX_CompleteFunction)();
#define FLASHIC_CONFIG_CS_0(IsSet)                              GPIOA_BITBAND.ODR.b15 = IsSet
#define FLASHIC_SPI_WRITE_BYTE_0(Data)                          SPI_WriteRead(&SPI1, Data)                      //return Data
#define FLASHIC_SPI_WAIT_0()                                    while (SPI1.SR.BITS.BSY)
#define FLASHIC_WRITE_ENABLE_TIME_OUT_0(Stage)                  FlashIC_CheckWriteEnableTimeout(Stage)  //reg8 FlashIC_CheckWriteEnableTimeout(reg8 Stage = 1: begin, 2: end, 0: check), return 1 to timeout
                                                                                                        //or false to disable (maybe infinity loop)
#define FLASHIC_SPI_DMA_RX(RXBuffer, Size, EndFunction)         SPI1_ReadDMA(RXBuffer, Size, EndFunction)
#define FLASHIC_SPI_DMA_TX(Data, Size, EndFunction)             SPI1_DMA_TX_CompleteFunction = EndFunction; DMA_Transfer(&DMA_SPI1_TX, Data, &SPI1.DR, Size)

#define USBP_BY_INTERRUPT

extern volatile GPIO_BITBAND_TypeDef GPIOB_BITBAND;
extern volatile GPIO_TypeDef GPIOB;
#define SSPI_MSB_0                              1
#define SSPI_POLARITY_0                         0
#define SSPI_CONFIG_MOSI_0(IsSet)               GPIOB_BITBAND.ODR.b5 = IsSet
#define SSPI_READ_MISO_0()                      GPIOB_BITBAND.IDR.b4
#define SSPI_CONFIG_SCK_0(IsSet)                GPIOB_BITBAND.ODR.b6 = IsSet

#define LOW                                     0
#define HIGH                                    1
#define SSPI_CONFIG_NSS(IsSet)                  GPIOB_BITBAND.ODR.b7 = IsSet
#define CONFIG_RESET(IsSet)                     GPIOB_BITBAND.ODR.b3 = IsSet
#define READ_RESET()                            GPIOB_BITBAND.IDR.b3

#include <soft_spi.h>
#define MFRC522_CONFIG_NSS_0(IsSet)             GPIOB_BITBAND.ODR.b7 = IsSet
#define MFRC522_SPI_WRITEBYTE(Data)             SSPI_WriteByte_0(Data)

#endif