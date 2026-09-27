#include "manejador.h"

int manejar_identify(int sockfd, const cJSON *json_msj, ContextoServidor *cs) {
  const cJSON *username = cJSON_GetObjectItemCaseSensitive(json_msj, "username");
  
  if(!(cJSON_IsString(username)) || (username == NULL)) {
    printf("manejar_identify(): campo 'username' ausente o inválido\n");
    cJSON *resp_err = fabrica_respuesta(RESPONSE, &(Campos) {
        .operation = INVALID_O,
        .result = INVALID,
      });
    enviar_resp(sockfd, resp_err, 1);
    return 1;
  }

  int resultado_op;
  char nom_usuario[9];
  strncpy(nom_usuario, username->valuestring, 8);
  nom_usuario[8] = '\0';

  g_mutex_lock(&cs->mtx_serv);

  resultado_op = agregar_cliente_gral(nom_usuario, sockfd, cs->sala_gral);

  if(resultado_op == 0) {
    GHashTableIter it;
    gpointer i_sockfd, cliente;
    g_hash_table_iter_init(&it, cs->sala_gral->usrs);

    cJSON *notif = fabrica_respuesta(NEW_USER, &(Campos) {
        .username = nom_usuario,
      });
    
    while(g_hash_table_iter_next(&it, &i_sockfd, &cliente)) {
      int sockfd_dest = GPOINTER_TO_INT(i_sockfd);
      if(sockfd_dest == sockfd) {
        continue;
      }

      enviar_resp(sockfd_dest, notif, 0);
    }
    cJSON_Delete(notif);
  }
    
  g_mutex_unlock(&cs->mtx_serv);

  // Error: Usuario ya existe
  if (resultado_op == -1) {        
    cJSON *resp_err = fabrica_respuesta(RESPONSE, &(Campos) {
        .operation = IDENTIFY_O,
        .result = USER_ALREADY_EXISTS
      });
    enviar_resp(sockfd, resp_err, 1);
    return 0;
  }
  
  //El usuario se agregó correctamente
  cJSON *resp_succes = fabrica_respuesta(RESPONSE, &(Campos) {
      .operation = IDENTIFY_O,
      .result = SUCCESS,
      .extra = nom_usuario
    });
  enviar_resp(sockfd, resp_succes, 1);
  
  return 0;
}

int manejar_status(int sockfd, const cJSON *json_msj, ContextoServidor *cs) {

  g_mutex_lock(&cs->mtx_serv);

  Cliente *cliente = g_hash_table_lookup(cs->sala_gral->usrs, GINT_TO_POINTER(sockfd));
  if(cliente == NULL) {
    g_mutex_unlock(&cs->mtx_serv);
    printf("manejar_status(): Cliente '%d' no identificado \n", sockfd);
    cJSON *resp_err = fabrica_respuesta(RESPONSE, &(Campos) {
        .operation = INVALID_O,
        .result = NOT_IDENTIFIED
      });
    enviar_resp(sockfd, resp_err, 1);
    return 1;
  }
  
  g_mutex_unlock(&cs->mtx_serv);
  
  return 0;
}

int manejar_users(int sockfd, const cJSON *json_msj, ContextoServidor *cs) {

  cJSON *json_usrs = cJSON_CreateObject();

  g_mutex_lock(&cs->mtx_serv);
  
  Cliente *cliente = g_hash_table_lookup(cs->sala_gral->usrs, GINT_TO_POINTER(sockfd));
  if(cliente == NULL) {
    g_mutex_unlock(&cs->mtx_serv);
    printf("manejar_users(): Cliente '%d' no identificado \n", sockfd);
    cJSON *resp_err = fabrica_respuesta(RESPONSE, &(Campos) {
        .operation = INVALID_O,
        .result = NOT_IDENTIFIED
      });
    enviar_resp(sockfd, resp_err, 1);
    return 1;
  }
  
  GHashTableIter it;
  gpointer i_sockfd, i_cliente;

  g_hash_table_iter_init(&it, cs->sala_gral->usrs);
  while(g_hash_table_iter_next(&it, &i_sockfd, &i_cliente)) {
    Cliente *c = (Cliente *) i_cliente;
    
    cJSON_AddStringToObject(json_usrs, c->username, TABLA_ESTADOS[c->status].cad_tipo);
  }
  
  g_mutex_unlock(&cs->mtx_serv);

  cJSON *resp = fabrica_respuesta(USER_LIST, &(Campos){
      .users = json_usrs
    });
  
  enviar_resp(sockfd, resp, 1);
  
  return 0;
}

