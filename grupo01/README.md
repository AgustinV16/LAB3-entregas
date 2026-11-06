# Laboratorio 3c: Desarrollo del Benchmark (LC-3)
## Módulo de División por CORDIC (Modo Lineal Vectorial en Punto Fijo Q15)

**Universidad Nacional de Misiones (UNaM)**
**Facultad de Ingeniería**
**Asignatura:** Arquitectura de Computadoras

---

## 1. Descripción del Proyecto

Este módulo forma parte del *workload* cooperativo del Benchmark de la arquitectura LC-3. Implementa la **división de dos números** `Z = Y / X` con el algoritmo **CORDIC** (*Coordinate Rotation Digital Computer*) en **modo lineal (m = 0), vectoring mode**, usando aritmética de **punto fijo Q15**.

La rutina usa **únicamente sumas, restas y desplazamientos de bits**. No emplea división ni multiplicación, lo que la hace apta para la LC-3, que no tiene esas instrucciones.

> **Nota sobre el modo:** el modo *circular* (tabla de `arctan(2⁻ⁱ)`) calcula `arctan(y/x)`, no `y/x`. Para obtener el cociente se usa el modo **lineal** (Tabla 7.1 de *Elementary Functions*, Muller, cap. 7: `z_n → z_0 + y_0/x_0`). No se usa el modo hiperbólico.

El desarrollo se realiza en dos etapas:
1. **Modelado y validación en Python:** análisis del comportamiento numérico, rango de variables y precisión.
2. **Implementación en Assembly LC-3:** traducción de la lógica iterativa para integrarla con los demás módulos del benchmark.

---

## 2. Fundamento Matemático

El objetivo es hallar `Z` tal que `Y₀ − Z·X₀ = 0`. El algoritmo parte de `y = Y₀` y `z = 0`, y en cada iteración empuja `y` hacia cero con pasos binarios decrecientes (1, ½, ¼, ⅛, …), como una búsqueda binaria con signo. Lo que se resta de `y` se acumula en `z`, que termina conteniendo el cociente.

### Ecuaciones iterativas (para i = 0 … N−1)

```
d_i     = +1  si y_i <= 0
          -1  si y_i  > 0            (es decir, d = -signo(y))

x_(i+1) = x_i                         (el divisor no cambia)
y_(i+1) = y_i + d_i * (x_i >> i)      (y_i + d·x·2^-i)
z_(i+1) = z_i - d_i * (1.0 >> i)      (z_i - d·2^-i)
```

Con `1.0 = 32768` en Q15, el término `1.0 >> i` es `32768 >> i`.

### Propiedades
- **No necesita tabla de arctan:** los ángulos son simplemente `2⁻ⁱ`.
- **No necesita compensación de factor de escala:** en modo lineal `K = 1`.
- **x permanece constante** durante todas las iteraciones.

---

## 3. Formato de Punto Fijo (Q15)

- **Estructura:** 1 bit de signo y 15 bits fraccionarios (16 bits, compatible con la LC-3).
- **Factor de escala:** `2¹⁵ = 32768`.
- **Conversión:** `valor_Q15 = round(valor_real × 32768)`; `valor_real = valor_Q15 / 32768`.
- **Rango representable:** `[-1.0, 1.0)`, es decir, enteros de −32768 a 32767.

---

## 4. Variables de Entrada, Internas y de Salida

### Entradas

| Variable | Descripción | Formato | Restricción |
| :--- | :--- | :--- | :--- |
| `y0` | Dividendo (numerador) | Q15, entero con signo | Cualquier signo |
| `x0` | Divisor (denominador) | Q15, entero con signo | **`x0 > 0`** |
| `iteraciones` | Cantidad de iteraciones (opcional, por defecto 16) | Entero | Más iteraciones, más precisión |

**Condición de convergencia:** `|y0 / x0| < 2`.
**Para que el resultado quepa en Q15 (16 bits):** `|y0| < |x0|`, o sea cociente en `[-1, 1)`.

### Variables internas

