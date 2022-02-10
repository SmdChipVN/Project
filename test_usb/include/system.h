#ifndef _SYSTEM_H_
#define _SYSTEM_H_

//#define TEST_LED_BUTTON
//#define TEST_SOUND
#define TEST_MASS

typedef struct
{
  unsigned int MsTick;  
  unsigned int ReShow;
  unsigned int ReDraw;
} SystemEvent_TypeDef;
typedef struct
{
  unsigned int LedType;
  unsigned int RaySize;
  unsigned int ColumnSize;
  unsigned int Speed;
} SystemInfo_TypeDef;
typedef struct
{
  SystemEvent_TypeDef* Event;
  SystemInfo_TypeDef Info;
  struct
  {
    unsigned int MsTime;
    //unsigned char Temp[4][1024];
  } Variables;
} System_TypeDef;

extern __no_init System_TypeDef System;

void System_Init();

#endif