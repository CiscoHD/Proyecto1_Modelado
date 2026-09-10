#include "include/socket_util.h"

int main() {
  int sock, sockdup;
  char msg[] = "Hola desde el nuevo servidor";

  sock = iniciar_conex(8000, 10, 1);

  sockdup = aceptar_conex(sock, 1);

  write(sockdup, msg, strlen(msg));

  close(sockdup);
  close(sock);
}
