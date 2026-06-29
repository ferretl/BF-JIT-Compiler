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
} Opcode;

typedef struct {
	Opcode opcode;
	int16_t argument;
} Instruction;

typedef struct {
	Instruction *instructions;
	size_t program_length;
} Program;

Program compile_bf_program(const char *brainfuck_program, size_t brainfuck_program_length, char *output_prefix,
						   bool dump_program);

Program generate_initial_IR_program(const char *brainfuck_program, size_t brainfuck_program_length);

char *opcode_to_string(Opcode opcode);
void dump_IR(Program program, const char *filename);

#endif // BF_JIT_COMPILE_H
