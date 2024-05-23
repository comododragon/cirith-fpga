/***************************************************************************
 *cr
 *cr            (C) Copyright 2010 The Board of Trustees of the
 *cr                        University of Illinois
 *cr                         All Rights Reserved
 *cr
 ***************************************************************************/

/* 
 * Base C implementation of MM
 */

#include "sgemm.h"

void sgemm(float *A, float *B, float *C) {
	for(int mm = 0; mm < M; mm++) {
		for(int nn = 0; nn < N; nn++) {
			float c = 0.0f;

			for(int i = 0; i < K; i++) {
				float a = A[mm + i * M];
				float b = B[nn + i * N];
				c += a * b;
			}

			C[mm + nn * M] = C[mm + nn * M] * BETA + ALPHA * c;
		}
	}
}