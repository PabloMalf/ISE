#include "kkk.h"
#include "stm32f4xx_hal.h"


static osThreadId_t id_Th_kkk;
static osMessageQueueId_t id_MsgQueue_kkk;


int init_Th_kkk(void);
static void Th_kkk(void *arg);
static int Init_MsgQueue_kkk(void);


static int Init_MsgQueue_kkk(){
	id_MsgQueue_kkk = osMessageQueueNew(MSGQUEUE_OBJECTS_KKK, sizeof(MSGQUEUE_OBJ_KKK), NULL);
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
		osThreadYield();
	}
}
