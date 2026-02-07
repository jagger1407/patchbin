#include <stdio.h>
#include <stdlib.h>

#include "argparse.h"
#include "operation.h"

void printHelp() {
    puts("patchbin - Binary file patcher");
    puts("Syntax:\tpatchbin [options]\n");
    puts("Options:");
    for(int i=ARG_INVALID+1;i<ARG_COUNT;i++) {
        char* text = arg_GetArgHelp(i);
        puts(text);
        free(text);
    }
}

int main(int argc, char** argv)
{
    Argument* args[argc];
    int argcnt = 0;
    int fcnt = 0;
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
        else if(op_ArgOpType(cur->type) != OP_INVALID) {
            opcnt++;
        }
        args[argcnt] = cur;
        argcnt++;
        arglist += cur->count + 1;
        cur = arg_Parse(arglist);

    }
    // Creating handles
    puts("Creating handles...");
    Operation* ops[opcnt];
    char* fnames[fcnt];
    FILE* files[fcnt];

    int oidx = 0;
    int fidx = 0;
    for(int i=0;i<argcnt;i++) {
        Argument* arg = args[i];
        if(arg->type == ARG_FILE) {
            files[fidx] = fopen(*arg->values, "rb+");
            if(files[fidx] == NULL) {
                fprintf(stderr, "ERROR: Files '%s' couldn't be opened: ", *arg->values);
                perror(NULL);
            }
            fnames[fidx] = *arg->values;
            fidx++;
            continue;
        }
        else if(op_ArgOpType(arg->type) != OP_INVALID) {
            ops[oidx] = op_Parse(arg);
            oidx++;
            continue;
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

    for(int o=0;o<opcnt;o++) {
        op_Free(ops[o]);
    }
    for(int a=0;a<argcnt;a++) {
        arg_Free(args[a]);
    }
    exit(EXIT_SUCCESS);
}