int manejar_room_users(int sockfd, const cJSON *json_msj, ContextoServidor *cs) {

  const cJSON *roomname = cJSON_GetObjectItemCaseSensitive(json_msj, "roomname");
  if(!(cJSON_IsString(roomname)) || (roomname == NULL)) {
    printf("manejar_room_users(): campo 'roomname' ausente o inválido\n");
    cJSON *resp_err = fabrica_respuesta(RESPONSE, &(Campos) {
        .operation = INVALID_O,
        .result = INVALID,
      });
    enviar_resp(sockfd, resp_err, 1);
    return 1;
  }

  char nom_sala[17];
  strncpy(nom_sala, roomname->valuestring, 16);
  nom_sala[16] = '\0';
  
  cJSON *json_usrs_sala = cJSON_CreateObject();
  
  g_mutex_lock(&cs->mtx_serv);

  Cliente *cliente = g_hash_table_lookup(cs->sala_gral->usrs, GINT_TO_POINTER(sockfd));
  if(cliente == NULL) {
    g_mutex_unlock(&cs->mtx_serv);
    printf("manejar_room_users(): Cliente '%d' no identificado \n", sockfd);
    cJSON *resp_err = fabrica_respuesta(RESPONSE, &(Campos) {
        .operation = INVALID_O,
        .result = NOT_IDENTIFIED
      });
    enviar_resp(sockfd, resp_err, 1);
    return 1;
  }

  Sala *sala = g_hash_table_lookup(cs->contex_salas->salas, nom_sala);
  if (sala == NULL) {
    g_mutex_unlock(&cs->mtx_serv);
    printf("manejar_room_users(): No existe la sala '%s'\n", nom_sala);
    cJSON *resp_err = fabrica_respuesta(RESPONSE, &(Campos) {
        .operation = ROOM_USERS_O,
        .result = NO_SUCH_ROOM,
        .extra = nom_sala
      });
    enviar_resp(sockfd, resp_err, 1);
    return 0;
  }

  if (!g_hash_table_contains(sala->usrs_sala, GINT_TO_POINTER(sockfd))) {
    g_mutex_unlock(&cs->mtx_serv);
    printf("manejar_room_usrs(): El usuario '%s' no está en la sala '%s'\n", cliente->username, nom_sala);
    cJSON *resp_err = fabrica_respuesta(RESPONSE, &(Campos) {
        .operation = ROOM_USERS_O,
        .result = NOT_JOINED,
        .extra = nom_sala
      });
    enviar_resp(sockfd, resp_err, 1);
    return 0;
  }

  GHashTableIter it_sala;
  gpointer i_sockfd, i_cliente;

  g_hash_table_iter_init(&it_sala, sala->usrs_sala);
  while(g_hash_table_iter_next(&it_sala, &i_sockfd, &i_cliente)) {
    Cliente *c = (Cliente *) i_cliente;
    
    cJSON_AddStringToObject(json_usrs_sala, c->username, TABLA_ESTADOS[c->status].cad_tipo);
  }

  g_mutex_unlock(&cs->mtx_serv);

  cJSON *resp = fabrica_respuesta(ROOM_USER_LIST, &(Campos){
      .roomname = nom_sala,
      .users = json_usrs_sala
    });
  
  enviar_resp(sockfd, resp, 1);
  
  return 0;
}

