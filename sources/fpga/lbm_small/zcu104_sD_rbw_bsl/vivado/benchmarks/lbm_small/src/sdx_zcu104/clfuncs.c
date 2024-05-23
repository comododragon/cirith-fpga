#include "clfuncs.h"

#include "lbm.h"

#include <CL/opencl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define ASSERT(cond, errVal) do {\
	if(!(cond)) {\
		retVal = errVal;\
		goto _err;\
	}\
} while(0)

const static size_t margin = 2 * SIZE_X * SIZE_Y * N_CELL_ENTRIES;
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
static cl_mem srcK = NULL;
static cl_mem dstK = NULL;
static bool tiktok = true;

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

	kernel = clCreateKernel(program, "lbm_small", &fRet);
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

	srcK = clCreateBuffer(context, CL_MEM_READ_WRITE, ((SIZE_X * SIZE_Y * SIZE_Z * N_CELL_ENTRIES) + (2 * margin)) * sizeof(float), NULL, &fRet);
	ASSERT(CL_SUCCESS == fRet, -1);
	dstK = clCreateBuffer(context, CL_MEM_READ_WRITE, ((SIZE_X * SIZE_Y * SIZE_Z * N_CELL_ENTRIES) + (2 * margin)) * sizeof(float), NULL, &fRet);
	ASSERT(CL_SUCCESS == fRet, -1);

	fRet = clSetKernelArg(kernel, 0, sizeof(cl_mem), &srcK);
	ASSERT(CL_SUCCESS == fRet, -1);
	fRet = clSetKernelArg(kernel, 1, sizeof(cl_mem), &dstK);
	ASSERT(CL_SUCCESS == fRet, -1);

_err:

	if(retVal) {
		if(dstK) {
			clReleaseMemObject(dstK);
			dstK = NULL;
		}
		if(srcK) {
			clReleaseMemObject(srcK);
			srcK = NULL;
		}
	}

	return retVal;
}

int clCopyToDevice(float *srcGrid, float *dstGrid) {
	int retVal = 0;
	cl_int fRet = 0;

	float *srcWPorch = srcGrid - margin;
	float *dstWPorch = dstGrid - margin;

	fRet = clEnqueueWriteBuffer(queue, srcK, CL_TRUE, 0, ((SIZE_X * SIZE_Y * SIZE_Z * N_CELL_ENTRIES) + (2 * margin)) * sizeof(float), srcWPorch, 0, NULL, NULL);
	ASSERT(CL_SUCCESS == fRet, -1);
	fRet = clEnqueueWriteBuffer(queue, dstK, CL_TRUE, 0, ((SIZE_X * SIZE_Y * SIZE_Z * N_CELL_ENTRIES) + (2 * margin)) * sizeof(float), dstWPorch, 0, NULL, NULL);
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

int clSwapBuffers(void) {
	int retVal = 0;
	cl_int fRet = 0;

	fRet = clSetKernelArg(kernel, 0, sizeof(cl_mem), tiktok? &dstK : &srcK);
	ASSERT(CL_SUCCESS == fRet, -1);
	fRet = clSetKernelArg(kernel, 1, sizeof(cl_mem), tiktok? &srcK : &dstK);
	ASSERT(CL_SUCCESS == fRet, -1);

	tiktok = !tiktok;

_err:

	return retVal;
}

int clCopyFromDevice(float *srcGrid, float *dstGrid) {
	int retVal = 0;
	cl_int fRet = 0;

	float *srcWPorch = tiktok? (srcGrid - margin) : (dstGrid - margin);
	float *dstWPorch = tiktok? (dstGrid - margin) : (srcGrid - margin);

	fRet = clEnqueueReadBuffer(queue, srcK, CL_TRUE, 0, ((SIZE_X * SIZE_Y * SIZE_Z * N_CELL_ENTRIES) + (2 * margin)) * sizeof(float), srcWPorch, 0, NULL, NULL);
	ASSERT(CL_SUCCESS == fRet, -1);
	fRet = clEnqueueReadBuffer(queue, dstK, CL_TRUE, 0, ((SIZE_X * SIZE_Y * SIZE_Z * N_CELL_ENTRIES) + (2 * margin)) * sizeof(float), dstWPorch, 0, NULL, NULL);
	ASSERT(CL_SUCCESS == fRet, -1);

_err:

	return retVal;
}

int clReleaseBuffers(void) {
	int retVal = 0;
	cl_int fRet = 0;

	if(dstK) {
		clReleaseMemObject(dstK);
		dstK = NULL;
	}
	if(srcK) {
		clReleaseMemObject(srcK);
		srcK = NULL;
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
