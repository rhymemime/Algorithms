#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "b_tree.h"

#define JSIZE JDISK_SECTOR_SIZE
#define ENTDEBUG 0
#define RDEBUG 0
#define ADEBUG 0
#define FDEBUG 0
#define FSDEBUG 0
#define PDEBUG 0
#define IDEBUG 0
#define ISDEBUG 0
#define ILDEBUG 0
#define SPDEBUG 0
#define SNDEBUG 0
#define FFDEBUG 0
#define CDEBUG 0

/*#define ENTDEBUG 1
#define RDEBUG 1
#define ADEBUG 1
#define FDEBUG 1
#define FSDEBUG 1
#define PDEBUG 1
#define IDEBUG 1
#define ISDEBUG 1
#define ILDEBUG 1
#define SPDEBUG 1
#define SNDEBUG 1
#define FFDEBUG 1
#define CDEBUG 1*/

void *b_tree_create(char *filename, long size, int key_size){
	if(ENTDEBUG)printf("\nIN CREATE\n\n");
	void * disk;
	unsigned long ffb = 2;
	unsigned int rlba = 1;
	int i;

	disk = jdisk_create(filename, size);
	B_Tree* bt = malloc(sizeof(B_Tree));
	Tree_Node* tn = malloc(sizeof(Tree_Node));

	bt->key_size = key_size;
	bt->root_lba = rlba;
	bt->first_free_block = ffb;
	bt->disk = disk;
	bt->size = jdisk_size(bt->disk);
	bt->num_lbas = bt->size / JSIZE;
	bt->keys_per_block = (JSIZE - 6) / (bt->key_size + 4);
	bt->lbas_per_block = bt->keys_per_block + 1;
	jdisk_write(disk, 0, (void*) bt);

	tn->internal=0;
	tn->nkeys=0;
	tn->lbas = malloc(sizeof(int *) * (bt->lbas_per_block+1));
	for(i=0; i<bt->lbas_per_block+1; i++){
		tn->lbas[i]=0;
	}
	tn->bytes[0] = tn->internal;
	tn->bytes[1] = tn->nkeys;
	memcpy(tn->bytes + JSIZE - bt->lbas_per_block*sizeof(int), tn->lbas, bt->lbas_per_block * sizeof(int));
	jdisk_write(disk, bt->root_lba, tn->bytes);

	if(CDEBUG){
		printf("keysize: %d\n", bt->key_size);
		printf("root_lba: %u\n", bt->root_lba);
		printf("first_free_block: %lu\n", bt->first_free_block);
		printf("keys per block: %d\n", bt->keys_per_block);
		printf("lbas per block: %d\n", bt->lbas_per_block);
		printf("num lbas: %d\n", bt->num_lbas);
	}
	return bt;

}
void *b_tree_attach(char *filename){
	if(ENTDEBUG)printf("\nIN ATTACH\n\n");

	if(ADEBUG){
		printf("size B_Tree: %d\n", sizeof(B_Tree));
	}

	B_Tree *bt;
	FILE *f;
	size_t result;
	void * jd;
	int rSize;
	char buf[JSIZE];

	jd = jdisk_attach(filename);
	bt = (B_Tree *) malloc(sizeof(B_Tree));

	/*if ((f = fopen(filename,"r")) == NULL){
	  printf("Error! opening file: %s", filename);
	  exit(1);
	  }*/

	rSize = sizeof(int) + sizeof(unsigned int) + sizeof(unsigned long);
	/*result = fread((void *)bt, 1, rSize, f);
	  if (result != rSize) {fputs ("Reading error",stderr); exit (3);}*/
	jdisk_read(jd, 0, (void *)buf);

	/*seems like an ironclad way of doing initial assignments,
	  there might be a cleaner way though*/
	bt->key_size = *(int *)(buf);
	bt->root_lba = *(unsigned int *)(buf + sizeof(int));
	bt->first_free_block = *(unsigned long *)(buf + sizeof(int) + sizeof(unsigned int));
	bt->disk = jd;
	bt->size = jdisk_size(jd);
	bt->num_lbas = bt->size / JSIZE;
	bt->keys_per_block = (JSIZE - 6) / (bt->key_size + 4);
	bt->lbas_per_block = bt->keys_per_block + 1;

	if(RDEBUG){
		printf("keysize: %d\n", bt->key_size);
		printf("root_lba: %u\n", bt->root_lba);
		printf("first_free_block: %lu\n", bt->first_free_block);
		printf("keys per block: %d\n", bt->keys_per_block);
		printf("lbas per block: %d\n", bt->lbas_per_block);
		printf("num lbas: %d\n", bt->num_lbas);
	}

	return (void *) bt;
}

