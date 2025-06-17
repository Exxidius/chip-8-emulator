#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#ifndef SCREEN_H
#define SCREEN_H

typedef struct Screen {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Surface *surface;
  SDL_Texture *texture;
  SDL_Event event;
} Screen;

int screenInit(Screen* s, int width, int height);
int screenDraw(Screen* s);
int screenCleanup(Screen* s);

#endif

