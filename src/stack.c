#include "string.h"

#include "stack.h"
#include "chip8.h"

int stackInit(Stack* stack) {
  stack->top_idx = 0;
  memset(stack->data, 0, STACK_SIZE * sizeof(uint16_t));
  return OK;
}

int stackPush(Stack* stack, uint16_t e) {
  if (stack->top_idx + 1 > STACK_SIZE) {
    printf("Error: (stackPush) Stack Overflow.\n");
    return ERROR;
  }

  stack->data[++stack->top_idx] = e;
  return e;
}

int stackPop(Stack* stack) {
  if (stack->top_idx == 0) {
    printf("Error: (stackPop) Stack is empty.\n");
    return ERROR;
  }

  uint16_t e = stack->data[stack->top_idx];
  stack->data[stack->top_idx--] = 0;

  return e;
}

