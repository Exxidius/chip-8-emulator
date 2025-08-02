CC = gcc

CFLAGS = -Wall -std=gnu2x $(shell pkg-config --cflags sdl3 sdl3-ttf)
LDFLAGS = $(shell pkg-config --libs sdl3 sdl3-ttf)

SRC = *.c
OUT = chip8-emulator

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $(OUT) $(SRC) $(LDFLAGS)

clean:
	rm -f $(OUT)