int manejar_text(int sockfd, const cJSON *json_msj, ContextoServidor *cs) {

  const cJSON *username_dst = cJSON_GetObjectItemCaseSensitive(json_msj, "username");
  if(!(cJSON_IsString(username_dst)) || (username_dst == NULL)) {
    printf("manejar_text(): campo 'username' ausente o inválido\n");
    cJSON *resp_err = fabrica_respuesta(RESPONSE, &(Campos) {
        .operation = INVALID_O,
        .result = INVALID,
      });
    enviar_resp(sockfd, resp_err, 1);
    return 1;
  }
  
  const cJSON *text = cJSON_GetObjectItemCaseSensitive(json_msj, "text");
  if(!(cJSON_IsString(text)) || (text == NULL)) {
    printf("manejar_text(): campo 'text' ausente o inválido\n");
    cJSON *resp_err = fabrica_respuesta(RESPONSE, &(Campos) {
        .operation = INVALID_O,
        .result = INVALID,
      });
    enviar_resp(sockfd, resp_err, 1);
    return 1;
  }

  int existe_usr_dst = 0;
  char nom_usuario_dst[9];
  strncpy(nom_usuario_dst, username_dst->valuestring, 8);
  nom_usuario_dst[8] = '\0';

  g_mutex_lock(&cs->mtx_serv);

  Cliente *c = g_hash_table_lookup(cs->sala_gral->usrs, GINT_TO_POINTER(sockfd));
  if(c == NULL) {
    g_mutex_unlock(&cs->mtx_serv);
    printf("manejar_text(): Cliente '%d' no identificado \n", sockfd);
    cJSON *resp_err = fabrica_respuesta(RESPONSE, &(Campos) {
        .operation = INVALID_O,
        .result = NOT_IDENTIFIED
      });
    enviar_resp(sockfd, resp_err, 1);
    return 1;
  }
  
  cJSON *text_from = fabrica_respuesta(TEXT_FROM, &(Campos) {
          .username = c->username,
          .text = text->valuestring
        });
  
  GHashTableIter it;
  gpointer i_sockfd, i_cliente;
  g_hash_table_iter_init(&it, cs->sala_gral->usrs);

  while(g_hash_table_iter_next(&it, &i_sockfd, &i_cliente)) {
    Cliente *i_c = (Cliente *) i_cliente;
    if(strcmp(i_c->username, nom_usuario_dst) == 0) {
      enviar_resp(GPOINTER_TO_INT(i_sockfd), text_from, 0);
      existe_usr_dst = 1;
      break;
    }
  }
  cJSON_Delete(text_from);
  
  g_mutex_unlock(&cs->mtx_serv);

  if(existe_usr_dst == 0) {
    cJSON *resp_err = fabrica_respuesta(RESPONSE, &(Campos) {
        .operation = TEXT_O,
        .result = NO_SUCH_USER,
        .extra = nom_usuario_dst
      });
    enviar_resp(sockfd,resp_err, 1);
  }
  
  return 0;
}

int manejar_public_text(int sockfd, const cJSON *json_msj, ContextoServidor *cs) {

  const cJSON *text = cJSON_GetObjectItemCaseSensitive(json_msj, "text");
  if(!(cJSON_IsString(text)) || (text == NULL)) {
    printf("manejar_public_text(): campo 'text' ausente o inválido\n");
    cJSON *resp_err = fabrica_respuesta(RESPONSE, &(Campos) {
        .operation = INVALID_O,
        .result = INVALID,
      });
    enviar_resp(sockfd, resp_err, 1);
    return 1;
  }

  g_mutex_lock(&cs->mtx_serv);

  Cliente *c = g_hash_table_lookup(cs->sala_gral->usrs, GINT_TO_POINTER(sockfd));
  if(c == NULL) {
    g_mutex_unlock(&cs->mtx_serv);
    printf("manejar_public_text(): Cliente '%d' no identificado \n", sockfd);
    cJSON *resp_err = fabrica_respuesta(RESPONSE, &(Campos) {
        .operation = INVALID_O,
        .result = NOT_IDENTIFIED
      });
    enviar_resp(sockfd, resp_err, 1);
    return 1;
  }
  
  cJSON *msj_global = fabrica_respuesta(PUBLIC_TEXT_FROM, &(Campos) {
      .username = c->username,
      .text = text->valuestring
    });
  
  GHashTableIter it;
  gpointer i_sockfd;
  g_hash_table_iter_init(&it, cs->sala_gral->usrs);

  while(g_hash_table_iter_next(&it, &i_sockfd, NULL)) {
    int sockfd_dst = GPOINTER_TO_INT(i_sockfd);
    if(sockfd_dst == sockfd) 
      continue;
    enviar_resp(sockfd_dst, msj_global, 0);
  }

  cJSON_Delete(msj_global);
  
  g_mutex_unlock(&cs->mtx_serv);
    
  return 0;
}

