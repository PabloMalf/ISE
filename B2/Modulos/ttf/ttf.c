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
  //fsStatus stat;
	FILE *f;
  MSGQUEUE_OBJ_TTF_MOSI msg_ttf;
	MSGQUEUE_OBJ_TTF_MISO msg_ttf_miso; 
	char c;
	char datos[MAX_DATA][24];
	char str[20];
	int i = 0;
	int j = 0;
	int k = 0;
	int z=0;
	Init_MsgQueue_ttf_miso();
  Init_MsgQueue_ttf_mosi();
	int rango;


	
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

					
					fflush (f);
					fclose(f);
					funmount("M0:");
					funinit("M0:");
			 }
		
		 else if(msg_ttf.cmd==RD){
			    //memset(datos, '\0', sizeof(datos));
			    i = 0;
					if(fsOK != finit("M0:")) return;
				  if(fsOK != fmount("M0:")) return;
			 
			 		if(msg_ttf.fichero==REG){
						 f = fopen ("M0:/data.csv","r");
						 rango = CAMPOS_REG;
					}
					else if (msg_ttf.fichero==USER){
						 f = fopen ("M0:/user.csv","r");
					   rango = CAMPOS_USU;
					}
					if (f == NULL) return;
										
					while (!feof (f)) {
				  c = fgetc(f);
					if(c!='\n'){
						if(c!=',')
							strcat(str,&c);
								 else{
									 strcat(str, "\0");
									 strcpy(datos[i], str); 
								   memset(str, '\0', sizeof(str));
								   i++;
								 }
								}
							}
			
					j=0;// "puntero" de el array datos[]
					k=0;// representa el numero del registr/usu
							
				  	while(j < i){ 
							
						   for(z=0;z < rango ;z++){
							   strcpy(msg_ttf_miso.datos[k][z].valor, datos[j]);
								  j++;
							 }
							k++;
					  }
					
								rewind(f);
						  	fflush(f);
								fclose(f);
					      funmount("M0:");
					      funinit("M0:");
							
					osMessageQueuePut(get_id_MsgQueue_ttf_miso(), &msg_ttf_miso, NULL, osWaitForever);
			}
	
	  }		
  }
}







