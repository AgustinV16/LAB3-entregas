# Grupo 23 · SQRT

**Consigna:** Non restoring

## Integrantes
<!-- Completar en el primer Pull Request -->
- De Los Santos, Dahiara Claribel (@dahizz)
- Kamchem, Melina (@melika2332)
- Wereszczuk, Danna Maira (@wereszczukdanna-cell)

## Archivos
<!-- Qué archivo hace qué -->
- `alto_nivel/lab3a.cpp`: Programa en C++ con la demostración de conversión a formato Q, compatibilización de formatos (Q3.12 y Q7.8) y operaciones de suma, resta, producto y división.
- `asm/lab3a.asm`: Implementación en assembly LC-3 de las conversiones de formato y multiplicación en punto fijo, utilizando subrutinas de corrimiento (`SHL32`) y suma (`MULS`).
- `bitacora.md`: Registro diario de las sesiones de trabajo, objetivos y dificultades del grupo.

## Cómo probarlo
<!-- Pasos para ensamblar y ejecutar, y un ejemplo de entrada y salida -->
1. Cargar el archivo `asm/lab3a.asm` en el simulador de la LC-3 (LC3Tools o lc3web).
2. Poner un breakpoint en `ST R1, X_Q8` para verificar el corrimiento aritmético a la derecha.
3. Poner un breakpoint en `M_SIG` dentro de la subrutina `MULS` para observar cómo crece el producto parcial en los registros R1 y R2.
4. Ejecutar paso a paso y revisar los valores guardados en memoria (ej. `PROD_Q8` debe dar `xFC2C`).