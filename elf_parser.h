#pragma once

#include <stdint.h>
#include <elf.h>

enum ValidateError
{
	FILE_OK = 0,
	FILE_NOT_EXISTS = 1,
	FILE_NOT_ELF = 2,
	FILE_COULD_NOT_READ_HEADER = 3,
	FILE_NOT_ARM = 4,
	FILE_NOT_32BIT = 5,
	FILE_NOT_RELOCATABLE = 6
};

Elf32_Ehdr * extract_elf_header(const char * FILE_PATH);
int is_valid_objfile(Elf32_Ehdr * header);
Elf32_Shdr * extract_section_header(const char * FILE_PATH, Elf32_Ehdr * header);
char * extract_section_names(const char * FILE_PATH, Elf32_Ehdr * elf_header, Elf32_Shdr * section_header);
Elf32_Shdr * find_section(Elf32_Shdr * sections, uint16_t shnum, const char * shstrtab, const char * name);
uint8_t * load_section_data(const char * FILE_PATH, Elf32_Shdr * section);

