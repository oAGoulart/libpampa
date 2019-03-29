#include <stdio.h>
#include <ctype.h>
#include <string.h>

// maximum length for owner's name
#define MAX_NAME_LENGTH 16

// all possible blocks names
const char* block_name[] = {
	"a", "b", "c", "d", "e"
};

// all possible apartments numbers
const unsigned short apartment_number[] = {
	101u, 102u, 103u, 
	201u, 202u, 203u, 
	301u, 302u, 303u
};

// all owners names
const char* owner_name[][4][9][MAX_NAME_LENGTH] = {
	{ 
		{ {"Paulo"}, {"Paula"}, {"Alexandra"}, {"Antonia"}, {"Bibiana"}, {"Claudia"}, {"Roberto"}, {"0"}, {"Luciana"} },
		{ {"Thiago"}, {"Marcela"}, {"Claudio"}, {"Osvaldo"}, {"Fernanda"}, {"Maria"}, {"Roberta"}, {"Paulino"}, {"Luciano"} },
		{ {"Joao"}, {"Marcelo"}, {"Claudia"}, {"Alyson"}, {"Fernando"}, {"Mariana"}, {"1"}, {"Michel"}, {"Fabio"} },
		{ {"1"}, {"Bianca"}, {"1"}, {"Allyne"}, {"1"}, {"Mario"}, {"1"}, {"Mica"}, {"Felipao"} }
	},
	{ 
		{ {"Olavo"}, {"Eduarda"}, {"1"}, {"Rafael"}, {"Diogo"}, {"Mauricio"}, {"Cristina"}, {"Ruan"}, {"Oscar"} },
		{ {"Bilac"}, {"Eduardo"}, {"1"}, {"Rafaela"}, {"Ricardo"}, {"Diviane"}, {"Cristiane"}, {"Jean"}, {"Osmar"} },
		{ {"Mathias"}, {"1"}, {"Sther"}, {"Cristina"}, {"Estefania"}, {"Fabio"}, {"Luis"}, {"Geanluca"}, {"Jeronimo"} },
		{ {"Joana"}, {"1"}, {"Juliana"}, {"Aldo"}, {"Juliano"}, {"Fabiana"}, {"Luiza"}, {"Nilse"}, {"Geronimo"} }
	},
	{ 
		{ {"Arthur"}, {"Juh"}, {"Bolivar"}, {"Gustavo"}, {"Uillian"}, {"Fabinho"}, {"Luizao"}, {"Lucia"}, {"Gianluca"} },
		{ {"Julio"}, {"Aline"}, {"Renato"}, {"Thielo"}, {"Mateus"}, {"Zeca"}, {"1"}, {"Tereza"}, {"Jonatan"} },
		{ {"Jessica"}, {"Alison"}, {"Renata"}, {"Amanda"}, {"Matheus"}, {"Ueslei"}, {"1"}, {"Jeferson"}, {"Jonathas"} },
		{ {"Maria"}, {"Alex"}, {"Antonio"}, {"Thiarles"}, {"Daiana"}, {"Jael"}, {"2"}, {"Larissa"}, {"1"} }
	},
	{ 
		{ {"3"}, {"3"}, {"3"}, {"Anderson"}, {"Mario"}, {"Crislaine"}, {"Rafael"}, {"Pele"}, {"Caesar"} },
		{ {"3"}, {"3"}, {"3"}, {"Jeckson"}, {"Marcio"}, {"Cristaldo"}, {"Renata"}, {"Casemiro"}, {"1"} },
		{ {"3"}, {"3"}, {"3"}, {"Marcela"}, {"Marcelita"}, {"Cesar"}, {"1"}, {"Maicon"}, {"Juan"} },
		{ {"3"}, {"3"}, {"3"}, {"Luigi"}, {"Arthur"}, {"1"}, {"Alister"}, {"Bibiana"}, {"Claudia"} }
	},
	{ 
		{ {"Joaozinho"}, {"Bibi"}, {"1"}, {"4"}, {"4"}, {"4"}, {"Gustavo"}, {"Alessandro"}, {"1"} },
		{ {"Paulo"}, {"Ronaldo"}, {"1"}, {"4"}, {"4"}, {"4"}, {"1"}, {"Alessandra"}, {"Thielo"} },
		{ {"Carlise"}, {"Quelen"}, {"1"}, {"4"}, {"4"}, {"4"}, {"Mauricio"}, {"Cristina"}, {"5"} },
		{ {"Claudio"}, {"Ketlen"}, {"1"}, {"4"}, {"4"}, {"4"}, {"Edson"}, {"1"}, {"5"} }
	}
};

// transform block character into its proper array index number
short get_block_index(const char block)
{
	short result = -1;
	char c = tolower(block);

	// search for block character inside array of possible blocks names
	for (int i = 0; i < (sizeof(block_name) / sizeof(block_name[0])); i++) {
		if (c == *block_name[i]) {
			result = i;
			break;
		}
	}

	return result;
}

// transform apartment number into its proper array index number
short get_apartment_index(const short apartment)
{
	short result = -1;

	// verify the apartment number isn't negative
	if (apartment >= 0) {
		// search for apartment number inside possible apartment numbers array
		for (int i = 0; i < (sizeof(apartment_number) / sizeof(apartment_number[0])); i++) {
			if (apartment == apartment_number[i]) {
				result = i;
				break;
			}
		}
	}

	return result;
}

// find and print the name of the apartment owner
void find_owner_name(const char block, const short building, const short apartment)
{
	short block_index = get_block_index(block);
	short building_index = (building >= 0 && building <= 3) ? building : -1;
	short apartment_index = get_apartment_index(apartment);
	char name[MAX_NAME_LENGTH];

	// verify that those aren't invalid indexes
	if (block_index == -1)
		printf("The block %c doesn't exist!\n", block);
	else if (building_index == -1)
		printf("The building %d doesn't exist!\n", building);
	else if (apartment_index == -1)
		printf("The apartment %d doesn't exist!\n", apartment);
	else {
		strncpy(name, *owner_name[block_index][building_index][apartment_index], sizeof(name));

		// verify that this isn't an invalid name
		if (!strncmp(name, "0", 1))
			printf("The apartment owner is unavailable!\n");
		else if (!strncmp(name, "1", 1))
			printf("This apartment is empty!\n");
		else if (!strncmp(name, "2", 1))
			printf("There was a problem with the delivery!\n");
		else if (!strncmp(name, "3", 1))
			printf("This apartment is under construction!\n");
		else if (!strncmp(name, "4", 1))
			printf("This apartment is unavailable!\n");
		else if (!strncmp(name, "5", 1))
			printf("This apartment is interdicted!\n");
		else
			// valid name, print it!
			printf("The owner's name is: %s\n", name);
	}
}

// entry point
int main()
{
	char block = '\0';
	short building = -1, apartment = -1;

	/* this tries all possible input combinations (for debugging)
	for (char c = 'a'; c <= 'e'; c++) {
		for (int i = 0; i <= 3; i++) {
			for (int j = 101; j <= 303; j++) {
				if (j % 100 == 4) {
					j -= 3;
					j += 100;
				}

				find_owner_name(c, i, j);
			}
		}
	}*/

	printf("Hello, where is the mailing address?\n");
	printf("(please, type in: [block] [building] [apartment])\n");
	scanf("%c %hd %hd", &block, &building, &apartment);


	find_owner_name(block, building, apartment);

}
