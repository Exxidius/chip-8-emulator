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

int IOInit(IO* io, int width, int height) {
  memset(io->keys_pressed, 0, 16);
  io->key_pressed = -1;
  io->key_released= -1;

  io->width = width;
  io->height = height;

  if (screenInit(io, width, height) != 0) {
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

  result = SDL_CreateWindowAndRenderer(
    "Chip8 Emulator",
    width * SCALING_FACTOR,
    height * SCALING_FACTOR,
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

void screenDraw(IO* io, uint8_t pixels[]) {
  SDL_SetRenderDrawColor(io->renderer, 0x00, 0x00, 0x00, 0x00);
  SDL_RenderClear(io->renderer);

  for (size_t y = 0; y < io->height; y++) {
    for (size_t x = 0; x < io->width; x++) {
      uint8_t value = pixels[y * io->width + x];

      if (value > 0) {
        SDL_SetRenderDrawColor(io->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
      } else {
        SDL_SetRenderDrawColor(io->renderer, 0x00, 0x00, 0x00, 0x00);
      }

      SDL_FRect r = {
        x * SCALING_FACTOR,
        y * SCALING_FACTOR,
        SCALING_FACTOR,
        SCALING_FACTOR
      };
      SDL_RenderFillRect(io->renderer, &r);
    }
  }
  SDL_RenderPresent(io->renderer);
}

int screenCleanup(IO* io) {
  SDL_DestroyRenderer(io->renderer);
  SDL_DestroyWindow(io->window);

  SDL_Quit();

  return OK;
}

int IOPoll(IO* io) {
  bool event_outstanding = SDL_PollEvent(&(io->event));

  while (event_outstanding) {
    switch (io->event.type) {
      case SDL_EVENT_QUIT:
        return QUIT;
        break;

      case SDL_EVENT_KEY_DOWN:
        IOSetKey(io, io->event.key.scancode);
        break;

      case SDL_EVENT_KEY_UP:
        IOResetKey(io, io->event.key.scancode);
        break;

      default:
        break;
    }

    event_outstanding = SDL_PollEvent(&(io->event));
  }
  return OK;
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

