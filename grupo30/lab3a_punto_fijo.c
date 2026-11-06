/*
 * =====================================================================
 *  LABORATORIO 3a - Arquitectura de Computadoras - UNaM
 *  Tema: trabajar con punto fijo en el procesador
 * =====================================================================
 *  Notacion usada en todo el programa:
 *      Qm.n = 1 bit de signo + m bits enteros + n bits fraccionarios,
 *      con m + n = 15, guardado en un entero de 16 bits con signo.
 *      Valor real = entero / 2^n        Resolucion = 1 / 2^n
 *
 *  Numero A -> Q3.12 : rango [-8, 8),     resolucion 1/4096
 *  Numero B -> Q7.8  : rango [-128, 128), resolucion 1/256
 *
 *  Actividades (guia de laboratorio):
 *    1) ingreso de dos numeros con coma (double) y breakpoints
 *    2) paso de cada numero a un formato Q distinto (casting a int16_t)
 *    3) conversion entre formatos Q, suma y resta
 *    4) multiplicacion y ajuste del resultado al formato Q elegido
 *    5) division conservando la mayor resolucion fraccionaria posible
 *
 *  Para depurar: poner un breakpoint en cada linea marcada con
 *  "<== BREAKPOINT n" y agregar las variables indicadas a las
 *  variables vigiladas (watch).
 * =====================================================================
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

/* ---- Formatos elegidos (cantidad de bits FRACCIONARIOS) ------------ */
#define N_A     12      /* A en Q3.12                                   */
#define N_B     8       /* B en Q7.8                                    */
#define N_SUMA  N_B     /* formato comun para sumar/restar: Q7.8        */
#define N_PROD  N_B     /* formato elegido para el producto: Q7.8       */

/* ---- Utilidades ---------------------------------------------------- */

/* Limita un valor de 32 bits al rango de un entero de 16 bits con signo */
static int16_t saturar16(int32_t v, int *saturo)
{
    if (v > 32767)  { *saturo = 1; return 32767;  }
    if (v < -32768) { *saturo = 1; return -32768; }
    return (int16_t)v;
}

/* Actividad 2: numero con coma -> Qm.n (casting a entero de 16 bits).
 * Se multiplica por 2^n y se suma/resta 0,5 antes del casting para que
 * el casting (que trunca hacia cero) termine redondeando al mas cercano. */
static int16_t float_a_q(double x, int n)
{
    double escalado = x * (double)(1 << n);
    return (int16_t)(escalado >= 0.0 ? escalado + 0.5 : escalado - 0.5);
}

/* Qm.n -> numero con coma (solo para mostrar y comparar) */
static double q_a_float(int32_t q, int n)
{
    return (double)q / (double)(1 << n);
}

/* ---- Actividad 3 ----------------------------------------------------
 * Cambio de formato Q: de n_orig a n_dest bits fraccionarios.
 *   n_dest < n_orig : se descartan k = n_orig - n_dest bits ->
 *                     desplazamiento a la DERECHA de k (con redondeo:
 *                     se suma 2^(k-1) antes de desplazar). Pierde
 *                     resolucion pero gana rango entero.
 *   n_dest > n_orig : desplazamiento a la IZQUIERDA de k -> gana
 *                     resolucion pero pierde rango (puede saturar).
 * Se trabaja con el modulo y se reaplica el signo (redondeo simetrico). */
static int16_t convertir_q(int16_t x, int n_orig, int n_dest, int *saturo)
{
    int neg = (x < 0);
    int32_t mag = neg ? -(int32_t)x : (int32_t)x;      /* |x| (hasta 32768) */
    *saturo = 0;

    if (n_dest < n_orig) {
        int k = n_orig - n_dest;
        mag = (mag + (1 << (k - 1))) >> k;
    } else if (n_dest > n_orig) {
        int k = n_dest - n_orig;
        mag = mag << k;
        if (mag > 32767) { mag = 32767; *saturo = 1; }
    }
    return (int16_t)(neg ? -mag : mag);
}

/* Suma y resta en un mismo formato Q (los dos operandos deben tener el
 * mismo n). El resultado se calcula en 32 bits y se satura a 16. */
static int16_t sumar_q(int16_t a, int16_t b, int *saturo)
{
    *saturo = 0;
    return saturar16((int32_t)a + (int32_t)b, saturo);
}

static int16_t restar_q(int16_t a, int16_t b, int *saturo)
{
    *saturo = 0;
    return saturar16((int32_t)a - (int32_t)b, saturo);
}

