#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "ctype.h"

#include "chip8.h"

void exitPrintUsage() {
  printf("Usage: chip8-emulator -f <path-to-rom> [-d]\n");
  exit(ERROR);
}

void parseOptions(int argc, char* argv[], Options* cli_options) {
  int opt;

  cli_options->emulator_type = CHIP8;

  while ((opt = getopt(argc, argv, "f:dnox")) != -1) {
    switch (opt) {
      case 'f':
        cli_options->rom_file = optarg;
        break;
      case 'd':
        cli_options->debug_active = 1;
        break;
      case 'o':
        cli_options->emulator_type = SUPER_CHIP_LEGACY;
        break;
      case 'n':
        cli_options->emulator_type = SUPER_CHIP_MODERN;
        break;
      case 'x':
        cli_options->emulator_type = XO_CHIP;
      case '?':
        if (optopt == 'f') {
          printf("Error: (main) No ROM to load supplied.\n");
          exitPrintUsage();
        } else if (isprint(optopt)) {
          printf("Error: (main) Unknown option `-%c'.\n", optopt);
          exitPrintUsage();
        } else {
          printf("Error: (main) Unknown option character `\\x%x'.\n", optopt);
          exitPrintUsage();
        }
      default:
        printf("Error: (main) Could not parse command line arguments!\n");
        exitPrintUsage();
    }
  }

  if (cli_options->rom_file == NULL) {
    printf("Error: (main) No ROM to load supplied.\n");
    exitPrintUsage();
  }
}

int main(int argc, char* argv[]) {
  // Disable error printing from getopt - handled in here
  opterr = 0;
  struct options cli_options = { 0, NULL };

  parseOptions(argc, argv, &cli_options);

  Emulator emulator;

  if (emulatorInit(&emulator, &cli_options) != 0) {
    printf("Error: (main) Could not initialize Emulator.\n");
    exit(ERROR);
  }

  int result = emulatorLoop(&emulator);

  emulatorCleanup(&emulator);
  return result;
}

