#include "clfuncs.h"

#include <CL/opencl.h>
#include <stdio.h>
#include <stdlib.h>

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
static cl_mem imgK = NULL;
static cl_mem histoK = NULL;

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

	kernel = clCreateKernel(program, "histo", &fRet);
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

int clPrepareBuffers(unsigned int imgWidth, unsigned int imgHeight, unsigned int histoWidth, unsigned int histoHeight) {
	int retVal = 0;
	cl_int fRet = 0;

	imgK = clCreateBuffer(context, CL_MEM_READ_ONLY, imgWidth * imgHeight * sizeof(unsigned int), NULL, &fRet);
	ASSERT(CL_SUCCESS == fRet, -1);
	histoK = clCreateBuffer(context, CL_MEM_READ_WRITE, histoWidth * histoHeight * sizeof(unsigned char), NULL, &fRet);
	ASSERT(CL_SUCCESS == fRet, -1);

	fRet = clSetKernelArg(kernel, 0, sizeof(cl_mem), &imgK);
	ASSERT(CL_SUCCESS == fRet, -1);
	fRet = clSetKernelArg(kernel, 1, sizeof(unsigned int), &imgWidth);
	ASSERT(CL_SUCCESS == fRet, -1);
	fRet = clSetKernelArg(kernel, 2, sizeof(unsigned int), &imgHeight);
	ASSERT(CL_SUCCESS == fRet, -1);
	fRet = clSetKernelArg(kernel, 3, sizeof(cl_mem), &histoK);
	ASSERT(CL_SUCCESS == fRet, -1);
	fRet = clSetKernelArg(kernel, 4, sizeof(unsigned int), &histoWidth);
	ASSERT(CL_SUCCESS == fRet, -1);
	fRet = clSetKernelArg(kernel, 5, sizeof(unsigned int), &histoHeight);
	ASSERT(CL_SUCCESS == fRet, -1);

_err:

	if(retVal) {
		if(histoK) {
			clReleaseMemObject(histoK);
			histoK = NULL;
		}
		if(imgK) {
			clReleaseMemObject(imgK);
			imgK = NULL;
		}
	}

	return retVal;
}

int clCopyToDevice(unsigned int *img, unsigned int imgWidth, unsigned int imgHeight) {
	int retVal = 0;
	cl_int fRet = 0;

	fRet = clEnqueueWriteBuffer(queue, imgK, CL_TRUE, 0, imgWidth * imgHeight * sizeof(unsigned int), img, 0, NULL, NULL);
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

int clCopyFromDevice(unsigned char *histo, unsigned int histoWidth, unsigned int histoHeight) {
	int retVal = 0;
	cl_int fRet = 0;

	fRet = clEnqueueReadBuffer(queue, histoK, CL_TRUE, 0, histoWidth * histoHeight * sizeof(unsigned char), histo, 0, NULL, NULL);
	ASSERT(CL_SUCCESS == fRet, -1);

_err:

	return retVal;
}

int clReleaseBuffers(void) {
	int retVal = 0;
	cl_int fRet = 0;

	if(histoK) {
		clReleaseMemObject(histoK);
		histoK = NULL;
	}
	if(imgK) {
		clReleaseMemObject(imgK);
		imgK = NULL;
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