int manejar_room_text(int sockfd, const cJSON *json_msj, ContextoServidor *cs) {

  const cJSON *roomname = cJSON_GetObjectItemCaseSensitive(json_msj, "roomname");
  if(!(cJSON_IsString(roomname)) || (roomname == NULL)) {
    printf("manejar_room_text(): campo 'roomname' ausente o inválido\n");
    cJSON *resp_err = fabrica_respuesta(RESPONSE, &(Campos) {
        .operation = INVALID_O,
        .result = INVALID,
      });
    enviar_resp(sockfd, resp_err, 1);
    return 1;
  }
  
  const cJSON *text = cJSON_GetObjectItemCaseSensitive(json_msj, "text");
  if(!(cJSON_IsString(text)) || (text == NULL)) {
    printf("manejar_room_text(): campo 'text' ausente o inválido\n");
    cJSON *resp_err = fabrica_respuesta(RESPONSE, &(Campos) {
        .operation = INVALID_O,
        .result = INVALID,
      });
    enviar_resp(sockfd, resp_err, 1);
    return 1;
  }
  
  char nom_sala[17];
  strncpy(nom_sala, roomname->valuestring, 16);
  nom_sala[16] = '\0';
  
  g_mutex_lock(&cs->mtx_serv);

  Cliente *c = g_hash_table_lookup(cs->sala_gral->usrs, GINT_TO_POINTER(sockfd));
  if(c == NULL) {
    g_mutex_unlock(&cs->mtx_serv);
    printf("manejar_room_text(): Cliente '%d' no identificado \n", sockfd);
    cJSON *resp_err = fabrica_respuesta(RESPONSE, &(Campos) {
        .operation = INVALID_O,
        .result = NOT_IDENTIFIED
      });
    enviar_resp(sockfd, resp_err, 1);
    return 1;
  }

  Sala *sala = g_hash_table_lookup(cs->contex_salas->salas, nom_sala);
  if (sala == NULL) {
    g_mutex_unlock(&cs->mtx_serv);
    printf("manejar_room_text(): No existe la sala '%s'\n", nom_sala);
    cJSON *resp_err = fabrica_respuesta(RESPONSE, &(Campos) {
        .operation = ROOM_TEXT_O,
        .result = NO_SUCH_ROOM,
        .extra = nom_sala
      });
    enviar_resp(sockfd, resp_err, 1);
    return 0;
  }

  if (!g_hash_table_contains(sala->usrs_sala, GINT_TO_POINTER(sockfd))) {
    g_mutex_unlock(&cs->mtx_serv);
    printf("manejar_room_text(): El usuario '%s' no está en la sala '%s'\n", c->username, nom_sala);
    cJSON *resp_err = fabrica_respuesta(RESPONSE, &(Campos) {
        .operation = ROOM_TEXT_O,
        .result = NOT_JOINED,
        .extra = nom_sala
      });
    enviar_resp(sockfd, resp_err, 1);
    return 0;
  }

  cJSON *msj_sala = fabrica_respuesta(ROOM_TEXT_FROM, &(Campos) {
      .roomname = nom_sala,
      .username = c->username,
      .text = text->valuestring
    });
  
  GHashTableIter it_sala;
  gpointer i_sockfd;
  g_hash_table_iter_init(&it_sala, sala->usrs_sala);

  while(g_hash_table_iter_next(&it_sala, &i_sockfd, NULL)) {
    int sockfd_dst_sala = GPOINTER_TO_INT(i_sockfd);
    if(sockfd_dst_sala == sockfd) 
      continue;
    enviar_resp(sockfd_dst_sala, msj_sala, 0);
  }

  cJSON_Delete(msj_sala);
    
  g_mutex_unlock(&cs->mtx_serv);
  
  return 0;
}

