#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <cjson/cJSON.h>


#define PORT_DEFAULT 1234  // Puerto por defecto.
#define BACKLOG 10         // Tamaño de la cola de conexiones. 

int iniciar_conex(int puerto, int backlog, int debug);
int aceptar_conex(int socket_fd, int debug);
int terminar_conex(int socket_fd, int debug);

int conectar(char *hostname, int puerto, int debug);

int enviar_resp(int socket_fd, cJSON *json, int liberarJSON);
