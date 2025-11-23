/*
logica de la librera del metodo de bicecion con la funcion (x^2)cos(x)
versión 2.0
autor: Jhon Alexis Gonzalez Cardenas
fecha de inicio: 6 de noviembre de 2025
ultima modificacion: 7 de noviembre de 2025
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "biselib.h"

// codigo de las funciones

resultado_biseccion Ini_combro_bisec(double a, double b) // calculos y converciones iniciales para el metodo de biseccion y comprobar si es posible aplicar el metodo
{
    double x1 = a, x2 = b;
    resultado_biseccion resultado, error_res;
    error_res.raiz = NAN;
    error_res.iteraciones = -1;

    if (x2 > x1) // comprobar que x2 sea mayor que x1 para que se cumpla el orden
    {

        resultado.fun1 = funcion(x1);
        resultado.fun2 = funcion(x2);

        if (resultado.fun1 < 0 && resultado.fun2 > 0) // comprobar si hay cambio de signo en el intervalo al evaluarlo para asinar el negativo para x1 y positivo para x2
        {
            resultado = aplicar_bisec(x1, x2);
            return resultado;
        }
        else if (resultado.fun1 > 0 && resultado.fun2 < 0) // comprobar si hay cambio de signo en el intervalo al evaluarlo para asinar el positivo para x1 y negativo para x2
        {
            resultado = aplicar_bisec(x2, x1);
            return resultado;
        }
        else
        {
            return error_res; // no se puede aplicar el metodo de biseccion
        }
    }
    else
    {
        return error_res; // no se puede aplicar el metodo de biseccion
    }
}

resultado_biseccion aplicar_bisec(double neg, double pos) // calculo del metodo de biseccion
{
    double negativo = neg, positivo = pos, Pn, tore = TOLERANCIA;
    resultado_biseccion res;
    int cont = 0;

    do
    {
        Pn = (negativo + positivo) / 2;
        res.fun1 = funcion(Pn);

        if (res.fun1 < 0)
        {
            negativo = Pn;
        }
        else if (res.fun1 > 0)
        {
            positivo = Pn;
        }

        cont++;
    } while ((res.fun1 > tore || res.fun1 < -tore) && cont < 30);
    res.raiz = Pn;
    res.iteraciones = cont;
    return res;
}

double funcion(double x) // definicion de la funcion (x^2)cos(x)
{
    return (x * x) * cos(x);
}