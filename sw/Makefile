# Compiler options
CXX = g++
CXXFLAGS = -O2 -Wall -Wextra -pedantic -std=c++17

# Building and documentation directories
SRC_DIR = src
OBJ_DIR = obj
DOC_DIR = doc

# Source and object files
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/notes/*.cpp) $(wildcard $(SRC_DIR)/filters/*.cpp) $(wildcard $(SRC_DIR)/exports/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

# Name of the resulting binary file
APP_NAME = notepad

# Targets
all: $(APP_NAME)

$(APP_NAME): $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR) $(OBJ_DIR)/notes $(OBJ_DIR)/filters $(OBJ_DIR)/exports
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/notes:
	mkdir -p $(OBJ_DIR)/notes

$(OBJ_DIR)/filters:
	mkdir -p $(OBJ_DIR)/filters

$(OBJ_DIR)/exports:
	mkdir -p $(OBJ_DIR)/exports

.PHONY: compile
compile: all
	@echo "The app was compiled."

.PHONY: run
run: all
	@echo "Running..."
	./$(APP_NAME)

.PHONY: doc
doc:
	doxygen Doxyfile
	@echo Documentation was generated.

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) $(APP_NAME) $(DOC_DIR)

.PHONY: rebuild
rebuild: clean all
