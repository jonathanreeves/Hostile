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

#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "vector.h"
#include "md2.h"
#include "frustum.h"

#define MOD_HEIGHT 0.45
#define MOD_HEIGHT2 0.225

#define MAX_HEALTH 40
#define IMAX_HEALTH 1.0/MAX_HEALTH 

#ifndef PDAMPING
#define PDAMPING 0.25
#endif

#define P_SHOT_TIMEOUT 30 
#define P_DYING_TIMEOUT 22
#define P_ATTACK_TIMEOUT 50
#define P_DEAD_TIMEOUT 100
#define P_READY_TIMEOUT 600

#define P_DEFAULT_SPEED 0.10

/* states controlling how the player is displayed */
enum pAnimStates{
  PLAYER_ANIM_IDLE0,
  PLAYER_ANIM_IDLE1,
  PLAYER_ANIM_IDLE2,
  PLAYER_ANIM_CROUCH,
  PLAYER_ANIM_RUN,
  PLAYER_ANIM_JUMP,
  PLAYER_ANIM_SHOT0,
  PLAYER_ANIM_SHOT1,
  PLAYER_ANIM_DYING,
  PLAYER_ANIM_DEAD
};

/* states controlling how input is handled for the player */
enum pInputStates{
  PLAYER_READY,
  PLAYER_SHOT,
  PLAYER_DYING,
  PLAYER_DEAD
};

/* events that can happen to a player, changing behavior */
enum pEvents{
  PLAYER_RUNNING,
  PLAYER_JUMPING,
  PLAYER_IDLE,
  PLAYER_GOT_SHOT,
  PLAYER_KILLED,
  PLAYER_TIMEOUT
};

typedef struct{
  MD2Model_t *model;
  vector_t forward; /* for moving forward/backward */
  vector_t right; /* for strafing */
  vector_t position;
  vector_t up;
  float speed; /* not currently in use */
  
  unsigned char fFlag; /* forward flag */
  unsigned char bFlag; /* backward flag */
  unsigned char lFlag; /* left flag */
  unsigned char rFlag; /* right flag */
  unsigned char jFlag; /* jump flag */
  unsigned int timer; /* for being shot/dying, etc */
  unsigned int timeout; /* likewise */
  
  float pitch; /* keep track of pitch */
  float yaw; /* keep track of yaw */
  float dFactor; /* make sure we're not bouncing everywhere */
  
  float radius; /* bounding volume */
  int health; /* current player health */

  float interpol; /* next three are for keyframe interpolation */
  int currentFrame;
  int nextFrame;
  int startFrame;
  int endFrame;
  int state; /* controls input for when you're shot, dying, etc */
} player_t;



/* first functions found in player.c */
player_t *PlayerNew(void);

void PlayerDestroy(player_t *player);

void PlayerCleanup(player_t *player);

void PlayerInit(player_t *player);

/* puts interpol back to zero */
void PlayerReset(player_t *player);

void PlayerLoad(player_t *player, const char *md2File, const char *skinFile);

void PlayerLoadUseSameModel(player_t *player1, player_t *player2);

void PlayerAnimate(player_t *player, float percent);

void PlayerAnimateCulled(player_t *player, frustum_t *frust, float percent);

void PlayerSetAnimation(player_t *player, int animID);

void PlayerUpdate(player_t *player);

void PlayerProcessEvent(player_t *player, int event);

/* next functions defined in playermove.c */
void PlayerSetPosition(player_t *player, float x, float y, float z);

void PlayerSetForward(player_t *player, float x, float y, float z);

void PlayerSetForwardByAngle(player_t *player, float p, float y);

void PlayerSetUpVector(player_t *player, float x, float y, float z);
		
void PlayerMoveForward(player_t *player, float speed);

void PlayerMoveBackward(player_t *player, float speed);

void PlayerStrafeRight(player_t *player, float speed);

void PlayerStrafeLeft(player_t *player, float speed);

void PlayerCorrect(player_t *player, float h);

#endif
