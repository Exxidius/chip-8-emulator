#include "stdio.h"
#include "stdint.h"
#include "string.h"

#include "chip8.h"
#include "screen.h"

int  main(int argc, char* argv[]) {
  printf("Info: Starting chip8 emulator...\n");

  emulatorInit();

  windowInit(DISPLAY_WIDTH * 8, DISPLAY_HEIGHT * 8);

  return emulatorLoop();
}
