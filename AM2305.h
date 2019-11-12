#ifndef	_AM2305_H
#define	_AM2305_H

//Writen By "Nima Askari"			www.gitHub.com/NimaLTD
//version 2.01

#include <stdbool.h>


bool 		AM2305_Init(void);
bool 		AM2305_ReadData(float *pTemp,float *pHumi);    



#endif
