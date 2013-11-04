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
#include <math.h>
#include <string.h> /* for memset */

#include "camera.h"

camera_t *CameraAlloc(void){
  camera_t *cam;
  cam = (camera_t *)malloc(sizeof(camera_t));
  return cam;
}

void CameraDestroy(camera_t *cam){
  free(cam);
}

void CameraInit(camera_t *cam){
  memset(cam, 0, sizeof(camera_t));
  cam->up.y = 1.0; /* always the default */
  cam->dFactor = CDAMPING;
  return;
}

/* now begin the important functions... */

/* d and h are camera distance and height from the player */
void CameraFollowPlayer(camera_t *cam, player_t *player, float d, float h){

  float diff;
  float newY;

  newY = player->position.y - d*player->forward.y + h;
  diff = newY - cam->position.y;
  
  cam->position.x = player->position.x - d*player->forward.x;
  cam->position.y += cam->dFactor*diff;
  cam->position.z = player->position.z - d*player->forward.z;  

  newY = player->position.y;
  diff = newY - cam->view.y;
  
  cam->view.x = player->position.x;
  cam->view.y += cam->dFactor*diff;
  cam->view.z = player->position.z;
}

void CameraSetPosition(camera_t *cam, float x, float y, float z){
  cam->position.x = x;
  cam->position.y = y;
  cam->position.z = z;

  /* update the forward pointing vector and the right pointing vector */
  VectorSubtract(&cam->forward, &cam->view, &cam->position);
  VectorNormalize(&cam->forward);
  VectorCross(&cam->right, &cam->forward, &cam->up);
  VectorNormalize(&cam->right);
  return;
}

void CameraSetViewPoint(camera_t *cam, float x, float y, float z){
  cam->view.x = x;
  cam->view.y = y;
  cam->view.z = z;

  /* update the forward pointing vector and the right pointing vector */
  VectorSubtract(&cam->forward, &cam->view, &cam->position);
  VectorNormalize(&cam->forward);
  VectorCross(&cam->right, &cam->forward, &cam->up);
  VectorNormalize(&cam->right);
  return;
}

void CameraSetUpVector(camera_t *cam, float x, float y, float z){
  cam->up.x = x;
  cam->up.y = y;
  cam->up.z = z;
}
		
void CameraRotateView(camera_t *cam, float ang, float x, float y, float z){

  vector_t newView;
  vector_t view;
  static float cosTheta;
  static float sinTheta;

  view = cam->forward;
  cosTheta = (float)cos(ang);
  sinTheta = (float)sin(ang);

  /* here we are going to rotate around the axis specified by x, y, z */
  newView.x = (cosTheta + (1 - cosTheta) * x * x) * view.x;
  newView.x += ((1 - cosTheta) * x * y - z * sinTheta) * view.y;
  newView.x += ((1 - cosTheta) * x * x + y * sinTheta) * view.z;
  
  newView.y = ((1 - cosTheta) * x * y + z * sinTheta) * view.x;
  newView.y += (cosTheta + (1 - cosTheta) * y * y) * view.y;
  newView.y += ((1 - cosTheta) * y * z - x * sinTheta) * view.z;
 
  newView.z = ((1 - cosTheta) * x * z - y * sinTheta) * view.x;
  newView.z += ((1 - cosTheta) * y * z + x * sinTheta) * view.y;
  newView.z += (cosTheta + (1 - cosTheta) * z * z) * view.z;
  
  VectorAdd(&newView, &cam->position, &newView);
  CameraSetViewPoint(cam, newView.x, newView.y, newView.z);

}

/* the next four functions use the forward/right vectors to move */
void CameraMoveForward(camera_t *cam, float amt){
  vector_t scaled;
  VectorScale(&scaled, &cam->forward, amt);
  VectorAdd(&cam->position, &cam->position, &scaled); 
}


void CameraMoveBackward(camera_t *cam, float amt){
  vector_t scaled;
  VectorScale(&scaled, &cam->forward, -amt);
  VectorAdd(&cam->position, &cam->position, &scaled); 
}

void CameraStrafeRight(camera_t *cam, float amt){
  vector_t scaled;
  VectorScale(&scaled, &cam->right, amt);
  VectorAdd(&cam->position, &cam->position, &scaled); 
  VectorAdd(&cam->view, &cam->view, &scaled); 
}


void CameraStrafeLeft(camera_t *cam, float amt){
  vector_t scaled;
  VectorScale(&scaled, &cam->right, -amt);
  VectorAdd(&cam->position, &cam->position, &scaled); 
  VectorAdd(&cam->view, &cam->view, &scaled); 
}

void CameraCorrect(camera_t *cam, float yPos){
  float diff;
  diff = yPos - cam->position.y;
  cam->position.y += cam->dFactor*diff;
  cam->view.y += cam->dFactor*diff;
}

void CameraUpdateView(camera_t *cam){
  gluLookAt(cam->position.x, cam->position.y, cam->position.z, 
		    cam->view.x, cam->view.y, cam->view.z, 
			cam->up.x, cam->up.y, cam->up.z);
  return;
}

