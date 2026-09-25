#include "distribuidor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int distribuidor_peticiones(int sockfd, const cJSON *json_msj, ContextoServidor *cs) {

  if(json_msj == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if(error_ptr != NULL) {
      cJSON *resp_err = fabrica_respuesta(RESPONSE, &(Campos) {
        .operation = INVALID_O,
        .result = INVALID,
      });
    enviar_resp(sockfd, resp_err, 1);
    }
    return 1; 
  }

  const cJSON *type = cJSON_GetObjectItemCaseSensitive(json_msj, "type");
  if(!(cJSON_IsString(type)) || (type == NULL)) {
    cJSON *resp_err = fabrica_respuesta(RESPONSE, &(Campos) {
        .operation = INVALID_O,
        .result = INVALID,
      });
    enviar_resp(sockfd, resp_err, 1);
    return 1;
  }

  TipoEntrada tipo = tipo_mensaje(type->valuestring);

  switch(tipo) {
  case IDENTIFY:
    return manejar_identify(sockfd, json_msj, cs);

  case STATUS:
    return manejar_status(sockfd, json_msj, cs);
    
  case USERS:
    return manejar_users(sockfd, json_msj, cs);
    
  case ROOM_USERS:
    return manejar_room_users(sockfd, json_msj, cs);
    
  case TEXT:
    return manejar_text(sockfd, json_msj, cs);
    
  case PUBLIC_TEXT:
    return manejar_public_text(sockfd, json_msj, cs);
    
  case ROOM_TEXT:
    return manejar_room_text(sockfd, json_msj, cs);
    
  case NEW_ROOM:
    return manejar_new_room(sockfd, json_msj, cs);
    
  case INVITE:
    return manejar_invite(sockfd, json_msj, cs);
    
  case JOIN_ROOM:
    return manejar_join_room(sockfd, json_msj, cs);
    
  case LEAVE_ROOM:
    return manejar_leave_room(sockfd, json_msj, cs);

  case DISCONNECT:
    return manejar_disconnect(sockfd, cs);

  default:
    printf("distribuidor_peticiones(): Entra caso default\n");
    return 1;
  }
  return 0;
}
