; =====================================================================
;  LABORATORIO 3a - LC-3 : puntos 3 y 4 en assembly
;
;  Notacion: Qm.n = 1 bit de signo + m enteros + n fraccionarios (m+n = 15)
;     A esta en memoria en Q3.12  (1.0 = 4096)
;     B esta en memoria en Q7.8   (1.0 = 256)
;
;  Punto 3 : pasar de un formato Q a otro, sumar y restar
;     - Q3.12 -> Q7.8 : desplazamiento a la DERECHA de 4 (redondeado)
;     - Q7.8 -> Q3.12 : desplazamiento a la IZQUIERDA de 4 (satura)
;     - suma y resta se hacen en Q7.8 (ambos con 8 bits fraccionarios)
;  Punto 4 : producto A*B
;     - A*B tiene 12+8 = 20 bits fraccionarios y ocupa hasta 31 bits
;     - se calcula en 32 bits (hi:lo) y se ajusta a Q7.8 con un
;       desplazamiento a la DERECHA de 12 (redondeado)
;
;  La LC-3 no tiene desplazamientos ni flag de acarreo ni multiplicador.
;  Todo se arma con sumas, y los desplazamientos a la izquierda se
;  hacen como ADD R,R,R.
;
;  Todas las subrutinas reciben/devuelven en R0 (y R1) y preservan el
;  resto de los registros (cada una guarda y restaura lo que usa).
;
;  Resultados en memoria al terminar (ver etiquetas):
;     A_EN_Q8 , B_EN_Q12 , SUMA , RESTA , PRODUCTO
; =====================================================================
        .ORIG x3000

; ---------------------------- PROGRAMA PRINCIPAL ---------------------
        LD   R0, A_Q12
        JSR  Q12_A_Q8            ; R0 = A en Q7.8
        ST   R0, A_EN_Q8

        LD   R0, B_Q8
        JSR  Q8_A_Q12            ; R0 = B en Q3.12
        ST   R0, B_EN_Q12

        LD   R0, A_EN_Q8         ; A (ya en Q7.8)
        LD   R1, B_Q8            ; B (Q7.8)
        JSR  SUMA_Q8             ; R0 = A + B  (Q7.8)
        ST   R0, SUMA

        LD   R0, A_EN_Q8
        LD   R1, B_Q8
        JSR  RESTA_Q8            ; R0 = A - B  (Q7.8)
        ST   R0, RESTA

        LD   R0, A_Q12           ; A en Q3.12
        LD   R1, B_Q8            ; B en Q7.8
        JSR  MUL_Q12_Q8          ; R0 = A * B  (Q7.8)
        ST   R0, PRODUCTO
        HALT

; --- Datos de entrada, ya formateados ---------------------------------
;     A =  3.14159  -> 3.14159 * 4096 = 12868   (x3244)  en Q3.12
;     B = -5.75     -> -5.75   * 256  = -1472   (xFA40)  en Q7.8
A_Q12       .FILL #12868
B_Q8        .FILL #-1472

; --- Resultados ---------------------------------------------------------
A_EN_Q8     .BLKW #1             ; A convertido a Q7.8
B_EN_Q12    .BLKW #1             ; B convertido a Q3.12
SUMA        .BLKW #1             ; A + B en Q7.8
RESTA       .BLKW #1             ; A - B en Q7.8
PRODUCTO    .BLKW #1             ; A * B en Q7.8


; =====================================================================
; Q12_A_Q8 : R0 = R0 en Q3.12  ->  R0 en Q7.8
;   Se descartan 12-8 = 4 bits:  R0 = (|R0| + 8) >> 4, con el signo
;   original. El +8 (= 2^3) es "medio LSB" del bit que se descarta y
;   hace que se redondee al mas cercano en vez de truncar.
;   El desplazamiento a la derecha se arma tomando los 12 bits altos de
;   |R0| de MSB a LSB y reconstruyendolos en otro registro.
; =====================================================================
Q12_A_Q8
        ST   R1, Q1_R1
        ST   R2, Q1_R2
        ST   R3, Q1_R3
        AND  R3, R3, #0          ; R3 = bandera de signo
        ADD  R0, R0, #0
        BRzp Q1_POS
        NOT  R0, R0
        ADD  R0, R0, #1          ; |x|  (-32768 queda como 32768 sin signo)
        ADD  R3, R3, #1
