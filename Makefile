# Compiler and flags
CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17

# HDF4 and HDF5 include and lib paths (adjust if custom-installed)
HDF4_INC := /usr/include/hdf
HDF4_LIB := /usr/lib64
HDF5_INC := /usr/include
HDF5_LIB := /usr/lib64

INCLUDES := -I$(HDF4_INC) -I$(HDF5_INC)

# Dynamic linking flags
LDFLAGS := -L$(HDF4_LIB) -L$(HDF5_LIB) \
    -lmfhdf -ldf -ljpeg -lz -lhdf5 -lhdf5_hl -lm

# Target binary name
TARGET := ts

# Source and object files
SRCS := src/main.cpp
OBJS := $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Link object files
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS)

# Compile source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean build
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean