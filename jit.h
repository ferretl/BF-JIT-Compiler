#ifndef BF_JIT_JIT_H
#define BF_JIT_JIT_H

#include <lightning.h>
#include "intermediate_representation.h"

typedef struct {
	jit_node_t *branch;
	size_t target;
} PendingJump;

typedef void (*CompiledProgram)(unsigned char *tape);
CompiledProgram compile_jit(const IR_Program *program);

#endif // BF_JIT_JIT_H
