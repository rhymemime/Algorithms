#include <stdio.h>
#include <math.h>
#include "fatrw.h"
#include <stdlib.h>
#include <string.h>
#include "jdisk.h"

#define SIZE_S JDISK_SECTOR_SIZE
#define USHSIZE sizeof(unsigned short)

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


unsigned short * init_fat(void *diskp){
	//unsigned short* fat_table;
	void * fat_table;

	int fatSize = (jdisk_size(diskp) / SIZE_S + 1) * sizeof(short) /  SIZE_S + 1;

	//printf("fatSize: %d\n\n", fatSize);

	fat_table = malloc(fatSize * (SIZE_S)+1);

	memset(fat_table, 0, USHSIZE * fatSize * SIZE_S);

	jdisk_read(diskp, 0, fat_table);

	return fat_table;
}

int get_fat_size(void *diskp){
	int fatSize = (jdisk_size(diskp) / SIZE_S + 1) * sizeof(short) /  SIZE_S + 1;
	return fatSize;
}

void file_export(char * diskfile, unsigned int sector_start, char * ofname){
	//if(*(fat_table + x) == x) // then last sector in file

	//printf("HERE I AM\n");
	void * diskp = jdisk_attach(diskfile);
	unsigned long j = jdisk_size(diskp);
	unsigned short * fat_table = init_fat(diskp);

	FILE * pFile;

	int numFatSec = get_fat_size(diskp);

	pFile = fopen(ofname, "a+");
	if (pFile == NULL){
		//printf("incorrect output file\n");
		exit(1);
	}

	//allocate space for pointer array
	unsigned short ** SP = malloc(USHSIZE * numFatSec);

	int i;
	//initialize pointer array
	for(i=0; i<numFatSec; i++){
		*(SP + i)=NULL;
	}
	*SP = fat_table;

	unsigned char * buf = malloc(SIZE_S+1);
	//unsigned short * ubuf = buf;

	//printf("sector start: %d\n", sector_start);
	int whichSec = sector_start / (SIZE_S/USHSIZE);

	int offset = sector_start - (SIZE_S/USHSIZE * whichSec);//deal with num sector
	int current;
	//printf("fatSec: %d\n", numFatSec);
	//int next = *(*(SP + whichSec) + offset);
	int next = sector_start - numFatSec+1;
	//printf("first next: %d\n", next);
	//printf("entering do\n");
	do{
		//printf("in do\n");
		current = next;

		whichSec = current / (SIZE_S/USHSIZE);

		//printf("currentttt: %d\nnext: %d\nwhichSec: %d", current, next,whichSec);
		offset = current - (SIZE_S/USHSIZE * whichSec);
		//printf("test99\n");
		if(*(SP + whichSec)==NULL){
			//printf("wutest\n");
			*(SP + whichSec) = malloc((SIZE_S+1));
			//printf("SecrorP: %d\n", *(SP + whichSec));
			jdisk_read(diskp, whichSec, *(SP + whichSec));
		}
		//current = *(*(SP + whichSec) + offset);
		//printf("test98\n");
		next = *(*(SP + whichSec) + offset);//proper logic
		//next = *(*SP + current); //test logic
		//printf("test98\n");
		//printf("curren: %d\nnext: %d\nwhichSec: %d\n", current, next,whichSec);
		if(next == 0 || next != current){
			//printf("in if\n");
			jdisk_read(diskp, current + numFatSec-1, buf);
			//printf("us SIZE_S: %d\n", (unsigned short)SIZE_S);
			fwrite(buf, SIZE_S, 1, pFile);
		}
		else{
			jdisk_read(diskp, current +numFatSec-1, buf);
			//unsigned short size = *(ubuf + 512);
			unsigned short size;
			if(*(buf+1023) == 0){
				printf("in in\n");
				//size = (unsigned short)*(buf + 1021);
			}
			else{
				//printf("in else\n");
				//size = (unsigned short) *(buf + 1022) + SIZE_S/USHSIZE + SIZE_S/USHSIZE/USHSIZE;
				size = (unsigned short) *(buf + 1022) + SIZE_S/USHSIZE;
			}
			//printf("buf 1020: %d, buf 1021: %d, buf 1022: %d, buf 1023: %d, buf 1024: %d\n",*(buf + 1020),*(buf + 1021),*(buf + 1022),(unsigned char)*(buf + 1023), *(buf + 1024));

			//printf("size: %d\n", size);
			fwrite(buf, size, 1, pFile);
		}
	}while(current != next && next != 0);//might have to do loop one more time?
	//fclose(pFile);
	/*jdisk_read(diskp, current, buf);
	//unsigned short size = *(ubuf + 512);
	unsigned short size;
	if(*(buf+1023) == 0){
		printf("in in\n");
		size = (unsigned)*(buf + 1022);
	}
	else{
		printf("in else\n");
		size = (unsigned) *(buf + 1023);
	}
	printf("buf 1020: %d, buf 1021: %d, buf 1022: %d, buf 1023: %d, buf 1024: %d\n",*(buf + 1020),*(buf + 1021),*(buf + 1022),(unsigned short)*(buf + 1023), *(buf + 1024));

	printf("size: %d\n", size);
	fwrite(buf, size, 1, pFile);*/
printf("Reads: %d\nWrites: %d\n", jdisk_reads(diskp), jdisk_writes(diskp));
}

