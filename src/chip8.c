#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#include "chip8.h"
#include "screen.h"
#include "stack.h"

uint8_t font[80] = {
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

int emulatorInit(Emulator* emulator) {
  memset(emulator->memory, 0, MEMORY_SIZE);
  memset(emulator->display, 0, DISPLAY_WIDTH * DISPLAY_HEIGHT);
  memset(emulator->regs, 0, NUMBER_REGS);

  emulator->PC = 0;
  emulator->I = 0;

  emulator->delay_timer = 0;
  emulator->sound_timer = 0;

  memcpy(emulator->memory + 0x050, font, sizeof(font));

  emulator->call_stack = (Stack*) malloc(sizeof(Stack));
  if (stackInit(emulator->call_stack) != 0) {
    printf("Error: (emulatorInit) Could not initialize stack.\n");
    return -1;
  }

  emulator->screen = (Screen*) malloc(sizeof(Screen));
  if (screenInit(emulator->screen, DISPLAY_WIDTH * 8, DISPLAY_HEIGHT * 8) != 0) {
    printf("Error: (emulatorInit) Could not initialize screen.\n");
    return -1;
  }

  printf("Info: (emulatorInit) Emulator initialized.\n");
  return 0;
}

int emulatorLoop(Emulator* emulator) {
  while (1) {
    if (screenDraw(emulator->screen) != 0) {
      break;
    }
  }
  return 0;
}

int emulatorCleanup(Emulator* emulator) {
  free(emulator->call_stack);

  screenCleanup(emulator->screen);
  free(emulator->screen);

  printf("Info: (emulatorCleanup) Emulator finished cleanup.\n");
  return 0;
}

