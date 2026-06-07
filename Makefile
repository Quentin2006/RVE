CXX := g++
NVCC := nvcc

SRC_DIR := src
BIN_DIR := bin
OBJ_DIR := obj

TARGET := $(BIN_DIR)/rve

CPPFLAGS := -I/usr/include/SDL2 -I/usr/include/glm
LDLIBS := -lSDL2 -lm

CXXFLAGS_COMMON := -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wformat=2
CXXFLAGS_DEBUG  := -Og -g3
CXXFLAGS_RELEASE:= -O3 -march=native -DNDEBUG
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

# nvcc flags — arch matches RTX 3060 Ti (sm_86). Adjust for your GPU.
NVCCFLAGS := -arch=sm_86 $(CPPFLAGS) --std=c++20 -DNDEBUG

SRCS_CPP := $(shell find $(SRC_DIR) -type f -name '*.cpp')
SRCS_CU  := $(shell find $(SRC_DIR) -type f -name '*.cu')

OBJS_CPP := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS_CPP))
OBJS_CU  := $(patsubst $(SRC_DIR)/%.cu,$(OBJ_DIR)/%_cu.o,$(SRCS_CU))
OBJS     := $(OBJS_CPP) $(OBJS_CU)
DEPS     := $(OBJS_CPP:.o=.d)

.DEFAULT_GOAL := all

all: $(TARGET)

$(TARGET): $(OBJS) | $(BIN_DIR)
	$(NVCC) $(OBJS) -o $@ $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(OBJ_DIR)/%_cu.o: $(SRC_DIR)/%.cu
	@mkdir -p $(dir $@)
	$(NVCC) $(NVCCFLAGS) -c $< -o $@

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
