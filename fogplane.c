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
#include <GL/glu.h>

#include "fogplane.h"

float fogColor[4] = {0.8, 0.8, 0.8, 1.0};

/* just set up the gl parameters */
void FogPlaneSetup(float near, float far){
  glFogi(GL_FOG_MODE, GL_LINEAR);
  glFogfv(GL_FOG_COLOR, fogColor);
  glFogf(GL_FOG_START, near);
  glFogf(GL_FOG_END, far);
  glHint(GL_FOG_HINT, GL_NICEST);
 
}

/* draw an alpha-blended set of planes which get more and more opaque as you
 * go down */
void FogPlaneDraw(float yaw, float x, float y, float z){
   
    glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPushMatrix();
	  glTranslatef(x, y, z);
	  glRotatef(yaw, 0.0, 1.0, 0.0); 
	  glTranslatef(70.0, 0.0, 0.0);
	  glBegin(GL_QUAD_STRIP);
	   
	    glColor4f(fogColor[0], fogColor[1], fogColor[2], 1.0);
		glVertex3f(0.0, 0.0, -100.0);
		glVertex3f(0.0, 0.0, 100.0);
		glVertex3f(0.0, 1.0, -100.0);
		glVertex3f(0.0, 1.0, 100.0);
	   
	    glColor4f(fogColor[0], fogColor[1], fogColor[2], 1.0);
		glVertex3f(0.0, 4.0, -100.0);
		glVertex3f(0.0, 4.0, 100.0);
		glVertex3f(0.0, 13.0, -100.0);
		glVertex3f(0.0, 13.0, 100.0);
	
	   	glColor4f(fogColor[0], fogColor[1], fogColor[2], 0.4);
		glVertex3f(0.0, 20.0, -100.0);
		glVertex3f(0.0, 20.0, 100.0);

	  	glColor4f(fogColor[0], fogColor[1], fogColor[2], 0.0);
		glVertex3f(0.0, 40.0, -100.0);
		glVertex3f(0.0, 40.0, 100.0);
		
	  glEnd();
	glPopMatrix();
	glDisable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
}


