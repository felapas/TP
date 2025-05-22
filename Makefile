# Compiler and Flags
CXX = g++
# Using C++11 as required by VPL [cite: 128]
# Adding common warning flags for robust development
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic -Iinclude
LDFLAGS = # Add linking flags if any, e.g. -lm for math library if not automatically linked

# Directories - assuming Makefile is in the root TP directory [cite: 112, 114]
SRCDIR = src
OBJDIR = obj
BINDIR = bin

# Files
# Find all .cpp files in SRCDIR
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
# Replace .cpp extension with .o and prefix with OBJDIR
OBJECTS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))

# Executable name - VPL instructions might specify this.
# The PDF (page 11) mentions "tp3.out" once[cite: 115], but the context is TP1.
# Let's assume tp1.out for TP1.
EXECUTABLE = $(BINDIR)/tp1.out

# Default target
all: $(EXECUTABLE)

# Rule to link the executable
$(EXECUTABLE): $(OBJECTS)
	@mkdir -p $(BINDIR) # Ensure bin directory exists
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

# Rule to compile source files into object files
# $(wildcard include/*.hpp) makes object files depend on all headers.
# A more precise dependency list per .cpp could be generated but this is a common simpler approach.
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(wildcard include/*.hpp) Makefile
	@mkdir -p $(OBJDIR) # Ensure obj directory exists
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Phony target for cleaning up
clean:
	rm -rf $(OBJDIR)/* $(BINDIR)/* # Remove contents of obj and bin

# Explicitly declare phony targets
.PHONY: all clean