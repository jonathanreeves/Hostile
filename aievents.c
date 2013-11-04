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


/* if we want to throw an event from the outside world */
void AIProcessEvent(ai_t *brain, int event){
  
  int state = brain->curState;

  brain->prevState = state;
  switch(event){

	case AI_GOT_SHOT: 
	  brain->body->health--;
	  if(state == AI_IDLE){
	    AIShotEnter(brain);
	  }
      else if(state == AI_CRUISING){
	    AIShotEnter(brain);
	  }
	  else if(state == AI_REPOSITION){
	    AIShotEnter(brain);
	  }
	  else if(state == AI_SCAN){
	    AIShotEnter(brain);
	  }
	  else if(state == AI_SHOT){
	    AIShotEnter(brain); /* each time you get shot you start over */
	  }
	  else if(state == AI_ATTACK){
	    AIShotEnter(brain);
	  }
	  break;

	case AI_IN_FOV:
      if(state == AI_IDLE){
	    AIAttackEnter(brain);
	  }
      else if(state == AI_CRUISING){
	    AIAttackEnter(brain);
	  }
	  else if(state == AI_REPOSITION){
	    AIAttackEnter(brain);
	  }
	  else if(state == AI_SCAN){
	    AIAttackEnter(brain);
	  }
	  else if(state == AI_ATTACK){
	    AIAttackUpdate(brain);
	  }
	  else if(state == AI_SHOT){
	    AIShotUpdate(brain);
	  }
	  else if(state == AI_DYING){
	    AIDyingUpdate(brain);
	  }
	  else if(state == AI_DEAD){
	    AIDeadUpdate(brain);
	  }
	  break;

	case AI_OUT_FOV:
	  
	  if(state == AI_IDLE){
	    AIIdleUpdate(brain);
	  }
      else if(state == AI_CRUISING){
	    AICruisingUpdate(brain);
	  }
	  else if(state == AI_REPOSITION){
	    AIRepositionUpdate(brain);
	  }
	  else if(state == AI_SCAN){
	    AIScanUpdate(brain);
	  }
      else if(state == AI_ATTACK){
	    AIScanEnter(brain);
	  }
	  else if(state == AI_SHOT){
	    AIShotUpdate(brain);
	  }
	  else if(state == AI_DYING){
	    AIDyingUpdate(brain);
	  }
	  else if(state == AI_DEAD){
	    AIDeadUpdate(brain);
	  }
	  break;

	case AI_ARRIVED:
	  if(state == AI_IDLE){
	    AIIdleUpdate(brain);
	  }
      else if(state == AI_CRUISING){
	    AIIdleEnter(brain);
	  }
	  else if(state == AI_REPOSITION){
	    AIScanEnter(brain);
	  }
	  else if(state == AI_SCAN){
	    AIScanUpdate(brain);
	  }
      else if(state == AI_ATTACK){
	    AIAttackUpdate(brain);
	  }
	  else if(state == AI_SHOT){
	    AIShotUpdate(brain);
	  }
	  else if(state == AI_DYING){
	    AIDyingUpdate(brain);
	  }
	  else if(state == AI_DEAD){
	    AIDeadUpdate(brain);
	  }
	  break;

	case AI_COLLIDE:
      if(state == AI_IDLE){
	    AIIdleUpdate(brain);
	  }
      else if(state == AI_CRUISING){
	    AICruisingUpdate(brain);
	  }
	  else if(state == AI_REPOSITION){
	    AIRepositionUpdate(brain);
	  }
	  else if(state == AI_SCAN){
	    AIScanUpdate(brain);
	  }
	  else if(state == AI_ATTACK){
	    AIAttackUpdate(brain);
	  }
	  else if(state == AI_SHOT){
	    AIShotUpdate(brain);
	  }
	  else if(state == AI_DYING){
	    AIDyingUpdate(brain);
	  }
	  else if(state == AI_DEAD){
	    AIDeadUpdate(brain);
	  }
	  break;

	case AI_KILLED:
	  brain->body->radius = 0.0; /* so we can't still be shot */
	  brain->body->health = -1; /* so you're really dead */
	  AIDyingEnter(brain);
	  break;

	case AI_TIMEOUT:
	  if(state == AI_IDLE){
	    AICruisingEnter(brain);
	  }
      else if(state == AI_CRUISING){
	    AIIdleEnter(brain);
	  }
	  else if(state == AI_REPOSITION){
	    AIScanEnter(brain);
	  }
	  else if(state == AI_SCAN){
	    AICruisingEnter(brain);
	  }
	  else if(state == AI_ATTACK){
	    AIRepositionEnter(brain);
	  }
	  else if(state == AI_SHOT){
	    AIScanEnter(brain);
	  }
	  else if(state == AI_DYING){
	    AIDeadEnter(brain);
	  }
	  else if(state == AI_DEAD){
	    AIDeadUpdate(brain);
	  }
	  break;

	default:
	  break;
  }

  return;
}

