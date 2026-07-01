#include <stdlib.h>
#include <string.h>
#include "../intermediate_representation.h"
#include "test_harness.h"

static IR_Program compile(const char *source) {
  return transform_brainfuck_to_ir(source, strlen(source), nullptr, false);
}

static void test_coalesce_add(void) {
  const IR_Program program = compile("+++++");
  CHECK(program.program_length == 1);
  CHECK(program.instructions[0].opcode == OP_ADD);
  CHECK(program.instructions[0].argument == 5);
  free(program.instructions);
}

static void test_coalesce_sub(void) {
  const IR_Program program = compile("-----");
  CHECK(program.program_length == 1);
  CHECK(program.instructions[0].opcode == OP_SUB);
  CHECK(program.instructions[0].argument == 5);
  free(program.instructions);
}

static void test_coalesce_move_right(void) {
  IR_Program const program = compile(">>>");
  CHECK(program.program_length == 1);
  CHECK(program.instructions[0].opcode == OP_MOVE_RIGHT);
  CHECK(program.instructions[0].argument == 3);
  free(program.instructions);
}

static void test_coalesce_move_left(void) {
  IR_Program const program = compile("<<<");
  CHECK(program.program_length == 1);
  CHECK(program.instructions[0].opcode == OP_MOVE_LEFT);
  CHECK(program.instructions[0].argument == 3);
  free(program.instructions);
}

static void test_clear_cell_minus(void) {
  IR_Program const program = compile("[-]");
  CHECK(program.program_length == 1);
  CHECK(program.instructions[0].opcode == OP_CLEAR_CELL);
  free(program.instructions);
}

static void test_clear_cell_plus(void) {
  IR_Program const program = compile("[+]");
  CHECK(program.program_length == 1);
  CHECK(program.instructions[0].opcode == OP_CLEAR_CELL);
  free(program.instructions);
}

static void test_jump_resolution(void) {
  /* "[>]" is NOT a clear-cell loop (body is a move), so brackets survive
	 and should point at each other's indices. */
  IR_Program const program = compile("[>]");
  CHECK(program.program_length == 3);
  CHECK(program.instructions[0].opcode == OP_JUMP_IF_ZERO);
  CHECK(program.instructions[0].argument == 2);
  CHECK(program.instructions[2].opcode == OP_JUMP_IF_NOT_ZERO);
  CHECK(program.instructions[2].argument == 0);
  free(program.instructions);
}

static void test_print_read(void) {
  IR_Program const program = compile(".,");
  CHECK(program.program_length == 2);
  CHECK(program.instructions[0].opcode == OP_PRINT);
  CHECK(program.instructions[1].opcode == OP_READ);
  free(program.instructions);
}

int main(void) {
  RUN_TEST(test_coalesce_add);
  RUN_TEST(test_coalesce_sub);
  RUN_TEST(test_coalesce_move_right);
  RUN_TEST(test_coalesce_move_left);
  RUN_TEST(test_clear_cell_minus);
  RUN_TEST(test_clear_cell_plus);
  RUN_TEST(test_jump_resolution);
  RUN_TEST(test_print_read);
  return TEST_SUMMARY();
}
