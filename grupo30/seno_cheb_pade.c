/*
 sen(x) con ENTRADA en Q3.12 y SALIDA en Q1.14 (formatos sin contar el bit de signo).
   Entrada : 16 bits = 1 signo + 3 enteros + 12 decimales, 1.0 = 4096,
             rango [-8, 7.99976] rad, paso 1/4096 = 0.000244 rad.
   Salida  : 16 bits = 1 signo + 1 entero + 14 decimales, 1.0 = 16384,
             rango [-2, 1.99994].
 formula : sen(x) ~= x * (1 - A*u) / (1 + B*u)  con u = x^2/2,  x en [0, pi/2]
*/
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define ONE         16384u   /* 1.0 en Q1.14 (salida)                  */
#define TWO_PI_Q12  25736u   /* 2*pi en Q3.12 (6.28318 * 4096)         */
#define PI_Q12      12868u   /* pi   en Q3.12 (3.14159 * 4096)         */
#define HALF_PI_Q12  6434u   /* pi/2 en Q3.12 (1.57080 * 4096)         */
#define COEF_A      3672u    /* 0.2241 (minimax, enteros Q1.14)        */
#define COEF_B      1814u    /* 0.1107                                 */

/* Contadores de iteraciones (vueltas de bucle) del ultimo calculo de seno */
static int g_iter_mulu = 0;
static int g_iter_divf = 0;

/* Producto en Q1.14 (half=0) o x*y/2 (half=1). a, b en [0, 32767].
   Multiplicacion "de izquierda a derecha" (Horner): solo el multiplicando
   ocupa un limb; el acumulador usa dos limbs de 15 bits (acc = hi*2^15 + lo). */
static uint16_t mulu(uint16_t a, uint16_t b, int half)
{
    if (a < b) { uint16_t t = a; a = b; b = t; }  /* b = el menor */
    if (b == 0) return 0;
    uint16_t hi = 0, lo = 0, n = 15;
    b <<= 1;                                      /* bit 14 -> bit 15 */
    while (!(b & 0x8000)) { b <<= 1; n--; }       /* saltear ceros iniciales */
    do {
        g_iter_mulu++;
        lo <<= 1; hi <<= 1;                       /* acc *= 2 */
        if (lo & 0x8000) { lo &= 0x7FFF; hi++; }
        if (b & 0x8000) {                         /* bit del multiplicador */
            lo += a;
            if (lo & 0x8000) { lo &= 0x7FFF; hi++; }
        }
        b <<= 1;
    } while (--n);
    lo += half ? 0x4000 : 0x2000;                 /* redondeo */
    if (lo & 0x8000) { lo &= 0x7FFF; hi++; }
    return half ? hi : (uint16_t)((hi << 1) + (lo >= 0x4000));
}

static uint16_t divf(uint16_t n, uint16_t d)
{
    uint16_t r = n, q = 0;
    for (int i = 0; i < 14; i++) {
        g_iter_divf++;
        q <<= 1; r <<= 1;
        if ((int16_t)(uint16_t)(r - d) >= 0) { r -= d; q |= 1; }
    }
    r <<= 1;
    if ((int16_t)(uint16_t)(r - d) >= 0) q++;
    return q;
}

/* x en Q3.12 con signo, [-8.0, 8.0). Devuelve sen(x) en Q1.14. */
int16_t sen_q12(int16_t x)
{
    g_iter_mulu = 0;
    g_iter_divf = 0;
    int neg = (x < 0);
    uint16_t ax = neg ? (uint16_t)(0u - (uint16_t)x) : (uint16_t)x; /* |x|, hasta 32768 */

    if (ax > TWO_PI_Q12) ax -= TWO_PI_Q12;                 /* sen(x) = sen(x - 2pi) -> [0, 2pi] */
    if (ax > PI_Q12)   { ax -= PI_Q12; neg ^= 1; }         /* sen(x) = -sen(x - pi) -> [0, pi]  */
    if (ax > HALF_PI_Q12) ax = PI_Q12 - ax;                /* sen(x) = sen(pi - x)  -> [0, pi/2] */
    ax <<= 2;                                              /* Q3.12 -> Q1.14, exacto */
    if (ax == 0) return 0;

    uint16_t u   = mulu(ax, ax, 1);
    uint16_t num = (uint16_t)(ONE - mulu(u, COEF_A, 0));
    uint16_t den = (uint16_t)(ONE + mulu(u, COEF_B, 0));
    uint16_t s   = mulu(ax, divf(num, den), 0);
    if (s > ONE) s = ONE;
    return neg ? (int16_t)(0 - s) : (int16_t)s;
}

#ifndef NO_MAIN
int main(void)
{
    /* ---- Calculo para un angulo ingresado por el usuario ---- */
    double rad;
    printf("Ingrese el angulo en radianes (entre -8 y 7.99976): ");
    if (scanf("%lf", &rad) != 1) { printf("Entrada invalida.\n"); return 1; }
    if (rad < -8.0 || rad > 32767.0 / 4096.0) {
        printf("Angulo fuera de rango para Q3.12 (debe estar en [-8, 7.99976]).\n");
        return 1;
    }
    int16_t x_q12 = (int16_t)lround(rad * 4096.0);       /* radianes -> Q3.12 */
    int16_t s_q14 = sen_q12(x_q12);
    printf("\nAngulo en Q3.12 : %d  (= %.6f rad)\n", x_q12, x_q12 / 4096.0);
    printf("sen(x) en Q1.14 : %d  (= %.6f)\n", s_q14, s_q14 / 16384.0);
    printf("sen(x) real     : %.6f\n", sin(x_q12 / 4096.0));
    printf("Iteraciones     : mulu = %d, divf = %d, total = %d\n\n",
           g_iter_mulu, g_iter_divf, g_iter_mulu + g_iter_divf);

    /* ---- Analisis de las 65 536 entradas ---- */
    double worst = 0, sum = 0, best = 1e9; int wx = 0; long exact = 0;
    for (int x = -32768; x <= 32767; x++) {
        int s = sen_q12((int16_t)x);
        double e = fabs(s - sin(x / 4096.0) * 16384.0);
        sum += e; if (s == (int)lround(sin(x / 4096.0) * 16384.0)) exact++;
        if (e > worst) { worst = e; wx = x; }
        if (e < best) best = e;
    }
    printf("sen(pi/4) = %d (esperado 11585)\n", sen_q12(3217));
    printf("error max = %.2f LSB en x=%d (%.4f rad), \nerror medio = %.2f LSB, \nmin=%.2f, \nexactos=%.1f%%\n",
           worst, wx, wx/4096.0, sum / 65536.0, best, 100.0*exact/65536);
    return 0;
}
#endif