void find_flush(void* b, void* t){
	//start at external, continue upwards and go back in to any changed
	B_Tree * bt = (B_Tree*) b;
	Tree_Node * tn = (Tree_Node*) t;
	Tree_Node * parent;

	while(tn!=NULL){
		if(tn->flush==1){
			if(FFDEBUG)printf("writing to lba: %d\n", tn->lba);
			jdisk_write(bt->disk, tn->lba, tn->bytes);
		}
		parent = tn->parent;
		free(tn->keys);
		free(tn->lbas);
		free(tn);
		tn = parent;
	}
	if(bt->flush==1){
		if(FFDEBUG)printf("writing to sector 0\n");
		jdisk_write(bt->disk, 0, bt);
	}
}

unsigned int b_tree_insert(void *b_tree, void *key, void *record){
	if(ENTDEBUG)printf("\nIN INSERT\n\n");
	//this is scary...
	unsigned int findLBA;
	unsigned int retBlock;
	unsigned int numKeysMove,moveSize;
	int i, ind;
	B_Tree * bt = (B_Tree*) b_tree;
	Tree_Node * tn = (Tree_Node*)malloc(sizeof(Tree_Node));

		if(ISDEBUG){
			for(i=0; i<bt->key_size; i++){
				char c = *((char*)key+i);
				printf("%c", c);
			}
			printf("\n");
			//printf("key: %s\n", (char*)key);
			//printf("char at strlen+x: %c\n", *((char*)key+sl+3));
		}
	findLBA = b_tree_find(bt, key);
	if(findLBA==0){//new key
		/* tn populated during find */
		tn = bt->tmp_e;
		if(IDEBUG){
			printf("IDEBUG!\n");
			printf("index: %d\n", bt->tmp_e_index);
			printf("internal: %d\n", tn->internal);
			printf("nkeys: %d\n", tn->nkeys);
			for(i=0; i<tn->nkeys+1; i++){//outting zero if not populated, might break something?
				printf("lba%d: %d\n",i, *(tn->lbas + i));
			}
			if(tn->parent==NULL)printf("Parent is NULL!\n");
			if(tn->parent!=NULL)printf("Parent lba is: %d\n",tn->parent->lba);
		}
		//make room for inserted key
		ind = bt->tmp_e_index;
		numKeysMove = tn->nkeys - bt->tmp_e_index;
		moveSize = numKeysMove * bt->key_size;
		if(IDEBUG)printf("numKeys Move: %d\n", numKeysMove);
		if(IDEBUG)printf("moveSize: %d\n", moveSize);

		//need to test!
		if(ind < bt->keys_per_block){
			memmove(*(tn->keys+ind+1), *(tn->keys+ind), moveSize);//how to test?
			if(IDEBUG)printf("read at index + 1: %s\n", *(tn->keys+ind+1));
			if(IDEBUG)printf("read at index + numKeysMove: %s\n", *(tn->keys+ind+numKeysMove));
		}
		//put key in right place
		memcpy((void*)*(tn->keys+ind), key, bt->key_size);//how to test?
		if(IDEBUG)printf("read at index: %s\n", *(tn->keys+ind));
		if(ISDEBUG){
			unsigned char sl = strlen(*(tn->keys+ind));
			printf("sl: %d\n", sl);
			for(i=0; i<bt->key_size; i++){
				char * ck = key;
				printf("%c", ck[i]);
			}
			printf("\n");
			//printf("key: %s\n", (char*)key);
			//printf("char at strlen+x: %c\n", *((char*)key+sl+3));
		}
		//make room for new LBA
		memmove(tn->lbas+ind+1, tn->lbas+ind, (numKeysMove+1)*sizeof(int));
		//put LBAs in right place
		//get first free block
		*(tn->lbas+ind) = bt->first_free_block;
		if(ILDEBUG){
			for(i=0; i<tn->nkeys+2; i++){
				printf("lba%d: %d\n",i, *(tn->lbas + i));
			}
		}
		if(bt->first_free_block > bt->num_lbas-1){
			printf("Too biig!!!!!!\n");
			//exit(1);
			return 0;
		}
		//memcpy(tn->bytes + JSIZE - bt->lbas_per_block*sizeof(int), tn->lbas, bt->lbas_per_block * sizeof(int));//testing moving after split
		jdisk_write(bt->disk, bt->first_free_block, record);//should work, leaving commented until splitting is written
		retBlock = bt->first_free_block;
		bt->first_free_block++;
		bt->flush=1;

		//increment nkeys
		tn->flush = 1;
		tn->nkeys++;
		tn->bytes[1] = tn->nkeys;
		//if(IDEBUG)printf("before split: tn parent nkeys: %d\n",tn->parent->nkeys);
		//check if split
		if(tn->nkeys > bt->keys_per_block){
			b_tree_split((void *) bt, (void*) tn);
		}
		memcpy(tn->bytes + JSIZE - bt->lbas_per_block*sizeof(int), tn->lbas, bt->lbas_per_block * sizeof(int));

		find_flush((void*)bt, (void*)tn);
		return retBlock;
	}
	if(findLBA!=0){//key already present, test
		jdisk_write(bt->disk, findLBA, record);
		return findLBA;
	}
	return -1;
}

