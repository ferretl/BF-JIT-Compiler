#ifndef BF_JIT_COMPILE_H
#define BF_JIT_COMPILE_H

#include <_stdlib.h>

typedef enum {
	OP_ADD,
	OP_SUB,
	OP_MOVE_LEFT,
	OP_MOVE_RIGHT,
	OP_PRINT,
	OP_READ,
	OP_JUMP_IF_ZERO,
	OP_JUMP_IF_NOT_ZERO,
	OP_CLEAR_CELL
} Opcode;

typedef struct {
	Opcode opcode;
	uint16_t argument;
} Instruction;

typedef struct {
	Instruction *instructions;
	size_t program_length;
} Program;

Program compile_bf_program(const char *brainfuck_program, size_t brainfuck_program_length, char *output_prefix,
						   bool dump_program);
size_t generate_initial_instructions(const char *brainfuck_program, size_t brainfuck_program_length,
									 Instruction *initial_instructions);
Program generate_initial_IR_program(const char *brainfuck_program, size_t brainfuck_program_length);
Program generate_optimised_IR_program(Program program);
Program coalesce_instructions(Program program);
Program resolve_program_jumps(Program program);
const char *opcode_to_string(Opcode opcode);
void dump_IR(Program program, const char *filename);

#endif // BF_JIT_COMPILE_H
