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

#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h> /* for memset */

#include "octree.h"
#include "ppm.h"

float maxMapY = 25.5; /* 255 times MAP_SCALE_Y */
int maxSubdivisions = MAX_SUBDIVISIONS;
int maxTriangles = MAX_TRIANGLES;
int currentSubdivision = 0;
int endNodeCount = 0;


octree_t *OctreeAlloc(void){
  octree_t *oct;
  oct = (octree_t *)malloc(sizeof(octree_t));
  OctreeInit(oct);
  return oct;
}

/* can't just free... need to traverse the tree and get rid of everything */
void OctreeCleanup(octree_t *node){

  if(node != NULL){
    if(node->nodeDivided){
	  OctreeCleanup(node->nextNodes[TOP_LEFT_FRONT]);
	  OctreeCleanup(node->nextNodes[TOP_LEFT_BACK]);
	  OctreeCleanup(node->nextNodes[TOP_RIGHT_BACK]);
	  OctreeCleanup(node->nextNodes[TOP_RIGHT_FRONT]);
	  OctreeCleanup(node->nextNodes[BOTTOM_LEFT_FRONT]);
	  OctreeCleanup(node->nextNodes[BOTTOM_LEFT_BACK]);
	  OctreeCleanup(node->nextNodes[BOTTOM_RIGHT_BACK]);
	  OctreeCleanup(node->nextNodes[BOTTOM_RIGHT_FRONT]);
	}
	else{
	  free(node->vertices);
	  free(node->texCoords);
	  free(node);
	}
  }
}


void OctreeDestroy(octree_t *node){
  OctreeCleanup(node);
  free(node);
}

/* just set stuff to zero */
void OctreeInit(octree_t *baseNode){
  int i;
  baseNode->nodeDivided = 0;
  baseNode->nodeWidthX = 0.0;
  baseNode->nodeWidthZ = 0.0;
  baseNode->nodeHeight = 0.0;
  baseNode->numTriangles = 0;
  baseNode->numVertices = 0;
  memset(&baseNode->nodeCenter, 0, sizeof(vector_t));
  baseNode->vertices = NULL;
  baseNode->texCoords = NULL;
  for(i = 0; i < 8; i++)
    baseNode->nextNodes[i] = NULL;
  return;
}

/* run through each node and give it a texture */
void OctreeSetNodeTexture(octree_t *node, unsigned int texID){

  if(node != NULL){
    if(node->nodeDivided){
      OctreeSetNodeTexture(node->nextNodes[TOP_LEFT_FRONT], texID);
      OctreeSetNodeTexture(node->nextNodes[TOP_LEFT_BACK], texID);
      OctreeSetNodeTexture(node->nextNodes[TOP_RIGHT_FRONT], texID);
      OctreeSetNodeTexture(node->nextNodes[TOP_RIGHT_BACK], texID);
      OctreeSetNodeTexture(node->nextNodes[BOTTOM_LEFT_FRONT], texID);
      OctreeSetNodeTexture(node->nextNodes[BOTTOM_LEFT_BACK], texID);
      OctreeSetNodeTexture(node->nextNodes[BOTTOM_RIGHT_FRONT], texID);
      OctreeSetNodeTexture(node->nextNodes[BOTTOM_RIGHT_BACK], texID);
    }
    else{
      node->texID = texID;
    }
  }
  return;
}


