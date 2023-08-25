# common variables access  the compiler
set (PROJECT_CMAKE_VERSION 3.8)
# The C++ Compiler version
set (PROJECT_CXX_STANDARD 20)

cmake_minimum_required (VERSION ${PROJECT_CMAKE_VERSION})
set (CMAKE_CXX_STANDARD ${PROJECT_CXX_STANDARD})

project (ssc)

add_executable (ssc "main.cpp" "outstream.h" "outstream.cpp" "characters.h" "fmt.h" "fmt.cpp" "sys.h" "sys.cpp")

target_compile_definitions (ssc PUBLIC PROJECT_SOURCE_PATH=\"${PROJECT_SOURCE_DIR}\")
