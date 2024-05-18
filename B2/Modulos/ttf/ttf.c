#include "ttf.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "rl_fs.h"

#define MSGQUEUE_OBJECTS_TTF 1
#define MAX_DATA 50

static osThreadId_t id_Th_ttf;
static osMessageQueueId_t id_MsgQueue_ttf_miso;
static osMessageQueueId_t id_MsgQueue_ttf_mosi;

int init_Th_ttf(void);
static void Th_ttf(void *arg);
static int Init_MsgQueue_ttf_mosi(void);
static int Init_MsgQueue_ttf_miso(void);

int init_Th_ttf(void){
	const osThreadAttr_t attr = {.stack_size = 4096};
	id_Th_ttf = osThreadNew(Th_ttf, NULL, &attr);
	if(id_Th_ttf == NULL)
		return(-1);
	return(0);
}

static int Init_MsgQueue_ttf_mosi(){
	id_MsgQueue_ttf_mosi = osMessageQueueNew(MSGQUEUE_OBJECTS_TTF, sizeof(MSGQUEUE_OBJ_TTF_MOSI), NULL);
	if(id_MsgQueue_ttf_mosi == NULL)
		return (-1); 
	return(0);
}

static int Init_MsgQueue_ttf_miso(){
	id_MsgQueue_ttf_miso = osMessageQueueNew(MSGQUEUE_OBJECTS_TTF, sizeof(MSGQUEUE_OBJ_TTF_MISO), NULL);
	if(id_MsgQueue_ttf_miso == NULL)
		return (-1); 
	return(0);
}

osMessageQueueId_t get_id_MsgQueue_ttf_mosi(void){
	return id_MsgQueue_ttf_mosi;
}

osMessageQueueId_t get_id_MsgQueue_ttf_miso(void){
	return id_MsgQueue_ttf_miso;
}


static void Th_ttf(void *arguments){	
  fsStatus stat;
	FILE *f;
  MSGQUEUE_OBJ_TTF_MOSI msg_ttf;
	MSGQUEUE_OBJ_TTF_MISO msg_ttf_miso; 
	char c;
	char adtos[MAX_DATA][24];
	char str[24];
	
  int i = 0;
	int j = 0;
	int a = 0;
	Init_MsgQueue_ttf_miso();
  Init_MsgQueue_ttf_mosi();


	while(1){
		if (osOK == osMessageQueueGet(get_id_MsgQueue_ttf_mosi(), &msg_ttf, NULL, osWaitForever)){
			 if(msg_ttf.cmd==WR){
				 	stat = finit ("M0:");
					if (stat == fsOK) {
						stat = fmount ("M0:");
						if (stat == fsOK) {
							f = fopen ("M0:/data.csv","a+");
							if (f != NULL) {
								for(a=0;a<5;a++){
								  fwrite(msg_ttf.data[a], sizeof(char), strlen(msg_ttf.data[a]), f);
								  fwrite(",", sizeof(char), 1, f);
								}
								fflush(f);
								fclose(f);
							}
						}
					}
//					
					stat=funmount("M0:");
					stat=funinit("M0:");
			 }
//		
		 else if(msg_ttf.cmd==RD){
				 	stat = finit ("M0:");
//					if (stat == fsOK) {
//						stat = fmount ("M0:");
//						if (stat == fsOK) {
//							f = fopen ("M0:/data.csv","r");
//							if (f != NULL) {

//								memset(str, '\0', sizeof(str));
//								while (!feof (f)) {
//								 c = fgetc(f);
//								 if(c!=',')
//							  	strcat(str,&c);
//								 else{
//									 strcat(str, "\0");
//									 strcpy(adtos[i], str); 
//								   memset(str, '\0', sizeof(str));
//								   i++;
//								 }
//								}
//								rewind(f);
//								fclose(f);
//							}
//								
//								
//						}
//					}
//					
//					stat=funmount("M0:");
//					stat=funinit("M0:");
//					for (j = 0; j < 50; j++) {
//           strcpy(msg_ttf_miso.adtos[j], adtos[j]);
//           }
//					
//					osMessageQueuePut(get_id_MsgQueue_ttf_miso(), &msg_ttf_miso, NULL, osWaitForever);
			 }
	
	  }
		
  }
}
