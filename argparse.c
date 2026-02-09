#include "argparse.h"

struct _arg_id {
    int argcount;
    char* opt;
    char* verbose;
    char* description;
};

const struct _arg_id _args[] = {
    { 1, "-f", "--file", ": [File Path]\t\t\t- Specifies a file that is to be patched." },
    { 1, "-d", "--directory", ": [Directory Path]\t\t- Specifies a folder where every file in it should be patched." },
    { 0, "-h", "--help", ": Print help information." },

    { 2, "-i", "--insert", ": [Offset] [Bytes]\t\t- Insert bytes at given offset.\n\t\t  " \
        "With \"s:[string]\" it is possible to use an ASCII string.\n\t\t  " \
        "With \"f:[path]\" it is possible to specify the contents of a file."
    },
    { 2, "-r", "--replace", ": [Offset] [Bytes]\t\t- Replace bytes at given offset.\n\t\t  " \
        "With \"s:[string]\" it is possible to use an ASCII string.\n\t\t  " \
        "With \"f:[path]\" it is possible to specify the contents of a file."
    },
    { 3, "-a", "--add", ": [Offset] [Data Type] [Value]\t- Add a value to bytes at given offset.\n\t\t  " \
                          "Available data types: s8 u8 s16 u16 s32 u32 s64 u64 f32 f64"
    },
    { 3, "-s", "--set", ": [Offset] [Data Type] [Value]\t- Set bytes at given offset to particular value.\n\t\t  " \
        "Available data types: s8 u8 s16 u16 s32 u32 s64 u64 f32 f64"
    },
    { 1, "-e", "--endian", ": [Endian]\t\t\t- Select endian for operation.\n\t\t  " \
        "Available data types: little big (default: little)"
    },
};

bool arg_IsArg(const char* str) {
    if(str == NULL) return false;
    if(str[0] == '-') return true;
    else return false;
}

ArgumentType arg_GetType(const char* argstr) {
    if(!arg_IsArg(argstr)) return ARG_INVALID;

    ArgumentType t = ARG_INVALID + 1;
    for(t;t < ARG_COUNT; t++) {
        if( strcmp(_args[t].opt, argstr) == 0 ||
            strcmp(_args[t].verbose, argstr) == 0) {

            return t;
        }
    }
    return ARG_INVALID;
}

char* arg_GetArgHelp(ArgumentType type) {
    if(type == ARG_INVALID) return NULL;
    struct _arg_id arg = _args[type];
    uint32_t len = strlen(arg.opt) + 1 + strlen(arg.verbose) + 1 + strlen(arg.description);
    char* str = (char*)malloc(len+1);
    str[len] = 0x00;
    sprintf(str, "%s %s\t%s", arg.opt, arg.verbose, arg.description);
    return str;
}

int8_t _arg_GetNibble(char c) {
    if(c >= '0' && c <= '9') {
        return c - '0';
    }
    else if(c >= 'a' && c <= 'f') {
        return 10 + (c - 'a');
    }
    else if(c >= 'A' && c <= 'F') {
        return 10 + (c - 'A');
    }
    else return -1;
}

uint8_t* arg_ReadBytes(char* bytestr, uint64_t* len) {
    if(bytestr == NULL || *bytestr == 0x00) return NULL;

    // Specify text strings
    if(bytestr[0] == 's' && bytestr[1] == ':') {
        int slen = strlen(bytestr) - 2;
        if(len) {
            *len = slen;
        }
        uint8_t* bytes = (uint8_t*)malloc(slen+1);
        bytes[slen] = 0x00;
        memcpy(bytes, bytestr+2, slen);
        return bytes;
    }

    // Specify file path
    if(bytestr[0] == 'f' && bytestr[1] == ':') {
        FILE* fp = fopen(bytestr+2, "rb");
        if(fp == NULL) {
            fprintf(stderr, "ERROR: File '%s' couldn't be opened: ", bytestr+2);
            perror(NULL);
            return NULL;
        }
        uint64_t size = ftell(fp);
        fseek(fp, 0, SEEK_END);
        size = ftell(fp) - size;
        fseek(fp, 0, SEEK_SET);

        if(len) {
            *len = size;
        }

        uint8_t* bytes = (uint8_t*)malloc(size);
        fread(bytes, 1, size, fp);
        fclose(fp);
        return bytes;
    }

    uint64_t blen = 0;
    int digits = 0;

    char* cur = bytestr;
    while(*cur != 0x00) {
        if(cur[0] == '0' && cur[1] == 'x') {
            cur += 2;
        }
        if(*cur == ' ') {
            cur++;
            if(digits > 2) return NULL;
            if(digits == 2) blen++;
            digits = 0;
            continue;
        }
        digits++;
        cur++;
    }
    if(digits == 2) blen++;

    if(len) {
        *len = blen;
    }

    uint8_t* bytes = (uint8_t*)malloc(blen);
    memset(bytes, 0x00, blen);
    uint32_t idx = 0;
    digits = 1;

    cur = bytestr;
    while(*cur != 0x00) {
        if(cur[0] == '0' && cur[1] == 'x') {
            cur += 2;
        }
        if(*cur == ' ') {
            cur++;
            if(digits == -1) idx++;
            digits = 1;
            continue;
        }
        int8_t b = _arg_GetNibble(*cur);
        if(b == -1) {
            free(bytes);
            return NULL;
        }
        bytes[idx] += b << (digits * 4);
        digits--;
        cur++;
    }

    return bytes;
}

uint64_t arg_ReadValue(char* str) {
    if(str == NULL || *str == 0x00) return (uint64_t)-1;

    int64_t sign = 1;
    if(*str == '-') {
        sign = -1;
        str++;
    }

    if(str[0] != '0' || str[1] != 'x') {
        return sign * atoll(str);
    }

    uint64_t out = 0;

    char* cur = str + strlen(str) - 1;
    int idx = 0;

    while(*cur != 'x') {
        int8_t nibble = _arg_GetNibble(*cur);
        if(nibble == -1) {
            return (uint64_t)-1;
        }
        out += ((uint64_t)nibble) << (idx * 4);
        idx++;
        cur--;
    }

    return sign * out;
}


Argument* arg_Parse(char** args) {
    if(!arg_IsArg(*args)) return NULL;
    ArgumentType at = arg_GetType(*args);
    if(at == ARG_INVALID) return NULL;

    Argument* arg = (Argument*)malloc(sizeof(Argument));

    arg->type = at;
    arg->count = _args[at].argcount;
    if(arg->count > 0) {
        arg->values = (char**)malloc(sizeof(char*)*arg->count);
        for(int i=0;i<arg->count;i++) {
            arg->values[i] = args[1+i];
        }
    }

    return arg;
}

void arg_Free(Argument* arg) {
    if(arg->values) free(arg->values);
    free(arg);
}

