#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "ctype.h"

#include "chip8.h"

#define MAX_ROM_NAME_SIZE 128

uint8_t debug_active = 0;

int main(int argc, char* argv[]) {
  // Disable error printing from getopt - handled in here
  opterr = 0;

  int opt;
  char* rom_file = NULL;

  while ((opt = getopt(argc, argv, "f:d")) != -1) {
    switch (opt) {
      case 'f':
        rom_file = optarg;
        break;
      case 'd':
        debug_active = 1;
        break;
      case '?':
        if (optopt == 'f') {
          printf("Error: (main) No ROM to load supplied.\n");
          exit(ERROR);
        } else if (isprint(optopt)) {
          printf("Error: (main) Unknown option `-%c'.\n", optopt);
          exit(ERROR);
        } else {
          printf("Error: (main) Unknown option character `\\x%x'.\n", optopt);
          exit(ERROR);
        }
      default:
        printf("Error: (main) Could not parse command line arguments!\n");
        exit(ERROR);
    }
  }

  if (rom_file == NULL) {
    printf("Error: (main) No ROM to load supplied.\n");
    exit(ERROR);
  }

  debugPrintf("Info: (main) ROM to load is %s\n", rom_file);
  debugPrintf("Info: (main) Starting chip8 emulator...\n");

  Emulator emulator;

  if (emulatorInit(&emulator, rom_file) != 0) {
    printf("Error: (main) Could not initialize Emulator!\n");
    exit(ERROR);
  }

  int result = emulatorLoop(&emulator);

  emulatorCleanup(&emulator);
  debugPrintf("Info: (main) chip8 emulator finshed with code %d\n", result);
  return result;
}
