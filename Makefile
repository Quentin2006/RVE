# RVE Makefile - Ray-traced Voxel Engine

CXX     = g++
LDFLAGS = -lglfw
SRCDIR  = src
BINDIR  = bin

SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:src/%.cpp=obj/%.o)

CXXFLAGS_WARN   = -Wall -Wextra -Wpedantic -Wconversion -Wshadow \
                 -Wcast-align -Wcast-qual -Wunused -Wuninitialized \
                 -Wmissing-declarations -Wmissing-include-dirs \
                 -Wswitch-default -Wswitch-enum -Wtype-limits

CXXFLAGS_OPT   = -O3 -march=native -mtune=native -ffast-math \
                 -funroll-loops -ftree-vectorize -flto -fno-semantic-interposition


CXXFLAGS       = -std=c++17 $(CXXFLAGS_WARN) $(CXXFLAGS_OPT)

.PHONY: all debug clean run

all: $(BINDIR)/rve

$(BINDIR)/rve: obj/main.o obj/window.o | $(BINDIR)
	$(CXX) $^ -o $@ $(LDFLAGS)

debug: CXXFLAGS = $(CXXFLAGS_COMMON) -O0 -g -DDEBUG
debug: $(BINDIR)/rve_debug

$(BINDIR)/rve_debug: obj/main.o obj/window.o | $(BINDIR)
	$(CXX) $^ -o $@ $(LDFLAGS)

$(BINDIR):
	@mkdir -p $@

obj:
	@mkdir -p $@

obj/%.o: src/%.cpp | obj
	$(CXX) $(CXXFLAGS) -Isrc -c $< -o $@

run: all
	./$(BINDIR)/rve

clean:
	rm -rf obj $(BINDIR)