int manejar_new_room(int sockfd, const cJSON *json_msj, ContextoServidor *cs) {

  const cJSON *roomname = cJSON_GetObjectItemCaseSensitive(json_msj, "roomname");
  if(!(cJSON_IsString(roomname)) || (roomname == NULL)) {
    printf("manejar_new_room(): campo 'roomname' ausente o inválido\n");
    cJSON *resp_err = fabrica_respuesta(RESPONSE, &(Campos) {
        .operation = INVALID_O,
        .result = INVALID,
      });
    enviar_resp(sockfd, resp_err, 1);
    return 1;
  }

  char nom_sala[17];
  strncpy(nom_sala, roomname->valuestring, 16);
  nom_sala[16] = '\0';
  
  g_mutex_lock(&cs->mtx_serv);

  Cliente *c = g_hash_table_lookup(cs->sala_gral->usrs, GINT_TO_POINTER(sockfd));
  if(c == NULL) {
    g_mutex_unlock(&cs->mtx_serv);
    printf("manejar_new_room(): Cliente '%d' no identificado \n", sockfd);
    cJSON *resp_err = fabrica_respuesta(RESPONSE, &(Campos) {
        .operation = INVALID_O,
        .result = NOT_IDENTIFIED
      });
    enviar_resp(sockfd, resp_err, 1);
    return 1;
  }
  
  if(g_hash_table_contains(cs->contex_salas->salas, nom_sala)) {
    printf("manejar_new_room(): ya existe la sala con nombre %s\n", nom_sala);
    cJSON *resp_err = fabrica_respuesta(RESPONSE, &(Campos) {
        .operation = NEW_ROOM_O,
        .result = ROOM_ALREADY_EXISTS,
        .extra = nom_sala
      });
    enviar_resp(sockfd, resp_err, 1);
    return 0;
  }

  Sala *nueva_sala = crear_sala(nom_sala);
  g_hash_table_insert(cs->contex_salas->salas, strdup(nom_sala), nueva_sala);
  
  agregar_cliente_sala(c->username, sockfd, nueva_sala);
  
  g_mutex_unlock(&cs->mtx_serv);

  cJSON *resp_succ = fabrica_respuesta(RESPONSE, &(Campos) {
      .operation = NEW_ROOM_O,
      .result = SUCCESS,
      .extra = nom_sala
    });
  enviar_resp(sockfd, resp_succ, 1);
  
  return 0;
}

