# Compiler
CXX = g++

# Compiler flags (Add -Ofast to make fast)
CXXFLAGS = -Ofast -std=c++20 -Wall -Wextra -pedantic -g  # Added -g flag for debugging

# Target executable
TARGET = main.exe

# Source files
SRCS = main.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Rule to link the object files to create the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule to compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean
