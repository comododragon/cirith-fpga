#include "clfuncs.h"

#include <CL/opencl.h>
#include <stdio.h>
#include <stdlib.h>

#include "sgemm.h"

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
static cl_mem AK = NULL;
static cl_mem BK = NULL;
static cl_mem CK = NULL;

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

	kernel = clCreateKernel(program, "sgemm", &fRet);
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

	AK = clCreateBuffer(context, CL_MEM_READ_ONLY, M * K * sizeof(float), NULL, &fRet);
	ASSERT(CL_SUCCESS == fRet, -1);
	BK = clCreateBuffer(context, CL_MEM_READ_ONLY, N * K * sizeof(float), NULL, &fRet);
	ASSERT(CL_SUCCESS == fRet, -1);
	CK = clCreateBuffer(context, CL_MEM_READ_WRITE, M * N * sizeof(float), NULL, &fRet);
	ASSERT(CL_SUCCESS == fRet, -1);

	fRet = clSetKernelArg(kernel, 0, sizeof(cl_mem), &AK);
	ASSERT(CL_SUCCESS == fRet, -1);
	fRet = clSetKernelArg(kernel, 1, sizeof(cl_mem), &BK);
	ASSERT(CL_SUCCESS == fRet, -1);
	fRet = clSetKernelArg(kernel, 2, sizeof(cl_mem), &CK);
	ASSERT(CL_SUCCESS == fRet, -1);

_err:

	if(retVal) {
		if(CK) {
			clReleaseMemObject(CK);
			CK = NULL;
		}
		if(BK) {
			clReleaseMemObject(BK);
			BK = NULL;
		}
		if(AK) {
			clReleaseMemObject(AK);
			AK = NULL;
		}
	}

	return retVal;
}

int clCopyToDevice(float *A, float *B) {
	int retVal = 0;
	cl_int fRet = 0;

	fRet = clEnqueueWriteBuffer(queue, AK, CL_TRUE, 0, M * K * sizeof(float), A, 0, NULL, NULL);
	ASSERT(CL_SUCCESS == fRet, -1);
	fRet = clEnqueueWriteBuffer(queue, BK, CL_TRUE, 0, N * K * sizeof(float), B, 0, NULL, NULL);
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

int clCopyFromDevice(float *C) {
	int retVal = 0;
	cl_int fRet = 0;

	fRet = clEnqueueReadBuffer(queue, CK, CL_TRUE, 0, M * N * sizeof(float), C, 0, NULL, NULL);
	ASSERT(CL_SUCCESS == fRet, -1);

_err:

	return retVal;
}

int clReleaseBuffers(void) {
	int retVal = 0;
	cl_int fRet = 0;

	if(CK) {
		clReleaseMemObject(CK);
		CK = NULL;
	}
	if(BK) {
		clReleaseMemObject(BK);
		BK = NULL;
	}
	if(AK) {
		clReleaseMemObject(AK);
		AK = NULL;
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
