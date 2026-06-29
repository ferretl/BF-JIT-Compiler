#include <stdio.h>
#include <_stdlib.h>


int main(const int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "No BF file was supplied!");
		return EXIT_FAILURE;
	}

	char* filename = argv[1];
	if (filename == NULL) {
		fprintf(stderr, "Error: missing file path argument.\n");
		fprintf(stderr, "Usage: %s [-i] <filename>\n", argv[0]);
		return EXIT_FAILURE;
	}

	FILE *input_file = fopen(filename, "r");
	if (input_file == NULL) {
		fprintf(stderr, "Error: could not open file '%s'.\n", filename);
		return EXIT_FAILURE;
	}

	size_t brainfuck_program_cap = 1024;
	size_t brainfuck_program_length = 0;
	char *brainfuck_program = malloc(brainfuck_program_cap);
	if (brainfuck_program == NULL) {
		fprintf(stderr, "Error: Allocation for program space failed.\n");
		fclose(input_file);
		return EXIT_FAILURE;
	}

	int character;
	while ((character = fgetc(input_file)) != EOF) {
		if (brainfuck_program_length + 1 >= brainfuck_program_cap) {
			brainfuck_program_cap *=2;
			char *grown = realloc(brainfuck_program, brainfuck_program_cap);
			if (grown == NULL) {
				fprintf(stderr, "Error: allocation failed.\n");
				free(brainfuck_program);
				fclose(input_file);
				return EXIT_FAILURE;
			}
			brainfuck_program = grown;
		}
		brainfuck_program[brainfuck_program_length++] = (char) character;
	}

	brainfuck_program[brainfuck_program_length] = '\0';
	fclose(input_file);


	free(brainfuck_program);
	return EXIT_SUCCESS;
}
