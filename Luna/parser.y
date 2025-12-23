%{
  #include "COMMONDEF.h"   /* Contains definition of 'symrec'. */
  #include <stdio.h>       /* For printf, etc. */
  #include <math.h>        /* For pow, used in the grammar. */
  #include "list.h"        /* For list_t to store command parameters */
  #include <stdlib.h>
  #include "TS.h"
  #include "importhandler.h"
  #include "error.h"

  extern int verbose;
  int yylex (void);
  extern TablaSimbolos* tabla;  /* Tabla definida externamente */

 void yyerror(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    _yyerror(fmt, args);
    va_end(args);
}
  

void onUndefinedVar(symrec* var)
{
  yyerror("Variable no definida: %s", var->name);
  char* toFree = var->name;
  TS_eliminar_elemento(tabla, var->name);
  free( toFree ); /*IMPORTANTE liberar el nombre tras el elemento (en el arbol) puesto que el árbol necesita leer el char* */
}

double dispatch_function(symrec* s, list_t* args) {
    if (!s) {
        yyerror("Error: Invalid symbol record\n");
        return NAN;
    }
    
    int num_args = args ? list_get_length(args) : 0;
    double result;

    // Validate number of arguments (0 to 3 supported)
    if (num_args < 0 || num_args > 3) {
        yyerror("Error: Unsupported number of arguments: %d\n", num_args);
        return NAN;
    }

    // Dispatch based on number of arguments using switch
    switch (num_args) {
        case 0: {
            double (*func)(void) = (double (*)(void))s->value.fun;
            result = func();
            break;
        }
        case 1: {
            list_reset_iterator(args);
            double* arg1_ptr = (double*)list_next(args);
            if (!arg1_ptr) {
                yyerror("Invalid argument\n"); 
                return NAN;
            }
            double arg1 = *arg1_ptr;
            double (*func)(double) = (double (*)(double))s->value.fun;
            result = func(arg1);
            break;
        }
        case 2: {
            list_reset_iterator(args);
            double* arg1_ptr = (double*)list_next(args);
            double* arg2_ptr = (double*)list_next(args);
            if (!arg1_ptr || !arg2_ptr) {
                yyerror("Invalid arguments\n"); 
                return NAN;
            }
            double arg1 = *arg1_ptr;
            double arg2 = *arg2_ptr;
            double (*func)(double, double) = (double (*)(double, double))s->value.fun;
            result = func(arg1, arg2);
            break;
        }
        case 3: {
            list_reset_iterator(args);
            double* arg1_ptr = (double*)list_next(args);
            double* arg2_ptr = (double*)list_next(args);
            double* arg3_ptr = (double*)list_next(args);
            if (!arg1_ptr || !arg2_ptr || !arg3_ptr) {
                yyerror("Invalid arguments\n"); 
                return NAN;
            }
            double arg1 = *arg1_ptr;
            double arg2 = *arg2_ptr;
            double arg3 = *arg3_ptr;
            double (*func)(double, double, double) = (double (*)(double, double, double))s->value.fun;
            result = func(arg1, arg2, arg3);
            break;
        }
    }

    // Clean up the argument list
    if (args) {
        list_reset_iterator(args);
        while (list_has_next(args)) {
            free(list_next(args));
        }
        list_destroy(args);
    }

    return result;
}




%}


%define api.value.type union

%token <double> NUM
%token <symrec*> VAR CONST FUN COMMAND
%token <char*> STRING
%nterm <double> exp
%nterm <list_t*> string_list
%nterm <list_t*> ARGS
%nterm <list_t*> NONVOID_ARGS

%destructor {free($$);} STRING
%destructor {if(isnan($$->value.var)) onUndefinedVar($$); } VAR

%precedence '='
%left '-' '+'
%left '*' '/'
%precedence NEG /* negation--unary minus */
%right '^'      /* exponentiation */

%% /* The grammar follows. */

input:
  %empty
| input line

line:
  '\n'
| stmt '\n' {printf(">>");}
| error '\n' { yyerrok; }
;

stmt:
  exp { if(verbose) printf("%.10g\n", $1); }
| command
;

command:
  COMMAND string_list { $1->value.cmd($2); }
;

string_list:
  %empty { $$ = list_create(); }
| string_list STRING { list_append($1, $2); $$ = $1; }
;

NONVOID_ARGS:
exp { 
                            $$ = list_create(); 
                            double* arg = malloc(sizeof(double)); 
                            *arg = $1; 
                            list_append($$, arg); 
                          }
| NONVOID_ARGS ',' exp { 
                            double* arg = malloc(sizeof(double)); 
                            *arg = $3; 
                            list_append($1, arg); 
                            $$ = $1; 
                          }
;

ARGS:
%empty  { $$ = 0; }
| NONVOID_ARGS { $$ = $1; }
;

exp:
  NUM
| VAR                {
      if (isnan($1->value.var)) {
          onUndefinedVar($1); 
          YYERROR;
      }
      $$ = $1->value.var;
  }
| CONST              {
      if (isnan($1->value.var)) {
          yyerror("Constante no definida: %s", $1->name);
          YYERROR;
      }
      $$ = $1->value.var;
  }
| CONST '=' exp      { yyerror("Asignación de una constante: %s", $1->name); YYERROR; }
| FUN '=' exp        { yyerror("Asignación de una funcion: %s", $1->name); YYERROR; }
| VAR '=' exp        { $$ = $3; $1->value.var = $3; }
| FUN '(' ARGS ')'   { $$ = dispatch_function($1, $3); }
| VAR '(' ARGS ')'   { 
    symrec* sym = TS_buscar(tabla, $1->name);
    
    if (!sym || (sym && sym->type == VAR) ) {
        /* intentar resolver de librerias */
        symrec symstruct;
        symstruct = import_resolve_symbol($1->name);
        
        if (symstruct.type == FUN) {
            /* insertar simbolo resuelto en TS */
            TS_eliminar_elemento(tabla, $1->name); /* NO liberamos nombre */

            TS_insertar(tabla, symstruct);
            $$ = dispatch_function(&symstruct, $3);
        } else {
            // Cleanup arguments before error
            if ($3) {
                list_reset_iterator($3);
                while (list_has_next($3)) {
                    free(list_next($3));
                }
                list_destroy($3);
            }

            char* toFree = $1->name;
            TS_eliminar_elemento(tabla, $1->name); /* NO liberamos nombre */
            yyerror("Undefined function: %s", toFree);
            free(toFree);
            YYERROR;
        }
    }
    else if ((sym->type == VAR && !isnan(sym->value.var)) || sym->type != VAR) {
        // Cleanup arguments before error
        if ($3) {
            list_reset_iterator($3);
            while (list_has_next($3)) {
                free(list_next($3));
            }
            list_destroy($3);
        }
        yyerror("Symbol is not a function: %s", $1->name);
        YYERROR;
    }
    else {
        // Existing function found (case shouldnt happen since it should be func already)
        $$ = dispatch_function(sym, $3);
    }
}
| exp '+' exp        { $$ = $1 + $3; }
| exp '-' exp        { $$ = $1 - $3; }
| exp '*' exp        { $$ = $1 * $3; }
| exp '/' exp        { 
                      if($3 == 0)
                      {
                        yyerror("Division by zero");
                        YYERROR;
                      }
                      $$ = $1 / $3; }
| '-' exp %prec NEG  { $$ = -$2; }
| exp '^' exp        { $$ = pow($1, $3); }
| '(' exp ')'        { $$ = $2; }
;

%%