#include <stdio.h>
#include <math.h>
#include "fatrw.h"
#include <stdlib.h>
#include <string.h>
#include "jdisk.h"

#define SIZE_S JDISK_SECTOR_SIZE
#define USHSIZE sizeof(unsigned short)

//put a lot of work into this, and I was too far in when i realized how awful my code was written
//hopefully B-Tree goes better cause this is some garbage

unsigned long fat_size(char *diskfile){//might need to change from ints
	FILE * pFile;
	pFile = fopen(diskfile, "r");
	fseek(pFile, 0L, SEEK_END);
	unsigned long disksize = ftell(pFile);
	unsigned long size_fat = (disksize / SIZE_S + 1) * sizeof(short) / SIZE_S + 1;
	if((disksize / SIZE_S + 1 - size_fat) * sizeof(short) / SIZE_S < size_fat){
		size_fat = (disksize / SIZE_S + 1 - size_fat) * sizeof(short) / SIZE_S + 1;
	}
	return size_fat;//need to confirm edge cases!
}



unsigned short * init_fat(void *diskp){
	void * fat_table;

	unsigned long fatSize = (jdisk_size(diskp) / SIZE_S + 1) * sizeof(unsigned short) /  SIZE_S + 1;

	fat_table = malloc(fatSize * (SIZE_S)+1);

	memset(fat_table, 0, USHSIZE * fatSize);

	jdisk_read(diskp, 0, fat_table);

	return fat_table;
}

unsigned long get_fat_size(void *diskp){
	unsigned long fatSize = (jdisk_size(diskp) / SIZE_S + 1) * sizeof(short) /  SIZE_S + 1;
	return fatSize;
}

void file_export(char * diskfile, unsigned long sector_start, char * ofname){

	void * diskp = jdisk_attach(diskfile);
	unsigned long j = jdisk_size(diskp);
	unsigned short * fat_table = init_fat(diskp);

	FILE * pFile;

	unsigned long numFatSec = get_fat_size(diskp);

	pFile = fopen(ofname, "a+");
	if (pFile == NULL){
		exit(1);
	}

	//allocate space for pointer array
	unsigned short ** SP = malloc(USHSIZE * numFatSec);

	unsigned long i;
	//initialize pointer array
	for(i=0; i<numFatSec; i++){
		*(SP + i)=NULL;
	}
	*SP = fat_table;

	//probably should be one smaller
	unsigned char * buf = malloc(SIZE_S+1);

	unsigned long whichSec = sector_start / (SIZE_S/USHSIZE);

	unsigned long offset = sector_start - (SIZE_S/USHSIZE * whichSec);
	unsigned long current;
	unsigned long next = sector_start;
	do{
		current = next;

		//calc which Sector the current FAT entry is in
		whichSec = current / (SIZE_S/USHSIZE);

		//calc offset for indexing differently on different SP
		//might be a source of errors
		offset = current - (SIZE_S/USHSIZE * whichSec);

		//if sector isnt in memory yet
		if(*(SP + whichSec)==NULL){
			*(SP + whichSec) = malloc((SIZE_S+1));
			jdisk_read(diskp, whichSec, *(SP + whichSec));
		}

		//find where going to next in FAT table
		next = *(*(SP + whichSec) + offset);

		//when we're writing the full size
		if(next == 0 || next != current){
			jdisk_read(diskp, current , buf);
			fwrite(buf, SIZE_S, 1, pFile);
		}
		else{
			jdisk_read(diskp, current, buf);
			unsigned short size;
			//edge case of one byte left
			if(*(buf+1023) != 255){
				size = *(buf + 1022) + *(buf + 1023) * 256;
			}
			else{
				size = SIZE_S-1;
			}

			fwrite(buf, size, 1, pFile);
		}
	}while(current != next && next != 0);

	printf("Reads: %lu\nWrites: %lu\n", jdisk_reads(diskp), jdisk_writes(diskp));
}

