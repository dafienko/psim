CC = clang++

MAIN = src/main.cpp
SRC_FILES = $(filter-out $(MAIN),$(wildcard src/*.cpp))

main: $(MAIN) $(SRC_FILES) 
	$(CC) -g -Wall -I src -framework GLUT -framework OpenGL $^ -o $@

clean:
	if [ -f *.o ]; then rm *.o; fi
	if [ -f main ]; then rm main; fi