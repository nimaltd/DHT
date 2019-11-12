# AM2301 And AM2305 Library for STM32
<br />
I hope use it and enjoy.
<br />
I use Stm32f407vg and Keil Compiler and Stm32CubeMX wizard.
 <br />
Please Do This ...
<br />
<br />
1) Select "General peripheral Initalizion as a pair of '.c/.h' file per peripheral" on project settings.
<br />
2) Enable a Timer with 1 microsecond tick. example for 168 MHz bus: set prescaler to 168-1.
<br />
3) Config "AM2305Config.h".
<br />
4) Call AM2305_Init() and AM2305_ReadData(&temp,&humid) on your app. 

