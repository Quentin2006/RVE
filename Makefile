CXX := g++
SRC_DIR := src
BIN_DIR := bin
TARGET := $(BIN_DIR)/rve

INCLUDES := -I/usr/include/SDL2 -I/usr/include/glm
LIBS := -lSDL2 -lm

CXXFLAGS_COMMON := -std=c++20 -Wall -Wextra -Wpedantic -pipe
CXXFLAGS_RELEASE := -O3 -flto -DNDEBUG
CXXFLAGS_DEBUG := -Og -g

BUILD ?= release

ifeq ($(BUILD),debug)
  CXXFLAGS := $(CXXFLAGS_COMMON) $(CXXFLAGS_DEBUG)
else
  CXXFLAGS := $(CXXFLAGS_COMMON) $(CXXFLAGS_RELEASE)
endif

SRCS := $(shell find $(SRC_DIR) -type f -name "*.cpp")

all: $(TARGET)

$(TARGET): $(SRCS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRCS) -o $@ $(LIBS)

$(BIN_DIR):
	mkdir -p $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BIN_DIR)

benchmark:
	python3 benchmark.py

M = "my lazy git commit comment"
git:
	git add .
	git commit -m "$(M)"
	git push

.PHONY: all run clean git benchmark
