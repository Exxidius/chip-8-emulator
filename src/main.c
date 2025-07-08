#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"

#include "chip8.h"

#define MAX_ROM_NAME_SIZE 128

uint8_t debug_active = 0;

int main(int argc, char* argv[]) {
  if (argc < 2) {
    debugPrintf("Error: (main) No ROM to load supplied!\n");
    debugPrintf("              Usage is ./chip8-emulator <ROM file>\n");
    exit(-1);
  }

  // TODO: check better command line parsing
  if (argc > 2 && strncmp(argv[2], "--debug", 7) == 0) {
    debug_active = 1;
  }

  char rom_file[MAX_ROM_NAME_SIZE] = { 0 };
  strncpy(rom_file, argv[1], MAX_ROM_NAME_SIZE);
  rom_file[MAX_ROM_NAME_SIZE - 1] = '\0';

  debugPrintf("Info: (main) ROM to load is %s\n", rom_file);

  debugPrintf("Info: (main) Starting chip8 emulator...\n");

  Emulator emulator;

  if (emulatorInit(&emulator, rom_file) != 0) {
    debugPrintf("Error: (main) Could not initialize Emulator!\n");
    exit(-1);
  }

  int result = emulatorLoop(&emulator);

  emulatorCleanup(&emulator);
  debugPrintf("Info: (main) chip8 emulator finshed with code %d\n", result);
  return result;
}
