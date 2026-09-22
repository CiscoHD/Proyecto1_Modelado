#include <cjson/cJSON.h>
#include <glib.h>
#include "include/socket_util.h"
#include "include/distribuidor.h"

#define BUFFER_SIZE 1048576

gpointer entrada_hilo(gpointer datos) {
  int sockfd_h;                          /*Socket file descriptor en el hilo*/
  int codigo_error = 0;
  sockfd_h = *(int *)datos;
  g_free(datos);

  char buffer[BUFFER_SIZE];

  printf("[Hilo: %p] Conexión con cliente %d\n", g_thread_self(), sockfd_h);

  while(codigo_error != 1) {
    memset(buffer, 0, sizeof(buffer));
    ssize_t valread = read(sockfd_h, buffer, sizeof(buffer) - 1);
    
    if(valread <= 0) {
      printf("[Hilo: %p] ERROR: El cliente %d se ha desconectado.\n", g_thread_self(), sockfd_h);
      break;
    }

    buffer[valread] = '\0';
    
    cJSON *json = cJSON_Parse(buffer);
    
    codigo_error = distribuidor_peticiones(sockfd_h, json);
    
    cJSON_Delete(json);
  }
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

  while(1) {
    if((sockfd_usr = aceptar_conex(sockfd, 1)) < 0)
      continue;

    /*Por cada cliente que se conecte, vamos a guardar un apuntador a
      sockfd_usr para poder pasarlos a g_thread_new()_*/
    int *psockfd_usr = g_malloc(sizeof(int));
    *psockfd_usr = sockfd_usr;

    /*Se crea un hilo para atender (con la función entrada_hilo) al
      nuevo cliente*/
    GThread *hilo = g_thread_new("usuario", entrada_hilo, psockfd_usr);
    
    g_thread_unref(hilo);
  }
  
  close(sockfd);
  return 0;
}
