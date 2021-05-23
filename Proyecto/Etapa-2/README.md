# Segunda etapa del proyecto

* **B86524 - Gabriel Revillat Zeledón**

El programa se encuentra en el directorio `cache_simulator`.

Incluye un Makefile para compilarlo.

El programa recibe ocho argumentos por linea de comandos (aparte del nombre del programa):
* `num_of_sets`: El número de conjuntos en la cache. Debe ser positivo y potencia de 2.
* `num_of_set_blocks`: El número de bloques en cada conjunto. Debe ser positivo y potencia de 2.
* `num_of_block_bytes`: El número de bytes en cada bloque. Debe ser mayor o igual que 4 y potencia de 2.
* `write_policy_1`: Bandera de escritura #1. Puede ser *write-allocate* o *no-write-allocate*. En esta etapa solo se evalua el *no-write-allocate*.
* `write_policy_2`: Bandera de escritura #2. Puede ser *write-through* o *write-back*. En esta etapa solo se evalua el *write-through*.
* `replacement`: Algoritmo de reemplazo. Puede ser *lru*, *fifo* o *random*.
* `cache_access_cycles`: El número de ciclos de reloj que va a tomar un acceso a la cache. Debe ser positivo.
* `memory_access_cycles`: El número de ciclos de reloj que va a tomar un acceso a la memoria. Debe ser mayor que el número de ciclos de acceso a la cache.

Además de los argumentos por línea de comandos, se le debe pasar al programa el archivo de la traza usando el operador `<`.

Ejemplo:

```
/cache_simulator 1 4 4 no-write-allocate write-through lru 13 230 < trace1.txt
```

Se incluyen dos archivos de traza que se pueden usar para correr el programa.

Salida obtenida con los agrumentos del ejemplo anterior y el archivo trace1.txt:

```
l 0x00000000 243 miss
s 0x00000000 243 hit
l 0x00000000 13 hit
s 0x00000000 243 hit
l 0x00000000 13 hit
s 0x00000000 243 hit
l 0x00000000 13 hit
s 0x00000000 243 hit
l 0x00000000 13 hit
s 0x00000000 243 hit
l 0x00000000 13 hit
s 0x00000000 243 hit
l 0x00000000 13 hit
s 0x00000000 243 hit
l 0x00000000 13 hit
s 0x00000000 243 hit

Total loads: 0
Total stores: 0
Load hits: 7
Load misses: 1
Store hits: 8
Store misses: 0
Evictions: 0
Total CPU Cycles: 2278
```

Salida obtenida con los agrumentos del ejemplo anterior y el archivo trace2.txt:

```
l 0x12345678 243 miss
s 0x54643493 473 miss
l 0x34568955 243 miss
s 0x12453568 473 miss
l 0x45429837 473 miss
s 0x43524389 703 miss
l 0x87654321 473 miss
s 0x45429837 243 hit
l 0x87654321 13 hit

Total loads: 0
Total stores: 0
Load hits: 1
Load misses: 4
Store hits: 1
Store misses: 3
Evictions: 3
Total CPU Cycles: 3337
```