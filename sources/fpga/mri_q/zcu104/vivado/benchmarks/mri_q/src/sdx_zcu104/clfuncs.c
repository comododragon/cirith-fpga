#include "clfuncs.h"

#include <CL/opencl.h>
#include <stdio.h>
#include <stdlib.h>

#include "consts.h"

#define ASSERT(cond, errVal) do {\
	if(!(cond)) {\
		retVal = errVal;\
		goto _err;\
	}\
} while(0)

const static cl_uint workDim = 1;
const static size_t globalSz[1] = {1};
const static size_t localSz[1] = {1};
static cl_uint platformsLen = 0;
static cl_platform_id platforms[MAX_NO_PLATFORMS];
static cl_uint devicesLen = 0;
static cl_device_id devices[MAX_NO_DEVICES];
static cl_context context = NULL;
static cl_command_queue queue = NULL;
static cl_program program = NULL;
static cl_kernel kernel = NULL;
static cl_mem kxK = NULL;
static cl_mem kyK = NULL;
static cl_mem kzK = NULL;
static cl_mem phiMagK = NULL;
static cl_mem xK = NULL;
static cl_mem yK = NULL;
static cl_mem zK = NULL;
static cl_mem QrK = NULL;
static cl_mem QiK = NULL;

int clInit(const char *programFileName) {
	int retVal = 0;
	cl_int fRet = 0;
	cl_int programRet = 0;
	FILE *programFile = NULL;
	size_t programSz = 0;
	char *programContent = NULL;

	fRet = clGetPlatformIDs(0, NULL, &platformsLen);
	ASSERT(CL_SUCCESS == fRet, -1);
	fRet = clGetPlatformIDs(MAX_NO_PLATFORMS, platforms, NULL);
	ASSERT(CL_SUCCESS == fRet, -1);

	fRet = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, 0, NULL, &devicesLen);
	ASSERT(CL_SUCCESS == fRet, -1);
	fRet = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, MAX_NO_DEVICES, devices, NULL);
	ASSERT(CL_SUCCESS == fRet, -1);

	context = clCreateContext(NULL, 1, devices, NULL, NULL, &fRet);
	ASSERT(CL_SUCCESS == fRet, -1);

	queue = clCreateCommandQueue(context, devices[0], 0, &fRet);
	ASSERT(CL_SUCCESS == fRet, -1);

	programFile = fopen(programFileName, "rb");
	ASSERT(programFile, -2);

	fseek(programFile, 0, SEEK_END);
	programSz = ftell(programFile);
	fseek(programFile, 0, SEEK_SET);
	programContent = malloc(programSz);
	fread(programContent, programSz, 1, programFile);

	program = clCreateProgramWithBinary(context, 1, devices, &programSz, (const unsigned char **) &programContent, &programRet, &fRet);
	ASSERT(CL_SUCCESS == programRet, -3);
	ASSERT(CL_SUCCESS == fRet, -1);
	fRet = clBuildProgram(program, 1, devices, NULL, NULL, NULL);
	ASSERT(CL_SUCCESS == fRet, -1);

	kernel = clCreateKernel(program, "mri_q", &fRet);
	ASSERT(CL_SUCCESS == fRet, -1);

_err:

	if(retVal) {
		if(kernel) {
			clReleaseKernel(kernel);
			kernel = NULL;
		}
		if(program) {
			clReleaseProgram(program);
			program = NULL;
		}
		if(queue) {
			clReleaseCommandQueue(queue);
			queue = NULL;
		}
		if(context) {
			clReleaseContext(context);
			context = NULL;
		}

		devicesLen = 0;
		platformsLen = 0;
	}

	if(programContent) {
		free(programContent);
		programContent = NULL;
	}
	if(programFile) {
		fclose(programFile);
		programFile = NULL;
	}

	return retVal;
}

