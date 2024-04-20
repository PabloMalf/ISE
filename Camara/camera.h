#ifndef __CAMERA_H
#define __CAMERA_H

#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"


#define MSGQUEUE_OBJECTS_CAMERA 4

/* PERIFERICO USADO:
	 PINOUT USED:  
    DCMI GPIO Configuration:
		 PA4  -> DCMI_HSYNC (HS: Horizontal synchronization input)
     PA6  -> DCMI_PIXCK (PLK: Pixel Clock Input)
		 PA8  -> MCO1 (XLK Clock out)
     PB6  -> DCMI_D5    (D5)
     PC6  -> DCMI_D0    (D0)
     PC7  -> DCMI_D1    (D1)
     PC8  -> DCMI_D2    (D2)
     PC9  -> DCMI_D3    (D3)
     PC11 -> DCMI_D4    (D4)
     PE5  -> DCMI_D6    (D6)
     PE6  -> DCMI_D7    (D7)		 
     PG9  -> DCMI_VSYNC (VS: Vertical Synchronization input)	
		 
		 

     12/18
			 
			 
*/

typedef struct{
	char  k;
} MSGQUEUE_OBJ_CAMERA;

int init_Th_camera(void);
osMessageQueueId_t get_id_MsgQueue_camera(void);

#endif
