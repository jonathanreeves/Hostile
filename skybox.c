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
#include <GL/gl.h>
#include <GL/glu.h>

#include "texture.h"
#include "skybox.h"

skybox_t *SkyboxLoad(float width, float length, float height, 
				float centerX, float centerZ){
    
  float x, y, z;
  texture_t tempTex;
  skybox_t *box;

  x = centerX - width/2;
  y = -height/2;
  z = centerZ - length/2;

  /* wont do much without this... */
  box = (skybox_t *)malloc(sizeof(skybox_t));
  if(box == NULL){
    fprintf(stderr, "error getting skybox memory\n");
	return NULL;
  }
  
  /* load all textures */
  box->texID[SKY_BACK] = TextureLoadSky(&tempTex, "textures/skyback.ppm");
  box->texID[SKY_FRONT] = TextureLoadSky(&tempTex, "textures/skyfront.ppm");	
  box->texID[SKY_BOTTOM] = TextureLoadSky(&tempTex, "textures/skybottom.ppm");	
  box->texID[SKY_TOP] = TextureLoadSky(&tempTex, "textures/skytop.ppm");	
  box->texID[SKY_LEFT] = TextureLoadSky(&tempTex, "textures/skyleft.ppm");	
  box->texID[SKY_RIGHT] = TextureLoadSky(&tempTex, "textures/skyright.ppm");	

  /* set the back face	 */

  VectorSet(box->back, x+width, y, z);
  VectorSet(box->back + 1, x+width, y+height, z);
  VectorSet(box->back + 2, x, y+height, z);
  VectorSet(box->back + 3, x, y, z);

  /* set the front face	 */
  VectorSet(box->front, x, y, z + length);
  VectorSet(box->front + 1, x, y + height, z + length);
  VectorSet(box->front + 2, x + width, y+height, z + length);
  VectorSet(box->front + 3, x + width, y, z + length);

  /* etc... */
  VectorSet(box->bottom, x, y, z);
  VectorSet(box->bottom + 1, x, y, z + length);
  VectorSet(box->bottom + 2, x + width, y, z + length);
  VectorSet(box->bottom + 3, x + width, y, z);


  VectorSet(box->top, x + width, y + height, z);
  VectorSet(box->top + 1, x + width, y + height, z + length);
  VectorSet(box->top + 2, x, y + height, z + length);
  VectorSet(box->top + 3, x, y + height, z);

  
  VectorSet(box->left, x + width, y, z);
  VectorSet(box->left + 1, x + width, y, z + length);
  VectorSet(box->left + 2, x + width, y + height, z + length);
  VectorSet(box->left + 3, x + width, y + height, z);
    

  VectorSet(box->right, x, y + height, z);
  VectorSet(box->right + 1, x, y + height, z + length);
  VectorSet(box->right + 2, x, y, z + length);
  VectorSet(box->right + 3, x, y, z);
 
  return box;
}

void SkyboxDraw(skybox_t *box){
	
  glBindTexture(GL_TEXTURE_2D, box->texID[SKY_BACK]);
  glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0); 
	glVertex3fv((float *)(box->back));
    glTexCoord2f(0.0, 0.0); 
	glVertex3fv((float *)(box->back + 1));
    glTexCoord2f(1.0, 0.0); 
	glVertex3fv((float *)(box->back + 2)); 
    glTexCoord2f(1.0, 1.0); 
	glVertex3fv((float *)(box->back + 3));
  glEnd();

  glBindTexture(GL_TEXTURE_2D, box->texID[SKY_FRONT]);
  glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0); 
	glVertex3fv((float *)(box->front));
  	glTexCoord2f(0.0, 0.0); 
	glVertex3fv((float *)(box->front + 1));
    glTexCoord2f(1.0, 0.0); 
	glVertex3fv((float *)(box->front + 2)); 
    glTexCoord2f(1.0, 1.0); 
	glVertex3fv((float *)(box->front + 3));
  glEnd();

  glBindTexture(GL_TEXTURE_2D, box->texID[SKY_BOTTOM]);
  glBegin(GL_QUADS);
  	glTexCoord2f(0.0, 1.0); 
	glVertex3fv((float *)(box->bottom));
    glTexCoord2f(0.0, 0.0); 
	glVertex3fv((float *)(box->bottom + 1));
    glTexCoord2f(1.0, 0.0); 
	glVertex3fv((float *)(box->bottom + 2)); 
    glTexCoord2f(1.0, 1.0); 
	glVertex3fv((float *)(box->bottom + 3));
  glEnd();

  glBindTexture(GL_TEXTURE_2D, box->texID[SKY_TOP]);
  glBegin(GL_QUADS);
  	glTexCoord2f(1.0, 0.0); 
	glVertex3fv((float *)(box->top));
    glTexCoord2f(1.0, 1.0); 
	glVertex3fv((float *)(box->top + 1));
    glTexCoord2f(0.0, 1.0); 
	glVertex3fv((float *)(box->top + 2)); 
    glTexCoord2f(0.0, 0.0); 
	glVertex3fv((float *)(box->top + 3));
  glEnd();
 
  glBindTexture(GL_TEXTURE_2D, box->texID[SKY_LEFT]);
  glBegin(GL_QUADS);
  	glTexCoord2f(1.0, 1.0); 
	glVertex3fv((float *)(box->left));
    glTexCoord2f(0.0, 1.0); 
	glVertex3fv((float *)(box->left + 1));
    glTexCoord2f(0.0, 0.0); 
	glVertex3fv((float *)(box->left + 2)); 
    glTexCoord2f(1.0, 0.0); 
	glVertex3fv((float *)(box->left + 3));
  glEnd();
  
  glBindTexture(GL_TEXTURE_2D, box->texID[SKY_RIGHT]);
  glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); 
    glVertex3fv((float *)(box->right));
	glTexCoord2f(1.0, 0.0); 
	glVertex3fv((float *)(box->right + 1));
    glTexCoord2f(1.0, 1.0); 
	glVertex3fv((float *)(box->right + 2)); 
    glTexCoord2f(0.0, 1.0); 
	glVertex3fv((float *)(box->right + 3));
  glEnd();

  return;
}

void SkyboxDestroy(skybox_t *skybox){
  free(skybox);
}
