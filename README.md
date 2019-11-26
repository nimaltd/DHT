# DHT Library for STM32 HAL   
Select "General peripheral Initalizion as a pair of '.c/.h' file per peripheral" on project settings.
<br />
2) Enable a Timer with 1 microsecond tick. example for 168 MHz bus: set prescaler to 168-1.
<br />
3) Config "AM2305Config.h".
<br />
4) Call AM2305_Init() and AM2305_ReadData(&temp,&humid) on your app. 

