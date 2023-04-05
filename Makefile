# Compiler options
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 `wx-config --cxxflags` -I/usr/local/Cellar/nlohmann-json/3.11.2/include

# Linker options
LD = g++
LDFLAGS = `wx-config --libs` -lcurl

# Source files
SRCS = $(wildcard src/*.cpp)

# Object files
BUILDDIR = build
OBJS = $(patsubst src/%.cpp, $(BUILDDIR)/%.o, $(SRCS))

# Executable file
EXE = $(BUILDDIR)/mychat.app

all: $(EXE)

$(EXE): $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o $(EXE)

$(BUILDDIR)/%.o: src/%.cpp
	mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@rm -rf $(BUILDDIR)

fmt:
	@clang-format -style=Google -i $(SRCS)
