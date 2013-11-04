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


/* apply the cardinal rules of flocking for the normal case */
void AIFlockApplyRules(ai_t *bot, vector_t *heading, float speed, 
				vector_t *position, vector_t *accum){

  vector_t temp;
  player_t *enemy;
  float ratio;
  float sepDis = 10.0;
  float minUrgency = 0.05;
  float maxUrgency = 0.3;
  accum->x = 0.0;
  accum->y = 0.0;
  accum->z = 0.0;
  
  /* apply cruising */
  enemy = AIGetEnemy();
  VectorSubtract(&temp, &bot->destination, &bot->body->position);
  VectorSetMagnitude(&temp, minUrgency*bot->cruising); 
  VectorAdd(accum, accum, &temp);
  VectorNormalize(accum);
  
  /* apply separation */
  VectorSubtract(&temp, &bot->mate->body->position, &bot->body->position);
  
  ratio = bot->mate_dis / sepDis;
  if(ratio < minUrgency)
    ratio = minUrgency;
  if(ratio > maxUrgency)
	ratio = maxUrgency;

  ratio = ratio*bot->separation;
  
  if(bot->mate_dis < sepDis){ /* then move away, get the yaw to be opposite */
    VectorSetMagnitude(&temp, -ratio);
  }
  else if(bot->mate_dis > sepDis){ /* then move closer */
    VectorSetMagnitude(&temp, ratio);
  }
  else{ /* right on target */
    VectorClear(&temp);	
  }
  VectorAdd(accum, accum, &temp);
  VectorNormalize(accum);
  
  /* apply alignment */
  temp = bot->mate->body->forward;
  ratio = minUrgency*bot->alignment*speed;
  VectorSetMagnitude(&temp, ratio);
  VectorAdd(accum, accum, &temp);
  VectorNormalize(accum);
  
  /* apply cohesion */
  bot->destination = bot->mate->destination;
  VectorSubtract(&temp, position, &bot->body->position);
  VectorSetMagnitude(&temp, minUrgency*bot->cohesion);
  VectorAdd(accum, accum, &temp);
  VectorNormalize(accum);

  /* apply avoidance */
  /* if nearest obstacle/enemy is less than avoidance distance */
  /* compute a vector pointing away from the obstacle/enemy */
  VectorSubtract(&temp, &enemy->position, &bot->body->position);
  if(VectorMagnitude2(&temp) < 1000.0){
    VectorSetMagnitude(&temp, maxUrgency*bot->avoidance);
    VectorAdd(accum, accum, &temp);
  }
}


/* apply the cardinal rules of flocking when you're scared and running */
void AIFlockApplyRulesScared(ai_t *bot, vector_t *heading, float speed, 
				vector_t *position, vector_t *accum){

  vector_t temp;
  player_t *enemy;
  float ratio;
  float sepDis = 10.0;
  float minUrgency = 0.05;
  float maxUrgency = 0.3;
  accum->x = 0.0;
  accum->y = 0.0;
  accum->z = 0.0;
  
  enemy = AIGetEnemy();
  
  /* apply separation */
  VectorSubtract(&temp, &bot->mate->body->position, &bot->body->position);
  
  ratio = bot->mate_dis / sepDis;
  if(ratio < minUrgency)
    ratio = minUrgency;
  if(ratio > maxUrgency)
	ratio = maxUrgency;

  /* turn down the separation because if we're scared we don't care */
  ratio = 0.01*ratio*bot->separation;
  
  if(bot->mate_dis < sepDis){ /* then move away, get the yaw to be opposite */
    VectorSetMagnitude(&temp, -ratio);
  }
  else if(bot->mate_dis > sepDis){ /* then move closer */
    VectorSetMagnitude(&temp, ratio);
  }
  else{ /* right on target */
    VectorClear(&temp);	
  }
  VectorAdd(accum, accum, &temp);
  VectorNormalize(accum);
  
  /* apply alignment */
  temp = bot->mate->body->forward;

  /* turn down the separation because if we're scared we don't care */
  ratio = 0.01*minUrgency*bot->alignment*speed;
  VectorSetMagnitude(&temp, ratio);
  VectorAdd(accum, accum, &temp);
  VectorNormalize(accum);
  
  /* apply cohesion turned down */
  VectorAdd(&bot->destination, &bot->destination, &bot->mate->destination);
  VectorSubtract(&temp, position, &bot->body->position);
  VectorSetMagnitude(&temp, 0.01*minUrgency*bot->cohesion);
  VectorAdd(accum, accum, &temp);
  VectorNormalize(accum);

  /* apply avoidance */
  /* compute a vector pointing away from the obstacle/enemy */
  VectorSubtract(&temp, &enemy->position, &bot->body->position);
  VectorSetMagnitude(&temp, maxUrgency*bot->avoidance);
  VectorAdd(accum, accum, &temp);
}


/* never finished this... supposed to set up formations */
void AIFlockApplyRulesFormation(ai_t *bot, vector_t *heading, float speed, 
				vector_t *position, vector_t *accum){

  vector_t temp;
  player_t *enemy;
  float ratio;
  float sepDis = 10.0;
  float minUrgency = 0.05;
  float maxUrgency = 0.3;
  accum->x = 0.0;
  accum->y = 0.0;
  accum->z = 0.0;
  
  /* apply cruising */
  enemy = AIGetEnemy();
  VectorSubtract(&temp, &bot->destination, &bot->body->position);
  VectorSetMagnitude(&temp, minUrgency*bot->cruising); 
  VectorAdd(accum, accum, &temp);
  VectorNormalize(accum);
  
  /* apply separation */
  VectorSubtract(&temp, &bot->mate->body->position, &bot->body->position);
  
  ratio = bot->mate_dis / sepDis;
  if(ratio < minUrgency)
    ratio = minUrgency;
  if(ratio > maxUrgency)
	ratio = maxUrgency;

  ratio = ratio*bot->separation;
  
  if(bot->mate_dis < 1.5*sepDis){ /* then move away */ 
    VectorSetMagnitude(&temp, -ratio);
  }
  else if(bot->mate_dis > 1.5*sepDis){ /* then move closer */
    VectorSetMagnitude(&temp, ratio);
  }
  else{ /* right on target */
    VectorClear(&temp);	
  }
  VectorAdd(accum, accum, &temp);
  VectorNormalize(accum);
  
  /* apply cohesion by going to the correct offset */
  bot->destination = bot->mate->destination;
  VectorSubtract(&temp, position, &bot->body->position);
  VectorSetMagnitude(&temp, minUrgency*bot->cohesion);
  VectorAdd(accum, accum, &temp);
  VectorNormalize(accum);

  /* apply avoidance */
  /* if nearest obstacle/enemy is less than avoidance distance */
  /* compute a vector pointing away from the obstacle/enemy */
  VectorSubtract(&temp, &enemy->position, &bot->body->position);
  if(VectorMagnitude2(&temp) < 1000.0){
    VectorSetMagnitude(&temp, maxUrgency*bot->avoidance);
    VectorAdd(accum, accum, &temp);
  }
}
