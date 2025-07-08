#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"

#include "chip8.h"

#define MAX_ROM_NAME_SIZE 128

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Error: (main) No ROM to load supplied!\n");
    printf("              Usage is ./chip8-emulator <ROM file>\n");
    exit(-1);
  }

  char rom_file[MAX_ROM_NAME_SIZE] = { 0 };
  strncpy(rom_file, argv[1], MAX_ROM_NAME_SIZE);
  rom_file[MAX_ROM_NAME_SIZE - 1] = '\0';

  printf("Info: (main) ROM to load is %s\n", rom_file);

  printf("Info: (main) Starting chip8 emulator...\n");

  Emulator emulator;

  if (emulatorInit(&emulator, rom_file) != 0) {
    printf("Error: (main) Could not initialize Emulator!\n");
    exit(-1);
  }

  int result = emulatorLoop(&emulator);

  emulatorCleanup(&emulator);
  printf("Info: (main) chip8 emulator finshed with code %d\n", result);
  return result;
}
