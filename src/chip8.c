#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "time.h"
#include "stdarg.h"

#include "chip8.h"
#include "IO.h"
#include "stack.h"

extern uint8_t debug_active;

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

int emulatorInit(Emulator* emulator, char* rom_file) {
  memset(emulator->memory, 0, MEMORY_SIZE);
  memset(emulator->display, 0, DISPLAY_WIDTH * DISPLAY_HEIGHT);
  memset(emulator->regs, 0, NUMBER_REGS);

  emulator->PC = 0x200;
  emulator->I = 0;

  memcpy(emulator->memory + 0x050, font, sizeof(font));

  emulator->rom = fopen(rom_file, "rb");
  if (!emulator->rom) {
    debugPrintf("Error: (emulatorInit) Could not open ROM file\n");
    return -1;
  }

  fread(emulator->memory + 0x200, 1, MEMORY_SIZE - 0x200, emulator->rom);

  emulator->call_stack = (Stack*) malloc(sizeof(Stack));
  if (stackInit(emulator->call_stack) != 0) {
    debugPrintf("Error: (emulatorInit) Could not initialize stack.\n");
    return -1;
  }

  emulator->io = (IO*) malloc(sizeof(IO));
  if (screenInit(emulator->io, DISPLAY_WIDTH * 8, DISPLAY_HEIGHT * 8) != 0) {
    debugPrintf("Error: (emulatorInit) Could not initialize screen.\n");
    return -1;
  }

  emulator->last_time_60Hz = emulatorCurrentTime_ms();

  emulator->delay_timer = 0;
  emulator->sound_timer = 0;

  debugPrintf("Info: (emulatorInit) Emulator initialized.\n");

  screenDraw(emulator->io, emulator->display);
  emulator->running = 1;

  return 0;
}

int emulatorLoop(Emulator* emulator) {
  while (emulator->running) {
    emulatorHandleTimer(emulator);

    emulatorFetch(emulator);

    if (emulatorDecodeExecute(emulator) != 0) {
      debugPrintf("Error: (emulatorLoop) Could not execute Instruction.\n");
      return -1;
    }

    emulatorSleep_ms(1000 / INSTRUCTIONS_FREQUENCY);

    if (IOPoll(emulator->io) != 0) {
      emulator->running = 0;
    }
  }
  return 0;
}

void emulatorFetch(Emulator* emulator) {
  uint8_t higher_byte = emulator->memory[emulator->PC];
  uint8_t lower_byte = emulator->memory[emulator->PC + 0x1];

  emulator->current_instruction = (higher_byte << 8) | lower_byte;
  emulator->PC += 0x2;
}

