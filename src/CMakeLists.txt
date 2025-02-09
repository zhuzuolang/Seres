cmake_minimum_required(VERSION 3.13.4)
project(Seres)

set(LLVM_ENABLE_PROJECTS "clang;clang-tools-extra;lld")
set(LLVM_DIR ${CMAKE_BINARY_DIR}/third_party/llvm-project-llvmorg-19.1.0/llvm/lib/cmake/llvm CACHE PATH llvm_dir_entry FORCE)
add_subdirectory(third_party)
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_subdirectory(lib)
add_executable(main.exe main.cpp)

llvm_map_components_to_libnames(llvm_libs all)
target_link_libraries(main.exe Frontend ${llvm_libs})
target_include_directories(main.exe PRIVATE ${PROJECT_SOURCE_DIR}/include/)