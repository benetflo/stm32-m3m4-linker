/*

Läs objektfiler

	- Extrahera sektioner .text och .data.
	- Läs symboltabeller.
	- Läs relocationtabeller.

Samla symboler

	- Skapa en global symboltabell.
	- Kontrollera dubletter och saknade symboler.

Beräkna adresser

	- Tilldela varje sektion en startadress i flash/RAM
	- Uppdatera symboltabellen med faktiska adresser.

Utför relocation

	- Ändra instruktioner i .text som refererar till andra symboler.

Skriv ut körbar fil

	- Skapa en binär fil (.bin) eller ELF med korrekt header.

*/

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
