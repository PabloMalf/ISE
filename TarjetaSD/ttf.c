#include "ttf.h"
#include <string.h>
#include <stdio.h>
#include "rl_fs.h"

static osThreadId_t id_Th_ttf;
static osMessageQueueId_t id_MsgQueue_ttf;

#define MSGQUEUE_OBJECTS_TTF 4

int init_Th_ttf(void);
static void Th_ttf(void *arg);
static int Init_MsgQueue_ttf(void);

int init_Th_ttf(void){
	id_Th_ttf = osThreadNew(Th_ttf, NULL, NULL);
	if(id_Th_ttf == NULL)
		return(-1);
	return(Init_MsgQueue_ttf());
}

static int Init_MsgQueue_ttf(){
	id_MsgQueue_ttf = osMessageQueueNew(MSGQUEUE_OBJECTS_TTF, sizeof(MSGQUEUE_OBJ_TTF), NULL);
	if(id_MsgQueue_ttf == NULL)
		return (-1); 
	return(0);
}

osMessageQueueId_t get_id_MsgQueue_ttf(void){
	return id_MsgQueue_ttf;
}

static void Th_ttf(void *arguments){	
	uint8_t num;
	char data[] = "Hey que pasa jaja ";
	fsStatus stat;
	FILE *f;

	while(1){
		if (osOK == osMessageQueueGet(get_id_MsgQueue_ttf(), &num, NULL, osWaitForever)){
		 stat = finit ("M0:");
			if (stat == fsOK) {
				stat = fmount ("M0:");
				if (stat == fsOK) {
								f = fopen ("M0:/test.txt","a+");
					if (f != NULL) {
				    fflush (stdout);
						fwrite(data, sizeof(char), strlen(data), f);
						fclose(f);
				  }
			  }
		  }
	  }
  }
}
