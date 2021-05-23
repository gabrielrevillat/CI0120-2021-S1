/**
 * Codigo fuente de la clase Cache.
 */

#include "../model/cache.h"

Cache::Cache(CacheData* cache_data) :
    num_of_sets(cache_data->num_of_sets),
    num_of_set_blocks(cache_data->num_of_set_blocks),
    num_of_block_bytes(cache_data->num_of_block_bytes),
    write_allocate(cache_data->write_allocate),
    write_through(cache_data->write_through),
    replacement_algorithm(cache_data->replacement),
    cache_access_cycles(cache_data->cache_access_cycles),
    memory_access_cycles(cache_data->memory_access_cycles),
    blocks(new CacheBlock*[this->num_of_sets])
{
    for (std::size_t set_index = 0; set_index < this->num_of_sets; ++set_index)
    {
        this->blocks[set_index] = new CacheBlock[this->num_of_set_blocks];

        for (std::size_t block_index = 0; block_index < this->num_of_set_blocks;
             ++block_index)
        {
            this->blocks[set_index][block_index].tag = -1;
            this->blocks[set_index][block_index].valid = false;
            this->blocks[set_index][block_index].dirty = false;
            this->blocks[set_index][block_index].first_in = false;
            this->blocks[set_index][block_index].lru_value = 100;
        }
    }

    this->address_info.tag_length = 0;
    this->address_info.index_length = 0;
    this->address_info.offset_length = 0;
    this->calculate_address_lengths();

    this->status.load_count = 0;
    this->status.store_count = 0;
    this->status.load_hit_count = 0;
    this->status.load_miss_count = 0;
    this->status.store_hit_count = 0;
    this->status.store_miss_count = 0;
    this->status.eviction_count = 0;
    this->status.total_cpu_cycles = 0;
}

Cache::~Cache()
{
    for (std::size_t index = 0; index < this->num_of_sets; ++index)
    {
        delete this->blocks[index];
    }
    delete [] blocks;
}

void Cache::handle_reference(Access reference)
{
    std::size_t address_tag = get_tag(reference.address);
    std::size_t address_index = get_index(reference.address);
    std::size_t access_cycles = 0;

    access_cycles += this->cache_access_cycles;

    if (is_miss(address_tag, address_index))
    {
        access_cycles += this->memory_access_cycles;

        // Direct-Mapped
        if (num_of_set_blocks == 1)
        {
            this->blocks[address_index][0].valid = true;
            this->blocks[address_index][0].tag = address_tag;
        }
        // Fully-Associative
        else if (num_of_sets == 1)
        {
            bool updated = false;
            for (std::size_t block = 0; block < this->num_of_set_blocks && !(updated);
                 ++block)
            {
                if (!(this->blocks[0][block].valid))
                {
                    if (block == 0)
                    {
                        this->blocks[0][block].first_in = true;
                    }

                    this->blocks[0][block].valid = true;
                    this->blocks[0][block].tag = address_tag;
                    updated = true;
                }
            }
            // Si llega al final hay que hacer reemplazo.
            if (!(updated))
            {
                access_cycles += this->memory_access_cycles;

                switch (this->replacement_algorithm)
                {
                case LRU:
                    this->do_lru_replacement(address_tag, 0);
                    break;
                case FIFO:
                    this->do_fifo_replacement(address_tag, 0);
                    break;
                case RANDOM:
                    this->do_random_replacement(address_tag, 0);
                    break;
                }

                ++this->status.eviction_count;
            }
        }

        if (reference.operation == LOAD)
        {
            ++this->status.load_miss_count;
        }
        else
        {
            access_cycles += this->memory_access_cycles;
            ++this->status.store_miss_count;
        }

        std::cout << access_cycles << " miss\n";
        this->status.total_cpu_cycles += access_cycles;
    }
    else
    {
        if (reference.operation == LOAD)
        {
            ++this->status.load_hit_count;
        }
        else
        {
            access_cycles += this->memory_access_cycles;
            ++this->status.store_hit_count;
        }

        std::cout << access_cycles << " hit\n";
        this->status.total_cpu_cycles += access_cycles;
    }

    if (this->replacement_algorithm == LRU)
    {
        this->update_lru_block(address_tag, address_index);
    }
}