Q1_POS  ADD  R0, R0, #8          ; + 2^3 (redondeo)
        AND  R1, R1, #0          ; R1 = resultado
        AND  R2, R2, #0
        ADD  R2, R2, #12         ; 12 bits altos
Q1_LP   ADD  R1, R1, R1          ; resultado <<= 1
        ADD  R0, R0, #0
        BRzp Q1_B0               ; bit 15 de R0 = 0 ?
        ADD  R1, R1, #1          ; bit 15 = 1 -> entra al resultado
Q1_B0   ADD  R0, R0, R0          ; R0 <<= 1  (sigue el proximo bit)
        ADD  R2, R2, #-1
        BRp  Q1_LP
        ADD  R0, R1, #0
        ADD  R3, R3, #0
        BRz  Q1_FIN
        NOT  R0, R0
        ADD  R0, R0, #1          ; reponer el signo
Q1_FIN  LD   R3, Q1_R3
        LD   R2, Q1_R2
        LD   R1, Q1_R1
        RET
Q1_R1   .BLKW #1
Q1_R2   .BLKW #1
Q1_R3   .BLKW #1


; =====================================================================
; Q8_A_Q12 : R0 = R0 en Q7.8  ->  R0 en Q3.12
;   Se ganan 4 bits fraccionarios: R0 = |R0| << 4 con el signo original.
;   Como Q3.12 solo llega a +-8, si |R0| >= 2048 (= 8.0 en Q7.8) no cabe
;   y se satura a +-32767.
; =====================================================================
Q8_A_Q12
        ST   R1, Q2_R1
        ST   R3, Q2_R3
        AND  R3, R3, #0          ; R3 = bandera de signo
        ADD  R0, R0, #0
        BRzp Q2_POS
        NOT  R0, R0
        ADD  R0, R0, #1          ; |x|
        ADD  R3, R3, #1
Q2_POS  ADD  R0, R0, #0
        BRn  Q2_SAT              ; era -32768 : no cabe
        LD   R1, Q2_NEG2048
        ADD  R1, R0, R1          ; |x| - 2048
        BRzp Q2_SAT              ; |x| >= 2048 : no cabe en Q3.12
        ADD  R0, R0, R0          ; << 1
        ADD  R0, R0, R0          ; << 2
        ADD  R0, R0, R0          ; << 3
        ADD  R0, R0, R0          ; << 4
        BR   Q2_SGN
Q2_SAT  LD   R0, Q2_MAX
Q2_SGN  ADD  R3, R3, #0
        BRz  Q2_FIN
        NOT  R0, R0
        ADD  R0, R0, #1
Q2_FIN  LD   R3, Q2_R3
        LD   R1, Q2_R1
        RET
Q2_R1      .BLKW #1
Q2_R3      .BLKW #1
Q2_NEG2048 .FILL #-2048
Q2_MAX     .FILL #32767


; =====================================================================
; SUMA_Q8 : R0 = R0 + R1   (los dos en el mismo formato Q)
;   Se suma en 16 bits. Hay desborde solo si los dos operandos tienen
;   el mismo signo y el resultado tiene signo distinto; en ese caso se
;   satura a 32767 o -32768.
; =====================================================================
SUMA_Q8
        ST   R2, SU_R2
        ADD  R2, R0, R1          ; R2 = suma
        ADD  R0, R0, #0
        BRn  SU_NEG
        ADD  R1, R1, #0          ; R0 >= 0
        BRn  SU_OK               ; signos distintos: no hay desborde
        ADD  R2, R2, #0          ; ambos >= 0: desborde si la suma < 0
        BRzp SU_OK
        LD   R2, SU_MAX
        BR   SU_OK
SU_NEG  ADD  R1, R1, #0          ; R0 < 0
        BRzp SU_OK               ; signos distintos: no hay desborde
        ADD  R2, R2, #0          ; ambos < 0: desborde si la suma >= 0
        BRn  SU_OK
        LD   R2, SU_MIN
