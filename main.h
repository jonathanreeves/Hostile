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

#ifndef __MAIN_H__
#define __MAIN_H__

#include "texture.h"
#include "camera.h"
#include "player.h"
#include "ai.h"
#include "skybox.h"
#include "projectile.h"
#include "terrain.h"
#include "frustum.h"
#include "fogplane.h"
#include "aiflock.h"
#include "font.h"

#define POW2(X) 1 << X

/* define some defaults */
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define MOUSE_SENS 0.05 /* 1/20 */
#define STEP_SIZE 0.10 /* trying to replace this by speed */
#define CAM_DISTANCE 5.0 /* distance from player */
#define CAM_HEIGHT 2.0 /* height above player */
#define WATER_LEVEL 7.4

/* global variables and their associated defaults some we want available
 outside of here...see global.h for details */
float stepSize = STEP_SIZE;
float camDistance = CAM_DISTANCE;
float camHeight = CAM_HEIGHT;
float waterLevel = WATER_LEVEL;

/* terrain objects */
terrain_t *ground;
terrain_t *water;

/* the frustum */
frustum_t frust;

/* a lovely skybox */
skybox_t *sky;

/* the player */
player_t *sod;

/* a couple of flocks of emergent ai controlled bots */
aiflock_t *flocks[2];

proj_node_t *plist;

float sizeX;
float sizeZ;

/* position of the "sun" */
GLfloat position0[3];

/* these functions needed to not take up space in main.c so they went to 
 * misc.c */
void SetupLights(void);
void Cleanup(void);

#endif
