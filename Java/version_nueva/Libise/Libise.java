package Libise;

import Libise.BiseLibJava.BiseLibJava;

//clase de prueba para la libreria de java

public class Libise {
    static {
        System.loadLibrary("biseLibJava");
    }

    public native String prueba();

    public static void main(String[] args) {

        BiseLibJava bise = new BiseLibJava();
        double[] resultado = bise.bisecionjava(1.0, 2.0, "(x^2)cos(x)");
        System.out.println("Resultado de la bisección: ");
        for (double val : resultado) {
            System.out.println(val);
        }
    }
}