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

extern float maxMapY;

/* includes vertex coloring for height maps */
void OctreeDrawHeight(octree_t *node){

  int i;
  int nTriangles;
  vector_t *ptr;
  texCoord_t *tptr;
  float color;

  /* just in case... */
  if(!node){
    return;
  }
  if(node->nodeDivided){
    OctreeDrawHeight(node->nextNodes[TOP_LEFT_FRONT]);
	OctreeDrawHeight(node->nextNodes[TOP_LEFT_BACK]);
	OctreeDrawHeight(node->nextNodes[TOP_RIGHT_BACK]);
	OctreeDrawHeight(node->nextNodes[TOP_RIGHT_FRONT]);
	OctreeDrawHeight(node->nextNodes[BOTTOM_LEFT_FRONT]);
	OctreeDrawHeight(node->nextNodes[BOTTOM_LEFT_BACK]);
	OctreeDrawHeight(node->nextNodes[BOTTOM_RIGHT_BACK]);
	OctreeDrawHeight(node->nextNodes[BOTTOM_RIGHT_FRONT]);
  }

  else{
	if(!node->vertices){
	  return;
	}

    nTriangles = node->numTriangles;
	ptr = node->vertices;
	tptr = node->texCoords;

	glBindTexture(GL_TEXTURE_2D, node->texID);
	glBegin(GL_TRIANGLES);
	for(i = 0; i < nTriangles*3; i += 3){
	 
	  color = ptr[i].y/maxMapY;
	  glColor4f(color, color, color, 1.0);
	  glTexCoord3fv((float *)(tptr + i));
	  glVertex3fv((float *)(ptr + i));

	  color = ptr[i + 1].y/maxMapY;
	  glColor4f(color, color, color, 1.0);
	  glTexCoord3fv((float *)(tptr + i + 1));
	  glVertex3fv((float *)(ptr + i + 1));

	  color = ptr[i + 2].y/maxMapY;
	  glColor4f(color, color, color, 1.0);
	  glTexCoord3fv((float *)(tptr + i + 2));
	  glVertex3fv((float *)(ptr + i + 2));
	}
	glEnd();
  }

  return;
}

/* do the same thing but with frustum culling enabled */
void OctreeDrawHeightCulled(octree_t *node, frustum_t *frust){

  int i;
  int nTriangles;
  vector_t *ptr;
  texCoord_t *tptr;
  float color;

  /* just in case... */
  if(!node){
    return;
  }
  else if(FrustumCheckCube(frust, &node->nodeCenter, node->nodeWidthX/2.0)){
    if(node->nodeDivided){
      OctreeDrawHeightCulled(node->nextNodes[TOP_LEFT_FRONT], frust);
	  OctreeDrawHeightCulled(node->nextNodes[TOP_LEFT_BACK], frust);
	  OctreeDrawHeightCulled(node->nextNodes[TOP_RIGHT_BACK], frust);
	  OctreeDrawHeightCulled(node->nextNodes[TOP_RIGHT_FRONT], frust);
	  OctreeDrawHeightCulled(node->nextNodes[BOTTOM_LEFT_FRONT], frust);
	  OctreeDrawHeightCulled(node->nextNodes[BOTTOM_LEFT_BACK], frust);
	  OctreeDrawHeightCulled(node->nextNodes[BOTTOM_RIGHT_BACK], frust);
	  OctreeDrawHeightCulled(node->nextNodes[BOTTOM_RIGHT_FRONT], frust);
    }

    else{
	  if(!node->vertices){
	    return;
	  }

      nTriangles = node->numTriangles;
	  ptr = node->vertices;
	  tptr = node->texCoords;

	  glBindTexture(GL_TEXTURE_2D, node->texID);
	  glBegin(GL_TRIANGLES);
	  for(i = 0; i < nTriangles*3; i += 3){
	 
	    color = ptr[i].y/maxMapY;
	    glColor4f(color, color, color, 1.0);
	    glTexCoord3fv((float *)(tptr + i));
	    glVertex3fv((float *)(ptr + i));

	    color = ptr[i + 1].y/maxMapY;
	    glColor4f(color, color, color, 1.0);
	    glTexCoord3fv((float *)(tptr + i + 1));
	    glVertex3fv((float *)(ptr + i + 1));

	    color = ptr[i + 2].y/maxMapY;
	    glColor4f(color, color, color, 1.0);
	    glTexCoord3fv((float *)(tptr + i + 2));
	    glVertex3fv((float *)(ptr + i + 2));
	  }
	  glEnd();
    }
  }

  return;
}

