#ifndef TS_H
#define TS_H

#include "COMMONDEF.h"
#include "ABB.h"


typedef struct TablaSimbolos TablaSimbolos;

// Funciones existentes
void TS_insertar(TablaSimbolos* ts, symrec entrada);
symrec* TS_buscar(TablaSimbolos* ts, const char* lexema);  // Buscar sin insertar
TablaSimbolos* TS_crear();
void TS_eliminar_elemento(TablaSimbolos* ts, const char *lexema);
void TS_destruir(TablaSimbolos* ts);
void TS_imprimir(TablaSimbolos* ts);
void TS_print_workspace(TablaSimbolos* ts);

// Nueva función: Busca y si no existe, inserta como VAR con NaN
symrec* TS_buscar_o_insertar_var(TablaSimbolos* ts, const char* lexema);

#endif