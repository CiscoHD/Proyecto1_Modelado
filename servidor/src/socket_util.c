#include "socket_util.h"

int iniciar_conex(int puerto, int backlog, int debug) {
  int sock_aux, aux;
  struct sockaddr_in my_addr;

  if((sock_aux = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    fprintf(stderr, "ERROR: función socket(), codigo de error %s\n", strerror(sock_aux));
    return -1;
  } if (debug)
      fprintf(stderr, "debug:: abrir_conexion() socket()=%d\t..........OK\n", sock_aux);

  int optval = 1;
  if (setsockopt(sock_aux, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
    fprintf(stderr, "Error en función setsockopt()\n");
    return -1;
  } if (debug)
      fprintf(stderr, "debug:: abrir_conexion() setsockopt()\t..........OK\n");

  if (puerto == 0)
    puerto = PORT_DEFAULT;

  my_addr.sin_family = AF_INET;		    /* familia de sockets INET para UNIX*/
  my_addr.sin_port = htons(puerto);	    /* convierte el entero formato PC a entero formato network*/
  my_addr.sin_addr.s_addr = INADDR_ANY; /* automaticamente usa la IP local */
  bzero(&(my_addr.sin_zero), 8);	    /* rellena con ceros el resto de la estructura */

  /* Con la estructura sockaddr_in completa, se declara en el Sistema
     que este proceso escuchará pedidos por la IP y el port
     definidos*/
  if ((aux = bind(sock_aux, (struct sockaddr *)&my_addr, sizeof(struct sockaddr))) == -1) {
    fprintf(stderr, "Error en función bind. Código de error %s\n", strerror(aux));
    return -1;
  } if (debug)
      fprintf(stderr, "debug:: abrir_conexion() bind()\t\t..........OK\n");

  /* Habilitamos el socket para recibir conexiones, con una cola de
     conexiones en espera que tendrá como máximo el tamaño
     especificado en BACKLOG*/

  if (backlog == 0)
    backlog = BACKLOG;

  if ((aux = listen(sock_aux, backlog)) == -1) {
    fprintf(stderr, "Error en función listen. Código de error %s\n", strerror(aux));
    return -1;
  } if (debug)
      fprintf(stderr, "debug:: abrir_conexion() listen()\t..........OK\n");
  
  return sock_aux;
}

int aceptar_conex(int socket_fd, int debug) {
  int newfd;					 /* Por este socket duplicado del inicial se transaccionará*/
  struct sockaddr_in their_addr; /* Contendra la direccion IP y número de puerto del cliente */
  unsigned int sin_size = sizeof(struct sockaddr_in);

  /*Se espera por conexiones ,*/
  if ((newfd = accept(socket_fd, (struct sockaddr *)&their_addr, &sin_size)) == -1) {
    fprintf(stderr, "Error en función accept. Código de error %s\n", strerror(newfd));
    return -1;
  }
  if (debug)
    fprintf(stderr, "debug:: aceptar_pedidos() conexión desde: %s\n", inet_ntoa(their_addr.sin_addr));
    
  return newfd;
}

int conectar(char *hostname, int puerto, int debug) {
  int sockfd;
  struct hostent *he;	  	     /* Se utiliza para convertir el nombre del host a su dirección IP */
  struct sockaddr_in their_addr; /* dirección del server donde se conectará */

  // convertimos el nombre del host a su dirección IP
  if ((he = gethostbyname(hostname)) == NULL) {
    herror("Error en Nombre de Host");
    exit(1);
  }

  /* Creamos el socket */
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)	{
    perror("Error en creación de socket");
    exit(1);
  } if (debug)
      fprintf(stderr, "debug:: conectar() socket()=%d\t\t..........OK\n", sockfd);

  if (debug)
    fprintf(stderr, "debug:: conectar() dst port()=%d\t..........OK\n", puerto);

  puerto = (puerto == 0) ? htons(PORT_DEFAULT) : htons(puerto);

  /* Establecemos their_addr con la direccion del server */
  their_addr.sin_family = AF_INET;
  their_addr.sin_port = puerto;
  their_addr.sin_addr = *((struct in_addr *)he->h_addr);
  bzero(&(their_addr.sin_zero), 8);

  /* Intentamos conectarnos con el servidor */
  if (connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1) {
    perror("conectar():: error tratando de conectar al server");
    exit(1);
  } if (debug)
      fprintf(stderr, "debug:: conectar() connect()\t\t..........OK\n");

  return sockfd;
}
