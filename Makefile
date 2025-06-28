# Compiler and flags
CXX := g++

# HDF4 and HDF5 include and lib paths
HDF4_INC := /usr/include/hdf
HDF4_LIB := /usr/lib64
HDF5_INC := /usr/include
HDF5_LIB := /usr/lib64

# Directories
SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
INCLUDE_DIR := include

# Source files
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# Include and link flags
INCLUDES := -I$(HDF4_INC) -I$(HDF5_INC) -I$(INCLUDE_DIR)
LDFLAGS := -L$(HDF4_LIB) -L$(HDF5_LIB) \
    -lmfhdf -ldf -ljpeg -lz -lhdf5 -lhdf5_hl -lm

# Build types
DEBUG_FLAGS := -g -O0 -Wall -Wextra -std=c++17
RELEASE_FLAGS := -O3 -Wall -Wextra -std=c++17

# Default target
all: release

# Release build
release: CXXFLAGS := $(RELEASE_FLAGS) $(INCLUDES)
release: $(BIN_DIR)/ts

# Debug build
debug: CXXFLAGS := $(DEBUG_FLAGS) $(INCLUDES)
debug: $(BIN_DIR)/ts

# Link target
$(BIN_DIR)/ts: $(OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Compile each .cpp to .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Ensure bin/ and obj/ dirs exist
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Clean build files
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean debug release

# make          # builds in release mode
# make release  # explicitly build release
# make debug    # builds with debug symbols (-g)
# make clean    # removes all build artifacts