int manejar_invite(int sockfd, const cJSON *json_msj, ContextoServidor *cs) {

  const cJSON *roomname = cJSON_GetObjectItemCaseSensitive(json_msj, "roomname");
  if(!(cJSON_IsString(roomname)) || (roomname == NULL)) {
    printf("manejar_invite(): campo 'roomname' ausente o inválido\n");
    cJSON *resp_err = fabrica_respuesta(RESPONSE, &(Campos) {
        .operation = INVALID_O,
        .result = INVALID,
      });
    enviar_resp(sockfd, resp_err, 1);
    return 1;
  }

  const cJSON *usernames = cJSON_GetObjectItemCaseSensitive(json_msj, "usernames");
  if(!(cJSON_IsArray(usernames)) || (usernames == NULL)) {
    printf("manejar_invite(): campo 'usernames' ausente o inválido\n");
    cJSON *resp_err = fabrica_respuesta(RESPONSE, &(Campos) {
        .operation = INVALID_O,
        .result = INVALID,
      });
    enviar_resp(sockfd, resp_err, 1);
    return 1;
  }

 
  char nom_sala[17];
  strncpy(nom_sala, roomname->valuestring, 16);
  nom_sala[16] = '\0';

  g_mutex_lock(&cs->mtx_serv);

  Cliente *cliente = g_hash_table_lookup(cs->sala_gral->usrs, GINT_TO_POINTER(sockfd));
  if(cliente == NULL) {
    g_mutex_unlock(&cs->mtx_serv);
    printf("manejar_invite(): Cliente '%d' no identificado \n", sockfd);
    cJSON *resp_err = fabrica_respuesta(RESPONSE, &(Campos) {
        .operation = INVALID_O,
        .result = NOT_IDENTIFIED
      });
    enviar_resp(sockfd, resp_err, 1);
    return 1;
  }

  Sala *sala = g_hash_table_lookup(cs->contex_salas->salas, nom_sala);
  if (sala == NULL) {
    g_mutex_unlock(&cs->mtx_serv);
    printf("manejar_invite(): No existe la sala '%s'\n", nom_sala);
    cJSON *resp_err = fabrica_respuesta(RESPONSE, &(Campos) {
        .operation = INVITE_O,
        .result = NO_SUCH_ROOM,
        .extra = nom_sala
      });
    enviar_resp(sockfd, resp_err, 1);
    return 0;
  }

   if (!g_hash_table_contains(sala->usrs_sala, GINT_TO_POINTER(sockfd))) {
    g_mutex_unlock(&cs->mtx_serv);
    printf("manejar_invite(): El usuario '%s' no está en la sala '%s'\n", cliente->username, nom_sala);
    cJSON *resp_err = fabrica_respuesta(RESPONSE, &(Campos) {
        .operation = INVITE_O,
        .result = NOT_JOINED,
        .extra = nom_sala
      });
    enviar_resp(sockfd, resp_err, 1);
    return 0;
  }

  const cJSON *usr_invitado = NULL;
  cJSON_ArrayForEach(usr_invitado, usernames) {
    
    if (!cJSON_IsString(usr_invitado) || (usr_invitado->valuestring == NULL)) {
      g_mutex_unlock(&cs->mtx_serv);
      printf("manejar_invite(): Error en la lista de usuarios del JSON \n");
      cJSON *resp_err = fabrica_respuesta(RESPONSE, &(Campos) {
          .operation = INVALID_O,
          .result = INVALID,
        });
      enviar_resp(sockfd, resp_err, 1);
      return 1;
    }

    const char *nom_usr_invitado = usr_invitado->valuestring;
    int existe_usr_invit = 0;
    int sockfd_usr_invit = -11;

    GHashTableIter it_gral;
    gpointer i_sock, i_cliente;
    g_hash_table_iter_init(&it_gral, cs->sala_gral->usrs);

    while (g_hash_table_iter_next(&it_gral, &i_sock, &i_cliente)) {
      Cliente *c = (Cliente *) i_cliente;
      if (strcmp(c->username, nom_usr_invitado) == 0) {
        existe_usr_invit = 1;
        sockfd_usr_invit = GPOINTER_TO_INT(i_sock);
        break;
      }
    }
    
    if (existe_usr_invit == 0) {
      g_mutex_unlock(&cs->mtx_serv);
      printf("manejar_invite(): No existe el usuario '%s' \n", nom_usr_invitado);
      cJSON *resp_err = fabrica_respuesta(RESPONSE, &(Campos) {
          .operation = INVITE_O,
          .result = NO_SUCH_USER,
          .extra = nom_usr_invitado
        });
      enviar_resp(sockfd, resp_err, 1);
      return 0;
    }

    int existe_en_usrs_sala = g_hash_table_contains(sala->usrs_sala,
                                                    GINT_TO_POINTER(sockfd_usr_invit)
                                                   );
    int existe_en_usrs_invit = g_hash_table_contains(sala->usrs_invit,
                                                     GINT_TO_POINTER(sockfd_usr_invit)
                                                    );
    
    if((existe_en_usrs_sala == 1) || (existe_en_usrs_invit == 1))
      continue;

    invitar_cliente_sala(nom_usr_invitado, GPOINTER_TO_INT(i_sock), sala);

    cJSON *notif_invite = fabrica_respuesta(INVITATION, &(Campos) {
        .username = cliente->username,
        .roomname = nom_sala
      });
    enviar_resp(sockfd_usr_invit, notif_invite, 1);
  }

  g_mutex_unlock(&cs->mtx_serv);
  
  return 0;
}

