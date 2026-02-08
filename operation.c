#include "operation.h"

const struct {
    OperationType otype;
    ArgumentType atype;
} mappings[] = {
    { OP_INSERT, ARG_INSERT },
    { OP_REPLACE, ARG_REPLACE },
    { OP_ADD, ARG_ADD },
    { OP_SET, ARG_SET }
};

int big = 0;

typedef void (*operation)(Operation* op, FILE* fp);
const operation operations[] = {
    &op_InsertData,
    &op_ReplaceData,
    &op_AddValue,
    &op_SetValue,
};

const char* opdt_identifiers[] = {
    "s8",
    "u8",
    "s16",
    "u16",
    "s32",
    "u32",
    "s64",
    "u64",
    "f32",
    "f64"
};

OperationDataType op_GetDataType(char* dtstr) {
    if(dtstr == NULL || *dtstr == 0x00) return OPDT_INVALID;

    for(int i=0;i<OPDT_COUNT;i++) {
        if(strcmp(dtstr, opdt_identifiers[i]) == 0) {
            return i;
        }
    }
    return OPDT_INVALID;
}

uint64_t op_ReadValue(OperationDataType dt, char* str) {
    if(dt != OPDT_FLOAT && dt != OPDT_DOUBLE) return arg_ReadValue(str);

    float sign = 1;
    if(*str == '-') {
        sign = -1;
        str++;
    }

    if(dt == OPDT_FLOAT) {
        float f = sign * atof(str);
        return *((uint64_t*)&f);
    }
    else {
        double d = sign * atof(str);
        return *((uint64_t*)&d);
    }
}

Operation* op_Parse(Argument* arg) {
    if(arg == NULL) return NULL;

    Operation* op = (Operation*)malloc(sizeof(Operation));
    memset(op, 0x00, sizeof(Operation));

    op->type = op_ArgOpType(arg->type);
    if(op->type == OP_INVALID) {
        op_Free(op);
        return NULL;
    }

    op->offset = arg_ReadValue(arg->values[0]);
    switch(op->type) {
        case OP_INSERT:
        case OP_REPLACE:
            op->data = arg_ReadBytes(arg->values[1], &op->datalen);
            break;
        case OP_ADD:
        case OP_SET:
            op->datatype = op_GetDataType(arg->values[1]);
            if(op->datatype == OPDT_INVALID) {
                op_Free(op);
                return NULL;
            }
            op->value = op_ReadValue(op->datatype, arg->values[2]);
        default:
            break;
    }


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

void op_SwapEndian(void* ptr, int len) {
    uint8_t* bptr = (uint8_t*)ptr;
    for(int i=0;i<(len/2);i++) {
        uint8_t tmp = bptr[i];
        bptr[i] = bptr[len-1-i];
        bptr[len-1-i] = tmp;
    }
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

void op_AddValue(Operation* op, FILE* fp) {
    if(op == NULL || fp == NULL || op->datatype == OPDT_INVALID) return;

    int dtlen[] = { 1, 1, 2, 2, 4, 4, 8, 8, 4, 8 };

    uint8_t ptr[dtlen[op->datatype]];

    fseek(fp, op->offset, SEEK_SET);
    fread(ptr, 1, dtlen[op->datatype], fp);

    switch(op->datatype) {
        case OPDT_BYTE:
            *((int8_t*)ptr) += (int8_t)op->value;
            break;
        case OPDT_UBYTE:
            *((uint8_t*)ptr) += (uint8_t)op->value;
            break;
        case OPDT_SHORT:
            *((int16_t*)ptr) += (int16_t)op->value;
            break;
        case OPDT_USHORT:
            *((uint16_t*)ptr) += (uint16_t)op->value;
            break;
        case OPDT_INT:
            *((int32_t*)ptr) += (int32_t)op->value;
            break;
        case OPDT_UINT:
            *((uint32_t*)ptr) += (uint32_t)op->value;
            break;
        case OPDT_LONG:
            *((int64_t*)ptr) += (int64_t)op->value;
            break;
        case OPDT_ULONG:
            *((uint64_t*)ptr) += op->value;
            break;
        case OPDT_FLOAT:
            *((float*)ptr) += *((float*)&op->value);
            break;
        case OPDT_DOUBLE:
            *((double*)ptr) += *((double*)&op->value);
            break;
        default:
            break;
    }

    if(big && dtlen[op->datatype] > 1) {
        op_SwapEndian(&op->value, dtlen[op->datatype]);
    }

    fseek(fp, op->offset, SEEK_SET);
    fwrite(ptr, 1, dtlen[op->datatype], fp);
}

void op_SetValue(Operation* op, FILE* fp) {
    if(op == NULL || fp == NULL || op->datatype == OPDT_INVALID) return;

    int dtlen[] = { 1, 1, 2, 2, 4, 4, 8, 8, 4, 8 };

    if(big && dtlen[op->datatype] > 1) {
        op_SwapEndian(&op->value, dtlen[op->datatype]);
    }

    fseek(fp, op->offset, SEEK_SET);
    fwrite(&op->value, 1, dtlen[op->datatype], fp);
}

void op_Apply(Operation* op, FILE* fp) {
    if(op == NULL || fp == NULL || op->type == OP_INVALID) return;

    operations[op->type](op, fp);
}



