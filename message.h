#ifndef __MESSAGE_H__
#define __MESSAGE_H__

typedef struct MSG{
  int sender;
  int receiver;
  int name;
  int timer; 
  struct MSG *next;
} message_t;

#endif
