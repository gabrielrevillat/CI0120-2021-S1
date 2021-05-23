/**
 * Encabezado de la clase Cache.
 */

#ifndef CACHE_H
#define CACHE_H

#include "arguments.h"

#include <cmath>
#include <cstddef>
#include <iostream>

#define LRU     0
#define FIFO    1
#define RANDOM  2

#define ADDRESS_LENGTH 32

#define LOAD    'l'
#define STORE   's'

/**
 * Estructura que representa la informacion de cada acceso a memoria
 * recibido del archivo de la traza.
 */
struct Access
{
    char operation;
    std::size_t address;
};

/**
 * Clase Cache.
 * 
 * Simula una memoria cache dirct-mapped o fully-associative,
 * con las politicas de escritura no-write-allocate y write-through,
 * y los algoritmos de remplazo LRU, FIFO y random.
 */
class Cache
{
// Estructuras privadas
private:
    /**
     * Estructura que representa un bloque de la cache.
     */
    struct CacheBlock
    {
        int tag;
        bool valid;
        bool dirty;
        bool first_in;
        std::size_t lru_value;
    };

    /**
     * Estructura que guarda informacion del estado actual de la cache.
     */
    struct CacheStatus
    {
        std::size_t load_count;
        std::size_t store_count;
        std::size_t load_hit_count;
        std::size_t load_miss_count;
        std::size_t store_hit_count;
        std::size_t store_miss_count;
        std::size_t eviction_count;
        std::size_t total_cpu_cycles;
    };

    /**
     * Estructura que guarda la cantidad de bits del tag, index y offset
     * de una direccion, despues de calcularlas.
     */
    struct AddressInfo
    {
        std::size_t tag_length;
        std::size_t index_length;
        std::size_t offset_length;
    };

// Atributos privados
private:
    // Numero de conjuntos en la cache.
    std::size_t num_of_sets;
    // Numero de bloques en cada conjunto.
    std::size_t num_of_set_blocks;
    // Numero de bytes en cada bloque.
    std::size_t num_of_block_bytes;

    // Banderas de escritura.
    bool write_allocate;
    bool write_through;

    // Identificador para algoritmo de reemplazo.
    int replacement_algorithm;
    
    // Numero de ciclos de acceso a cache y memoria.
    std::size_t cache_access_cycles;
    std::size_t memory_access_cycles;

    // Informacion de las direcciones de memoria.
    AddressInfo address_info;
    // Estado actual de la cache.
    CacheStatus status;

    // Contenedor de la cache. Arreglo de punteros a bloques.
    CacheBlock** blocks;

// Metodos publicos
public:

    /**
     * Construye un objeto Cache con los datos de @a cache_data.
     * 
     * @param cache_data    struct con datos para inicializar los atributos
     * de la cache.
     */
    Cache(CacheData* cache_data);

    /**
     * Destruye la cache.
     */
    ~Cache();

    /**
     * Realiza los accesos a memoria y cache correspondientes, 
     * segun la operacion y la direccion obtenidas de @a reference. 
     * Verifica si es miss o hit, invoca al algoritmo de reemplazo 
     * correspondiente e incrementa los contadores del estado de la cache.
     * 
     * @param reference Acceso que contiene la operacion y direccion
     * recibidas de la linea actual del archivo de la traza.
     */
    void handle_reference(Access reference);

    // Getters

    std::size_t get_load_count();
    std::size_t get_store_count();
    std::size_t get_load_hit_count();
    std::size_t get_load_miss_count();
    std::size_t get_store_hit_count();
    std::size_t get_store_miss_count();
    std::size_t get_eviction_count();
    std::size_t get_total_cpu_cycles();

// Metodos privados
private:

    // Calcula el numero de bits del tag, index y offset de las direcciones.
    void calculate_address_lengths();
    // Obtiene el tag de una direccion.
    std::size_t get_tag(std::size_t address);
    // Obtiene el index de una direccion.
    std::size_t get_index(std::size_t address);

    // Verifica si una direccion da un miss.
    bool is_miss(std::size_t tag, std::size_t index);

    // Realizan los algoritmos de reemplazo.
    void do_lru_replacement(std::size_t tag, std::size_t index);
    void do_fifo_replacement(std::size_t tag, std::size_t index);
    void do_random_replacement(std::size_t tag, std::size_t index);

    // Actualiza el estado del bloque para efectos del algoritmo LRU.
    void update_lru_block(std::size_t tag, std::size_t index);
    
};

#endif /* CACHE_H */