# Chip8 Emulator

A Chip8 emulator written in C to practice emulation.

The main implementation is now finished and can be built using the Makefile in the src directory.
You will need SDL3 and SDL3-ttf to build the emulator as these libraries are used to emulate the display.
Please find installation instructions for SDL3/SDL3-ttf directly on the [libsdl](https://www.libsdl.org) website.

To build the emulator run ``make`` in the root directory.
This will output the binary ``chip8-emulator`` - the emulator can than be run with the following command

```bash
chip8-emulator -f <path-to-rom> [-dhnox]
```

To clean the source tree run ``make clean``.

Further planned features to practice before working on a GBA emulator include:
   - [x] A debugging interface with SDL3 (initial version working)
   - [x] Proper command line flag support
   - [ ] Support for the newer/different version of Chip8 (Super Chip etc.)

Resources (Thanks to the authors for providing these!)
   + [Tobias V. Langhoff's high level Chip8 guide](https://tobiasvl.github.io/blog/write-a-chip-8-emulator)
   + [Timendus Chip8 test roms](https://github.com/Timendus/chip8-test-suite?tab=readme-ov-file)
   + [Corax Chip8 test roms](https://github.com/corax89/chip8-test-rom)

