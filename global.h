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

#ifndef __GLOBAL_H__
  #define __GLOBAL_H__
  #ifndef __MAIN_H__

  #include "projectile.h"
  #include "terrain.h"
  #include "frustum.h"
  #include "skybox.h"
  #include "player.h"
  #include "aiflock.h"
  #include "font.h"

  /* global variables we need to use that are associated with main */
  extern float stepSize;
  extern float camDistance;
  extern float camHeight;
  extern float waterLevel;

  /* terrain objects */
  extern terrain_t *ground;
  extern terrain_t *water;

  /* skybox */
  extern skybox_t *sky;

  /* the player */
  extern player_t *sod;

  /* flocks of emergent ai */
  extern aiflock_t *flocks[2];
  
  /* the frustum */
  extern frustum_t frust;

  /* projectile list */
  extern proj_node_t *plist;

  /* map size */
  extern float sizeX;
  extern float sizeZ;
 
  /* light position */ 
  extern GLfloat position0[3];
  #endif
#endif
