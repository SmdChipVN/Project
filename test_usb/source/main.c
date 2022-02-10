//#include <config.h>
//#include <firework.h>
//
///*PIN:
//  LED_DATA: PWM-TIM2_CH1-PA0
//  LED_CLOCK: OUT-PA1
//  BTN_1: IN-PB2
//  BTN_2: IN-PB10
//  BTN_3: IN-PB11
//  AUDIO: PWM-TIM1_CH2N-PB14
//  CS: OUT-PA15
//  SCK: SCK-SPI1-PB3
//  MISO: MISO-SPI1-PB4
//  MOSI: MOSI-SPI1-PB5
//  LED: OUT-PB7
//*/
////=================  system  ==========================//
//static unsigned int _SystemEvent = 0;
//System_TypeDef System = {
//  .MsTick = 0,
//};
//void SysTick_Handler()
//{
//  System.MsTick++;
//  System.Event->MsTick = 1;
//}
//
//void main()
//{
//  System.Event = (SystemEvent_TypeDef*)(BITBAND_SRAM_ADDRESS((unsigned int)(&_SystemEvent), 0));
//  //begin task
//  extern void RTOS_ASM_Begin(CPU_TypeDef* CPU);
//  RTOS_ASM_Begin(&Tasks[0].CPU);
//  Tasks[0].Status = TASK_STATUS_RUNNING;
//  TaskManager.Running = 1;
//  TaskManager.Total = 1;
//  RTOS_CreateTask(Firework_Task);
//  
//  //init rcc: F_SYS: 64MHz
//  FLASH.ACR.BITS.LATENCY = 2;
//  RCCInit_TypeDef RCCInit = (RCCInit_TypeDef){
//    .Source = RCC_SOURCE_PLL,
//    .PLL = { .Source = PLL_SOURCE_HSI_DIV_2, .Multiple = 16 - 2, },     //note: 16 is multiple
//    .Prescale = { .AHB = AHB_PRESCALE_1, .APB1 = APB_PRESCALE_2, .APB2 = APB_PRESCALE_1, .ADC = ADC_PRESCALE_2, .USB_Div1p5 = 1, },
//  };
//  RCC_Init(&RCC, &RCCInit);
//
//  //init gpio
//  RCC_BITBAND.APB2ENR.AFIOEN = 1;
//  AFIO.MAPR.BITS.SWJ_CFG = 2;
//  RCC_BITBAND.APB2ENR.IOPAEN = 1;
//  GPIO_Mode(&GPIOA, BIT0, OUTPUT_AF_PUSHPULL_50MHZ);
//  GPIOA_BITBAND.ODR._15 = 1;
//  GPIO_Mode(&GPIOA, BIT1 | BIT15, OUTPUT_PUSHPULL_50MHZ);
//  RCC_BITBAND.APB2ENR.IOPBEN = 1;
//  GPIOB.ODR.REG = BIT1 | BIT2 | BIT10 | BIT9;
//  GPIO_Mode(&GPIOB, BIT1 | BIT2 | BIT10, INPUT_PULLUP);
//  GPIO_Mode(&GPIOB, BIT14 | BIT3 | BIT4 | BIT5, OUTPUT_AF_PUSHPULL_50MHZ);  
//  GPIO_Mode(&GPIOB, BIT0 | BIT7 | BIT8 | BIT9, OUTPUT_PUSHPULL_50MHZ);  
//  RCC_BITBAND.APB2ENR.IOPCEN = 1;
//  GPIOC.ODR.REG = BIT13 | BIT14 | BIT15;
//  GPIO_Mode(&GPIOC, BIT13 | BIT14 | BIT15, OUTPUT_PUSHPULL_50MHZ);
//  
//
//  //init button
//  Button_Begin(&_ButtonFunction, _Buttons, 3);
//  //init led
//  Led_Begin(&_LedFunction, _Leds, 7);
//  
//  //init system tick
//  STK_Init(64000);
//  
////  //init spi with RX-DMA
////  RCC_BITBAND.APB2ENR.SPI1EN = 1;
////  SPIInit_TypeDef SPIInit = (SPIInit_TypeDef){
////    .BITS = {
////      .Mode = {.Phase = 0, .Polarity = 0, .IsMaster = 1, .Baudrate_Div = 2, .IsLowBitFirst = 0, .IsFrame16Bit = 0, .IsHalfDuplex = 0, },
////      .DMA = { .Rx = 1, .Tx = 0, },
////      .Interrupt = { .Error = 0, .Rx_NotEmpty = 0, .Tx_Empty = 0, },
////    },
////  };  
////  SPI_Init(&SPI1, &SPIInit);
////  RCC_BITBAND.AHBENR.DMA1EN = 1;
////  DMAInit_TypeDef DMAInit = (DMAInit_TypeDef){
////    .Mode = { .IsReadFromMem = 0, .IsCircleMode = 0, .Peri_IsIncrement = 0, .Mem_IsIncrement = 1, .Peri_Size = 0, .Mem_Size = 0, .PriorityLevel = 0, .IsMemToMem = 0, },
////    .Interrupt = { .Trans_Compelete = 0, .HalfTrans = 0, .Error = 0, },
////  };
////  DMA_Init(&DMA_SPI1_RX, &DMAInit);
////  //init led timer with dma
////  RCC_BITBAND.APB1ENR.TIM2EN = 1;
////  GENERAL_TIMER_Init(&TIM2, 0, 200);
////  GENERAL_TIMER_EnableDMA(&TIM2);
////  DMAInit = (DMAInit_TypeDef){
////    .Mode = { .IsReadFromMem = 1, .IsCircleMode = 0, .Peri_IsIncrement = 0, .Mem_IsIncrement = 1, .Peri_Size = 0, .Mem_Size = 0, .PriorityLevel = 0, .IsMemToMem = 0, },
////    .Interrupt = { .Trans_Compelete = 0, .HalfTrans = 0, .Error = 0, },
////  };
////  DMA_Init(&DMA_TIM2_CH1, &DMAInit);
////  //init audio timer
////  RCC_BITBAND.APB2ENR.TIM1EN = 1;
////  ADVANCED_TIMER_Init(&TIM1, 1, 256);
////  ADVANCED_TIMER_GeneratePWM(&TIM1, 2 | BIT7, 128, 0);
//  while (1)
//  {
//    if (System.Event->MsTick)
//    {
//      Led_Process(&_LedFunction, _Leds, 7);
//      Button_Process(&_ButtonFunction, _Buttons, 3);
//      System.Event->MsTick = 0;
//    }
//    if (Button_Press(_Buttons, 0))
//    {
//      Led_Blink(&_LedFunction, _Leds, 0, 5, 100);
//    }
//    if (Button_Press(_Buttons, 1))
//    {
//      Led_Blink(&_LedFunction, _Leds, 1, 5, 100);
//    }
//    if (Button_Press(_Buttons, 2))
//    {
//      Led_Blink(&_LedFunction, _Leds, 2, 5, 100);
//    }
//  }  
//}

__weak
void main()
{
  
  while (1)
  {
    
  }
}