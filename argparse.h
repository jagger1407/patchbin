#ifndef ARGPARSE_H
#define ARGPARSE_H

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef enum argtype_e {
    ARG_INVALID,

    ARG_FILE,
    ARG_INSERT,
    ARG_REPLACE,
    ARG_HELP,

    ARG_COUNT
} ArgumentType;

typedef struct argument_s{
    ArgumentType type;
    int count;
    char** values;
} Argument;

bool arg_IsArg(const char* str);

ArgumentType arg_GetType(const char* argstr);

char* arg_GetArgHelp(ArgumentType type);

uint8_t* arg_ReadBytes(char* bytestr);

Argument* arg_Parse(char** args);

void arg_Free(Argument* arg);

#endif // ARGPARSE_H
