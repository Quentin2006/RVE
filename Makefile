CXX      := nvcc
CXXFLAGS := -arch=native -std=c++20
LIBS     := -lSDL2

SRC_DIR  := src
OBJ_DIR  := obj
BIN_DIR  := bin

SRCS     := $(wildcard $(SRC_DIR)/*.cu)
OBJS     := $(SRCS:$(SRC_DIR)/%.cu=$(OBJ_DIR)/%.o)
TARGET   := $(BIN_DIR)/rve

all: $(TARGET)

$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cu | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean run
