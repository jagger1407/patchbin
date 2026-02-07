#ifndef OPERATION_H
#define OPERATION_H

#include <stdio.h>
#include <stdint.h>

#include "argparse.h"

typedef enum optype_e : int8_t {
    OP_INVALID = -1,

    OP_INSERT,
    OP_REPLACE,

    OP_COUNT
} OperationType;

typedef struct operation_s {
    OperationType type;
    uint64_t offset;
    uint8_t* data;
    uint64_t datalen;
} Operation;

Operation* op_Parse(Argument* arg);

void op_Free(Operation* op);

OperationType op_ArgOpType(ArgumentType type);

void op_InsertData(Operation* op, FILE* fp);

void op_ReplaceData(Operation* op, FILE* fp);

void op_Apply(Operation* op, FILE* fp);

#endif // OPERATION_H
