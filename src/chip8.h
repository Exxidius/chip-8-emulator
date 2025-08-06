#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "stack.h"
#include "IO.h"

#ifndef CHIP8_H
#define CHIP8_H

// Emulator releated
#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32
#define MEMORY_SIZE 4096
#define MEMORY_OFFSET 0x050
#define NUMBER_REGS 16
#define TIMER_FREQUENCY 60
#define INSTRUCTIONS_FREQUENCY 1000

#define TRUE 1
#define FALSE 0

// Return Codes
#define ERROR -1
#define NO_KEY_PRESSED -2
#define QUIT -3
#define OK 0

// Values get ored together for return
// from input key handling
#define PAUSE 2
#define STEP_MODE 4
#define SHOULD_STEP 8

typedef enum {
  CHIP8,
  SUPER_CHIP_MODERN,
  SUPER_CHIP_LEGACY,
  XO_CHIP,
} emulator_t;

typedef struct options {
  uint8_t debug_active;
  char* rom_file;
  emulator_t emulator_type;
} Options;

typedef struct Emulator {
  uint8_t display[DISPLAY_WIDTH * DISPLAY_HEIGHT];
  uint8_t memory[MEMORY_SIZE];
  uint8_t regs[NUMBER_REGS];

  uint8_t delay_timer;
  uint8_t sound_timer;
  size_t last_time_60Hz;

  uint8_t running;
  uint8_t paused;
  uint8_t step_mode;
  uint8_t should_step;

  FILE* rom_fd;

  uint16_t PC;
  uint16_t I;

  uint16_t current_instruction;

  Stack* call_stack;
  IO* io;

  Options* cli_options;

  int acc;
} Emulator;


int emulatorInit(Emulator* emulator, Options* cli_options);
int emulatorLoop(Emulator* emulator);
int emulatorDraw(Emulator* emulator);
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
int OpCode0x5(Emulator* emulator, uint16_t X, uint16_t Y, uint16_t N);
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

#endif

