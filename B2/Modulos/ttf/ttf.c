#include "ttf.h"
#include <string.h>
#include <stdio.h>
#include "rl_fs.h"
#include <stdlib.h>

#define MSGQUEUE_OBJECTS_TTF 1
#define MAX_DATA 50
#define MAX_SIZE 100

static osThreadId_t id_Th_ttf;
static osMessageQueueId_t id_MsgQueue_ttf_miso;
static osMessageQueueId_t id_MsgQueue_ttf_mosi;

char basura [2];

int init_Th_ttf(void);
static void Th_ttf(void *arg);
static int Init_MsgQueue_ttf_mosi(void);
static int Init_MsgQueue_ttf_miso(void);

int init_Th_ttf(void){
	id_Th_ttf = osThreadNew(Th_ttf, NULL, NULL);
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
	char adtos[MAX_DATA][20];
	char str[20];
	
  int i = 0;
	int j = 0;
	Init_MsgQueue_ttf_miso();
  Init_MsgQueue_ttf_mosi();


	while(1){
		if (osOK == osMessageQueueGet(get_id_MsgQueue_ttf_mosi(), &msg_ttf, NULL, osWaitForever)){
			 if(msg_ttf.cmd==WR){
				 	stat = finit ("M0:");
					if (stat == fsOK) {
						stat = fmount ("M0:");
						if (stat == fsOK) {
							f = fopen ("M0:/test.txt","a+");
							if (f != NULL) {
								fwrite(msg_ttf.data, sizeof(char), strlen(msg_ttf.data), f);
								fwrite(",", sizeof(char), 1, f);
								fclose(f);
							}
						}
					}
					
					stat=funmount("M0:");
					stat=funinit("M0:");
			 }
		
		 else if(msg_ttf.cmd==RD){
				 	stat = finit ("M0:");
					if (stat == fsOK) {
						stat = fmount ("M0:");
						if (stat == fsOK) {
							f = fopen ("M0:/test.txt","r");
							if (f != NULL) {

								memset(str, '\0', sizeof(str));
								while (!feof (f)) {
								 c = fgetc(f);
								 if(c!=',')
							  	strcat(str,&c);
								 else{
									 strcat(str, "\0");
									 strcpy(adtos[i], str); 
								   memset(str, '\0', sizeof(str));
								   i++;
								 }
								}
								rewind(f);
								fclose(f);
							}
								
								
						}
					}
					
					stat=funmount("M0:");
					stat=funinit("M0:");
					for (j = 0; j < 50; j++) {
           strcpy(msg_ttf_miso.adtos[j], adtos[j]);
           }
					
					osMessageQueuePut(get_id_MsgQueue_ttf_miso(), &msg_ttf_miso, NULL, osWaitForever);
			 }
	
	  }
		
  }
}
