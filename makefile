NVCC      := nvcc
CXX       := g++

SRC_DIR   := src
OBJ_DIR   := obj
BIN_DIR   := bin
INCLUDES  := -I/usr/include/SDL2 -I/usr/include/glm -I/opt/cuda/include
LIBS      := -lSDL2 -lm

CXXFLAGS  := -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
            -Wsign-conversion -Wnull-dereference -Wno-unused-result \
            -Wdouble-promotion -Wformat=2 -Wcast-align -Wcast-qual \
            -Wmissing-noreturn -Wundef -fno-common

NVCCFLAGS := -std=c++20 --gpu-architecture=sm_86 \
            --expt-relaxed-constexpr --expt-extended-lambda \
            -Wno-deprecated-gpu-targets

ifdef DEBUG
    CXXFLAGS  += -O0 -g3 -DDEBUG -fsanitize=address,undefined,leak -fno-omit-frame-pointer
    NVCCFLAGS += -O0 -g -G -DDEBUG
    LIBS     += -lubsan -lasan
    OBJ_DIR   := obj/debug
    TARGET    := $(BIN_DIR)/rve_debug
else
    CXXFLAGS  += -O2 -DNDEBUG
    NVCCFLAGS += -O2 --use_fast_math -DNDEBUG
    TARGET    := $(BIN_DIR)/rve
endif

CUDA_SRCS := $(wildcard $(SRC_DIR)/*.cu)
CPP_SRCS  := $(wildcard $(SRC_DIR)/*.cpp)
CUDA_OBJS := $(CUDA_SRCS:$(SRC_DIR)/%.cu=$(OBJ_DIR)/%.o)
CPP_OBJS  := $(CPP_SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
OBJS      := $(CUDA_OBJS) $(CPP_OBJS)

all: $(TARGET)

debug:
	$(MAKE) DEBUG=1

$(TARGET): $(OBJS) | $(BIN_DIR)
	$(NVCC) $(NVCCFLAGS) $^ -o $@ $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cu | $(OBJ_DIR)
	$(NVCC) $(NVCCFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all debug clean run
