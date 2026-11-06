#include <stdio.h>
#include <stdint.h>

/* Laboratorio 3a - Punto fijo
 *   Num1 -> Q3.12 : escala 2^12 = 4096, rango [-8, 8)
 *   Num2 -> Q7.8  : escala 2^8  = 256,  rango [-128, 128)
 *
 * Variables sugeridas para "Watch" en el depurador:
 *   num1_f, num2_f, q3_12, q7_8, q7_8_alineado, suma32, resta32,
 *   mult_intermedia, mult32, div_intermedia
 */

int main(void) {
    float num1_f, num2_f;

    /* ---------- Punto 1: ingreso de dos numeros con coma ---------- */
    printf("Ingrese el primer numero decimal (ej: 3.5): ");
    if (scanf("%f", &num1_f) != 1) { printf("Entrada invalida.\n"); return 1; }
    printf("Ingrese el segundo numero decimal (ej: 1.25): ");
    if (scanf("%f", &num2_f) != 1) { printf("Entrada invalida.\n"); return 1; }

    /* BREAKPOINT 1: aqui conviene detenerse y mirar num1_f y num2_f */

    /* ---------- Punto 2: casting float -> entero de 16 bits ---------- */
    float esc1 = num1_f * 4096.0f;   /* x 2^12 */
    float esc2 = num2_f * 256.0f;    /* x 2^8  */
    if (esc1 < -32768.0f || esc1 > 32767.0f) {
        printf("Num1 fuera del rango de Q3.12 [-8, 8).\n");
        return 1;
    }
    if (esc2 < -32768.0f || esc2 > 32767.0f) {
        printf("Num2 fuera del rango de Q7.8 [-128, 128).\n");
        return 1;
    }
    int16_t q3_12 = (int16_t)esc1;   /* Q3.12 */
    int16_t q7_8  = (int16_t)esc2;   /* Q7.8  */

    /* BREAKPOINT 2: observar q3_12 y q7_8 (los enteros que representan a los numeros) */

    /* ---------- Punto 3: compatibilizar formatos y sumar / restar ----------
     * Q7.8 tiene 8 bits fraccionarios y Q3.12 tiene 12. Para alinear la coma
     * se multiplica por 2^(12-8) = 16, es decir, se desplaza 4 bits a la
     * izquierda (<< 4). Se hace en 32 bits para poder detectar desbordes. */
    int32_t q7_8_alineado = (int32_t)q7_8 * 16;          /* equivale a q7_8 << 4 -> Q3.12 */
    int32_t suma32  = (int32_t)q3_12 + q7_8_alineado;    /* Q3.12 */
    int32_t resta32 = (int32_t)q3_12 - q7_8_alineado;    /* Q3.12 */
    int sum_ovf  = (suma32  < -32768 || suma32  > 32767);
    int rest_ovf = (resta32 < -32768 || resta32 > 32767);
    int16_t suma_q3_12  = (int16_t)suma32;
    int16_t resta_q3_12 = (int16_t)resta32;

    /* ---------- Punto 4: multiplicacion ----------
     * Q3.12 x Q7.8 -> 12 + 8 = 20 bits fraccionarios (Q11.20 en 32 bits).
     * Para volver a Q3.12 hay que descartar 8 bits fraccionarios:
     * desplazamiento aritmetico a la derecha de 8 (>> 8, o sea dividir por 256). */
    int32_t mult_intermedia = (int32_t)q3_12 * (int32_t)q7_8;   /* Q11.20 */
    int32_t mult32 = mult_intermedia >> 8;                      /* Q3.12 */
    int mult_ovf = (mult32 < -32768 || mult32 > 32767);
    int16_t mult_resultado = (int16_t)mult32;

    /* ---------- Punto 5: division con la mayor resolucion posible ----------
     * Cociente de enteros crudos: (A*2^12)/(B*2^8) = (A/B)*2^4 -> solo 4 bits
     * fraccionarios. Para ganar resolucion se escala el dividendo antes de dividir:
     *     (q3_12 << 16) / q7_8  =  (A/B) * 2^(12+16-8) = (A/B) * 2^20
     * 16 es el mayor desplazamiento con el que q3_12 << k sigue entrando en 32 bits.
     * Rango del cociente: |A|max / |B|min = 8 / 2^-8 = 2^11 -> 11 bits enteros.
     * Formato resultante: Q11.20 (1 signo + 11 enteros + 20 fraccionarios = 32 bits). */
    int div_error = (q7_8 == 0);
    int64_t div_intermedia = 0;
    int32_t div_resultado = 0;       /* Q11.20 */
    if (!div_error) {
        div_intermedia = ((int64_t)q3_12 * 65536) / (int64_t)q7_8;
        if (div_intermedia > INT32_MAX) div_intermedia = INT32_MAX; /* caso extremo: +2048 exacto */
        div_resultado = (int32_t)div_intermedia;
    }

    /* BREAKPOINT 3: observar suma32, resta32, mult_intermedia, mult32 y div_intermedia */

    printf("\n--- RESULTADOS EN PUNTO FIJO ---\n");
    printf("Q3.12 (Num1): %d (0x%04X) = %.6f\n", q3_12, (uint16_t)q3_12, q3_12 / 4096.0);
    printf("Q7.8  (Num2): %d (0x%04X) = %.6f\n", q7_8,  (uint16_t)q7_8,  q7_8 / 256.0);
    printf("Q7.8 alineado a Q3.12 (<< 4): %d\n", (int)q7_8_alineado);
    printf("Suma  (Q3.12): %d = %.6f%s\n", suma_q3_12, suma32 / 4096.0,
           sum_ovf ? "  [DESBORDE en 16 bits]" : "");
    printf("Resta (Q3.12): %d = %.6f%s\n", resta_q3_12, resta32 / 4096.0,
           rest_ovf ? "  [DESBORDE en 16 bits]" : "");
    printf("Multiplicacion (Q3.12): %d = %.6f%s\n", mult_resultado, mult32 / 4096.0,
           mult_ovf ? "  [DESBORDE en 16 bits]" : "");
    if (div_error)
        printf("Division: error, Num2 representado como 0 en Q7.8\n");
    else
        printf("Division (Q11.20): %ld = %.9f\n", (long)div_resultado, div_resultado / 1048576.0);

    return 0;
}
