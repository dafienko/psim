ifeq ($(OS),Windows_NT)     # is Windows_NT on XP, 2000, 7, Vista, 10...
    CC = g++
	CFLAGS = -lglew32 -lglfw3 -lopengl32 -lgdi32
else
    CC = clang++
	CFLAGS = -framework GLUT -framework OpenGL
endif

MAIN = src/main.cpp
SRC_FILES = $(filter-out $(MAIN),$(wildcard src/*.cpp))

main: $(MAIN) $(SRC_FILES) 
	$(CC) -g -Wall -I src $^ -o $@ $(CFLAGS)

clean:
	if [ -f *.o ]; then rm *.o; fi
	if [ -f main ]; then rm main; fi