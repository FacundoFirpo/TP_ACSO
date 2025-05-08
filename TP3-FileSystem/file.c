#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "file.h"
#include "inode.h"
#include "diskimg.h"

/**
 * TODO
 */
int file_getblock(struct unixfilesystem *fs, int inumber, int blockNum, void *buf) {
    struct inode in;
    if (inode_iget(fs, inumber, &in) < 0) {
        return -1;
    }

    int diskBlock = inode_indexlookup(fs, &in, blockNum);
    if (diskBlock < 0) {
        return -1;
    }

    int res = diskimg_readsector(fs->dfd, diskBlock, buf);
    if (res != DISKIMG_SECTOR_SIZE) {
        return -1;
    }

    // Calcular el tamaño real de datos válidos en este bloque
    int filesize = inode_getsize(&in);
    int offset = blockNum * DISKIMG_SECTOR_SIZE;
    if (filesize > offset) {
        int valid_bytes = filesize - offset;
        if (valid_bytes > DISKIMG_SECTOR_SIZE) valid_bytes = DISKIMG_SECTOR_SIZE;
        return valid_bytes;
    } else {
        // El bloque solicitado está fuera del tamaño del archivo
        return 0;
    }
}