SU_OK   ADD  R0, R2, #0
        LD   R2, SU_R2
        RET
SU_R2   .BLKW #1
SU_MAX  .FILL #32767
SU_MIN  .FILL #-32768


; =====================================================================
; RESTA_Q8 : R0 = R0 - R1   (los dos en el mismo formato Q)
;   R0 - R1 = R0 + (-R1).  Hay desborde solo si los signos de R0 y R1
;   son distintos y el signo del resultado difiere del de R0.
; =====================================================================
RESTA_Q8
        ST   R2, RE_R2
        NOT  R2, R1
        ADD  R2, R2, #1          ; R2 = -R1
        ADD  R2, R0, R2          ; R2 = R0 - R1
        ADD  R0, R0, #0
        BRn  RE_NEG
        ADD  R1, R1, #0          ; R0 >= 0
        BRzp RE_OK               ; mismo signo: no hay desborde
        ADD  R2, R2, #0          ; R0 >= 0, R1 < 0: debe dar positivo
        BRzp RE_OK
        LD   R2, RE_MAX
        BR   RE_OK
RE_NEG  ADD  R1, R1, #0          ; R0 < 0
        BRn  RE_OK               ; mismo signo: no hay desborde
        ADD  R2, R2, #0          ; R0 < 0, R1 >= 0: debe dar negativo
        BRn  RE_OK
        LD   R2, RE_MIN
RE_OK   ADD  R0, R2, #0
        LD   R2, RE_R2
        RET
RE_R2   .BLKW #1
RE_MAX  .FILL #32767
RE_MIN  .FILL #-32768


; =====================================================================
; MUL_Q12_Q8 : R0 = R0 (Q3.12) * R1 (Q7.8)  ->  R0 en Q7.8
;   1) se trabaja con los modulos y se cuenta cuantos operandos son
;      negativos (el producto es negativo si es impar)
;   2) MUL32 calcula |A|*|B| + 2048 en 32 bits como  hi*2^15 + lo
;      (el +2048 = 2^11 es medio LSB del bit que se descarta)
;   3) se descartan 12 bits (Q3.12 x Q7.8 -> 20 fraccionarios -> 8):
;         (hi*2^15 + lo) >> 12  =  hi*8 + (lo >> 12)
;      lo >> 12 se obtiene restando 4096 hasta que no se pueda (<= 7 veces)
;   4) si hi >= 4096 el resultado no entra en 16 bits: se satura a 32767
; =====================================================================
MUL_Q12_Q8
        ST   R1, MM_R1
        ST   R2, MM_R2
        ST   R3, MM_R3
        ST   R4, MM_R4
        ST   R5, MM_R5
        ST   R6, MM_R6
        ST   R7, MM_R7
        AND  R2, R2, #0          ; R2 = cantidad de operandos negativos
        ADD  R0, R0, #0
        BRzp MM_A
        NOT  R0, R0
        ADD  R0, R0, #1          ; |A|
        ADD  R2, R2, #1
MM_A    ADD  R0, R0, #0
        BRzp MM_B0
        LD   R0, MM_MAX15        ; |-32768| no cabe en 15 bits: usar 32767
MM_B0   ADD  R1, R1, #0
        BRzp MM_B
        NOT  R1, R1
        ADD  R1, R1, #1          ; |B|
        ADD  R2, R2, #1
MM_B    ADD  R1, R1, #0
        BRzp MM_C
        LD   R1, MM_MAX15
MM_C    ST   R2, MM_SGN
        LD   R2, MM_HALF         ; lo inicial = 2048 (redondeo)
        JSR  MUL32               ; R3 = hi , R2 = lo
        LD   R5, MM_NEG4096
        ADD  R4, R3, R5          ; hi - 4096
        BRzp MM_SAT              ; hi >= 4096 : no cabe en Q7.8
        ADD  R0, R3, R3          ; hi * 2
        ADD  R0, R0, R0          ; hi * 4
        ADD  R0, R0, R0          ; hi * 8