int manejar_join_room(int sockfd, const cJSON *json_msj, ContextoServidor *cs) {

  const cJSON *roomname = cJSON_GetObjectItemCaseSensitive(json_msj, "roomname");
  if(!(cJSON_IsString(roomname)) || (roomname == NULL)) {
    printf("manejar_join_room(): campo 'roomname' ausente o inválido\n");
    cJSON *resp_err = fabrica_respuesta(RESPONSE, &(Campos) {
        .operation = INVALID_O,
        .result = INVALID,
      });
    enviar_resp(sockfd, resp_err, 1);
    return 1;
  }

  char nom_sala[17];
  strncpy(nom_sala, roomname->valuestring, 16);
  nom_sala[16] = '\0';

  g_mutex_lock(&cs->mtx_serv);

  Cliente *cliente = g_hash_table_lookup(cs->sala_gral->usrs, GINT_TO_POINTER(sockfd));
  if(cliente == NULL) {
    g_mutex_unlock(&cs->mtx_serv);
    printf("manejar_join_room(): Cliente '%d' no identificado \n", sockfd);
    cJSON *resp_err = fabrica_respuesta(RESPONSE, &(Campos) {
        .operation = INVALID_O,
        .result = NOT_IDENTIFIED
      });
    enviar_resp(sockfd, resp_err, 1);
    return 1;
  }

  Sala *sala = g_hash_table_lookup(cs->contex_salas->salas, nom_sala);
  if(sala == NULL) {
    g_mutex_unlock(&cs->mtx_serv);
    printf("manejar_join_room(): No existe la sala '%s'\n", nom_sala);
    cJSON *resp_err = fabrica_respuesta(RESPONSE, &(Campos) {
        .operation = INVITE_O,
        .result = NO_SUCH_ROOM,
        .extra = nom_sala
      });
    enviar_resp(sockfd, resp_err, 1);
    return 0;
  }

  if (!g_hash_table_contains(sala->usrs_invit, GINT_TO_POINTER(sockfd))) {
    g_mutex_unlock(&cs->mtx_serv);
    printf("manejar_join_room(): El usuario '%s' no está invitado\n", cliente->username);
    cJSON *resp_no_invit = fabrica_respuesta(RESPONSE, &(Campos) {
        .operation = JOIN_ROOM_O,
        .result = NOT_INVITED,
        .extra = nom_sala
      });
    enviar_resp(sockfd, resp_no_invit, 1);
    return 0;
  }

  agregar_cliente_sala(cliente->username, sockfd, sala);

  cJSON *notif_join = fabrica_respuesta(JOINED_ROOM, &(Campos) {
      .roomname = nom_sala,
      .username = cliente->username
    });
  
  GHashTableIter it_sala;
  gpointer i_sockfd;
  g_hash_table_iter_init(&it_sala, sala->usrs_sala);

  while(g_hash_table_iter_next(&it_sala, &i_sockfd, NULL)) {
    int sockfd_dst = GPOINTER_TO_INT(i_sockfd);
    if(sockfd_dst == sockfd) 
      continue;
    enviar_resp(sockfd_dst, notif_join, 0);
  }
    
  g_mutex_unlock(&cs->mtx_serv);

  cJSON_Delete(notif_join);

  cJSON *resp_succ = fabrica_respuesta(RESPONSE, &(Campos) {
      .operation = JOIN_ROOM_O,
      .result = SUCCESS,
      .extra = nom_sala
    });
  enviar_resp(sockfd, resp_succ, 1);
  
  return 0;
}

int manejar_leave_room(int sockfd, const cJSON *json_msj, ContextoServidor *cs) {

  const cJSON *roomname = cJSON_GetObjectItemCaseSensitive(json_msj, "roomname");
  if(!(cJSON_IsString(roomname)) || (roomname == NULL)) {
    printf("manejar_leave_room(): campo 'roomname' ausente o inválido\n");
    cJSON *resp_err = fabrica_respuesta(RESPONSE, &(Campos) {
        .operation = INVALID_O,
        .result = INVALID,
      });
    enviar_resp(sockfd, resp_err, 1);
    return 1;
  }

  char nom_sala[17];
  strncpy(nom_sala, roomname->valuestring, 16);
  nom_sala[16] = '\0';
  
  g_mutex_lock(&cs->mtx_serv);

  Cliente *cliente = g_hash_table_lookup(cs->sala_gral->usrs, GINT_TO_POINTER(sockfd));
  if(cliente == NULL) {
    g_mutex_unlock(&cs->mtx_serv);
    printf("manejar_leave_room(): Cliente '%d' no identificado \n", sockfd);
    cJSON *resp_err = fabrica_respuesta(RESPONSE, &(Campos) {
        .operation = INVALID_O,
        .result = NOT_IDENTIFIED
      });
    enviar_resp(sockfd, resp_err, 1);
    return 1;
  }

  Sala *sala = g_hash_table_lookup(cs->contex_salas->salas, nom_sala);
  if (sala == NULL) {
    g_mutex_unlock(&cs->mtx_serv);
    printf("manejar_leave_room(): No existe la sala '%s'\n", nom_sala);
    cJSON *resp_err = fabrica_respuesta(RESPONSE, &(Campos) {
        .operation = INVITE_O,
        .result = NO_SUCH_ROOM,
        .extra = nom_sala
      });
    enviar_resp(sockfd, resp_err, 1);
    return 0;
  }

   if (!g_hash_table_contains(sala->usrs_sala, GINT_TO_POINTER(sockfd))) {
     g_mutex_unlock(&cs->mtx_serv);
     printf("manejar_leave_room(): Cliente '%d' no identificado \n", sockfd);
     cJSON *resp_err = fabrica_respuesta(RESPONSE, &(Campos) {
         .operation = LEAVE_ROOM_O,
         .result = NOT_JOINED,
         .extra = nom_sala
       });
     enviar_resp(sockfd, resp_err, 1);
     return 0;
   }

   g_hash_table_remove(sala->usrs_sala, GINT_TO_POINTER(sockfd));

   if(g_hash_table_size(sala->usrs_sala) == 0) {
     g_hash_table_remove(cs->contex_salas->salas, nom_sala);
     printf("manejar_leave_room(): Sala %s eliminada por quedar vacia.\n", nom_sala);
   } else {
     cJSON *notif_left = fabrica_respuesta(LEFT_ROOM, &(Campos){
         .roomname = nom_sala,
         .username = cliente->username
       });
   
     GHashTableIter it_sala;
     gpointer i_sockfd;
     g_hash_table_iter_init(&it_sala, sala->usrs_sala);

     while(g_hash_table_iter_next(&it_sala, &i_sockfd, NULL)) {
       int sockfd_dst = GPOINTER_TO_INT(i_sockfd);
       if(sockfd_dst == sockfd) 
         continue;
       enviar_resp(sockfd_dst, notif_left, 0);
     }
     cJSON_Delete(notif_left);
   }
  
   g_mutex_unlock(&cs->mtx_serv);

   return 0;
}

