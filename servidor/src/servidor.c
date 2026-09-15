#include <cjson/cJSON.h>
#include "include/socket_util.h"

#define BUFFER_SIZE 1048576

int main(int argc, char *argv[]) {

  if(argc < 2) {
    printf("Uso: %s <puerto>'\n", argv[0]);
    return 1;
  }
    
  int sock, sockdup, puerto = atoi(argv[1]);
  char resp[] = "{\"type\": \"RESPONSE\"}";
  char buffer[BUFFER_SIZE];
  
  if(puerto <= 1024) {
    printf("Puerto inválido, se usará el puerto por defecto (1234)\n");
    puerto = 0;
  }
  
  sock = iniciar_conex(puerto, 10, 1);

  while(1) {
    if((sockdup = aceptar_conex(sock, 1)) < 0)
      continue;

    while(1) {
      memset(buffer, 0, sizeof(buffer));
      ssize_t valread = read(sockdup, buffer, sizeof(buffer) - 1);
      if(valread <= 0) {
        printf("ERROR: No se puede leer del cliente.\n");
        break;
      }

      buffer[valread] = '\0';

      cJSON *json = cJSON_Parse(buffer);
      if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
          printf("Error: %s\n", error_ptr);
        }
        cJSON_Delete(json);
        continue;
      }

      cJSON *type = cJSON_GetObjectItemCaseSensitive(json, "type");
      if (cJSON_IsString(type) && (type->valuestring != NULL)) {
        printf("Tipo operación: %s\n", type->valuestring);
      }
    
      cJSON_Delete(json);
      write(sockdup, resp, strlen(resp));
    }

    close(sockdup);
  }
  close(sock);
  return 0;
}
