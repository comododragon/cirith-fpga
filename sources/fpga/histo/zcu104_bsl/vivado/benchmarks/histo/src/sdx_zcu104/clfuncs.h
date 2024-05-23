#ifndef CLFUNCS_H
#define CLFUNCS_H

#define MAX_NO_PLATFORMS 256
#define MAX_NO_DEVICES 256

int clInit(const char *programFileName);
int clPrepareBuffers(unsigned int imgWidth, unsigned int imgHeight, unsigned int histoWidth, unsigned int histoHeight);
int clCopyToDevice(unsigned int *img, unsigned int imgWidth, unsigned int imgHeight);
int clRunKernel(void);
int clCopyFromDevice(unsigned char *histo, unsigned int histoWidth, unsigned int histoHeight);
int clReleaseBuffers(void);
int clRelease(void);

#endif
