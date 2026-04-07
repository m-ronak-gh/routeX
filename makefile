# Define variables
CC = gcc
TARGET = routeX
SRCS =  app/main.c lib/graph.c lib/heap.c lib/astar.c lib/dijkstra.c lib/greedy.c lib/bfs.c lib/dfs.c viz/renderer.c viz/camera.c

# Path to SDL2 library
SDL_PATH = C:/msys64/ucrt64

# Compiler flags: -I specifies the include path
CFLAGS = -I$(SDL_PATH)/include/SDL2 -Wall -Wextra -std=c11 -g -O2
# Linker flags: -L specifies the library path, -l specifies the libraries to link
LDFLAGS = -L$(SDL_PATH)/lib -lmingw32 -lSDL2main -lSDL2 -lm

# Default target
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(SRCS) -o $(TARGET) $(CFLAGS) $(LDFLAGS)

## Run heap tests
testHeap: tests/testHeap.c lib/heap.c
	$(CC) -Wall -Wextra -std=c11 -g -O2 -o testHeap $^ -lm
	./testHeap.exe

## Run graph loader tests
testGraph: tests/testGraph.c lib/graph.c
	$(CC) -Wall -Wextra -std=c11 -g -O2 -o testGraph $^ -lm
	./testGraph.exe

clean:
	-del /q /f $(TARGET).exe testHeap.exe testGraph.exe

phony: all clean testHeap testGraph