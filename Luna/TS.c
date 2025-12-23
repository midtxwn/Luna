#define _GNU_SOURCE  // para incluir ctes de math.h
#include "TS.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "COMMANDS.h"
#include "importhandler.h"
#include <math.h>
#include "parser.tab.h"
struct TablaSimbolos {
    ABB* arbol;
};

#include <math.h>  // Para sin, cos, log, exp

// ...
// Insertar predefined_symbols predefinidas



symrec predefined_symbols[] = {
    {.name = "sin",   .type = FUN,       .value.fun = sin                    },
    {.name = "cos",   .type = FUN,       .value.fun = cos                    },
    {.name = "log",   .type = FUN,       .value.fun = log                    },  // Logaritmo natural
    {.name = "exp",   .type = FUN,       .value.fun = exp                    },
    {.name = "E_0",   .type = CONST,     .value.var = M_E                    },
    {.name = "PI_0",  .type = CONST,     .value.var = M_PI                   },
    {.name = "LOAD",  .type = COMMAND,   .value.cmd = cmd_load               },
    {.name = "PRINT", .type = COMMAND,   .value.cmd = cmd_print              },
    {.name = "ECHO",  .type = COMMAND,   .value.cmd = cmd_echo               },
    {.name = "CLEAN",  .type = COMMAND,  .value.cmd = cmd_clean              },
    {.name = "CLEAR",  .type = COMMAND,  .value.cmd = cmd_clear              },
    {.name = "WORKSPACE",  .type = COMMAND,       .value.cmd = cmd_workspace },
    {.name = "QUIT",  .type = COMMAND,       .value.cmd = cmd_quit           },
    {.name = "HELP",  .type = COMMAND,       .value.cmd = cmd_help           },
    {.name = "IMPORT",  .type = COMMAND,       .value.cmd = cmd_import       },


};

TablaSimbolos* TS_crear() {
    TablaSimbolos* ts = (TablaSimbolos*)malloc(sizeof(TablaSimbolos));
    ts->arbol = ABB_crear();
    import_init();
    

    for (int i = 0; i < sizeof(predefined_symbols)/sizeof(predefined_symbols[0]); i++) {
        if (predefined_symbols[i].type == FUN)
        {
            symrec copy = predefined_symbols[i];
            copy.name = strdup(predefined_symbols[i].name);
            TS_insertar(ts, copy);
            continue;
        }
            
        TS_insertar(ts, predefined_symbols[i]);
    }

    return ts;
}

// Destruye la tabla y libera memoria
void TS_destruir(TablaSimbolos* ts) {
    if (ts) {
        ABB_destruir(ts->arbol);
        free(ts);
    }
}

// Inserta un símbolo en la tabla
void TS_insertar(TablaSimbolos* ts, symrec entrada) {
    ABB_insertar(ts->arbol, entrada);
}

// Busca un símbolo por nombre (devuelve NULL si no existe)
symrec* TS_buscar(TablaSimbolos* ts, const char* lexema) {
    return ABB_buscar(ts->arbol, lexema);
}

// Elimina el NODO pedido, pero no el nombre asociado (dato.name). Es responsabilidad del llamante liberarlo.
void TS_eliminar_elemento(TablaSimbolos* ts, const char *lexema)
{
    if (!ts || !lexema) return;
    ABB_eliminar(ts->arbol,lexema);
    
}


// Busca un símbolo y lo inserta como VAR con NaN si no existe
symrec* TS_buscar_o_insertar_var(TablaSimbolos* ts, const char* lexema) {
    symrec* s = TS_buscar(ts, lexema);
    if (!s) {
        symrec nueva = {
            .name = strdup(lexema),
            .type = VAR,
            .value.var = NAN
        };
        TS_insertar(ts, nueva);
        s = TS_buscar(ts, lexema);
    }
    return s;
}


// Imprime la tabla (para depuración)
void TS_imprimir(TablaSimbolos* ts) {
    if (!ts) return;
    ABB_imprimir(ts->arbol);
}

void TS_print_workspace(TablaSimbolos* ts) {
    if (!ts) return;
    ABB_imprimir_noindent(ts->arbol);
}