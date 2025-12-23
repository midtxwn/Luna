%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>    // Para NAN
#include "TS.h"
#include "COMMONDEF.h"
#include "parser.tab.h"
#include "error.h"
#include "list.h"

int simulatedEndline = 0;
extern list_t* OPEN_FILES_FILENAMES;

void destroy_open_files()
{
    while(yyin != stdin)
    {
        fclose(yyin);
        yypop_buffer_state(); /* solo cerrar fichero y pop del estado si es un fichero proveniente del comando LOAD (no stdin) */
    }

}




void load_command_aux(FILE* newfile, char* filename)
{

    int found = 0;
    printf("[] LOADING FILE...\n");
    list_reset_iterator(OPEN_FILES_FILENAMES);
    while (list_has_next(OPEN_FILES_FILENAMES)) {
        char *searching = list_next(OPEN_FILES_FILENAMES);
        if (strcmp(filename, searching) == 0) {
            found = 1;
            break;
        }
    }
    
    if (found) {
        // Lanzar error si el archivo ya está abierto
        flerror("Error: El archivo ya está abierto");
        free(filename);
        fclose(newfile);
        return;
    }

    BEGIN(0); // devolvemos flex a condicion initial, porque abrir un archivo significa que no queremos estar en AFTER_NEWLINE
    
     // Agregar el nuevo nombre de archivo a la lista
    list_append(OPEN_FILES_FILENAMES, filename);


    yypush_buffer_state( yy_create_buffer ( newfile, YY_BUF_SIZE)  );

}


extern TablaSimbolos* tabla;  /* Tabla definida externamente */

%}



%option noyywrap
%option nounput
%option noinput

LETTER  [a-zA-Z]
DECIMAL_DIGIT   [[:digit:]]
DECIMAL_DIGITS  {DECIMAL_DIGIT}(_?{DECIMAL_DIGIT})*
ALNUM_STRING_ ({LETTER}|_)(({LETTER}|_)|{DECIMAL_DIGIT})*
DECIMAL_EXPONENT    [eE][+-]?{DECIMAL_DIGITS}
INT_LIT (0|[1-9](_?{DECIMAL_DIGITS})*)
FLOAT_LIT   ({DECIMAL_DIGITS}\.{DECIMAL_DIGITS}?{DECIMAL_EXPONENT}?|{DECIMAL_DIGITS}{DECIMAL_EXPONENT}|\.{DECIMAL_DIGITS}{DECIMAL_EXPONENT}?)
NUM_ ({FLOAT_LIT}|{INT_LIT})

ESCAPED_CHARS_TEXT (\\[abfnrtv\\'"])
STRING_LITERAL_ \"({ESCAPED_CHARS_TEXT}|[^\\"])*\"
%x AFTER_NEWLINE 
/* Esta condición de arranque permite eliminar restricciones en ficheros de carga (por ejemplo, sin esta condición no se podrían 
introducir instrucciones de LOAD en un fichero y no tener un \n o un ';' al final (es decir, tener EOF tras la instrucción de LOAD). Esencialmente,
esta condición provoca que el analizador sintáctico (flex) se comporte de manera diferente la primera vez que encuentra un EOF frente a la segunda vez
que se llama a yylex y se sigue teniendo EOF como siguiente componente*/

%%



[ \t]+   { /* Ignorar espacios */ }

\n       { return '\n'; }




<INITIAL><<EOF>>    { BEGIN(AFTER_NEWLINE); return '\n'; } 

<AFTER_NEWLINE><<EOF>>  { 
   
    BEGIN(INITIAL); 
    if(yyin != stdin)
    {
        fclose(yyin);
        yypop_buffer_state(); /* solo cerrar fichero y pop del estado si es un fichero proveniente del comando LOAD (no stdin) */

        // Eliminar el nombre del archivo actual de la lista
        char* last = list_last(OPEN_FILES_FILENAMES);
        if(last){
            list_delete(OPEN_FILES_FILENAMES, last);
            free(last);
        }
    
    }
    return '\n';
    
}

{STRING_LITERAL_} {
    yylval.STRING = strdup(yytext);
    return STRING;
}

{NUM_} {
    yylval.NUM = atof(yytext);
    return NUM;
}



{ALNUM_STRING_}    {
    symrec* s = TS_buscar_o_insertar_var(tabla, yytext); // copiará de yytext en caso de que no exista
    if (s->type == FUN) {
        yylval.FUN = s;   // Token FUN si es función
        return FUN;
    }
    else if (s->type == CONST) {
        yylval.CONST = s;   // Token CONST si es función
        return CONST;
    }
    else if (s->type == COMMAND) {
        yylval.COMMAND = s;   // Token COMMAND si es COMANDO
        return COMMAND;
    }
    else {
        yylval.VAR = s;   // Token VAR (insertado o existente)
        return VAR;
    }
}

"+"     { return '+'; }
"-"     { return '-'; }
"*"     { return '*'; }
"/"     { return '/'; }
"^"     { return '^'; }
"("     { return '('; }
")"     { return ')'; }
"="     { return '='; }
","     { return ','; }
";"+    { return '\n'; }

.       { flerror("Caracteres inválidos '%s'", yytext); }

%%

