#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "inode.h"
#include "diskimg.h"


/**
 * TODO
 */
int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp) {
    if (inumber < 1) return -1;

    int inodes_per_block = DISKIMG_SECTOR_SIZE / sizeof(struct inode);
    int inode_block = (inumber - 1) / inodes_per_block + INODE_START_SECTOR;
    int inode_offset = (inumber - 1) % inodes_per_block;

    struct inode buf[inodes_per_block];
    int res = diskimg_readsector(fs->dfd, inode_block, buf);
    if (res != DISKIMG_SECTOR_SIZE) {
        return -1;
    }

    *inp = buf[inode_offset];
    return 0;
}

/**
 * TODO
 */
 int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp, int blockNum) {
    if (!(inp->i_mode & IALLOC)) return -1;
    if (blockNum < 0) return -1;

    // Archivos pequeños (no ILARG)
    if (!(inp->i_mode & ILARG)) {
        if (blockNum >= 8) return -1;
        int block = inp->i_addr[blockNum];
        if (block == 0) return -1;
        return block;
    } else {
        // Archivos grandes (ILARG): acceso indirecto
        // No implementado aquí, pero se deja el esqueleto
        // Se debería leer el bloque indirecto y buscar el bloque físico
        return -1;
    }
}

int inode_getsize(struct inode *inp) {
  return ((inp->i_size0 << 16) | inp->i_size1); 
}
