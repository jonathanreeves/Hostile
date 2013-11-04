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

#ifndef __PROJECTILE_H__
#define __PROJECTILE_H__

#include "vector.h"
#include "aiflock.h"

#define MAX_PROJECTILES 20 /* not actually using this yet */
#define PROJ_SPEED 0.9 
#define PROJECTILE_RANGE 80

enum proj_id { /* in case we want to branch out at any time */
  P_LASER,
  P_BULLET,
  P_ROCKET
};

typedef struct {
  vector_t position;
  vector_t velocity;
  float speed;
  float traveled;
  float radius;
  int pid;
} projectile_t; 

/* doubly linked list of all projectiles */
typedef struct PROJ_NODE{
  projectile_t *proj; 
  struct PROJ_NODE *next;
  struct PROJ_NODE *prev;
} proj_node_t;


/* forward must be a unit vector, pid is who sent the projectile */
/* forward is direction, position is starting position */
projectile_t *ProjectileNew(vector_t *forward, vector_t *position, 
				             float speed, int pid);

void ProjectileDestroy(projectile_t *proj);

void ProjectileInit(projectile_t *proj);

void ProjectileDraw(projectile_t *proj);

void ProjectileUpdate(projectile_t *proj);


/* linked list-related stuff */
#define PLISTFIRST(N) ((N)->next)
#define PLISTNEXT(N) ((N)->next)
#define PLISTLAST(N) ((N)->prev)
#define PLISTPREV(N) ((N)->prev)
#define PLISTNIL(N) (N)

proj_node_t *PListNew(void);

void PListDraw(proj_node_t *node);

void PListUpdate(proj_node_t *node, player_t *me, aiflock_t *enemies[], 
				int nFlocks);

void PListInsertBefore(proj_node_t *node, projectile_t *proj);

void PListInsertAfter(proj_node_t *node, projectile_t *proj);

void PListAppend(proj_node_t *node, projectile_t *proj);

void PListPrepend(proj_node_t *node, projectile_t *proj);

void PListDeleteNode(proj_node_t *node);

int PListEmpty(proj_node_t *node);

void PListFree(proj_node_t *node);

projectile_t *PListGetProjectile(proj_node_t *node);

#endif
