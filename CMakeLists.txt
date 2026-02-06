cmake_minimum_required(VERSION 3.10)

project(patchbin LANGUAGES C)

add_executable(patchbin main.c)

install(TARGETS patchbin RUNTIME DESTINATION bin)