void * new_split_node(void *b, void *t){
	if(ENTDEBUG)printf("\nMaking New Node\n\n");
	int i, middle;
	B_Tree * bt = (B_Tree*) b;
	Tree_Node * tn = (Tree_Node*) t;
	Tree_Node * rtn = (Tree_Node*)malloc(sizeof(Tree_Node));
	Tree_Node * parent = tn->parent;

	rtn->lbas = malloc(sizeof(int *) * (bt->lbas_per_block+1));
	rtn->keys = malloc(sizeof(char *) * (bt->keys_per_block+1));

	/* initialize placement of keys, still need to test */
	for(i=0; i<bt->keys_per_block+1; i++){
		*(rtn->keys+i) = rtn->bytes + (bt->key_size * i) + 2;
	}

	middle = bt->keys_per_block/2;

	rtn->nkeys = bt->keys_per_block/2 + (bt->keys_per_block)%2;//plus 1 or not?
	if(ISDEBUG)printf("rtn nkeys: %d\n", rtn->nkeys);
	rtn->internal = tn->internal;
	rtn->lba = bt->first_free_block;
		if(bt->first_free_block > bt->num_lbas-1){
			printf("Too biig!!!!!!\n");
			exit(1);
		}
	bt->first_free_block++;
	bt->flush=1;
	rtn->parent = parent;
	rtn->parent_index = tn->parent_index+1;//was -1, not sure why, defo isn't, do we set back split?

	/* Setup right hand side bytes */
	rtn->bytes[0] = rtn->internal;
	rtn->bytes[1] = rtn->nkeys;
	memcpy(rtn->bytes+2, *(tn->keys+middle+1), rtn->nkeys * bt->key_size);

	memcpy(rtn->lbas, tn->lbas+middle+1, sizeof(int) * (rtn->nkeys+1));//should be +1???? might cause seg faults???// messed with parents for mult
	if(PDEBUG){
		 printf("looking here!!!!!!!\n\n");
		 for(i=0; i<tn->nkeys+1; i++){
			 printf("tn: lba%d: %d\n",i, (int)*(tn->lbas + i));
		 }
		for(i=0; i<rtn->nkeys+1; i++){
			printf("rtn: lba%d: %d\n",i, *(rtn->lbas + i));//for some reason, not getting last value in tn lba
		}
	}
	memcpy(rtn->bytes + JSIZE - bt->lbas_per_block*sizeof(int), rtn->lbas, bt->lbas_per_block * sizeof(int));
	if(ISDEBUG)printf("copy to end keys %d, and begining lba %d\n", 2 + rtn->nkeys * bt->key_size, JSIZE - bt->lbas_per_block*sizeof(int));
	if(FFDEBUG)printf("writing to lba: %d\n", rtn->lba);
	jdisk_write(bt->disk, rtn->lba, rtn->bytes);

	//left hand side
	//tn->nkeys = tn->nkeys - rtn->nkeys - 1;
	tn->nkeys =  bt->keys_per_block/2;
	tn->bytes[1] = tn->nkeys;
	tn->flush=1;
	//theoretically shouldnt have to memset if nkeys is right
	//memset(tn->bytes+2+middle*bt->key_size...)
	/*shorten left hand side */
	if(PDEBUG){
		printf("Printing rtn: \n");
		printf("internal: %d\n", rtn->internal);
		printf("nkeys: %d\n", rtn->nkeys);
		for(i=0; i<rtn->nkeys+1; i++){
			printf("lba%d: %d\n",i, *(rtn->lbas + i));
		}
		for(i=0; i<rtn->nkeys; i++){
			printf("keys%d, %s\n",i, *(rtn->keys+i));
		}
		printf("rtn lba: %d\n", rtn->lba);
		printf("\nPrinting tn: \n");
		printf("internal: %d\n", tn->internal);
		printf("nkeys: %d\n", tn->nkeys);
		for(i=0; i<tn->nkeys+1; i++){
			printf("lba%d: %d\n",i, *(tn->lbas + i));
		}
		for(i=0; i<tn->nkeys; i++){
			printf("keys%d, %s\n",i, *(tn->keys+i));
		}
	}
	return rtn;
}

