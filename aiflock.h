#ifndef __AIFLOCK_H__
#define __AIFLOCK_H__

#include "ai.h"
#include "player.h"

/* a linked likst of enemy bots */
typedef struct ELIST{
  ai_t *bot;
  struct ELIST *next;
  struct ELIST *prev;
} elist_t;


/* a flock structure for controlling enemy behavior */
typedef struct {
  elist_t *bots;
  int nBots;
  float maxUrgency;
  float minUrgency;
  float speed; /* average speed of the flock (magnitude squared of heading) */
  vector_t heading; /* average heading of the flock */
  vector_t position; /* average position of the flock */
} aiflock_t;


aiflock_t *AIFlockNew(void);

void AIFlockLoadAll(aiflock_t *flock, const char *md2File, 
				       const char *skinFile, int nBots);

void AIFlockDraw(aiflock_t *flock);

void AIFlockDrawCulled(aiflock_t *flock);

void AIFlockUpdate(aiflock_t *flock, player_t *me);

void AIScream(ai_t *bot1, ai_t *bot2);

/* apply the four cardinal rules of flocking to an individual bot 
 * (separation, alignment, cohesion, avoidance) store result in the
 * accumulator provided. Note that heading and position are flock average
 * and speed is magnitude of heading */
void AIFlockApplyRules(ai_t *bot, vector_t *heading, float speed, 
				vector_t *position, vector_t *accum);

void AIFlockApplyRulesScared(ai_t *bot, vector_t *heading, float speed, 
				vector_t *position, vector_t *accum);

void AIFlockApplyRulesFormation(ai_t *bot, vector_t *heading, float speed, 
				vector_t *position, vector_t *accum);

void AIFlockDestroy(aiflock_t *flock);

/* find each boid's closest flock mate and get average heading/position, as
 * well as determine your confidence level */
void AIFlockSenseMates(aiflock_t *flock);


/* stuff for the bot list contained in the flock */
#define ELISTFIRST(N) ((N)->next)
#define ELISTNEXT(N) ((N)->next)
#define ELISTLAST(N) ((N)->prev)
#define ELISTPREV(N) ((N)->prev)
#define ELISTNIL(N) (N)

elist_t *EListNew(void);

void EListInsertBefore(elist_t *node, ai_t *bot);

void EListInsertAfter(elist_t *node, ai_t *bot);

void EListAppend(elist_t *node, ai_t *bot);

void EListPrepend(elist_t *node, ai_t *bot);

void EListDeleteNode(elist_t *node);

int EListEmpty(elist_t *node);

void EListFree(elist_t *node);


#endif 
