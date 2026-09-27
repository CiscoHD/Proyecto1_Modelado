#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <glib.h>
#include "protocolo.h"

typedef struct {
  char *username;
  Estado status;
} Cliente;

typedef struct {
  GHashTable *usrs;         /*K:"sockfd, V:Cliente*/
} SalaGeneral;

typedef struct {
  char *nombre_sala;
  GHashTable *usrs_sala;    /*K:"sockfd, V:Cliente*/
  GHashTable *usrs_invit;   /*K:"sockfd, V:Cliente*/
} Sala;

typedef struct {
  GHashTable *salas;        /*K:"nombre_sala", V:Sala*/
  //GMutex mtx_salas;
} ContextoSalas;

typedef struct {
  SalaGeneral *sala_gral;
  ContextoSalas *contex_salas;
  GMutex mtx_serv;
} ContextoServidor;

typedef struct {
  int socket_fd_h;
  ContextoServidor *contexto;
} DatosHilo;

Cliente* crear_cliente(const char *name);
ContextoServidor* crear_contexto_servidor();
ContextoSalas* crear_contexto_salas();
SalaGeneral* crear_sala_general();
Sala* crear_sala(const char *nom_sala);

int agregar_cliente_gral(const char *username, int sockfd, SalaGeneral *gral);
int agregar_cliente_sala(const char *username, int sockfd, Sala *sala);
int invitar_cliente_sala(const char *username, int sockfd, Sala *sala);

void libera_memoria_cliente_hash(gpointer data);
void libera_memoria_cliente(Cliente *cliente);
void libera_memoria_sala_hash(gpointer data);
void libera_memoria_sala(Sala *sala);

#endif
