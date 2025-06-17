#include "stdint.h"

#include "stack.h"
#include "screen.h"

#ifndef CHIP8_H
#define CHIP8_H

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32

#define MEMORY_SIZE 4096

#define NUMBER_REGS 16

#define TIMER_TICKS_PER_SECOND 60
#define INSTRUCTIONS_PER_SECOND 700

typedef struct Emulator {
  uint8_t display[DISPLAY_WIDTH * DISPLAY_HEIGHT];
  uint8_t memory[MEMORY_SIZE];
  uint8_t regs[NUMBER_REGS];

  uint8_t delay_timer;
  uint8_t sound_timer;

  uint16_t PC;
  uint16_t I;

  Stack* call_stack;
  Screen* screen;
} Emulator;

int emulatorInit(Emulator* emulator);
int emulatorLoop(Emulator* emulator);
int emulatorCleanup(Emulator* emulator);

#endif

