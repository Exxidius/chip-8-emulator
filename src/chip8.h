#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "stack.h"
#include "IO.h"

#ifndef CHIP8_H
#define CHIP8_H

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32
#define MEMORY_SIZE 4096
#define MEMORY_OFFSET 0x050
#define NUMBER_REGS 16
#define TIMER_FREQUENCY 60
#define INSTRUCTIONS_FREQUENCY 700

typedef struct Emulator {
  uint8_t display[DISPLAY_WIDTH * DISPLAY_HEIGHT];
  uint8_t memory[MEMORY_SIZE];
  uint8_t regs[NUMBER_REGS];

  uint8_t delay_timer;
  uint8_t sound_timer;

  uint8_t running;

  FILE* rom;

  uint16_t PC;
  uint16_t I;

  uint16_t current_instruction;

  Stack* call_stack;
  IO* io;

  // Internal Variables
  size_t last_time_60Hz;
  int acc;
} Emulator;

int emulatorInit(Emulator* emulator, char* rom_file);
int emulatorLoop(Emulator* emulator);
int emulatorCleanup(Emulator* emulator);
int emulatorTimer60Hz(Emulator* emulator);
int emulatorDecodeExecute(Emulator* emulator);

void emulatorFetch(Emulator* emulator);
void emulatorStore(Emulator* emulator, uint16_t X);
void emulatorLoad(Emulator* emulator, uint16_t X);

size_t emulatorCurrentTime_ms();

void emulatorHandleTimer(Emulator* emulator);
void emulatorSleep_ms(size_t ms);

int OpCode0x0(Emulator* emulator);
int OpCode0x1(Emulator* emulator, uint16_t NNN);
int OpCode0x2(Emulator* emulator, uint16_t NNN);
int OpCode0x3(Emulator* emulator, uint16_t X, uint16_t NN);
int OpCode0x4(Emulator* emulator, uint16_t X, uint16_t NN);
int OpCode0x5(Emulator* emulator, uint16_t X, uint16_t Y);
int OpCode0x6(Emulator* emulator, uint16_t X, uint16_t NN);
int OpCode0x7(Emulator* emulator, uint16_t X, uint16_t NN);
int OpCode0x8(Emulator* emulator, uint16_t X, uint16_t Y, uint16_t N);
int OpCode0x9(Emulator* emulator, uint16_t X, uint16_t Y);
int OpCode0xA(Emulator* emulator, uint16_t NNN);
int OpCode0xB(Emulator* emulator, uint16_t NNN);
int OpCode0xC(Emulator* emulator, uint16_t X, uint16_t NN);
int OpCode0xD(Emulator* emulator, uint16_t X, uint16_t Y, uint16_t N);
int OpCode0xE(Emulator* emulator, uint16_t X, uint16_t NN);
int OpCode0xF(Emulator* emulator, uint16_t X, uint16_t NN);

int debugPrintf(const char* format, ...);

#endif

