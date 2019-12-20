all: game.c  common.c
	gcc game.c common.c -o open-akinator -lm
debug: game.c common.c
	gcc game.c common.c -o open-akinator -g -lm
	gdb open-akinator
clean: open-akinator
	rm open-akinator
