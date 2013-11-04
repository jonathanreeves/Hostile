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

#include "ai.h"
#include "global.h"

/* the procedure for generating AI will go as follows:
 1) make a player with a body, set initial position, etc
 2) get an ai brain (ai_t)
 3) give the brain to the player by calling AIInit
*/

int idCounter = 1; /* each new bot gets its own id, 0 reserved for player1 */

ai_t *AINew(player_t *body){
  ai_t *brain;
  brain = (ai_t *)malloc(sizeof(ai_t));
  AIInit(brain, body);
  return brain; 
}

void AIDestroy(ai_t *brain){
  if(brain != NULL){
	PlayerDestroy(brain->body);
	free(brain);
  }
}

void AIInit(ai_t *brain, player_t *body){
  brain->id = idCounter++;
  brain->body = body;
  brain->curState = AI_IDLE;
  brain->prevState = AI_IDLE;
  brain->fov = FOV;
  brain->timer = 0;
  brain->timeout = 0;
  brain->destination = body->position;
  brain->mate = NULL;
  brain->mate_dis = 1000000; /* just something big */
  brain->cruising = 1.0;
  brain->separation = 1.0;
  brain->alignment = 1.0;
  brain->cohesion = 1.0;
  brain->avoidance = 1.0;
  brain->confidence = 0;
  brain->threshold = 0;
  return;
}


/* check if something is within our vision.
 keep track of the actual dot product using a global variable to be used
 by other processing functions */


int AICheckFOV(ai_t *brain, vector_t *point, terrain_t *ground){
  vector_t forward;
  vector_t position;
  vector_t temp;
  float curDis;
  float curHeight;
  
  forward = brain->body->forward;
  position = brain->body->position;
  VectorSubtract(&brain->enemyDir, point, &position);

  brain->enemy_dis = VectorMagnitude(&brain->enemyDir); 
  VectorScale(&brain->enemyDir, &brain->enemyDir, 1.0/brain->enemy_dis);

  /* check first if hidden by terrain */
  curDis = 1.0;
  while(curDis < brain->enemy_dis){
	VectorScale(&temp, &brain->enemyDir, curDis);
	curHeight = TerrainGetHeight(ground, 
					temp.x + brain->body->position.x, 
					              temp.z + brain->body->position.z);
	if(temp.y + brain->body->position.y + MOD_HEIGHT < curHeight || 
					curDis > 40.0){
	  return 0;
	}
    curDis += 1.0;
  }

  /* now check if in the angular field of view */
  brain->dotEnemy = VectorDot(&brain->enemyDir, &forward);
#ifdef OBSERVE
  return 0;
#else
  return (brain->dotEnemy >= COSHALF);
#endif
}


void AISetState(ai_t *brain, int state){
  brain->prevState = brain->curState;
  brain->curState = state;
}

int AIGetState(ai_t *brain){
  return brain->curState;
}


/* like doing a cross product for only the y coord and testing the sign */
int AIGetTurnDirectionH(ai_t *brain, vector_t *dir){
  float test;
  test = (brain->body->forward.z * dir->x) - 
		  (brain->body->forward.x * dir->z);
  return (test < 0)?-1:1;
}

int AIGetTurnDirectionV(ai_t *brain, vector_t *dir){
  float dot;
  /* we're going to do a projection, note that forward is of unit length */
  dot = VectorDot(&brain->body->forward, dir);
  return (dot*brain->body->forward.y > dir->y)?-1:1;
}

void AIUpdatePosition(ai_t *brain){
  float y;

  PlayerMoveForward(brain->body, stepSize);
  y = TerrainGetHeight(ground, brain->body->position.x, 
					             brain->body->position.z) + MOD_HEIGHT2;
  PlayerCorrect(brain->body, y); 

  /* clamp to the edges of the map if you're trying to go over */
  if(brain->body->position.x > sizeX - 10.0){
    brain->body->position.x = sizeX - 10.0;
	brain->body->yaw += 180.0;
  }
  else if (brain->body->position.x < 10.0){
	brain->body->position.x = 10.0;
	brain->body->yaw += 180.0;
  }
	
  if(brain->body->position.z > sizeZ - 10.0){
    brain->body->position.z = sizeZ - 10.0;
	brain->body->yaw += 180.0;
  }
  else if (brain->body->position.z < 10.0){
	brain->body->position.z = 10.0;
	brain->body->yaw += 180.0;
  }
}

void AIUpdateForwardByAngle(ai_t *brain){
  vector_t xaxis = {1.0, 0.0, 0.0};

  /* and update according to pitch and yaw */
  if(brain->body->yaw > 360.0)
    brain->body->yaw = brain->body->yaw - 360.0;
  else if(brain->body->yaw <= 0)
    brain->body->yaw = brain->body->yaw + 360;

  VectorRotateZ(&xaxis, brain->body->pitch);
  VectorRotateY(&xaxis, brain->body->yaw);
  PlayerSetForward(brain->body, xaxis.x, xaxis.y, xaxis.z);
}


/* try and point towards the direction you want to go (turn pitch/yaw)*/
void AIUpdateForwardByDesire(ai_t *brain, vector_t *desired){
  float dotDesired;
  float strength;

  dotDesired = VectorDot(desired, &brain->body->forward);
  if(dotDesired < 1){
	strength = (1 - dotDesired);
    brain->body->yaw += MAX_TURN*(AIGetTurnDirectionH(brain, desired))*strength;
  }
  AIUpdateForwardByAngle(brain);
}


