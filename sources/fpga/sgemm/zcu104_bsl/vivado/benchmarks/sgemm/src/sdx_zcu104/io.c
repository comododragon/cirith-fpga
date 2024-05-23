/***************************************************************************
 *cr
 *cr            (C) Copyright 2010 The Board of Trustees of the
 *cr                        University of Illinois
 *cr                         All Rights Reserved
 *cr
 ***************************************************************************/

/* I/O routines for reading and writing matrices in column-major
 * layout
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

bool readColMajorMatrixFile(const char *fn, int *nr_row, int *nr_col, float **v) {
	fprintf(stderr, "Opening file:%s\n", fn);
	FILE *ipf = fopen(fn, "r");
	if(!ipf)
		return false;

	fscanf(ipf, "%d", nr_row); 
	fscanf(ipf, "%d", nr_col);

	fprintf(stderr, "Matrix dimension: %dx%d\n", *nr_row, *nr_col);
	*v = malloc(*nr_row * *nr_col * sizeof(float));

	for(int i = 0; i < (*nr_row * *nr_col); i++)
		fscanf(ipf, "%f", &((*v)[i]));

	fclose(ipf);

	return true;
}

bool writeColMajorMatrixFile(const char *fn, int nr_row, int nr_col, float *v) {
	fprintf(stderr, "Opening file:%s for write.\n", fn);
	FILE *opf = fopen(fn, "w");
	if(!opf)
		return false;

	fprintf(opf, "%d %d ", nr_row, nr_col); 

	fprintf(stderr, "Matrix dimension: %dx%d\n", nr_row, nr_col);

	for(int i = 0; i < (nr_row * nr_col); i++)
		fprintf(opf, "%f ", v[i]);

	fclose(opf);

	return true;
}