void * new_root_node(void *b, void *t){
	if(ENTDEBUG)printf("\nMaking New Root Node\n\n");
	int i, middle;
	B_Tree * bt = (B_Tree*) b;
	Tree_Node * tn = (Tree_Node*) t;
	Tree_Node * roottn = (Tree_Node*)malloc(sizeof(Tree_Node));

	tn->parent = roottn;
	tn->parent_index=0;

	roottn->parent = NULL;
	roottn->nkeys=0;
	roottn->flush=1;
	roottn->internal=1;
	roottn->lba = bt->first_free_block;
		if(bt->first_free_block > bt->num_lbas-1){
			printf("Too biig!!!!!!\n");
			exit(1);
		}
	bt->first_free_block++;
	bt->flush=1;
	bt->root_lba=roottn->lba;
	bt->flush=1;
	roottn->bytes[0] = roottn->internal;
	roottn->bytes[1] = roottn->nkeys;
	roottn->flush = 1;

	roottn->lbas = malloc(sizeof(int *) * (bt->lbas_per_block+1));
	*(roottn-> lbas) = tn->lba;
	memcpy(roottn->bytes + JSIZE - bt->lbas_per_block*sizeof(int), roottn->lbas, bt->lbas_per_block * sizeof(int));//might be trouble here...



	roottn->keys = malloc(sizeof(char *) * (bt->keys_per_block+1));

	/* initialize placement of keys, still need to test */
	for(i=0; i<bt->keys_per_block+1; i++){
		*(roottn->keys+i) = roottn->bytes + (bt->key_size * i) + 2;
	}
}

