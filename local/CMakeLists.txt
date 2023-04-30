cmake_minimum_required(VERSION 3.22)
project(vec-add VERSION 0.0.1 DESCRIPTION "vec add" LANGUAGES C)

add_custom_command(OUTPUT vec-add.o vec-add.h
                COMMAND ispc --target=avx2 --arch=x86-64
                        ${CMAKE_SOURCE_DIR}/vec-add.ispc
                        -h vec-add.h -o vec-add.o
                DEPENDS vec-add.ispc)
add_library(vec-add STATIC vec-add.o)
set_target_properties(vec-add PROPERTIES LINKER_LANGUAGE C)
find_package(OpenMP REQUIRED)
find_package(ispcrt REQUIRED)
add_executable(ispc-vec-add simple.c)
target_link_libraries(ispc-vec-add  vec-add OpenMP::OpenMP_C ispcrt::ispcrt)
target_compile_definitions(ispc-vec-add PRIVATE KERNEL_FILE="${CMAKE_BINARY_DIR}/vec-add.h")
