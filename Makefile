# RVE Makefile - Ray-traced Voxel Engine

# Use nvcc - CUDA is the core of this project
CXX     = nvcc
LDFLAGS = -lglfw -lGL -ldl -lpthread
SRCDIR  = src
BINDIR  = bin

SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:src/%.cpp=obj/%.o)

# All the flags
CXXFLAGS = -O3 -arch=native \
           -Xcompiler "-Wall -Wextra -ffast-math -funroll-loops -ftree-vectorize -fstack-protector-strong"

.PHONY: all debug clean run

all: $(BINDIR)/rve

$(BINDIR)/rve: obj/main.o obj/window.o | $(BINDIR)
	$(CXX) $^ -o $@ $(LDFLAGS)

debug: CXXFLAGS = -O0 -g -DDEBUG -arch=native -Xcompiler "-Wall -Wextra"
debug: $(BINDIR)/rve_debug

$(BINDIR)/rve_debug: obj/main.o obj/window.o | $(BINDIR)
	$(CXX) $^ -o $@ $(LDFLAGS)

$(BINDIR):
	@mkdir -p $@

obj:
	@mkdir -p $@

obj/%.o: src/%.cpp | obj
	$(CXX) $(CXXFLAGS) -x cu -Isrc -c $< -o $@

run: all
	./$(BINDIR)/rve

clean:
	rm -rf obj $(BINDIR)