b_tree_split(void *b_tree, void *t){//remember to set flushes
	if(ENTDEBUG)printf("\nSPLITTING\n\n");
	B_Tree * bt = (B_Tree*) b_tree;
	unsigned char pnkeys;
	int numKeysMove;
	unsigned char ** pkeys;
	int middle, i;
	Tree_Node * tn;// = (Tree_Node*)malloc(sizeof(Tree_Node));
	tn = (Tree_Node *) t;

	Tree_Node * rtn;// = (Tree_Node*)malloc(sizeof(Tree_Node));
	//make new node for right keys

	//deal with if root node
	if(tn->parent==NULL){
		new_root_node((void*) bt, (void*) tn);
	}
	/* Init Middle Key, untested for odd maxkeys */
	middle = bt->keys_per_block/2;
	if(SPDEBUG)printf("middle: %d\n", middle);

	if(SPDEBUG)printf("TEST!\n");
	if(SPDEBUG)printf("tn lba: %d\n\n", tn->lba);
	if(SPDEBUG)printf("tn parent lba: %d\n\n", tn->parent->lba);
	pnkeys = tn->parent->nkeys;
	if(SPDEBUG)printf("TEST!\n");
	pkeys = tn->parent->keys;

	if(SPDEBUG)printf("TEST!\n");
	/*Print Statements for Debugging*/
	if(SPDEBUG)printf("parent lba: %d\n", tn->parent->lba);
	if(SPDEBUG)printf("TEST444444!\n");
	if(SPDEBUG)printf("*(tn->keys+middle): %s\n", *(tn->keys+middle));

	/* Moving Middle Key to Parent */
	numKeysMove = pnkeys - tn->parent_index;
	if(SPDEBUG)printf("TEST555555!\n");
	memmove(*(pkeys + tn->parent_index+1), *(pkeys + tn->parent_index), numKeysMove * bt->key_size);//changed from cpy to move
	if(SPDEBUG)printf("TEST666666!\n");
	memcpy(*(pkeys + tn->parent_index), *(tn->keys+middle), bt->key_size);
	if(SPDEBUG)printf("TEST777777!\n");

	/*Print Statements for Debugging*/
	//if(SPDEBUG)printf("read at index + 1: %s\n", *(pkeys+tn->parent_index+1));
	//if(SPDEBUG)printf("read at index + numKeysMove: %s\n", *(pkeys+tn->parent_index+numKeysMove));
	//if(SPDEBUG)printf("read at index: %s\n", *(pkeys+tn->parent_index));

	//deal with lbas
	if(SPDEBUG)printf("TEST888888!\n");
	memmove(tn->parent->lbas + tn->parent_index+1, tn->parent->lbas + tn->parent_index, sizeof(int) * (numKeysMove+1));//changed to memmoove from cpu

	//set pointer before going into function
	//copy right half to new node
	rtn = new_split_node((void*)bt, (void*)tn);
	*(tn->parent->lbas + tn->parent_index+1) = rtn->lba;
	tn->parent->nkeys++;
	tn->parent->bytes[1] = tn->parent->nkeys;
	tn->parent->flush=1;

	if(SPDEBUG){
		printf("checking lbas out of new split node\n");
		for(i=0; i<tn->nkeys+1; i++){
			printf("lba%d: %d\n",i, *(tn->parent->lbas + i));
		}
	}
	//delete middle and right keys in current node
	//if parent too big, split
	if(tn->parent->nkeys > bt->keys_per_block){
		if(SPDEBUG)printf("RECURSIVELY SPLIT\n");
		if(SPDEBUG){
			for(i=0; i<tn->parent->nkeys+1; i++){
				printf("lba%d: %d\n",i, *(tn->lbas + i));
			}
		}
		b_tree_split((void*)bt, (void*) tn->parent);
	}
	memcpy(tn->parent->bytes + JSIZE - bt->lbas_per_block*sizeof(int), tn->parent->lbas, bt->lbas_per_block * sizeof(int));
}


