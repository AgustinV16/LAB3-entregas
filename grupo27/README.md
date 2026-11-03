# Grupo 27 · Interfaz para el banco de algoritmos / calculadora

**Consigna:** Debe poder dar formato de decimal a binario con punto fijo y cambiar entre modo entrada por teclado y pasaje por memoria de los datos para calcular los resultados de cada algoritmo

## Integrantes
- Zimmermann, Miguel Angel (@ZimmerMaster26)
- Espinola, Tobias (@T0bias1)
- Gonzalez, Nahuel Sebastián (@Acelerator909)

## Archivos
- `main.asm`: Programa principal (Se usa como entrada para probar el funcionamiento).
- `main_menu.asm`: Subrutina principal de la interfaz del menu.
- `subrutinas/`: Carpeta con las subrutinas esenciales.
    - `circlsh.asm: Subrutina CIRCLSH ara rotar de manera circular dos regitstros, hacia la izquierda.
    - `fromdec.asm`: Subrutina FROMDEC para pasaje de texto decimal a valor en binario (entero y fraccionario, 1 WORD cada uno).
    - `fromhex.asm`: Subrutina FROMHEX para pasaje de texto hexadecimal a vaolr en binario (1 WORD).
    - `getcfilered.asm`: Subrutina GETCFILTERED para obtención de caracter por teclado unicamente si cumple sierto filtro.
    - `getline.asm`: Subrutina GETLINE para obtención de linea (hasta encontrar '\n').
    - `pause.asm`: Subrutina PAUSE que muestra por consola un prompt y continua ejecucion al pulsarce una tecla.
    - `sumover.asm`: Subrutina SUMOVER para sumar dos registros teniendo en cuenta un bit de acarreo (de entrada y salida).
    - `todec.asm`: Subrutina TODEC para pasaje de valor en binario (entero y fraccionario, 1 WORD cada uno) a texto decimal.
- `q_cast/`: Carpeta con las subrutinas de casteo.
    - `DQ16_to_Q8.asm`: Subrutina para pasaje de valor Q15.16 a Q7.8.
    - `DQ16_to_Q12.asm`: Subrutina para pasaje de valor Q15.16 a Q3.12.
    - `Q8_to_DQ16.asm`: Subrutina para pasaje de valor Q7.8 a Q15.16.
    - `Q12_to_DQ16.asm`: Subrutina para pasaje de valor Q3.12 a Q15.16.
- `val_io/`: Carpeta para subrutinas de obtencion y muestra de valores.
    - `getval_buffer.asm`: Buffer compartido.
    - `getval_int.asm`: Subrutina para obtencion de valor entero (acepta direcciones de texto para ciertas funcionalidades).
    - `getval_q8.asm`: Subrutina para obtencion de Q7.8 (acepta direcciones de texto para ciertas funcionalidades).
    - `getval_q12.asm`: Subrutina para obtencion de Q3.12 (acepta direcciones de texto para ciertas funcionalidades).
    - `getval.asm`: Subrutina para obtencion de Q15.16 (acepta direcciones de texto para ciertas funcionalidades).
    - `outval.asm`: Subrutina para muestra por consola de un valor (Q15.16).


Para probar el funcionamiento actual, simplemente se debe cargar el proyecto (todos los archivos dentro del directorio `main_menu/`) en el simulador.
Luego seleccionamos el archivo main.asm como ENTRY, pulsando en `...` y seleccionar como entrada.
Compilamos y ejecutamos.
