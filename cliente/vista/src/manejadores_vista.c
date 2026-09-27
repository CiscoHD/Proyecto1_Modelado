#include "manejadores_vista.h"

VentanaGral* iniciar_ventana() {
  setlocale(LC_ALL, "");

  VentanaGral *v = (VentanaGral*) malloc(sizeof(VentanaGral));
  g_mutex_init(&v->mtx_ventana);
  
  initscr();              
  cbreak();               
  noecho();               
  keypad(stdscr, TRUE);   
  curs_set(1);

  if (has_colors()) {
    start_color();

    use_default_colors();
    init_pair(EXITO,    COLOR_GREEN, -1);
    init_pair(ERROR,    COLOR_RED, -1);
    init_pair(MSJ_PRIV, COLOR_MAGENTA, -1);
    init_pair(MSJ_SALA, COLOR_CYAN, -1);
    init_pair(SISTEMA,  COLOR_YELLOW, -1);
  }

  v->v_msjs = newwin(LINES - 1, COLS, 0, 0);
  v->v_entrada = newwin(1, COLS, LINES - 1, 0);

  scrollok(v->v_msjs, TRUE);
  keypad(v->v_entrada, TRUE);
  
  wrefresh(v->v_msjs);
  wrefresh(v->v_entrada);

  return v;
}

void destruir_ventana_gral(VentanaGral *v) {

  if (v->v_msjs)
    delwin(v->v_msjs);
  
  if (v->v_entrada)
    delwin(v->v_entrada);

  g_mutex_clear(&v->mtx_ventana);
  endwin(); 

  free(v);
}


void imprimir_mensaje(VentanaGral *v, const char *tipo, const char *usr, const char *msj, int color){

  g_mutex_lock(&v->mtx_ventana);

  if (has_colors() && color > 0) {
    wattron(v->v_msjs, COLOR_PAIR(color) | A_BOLD);
    wprintw(v->v_msjs, "%s ", tipo);
    wattroff(v->v_msjs, COLOR_PAIR(color) | A_BOLD);
  } else {
    wprintw(v->v_msjs, "%s ", tipo);
  }
  
  if ((usr != NULL) && (strlen(usr) > 0)) {
    wprintw(v->v_msjs, "%s : ", usr);
  }

  wprintw(v->v_msjs, "%s", msj);

  wrefresh(v->v_msjs);
  
  g_mutex_unlock(&v->mtx_ventana);
  
}

void actualizar_entrada(VentanaGral *v, const char *username, const char *estado) {

  const char *usr = (username && strlen(username) > 0) ? username : "NO_IDENTIFICADO";
  const char *est = (estado && strlen(estado) > 0) ? estado : "NO_IDENTIFICADO";
  
  g_mutex_lock(&v->mtx_ventana);

  wclear(v->v_entrada);
  wprintw(v->v_entrada, "[%s | %s] > ",  usr, est);
  wrefresh(v->v_entrada);

  g_mutex_unlock(&v->mtx_ventana);
  
  return;
}

void leer_entrada(VentanaGral *v, char *buffer, int max_len) {

  echo();
  wgetnstr(v->v_entrada, buffer, max_len - 1);
  noecho();
  
  g_mutex_lock(&v->mtx_ventana);

  wclear(v->v_entrada);
  wrefresh(v->v_entrada);
  
  g_mutex_unlock(&v->mtx_ventana);

}
