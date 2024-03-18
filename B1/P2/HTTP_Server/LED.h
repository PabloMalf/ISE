#ifndef __LED_H
#define __LED_H

#include "cmsis_os2.h"

#define F_R_ON			1U << 0
#define F_G_ON			1U << 1
#define F_B_ON			1U << 2

#define F_R_OFF			1U << 3
#define F_G_OFF			1U << 4
#define F_B_OFF			1U << 5

#define F_R_500MS		1U << 6
#define F_G_500MS		1U << 7
#define F_B_500MS		1U << 8

//...
//#define KKK				1U << 31

int Init_Th_LED(void);

osThreadId_t get_id_Th_LED(void);

#endif
