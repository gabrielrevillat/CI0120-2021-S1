/**
 * Programa que simula el juego de la papa caliente con el uso de pthreads.
 */

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

/**
 * Memoria compartida entre los hilos jugadores.
 */
typedef struct shared_data
{
    size_t player_count;
    int potato_id;
    bool rotating_right;
    bool playing;

    sem_t* player_turn;
    bool* active_players;
} shared_data_t;

/**
 * Datos privados para cada hilo jugador.
 */
typedef struct private_data
{
    size_t player_id;
    bool active;
    shared_data_t* shared_data;
} private_data_t;

/**
 * Comienza la ejecucion de un nuevo hilo.
 * 
 * @param data  Direccion de memoria de los datos privados de cada hilo.
 */
void* run(void* data);

/**
 * Verifica el numero de argumentos recibidos por linea de comandos 
 * y se asegura de que cada argumento sea valido.
 * 
 * @param shared_data   Memoria compartida de los hilos.
 * @param argc          El numero de argumentos recibidos por linea de comandos.
 * @param argv          Arreglo de argumentos recibidos por linea de comandos.
 */
int analyze_arguments(shared_data_t* shared_data, int argc, char* argv[]);

/**
 * Crea los hilos segun los atributos de @a shared_data 
 * y prepara la simulacion del juego de la papa caliente.
 * 
 * @param shared_data   Memoria compartida de los hilos.
 */
int create_threads(shared_data_t* shared_data);

/**
 * Le aplica las reglas de Collatz al valor de la papa caliente.
 * 
 * @param shared_data   Memoria compartida de los hilos.
 */
int change_potato_value(int potato_value);

/**
 * Verifica si en la partida quedan otros jugadores activos.
 * 
 * @param shared_data   Memoria compartida de los hilos.
 * @param id            Identificador del hilo que invoca a esta subrutina.
 * @return true si ya no quedan otros jugadores activos; de lo contrario false.
 */
bool no_more_players(shared_data_t* shared_data, size_t id);

/**
 * Subrutina principal. Comienza la ejecucion del programa.
 * 
 * @param argc  El numero de argumentos recibidos por linea de comandos.
 * @param argv  Arreglo de argumentos recibidos por linea de comandos.
 */
int main(int argc, char* argv[])
{
    int error = 0;

    srand(time(NULL));

    shared_data_t* shared_data = (shared_data_t*) calloc(1, sizeof(shared_data_t));

    if (shared_data)
    {
        error = analyze_arguments(shared_data, argc, argv);

        if (error == 0)
        {
            shared_data->playing = true;

            shared_data->player_turn = (sem_t*)
                calloc(shared_data->player_count, sizeof(sem_t));
            shared_data->active_players = (bool*)
                calloc(shared_data->player_count, sizeof(bool));

            error = create_threads(shared_data);

            free(shared_data->active_players);
            free(shared_data->player_turn);
            free(shared_data);
        }
    }
    else
    {
        fprintf(stderr, "Error: Could not allocate shared memory.\n");
        error = 11;
    }

    return error;
}

int analyze_arguments(shared_data_t* shared_data, int argc, char* argv[])
{
    int error = 0;

    if (argc >= 4)
    {

        if (sscanf(argv[1], "%zu", &shared_data->player_count) != 1
            || shared_data->player_count == 0)
        {
            fprintf(stderr, "Error: Invalid number of players.\n");
            error = 2;
        }
        else if (sscanf(argv[2], "%d", &shared_data->potato_id) != 1
            || shared_data->potato_id <= 1)
        {
            fprintf(stderr, "Error: Invalid potato value. Must be greater than 1.\n");
            error = 3;
        }
        else
        {
            int direction = 0;

            if (sscanf(argv[3], "%d", &direction) == 1)
            {
                shared_data->rotating_right = direction;
            }
            else
            {
                fprintf(stderr, "Error: Invalid value for direction.\n");
                error = 4;
            }
        }
    }
    else
    {
        fprintf(stderr, "Usage: hot_potato_game player_count "
                        "initial_potato_id direction\n"
                        "direction: 0 to rotate to left, "
                        "1 or any other number to rotate to right\n");
        error = 1;
    }

    return error;
}

