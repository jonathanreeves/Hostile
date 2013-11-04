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

#ifndef __AI_H__
#define __AI_H__

#include "player.h"
#include "terrain.h"
#include "message.h"

#define IRMAX 4.656612e-10 /* one over RAND_MAX */
#define UNIRAND(X) (2*X*((IRMAX*rand()) - 0.5))
#define SIGN(X) ((X>=0)?1:-1)

#define FOV 80 
#define COSHALF 0.766044443119 /* cosine of half the field of view */
#define MAX_TURN 5 /* maximum degrees in one iteration */

#define POS_ERR 60 /* random error in locating the player */
#define TIME_ERR 100 /* random error in timeouts for certain events */
#define R_TIME_MOD TIME_ERR*IRMAX
#define AI_JITTER 5

#define AI_DESIRED_RANGE 20  /* how close you want to be to the enemy */
#define AI_MOVE_SIZE 20 /* don't remember if I'm still using this... */

#define AI_IDLE_TIMEOUT 20
#define AI_CRUISING_TIMEOUT 300 
#define AI_SCAN_TIMEOUT 100 
#define AI_REPO_TIMEOUT 50
#define AI_SHOT_TIMEOUT 30 
#define AI_DYING_TIMEOUT 22
#define AI_ATTACK_TIMEOUT 20
#define AI_DEAD_TIMEOUT 200

#define AI_CONFIDENCE_MAX 64 /* distance in magnitude squared */
#define AI_ATTACK_THRESH 2 /* confidence must be higher before you attack */
#define AI_ATTACK_PERIOD 40 /* how often you fire when in attack mode */

enum ai_states {
  AI_IDLE,
  AI_CRUISING, /* normal AI behavior */
  AI_REPOSITION,
  AI_SCAN,
  AI_ATTACK,
  AI_SHOT,
  AI_DYING,
  AI_DEAD
};

enum ai_events {
  AI_GOT_SHOT, /* you got shot */
  AI_IN_FOV, /* enemy in field of view */
  AI_OUT_FOV, /* enemy no longer in field of view */
  AI_ARRIVED, /* you arrived at your destination */
  AI_COLLIDE, /* you hit something */
  AI_KILLED, /* oops, no more health */
  AI_TIMEOUT /* whatever you were doing, you got bored */
};

/* a huge brain structure for AI bots */
typedef struct AIBOID {
  int id;
  int curState;
  int prevState;
  float fov; /* angle range over which the player can see */
  unsigned int timer;
  unsigned int timeout;
  player_t *body;
  vector_t enemyDir; /* a unit vector which always points towards an enemy */
  vector_t destination; /* a point defining where we want to go */
  vector_t destDir; /* unit vector pointing towards our destination */
  float dotEnemy; /* dot product between you and the enemy */
  float dotDest; /* dot product between you and the destination */
  float tPitch; /* the target pitch you want to have (actual is in body) */
  float tYaw; /* the target yaw you want to have (actual is in body) */
  float enemy_dis; /* distance by direct pointing */
  
  /* now for the flocking stuff... */
  struct AIBOID *mate; /* your nearest friend */
  float mate_dis; /* distance to your nearest mate in magnitude squared */
  float cruising; /* behavioral traits */
  float separation;
  float alignment;
  float cohesion;
  float avoidance;
  int confidence; /* when confident enough, attack */
  int threshold; /* defines your threshold */
} ai_t;


/* the procedure for generating AI will go as follows:
 1) make a player with a body, set initial position, etc
 2) get an ai brain (ai_t)
 3) give the brain to the player by calling AIInit
 4) from then on work only with the brain, not the player */

void AIInit(ai_t *brain, player_t *body);

ai_t *AINew(player_t *body);

void AIDestroy(ai_t *brain);

/* last argument is actually a return variable */
int AICheckFOV(ai_t *brain, vector_t *point, terrain_t *ground);

void AIProcessEvent(ai_t *brain, int event);

void AISetState(ai_t *brain, int state);

int AIGetState(ai_t *brain);

/* plus or minus...tells you whether to increase or decrease pitch/yaw */
int AIGetTurnDirectionH(ai_t *brain, vector_t *dir);

int AIGetTurnDirectionV(ai_t *brain, vector_t *dir);

void AIUpdatePosition(ai_t *brain);

void AIUpdateForwardByAngle(ai_t *brain);

void AIUpdateForwardByDesire(ai_t *brain, vector_t *desired);

/* state machine stuff */
void AIIdleEnter(ai_t *brain);
void AIIdleExit(ai_t *brain);
void AIIdleUpdate(ai_t *brain);

void AICruisingEnter(ai_t *brain);
void AICruisingExit(ai_t *brain);
void AICruisingUpdate(ai_t *brain);

void AIRepositionEnter(ai_t *brain);
void AIRepositionExit(ai_t *brain);
void AIRepositionUpdate(ai_t *brain);

void AIScanEnter(ai_t *brain);
void AIScanExit(ai_t *brain);
void AIScanUpdate(ai_t *brain);

void AIAttackEnter(ai_t *brain);
void AIAttackExit(ai_t *brain);
void AIAttackUpdate(ai_t *brain);

void AIShotEnter(ai_t *brain);
void AIShotExit(ai_t *brain);
void AIShotUpdate(ai_t *brain);

void AIDyingEnter(ai_t *brain);
void AIDyingExit(ai_t *brain);
void AIDyingUpdate(ai_t *brain);

void AIDeadEnter(ai_t *brain);
void AIDeadExit(ai_t *brain);
void AIDeadUpdate(ai_t *brain);

/* actually defined in aiflock.c */
player_t *AIGetEnemy(void);
#endif
