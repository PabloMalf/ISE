#include "ttf.h"
#include <string.h>
#include <stdio.h>
#include "rl_fs.h"

static osThreadId_t id_Th_ttf;
static osMessageQueueId_t id_MsgQueue_ttf;
static entrada entradas[MAX_USU]; // maximo por ahora de 10 entradas
char basura [2];

#define MSGQUEUE_OBJECTS_TTF 4

int init_Th_ttf(void);
static void Th_ttf(void *arg);
static int Init_MsgQueue_ttf(void);
static void write_data(void);

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

//static void write_data(void){
//	char data[] = "Que locura joselu ";
//	fsStatus stat;
//	FILE *f;
//	
//	stat = finit ("M0:");
//	if (stat == fsOK) {
//		stat = fmount ("M0:");
//		if (stat == fsOK) {
//			f = fopen ("M0:/test.txt","a+");
//			if (f != NULL) {
//				//fflush (stdout);
//				fwrite(data, sizeof(char), strlen(data), f);
//				fclose(f);
//			}
//		}
//	}
//	
//	stat=funmount("M0:");
//	stat=funinit("M0:");
//	
//}


static void Th_ttf(void *arguments){	
  fsStatus stat;
	FILE *f;
  MSGQUEUE_OBJ_TTF msg_ttf;
	int i;
	//char dataRD[100]; // por ahora no lo usamos
	
	while(1){
		if (osOK == osMessageQueueGet(get_id_MsgQueue_ttf(), &msg_ttf, NULL, osWaitForever)){
			 if(msg_ttf.state==WR){
		   	//write_data();
				 	stat = finit ("M0:");
					if (stat == fsOK) {
						stat = fmount ("M0:");
						if (stat == fsOK) {
							f = fopen ("M0:/test.txt","a+");
							if (f != NULL) {
								//fflush (stdout);
								fwrite(msg_ttf.name, sizeof(char), strlen(msg_ttf.name), f);
								fclose(f);
							}
						}
					}
					
					stat=funmount("M0:");
					stat=funinit("M0:");
			 }
		
			 else if(msg_ttf.state==RD){
		   	//write_data();
				 	stat = finit ("M0:");
					if (stat == fsOK) {
						stat = fmount ("M0:");
						if (stat == fsOK) {
							f = fopen ("M0:/test.txt","r");
							if (f != NULL) {
								//fflush (stdout);
								//fgets(dataRD, sizeof(dataRD), f);
								//while((caracter = fgetc(f)) != EOF)
								i = 0; // variable para recorrer todos las entradas
								while((!feof(f))|| i<MAX_USU) // sale cuando llegue al final o cuando ya no pueda meter mas usuarios 
	              {
									fread(&entradas[i].idTarjeta, sizeof(char), 6, f);
									fread(&entradas[i].fechaHora, sizeof(char), 19, f);
									fread(&entradas[i].tipoAcceso, sizeof(char), 1, f);
									
									fread(&basura, sizeof(char), 2, f);
									i++;
	              }
								fclose(f);
							}
						}
					}
					
					stat = funmount("M0:");
					stat = funinit("M0:");
			 }
	  }
		
  }
}
