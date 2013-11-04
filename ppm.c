/* 
 * Hostile Takeover: 
 * CS248 Final Project, Fall 2004
 * 
 * Written by Jonathan Reeves
 * jrreeves@stanford.edu
 *
 * A simple 3D video game demonstrating the use of OpenGL for graphics 
 * rendering with a number of advanced techniques and optimizations to make
 * it interesting. See the README file which came with this package for more
 * details. It also includes a list of sources which were consulted while
 * building this software package.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include "ppm.h"

PPMImage *LoadPPMFile(const char *Filename) {

  FILE *fp; 
  int Data, imgSize;
  int err;
  PPMImage *image;
 
  fp = fopen(Filename,"r");
  if (!fp){
    return NULL;
  }

  /* Read magic number. */
  fscanf(fp, "P6\n");

  /* Comment line(s). */
  while ((Data=getc(fp))=='#'){
    fscanf(fp,"%*[^\n]\n");
  }

  ungetc(Data, fp);

  image = (PPMImage *)malloc(sizeof(PPMImage));

  /* Image dimensions. */
  if (fscanf(fp, "%d %d", &image->sizeX , &image->sizeY)!=2 ||
      image->sizeX == 0 || image->sizeY == 0){
    return NULL;
  }

  imgSize = image->sizeX*image->sizeY;
  image->data = (unsigned char *)malloc(imgSize*3);
  
  /* Maximum value in image. */
  fscanf(fp, "%*d%*c");


  /* Load image. */
  err = fread(image->data, 3, imgSize, fp);
  if(err != imgSize){
    free(image->data);
	free(image);
	return NULL;
  }
  
  fclose(fp);
  return image;
}


void DestroyPPMFile(PPMImage *img){
  free(img->data);
  free(img);
}
