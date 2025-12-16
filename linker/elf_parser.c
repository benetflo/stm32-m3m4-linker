#include <stdint.h>
#include <stdio.h>
#include <elf.h>
#include <stdlib.h>
#include <string.h>

#include "elf_parser.h"

Elf32_Ehdr * extract_elf_header(const char * FILE_PATH)
{
        FILE * fp = fopen(FILE_PATH, "rb");
        Elf32_Ehdr * header = malloc(sizeof(Elf32_Ehdr));

        if (!fp)
        {
                free(header);
                return NULL;
        }

        if (fread(header, 1, sizeof(Elf32_Ehdr), fp) != sizeof(Elf32_Ehdr))
        {
                fclose(fp);
                free(header);
                return NULL;
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
        #ifdef DEBUG
                printf("e_type=0x%02X, e_machine=0x%02X, EI_CLASS=%d\n",
                header->e_type, header->e_machine, header->e_ident[EI_CLASS]);
                fflush(stdout);
        #endif

        if (header->e_ident[EI_MAG0] != ELFMAG0 ||
            header->e_ident[EI_MAG1] != ELFMAG1 ||
            header->e_ident[EI_MAG2] != ELFMAG2 ||
            header->e_ident[EI_MAG3] != ELFMAG3)
        {
                return FILE_NOT_ELF;
        }

        if (header->e_ident[EI_CLASS] != ELFCLASS32)
        {
                return FILE_NOT_32BIT;
        }

        if (header->e_machine != EM_ARM)
        {
                return FILE_NOT_ARM;
        }

        // check if file is already linked
        if (header->e_type != ET_REL)
        {
                return FILE_NOT_RELOCATABLE;
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

        if (fread(sections, header->e_shentsize, header->e_shnum, fp) != header->e_shnum)
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

Elf32_Shdr * find_section(Elf32_Shdr * sections, uint16_t shnum, const char * shstrtab, const char * name)
{
        // loop through all sections in ELF header
        for (int i = 0; i < shnum; i++)
        {
                //shstrtab looks something like this: "\0.text\0.data\0.bss\0.symtab\0.strtab\0.rel.text\0"

                // jump to offset in strtab to find section name
                const char * secname = shstrtab + sections[i].sh_name;
        
                if (strcmp(secname, name) == 0) 
                {
                        return &sections[i];
                }
        }

        return NULL;
}

uint8_t * load_section_data(const char * FILE_PATH, Elf32_Shdr * section)
{
        FILE * fp = fopen(FILE_PATH, "rb");

        if (!fp)
        {
                return NULL;
        }

        uint8_t * buffer = malloc(section->sh_size);

        if (!buffer)
        {
                fclose(fp);
                return NULL;
        }

        // jump to section offset
        fseek(fp, section->sh_offset, SEEK_SET);

        // Read the section data from the file into buffer
        // and ensure the number of bytes read matches the expected section size
        if (fread(buffer, 1, section->sh_size, fp) != section->sh_size)
        {
                free(buffer);
                fclose(fp);
                return NULL;
        }

        fclose(fp);
        return buffer;
}
