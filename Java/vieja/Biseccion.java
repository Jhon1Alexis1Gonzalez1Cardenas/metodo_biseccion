public class Biseccion {
    // Carga la librería nativa (sin extensión .dll/.so)
    static {
        System.loadLibrary("biselib_jni");
    }

    // Declaración del método nativo (corresponde a Ini_combro_bisec)
    public native ResultadoBiseccion resolver(double a, double b);

    // Clase para almacenar el resultado (similar a tu struct en C)
    public static class ResultadoBiseccion {
        public double raiz;
        public double fun1;
        public double fun2;
        public int iteraciones;
    }
}