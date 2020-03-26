#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "b_tree.h"

#define JSIZE JDISK_SECTOR_SIZE

void *b_tree_create(char *filename, long size, int key_size){
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

	return bt;

}
void *b_tree_attach(char *filename){

	B_Tree *bt;
	FILE *f;
	size_t result;
	void * jd;
	int rSize;
	char buf[JSIZE];

	jd = jdisk_attach(filename);
	bt = (B_Tree *) malloc(sizeof(B_Tree));

	rSize = sizeof(int) + sizeof(unsigned int) + sizeof(unsigned long);
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

	return (void *) bt;
}

void find_flush(void* b, void* t){
	//start at external, continue upwards and go back in to any changed
	//split nodes are handled when created
	B_Tree * bt = (B_Tree*) b;
	Tree_Node * tn = (Tree_Node*) t;
	Tree_Node * parent;

	while(tn!=NULL){
		if(tn->flush==1){
			jdisk_write(bt->disk, tn->lba, tn->bytes);
		}
		parent = tn->parent;
		free(tn->keys);
		free(tn->lbas);
		free(tn);
		tn = parent;
	}
	if(bt->flush==1){
		jdisk_write(bt->disk, 0, bt);
	}
}

unsigned int b_tree_insert(void *b_tree, void *key, void *record){

	unsigned int findLBA;
	unsigned int retBlock;
	unsigned int numKeysMove,moveSize;
	int i, ind;
	B_Tree * bt = (B_Tree*) b_tree;
	Tree_Node * tn = (Tree_Node*)malloc(sizeof(Tree_Node));

	findLBA = b_tree_find(bt, key);
	if(findLBA==0){//new key
		/* tn populated during find */
		tn = bt->tmp_e;
		//make room for inserted key
		ind = bt->tmp_e_index;
		numKeysMove = tn->nkeys - bt->tmp_e_index;
		moveSize = numKeysMove * bt->key_size;

		if(ind < bt->keys_per_block){
			memmove(*(tn->keys+ind+1), *(tn->keys+ind), moveSize);//how to test?
		}

		//put key in right place
		memcpy((void*)*(tn->keys+ind), key, bt->key_size);//how to test?

		//make room for new LBA
		memmove(tn->lbas+ind+1, tn->lbas+ind, (numKeysMove+1)*sizeof(int));

		//get first free block
		*(tn->lbas+ind) = bt->first_free_block;

		//exit if disk is too small
		if(bt->first_free_block > bt->num_lbas-1){
			return 0;
		}

		jdisk_write(bt->disk, bt->first_free_block, record);
		retBlock = bt->first_free_block;
		bt->first_free_block++;
		bt->flush=1;

		//increment nkeys
		tn->flush = 1;
		tn->nkeys++;
		tn->bytes[1] = tn->nkeys;
		//check if split
		if(tn->nkeys > bt->keys_per_block){
			b_tree_split((void *) bt, (void*) tn);
		}
		//copy lbas into bytes
		memcpy(tn->bytes + JSIZE - bt->lbas_per_block*sizeof(int), tn->lbas, bt->lbas_per_block * sizeof(int));

		find_flush((void*)bt, (void*)tn);
		return retBlock;
	}
	if(findLBA!=0){//key already present
		jdisk_write(bt->disk, findLBA, record);
		return findLBA;
	}
	return -1;
}

void * new_split_node(void *b, void *t){
	int i, middle;
	B_Tree * bt = (B_Tree*) b;
	Tree_Node * tn = (Tree_Node*) t;
	Tree_Node * rtn = (Tree_Node*)malloc(sizeof(Tree_Node));
	Tree_Node * parent = tn->parent;

	rtn->lbas = malloc(sizeof(int *) * (bt->lbas_per_block+1));
	rtn->keys = malloc(sizeof(char *) * (bt->keys_per_block+1));

	/* initialize placement of keys */
	for(i=0; i<bt->keys_per_block+1; i++){
		*(rtn->keys+i) = rtn->bytes + (bt->key_size * i) + 2;
	}

	middle = bt->keys_per_block/2;

	rtn->nkeys = bt->keys_per_block/2 + (bt->keys_per_block)%2;
	rtn->internal = tn->internal;
	rtn->lba = bt->first_free_block;
		if(bt->first_free_block > bt->num_lbas-1){
			//exit(1);//just changed when cleaning up!!!!
			return 0;
		}
	bt->first_free_block++;
	bt->flush=1;
	rtn->parent = parent;
	rtn->parent_index = tn->parent_index+1;

	/* Setup right hand side bytes */
	rtn->bytes[0] = rtn->internal;
	rtn->bytes[1] = rtn->nkeys;
	memcpy(rtn->bytes+2, *(tn->keys+middle+1), rtn->nkeys * bt->key_size);

	memcpy(rtn->lbas, tn->lbas+middle+1, sizeof(int) * (rtn->nkeys+1));// messed with parens for mult

	memcpy(rtn->bytes + JSIZE - bt->lbas_per_block*sizeof(int), rtn->lbas, bt->lbas_per_block * sizeof(int));

	jdisk_write(bt->disk, rtn->lba, rtn->bytes);

	//left hand side
	tn->nkeys =  bt->keys_per_block/2;
	tn->bytes[1] = tn->nkeys;
	tn->flush=1;
	//theoretically shouldnt have to memset if nkeys is right
	//memset(tn->bytes+2+middle*bt->key_size...)
	
	return rtn;
}

