#ifndef MRI_Q_H
#define MRI_Q_H

#include "consts.h"

void ComputePhiMagCPU(int numK, 
                 float* phiR, float* phiI, float* phiMag);

void createDataStructsCPU(int numK, int numX, float** phiMag,
	 float** Qr, float** Qi);

#endif