/* ---- Actividad 4 ----------------------------------------------------
 * Multiplicacion. Si a esta en Q(.n_a) y b en Q(.n_b), el producto
 * entero a*b tiene n_a + n_b bits fraccionarios (aqui 12 + 8 = 20) y
 * necesita 32 bits. Para expresarlo en Q(.n_res) hay que descartar
 * k = n_a + n_b - n_res bits: desplazamiento a la derecha de k (con
 * redondeo). Con A en Q3.12, B en Q7.8 y resultado en Q7.8: k = 12.
 * "crudo" devuelve el producto exacto de 32 bits (sin ajustar).       */
static int16_t multiplicar_q(int16_t a, int n_a, int16_t b, int n_b,
                             int n_res, int32_t *crudo, int *saturo)
{
    int neg = ((a < 0) != (b < 0));
    int32_t ma = (a < 0) ? -(int32_t)a : (int32_t)a;
    int32_t mb = (b < 0) ? -(int32_t)b : (int32_t)b;
    if (ma > 32767) ma = 32767;               /* -32768 se trata como -32767 */
    if (mb > 32767) mb = 32767;

    int64_t prod = (int64_t)ma * mb;           /* exacto, hasta 2^30        */
    *crudo = (int32_t)(neg ? -prod : prod);

    int k = (n_a + n_b) - n_res;               /* bits a descartar          */
    int64_t r = (k > 0) ? ((prod + ((int64_t)1 << (k - 1))) >> k)
                        : (prod << (-k));
    *saturo = 0;
    if (r > 32767) { r = 32767; *saturo = 1; }
    return (int16_t)(neg ? -r : r);
}

/* ---- Actividad 5 ----------------------------------------------------
 * Division a/b conservando la mayor resolucion fraccionaria posible.
 * El cociente entero a/b sin mas cuidado tendria n_a - n_b fraccionarios
 * (aqui 4) y perderia casi todo. Para evitarlo se amplia el numerador:
 *      q = a * 2^(n + n_b) / (b * 2^n_a)      -> cociente con n bits
 * y se busca el MAYOR n (de 15 a 0) para el cual q todavia cabe en 16
 * bits con signo. El formato resultante es Q(15-n).n : se decide segun
 * el tamano real del cociente. Se redondea al mas cercano.            */
static int16_t dividir_q(int16_t a, int n_a, int16_t b, int n_b,
                         int *n_res, int *saturo)
{
    int neg = ((a < 0) != (b < 0));
    int64_t ma = (a < 0) ? -(int64_t)a : (int64_t)a;
    int64_t mb = (b < 0) ? -(int64_t)b : (int64_t)b;

    *saturo = 0;
    for (int n = 15; n >= 0; n--) {
        int64_t num = ma << (n + n_b);
        int64_t den = mb << n_a;
        int64_t q = (2 * num + den) / (2 * den);       /* redondeo */
        if (q <= 32767) {
            *n_res = n;
            return (int16_t)(neg ? -q : q);
        }
    }
    *n_res = 0;                                        /* no cabe: satura */
    *saturo = 1;
    return (int16_t)(neg ? -32767 : 32767);
}

/* ---- Salida por pantalla ------------------------------------------- */
static void mostrar(const char *nombre, int32_t q, int n, int bits32)
{
    if (bits32)
        printf("  %-26s entero=%11d  hex=0x%08X  real=%.8f\n",
               nombre, q, (unsigned)q, q_a_float(q, n));
    else
        printf("  %-26s entero=%6d  hex=0x%04X  real=%.8f\n",
               nombre, q, (unsigned)(uint16_t)q, q_a_float(q, n));
}

