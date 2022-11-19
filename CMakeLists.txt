cmake_minimum_required(VERSION 3.23)
project(astparser C)

set(CMAKE_C_STANDARD 99)

add_executable(astparser src/main.c src/tokenizer.c src/ast.c src/builder.c include/builder.h)
