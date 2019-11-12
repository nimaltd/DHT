#ifndef	_AM2305CONFIG_H_
#define	_AM2305CONFIG_H_

// Enable A Timer on CubeMX , for 1 microsecond Tick , example: for 168MHZ= 168-1 prescaller and set auto reload to 65000

#define		_AM2305_TIMER						    htim7
#define   _AM2305_USE_FREERTOS        1
#define		_AM2305_GPIO  							GPIOD
#define		_AM2305_PIN		  						GPIO_PIN_8


#endif

