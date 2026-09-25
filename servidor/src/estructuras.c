#include "estructuras.h"

Cliente* crear_cliente(const char *nom_usr) {
  Cliente *cliente = (Cliente *) malloc(sizeof(Cliente));

  cliente->username = (char *) malloc(sizeof(char) * (strlen(nom_usr) + 1));

  strcpy(cliente->username, nom_usr);
  cliente->status = ACTIVE;
  
  return cliente;
}

SalaGeneral* crear_sala_general() {
  SalaGeneral *general = (SalaGeneral *) malloc(sizeof(SalaGeneral));
  
  general->usrs = g_hash_table_new_full(g_direct_hash, g_direct_equal,
                                          NULL,
                                          libera_memoria_cliente_hash);

  return general;
}

ContextoServidor* crear_contexto_servidor() {
  ContextoServidor *cs = (ContextoServidor *) malloc(sizeof(ContextoServidor));

  cs->sala_gral = crear_sala_general();
  cs->contex_salas = crear_contexto_salas();

  g_mutex_init(&cs->mtx_serv);
  return cs;
}

ContextoSalas* crear_contexto_salas() {
  ContextoSalas *salas = (ContextoSalas *) malloc(sizeof(ContextoSalas));

  salas->salas = g_hash_table_new_full(g_str_hash, g_str_equal,
                                       free,
                                       libera_memoria_sala_hash
                                      );
  return salas;
}

Sala* crear_sala(const char *nom_sala) {
  Sala *sala = (Sala *) malloc(sizeof(Sala));

  sala->usrs_sala = g_hash_table_new_full(g_direct_hash, g_direct_equal,
                                          NULL,
                                          libera_memoria_cliente_hash);
  
  sala->usrs_invit = g_hash_table_new_full(g_direct_hash, g_direct_equal,
                                           NULL,
                                           libera_memoria_cliente_hash);
  
  sala->nombre_sala = (char *) malloc(sizeof(char) * (strlen(nom_sala) + 1));

  strcpy(sala->nombre_sala, nom_sala);
  return sala;
}

int agregar_cliente_gral(const char *username, int sockfd, SalaGeneral *gral) {

  if(g_hash_table_contains(gral->usrs, GINT_TO_POINTER(sockfd))) {
    printf("agregar_cliente(): El cliente se encontró\n");
    return -1;   
  }

  GHashTableIter iter;
  gpointer i_sockfd, cliente;
  g_hash_table_iter_init(&iter, gral->usrs);
  
  while (g_hash_table_iter_next(&iter, &i_sockfd, &cliente)) {
    Cliente *c = (Cliente *) cliente;
    if (strcmp(c->username, username) == 0) {
      printf("agregar_cliente(): El nombre de usuario ya existe\n");
      return -1;
    }
  }
  
  Cliente *nuevo = crear_cliente(username);
  g_hash_table_insert(gral->usrs, GINT_TO_POINTER(sockfd), nuevo);
  return 0;
}

int agregar_cliente_sala(const char *username, int sockfd, Sala *sala) {
  if(sala == NULL) {
    printf("agregar_cliente_sala(): La sala no existe\n");
    return -1;   //Error de parametro. 
  }

  Cliente *nuevo_cliente = crear_cliente(username);
  g_hash_table_remove(sala->usrs_invit, GINT_TO_POINTER(sockfd));
  
  g_hash_table_insert(sala->usrs_sala, GINT_TO_POINTER(sockfd), nuevo_cliente);
  return 0;
}

int invitar_cliente_sala(const char *username, int sockfd, Sala *sala) {
  if(sala == NULL) {
    printf("invitar_cliente_sala(): La sala no existe\n");
    return -1;   //Error de parametro.     
  }

  Cliente *nuevo_invitado = crear_cliente(username);
  g_hash_table_insert(sala->usrs_invit, GINT_TO_POINTER(sockfd), nuevo_invitado);
  return 0; 
}

void libera_memoria_cliente_hash(gpointer data) {
  Cliente *cliente = (Cliente *) data;
  libera_memoria_cliente(cliente);
}

void libera_memoria_cliente(Cliente *cliente) {
  if(cliente == NULL)
    return;

  if(cliente->username != NULL)
    free(cliente->username);

  free(cliente);
}

void libera_memoria_sala_hash(gpointer data) {
  Sala *sala = (Sala *) data;
  libera_memoria_sala(sala);
}

void libera_memoria_sala(Sala *sala) {
  if(sala == NULL)
    return;

  g_hash_table_destroy(sala->usrs_sala);
  g_hash_table_destroy(sala->usrs_invit);
  
  free(sala->nombre_sala);

  //g_mutex_clear(&sala->mtx_sala);
  
  free(sala);
}