/* no coloring of vertices */
void OctreeDraw(octree_t *node){

  int i;
  int nTriangles;
  vector_t *ptr;
  texCoord_t *tptr;

  /* just in case... */
  if(!node){
    return;
  }
  if(node->nodeDivided){
    OctreeDraw(node->nextNodes[TOP_LEFT_FRONT]);
	OctreeDraw(node->nextNodes[TOP_LEFT_BACK]);
	OctreeDraw(node->nextNodes[TOP_RIGHT_BACK]);
	OctreeDraw(node->nextNodes[TOP_RIGHT_FRONT]);
	OctreeDraw(node->nextNodes[BOTTOM_LEFT_FRONT]);
	OctreeDraw(node->nextNodes[BOTTOM_LEFT_BACK]);
	OctreeDraw(node->nextNodes[BOTTOM_RIGHT_BACK]);
	OctreeDraw(node->nextNodes[BOTTOM_RIGHT_FRONT]);

  }

  else{
	if(!node->vertices){
	  return;
	}

    nTriangles = node->numTriangles;
	ptr = node->vertices;
	tptr = node->texCoords;

	glBindTexture(GL_TEXTURE_2D, node->texID);
	glBegin(GL_TRIANGLES);
	for(i = 0; i < nTriangles*3; i += 3){
	 
	  glTexCoord3fv((float *)(tptr + i));
	  glVertex3fv((float *)(ptr + i));

	  glTexCoord3fv((float *)(tptr + i + 1));
	  glVertex3fv((float *)(ptr + i + 1));

	  glTexCoord3fv((float *)(tptr + i + 2));
	  glVertex3fv((float *)(ptr + i + 2));
	}
	glEnd();
  }

  return;
}

/* no coloring but with culling enabled */
void OctreeDrawCulled(octree_t *node, frustum_t *frust){

  int i;
  int nTriangles;
  vector_t *ptr;
  texCoord_t *tptr;

  /* just in case... */
  if(!node){
    return;
  }

  else if(FrustumCheckCube(frust, &node->nodeCenter, node->nodeWidthX/2.0)){
    if(node->nodeDivided){
      OctreeDrawCulled(node->nextNodes[TOP_LEFT_FRONT], frust);
	  OctreeDrawCulled(node->nextNodes[TOP_LEFT_BACK], frust);
	  OctreeDrawCulled(node->nextNodes[TOP_RIGHT_BACK], frust);
	  OctreeDrawCulled(node->nextNodes[TOP_RIGHT_FRONT], frust);
	  OctreeDrawCulled(node->nextNodes[BOTTOM_LEFT_FRONT], frust);
	  OctreeDrawCulled(node->nextNodes[BOTTOM_LEFT_BACK], frust);
	  OctreeDrawCulled(node->nextNodes[BOTTOM_RIGHT_BACK], frust);
	  OctreeDrawCulled(node->nextNodes[BOTTOM_RIGHT_FRONT], frust);
    }

    else{
	  if(!node->vertices){
	    return;
	  }

      nTriangles = node->numTriangles;
	  ptr = node->vertices;
	  tptr = node->texCoords;

	  glBindTexture(GL_TEXTURE_2D, node->texID);
	  glBegin(GL_TRIANGLES);
	  for(i = 0; i < nTriangles*3; i += 3){
	 
	    glTexCoord3fv((float *)(tptr + i));
	    glVertex3fv((float *)(ptr + i));

	    glTexCoord3fv((float *)(tptr + i + 1));
	    glVertex3fv((float *)(ptr + i + 1));

	    glTexCoord3fv((float *)(tptr + i + 2));
	    glVertex3fv((float *)(ptr + i + 2));
	  }
	  glEnd();
    }
  }

  return;
}
