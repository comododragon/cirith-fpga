#ifndef CLFUNCS_H
#define CLFUNCS_H

#define MAX_NO_PLATFORMS 256
#define MAX_NO_DEVICES 256

int clInit(const char *programFileName);
int clPrepareBuffers();
int clCopyToDevice(float *srcGrid, float *dstGrid);
int clRunKernel(void);
int clSwapBuffers(void);
int clCopyFromDevice(float *srcGrid, float *dstGrid);
int clReleaseBuffers(void);
int clRelease(void);

#endif
