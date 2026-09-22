#ifndef DISTRIBUIDOR_H
#define DISTRIBUIDOR_H

#include <cjson/cJSON.h>
#include <glib.h>
#include "protocolo.h"
#include "manejador.h"

int distribuidor_peticiones(int sockfd, const cJSON *json_msj);

#endif 
