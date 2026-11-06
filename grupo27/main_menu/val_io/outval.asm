

.EXTERNAL GETVAL_BUFFER-BUFFER

.EXTERNAL TODEC-TODEC

;
;   Subrutina para mostrar un valor por pantalla en formato decimal.
;

OUTVAL

    ; [R0] entero
    ; [R1] fraccionario

    ST R3 outval_r3                     ;
    ST R4 outval_r4                     ;
    ST R7 outval_r7                     ;

    LEA R2 GETVAL_BUFFER-BUFFER         ;
    AND R3 R3 #0                        ;
    JSR TODEC-TODEC                     ;
    LEA R0 GETVAL_BUFFER-BUFFER         ;
    PUTS                                ;   print(todec(value))
    AND R0 R0 #0                        ;
    ADD R0 R0 #10                       ;
    OUT                                 ;   print('\n')

    LD R3 outval_r3                     ;
    LD R4 outval_r4                     ;
    LD R7 outval_r7                     ;
    RET                                 ;

outval_r3       .FILL #0 
outval_r4       .FILL #0 
outval_r7       .FILL #0 