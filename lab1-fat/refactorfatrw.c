#include <stdio.h>
#include <math.h>
#include "fatrw.h"
#include <stdlib.h>
#include <string.h>
#include "jdisk.h"

#define SIZE_S 1024

/*struct {
  unsigned short *entry
  };fat*/

int fat_size(char *diskfile){//might need to change from ints
	FILE * pFile;
	pFile = fopen(diskfile, "r");
	fseek(pFile, 0L, SEEK_END);
	int disksize = ftell(pFile);
	int size_fat = (disksize / SIZE_S + 1) * sizeof(short) / SIZE_S + 1;
	if((disksize / SIZE_S + 1 - size_fat) * sizeof(short) / SIZE_S < size_fat){
		size_fat = (disksize / SIZE_S + 1 - size_fat) * sizeof(short) / SIZE_S + 1;
	}
	return size_fat;//need to confirm edge cases!
}

/*void file_import(char *diskfile, char *ifname){

	//open file and get size
	FILE * pFile;
	pFile = fopen(ifname, "r");
	fseek(pFile, 0L, SEEK_END);
	int fsize = ftell(pFile);
	//printf("filesize: %d", sz);
}*/

unsigned short get_file_link(char *diskfile, unsigned short fat_link){

}

unsigned short * init_fat(void *diskp){
	//unsigned short* fat_table;
	void * fat_table;

	int fatSize = (jdisk_size(diskp) / JDISK_SECTOR_SIZE + 1) * sizeof(short) /  JDISK_SECTOR_SIZE + 1;

	printf("fatSize: %d\n\n", fatSize);

	fat_table = malloc(fatSize * (JDISK_SECTOR_SIZE)+1);

	memset(fat_table, 0, sizeof(unsigned short) * fatSize * JDISK_SECTOR_SIZE);

	jdisk_read(diskp, 0, fat_table);

	return fat_table;
}

int get_fat_size(void *diskp){
	int fatSize = (jdisk_size(diskp) / JDISK_SECTOR_SIZE + 1) * sizeof(short) /  JDISK_SECTOR_SIZE + 1;
	return fatSize;
}

void file_export(char * diskfile, unsigned int sector_start, char * ofname){
	//if(*(fat_table + x) == x) // then last sector in file
	
	printf("HERE I AM\n");
	void * diskp = jdisk_attach(diskfile);
	unsigned long j = jdisk_size(diskp);
	unsigned short * fat_table = init_fat(diskp);

	FILE * pFile;

	int num_fat_sectors = get_fat_size(diskp);

	pFile = fopen(ofname, "a+");
	if (pFile == NULL){
		printf("incorrect output file\n");
		exit(1);
	}
		
	//allocate space for pointer array
	unsigned short ** SectorPointers = malloc(sizeof(unsigned short) * num_fat_sectors);

	int i;
	//initialize pointer array
	for(i=0; i<num_fat_sectors; i++){
		SectorPointers[i]=NULL;
	}
	SectorPointers[0] = fat_table;

	char * buf = malloc(JDISK_SECTOR_SIZE);

	unsigned int whichSector = sector_start / (JDISK_SECTOR_SIZE/sizeof(unsigned short));

	int offset = sector_start - (JDISK_SECTOR_SIZE/sizeof(unsigned short) * whichSector);//deal with num sectors
	int current;
	int next = *(*(SectorPointers + whichSector) + offset);
	do{
		current = next;

		whichSector = current / (JDISK_SECTOR_SIZE/sizeof(unsigned short));

		offset = current - (JDISK_SECTOR_SIZE/sizeof(unsigned short) * whichSector);
		//current = *(*(SectorPointers + whichSector) + offset);
		next = *(*(SectorPointers + whichSector) + offset);
		printf("curren: %d\nnext: %d\n", current, next);
		if(next == 0 || next != current){
			jdisk_read(diskp, current, buf);
			fwrite(buf, JDISK_SECTOR_SIZE, 1, pFile);
		}
		else{
			jdisk_read(diskp, current, buf);
			unsigned short size = (unsigned short)*(buf + 1022);
			printf("buf 1020: %d, buf 1021: %d, buf 1022: %d, buf 1023: %d, buf 1024: %d\n",*(buf + 1020),*(buf + 1021),*(buf + 1022),*(buf + 1023), *(buf + 1024));

			printf("size: %d\n", size);
			fwrite(buf, size, 1, pFile);
		}
	}while(current != next && next != 0);//might have to do loop one more time?
}

