#ifndef MANEJADOR_h
#define MANEJADOR_h

#include <stdio.h>
#include <string.h>
#include "protocolo.h"
#include "socket_util.h"

int manejar_identify   (int sockfd, const cJSON *json_msj);
int manejar_status     (int sockfd, const cJSON *json_msj);
int manejar_users      (int sockfd, const cJSON *json_msj);
int manejar_room_users (int sockfd, const cJSON *json_msj);
int manejar_text       (int sockfd, const cJSON *json_msj);
int manejar_public_text(int sockfd, const cJSON *json_msj);
int manejar_room_text  (int sockfd, const cJSON *json_msj);
int manejar_new_room   (int sockfd, const cJSON *json_msj);
int manejar_invite     (int sockfd, const cJSON *json_msj);
int manejar_join_room  (int sockfd, const cJSON *json_msj);
int manejar_leave_room (int sockfd, const cJSON *json_msj);
int manejar_disconnect (int sockfd, const cJSON *json_msj);

#endif
