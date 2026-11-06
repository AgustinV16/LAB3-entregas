; Laboratorio 3a - Assembly, punto 3: conversion de formato Q, suma y resta
; NUM1 en Q3.12 (3.5  * 4096 = 14336)
; NUM2 en Q7.8  (1.25 * 256  = 320)
; Conversion: Q7.8 -> Q3.12 = multiplicar por 2^(12-8) = 16 (desplazar 4 bits a la izquierda).
; La LC-3 no tiene instruccion de shift, asi que se duplica el valor 4 veces con ADD Rx,Rx,Rx.
; Resultado: SUMA y RESTA en Q3.12 (rango representable [-8, 8)).
; Los datos van DESPUES del HALT para que la LC-3 no los ejecute como instrucciones.

        .ORIG x3000

        LD   R1, NUM2          ; R1 = NUM2 (Q7.8)
        ADD  R1, R1, R1        ; x2
        ADD  R1, R1, R1        ; x4
        ADD  R1, R1, R1        ; x8
        ADD  R1, R1, R1        ; x16 -> R1 = NUM2 alineado a Q3.12

        LD   R2, NUM1          ; R2 = NUM1 (Q3.12)

        ADD  R3, R2, R1        ; R3 = NUM1 + NUM2_alineado   (suma, Q3.12)
        ST   R3, SUMA

        NOT  R4, R1            ; resta = NUM1 + (-NUM2_alineado)
        ADD  R4, R4, #1        ; R4 = -NUM2_alineado (complemento a 2)
        ADD  R5, R2, R4        ; R5 = NUM1 - NUM2_alineado   (resta, Q3.12)
        ST   R5, RESTA

        HALT

NUM1    .FILL #14336           ; 3.5  en Q3.12
NUM2    .FILL #320             ; 1.25 en Q7.8
SUMA    .BLKW 1                ; esperado: 19456 (4.75)
RESTA   .BLKW 1                ; esperado:  9216 (2.25)

        .END
