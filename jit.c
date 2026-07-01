#include "jit.h"
#include <lightning.h>
#include <stdlib.h>
#include "intermediate_representation.h"

static jit_state_t *_jit;

CompiledProgram compile_jit(const IR_Program *program) {
	_jit = jit_new_state();

	jit_prolog();

	/* Load tape into V0 */
	jit_node_t *tape_arg = jit_arg();
	jit_getarg(JIT_V0, tape_arg);

	/* generate label array for branching */
	jit_node_t **labels = calloc(program->program_length, sizeof(jit_node_t *));
	PendingJump *pending_jumps = calloc(program->program_length, sizeof(PendingJump));
	size_t pending_jump_count = 0;

	for (size_t i = 0; i < program->program_length; i++) {

		labels[i] = jit_label();
		const Instruction instruction = program->instructions[i];

		switch (instruction.opcode) {
			case OP_ADD:
				jit_ldr_uc(JIT_R0, JIT_V0);
				jit_addi(JIT_R0, JIT_R0, instruction.argument);
				jit_str_c(JIT_V0, JIT_R0);
				break;
			case OP_SUB:
				jit_ldr_uc(JIT_R0, JIT_V0);
				jit_subi(JIT_R0, JIT_R0, instruction.argument);
				jit_str_c(JIT_V0, JIT_R0);
				break;
			case OP_MOVE_LEFT:
				jit_subi(JIT_V0, JIT_V0, instruction.argument);
				break;
			case OP_MOVE_RIGHT:
				jit_addi(JIT_V0, JIT_V0, instruction.argument);
				break;
			case OP_PRINT:
				jit_ldr_uc(JIT_R0, JIT_V0);
				jit_prepare();
				jit_pushargr(JIT_R0);
				jit_finishi(putchar);
				break;
			case OP_READ:
				jit_prepare();
				jit_finishi(getchar);
				jit_retval(JIT_R0);
				jit_str_c(JIT_V0, JIT_R0);
				break;
			case OP_JUMP_IF_ZERO:
				jit_ldr_uc(JIT_R1, JIT_V0);
				jit_node_t *branch_forward = jit_beqi(JIT_R1, 0);
				pending_jumps[pending_jump_count++] = (PendingJump) {branch_forward, instruction.argument};
				break;
			case OP_JUMP_IF_NOT_ZERO:
				jit_ldr_uc(JIT_R1, JIT_V0);
				jit_node_t *branch_backwards = jit_bnei(JIT_R1, 0);
				pending_jumps[pending_jump_count++] = (PendingJump) {branch_backwards, instruction.argument};
				break;
			case OP_CLEAR_CELL:
				jit_movi(JIT_R0, 0);
				jit_str_c(JIT_V0, JIT_R0);
				break;
		}
	}

	// patch all saved branches
	for (size_t i = 0; i < pending_jump_count; i++) {
		const PendingJump jump = pending_jumps[i];
		jit_patch_at(jump.branch, labels[jump.target]);
	}

	jit_ret();

	const CompiledProgram fn = jit_emit();

	jit_clear_state();
	free(labels);
	free(pending_jumps);

	return fn;
}
