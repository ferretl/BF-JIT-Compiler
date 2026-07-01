#include "intermediate_representation.h"
#include <stdio.h>
#include <string.h>

IR_Program transform_brainfuck_to_ir(const char *brainfuck_program, const size_t brainfuck_program_length,
									 char *output_prefix, const bool dump_program) {

	const IR_Program initial_IR_program = generate_initial_IR_program(brainfuck_program, brainfuck_program_length);
	if (initial_IR_program.instructions == nullptr && brainfuck_program_length > 0) {
		return initial_IR_program;
	}

	const IR_Program optimised_ir_program = generate_optimised_IR_program(initial_IR_program);
	if (optimised_ir_program.instructions == nullptr) {
		free(initial_IR_program.instructions);
		return optimised_ir_program;
	}

	if (dump_program) {
		const size_t dump_filename_length = strlen(output_prefix) + strlen("_dump.txt") + 1;
		char *dump_filename = malloc(dump_filename_length);
		if (dump_filename == NULL) {
			fprintf(stderr, "Error: Unable to allocate dump filename\n");
			free(initial_IR_program.instructions);
			free(optimised_ir_program.instructions);
			return (IR_Program) {nullptr, 0};
		}

		snprintf(dump_filename, dump_filename_length, "%s_dump.txt", output_prefix);
		dump_IR(optimised_ir_program, dump_filename);
		free(dump_filename);
	}

	free(initial_IR_program.instructions);
	return optimised_ir_program;
}

size_t generate_initial_instructions(const char *brainfuck_program, const size_t brainfuck_program_length,
									 Instruction *initial_instructions) {
	size_t program_length = 0;
	for (size_t i = 0; i < brainfuck_program_length; i++) {
		const char current_character = brainfuck_program[i];
		switch (current_character) {
			case '.':
				initial_instructions[program_length] = (Instruction) {OP_PRINT, 0};
				program_length++;
				break;
			case ',':
				initial_instructions[program_length] = (Instruction) {OP_READ, 0};
				program_length++;
				break;
			case '[':
				initial_instructions[program_length] = (Instruction) {OP_JUMP_IF_ZERO, 0};
				program_length++;
				break;
			case ']':
				initial_instructions[program_length] = (Instruction) {OP_JUMP_IF_NOT_ZERO, 0};
				program_length++;
				break;
			case '+':
			case '-':
				uint16_t coalesce_count = 1;
				while (i + 1 < brainfuck_program_length && brainfuck_program[i + 1] == current_character &&
					   coalesce_count < UINT16_MAX) {
					coalesce_count++;
					i++;
				}
				initial_instructions[program_length] =
						(Instruction) {current_character == '+' ? OP_ADD : OP_SUB, coalesce_count};
				program_length++;
				break;
			case '>':
			case '<':
				uint16_t move_coalesce_count = 1;
				while (i + 1 < brainfuck_program_length && brainfuck_program[i + 1] == current_character &&
					   move_coalesce_count < UINT16_MAX) {
					move_coalesce_count++;
					i++;
				}
				initial_instructions[program_length] =
						(Instruction) {current_character == '>' ? OP_MOVE_RIGHT : OP_MOVE_LEFT, move_coalesce_count};
				program_length++;
				break;
			default:
				break;
		}
	}
	return program_length;
}

IR_Program generate_initial_IR_program(const char *brainfuck_program, const size_t brainfuck_program_length) {

	IR_Program initial_program = {nullptr, 0};

	Instruction *initial_instructions = malloc(brainfuck_program_length * sizeof(Instruction));
	if (initial_instructions == NULL) {
		fprintf(stderr, "Error: Unable to initialise IR Array\n");
		return initial_program;
	}

	const size_t program_length =
			generate_initial_instructions(brainfuck_program, brainfuck_program_length, initial_instructions);

	initial_program.instructions = initial_instructions;
	initial_program.program_length = program_length;

	return initial_program;
}