int clPrepareBuffers(void) {
	int retVal = 0;
	cl_int fRet = 0;

	kxK = clCreateBuffer(context, CL_MEM_READ_ONLY, NUMK * sizeof(float), NULL, &fRet);
	ASSERT(CL_SUCCESS == fRet, -1);
	kyK = clCreateBuffer(context, CL_MEM_READ_ONLY, NUMK * sizeof(float), NULL, &fRet);
	ASSERT(CL_SUCCESS == fRet, -1);
	kzK = clCreateBuffer(context, CL_MEM_READ_ONLY, NUMK * sizeof(float), NULL, &fRet);
	ASSERT(CL_SUCCESS == fRet, -1);
	phiMagK = clCreateBuffer(context, CL_MEM_READ_ONLY, NUMK * sizeof(float), NULL, &fRet);
	ASSERT(CL_SUCCESS == fRet, -1);
	xK = clCreateBuffer(context, CL_MEM_READ_ONLY, NUMX * sizeof(float), NULL, &fRet);
	ASSERT(CL_SUCCESS == fRet, -1);
	yK = clCreateBuffer(context, CL_MEM_READ_ONLY, NUMX * sizeof(float), NULL, &fRet);
	ASSERT(CL_SUCCESS == fRet, -1);
	zK = clCreateBuffer(context, CL_MEM_READ_ONLY, NUMX * sizeof(float), NULL, &fRet);
	ASSERT(CL_SUCCESS == fRet, -1);
	QrK = clCreateBuffer(context, CL_MEM_READ_WRITE, NUMX * sizeof(float), NULL, &fRet);
	ASSERT(CL_SUCCESS == fRet, -1);
	QiK = clCreateBuffer(context, CL_MEM_READ_WRITE, NUMX * sizeof(float), NULL, &fRet);
	ASSERT(CL_SUCCESS == fRet, -1);

	fRet = clSetKernelArg(kernel, 0, sizeof(cl_mem), &kxK);
	ASSERT(CL_SUCCESS == fRet, -1);
	fRet = clSetKernelArg(kernel, 1, sizeof(cl_mem), &kyK);
	ASSERT(CL_SUCCESS == fRet, -1);
	fRet = clSetKernelArg(kernel, 2, sizeof(cl_mem), &kzK);
	ASSERT(CL_SUCCESS == fRet, -1);
	fRet = clSetKernelArg(kernel, 3, sizeof(cl_mem), &phiMagK);
	ASSERT(CL_SUCCESS == fRet, -1);
	fRet = clSetKernelArg(kernel, 4, sizeof(cl_mem), &xK);
	ASSERT(CL_SUCCESS == fRet, -1);
	fRet = clSetKernelArg(kernel, 5, sizeof(cl_mem), &yK);
	ASSERT(CL_SUCCESS == fRet, -1);
	fRet = clSetKernelArg(kernel, 6, sizeof(cl_mem), &zK);
	ASSERT(CL_SUCCESS == fRet, -1);
	fRet = clSetKernelArg(kernel, 7, sizeof(cl_mem), &QrK);
	ASSERT(CL_SUCCESS == fRet, -1);
	fRet = clSetKernelArg(kernel, 8, sizeof(cl_mem), &QiK);
	ASSERT(CL_SUCCESS == fRet, -1);

_err:

	if(retVal) {
		if(QiK) {
			clReleaseMemObject(QiK);
			QiK = NULL;
		}
		if(QrK) {
			clReleaseMemObject(QrK);
			QrK = NULL;
		}
		if(zK) {
			clReleaseMemObject(zK);
			zK = NULL;
		}
		if(yK) {
			clReleaseMemObject(yK);
			yK = NULL;
		}
		if(xK) {
			clReleaseMemObject(xK);
			xK = NULL;
		}
		if(phiMagK) {
			clReleaseMemObject(phiMagK);
			phiMagK = NULL;
		}
		if(kzK) {
			clReleaseMemObject(kzK);
			kxK = NULL;
		}
		if(kyK) {
			clReleaseMemObject(kyK);
			kxK = NULL;
		}
		if(kxK) {
			clReleaseMemObject(kxK);
			kxK = NULL;
		}
	}

	return retVal;
}

