#include "stdio.h"
#include "string.h"

#include "stack.h"

int stackInit(Stack* stack) {
  stack->top_idx = 0;
  memset(stack->data, 0, STACK_SIZE);
  printf("Info: (stackInit) Initialized stack.\n");
  return 0;
}

int stackPush(Stack* stack, uint16_t e) {
  if (stack->top_idx + 1 > STACK_SIZE) {
    printf("Error: (stackPush) Stack Overflow.\n");
    return -1;
  }

  stack->data[++stack->top_idx] = e;
  return e;
}

int stackPop(Stack* stack) {
  if (stack->top_idx == 0) {
    printf("Error: (stackPop) Stack is empty.\n");
    return -1;
  }

  return stack->data[stack->top_idx--];
}

