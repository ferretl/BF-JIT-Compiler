cmake_minimum_required(VERSION 4.2)
project(BF_JIT C)

if (APPLE)
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew")
endif ()

# Find GNU lightning
find_path(LIGHTNING_INCLUDE_DIR lightning.h)
find_library(LIGHTNING_LIBRARY lightning)

include_directories(${LIGHTNING_INCLUDE_DIR})

set(CMAKE_C_STANDARD 23)

add_executable(BF_JIT main.c
        intermediate_representation.h
        intermediate_representation.c
        jit.c
        jit.h
        tests/test_harness.h
)
target_link_libraries(BF_JIT ${LIGHTNING_LIBRARY})