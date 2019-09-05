run: compile
	./rpg
compile:
	gcc -o rpg renderers.c rpg.c physics.c map.c utils.c dynamicObject.c -g -D_THREAD_SAFE -I/usr/local/include/SDL2 -L/usr/local/lib -lSDL2 -lSDL2_image -lSDL2_ttf
