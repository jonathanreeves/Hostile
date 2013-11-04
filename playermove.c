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

#include "player.h"


/* now begin the important functions... */
void PlayerSetPosition(player_t *player, float x, float y, float z){
  player->position.x = x;
  player->position.y = y;
  player->position.z = z;

  return;
}

void PlayerSetForward(player_t *player, float x, float y, float z){
  player->forward.x = x;
  player->forward.y = y;
  player->forward.z = z;

  /* update the forward pointing vector and the right pointing vector */
  VectorNormalize(&player->forward);
  VectorCross(&player->right, &player->forward, &player->up);
  VectorNormalize(&player->right);
  return;
}

void PlayerSetForwardByAngle(player_t *player, float p, float y){
 
  vector_t xaxis = {1.0, 0.0, 0.0};
  vector_t yaxis = {0.0, 1.0, 0.0};
  vector_t zaxis = {0.0, 0.0, 1.0};

  VectorRotate(&xaxis, &zaxis, p);
  VectorRotate(&xaxis, &yaxis, y);
  
  player->forward.x = xaxis.x;
  player->forward.y = xaxis.y;
  player->forward.z = xaxis.z;
  player->pitch = p;
  player->yaw = y;

  /* update the forward pointing vector and the right pointing vector */
  VectorNormalize(&player->forward);
  VectorCross(&player->right, &player->forward, &player->up);
  VectorNormalize(&player->right);
  return;
}


void PlayerSetUpVector(player_t *player, float x, float y, float z){
  player->up.x = x;
  player->up.y = y;
  player->up.z = z;
}
		

/* the next four functions use the forward/right vectors to move */
void PlayerMoveForward(player_t *player, float speed){
  vector_t scaled;
  VectorScale(&scaled, &player->forward, speed);
  VectorAdd(&player->position, &player->position, &scaled); 
}


void PlayerMoveBackward(player_t *player, float speed){
  vector_t scaled;
  VectorScale(&scaled, &player->forward, -speed);
  VectorAdd(&player->position, &player->position, &scaled); 
}


void PlayerStrafeRight(player_t *player, float speed){
  vector_t scaled;
  VectorScale(&scaled, &player->right, speed);
  VectorAdd(&player->position, &player->position, &scaled); 
}


void PlayerStrafeLeft(player_t *player, float speed){
  vector_t scaled;
  VectorScale(&scaled, &player->right, -speed);
  VectorAdd(&player->position, &player->position, &scaled); 
}

/* for clamping the player to the terrain */
void PlayerCorrect(player_t *player, float h){
  float diff;
  diff = h - player->position.y;
  player->position.y += player->dFactor*diff;
}

