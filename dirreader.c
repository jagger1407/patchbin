#include "dirreader.h"

#ifdef __linux__

char** dir_GetFiles(const char* path, int* cnt) {
    if(path == NULL || *path == 0x00) return NULL;

    DIR* dir = opendir(path);
    if(!dir) {
        perror(NULL);
        return NULL;
    }

    int fcnt = 0;

    int plen = strlen(path);

    struct dirent* entry = readdir(dir);
    while(entry != NULL) {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {

            entry = readdir(dir);
            continue;
        }
        int flen = plen + strlen(entry->d_name) + 2;
        char fpath[flen];
        memset(fpath, 0x00, flen);
        strncpy(fpath, path, flen-1);
        if(fpath[strlen(fpath)-1] != '/') {
            fpath[strlen(fpath)] = '/';
        }
        strcat(fpath, entry->d_name);
        struct stat st;
        stat(fpath, &st);
        if(S_ISDIR(st.st_mode)) {
            continue;
        }

        fcnt++;

        entry = readdir(dir);
    }
    rewinddir(dir);

    if(cnt) {
        *cnt = fcnt;
    }
    if(fcnt == 0) return NULL;
    char** dirfiles = (char**)malloc(sizeof(char*)*fcnt);
    memset(dirfiles, 0x00, sizeof(char*)*fcnt);
    int idx = 0;

    entry = readdir(dir);
    while(entry != NULL) {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {

            entry = readdir(dir);
            continue;
        }
        int flen = plen + strlen(entry->d_name) + 2;
        char fpath[flen];
        memset(fpath, 0x00, flen);
        strncpy(fpath, path, flen-1);
        if(fpath[strlen(fpath)-1] != '/') {
            fpath[strlen(fpath)] = '/';
        }
        strcat(fpath, entry->d_name);
        struct stat st;
        stat(fpath, &st);
        if(S_ISDIR(st.st_mode)) {
            continue;
        }
        char* str = (char*)malloc(flen);
        memset(str, 0x00, flen);
        strcpy(str, fpath);
        dirfiles[idx] = str;
        idx++;
        entry = readdir(dir);
    }

    closedir(dir);
    return dirfiles;
}

#endif
#ifdef _WIN32

char** dir_GetFiles(const char* path, int* cnt) {
    if(path == NULL || *path == 0x00) return NULL;

    int fcnt = 0;
    int plen = strlen(path);

    char searchpath[MAX_PATH];
    snprintf(searchpath, sizeof(searchpath), "%s\\*", path);

    WIN32_FIND_DATA fd;
    HANDLE hFind = FindFirstFile(searchpath, &fd);

    if (hFind == INVALID_HANDLE_VALUE) {
        fputs("Error opening directory.\n", stderr);
        return NULL;
    }

    do {
        if (strcmp(fd.cFileName, ".") == 0 ||
            strcmp(fd.cFileName, "..") == 0 ||
            (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            continue;
            }

            fcnt++;

    } while (FindNextFile(hFind, &fd) != 0);
    FindClose(hFind);

    if(cnt) {
        *cnt = fcnt;
    }
    if(fcnt == 0) return NULL;
    char** dirfiles = (char**)malloc(sizeof(char*)*fcnt);
    memset(dirfiles, 0x00, sizeof(char*)*fcnt);
    int idx = 0;

    FindFirstFile(searchpath, &fd);
    if (hFind == INVALID_HANDLE_VALUE) {
        fputs("Error opening directory.\n", stderr);
        return NULL;
    }
    do {
        if (    strcmp(fd.cFileName, ".") == 0 ||
                strcmp(fd.cFileName, "..") == 0 ||
                (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            continue;
        }
        int flen = plen + strlen(fd.cFileName) + 2;
        char fpath[flen];
        memset(fpath, 0x00, flen);
        strncpy(fpath, path, flen-1);
        if(fpath[strlen(fpath)-1] != '\\') {
            fpath[strlen(fpath)] = '\\';
        }
        strcat(fpath, fd.cFileName);
        char* str = (char*)malloc(flen);
        memset(str, 0x00, flen);
        strcpy(str, fpath);
        dirfiles[idx] = str;
        idx++;
    } while (FindNextFile(hFind, &fd) != 0);
    FindClose(hFind);

    return dirfiles;
}

#endif

void dir_Free(char ** dirfiles, int cnt) {
    if(dirfiles == NULL) return;
    for(int i=0;i<cnt;i++) {
        if(dirfiles[i]) {
            free(dirfiles[i]);
        }
    }
    free(dirfiles);
}

