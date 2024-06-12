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
static void ttf_WR_RD(MSGQUEUE_OBJ_TTF_MOSI * mosi);

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

static void get_next_line(FILE *f, char exit [50]){
	char c = 0;
	uint8_t i = 0;
	memset(exit, 0x00, 50);
	for(c = fgetc(f); c !='\n' && !feof(f); i++){
		exit[i] = c;
		c = fgetc(f);
	}
}


static void ttf_WR_RD(MSGQUEUE_OBJ_TTF_MOSI * mosi){
	FILE *f;
	char *token;
	char line [50];
	uint8_t i, j = 0;
	MSGQUEUE_OBJ_TTF_MISO msg_ttf_miso = {.eof=0};
	
	if(fsOK != finit("M0:")) return;
	//if(fsOK != fmount("M0:"))
		if(fsOK != fmount("M0:")) return;
	
	
	if(mosi->cmd==DEL){
		f = fopen ("M0:/data.csv","w"); // LLL HACER 2 PUTS, EL PRIMERO LO IGNORA
	}
	
	else if(mosi->cmd==WR){
		if(mosi->fichero==REG)
			 f = fopen ("M0:/data.csv","a+");
		else if (mosi->fichero==USER)
			 f = fopen ("M0:/user.csv","a+");

		if (f == NULL) return;
		fwrite(mosi->data, sizeof(char), strlen(mosi->data), f);
		fwrite(",\n", sizeof(char), 2, f);
	}

	else if(mosi->cmd==RD){
		memset(msg_ttf_miso.datos, '\0', sizeof(msg_ttf_miso.datos));
		switch(mosi->fichero){
			case USER:
				f = fopen ("M0:/user.csv","r");
				if (f == NULL) return;
			
				do{
					get_next_line(f, line);
				}while(!feof(f) && strncmp(line, mosi->data, 14));
		
				if(feof(f)){//msg no encontrado
					sprintf(msg_ttf_miso.datos[0][0].valor,"ID FAIL");
				}
				else{//msg encontrado
					token = strtok(line, ",");
					for(i=0; token != NULL; i++){
						strcpy(msg_ttf_miso.datos[0][i].valor,token);
						token = strtok(NULL, ",");
					}
					osMessageQueuePut(get_id_MsgQueue_ttf_miso(), &msg_ttf_miso, 0, 0);
				}
			break;
			
				
			case REG:
				f = fopen ("M0:/data.csv","r");
				if (f == NULL) return;
				do{
					get_next_line(f, line);
					token = strtok(line, ",");
					for(i=0; token != NULL; i++){
						strcpy(msg_ttf_miso.datos[j][i].valor,token);
						token = strtok(NULL, ",");
					}
					j++;
					if (j==20){ // enviar en bloques de 20
						j=0;
						while(0 == osMessageQueueGetSpace(id_MsgQueue_ttf_miso)) osThreadYield();
						//osMessageQueuePut(get_id_MsgQueue_ttf_miso(), &msg_ttf_miso, 0, 0);
						memset(msg_ttf_miso.datos, '\0', sizeof(msg_ttf_miso.datos));
					}
				}while(!feof(f));
				
				msg_ttf_miso.eof = 1;
				while(0 == osMessageQueueGetSpace(id_MsgQueue_ttf_miso)) osThreadYield();
				osMessageQueuePut(get_id_MsgQueue_ttf_miso(), &msg_ttf_miso, 0, 0);
				memset(msg_ttf_miso.datos, '\0', sizeof(msg_ttf_miso.datos));
			break;
		}
	}
								 
	rewind(f);
	fflush(f);
	fclose(f);
	funmount("M0:");
	funinit("M0:");
}


static void Th_ttf(void *arguments){	
	MSGQUEUE_OBJ_TTF_MOSI msg_ttf;
	
	Init_MsgQueue_ttf_miso();
  Init_MsgQueue_ttf_mosi();

	while(1){
		if (osOK == osMessageQueueGet(get_id_MsgQueue_ttf_mosi(), &msg_ttf, NULL, osWaitForever)){
			ttf_WR_RD(&msg_ttf);
		}		
	}
}

