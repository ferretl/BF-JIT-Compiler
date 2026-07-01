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
)
target_link_libraries(BF_JIT ${LIGHTNING_LIBRARY})

enable_testing()

add_executable(test_ir
        tests/test_ir.c
        intermediate_representation.c)
target_include_directories(test_ir PRIVATE ${CMAKE_SOURCE_DIR})

add_test(NAME ir_tests COMMAND test_ir)

add_custom_target(check
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        DEPENDS test_ir
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})