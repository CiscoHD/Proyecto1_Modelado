#include <stdio.h>
#include <locale.h>
#include <sds.h>
#include "socket_util.h"
#include "manejadores_vista.h"

#define BUFFER_SIZE 1048576

typedef struct {
  VentanaGral *v;
  int sockfd_serv;
} DatosHilo;

gpointer escuchar_servidor(gpointer datos) {
  DatosHilo *d = (DatosHilo *) datos;

  VentanaGral *ventana = d->v;
  int sockfd_s = d->sockfd_serv;
  
  int desconectar = 0;
  char buffer_notif[BUFFER_SIZE];
  
  while (desconectar != 1) {
    memset(buffer_notif, 0, sizeof(buffer_notif));
    ssize_t valread = read(sockfd_s, buffer_notif, sizeof(buffer_notif) - 1);
    
    if(valread <= 0) {
      sds mensaje_err = "";
      mensaje_err = sdscatprintf(mensaje_err, "Se ha pérdido la conexión en el socket %d", sockfd_s);
      imprimir_mensaje(ventana, "[ERROR]:", NULL, mensaje_err, ERROR);
      desconectar = 1;
      break;
    }
    //determina_notificación();
  }
  
  return NULL;
}

int main(int argc, char *argv[]) {

  if(argc < 3) {
    printf("Uso: %s <ip_addr> <puerto>\n", argv[0]);
    return 1;
  }

  int sockfd, conectado;
  const char *ip = argv[1];
  int puerto = atoi(argv[2]);
  char buffer[BUFFER_SIZE];
  
  if(puerto <= 1024) {
    printf("Puerto inválido, se usará el puerto por defecto (1234)\n");
    puerto = 0;
  }

  sockfd = conectar(ip, puerto, 1);
  
  VentanaGral *ventana = iniciar_ventana();
  if (!ventana) {
      printf("Error al inicializar la interfaz TUI.\n");
      return 1;
  }
  
  DatosHilo *datos = (DatosHilo *) malloc(sizeof(DatosHilo));
  datos->v = ventana;
  datos->sockfd_serv = sockfd;
  
  GThread *hilo_escucha = g_thread_new(NULL, escuchar_servidor, datos);
  
  actualizar_entrada(ventana, NULL, NULL);

  if (sockfd < 0) {
    imprimir_mensaje(ventana, "[ERROR]:", NULL, "No se pudo establecer la conexión", ERROR);
    imprimir_mensaje(ventana, "[SISTEMA]:", NULL, "Presiona cualquier tecla para salir", SISTEMA);
    getch();   
    destruir_ventana_gral(ventana);
    return 1;
  }

  imprimir_mensaje(ventana, "[SISTEMA]:",NULL,"Conexión exitosa", SISTEMA);
  
  conectado = 1;
  while(conectado) {
    leer_entrada(ventana, buffer, sizeof(buffer));

    parsear_entrada(buffer);

    imprimir_mensaje(ventana, "[ECHO]:", NULL, buffer, -1);
    actualizar_entrada(ventana, NULL, NULL);
  }
  
  destruir_ventana_gral(ventana);
  close(sockfd);
  return 0;
}

