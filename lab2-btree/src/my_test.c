#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "b_tree.h"

#define BUFSIZE 4000
#define TESTFIND 0
#define TESTINSERT 1
#define CREATE 1

void usage(char *s)
{
	fprintf(stderr, "usage: b_tree_test file [CREATE file_size key_size]\n");
	if (s != NULL) fprintf(stderr, "%s\n", s);
	exit(1);
}
int main(int argc, char** argv){

	if (argc != 3) usage(NULL);
	void *bp, *jd;
	int key_size, record_size, m, i;
	unsigned long file_size;
	unsigned int lba;
	char line[BUFSIZE];
	char fi[BUFSIZE];
	char key[BUFSIZE];
	char val[BUFSIZE];
	int outi;

	char* tmp2 = malloc(200);
	if(!CREATE)bp = b_tree_attach(argv[1]);
	if(CREATE)bp = b_tree_create(argv[1], 20480, 25);
	void *vtmp;
	void *rtmp = malloc(1024);
	char * b = rtmp;
	b[0]='t';
	b[1]='e';
	b[2]='s';
	b[3]='t';

	for(i=4; i<1024; i++){
		b[i] = (char)0;
	}

	if(TESTINSERT){
		tmp2[0] = 'H';
		tmp2[1] = 'l';
		tmp2[2] = 'f';
		for(i=3; i<200; i++){
			tmp2[i] = (char)0;
		}
		vtmp = tmp2;
		b_tree_insert(bp, vtmp, rtmp);
	}

	if(TESTFIND){
		tmp2[0] = 'E';
		tmp2[1] = 'l';
		tmp2[2] = 'i';
		for(i=3; i<200; i++){
			tmp2[i] = (char)0;
		}
		vtmp = tmp2;
		outi = b_tree_find(bp, vtmp);
		printf("out = %d\n", outi);

		tmp2[0] = 'J';
		tmp2[1] = 'a';
		tmp2[2] = 'm';
		tmp2[3] = 'e';
		tmp2[4] = 's';
		for(i=5; i<200; i++){
			tmp2[i] = (char)0;
		}
		vtmp = tmp2;
		outi = b_tree_find(bp, vtmp);
		printf("out = %d\n", outi);
		tmp2[0] = 'A';
		tmp2[1] = 'l';
		tmp2[2] = 'e';
		tmp2[3] = 'x';
		tmp2[4] = 'i';
		tmp2[5] = 's';
		for(i=6; i<200; i++){
			tmp2[i] = (char)0;
		}
		vtmp = tmp2;
		outi = b_tree_find(bp, vtmp);
		printf("out = %d\n", outi);

		tmp2[0] = 'D';
		tmp2[1] = 'a';
		tmp2[2] = 'n';
		tmp2[3] = 'i';
		tmp2[4] = 'e';
		tmp2[5] = 'l';
		for(i=6; i<200; i++){
			tmp2[i] = (char)0;
		}
		vtmp = tmp2;
		outi = b_tree_find(bp, vtmp);
		printf("out = %d\n", outi);

		tmp2[0] = 'C';
		tmp2[1] = 'a';
		tmp2[2] = 'l';
		tmp2[3] = 'e';
		tmp2[4] = 'b';
		for(i=5; i<200; i++){
			tmp2[i] = (char)0;
		}
		vtmp = tmp2;
		outi = b_tree_find(bp, vtmp);
		printf("out = %d\n", outi);
	}
}
