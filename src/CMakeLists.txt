cmake_minimum_required(VERSION 3.13.4)
project(Seres)

add_subdirectory(lib)
add_executable(main.exe main.cpp)

target_link_libraries(main.exe Frontend)
target_include_directories(main.exe PRIVATE ${PROJECT_SOURCE_DIR}/include/)