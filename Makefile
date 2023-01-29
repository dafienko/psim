ifeq ($(OS),Windows_NT) # windows
    CC = g++
	CFLAGS = -lglew32 -lglfw3 -lopengl32 -lgdi32 -l:freetype.a
else # mac
    CC = clang++
	CINCLUDE = -isystem /opt/homebrew/include/
	CFLAGS = -L /opt/homebrew/lib/ -lglfw -lglew -framework OpenGL
endif

MAIN = src/main.cpp
SRC_FILES = $(filter-out $(MAIN),$(wildcard src/*.cpp))

main: $(MAIN) $(SRC_FILES) 
	$(CC) -std=c++20 -g -Wall $(CINCLUDE) -I src $^ -o $@ $(CFLAGS)

clean:
	if [ -f *.o ]; then rm *.o; fi
	if [ -f main ]; then rm main; fi