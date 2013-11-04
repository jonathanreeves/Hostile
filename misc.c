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

#include "global.h"

/* set up all the lights we'll be using */
void SetupLights(void){
  GLfloat ambient1[4] = {0.5, 0.5, 1.0, 0.5};
  GLfloat specular1[4] = {0.0, 0.0, 0.0, 0.0};
  GLfloat diffuse1[4] = {0.0, 0.0, 0.0, 0.0};
  
  GLfloat ambient0[4] = {0.8, 0.8, 0.8, 1.0};
  GLfloat specular0[4] = {0.5, 0.5, 0.5, 1.0};
  GLfloat diffuse0[4] = {0.4, 0.4, 0.4, 1.0};

  /* set the position of the "sun" global variable */
  position0[0] = 50.0;
  position0[1] = 8.0;
  position0[2] = 15.0;


  glLightfv(GL_LIGHT0, GL_POSITION, position0);
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
  glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);
 
  glLightfv(GL_LIGHT1, GL_AMBIENT, ambient1);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse1);
  glLightfv(GL_LIGHT1, GL_SPECULAR, specular1);
}


/* destroy everything we created */
void Cleanup(void){
  FontDestroyAll();
  PlayerDestroy(sod);
  TerrainDestroy(ground);
  TerrainDestroy(water);
  SkyboxDestroy(sky);
  PListFree(plist);
  AIFlockDestroy(flocks[0]);
  AIFlockDestroy(flocks[1]);
  exit(0);
}

