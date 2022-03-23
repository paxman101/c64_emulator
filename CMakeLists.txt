cmake_minimum_required(VERSION 3.20)
project(c64_emulator)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(6510_Emulator)

find_package(Threads REQUIRED)

add_executable(c64_emulator main.cpp vic.cpp vic.h io.cpp io.h memory.cpp memory.h sid.cpp sid.h)

target_link_libraries(c64_emulator 6510 Threads::Threads)

add_compile_definitions(PROJECT_ROOT=\"${PROJECT_SOURCE_DIR}\")
