# Primera etapa del proyecto

* **B86524 - Gabriel Revillat Zeledón**

El programa se encuentra en el directorio `hot_potato_game`.

Incluye un Makefile para compilarlo. En caso de no usar el Makefile, compilar de esta forma:

```
gcc -g -std=c11 -Wall -Wextra hot_potato_game.c -o hot_potato_game -lpthread
```

El programa recibe tres argumentos por linea de comandos:
* `player_count`: El número de jugadores de la ronda.
* `initial_potato_id`: El valor inicial para la papa.
* `direction`: El sentido de rotación del juego. Introducir 0 para rotar a la izquierda y 1 o cualquier otro número para rotar a la derecha.

Ejemplo:

```
./hot_potato_game 4 40 1
```