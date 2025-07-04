#include "stdio.h"

#include "IO.h"
#include "chip8.h"

int screenInit(IO* io, int width, int height)
{
  int result = SDL_Init(SDL_INIT_VIDEO);

  if (!result) {
    printf("Error: (screenInit) Couldnt initialize SDL.\n");
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
    printf("Error: (screenInit) Couldn't create renderer.\n");
    return -3;
  }

  printf("Info: (screenInit) IO initialized.\n");
  return 0;
}

void screenDraw(IO* io, uint8_t pixels[]) {
  SDL_SetRenderDrawColor(io->renderer, 0x00, 0x00, 0x00, 0x00);
  SDL_RenderClear(io->renderer);

  for (size_t x = 0; x < DISPLAY_WIDTH; x++) {
    for (size_t y = 0; y < DISPLAY_HEIGHT; y++) {
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

  printf("Info: (screenCleanup) SDL finished cleanup.\n");
  return 0;
}

int IOPoll(IO* io) {
  SDL_PollEvent(&(io->event));
  int result = 0;

  switch (io->event.type) {
    case SDL_EVENT_QUIT:
      printf("Info: (IOPoll) Window quit.\n");
      result = -1;
      break;

    default:
      break;
  }
  return result;
}

