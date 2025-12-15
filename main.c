#include <elf.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "elf_parser.h"
#include "linker.h"

int main(int argc, char **argv)
{
    MemoryLayout layout;

    Elf32_Ehdr * elf_header = NULL;
    Elf32_Shdr * section_header = NULL;
    char * section_names = NULL;

    // loop through arguments starting at 1 (skip main program name)
    for(int i = 1; i < argc; i++)
    {
        elf_header = extract_elf_header(argv[i]);
        
        int check_file = is_valid_objfile(elf_header);

        switch (check_file)
        {
            case FILE_OK:

                section_header = extract_section_header(argv[i], elf_header);
                section_names = extract_section_names(argv[i], elf_header, section_header);
                printf("File OK\n");
                
                // Calculate memory layout
                layout = calculate_layout(section_header, section_names, elf_header->e_shnum);

                #ifdef DEBUG
                    printf(".text  addr=0x%08X size=%u\n",
                        layout.text_addr, layout.text_size);
                    printf(".data  RAM =0x%08X size=%u\n",
                        layout.data_ram_addr, layout.data_size);
                    printf(".data  FLASH=0x%08X\n",
                        layout.data_flash_addr);
                    printf(".bss   addr=0x%08X size=%u\n",
                        layout.bss_addr, layout.bss_size);
                #endif
                
                // Build flash image
                build_flash_image(section_header, section_names, elf_header->e_shnum, argv[i]);
                    
                free(elf_header);
                free(section_header);
                free(section_names);

                break;
            case FILE_COULD_NOT_READ_HEADER:
                printf("ERROR: Failed reading elf header! File: %s\n", argv[i]);
                break;
            case FILE_NOT_ELF:
                printf("ERROR: File is not a object file! File: %s\n", argv[i]);
                break;
            case FILE_NOT_ARM:
                printf("ERROR: This file was not built for ARM architecture! File: %s\n", argv[i]);
                break;
            case FILE_NOT_32BIT:
                printf("ERROR: File is not 32 bit! File: %s\n", argv[i]);
                break;
            case FILE_NOT_RELOCATABLE:
                printf("ERROR: File is already linked! File: %s\n", argv[i]);
                break;
            default:
                printf("Default\n");
                break;
        }
    }
    
    return 0;
}
