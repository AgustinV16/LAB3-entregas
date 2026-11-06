# Grupo 30 · SIN

**Consigna:** Por Chebyshev y Padé

## Integrantes
<!-- Completar en el primer Pull Request -->
- Meretz, Eliana Marife (@elimeretz)
- Meretz, Mariana Belen (@meretzmariana)

## Archivos
<!-- Qué archivo hace qué -->
- `seno_cheb_pade.asm`: implementación en Assembly de la función seno mediante el método híbrido Chebyshev-Padé.
- `seno_cheb_pade.c`: implementación en lenguaje C de la función seno mediante el método híbrido Chebyshev-Padé.
- `Informe-Chebyshev–Padé-Grupo 30.pdf`: informe general del Laboratorio 3.
- `informe_Lab3a.pdf`: informe correspondiente a la primera parte del Laboratorio 3 (Lab 3a).
- `lab3a_punto_fijo.ams`: implementación en Assembly del punto 3 y 4 del Lab 3a
- `lab3a_puntos_3_y_4.asm`: implementación en lenguaje C del punto 3 y 4 del Lab 3a


## Cómo probarlo
<!-- Pasos para ensamblar y ejecutar, y un ejemplo de entrada y salida -->
La implementación en Assembly se encuentra funcionando en el simulador desarrollado por el Grupo 8 y ya está publicada, por lo que puede ejecutarse desde allí.

### Formatos de punto fijo
- Entrada: Q3.12, con signo y 16 bits.
- Salida: Q1.14, con signo y 16 bits.

### Rangos de valores
- Entrada: desde -8 hasta aproximadamente 7,99976 radianes.
- Salida: desde -1 hasta aproximadamente 1.

###Resolución de los formatos
La resolución indica cuál es el menor incremento que puede representarse entre dos valores consecutivos. Se calcula dividiendo 1 por 2 elevado a la cantidad de bits destinados a la parte fraccionaria.

Entrada (Q3.12): tiene 12 bits fraccionarios. Por lo tanto, su resolución es (1/2^12), equivalente a aproximadamente 0,00024414 radianes.
Salida (Q1.14): tiene 14 bits fraccionarios. Su resolución es (1/2^14), equivalente a aproximadamente 0,00006104.

## Bitácora de desarrollo
En esta sección se detallan las actividades realizadas durante el desarrollo del Lab 3, incluyendo el análisis del problema, la implementación de la función asignada, las dificultades encontradas y la coordinación con los demás grupos.

### 7 de octubre de 2026 – Inicio del laboratorio
Comenzamos a trabajar con el Lab 3a y el TP2. Para avanzar, leímos los apuntes de la cátedra sobre números en punto fijo y empezamos a practicar operaciones utilizando distintos formatos Q.
Estos contenidos fueron importantes porque necesitábamos representar números reales mediante valores enteros para poder realizar los cálculos en el procesador LC-3.
También comenzamos a desarrollar código en Assembly con ayuda de herramientas de inteligencia artificial. Durante este proceso realizamos distintas pruebas y correcciones para detectar errores.
Una de las principales dificultades fue comprender cómo realizar multiplicaciones y divisiones utilizando distintos formatos Q. Además, tuvimos dificultades con la depuración del código, ya que no teníamos mucha experiencia realizando este tipo de tareas.

### 14 de octubre de 2026 – Asignación de la función seno
Se nos asignó el cálculo de la función seno (`SIN`) utilizando el método híbrido Chebyshev-Padé.
A partir de esta asignación comenzamos a investigar y comprender cómo funciona esta aproximación matemática y cómo podíamos llevarla a lenguaje Assembly para ejecutarla en el procesador LC-3.
El principal desafío era adaptar los cálculos matemáticos a las posibilidades de la arquitectura, teniendo en cuenta el uso de números en punto fijo y las operaciones disponibles.

### 21 de octubre de 2026 – Puesta en común de los algoritmos
Comenzamos a compartir con los demás grupos los programas y algoritmos que cada uno debía implementar en Assembly para integrar el *workload* del laboratorio.
Esta actividad nos permitió conocer cómo se estaban desarrollando las distintas funciones y comprender la importancia de establecer criterios comunes para que todos los programas pudieran integrarse correctamente.
También relacionamos este trabajo con los contenidos vistos en clase.

### 28 de octubre de 2026 – Organización del trabajo con Git y GitHub
Creamos nuestras cuentas de Git y GitHub para organizar el trabajo colaborativo y subir los archivos correspondientes al proyecto.
Esto nos permitió comenzar a organizar el código Assembly, el informe y los demás archivos necesarios para la entrega del laboratorio.

### 29 de octubre de 2026 – Finalización del proyecto e informe.
Trabajamos en la finalización del proyecto y en la elaboración del informe.
En esta etapa organizamos la información sobre el método utilizado, los formatos de punto fijo, las operaciones necesarias y el procedimiento desarrollado para calcular la función seno.
También tuvimos en cuenta los contenidos de arquitectura estudiados durante la cursada, especialmente las características de la ISA LC-3 y las instrucciones disponibles para implementar nuestro algoritmo.

### 1 de noviembre de 2026 – Intercambio con otros grupos
Nos comunicamos con otros grupos que estaban trabajando en funciones trigonométricas, como seno y coseno, para consultar qué formatos de punto fijo estaban utilizando.
El objetivo fue compartir información y conocer cómo habían resuelto la representación de los números, ya que era importante mantener cierta compatibilidad entre las funciones que luego formarían parte del mismo sistema.

### 2 de noviembre de 2026 – Coordinación con el grupo de la interfaz
Nos comunicamos con el grupo 27, encargado del desarrollo de la interfaz, para acordar cómo se integrarían los distintos programas.
Durante la comunicación conversamos sobre el diseño de la interfaz, los registros que utilizaríamos y los formatos numéricos que debíamos respetar para que las funciones pudieran comunicarse correctamente.
Además, solicitamos al Ing. Carballo un usuario y una contraseña para acceder al ensamblador disponible en `lgrum.xyz`, con el objetivo de contar con acceso a esta herramienta para trabajar con nuestros programas Assembly y realizar las modificaciones necesarias.

### 3 de noviembre de 2026 – Consulta al tutor
Fuimos a buscar al tutor para presentarle los avances realizados y consultar si el trabajo estaba encaminado correctamente, pero no se encontraba disponible.
Por este motivo, dejamos pendiente la revisión para el día siguiente.

### 4 de noviembre de 2026 – Presentación y aprobación de los avances
Presentamos al tutor los avances realizados durante el desarrollo del laboratorio.
Luego de revisar el trabajo, nos dio el visto bueno para subir los archivos del proyecto. No solicitó cambios ni modificaciones, por lo que pudimos continuar con la preparación de la entrega.

### 5 de noviembre de 2026 – Presentación del LABORATORIO 3
Realizamos la carga de los archivos correspondientes al Grupo 30 en el repositorio de GitHub. Subimos los informes y los códigos desarrollados en lenguaje C y Assembly, correspondientes a las dos partes del Laboratorio 3.
También subimos al ensamblador del Grupo 8 los códigos en Assembly correspondientes al cálculo de la función seno mediante el método híbrido Chebyshev-Padé, donde quedaron disponibles para su ejecución y prueba.
