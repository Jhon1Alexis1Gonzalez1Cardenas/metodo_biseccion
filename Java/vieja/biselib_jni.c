// biselib_jni.c
#include <jni.h>
#include "Biseccion.h" // El archivo generado por javac -h
#include "biselib.h"   // Tu librería original

JNIEXPORT jobject JNICALL Java_Biseccion_resolver(
    JNIEnv *env, jobject obj, jdouble a, jdouble b)
{

    // Llama a tu función original en C
    resultado_biseccion res_c = Ini_combro_bisec(a, b);

    // Busca la clase ResultadoBiseccion de Java
    jclass class_Resultado = (*env)->FindClass(env, "Biseccion$ResultadoBiseccion");

    // Crea un nuevo objeto de la clase ResultadoBiseccion
    jobject res_java = (*env)->AllocObject(env, class_Resultado);

    // Obtiene los IDs de los campos
    jfieldID fid_raiz = (*env)->GetFieldID(env, class_Resultado, "raiz", "D");
    jfieldID fid_fun1 = (*env)->GetFieldID(env, class_Resultado, "fun1", "D");
    jfieldID fid_fun2 = (*env)->GetFieldID(env, class_Resultado, "fun2", "D");
    jfieldID fid_iteraciones = (*env)->GetFieldID(env, class_Resultado, "iteraciones", "I");

    // Asigna los valores desde C a Java
    (*env)->SetDoubleField(env, res_java, fid_raiz, res_c.raiz);
    (*env)->SetDoubleField(env, res_java, fid_fun1, res_c.fun1);
    (*env)->SetDoubleField(env, res_java, fid_fun2, res_c.fun2);
    (*env)->SetIntField(env, res_java, fid_iteraciones, res_c.iteraciones);

    return res_java;
}