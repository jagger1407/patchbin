#ifndef DIRREADER_H
#define DIRREADER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef __linux__

#include <sys/stat.h>
#include <dirent.h>
char** dir_GetFiles(const char* path, int* cnt);

#endif
#ifdef _WIN32
#undef __linux__

#include <windows.h>
char** dir_GetFiles(const char* path, int* cnt);

#endif

void dir_Free(char** dirfiles, int cnt);

#endif // DIRREADER_H
