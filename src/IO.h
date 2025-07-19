#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_scancode.h>

#include "inttypes.h"

#ifndef IO_H
#define IO_H

typedef struct IO {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Surface *surface;
  SDL_Texture *texture;
  SDL_Event event;
} IO;

int screenInit(IO* io, int width, int height);
int screenCleanup(IO* io);

void screenDraw(IO* io, uint8_t pixels[]);

int IOPoll(IO* io);
int IOcheckKeyPressed(IO* io, uint8_t VX);

uint8_t IOgetKeyPressed(IO* io);

#endif

