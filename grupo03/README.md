# Grupo 03 · Multiplicación

**Consigna:** Por algoritmo de shift + sumas

## Integrantes
<!-- Completar en el primer Pull Request -->
- Herrera Paulus, Diego Sebastian (@Diego11331)
- Sedoff, Nicolás Alejandro (@Altrupro73)
- Garcia, Franco (@Mekkii088)

## Archivos
<!-- Qué archivo hace qué -->
- `Multiplicacion_3_Formatos.asm`: código assebly principal, es el que realiza la multiplicación.

## Cómo probarlo
<!-- Pasos para ensamblar y ejecutar, y un ejemplo de entrada y salida -->
- `Multiplicacion_3_Formatos.asm`: multiplica dos números en punto fijo (Q12.4, Q10.6 o Q8.8) con shift + sumas. El producto queda en **R1 (palabra alta)** y **R0 (palabra baja)**.

**Pasos**
1. Abrir el `.asm` en el simulador LC-3 y ensamblarlo.
2. Ejecutarlo y, en la consola, escribir solo `1`, `2` o `3` para el formato (**sin Enter**).
3. Escribir cada valor (entero o hasta 2 decimales, con punto) y presionar **Enter**.
4. Al terminar se imprime el producto en hexadecimal.

**Ejemplo** (Q8.8, `-2.5 × 3.14`)
```
Opcion: 3
Ingrese el primer valor: -2.5
Ingrese el segundo valor: 3.14
Producto alto: xFFF8
Producto bajo: x2600
```
`xFFF82600` = -514560, y -514560 / 2^16 = **-7.8515625**.