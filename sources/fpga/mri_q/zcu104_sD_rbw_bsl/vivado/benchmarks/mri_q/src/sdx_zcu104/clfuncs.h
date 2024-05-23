#ifndef CLFUNCS_H
#define CLFUNCS_H

#define MAX_NO_PLATFORMS 256
#define MAX_NO_DEVICES 256

int clInit(const char *programFileName);
int clPrepareBuffers();
int clCopyToDevice(float *kx, float *ky, float *kz, float *phiMag, float *x, float *y, float *z);
int clRunKernel(void);
int clCopyFromDevice(float *Qr, float *Qi);
int clReleaseBuffers(void);
int clRelease(void);

#endif
