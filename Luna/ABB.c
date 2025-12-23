#include "ABB.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "parser.tab.h"

typedef struct NodoABB {
    symrec dato;
    struct NodoABB *izq;
    struct NodoABB *der;
} NodoABB;

struct ABB {
    NodoABB* raiz;
};

// Destruye un nodo y sus hijos (liberando memoria)
static void _destruir_nodos(NodoABB *nodo) {
    if (!nodo) return;
    _destruir_nodos(nodo->izq);
    _destruir_nodos(nodo->der);
    if (nodo->dato.type == VAR || nodo->dato.type == FUN) {
        free(nodo->dato.name);  // Solo liberar nombres de variables o funciones
    }
    free(nodo);
}

// Crea un ABB vacío
ABB* ABB_crear(void) {
    ABB* arbol = (ABB*)malloc(sizeof(ABB));
    arbol->raiz = NULL;
    return arbol;
}

// Inserta un símbolo en el ABB (reemplaza si ya existe)
void ABB_insertar(ABB *arbol, symrec dato) {
    NodoABB **actual = &arbol->raiz;
    while (*actual) {
        int cmp = strcmp(dato.name, (*actual)->dato.name);
        if (cmp == 0) {
            // Solo actualizar valor (no liberar nombre)
            (*actual)->dato = dato;
            return;
        }
        actual = (cmp < 0) ? &(*actual)->izq : &(*actual)->der;
    }
    // Insertar nuevo nodo (asignar puntero, no copia)
    *actual = (NodoABB*)malloc(sizeof(NodoABB));
    (*actual)->dato = dato;
    (*actual)->izq = (*actual)->der = NULL;
}

// Busca un símbolo por nombre (devuelve NULL si no existe)
symrec* ABB_buscar(ABB *arbol, const char *lexema) {
    NodoABB *actual = arbol->raiz;
    while (actual) {
        int cmp = strcmp(lexema, actual->dato.name);
        if (cmp == 0) return &actual->dato;
        actual = (cmp < 0) ? actual->izq : actual->der;
    }
    return NULL;
}

// Encuentra el nodo mínimo en un subárbol
static NodoABB* _encontrar_minimo(NodoABB* nodo) {
    while (nodo && nodo->izq) nodo = nodo->izq;
    return nodo;
}

// Función auxiliar para eliminar un nodo
static NodoABB* _eliminar_nodo(NodoABB* raiz, const char *lexema) {
    if (!raiz) return raiz;

    int cmp = strcmp(lexema, raiz->dato.name);
    if (cmp < 0) {
        raiz->izq = _eliminar_nodo(raiz->izq, lexema);
    } else if (cmp > 0) {
        raiz->der = _eliminar_nodo(raiz->der, lexema);
    } else {
        // Caso 1: Nodo sin hijos o con un solo hijo
        
        if (!raiz->izq) {
            NodoABB* temp = raiz->der;
            free(raiz);  // Liberar solo el nodo, no el nombre
            return temp;
        } else if (!raiz->der) {
            NodoABB* temp = raiz->izq;
            free(raiz);  // Liberar solo el nodo, no el nombre
            return temp;
        }

        // Caso 2: Nodo con dos hijos
        // Reemplazar por el sucesor inorden (mínimo del subárbol derecho)
        NodoABB* temp = _encontrar_minimo(raiz->der);
        
        // Copiar SOLO el valor (no el puntero al nombre)
        raiz->dato.value = temp->dato.value;
        raiz->dato.type = temp->dato.type;

        // Eliminar el sucesor inorden (su nombre NO se libera aquí)
        raiz->der = _eliminar_nodo(raiz->der, temp->dato.name);
    }
    return raiz;
}
// Elimina un símbolo del ABB
void ABB_eliminar(ABB *arbol, const char *lexema) {
    if (!arbol) return;
    arbol->raiz = _eliminar_nodo(arbol->raiz, lexema);
}

// Destruye el ABB
void ABB_destruir(ABB *arbol) {
    if (!arbol) return;
    _destruir_nodos(arbol->raiz);
    free(arbol);
}

// Imprime el ABB (para depuración)
static void _imprimir_nodos(NodoABB* nodo, int nivel) {
    if (!nodo) return;
    _imprimir_nodos(nodo->izq, nivel + 1);
    printf("%*s%s (%s)\n", nivel*4, "", nodo->dato.name, 
           nodo->dato.type == VAR ? "VAR" : "FUN");
    _imprimir_nodos(nodo->der, nivel + 1);
}

static void _imprimir_nodos_noindent(NodoABB* nodo) {
    if (!nodo) return;
    _imprimir_nodos_noindent(nodo->izq);
    if (nodo->dato.type == VAR)
    {
    printf("%s\t =\t %lf\t(%s)\n", nodo->dato.name, nodo->dato.value.var, 
           "VAR");
    }
    
    _imprimir_nodos_noindent(nodo->der);
}

void ABB_imprimir(ABB *arbol) {
    if (!arbol) return;
    _imprimir_nodos(arbol->raiz, 0);
}

void ABB_imprimir_noindent(ABB *arbol) {
    if (!arbol) return;
    _imprimir_nodos_noindent(arbol->raiz);
}