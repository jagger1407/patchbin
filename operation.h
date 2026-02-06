#ifndef OPERATION_H
#define OPERATION_H

#include <stdio.h>
#include <stdint.h>

typedef enum optype_e : uint8_t {
    OP_INVALID,

    OP_INSERT,
    OP_REPLACE,

    OP_COUNT
} OperationType;

typedef struct operation_s {
    OperationType type;
    uint64_t offset;
    uint8_t* data;
} Operation;



#endif // OPERATION_H
