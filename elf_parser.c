#include <stdint.h>
#include <stdio.h>
#include "elf_parser.h"
#include <elf.h>
#include <stdlib.h>

Elf32_Ehdr extract_elf_header(const char * FILE_PATH)
{

        FILE * fp = fopen(FILE_PATH, "rb");
        Elf32_Ehdr header = {0};

        if (!fp)
        {
                return header; // return "empty" header
        }

        if (fread(&header, 1, sizeof(Elf32_Ehdr), fp) != sizeof(Elf32_Ehdr))
        {
                fclose(fp);
                return (Elf32_Ehdr){0};
        }

        fclose(fp);
        return header;
}

int is_valid_objfile(Elf32_Ehdr * header)
{
        if (!header)
        {
                return FILE_COULD_NOT_READ_HEADER;
        }

        if (header->e_ident[EI_MAG0] != ELFMAG0 ||
            header->e_ident[EI_MAG1] != ELFMAG1 ||
            header->e_ident[EI_MAG2] != ELFMAG2 ||
            header->e_ident[EI_MAG3] != ELFMAG3)
        {
                return FILE_NOT_ELF;
        }

        if (header->e_ident[EI_CLASS] != ELFCLASS32)
        {
                return FILE_NOT_ELF;
        }

        if (header->e_machine != EM_ARM)
        {
                return FILE_NOT_ELF;
        }

        if (header->e_type != ET_REL)
        {
                return FILE_NOT_ELF;
        }

        return FILE_OK;
}

Elf32_Shdr * extract_section_header(const char * FILE_PATH, Elf32_Ehdr * header)
{
        FILE * fp = fopen(FILE_PATH, "rb");

        if (!fp)
        {
                return NULL;
        }

        // jump to section table
        if (fseek(fp, header->e_shoff, SEEK_SET) != 0)
        {
                fclose(fp);
                return NULL;
        }

        // struct for sections
        Elf32_Shdr * sections = malloc(header->e_shentsize * header->e_shnum);

        if (!sections)
        {
                fclose(fp);
                return NULL;
        }

        if (fread(sections, header->e_shentsize, header->e_shnum, fp))
        {
                free(sections);
                fclose(fp);
                return NULL;
        }


        fclose(fp);

        return sections;
}

char * extract_section_names(const char * FILE_PATH, Elf32_Ehdr * elf_header, Elf32_Shdr * section_header)
{
        FILE * fp = fopen(FILE_PATH, "rb");

        if(!fp)
        {
                return NULL;
        }

        Elf32_Shdr shstr = section_header[elf_header->e_shstrndx];

        char * names = malloc(shstr.sh_size);

        if (!names)
        {
                fclose(fp);
                return NULL;
        }

        // jump to string table
        if (fseek(fp, shstr.sh_offset, SEEK_SET) != 0 ||
            fread(names, 1, shstr.sh_size, fp) != shstr.sh_size) 
	{

		free(names);
		fclose(fp);
		return NULL;
	}

	fclose(fp);

        return names;
}
