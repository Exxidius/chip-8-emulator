#include "stdint.h"

#ifndef STACK_H
#define STACK_H

#define STACK_SIZE 16

typedef struct Stack {
  int top_idx;
  uint16_t data[STACK_SIZE];
} Stack;

int stackInit(Stack* stack);
int stackPush(Stack* stack, uint16_t e);
int stackPop(Stack* stack);

#endif

