#pragma once

#include <stdint.h>
#include <elf.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct {
    uint32_t text_addr;
    uint32_t data_flash_addr;
    uint32_t data_ram_addr;
    uint32_t bss_addr;

    size_t text_size;
    size_t data_size;
    size_t bss_size;
} MemoryLayout;

MemoryLayout calculate_layout(
    Elf32_Shdr *sections,
    char *section_names,
    int shnum
);

bool build_flash_image(Elf32_Shdr * sections, char * names, int shnum, const char * inp_file);