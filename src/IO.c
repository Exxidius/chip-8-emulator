#include "stdio.h"
#include "stdbool.h"
#include "string.h"

#include "IO.h"
#include "chip8.h"

uint16_t keycodes[16] = {
  SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4,
  SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_R,
  SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_F,
  SDL_SCANCODE_Z, SDL_SCANCODE_X, SDL_SCANCODE_C, SDL_SCANCODE_V
};

uint8_t position_to_key[16] = {
  0x1, 0x2, 0x3, 0xC,
  0x4, 0x5, 0x6, 0xD,
  0x7, 0x8, 0x9, 0xE,
  0xA, 0x0, 0xB, 0xF
};

uint8_t key_to_position[16] = {
  0xD, 0x0, 0x1, 0x2,
  0x4, 0x5, 0x6, 0x8,
  0x9, 0xA, 0xC, 0xE,
  0x3, 0x7, 0xB, 0xF
};

int IOInit(IO* io, int width, int height, int debug_active) {
  memset(io->keys_pressed, 0, 16);
  io->key_pressed = -1;
  io->key_released= -1;

  io->width = width;
  io->height = height;
  io->debug_width = 0;
  io->debug_height = 0;

  if (debug_active) {
    io->debug_width += 32;
    io->debug_height += 32;
  }

  if (screenInit(io, width, height) != OK) {
    printf("Error: (IOInit) Could not initialize screen.\n");
    return ERROR;
  }

  return OK;
}

int screenInit(IO* io, int width, int height) {
  int result = SDL_Init(SDL_INIT_VIDEO);

  if (!result) {
    printf("Error: (screenInit) Couldnt initialize SDL.\n");
    return ERROR;
  }

  result = TTF_Init();
  if (result < 0) {
    printf("Error: (IOInit) Could not initialize font.");
    return ERROR;
  }

  io->font = TTF_OpenFont("res/UbuntuMono-R.ttf", 22.0f);
  if (!io->font) {
    printf("Couldn't open font.\n");
    return ERROR;
  }

  result = SDL_CreateWindowAndRenderer(
    "Chip8 Emulator",
    (width + io->debug_width) * SCALING_FACTOR,
    (height + io->debug_height) * SCALING_FACTOR,
    SDL_WINDOW_ALWAYS_ON_TOP,
    &(io->window),
    &(io->renderer)
  );

  if (!result) {
    printf("Error: (screenInit) Couldn't create renderer.\n");
    return ERROR;
  }

  return OK;
}

int screenGetPosition(IO* io, int x, int y) {
  return y * io->width + x;
}

void screenDrawRect(IO* io, int x, int y, int width, int height) {
    SDL_FRect r = {
      x * SCALING_FACTOR,
      y * SCALING_FACTOR,
      width,
      height
    };
    SDL_RenderFillRect(io->renderer, &r);
}

int screenDrawText(IO* io, const char* text, int len, SDL_FRect* pos) {
  SDL_Surface* text_s = NULL;
  SDL_Color color = { 255, 255, 255, SDL_ALPHA_OPAQUE };

  text_s = TTF_RenderText_Blended(io->font, text, len, color);
  if (text_s == NULL) {
    printf("Error: (screenDrawText) Couldn't create text.\n");
    return ERROR;
  }

  io->texture = SDL_CreateTextureFromSurface(io->renderer, text_s);
  SDL_DestroySurface(text_s);

  if (io->texture == NULL) {
    printf("Error: (screenDrawText) Couldn't create texture.\n");
    return ERROR;
  }

  SDL_GetTextureSize(io->texture, &(pos->w), &(pos->h));
  SDL_RenderTexture(io->renderer, io->texture, NULL, pos);

  return OK;
}

int screenDrawRegs(IO* io, uint8_t* registers) {
  SDL_FRect r = {
    (io->width + 1) * SCALING_FACTOR,
    SCALING_FACTOR,
    0,
    0
  };

  if (screenDrawText(io, "Registers", 9, &r) != OK) {
    goto error;
  }

  // VX: 0xVV --> 8 characters + 0 byte
  char reg[9];
  int height_init = SCALING_FACTOR + 4;
  r.y = height_init;

  for (int i = 0; i < NUMBER_REGS; i++) {
    if (i <= 7) {
      r.x = (io->width + 1) * SCALING_FACTOR + 1;
    } else {
      r.x = (io->width + 17) * SCALING_FACTOR;
    }

    if (i == 8) {
      r.y = height_init;
    }

    r.y += SCALING_FACTOR + 18;

    snprintf(reg, 9, "V%X: 0x%02X", i, registers[i]);

    if (screenDrawText(io, reg, 8, &r) != OK) {
      goto error;
    }
  }

  return OK;

  error:
    printf("Error: (screenDrawRegs) Couldn't draw Text.\n");
    return ERROR;
}

