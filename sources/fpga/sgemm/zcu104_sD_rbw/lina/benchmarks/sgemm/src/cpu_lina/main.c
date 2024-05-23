/***************************************************************************
 *cr
 *cr            (C) Copyright 2010 The Board of Trustees of the
 *cr                        University of Illinois
 *cr                         All Rights Reserved
 *cr
 ***************************************************************************/

/* 
 * Main entry of dense matrix-matrix multiplication kernel
 */

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <malloc.h>
#include <parboil.h>
#include "sgemm.h"

// I/O routines
extern bool readColMajorMatrixFile(const char *fn, int *nr_row, int *nr_col, float **v);
extern bool writeColMajorMatrixFile(const char *fn, int, int, float *v);

int
main (int argc, char *argv[]) {
	// XXX argc and argv are hardcoded since Lina does not pass arguments during trace
	int _argc = 5;
	char *_argv[] = {
		argv[0],
		"-i",
		"matrix1.txt,matrix2t.txt,matrix2t.txt",
		"-o",
		"matrix3.txt"
	};

  struct pb_Parameters *params;
  struct pb_TimerSet timers;

  int matArow, matAcol;
  int matBrow, matBcol;

  float *matA = NULL, *matBT = NULL;

  pb_InitializeTimerSet(&timers);

  /* Read command line. Expect 3 inputs: A, B and B^T 
     in column-major layout*/
  params = pb_ReadParameters(&_argc, _argv);
  if ((params->inpFiles[0] == NULL) 
      || (params->inpFiles[1] == NULL)
      || (params->inpFiles[2] == NULL)
      || (params->inpFiles[3] != NULL))
    {
      fprintf(stderr, "Expecting three input filenames\n");
      exit(-1);
    }
 
  /* Read in data */
  pb_SwitchToTimer(&timers, pb_TimerID_IO);

  // load A
  readColMajorMatrixFile(params->inpFiles[0],
      &matArow, &matAcol, &matA);

  // load B^T
  readColMajorMatrixFile(params->inpFiles[2],
      &matBcol, &matBrow, &matBT);

  pb_SwitchToTimer( &timers, pb_TimerID_COMPUTE );

  // allocate space for C
  float *matC = calloc(matArow * matBcol, sizeof(float));

  if(matArow != M || matBcol != N || matAcol != K) {
    printf("ERROR: This version has fixed dimensions. M must be %d, N must be %d, K must be %d\n", M, N, K);
    exit(-1);
  }

  sgemm(matA, matBT, matC);

  if (params->outFile) {
    /* Write C to file */
    pb_SwitchToTimer(&timers, pb_TimerID_IO);
    writeColMajorMatrixFile(params->outFile, matArow, matBcol, matC); 
  }

  pb_SwitchToTimer(&timers, pb_TimerID_NONE);

  double CPUtime = pb_GetElapsedTime(&(timers.timers[pb_TimerID_COMPUTE]));
  printf("GFLOPs = %lf\n",  2.* matArow * matBcol * matAcol/CPUtime/1e9);
  pb_PrintTimerSet(&timers);
  pb_FreeParameters(params);
  if(matA) free(matA);
  if(matBT) free(matBT);
  if(matC) free(matC);
  return 0;
}
