#include <system.h>
#ifdef TEST_MASS
//==============================================================================================================================================================
//==============================================================================================================================================================
//==============================================================================================================================================================
/*PIN:
  CS: OUT-PA15
  SCK: SCK-SPI1-PB3
  MISO: MISO-SPI1-PB4
  MOSI: MOSI-SPI1-PB5
*/
#include <stm32f103c6t6.h>
#include <flashIC.h>
#include <USB_HID.h>
#include <USB_CDC.h>
#include <USB_MASS.h>
//==============================================================================================================================================================
__no_init FlashIC_TypeDef FlashIC;
reg8 FlashIC_CheckWriteEnableTimeout(reg8 Stage)
{
  static unsigned int begin_check_at;
  if (Stage)
  {
    begin_check_at = System.Variables.MsTime;
    return 0;
  }
  return (System.Variables.MsTime - begin_check_at) > 100;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void (*SPI1_DMA_RX_CompleteFunction)() = NULL;
void DMA1_CN2_IRQHandler()
{
  DMA1.IFCR.REG = 0xFFFFFFFF;
  DMA_SPI1_TX.CCR.BITS.TCIE = 1;
  if (SPI1_DMA_RX_CompleteFunction)
    SPI1_DMA_RX_CompleteFunction();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void SPI1_ReadDMA(unsigned char* RXBuffer, reg16 Size, void (*EndFunction)())
{
  SPI1_DMA_RX_CompleteFunction = EndFunction;
  DMA_Transfer(&DMA_SPI1_RX, RXBuffer, &SPI1.DR, Size);
  DMA_SPI1_TX.CCR.BITS.TCIE = 0;
  DMA_Transfer(&DMA_SPI1_TX, RXBuffer, &SPI1.DR, Size);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void (*SPI1_DMA_TX_CompleteFunction)() = NULL;
void DMA1_CN3_IRQHandler()
{
  DMA1.IFCR.REG = 0xFFFFFFFF;
  if (SPI1_DMA_TX_CompleteFunction)
    SPI1_DMA_TX_CompleteFunction();
}
//==============================================================================================================================================================
void SysTick_Handler()
{
  System.Variables.MsTime++;
  System.Event->MsTick = 1;
}
//==============================================================================================================================================================
void BaseProcess()
{

}
//==============================================================================================================================================================
void Delay_ms(unsigned int Time)
{
  unsigned int now = System.Variables.MsTime;
  while ((System.Variables.MsTime - now) < Time)
    BaseProcess();
}

void HardFault_Handler()
{
  return;
}


reg8 USB_HID_Received(reg8 EndpointIndex, const unsigned char* Data, reg16 Length)
{
  if (EndpointIndex)
    USBP_Send(EndpointIndex, "ABCD", 64);
  else
  {
    USB_HID_SendEndpoint0("EFGH", 64);
  }
  return 0;
}
reg8 USB_CDC_Received(reg8 EndpointIndex, const unsigned char* Data, reg16 Length)
{
  USBP_Send(1, "XYZW\r\n", 6);
  return 0;
}
void USB_MASS_ProcessRequest(unsigned char* Data, reg16 Length)
{
  
}
unsigned char sd_card = 0;
//==============================================================================================================================================================
void main()
{
  //init pin
  RCC_BITBAND.APB2_ENR.AFIO = 1;
  AFIO.MAPR.BITS.SWJ_CFG = 2;
  RCC_BITBAND.APB2_ENR.IOPA = 1;
  GPIO_Mode(&GPIOA, BIT15, GPIO_MODE_OUTPUT_PUSHPULL_50MHz);
  GPIO_Mode(&GPIOA, BIT11 | BIT12, GPIO_MODE_AF_OUTPUT_PUSHPULL_50MHz);
  RCC_BITBAND.APB2_ENR.IOPB = 1;
  GPIOB.ODR.REG = BIT4;
  GPIO_Mode(&GPIOB, BIT4, GPIO_MODE_INPUT_PULL);
  GPIO_Mode(&GPIOB, BIT7 | BIT8, GPIO_MODE_OUTPUT_PUSHPULL_50MHz);
  GPIO_Mode(&GPIOB, BIT3 | BIT5 | BIT14, GPIO_MODE_AF_OUTPUT_PUSHPULL_50MHz);
  //init variables

  //init driver
  FLASH.ACR.BITS.LATENCY = 2;
  //RCC: Source = PLL (48MHz); PLL: Source = HSI/2 (4MHz) & Multiple = 12; AHB = 48MHz (max 72Mhz), APB1 = 24MHz (max 36MHz), APB2 = 48MHz (max 72Mhz), ADC = 12MHz (max 14Mhz), USB = must 48MHz
  RCCInit_TypeDef RCCInit = (RCCInit_TypeDef){
    .Source = RCC_SOURCE_PLL,
    .PLL = {.BITS = { .Source = PLL_SOURCE_HSI_DIV_2, .Multiple = 10, }},         //Real multiple += 2
    .Prescale = {.BITS = { .AHB = AHB_PRESCALE_1, .APB1 = APB_PRESCALE_2, .APB2 = APB_PRESCALE_1, .ADC = ADC_PRESCALE_4, .USB_NotDiv = 1, }},     
  };
  RCC_Init(&RCC, &RCCInit);
  {
    static unsigned char USBRX_Buff[64];
    static USBEndpointBuffer_TypeDef USBEndpointBuffer[] = {
      {.RXLength = 0, .TXLength = 0, .RXData = USBRX_Buff, .TXData = NULL},
      {.RXLength = 0, .TXLength = 0, .RXData = USBRX_Buff, .TXData = NULL},
      {.RXLength = 0, .TXLength = 0, .RXData = USBRX_Buff, .TXData = NULL},
    };
    USBP_Init(USBEndpointBuffer);
    
    //USB_CDC_Init();
    
    USB_MASS_Init();
    
    //static unsigned char hid_buff[128];
    //USB_HID_Init(hid_buff);
    
    RCC_BITBAND.APB1_ENR.USB = 1;
    const USBEndpointAddress_TypeDef USBEndpointAddress[] = {
      (USBEndpointAddress_TypeDef){.TX = 16, .RX = 16 + 64},            //endpoint 0
      (USBEndpointAddress_TypeDef){.TX = 16 + 128, .RX = 16 + 192},     //endpoint 1
      (USBEndpointAddress_TypeDef){.TX = 16 + 256, .RX = 16 + 320},     //endpoint 2
    };
    USB_Init(USBEndpointAddress);
    NVIC.ISER.BITS.USB_LP_CAN1_RX0 = 1;
  }
  asm("CPSIE I");

//  System_Init();
//  STK_Init(48000);
  
  
  //SPI 1 (12MHz, DMA-TX-RX) remap
  RCC_BITBAND.APB2_ENR.SPI1 = 1;
  AFIO_BITBAND.MAPR.SPI1 = 1;
  SPIInit_TypeDef SPIInit = (SPIInit_TypeDef){
    .Mode = {.BITS = {.Phase = 0, .Polarity = 0, .IsMaster = 1, .Baudrate_Div = 1, .IsLowBitFirst = 0, .IsFrame16Bit = 0, .IsHalfDuplex = 0, }, },  //F_SPI = F_BUS / 2 ^ (Baudrate_Div + 1) (MAX 18MHz)
    .DMA = {.BITS = {  .Rx = 1, .Tx = 1, }, }, 
    .Interrupt = {.BITS = {  .Error = 0, .Rx_NotEmpty = 0, .Tx_Empty = 0, }, }, 
  };
  SPI_Init(&SPI1, &SPIInit);
  //DMA for SPI1
  RCC_BITBAND.AHB_ENR.DMA1 = 1;
  DMAInit_TypeDef DMAInit = (DMAInit_TypeDef){
    .Mode = {.BITS = { .IsReadFromMem = 0, .IsCircleMode = 0, .Peri_IsIncrement = 0, .Mem_IsIncrement = 1, .Peri_Size = 0, .Mem_Size = 0, .PriorityLevel = 0, .IsMemToMem = 0, },},  //size: 0: 1 byte / 1: 2 byte / 2: 4 byte
    .Interrupt = {.BITS = {.Trans_Compelete = 1, .HalfTrans = 0, .Error = 0, }, },
  };
  DMA_Init(&DMA_SPI1_RX, &DMAInit);
  DMAInit.Mode.BITS.IsReadFromMem = 1;
  DMA_Init(&DMA_SPI1_TX, &DMAInit);
  NVIC.ISER.BITS.DMA1_CN2 = 1;
  NVIC.ISER.BITS.DMA1_CN3 = 1;
  
  sd_card = FlashIC_GetInfo_0();
  
  while (1)
  {
    USBP_Process();
//    static unsigned int now = 0;
//    if ((System.Variables.MsTime - now) > 1000)
//    {
//      if (USB_CDC_Manager.PortState)
//        USBP_Send(1, "ABCD\r\n", 6);
//      now = System.Variables.MsTime;
//    }
  }
}

//#include <soft_spi.h>
//#include <mfrc522.h>
//#include <config.h>
//
//__root unsigned char xx[8];
//
//extern Uid uid;
//
//void main()
//{
//  RCC_BITBAND.APB2_ENR.AFIO = 1;
//  AFIO.MAPR.BITS.SWJ_CFG = 2;
//  RCC_BITBAND.APB2_ENR.IOPB = 1;
//  GPIOB.ODR.REG = BIT3 | BIT4 | BIT7;
//  GPIO_Mode(&GPIOB, BIT3 | BIT4, GPIO_MODE_INPUT_PULL);
//  GPIO_Mode(&GPIOB, BIT5 | BIT6 | BIT7, GPIO_MODE_OUTPUT_PUSHPULL_50MHz);
//    //init driver
//  FLASH.ACR.BITS.LATENCY = 2;
//  //RCC: Source = PLL (48MHz); PLL: Source = HSI/2 (4MHz) & Multiple = 12; AHB = 48MHz (max 72Mhz), APB1 = 24MHz (max 36MHz), APB2 = 48MHz (max 72Mhz), ADC = 12MHz (max 14Mhz), USB = must 48MHz
//  RCCInit_TypeDef RCCInit = (RCCInit_TypeDef){
//    .Source = RCC_SOURCE_PLL,
//    .PLL = {.BITS = { .Source = PLL_SOURCE_HSI_DIV_2, .Multiple = 10, }},         //Real multiple += 2
//    .Prescale = {.BITS = { .AHB = AHB_PRESCALE_1, .APB1 = APB_PRESCALE_2, .APB2 = APB_PRESCALE_1, .ADC = ADC_PRESCALE_4, .USB_NotDiv = 1, }},     
//  };
//  RCC_Init(&RCC, &RCCInit);  
//
//  PCD_Init();
//  
//  while (1)
//  {
////    static const unsigned char addr[] = {0x80 | (0x07 << 1), 0x08 << 1, 0x09 << 1, 0x0A, 0x00, };
////    SSPI_CONFIG_NSS(0);
////    SSPI_WriteByte_0(addr[0]);
////    for (unsigned int i = 1; i < sizeof(addr); i++)
////      xx[i - 1] = SSPI_WriteByte_0(addr[i]);
////    SSPI_CONFIG_NSS(1);
//    
//    //extern reg8 MFRC522_ReadCard(unsigned char* CardUID);    
//    if (MFRC522_ReadCard(xx) == 1)
//      xx[4]++;
//    //for (unsigned int i = 0; i < 500000; i++);
//    
////    if(PICC_IsNewCardPresent())
////    {
////      PICC_ReadCardSerial();
////      for (unsigned int i = 0; i < 500000; i++);
////    }
//  }
//}

//==============================================================================================================================================================
//==============================================================================================================================================================
//==============================================================================================================================================================
#endif