int manejar_disconnect(int sockfd, ContextoServidor *cs) {

  g_mutex_lock(&cs->mtx_serv);
  
  Cliente *cliente = g_hash_table_lookup(cs->sala_gral->usrs, GINT_TO_POINTER(sockfd));
  if(cliente == NULL) {
    g_mutex_unlock(&cs->mtx_serv);
    return 1;
  }
  
  char nom_usuario[9];
  strncpy(nom_usuario, cliente->username, 8);
  nom_usuario[8] = '\0';

  g_hash_table_remove(cs->sala_gral->usrs, GINT_TO_POINTER(sockfd));

  cJSON *notif_global = fabrica_respuesta(DISCONNECTED, &(Campos) {
      .username = nom_usuario,
    });
  
  GHashTableIter it_sala_gral;
  gpointer i_sockfd;
  
  g_hash_table_iter_init(&it_sala_gral, cs->sala_gral->usrs);
  while(g_hash_table_iter_next(&it_sala_gral, &i_sockfd, NULL)) {
    int sockfd_dst = GPOINTER_TO_INT(i_sockfd);
    enviar_resp(sockfd_dst, notif_global, 0);
  }

  cJSON_Delete(notif_global);
    
  GHashTableIter it_salas;
  gpointer nom_sala, sala;

  g_hash_table_iter_init(&it_salas, cs->contex_salas->salas);

  // Iteramos sobre todas las salas del servidor
  while(g_hash_table_iter_next(&it_salas, &nom_sala, &sala)) {
    Sala *s = (Sala *) sala;

    g_hash_table_remove(s->usrs_invit, GINT_TO_POINTER(sockfd));
    
    if (g_hash_table_contains(s->usrs_sala, GINT_TO_POINTER(sockfd))) {
        g_hash_table_remove(s->usrs_sala, GINT_TO_POINTER(sockfd)); 
        printf("manejar_disconnect(): Hay %d usuarios en la sala '%s'", g_hash_table_size(s->usrs_sala), s->nombre_sala);
        if(g_hash_table_size(s->usrs_sala) == 0) {
          printf("manejar_disconect(): Sala %s eliminada por quedar vacia.\n", s->nombre_sala);
          g_hash_table_iter_remove(&it_salas);
        } else {
          cJSON *notif = fabrica_respuesta(LEFT_ROOM, &(Campos){
              .roomname = s->nombre_sala,
              .username = nom_usuario
            });

          GHashTableIter it_usrs_sala;
          gpointer i_sockfd, cliente;

          g_hash_table_iter_init(&it_usrs_sala, s->usrs_sala);
          while(g_hash_table_iter_next(&it_usrs_sala, &i_sockfd, &cliente)) {
            int sockfd_dest = GPOINTER_TO_INT(i_sockfd);
            enviar_resp(sockfd_dest, notif, 0);
          }
          cJSON_Delete(notif);
        }
    }
  }
  
  g_mutex_unlock(&cs->mtx_serv);
    
  return 1; 
}
