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

#ifndef __SKYBOX_H__
#define __SKYBOX_H__

#include "vector.h"

enum SKYTEXLIST {
  SKY_BACK,
  SKY_FRONT,
  SKY_BOTTOM,
  SKY_TOP,
  SKY_LEFT,
  SKY_RIGHT
};

typedef struct {
  vector_t front[4];
  vector_t back[4];
  vector_t bottom[4];
  vector_t top[4];
  vector_t left[4];
  vector_t right[4];
  int texID[6]; /* all six textures */
} skybox_t;

skybox_t *SkyboxLoad(float width, float length, float height, 
				  float centerX, float centerZ);

void SkyboxDestroy(skybox_t *skybox);

void SkyboxDraw(skybox_t *box);

#endif
