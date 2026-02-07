#include "operation.h"

const struct {
    OperationType otype;
    ArgumentType atype;
} mappings[] = {
    { OP_INSERT, ARG_INSERT },
    { OP_REPLACE, ARG_REPLACE }
};

Operation* op_Parse(Argument* arg) {
    if(arg == NULL) return NULL;

    Operation* op = (Operation*)malloc(sizeof(Operation));
    memset(op, 0x00, sizeof(Operation));

    switch(arg->type) {
        case ARG_INSERT:
            op->type = OP_INSERT;
            break;
        case ARG_REPLACE:
            op->type = OP_REPLACE;
            break;
        default:
            op->type = OP_INVALID;
            break;
    }
    if(op->type == OP_INVALID) {
        free(op);
        return NULL;
    }

    op->offset = arg_ReadOffset(arg->values[0]);
    op->data = arg_ReadBytes(arg->values[1], &op->datalen);

    return op;
}

void op_Free(Operation* op) {
    if(op == NULL) return;

    if(op->data) {
        free(op->data);
    }
    free(op);
}

OperationType op_ArgOpType(ArgumentType type) {
    for(int i=0;i<OP_COUNT;i++) {
        if(mappings[i].atype == type)
            return mappings[i].otype;
    }
    return OP_INVALID;
}


void op_InsertData(Operation* op, FILE* fp) {
    if(fp == NULL || op == NULL || op->data == NULL) return;

    fseek(fp, 0x00, SEEK_SET);
    uint64_t size = ftell(fp);
    fseek(fp, 0x00, SEEK_END);
    size = ftell(fp) - size;

    fseek(fp, op->offset, SEEK_SET);
    uint8_t* fdata = (uint8_t*)malloc(size - op->offset);
    fread(fdata, 1, size - op->offset, fp);

    fseek(fp, op->offset, SEEK_SET);
    fwrite(op->data, 1, op->datalen, fp);
    fwrite(fdata, 1, size - op->offset, fp);
}

void op_ReplaceData(Operation* op, FILE* fp) {
    if(fp == NULL || op == NULL || op->data == NULL) return;

    fseek(fp, op->offset, SEEK_SET);
    fwrite(op->data, 1, op->datalen, fp);
}


void op_Apply(Operation* op, FILE* fp) {
    if(op == NULL || fp == NULL || op->type == OP_INVALID) return;

    switch(op->type) {
        case OP_INSERT:
            op_InsertData(op, fp);
            break;
        case OP_REPLACE:
            op_ReplaceData(op, fp);
            break;
        default:
            break;
    }
}



