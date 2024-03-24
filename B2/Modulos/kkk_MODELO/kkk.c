#include "kkk.h"
#include "stm32f4xx_hal.h"

#define  S	1000000U
#define MS	1000U
#define US	1U


static osThreadId_t id_Th_kkk;
static osMessageQueueId_t id_MsgQueue_kkk;


int init_Th_kkk(void);
static void Th_kkk(void *arg);
static int Init_MsgQueue_kkk(void);


static int Init_MsgQueue_kkk(){
	id_MsgQueue_kkk = osMessageQueueNew(MSGQUEUE_OBJECTS_kkk, sizeof(MSGQUEUE_OBJ_kkk), NULL);
	if(id_MsgQueue_kkk == NULL)
		return (-1); 
	return(0);
}


int init_Th_kkk(void){
	id_Th_kkk = osThreadNew(Th_kkk, NULL, NULL);
	if(id_Th_kkk == NULL)
		return(-1);
	return(Init_MsgQueue_kkk());
}


osMessageQueueId_t get_id_MsgQueue_kkk(void){
	return id_MsgQueue_kkk;
}


static void Th_kkk(void *argument){

	while(1){
		
	}
}