//populate the tree node, still need to deal with free list
void pop_tnode(void * t, void * b, unsigned int lba, void * p){
	if(ENTDEBUG)printf("\nPOPULATING TNODE at lba: %d\n\n", lba);

	int i;

	B_Tree * bt = (B_Tree*)b;
	Tree_Node * tn = (Tree_Node*)t;
	Tree_Node * parent = (Tree_Node*)p;
	jdisk_read(bt->disk, lba, (void*) tn);

	tn->internal = tn->bytes[0];
	tn->nkeys = tn->bytes[1];
	tn->flush = 1;
	tn->lba = lba;
	tn->parent = parent;
	tn->parent_index=-1;
	if(PDEBUG)printf("lba for cmp: %d\n", tn->lba);
	if(p!=NULL){
		for(i=0; i<parent->nkeys+1; i++){
			if(PDEBUG)printf("parent lba at %d: %d\n", i, *(parent->lbas+i));
			if(*(parent->lbas+i) == tn->lba){
				tn->parent_index = i;
			}
		}
		if(tn->parent_index==-1)printf("LBA NOT FOUND IN PARENT, SOMETHINGS WRONG\n");
	}

	//tn->keys = malloc(bt->keys_per_block*bt->key_size+1);//was working, testing with smaller size
	tn->keys = malloc(sizeof(char *) * (bt->keys_per_block+1));

	/* initialize placement of keys, still need to test */
	for(i=0; i<bt->keys_per_block+1; i++){
		*(tn->keys+i) = tn->bytes + (bt->key_size * i) + 2;
	}

	/* room for an extra lba for splitting */
	tn->lbas = malloc(sizeof(int *) * (bt->lbas_per_block+1));
	/*copy all keys in tn->lbas*/
	//if(tn->nkeys!=0){//this might break a bunch of stuff
	memcpy(tn->lbas, tn->bytes + JSIZE - bt->lbas_per_block*sizeof(int), bt->lbas_per_block * sizeof(int));//want to copy into seperate array to deal with bigger size
	//}
	//tn->lbas = tn->bytes + JSIZE - bt->lbas_per_block*sizeof(int);//works, but prefer copying into seperate array to control size


	/*need to expand with populating free list*/

	if(PDEBUG){
		printf("internal: %d\n", tn->internal);
		printf("size malloced for keys: %d\n", sizeof(char *) * (bt->keys_per_block+1));
		printf("size malloced for lbas: %d\n", sizeof(int) * (bt->lbas_per_block+1));
		printf("nkeys: %d\n", tn->nkeys);
		for(i=0; i<tn->nkeys+1; i++){
			printf("lba%d: %d\n",i, *(tn->lbas + i));
		}
		for(i=0; i<tn->nkeys; i++){
			printf("strlen key: %d\n", strlen(*(tn->keys+i)));
			unsigned char sl = strlen(*(tn->keys+i));
			printf("keys%d, %s\n",i, *(tn->keys+i));
			if(*(*(tn->keys+i) + sl) == '\0'){
				printf("TEST\n");
				printf("%c\n", *(*(tn->keys+i) + sl + 2));
			}
		}
		if(p!=NULL)	printf("parent: %d\n", tn->parent->lba);
		if(p!=NULL)	printf("parent index: %d\n", tn->parent_index);
		if(p==NULL)	printf("parent: NULL\n");
	}
	//if(ENTDEBUG)printf("\nLEAVING POPULATE\n\n");
}

