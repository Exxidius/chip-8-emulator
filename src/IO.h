#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_scancode.h>

#include "inttypes.h"

#ifndef IO_H
#define IO_H

#define SCALING_FACTOR 8

typedef struct IO {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Surface *surface;
  SDL_Texture *texture;
  SDL_Event event;
  uint8_t keys_pressed[16];
  int key_pressed;
  int key_released;
  int width;
  int height;
} IO;

int screenInit(IO* io, int width, int height);
int screenCleanup(IO* io);

void screenDraw(IO* io, uint8_t pixels[]);

int IOInit(IO* io, int width, int height);
int IOPoll(IO* io);
int IOCheckKeyPressed(IO* io, uint8_t VX);
int IOGetKeyPressed(IO* io);

void IOSetKey(IO* io, SDL_Scancode key);
void IOResetKey(IO* io, SDL_Scancode key);

#endif

