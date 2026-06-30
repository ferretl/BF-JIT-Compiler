#include <_stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compile.h"

static char *get_input_file_prefix(const char *filename) {
	const size_t filename_length = strlen(filename);
	char *output = malloc(filename_length - 3); /* room for ".s" plus '\0' when appending */
	if (output == NULL) {
		return nullptr;
	}
	strcpy(output, filename);

	const char *slash = strrchr(output, '/');
	char *dot = strrchr(output, '.');
	if (dot != NULL && (slash == NULL || dot > slash)) {
		strcpy(dot, ""); /* replace the extension */
	} else {
		strcat(output, ""); /* no extension: append */
	}
	return output;
}

int main(const int argc, char *argv[]) {

	bool dump_program = false;
	const char *filename = nullptr;
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-d") == 0) {
			dump_program = true;
		} else {
			filename = argv[i];
		}
	}

	if (filename == NULL) {
		fprintf(stderr, "Error: missing file path argument.\n");
		fprintf(stderr, "Usage: %s <filename> [-d]\n", argv[0]);
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
			brainfuck_program_cap *= 2;
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


	char *filename_prefix = dump_program ? get_input_file_prefix(filename) : nullptr;
	compile_bf_program(brainfuck_program, brainfuck_program_length, filename_prefix, dump_program);

	free(brainfuck_program);
	return EXIT_SUCCESS;
}
