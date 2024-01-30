# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -std=c++11

# Source files
SRCS = main.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Executable
EXEC = runme

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(OBJS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: $(EXEC)
	@./test.sh

.PHONY: clean
clean:
	rm -f $(OBJS) $(EXEC)
	rm -f $ result.txt
	

