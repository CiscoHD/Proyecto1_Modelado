#ifndef PARSER_H
#define PARSER_H
#include <stdio.h>
#include <string.h>
#include <cjson/cJSON.h>

typedef enum {
  IDENTIFY,
  STATUS,

  USERS,
  ROOM_USERS,
  
  TEXT,
  PUBLIC_TEXT,
  ROOM_TEXT,

  NEW_ROOM,
  INVITE,
  JOIN_ROOM,
  LEAVE_ROOM,

  DISCONNECT,

  TOTAL_TIPOS
} TipoMsj;

typedef struct {
  const char *cad_tipo;   /*La cadena que representa al tipo de
                            mensaje*/
  TipoMsj tipo_msj;       /*La constante del tipo de mensaje que
                            representa la cadena*/
} Mapeo;

/*Arreglo de mapeos Cadena->Constante*/
extern const Mapeo TABLA_MENSAJES[TOTAL_TIPOS];  

/*Función que recibe una cadena y determina si pertenece a alguna
  cadena en TABLA_MENSAJE

  Regresa -1 si la entrada es nula o no
  coincide con nigún mapeo en TABLA_MENSAJES

  Regresa la constante de enumeración si la cadena se encuentra en
  algún mapeo de TABLA_MENSAJES
*/
TipoMsj tipo_mensaje(const char *mensaje);

#endif
