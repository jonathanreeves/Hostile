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

#ifndef __FRUSTUM_H__
#define __FRUSTUM_H__

#include "vector.h"

/* note that there are six vectors describing the frustum, each with
 * four elements (D component gives offset of the plane) */
typedef struct{
  float projection[16]; /* the raw projection matrix which rarely changes */
  float pView[6][4]; /* the perspective view which changes every cycle */
} frustum_t;

enum FrustumSide{
  RIGHT, 
  LEFT, 
  BOTTOM,
  TOP,  
  BACK,  
  FRONT   
}; 

enum PlaneData{
  A,
  B,
  C,
  D 
};

void FrustumUpdateView(frustum_t *frust);

void FrustumUpdateProjection(frustum_t *frust);

int FrustumCheckPoint(frustum_t *frust, vector_t *point);

int FrustumCheckSphere(frustum_t *frust, vector_t *point, float radius);

int FrustumCheckCube(frustum_t *frust, vector_t *point, float size );

void MatrixMult(float *ret, float *arg1, float *arg2, int M, int N);
#endif
