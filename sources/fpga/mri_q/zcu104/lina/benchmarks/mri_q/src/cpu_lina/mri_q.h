#ifndef MRI_Q_H
#define MRI_Q_H

#define NUMK 256
#define NUMX 32768
#define NUMK_STR "256"

void ComputePhiMagCPU(int numK, 
                 float* phiR, float* phiI, float* phiMag);

void mri_q(float *kx, float *ky, float *kz, float *phiMag,
            float* x, float* y, float* z,
            float *Qr, float *Qi);

void createDataStructsCPU(int numK, int numX, float** phiMag,
	 float** Qr, float** Qi);

#endif
