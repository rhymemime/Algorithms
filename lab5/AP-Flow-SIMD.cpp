#include "AP-Flow.h"
#include "MOA.h"
#include <emmintrin.h>

void print_simd(string s, __m128i v)
{
	uint16_t array[8];
	__m128i *c;
	int i;

	c = (__m128i *) array;
	*c = v;
	printf("%-30s", s.c_str());
	for (i = 7; i >= 0; i--) printf(" %04x", array[i]);
	printf("\n");
}

void APFlow::CalcFlow()
{
	int i, j, v, f;
	__m128i alli, *vv, fv, *iv, rv;
	for (i = 0; i < N*N; i++) Flow[i] = Adj[i];

	for (v = 0; v < N; v++) {
		for (i = 0; i < N; i++) {

			/* Create a vector alli, which is 16 instances of Flow[i*N+v] */
			alli = _mm_set1_epi8(Flow[i*N+v]);
			//print_simd("alli=_mm_set1_epi8(Flow[i*N+v])", alli);

			for (j = 0; j < N; j += 16) {

				/* Load Flow[v*N+j] through Flow[v*N+j+15] to vector vv */
				vv = (__m128i *) &Flow[v*N+j];
				//print_simd("vv = &Flow[v*N+j]", *vv);


				/* Create fv, which is the flow from i to each of j through j+15
				 *            through v. This is simply the min of alli and vv. */
				fv =  _mm_min_epu8 (*vv, alli);

				/* Load Flow[i*N+j] through Flow[i*N+j+15] to vector iv */
				iv = (__m128i *) &Flow[i*N+j];

				/* Create rv, which is the max of each value of fv and iv. */
				rv =  _mm_max_epu8 (fv, *iv);

				/* Store rv into Flow[i*N+j] through Flow[i*N+j+15] */
				uint8_t *ii = (uint8_t *) &rv;
				for(f=0; f<16; f++){
					Flow[i*N+j+f] = *(ii+f);
				}
			}   
		}
	}
}