int create_threads(shared_data_t* shared_data)
{
    int error = 0;

    pthread_t* players = (pthread_t*)
        malloc(shared_data->player_count * sizeof(pthread_t));
    
    private_data_t* private_data = (private_data_t*)
        calloc(shared_data->player_count, sizeof(private_data_t));

    if (players && private_data)
    {
        for (size_t index = 0; index < shared_data->player_count; ++index)
        {
            // Todos los semaforos empiezan bloqueados, excepto el 0.
            sem_init(&shared_data->player_turn[index], 0, !(index));
            shared_data->active_players[index] = true;

            private_data[index].player_id = index;
            private_data[index].active = true;
            private_data[index].shared_data = shared_data;

            error = pthread_create(&players[index], NULL, run, &private_data[index]);

            if (error != 0)
            {
                fprintf(stderr, "Error: Could not create thread %zu\n", index);
                error = 22;
            }
        }

        for (size_t index = 0; index < shared_data->player_count; ++index)
        {
            pthread_join(players[index], NULL);
            sem_destroy(&shared_data->player_turn[index]);
        }

        free(private_data);
        free(players);
    }
    else
    {
        fprintf(stderr,
                "Error: Could not allocate memory for %zu threads.\n",
                shared_data->player_count);
        error = 21;
    }

    return error;
}

void* run(void* data)
{
    private_data_t* private_data = (private_data_t*) data;
    shared_data_t* shared_data = private_data->shared_data;

    // Mientras el juego no haya terminado
    while (shared_data->playing)
    {
        // Esperar a que sea mi turno.
        sem_wait(&shared_data->player_turn[ private_data->player_id ]);

        if (private_data->active)
        {
            // Si ya no quedan otros jugadores activos, gano yo.
            if (no_more_players(shared_data, private_data->player_id))
            {
                printf("Thread #%zu won the game! No more players left.\n",
                       private_data->player_id);

                shared_data->playing = false;
            }
            else
            {
                shared_data->potato_id = change_potato_value(shared_data->potato_id);

                // Si el valor de la papa me queda en 1, pierdo.
                if (shared_data->potato_id == 1)
                {
                    shared_data->potato_id = rand() % 200;
                    printf("The potato has exploded! "
                        "Thread #%zu sets potato's value to %d and leaves the game.\n",
                        private_data->player_id,
                        shared_data->potato_id);
                    
                    private_data->active = false;
                    shared_data->active_players[ private_data->player_id ] = false;
                }
            }
        }

        size_t next_player = 0;
        
        if (shared_data->rotating_right)
        {
            next_player = (private_data->player_id + 1) % shared_data->player_count;
        }
        else
        {
            next_player = (private_data->player_id == 0)
                ? (shared_data->player_count - 1)
                : (private_data->player_id - 1);
        }

        if (shared_data->playing)
        {
            printf("Thread #%zu (%s) has the potato with value %d. Next player: #%zu.\n",
                   private_data->player_id,
                   (private_data->active) ? "active" : "passive",
                   shared_data->potato_id,
                   next_player);
        }

        sleep(2);
        // Pasar la papa.
        sem_post(&shared_data->player_turn[next_player]);
    }

    return NULL;
}

bool no_more_players(shared_data_t* shared_data, size_t id)
{
    for (size_t index = 0; index < shared_data->player_count; ++index)
    {
        // Si encuentro un jugador activo y no soy yo
        if (shared_data->active_players[index] == true && index != id)
        {
            // Aun quedan otros jugadores activos.
            return false;
        }
    }

    return true;
}

int change_potato_value(int potato_value)
{
    return ( (potato_value % 2 == 0) ? potato_value / 2 : potato_value * 3 + 1 );
}