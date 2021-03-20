FILES=src/main.c src/specials.c src/votes.c
OUT=build/gopher
CFLAGS=-O3
all:
	$(CC) $(FILES) $(CFLAGS) -o $(OUT)
