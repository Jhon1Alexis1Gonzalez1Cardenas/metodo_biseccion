
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "Libise_BiseLibJava_BiseLibJava.h"

/*
 * Class:     Libise_BiseLibJava_BiseLibJava
 * Method:    bisecionjava
 * Signature: (DDLjava/lang/String;)[D
 */
JNIEXPORT jdoubleArray JNICALL Java_Libise_BiseLibJava_BiseLibJava_bisecionjava(JNIEnv *env, jobject obj1, jdouble x1, jdouble x2, jstring func)
{
  jchar *strchars = (*env)->GetStringChars(env, func, NULL);
  if (strchars == NULL)
  {
    return NULL; // error al obtener los caracteres de la cadena
  }

  jsize len = (*env)->GetStringLength(env, func);

  jchar *buffer = (jchar *)malloc((len + 1) * sizeof(jchar));
  if (buffer == NULL)
  {
    (*env)->ReleaseStringChars(env, func, strchars);
    return NULL; // error de asignacion de memoria
  }

  memcpy(buffer, strchars, len * sizeof(jchar));
  buffer[len] = '\0'; // null-terminate the string

  double a = (double)x1, b = (double)x2;
  const char *function_str = (*env)->GetStringUTFChars(env, func, NULL);
  if (function_str == NULL)
  {
    return NULL; // OOM
  }

  resultado_biseccion resultado = Ini_combro_bisec(a, b, function_str);

  jdoubleArray arrayOut;
  arrayOut = (*env)->NewDoubleArray(env, 3);

  if (arrayOut == NULL)
  {
    return NULL; // error al crear el array
  }

  jdouble arrayAux[3];
  arrayAux[0] = (jdouble)resultado.raiz;
  arrayAux[1] = (jdouble)resultado.iteraciones;
  arrayAux[2] = (jdouble)resultado.codigo;

  (*env)->SetDoubleArrayRegion(env, arrayOut, 0, 3, arrayAux);

  free(buffer);
  (*env)->ReleaseStringChars(env, func, strchars);
  (*env)->ReleaseStringUTFChars(env, func, function_str);

  return arrayOut;
}
// codigo de las funciones

resultado_biseccion Ini_combro_bisec(double a, double b, const char *funcion) // calculos y converciones iniciales para el metodo de biseccion y comprobar si es posible aplicar el metodo
{
  double x1 = a, x2 = b;
  resultado_biseccion resultado, error_res;
  comprobar res;
  error_res.raiz = NAN;
  error_res.iteraciones = -1;
  res = evaluar_expresion(funcion, 1);

  if (res.codigo == 0)
  {

    if (x2 > x1) // comprobar que x2 sea mayor que x1 para que se cumpla el orden
    {
      res = evaluar_expresion(funcion, x1);
      resultado.fun1 = res.valor_duble;
      res = evaluar_expresion(funcion, x2);
      resultado.fun2 = res.valor_duble;

      if (resultado.fun1 < 0 && resultado.fun2 > 0) // comprobar si hay cambio de signo en el intervalo al evaluarlo para asinar el negativo para x1 y positivo para x2
      {
        resultado = aplicar_bisec(x1, x2, funcion);
        resultado.codigo = 0;
        return resultado;
      }
      else if (resultado.fun1 > 0 && resultado.fun2 < 0) // comprobar si hay cambio de signo en el intervalo al evaluarlo para asinar el positivo para x1 y negativo para x2
      {
        resultado = aplicar_bisec(x2, x1, funcion);
        resultado.codigo = 0;
        return resultado;
      }
      else
      {
        error_res.codigo = 5;
        return error_res; // no se puede aplicar el metodo de biseccion ambos valores son del mismo signo
      }
    }
    else
    {
      error_res.codigo = 6;
      return error_res; // no se puede aplicar el metodo de biseccion el valor de x2 no es mayor que x1
    }
  }
  else
  {
    error_res.codigo = res.codigo;
    return error_res; // error en la evaluacion de la funcion
  }
}

