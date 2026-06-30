#ifndef BF_JIT_JIT_H
#define BF_JIT_JIT_H

#include "intermediate_representation.h"

typedef void (*CompiledProgram)(void);
CompiledProgram compile_jit(IR_Program program);

#endif // BF_JIT_JIT_H