int main(void)
{
    double  a_real, b_real;                 /* Actividad 1: numeros con coma */
    int16_t a_q, b_q;                       /* Actividad 2                   */
    int16_t a_en_q78, b_en_q312;            /* Actividad 3: conversiones     */
    int16_t suma_q, resta_q;                /* Actividad 3: suma y resta     */
    int32_t prod_crudo;                     /* Actividad 4: producto 32 bits */
    int16_t prod_q;                         /* Actividad 4: producto ajustado*/
    int16_t div_q = 0;                      /* Actividad 5                   */
    int     n_div = 0;                      /* Actividad 5: bits del cociente*/
    int     s1, s2, s3, s4, s5, s6;         /* banderas de saturacion        */

    printf("=== LABORATORIO 3a - PUNTO FIJO ===\n");
    printf("Numero A se guarda en Q%d.%d  [%.4f, %.4f]\n", 15 - N_A, N_A,
           -(double)(1 << (15 - N_A)),
           (double)(1 << (15 - N_A)) - 1.0 / (1 << N_A));
    printf("Numero B se guarda en Q%d.%d  [%.4f, %.4f]\n\n", 15 - N_B, N_B,
           -(double)(1 << (15 - N_B)),
           (double)(1 << (15 - N_B)) - 1.0 / (1 << N_B));

    /* ---- Actividad 1: ingreso de los dos numeros --------------------- */
    printf("Ingrese el numero A: ");
    if (scanf("%lf", &a_real) != 1) { printf("Entrada invalida.\n"); return 1; }
    printf("Ingrese el numero B: ");
    if (scanf("%lf", &b_real) != 1) { printf("Entrada invalida.\n"); return 1; }
    /* <== BREAKPOINT 1  (vigilar: a_real, b_real) */

    if (a_real < -(double)(1 << (15 - N_A)) ||
        a_real > (double)(1 << (15 - N_A)) - 1.0 / (1 << N_A)) {
        printf("A esta fuera del rango de Q%d.%d.\n", 15 - N_A, N_A);
        return 1;
    }
    if (b_real < -(double)(1 << (15 - N_B)) ||
        b_real > (double)(1 << (15 - N_B)) - 1.0 / (1 << N_B)) {
        printf("B esta fuera del rango de Q%d.%d.\n", 15 - N_B, N_B);
        return 1;
    }

    /* ---- Actividad 2: pasar cada numero a un formato Q distinto ------ */
    a_q = float_a_q(a_real, N_A);
    b_q = float_a_q(b_real, N_B);
    /* <== BREAKPOINT 2  (vigilar: a_q, b_q) */

    printf("\n--- Actividad 2: representacion en punto fijo ---\n");
    mostrar("A en Q3.12", a_q, N_A, 0);
    mostrar("B en Q7.8",  b_q, N_B, 0);

    /* ---- Actividad 3: compatibilizar formatos, sumar y restar -------- */
    a_en_q78  = convertir_q(a_q, N_A, N_B, &s1);   /* Q3.12 -> Q7.8 (>> 4) */
    b_en_q312 = convertir_q(b_q, N_B, N_A, &s2);   /* Q7.8 -> Q3.12 (<< 4) */
    suma_q    = sumar_q(a_en_q78, b_q, &s3);       /* ambos en Q7.8        */
    resta_q   = restar_q(a_en_q78, b_q, &s4);
    /* <== BREAKPOINT 3  (vigilar: a_en_q78, b_en_q312, suma_q, resta_q) */

    printf("\n--- Actividad 3: conversion entre formatos, suma y resta ---\n");
    mostrar("A convertido a Q7.8",  a_en_q78,  N_SUMA, 0);
    mostrar("B convertido a Q3.12", b_en_q312, N_A,    0);
    if (s2) printf("  (B no cabe en Q3.12: se satura)\n");
    mostrar("A + B en Q7.8", suma_q,  N_SUMA, 0);
    mostrar("A - B en Q7.8", resta_q, N_SUMA, 0);
    if (s3 || s4) printf("  (la suma o la resta se saturo)\n");
    printf("  esperado (double)  A + B = %.8f   A - B = %.8f\n",
           a_real + b_real, a_real - b_real);
    printf("  error              |A + B| = %.6f   |A - B| = %.6f\n",
           fabs(q_a_float(suma_q, N_SUMA) - (a_real + b_real)),
           fabs(q_a_float(resta_q, N_SUMA) - (a_real - b_real)));

    /* ---- Actividad 4: multiplicacion --------------------------------- */
    prod_q = multiplicar_q(a_q, N_A, b_q, N_B, N_PROD, &prod_crudo, &s5);
    /* <== BREAKPOINT 4  (vigilar: prod_crudo, prod_q) */

    printf("\n--- Actividad 4: multiplicacion ---\n");
    mostrar("A*B exacto (Q10.20)", prod_crudo, N_A + N_B, 1);
    mostrar("A*B ajustado a Q7.8", prod_q, N_PROD, 0);
    printf("  ajuste: desplazamiento a la derecha de %d bits (con redondeo)\n",
           N_A + N_B - N_PROD);
    if (s5) printf("  (el producto no cabe en Q7.8: se satura)\n");
    printf("  esperado (double)  A * B = %.8f   error = %.6f\n",
           a_real * b_real,
           fabs(q_a_float(prod_q, N_PROD) - a_real * b_real));

    /* ---- Actividad 5: division --------------------------------------- */
    printf("\n--- Actividad 5: division ---\n");
    if (b_q == 0) {
        printf("  B vale 0: la division A / B no esta definida.\n");
    } else {
        div_q = dividir_q(a_q, N_A, b_q, N_B, &n_div, &s6);
        /* <== BREAKPOINT 5  (vigilar: div_q, n_div) */
        printf("  formato elegido para A / B: Q%d.%d\n", 15 - n_div, n_div);
        mostrar("A / B", div_q, n_div, 0);
        if (s6) printf("  (el cociente no cabe en 16 bits: se satura)\n");
        printf("  esperado (double)  A / B = %.8f   error = %.8f\n",
               a_real / b_real,
               fabs(q_a_float(div_q, n_div) - a_real / b_real));
    }

    return 0;
}
