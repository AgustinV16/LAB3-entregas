# Grupo 06 · EXP

**Consigna:** Algoritmo multiplicativo de alta radix

## Integrantes
<!-- Completar en el primer Pull Request -->
- Apellido, Nombre (@usuario-github)
- Apellido, Nombre (@usuario-github)

## Archivos

- `lab3_punto_fijo.c` convierte dos operandos a Q3.12 y Q7.8 y calcula la suma, la resta, el producto y el cociente. Informa si un resultado no cabe en su formato.
- `lab3a_puntofijo.asm` implementa la compatibilización, la suma, la resta y la multiplicación en LC-3.
- `exp_alta_radix.c` calcula la exponencial con el algoritmo multiplicativo de base 16. La entrada es Q3.12 y la salida, Q7.8.
- `exp_lc3.asm` implementa el mismo cálculo de EXP en LC-3.
- `Informe Lab 3 - Arquitectura.pdf` documenta ambos desarrollos, las pruebas y la bitácora.

## Cómo probarlo

### En C

Desde `grupo06`, compilá con GCC.

```bash
gcc -std=c11 -Wall -Wextra lab3_punto_fijo.c -o lab3_punto_fijo
gcc -std=c11 -Wall -Wextra exp_alta_radix.c -o exp_lab -lm
```

Ejecutá `./lab3_punto_fijo` e ingresá `0.0625` y `0.125`, usando punto decimal. Debe mostrar suma `0.1875`, resta `-0.0625`, producto `0.0078125` y cociente `0.5`.

Con `./exp_lab 3` se obtiene el resultado indicado en la prueba de EXP de abajo. `./exp_lab --pruebas` recorre las 65.536 entradas Q3.12. En CMD de Windows, ejecutá los archivos `.exe` que genera GCC.

### En Assembly LC-3

Ensamblá `lab3a_puntofijo.asm` y ejecutalo desde la dirección `x3000` en el simulador. El programa toma sus operandos de constantes `.FILL` en memoria.

Con los operandos de ejemplo, `RES_SUMA` queda en `x0030`, `RES_RESTA` en `xFFF0` y `RES_MULT` en `x0002`. Los estados `EST_SUMA`, `EST_RESTA` y `EST_MULT` valen `0` si el resultado cabe en Q7.8 y `1` si se desborda. Con estado `1`, el valor guardado es `0`.

## Prueba de la función EXP

La versión en Assembly está disponible en el simulador del **Grupo 8**, donde puede ejecutarse directamente.

Para comprobar su funcionamiento, se puede evaluar $e^3$. El valor esperado en la salida `RES_EXP` es **`x1416`** en hexadecimal, que equivale a **5142** como entero. La salida está en formato Q7.8, por lo que se divide ese entero por 256 para obtener **20,0859375**.

### Representación en punto fijo

La entrada y la salida se almacenan en palabras de **16 bits con signo**. La entrada usa **Q3.12**, con 12 bits fraccionarios, y la salida usa **Q7.8**, con 8 bits fraccionarios.

### Rango de entrada y salida

El exponente $x$ se puede representar desde **−8** hasta **7,999755859375**. El mayor valor de entrada que se puede procesar sin saturar la salida es **4,851806640625**.

Aunque Q7.8 con signo abarca desde **−128** hasta **127,99609375**, la función EXP utiliza la parte no negativa de ese rango. Los resultados muy pequeños pueden redondearse a **0**, mientras que los que superan el límite se saturan en **127,99609375**.

### Resolución

La resolución indica cuánto cambia el valor al pasar de un número representable al siguiente. Se obtiene mediante $2^{-n}$, donde $n$ es la cantidad de bits fraccionarios.

- **Entrada Q3.12**. $2^{-12} = 1/4096$ equivale a **0,000244140625**.
- **Salida Q7.8**. $2^{-8} = 1/256$ equivale a **0,00390625**.
