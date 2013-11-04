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

extern int idCounter;

/* define behavior for bots in idle state */
void AIIdleEnter(ai_t *brain){
  brain->prevState = brain->curState;
  brain->curState = AI_IDLE;
  brain->timer = 0;
  brain->timeout = AI_IDLE_TIMEOUT + R_TIME_MOD*rand();
  PlayerSetAnimation(brain->body, PLAYER_ANIM_IDLE0);
}

void AIIdleExit(ai_t *brain){
}

void AIIdleUpdate(ai_t *brain){
  brain->timer++;
}


/* define behavior for bots in crusing state. This should be the flocking
 * emergent behavioral model. It's what happens when enemies are just
 * moving around the map trying to form groups */
void AICruisingEnter(ai_t *brain){
  
  player_t *enemy;
  vector_t destDir;
 
  brain->prevState = brain->curState;
  brain->curState = AI_CRUISING;
  brain->timer = 0;
  brain->timeout = AI_CRUISING_TIMEOUT + R_TIME_MOD*rand();
  PlayerSetAnimation(brain->body, PLAYER_ANIM_RUN);
  PlayerReset(brain->body); 
  
  /* I believe one might refer to this as cheating */
  if((enemy = AIGetEnemy()) != NULL){
    brain->destination.x = enemy->position.x + POS_ERR*UNIRAND(1.0);
    brain->destination.z = enemy->position.z + POS_ERR*UNIRAND(1.0);
    brain->destination.y = enemy->position.y;
	/* set your body pointing in the right direction */
	VectorSubtract(&destDir, &brain->destination, &brain->body->position);
	VectorNormalize(&destDir); /* direction to our destination */
	brain->destDir = destDir;
	brain->dotDest = VectorDot(&destDir, &brain->body->forward);
	brain->tPitch = 0;
  }

}

void AICruisingExit(ai_t *brain){
}

void AICruisingUpdate(ai_t *brain){
  player_t *enemy;

  brain->timer++;
  if((enemy = AIGetEnemy()) != NULL){
	/* add a small jitter to the destination */
    brain->destination.x = brain->destination.x + 
			AI_JITTER*UNIRAND(1.0);
    brain->destination.z = brain->destination.z + 
			AI_JITTER*UNIRAND(1.0);
  }
}


/* define behavior for bots in reposition state */
void AIRepositionEnter(ai_t *brain){
  player_t *enemy;
  vector_t orthog;
  vector_t temp;
  float diff;
 
  brain->prevState = brain->curState;
  brain->curState = AI_REPOSITION;
  brain->timer = 0;
  brain->timeout = AI_REPO_TIMEOUT + R_TIME_MOD*rand();
  PlayerSetAnimation(brain->body, PLAYER_ANIM_RUN);
  PlayerReset(brain->body); 
  VectorClear(&brain->destination);
  
  /* we're currently fighting, we just want to move slightly
   * before attacking again */ 
  if((enemy = AIGetEnemy()) != NULL){
	/* get a vector pointing towards the enemy */
	VectorSubtract(&brain->enemyDir, &enemy->position, &brain->body->position);
	/* get the distance to the enemy */
	brain->enemy_dis = VectorMagnitude(&brain->enemyDir);
	VectorScale(&brain->enemyDir, &brain->enemyDir, 1.0/brain->enemy_dis);
	/* get a vector orthogonal to the direction of the enemy */
	VectorCross(&orthog, &brain->enemyDir, &brain->body->up);
	/* decide if we need to move closer */
	if(brain->enemy_dis > AI_DESIRED_RANGE){
      diff = brain->enemy_dis - AI_DESIRED_RANGE + UNIRAND(6.0);
	  VectorScale(&temp, &brain->enemyDir, diff);
	  VectorAdd(&brain->destination, &temp, &brain->body->position);
	}
	/* now move sideways some */
	diff = AI_MOVE_SIZE + UNIRAND(6.0);
	VectorScale(&temp, &orthog, 0.2*SIGN(UNIRAND(1.0))*diff);
	VectorAdd(&brain->destination, &brain->destination, &temp);
  }
}

void AIRepositionExit(ai_t *brain){
}
void AIRepositionUpdate(ai_t *brain){
  brain->timer++;
  brain->destination.x = brain->destination.x + 
			AI_JITTER*((IRMAX*rand()) - 0.5);
  brain->destination.z = brain->destination.z + 
			AI_JITTER*((IRMAX*rand()) - 0.5);
}


/* define behavior for bots in the scan state */
void AIScanEnter(ai_t *brain){
  player_t *enemy = AIGetEnemy();
  brain->prevState = brain->curState;
  brain->curState = AI_SCAN;
  brain->timer = 0;
  brain->timeout = AI_SCAN_TIMEOUT + R_TIME_MOD*rand();
  PlayerSetAnimation(brain->body, PLAYER_ANIM_IDLE1);
  VectorSubtract(&brain->enemyDir, &enemy->position, &brain->body->position);
  PlayerReset(brain->body); 
}

void AIScanExit(ai_t *brain){
}
void AIScanUpdate(ai_t *brain){
  brain->timer++;
  brain->body->yaw += 2*AIGetTurnDirectionH(brain, &brain->enemyDir);
  brain->body->pitch = 0;
}


