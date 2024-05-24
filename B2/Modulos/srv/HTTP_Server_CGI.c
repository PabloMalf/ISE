#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#include "rl_net.h"   
#include "srv.h"





#if      defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#pragma  clang diagnostic push
#pragma  clang diagnostic ignored "-Wformat-nonliteral"
#endif

extern char identificacion[MAX_USU][20];
extern char fechaHora[MAX_USU][20];
extern char tipoAcceso[MAX_USU][20];
extern char nombre[MAX_USU][20];
extern char mensajeInfo[50];


 
 

uint32_t netCGI_Script (const char *env, char *buf, uint32_t buflen, uint32_t *pcgi) {
  
 
  uint32_t len = 0U;
  
  switch (env[0]) {
    case 'a':
      switch (env[2]) {
        case '1':
          len = (uint32_t)sprintf (buf,&env[4], fechaHora[0]);
          break;
        case '2':
          len = (uint32_t)sprintf (buf, &env[4], nombre[0]);
          break;
        case '3':        
          len = (uint32_t)sprintf (buf, &env[4], identificacion[0]);
          break;
        case '4':
          len = (uint32_t)sprintf (buf, &env[4], tipoAcceso[0]);
          break;
      }
      break;
      
     case 'b':
      switch (env[2]) {
       case '1':
          len = (uint32_t)sprintf (buf,&env[4], fechaHora[1]);
          break;
        case '2':
          len = (uint32_t)sprintf (buf, &env[4], nombre[1]);
          break;
        case '3':        
          len = (uint32_t)sprintf (buf, &env[4], identificacion[1]);
          break;
        case '4':
          len = (uint32_t)sprintf (buf, &env[4], tipoAcceso[1]);
          break;
      }
      break;
    
	  case 'c':
      switch (env[2]) {
       case '1':
          len = (uint32_t)sprintf (buf,&env[4], fechaHora[2]);
          break;
        case '2':
          len = (uint32_t)sprintf (buf, &env[4], nombre[2]);
          break;
        case '3':        
          len = (uint32_t)sprintf (buf, &env[4], identificacion[2]);
          break;
        case '4':
          len = (uint32_t)sprintf (buf, &env[4], tipoAcceso[2]);
          break;
      }
      break;
      
    case 'd':
      switch (env[2]) {
       case '1':
          len = (uint32_t)sprintf (buf,&env[4], fechaHora[3]);
          break;
        case '2':
          len = (uint32_t)sprintf (buf, &env[4], nombre[3]);
          break;
        case '3':        
          len = (uint32_t)sprintf (buf, &env[4], identificacion[3]);
          break;
        case '4':
          len = (uint32_t)sprintf (buf, &env[4], tipoAcceso[3]);
          break;
      }
      break;
     
      case 'e':
      switch (env[2]) {
       case '1':
          len = (uint32_t)sprintf (buf,&env[4], fechaHora[4]);
          break;
        case '2':
          len = (uint32_t)sprintf (buf, &env[4], nombre[4]);
          break;
        case '3':        
          len = (uint32_t)sprintf (buf, &env[4], identificacion[4]);
          break;
        case '4':
          len = (uint32_t)sprintf (buf, &env[4], tipoAcceso[4]);
          break;
      }
      break;
      
      case 'f':
      switch (env[2]) {
          case '1':
          len = (uint32_t)sprintf (buf,&env[4], fechaHora[5]);
          break;
        case '2':
          len = (uint32_t)sprintf (buf, &env[4], nombre[5]);
          break;
        case '3':        
          len = (uint32_t)sprintf (buf, &env[4], identificacion[5]);
          break;
        case '4':
          len = (uint32_t)sprintf (buf, &env[4], tipoAcceso[5]);
          break;
      }
      break;
      
      case 'g':
      switch (env[2]) {
         case '1':
          len = (uint32_t)sprintf (buf,&env[4], fechaHora[6]);
          break;
        case '2':
          len = (uint32_t)sprintf (buf, &env[4], nombre[6]);
          break;
        case '3':        
          len = (uint32_t)sprintf (buf, &env[4], identificacion[6]);
          break;
        case '4':
          len = (uint32_t)sprintf (buf, &env[4], tipoAcceso[6]);
          break;
      }
      break;
      
      case 'h':
      switch (env[2]) {
         case '1':
          len = (uint32_t)sprintf (buf,&env[4], fechaHora[7]);
          break;
        case '2':
          len = (uint32_t)sprintf (buf, &env[4], nombre[7]);
          break;
        case '3':        
          len = (uint32_t)sprintf (buf, &env[4], identificacion[7]);
          break;
        case '4':
          len = (uint32_t)sprintf (buf, &env[4], tipoAcceso[7]);
          break;
      }
      break;
      
      case 'i':
      switch (env[2]) {
          case '1':
          len = (uint32_t)sprintf (buf,&env[4], fechaHora[8]);
          break;
        case '2':
          len = (uint32_t)sprintf (buf, &env[4], nombre[8]);
          break;
        case '3':        
          len = (uint32_t)sprintf (buf, &env[4], identificacion[8]);
          break;
        case '4':
          len = (uint32_t)sprintf (buf, &env[4], tipoAcceso[8]);
          break;
      }
      break;
      
      case 'j':
      switch (env[2]) {
         case '1':
          len = (uint32_t)sprintf (buf,&env[4], fechaHora[9]);
          break;
        case '2':
          len = (uint32_t)sprintf (buf, &env[4], nombre[9]);
          break;
        case '3':        
          len = (uint32_t)sprintf (buf, &env[4], identificacion[9]);
          break;
        case '4':
          len = (uint32_t)sprintf (buf, &env[4], tipoAcceso[9]);
          break;
      }
      break;
      
      case 'k':
      switch (env[2]) {
        case '1':
          len = (uint32_t)sprintf (buf,&env[4], fechaHora[10]);
          break;
        case '2':
          len = (uint32_t)sprintf (buf, &env[4], nombre[10]);
          break;
        case '3':        
          len = (uint32_t)sprintf (buf, &env[4], identificacion[10]);
          break;
        case '4':
          len = (uint32_t)sprintf (buf, &env[4], tipoAcceso[10]);
          break;
      }
      break;
      
       case 'l':
      switch (env[2]) {
         case '1':
          len = (uint32_t)sprintf (buf,&env[4], fechaHora[11]);
          break;
        case '2':
          len = (uint32_t)sprintf (buf, &env[4], nombre[11]);
          break;
        case '3':        
          len = (uint32_t)sprintf (buf, &env[4], identificacion[11]);
          break;
        case '4':
          len = (uint32_t)sprintf (buf, &env[4], tipoAcceso[11]);
          break;
      }
      break;
      
       case 'm':
      switch (env[2]) {
         case '1':
          len = (uint32_t)sprintf (buf,&env[4], fechaHora[12]);
          break;
        case '2':
          len = (uint32_t)sprintf (buf, &env[4], nombre[12]);
          break;
        case '3':        
          len = (uint32_t)sprintf (buf, &env[4], identificacion[12]);
          break;
        case '4':
          len = (uint32_t)sprintf (buf, &env[4], tipoAcceso[12]);
          break;
      }
      break;
      
      
       case 'n':
      switch (env[2]) {
         case '1':
          len = (uint32_t)sprintf (buf,&env[4], fechaHora[13]);
          break;
        case '2':
          len = (uint32_t)sprintf (buf, &env[4], nombre[13]);
          break;
        case '3':        
          len = (uint32_t)sprintf (buf, &env[4], identificacion[13]);
          break;
        case '4':
          len = (uint32_t)sprintf (buf, &env[4], tipoAcceso[13]);
          break;
      }
      break;
      
       case 'o':
      switch (env[2]) {
         case '1':
          len = (uint32_t)sprintf (buf,&env[4], fechaHora[14]);
          break;
        case '2':
          len = (uint32_t)sprintf (buf, &env[4], nombre[14]);
          break;
        case '3':        
          len = (uint32_t)sprintf (buf, &env[4], identificacion[14]);
          break;
        case '4':
          len = (uint32_t)sprintf (buf, &env[4], tipoAcceso[14]);
          break;
      }
      break;
    
       case 'z': 
       switch (env[2]) {
         case '1':
          len = (uint32_t)sprintf (buf, &env[4], mensajeInfo);
      break;
     }
      
  }
  return (len);
}

#if      defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#pragma  clang diagnostic pop
#endif
