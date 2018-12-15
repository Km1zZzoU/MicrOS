#include "elf_loader.h"

void elf_loader_load(uint8_t *content)
{
    elf_header *header = elf_get_header(content);

    uint32_t size_in_memory = elf_get_total_size_in_memory(content);
    uint32_t pages_count = (size_in_memory / 1024 / 1024 / 4) + 1;

    uint32_t initial_page = (uint32_t)header->entry_position / 1024 / 1024 / 4;

    for (uint32_t i = 0; i < pages_count; i++)
    {
        uint32_t physical_address = physical_memory_alloc_page();
        paging_map_page(physical_address, initial_page + i);
    }

    for (uint32_t i = 0; i < header->section_header_table_entries_in_section_count; i++)
    {
        elf_section_header *section_header = elf_get_section_header_with_index(content, i);
        if (section_header->virtual_address != 0)
        {
            memcpy((void *)section_header->virtual_address, content + section_header->file_offset, section_header->size);
        }
    }
}