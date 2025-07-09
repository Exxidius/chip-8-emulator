#include "string.h"

#include "stack.h"
#include "chip8.h"

int stackInit(Stack* stack) {
  stack->top_idx = 0;
  memset(stack->data, 0, STACK_SIZE * sizeof(uint16_t));
  debugPrintf("Info: (stackInit) Initialized stack.\n");
  return 0;
}

int stackPush(Stack* stack, uint16_t e) {
  if (stack->top_idx + 1 > STACK_SIZE) {
    debugPrintf("Error: (stackPush) Stack Overflow.\n");
    return -1;
  }

  debugPrintf("Info: (stackPush) Pushed %X.\n", e);
  stack->data[++stack->top_idx] = e;
  return e;
}

int stackPop(Stack* stack) {
  if (stack->top_idx == 0) {
    debugPrintf("Error: (stackPop) Stack is empty.\n");
    return -1;
  }

  uint16_t e = stack->data[stack->top_idx--];
  debugPrintf("Info: (stackPush) Popped %X.\n", e);

  return e;
}