/* load the heightmap stored as individual vertices, make them triangles */
void OctreeLoadHeight(octree_t *baseNode, vector_t *vertices, 
				          texCoord_t *texCoords, int sizeX, int sizeZ){

  int i, j;
  int index;
  int nTriangles;
  vector_t *triVerts;
  vector_t *ptr1, *ptr2;
  texCoord_t *triTexCoords;
  texCoord_t *tptr1, *tptr2;
  
  nTriangles = 2*(sizeX - 1)*(sizeX - 1);

  /* allocate memory for all vertices and texture coordinates */
  triVerts = (vector_t *)malloc(3*nTriangles*sizeof(vector_t));
  triTexCoords = (texCoord_t *)malloc(3*nTriangles*sizeof(texCoord_t));
  
  /* now describe each triangles and its individual texture coordinates
   each four vertices make two triangles */
  index = 0;
  for(i = 0; i < sizeZ - 1; i++){
	ptr1 = vertices + i*sizeX;
	ptr2 = vertices + (i+1)*sizeX;
	tptr1 = texCoords + i*sizeX;
	tptr2 = texCoords + (i+1)*sizeX;
	for(j = 0; j < sizeX - 1; j++){
	  /* triangle 1 */
	  triVerts[index] = *(ptr1 + j);
	  triTexCoords[index++] = *(tptr1 + j);
	  triVerts[index] = *(ptr2 + j);
	  triTexCoords[index++] = *(tptr2 + j);
	  triVerts[index] = *(ptr2 + j + 1);
	  triTexCoords[index++] = *(tptr2 + j + 1);

	  /* triangle 2 */
	  triVerts[index] = *(ptr1 + j);
	  triTexCoords[index++] = *(tptr1 + j);
	  triVerts[index] = *(ptr2 + j + 1);
	  triTexCoords[index++] = *(tptr2 + j + 1);
	  triVerts[index] = *(ptr1 + j + 1);
	  triTexCoords[index++] = *(tptr1 + j + 1);
	}
  }

  /* make a perfect cube */
  baseNode->nodeWidthX = (float)sizeX;
  baseNode->nodeWidthZ = (float)sizeX;
  baseNode->nodeHeight = (float)sizeX;
  baseNode->numTriangles = nTriangles;
  baseNode->numVertices = 3*nTriangles;
  baseNode->nodeDivided = 0;
  baseNode->nodeCenter.x = (baseNode->nodeWidthX-1)/2.0;
  baseNode->nodeCenter.y = (baseNode->nodeWidthX-1)/2.0;
  baseNode->nodeCenter.z = (baseNode->nodeWidthX-1)/2.0;
 
  /* now subdivide */
  OctreeCreateNode(baseNode, triVerts, triTexCoords, baseNode->numVertices); 

  free(triVerts);
  free(triTexCoords);
}



void OctreeGetNewCenter(octree_t *node, vector_t *retvec, int nID){

  vector_t center = node->nodeCenter;
  float width = node->nodeWidthX;

  switch(nID){
    case TOP_LEFT_FRONT:
	  retvec->x = center.x - (width)/4.0;
	  retvec->y = center.y + (width)/4.0;
	  retvec->z = center.z + (width)/4.0;
	  break;
	case TOP_LEFT_BACK:
      retvec->x = center.x - (width)/4.0;
	  retvec->y = center.y + (width)/4.0;
	  retvec->z = center.z - (width)/4.0;
	  break;
	case TOP_RIGHT_BACK:
      retvec->x = center.x + (width)/4.0;
	  retvec->y = center.y + (width)/4.0;
	  retvec->z = center.z - (width)/4.0;
	  break;
	case TOP_RIGHT_FRONT:
      retvec->x = center.x + (width)/4.0;
	  retvec->y = center.y + (width)/4.0;
	  retvec->z = center.z + (width)/4.0;
	  break;
    case BOTTOM_LEFT_FRONT:
	  retvec->x = center.x - (width)/4.0;
	  retvec->y = center.y - (width)/4.0;
	  retvec->z = center.z + (width)/4.0;
	  break;
	case BOTTOM_LEFT_BACK:
      retvec->x = center.x - (width)/4.0;
	  retvec->y = center.y - (width)/4.0;
	  retvec->z = center.z - (width)/4.0;
	  break;
	case BOTTOM_RIGHT_BACK:
      retvec->x = center.x + (width)/4.0;
	  retvec->y = center.y - (width)/4.0;
	  retvec->z = center.z - (width)/4.0;
	  break;
	case BOTTOM_RIGHT_FRONT:
      retvec->x = center.x + (width)/4.0;
	  retvec->y = center.y - (width)/4.0;
	  retvec->z = center.z + (width)/4.0;
	  break;
  }
  return;
}

