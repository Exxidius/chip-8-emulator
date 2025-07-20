#include "stdio.h"
#include "stdbool.h"

#include "IO.h"
#include "chip8.h"

uint16_t keycodes[16] = {
  SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4,
  SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_R,
  SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_F,
  SDL_SCANCODE_Z, SDL_SCANCODE_X, SDL_SCANCODE_C, SDL_SCANCODE_V
};

int screenInit(IO* io, int width, int height)
{
  int result = SDL_Init(SDL_INIT_VIDEO);

  if (!result) {
    debugPrintf("Error: (screenInit) Couldnt initialize SDL.\n");
    return -3;
  }

  result = SDL_CreateWindowAndRenderer(
    "Chip8 Emulator",
    width,
    height,
    SDL_WINDOW_ALWAYS_ON_TOP,
    &(io->window),
    &(io->renderer)
  );

  if (!result) {
    debugPrintf("Error: (screenInit) Couldn't create renderer.\n");
    return -3;
  }

  debugPrintf("Info: (screenInit) IO initialized.\n");
  return 0;
}

void screenDraw(IO* io, uint8_t pixels[]) {
  SDL_SetRenderDrawColor(io->renderer, 0x00, 0x00, 0x00, 0x00);
  SDL_RenderClear(io->renderer);

  for (size_t y = 0; y < DISPLAY_HEIGHT; y++) {
    for (size_t x = 0; x < DISPLAY_WIDTH; x++) {
      uint8_t value = pixels[y * DISPLAY_WIDTH + x];

      if (value > 0) {
        SDL_SetRenderDrawColor(io->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
      } else {
        SDL_SetRenderDrawColor(io->renderer, 0x00, 0x00, 0x00, 0x00);
      }

      SDL_FRect r = { x * 8, y * 8, 8, 8};
      SDL_RenderFillRect(io->renderer, &r);
    }
  }
  SDL_RenderPresent(io->renderer);
}

int screenCleanup(IO* io) {
  SDL_DestroyRenderer(io->renderer);
  SDL_DestroyWindow(io->window);

  SDL_Quit();

  debugPrintf("Info: (screenCleanup) SDL finished cleanup.\n");
  return 0;
}

int IOPoll(IO* io) {
  bool event_outstanding = SDL_PollEvent(&(io->event));

  while (event_outstanding) {
    switch (io->event.type) {
      case SDL_EVENT_QUIT:
        debugPrintf("Info: (IOPoll) Window quit.\n");
        return -1;
        break;

      case SDL_EVENT_KEY_DOWN:
        debugPrintf("Info: (IOPoll) Key down event.\n");
        IOSetKey(io, io->event.key.scancode);
        break;

      case SDL_EVENT_KEY_UP:
        debugPrintf("Info: (IOPoll) Key down event.\n");
        IOSetKey(io, io->event.key.scancode);
        break;

      default:
        break;
    }

    event_outstanding = SDL_PollEvent(&(io->event));
  }
  return 0;
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
    }
  }
}

int IOCheckKeyPressed(IO* io, uint8_t VX) {
  if (io->keys_pressed[VX] == 1) {
    return 1;
  } else {
    return 0;
  }
}

int IOGetKeyPressed(IO* io) {
  for (int i = 0; i <= 0xF; i++) {
    if (io->keys_pressed[i] == 1) {
      return i;
    }
  }
  return -1;
}

