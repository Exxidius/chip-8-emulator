#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Surface *surface;
SDL_Texture *texture;
SDL_Event event;

int windowInit(int width, int height)
{
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
    return 3;
  }

  if (!SDL_CreateWindowAndRenderer("Chip8 Emulator", width, height, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError());
    return 3;
  }

  if (!SDL_SetWindowResizable(window, false)) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't set window non resizable %s", SDL_GetError());
    return 3;
  }

  return 0;
}

int windowDraw() {
  SDL_PollEvent(&event);
  if (event.type == SDL_EVENT_QUIT) {
    return 1;
  }
  SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);

  return 0;
}

int windowCleanup() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  SDL_Quit();

  return 0;
}

