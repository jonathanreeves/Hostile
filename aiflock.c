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
#include <string.h>
#include <math.h>

#include "global.h"
#include "aiflock.h"

player_t *curEnemy = NULL;

/* this section uses a lot of globals. See global.h for more details */

aiflock_t *AIFlockNew(void){
  aiflock_t *flock;
  flock = (aiflock_t *)malloc(sizeof(aiflock_t));
  flock->bots = EListNew(); 
  flock->nBots = 0;
  flock->minUrgency = 0;
  flock->maxUrgency = 0;
  memset(&flock->heading, 0, sizeof(vector_t));
  memset(&flock->position, 0, sizeof(vector_t));
  return flock;
}

/* this is a little messy, but it's heirarchical. Build a player, then give
 * it a brain, then add it to the list of enemies in a flock */
void AIFlockLoadAll(aiflock_t *flock, const char *md2File, 
				const char *skinFile, int nBots){

  /* this function uses sizeX, sizeY which are globals */
  player_t *chump1, *chump2;
  ai_t *enemy;
  float sPosX, sPosY, sPosZ, sYaw;
  float centerX, centerZ;
  int i;
  
  if(nBots < 1)
    return;

  /* pick a random center somewhere on the map */
  centerX = (sizeX - 20)*IRMAX*(rand()) + 10;
  centerZ = (sizeZ - 20)*IRMAX*(rand()) + 10;
  
  /* setup the first bot and then make all others use his skin */
  sPosX = 20*UNIRAND(1.0) + centerX;
  sPosZ = 20*UNIRAND(1.0) + centerZ;
  sPosY = TerrainGetHeight(ground, sPosX, sPosZ) + MOD_HEIGHT2;
  sYaw = (180)*IRMAX*rand();
  
  chump1 = PlayerNew(); 
  PlayerLoad(chump1, md2File, skinFile);
  PlayerSetPosition(chump1, sPosX, sPosY, sPosZ);
  PlayerSetForwardByAngle(chump1, 0, sYaw);
  PlayerSetUpVector(chump1, 0.0, 1.0, 0.0);
  PlayerSetAnimation(chump1, PLAYER_ANIM_IDLE0);
  enemy = AINew(chump1);
  enemy->threshold = (rand()%4)+1; /* give him an attack threshold between
									* one and 5 */
  EListAppend(flock->bots, enemy);
  flock->nBots++;

  /* now repeat for all other bots */
  for(i = 1; i < nBots; i++){
    sPosX = 20*(UNIRAND(1.0)) + centerX;
    sPosZ = 20*(UNIRAND(1.0)) + centerZ;
    sPosY = TerrainGetHeight(ground, sPosX, sPosZ) + MOD_HEIGHT2;
    sYaw = (180)*IRMAX*rand();
 
    chump2 = PlayerNew(); 
    PlayerLoadUseSameModel(chump2, chump1);
    PlayerSetPosition(chump2, sPosX, sPosY, sPosZ);
    PlayerSetForwardByAngle(chump2, 0, 180);
    PlayerSetUpVector(chump2, 0.0, 1.0, 0.0);
    PlayerSetAnimation(chump2, PLAYER_ANIM_IDLE0);
    enemy = AINew(chump2);
    enemy->threshold = (rand()%4)+1; 
	EListAppend(flock->bots, enemy);
    flock->nBots++;
  }

  return;
}

void AIFlockDraw(aiflock_t *flock){
  elist_t *ptr;
  /* frust is a global varaible in global.h */
  for(ptr = flock->bots->next; ptr != flock->bots; ptr = ptr->next){
	glPushMatrix();
    PlayerAnimate(ptr->bot->body, 0.2);
	glPopMatrix();
  }
}

void AIFlockDrawCulled(aiflock_t *flock){
  elist_t *ptr;
  /* frust is a global varaible in global.h */
  for(ptr = flock->bots->next; ptr != flock->bots; ptr = ptr->next){
	glPushMatrix();
    PlayerAnimateCulled(ptr->bot->body, &frust, 0.2);
	glPopMatrix();
  }
}

