

;
;   Programa principal, para probar el funcionamiento de la subrutina MAIN_MENU.
;   (Se puede usar la subrutina MAIN_MENU desde otro programa que no sea este si es necesario).
;



.EXTERNAL MAIN_MENU-MAIN_MENU

.ORIG x3000

    JSR MAIN_MENU-MAIN_MENU
    HALT


.END