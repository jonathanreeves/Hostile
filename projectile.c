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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "projectile.h"
#include "collision.h"

/* forward is expected to be a unit vector */
projectile_t *ProjectileNew(vector_t *forward, vector_t *position, 
			 	                          float speed, int pid){
  projectile_t *projectile;

  projectile = (projectile_t *)malloc(sizeof(projectile_t));
  memcpy(&projectile->velocity, forward, sizeof(vector_t));
  memcpy(&projectile->position, position, sizeof(vector_t));
  VectorScale(&projectile->velocity, forward, speed);
  projectile->speed = speed;
  projectile->traveled = 0.0;
  projectile->radius = 0.1;
  projectile->pid = pid;
  return projectile;
}

void ProjectileDestroy(projectile_t *proj){
  free(proj);
}

void ProjectileInit(projectile_t *proj){
  memset(&proj->velocity, 0, sizeof(vector_t));
  memset(&proj->position, 0, sizeof(vector_t));
  proj->speed = 0.0;
  proj->traveled = 0.0;
  proj->radius = 0.0;
  proj->pid = 0;
}

void ProjectileUpdate(projectile_t *proj){
  VectorAdd(&proj->position, &proj->position, &proj->velocity);
  proj->traveled += proj->speed;
}

void ProjectileDraw(projectile_t *proj){
  glColor3f(1.0, 0.2, 0.0);
  glPushMatrix();
    glTranslatef(0.0, 0.5, 0.0);
    glTranslatef(proj->position.x, proj->position.y, proj->position.z);
    glutSolidSphere(proj->radius, 10, 10);
  glPopMatrix();	  
}

proj_node_t *PListNew(void){
  proj_node_t *baseNode;
  baseNode = (proj_node_t *)malloc(sizeof(proj_node_t));
  baseNode->prev = baseNode;
  baseNode->next = baseNode;
  return baseNode;
}

void PListDraw(proj_node_t *node){
  proj_node_t *ptr;
  for(ptr = node->next; ptr != node; ptr = ptr->next){
    ProjectileDraw(ptr->proj); 
  }
}

void PListUpdate(proj_node_t *node, player_t *me, aiflock_t *enemies[], 
				int nFlocks){
  proj_node_t *pPtr;
  elist_t *ePtr;
  int cFlag = 0; /* flag for enemy collisions */
  int i;
  
  /* for each projectile */
  for(pPtr = node->next; pPtr != node; pPtr = pPtr->next){
    if(pPtr->proj->pid == 0){ /* player 1 sent this projectile */
	  /* for each flock */
	  for(i = 0; i < nFlocks; i++){
		/* for each enemy in the flock */
	    for(ePtr = enemies[i]->bots->next; ePtr != enemies[i]->bots; 
						ePtr = ePtr->next){
	      if(CollisionPrPl(pPtr->proj, ePtr->bot->body)){
	        AIProcessEvent(ePtr->bot, AI_GOT_SHOT);
	        PListDeleteNode(pPtr);
		    cFlag++;
		    break; /* we can only hit one bot at a time */
	      }
	    }
	    if(cFlag == 0){
          ProjectileUpdate(pPtr->proj);
          if(pPtr->proj->traveled > PROJECTILE_RANGE){
	        PListDeleteNode(pPtr);
			break;
		  }
	    }
		else{
		  break;
		}
	  }
    }
    else { /* must have been sent by an ai bot */
	  if(CollisionPrPl(pPtr->proj, me)){
		PlayerProcessEvent(me, PLAYER_GOT_SHOT);
	    PListDeleteNode(pPtr);
	  }
	  else{
        ProjectileUpdate(pPtr->proj);
          if(pPtr->proj->traveled > 20.0){
	      PListDeleteNode(pPtr);
	    }
	  }
	}
  }
}

void PListInsertBefore(proj_node_t *node, projectile_t *proj){
  
  proj_node_t *newNode;
  newNode = (proj_node_t *)malloc(sizeof(proj_node_t));
  newNode->proj = proj;

  newNode->next = node;
  newNode->prev = node->prev;
  newNode->next->prev = newNode;
  newNode->prev->next = newNode;
}

void PListInsertAfter(proj_node_t *node, projectile_t *proj){
  PListInsertBefore(node->next, proj);
}
		
void PListAppend(proj_node_t *node, projectile_t *proj){
  PListInsertBefore(node, proj);
}

void PListPrepend(proj_node_t *node, projectile_t *proj){
  PListInsertBefore(node->next, proj);
}


void PListDeleteNode(proj_node_t *node){
  node->next->prev = node->prev;
  node->prev->next = node->next;
  free(node->proj);
  free(node);
}

int PListEmpty(proj_node_t *node){
  return (node->next == node);
}

void PListFree(proj_node_t *node){
  while(!PListEmpty(node)){
    PListDeleteNode(PLISTFIRST(node));
  }
  free(node);
}

projectile_t *PListGetProjectile(proj_node_t *node){
  return node->proj;
}
