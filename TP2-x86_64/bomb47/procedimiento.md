# Resolución de la Fase 4

La fase 4 solicita una cadena de **6 caracteres**. Analizando el código ensamblador, se identificó que:

- Cada caracter del string es transformado con `ord(c) & 0xF`, es decir, se toman los **4 bits menos significativos** de su código ASCII.
- Ese valor (entre 0 y 15) se usa como **índice** en un arreglo de 16 enteros llamado `array.0`.
- Se suman los 6 valores obtenidos del arreglo.
- Si la suma no es exactamente **60**, la bomba explota.

El arreglo `array.0` fue leído con GDB y tiene los siguientes valores:

```python
[2, 13, 7, 14, 5, 10, 6, 15, 1, 12, 3, 4, 11, 8, 16, 9]
 ```

Para resolver la fase, se generaron combinaciones posibles de caracteres cuyos índices en el arreglo sumaran 60.

Una solución válida encontrada fue:

  !!#.
(Espacio, espacio, exclamación, exclamación, numeral, punto)

Al ingresar esa cadena como input, se desactiva correctamente la bomba en la fase 4.
