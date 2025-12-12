#ifndef VALIDATE_OBJFILE_H
#define VALIDATE_OBJFILE_H

#include <elf.h>

enum ValidateError
{
	FILE_OK = 0,
	FILE_NOT_EXISTS = 1,
	FILE_NOT_ELF = 2,
	FILE_COULD_NOT_READ_HEADER = 3
};

Elf64_Ehdr extract_elf_header(const char * FILE_PATH);
int is_valid_objfile(Elf64_Ehdr * header);
Elf64_Shdr * extract_section_header(const char * FILE_PATH, Elf64_Ehdr * header);
char * extract_section_names(const char * FILE_PATH, Elf64_Ehdr * elf_header, Elf64_Shdr * section_header);

#endif
