#include <stdint.h>
#include <stdio.h>
#include "validate_objfile.h"
#include <elf.h>
#include <stdlib.h>

Elf64_Ehdr extract_elf_header(const char * FILE_PATH)
{

	FILE * fp = fopen(FILE_PATH, "rb");
	Elf64_Ehdr header = {0};

	if (!fp)
	{
		return header; // return "empty" header
	}

	if (fread(&header, 1, sizeof(Elf64_Ehdr), fp) != sizeof(Elf64_Ehdr))
	{
		fclose(fp);
	}
	return header;
}

int is_valid_objfile(Elf64_Ehdr * header)
{
	if (header->e_ident[0] == 0x7F &&
            header->e_ident[1] == 'E' &&
            header->e_ident[2] == 'L' &&
            header->e_ident[3] == 'F')
	{
		return FILE_OK;
	}

	return FILE_NOT_ELF;
}

Elf64_Shdr * extract_section_header(const char * FILE_PATH, Elf64_Ehdr * header)
{
	FILE * fp = fopen(FILE_PATH, "rb");

	if (!fp)
	{
		return NULL;
	}

	// jump to section table
	fseek(fp, header->e_shoff, SEEK_SET);

	// struct for sections
	Elf64_Shdr * sections = malloc(header->e_shentsize * header->e_shnum);

	if (!sections)
	{
		fclose(fp);
		return NULL;
	}

	fread(sections, header->e_shentsize, header->e_shnum, fp);
	fclose(fp);

	return sections;
}

char * extract_section_names(const char * FILE_PATH, Elf64_Ehdr * elf_header, Elf64_Shdr * section_header)
{
	FILE * fp = fopen(FILE_PATH, "rb");

	if(!fp)
	{
		return NULL;
	}

	Elf64_Shdr shstr = section_header[elf_header->e_shstrndx];

	char * names = malloc(shstr.sh_size);

	if (!names)
	{
		fclose(fp);
		return NULL;
	}

	// jump to string table
	fseek(fp, shstr.sh_offset, SEEK_SET);

	fread(names, 1, shstr.sh_size, fp);
	fclose(fp);

	return names;
}
