#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "time.h"
#include "stdarg.h"

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

int emulatorInit(Emulator* emulator, Options* cli_options) {
  srand(time(NULL));

  emulator->cli_options = cli_options;

  memset(emulator->memory, 0, MEMORY_SIZE);
  memset(emulator->display, 0, DISPLAY_WIDTH * DISPLAY_HEIGHT);
  memset(emulator->regs, 0, NUMBER_REGS);

  emulator->PC = 0x200;
  emulator->I = 0;

  memcpy(emulator->memory + MEMORY_OFFSET, font, sizeof(font));

  emulator->rom_fd = fopen(emulator->cli_options->rom_file, "rb");
  if (!emulator->rom_fd) {
    printf("Error: (emulatorInit) Could not open ROM file\n");
    return ERROR;
  }

  fread(emulator->memory + 0x200, 1, MEMORY_SIZE - 0x200, emulator->rom_fd);

  emulator->call_stack = (Stack*) malloc(sizeof(Stack));
  if (stackInit(emulator->call_stack) != 0) {
    printf("Error: (emulatorInit) Could not initialize stack.\n");
    return ERROR;
  }

  emulator->io = (IO*) malloc(sizeof(IO));
  if (IOInit(
        emulator->io,
        DISPLAY_WIDTH,
        DISPLAY_HEIGHT,
        emulator->cli_options->debug_active
    ) != 0) {
    printf("Error: (emulatorInit) Could not initialize IO.\n");
    return ERROR;
  }

  emulator->last_time_60Hz = emulatorCurrentTime_ms();

  emulator->delay_timer = 0;
  emulator->sound_timer = 0;

  screenDraw(emulator->io, emulator->display);
  emulator->running = 1;
  emulator->paused = 0;

  return OK;
}

int emulatorLoop(Emulator* emulator) {
  while (emulator->running) {
    emulatorHandleTimer(emulator);

    emulatorFetch(emulator);

    if (emulatorDecodeExecute(emulator) != 0) {
      printf("Error: (emulatorLoop) Could not execute Instruction.\n");
      return ERROR;
    }

    emulatorSleep_ms((float)1000 / INSTRUCTIONS_FREQUENCY);

    if (IOPoll(emulator->io) == QUIT) {
      emulator->running = 0;
    }
  }
  return OK;
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

  switch (first_nibble) {
    case 0x0:
      if (OpCode0x0(emulator) != 0) {
        printf("Error: (emulatorDecodeExecute) OpCode0x0 failed.\n");
        return ERROR;
      }
      break;
    case 0x1:
      OpCode0x1(emulator, NNN);
      break;
    case 0x2:
      OpCode0x2(emulator, NNN);
      break;
    case 0x3:
      OpCode0x3(emulator, X, NN);
      break;
    case 0x4:
      OpCode0x4(emulator, X, NN);
      break;
    case 0x5:
      if (OpCode0x5(emulator, X, Y, N) != 0) {
        printf("Error: (emulatorDecodeExecute) OpCode0x5 failed.\n");
        return ERROR;
      }
      break;
    case 0x6:
      OpCode0x6(emulator, X, NN);
      break;
    case 0x7:
      OpCode0x7(emulator, X, NN);
      break;
    case 0x8:
      if (OpCode0x8(emulator, X, Y, N) != 0) {
        printf("Error: (emulatorDecodeExecute) OpCode0x8 failed.\n");
        return ERROR;
      }
      break;
    case 0x9:
      OpCode0x9(emulator, X, Y);
      break;
    case 0xA:
      OpCode0xA(emulator, NNN);
      break;
    case 0xB:
      OpCode0xB(emulator, NNN);
      break;
    case 0xC:
      OpCode0xC(emulator, X, NN);
      break;
    case 0xD:
      OpCode0xD(emulator, X, Y, N);
      break;
    case 0xE:
      if (OpCode0xE(emulator, X, NN) != 0) {
        printf("Error: (emulatorDecodeExecute) OpCode0xE failed.\n");
        return ERROR;
      }
      break;
    case 0xF:
      if (OpCode0xF(emulator, X, NN) != 0) {
        printf("Error: (emulatorDecodeExecute) OpCode0xF failed.\n");
        return ERROR;
      }
      break;
    default:
      printf("Error: (emulatorDecodeExecute) Not a valid instruction.\n");
      return ERROR;
  }
  return OK;
}

