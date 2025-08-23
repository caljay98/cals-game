CC=gcc
CFLAGS=-I. -s -static -O2 -std=c99 -Wall -IC:\raylib\raylib\src -Iexternal -DPLATFORM_DESKTOP -lraylib -lopengl32 -lgdi32 -lwinmm
DEPS = Player.h RectGround.h TriGround.h World.h CollisionLib.h
OBJ = Main.o Player.o RectGround.o TriGround.o World.o CollisionLib.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

game: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
	game.exe
	
.PHONY: clean

clean:
	rm -f game.exe *.o