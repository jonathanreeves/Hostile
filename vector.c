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


#include "vector.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

vector_t XAXIS = {1.0, 0.0, 0.0};
vector_t YAXIS = {0.0, 1.0, 0.0};
vector_t ZAXIS = {0.0, 0.0, 1.0};

vector_t *VectorAlloc(void){
  vector_t *v;
  v = (vector_t *)malloc(sizeof(vector_t));
  return v;
}

void VectorDealloc(vector_t *v){
  free(v);
}

void VectorClear(vector_t *arg){
  memset(arg, 0, sizeof(vector_t));
}

void VectorGetXAxis(vector_t *retvec){
  memcpy((void *)retvec, (void *)(&XAXIS), sizeof(vector_t));
}

void VectorGetYAxis(vector_t *retvec){
  memcpy((void *)retvec, (void *)(&YAXIS), sizeof(vector_t));
}

void VectorGetZAxis(vector_t *retvec){
  memcpy((void *)retvec, (void *)(&ZAXIS), sizeof(vector_t));
}

void VectorSet(vector_t *arg, float x, float y, float z){
  arg->x = x;
  arg->y = y;
  arg->z = z;
}

void VectorAdd(vector_t *outvec, vector_t *arg1, vector_t *arg2){
  outvec->x = arg1->x + arg2->x;
  outvec->y = arg1->y + arg2->y;
  outvec->z = arg1->z + arg2->z;
}

void VectorSubtract(vector_t *outvec, vector_t *arg1, vector_t *arg2){
  outvec->x = arg1->x - arg2->x;
  outvec->y = arg1->y - arg2->y;
  outvec->z = arg1->z - arg2->z;
}

void VectorScale(vector_t *outvec, vector_t *arg1, float amt){
  outvec->x = amt*arg1->x;
  outvec->y = amt*arg1->y;
  outvec->z = amt*arg1->z;
}

void VectorCross(vector_t *outvec, vector_t *arg1, vector_t *arg2){
  outvec->x = (arg1->y * arg2->z) - (arg1->z * arg2->y);
  outvec->y = (arg1->z * arg2->x) - (arg1->x * arg2->z);
  outvec->z = (arg1->x * arg2->y) - (arg1->y * arg2->x);
}

float VectorMagnitude(vector_t *arg){
  return sqrt((arg->x*arg->x) + (arg->y*arg->y) + (arg->z*arg->z));
}

float VectorMagnitude2(vector_t *arg){
  return ((arg->x*arg->x) + (arg->y*arg->y) + (arg->z*arg->z));
}

void VectorSetMagnitude(vector_t *arg, float newMag){
  VectorNormalize(arg);
  VectorScale(arg, arg, newMag);
}

void VectorNormalize(vector_t *arg){
  float mag = VectorMagnitude(arg);
  arg->x = arg->x/mag;
  arg->y = arg->y/mag;
  arg->z = arg->z/mag;
}

float VectorDot(vector_t *arg1, vector_t *arg2){
  return (arg1->x * arg2->x) + (arg1->y * arg2->y) + (arg1->z * arg2->z);
}


void VectorNormal(vector_t *out, vector_t *arg1, 
				    vector_t *arg2, vector_t *arg3){
  vector_t v1minusv2;
  vector_t v2minusv3;

  VectorSubtract(&v1minusv2, arg1, arg2);
  VectorSubtract(&v2minusv3, arg2, arg3);
  
  VectorCross(out, &v1minusv2, &v2minusv3);
  VectorNormalize(out);
}

/* rotate around an axis */
void VectorRotate(vector_t *arg, vector_t *axis, float ang){

  static float cosTheta;
  static float sinTheta;
  vector_t temp;

  /* do the rotation in degrees */
  cosTheta = (float)cos(ang*M_PI/180.0);
  sinTheta = (float)sin(ang*M_PI/180.0);

  /* here we are going to rotate around the axis specified by x, y, z */
  temp.x = (cosTheta + (1 - cosTheta) * axis->x * axis->x) * arg->x;
  temp.x += ((1 - cosTheta) * axis->x * axis->y - axis->z * sinTheta) * arg->y;
  temp.x += ((1 - cosTheta) * axis->x * axis->x + axis->y * sinTheta) * arg->z;
  
  temp.y = ((1 - cosTheta) * axis->x * axis->y + axis->z * sinTheta) * arg->x;
  temp.y += (cosTheta + (1 - cosTheta) * axis->y * axis->y) * arg->y;
  temp.y += ((1 - cosTheta) * axis->y * axis->z - axis->x * sinTheta) * arg->z;
 
  temp.z = ((1 - cosTheta) * axis->x * axis->z - axis->y * sinTheta) * arg->x;
  temp.z += ((1 - cosTheta) * axis->y * axis->z + axis->x * sinTheta) * arg->y;
  temp.z += (cosTheta + (1 - cosTheta) * axis->z * axis->z) * arg->z;
 
  arg->x = temp.x;
  arg->y = temp.y;
  arg->z = temp.z; 
}

/* these next three speed up the previous function a lot in special cases */
void VectorRotateX(vector_t *arg, float ang){

  static float cosTheta;
  static float sinTheta;
  vector_t temp;

  /* do the rotation in degrees */
  cosTheta = (float)cos(ang*M_PI/180.0);
  sinTheta = (float)sin(ang*M_PI/180.0);

  /* here we are going to rotate around the x axis */
  temp.x = arg->x;
  temp.y = -sinTheta * arg->z;
  temp.z = sinTheta * arg->y + cosTheta  * arg->z;
 
  arg->x = temp.x;
  arg->y = temp.y;
  arg->z = temp.z; 
}


void VectorRotateY(vector_t *arg, float ang){

  static float cosTheta;
  static float sinTheta;
  vector_t temp;

  /* do the rotation in degrees */
  cosTheta = (float)cos(ang*M_PI/180.0);
  sinTheta = (float)sin(ang*M_PI/180.0);

  /* here we are going to rotate around the axis specified by x, y, z */
  temp.x = cosTheta * arg->x + sinTheta * arg->z;
  temp.y = arg->y;
  temp.z = -sinTheta * arg->x + cosTheta * arg->z;
 
  arg->x = temp.x;
  arg->y = temp.y;
  arg->z = temp.z; 
}

void VectorRotateZ(vector_t *arg, float ang){

  static float cosTheta;
  static float sinTheta;
  vector_t temp;

  /* do the rotation in degrees */
  cosTheta = (float)cos(ang*M_PI/180.0);
  sinTheta = (float)sin(ang*M_PI/180.0);

  /* here we are going to rotate around the axis specified by x, y, z */
  temp.x = cosTheta * arg->x - sinTheta * arg->y;
  temp.y = sinTheta * arg->x + cosTheta * arg->y;
  temp.z = arg->z;
 
  arg->x = temp.x;
  arg->y = temp.y;
  arg->z = temp.z; 
}

float VectorGetYaw(vector_t *arg){

  float yaw;
  /* figure out what quadrant we're in, react accordingly */
  if(arg->z > 0){
    if(arg->x > 0){ /* quadrant 1 */
	  yaw = atanf(arg->z/arg->x) + M_PI;
	}
    else{ /* quadrant 2 */
	  yaw = (atanf(arg->z/arg->x) + M_PI_2) + M_PI;
	}
  }
  else{
    if(arg->x < 0){ /* quadrant 3 */
	  yaw = (atanf(arg->z/arg->x) + M_PI) + M_PI;
	}
    else{ /* quadrant 4 */
	  yaw = (M_2PI - atanf(arg->z/arg->x)) + M_PI;
	}
  }

  return yaw;
}
