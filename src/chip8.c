#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "time.h"

#include "chip8.h"
#include "IO.h"
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

  memcpy(emulator->memory + 0x050, font, sizeof(font));

  emulator->call_stack = (Stack*) malloc(sizeof(Stack));
  if (stackInit(emulator->call_stack) != 0) {
    printf("Error: (emulatorInit) Could not initialize stack.\n");
    return -1;
  }

  emulator->io = (IO*) malloc(sizeof(IO));
  if (screenInit(emulator->io, DISPLAY_WIDTH * 8, DISPLAY_HEIGHT * 8) != 0) {
    printf("Error: (emulatorInit) Could not initialize screen.\n");
    return -1;
  }

  emulator->last_time_60Hz = emulatorCurrentTime_ms();

  emulator->delay_timer = 0;
  emulator->sound_timer = 0;

  printf("Info: (emulatorInit) Emulator initialized.\n");

  screenDraw(emulator->io, emulator->display);
  emulator->running = 1;

  return 0;
}

int emulatorLoop(Emulator* emulator) {
  while (emulator->running) {
    emulatorHandleTimer(emulator);

    emulatorFetch(emulator);
    emulatorDecodeExecute(emulator);

    emulatorSleep_ms(1000 / INSTRUCTIONS_FREQUENCY);

    if (IOPoll(emulator->io) != 0) {
      emulator->running = 0;
    }
  }
  return 0;
}

void emulatorFetch(Emulator* emulator) {
  uint16_t* curr_inst = &emulator->current_instruction;
  uint8_t* inst_ptr = &emulator->memory[emulator->PC];

  memcpy(curr_inst, inst_ptr, sizeof(uint16_t));
  emulator->PC += 0x2;

  // TODO: REMOVE ME ONCE DECODE AND EXECUTE IS HERE
  if (emulator->PC > MEMORY_SIZE) { emulator->PC = 0; }
}

void emulatorDecodeExecute(Emulator* emulator) {
  uint16_t first_nibble = emulator->current_instruction & 0xF000;
  uint16_t second_nibble = emulator->current_instruction & 0x0F00;
  uint16_t third_nibble = emulator->current_instruction & 0x00F0;
  uint16_t fourth_nibble = emulator->current_instruction & 0x000F;

  switch (first_nibble) {
    case 0x0:

      break;

    case 0x1:
      break;

    case 0x2:
      break;

    case 0x3:
      break;

    case 0x4:
      break;

    case 0x5:
      break;

    case 0x6:
      break;

    case 0x7:
      break;

    case 0x8:
      break;

    case 0x9:
      break;

    case 0xA:
      break;

    case 0xB:
      break;

    case 0xC:
      break;

    case 0xD:
      break;

    case 0xE:
      break;

    case 0xF:
      break;
  }
}

void emulatorHandleTimer(Emulator* emulator) {
  int decrease = emulatorTimer60Hz(emulator);

  if (decrease == 0) {
    return;
  }

  if (emulator->delay_timer > 0) {
    emulator->delay_timer--;
  }

  if (emulator->sound_timer > 0) {
    // TODO: make beep sound if > 0
    emulator->sound_timer--;
  }
}

int emulatorTimer60Hz(Emulator* emulator) {
  size_t now = emulatorCurrentTime_ms();
  double diff = now - emulator->last_time_60Hz;

  double update_rate_ms = (double) 1000 / TIMER_FREQUENCY;

  if (diff > update_rate_ms) {
    emulator->last_time_60Hz = emulatorCurrentTime_ms();
    return 1;
  }

  return 0;
}

int emulatorCleanup(Emulator* emulator) {
  free(emulator->call_stack);

  screenCleanup(emulator->io);
  free(emulator->io);

  printf("Info: (emulatorCleanup) Emulator finished cleanup.\n");
  return 0;
}

size_t emulatorCurrentTime_ms() {
  struct timespec ts;
  timespec_get(&ts, TIME_UTC);

  return (size_t)(ts.tv_sec) * 1000 + (ts.tv_nsec / 1000000);
}

void emulatorSleep_ms(size_t ms) {
  struct timespec req;
  req.tv_sec = 0;
  req.tv_nsec = 1000000 * ms;
  nanosleep(&req, NULL);
}

