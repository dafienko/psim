ifeq ($(OS),Windows_NT) # windows
    CC = g++
	LDFLAGS = -lglew32 -lglfw3 -lopengl32 -lgdi32 -l:freetype.a
	RMDIR = rmdir /s /q
	RM = del /q /f
	BIN = main.exe
else # mac
    CC = clang++
	CINCLUDE = -isystem /opt/homebrew/include/ -isystem /opt/homebrew/include/freetype2/
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

CCFLAGS = -g -Wall -std=c++20
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP)/$*.d

all: $(BIN)

$(BIN): $(OBJ_FILES)
	$(CC) $(CCFLAGS) $(OBJ_FILES) -o $@ $(LDFLAGS)

$(BUILD)/%.o: $(SRC)/%.cpp $(DEP)/%.d | $(DEP)
	$(CC) $(DEPFLAGS) $(CCFLAGS) -c $< -o $@

$(BUILD): 
	mkdir "$@"

$(DEP): | $(BUILD)
	mkdir "$@"

clean:
	$(RMDIR) $(BUILD)
	$(RM) $(BIN)

DEPFILES = $(patsubst $(SRC)/%.cpp, $(DEP)/%.d, $(SRC_FILES))
$(DEPFILES):
include $(wildcard $(DEPFILES))