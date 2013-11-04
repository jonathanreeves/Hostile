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

#ifndef __COLLISION_H__
#define __COLLISION_H__

#include "octree.h"
#include "player.h"
#include "projectile.h"

/* projectile -> player */
int CollisionPrPl(projectile_t *projectile, player_t *player);

/* player -> player */
int CollisionPlPl(player_t *player1, player_t *player2);

/* player -> terrain */
int CollisionPrTe(projectile_t *proj, octree_t *ground);

#define CollisionPlPr(X, Y) CollisionPrPl(Y, X)
#define CollisionPrTe(X, Y) CollisionTePr(Y, X)

#endif
