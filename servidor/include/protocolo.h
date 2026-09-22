#ifndef PROTOCOLO_H
#define PROTOCOLO_H

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

  TOTAL_TIPOS_E
} TipoEntrada;

typedef enum {
  IDENTIFY_O,      /*Destinado para campo "operation":"IDENTIFY"*/
  INVITATION,
  INVITE_O,
  NEW_ROOM_O,
  NEW_USER,
  NEW_STATUS,
  JOIN_ROOM_O,
  JOINED_ROOM,
  LEAVE_ROOM_O,
  LEFT_ROOM,
  DISCONNECTED,
  
  USER_LIST,
  ROOM_USERS_O,
  ROOM_USERS_LIST,
  
  TEXT_O,
  TEXT_FROM,
  PUBLIC_TEXT_O,
  PUBLIC_TEXT_FROM,
  ROOM_TEXT_O,
  ROOM_TEXT_FROM,
  INVALID_O,
    
  RESPONSE,
  
  TOTAL_TIPOS_RESP
} TipoResp;

typedef enum {
  SUCCESS,
  USER_ALREADY_EXISTS,
  NO_SUCH_USER,
  ROOM_ALREADY_EXISTS,
  NO_SUCH_ROOM,
  NOT_INVITED,
  NOT_JOINED,

  INVALID,

  TOTAL_TIPOS_RESUL
} TipoResul;

typedef enum {
  ACTIVE,
  AWAY,
  BUSY,
  TOTAL_ESTADOS
} Estado;

typedef struct {
  const char     *cad_tipo;   /*La cadena que representa al tipo de
                            mensaje*/
  TipoEntrada    tipo_ent;    /*La constante del tipo de mensaje que
                            representa la cadena*/
} MapeoEntrada;

typedef struct {
  const char *cad_tipo;
  TipoResp   tipo_resp;
} MapeoRespuestas;

typedef struct {
  const char *cad_tipo;
  TipoResul  tipo_resul;
} MapeoResultados;

typedef struct {
  const char  *cad_tipo;
  Estado      tipo_estado;
} MapeoEstados;

typedef struct {
  const char *username;  /*Nombre de algún usuario*/
  const char *extra;     /*Información extra que se envía con alguna
                           respuesta*/
  const char *text;      /*Texto de que un usuario envía a otro(s)*/
  const char *roomname;  /*Nombre de una sala*/
  const char *users;     /*Diccionario de usuarios en servidor*/
  const char *usernames; /*Lista de usuarios a invitar*/

  TipoResp   type;       /*Tipo de respuesta que se envía a un
                           cliente*/
  TipoResp   operation;  /*Tipo de operación a la que se responde*/
  TipoResul  result;     /*Resultado de la operación realizada*/
  Estado     status;     /*Estado del usuario*/
} Campos;

/*Arreglo de mapeos Cadena->Constante*/
extern const MapeoEntrada    TABLA_ENTRADAS[TOTAL_TIPOS_E];
extern const MapeoRespuestas TABLA_RESP[TOTAL_TIPOS_RESP];
extern const MapeoResultados TABLA_RESUL[TOTAL_TIPOS_RESUL];
extern const MapeoEstados    TABLA_ESTADOS[TOTAL_ESTADOS];

/*Función que recibe una cadena y determina si pertenece a alguna
  cadena en TABLA_MENSAJE

  Regresa -1 si la entrada es nula o no
  coincide con nigún mapeo en TABLA_MENSAJES

  Regresa la constante de enumeración si la cadena se encuentra en
  algún mapeo de TABLA_MENSAJES
*/
TipoEntrada tipo_mensaje(const char *mensaje);

cJSON *fabrica_respuesta(TipoResp tipo, const Campos *datos);


#endif
