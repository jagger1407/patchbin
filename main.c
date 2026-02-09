#include <stdio.h>
#include <stdlib.h>

#include "argparse.h"
#include "operation.h"
#include "dirreader.h"

extern int big;

void printHelp() {
    puts("patchbin - Binary file patcher");
    puts("Syntax:\tpatchbin [options]\n");
    puts("Options:");
    for(int i=ARG_INVALID+1;i<ARG_COUNT;i++) {
        char* text = arg_GetArgHelp(i);
        puts(text);
        free(text);
        puts("");
    }
}

int main(int argc, char** argv)
{
    if(argc == 1) {
        printHelp();
        exit(EXIT_SUCCESS);
    }
    Argument* args[argc];
    int argcnt = 0;
    int fcnt = 0;
    int dcnt = 0;
    int opcnt = 0;

    char** arglist = argv+1;

    // Parsing Arguments
    Argument* cur = arg_Parse(arglist);
    while(cur != NULL) {
        if(cur->type == ARG_HELP) {
            printHelp();
            exit(EXIT_SUCCESS);
        }
        if(cur->type == ARG_FILE) {
            fcnt++;
        }
        if(cur->type == ARG_DIR) {
            dcnt++;
        }
        else if(op_ArgOpType(cur->type) != OP_INVALID) {
            opcnt++;
        }
        args[argcnt] = cur;
        argcnt++;
        arglist += cur->count + 1;
        if(arglist >= argv + argc) break;
        cur = arg_Parse(arglist);
    }
    // If no files are given, no patching will occur
    if(fcnt == 0 && dcnt == 0) {
        for(int a=0;a<argcnt;a++) {
            arg_Free(args[a]);
        }
        puts("No files given.");
        exit(EXIT_SUCCESS);
    }
    if(opcnt == 0) {
        for(int a=0;a<argcnt;a++) {
            arg_Free(args[a]);
        }
        puts("No operations given.");
        exit(EXIT_SUCCESS);
    }
    // Creating handles
    //puts("Creating handles...");
    Operation* ops[opcnt];
    char* fnames[fcnt];
    FILE* files[fcnt];
    char** dirs[dcnt];
    int dirfcnt[dcnt];

    int oidx = 0;
    int didx = 0;
    int fidx = 0;
    for(int i=0;i<argcnt;i++) {
        Argument* arg = args[i];
        if(arg->type == ARG_FILE) {
            files[fidx] = fopen(*arg->values, "rb+");
            if(files[fidx] == NULL) {
                fprintf(stderr, "ERROR: File '%s' couldn't be opened: ", *arg->values);
                perror(NULL);
            }
            fnames[fidx] = *arg->values;
            fidx++;
            continue;
        }
        else if(arg->type == ARG_DIR) {
            dirs[didx] = dir_GetFiles(*arg->values, &dirfcnt[didx]);
            didx++;
        }
        else if(op_ArgOpType(arg->type) != OP_INVALID) {
            ops[oidx] = op_Parse(arg);
            oidx++;
            continue;
        }
        else if(arg->type == ARG_ENDIAN) {
            if(strcmp(arg->values[0], "big") == 0) {
                big = 1;
            }
        }
    }

    // Applying patches
    for(int f=0;f<fcnt;f++) {
        if(files[f] == NULL) continue;
        printf("Applying patches to '%s'... ", fnames[f]);
        for(int o=0;o<opcnt;o++) {
            op_Apply(ops[o], files[f]);
        }
        fclose(files[f]);
        printf("Done.\n");
    }
    for(int d=0;d<dcnt;d++) {
        if(dirs[d] == NULL) continue;
        for(int df=0;df<dirfcnt[d];df++) {
            printf("Applying patches to '%s'... ", dirs[d][df]);
            FILE* fp = fopen(dirs[d][df], "rb+");
            if(fp == NULL) {
                fprintf(stderr, "ERROR: File '%s' couldn't be opened: ", dirs[d][df]);
                perror(NULL);
                continue;
            }
            for(int o=0;o<opcnt;o++) {
                op_Apply(ops[o], fp);
            }
            printf("Done.\n");
            fclose(fp);
        }

    }

    for(int d=0;d<dcnt;d++) {
        dir_Free(dirs[d], dirfcnt[d]);
    }
    for(int o=0;o<opcnt;o++) {
        op_Free(ops[o]);
    }
    for(int a=0;a<argcnt;a++) {
        arg_Free(args[a]);
    }
    exit(EXIT_SUCCESS);
}