void * new_root_node(void *b, void *t){
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
	memcpy(roottn->bytes + JSIZE - bt->lbas_per_block*sizeof(int), roottn->lbas, bt->lbas_per_block * sizeof(int));



	roottn->keys = malloc(sizeof(char *) * (bt->keys_per_block+1));

	/* initialize placement of keys */
	for(i=0; i<bt->keys_per_block+1; i++){
		*(roottn->keys+i) = roottn->bytes + (bt->key_size * i) + 2;
	}
}

b_tree_split(void *b_tree, void *t){
	B_Tree * bt = (B_Tree*) b_tree;
	unsigned char pnkeys;
	int numKeysMove;
	unsigned char ** pkeys;
	int middle, i;
	Tree_Node * tn;
	tn = (Tree_Node *) t;

	Tree_Node * rtn;

	//deal with if root node
	if(tn->parent==NULL){
		new_root_node((void*) bt, (void*) tn);
	}
	/* Init Middle Key, untested for odd maxkeys */
	middle = bt->keys_per_block/2;

	pnkeys = tn->parent->nkeys;
	pkeys = tn->parent->keys;

	/* Moving Middle Key to Parent */
	numKeysMove = pnkeys - tn->parent_index;
	memmove(*(pkeys + tn->parent_index+1), *(pkeys + tn->parent_index), numKeysMove * bt->key_size);//changed from cpy to move
	memcpy(*(pkeys + tn->parent_index), *(tn->keys+middle), bt->key_size);

	//deal with lbas
	memmove(tn->parent->lbas + tn->parent_index+1, tn->parent->lbas + tn->parent_index, sizeof(int) * (numKeysMove+1));//changed to memmoove from cpu

	//copy right half to new node
	rtn = new_split_node((void*)bt, (void*)tn);
	*(tn->parent->lbas + tn->parent_index+1) = rtn->lba;
	tn->parent->nkeys++;
	tn->parent->bytes[1] = tn->parent->nkeys;
	tn->parent->flush=1;

	//delete middle and right keys in current node
	//if parent too big, recursively split
	if(tn->parent->nkeys > bt->keys_per_block){
		b_tree_split((void*)bt, (void*) tn->parent);
	}

	//wait to do memcpys to avoid overwriting keys
	memcpy(tn->parent->bytes + JSIZE - bt->lbas_per_block*sizeof(int), tn->parent->lbas, bt->lbas_per_block * sizeof(int));
}


//populate the tree node, still need to deal with free list
void pop_tnode(void * t, void * b, unsigned int lba, void * p){

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
	if(p!=NULL){
		for(i=0; i<parent->nkeys+1; i++){
			if(*(parent->lbas+i) == tn->lba){
				tn->parent_index = i;
			}
		}
		if(tn->parent_index==-1)printf("LBA NOT FOUND IN PARENT, SOMETHINGS WRONG\n");
	}

	tn->keys = malloc(sizeof(char *) * (bt->keys_per_block+1));

	/* initialize placement of keys, still need to test */
	for(i=0; i<bt->keys_per_block+1; i++){
		*(tn->keys+i) = tn->bytes + (bt->key_size * i) + 2;
	}

	/* room for an extra lba for splitting */
	tn->lbas = malloc(sizeof(int *) * (bt->lbas_per_block+1));
	/*copy all keys in tn->lbas*/
	memcpy(tn->lbas, tn->bytes + JSIZE - bt->lbas_per_block*sizeof(int), bt->lbas_per_block * sizeof(int));//want to copy into seperate array to deal with bigger size

}

unsigned int b_tree_find(void *b_tree, void *key){
	B_Tree * bt = (B_Tree*)b_tree;
	Tree_Node * tn;
	Tree_Node * parent;
	int i;
	int found, lt;
	unsigned int next_lba, ret_lba;
	tn = (Tree_Node *) malloc(sizeof(Tree_Node));

	//jdisk_read(bt->disk, bt->root_lba, (void*) tn);

	pop_tnode(tn, bt, bt->root_lba, NULL);

	found=0;
	while(tn->internal){
		lt = 0;
		for(i=0; i < tn->nkeys && lt==0 && found ==0; i++){//got rid of found=0
			if(memcmp(key, *(tn->keys+i), bt->key_size) < 0){
				lt=1;
			}
			else if(memcmp(key, *(tn->keys+i), bt->key_size) > 0){
			}
			else if(memcmp(key, *(tn->keys+i), bt->key_size) == 0){
				found=1;//not completely tested
			}
			else{
			}
		}
		//index into keys
		if(lt) i--;
		if(found==1){
			i--;
			found++;
		}
		else if(found>1){
			i = tn->nkeys;
		}

		next_lba = *(tn->lbas+i);

		//set tn to next LBA and populate
		parent = tn;
		tn = (Tree_Node *) malloc(sizeof(Tree_Node));
		pop_tnode(tn, bt, next_lba, parent);
	}

	for(i=0; i < tn->nkeys; i++){
		if(memcmp(key, *(tn->keys+i), bt->key_size) < 0){
			break;
		}
		else if(memcmp(key, *(tn->keys+i), bt->key_size) > 0){
		}
		else if(memcmp(key, *(tn->keys+i), bt->key_size) == 0){
			ret_lba = *(tn->lbas+i);
			return ret_lba;
		}
		else{
		}
	}
	if(found){
		ret_lba = *(tn->lbas+i);
		return ret_lba;
	}
	else{
		//save current tree node in bt when find misses, untested
		bt->tmp_e = tn;
		bt->tmp_e_index = i;
		return 0;
	}
}
void *b_tree_disk(void *b_tree){
	B_Tree * bt = (B_Tree*)b_tree;
	return bt->disk;
}

int b_tree_key_size(void *b_tree){
	B_Tree * bt = (B_Tree*)b_tree;
	return bt->key_size;
}

void b_tree_print_tree(void *b_tree){
}
