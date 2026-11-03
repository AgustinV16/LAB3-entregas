#include <stdio.h>
#include <stdint.h>
#include <math.h>

int main(void) {
    // el algoritmo de cordic configurado en modo lineal se puede entender de la siguiente forma
    // funciona con 3 variables, la primera(x) donde estara el valor fijo del primer factor
    // la segunda(y) funciona como acumulador
    // y la tercera(z) como "contador" el cual iremos vaciando
    // la idea es que a Z se le resten (o sumen) potencias fijas de 2 (desplazamiento de bits)
    // mientras que, a la variable Y se le realiza la operacion inversa, pero multiplicada por X
    // es decir ->      z = z +- 2^(-n);        y = y +- x * 2^(-n)
    // se realiza en bucle hasta que Z llegue a 0
    // mientras Z > 0, se resta en Z y se suma en Y
    // si Z < 0, nos pasamos y se corrige, se suma en Z y se resta en Y
    // si Z == 0 llegamos a un resultado exacto, y en ese momento tendremos guardado en Y el resultado de a*b o x*z

    // variables
    float a, b, resultado;
    int16_t a_q8, b_q8;
    int32_t z;              // multiplicador que voy vaciar
    int32_t y_q16;          // acumulador
    int32_t current_z;      // constante movil
    int32_t current_x;      // multiplicando movil

    printf("--- MULTIPLICACION POR CORDIC ---\n");
    printf("Ingrese el primer factor: ");
    scanf("%f", &a);

    printf("Ingrese el segundo factor: ");
    scanf("%f", &b);

    // casteo de float a punto fijo formato Q7.8 --> multiplica por 2^8
    a_q8 = (int16_t)lroundf(a * 256.0f);
    b_q8 = (int16_t)lroundf(b * 256.0f);

    // se inicializa a z con el multiplicador
    z = b_q8;

    // se inicializa el acumulador en 0
    y_q16 = 0;

    // constante movil iniciada en 64(2^6)
    // para que quede en formato Q7.8 se deben correr 8 bits a la izquierda
    // se inicia en 64 ya que es el valor maximo de un numero en Q7.8
    current_z = 64 * 256; // * 2^8 --> 64.0 en Q7.8

    // realizo lo mismo que en current_z pero multiplicado por x
    // se multiplica por 64 para que quede en formato Q7.8
    // queda con 8 bits fraccionarios extra, para que los desplazamientos de bits no pierdan decimales
    current_x = (int32_t)a_q8 * 16384; // * 2^14 para evitar perder decimales

    for (int i=0; i < 15; i++) {
        if (z == 0) break;
        if (z > 0) {
            z -= current_z;
            y_q16 += current_x;
        } else {
            z += current_z;
            y_q16 -= current_x;
        }

        current_x /= 2;
        current_z /= 2;
    }

    // reduccion a Q7.8 con redondeo
    int32_t y_q78 = (y_q16 + 128) >> 8;
    resultado = (float)y_q78 / 256.0f;

    printf("Resultado directo (float original): %f\n", a * b);
    // se divide por 65536 ya que se multiplico por 2^8 dos veces (una para cada factor)
    printf("Resultado ideal cuantizado: %f\n", (float)((int32_t)a_q8 * b_q8) / 65536.0f);
    printf("Resultado en formato Q7.8: %i\n", y_q78);
    printf("Resultado convertido en decimal: %f\n", resultado);

    return 0;
}