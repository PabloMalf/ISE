#include "ttf.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "rl_fs.h"

#define MSGQUEUE_OBJECTS_TTF 1

static osThreadId_t id_Th_ttf;
static osMessageQueueId_t id_MsgQueue_ttf_miso;
static osMessageQueueId_t id_MsgQueue_ttf_mosi;

int init_Th_ttf(void);
static void Th_ttf(void *arg);
static int Init_MsgQueue_ttf_mosi(void);
static int Init_MsgQueue_ttf_miso(void);

void get_next_line(FILE *f, char exit [50]);

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

void get_next_line(FILE *f, char exit [50]){
	char c = 0;
	uint8_t i = 0;
	memset(exit, 0x00, 50);
	for(c = fgetc(f); c !='\n' && !feof(f); i++){
		exit[i] = c;
		c = fgetc(f);
	}
}

static void Th_ttf(void *arguments){	
	FILE *f;
  MSGQUEUE_OBJ_TTF_MOSI msg_ttf;
	MSGQUEUE_OBJ_TTF_MISO msg_ttf_miso; 
	char *token;
	char line [50];
	int i,j = 0;
	Init_MsgQueue_ttf_miso();
  Init_MsgQueue_ttf_mosi();

	while(1){
		if (osOK == osMessageQueueGet(get_id_MsgQueue_ttf_mosi(), &msg_ttf, NULL, osWaitForever)){
			
			 if(msg_ttf.cmd==WR){
					if(fsOK != finit("M0:")) return;
				  if(fsOK != fmount("M0:")) return;
				 
					if(msg_ttf.fichero==REG)
						 f = fopen ("M0:/data.csv","a+");
					else if (msg_ttf.fichero==USER)
						 f = fopen ("M0:/user.csv","a+");
	
					if (f == NULL) return;
					fwrite(msg_ttf.data, sizeof(char), strlen(msg_ttf.data), f);
					fwrite(",\n", sizeof(char), 2, f);

					
					fflush (f);
					fclose(f);
					funmount("M0:");
					funinit("M0:");
			 }
		
		 else if(msg_ttf.cmd==RD){
			    i = 0;
					if(fsOK != finit("M0:")) return;
				  if(fsOK != fmount("M0:")) return;
			 
					switch(msg_ttf.fichero){
						case USER:
							f = fopen ("M0:/user.csv","r");
							if (f == NULL) return;
						
							do{
								get_next_line(f, line);
							}while(!feof(f) && strncmp(line, msg_ttf.data, 14));
					
							if(feof(f)){
								//msg no encontrado
								sprintf(msg_ttf_miso.datos[0][0].valor,"ID FAIL");
							}
							else{
								j=0;
								//msg encontrado
								token = strtok(line, ",");
								for(i=0; token != NULL; i++){
									strcpy(msg_ttf_miso.datos[0][i].valor,token);
									token = strtok(NULL, ",");
								}
								
							}
						break;
						
						case REG:
							f = fopen ("M0:/data.csv","r");
							if (f == NULL) return;

						  j=0;
							do{
								get_next_line(f, line);
								token = strtok(line, ",");
								for(i=0; token != NULL; i++){
									strcpy(msg_ttf_miso.datos[j][i].valor,token);
									token = strtok(NULL, ",");
								}
								j++;
							}while(!feof(f));
						break;
					}
					
					rewind(f);
					fflush(f);
					fclose(f);
					funmount("M0:");
					funinit("M0:");
							
					osMessageQueuePut(get_id_MsgQueue_ttf_miso(), &msg_ttf_miso, 0, 0);
	 }
  }		
 }
}


