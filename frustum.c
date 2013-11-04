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

#include "frustum.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>

/* a useful routine not used anywhere else... */
void MatrixMult(float *ret, float *arg1, float *arg2, int M, int N){
  int i, j, k;
  float temp;
  
  for(i = 0; i < M; i++){
    for(j = 0; j < N; j++){
	  temp = 0.0;
      for(k = 0; k < M; k++){
	    temp += (*(arg1 + M*i + k)) * (*(arg2 + k*M + j)); 
      }
	  *(ret + M*i + j) = temp;
    }
  }
}

/* we need a special normalization routine since it's a 4d vector */
void NormalizePlane(frustum_t *frust, int side){

  float magnitude;
  magnitude = (float)sqrt( (frust->pView)[side][A] * (frust->pView)[side][A] + 
                  (frust->pView)[side][B] * (frust->pView)[side][B] + 
                   (frust->pView)[side][C] * (frust->pView)[side][C] );

  (frust->pView)[side][A] /= magnitude;
  (frust->pView)[side][B] /= magnitude;
  (frust->pView)[side][C] /= magnitude;
  (frust->pView)[side][D] /= magnitude; 
}

/* unnecessary overhead but API consistency */
void FrustumUpdateProjection(frustum_t *frust){
  glGetFloatv(GL_PROJECTION_MATRIX, frust->projection);
}

void FrustumUpdateView(frustum_t *frust){
		
  float modelView[16];
  float viewFrust[16];

  glGetFloatv(GL_MODELVIEW_MATRIX, modelView);
  MatrixMult(viewFrust, modelView, frust->projection, 4, 4); 
				 
    /* get the right side of the frustum */
  (frust->pView)[RIGHT][A] = viewFrust[3] - viewFrust[0];
  (frust->pView)[RIGHT][B] = viewFrust[7] - viewFrust[4];
  (frust->pView)[RIGHT][C] = viewFrust[11] - viewFrust[ 8];
  (frust->pView)[RIGHT][D] = viewFrust[15] - viewFrust[12];

  /* Normalize the 4d vector */
  NormalizePlane(frust, RIGHT);

  /* get the left side of the frustum */
  (frust->pView)[LEFT][A] = viewFrust[3] + viewFrust[0];
  (frust->pView)[LEFT][B] = viewFrust[7] + viewFrust[4];
  (frust->pView)[LEFT][C] = viewFrust[11] + viewFrust[8];
  (frust->pView)[LEFT][D] = viewFrust[15] + viewFrust[12];

  /* Normalize the 4d vector */
  NormalizePlane(frust, LEFT);

  /* etc... */
  (frust->pView)[BOTTOM][A] = viewFrust[3] + viewFrust[1];
  (frust->pView)[BOTTOM][B] = viewFrust[7] + viewFrust[5];
  (frust->pView)[BOTTOM][C] = viewFrust[11] + viewFrust[ 9];
  (frust->pView)[BOTTOM][D] = viewFrust[15] + viewFrust[13];

  NormalizePlane(frust, BOTTOM);

  (frust->pView)[TOP][A] = viewFrust[3] - viewFrust[1];
  (frust->pView)[TOP][B] = viewFrust[7] - viewFrust[5];
  (frust->pView)[TOP][C] = viewFrust[11] - viewFrust[9];
  (frust->pView)[TOP][D] = viewFrust[15] - viewFrust[13];

  NormalizePlane(frust, TOP);

  (frust->pView)[BACK][A] = viewFrust[3] - viewFrust[2];
  (frust->pView)[BACK][B] = viewFrust[7] - viewFrust[6];
  (frust->pView)[BACK][C] = viewFrust[11] - viewFrust[10];
  (frust->pView)[BACK][D] = viewFrust[15] - viewFrust[14];

  NormalizePlane(frust, BACK);

  (frust->pView)[FRONT][A] = viewFrust[3] + viewFrust[2];
  (frust->pView)[FRONT][B] = viewFrust[7] + viewFrust[6];
  (frust->pView)[FRONT][C] = viewFrust[11] + viewFrust[10];
  (frust->pView)[FRONT][D] = viewFrust[15] + viewFrust[14];

  NormalizePlane(frust, FRONT);
}

/* check if a point is in the frustum */
int FrustumCheckPoint(frustum_t *frust, vector_t *point){
  
  int i;
  float x, y, z;
  x = point->x;
  y = point->y;
  z = point->z;

  /* check all the sides of the frustum  -- there are six */
  for(i = 0; i < 6; i++ ){
    if((frust->pView)[i][A] * x + (frust->pView)[i][B] * y + 
			  (frust->pView)[i][C] * z + (frust->pView)[i][D] <= 0){
        return 0;
    }
  }

  return 1;
}


/* sphere checking is simple and fast because a sphere is a simple object */
int FrustumCheckSphere(frustum_t *frust, vector_t *point, float radius){

  int i;
  float x, y, z;
  x = point->x;
  y = point->y;
  z = point->z;


  /* check all the sides of the frustum */
  for(i = 0; i < 6; i++ ){
     
    if( frust->pView[i][A] * x + (frust->pView)[i][B] * y + 
			(frust->pView)[i][C] * z + (frust->pView)[i][D] <= -radius ){
      return 0;
    }
  }
  return 1;
}

/* this one is the most complicated and we should avoid using it whenever
 * possible for speed */
int FrustumCheckCube(frustum_t *frust, vector_t *point, float size ){

  int i;
  float x, y, z;
  x = point->x;
  y = point->y;
  z = point->z;

  /* check each side of the frustum... works similar to sphere except that
   * we have planes intersecting planes */
  for(i = 0; i < 6; i++ ){
    if( (frust->pView)[i][A] * (x - size) + (frust->pView)[i][B] * (y - size) + 
			     (frust->pView)[i][C] * (z - size) + (frust->pView)[i][D] > 0)
      continue;
    if( (frust->pView)[i][A] * (x + size) + (frust->pView)[i][B] * (y - size) + 
			     (frust->pView)[i][C] * (z - size) + (frust->pView)[i][D] > 0)
      continue;
    if( (frust->pView)[i][A] * (x - size) + (frust->pView)[i][B] * (y + size) + 
			     (frust->pView)[i][C] * (z - size) + (frust->pView)[i][D] > 0)
      continue;
    if( (frust->pView)[i][A] * (x + size) + (frust->pView)[i][B] * (y + size) + 
			     (frust->pView)[i][C] * (z - size) + (frust->pView)[i][D] > 0)
      continue;
    if( (frust->pView)[i][A] * (x - size) + (frust->pView)[i][B] * (y - size) + 
			     (frust->pView)[i][C] * (z + size) + (frust->pView)[i][D] > 0)
      continue;
    if( (frust->pView)[i][A] * (x + size) + (frust->pView)[i][B] * (y - size) + 
			     (frust->pView)[i][C] * (z + size) + (frust->pView)[i][D] > 0)
      continue;
    if( (frust->pView)[i][A] * (x - size) + (frust->pView)[i][B] * (y + size) + 
			     (frust->pView)[i][C] * (z + size) + (frust->pView)[i][D] > 0)
      continue;
    if( (frust->pView)[i][A] * (x + size) + (frust->pView)[i][B] * (y + size) + 
			     (frust->pView)[i][C] * (z + size) + (frust->pView)[i][D] > 0)
      continue;

    return 0;
  }
  return 1;
}

