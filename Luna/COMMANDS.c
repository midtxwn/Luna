#include "COMMANDS.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TS.h"
#include "importhandler.h"
#include "error.h"




extern void load_command_aux(FILE* newFile, char* fileName); // necesario que la funcion este en el .c de flex para poder acceder al valor de definiciones preprocesadas como YY_BUFF_SIZE
// Variable global para la tabla de símbolos
extern TablaSimbolos* tabla;
extern void destroy_open_files();
extern int yylex_destroy( void );
list_t* OPEN_FILES_FILENAMES;
// Variable para controlar la verbosidad del parser
int verbose = 1;



char* _processString(char* string) // necesario para eliminar de forma efectiva el " inicial y final
{
    int len = strlen(string);
    if(len>0)
    {
        string[len-1] = '\0';
    }
    return string + 1;
}
//TODO liberar texto manualmente odnde se hace list destroy

// Comando LOAD "filename"
// Abre el archivo y cambia yyin para analizarlo
void cmd_load(list_t* params) {
    // Comprobar que hay exactamente 1 parámetro
    if (list_get_length(params) != 1) {
        //: Lanzar error - "LOAD requiere exactamente 1 parámetro"
        commanderror("Bad parameters to LOAD: Expected 1 parameter");
        list_destroy(params);
        return;
    }

    // Obtener el nombre del archivo
    list_reset_iterator(params);
    char* filename_with_quotes = (char*)list_next(params);
    char* filename = filename_with_quotes + 1;  // Saltar la comilla inicial
    filename[strlen(filename) - 1] = '\0';      // Reemplazar comilla final con \0

    // Abrir el archivo
    FILE* file = fopen(filename, "r");
    if (!file) {
        // : Lanzar error - "No se pudo abrir el archivo"
        commanderror("File not found: %s" , filename);
        free(filename_with_quotes);  // Liberar la cadena original
        list_destroy(params);
        return;
    }
    
    load_command_aux(file, filename_with_quotes);

    
    //free(filename_with_quotes);  // Liberar la cadena original
    list_destroy(params);
}

// Comando ECHO "on"
// Activa la verbosidad del parser
void cmd_echo(list_t* params) {
    // Comprobar que hay exactamente 1 parámetro
    if (list_get_length(params) != 1) {
        // : Lanzar error - "ECHO requiere exactamente 1 parámetro"
        commanderror("ECHO requires exactly 1 parameter");
        list_destroy(params);
        return;
    }

    // Obtener el parámetro
    list_reset_iterator(params);
    char* arg_with_quotes = (char*)list_next(params);
    char* arg = arg_with_quotes + 1;  // Saltar comilla inicial
    arg[strlen(arg) - 1] = '\0';      // Reemplazar comilla final con \0

    // Comprobar si es "on"
    if (strcmp(arg, "on") == 0) {
        verbose = 1;
    
    } else {
        verbose = 0;
    }

    free(arg_with_quotes);  // Liberar la cadena original
    list_destroy(params);
}

// Comando PRINT "VARNAME"
// Busca una variable en la tabla de símbolos e imprime su valor
void cmd_print(list_t* params) {
    // Comprobar que hay exactamente 1 parámetro
    if (list_get_length(params) != 1) {
        // : Lanzar error - "PRINT requiere exactamente 1 parámetro"
        commanderror("PRINT Command requires exactly 1 parameter");
        list_destroy(params);
        return;
    }

    // Obtener el nombre de la variable
    list_reset_iterator(params);
    char* varname_with_quotes = (char*)list_current(params);
 

    // Buscar en la tabla de símbolos
    symrec* var = TS_buscar(tabla, _processString(varname_with_quotes));
    if (var && !isnan(var->value.var)) {
        printf("%.10g\n", var->value.var);
    } else {
        //: Lanzar error - "Variable no encontrada o no definida"
        commanderror("Attempt to use undefined VAR");
    }

    free(varname_with_quotes);  // Liberar la cadena original
    list_destroy(params);
}

// Comando QUIT
// Termina el programa
void cmd_quit(list_t* params) {
    // Comprobar que no hay parámetros
    if (list_get_length(params) != 0) {
        // : Lanzar error - "QUIT no acepta parámetros"
        commanderror("QUIT Does not accept any parameters.");
        list_destroy(params);
        return;
    }


    destroy_open_files();
    TS_destruir(tabla);
    import_cleanup();
    list_destroy(params);
    list_destroy(OPEN_FILES_FILENAMES);
    yylex_destroy();
    exit(0);  // Salir del programa, termina el análisis
}

void cmd_import(list_t* params) {
    if (list_get_length(params) != 1) {
        commanderror( "IMPORT requires exactly 1 parameter"); //   error
        list_destroy(params);
        return;
    }

    list_reset_iterator(params);
    char* libname_with_quotes = (char*)list_current(params);
    char* libname = _processString(libname_with_quotes);  // Remove quotes
    
    if (!import_library(libname)) {
        commanderror("Failed to import library: %s\n", libname);
    } else
        printf("Successfully imported library\n");
    free(libname_with_quotes);
    list_destroy(params);
}


