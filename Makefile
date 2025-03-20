# opengl-c-voxel-engine/Makefile

PROJ_NAME := opengl-c-voxel-engine

CC         := gcc
CC_VERSION := 17

WARNING_FLAGS := -Wall -Wextra -Werror

PACKAGED_LIBS := gl glfw3 glew freetype2
THIRDPARTY_LIBS := open-simplex-noise-in-c

INCLUDES := -Ilib/glad/include -Iinclude -Ilib/cglm/include -Ilib/stb -Ilib/open-simplex-noise-in-c

CFLAGS := -std=c$(CC_VERSION) -ggdb $(WARNING_FLAGS) `pkg-config --cflags $(PACKAGED_LIBS)`
CFLAGS += -Wuninitialized -Wformat -Wno-unused

LDFLAGS := -lm -lrt `pkg-config --libs $(PACKAGED_LIBS)`

SOURCES  := $(wildcard src/*.c) $(wildcard src/**/*.c) $(wildcard src/**/**/*.c) $(wildcard src/**/**/**/*.c)
OBJECTS  := $(SOURCES:.c=.o)

TEST_DIR  := test
DEBUG_DIR := debug
BUILD_DIR := build

BIN_NAME := $(BUILD_DIR)/$(PROJ_NAME)

# Main interface
# -----------------------------------
.PHONY: run
run: all
	$(BIN_NAME)

.PHONY: all
all: dirs libs main 

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(OBJECTS)
# -----------------------------------

# Directories
# -----------------------------------
.PHONY: dirs
dirs: build-dir debug-dir

.PHONY: libs
libs: build-glad build-open-simplex-noise-in-c

.PHONY: build-glad
build-glad:
	$(CC) -c -std=c$(CC_VERSION) lib/glad/src/glad.c -Ilib/glad/include -o $(BUILD_DIR)/glad.o $(LDFLAGS)

.PHONY: build-open-simplex-noise-in-c
build-open-simplex-noise-in-c:
	cd lib/open-simplex-noise-in-c && $(MAKE) && cd -

.PHONY: build
build-dir:
	mkdir -p ./$(BUILD_DIR)

.PHONY: debug-dir
debug-dir:
	mkdir -p ./$(DEBUG_DIR)
# -----------------------------------

# Build Step
# -----------------------------------
.PHONY: main
main: $(OBJECTS)
	$(CC) $(CFLAGS) main.c -o $(BIN_NAME) $^ $(INCLUDES) $(LDFLAGS) $(BUILD_DIR)/glad.o lib/open-simplex-noise-in-c/open-simplex-noise.o

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $< $(INCLUDES) $(LDFLAGS)
# -----------------------------------

