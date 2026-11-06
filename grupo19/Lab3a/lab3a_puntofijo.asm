.ORIG x3000
    ; CARGA DE DATOS
    LD R1, A_Q12    ; A en Q3.12
    LD R2, B_Q8     ; B en Q7.8

    ; ALINEACIÓN DE FORMATOS
    ; B_Q8 de Q7.8 a Q3.12 mediante 4 desplazamientos a la izquierda
    ADD R3, R2, R2
    ADD R3, R3, R3
    ADD R3, R3, R3
    ADD R3, R3, R3
    ST R3, B_Q12    ; Guardo el valor de B ya alineado a Q3.12

    ; --- MULTIPLICACIÓN ---
    ; A (Q3.12) * B (Q7.8) mediante sumas sucesivas.
    ; Uso R1 como base a sumar y R2 como contador.
    AND R0, R0, #0  ; Acumulador, comienza en 0
    ADD R4, R2, #0  ; Copia de B_Q8 -> contador del bucle

MULT_LOOP
    ADD R0, R0, R1  ; Acumulo A en R0
    ADD R4, R4, #-1 ; Decrementa el contador
    BRp MULT_LOOP   ; Se repite mientras sea positivo el resultado

    ST R0, RES_MULT ; Guardo el producto resultante

    ; --- DIVISIÓN ---
    ; A (Q3.12) / B (Q7.8) mediante restas sucesivas.
    ; El resultado natural tendrá 4 bits fraccionarios (Q11.4).
    
    ; Obtengo el divisor negativo (-B) con complemento a 2
    NOT R5, R2
    ADD R5, R5, #1

    AND R6, R6, #0  ; Cociente, empieza en 0
    ADD R7, R1, #0  ; Copia de A_Q12 --> Dividendo

DIV_LOOP
    ADD R7, R7, R5  ; Resto el divisor al dividendo
    BRn FIN_DIV     ; Si el resultado es negativo, se pasó y termina
    
    ADD R6, R6, #1  ; Si no se pasó, se suma 1 al cociente
    BRnzp DIV_LOOP  ; Se repite incondicionalmente

FIN_DIV
    ST R6, RES_DIV  ; Guardo el cociente final en memoria

; VARIABLES Y DATOS EN MEMORIA
A_Q12       .FILL x0200     ; Valor de A en Q3.12 (0.125) --> 512
B_Q8        .FILL x0014     ; Valor de B en Q7.8 (0.078125) --> 20

B_Q12       .BLKW #1         ; Espacio reservado para B alineado
RES_MULT    .BLKW #1         ; Espacio reservado para el Producto
RES_DIV     .BLKW #1         ; Espacio reservado para el Cociente

.END