| Variable | Descripción |
| :--- | :--- |
| `UNO_Q15` | Constante `1.0` en Q15 (`32768`). Base de los pasos `2⁻ⁱ`. |
| `xi` | Divisor. Se inicializa con `x0` y no cambia. |
| `yi` | Residuo. Se inicializa con `y0`; el algoritmo lo lleva hacia 0. |
| `zi` | Acumulador del cociente. Se inicializa en 0. |
| `d` | Dirección de la iteración: `+1` si `yi <= 0`, `-1` si `yi > 0`. |
| `i` | Índice de iteración (0 … N−1); también es el desplazamiento. |

### Salida

| Variable | Descripción | Formato |
| :--- | :--- | :--- |
| `zi` (valor retornado) | Cociente `y0 / x0` | Q15, entero con signo |

Para obtener el valor real: `cociente = zi / 32768`.

---

## 5. Código en Python

```python
def cordic_division_q15(y0, x0, iteraciones=16):
    """
    Calcula y0 / x0 en Q15 con CORDIC lineal (vectoring mode).
    Requisitos: x0 > 0 y |y0/x0| < 2 (|y0| < |x0| para que el
    resultado quepa en Q15).
    """
    UNO_Q15 = 1 << 15   # 1.0 en Q15

    xi = x0
    yi = y0
    zi = 0              # cociente

    for i in range(iteraciones):
        if yi <= 0:
            d = 1       # y negativo o cero: hay que subir y
        else:
            d = -1      # y positivo: hay que bajar y
        yi = yi + d * (xi >> i)
        zi = zi - d * (UNO_Q15 >> i)
    return zi
```

---

## 6. Resultados de Validación

Valores obtenidos con 16 iteraciones:

| Y / X | Real | CORDIC (Q15) | CORDIC (real) | Error |
| :---: | :---: | :---: | :---: | :---: |
| 0.4 / 0.8 | 0.50000 | 16383 | 0.49997 | 3.1e-05 |
| 0.1 / 0.9 | 0.11111 | 3639 | 0.11105 | 5.8e-05 |
| 0.5 / 0.9 | 0.55556 | 18203 | 0.55551 | 4.4e-05 |
| −0.3 / 0.6 | −0.50000 | −16385 | −0.50003 | 3.1e-05 |
| −0.2 / 0.7 | −0.28571 | −9359 | −0.28561 | 1.0e-04 |
| 0.0 / 0.7 | 0.00000 | −1 | −0.00003 | 3.1e-05 |

- **Precisión típica:** error del orden de 1 a 4 LSB (≈ 10⁻⁴).
- **Caso `y0 = 0`:** no da exactamente 0, sino ±1 LSB, porque CORDIC no tiene la opción `d = 0` y siempre da un paso hacia un lado.
- **Divisores muy pequeños:** el error aumenta (en pruebas aleatorias con `x0 ≥ 1000` llegó a ≈ 4·10⁻³), porque `x >> i` pierde bits. Conviene normalizar el divisor si puede ser pequeño.

---

## 7. Consideraciones para la Implementación en LC-3

- **Sin desplazamiento a la derecha nativo:** la LC-3 no tiene instrucción de shift. `x >> i` y `1.0 >> i` deben implementarse con una tabla de potencias de 2 precalculada, o con un bucle de divisiones sucesivas por 2.
- **Constante 32768:** en 16 bits con signo equivale a `0x8000` (−32768). Como `+32768` y `−32768` coinciden módulo 2¹⁶, la aritmética en complemento a 2 da el resultado correcto, ya que el valor final está en `[-1, 1)`.
- **Signo del divisor:** el algoritmo asume `x0 > 0`. Si el divisor puede ser negativo, hay que invertir el signo de ambos operandos antes de iterar.
- **Rango del cociente:** si el cociente puede ser ≥ 1, hay que normalizar (desplazar el dividendo) y corregir el resultado al final.

---

## 8. Bibliografía y Referencias
* **Muller, J.-M.** *Elementary Functions: Algorithms and Implementation*, cap. 7: *The CORDIC Algorithm* (Tablas 7.1 y 7.2).
* **Patt, Y. N., & Patel, S. J.** *Introduction to Computing Systems: From Bits & Gates to C/C++ & Beyond* (3rd ed.). McGraw-Hill.
* **Volder, J. E.** (1959). *The CORDIC Computing Technique*. IRE Transactions on Electronic Computers.
* **Guía Oficial de Cátedra:** *Laboratorio N° 3c - Desarrollo del Benchmark*, Facultad de Ingeniería, UNaM.