resultado_biseccion aplicar_bisec(double neg, double pos, const char *funcion) // metodo de biseccion
{
  double negativo = neg, positivo = pos, Pn, tore = TOLERANCIA;
  const char *fx = funcion;
  resultado_biseccion res;
  comprobar res_eval;
  int cont = 0;

  do
  {
    Pn = (negativo + positivo) / 2;
    res_eval = evaluar_expresion(fx, Pn);
    res.fun1 = res_eval.valor_duble;

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

static int is_ident_char(char c)
{
  return isalpha((unsigned char)c);
}

static void free_function(Function *f)
{
  if (!f)
    return;
  free(f->rpn);
  free(f);
}

/* Map function name to a code; returns 1 if recognized, 0 otherwise */
static int is_function_name(const char *s)
{
  /* supported: sin cos tan asin acos atan exp ln log sqrt cbrt */
  if (!s || !*s)
    return 0;
  if (strcmp(s, "sin") == 0)
    return 1;
  if (strcmp(s, "cos") == 0)
    return 1;
  if (strcmp(s, "tan") == 0)
    return 1;
  if (strcmp(s, "asin") == 0)
    return 1;
  if (strcmp(s, "acos") == 0)
    return 1;
  if (strcmp(s, "atan") == 0)
    return 1;
  if (strcmp(s, "exp") == 0)
    return 1;
  if (strcmp(s, "ln") == 0)
    return 1; /* natural log */
  if (strcmp(s, "log") == 0)
    return 1; /* log base 10 */
  if (strcmp(s, "sqrt") == 0)
    return 1;
  if (strcmp(s, "cbrt") == 0)
    return 1;
  return 0;
}

/* precedence for operators */
static int prec(char op)
{
  switch (op)
  {
  case '+':
  case '-':
    return 1;
  case '*':
  case '/':
    return 2;
  case '^':
    return 4; /* right-assoc */
  default:
    return 0;
  }
}

/* right associative? only '^' is right-assoc here */
static int is_right_assoc(char op)
{
  return op == '^';
}

static Token *tokenize_and_normalize(const char *s, size_t *out_len, char *detected_var, int *errcode)
{
  size_t cap = 64, len = 0;
  Token *arr = (Token *)malloc(cap * sizeof(Token));
  if (!arr)
  {
    *errcode = 4;
    return NULL;
  }
  *detected_var = 0;
  *errcode = 0;
  const char *p = s;
  const char *last_end = s; /* puntero al fin del token anterior (para detectar adyacencia) */
  Token prev = {0};
  int have_prev = 0;

  while (*p)
  {
    if (isspace((unsigned char)*p))
    {
      p++;
      continue;
    }

    /* Guardamos el inicio del posible token actual */
    const char *p_start = p;

    /* digit or dot => number */
    if (isdigit((unsigned char)*p) || (*p == '.' && isdigit((unsigned char)*(p + 1))))
    {
      /* Si el token actual empieza exactamente donde terminó el anterior
         y el anterior era un número, consideramos esto un número mal formado
         (ej "2.3.4" -> error) en lugar de insertar multiplicación implícita. */
      if (have_prev && prev.type == TOK_NUMBER && last_end == p_start)
      {
        *errcode = 1; /* parse error */
        free(arr);
        return NULL;
      }

      char *end;
      double val = strtod(p, &end);
      p = end;
      Token t;
      memset(&t, 0, sizeof(t));
      t.type = TOK_NUMBER;
      t.value = val;
      /* implicit multiplication: prev is number/variable/rparen and next is variable/function/lparen */
      if (have_prev && (prev.type == TOK_VARIABLE || prev.type == TOK_RPAREN))
      {
        Token mul;
        memset(&mul, 0, sizeof(mul));
        mul.type = TOK_OPERATOR;
        mul.op = '*';
        if (len + 1 >= cap)
        {
          cap *= 2;
          arr = realloc(arr, cap * sizeof(Token));
          if (!arr)
          {
            *errcode = 4;
            return NULL;
          }
        }
        arr[len++] = mul;
      }
      if (len + 1 >= cap)
      {
        cap *= 2;
        arr = realloc(arr, cap * sizeof(Token));
        if (!arr)
        {
          *errcode = 4;
          return NULL;
        }
      }
      arr[len++] = t;
      prev = t;
      have_prev = 1;
      last_end = p;
      continue;
    }

    /* unicode sqrt char? U+221A: bytes in UTF-8 0xE2 0x88 0x9A */
    if ((unsigned char)*p == 0xE2 && (unsigned char)*(p + 1) == 0x88 && (unsigned char)*(p + 2) == 0x9A)
    {
      /* treat as function sqrt */
      Token t;
      memset(&t, 0, sizeof(t));
      t.type = TOK_FUNCTION;
      strcpy(t.fname, "sqrt");
      /* implicit multiplication handling as above */
      if (have_prev && (prev.type == TOK_NUMBER || prev.type == TOK_VARIABLE || prev.type == TOK_RPAREN))
      {
        Token mul;
        memset(&mul, 0, sizeof(mul));
        mul.type = TOK_OPERATOR;
        mul.op = '*';
        if (len + 1 >= cap)
        {
          cap *= 2;
          arr = realloc(arr, cap * sizeof(Token));
          if (!arr)
          {
            *errcode = 4;
            return NULL;
          }
        }
        arr[len++] = mul;
      }
      if (len + 1 >= cap)
      {
        cap *= 2;
        arr = realloc(arr, cap * sizeof(Token));
        if (!arr)
        {
          *errcode = 4;
          return NULL;
        }
      }
      arr[len++] = t;
      prev = t;
      have_prev = 1;
      p += 3;
      last_end = p;
      continue;
    }

    /* letter => variable or function identifier */
    if (is_ident_char(*p))
    {
      char id[32];
      int i = 0;
      while (is_ident_char(*p) && i < (int)sizeof(id) - 1)
      {
        id[i++] = (char)tolower((unsigned char)*p);
        p++;
      }
      id[i] = 0;
      /* check if this is a single-letter variable */
      if (strlen(id) == 1)
      {
        Token t;
        memset(&t, 0, sizeof(t));
        t.type = TOK_VARIABLE;
        /* check multiple variables */
        if (*detected_var == 0)
          *detected_var = id[0];
        else if (*detected_var != id[0])
        {
          *errcode = 3;
          free(arr);
          return NULL;
        }
        /* implicit multiplication */
        if (have_prev && (prev.type == TOK_NUMBER || prev.type == TOK_VARIABLE || prev.type == TOK_RPAREN))
        {
          Token mul;
          memset(&mul, 0, sizeof(mul));
          mul.type = TOK_OPERATOR;
          mul.op = '*';
          if (len + 1 >= cap)
          {
            cap *= 2;
            arr = realloc(arr, cap * sizeof(Token));
            if (!arr)
            {
              *errcode = 4;
              return NULL;
            }
          }
          arr[len++] = mul;
        }
        if (len + 1 >= cap)
        {
          cap *= 2;
          arr = realloc(arr, cap * sizeof(Token));
          if (!arr)
          {
            *errcode = 4;
            return NULL;
          }
        }
        arr[len++] = t;
        prev = t;
        have_prev = 1;
      }
      else
      {
        /* function name */
        if (!is_function_name(id))
        {
          /* unknown function considered as error */
          *errcode = 1;
          free(arr);
          return NULL;
        }
        Token t;
        memset(&t, 0, sizeof(t));
        t.type = TOK_FUNCTION;
        strncpy(t.fname, id, sizeof(t.fname) - 1);
        /* implicit multiplication */
        if (have_prev && (prev.type == TOK_NUMBER || prev.type == TOK_VARIABLE || prev.type == TOK_RPAREN))
        {
          Token mul;
          memset(&mul, 0, sizeof(mul));
          mul.type = TOK_OPERATOR;
          mul.op = '*';
          if (len + 1 >= cap)
          {
            cap *= 2;
            arr = realloc(arr, cap * sizeof(Token));
            if (!arr)
            {
              *errcode = 4;
              return NULL;
            }
          }
          arr[len++] = mul;
        }
        if (len + 1 >= cap)
        {
          cap *= 2;
          arr = realloc(arr, cap * sizeof(Token));
          if (!arr)
          {
            *errcode = 4;
            return NULL;
          }
        }
        arr[len++] = t;
        prev = t;
        have_prev = 1;
      }
      last_end = p;
      continue;
    }

    /* parentheses and operators */
    if (*p == '(')
    {
      Token t;
      memset(&t, 0, sizeof(t));
      t.type = TOK_LPAREN;
      /* implicit multiplication: e.g., 2( -> 2 * ( */
      if (have_prev && (prev.type == TOK_NUMBER || prev.type == TOK_VARIABLE || prev.type == TOK_RPAREN))
      {
        Token mul;
        memset(&mul, 0, sizeof(mul));
        mul.type = TOK_OPERATOR;
        mul.op = '*';
        if (len + 1 >= cap)
        {
          cap *= 2;
          arr = realloc(arr, cap * sizeof(Token));
          if (!arr)
          {
            *errcode = 4;
            return NULL;
          }
        }
        arr[len++] = mul;
      }
      if (len + 1 >= cap)
      {
        cap *= 2;
        arr = realloc(arr, cap * sizeof(Token));
        if (!arr)
        {
          *errcode = 4;
          return NULL;
        }
      }
      arr[len++] = t;
      prev = t;
      have_prev = 1;
      p++;
      last_end = p;
      continue;
    }
    if (*p == ')')
    {
      Token t;
      memset(&t, 0, sizeof(t));
      t.type = TOK_RPAREN;
      if (len + 1 >= cap)
      {
        cap *= 2;
        arr = realloc(arr, cap * sizeof(Token));
        if (!arr)
        {
          *errcode = 4;
          return NULL;
        }
      }
      arr[len++] = t;
      prev = t;
      have_prev = 1;
      p++;
      last_end = p;
      continue;
    }
    /* operators + - * / ^ */
    if (strchr("+-*/^", *p))
    {
      Token t;
      memset(&t, 0, sizeof(t));
      t.type = TOK_OPERATOR;
      t.op = *p;
      /* handle unary minus: if at start or after operator or after lparen, convert to unary by using 'u' char */
      if (t.op == '-')
      {
        if (!have_prev || (have_prev && (prev.type == TOK_OPERATOR || prev.type == TOK_LPAREN || prev.type == TOK_FUNCTION)))
        {
          t.op = 'u'; /* unary minus */
        }
      }
      if (len + 1 >= cap)
      {
        cap *= 2;
        arr = realloc(arr, cap * sizeof(Token));
        if (!arr)
        {
          *errcode = 4;
          return NULL;
        }
      }
      arr[len++] = t;
      prev = t;
      have_prev = 1;
      p++;
      last_end = p;
      continue;
    }

    /* caret (^) handled above; sqrt symbol handled; unknown char => error */
    *errcode = 1;
    free(arr);
    return NULL;
  }

  *out_len = len;
  return arr;
}

/* --- Shunting-yard: convert infix tokens to RPN tokens --- */
static Token *infix_to_rpn(Token *in, size_t in_len, size_t *out_len, int *errcode)
{
  Token *output = (Token *)malloc((in_len + 32) * sizeof(Token));
  Token *opstack = (Token *)malloc((in_len + 32) * sizeof(Token));
  if (!output || !opstack)
  {
    free(output);
    free(opstack);
    *errcode = 4;
    return NULL;
  }
  size_t out_cap = in_len + 32, out_i = 0, op_top = 0;

  for (size_t i = 0; i < in_len; i++)
  {
    Token t = in[i];
    if (t.type == TOK_NUMBER || t.type == TOK_VARIABLE)
    {
      output[out_i++] = t;
    }
    else if (t.type == TOK_FUNCTION)
    {
      opstack[op_top++] = t;
    }
    else if (t.type == TOK_OPERATOR)
    {
      if (t.op == 'u')
      {
        /* treat unary minus as a function "uminus" with high precedence */
        Token fun;
        memset(&fun, 0, sizeof(fun));
        fun.type = TOK_FUNCTION;
        strcpy(fun.fname, "_u"); /* internal */
        opstack[op_top++] = fun;
        continue;
      }
      while (op_top > 0)
      {
        Token top = opstack[op_top - 1];
        if (top.type == TOK_FUNCTION)
        {
          output[out_i++] = top;
          op_top--;
        }
        else if (top.type == TOK_OPERATOR)
        {
          char topop = top.op;
          int ptop = prec(topop);
          int pcur = prec(t.op);
          if ((is_right_assoc(t.op) && pcur < ptop) || (!is_right_assoc(t.op) && pcur <= ptop))
          {
            output[out_i++] = top;
            op_top--;
          }
          else
            break;
        }
        else
          break;
      }
      opstack[op_top++] = t;
    }
    else if (t.type == TOK_LPAREN)
    {
      opstack[op_top++] = t;
    }
    else if (t.type == TOK_RPAREN)
    {
      int found = 0;
      while (op_top > 0)
      {
        Token top = opstack[--op_top];
        if (top.type == TOK_LPAREN)
        {
          found = 1;
          break;
        }
        output[out_i++] = top;
      }
      if (!found)
      {
        free(output);
        free(opstack);
        *errcode = 1;
        return NULL;
      } /* mismatched paren */
      /* if function on top of stack, pop it to output */
      if (op_top > 0 && opstack[op_top - 1].type == TOK_FUNCTION)
      {
        output[out_i++] = opstack[--op_top];
      }
    }
  }
  while (op_top > 0)
  {
    Token top = opstack[--op_top];
    if (top.type == TOK_LPAREN || top.type == TOK_RPAREN)
    {
      free(output);
      free(opstack);
      *errcode = 1;
      return NULL;
    }
    output[out_i++] = top;
  }
  free(opstack);
  *out_len = out_i;
  return output;
}

/* --- Evaluation of RPN --- */
static comprobar eval_rpn(Token *rpn, size_t rpn_len, char varname, double x)
{
  comprobar res;
  res.valor_duble = 0.0;
  res.valor_int = 0;
  res.codigo = 0;
  double *stack = (double *)malloc((rpn_len + 8) * sizeof(double));
  if (!stack)
  {
    res.codigo = 4;
    return res;
  }
  size_t top = 0;
  for (size_t i = 0; i < rpn_len; i++)
  {
    Token t = rpn[i];
    if (t.type == TOK_NUMBER)
    {
      stack[top++] = t.value;
    }
    else if (t.type == TOK_VARIABLE)
    {
      stack[top++] = x;
    }
    else if (t.type == TOK_OPERATOR)
    {
      if (t.op == '+')
      {
        if (top < 2)
        {
          res.codigo = 2;
          free(stack);
          return res;
        }
        double b = stack[--top];
        double a = stack[--top];
        stack[top++] = a + b;
      }
      else if (t.op == '-')
      {
        if (top < 2)
        {
          res.codigo = 2;
          free(stack);
          return res;
        }
        double b = stack[--top];
        double a = stack[--top];
        stack[top++] = a - b;
      }
      else if (t.op == '*')
      {
        if (top < 2)
        {
          res.codigo = 2;
          free(stack);
          return res;
        }
        double b = stack[--top];
        double a = stack[--top];
        stack[top++] = a * b;
      }
      else if (t.op == '/')
      {
        if (top < 2)
        {
          res.codigo = 2;
          free(stack);
          return res;
        }
        double b = stack[--top];
        double a = stack[--top];
        if (b == 0.0)
        {
          res.codigo = 2;
          free(stack);
          return res;
        }
        stack[top++] = a / b;
      }
      else if (t.op == '^')
      {
        if (top < 2)
        {
          res.codigo = 2;
          free(stack);
          return res;
        }
        double exp = stack[--top];
        double base = stack[--top];
        /* handle negative base with non-integer exponent -> domain error */
        double v = pow(base, exp);
        if (!isfinite(v))
        {
          res.codigo = 2;
          free(stack);
          return res;
        }
        stack[top++] = v;
      }
      else
      {
        res.codigo = 2;
        free(stack);
        return res;
      }
    }
    else if (t.type == TOK_FUNCTION)
    {
      if (strcmp(t.fname, "_u") == 0)
      {
        /* unary minus */
        if (top < 1)
        {
          res.codigo = 2;
          free(stack);
          return res;
        }
        double a = stack[--top];
        stack[top++] = -a;
      }
      else
      {
        if (top < 1)
        {
          res.codigo = 2;
          free(stack);
          return res;
        }
        double a = stack[--top];
        double v = 0.0;
        if (strcmp(t.fname, "sin") == 0)
          v = sin(a);
        else if (strcmp(t.fname, "cos") == 0)
          v = cos(a);
        else if (strcmp(t.fname, "tan") == 0)
          v = tan(a);
        else if (strcmp(t.fname, "asin") == 0)
        {
          if (a < -1.0 || a > 1.0)
          {
            res.codigo = 2;
            free(stack);
            return res;
          }
          v = asin(a);
        }
        else if (strcmp(t.fname, "acos") == 0)
        {
          if (a < -1.0 || a > 1.0)
          {
            res.codigo = 2;
            free(stack);
            return res;
          }
          v = acos(a);
        }
        else if (strcmp(t.fname, "atan") == 0)
          v = atan(a);
        else if (strcmp(t.fname, "exp") == 0)
          v = exp(a);
        else if (strcmp(t.fname, "ln") == 0)
        {
          if (a <= 0.0)
          {
            res.codigo = 2;
            free(stack);
            return res;
          }
          v = log(a);
        }
        else if (strcmp(t.fname, "log") == 0)
        {
          if (a <= 0.0)
          {
            res.codigo = 2;
            free(stack);
            return res;
          }
          v = log10(a);
        }
        else if (strcmp(t.fname, "sqrt") == 0)
        {
          if (a < 0.0)
          {
            res.codigo = 2;
            free(stack);
            return res;
          }
          v = sqrt(a);
        }
        else if (strcmp(t.fname, "cbrt") == 0)
          v = cbrt(a);
        else
        {
          res.codigo = 2;
          free(stack);
          return res;
        }
        if (!isfinite(v))
        {
          res.codigo = 2;
          free(stack);
          return res;
        }
        stack[top++] = v;
      }
    }
    else
    {
      res.codigo = 2;
      free(stack);
      return res;
    }
  }

  if (top != 1)
  {
    res.codigo = 2;
    free(stack);
    return res;
  }
  res.valor_duble = stack[0];
  res.valor_int = (int)res.valor_duble;
  res.codigo = 0;
  free(stack);
  return res;
}

Function *detectar_funcion(const char *expr)
{
  if (!expr)
    return NULL;
  int err = 0;
  char detected_var = 0;
  size_t infix_len = 0;
  Token *infix = tokenize_and_normalize(expr, &infix_len, &detected_var, &err);
  if (!infix)
    return NULL;
  size_t rpn_len = 0;
  Token *rpn = infix_to_rpn(infix, infix_len, &rpn_len, &err);
  free(infix);
  if (!rpn)
    return NULL;
  Function *f = (Function *)malloc(sizeof(Function));
  if (!f)
  {
    free(rpn);
    return NULL;
  }
  f->rpn = rpn;
  f->rpn_len = rpn_len;
  f->varname = detected_var ? detected_var : 'x';
  return f;
}

comprobar evaluar_funcion(Function *f, double x)
{
  comprobar c;
  if (!f)
  {
    c.valor_duble = 0.0;
    c.valor_int = 0;
    c.codigo = 1; // Error de análisis (parse error)
    return c;
  }
  return eval_rpn(f->rpn, f->rpn_len, f->varname, x);
}

/* liberar la función */
void liberar_funcion(Function *f)
{
  free_function(f);
}

/* Nueva función: evaluar expresión directamente sin Function */
comprobar evaluar_expresion(const char *expr, double x)
{
  comprobar resultado;

  Function *f = detectar_funcion(expr);
  if (f == NULL)
  {
    resultado.codigo = 1; // Error de análisis
    resultado.valor_duble = 0.0;
    resultado.valor_int = 0;
    return resultado;
  }

  resultado = evaluar_funcion(f, x);
  liberar_funcion(f);
  return resultado;
}