void OctreeCreateNewNode(octree_t *node, vector_t *vertices, texCoord_t *texCoords, int *pList, int nVertices, int nTriangles, int nodeID){
 
  vector_t *newVertices;
  texCoord_t *newTexCoords;
  vector_t newCenter;
  int index, i;
  
  if(nTriangles) {
    newVertices = (vector_t *)malloc(3*nTriangles*sizeof(vector_t));
    newTexCoords = (texCoord_t *)malloc(3*nTriangles*sizeof(texCoord_t));
    
	index = 0;
    for(i = 0; i < nVertices; i++){
      if(pList[i / 3]){
        newVertices[index] = vertices[i];
		newTexCoords[index] = texCoords[i];
        index++;
      }
    }

	/* allocate new node */
    node->nextNodes[nodeID] = (octree_t *)malloc(sizeof(octree_t));
	OctreeInit(node->nextNodes[nodeID]);

    OctreeGetNewCenter(node, &newCenter, nodeID);
	node->nextNodes[nodeID]->nodeCenter = newCenter;
	node->nextNodes[nodeID]->nodeWidthX = node->nodeWidthX/2.0;
	node->nextNodes[nodeID]->nodeWidthZ = node->nodeWidthZ/2.0;
	node->nextNodes[nodeID]->nodeHeight = node->nodeHeight/2.0;
	node->nextNodes[nodeID]->numTriangles = nTriangles;
	node->nextNodes[nodeID]->numVertices = 3*nTriangles;
    
    currentSubdivision++;

	/* mutually recursive function calls */
	OctreeCreateNode(node->nextNodes[nodeID], newVertices, 
					newTexCoords, 3*nTriangles);
	
	currentSubdivision--;

    free(newVertices);
    free(newTexCoords);
  }

}