int screenDrawGeneralInfo(IO* io, DebugInformation* info) {
  SDL_FRect r = {
    SCALING_FACTOR,
    (io->height + 1) * SCALING_FACTOR,
    0,
    0
  };

  if (screenDrawText(io, "General Info", 12, &r) != OK) {
    goto error;
  }

  r.y += 4;
  char text[32] = { 0 };

  r.y += SCALING_FACTOR + 18;
  snprintf(text, 23, "PC:             0x%04X", info->PC);
  if (screenDrawText(io, text, 22, &r) != OK) {
    goto error;
  }

  r.y += SCALING_FACTOR + 18;
  snprintf(text, 23, "I:              0x%04X", info->I);
  if (screenDrawText(io, text, 22, &r) != OK) {
    goto error;
  }

  r.y += SCALING_FACTOR + 18;
  snprintf(text, 23, "Delay Timer:        %02d", info->delay_timer);
  if (screenDrawText(io, text, 22, &r) != OK) {
    goto error;
  }

  r.y += SCALING_FACTOR + 18;
  snprintf(text, 23, "Sound Timer:        %02d", info->sound_timer);
  if (screenDrawText(io, text, 22, &r) != OK) {
    goto error;
  }

  r.y += 3 * (SCALING_FACTOR + 18);
  if (info->paused) {
    if (screenDrawText(io, "Paused", 6, &r) != OK) {
      goto error;
    }
  }

  r.y += SCALING_FACTOR + 18;
  if (info->step_mode) {
    if (screenDrawText(io, "Step mode active", 16, &r) != OK) {
      goto error;
    }
  }

  return OK;

  error:
    printf("Error: (screendDrawGeneralInfo) Couldn't draw Text.\n");
    return ERROR;
}

int screenDrawInstructions(IO* io, uint16_t PC, uint8_t* memory) {
  SDL_FRect r = {
    (io->width + 1) * SCALING_FACTOR,
    (io->height + 1) * SCALING_FACTOR,
    0,
    0
  };

  if (screenDrawText(io, "Next Instructions", 17, &r) != OK) {
    goto error;
  }

  // 0xX: 0xVV --> 9 characters + 0 byte
  char mem[10];
  int height_init = (io->height + 1) * SCALING_FACTOR + 4;
  r.y = height_init;

  for (int i = 0; i < 16; i++) {
    if (i <= 7) {
      r.x = (io->width + 1) * SCALING_FACTOR + 1;
    } else {
      r.x = (io->width + 17) * SCALING_FACTOR + 2;
    }

    if (i == 8) {
      r.y = height_init;
    }

    r.y += SCALING_FACTOR + 18;

    snprintf(mem, 10, "0x%X: 0x%02X", i, memory[PC + i]);

    if (screenDrawText(io, mem, 9, &r) != OK) {
      goto error;
    }
  }

  return OK;

  error:
    printf("Error: (screenDrawInstructions) Couldn't draw Text.\n");
    return ERROR;
  return OK;
}

int screenDrawCallStack(IO* io, Stack* stack) {
  SDL_FRect r = {
    (((int)(io->width / 2)) + 1) * SCALING_FACTOR,
    (io->height + 1) * SCALING_FACTOR,
    0,
    0
  };

  if (screenDrawText(io, "Stack", 5, &r) != OK) {
    goto error;
  }

  // XX: 0xXXXX --> 10 characters + 0 byte
  char data[11];
  int height_init = (io->height + 1) * SCALING_FACTOR + 4;
  r.y = height_init;

  for (int i = 0; i < STACK_SIZE; i++) {
    if (i <= 7) {
      r.x = (((int)(io->width / 2)) + 1) * SCALING_FACTOR;
    } else {
      r.x = (((int)(io->width / 2)) + 17) * SCALING_FACTOR + 2;
    }

    if (i == 8) {
      r.y = height_init;
    }

    r.y += SCALING_FACTOR + 18;

    snprintf(data, 11, "%02d: 0x%04X", i, stack->data[i]);

    if (screenDrawText(io, data, 10, &r) != OK) {
      goto error;
    }
  }

  return OK;

  error:
    printf("Error: (screenDrawCallStack) Couldn't draw Text.\n");
    return ERROR;
}

