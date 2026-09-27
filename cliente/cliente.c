#include <stdio.h>
#include <locale.h>
#include "socket_util.h"
#include "manejadores_vista.h"

#define BUFFER_SIZE 1048576

int main(int argc, char *argv[]) {

  if(argc < 3) {
    printf("Uso: %s <ip_addr> <puerto>\n", argv[0]);
    return 1;
  }

  int sockfd;
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
      fprintf(stderr, "Error al inicializar la interfaz TUI.\n");
      return 1;
  }
  
  actualizar_entrada(ventana, NULL, NULL);

  if (sockfd < 0) {
    imprimir_mensaje(ventana, "[ERROR]:", NULL, "No se pudo establecer la conexión\n", ERROR);
    imprimir_mensaje(ventana, "[SISTEMA]:", NULL, "Presiona cualquier tecla para salir\n", SISTEMA);
    getch();   
    destruir_ventana_gral(ventana);
    return 1;
  }

  imprimir_mensaje(ventana, "[SISTEMA]:",NULL,"Conexión exitosa\n", SISTEMA);
  imprimir_mensaje(ventana, "[SISTEMA]:", NULL, "Presiona cualquier tecla para salir\n", SISTEMA);

  leer_entrada(ventana, buffer, sizeof(buffer));

  imprimir_mensaje(ventana, "[ECHO]:", NULL, buffer, -1);
  actualizar_entrada(ventana, NULL, NULL);

  sleep(3);

  destruir_ventana_gral(ventana);
  close(sockfd);
  return 0;
}

