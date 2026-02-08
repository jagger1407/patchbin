#ifndef OPERATION_H
#define OPERATION_H

#include <stdio.h>
#include <stdint.h>

#include "argparse.h"

typedef enum optype_e : int8_t {
    OP_INVALID = -1,

    OP_INSERT,
    OP_REPLACE,
    OP_ADD,
    OP_SET,

    OP_COUNT
} OperationType;

typedef enum opdtype_e : int8_t {
    OPDT_INVALID = -1,

    OPDT_BYTE,
    OPDT_UBYTE,
    OPDT_SHORT,
    OPDT_USHORT,
    OPDT_INT,
    OPDT_UINT,
    OPDT_LONG,
    OPDT_ULONG,
    OPDT_FLOAT,
    OPDT_DOUBLE,

    OPDT_COUNT
} OperationDataType;

typedef struct operation_s {
    OperationType type;
    uint64_t offset;
    uint8_t* data;
    uint64_t datalen;
    uint64_t value;
    OperationDataType datatype;
} Operation;

Operation* op_Parse(Argument* arg);

void op_Free(Operation* op);

OperationType op_ArgOpType(ArgumentType type);

void op_InsertData(Operation* op, FILE* fp);

void op_ReplaceData(Operation* op, FILE* fp);

void op_AddValue(Operation* op, FILE* fp);

void op_SetValue(Operation* op, FILE* fp);

void op_Apply(Operation* op, FILE* fp);

#endif // OPERATION_H
