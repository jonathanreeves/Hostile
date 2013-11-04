#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "player.h"

/* note that events for players are significantly less compilcated and
 * only require a few basic steps no matter what state you're in. Thus while
 * the flow looks a lot like the ai (see all ai files), it's much simpler */

/* if we want to be thrown an event from the outside world */
void PlayerProcessEvent(player_t *player, int event){
  
  switch(event){
    case PLAYER_RUNNING: 
	  if(player->state == PLAYER_READY){
		player->timer = 0;
	    PlayerSetAnimation(player, PLAYER_ANIM_RUN);
		return;
	  }
	  else if(player->state == PLAYER_SHOT){
		return;
	  }
	  else if(player->state == PLAYER_DYING){
	    return;
	  }
	  else if(player->state == PLAYER_DEAD){
	    return;
	  }
	  break;
    case PLAYER_JUMPING: 
	  if(player->state == PLAYER_READY){
		player->timer = 0;
	    PlayerSetAnimation(player, PLAYER_ANIM_JUMP);
		return;
	  }
	  else if(player->state == PLAYER_SHOT){
		return;
	  }
	  else if(player->state == PLAYER_DYING){
	    return;
	  }
	  else if(player->state == PLAYER_DEAD){
	    return;
	  }
	  break;
    case PLAYER_IDLE: 
	  if(player->state == PLAYER_READY){
		player->timer = 0;
	    PlayerSetAnimation(player, PLAYER_ANIM_IDLE0);
		return;
	  }
	  else if(player->state == PLAYER_SHOT){
		return;
	  }
	  else if(player->state == PLAYER_DYING){
	    return;
	  }
	  else if(player->state == PLAYER_DEAD){
	    return;
	  }
	  break;

	case PLAYER_GOT_SHOT: 
	  player->health--;
	  if(player->state == PLAYER_READY){
		PlayerReset(player);
	    player->state = PLAYER_SHOT;
		player->timer = 0;
	    PlayerSetAnimation(player, PLAYER_ANIM_SHOT0);
		player->timeout = P_SHOT_TIMEOUT;
		return;
	  }
	  else if(player->state == PLAYER_SHOT){
		player->timer = 0;
		PlayerReset(player);
		return;
	  }
	  else if(player->state == PLAYER_DYING){
	    return;
	  }
	  else if(player->state == PLAYER_DEAD){
	    return;
	  }
	  break;
	case PLAYER_KILLED: 
	  if(player->state == PLAYER_READY){
		PlayerReset(player);
	    player->state = PLAYER_DYING;
		player->timer = 0;
		player->health = -1; /* means you're really dead */
		player->radius = 0; /* means you can't be shot anymore */
	    PlayerSetAnimation(player, PLAYER_ANIM_DYING);
		player->timeout = P_DYING_TIMEOUT;
		return;
	  }
	  else if(player->state == PLAYER_SHOT){
		PlayerReset(player);
	    player->state = PLAYER_DYING;
		player->timer = 0;
	    PlayerSetAnimation(player, PLAYER_ANIM_DYING);
		player->timeout = P_DYING_TIMEOUT;
		return;
	  }
	  else if(player->state == PLAYER_DYING){
	    return;
	  }
	  else if(player->state == PLAYER_DEAD){
	    return;
	  }
	  break;
   
	case PLAYER_TIMEOUT: 
	  if(player->state == PLAYER_READY){
		player->timer = 0;
		return;
	  }
	  else if(player->state == PLAYER_SHOT){
		PlayerReset(player);
	    player->state = PLAYER_READY;
		player->timer = 0;
		if(player->rFlag || player->lFlag || 
						player->fFlag || player->bFlag)
	      PlayerSetAnimation(player, PLAYER_ANIM_RUN);
		else
		  PlayerSetAnimation(player, PLAYER_ANIM_IDLE0);
		player->timeout = P_READY_TIMEOUT;
		return;
	  }
	  else if(player->state == PLAYER_DYING){
		PlayerReset(player);
		player->state = PLAYER_DEAD;
	    PlayerSetAnimation(player, PLAYER_ANIM_DEAD);
		player->timer = 0;
		player->timeout = P_DEAD_TIMEOUT;
	    return;
	  }
	  else if(player->state == PLAYER_DEAD){
		player->timer = 0;
	    return;
	  }
	  break;
  }
  return;
}

