#include "protocolo.h"

const MapeoEntrada TABLA_ENTRADAS[TOTAL_TIPOS_E] = {
  {"IDENTIFY", IDENTIFY},
  {"STATUS",STATUS },
  {"USERS", USERS},
  {"ROOM_USERS", ROOM_USERS},
  {"TEXT", TEXT},
  {"PUBLIC_TEXT", PUBLIC_TEXT},
  {"ROOM_TEXT", ROOM_TEXT},
  {"NEW_ROOM", NEW_ROOM},
  {"INVITE", INVITE},
  {"JOIN_ROOM", JOIN_ROOM},
  {"LEAVE_ROOM", LEAVE_ROOM},
  {"DISCONNECT", DISCONNECT},
};

const MapeoRespuestas TABLA_RESP[TOTAL_TIPOS_RESP] = {
  {"", TIPO_VACIO},
  {"IDENTIFY", IDENTIFY_O},
  {"INVITATION", INVITATION},
  {"INVITE", INVITE_O},
  {"NEW_ROOM", NEW_ROOM_O},
  {"NEW_USER", NEW_USER},
  {"NEW_STATUS", NEW_STATUS},
  {"JOIN_ROOM", JOIN_ROOM_O},
  {"JOINED_ROOM", JOINED_ROOM},
  {"LEAVE_ROOM", LEAVE_ROOM_O},
  {"LEFT_ROOM", LEFT_ROOM},
  {"DISCONNECTED", DISCONNECTED},
  {"USER_LIST", USER_LIST},
  {"ROOM_USERS", ROOM_USERS_O},
  {"ROOM_USER_LIST", ROOM_USER_LIST},
  {"TEXT", TEXT_O},
  {"TEXT_FROM", TEXT_FROM},
  {"PUBLIC_TEXT", PUBLIC_TEXT_O},
  {"PUBLIC_TEXT_FROM", PUBLIC_TEXT_FROM},
  {"ROOM_TEXT", ROOM_TEXT_O},
  {"ROOM_TEXT_FROM", ROOM_TEXT_FROM},
  {"INVALID", INVALID_O},
  {"RESPONSE", RESPONSE}
};

const MapeoResultados TABLA_RESUL[TOTAL_TIPOS_RESUL] = {
  {"", RESULT_VACIO},
  {"SUCCESS", SUCCESS},
  {"USER_ALREADY_EXISTS", USER_ALREADY_EXISTS},
  {"NO_SUCH_USER", NO_SUCH_USER},
  {"ROOM_ALREADY_EXISTS", ROOM_ALREADY_EXISTS},
  {"NO_SUCH_ROOM", NO_SUCH_ROOM},
  {"NOT_INVITED", NOT_INVITED},
  {"NOT_JOINED", NOT_JOINED},
  {"NOT_IDENTIFIED", NOT_IDENTIFIED},
  {"INVALID", INVALID}
};

const MapeoEstados TABLA_ESTADOS[TOTAL_ESTADOS] = {
  {"ESTADO_VACIO", ESTADO_VACIO},
  {"ACTIVE", ACTIVE},
  {"AWAY", AWAY},
  {"BUSY", BUSY},
};

TipoEntrada tipo_mensaje(const char *mensaje) {
  if(mensaje == NULL)
    return -1;

  for(int i=0; i < TOTAL_TIPOS_E; i++){
    if(strcmp(mensaje, TABLA_ENTRADAS[i].cad_tipo) == 0)
      return TABLA_ENTRADAS[i].tipo_ent;
  }
  return -1;
}

cJSON *fabrica_respuesta(TipoResp tipo, const Campos *c) {
  if(tipo < 0 || (c == NULL))
    return NULL;
  
  cJSON *json = cJSON_CreateObject();
  // Campos c = (datos != NULL) ? datos : (Campos){0};

  cJSON_AddStringToObject(json, "type", TABLA_RESP[tipo].cad_tipo);
  if(c->operation != TIPO_VACIO)
    cJSON_AddStringToObject(json, "operation", TABLA_RESP[c->operation].cad_tipo);
  if(c->result != RESULT_VACIO)
    cJSON_AddStringToObject(json, "result", TABLA_RESUL[c->result].cad_tipo);
  if(c->status != ESTADO_VACIO)
    cJSON_AddStringToObject(json, "status", TABLA_RESUL[c->status].cad_tipo);
  if(c->username != NULL)
    cJSON_AddStringToObject(json, "username", c->username);
  if(c->text != NULL)
    cJSON_AddStringToObject(json, "text", c->text);
  if(c->roomname != NULL)
    cJSON_AddStringToObject(json, "roomname", c->roomname);
  if(c->users != NULL)
    cJSON_AddItemToObject(json, "users", c->users);
  if(c->extra != NULL) 
    cJSON_AddStringToObject(json, "extra", c->extra);

  return json;
}