int emulatorDecodeExecute(Emulator* emulator) {
  uint16_t first_nibble = (emulator->current_instruction & 0xF000) >> 12;
  uint16_t X = (emulator->current_instruction & 0x0F00) >> 8;
  uint16_t Y = (emulator->current_instruction & 0x00F0) >> 4;
  uint16_t N = emulator->current_instruction & 0x000F;
  uint16_t NN = emulator->current_instruction & 0x00FF;
  uint16_t NNN = emulator->current_instruction & 0x0FFF;

  debugPrintf(
    "Info: (emulatorExecute) Current instruction is %04X.\n",
    emulator->current_instruction
  );

  switch (first_nibble) {
    case 0x0:
      switch (emulator->current_instruction) {
        case 0x00E0:
          debugPrintf("Info: (emulatorExecute) Clear screen\n");
          memset(emulator->display, 0, DISPLAY_HEIGHT * DISPLAY_WIDTH);
          break;

        case 0x00EE:
          debugPrintf("Info: (emulatorExecute) Jump to %X.\n", emulator->PC);
          emulator->PC = stackPop(emulator->call_stack);
          break;

        default:
          debugPrintf("Error: (emulatorExecute) Not a valid instruction. Aborting.\n");
          return -1;
      }
      break;

    case 0x1:
      debugPrintf("Info: (emulatorExecute) Jump to %X\n", NNN);
      emulator->PC = NNN;
      break;

    case 0x2:
      debugPrintf("Info: (emulatorExecute) Calling to %X.\n", emulator->PC);
      stackPush(emulator->call_stack, emulator->PC);
      emulator->PC = NNN;
      break;

    case 0x3:
      if (emulator->regs[X] == NN) {
        debugPrintf("Info: (emulatorExecute) Skipping VX == NN.\n");
        emulator->PC += 0x2;
      }
      break;

    case 0x4:
      if (emulator->regs[X] != NN) {
        debugPrintf("Info: (emulatorExecute) Skipping VX != NN.\n");
        emulator->PC += 0x2;
      }
      break;

    case 0x5:
      if (emulator->regs[X] == emulator->regs[Y]) {
        debugPrintf("Info: (emulatorExecute) Skipping VX == VY.\n");
        emulator->PC += 0x2;
      }
      break;

    case 0x6:
      debugPrintf("Info: (emulatorExecute) Set register %X to %X\n", X, NN);
      emulator->regs[X] = NN;
      break;

    case 0x7:
      debugPrintf("Info: (emulatorExecute) Add %X to register %X\n", NN, X);
      emulator->regs[X] += NN;
      break;

    case 0x8:
      switch (N) {
        case 0x0:
          debugPrintf("Info: (emulatorExecute) Set VX to VY\n");
          emulator->regs[X] = emulator->regs[Y];
          break;

        case 0x1:
          debugPrintf("Info: (emulatorExecute) VX |= VY\n");
          emulator->regs[X] |= emulator->regs[Y];
          break;

        case 0x2:
          debugPrintf("Info: (emulatorExecute) VX &= VY\n");
          emulator->regs[X] &= emulator->regs[Y];
          break;

        case 0x3:
          debugPrintf("Info: (emulatorExecute) VX ^= VY\n");
          emulator->regs[X] ^= emulator->regs[Y];
          break;

        case 0x4:
          debugPrintf("Info: (emulatorExecute) VX += VY\n");
          if ((size_t) emulator->regs[X] + (size_t) emulator->regs[Y] > 255) {
            emulator->regs[0xE] = 1;
          } else {
            emulator->regs[0xE] = 0;
          }
          emulator->regs[X] += emulator->regs[Y];
          break;

        case 0x5:
          debugPrintf("Info: (emulatorExecute) VX = VX - VY\n");
          if (emulator->regs[X] > emulator->regs[Y]) {
            emulator->regs[0xE] = 1;
          } else {
            emulator->regs[0xE] = 0;
          }
          emulator->regs[X] = emulator->regs[X] - emulator->regs[Y];
          break;

        case 0x6:
          break;

        case 0x7:
          debugPrintf("Info: (emulatorExecute) VX = VY - VX\n");
          if (emulator->regs[Y] > emulator->regs[X]) {
            emulator->regs[0xE] = 1;
          } else {
            emulator->regs[0xE] = 0;
          }
          emulator->regs[X] = emulator->regs[Y] - emulator->regs[X];
          break;

        case 0xE:
          break;

        default:
          debugPrintf("Error: (emulatorExecute) Not a valid instruction. Aborting.\n");
          return -1;
      }
      break;

    case 0x9:
      if (emulator->regs[X] != emulator->regs[Y]) {
        debugPrintf("Info: (emulatorExecute) Skipping VX != VY.\n");
        emulator->PC += 0x2;
      }
      break;

    case 0xA:
      debugPrintf("Info: (emulatorExecute) Set I to %X\n", NNN);
      emulator->I = NNN;
      break;

    case 0xB:
      break;

    case 0xC:
      break;

    case 0xD:
      debugPrintf("Info: (emulatorExecute) Display sprite\n");
      emulatorDisplay(emulator, X, Y, N);
      break;

    case 0xE:
      break;

    case 0xF:
      break;

    default:
      debugPrintf("Error: (emulatorExecute) Not a valid instruction. Aborting.\n");
      return -1;
  }
  return 0;
}

void emulatorDisplay(Emulator* emulator, uint16_t X, uint16_t Y, uint16_t N) {
  uint16_t disp_x = emulator->regs[X] % DISPLAY_WIDTH;
  uint16_t disp_y = emulator->regs[Y] % DISPLAY_HEIGHT;

  emulator->regs[NUMBER_REGS - 1] = 0;

  for (int i = 0; i < N; i++) {
    uint8_t byte = emulator->memory[emulator->I + i];

    for (int byte_i = 7; byte_i >= 0; byte_i--) {
      uint8_t bit = (byte >> byte_i) & 1;

      size_t screen_offset = disp_y * DISPLAY_WIDTH + disp_x;

      if (bit == 1 && emulator->display[screen_offset] == 1) {
        emulator->regs[NUMBER_REGS - 1] = 1;
      }

      emulator->display[screen_offset] ^= bit;

      if (disp_x == DISPLAY_WIDTH - 1) {
        break;
      }

      debugPrintf("%d, %d\n",disp_x, disp_y);
      disp_x++;
    }

    if (disp_y == DISPLAY_HEIGHT - 1) {
      break;
    }

    disp_x = emulator->regs[X] % DISPLAY_WIDTH;
    disp_y++;
  }

  screenDraw(emulator->io, emulator->display);
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

  debugPrintf("Info: (emulatorCleanup) Emulator finished cleanup.\n");
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

int debugPrintf(const char* format, ...) {
  if (debug_active) {
    int result;
    va_list va;
    va_start(va, format);
    result = vprintf(format, va);
    va_end(va);
    return result;
  }
  return 0;
}

