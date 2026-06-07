CXX := g++

SRC_DIR := src
BIN_DIR := bin
OBJ_DIR := obj

TARGET := $(BIN_DIR)/rve

CPPFLAGS := -I/usr/include/SDL2 -I/usr/include/glm
LDLIBS := -lSDL2 -lm

CXXFLAGS_COMMON := -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wformat=2
CXXFLAGS_DEBUG  := -Og -g3
CXXFLAGS_RELEASE:= -O3 -march=native -flto -DNDEBUG
CXXFLAGS_BENCHMARK := -O3 -march=native -g3 -fno-omit-frame-pointer -fno-optimize-sibling-calls -DNDEBUG

BUILD ?= release

ifeq ($(BUILD),debug)
    CXXFLAGS := $(CXXFLAGS_COMMON) $(CXXFLAGS_DEBUG)
else ifeq ($(BUILD),release)
    CXXFLAGS := $(CXXFLAGS_COMMON) $(CXXFLAGS_RELEASE)
else ifeq ($(BUILD),benchmark)
    CXXFLAGS := $(CXXFLAGS_COMMON) $(CXXFLAGS_BENCHMARK)
else
    $(error Unknown BUILD=$(BUILD))
endif

SRCS := $(shell find $(SRC_DIR) -type f -name '*.cpp')
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

.DEFAULT_GOAL := all

all: $(TARGET)

$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(BIN_DIR):
	mkdir -p $@

run: $(TARGET)
	./$(TARGET)

benchmark:
	python3 benchmark.py

clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)

M ?= update

git:
	git add .
	git commit -m "$(M)"
	git push

-include $(DEPS)

.PHONY: all run benchmark clean git