unsigned int b_tree_find(void *b_tree, void *key){
	if(ENTDEBUG)printf("\nIN FIND\n\n");
	B_Tree * bt = (B_Tree*)b_tree;
	Tree_Node * tn;
	Tree_Node * parent;
	int i;
	int found, lt;
	unsigned int next_lba, ret_lba;
	tn = (Tree_Node *) malloc(sizeof(Tree_Node));
	if(FDEBUG)printf("looking at root lba: %d\n",bt->root_lba);
	if(FDEBUG)printf("sizeof Tree Node: %d\n",sizeof(Tree_Node));

	//jdisk_read(bt->disk, bt->root_lba, (void*) tn);

	pop_tnode(tn, bt, bt->root_lba, NULL);

	if(FSDEBUG)printf("tn->key str: %s\n", *tn->keys);
	if(FSDEBUG)printf("passed key str: %s\n", (char*)key);

	found=0;
	while(tn->internal){
		lt = 0;
		if(FDEBUG)printf("tn->nkeys: %d\n", tn->nkeys);
		if(FDEBUG)printf("tn->internal: %d\n", tn->internal);
		if(FDEBUG)printf("found: %d\n", found);
		for(i=0; i < tn->nkeys && lt==0 && found ==0; i++){//got rid of found=0
			if(memcmp(key, *(tn->keys+i), bt->key_size) < 0){
				if(FDEBUG)printf("in < 0\n");
				lt=1;
			}
			else if(memcmp(key, *(tn->keys+i), bt->key_size) > 0){
				if(FDEBUG)printf("in > 0\n");
			}
			else if(memcmp(key, *(tn->keys+i), bt->key_size) == 0){
				if(FDEBUG)printf("in == 0\n");
				found=1;//not completely tested
			}
			else{
				if(FDEBUG)printf("shouldn't happen\n");
			}
		}
		//index into keys
		//if(lt||found)i--;//was working!
		if(lt) i--;
		if(found==1){
			i--;
			found++;
		}
		else if(found>1){
			i = tn->nkeys;
		}

		//if(i==-1) i++;
		//if(found) i = tn->nkeys+1;
		if(FSDEBUG)printf("keys + %d: %d\n",i, *(tn->lbas+i));
		next_lba = *(tn->lbas+i);

		//set tn to next LBA and populate
		parent = tn;
		tn = (Tree_Node *) malloc(sizeof(Tree_Node));
		//jdisk_read(bt->disk, next_lba, (void*) tn);
		pop_tnode(tn, bt, next_lba, parent);
	}

	if(FDEBUG)printf("out while: tn->nkeys: %d\n", tn->nkeys);
	for(i=0; i < tn->nkeys; i++){
		if(memcmp(key, *(tn->keys+i), bt->key_size) < 0){
			if(FDEBUG)printf("in < 0\n");
			break;
			//ret_lba = *(tn->lbas+i);
			//return ret_lba;
		}
		else if(memcmp(key, *(tn->keys+i), bt->key_size) > 0){
			if(FDEBUG)printf("in > 0\n");
		}
		else if(memcmp(key, *(tn->keys+i), bt->key_size) == 0){
			if(FDEBUG)printf("in == 0\n");
			ret_lba = *(tn->lbas+i);
			return ret_lba;
		}
		else{
			if(FDEBUG)printf("shouldn't happen\n");
		}
	}
	if(found){
		if(FDEBUG)printf("returning found internal\n");
		ret_lba = *(tn->lbas+i);
		return ret_lba;
	}
	else{
		//save current tree node in bt when find misses, untested
		if(FDEBUG)printf("returning from find, NOT FOUND\n");
		bt->tmp_e = tn;
		bt->tmp_e_index = i;
		return 0;
	}
}
void *b_tree_disk(void *b_tree){
	if(ENTDEBUG)printf("in disk\n");
	B_Tree * bt = (B_Tree*)b_tree;
	return bt->disk;
}

int b_tree_key_size(void *b_tree){
	if(ENTDEBUG)printf("in size\n");
	B_Tree * bt = (B_Tree*)b_tree;
	return bt->key_size;
}

void b_tree_print_tree(void *b_tree){
	if(ENTDEBUG)printf("in print\n");
}