// Comando WORKSPACE
// Imprime la tabla de símbolos
void cmd_workspace(list_t* params) {
    // Comprobar que no hay parámetros
    if (list_get_length(params) != 0) {
        //: Lanzar error - "WORKSPACE no acepta parámetros"
        commanderror("WORKSPACE does not accept any parameters");
        list_destroy(params);
        return;
    }

    TS_print_workspace(tabla);
    list_destroy(params);
}

// Comando CLEAR
// Destruye la tabla de símbolos y crea una nueva
void cmd_clear(list_t* params) {
    // Comprobar que no hay parámetros
    int paramLength = list_get_length(params);
    if (paramLength > 1 || paramLength < 0) {
        //: Lanzar error - "CLEAR no acepta parámetros"
        commanderror("CLEAR does not accept any parameters");
        list_destroy(params);
        return;
    }

    if(paramLength == 0)
    {
        TS_destruir(tabla);      // Destruir la tabla actual
        tabla = TS_crear();      // Crear una nueva tabla
    }
    else
    {
        list_reset_iterator(params);
        char* oString = list_current(params);
        TS_eliminar_elemento(tabla,_processString(oString));
        if(oString) free(oString);
    }
    list_destroy(params);
}

// Comando CLEAN
// Limpia la terminal
void cmd_clean(list_t* params) {
    // Comprobar que no hay parámetros
    if (list_get_length(params) != 0) {
        // : Lanzar error - "CLEAN no acepta parámetros"
        commanderror("CLEAN does not accept any parameters");
        list_destroy(params);
        return;
    }

    system("clear");  // Para sistemas UNIX
    list_destroy(params);
}

// Comando HELP
void cmd_help(list_t* params) {
    int param_count = list_get_length(params);
    char* requested_cmd = NULL;

    // verificar cantidad de parámetros
    if (param_count > 1) {
        commanderror("HELP takes maximum 1 parameter");
        
        // liberar todos los parámetros
        list_reset_iterator(params);
        while (list_has_next(params)) {
            char* s = (char*)list_next(params);
            free(s);
        }
        list_destroy(params);
        return;
    }

    // obtener parámetro si existe
    if (param_count == 1) {
        list_reset_iterator(params);
        requested_cmd = (char*)list_next(params);
    }

    // mostrar ayuda específica o general
    if (requested_cmd) {
        if (strcasecmp(requested_cmd, "\"LOAD\"") == 0) {
            printf("\nLOAD \"filename\"\n");
            printf("  Loads and executes a script file\n");
            printf("  Example: LOAD \"example.txt\"\n");
        }
        else if (strcasecmp(requested_cmd, "\"ECHO\"") == 0) {
            printf("\nECHO \"on|off\"\n");
            printf("  Controls interpreter verbose output\n");
            printf("  Example: ECHO \"on\" to enable verbose mode\n");
        }
        else if (strcasecmp(requested_cmd, "\"PRINT\"") == 0) {
            printf("\nPRINT \"variable_name\"\n");
            printf("  Displays a variable's value or a constant's value\n");
            printf("  Example: PRINT \"PI_0\" → 3.1416\n");
        }
        else if (strcasecmp(requested_cmd, "\"QUIT\"") == 0) {
            printf("\nQUIT\n");
            printf("  Terminates the interpreter\n");
        }
        else if (strcasecmp(requested_cmd, "\"CLEAR\"") == 0) {
            printf("\nCLEAR\n");
            printf("  Resets the symbol table (clears all variables)\n");
        }
        else if (strcasecmp(requested_cmd, "\"IMPORT\"") == 0) {
            printf("\nIMPORT \"libraryname\"\n");
            printf("  Loads functions from a dynamic library (.so/.dll)\n");
            printf("  Example: IMPORT \"./mylib.so\"\n");

        }
        else if (strcasecmp(requested_cmd, "\"HELP\"") == 0) {
            printf("\nHELP [\"command\"]\n");
            printf("  Shows general help or specific command help\n");
            printf("  Example: HELP \"LOAD\" shows detailed help\n");
        }
        else {
            commanderror("Unknown command: %s", requested_cmd);
        }
        free(requested_cmd);
    } else {
        // ayuda general
        printf("\nAvailable commands (use HELP \"command\" for details):\n");
        printf("─────────────────────────────────────────────────────\n");
        printf("  LOAD     Load script file\n");
        printf("  ECHO     Control verbose mode\n");
        printf("  PRINT    Display variable value\n"); 
        printf("  QUIT     Exit interpreter\n");
        printf("  WORKSPACE  Show symbol table\n");
        printf("  CLEAR    Reset symbol table\n");
        printf("  CLEAN    Clear screen\n");
        printf("  IMPORT   Load functions from dynamic library\n");
        printf("  HELP     Show this help message\n");
        printf("\nExamples:\n");
        printf("  IMPORT \"mathlib.so\"  Load external functions\n");
        printf("  HELP \"IMPORT\"        Show import command details\n");
        printf("  PRINT \"result\"       Display variable value\n");
    }

    // liberar recursos de la lista evitando double-free
    list_reset_iterator(params);
    while (list_has_next(params)) {
        char* s = (char*)list_next(params);
        // solo liberar si no es el comando solicitado (ya liberado)
        if (s != requested_cmd) {
            free(s);
        }
    }
    list_destroy(params);
    
}