/* define behavior for bots in the attack state */
void AIAttackEnter(ai_t *brain){
  player_t *enemy;
		
  brain->prevState = brain->curState;
  brain->timer = 0;
  if((enemy = AIGetEnemy()) != NULL){
	/* get a vector pointing towards the enemy */
	VectorSubtract(&brain->enemyDir, &enemy->position, &brain->body->position);
	/* get the distance to the enemy */
	brain->enemy_dis = VectorMagnitude(&brain->enemyDir);
	VectorScale(&brain->enemyDir, &brain->enemyDir, 1.0/brain->enemy_dis);
	
	/* decide if we need to move closer and if we do, we're in the wrong
	 * state (should be in reposition */
	if(brain->enemy_dis > AI_DESIRED_RANGE){
	  AIRepositionEnter(brain);
	}
	else{
       brain->curState = AI_ATTACK;
       brain->timeout = AI_ATTACK_TIMEOUT + R_TIME_MOD*rand();
       PlayerSetAnimation(brain->body, PLAYER_ANIM_IDLE2);
       PlayerReset(brain->body); 
	}
  }
}

void AIAttackExit(ai_t *brain){
}
void AIAttackUpdate(ai_t *brain){
  float strength;
  float diff;
  static int count;
  player_t *enemy;
  vector_t prediction = {0.0, 0.0, 0.0};
  vector_t temp;
  projectile_t *proj;
  
  brain->timer++;
  /* use info you can see from the player to figure out where he'll be in
   * the next step...we're trying to aim with some accuracy. The following
   * calculations approximate the exact solution which can be found for
   * the collision time of two moving projectiles. The general solution
   * only exists under certain conditions. This always exists, and works
   * quite well at reasonable range */
  enemy = AIGetEnemy();
  if(enemy->lFlag){
    VectorSubtract(&prediction, &prediction, &enemy->right);
  }
  else if(enemy->rFlag){
    VectorAdd(&prediction, &prediction, &enemy->right);
  }
  if(enemy->fFlag){
    VectorAdd(&prediction, &prediction, &enemy->forward);
  }
  else if(enemy->bFlag){
    VectorSubtract(&prediction, &prediction, &enemy->forward);
  }
  VectorSubtract(&temp, &enemy->position, &brain->body->position);
  VectorAdd(&temp, &temp, &prediction);
  VectorScale(&brain->enemyDir, &temp, enemy->speed);
  brain->enemy_dis = VectorMagnitude(&brain->enemyDir);
  VectorScale(&brain->enemyDir, &brain->enemyDir, 1.0/brain->enemy_dis);
  brain->dotEnemy = VectorDot(&brain->enemyDir, &brain->body->forward);
 
  /* decide if we need to move closer */
  if(brain->enemy_dis > AI_DESIRED_RANGE){
    diff = brain->enemy_dis - AI_DESIRED_RANGE + UNIRAND(3.0);
    VectorScale(&temp, &brain->enemyDir, diff);
    VectorAdd(&brain->destination, &temp, &brain->body->position);
	AIUpdateForwardByDesire(brain, &brain->destination);
	PlayerMoveForward(brain->body, brain->body->speed);
  }
  else{
    if(brain->dotEnemy < 1){
	  strength = 2 - brain->dotEnemy;
	  /* we need to aim at our opponent pitch/yaw */
      brain->body->yaw += 
			MAX_TURN*strength*(AIGetTurnDirectionH(brain, &brain->enemyDir));
      brain->body->pitch += 
			MAX_TURN*strength*(AIGetTurnDirectionV(brain, &brain->enemyDir));
    }
    if(!(count % AI_ATTACK_PERIOD)){
	  /* fire the projectile in the exactly direction even though we may
	   * not have our bodies pointed there yet */
      proj = ProjectileNew(&brain->enemyDir, &brain->body->position, 
				      PROJ_SPEED, 2);
      PListInsertAfter(plist, proj);
	  count = 0;
    } count++;
  }
}


/* define behavior for bots in the shot state (got shot) */
void AIShotEnter(ai_t *brain){
  brain->prevState = brain->curState;
  brain->timer = 0;
  brain->curState = AI_SHOT;
  brain->timeout = AI_SHOT_TIMEOUT;
  PlayerSetAnimation(brain->body, PLAYER_ANIM_SHOT0);
  PlayerReset(brain->body); 
}
void AIShotExit(ai_t *brain){
}
void AIShotUpdate(ai_t *brain){
  player_t *enemy;
  float strength;

  brain->timer++;
  enemy = AIGetEnemy();
  VectorSubtract(&brain->enemyDir, &enemy->position, &brain->body->position);
  VectorNormalize(&brain->enemyDir);
  brain->dotEnemy = VectorDot(&brain->body->forward, &brain->enemyDir);
  strength = 1 - brain->dotEnemy;
  brain->body->yaw += 
		MAX_TURN*strength*(AIGetTurnDirectionH(brain, &brain->enemyDir));
  brain->body->pitch += 
		MAX_TURN*strength*(AIGetTurnDirectionV(brain, &brain->enemyDir));

  PlayerSetForwardByAngle(brain->body, brain->body->pitch, brain->body->yaw);
}


/* define behavior for bots that are dying */
void AIDyingEnter(ai_t *brain){
  brain->prevState = brain->curState;
  brain->timer = 0;
  brain->curState = AI_DYING;
  brain->timeout = AI_DYING_TIMEOUT;
  brain->body->health = 255;
  PlayerSetAnimation(brain->body, PLAYER_ANIM_DYING);
  PlayerReset(brain->body); 
}
void AIDyingExit(ai_t *brain){
}
void AIDyingUpdate(ai_t *brain){
  brain->timer++;
}


/* define behavior for bots that are dead */
void AIDeadEnter(ai_t *brain){
  brain->prevState = brain->curState;
  brain->timer = 0;
  brain->curState = AI_DEAD;
  brain->timeout = AI_DEAD_TIMEOUT;
  PlayerSetAnimation(brain->body, PLAYER_ANIM_DEAD);
  PlayerReset(brain->body); 
}
void AIDeadExit(ai_t *brain){
}
void AIDeadUpdate(ai_t *brain){
  brain->timer++;
}

