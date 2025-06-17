#include "stdio.h"
#include "stdint.h"
#include "string.h"

#include "chip8.h"

int main(int argc, char* argv[]) {
  printf("Info: (main) Starting chip8 emulator...\n");

  Emulator emulator;
  emulatorInit(&emulator);

  int result = emulatorLoop(&emulator);

  emulatorCleanup(&emulator);
  printf("Info: (main) chip8 emulator finshed with code %d\n", result);
  return result;
}