player_t *AIGetEnemy(void){
  return curEnemy;
}

/* also calculates average heading and position as well as confidence level */
void AIFlockSenseMates(aiflock_t *flock){
  
  elist_t *ptr1, *ptr2;
  float tempDis;
  vector_t temp;
  vector_t heading = {0, 0, 0};
  vector_t position = {0, 0, 0};
  
  /* for each flock member */ 
  for(ptr1 = flock->bots->next; ptr1 != flock->bots; ptr1 = ptr1->next){
	/* start with no confidence */
	ptr1->bot->confidence = 0;
    /* for each other flock member besides yourself */
	ptr1->bot->mate_dis = 1000000; /* just something big as an initializer */
    for(ptr2 = flock->bots->next; ptr2 != flock->bots; ptr2 = ptr2->next){
	  if(ptr1 != ptr2){
		VectorSubtract(&temp, &ptr1->bot->body->position, 
						&ptr2->bot->body->position);
        tempDis = VectorMagnitude2(&temp);
	    if(tempDis < ptr1->bot->mate_dis){
		  ptr1->bot->mate_dis = tempDis;
		  ptr1->bot->mate = ptr2->bot;
	    }
        if(tempDis < AI_CONFIDENCE_MAX){
		  ptr1->bot->confidence++;
		}		
	  } /* end check to make sure you're not looking at yourself */
    } /* end loop to look for others besides yourself */
    VectorAdd(&heading, &heading, &ptr1->bot->body->forward);
    VectorAdd(&position, &position, &ptr1->bot->body->position);
  } /* end loop for each flock member */

  /* finish average heading and position */
  VectorScale(&flock->heading, &heading, 1/flock->nBots);
  VectorScale(&flock->position, &position, 1/flock->nBots);
  flock->speed = VectorMagnitude(&flock->heading);
  VectorScale(&flock->heading, &flock->heading, 1.0/flock->speed);
}

/* under attack */
void AIScream(ai_t *bot1, ai_t *bot2){
  switch(bot2->curState){
    case AI_IDLE:
	  AIScanEnter(bot2);
	  break;
	case AI_CRUISING:
	  AIScanEnter(bot2);
	  break;
	case AI_SCAN:
	  bot2->body->yaw = bot1->body->yaw;
	default:
	  return;
  }
}