int screenDrawDebugUI(IO* io, DebugInformation* info) {
  SDL_SetRenderDrawColor(io->renderer, 0xFF, 0xFF, 0xFF, 0xFF);

  int full_width = io->width + io->debug_width;
  int full_height = io->height + io->debug_height;
  int debug_height = io->debug_height * SCALING_FACTOR;
  screenDrawRect(io, io->width, 0, 1, full_height * SCALING_FACTOR + 1);
  screenDrawRect(io, 0, io->height, full_width * SCALING_FACTOR, 1);
  screenDrawRect(io, io->width / 2, io->height, 1, debug_height);

  if (screenDrawRegs(io, info->regs) != OK) {
    printf("Error: (screenDrawDebugUI) Couldn't draw registers to screen.\n");
    return ERROR;
  }

  if (screenDrawInstructions(io, info->PC, info->memory) != OK) {
    printf("Error: (screenDrawDebugUI) Couldn't draw instrs. to screen.\n");
    return ERROR;
  }

  if (screenDrawGeneralInfo(io, info) != OK) {
    printf("Error: (screenDrawDebugUI) Couldn't draw gen. info to screen.\n");
    return ERROR;
  }

  if (screenDrawCallStack(io, info->stack) != OK) {
    printf("Error: (screenDrawDebugUI) Couldn't draw call stack to screen.\n");
    return ERROR;
  }

  return OK;
}

void screenDraw(IO* io, uint8_t pixels[]) {
  // TODO: speed me up --> no pixel per pixel drawing
  SDL_SetRenderDrawColor(io->renderer, 0x00, 0x00, 0x00, 0x00);
  SDL_RenderClear(io->renderer);

  for (size_t y = 0; y < io->height; y++) {
    for (size_t x = 0; x < io->width; x++) {
      uint8_t value = pixels[screenGetPosition(io, x, y)];

      if (value > 0) {
        SDL_SetRenderDrawColor(io->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
      } else {
        SDL_SetRenderDrawColor(io->renderer, 0x00, 0x00, 0x00, 0x00);
      }

      screenDrawRect(io, x, y, SCALING_FACTOR, SCALING_FACTOR);
    }
  }
}

void screenRenderPresent(IO* io) {
  SDL_RenderPresent(io->renderer);
  io->texture = NULL;
}

int screenCleanup(IO* io) {
  SDL_DestroyRenderer(io->renderer);
  SDL_DestroyWindow(io->window);

  if (io->font) {
    TTF_CloseFont(io->font);
  }

  TTF_Quit();
  SDL_Quit();

  return OK;
}

int IOPoll(IO* io) {
  bool event_outstanding = SDL_PollEvent(&(io->event));
  int result = 0;

  while (event_outstanding) {
    switch (io->event.type) {
      case SDL_EVENT_QUIT:
        return QUIT;

      case SDL_EVENT_KEY_DOWN:
        switch (io->event.key.scancode) {
          case SDL_SCANCODE_P:
            result |= PAUSE;
            break;

          case SDL_SCANCODE_M:
            result |= STEP_MODE;
            break;

          case SDL_SCANCODE_N:
            result |= SHOULD_STEP;
            break;

          case SDL_SCANCODE_ESCAPE:
            return QUIT;

          default:
            IOSetKey(io, io->event.key.scancode);
            break;
        }
        break;

      case SDL_EVENT_KEY_UP:
        IOResetKey(io, io->event.key.scancode);
        break;
    }

    event_outstanding = SDL_PollEvent(&(io->event));
  }

  return result;
}

void IOSetKey(IO* io, SDL_Scancode key) {
  for (int i = 0; i <= 0xF; i++) {
    if (keycodes[i] == key) {
      io->keys_pressed[i] = 1;
    }
  }
}

void IOResetKey(IO* io, SDL_Scancode key) {
  for (int i = 0; i <= 0xF; i++) {
    if (keycodes[i] == key) {
      io->keys_pressed[i] = 0;
      io->key_released = position_to_key[i];
    }
  }
}

int IOCheckKeyPressed(IO* io, uint8_t VX) {
    return io->keys_pressed[key_to_position[VX]];
}

int IOGetKeyPressed(IO* io) {
  for (int i = 0; i <= 0xF; i++) {
    // If the key we pressed before is the same as the last released
    // we can return it to the user and continue with execution
    if (io->key_pressed == io->key_released) {
      io->key_pressed = -1;
      return position_to_key[i];
    }

    if (io->keys_pressed[i] == 1) {
      io->key_pressed = position_to_key[i];
    }
  }
  return NO_KEY_PRESSED;
}

