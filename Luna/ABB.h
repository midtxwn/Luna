#ifndef ABB_H
#define ABB_H



#include "COMMONDEF.h"


typedef struct ABB ABB;

ABB* ABB_crear(void);
void ABB_insertar(ABB *arbol, symrec dato);
symrec* ABB_buscar(ABB *arbol, const char *lexema);
void ABB_eliminar(ABB *arbol, const char *lexema);
void ABB_destruir(ABB *arbol);
void ABB_imprimir(ABB *arbol);
void ABB_imprimir_noindent(ABB *arbol);

#endif