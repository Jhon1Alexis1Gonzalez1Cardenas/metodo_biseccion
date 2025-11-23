/*
archivo cabecera de la libreria del metodo de biseccion con la funcion (x^2)cos(x)
versión 2.0
autor: Jhon Alexis Gonzalez Cardenas
fecha de inicio: 6 de noviembre de 2025
ultima modificacion: 7 de noviembre de 2025
*/
#ifndef BISELIB_H
#define BISELIB_H

// constantes
#define TOLERANCIA 0.000000000000001

// estructuras
typedef struct
{
    double raiz, fun1, fun2;
    int iteraciones;
} resultado_biseccion;

// declaracion de funciones
resultado_biseccion Ini_combro_bisec(double a, double b);
resultado_biseccion aplicar_bisec(double neg, double pos);
double funcion(double x);

#endif // BISELIB_H
