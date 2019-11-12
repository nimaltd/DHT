
#include "AM2305.h"
#include "AM2305Config.h"
#include "gpio.h"
#include "tim.h"
#if  (_AM2305_USE_FREERTOS==1)   
#include "CMSIS_os.h"
#define AM2305_DelayMs(x)     osDelay(x)
#else
#define AM2305_DelayMs(x)     HAL_Delay(x)
#endif

uint8_t AM2305_ReadRawData[5];
int8_t  AM2305_ReadDataArray[40];
int32_t AM2305_ReadTime[2];
//#############################################################################################
void    AM2305_DelayUs(uint16_t DelayUs)
{
  _AM2305_TIMER.Instance->CNT=0;
  while(_AM2305_TIMER.Instance->CNT<DelayUs);  
}
//#############################################################################################
void  AM2305_PortInput(void)
{
  GPIO_InitTypeDef  gpio;
  gpio.Mode = GPIO_MODE_INPUT;
  gpio.Speed = GPIO_SPEED_FREQ_HIGH;
  gpio.Pin = _AM2305_PIN;
  gpio.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(_AM2305_GPIO,&gpio);  
}
//#############################################################################################
void  AM2305_PortOutput(void)
{
  GPIO_InitTypeDef  gpio;
  HAL_GPIO_WritePin(_AM2305_GPIO,_AM2305_PIN,GPIO_PIN_SET);
  gpio.Mode = GPIO_MODE_OUTPUT_OD;
  gpio.Speed = GPIO_SPEED_FREQ_HIGH;
  gpio.Pin = _AM2305_PIN;
  gpio.Pull = GPIO_NOPULL;   
  HAL_GPIO_Init(_AM2305_GPIO,&gpio); 
}
//#############################################################################################
int32_t  AM2305_WaitForChangeState(uint16_t TimeoutUs,GPIO_PinState StartState)
{
  AM2305_DelayUs(2);
  StartState = HAL_GPIO_ReadPin(_AM2305_GPIO,_AM2305_PIN);
  _AM2305_TIMER.Instance->CNT=0;
  while(_AM2305_TIMER.Instance->CNT <= TimeoutUs)
  {
    if(HAL_GPIO_ReadPin(_AM2305_GPIO,_AM2305_PIN) != StartState)
      return _AM2305_TIMER.Instance->CNT;   
  }
  return -1;  
}
//#############################################################################################
bool  AM2305_StartToRead(void)
{  
  AM2305_PortOutput();
  HAL_GPIO_WritePin(_AM2305_GPIO,_AM2305_PIN,GPIO_PIN_RESET);
  AM2305_DelayMs(5);
  HAL_GPIO_WritePin(_AM2305_GPIO,_AM2305_PIN,GPIO_PIN_SET);
  AM2305_DelayUs(20);    
  HAL_GPIO_WritePin(_AM2305_GPIO,_AM2305_PIN,GPIO_PIN_RESET);
  AM2305_DelayUs(2);
  AM2305_PortInput(); 
  AM2305_ReadTime[0] = AM2305_WaitForChangeState(100,GPIO_PIN_RESET);
  AM2305_ReadTime[1] = AM2305_WaitForChangeState(100,GPIO_PIN_SET);
  if((AM2305_ReadTime[0] >= 65) && (AM2305_ReadTime[0]<=100) && (AM2305_ReadTime[1] >= 65) && (AM2305_ReadTime[1]<=100))
    return true;
  else
  {
    AM2305_PortOutput();
    return false;  
  }
}
//#############################################################################################
int8_t    AM2305_ReadBit(void)
{
  int32_t ReadTime[2];
  ReadTime[0] = AM2305_WaitForChangeState(70,GPIO_PIN_RESET);
  ReadTime[1] = AM2305_WaitForChangeState(100,GPIO_PIN_SET); 
  if((ReadTime[0] >= 30) && (ReadTime[0]<=70) && (ReadTime[1] >= 10) && (ReadTime[1]<=50))
    return 0;    
  else if((ReadTime[0] >= 30) && (ReadTime[0]<=70) && (ReadTime[1] >= 50) && (ReadTime[1]<=100))
    return 1;
  else
    return -1; 
}
//#############################################################################################
bool 		AM2305_Init(void)
{
  _AM2305_TIMER.Instance->ARR = 0xFFFF;
  HAL_TIM_Base_Start(&_AM2305_TIMER);
  AM2305_PortOutput();
  while(HAL_GetTick() < 3000)
    AM2305_DelayMs(1);
  AM2305_DelayMs(1000);
  return AM2305_StartToRead();
}
//#############################################################################################
bool 		AM2305_ReadData(float *pTemp,float *pHumi)
{
  if(AM2305_StartToRead()==false)
    return false; 
  for(uint8_t i=0 ; i<5; i++)
    AM2305_ReadRawData[i]=0;
  for(uint8_t i=0 ; i<40; i++)
   AM2305_ReadDataArray[i] = AM2305_ReadBit(); 
  for(uint8_t i=0 ; i<40; i++)
  {
    if(AM2305_ReadDataArray[i] == -1)
      return false;    
    if(AM2305_ReadDataArray[i]==1)
      AM2305_ReadRawData[i/8] |= 1<<((8-(i%8))-1);
  }
  if(((AM2305_ReadRawData[0]+AM2305_ReadRawData[1]+AM2305_ReadRawData[2]+AM2305_ReadRawData[3])&0x00FF)==AM2305_ReadRawData[4])
  {
    if(pHumi!=NULL)
    {
      *pHumi = ((AM2305_ReadRawData[0]*256)+AM2305_ReadRawData[1]) / 10.0f;      
    }
    if(pTemp!=NULL)
    {
      *pTemp = ((AM2305_ReadRawData[2]*256)+AM2305_ReadRawData[3]) / 10.0f;      
    }  
    return true;    
  }
  else
    return false;    
}
//#############################################################################################
