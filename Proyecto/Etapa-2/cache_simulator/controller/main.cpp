/**
 * Programa que simula una cache con los requerimientos especificos
 * de la etapa 2 del proyecto.
 */

#include "../model/arguments.h"
#include "../model/cache.h"

#include <cstdio>
#include <ctime>
#include <iostream>
#include <string>

/**
 * Lee cada linea del archivo de la traza e invoca al metodo
 * para acceder a la cache.
 * 
 * @param cache Objeto de la clase Cache que maneja cada acceso a cache/memoria.
 */
void read_trace_file(Cache* cache);

/**
 * Imprime el estado final de la cache despues de leer
 * cada linea del archivo de la traza.
 * 
 * @param cache Objeto de la clase Cache que maneja cada acceso a cache/memoria.
 */
void print_cache_results(Cache* cache);

/**
 * Comienza la ejecucion del programa.
 * 
 * @param argc  El numero de argumentos recibidos por linea de comandos.
 * @param argv  Arreglo de argumentos recibidos por linea de comandos.
 */
int main(int argc, char* argv[])
{
    int error = 0;

    srand(time(NULL));

    CacheData* cache_data = new CacheData();

    if (cache_data != nullptr)
    {
        error = analyze_arguments(argc, argv, cache_data);

        if (error == 0)
        {
            Cache* cache = new Cache(cache_data);

            if (cache != nullptr)
            {
                read_trace_file(cache);

                std::cout << '\n';
                print_cache_results(cache);
                
                delete cache;
            }
            else
            {
                std::cerr << "Error: Could not create cache\n";
                error = 12;
            }
        }
        
        delete cache_data;
    }
    else
    {
        std::cerr << "Error: Could not allocate cache data\n";
        error = 11;
    }

    return error;
}

void read_trace_file(Cache* cache)
{
    Access access;
    std::string current_line;

    std::size_t line_counter = 0;

    while (std::getline(std::cin, current_line))
    {
        ++line_counter;
        if (current_line.substr(0, 2) == "//")
        {
            // Comment.
        }
        else if ((sscanf(current_line.c_str(), "%c %lx",
                &access.operation, &access.address) != 2)
                || (access.operation != LOAD && access.operation != STORE)
                || (access.address > 0xffffffff))
        {
            std::cerr << "Syntax error in line #" << line_counter << "\n";
        }
        else
        {
            printf("%c 0x%08lx ", access.operation, access.address);

            cache->handle_reference(access);
        }
    }
}

void print_cache_results(Cache* cache)
{
    std::cout << "Total loads: " << cache->get_load_count() << '\n';
    std::cout << "Total stores: " << cache->get_store_count() << '\n';
    std::cout << "Load hits: " << cache->get_load_hit_count() << '\n';
    std::cout << "Load misses: " << cache->get_load_miss_count() << '\n';
    std::cout << "Store hits: " << cache->get_store_hit_count() << '\n';
    std::cout << "Store misses: " << cache->get_store_miss_count() << '\n';
    std::cout << "Evictions: " << cache->get_eviction_count() << '\n';
    std::cout << "Total CPU Cycles: " << cache->get_total_cpu_cycles() << '\n';
}