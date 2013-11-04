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


#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "texture.h"
#include "ppm.h"

int TextureLoadGround(texture_t *tex, const char *filename){

  unsigned int tempTex;
  PPMImage *rawTex;
  int sizeX, sizeY;

  rawTex = LoadPPMFile(filename);
  if(rawTex == NULL)
    return -1;
  sizeX = rawTex->sizeX;
  sizeY = rawTex->sizeY;
 
  glGenTextures(1, &tempTex);
  glBindTexture(GL_TEXTURE_2D, tempTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, sizeX, sizeY, GL_RGB,
				  GL_UNSIGNED_BYTE, rawTex->data);

  tex->sizeX = sizeX;
  tex->sizeY = sizeY;
  tex->texID = tempTex;

  DestroyPPMFile(rawTex);
  return tempTex;
	
}


int TextureLoadWater(texture_t *tex, const char *filename){
		
  unsigned int tempTex;
  PPMImage *rawTex;
  int sizeX, sizeY;

  rawTex = LoadPPMFile(filename);
  if(rawTex == NULL)
    return -1;
  sizeX = rawTex->sizeX;
  sizeY = rawTex->sizeY;
  
  glGenTextures(1, &tempTex);
  glBindTexture(GL_TEXTURE_2D, tempTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, sizeX, sizeY, GL_RGB,
				  GL_UNSIGNED_BYTE, rawTex->data);
  
  tex->sizeX = sizeX;
  tex->sizeY = sizeY;
  tex->texID = tempTex;


  DestroyPPMFile(rawTex);
  return tempTex;
}


int TextureLoadSkin(texture_t *tex, const char *filename){
  
  unsigned int tempTex;
  PPMImage *rawTex;
  int sizeX, sizeY;

  rawTex = LoadPPMFile(filename);
  if(rawTex == NULL)
    return -1;
  sizeX = rawTex->sizeX;
  sizeY = rawTex->sizeY;
  
  glGenTextures(1, &tempTex);
  glBindTexture(GL_TEXTURE_2D, tempTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, sizeX, sizeY, GL_RGB,
				  GL_UNSIGNED_BYTE, rawTex->data);
 
  tex->sizeX = sizeX;
  tex->sizeY = sizeY;
  tex->texID = tempTex;

  DestroyPPMFile(rawTex);  
  return tempTex;
}

int TextureLoadSky(texture_t *tex, const char *filename){
  
  unsigned int tempTex;
  PPMImage *rawTex;
  int sizeX, sizeY;

  rawTex = LoadPPMFile(filename);
  if(rawTex == NULL)
    return -1;
  sizeX = rawTex->sizeX;
  sizeY = rawTex->sizeY;
  
  glGenTextures(1, &tempTex);
  glBindTexture(GL_TEXTURE_2D, tempTex);
    
  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, sizeX, sizeY, GL_RGB, 
				  GL_UNSIGNED_BYTE, rawTex->data);

  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);    
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  tex->sizeX = sizeX;
  tex->sizeY = sizeY;
  tex->texID = tempTex;

  DestroyPPMFile(rawTex);  
  return tempTex;

}


int TextureLoadTree(texture_t *tex, const char *filename){
  
  unsigned int tempTex;
  PPMImage *rawTex;
  int sizeX, sizeY;
  
  rawTex = LoadPPMFile(filename);
  if(rawTex == NULL)
    return -1;
  sizeX = rawTex->sizeX;
  sizeY = rawTex->sizeY;

  glGenTextures(1, &tempTex);
  glBindTexture(GL_TEXTURE_2D, tempTex);
    
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, sizeX, sizeY, GL_RGBA, 
				  GL_UNSIGNED_BYTE, rawTex->data);

  tex->sizeX = sizeX;
  tex->sizeY = sizeY;
  tex->texID = tempTex;

  DestroyPPMFile(rawTex);  
  return tempTex;
}