void OctreeCreateNode(octree_t *node, vector_t *vertices, texCoord_t *texCoords, int nVertices){
  
  int nTriangles;
  int triCount[8] = {0, 0, 0, 0, 0, 0, 0, 0};  
  int *pList[8];
  vector_t pt;
  vector_t center;
  float width;
  int i;

  center = node->nodeCenter;
  width = node->nodeWidthX;

  nTriangles = nVertices/3;
  
  if((nTriangles > maxTriangles) && 
				  (currentSubdivision < maxSubdivisions)){
   
	/* allocate the pLists */
	for(i = 0; i < 8; i++){
	  pList[i] = (int *)malloc(nTriangles*sizeof(int));
	  memset(pList[i], 0, nTriangles*sizeof(int));
	}
   	
	
	node->nodeDivided = 1;

    for(i = 0; i < nVertices; i++){
      
      pt = vertices[i];
     
	  /* TOP LEFT FRONT */
	  if((pt.x <= center.x) && (pt.y >= center.y) && (pt.z >= center.z) ){ 
		pList[0][i/3] = 1;
	  }
     
	  /* TOP LEFT BACK */
	  if((pt.x <= center.x) && (pt.y >= center.y) && (pt.z <= center.z)){ 
		pList[1][i/3] = 1;
	  }
      
	  /* TOP RIGHT BACK */
	  if((pt.x >= center.x) && (pt.y >= center.y) && (pt.z <= center.z)){ 
		pList[2][i/3] = 1;
	  }

	  /* TOP RIGHT FRONT */
      if((pt.x >= center.x) && (pt.y >= center.y) && (pt.z >= center.z)){
		pList[3][i/3] = 1;
	  }

	  /* BOTTOM LEFT FRONT */
	  if((pt.x <= center.x) && (pt.y <= center.y) && (pt.z >= center.z)){ 
		pList[4][i/3] = 1;
	  }
     
	  /* BOTTOM LEFT BACK */
      if((pt.x <= center.x) && (pt.y <= center.y) && (pt.z <= center.z)){ 
		pList[5][i/3] = 1;
	  }
      
	  /* BOTTOM RIGHT BACK */
	  if((pt.x >= center.x) && (pt.y <= center.y) && (pt.z <= center.z)){ 
		pList[6][i/3] = 1;
	  }

	  /* BOTTOM RIGHT FRONT */
      if((pt.x >= center.x) && (pt.y <= center.y) && (pt.z >= center.z)){
		pList[7][i/3] = 1;
	  }
    } 
    
	for(i = 0; i < nTriangles; i++){
	  if(pList[0][i]) triCount[0]++;
	  if(pList[1][i]) triCount[1]++;
	  if(pList[2][i]) triCount[2]++;
	  if(pList[3][i]) triCount[3]++;
      if(pList[4][i]) triCount[4]++;
	  if(pList[5][i]) triCount[5]++;
	  if(pList[6][i]) triCount[6]++;
	  if(pList[7][i]) triCount[7]++;
	}

    OctreeCreateNewNode(node, vertices, texCoords, pList[0], nVertices, 
					    triCount[0], TOP_LEFT_FRONT);
    OctreeCreateNewNode(node, vertices, texCoords, pList[1], nVertices, 
					    triCount[1], TOP_LEFT_BACK);
    OctreeCreateNewNode(node, vertices, texCoords, pList[2], nVertices, 
					    triCount[2], TOP_RIGHT_BACK);
    OctreeCreateNewNode(node, vertices, texCoords, pList[3], nVertices, 
					    triCount[3], TOP_RIGHT_FRONT);
    OctreeCreateNewNode(node, vertices, texCoords, pList[4], nVertices, 
					    triCount[4], BOTTOM_LEFT_FRONT);
    OctreeCreateNewNode(node, vertices, texCoords, pList[5], nVertices, 
					    triCount[5], BOTTOM_LEFT_BACK);
    OctreeCreateNewNode(node, vertices, texCoords, pList[6], nVertices, 
					    triCount[6], BOTTOM_RIGHT_BACK);
    OctreeCreateNewNode(node, vertices, texCoords, pList[7], nVertices, 
					    triCount[7], BOTTOM_RIGHT_FRONT);


	/* free the pLists */
	for(i = 0; i < 8; i++){
	  free(pList[i]);
	}
  }
  else{
    OctreeAssignVertices(node, vertices, texCoords, nVertices);
  }
}

/* this is when you're done descending and ready to actually attach some
 * real vertices */
void OctreeAssignVertices(octree_t *node, vector_t *vertices, texCoord_t *texCoords, int nVertices){

  node->nodeDivided = 0;
  node->numTriangles = nVertices/3;
  node->numVertices = nVertices;
  node->vertices = (vector_t *)malloc(nVertices*sizeof(vector_t));
  node->texCoords = (texCoord_t *)malloc(nVertices*sizeof(texCoord_t));
  memset(node->vertices, 0, sizeof(vector_t) * nVertices);
  memset(node->texCoords, 0, sizeof(texCoord_t) * nVertices);
  memcpy(node->vertices, vertices, sizeof(vector_t) * nVertices);
  memcpy(node->texCoords, texCoords, sizeof(texCoord_t) * nVertices);
  endNodeCount++;
}


vector_t *OctreeGetCenter(octree_t *node){
  return &node->nodeCenter;
}

float OctreeGetWidth(octree_t *node){
  return node->nodeWidthX;
}
		
unsigned char OctreeIsSubDivided(octree_t *node){
  return node->nodeDivided;
}

/* for debugging */
void printNode(octree_t *node){
  int i;
  int sizeX, sizeZ;

  sizeX = (int)node->nodeWidthX;
  sizeZ = (int)node->nodeWidthZ;

  printf("sizeX = %d\n", sizeX);
  printf("sizeZ = %d\n", sizeZ);

  for(i = 0; i < sizeZ*sizeX; i++){
    printf("(%f, %f, %f)\n", node->vertices[i].x, node->vertices[i].y, node->vertices[i].z);
  }
  printf("\n");
}

