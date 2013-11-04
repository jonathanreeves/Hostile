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

#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "vector.h"
#include "player.h"

#ifndef CDAMPING
#define CDAMPING 0.20
#endif


typedef struct {
  /* these three are treated as points */
  vector_t position;
  vector_t view;
  vector_t up;
  /* these two are treated as actual vectors */
  vector_t forward;
  vector_t right;

  /* damping */
  float dFactor;

} camera_t;


/* if needed */
camera_t *CameraAlloc(void);
void CameraDestroy(camera_t *cam);
void CameraInit(camera_t *cam);
void CameraZoom(camera_t *cam, float times);

/* most of these are pretty self explanatory */
void CameraUpdateView(camera_t *cam);

void CameraSetPosition(camera_t *cam, float x, float y, float z);

void CameraSetViewPoint(camera_t *cam, float x, float y, float z);

void CameraSetUpVector(camera_t *cam, float x, float y, float z);

void CameraFollowPlayer(camera_t *cam, player_t *player, float d, float h);

void CameraRotateView(camera_t *cam, float ang, float x, float y, float z);

void CameraMoveForward(camera_t *cam, float amt);

void CameraMoveBackward(camera_t *cam, float amt);

void CameraStrafeLeft(camera_t *cam, float amt);

void CameraStrafeRight(camera_t *cam, float amt);

void CameraCorrect(camera_t *cam, float yPos);

#endif
