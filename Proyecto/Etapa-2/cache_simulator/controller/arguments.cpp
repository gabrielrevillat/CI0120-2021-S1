/**
 * Analiza los argumentos recibidos de la linea de comandos.
 */

#include "../model/arguments.h"

#include <iostream>

int analyze_arguments(int argc, char* argv[], CacheData* cache_data)
{
    int error = 0;

    if (argc >= 9)
    {

        if (sscanf(argv[1], "%zu", &cache_data->num_of_sets) != 1
            || cache_data->num_of_sets <= 0
            || !(is_power_of_two(cache_data->num_of_sets)))
        {
            std::cerr << "Error: Invalid number of sets\n";
            std::cerr << "Value must be positive and a power of two\n";
            error = 2;
        }
        else if (sscanf(argv[2], "%zu", &cache_data->num_of_set_blocks) != 1
            || cache_data->num_of_set_blocks <= 0
            || !(is_power_of_two(cache_data->num_of_set_blocks)))
        {
            std::cerr << "Error: Invalid number of blocks per set\n";
            std::cerr << "Value must be positive and a power of two\n";
            error = 3;
        }
        else if (sscanf(argv[3], "%zu", &cache_data->num_of_block_bytes) != 1
            || cache_data->num_of_block_bytes <= 3
            || !(is_power_of_two(cache_data->num_of_block_bytes)))
        {
            std::cerr << "Error: Invalid number of block bytes\n";
            std::cerr << "Value must be higher or equal to four and a power of two\n";
            error = 4;
        }
        else if (std::string(argv[4]) != "write-allocate"
            && std::string(argv[4]) != "no-write-allocate")
        {
            std::cerr << "Error: Invalid write policy #1\n";
            error = 5;
        }
        else if (std::string(argv[5]) != "write-through"
            && std::string(argv[5]) != "write-back")
        {
            std::cerr << "Error: Invalid write policy #2\n";
            error = 6;
        }
        else if (std::string(argv[4]) == "no-write-allocate"
            && std::string(argv[5]) == "write-back")
        {
            std::cerr << "Error: Invalid combinations of write policies\n";
            error = 7;
        }
        else if (std::string(argv[6]) != "lru"
            && std::string(argv[6]) != "fifo"
            && std::string(argv[6]) != "random")
        {
            std::cerr << "Error: Invalid replacement policy\n";
            error = 8;
        }
        else if (sscanf(argv[7], "%zu", &cache_data->cache_access_cycles) != 1
            || cache_data->cache_access_cycles <= 0)
        {
            std::cerr << "Error: Invalid num of cache access cycles\n";
            std::cerr << "Value must be positive\n";
            error = 9;
        }
        else if (sscanf(argv[8], "%zu", &cache_data->memory_access_cycles) != 1
            || cache_data->memory_access_cycles < cache_data->cache_access_cycles)
        {
            std::cerr << "Error: Invalid num of memory access cycles\n";
            std::cerr << "Value must be higher than cache access cycles\n";
            error = 10;
        }
        else
        {
            if (std::string(argv[4]) == "write-allocate")
            {
                cache_data->write_allocate = true;
            }

            if (std::string(argv[5]) == "write-through")
            {
                cache_data->write_through = true;
            }

            if (std::string(argv[6]) == "lru")
            {
                cache_data->replacement = 0;
            }
            else if (std::string(argv[6]) == "fifo")
            {
                cache_data->replacement = 1;
            }
            else
            {
                cache_data->replacement = 2;
            }

            // Especificaciones exclusivas para la etapa 2 del proyecto.
            if (cache_data->num_of_sets > 1 && cache_data->num_of_set_blocks > 1)
            {
                std::cerr << "M-Way Set-Associative Cache is not evaluated "
                          << "in phase #2 of this project.\n";
                error = -1;
            }
            if (cache_data->write_allocate || !(cache_data->write_through))
            {
                std::cerr << "Write allocate and write back policies are not "
                          << "evaluated in phase #2 of this project.\n";
                error = -1;
            }
        }
    }
    else
    {
        std::cerr << "Usage: cache_simulator number_of_sets "
                  << "num_of_set_blocks num_of_block_bytes "
                  << "write_policy_1 write_policy_2 "
                  << "replacement_policy "
                  << "cache_access_cycles memory_access_cycles "
                  << "< trace_file\n\n"
                  << "write_policy_1 options:\n"
                  << "\twrite-allocate\n" << "\tno-write-allocate\n\n"
                  << "mwrite_policy_2 options:\n"
                  << "\twrite-through\n" << "\twrite-back\n\n"
                  << "replacement_policy options:\n"
                  << "\tlru\n" << "\tfifo\n" << "\trandom\n";
        error = 1;
    }

    return error;
}

bool is_power_of_two(std::size_t number)
{
    return (number != 0 && ((number & (number - 1)) == 0));
}