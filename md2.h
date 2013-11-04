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

#ifndef __MD2_H__
#define __MD2_H__

#include <GL/gl.h>
#include <GL/glu.h>

#include "vector.h"
#include "texture.h"

#define MAX_FRAMES 512


/* texture coordinate index */
typedef struct {
   short s;
   short t;
} texCoordIndex_t;


/* info for a single frame point */
typedef struct {
   unsigned char v[3];
   unsigned char normalIndex;
} framePoint_t;


/* information for a single frame */
typedef struct {
   float scale[3];
   float translate[3];
   char name[16];
   framePoint_t fp[1];
} frame_t;


/* data for a single triangle */
typedef struct {
   unsigned short meshIndex[3];		/* vertex indices */
   unsigned short texCoordIndex[3];		/* texture coordinate indices */
} mesh_t;


typedef struct {
   int ident;		
   int version;		 /* should be 8 */
   int skinWidth;    /* width of texture */
   int skinHeight;   /* height of texture */
   int frameSize;    /* bytes per frame */
   int numSkins;     /* number of textures */
   int numPoints;    /* number of points */
   int numST;        /* number of textures */
   int numTris;      /* number of triangles */
   int numGLcmds;	 /* number of OpenGL command types */
   int numFrames;    /* total number of frames */
   int offsetSkins;  /* offset to skin names (64 bytes each) */
   int offsetST;     /* offset to texture u-v values */
   int offsetTris;   /* offset to triangle mesh */
   int offsetFrames; /* offset to frame (vertices) */
   int offsetGLcmds; /* type of OpenGL commands to use */
   int offsetEnd;    /* EOF */
} MD2Header_t;

/* here's the gold */
typedef struct {
     int numFrames;			/* number of model frames */
     int numVertices;         /* number of vertices */
     int numTriangles;        /* number of triangles */
     int numST;               /* number of skins */
     int frameSize;           /* size of each frame in bytes */
     unsigned int texID;     /* openGL texture ID number */
	 mesh_t *triIndex;        /* triangle list */
     texCoord_t *texCoordList;  /* texture coordinate list */
     vector_t *vertexList;    /* vertex list */
	 vector_t *normalList; /* normals for every vertex */
	 unsigned int usageCount; /* how many people are using this model */
} MD2Model_t;


MD2Model_t *MD2ModelAlloc(void);

void MD2ModelInit(MD2Model_t *model); 

/* load model and skin/texture at the same time */
int MD2ModelLoad(MD2Model_t *model, const char *md2File, const char *skinFile);

/* render model with interpolation to get animation */
int MD2ModelAnimate(MD2Model_t *model, int currentFrame, int nextFrame, float interpol);

/* draw a single frame */
int MD2ModelDrawFrame(MD2Model_t *model, int keyFrame);

/* free memory including the model itself, watch the usage count */
int MD2ModelDestroy(MD2Model_t *model);

/* free memory not including the model itself, watch the usage count */
int MD2ModelCleanup(MD2Model_t *model);

#endif
