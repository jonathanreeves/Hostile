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


#ifndef __VECTOR_H__
#define __VECTOR_H__

#ifndef M_2PI 
#define M_2PI 6.283185307179586476925286766
#endif

#ifndef M_3PI_2 
#define M_3PI_2 4.71238898038
#endif

#ifndef M_PI_180 
#define M_PI_180 0.01745329252
#endif

#ifndef M_180_PI
#define M_180_PI 57.2957795131
#endif

typedef struct {
  float x;
  float y;
  float z;
} vector_t;

vector_t *VectorAlloc(void);

void VectorClear(vector_t *arg);

void VectorGetXAxis(vector_t *retvec);

void VectorGetYAxis(vector_t *retvec);

void VectorGetZAxis(vector_t *retvec);

void VectorDealloc(vector_t *v);

void VectorSet(vector_t *arg, float x, float y, float z);

void VectorAdd(vector_t *outvec, vector_t *arg1, vector_t *arg2);

void VectorSubtract(vector_t *outvec, vector_t *arg1, vector_t *arg2);

void VectorScale(vector_t *outvec, vector_t *arg1, float amt);

void VectorCross(vector_t *outvec, vector_t *arg1, vector_t *arg2);

void VectorNormal(vector_t *out, vector_t *arg1, 
				vector_t *arg2, vector_t *arg3);

float VectorMagnitude(vector_t *arg);

float VectorMagnitude2(vector_t *arg);

void VectorSetMagnitude(vector_t *arg, float newMag);

void VectorNormalize(vector_t *arg);

float VectorDot(vector_t *arg1, vector_t *arg2);

void VectorRotate(vector_t *arg, vector_t *axis, float ang);

void VectorRotateX(vector_t *arg, float ang);

void VectorRotateY(vector_t *arg, float ang);

void VectorRotateZ(vector_t *arg, float ang);

/* try to avoid this function as much as possible, especially since it
 * doesn't work  */
float VectorGetYaw(vector_t *arg);

#endif

