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

#ifndef __OCTREE_H__
#define __OCTREE_H__

#include "vector.h"
#include "texture.h"
#include "frustum.h"

#define MAX_SUBDIVISIONS 4
#define MAX_TRIANGLES 600

enum OctreeNodes {
  TOP_LEFT_FRONT,
  TOP_LEFT_BACK,
  TOP_RIGHT_BACK,
  TOP_RIGHT_FRONT,
  BOTTOM_LEFT_FRONT,
  BOTTOM_LEFT_BACK,
  BOTTOM_RIGHT_BACK,
  BOTTOM_RIGHT_FRONT
};


typedef struct OCTREE {
  unsigned char nodeDivided; /* flag to test if this node is subdivided */
  float nodeWidthX; /* width of this node in X */
  float nodeWidthZ; /* width of this node in Z */
  int nodeHeight; 
  int numTriangles; /* number of triangles in this node */
  int numVertices; /* number of vertices */
  unsigned int texID; /* texture drawn if at all */
  vector_t nodeCenter; /* coordinates of the center of the node */
  vector_t *vertices; /* array of vertices for the triangles */
  texCoord_t *texCoords; /* array of texture coordinates */
  struct OCTREE *nextNodes[8];
} octree_t;

octree_t *OctreeAlloc(void);

void OctreeDestroy(octree_t *node);

void OctreeCleanup(octree_t *node);

vector_t *OctreeGetCenter(octree_t *node);

void OctreeGetNewCenter(octree_t *node, vector_t *retvec, int nID);

void OctreeCreateNode(octree_t *node, vector_t *vertices, texCoord_t *texCoords, int nVertices);

void OctreeCreateNewNode(octree_t *node, vector_t *vertices, texCoord_t *texCoords, int *pList, int nVertices, int nTriangles, int nodeID);

void OctreeAssignVertices(octree_t *node, vector_t *vertices, texCoord_t *texCoords, int nVertices);

float OctreeGetWidth(octree_t *node);

int OctreeTriangleCount(octree_t *node);

int OctreeStripCount(octree_t *node);

unsigned char OctreeIsSubDivided(octree_t *node);

void OctreeGetDimensions(octree_t *node);

void OctreeDraw(octree_t *node);

void OctreeDrawCulled(octree_t *node, frustum_t *frust);

/* includes vertex coloring for height effects */
void OctreeDrawHeight(octree_t *node);

void OctreeDrawHeightCulled(octree_t *node, frustum_t *frust);

void OctreeInit(octree_t *baseNode);

/* not sure if this should even be in here...maybe in terrain.c? */
void OctreeLoadHeight(octree_t *baseNode, vector_t *vertices, 
				          texCoord_t *texCoords, int sizeX, int sizeZ);

void OctreeSetNodeTexture(octree_t *node, unsigned int texID);

void printNode(octree_t *baseNode);

#endif
