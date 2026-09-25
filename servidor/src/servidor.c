#include <cjson/cJSON.h>
#include <glib.h>
#include "include/socket_util.h"
#include "include/distribuidor.h"
#include "estructuras.h"

#define BUFFER_SIZE 1048576

gpointer entrada_hilo(gpointer datos) {
  DatosHilo *d = (DatosHilo *)datos;

  int sockfd_h = d->socket_fd_h;         /*Socket file descriptor en el hilo*/
  ContextoServidor *cs = d->contexto;
  g_free(d);
  
  int desconectar = 0;
  char buffer[BUFFER_SIZE];

  printf("[Hilo: %p] Conexión con cliente %d\n", g_thread_self(), sockfd_h);

  while(desconectar != 1) {
    memset(buffer, 0, sizeof(buffer));
    ssize_t valread = read(sockfd_h, buffer, sizeof(buffer) - 1);
    
    if(valread <= 0) {
      printf("[Hilo: %p] ERROR: El cliente %d se ha desconectado.\n", g_thread_self(), sockfd_h);
      desconectar = 1;
      break;
    }

    buffer[valread] = '\0';
    
    cJSON *json = cJSON_Parse(buffer);
    
    desconectar = distribuidor_peticiones(sockfd_h, json, cs);

    cJSON_Delete(json);
  }

  manejar_disconnect(sockfd_h, cs);
  
  close(sockfd_h);
  return NULL;
}

int main(int argc, char *argv[]) {

  if(argc < 2) {
    printf("Uso: %s <puerto>'\n", argv[0]);
    return 1;
  }
    
  int sockfd;
  int sockfd_usr;
  int puerto = atoi(argv[1]);

  if(puerto <= 1024) {
    printf("Puerto inválido, se usará el puerto por defecto (1234)\n");
    puerto = 0;
  }
  
  sockfd = iniciar_conex(puerto, 10, 1);

  ContextoServidor *cs = crear_contexto_servidor(); 

  while(1) {
    if((sockfd_usr = aceptar_conex(sockfd, 1)) < 0)
      continue;

    DatosHilo *datos = (DatosHilo *) malloc(sizeof(DatosHilo));
    datos->socket_fd_h = sockfd_usr;
    datos->contexto = cs;

    /*Se crea un hilo para atender (con la función entrada_hilo) al
      nuevo cliente*/
    GThread *hilo = g_thread_new("usuario", entrada_hilo, datos);

        
    g_thread_unref(hilo);
  }
  
  close(sockfd);
  return 0;
}
