
#include "dht.h"
#include "dhtConf.h"
#include "tim.h"
#if (_DHT_USE_FREERTOS==1)
#include "cmsis_os.h"
#define DHT_delayMs(x)     osDelay(x)
#else
#define DHT_delayMs(x)     HAL_Delay(x)
#endif

//###############################################################################################################
void  DHT_pinChangeCallBack(DHT_t *dht)
{
  dht->time = HAL_GetTick();
  if(dht->cnt < sizeof(dht->data)-1)
  {
    dht->data[dht->cnt] = dht->tim->Instance->CNT - dht->lastCNT;
    dht->lastCNT = dht->tim->Instance->CNT;
    dht->cnt++;
  }
}
//###############################################################################################################
void  DHT_delayUs(DHT_t *dht, uint16_t DelayUs)
{
  dht->tim->Instance->CNT=0;
  while(dht->tim->Instance->CNT < DelayUs);  
}
//###############################################################################################################
void  DHT_output(DHT_t *dht)
{
  GPIO_InitTypeDef  gpio;
  dht->gpio->BSRR = dht->pin;
  gpio.Mode = GPIO_MODE_OUTPUT_OD;
  gpio.Speed = GPIO_SPEED_FREQ_HIGH;
  gpio.Pin = dht->pin;
  gpio.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(dht->gpio,&gpio);    
}
//###############################################################################################################
void  DHT_input(DHT_t *dht)
{
  GPIO_InitTypeDef  gpio;
  gpio.Mode = GPIO_MODE_IT_RISING_FALLING;
  gpio.Speed = GPIO_SPEED_FREQ_HIGH;
  gpio.Pin = dht->pin;
  gpio.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(dht->gpio,&gpio);    
}
//###############################################################################################################
bool  DHT_decode(DHT_t *dht,uint8_t *byteArray)
{
  int8_t bit;
  switch(dht->type)
  {
    case DHT_Type_DHT21:
    case DHT_Type_AM2301:
    case DHT_Type_AM2305:
      if((dht->data[0] < 60) || (dht->data[0] > 100) || (dht->data[1] < 60) || (dht->data[1] > 100))
        return false;      
      bit = 7;
      for(uint8_t i=0 ; i<80 ; i+=2)
      {
        if((dht->data[i+2] >= 35) && (dht->data[i+2] <= 70))
        {
          if((dht->data[i+3] >= 10) && (dht->data[i+3] <= 45))
            *byteArray &= ~(1<<bit); 
          else if((dht->data[i+3] >= 55) && (dht->data[i+3] <= 95))
            *byteArray |= (1<<bit); 
          else
            return false;
          bit--;
          if(bit == -1)
          {
            bit = 7;
            byteArray++;
          }
        }
        else
          return false;  
      }
    return true;  
    default:
      return false;        
  }   
}
//###############################################################################################################
void  DHT_init(DHT_t *dht, DHT_Type_t type, TIM_HandleTypeDef *tim,uint16_t  timerBusFrequencyMHz, GPIO_TypeDef *gpio, uint16_t  pin)
{
  dht->tim = tim;
  dht->gpio = gpio;
  dht->pin = pin;
  dht->type = type;
  DHT_output(dht); 
  dht->tim->Init.Prescaler = timerBusFrequencyMHz - 1;
  dht->tim->Init.CounterMode = TIM_COUNTERMODE_UP;
  dht->tim->Init.Period = 0xFFFF;
  dht->tim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  HAL_TIM_Base_Init(dht->tim);
  HAL_TIM_Base_Start(dht->tim);
  while(HAL_GetTick()<2000)
    DHT_delayMs(1);
  DHT_delayMs(20);
}
//###############################################################################################################
bool  DHT_readData(DHT_t *dht, float *Temperature, float *Humidity)
{
  uint32_t  startTime;
  switch(dht->type)
  {
    case DHT_Type_DHT21:
    case DHT_Type_AM2301:
    case DHT_Type_AM2305:
      DHT_output(dht);
      dht->gpio->BRR = dht->pin;    
      DHT_delayMs(5);
      dht->gpio->BSRR = dht->pin;
      DHT_delayUs(dht,20);
      dht->gpio->BRR = dht->pin;    
      DHT_delayUs(dht,5);
      dht->cnt = 0;
      dht->lastCNT = 0;
      dht->tim->Instance->CNT = 0;
      startTime = HAL_GetTick();
      DHT_input(dht);
      while(1)
      {
        if(HAL_GetTick() - startTime > 8)
          goto ERROR;
        if(HAL_GetTick() - dht->time > 1)
        {
          uint8_t data[5];
          if(DHT_decode(dht,data) == false)
            goto ERROR;
          if(((data[0] + data[1] + data[2] + data[3]) & 0x00FF) != data[4])
            goto ERROR;
          dht->temperature = (float)(data[2]*256 + data[3]) / 10.0f;
          if(Temperature != NULL)
            *Temperature = dht->temperature;
          dht->humidity = (float)(data[0]*256 + data[1]) / 10.0f;
          if(Humidity != NULL)
            *Humidity = dht->humidity;
          dht->dataValid = true;
          DHT_output(dht);
          return true;
        }
      }        
      default:
      break;    
  }
  ERROR:
  dht->dataValid = false;
  DHT_output(dht);
  return false;    
}
//###############################################################################################################
