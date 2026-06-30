#ifndef BF_JIT_COMPILE_H
#define BF_JIT_COMPILE_H

#include <stdlib.h>

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
} IR_Program;

IR_Program transform_brainfuck_to_ir(const char *brainfuck_program, size_t brainfuck_program_length,
									 char *output_prefix, bool dump_program);
size_t generate_initial_instructions(const char *brainfuck_program, size_t brainfuck_program_length,
									 Instruction *initial_instructions);
IR_Program generate_initial_IR_program(const char *brainfuck_program, size_t brainfuck_program_length);
IR_Program generate_optimised_IR_program(IR_Program program);
IR_Program coalesce_instructions(IR_Program program);
IR_Program resolve_program_jumps(IR_Program program);
const char *opcode_to_string(Opcode opcode);
void dump_IR(IR_Program program, const char *filename);

#endif // BF_JIT_COMPILE_H
