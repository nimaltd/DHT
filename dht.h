#ifndef _DHT_H
#define _DHT_H

/*
  Author:     Nima Askari
  WebSite:    http://www.github.com/NimaLTD
  Instagram:  http://instagram.com/github.NimaLTD
  Youtube:    https://www.youtube.com/channel/UCUhY7qY1klJm1d2kulr9ckw
  
  Version:    2.0.1
  
  
  Reversion History:
  (2.0.1)
  Fix STM32F4 gpio register.
  
  (2.0.0)
  Rewrite completly and Add external interrupt to reading pulses.
  
  (1.0.0)
  First Release.

*/
#ifdef __cplusplus
 extern "C" {
#endif
//#########################################################################################################################
#include <stdbool.h>
#include "tim.h"
//#########################################################################################################################
typedef enum
{
  DHT_Type_DHT11 = 0,
  DHT_Type_DHT12,
  DHT_Type_DHT21,
  DHT_Type_DHT22,
  DHT_Type_AM2301,
  DHT_Type_AM2305
  
}DHT_Type_t;

typedef struct
{
  TIM_HandleTypeDef   *tim;
  GPIO_TypeDef        *gpio;
  uint16_t            pin;
  DHT_Type_t          type;
  uint8_t             data[84]; 
  uint16_t            cnt;  
  uint32_t            time;
  uint32_t            lastCNT;
  float               temperature;
  float               humidity;
  bool                dataValid;
  
}DHT_t;

//#########################################################################################################################
void  DHT_pinChangeCallBack(DHT_t *dht);
void  DHT_init(DHT_t *dht, DHT_Type_t type, TIM_HandleTypeDef *tim, uint16_t  timerBusFrequencyMHz, GPIO_TypeDef *gpio, uint16_t  pin);
bool  DHT_readData(DHT_t *dht, float *Temperature, float *Humidity);
//#########################################################################################################################
#ifdef __cplusplus
}
#endif
#endif
