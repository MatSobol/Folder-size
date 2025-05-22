CC = gcc

SRC = ./src/main.c ./src/graph/graph.c ./src/sizeCalc/sizeCalc.c ./src/queue/queue.c ./src/sqlite/sqlite.c
HEADERS = ./src/graph/graph.h ./src/sizeCalc/sizeCalc.h ./src/queue/queue.h ./src/sqlite/sqlite.h

CFLAGS = -Wall
OUT = ./build/main.exe

SYSTEM_LIBS = -lshlwapi -lws2_32
STATIC_LIBS = ./libraries/libsqlite3.a
LIBS = $(SYSTEM_LIBS) $(STATIC_LIBS)

$(OUT): $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LIBS)

clean:
	rm -rf ./build/*
