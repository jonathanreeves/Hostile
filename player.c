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
#include "global.h"

player_t *PlayerNew(void){
  player_t *player;
  player = (player_t *)malloc(sizeof(player_t));
  PlayerInit(player);
  return player;
}

void PlayerDestroy(player_t *player){
  if(player != NULL){
	if(player->model != NULL){
      MD2ModelDestroy(player->model);
	  player->model = NULL;
	}
    free(player);
  }
	
}

void PlayerCleanup(player_t *player){
  MD2ModelDestroy(player->model);
}

void PlayerInit(player_t *player){
  memset(player, 0, sizeof(player_t));
  player->model = NULL;
  player->up.y = 1.0; /* always the default */
  player->dFactor = PDAMPING;
  player->health = MAX_HEALTH;
  player->timer = 0;
  player->timeout = 255;
  player->speed = P_DEFAULT_SPEED;
  return;
}

void PlayerReset(player_t *player){
  player->interpol = 0;
}

void PlayerLoad(player_t *player, const char *md2File, const char *skinFile){
  
  int err;
  player->model = MD2ModelAlloc();
  err = MD2ModelLoad(player->model, md2File, skinFile);
  if(err != 0){
    fprintf(stderr, "error loading model %s\n", md2File);
    free(player->model);
	return;
  }
  /* only change what you don't want to still be zero */
  player->forward.x = 1.0;
  player->right.z = 1.0;
  player->endFrame = 1;

  /* FIXME calculate the bounding radius based on the model */
  player->radius = 0.6;
  player->state = PLAYER_READY;

  return;
}

void PlayerLoadUseSameModel(player_t *player1, player_t *player2){
  player2->model->usageCount++;
  player1->model = player2->model;

  /* only change what you don't want to still be zero */
  player1->forward.x = 1.0;
  player1->right.z = 1.0;
  player1->endFrame = 1;

  /* FIXME calculate the bounding radius based on the model */
  player1->radius = 0.6;
  player1->state = PLAYER_READY;

  return;
}

void PlayerAnimate(player_t *player, float percent){
  
  if(player->startFrame > player->currentFrame){
    player->currentFrame = player->startFrame;
  }

  if((player->startFrame < 0) || (player->endFrame < 0)){
    return;
  }

  if((player->startFrame >= player->model->numFrames) ||
				  (player->endFrame >= player->model->numFrames)){
    return;
  }

  if(player->interpol >= 1.0){
    player->interpol = 0.0;
	player->currentFrame++;

	if(player->currentFrame >= player->endFrame){
	  player->currentFrame = player->startFrame;
    }

	player->nextFrame = player->currentFrame+1;

	if(player->nextFrame >= player->endFrame){
      player->nextFrame = player->startFrame;
    }
  }
   
  /* begin actual opengl rendering */	
  glTranslatef(player->position.x, player->position.y + MOD_HEIGHT2,
				player->position.z);
  if(player->state != PLAYER_DEAD){
     glRotatef(player->yaw, 0.0, 1.0, 0.0);
  }	
  glRotatef(-90.0, 1.0, 0.0, 0.0);
  glScalef(0.02, 0.02, 0.02);
  MD2ModelAnimate(player->model, player->currentFrame, 
				    player->nextFrame, player->interpol);
  /* end actual opengl rendering */

  player->interpol += percent;
}


void PlayerAnimateCulled(player_t *player, frustum_t *frust, float percent){
 
  if(FrustumCheckSphere(frust, &player->position, player->radius)){
    if(player->startFrame > player->currentFrame){
      player->currentFrame = player->startFrame;
    }

    if((player->startFrame < 0) || (player->endFrame < 0)){
      return;
    }

    if((player->startFrame >= player->model->numFrames) ||
				    (player->endFrame >= player->model->numFrames)){
      return;
    }

    if(player->interpol >= 1.0){
      player->interpol = 0.0;
	  player->currentFrame++;

	  if(player->currentFrame >= player->endFrame){
	    player->currentFrame = player->startFrame;
      }

	  player->nextFrame = player->currentFrame+1;

	  if(player->nextFrame >= player->endFrame){
        player->nextFrame = player->startFrame;
      }
    }

    /* begin actual opengl rendering */	
    glTranslatef(player->position.x, player->position.y + MOD_HEIGHT2,
					player->position.z);
    if(player->state != PLAYER_DEAD){
	  glRotatef(player->yaw, 0.0, 1.0, 0.0);
    }	
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glScalef(0.02, 0.02, 0.02);
    MD2ModelAnimate(player->model, player->currentFrame, 
				    player->nextFrame, player->interpol);
	/* end actual opengl rendering */

    player->interpol += percent;
  }
}


void PlayerSetAnimation(player_t *player, int animID){
  switch(animID){
    case PLAYER_ANIM_IDLE0:
	  player->startFrame = 0;
	  player->endFrame = 39;
	  break;
    case PLAYER_ANIM_IDLE1:
	  player->startFrame = 74;
	  player->endFrame = 95;
	  break;
	case PLAYER_ANIM_IDLE2:
	  player->startFrame = 113;
	  player->endFrame = 122;
	  break;
	case PLAYER_ANIM_RUN:
	  player->startFrame = 40;
	  player->endFrame = 46;
	  break;
	case PLAYER_ANIM_JUMP:
	  player->startFrame = 67;
	  player->endFrame = 73;
	  break;
	case PLAYER_ANIM_SHOT0:
	  player->startFrame = 47;
	  player->endFrame = 60;
	  break;
	case PLAYER_ANIM_SHOT1: 
	  player->startFrame = 61;
	  player->endFrame = 66;
	  break;
	case PLAYER_ANIM_DYING:
	  player->startFrame = 178;
	  player->endFrame = 185;
	  break;
	case PLAYER_ANIM_DEAD:
	  player->startFrame = 183;
	  player->endFrame = 184;
	  break;
	break;
  }
}


/* this function uses a lot of globals...see global.h for details */
void PlayerUpdate(player_t *player){
  
  float x, y, z;

  switch(player->state){ 
    case PLAYER_READY:
      /* update the protagonist's position */
      if(player->fFlag){
        PlayerMoveForward(player, player->speed);
      }
      else if(player->bFlag){
        PlayerMoveBackward(player, player->speed);
      }

      if(player->lFlag){
        PlayerStrafeLeft(player, player->speed);
      }
      else if(player->rFlag){
        PlayerStrafeRight(player, player->speed);
      }

      /* correct his height relative to the terrain */
      x = player->position.x;
      z = player->position.z;
      y = TerrainGetHeight(ground, x, z) + MOD_HEIGHT2;
      PlayerCorrect(player, y);
 
      /* clamp him to the edges of the map */
      if(x > (sizeX-10.0))
        player->position.x = sizeX-10.0;
      else if(x < 10.0)
        player->position.x = 10.0;

      if(z > (sizeZ-10.0))
        player->position.z = sizeZ-10.0;
      else if(z < 10.0)
        player->position.z = 10.0;
      
	  break;

    case PLAYER_SHOT:
	  player->timer++;
      break;
    case PLAYER_DYING:
      player->timer++;
      break;
    case PLAYER_DEAD:
	  player->timer++;
	  break;
  }
  if(player->timer > player->timeout){
    PlayerProcessEvent(player, PLAYER_TIMEOUT);
  }
  if(player->health == 0){
    PlayerProcessEvent(player, PLAYER_KILLED);
  }
  return;
}
