/***************************************************************************
 *cr
 *cr            (C) Copyright 2007 The Board of Trustees of the
 *cr                        University of Illinois
 *cr                         All Rights Reserved
 *cr
 ***************************************************************************/

#include "mri_q.h"

#include <malloc.h>
#include <string.h>

#include "sincoslt.h"

#define PI   3.1415926535897932384626433832795029f
#define PIx2 6.2831853071795864769252867665590058f

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define K_ELEMS_PER_GRID 2048

void 
ComputePhiMagCPU(int numK, 
                 float* phiR, float* phiI, float* phiMag) {
  int indexK = 0;
  for (indexK = 0; indexK < numK; indexK++) {
    float real = phiR[indexK];
    float imag = phiI[indexK];
    phiMag[indexK] = real*real + imag*imag;
  }
}

void mri_q(float *kx, float *ky, float *kz, float *phiMag,
            float* x, float* y, float* z,
            float *Qr, float *Qi) {
  float expArg;
  float cosArg;
  float sinArg;

  int indexK, indexX;
  for (indexK = 0; indexK < NUMK; indexK++) {
    for (indexX = 0; indexX < NUMX; indexX++) {
      expArg = PIx2 * (kx[indexK] * x[indexX] +
                       ky[indexK] * y[indexX] +
                       kz[indexK] * z[indexX]);

      COS(cosArg, expArg);
      SIN(sinArg, expArg);

      float phi = phiMag[indexK];
      Qr[indexX] += phi * cosArg;
      Qi[indexX] += phi * sinArg;
    }
  }
}

void createDataStructsCPU(int numK, int numX, float** phiMag,
	 float** Qr, float** Qi)
{
  *phiMag = (float* ) memalign(16, numK * sizeof(float));
  *Qr = (float*) memalign(16, numX * sizeof (float));
  memset((void *)*Qr, 0, numX * sizeof(float));
  *Qi = (float*) memalign(16, numX * sizeof (float));
  memset((void *)*Qi, 0, numX * sizeof(float));
}
