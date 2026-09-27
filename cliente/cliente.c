#include <stdio.h>
#include "socket_util.h"

#define BUFFER_SIZE 1048576

int main(int argc, char *argv[]) {

  if(argc < 3) {
    printf("Uso: %s <ip_addr> <puerto>\n", argv[0]);
    return 1;
  }

  int sockfd;
  const char *ip = argv[1];
  int puerto = atoi(argv[2]);

  if(puerto <= 1024) {
    printf("Puerto inválido, se usará el puerto por defecto (1234)\n");
    puerto = 0;
  }

  sockfd = conectar(ip, puerto, 1);
  if(sockfd < 0) {
    printf("ERROR: No se pudo establecer la conexion");
    return 1;
  }

  printf("Se estableció la conexión");

  close(sockfd);
  
  return 0;
}

