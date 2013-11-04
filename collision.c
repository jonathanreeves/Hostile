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

#include "collision.h"

/* not using very complicated stuff right now...just simple sphere
 * collision although I've have paved the way for more */
int CollisionPrPl(projectile_t *proj, player_t *player){

/*  vector_t dv;
  vector_t dp; */
  vector_t temp;
  float r;
/*  float pp, pv, vv; */
  float tmin;

  VectorSubtract(&temp, &proj->position, &player->position);
  r = VectorMagnitude2(&temp);
  tmin = proj->radius - player->radius;
  return r <= (tmin*tmin);
/*  VectorScale(&temp, &player->forward, player->speed);
  VectorSubtract(&dv, &temp, &proj->velocity);
  VectorSubtract(&dp, &player->position, &proj->position);
  r = proj->radius + player->radius;

  pp = VectorMagnitude2(&dp) - r*r;
  if(pp < 0){
    return 1;
 }
  pv = VectorDot(&dp, &dv); 
  if(pv >= 0){
    return 0;
  }
  vv = VectorMagnitude2(&dv);
  if((pv + vv) <= 0 && (vv + 2*pv + pp) >= 0){
    return 0;
  }

  tmin = -pv/vv;
  return(pp + pv*tmin > 0); */
}


int CollisionPlPl(player_t *player1, player_t *player2){

/*  vector_t dv;
  vector_t dp;
  vector_t tempVel1;
  vector_t tempVel2;
  float r;
  float pp, pv, vv;
  float tmin;

  VectorScale(&tempVel1, &player1->forward, player1->speed);
  VectorScale(&tempVel2, &player2->forward, player2->speed);
  VectorSubtract(&dv, &tempVel2, &tempVel1);
  VectorSubtract(&dp, &player2->position, &player1->position);
  r = player1->radius + player2->radius;
  pp = VectorMagnitude2(&dp) - r*r;
 if(pp < 0){
    return 1;
  }
  pv = VectorDot(&dp, &dv); 
  if(pv >= 0){
    return 0;
  }
  vv = VectorMagnitude2(&dv);
  if((pv + vv) <= 0 && (vv + 2*pv + pp) >= 0){
    return 0;
  }
  tmin = -pv/vv;
  return(pp + pv*tmin > 0);*/
    return 0; 
}


int CollisionPrTe(projectile_t *proj, octree_t *ground){
  return 0;
}

