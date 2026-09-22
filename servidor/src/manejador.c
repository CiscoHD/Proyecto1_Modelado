#include "manejador.h"

int manejar_identify(int sockfd, const cJSON *json_msj) {
  const cJSON *username = cJSON_GetObjectItemCaseSensitive(json_msj, "username");
  
  if(!(cJSON_IsString(username)) || (username == NULL)) {
    printf("ERROR: campo 'username' ausenete o inválido\n");
    cJSON *resp_err = fabrica_respuesta(RESPONSE, &(Campos) {
        .operation = INVALID_O,
        .result = INVALID,
      });
    enviar_resp(sockfd, resp_err, 1);
    return 1;
  }

  char nom_usuario[9];
  strncpy(nom_usuario, username->valuestring, 8);
  nom_usuario[9] = '\0';
  //const char *usuario = username->valuestring;
  cJSON *resp = fabrica_respuesta(RESPONSE, &(Campos) {
      .operation = IDENTIFY_O,
      .result = SUCCESS,
      .extra = nom_usuario
    });
  enviar_resp(sockfd, resp, 1);
  return 0;
}

int manejar_status(int sockfd, const cJSON *json_msj) {
  return 0;
}

int manejar_users(int sockfd, const cJSON *json_msj) {
  printf("Lista de usuarios : \n");
  return 0;
}

int manejar_room_users(int sockfd, const cJSON *json_msj) {
  return 0;
}

int manejar_text(int sockfd, const cJSON *json_msj) {
  return 0;
}

int manejar_public_text(int sockfd, const cJSON *json_msj) {
  return 0;
}

int manejar_room_text(int sockfd, const cJSON *json_msj) {
  return 0;
}

int manejar_new_room(int sockfd, const cJSON *json_msj) {
  return 0;
}

int manejar_invite(int sockfd, const cJSON *json_msj) {
  return 0;
}

int manejar_join_room(int sockfd, const cJSON *json_msj) {
  return 0;
}

int manejar_leave_room(int sockfd, const cJSON *json_msj) {
  return 0;
}

int manejar_disconnect(int sockfd, const cJSON *json_msj) {
  return 0;
}