void file_import(char * diskfile, char * ifname){
	void * diskp = jdisk_attach(diskfile);
	unsigned long j = jdisk_size(diskp);
	unsigned short * fat_table = init_fat(diskp);
	//printf("fat_table: %d\n", *fat_table);
	//printf("fat_table: %d\n", *(fat_table+1));

	int numFatSec = get_fat_size(diskp);

	unsigned short ** SP = malloc(USHSIZE * numFatSec);
	int i;
	for(i=0; i<numFatSec; i++){
		SP[i]=NULL;
	}
	SP[0] = fat_table;

	FILE * pFile;
	pFile = fopen(ifname, "r");
	fseek(pFile, 0L, SEEK_END);
	int fsize = ftell(pFile);

	int numSec = fsize / SIZE_S;
	//printf("mod is %d\n", fsize%SIZE_S);
	if(fsize%SIZE_S!=0) numSec++;
	int current = *fat_table;
	for(i=0; i<numSec; i++){
		unsigned int whichSec = current/(SIZE_S/USHSIZE);
		unsigned int offset = current - (SIZE_S/USHSIZE * whichSec);
		if(*(SP + whichSec)==NULL){
			*(SP + whichSec) = malloc((SIZE_S+1));
			//printf("SecrorP: %d\n", *(SP + whichSec));
			jdisk_read(diskp, whichSec, *(SP + whichSec));
		}
		int next = *(*(SP + whichSec) + offset);
		if(current==next || current == 0){
			printf("Not enough free sectors (%d) for %s, which needs %d.\n", i,ifname, numSec);
			exit(1);
		}
		current = next;
	}
	void * buf = malloc(SIZE_S);

	rewind(pFile);

	fread(buf,1,SIZE_S,pFile);

	/*if(*(fat_table + x) == NULL){//not sure how to memset to null or what to check against
	  jdisk_read(diskp, x/SIZE_S, fat_table + x/SIZE_S);
	  }*/

	//printf("TEST!\n");

	int file_start = *fat_table;
	//printf("checking indexing: %d, %d\n", *fat_table, *(fat_table+511));
	unsigned short move1, move2;
	unsigned int whichSec;
	int offset;

	while(fsize>SIZE_S){
		whichSec = *fat_table/(SIZE_S/USHSIZE);
		//offset = sector_start - (SIZE_S/USHSIZE * whichSec);
		//printf("whichsector: %d\n", whichSec);
		//printf("SP[0]: %d\n", *SP[0]);
		if(*(SP + whichSec)==NULL){
			*(SP + whichSec) = malloc((SIZE_S+1));
			//printf("SecrorP: %d\n", *(SP + whichSec));
			jdisk_read(diskp, whichSec, *(SP + whichSec));
		}

		//jdisk_write(diskp, (**(SP + whichSec) + (*fat_table) - (whichSec * SIZE_S/USHSIZE)) + numFatSec - 1, buf);
		jdisk_write(diskp, *fat_table + numFatSec - 1, buf);
		fread(buf,1,SIZE_S,pFile);
		fsize -= SIZE_S;
		*fat_table = *(*(SP + whichSec) + *fat_table - whichSec * SIZE_S/USHSIZE);
	}
	//printf("out of while\n");

	whichSec = *fat_table/(SIZE_S/USHSIZE);
	//printf("whichSec, again: %d\n",whichSec);
	if(*(SP + whichSec)==NULL){
		*(SP + whichSec) = malloc((SIZE_S+1));
		//printf("SecrorP: %d\n", *(SP + whichSec));
		jdisk_read(diskp, whichSec, *(SP + whichSec));
	}
	//offset = sector_start - (SIZE_S/USHSIZE * whichSec);
	move1 = *fat_table;
	move2 = *(*(SP + whichSec) + move1 - (SIZE_S/USHSIZE) * whichSec);
	int change0 = 0;
	if(fsize == SIZE_S){//ISNT WORKING
		//printf("whichSec!!!!: %d\n", whichSec);
		//change current to 0
		//printf("where change to 0: %d\n", fat_table - (*(SP+whichSec) + *fat_table - SIZE_S/USHSIZE * whichSec));
		//printf("fat_table: %d\n", *fat_table);
		*(*(SP+whichSec) + *fat_table - SIZE_S/USHSIZE * whichSec) = (unsigned short) 0;
		change0 = 1;
		//printf("defo should be 0: %d\n", *(*(SP+whichSec) + *fat_table - SIZE_S/USHSIZE * whichSec));
	}
	else{
		//printf("fsize: %d\n", fsize);
		unsigned short * ubuf = buf;
		if(fsize >= 512){
			//printf("fsize - : %d", (fsize - SIZE_S/USHSIZE));
			char * cbuf = buf;
			*(cbuf + SIZE_S-1) =(unsigned short)fsize - SIZE_S/USHSIZE - SIZE_S/USHSIZE/USHSIZE;
			//*(ubuf + SIZE_S/USHSIZE) = (unsigned short)(fsize - SIZE_S/USHSIZE)/USHSIZE;
			//*(ubuf + SIZE_S/2-2) = fsize - 255;
		}
		else{
			//printf("TEST7\n");
			//*(ubuf + (SIZE_S/USHSIZE * *(*(SP + whichSec)+numFatSec)-1)) = (unsigned short)fsize;
			*(ubuf + (SIZE_S / USHSIZE) - 1) = (unsigned short)fsize;
			//*(ubuf + (SIZE_S / USHSIZE)) = (unsigned short)7;
			/*			printf("*ubuf + (SIZE_S / USHSIZE): %d\n", *(ubuf + (SIZE_S / USHSIZE) - 1));
						printf("buf: %d\n", buf);
						printf("ubuf: %d\n", ubuf+ (SIZE_S / USHSIZE));*/
		}
	}
	if(*(SP + whichSec)==NULL){//no idea what this does rn
		//printf("TEST5!\n");
		*(SP + whichSec) = malloc((SIZE_S)+1);
		jdisk_read(diskp, whichSec, *(SP + whichSec));
		//printf("inSectorP %d\n", **(SP + whichSec));
	}
	//printf("should be 0: %d\n", *(*(SP + whichSec) + (*fat_table) - (whichSec * SIZE_S/USHSIZE)) + numFatSec - 1);
	//printf("fat_table: %d\n", *fat_table);
	//jdisk_write(diskp, *(*(SP + whichSec) + (*fat_table) - (whichSec * (SIZE_S/USHSIZE))) + numFatSec - 1, buf);
	jdisk_write(diskp, *fat_table + numFatSec - 1, buf);//movinng vars before writing?
	//printf("TEST3!\n");
	*fat_table = move2;
	//printf("out fattable: %d\nmove1: %d\n", *fat_table, move1);
	//printf("arithmetic: %d!\nwhichSec: %d\n", (SIZE_S/USHSIZE) * whichSec, whichSec);
	//*(fat_table + move1) = move1;
	if(change0==0)
		*(*SP + whichSec + move1 - SIZE_S *whichSec /USHSIZE) = move1;
	for(i=0; i<numFatSec; i++){
		if(SP[i]!=NULL){
			jdisk_write(diskp, i, *(SP + i));
		}
	}
	//jdisk_write(diskp, 0, fat_table);
	//}//account for when not enough sectors


printf("New file starts at sector %d\nReads: %d\nWrites: %d\n",file_start, jdisk_reads(diskp), jdisk_writes(diskp));
}

int main(int argc, char** argv){//will eventually rename import
	//printf("size s = %d, sizeof short = %d\n\n", SIZE_S, sizeof(short));
	//printf("size_fat = %d\n\n", fat_size(4096));
	//file_import("test_file.txt", "t1.jdisk");

	//unsigned short * i = read_fat("t1.jdisk");
	//printf("i: %d\n", *i);
	//printf("SIZE_S %d\n", SIZE_S);
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
		if(argc!=5){printf("usage: FATRW diskfile import input-file || FATRW diskfile export starting-block output-file\n"); exit(1);}
		file_export(diskname, starting_block, ofname);
	}
	else{printf("usage: FATRW diskfile import input-file || FATRW diskfile export starting-block output-file\n"); exit(1);}

	return 1;
}
