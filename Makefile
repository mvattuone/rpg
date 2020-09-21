run: compile
	./rpg
compile:
	gcc -g -o rpg camera.c font.c game.c inventory.c renderers.c rpg.c quest.c item.c menu.c physics.c map.c utils.c dynamic_object.c -g -D_THREAD_SAFE -I/usr/local/include/SDL2 -L/usr/local/lib -lSDL2 -lSDL2_image -lSDL2_ttf 
