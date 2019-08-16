run: compile
	./rpg.o
compile:
	gcc -o rpg.o rpg.c physics.c -g -D_THREAD_SAFE -I/usr/local/include/SDL2 -L/usr/local/lib -lSDL2 -lSDL2_image -lSDL2_ttf
