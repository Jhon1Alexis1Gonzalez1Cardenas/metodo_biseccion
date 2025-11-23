# Método de bisección — Resumen del repositorio

Breve
- Esta librería implementa el método de bisección para encontrar raíces de funciones de una variable. Hay dos líneas de trabajo: la versión "vieja" (específica para f(x) = (x^2)cos(x)) y la versión "nueva" (analizador de expresiones genérico).
- El punto medio usado en la iteración es $P_n = \frac{a+b}{2}$.

Estructura y qué es "viejo" / "actual"
- Versiones antiguas (2.0):
  - Implementación C específica: [C/version_vieja/biselib.c](C/version_vieja/biselib.c), [C/version_vieja/biselib.h](C/version_vieja/biselib.h).
  - JNI / Java antigua: [Java/vieja/biselib.c](Java/vieja/biselib.c), [Java/vieja/biselib_jni.c](Java/vieja/biselib_jni.c), y la clase Java [Java/vieja/Biseccion.java](Java/vieja/Biseccion.java).
- Versiones actuales (3.0):
  - Implementación C genérica (parser + RPN): [C/version_actual/biselib.c](C/version_actual/biselib.c), [C/version_actual/biselib.h](C/version_actual/biselib.h).
  - JNI / Java nueva: [Java/nueva/biselib.c](Java/nueva/biselib.c), interface JNI [Java/nueva/Libise/BiseLibJava/Libise_BiseLibJava_BiseLibJava.c](Java/nueva/Libise/BiseLibJava/Libise_BiseLibJava_BiseLibJava.c) y clases [Java/nueva/Libise/BiseLibJava/BiseLibJava.java](Java/nueva/Libise/BiseLibJava/BiseLibJava.java), [Java/nueva/Libise/Libise.java](Java/nueva/Libise/Libise.java).

API y símbolos importantes (referencias)
- Estructuras y tipos:
  - [`resultado_biseccion`](C/version_actual/biselib.h) — contenedor de resultado (raíz, iteraciones, código).
  - [`comprobar`](C/version_actual/biselib.h) — resultado de evaluación (valor + código).
- Funciones clave:
  - [`Ini_combro_bisec`](C/version_actual/biselib.c) — inicialización y comprobación del intervalo.
  - [`aplicar_bisec`](C/version_actual/biselib.c) — núcleo del método de bisección.
  - [`evaluar_expresion`](C/version_actual/biselib.c) — evalúa una expresión en un punto (parser + RPN).
  - JNI expuesta: el método nativo Java `bisecionjava` corresponde a la función en [Libise_BiseLibJava_BiseLibJava.c](Java/nueva/Libise/BiseLibJava/Libise_BiseLibJava_BiseLibJava.c) y a la declaración en [BiseLibJava.java](Java/nueva/Libise/BiseLibJava/BiseLibJava.java).

Uso del archivo de propiedades (`.properties`) para mensajes / errores
- Archivo en este repo: [errores.properties](errores.properties).
- Propósito: centralizar mensajes de error/traducciones y mapear códigos internos (`comprobar.codigo`, `resultado_biseccion.codigo`) a mensajes amigables.
- Recomendación por lenguaje:
  - Java: usar java.util.Properties
    - Colocar `errores.properties` en el classpath y cargar con `Properties.load(InputStream)`. Mapear la clave `ERROR_<n>` o `ERROR_DESCRIPTION_<n>` al código devuelto.
  - C: leer el fichero en tiempo de ejecución (parser simple)
    - Formato clave=valor: implementar función que busque la línea que comience con la clave (por ejemplo `ERROR_DESCRIPTION_2=`) y devuelva la cadena.
    - Alternativa: generar un header C a partir de `errores.properties` en el proceso de build para evitar parse en runtime.
- Ejemplo de claves usadas en este repo: `ERROR_0..ERROR_6`, `ERROR_DESCRIPTION_0..6`, `ERROR_HELP_1..6` (ver [errores.properties](errores.properties)).

Qué buscar o integrar en tu aplicación
- Para migrar uso viejo -> nuevo:
  - Reemplazar llamadas fijas (versión vieja que usa `funcion(x)`) por llamadas a [`Ini_combro_bisec`](C/version_actual/biselib.c) con la cadena de la función.
  - En Java, usar la clase [Libise.BiseLibJava.BiseLibJava](Java/nueva/Libise/BiseLibJava/BiseLibJava.java) que expone `bisecionjava`.
- Mensajes de error: siempre mapear `resultado.codigo` o `comprobar.codigo` con las claves en [errores.properties](errores.properties).

Archivos y referencias rápidas (abrir)
- C (actual):
  - [C/version_actual/biselib.h](C/version_actual/biselib.h)
  - [C/version_actual/biselib.c](C/version_actual/biselib.c)
- C (vieja):
  - [C/version_vieja/biselib.h](C/version_vieja/biselib.h)
  - [C/version_vieja/biselib.c](C/version_vieja/biselib.c)
- Java (nueva):
  - [Java/nueva/biselib.c](Java/nueva/biselib.c)
  - [Java/nueva/Libise/Libise.java](Java/nueva/Libise/Libise.java)
  - [Java/nueva/Libise/BiseLibJava/BiseLibJava.java](Java/nueva/Libise/BiseLibJava/BiseLibJava.java)
  - [Java/nueva/Libise/BiseLibJava/Libise_BiseLibJava_BiseLibJava.c](Java/nueva/Libise/BiseLibJava/Libise_BiseLibJava_BiseLibJava.c)
  - [Java/nueva/Libise/BiseLibJava/Libise_BiseLibJava_BiseLibJava.h](Java/nueva/Libise/BiseLibJava/Libise_BiseLibJava_BiseLibJava.h)
- Java (vieja):
  - [Java/vieja/biselib.c](Java/vieja/biselib.c)
  - [Java/vieja/biselib_jni.c](Java/vieja/biselib_jni.c)
  - [Java/vieja/Biseccion.java](Java/vieja/Biseccion.java)
  - [Java/vieja/Biseccion.h](Java/vieja/Biseccion.h)
- Mensajes / propiedades:
  - [errores.properties](errores.properties)

Si quieres, puedo:
- Añadir un ejemplo corto de código en Java para cargar [errores.properties](errores.properties) y mapear códigos.
- Generar un pequeño parser C para `errores.properties` o convertirlo a `errores.h`.

Símbolos referenciados
- [`resultado_biseccion`](C/version_actual/biselib.h)
- [`comprobar`](C/version_actual/biselib.h)
- [`Ini_combro_bisec`](C/version_actual/biselib.c)
- [`aplicar_bisec`](C/version_actual/biselib.c)
- [`evaluar_expresion`](C/version_actual/biselib.c)
- Java nativo: [`bisecionjava`](Java/nueva/Libise/BiseLibJava/BiseLibJava.java) / [`Java_Libise_BiseLibJava_BiseLibJava_bisecionjava`](Java/nueva/Libise/BiseLibJava/Libise_BiseLibJava_BiseLibJava.c)