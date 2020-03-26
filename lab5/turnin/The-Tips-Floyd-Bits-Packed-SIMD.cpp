#include "The-Tips.h"
#include <emmintrin.h>
#include "MOA.h"
#include <iostream>

double TheTips::solve(vector <string> Cl, vector <int> probability, int print)
{
	int i, j, k, l, v;
	//vector < vector <int> > C;
	uint8_t **C;
	__m128i *b1, *b2, *b3, tmp;
	vector <double> p;
	double x;
	const uint8_t one = 1;
	const uint8_t zero = 0;
	uint8_t *buf1, *buf2;

	C = (uint8_t **) malloc(Cl.size() * sizeof(uint8_t*));
	int numbytes = ceil(Cl.size() / 8.0);
	int rem = numbytes % 16;
	if(rem != 0){
		numbytes+=16 - rem;
	}

	/* Change the Y/N's to 1/0's */

	for (i = 0; i < Cl.size(); i++) {
		C[i] = (uint8_t *) malloc(numbytes);
		for (j = 0; j < numbytes; j++) {
			C[i][j] = zero;
			if(j*8 < Cl.size()){
				/* for each bit */
				for(k=0; k<8 && j*8+k < Cl.size(); k++){
					if(Cl[i][(j*8)+k] == 'Y'){
						/* by or'ing with 7-k we order the bits correctly */
						C[i][j] |= one << 7-k;
					}
				}
			}
		}
		/* same as C[i][i], accounting for doing bits */
		C[i][i/8] |= one << 7-(i%8);
	}

	for (v = 0; v < Cl.size(); v++) {
		for (i = 0; i < Cl.size(); i++) {
			/* if C[i]["v"] is not 0, accounting for bits */
			if (C[i][v/8] & 1 << 7-v%8) {
				/* working with 16 bytes at a time in SIMD call */
				for (j = 0; j < numbytes/16; j++) {
					buf1 = &C[i][j*16];
					b1 = (__m128i *) buf1;
					buf2 = &C[v][j*16];
					b2 = (__m128i *) buf2;
					tmp = _mm_or_si128(*b1, *b2);
					*b1 = tmp;
				}
			}
		}
	}

	/* Calculate the values of p from the probabilities and reachability: */

	p.resize(Cl.size(), 0);

	for (i = 0; i < Cl.size(); i++) {
		x = probability[i];
		//cout << "x: " << x << endl;
		x /= 100.0;
		for (j = 0; j < numbytes; j++) {
			for(k=0; k<8; k++){
				if ((C[i][j] & one << 7-k) !=0){
					/* account for bits */
					p[j*8+k] += ((1 - p[j*8+k]) * x);
				}
			}
		}
	}

	if (print) {
		printf("\nThe Expected Values\n\n");
		for (i = 0; i < Cl.size(); i++) {
			printf("I: %X    Prob: %7.5lf\n", i, p[i]);
		}
		printf("\n");
	}

	/* Calculate the final return value */

	x = 0;
	for (i = 0; i < Cl.size(); i++) x += p[i];

	return x;
}