void file_import(char * diskfile, char * ifname){
    void * diskp = jdisk_attach(diskfile);
	unsigned long j = jdisk_size(diskp);
	unsigned short * fat_table = init_fat(diskp);
	//printf("fat_table: %d\n", *fat_table);
	//printf("fat_table: %d\n", *(fat_table+1));

	int num_fat_sectors = get_fat_size(diskp);

	unsigned short ** SectorPointers = malloc(sizeof(unsigned short) * num_fat_sectors);
	int i;
	for(i=0; i<num_fat_sectors; i++){
		SectorPointers[i]=NULL;
	}
	SectorPointers[0] = fat_table;

	FILE * pFile;
	pFile = fopen(ifname, "r");
	fseek(pFile, 0L, SEEK_END);
	int fsize = ftell(pFile);

	void * buf = malloc(JDISK_SECTOR_SIZE);

	rewind(pFile);

	fread(buf,1,JDISK_SECTOR_SIZE,pFile);

	/*if(*(fat_table + x) == NULL){//not sure how to memset to null or what to check against
		jdisk_read(diskp, x/JDISK_SECTOR_SIZE, fat_table + x/JDISK_SECTOR_SIZE);
	}*/

	printf("TEST!\n");

	int file_start = *fat_table;
	//printf("checking indexing: %d, %d\n", *fat_table, *(fat_table+511));
	unsigned short move1, move2;
	unsigned int whichSector = *fat_table/(JDISK_SECTOR_SIZE/sizeof(unsigned short));
	printf("whichSector %d\n", whichSector);

	while(fsize>JDISK_SECTOR_SIZE){
		whichSector = *fat_table/(JDISK_SECTOR_SIZE/sizeof(unsigned short));
		printf("whichsector: %d\n", whichSector);
		printf("SectorPointers[0]: %d\n", *SectorPointers[0]);
		if(*(SectorPointers + whichSector)==NULL){
			*(SectorPointers + whichSector) = malloc((JDISK_SECTOR_SIZE+1));
			printf("SecrorP: %d\n", *(SectorPointers + whichSector));
			//int result = jdisk_read(diskp, whichSector, *(SectorPointers + whichSector));
			jdisk_read(diskp, whichSector, *(SectorPointers + whichSector));
			//printf("inSectorP %d\nresult: %d\n", **(SectorPointers + whichSector), result);
		}
		//jdisk_write(diskp, (**(SectorPointers + whichSector) + (*fat_table) - (whichSector * JDISK_SECTOR_SIZE/sizeof(unsigned short))) + num_fat_sectors - 1, buf);
		jdisk_write(diskp, (**(SectorPointers + whichSector) + (*fat_table) - (whichSector * JDISK_SECTOR_SIZE/sizeof(unsigned short))) + num_fat_sectors - 1, buf);
		//printf("indexing into Sector Pointers: %d\n", (**(SectorPointers + whichSector) + (*fat_table) - (whichSector * JDISK_SECTOR_SIZE/sizeof(unsigned short))) + num_fat_sectors - 1);
		fread(buf,1,JDISK_SECTOR_SIZE,pFile);
		fsize -= JDISK_SECTOR_SIZE;
		//printf("whichSector: %d\n", whichSector);
		*fat_table = *(*(SectorPointers + whichSector) + *fat_table - whichSector * JDISK_SECTOR_SIZE/sizeof(unsigned short));
	}
	
		//write_sector = *fat_table
		
	//if(fsize - JDISK_SECTOR_SIZE == 0){
	//}
	//else{//writing last sector, might need to be changed
		
	//printf("num_fat_sectors: %d, fat_table_def: %d!\n",  num_fat_sectors, *fat_table);

	whichSector = *fat_table/(JDISK_SECTOR_SIZE/sizeof(unsigned short));
	if(fsize == JDISK_SECTOR_SIZE){
		//change current to 0
	}
	else{
		printf("fsize: %d\n", fsize);
		unsigned short * ubuf = buf;
		if(fsize >= 255){
			*(ubuf + JDISK_SECTOR_SIZE/2-1) = 255;
			*(ubuf + JDISK_SECTOR_SIZE/2-2) = fsize - 255;
		}
		else{
			*(ubuf + (JDISK_SECTOR_SIZE/2* *(*(SectorPointers + whichSector)+num_fat_sectors)-1)) = (unsigned short)fsize;
			//printf
		}
	}
	if(*(SectorPointers + whichSector)==NULL){//no idea what this does rn
		printf("TEST5!\n");
		*(SectorPointers + whichSector) = malloc((JDISK_SECTOR_SIZE)+1);
		jdisk_read(diskp, whichSector, *(SectorPointers + whichSector));
		printf("inSectorP %d\n", **(SectorPointers + whichSector));
	}
		printf("TEST2!\n");
	jdisk_write(diskp, (**(SectorPointers + whichSector) + (*fat_table) - (whichSector * JDISK_SECTOR_SIZE/sizeof(unsigned short))) + num_fat_sectors - 1, buf);
		printf("TEST3!\n");
	move1 = *fat_table;
	move2 = *(*(SectorPointers + whichSector) + move1 - (JDISK_SECTOR_SIZE/sizeof(unsigned short)) * whichSector);
	*fat_table = move2;
	printf("out fattable: %d\nmove1: %d\n", *fat_table, move1);
	printf("arithmetic: %d!\nwhichSector: %d\n", (JDISK_SECTOR_SIZE/sizeof(unsigned short)) * whichSector, whichSector);
	//*(fat_table + move1) = move1;
	*(*SectorPointers + whichSector + move1 - JDISK_SECTOR_SIZE *whichSector /sizeof(unsigned short)) = move1;
	for(i=0; i<num_fat_sectors; i++){
		if(SectorPointers[i]!=NULL){
			jdisk_write(diskp, i, *(SectorPointers + i));
		}
	}
	jdisk_write(diskp, 0, fat_table);
	//}//account for when not enough sectors
	

	printf("New file starts at sector %d\nReads: %d\nWrites: %d\n",file_start, jdisk_reads(diskp), jdisk_writes(diskp));
}

int main(int argc, char** argv){//will eventually rename import
	//printf("size s = %d, sizeof short = %d\n\n", SIZE_S, sizeof(short));
	//printf("size_fat = %d\n\n", fat_size(4096));
	//file_import("test_file.txt", "t1.jdisk");

	//unsigned short * i = read_fat("t1.jdisk");
	//printf("i: %d\n", *i);
	char * diskname;
	char * command;
	char * ifname;
	char * ofname;
	int starting_block;
	if (argc==4){//ugly but easy to write shrug
		diskname = argv[1];
		command = argv[2];
		ifname = argv[3];
	}else if(argc==5){
		diskname = argv[1];
		command = argv[2];
		starting_block = atoi(argv[3]);
		ofname = argv[4];
	}
	else {printf("usage: FATRW diskfile import input-file || FATRW diskfile export starting-block output-file\n"); exit(1);}

	if(!strcmp(command,"import")){
		file_import(diskname, ifname);
	}
	else if(!strcmp(command,"export")){
		file_export(diskname, starting_block, ofname);
	}
	else{printf("usage: FATRW diskfile import input-file || FATRW diskfile export starting-block output-file\n"); exit(1);}

	return 1;
}
