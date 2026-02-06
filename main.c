#include <stdio.h>
#include <stdlib.h>

#include "argparse.h"

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
    int cnt = 0;

    char** arglist = argv+1;

    Argument* cur = arg_Parse(arglist);
    while(cur != NULL) {
        if(cur->type == ARG_HELP) {
            printHelp();
            exit(EXIT_SUCCESS);
        }
        args[cnt] = cur;
        cnt++;
        arglist += cur->count + 1;
        cur = arg_Parse(arglist);

    }



    char* argtypes[] = {
        "",
        "ARG_FILE",
        "ARG_INSERT",
        "ARG_REPLACE",
        "ARG_HELP",
    };

    puts("Arguments found:");
    for(int i=0;i<cnt;i++) {
        Argument* arg = args[i];
        printf("Argument %s : Values", argtypes[arg->type]);
        for(int v=0;v<arg->count;v++) {
            printf(" '%s'", arg->values[v]);
        }
        printf("\n");
        arg_Free(arg);
    }



    exit(EXIT_SUCCESS);
}
