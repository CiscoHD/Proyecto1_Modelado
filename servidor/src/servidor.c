#include <cjson/cJSON.h>
#include "include/socket_util.h"

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {

  if(argc < 2) {
    printf("Uso: %s <puerto>'\n", argv[0]);
    return 1;
  }
    
  int sock, sockdup, puerto = 0;
  char resp[] = "{\"type\": \"RESPONSE\"}";
  char buffer[BUFFER_SIZE];  

  sock = iniciar_conex(puerto, 10, 1);

  memset(buffer, 0, sizeof(buffer));              /* Limpiamos el buffer */
  
  while(1) {
    sockdup = aceptar_conex(sock, 1);

    ssize_t valread = read(sockdup, buffer, sizeof(buffer) - 1); 
    
    cJSON *json = cJSON_Parse(buffer);
    if (json == NULL) {
      const char *error_ptr = cJSON_GetErrorPtr();
      if (error_ptr != NULL) {
        printf("Error: %s\n", error_ptr);
      }
      cJSON_Delete(json);
      return 1;
    }

    cJSON *type = cJSON_GetObjectItemCaseSensitive(json, "type");
    if (cJSON_IsString(type) && (type->valuestring != NULL)) {
      printf("Tipo operación: %s\n", type->valuestring);
    }
    
    cJSON_Delete(json);
        
    write(sockdup, resp, strlen(resp));
    close(sockdup);
  }
    
  close(sock);
}
