#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(void) {
    float a, b, suma, producto, cociente;
    int16_t a_q12, b_q8;
    int16_t a_q8, b_q12;
    int16_t suma_q8, producto_q8, cociente_q8;
    int32_t producto_q20;

    printf("Ingrese el primer numero: ");
    scanf("%f", &a);

    printf("Ingrese el segundo numero: ");
    scanf("%f", &b);

    while(b == 0) {
        printf("El segundo numero no puede ser 0, reingrese: ");
        scanf("%f", &b);
    }

    // para convertir un valor de formato Qm.n a un entero de 16 bits se hace lo siguiente
    // se corren LOS BITS hacia la izquierda "n" cantidad de veces ---> (1 << 12)
    // esto equivale a correr la coma "n" veces hacia la derecha
    // tambien equivale a multiplicar el valor original por 2^n
    // 2^n = (1 << n) --> en Q3.12, n = 12, entonces 2^12 = (1 << 12)
    // despues se lo castea a entero de 16 bits
    a_q12   = (int16_t)(a * (1 << 12)); // (int16_t)( a * 2^12 )
    b_q8    = (int16_t)(b * (1 << 8));  // (int16_t)( b * 2^8  )

    // suma directa
    suma = a+b;

    // conversion de A: de Q3.12 a Q7.8
    // se desplazan 4 bits hacia la derecha
    // antes la coma estaba en el bit 12, luego los bits se corrieron
    // y la coma paso a estar 4 bits a la derecha, osea en el bit 8
    // es la diferencia de "n" entre ambos numeros (12 - 8) = 4
    a_q8 = a_q12 >> 4;

    // con los valores alineados, realizo la suma
    // el resultado mantiene el formato Q7.8
    suma_q8 = a_q8 + b_q8;

    // producto directo
    producto = a * b;

    // al multiplicar dos numeros en formato Q, se suman tanto "m" como "n"
    // Q3.12 * Q7.8 = Q3+7.12+8 --> Q10.20
    // esto supera los 16 bits, se necesita una variable mas grande temporal de 32 bits
    producto_q20 = (int32_t)a_q12 * (int32_t)b_q8;
    producto_q8 = producto_q20 >> 12;

    // cociente directo
    cociente = a/b;

    // al dividir dos numeros en formato Q, se restan tanto "m" como "n"
    // Q3.12 / Q7.8 = Q3-7.12-8 --> Q-4.4
    // no tiene sentido que uno quede negativo
    // se preescala el numerador(a) a 32 bits para evitar perder precision
    // se busca que la resta de m y n nos de como resultado el formato Q que buscamos
    // el numerador esta en Q3.12, lo casteamos a 32 bits y nos queda en formato Q19.12
    // todos los bits nuevos van a la parte entera, luego corremos 4 bits a la izquierda (<< 4)
    // esto agranda la parte decimal y achica la parte entera en 4 posiciones
    // Q19-4,12+4 ---> Q15.16
    // luego la division con el valor "b" en formato Q7.8 se realiza sin problema
    // Q15.16 / Q7.8 ---> Q15-7,16-8 = Q8.8 (en realidad seria Q7.8 porque no se cuenta el bit del signo)
    cociente_q8 = ((int32_t)a_q12 << 4) / b_q8;

    printf("Valor ORIGINAL de A: %f\n", a);
    printf("Valor ORIGINAL de B: %f\n", b);
    printf("Valor de A en Q3.12: %hd\n", a_q12);
    printf("Valor de B en Q7.8: %hd\n", b_q8);
    printf("\n--- SUMA ---\n");
    printf("Suma directa A+B: %f\n", suma);
    printf("Suma en formato Q7.8: %hd\n", suma_q8);
    printf("Suma convertida a decimal: %f\n", (float)suma_q8/(1 << 8));
    printf("\n-- PRODUCTO ---\n");
    printf("Producto directo A*B: %f\n", producto);
    printf("Producto temporal de 32 bits, formato Q10.20: %i\n", producto_q20);
    printf("Producto casteado a 16 bits, formato Q7.8: %hd\n", producto_q8);
    printf("Producto convertido a decimal: %f\n", (float)producto_q8/(1 << 8));
    printf("\n--- COCIENTE ---\n");
    printf("Cociente directo A/B: %f\n", cociente);
    printf("Cociente en formato Q7.8: %hd\n", cociente_q8);
    printf("Cociente convertido a decimal: %f\n", (float)cociente_q8/(1 << 8));
    return 0;
}