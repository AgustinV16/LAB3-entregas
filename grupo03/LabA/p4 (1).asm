; Laboratorio 3a - Assembly, punto 4: multiplicacion Q3.12 x Q7.8 -> Q3.12
; NUM1 en Q3.12 (3.5  * 4096 = 14336)
; NUM2 en Q7.8  (1.25 * 256  = 320)
;
; El producto crudo NUM1*NUM2 tiene 12+8 = 20 bits fraccionarios y puede NO entrar en
; 16 bits (14336*256 = 3670016 > 65535). El ajuste a Q3.12 es dividir por 256 (>> 8).
; Para no perder bits, el producto se mantiene como  Q*256 + R :
;   Q = parte alta (ya ajustada, es el resultado en Q3.12)
;   R = 8 bits bajos descartados (0..255)
; Para eso se parte el multiplicando:  NUM1 = Ah*256 + Al  (Al = NUM1 AND xFF, 0..255)
; y por cada vuelta del bucle:  Q += Ah ;  R += Al ;  si R >= 256 entonces R -= 256, Q += 1.
; Al terminar, Q = piso(NUM1*NUM2 / 256) exacto.  Rango del resultado: [-8, 8).
;
; Registros: R1=NUM1  R2=NUM2 (contador)  R3=Q  R4=R  R5=Al  R6=Ah  R0=auxiliar

        .ORIG x3000

        LD   R1, NUM1
        LD   R2, NUM2          ; los flags quedan segun NUM2
        BRzp SIGNO_OK          ; si NUM2 < 0: NUM1*NUM2 = (-NUM1)*(-NUM2)
        NOT  R2, R2
        ADD  R2, R2, #1        ; R2 = -NUM2 (ahora >= 0)
        NOT  R1, R1
        ADD  R1, R1, #1        ; R1 = -NUM1

; ---- partir NUM1 en parte baja Al y parte alta Ah ----
SIGNO_OK LD  R0, MASK_LO
        AND  R5, R1, R0        ; R5 = Al = NUM1 AND x00FF
        LD   R0, MASK_HI
        AND  R6, R1, R0        ; R6 = Ah*256 = NUM1 AND xFF00
        AND  R4, R4, #0        ; R4 = contador = 0
        ADD  R6, R6, #0        ; flags segun Ah*256
        BRz  AH_FIN
        BRn  AH_NEG
AH_POS  ADD  R4, R4, #1        ; Ah*256 > 0: restar 256 hasta llegar a 0
        LD   R0, NEG_256
        ADD  R6, R6, R0
        BRp  AH_POS
        BRnzp AH_FIN
AH_NEG  ADD  R4, R4, #-1       ; Ah*256 < 0: sumar 256 hasta llegar a 0
        LD   R0, POS_256
        ADD  R6, R6, R0
        BRn  AH_NEG
AH_FIN  ADD  R6, R4, #0        ; R6 = Ah (con signo)
        AND  R4, R4, #0        ; R4 = R = 0
        AND  R3, R3, #0        ; R3 = Q = 0

; ---- multiplicacion por sumas sucesivas (NUM2 veces) ----
        ADD  R2, R2, #0        ; flags segun el contador
MULT_LOOP BRnz MULT_FIN        ; contador = 0 -> terminar
        ADD  R3, R3, R6        ; Q = Q + Ah
        ADD  R4, R4, R5        ; R = R + Al
        LD   R0, NEG_256
        ADD  R0, R4, R0        ; R0 = R - 256
        BRn  SIN_ACARREO
        ADD  R4, R0, #0        ; R = R - 256
        ADD  R3, R3, #1        ; Q = Q + 1  (acarreo hacia la parte alta)
SIN_ACARREO ADD R2, R2, #-1    ; contador--
        BRnzp MULT_LOOP

MULT_FIN ST  R3, RESULTADO     ; resultado ajustado en Q3.12
        ST   R4, RESTO         ; bits fraccionarios descartados (solo informativo)
        HALT

NUM1      .FILL #14336         ; 3.5  en Q3.12
NUM2      .FILL #320           ; 1.25 en Q7.8
MASK_LO   .FILL x00FF
MASK_HI   .FILL xFF00
NEG_256   .FILL #-256
POS_256   .FILL #256
RESULTADO .BLKW 1              ; esperado: 17920 (4.375)
RESTO     .BLKW 1              ; esperado: 0

        .END
