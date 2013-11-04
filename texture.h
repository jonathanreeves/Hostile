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


#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#define TEXMAX 10;

/* texture coordinate -- used in many octree generation as well */
typedef struct {
   float s;
   float t;
} texCoord_t;

typedef struct {
  int texID;
  int sizeX;
  int sizeY;
} texture_t;

int TextureLoadGround(texture_t *tex, const char *filename);
int TextureLoadWater(texture_t *tex, const char *filename);
int TextureLoadSkin(texture_t *tex, const char *filename);
int TextureLoadSky(texture_t *tex, const char *filename);
int TextureLoadTree(texture_t *tex, const char *filename);
#endif


