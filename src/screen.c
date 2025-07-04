#include "stdio.h"

#include "screen.h"
#include "chip8.h"

int screenInit(Screen* s, int width, int height)
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
    &(s->window),
    &(s->renderer)
  );

  if (!result) {
    printf("Error: (screenInit) Couldn't create renderer.\n");
    return -3;
  }

  printf("Info: (screenInit) Screen initialized.\n");
  return 0;
}

int screenDraw(Screen* s, uint8_t pixels[]) {
  SDL_PollEvent(&(s->event));
  if ((s->event).type == SDL_EVENT_QUIT) {
    printf("Info: (screenDraw) Window quit.\n");
    return -1;
  }

  SDL_SetRenderDrawColor(s->renderer, 0x00, 0x00, 0x00, 0x00);
  SDL_RenderClear(s->renderer);

  for (size_t x = 0; x < DISPLAY_WIDTH; x++) {
    for (size_t y = 0; y < DISPLAY_HEIGHT; y++) {
      uint8_t color = pixels[y * DISPLAY_WIDTH + x];

    }
  }

  SDL_RenderPresent(s->renderer);

  return 0;
}

int screenCleanup(Screen* s) {
  SDL_DestroyRenderer(s->renderer);
  SDL_DestroyWindow(s->window);

  SDL_Quit();

  printf("Info: (screenCleanup) SDL finished cleanup.\n");
  return 0;
}

