
#include "pathname.h"
#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/**
 * TODO
 */
int pathname_lookup(struct unixfilesystem *fs, const char *pathname) {
    if (!pathname || pathname[0] != '/') {
        return -1;
    }

    // Comenzar desde el inodo raíz
    int curr_inumber = 1;
    char path_copy[256];
    strncpy(path_copy, pathname, sizeof(path_copy));
    path_copy[sizeof(path_copy) - 1] = '\0';

    char *token = strtok(path_copy, "/");
    while (token != NULL) {
        struct direntv6 entry;
        int res = directory_findname(fs, token, curr_inumber, &entry);
        if (res < 0) {
            return -1;
        }
        curr_inumber = entry.d_inumber;
        token = strtok(NULL, "/");
    }

    return curr_inumber;
}
