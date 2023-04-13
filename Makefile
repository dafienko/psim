ifeq ($(OS),Windows_NT) # windows
    CC = g++
	LDFLAGS = -lglew32 -lglfw3 -lopengl32 -lgdi32 -l:freetype.a
	RMDIR = rmdir /s /q
	RM = del /q /f
	BIN = main.exe
else # mac
    CC = g++
	CINCLUDE = -isystem /opt/homebrew/include/ -isystem /opt/homebrew/include/freetype2/ -isystem /Library/Developer/CommandLineTools/usr/lib/clang/14.0.0/include
	LDFLAGS = -L /opt/homebrew/lib/ -lglfw -lglew -lfreetype -framework OpenGL
	RMDIR = rm -rf
	RM = rm -f
	BIN = main
endif

SRC = src
BUILD = build
DEP = $(BUILD)/deps

SRC_FILES = $(wildcard $(SRC)/*.cpp)
OBJ_FILES = $(patsubst $(SRC)/%.cpp, $(BUILD)/%.o, $(SRC_FILES))

CCFLAGS = -g -Wall -std=c++20 --coverage
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP)/$*.d

all: $(BIN)

$(BIN): $(OBJ_FILES)
	$(CC) $(CCFLAGS) $(OBJ_FILES) -o $@ $(LDFLAGS)

$(BUILD)/%.o: $(SRC)/%.cpp $(DEP)/%.d | $(DEP)
	$(CC) $(DEPFLAGS) $(CCFLAGS) $(CINCLUDE) -c $< -o $@

$(BUILD): 
	mkdir "$@"

$(DEP): | $(BUILD)
	mkdir "$@"

.PHONY: docs
docs:
	doxygen

.PHONY: static
static:
	cppcheck --enable=all src/ &> static-analysis-report.txt && cpplint src/*.cpp &> style-report.txt

.PHONY: coverage
coverage:
	gcov -r build/*.gcno
	gcovr --html-details coverage/

clean:
	$(RMDIR) $(BUILD)
	$(RM) $(BIN)
	$(RM) *.gcov

DEPFILES = $(patsubst $(SRC)/%.cpp, $(DEP)/%.d, $(SRC_FILES))
$(DEPFILES):
include $(wildcard $(DEPFILES))
