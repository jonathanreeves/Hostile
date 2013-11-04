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

#include "terrain.h"
#include "texture.h"
#include "ppm.h"

terrain_t *TerrainLoadMap(const char *mapFile, const char *texFile){
 
  int i, j;
  int sizeX, sizeZ;  
  int mapTexID;
  texture_t tempTex;
  terrain_t *terr;
  PPMImage *heightMap;
  texCoord_t *texCoords;
  
  /* first step is allocation */
  terr = (terrain_t *)malloc(sizeof(terrain_t)); 

  if(terr == NULL){
    fprintf(stderr, "error allocating terrain memory\n");
	return NULL;
  }
  if((heightMap = LoadPPMFile(mapFile)) == NULL){
    fprintf(stderr, "error loading file: %s\n", mapFile);
	free(terr);
	return NULL;
  }
  sizeX = heightMap->sizeX;
  sizeZ = heightMap->sizeY;
  terr->sizeX = sizeX;
  terr->sizeZ = sizeZ;

  /* process the PPM data into vertices also calculate the texture coordinates
   since they will go into the octree for drawing */
  terr->map = (vector_t *)malloc(sizeX*sizeZ*sizeof(vector_t));
  if(terr->map == NULL){
    fprintf(stderr, "error allocating map memory\n");
	DestroyPPMFile(heightMap);
	free(terr);
	return NULL;
  }
  texCoords = (texCoord_t *)malloc(sizeX*sizeZ*sizeof(texCoord_t));
  if(texCoords == NULL){
    fprintf(stderr, "error allocating texture coord memory\n");
	DestroyPPMFile(heightMap);
	free(terr->map);
	free(terr);
	return NULL;
  }

  for(i = 0; i < sizeZ; i++){
    for(j = 0; j < sizeX; j++){
	  terr->map[j + i*sizeX].x = (float)j;
	  terr->map[j + i*sizeX].z = (float)i;
	  terr->map[j + i*sizeX].y =
			 ((float)*(heightMap->data + 3*(j + i*sizeX)))*MAP_SCALE_Y;

	  texCoords[j + i*sizeX].s = 
			 ((float)j)*MAP_TEX_SCALE/((float)sizeX - 1.0);
	  texCoords[j + i*sizeX].t = 
			 ((float)i)*MAP_TEX_SCALE/((float)sizeZ - 1.0);
    
    }
  }

  /* get ready for octree action...OctreeAlloc also intializes */
  terr->octree = OctreeAlloc();
  OctreeLoadHeight(terr->octree, terr->map, texCoords, sizeX, sizeZ);

  /* we're done with these */
  DestroyPPMFile(heightMap);
  free(texCoords);

  /* alright so assuming that went well, we should be able to set our 
   terrain texture now */
  mapTexID = TextureLoadGround(&tempTex, texFile);
  terr->texID = mapTexID;

  /* set the texture to the octree */
  OctreeSetNodeTexture(terr->octree, mapTexID);

  return(terr);
}



terrain_t *TerrainMakeWater(int sizeX, int sizeZ, 
				float level, const char *texFile){
  int i, j;
  int mapTexID;
  texture_t tempTex;
  terrain_t *terr;
  texCoord_t *texCoords;
  
  /* first step is allocation */
  terr = (terrain_t *)malloc(sizeof(terrain_t)); 

  if(terr == NULL){
    fprintf(stderr, "error allocating terrain memory\n");
	return NULL;
  }
  
  terr->sizeX = sizeX;
  terr->sizeZ = sizeZ;

  /* process the PPM data into vertices also calculate the texture coordinates
   since they will go into the octree for drawing */
  terr->map = (vector_t *)malloc(sizeX*sizeZ*sizeof(vector_t));
  if(terr->map == NULL){
    fprintf(stderr, "error allocating map memory\n");
	free(terr);
	return NULL;
  }
  texCoords = (texCoord_t *)malloc(sizeX*sizeZ*sizeof(texCoord_t));
  if(texCoords == NULL){
    fprintf(stderr, "error allocating texture coord memory\n");
	free(terr->map);
	free(terr);
	return NULL;
  }
  
  for(i = 0; i < sizeZ; i++){
    for(j = 0; j < sizeX; j++){
	  terr->map[j + i*sizeX].x = (float)j;
      terr->map[j + i*sizeX].z = (float)i;
	  terr->map[j + i*sizeX].y = level;

	  texCoords[j + i*sizeX].s = 
			  ((float)j)*WATER_TEX_SCALE/((float)sizeX - 1.0);
	  texCoords[j + i*sizeX].t = 
			  ((float)i)*WATER_TEX_SCALE/((float)sizeZ - 1.0);
	}
  }

  /* get ready for octree action...OctreeAlloc also intializes */
  terr->octree = OctreeAlloc();
  OctreeLoadHeight(terr->octree, terr->map, texCoords, sizeX, sizeZ);

  /* we're done with these */
  free(texCoords);

  /* alright so assuming that went well, we should be able to set our 
   terrain texture now */
  mapTexID = TextureLoadWater(&tempTex, texFile);
  terr->texID = mapTexID;

  /* set the texture to the octree */
  OctreeSetNodeTexture(terr->octree, mapTexID);

  return(terr);
}



/* trying to have good memory management */
void TerrainDestroy(terrain_t *terr){
  if(terr != NULL){
    if(terr->octree != NULL){
      OctreeCleanup(terr->octree);
    }
    if(terr->map != NULL){
      free(terr->map);
	}
	free(terr);
  }
}

/* important function for characters to stay attached to the ground */
float TerrainGetHeight(terrain_t *terr, float x, float z){
  int sizeX;
  sizeX = terr->sizeX;
  return (terr->map + (int)(z + 0.5)*sizeX + (int)(x + 0.5))->y;
}

/* unnecessary overhead...just for API consistency */
void TerrainDraw(terrain_t *terr){
  OctreeDraw(terr->octree);  
}

void TerrainDrawCulled(terrain_t *terr, frustum_t *frust){
  OctreeDrawCulled(terr->octree, frust);  
}

void TerrainDrawShaded(terrain_t *terr){
  OctreeDrawHeight(terr->octree);  
}

void TerrainDrawShadedCulled(terrain_t *terr, frustum_t *frust){
  OctreeDrawHeightCulled(terr->octree, frust);  
}