void AIFlockUpdate(aiflock_t *flock, player_t *me){
  elist_t *ptr;
  vector_t accum;
  vector_t *enemy_pos = &me->position;	

  /* must be done for all flock members */
  /* function also calculates the average flock heading and position */
  AIFlockSenseMates(flock); 

  /* needed in aistates (calles AIGetEnemy) */
  curEnemy = me;
  for(ptr = flock->bots->next; ptr != flock->bots; ptr = ptr->next){
    /* for all enemies. Note that ground is a global found in global.h */

    /* get the current state and see if we want to update */
    switch(AIGetState(ptr->bot)){
      case AI_IDLE:
	    if(AICheckFOV(ptr->bot, enemy_pos, ground)){ 
		  if(ptr->bot->confidence >= ptr->bot->threshold){
	        AIAttackEnter(ptr->bot);
			/* tell your friend to start looking around */
		    AIScream(ptr->bot, ptr->bot->mate);
		  }
		  else{
			AICruisingEnter(ptr->bot); /* he'll figure out he's scared fast */
		  }
	    }
	    else{
		  AIIdleUpdate(ptr->bot);
          AIUpdateForwardByAngle(ptr->bot);
	    }
	    break;

      case AI_CRUISING:
	    if(AICheckFOV(ptr->bot, enemy_pos, ground)){
		  if(ptr->bot->confidence >= ptr->bot->threshold){
			/* tell your friend to start looking around */
	        AIAttackEnter(ptr->bot);
		    AIScream(ptr->bot, ptr->bot->mate);
		  }
		  else{
            AIFlockApplyRulesScared(ptr->bot, &flock->heading, flock->speed, 
						 &flock->position, &accum);
			ptr->bot->timer = 0; /* never stop running if you're scared */
            AIUpdateForwardByDesire(ptr->bot, &accum);
		    AIUpdatePosition(ptr->bot);
		  }
	    }
	    else{
		  /* figure out where we would like to go */
		  AICruisingUpdate(ptr->bot);
  	      /* now adhere to the rules */
          AIFlockApplyRules(ptr->bot, &flock->heading, flock->speed, 
						 &flock->position, &accum);
          AIUpdateForwardByDesire(ptr->bot, &accum);
		  AIUpdatePosition(ptr->bot); 
		}
	    break;
	
      case AI_REPOSITION:
		AIRepositionUpdate(ptr->bot);
     /*   AIFlockApplyRules(ptr->bot, &flock->heading, flock->speed, 
						 &flock->position, &accum); */
        AIUpdateForwardByDesire(ptr->bot, &accum);
		AIUpdatePosition(ptr->bot); 
	    break;
	  
	  case AI_SCAN:
        if(AICheckFOV(ptr->bot, enemy_pos, ground)){
		  if(ptr->bot->confidence >= ptr->bot->threshold){
	        AIAttackEnter(ptr->bot);
			/* tell your friend to start looking around */
		    AIScream(ptr->bot, ptr->bot->mate);
		  }
		  else{
		    AICruisingEnter(ptr->bot); /* he'll figure out he's scared */
		  }
	    }
	    else{
	      AIScanUpdate(ptr->bot);
          AIUpdateForwardByAngle(ptr->bot);
		}
  	    break;

	  case AI_ATTACK:
        if(AICheckFOV(ptr->bot, enemy_pos, ground)){
	      AIAttackUpdate(ptr->bot);
          AIUpdateForwardByAngle(ptr->bot);
		}
	    else{
	      AIScanEnter(ptr->bot);
	    }
	    break;

      case AI_SHOT:
	    AIShotUpdate(ptr->bot);
	    break;
  	
	  case AI_DYING:
	    AIDyingUpdate(ptr->bot);
	    break;

	  case AI_DEAD:
	    AIDeadUpdate(ptr->bot);
		if(ptr->bot->timer > AI_DEAD_TIMEOUT){
		  EListDeleteNode(ptr); 
		  flock->nBots--;
		  continue;
		}
	    break;
    }

    /* check for timeout */
    if(ptr->bot->timer > ptr->bot->timeout){
      AIProcessEvent(ptr->bot, AI_TIMEOUT);
    }

    /* check for death */
    if(ptr->bot->body->health == 0){
      AIProcessEvent(ptr->bot, AI_KILLED);
    }

  }
}

void AIFlockDestroy(aiflock_t *flock){
  if(flock != NULL){
    EListFree(flock->bots);
    free(flock);
  }
}




/* enemy list related stuff... */

elist_t *EListNew(void){
  elist_t *baseNode;
  baseNode = (elist_t *)malloc(sizeof(elist_t));
  baseNode->prev = baseNode;
  baseNode->next = baseNode;
  return baseNode;
}

void EListInsertBefore(elist_t *node, ai_t *bot){
  elist_t *newNode;
  newNode = (elist_t *)malloc(sizeof(elist_t));
  newNode->bot = bot;
  
  newNode->next = node;
  newNode->prev = node->prev;
  newNode->next->prev = newNode;
  newNode->prev->next = newNode;
}

void EListInsertAfter(elist_t *node, ai_t *bot){
  EListInsertBefore(node->next, bot);
}

void EListAppend(elist_t *node, ai_t *bot){
  EListInsertBefore(node, bot);
}

void EListPrepend(elist_t *node, ai_t *bot){
  EListInsertBefore(node->next, bot);
}

void EListDeleteNode(elist_t *node){
  node->next->prev = node->prev;
  node->prev->next = node->next;
  AIDestroy(node->bot);
  free(node);
}

int EListEmpty(elist_t *node){
  return (node->next == node);
}



void EListFree(elist_t *node){
  while(!EListEmpty(node)){
    EListDeleteNode(ELISTFIRST(node));
  }
  free(node);
}