void emulatorStore(Emulator* emulator, uint16_t X) {
  for (int i = 0; i <= X; i++) {
    emulator->memory[emulator->I + i] = emulator->regs[i];
  }
}

void emulatorLoad(Emulator* emulator, uint16_t X) {
  for (int i = 0; i <= X; i++) {
    emulator->regs[i] = emulator->memory[emulator->I + i];
  }
}

void emulatorHandleTimer(Emulator* emulator) {
  int decrease = emulatorTimer60Hz(emulator);

  if (decrease == FALSE) {
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
    return TRUE;
  }

  return OK;
}

int emulatorCleanup(Emulator* emulator) {
  free(emulator->call_stack);

  screenCleanup(emulator->io);
  free(emulator->io);

  return OK;
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

int OpCode0x0(Emulator* emulator) {
  switch (emulator->current_instruction) {
    case 0x00E0:
      memset(emulator->display, 0, DISPLAY_HEIGHT * DISPLAY_WIDTH);
      break;

    case 0x00EE:
      emulator->PC = stackPop(emulator->call_stack);
      break;

    default:
      printf("Error: (OpCode0x0) Not a valid instruction. Aborting.\n");
      return ERROR;
  }
  return OK;
}

int OpCode0x1(Emulator* emulator, uint16_t NNN) {
  emulator->PC = NNN;
  return OK;
}

int OpCode0x2(Emulator* emulator, uint16_t NNN) {
  stackPush(emulator->call_stack, emulator->PC);
  emulator->PC = NNN;
  return OK;
}

int OpCode0x3(Emulator* emulator, uint16_t X, uint16_t NN) {
  if (emulator->regs[X] == NN) {
    emulator->PC += 0x2;
  }
  return OK;
}

int OpCode0x4(Emulator* emulator, uint16_t X, uint16_t NN) {
  if (emulator->regs[X] != NN) {
    emulator->PC += 0x2;
  }
  return OK;
}

int OpCode0x5(Emulator* emulator, uint16_t X, uint16_t Y, uint16_t N) {
  switch (N) {
    case 0x0:
      if (emulator->regs[X] == emulator->regs[Y]) {
        emulator->PC += 0x2;
      }
      break;
    case 0x2:
      break;
    case 0x3:
      break;
    default:
      printf("Error: (OpCode0x5) Not a valid instruction. Aborting.\n");
      return ERROR;
  }
  return OK;
}

int OpCode0x6(Emulator* emulator, uint16_t X, uint16_t NN) {
  emulator->regs[X] = NN;
  return OK;
}

int OpCode0x7(Emulator* emulator, uint16_t X, uint16_t NN) {
  emulator->regs[X] += NN;
  return OK;
}

int OpCode0x8(Emulator* emulator, uint16_t X, uint16_t Y, uint16_t N) {
  switch (N) {
    case 0x0:
      emulator->regs[X] = emulator->regs[Y];
      break;

    case 0x1:
      emulator->regs[X] |= emulator->regs[Y];
      break;

    case 0x2:
      emulator->regs[X] &= emulator->regs[Y];
      break;

    case 0x3:
      emulator->regs[X] ^= emulator->regs[Y];
      break;

    case 0x4:
      emulator->acc = emulator->regs[X] + emulator->regs[Y];
      emulator->regs[X] = emulator->acc;
      emulator->regs[0xF] = emulator->acc > UINT8_MAX ? 1 : 0;
      break;

    case 0x5:
      emulator->acc = emulator->regs[X] - emulator->regs[Y];
      emulator->regs[X] = emulator->acc;
      emulator->regs[0xF] = emulator->acc >= 0 ? 1 : 0;
      break;

    case 0x6:
      emulator->acc = emulator->regs[X];
      emulator->regs[X] >>= 0x1;
      emulator->regs[0xF] = emulator->acc & 0x1;
      break;

    case 0x7:
      emulator->acc = emulator->regs[Y] - emulator->regs[X];
      emulator->regs[X] = emulator->acc;
      emulator->regs[0xF] = emulator->acc >= 0 ? 1 : 0;
      break;

    case 0xE:
      emulator->acc = emulator->regs[X];
      emulator->regs[X] <<= 0x1;
      emulator->regs[0xF] = (emulator->acc >> 0x7) & 0x1;
      break;

    default:
      printf("Error: (OpCode0x8) Not a valid instruction. Aborting.\n");
      return ERROR;
  }
  return OK;
}

int OpCode0x9(Emulator* emulator, uint16_t X, uint16_t Y) {
  if (emulator->regs[X] != emulator->regs[Y]) {
    emulator->PC += 0x2;
  }
  return OK;
}

int OpCode0xA(Emulator* emulator, uint16_t NNN)  {
  emulator->I = NNN;
  return OK;
}

int OpCode0xB(Emulator* emulator, uint16_t NNN) {
  emulator->PC = emulator->regs[0x0] + NNN;
  return OK;
}

int OpCode0xC(Emulator* emulator, uint16_t X, uint16_t NN) {
  int random = rand() % UINT8_MAX;
  emulator->regs[X] = random & NN;
  return OK;
}

int OpCode0xD(Emulator* emulator, uint16_t X, uint16_t Y, uint16_t N) {
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

      disp_x++;
    }

    if (disp_y == DISPLAY_HEIGHT - 1) {
      break;
    }

    disp_x = emulator->regs[X] % DISPLAY_WIDTH;
    disp_y++;
  }

  screenDraw(emulator->io, emulator->display);
  return OK;
}

