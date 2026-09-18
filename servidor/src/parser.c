#include "parser.h"

const Mapeo TABLA_MENSAJES[TOTAL_TIPOS] = {
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
  {"DISCONNECT", DISCONNECT}
};

TipoMsj tipo_mensaje(const char *mensaje) {
  if(mensaje == NULL)
    return -1;

  for(int i=0; i < TOTAL_TIPOS; i++){
    if(strcmp(mensaje, TABLA_MENSAJES[i].cad_tipo) == 0)
      return TABLA_MENSAJES[i].tipo_msj;
  }
  return -1;
}
