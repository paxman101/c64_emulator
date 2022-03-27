cmake_minimum_required(VERSION 3.20)
project(c64_emulator)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(6510_Emulator)

find_package(Threads REQUIRED)

add_definitions(-DHAVE_LIBC)
add_definitions(-DHAVE_STDIO_H)
set( SDL_STATIC ON CACHE BOOL "" FORCE )
set( SDL_SHARED OFF CACHE BOOL "" FORCE )
add_subdirectory(SDL)

add_executable(c64_emulator main.cpp vic.cpp vic.h io.cpp io.h memory.cpp memory.h sid.cpp sid.h colorram.cpp colorram.h cia.cpp cia.h)

target_link_libraries(c64_emulator 6510 Threads::Threads SDL2::SDL2main SDL2-static)

add_compile_definitions(PROJECT_ROOT=\"${PROJECT_SOURCE_DIR}\")
