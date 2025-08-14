#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "inttypes.h"
#include "stack.h"

#ifndef IO_H
#define IO_H

#define SCALING_FACTOR 8

typedef struct IO {
  // SDL Handling
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Surface *surface;
  SDL_Texture *texture;
  SDL_Event event;

  // Font handling
  TTF_Font *font;
  SDL_Texture* font_texture;

  uint8_t keys_pressed[16];
  int key_pressed;
  int key_released;

  // Window information
  int width;
  int height;
  int debug_width;
  int debug_height;
} IO;

typedef struct DebugInformation {
  uint8_t* memory;
  uint8_t* regs;
  Stack* stack;
  uint16_t PC;
  uint16_t I;
  uint8_t delay_timer;
  uint8_t sound_timer;
  uint8_t paused;
  uint8_t step_mode;
} DebugInformation;

int screenInit(IO* io, int width, int height);
int screenCleanup(IO* io);
int screenGetPosition(IO* io, int x, int y);
int screenDrawText(IO* io, const char* text, int len, SDL_FRect* pos);
int screenDrawDebugUI(IO* io, DebugInformation* info);
int screenDrawRegs(IO* io, uint8_t* registers);
int screenDrawInstructions(IO* io, uint16_t PC, uint8_t* memory);
int screenDrawCallStack(IO* io, Stack* stack);
int screendDrawGeneralInfo(IO* io, DebugInformation* info);

void screenDraw(IO* io, uint8_t pixels[]);
void screenDrawRect(IO* io, int x, int y, int width, int height);
void screenRenderPresent(IO* io);

int IOInit(IO* io, int width, int height, int debug_active);
int IOPoll(IO* io);
int IOCheckKeyPressed(IO* io, uint8_t VX);
int IOGetKeyPressed(IO* io);

void IOSetKey(IO* io, SDL_Scancode key);
void IOResetKey(IO* io, SDL_Scancode key);

#endif

