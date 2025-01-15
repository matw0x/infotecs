CXX = g++
CXX_FLAGS = -Wall -Wextra -Werror -std=c++17 -fPIC -pthread

SOURCE_DIR = src
BUILD_DIR = build
LIBRARY_NAME = liblogger.so
TARGET = app

LIB_SOURCES = $(SOURCE_DIR)/logger.cpp
TEST_SOURCES = $(SOURCE_DIR)/test.cpp
BINARIES = $(BUILD_DIR)/$(TARGET)
LIBRARIES = $(BUILD_DIR)/$(LIBRARY_NAME)

.PHONY: all library test clean CREATE_BUILD_DIR

all: CREATE_BUILD_DIR library test

library:
	$(CXX) $(CXX_FLAGS) -shared $(LIB_SOURCES) -o $(LIBRARIES)

test: library
	$(CXX) $(CXX_FLAGS) $(TEST_SOURCES) -o $(BINARIES) -L$(BUILD_DIR) -llogger

CREATE_BUILD_DIR:
	@mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)