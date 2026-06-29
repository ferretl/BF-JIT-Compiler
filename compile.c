#include "compile.h"

#include <stdio.h>

Program compile_bf_program(const char *brainfuck_program, const size_t brainfuck_program_length) {
	Program initial_IR_program = generate_initial_IR_program(brainfuck_program, brainfuck_program_length);
	dump_IR(initial_IR_program, "initial_program_dump.txt");
	return (Program) {nullptr, 0};
}

Program generate_initial_IR_program(const char *brainfuck_program, const size_t brainfuck_program_length) {

	Program initial_program = {nullptr, 0};

	Instruction *initial_instructions = malloc(brainfuck_program_length * sizeof(Instruction));
	if (initial_instructions == NULL) {
		fprintf(stderr, "Error: Unable to initialise IR Array");
		return initial_program;
	}

	size_t program_length = 0;
	for (size_t i = 0; i < brainfuck_program_length; i++) {
		const char current_character = brainfuck_program[i];
		printf("current char: %c\n", current_character);
		switch (current_character) {
			case '.':
				initial_instructions[program_length].opcode = OP_PRINT;
				initial_instructions[program_length].argument = 0;
				program_length++;
				break;
			case ',':
				initial_instructions[program_length].opcode = OP_READ;
				initial_instructions[program_length].argument = 0;
				program_length++;
				break;
			case '[':
				initial_instructions[program_length].opcode = OP_JUMP_IF_ZERO;
				initial_instructions[program_length].argument = 0; // will be resolved later
				program_length++;
				break;
			case ']':
				initial_instructions[program_length].opcode = OP_JUMP_IF_NOT_ZERO;
				initial_instructions[program_length].argument = 0; // will be resolved later
				program_length++;
				break;
			case '+':
			case '-':
				int16_t coalesce_count = 1;
				while (i + 1 < brainfuck_program_length && brainfuck_program[i + 1] == current_character) {
					coalesce_count++;
					i++;
				}
				initial_instructions[program_length].opcode = current_character == '+' ? OP_ADD : OP_SUB;
				initial_instructions[program_length].argument = coalesce_count;
				program_length++;
				break;
			case '>':
			case '<':
				int16_t move_coalesce_count = 1;
				while (i + 1 < brainfuck_program_length && brainfuck_program[i + 1] == current_character) {
					move_coalesce_count++;
					i++;
				}
				initial_instructions[program_length].opcode = current_character == '>' ? OP_MOVE_RIGHT : OP_MOVE_LEFT;
				initial_instructions[program_length].argument = move_coalesce_count;
				program_length++;
				break;
			default:
				break;
		}
		printf("Instruction generated: %s\t", opcode_to_string(initial_instructions[program_length].opcode));
		printf("Argument: %d\n", initial_instructions[program_length].argument);
	}

	initial_program.instructions = initial_instructions;
	initial_program.program_length = program_length;

	return initial_program;
}

char *opcode_to_string(const Opcode opcode) {
	switch (opcode) {
		case OP_ADD:
			return "OP_ADD";
		case OP_SUB:
			return "OP_SUB";
		case OP_MOVE_LEFT:
			return "OP_MOVE_LEFT";
		case OP_MOVE_RIGHT:
			return "OP_MOVE_RIGHT";
		case OP_PRINT:
			return "OP_PRINT";
		case OP_READ:
			return "OP_READ";
		case OP_JUMP_IF_ZERO:
			return "OP_JUMP_IF_ZERO";
		case OP_JUMP_IF_NOT_ZERO:
			return "OP_JUMP_IF_NOT_ZERO";
		default:
			return "";
	}
}
void dump_IR(const Program program, const char *filename) {
	FILE *output_file = fopen(filename, "w");
	if (output_file == NULL) {
		fprintf(stderr, "Error: could not create output file\n");
		return;
	}

	for (int i = 0; i < program.program_length; i++) {
		const Instruction instruction = program.instructions[i];
		fprintf(output_file, "%s %d\n", opcode_to_string(instruction.opcode), instruction.argument);
	}

	fclose(output_file);
}