std::size_t Cache::get_load_count()
{
    return this->status.load_count;
}

std::size_t Cache::get_store_count()
{
    return this->status.store_count;
}

std::size_t Cache::get_load_hit_count()
{
    return this->status.load_hit_count;
}

std::size_t Cache::get_load_miss_count()
{
    return this->status.load_miss_count;
}

std::size_t Cache::get_store_hit_count()
{
    return this->status.store_hit_count;
}

std::size_t Cache::get_store_miss_count()
{
    return this->status.store_miss_count;
}

std::size_t Cache::get_eviction_count()
{
    return this->status.eviction_count;
}

std::size_t Cache::get_total_cpu_cycles()
{
    return this->status.total_cpu_cycles;
}

void Cache::calculate_address_lengths()
{
    this->address_info.index_length = std::log2(this->num_of_sets);
    this->address_info.offset_length = std::log2(this->num_of_block_bytes);
    this->address_info.tag_length = (ADDRESS_LENGTH
                                    - this->address_info.offset_length
                                    - this->address_info.index_length);
}

std::size_t Cache::get_tag(std::size_t address)
{
    return address >> (this->address_info.index_length
                    + this->address_info.offset_length);
}

std::size_t Cache::get_index(std::size_t address)
{
    std::size_t mask = (this->num_of_sets - 1);
    return (address >> this->address_info.offset_length) & mask;
}

bool Cache::is_miss(std::size_t tag, std::size_t index)
{
    for (std::size_t block = 0; block < this->num_of_set_blocks; ++block)
    {
        if (this->blocks[index][block].tag == tag && this->blocks[index][block].valid)
        {
            return false;
        }
    }

    return true;
}

void Cache::do_lru_replacement(std::size_t tag, std::size_t index)
{
    std::size_t lru_block_index = 0;
    std::size_t value = 0;

    for (std::size_t block = 1; block < this->num_of_set_blocks; ++block)
    {
        if (this->blocks[index][block].lru_value
            > this->blocks[index][lru_block_index].lru_value)
        {
            lru_block_index = block;
        }
    }

    this->blocks[index][lru_block_index].tag = tag;    
}

void Cache::do_fifo_replacement(std::size_t tag, std::size_t index)
{
    bool replaced = false;
    for (std::size_t block = 0; block < this->num_of_set_blocks && !(replaced);
         ++block)
    {
        if (this->blocks[index][block].first_in)
        {
            this->blocks[index][block].first_in = false;
            this->blocks[index][block].tag = tag;
            this->blocks[index][(block + 1) % this->num_of_set_blocks].first_in = true;
            replaced = true;
        }
    }
}

void Cache::do_random_replacement(std::size_t tag, std::size_t index)
{
    this->blocks[index][rand() % this->num_of_set_blocks].tag = tag;
}

void Cache::update_lru_block(std::size_t tag, std::size_t index)
{
    std::size_t mru_block_index = 0;
    bool is_mru = false;

    for (std::size_t block = 0; block < this->num_of_set_blocks; ++block)
    {
        if (this->blocks[index][block].tag == tag && this->blocks[index][block].valid)
        {
            if (this->blocks[index][block].lru_value == 0)
            {
                is_mru = true;
            }
            else
            {
                mru_block_index = block;
            }
        }
    }

    if (!(is_mru))
    {
        for (std::size_t block = 0; block < this->num_of_set_blocks; ++block)
        {
            if (block != mru_block_index)
            {
                ++this->blocks[index][block].lru_value;
            }
            else
            {
                this->blocks[index][block].lru_value = 0;
            }
        }
    }
}