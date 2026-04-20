NVCC      := nvcc
CXX       := g++
NVCCFLAGS := -std=c++20
CXXFLAGS  := -std=c++20
INCLUDES  := -I/usr/include/SDL2
LIBS      := -lSDL2
SRC_DIR   := src
OBJ_DIR   := obj
BIN_DIR   := bin

CUDA_SRCS := $(wildcard $(SRC_DIR)/*.cu)
CPP_SRCS  := $(wildcard $(SRC_DIR)/*.cpp)

CUDA_OBJS := $(CUDA_SRCS:$(SRC_DIR)/%.cu=$(OBJ_DIR)/%.o)
CPP_OBJS  := $(CPP_SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
OBJS      := $(CUDA_OBJS) $(CPP_OBJS)

TARGET    := $(BIN_DIR)/rve

all: $(TARGET)

# Link with nvcc so it handles CUDA runtime correctly
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(NVCC) $(NVCCFLAGS) $^ -o $@ $(LIBS)

# nvcc compiles .cu files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cu | $(OBJ_DIR)
	$(NVCC) $(NVCCFLAGS) $(INCLUDES) -c $< -o $@

# g++ compiles .cpp files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean run
