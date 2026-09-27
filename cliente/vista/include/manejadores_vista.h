#ifndef MANEJADORES_VISTA_H
#define MANEJADORES_VISTA_H
#include <ncurses.h>
#include <glib.h>
#include <locale.h>

typedef struct {
  WINDOW *v_msjs;   
  WINDOW *v_entrada;    
  GMutex mtx_ventana;
} VentanaGral;

typedef enum {
  EXITO,
  ERROR,
  MSJ_PRIV,
  MSJ_SALA,
  SISTEMA,
} TipoPreambulo;

VentanaGral* iniciar_ventana();
void destruir_ventana_gral(VentanaGral *ventana);

void imprimir_mensaje(VentanaGral *v, const char *tipo, const char *usr, const char *msj, int color);
void actualizar_entrada(VentanaGral *v, const char *username, const char *estado);
void leer_entrada(VentanaGral *v, char *buffer, int max_len);

#endif
