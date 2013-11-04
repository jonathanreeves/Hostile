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

#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include "vector.h"
#include "frustum.h"
#include "octree.h"

#define MAP_SCALE_Y 0.08
#define MAP_TEX_SCALE 20.0
#define WATER_TEX_SCALE 50.0

typedef struct {
  int sizeX;
  int sizeZ;
  int texID;
  vector_t *map;
  octree_t *octree;
} terrain_t;

terrain_t *TerrainLoadMap(const char *mapFile, const char *texFile);

terrain_t *TerrainMakeWater(int sizeX, int sizeZ, 
				float level, const char *texFile);

void TerrainDestroy(terrain_t *terr);

float TerrainGetHeight(terrain_t *terr, float x, float z);

void TerrainDraw(terrain_t *terr);

void TerrainDrawCulled(terrain_t *terr, frustum_t *frust);

void TerrainDrawShaded(terrain_t *terr);

void TerrainDrawShadedCulled(terrain_t *terr, frustum_t *frust);

#endif