MM_DIV  ADD  R2, R2, R5          ; lo - 4096
        BRn  MM_SGNQ
        ADD  R0, R0, #1          ; suma 1 por cada 4096 que entra en lo
        BR   MM_DIV
MM_SAT  LD   R0, MM_MAX15        ; saturacion
MM_SGNQ LD   R2, MM_SGN
        AND  R2, R2, #1          ; negativos impar -> resultado negativo
        BRz  MM_FIN
        NOT  R0, R0
        ADD  R0, R0, #1
MM_FIN  LD   R7, MM_R7
        LD   R6, MM_R6
        LD   R5, MM_R5
        LD   R4, MM_R4
        LD   R3, MM_R3
        LD   R2, MM_R2
        LD   R1, MM_R1
        RET
MM_R1      .BLKW #1
MM_R2      .BLKW #1
MM_R3      .BLKW #1
MM_R4      .BLKW #1
MM_R5      .BLKW #1
MM_R6      .BLKW #1
MM_R7      .BLKW #1
MM_SGN     .BLKW #1
MM_HALF    .FILL #2048
MM_NEG4096 .FILL #-4096
MM_MAX15   .FILL #32767


; =====================================================================
; MUL32 : producto sin signo de 32 bits
;   Entrada : R0, R1 = modulos (0..32767)     R2 = valor inicial de lo
;   Salida  : R3 = hi , R2 = lo      (R2 inicial + R0*R1 = hi*2^15 + lo)
;   Destruye R0, R1, R4, R5, R6.
;
;   Es la multiplicacion "a mano": por cada bit en 1 del multiplicador
;   (R1) se suma el multiplicando (R0) corrido a su posicion. Como la
;   suma no cabe en 16 bits, el acumulador y el multiplicando se
;   guardan en dos mitades de 15 bits (hi, lo). Una suma de dos mitades
;   de 15 bits nunca desborda 16 bits y el acarreo es el bit 15, que se
;   limpia y se pasa a la mitad alta (la LC-3 no tiene flag de acarreo).
;   El multiplicador menor queda en R1 para hacer menos vueltas, y el
;   bucle termina cuando la mascara supera al multiplicador.
; =====================================================================
MUL32
        ST   R7, M32_R7
        NOT  R5, R1              ; dejar el menor en R1
        ADD  R5, R5, #1
        ADD  R5, R0, R5
        BRzp M32_OK
        ADD  R5, R0, #0
        ADD  R0, R1, #0
        ADD  R1, R5, #0
M32_OK  ADD  R6, R0, #0          ; a_lo = multiplicando (mitad baja)
        AND  R0, R0, #0          ; a_hi = 0           (mitad alta)
        AND  R3, R3, #0          ; acc_hi = 0
        AND  R4, R4, #0
        ADD  R4, R4, #1          ; mascara = bit 0
        LD   R7, M32_M15         ; R7 = x7FFF
M32_LP  AND  R5, R1, R4          ; bit actual del multiplicador ?
        BRz  M32_NA
        ADD  R2, R2, R6          ; acc_lo += a_lo
        BRzp M32_NC
        AND  R2, R2, R7          ; acarreo: limpiar bit 15
        ADD  R3, R3, #1          ; y sumarlo a acc_hi
M32_NC  ADD  R3, R3, R0          ; acc_hi += a_hi
M32_NA  ADD  R6, R6, R6          ; (a_hi, a_lo) <<= 1
        BRzp M32_N2
        AND  R6, R6, R7          ; sobro un bit: pasa a la mitad alta
        ADD  R0, R0, R0
        ADD  R0, R0, #1
        BR   M32_N3
M32_N2  ADD  R0, R0, R0
M32_N3  ADD  R4, R4, R4          ; mascara <<= 1
        NOT  R5, R4
        ADD  R5, R5, #1
        ADD  R5, R1, R5          ; multiplicador - mascara
        BRzp M32_LP              ; seguir mientras mascara <= multiplicador
        LD   R7, M32_R7
        RET
M32_R7  .BLKW #1
M32_M15 .FILL #32767

        .END
