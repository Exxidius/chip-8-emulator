#ifndef CHIP8_H
#define CHIP8_H

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32

#define STACK_SIZE 16

#define TIMER_TICKS_PER_SECOND 60
#define INSTRUCTIONS_PER_SECOND 700

int emulatorInit();
int emulatorLoop();

#endif

