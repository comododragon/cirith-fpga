/***************************************************************************
 *
 *            (C) Copyright 2010 The Board of Trustees of the
 *                        University of Illinois
 *                         All Rights Reserved
 *
 ***************************************************************************/

#include <parboil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

#define UINT8_MAX 255

#define IMG_WIDTH 996
#define IMG_HEIGHT 1040

/******************************************************************************
* Implementation: Reference
* Details:
* This implementations is a scalar, minimally optimized version. The only 
* optimization, which reduces the number of pointer chasing operations is the 
* use of a temporary pointer for each row.
******************************************************************************/

void histo(unsigned int *img, unsigned char *hist) {
  unsigned int i;
  for (i = 0; i < IMG_WIDTH * IMG_HEIGHT; ++i) {
    const unsigned int value = img[i];
    if (hist[value] < UINT8_MAX) {
      ++hist[value];
    }
  }
}

int main(int argc, char* argv[]) {
  // XXX argc and argv are hardcoded since Lina does not pass arguments during trace
  int _argc = 8;
  char *_argv[] = {
    argv[0],
    "-i",
    "img.bin",
    "-o",
    "ref.bmp",
    "--",
    "1",
    "4"
  };

  struct pb_TimerSet timers;
  struct pb_Parameters *parameters;

  printf("Base implementation of histogramming.\n");
  printf("Maintained by Nady Obeid <obeid1@ece.uiuc.edu>\n");

  parameters = pb_ReadParameters(&_argc, _argv);
  if (!parameters)
    return -1;

  if(!parameters->inpFiles[0]){
    fputs("Input file expected\n", stderr);
    return -1;
  }

  int numIterations;
  if (_argc >= 2){
    numIterations = atoi(_argv[1]);
  } else {
    fputs("Expected at least one command line argument\n", stderr);
    return -1;
  }

  if(numIterations != 1) {
    fputs("Error: numIterations is not 1. This won't work on a SDSoC 2018.2 project (out-of-memory bug). Sorry :(\n", stderr);
    return -2;
  }

  pb_InitializeTimerSet(&timers);
  
  char *inputStr = "Input";
  char *outputStr = "Output";
  
  pb_AddSubTimer(&timers, inputStr, pb_TimerID_IO);
  pb_AddSubTimer(&timers, outputStr, pb_TimerID_IO);
  
  pb_SwitchToSubTimer(&timers, inputStr, pb_TimerID_IO);  

  unsigned int img_width, img_height;
  unsigned int histo_width, histo_height;

  FILE* f = fopen(parameters->inpFiles[0],"rb");
  int result = 0;

  result += fread(&img_width,    sizeof(unsigned int), 1, f);
  result += fread(&img_height,   sizeof(unsigned int), 1, f);
  result += fread(&histo_width,  sizeof(unsigned int), 1, f);
  result += fread(&histo_height, sizeof(unsigned int), 1, f);

  if (result != 4){
    fputs("Error reading input and output dimensions from file\n", stderr);
    return -1;
  }
  if(img_width != IMG_WIDTH) {
    fprintf(stderr, "Error: img_width should be %d (hardcoded for Lina DSE)\n", IMG_WIDTH);
    return -3;
  }
  if(img_height != IMG_HEIGHT) {
    fprintf(stderr, "Error: img_height should be %d (hardcoded for Lina DSE)\n", IMG_HEIGHT);
    return -3;
  }

  unsigned int* img = (unsigned int*) malloc (img_width*img_height*sizeof(unsigned int));
  unsigned char* hist = (unsigned char*) calloc (histo_width*histo_height, sizeof(unsigned char));
  
  pb_SwitchToSubTimer(&timers, "Input", pb_TimerID_IO);

  result = fread(img, sizeof(unsigned int), img_width*img_height, f);

  fclose(f);

  if (result != img_width*img_height){
    fputs("Error reading input array from file\n", stderr);
    return -1;
  }

  pb_SwitchToTimer(&timers, pb_TimerID_COMPUTE);

  int iter;
  for (iter = 0; iter < numIterations; iter++){
    memset(hist,0,histo_height*histo_width*sizeof(unsigned char));
    histo(img, hist);
  }

//  pb_SwitchToTimer(&timers, pb_TimerID_IO);
  pb_SwitchToSubTimer(&timers, outputStr, pb_TimerID_IO);

  if (parameters->outFile) {
    dump_histo_img(hist, histo_height, histo_width, parameters->outFile);
  }

  pb_SwitchToTimer(&timers, pb_TimerID_COMPUTE);

  free(img);
  free(hist);

  pb_SwitchToTimer(&timers, pb_TimerID_NONE);

  printf("\n");
  pb_PrintTimerSet(&timers);
  pb_FreeParameters(parameters);

  return 0;
}
