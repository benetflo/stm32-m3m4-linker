#include <elf.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "linker.h"
#include "elf_parser.h"

MemoryLayout calculate_layout(
    Elf32_Shdr * sections,
    char * names,
    int shnum
)
{
    MemoryLayout layout = {0};

    layout.text_addr = 0x08000000;
    layout.data_ram_addr = 0x20000000;

    // loop through all sections
    for (int i = 1; i < shnum; i++) 
    {
        // get section names
        char * name = names + sections[i].sh_name;

        if (strcmp(name, ".text") == 0)
            layout.text_size += sections[i].sh_size;
        else if (strcmp(name, ".data") == 0)
            layout.data_size += sections[i].sh_size;
        else if (strcmp(name, ".bss") == 0)
            layout.bss_size += sections[i].sh_size;
    }

    layout.data_flash_addr = layout.text_addr + layout.text_size;
    // .data section: initialized global and static variables
    // Stored in RAM during execution. Initial values are typically copied from FLASH at startup.
    
    layout.bss_addr = layout.data_ram_addr + layout.data_size;
    // .bss section: zero-initialized global and static variables
    // Occupies RAM, but no data is stored in the ELF file. Must be zeroed at startup.


    return layout;
}

bool build_flash_image(Elf32_Shdr * sections, char * names, int shnum, const char * inp_file)
{
    
        FILE * output_fp = fopen("output.bin", "wb");
        if (!output_fp)
        {
            perror("Error opening output.bin");
            return false;
        }

        // write .text
        for(int i = 1; i < shnum; i++)
        {
            char * secname = names + sections[i].sh_name;

            if (strcmp(secname, ".text") == 0)
            {
                uint8_t * data = load_section_data(inp_file, &sections[i]);
                fwrite(data, 1, sections[i].sh_size, output_fp);
                free(data);
            }
        }

        // write .data
        for(int i = 1; i < shnum; i++)
        {
            char * secname = names + sections[i].sh_name;

            if (strcmp(secname, ".data") == 0)
            {
                uint8_t * data = load_section_data(inp_file, &sections[i]);
                fwrite(data, 1, sections[i].sh_size, output_fp);
                free(data);
            }
        }

        fclose(output_fp);
        return true;
}