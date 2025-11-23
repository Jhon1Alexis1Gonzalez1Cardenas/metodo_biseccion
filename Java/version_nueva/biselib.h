/*
librera del metodo de bicecion con cualquier funcion matematica
versión 3.0
autor: Jhon Alexis Gonzalez Cardenas
fecha de inicio: 6 de noviembre de 2025
fecha de ultima modificacion: 20 de junio de 2026
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#ifndef BISELIB_H
#define BISELIB_H

// constantes
#define TOLERANCIA 0.000000000000001

// estructuras
typedef struct
{
    double raiz, fun1, fun2;
    int iteraciones, codigo;
} resultado_biseccion;

typedef struct
{
    double valor_duble; /* valor de la evaluación (si éxito) */
    int valor_int;      /* valor en int (cast de valor_duble) */
    int codigo;         /* 0 éxito, >0 error (ver documentación) */
} comprobar;

typedef enum
{
    TOK_NUMBER,
    TOK_VARIABLE,
    TOK_OPERATOR,
    TOK_FUNCTION,
    TOK_LPAREN,
    TOK_RPAREN
} TokenType;

typedef struct
{
    TokenType type;
    double value;   /* si number */
    char op;        /* si operator */
    char fname[16]; /* si function (sin, cos, ln, ...) */
} Token;

typedef struct
{
    Token *rpn; /* arreglo de tokens en RPN */
    size_t rpn_len;
    char varname; /* variable usada (ej 'x'), 0 si ninguna */
} Function;

// declaracion de funciones
resultado_biseccion Ini_combro_bisec(double a, double b, const char *funcion);
resultado_biseccion aplicar_bisec(double neg, double pos, const char *funcion);
static int is_ident_char(char c);
static void free_function(Function *f);
static int is_function_name(const char *s);
static int prec(char op);
static int is_right_assoc(char op);
static Token *tokenize_and_normalize(const char *s, size_t *out_len, char *detected_var, int *errcode);
static Token *infix_to_rpn(Token *in, size_t in_len, size_t *out_len, int *errcode);
static comprobar eval_rpn(Token *rpn, size_t rpn_len, char varname, double x);
Function *detectar_funcion(const char *expr);
comprobar evaluar_funcion(Function *f, double x);
comprobar evaluar_expresion(const char *expr, double x);
void liberar_funcion(Function *f); // para subirlo al repositorio

#endif /* BISELIB_H */