void file_import(char * diskfile, char * ifname){
	void * diskp = jdisk_attach(diskfile);
	unsigned long j = jdisk_size(diskp);
	unsigned short * fat_table = init_fat(diskp);

	unsigned long numFatSec = get_fat_size(diskp);

	unsigned short ** SP = malloc(USHSIZE * numFatSec);
	unsigned long i;

	//Sector Pointers
	for(i=0; i<numFatSec; i++){
		*(SP + i)=NULL;
	}
	*SP = fat_table;

	//open file and get size
	FILE * pFile;
	pFile = fopen(ifname, "r");
	fseek(pFile, 0L, SEEK_END);
	unsigned long fsize = ftell(pFile);

	//get number of sectors for file
	unsigned long numSec = fsize / SIZE_S;
	//handle 1024 edge case
	if(fsize%SIZE_S!=0) numSec++;

	//check for enough free sectors
	unsigned long current = *fat_table;
	for(i=0; i<numSec; i++){
		unsigned long whichSec = current/(SIZE_S/USHSIZE);
		unsigned long offset = current - (SIZE_S/USHSIZE * whichSec);
		if(*(SP + whichSec)==NULL){
			*(SP + whichSec) = malloc((SIZE_S+1));
			jdisk_read(diskp, whichSec, *(SP + whichSec));
		}
		unsigned long next = *(*(SP + whichSec) + offset);
		if(current==next || current == 0){
			printf("Not enough free sectors (%lu) for %s, which needs %d.\n", i,ifname, numSec);
			exit(1);
		}
		current = next;
	}
	void * buf = malloc(SIZE_S);

	rewind(pFile);

	fread(buf,1,SIZE_S,pFile);

	unsigned long file_start = *fat_table;
	unsigned short move1, move2;
	unsigned long whichSec;
	unsigned long offset;

	while(fsize>SIZE_S){
		//base case of writing 1024 at a time
		whichSec = *fat_table/(SIZE_S/USHSIZE);
		//check if we need to update Sector Pointer
		if(*(SP + whichSec)==NULL){
			*(SP + whichSec) = malloc((SIZE_S+1));
			jdisk_read(diskp, whichSec, *(SP + whichSec));
		}

		jdisk_write(diskp, *fat_table + numFatSec - 1, buf);
		fread(buf,1,SIZE_S,pFile);
		fsize -= SIZE_S;
		*fat_table = *(*(SP + whichSec) + *fat_table - whichSec * SIZE_S/USHSIZE);
	}

	whichSec = *fat_table/(SIZE_S/USHSIZE);
	if(*(SP + whichSec)==NULL){
		*(SP + whichSec) = malloc((SIZE_S+1));
		jdisk_read(diskp, whichSec, *(SP + whichSec));
	}
	//hold value @ 0 entry in FAT table
	move1 = *fat_table;
	move2 = *(*(SP + whichSec) + move1 - (SIZE_S/USHSIZE) * whichSec);

	//messy nonsense for dealing with an edge case
	unsigned long change0 = 0;
	if(fsize == SIZE_S){
		*(*(SP+whichSec) + *fat_table - SIZE_S/USHSIZE * whichSec) = 0;
		change0 = 1;
	}
	else{
		unsigned short * ubuf = buf;
		if(fsize >= 512){
			char * cbuf = buf;
			*(cbuf + SIZE_S-1) =(unsigned short)fsize - SIZE_S/USHSIZE - SIZE_S/USHSIZE/USHSIZE;
		}
		else{
			*(ubuf + (SIZE_S / USHSIZE) - 1) = (unsigned short)fsize;
		}
	}
	if(*(SP + whichSec)==NULL){
		*(SP + whichSec) = malloc((SIZE_S)+1);
		jdisk_read(diskp, whichSec, *(SP + whichSec));
	}
	jdisk_write(diskp, *fat_table + numFatSec - 1, buf);
	*fat_table = move2;
	if(change0==0)
		*(*SP + whichSec + move1 - SIZE_S *whichSec /USHSIZE) = move1;
	for(i=0; i<numFatSec; i++){
		if(SP[i]!=NULL){
			jdisk_write(diskp, i, *(SP + i));
		}
	}


printf("New file starts at sector %lu\nReads: %lu\nWrites: %lu\n",file_start, jdisk_reads(diskp), jdisk_writes(diskp));
}

int main(int argc, char** argv){
	char * diskname;
	char * command;
	char * ifname;
	char * ofname;
	unsigned long starting_block;
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
		if(argc!=5){printf("usage: FATRW diskfile import input-file || FATRW diskfile export starting-block output-file\n"); exit(1);}
		file_export(diskname, starting_block, ofname);
	}
	else{printf("usage: FATRW diskfile import input-file || FATRW diskfile export starting-block output-file\n"); exit(1);}

	return 1;
}
