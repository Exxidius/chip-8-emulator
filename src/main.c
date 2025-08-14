#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "unistd.h"
#include "ctype.h"

#include "chip8.h"

void exitPrintUsage() {
  printf("Usage: chip8-emulator -f <path-to-rom> [-dhnox]\n");
  printf("  -d   Enable debug mode (interface).\n");
  printf("  -h   Print this help message.\n");
  printf("  -n   Enable super-chip modern mode (quirks).\n");
  printf("  -o   Enable super-chip legacy mode (quirks).\n");
  printf("  -x   Enable XO-chip mode (quirks).\n");
  exit(ERROR);
}

void exitModeCheck() {
  printf("Error: (exitModeCheck) Cannot set multiple chip modes at once.\n");
  exitPrintUsage();
}

void parseOpts(int argc, char* argv[], Options* cli_options) {
  int opt;
  int sc_modern = 0;
  int sc_legacy = 0;
  int xo_chip = 0;

  cli_options->emulator_type = CHIP8;

  while ((opt = getopt(argc, argv, "f:dhnox")) != -1) {
    switch (opt) {
      case 'f':
        cli_options->rom_file = optarg;
        break;
      case 'd':
        cli_options->debug_active = 1;
        break;
      case 'h':
        exitPrintUsage();
      case 'n':
        if (sc_legacy || xo_chip) { exitModeCheck(); }
        sc_modern= 1;
        cli_options->emulator_type = SUPER_CHIP_MODERN;
        break;
      case 'o':
        if (sc_modern || xo_chip) { exitModeCheck(); }
        sc_legacy = 1;
        cli_options->emulator_type = SUPER_CHIP_LEGACY;
        break;
      case 'x':
        if (sc_modern || sc_legacy) { exitModeCheck(); }
        xo_chip = 1;
        cli_options->emulator_type = XO_CHIP;
        break;
      case '?':
        if (optopt == 'f') {
          printf("Error: (parseOpts) No ROM to load supplied.\n");
          exitPrintUsage();
        } else if (isprint(optopt)) {
          printf("Error: (parseOpts) Unknown option `-%c'.\n", optopt);
          exitPrintUsage();
        } else {
          printf("Error: (parseOpts) Unknown option `\\x%x'.\n", optopt);
          exitPrintUsage();
        }
      default:
        printf("Error: (parseOpts) Parsing command line arguments failed.\n");
        exitPrintUsage();
    }
  }

  // TODO: remove once implemented
  if (sc_modern || sc_legacy || xo_chip) {
    printf("Error: (parseOpts) Selected version not yet implemented.\n");
    exit(ERROR);
  }

  if (cli_options->rom_file == NULL) {
    printf("Error: (parseOpts) No ROM to load supplied.\n");
    exitPrintUsage();
  }
}

int main(int argc, char* argv[]) {
  // Disable error printing from getopt - handled in here
  opterr = 0;
  Options cli_options = { 0, NULL };

  parseOpts(argc, argv, &cli_options);

  Emulator emulator;

  if (emulatorInit(&emulator, &cli_options) != OK) {
    printf("Error: (main) Could not initialize Emulator.\n");
    exit(ERROR);
  }

  int result = emulatorLoop(&emulator);

  emulatorCleanup(&emulator);
  return result;
}

