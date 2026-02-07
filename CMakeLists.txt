cmake_minimum_required(VERSION 3.10)

project(patchbin LANGUAGES C)

if(CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE Release)
endif()

set(PROJECT_SOURCES
    argparse.c
    operation.c
    main.c
)

add_executable(patchbin ${PROJECT_SOURCES})

install(TARGETS patchbin RUNTIME DESTINATION bin)