int clCopyToDevice(float *kx, float *ky, float *kz, float *phiMag, float *x, float *y, float *z) {
	int retVal = 0;
	cl_int fRet = 0;

	fRet = clEnqueueWriteBuffer(queue, kxK, CL_TRUE, 0, NUMK * sizeof(float), kx, 0, NULL, NULL);
	ASSERT(CL_SUCCESS == fRet, -1);
	fRet = clEnqueueWriteBuffer(queue, kyK, CL_TRUE, 0, NUMK * sizeof(float), ky, 0, NULL, NULL);
	ASSERT(CL_SUCCESS == fRet, -1);
	fRet = clEnqueueWriteBuffer(queue, kzK, CL_TRUE, 0, NUMK * sizeof(float), kz, 0, NULL, NULL);
	ASSERT(CL_SUCCESS == fRet, -1);
	fRet = clEnqueueWriteBuffer(queue, phiMagK, CL_TRUE, 0, NUMK * sizeof(float), phiMag, 0, NULL, NULL);
	ASSERT(CL_SUCCESS == fRet, -1);
	fRet = clEnqueueWriteBuffer(queue, xK, CL_TRUE, 0, NUMX * sizeof(float), x, 0, NULL, NULL);
	ASSERT(CL_SUCCESS == fRet, -1);
	fRet = clEnqueueWriteBuffer(queue, yK, CL_TRUE, 0, NUMX * sizeof(float), y, 0, NULL, NULL);
	ASSERT(CL_SUCCESS == fRet, -1);
	fRet = clEnqueueWriteBuffer(queue, zK, CL_TRUE, 0, NUMX * sizeof(float), z, 0, NULL, NULL);
	ASSERT(CL_SUCCESS == fRet, -1);

_err:

	return retVal;
}

int clRunKernel(void) {
	int retVal = 0;
	cl_int fRet = 0;

	fRet = clEnqueueNDRangeKernel(queue, kernel, workDim, NULL, globalSz, localSz, 0, NULL, NULL);
	ASSERT(CL_SUCCESS == fRet, -1);
	clFinish(queue);

_err:

	return retVal;
}

int clCopyFromDevice(float *Qr, float *Qi) {
	int retVal = 0;
	cl_int fRet = 0;

	fRet = clEnqueueReadBuffer(queue, QrK, CL_TRUE, 0, NUMX * sizeof(float), Qr, 0, NULL, NULL);
	ASSERT(CL_SUCCESS == fRet, -1);
	fRet = clEnqueueReadBuffer(queue, QiK, CL_TRUE, 0, NUMX * sizeof(float), Qi, 0, NULL, NULL);
	ASSERT(CL_SUCCESS == fRet, -1);

_err:

	return retVal;
}

int clReleaseBuffers(void) {
	int retVal = 0;
	cl_int fRet = 0;

	if(QiK) {
		clReleaseMemObject(QiK);
		QiK = NULL;
	}
	if(QrK) {
		clReleaseMemObject(QrK);
		QrK = NULL;
	}
	if(zK) {
		clReleaseMemObject(zK);
		zK = NULL;
	}
	if(yK) {
		clReleaseMemObject(yK);
		yK = NULL;
	}
	if(xK) {
		clReleaseMemObject(xK);
		xK = NULL;
	}
	if(phiMagK) {
		clReleaseMemObject(phiMagK);
		phiMagK = NULL;
	}
	if(kzK) {
		clReleaseMemObject(kzK);
		kxK = NULL;
	}
	if(kyK) {
		clReleaseMemObject(kyK);
		kxK = NULL;
	}
	if(kxK) {
		clReleaseMemObject(kxK);
		kxK = NULL;
	}

_err:

	return retVal;
}

int clRelease(void) {
	int retVal = 0;
	cl_int fRet = 0;

	if(kernel) {
		clReleaseKernel(kernel);
		kernel = NULL;
	}
	if(program) {
		clReleaseProgram(program);
		program = NULL;
	}
	if(queue) {
		clReleaseCommandQueue(queue);
		queue = NULL;
	}
	if(context) {
		clReleaseContext(context);
		context = NULL;
	}

	devicesLen = 0;
	platformsLen = 0;

_err:

	return retVal;
}
