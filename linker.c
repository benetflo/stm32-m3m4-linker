#include <stdio.h>
#include "validate_objfile.h"
#include <elf.h>
#include <stdlib.h>

int main(int argc, char * argv[])
{

	Elf64_Ehdr elf_header;
	Elf64_Shdr * section_header;
	char * section_names;

	if (argc > 1)
	{
		// check if additional arguments are valid object files
		for(int i = 1; i < argc; i++) // start at 1 skip linker program arg
		{
			elf_header = extract_elf_header(argv[i]);

			if (is_valid_objfile(&elf_header) == FILE_OK)
			{
				// section header needs to be free'd
				section_header = extract_section_header(argv[i], &elf_header);

				section_names = extract_section_names(argv[i], &elf_header, section_header);

				free(section_header);
				free(section_names);
			}
		}
	}

	return 0;
}
