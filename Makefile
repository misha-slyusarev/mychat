# Compiler options
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 `wx-config --cxxflags`

# Linker options
LD = g++
LDFLAGS = `wx-config --libs` -lcurl

# Source files
SRCS = main.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Executable file
EXE = mychat.app

all: $(EXE)

$(EXE): $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o $(EXE)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXE)

fmt:
	clang-format -style=Google -i $(SRCS)