IR_Program generate_optimised_IR_program(const IR_Program program) {
	const IR_Program coalesced_program = coalesce_instructions(program);
	if (coalesced_program.instructions == nullptr) {
		return coalesced_program;
	}

	const IR_Program resolved_program = resolve_program_jumps(coalesced_program);
	if (resolved_program.instructions == nullptr) {
		free(coalesced_program.instructions);
		return resolved_program;
	}

	return resolved_program;
}

IR_Program coalesce_instructions(const IR_Program program) {
	IR_Program coalesced_program = {nullptr, 0};
	Instruction *coalesced_instructions = malloc(program.program_length * sizeof(*coalesced_instructions));
	if (coalesced_instructions == NULL) {
		fprintf(stderr, "Error: Unable to initialise coalesced instruction array\n");
		return coalesced_program;
	}

	size_t coalesced_program_length = 0;
	for (size_t i = 0; i < program.program_length; i++) {
		bool is_clear_cell_loop = false;

		if (i + 2 < program.program_length) {
			const Instruction first_instruction = program.instructions[i];
			const Instruction second_instruction = program.instructions[i + 1];
			const Instruction third_instruction = program.instructions[i + 2];

			is_clear_cell_loop = first_instruction.opcode == OP_JUMP_IF_ZERO &&
								 (second_instruction.opcode == OP_ADD || second_instruction.opcode == OP_SUB) &&
								 second_instruction.argument == 1 && third_instruction.opcode == OP_JUMP_IF_NOT_ZERO;
		}

		if (is_clear_cell_loop) {
			coalesced_instructions[coalesced_program_length] = (Instruction) {
					OP_CLEAR_CELL,
					0,
			};
			i += 2;
		} else {
			coalesced_instructions[coalesced_program_length] = program.instructions[i];
		}

		coalesced_program_length++;
	}

	coalesced_program.instructions = coalesced_instructions;
	coalesced_program.program_length = coalesced_program_length;

	return coalesced_program;
}

IR_Program resolve_program_jumps(const IR_Program program) {
	IR_Program resolved_program = {nullptr, 0};

	size_t loop_stack_position = 0;
	size_t *loop_starting_points = malloc(program.program_length * sizeof(*loop_starting_points));
	if (loop_starting_points == NULL) {
		fprintf(stderr, "Error: Unable to allocate space for resolved program\n");
		return resolved_program;
	}

	for (size_t i = 0; i < program.program_length; i++) {
		const Instruction instruction = program.instructions[i];

		if (instruction.opcode == OP_JUMP_IF_ZERO) {
			loop_starting_points[loop_stack_position++] = i;
		} else if (instruction.opcode == OP_JUMP_IF_NOT_ZERO) {
			if (loop_stack_position == 0) {
				fprintf(stderr, "Error: Unmatched closing bracket in program\n");
				free(loop_starting_points);
				return resolved_program;
			}

			const size_t starting_index = loop_starting_points[--loop_stack_position];
			const size_t ending_index = i;

			if (starting_index > UINT16_MAX || ending_index > UINT16_MAX) {
				fprintf(stderr, "Error: IR_Program too large for 16-bit jump arguments\n");
				free(loop_starting_points);
				return resolved_program;
			}

			program.instructions[starting_index].argument = (uint16_t) ending_index;
			program.instructions[ending_index].argument = (uint16_t) starting_index;
		}
	}

	if (loop_stack_position > 0) {
		fprintf(stderr, "Error: Unmatched opening bracket in program\n");
		free(loop_starting_points);
		return resolved_program;
	}

	resolved_program.instructions = program.instructions;
	resolved_program.program_length = program.program_length;

	free(loop_starting_points);
	return resolved_program;
}

const char *opcode_to_string(const Opcode opcode) {
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
		case OP_CLEAR_CELL:
			return "OP_CLEAR_CELL";
		default:
			return "";
	}
}
void dump_IR(const IR_Program program, const char *filename) {
	FILE *output_file = fopen(filename, "w");
	if (output_file == NULL) {
		fprintf(stderr, "Error: could not create output file\n");
		return;
	}

	for (int i = 0; i < program.program_length; i++) {
		const Instruction instruction = program.instructions[i];
		fprintf(output_file, "%s %u\n", opcode_to_string(instruction.opcode), instruction.argument);
	}

	fclose(output_file);
}
