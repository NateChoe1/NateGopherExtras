FILES=src/main.c src/specials.c
OUT=build/gopher
all:
	$(CC) $(FILES) -o $(OUT)
