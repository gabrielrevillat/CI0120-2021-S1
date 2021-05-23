/**
 * Analiza los argumentos recibidos de la linea de comandos.
 */

#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <cstddef>

/**
 * Estructura que guarda informacion para inicializar
 * los atributos de la cache.
 */
struct CacheData
{
    std::size_t num_of_sets;
    std::size_t num_of_set_blocks;
    std::size_t num_of_block_bytes;
    std::size_t cache_access_cycles;
    std::size_t memory_access_cycles;
    bool write_allocate;
    bool write_through;
    int replacement;
};

/**
 * Verifica el numero de argumentos recibidos por linea de comandos 
 * y se asegura de que cada argumento sea valido.
 * 
 * @param argc          El numero de argumentos recibidos por linea de comandos.
 * @param argv          Arreglo de argumentos recibidos por linea de comandos.
 * @param cache_data    Datos para inicializar la cache.
 */
int analyze_arguments(int argc, char* argv[], CacheData* cache_data);

/**
 * Verifica si @a number es una potencia de 2.
 * 
 * @param number    El numero por verificar.
 * @return true si es potencia de 2; de lo contrario, false.
 */
bool is_power_of_two(std::size_t number);

#endif /* ARGUMENTS_H */