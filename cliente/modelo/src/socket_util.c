#include "socket_util.h"

int conectar(const char *ip, int puerto, int debug) {
  int sockfd;
  struct sockaddr_in serv_addr; /* dirección del server donde se
                                    conectará */

  /* Creamos el socket */
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)	{
    perror("Error en creación de socket");
    exit(1);
  } if (debug)
      fprintf(stdout, "debug:: conectar() socket()=%d\t\t..........OK\n", sockfd);

  puerto = (puerto == 0) ? htons(PORT) : htons(puerto);

  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = puerto;

  if (debug)
    fprintf(stdout, "debug:: conectar() dst port()=%d\t..........OK\n", puerto);
  
  if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
    fprintf(stderr, "[ERROR] Dirección IP inválida: %s\n", ip);
    close(sockfd);
    return -1;
  }
  
  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("[ERROR] Falló la conexión con el servidor");
    close(sockfd);
    return -1;
  }

  if (debug)
      fprintf(stdout, "debug:: conectar() connect()\t\t..........OK\n");

  return sockfd;
}