int OpCode0xE(Emulator* emulator, uint16_t X, uint16_t NN) {

  if (X > 0xF) {
    printf("Error: (OpCode0xE) Key not possible. Aborting.\n");
    return ERROR;
  }
  int result = IOCheckKeyPressed(emulator->io, emulator->regs[X]);

  switch (NN) {
    case 0x9E:
      if (result == 1) {
        emulator->PC += 2;
      }
      break;
    case 0xA1:
      if (result == 0) {
        emulator->PC += 2;
      }
      break;
    default:
      printf("Error: (OpCode0xE) Not a valid instruction. Aborting.\n");
      return ERROR;
  }
  return OK;
}

int OpCode0xF(Emulator* emulator, uint16_t X, uint16_t NN) {
  int result;
  switch (NN) {
    case 0x07:
      emulator->regs[X] = emulator->delay_timer;
      break;
    case 0x15:
      emulator->delay_timer = emulator->regs[X];
      break;
    case 0x18:
      emulator->sound_timer = emulator->regs[X];
      break;
    case 0x1E:
      emulator->I += emulator->regs[X];
      break;
    case 0x0A:
      // getKeyPressed only returns the key once released
      result = IOGetKeyPressed(emulator->io);
      if (result == NO_KEY_PRESSED) {
        emulator->PC -= 2;
      } else {
        emulator->regs[X] = result;
      }
      break;
    case 0x29:
      emulator->I = MEMORY_OFFSET + emulator->regs[X & 0xF] * 5;
      break;
    case 0x33:
      emulator->memory[emulator->I] = (emulator->regs[X] / 100) % 10;
      emulator->memory[emulator->I + 1] = (emulator->regs[X] / 10) % 10;
      emulator->memory[emulator->I + 2] = emulator->regs[X] % 10;
      break;
    case 0x55:
      emulatorStore(emulator, X);
      break;
    case 0x65:
      emulatorLoad(emulator, X);
      break;
    default:
      printf("Error: (OpCode0xF) Not a valid instruction. Aborting.\n");
      return ERROR;
  }
  return OK;
}

