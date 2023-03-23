cmake_minimum_required(VERSION 3.14)

project(simple)
find_package(OpenMP REQUIRED)
find_package(ispcrt REQUIRED)
add_executable(host_simple simple.c)
add_ispc_kernel(xe_simple vec-add.ispc "")
target_link_libraries(host_simple PRIVATE OpenMP::OpenMP_C ispcrt::ispcrt)
