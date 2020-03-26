#ifndef _FATRW_
#define _FATRW

#include "jdisk.h"

#define JDISK_SECTOR_SIZE (1024)
#define JDISK_DELAY (1)

void file_import(char*, char*);
void file_export(char * diskname,unsigned long starting_block, char* ofname);

#endif
