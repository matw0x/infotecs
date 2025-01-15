CXX = g++
CXX_FLAGS = -Wall -Wextra -Werror -std=c++17 -fPIC -pthread

SOURCE_DIR = src
BUILD_DIR = build
LIBRARY_NAME = liblogger.so
APP_TARGET = app
TEST_TARGET = test

LIB_SOURCES = $(SOURCE_DIR)/logger.cpp
TEST_SOURCES = $(SOURCE_DIR)/test.cpp
APP_SOURCES = $(SOURCE_DIR)/main.cpp

APP_BIN = $(BUILD_DIR)/$(APP_TARGET)
TEST_BIN = $(BUILD_DIR)/$(TEST_TARGET)
LIBRARIES = $(BUILD_DIR)/$(LIBRARY_NAME)

.PHONY: all library test clean CREATE_BUILD_DIR app

all: CREATE_BUILD_DIR library app test

app: CREATE_BUILD_DIR library
	$(CXX) $(CXX_FLAGS) $(APP_SOURCES) -o $(APP_BIN) -L$(BUILD_DIR) -Wl,-rpath=$(BUILD_DIR) -llogger

library: CREATE_BUILD_DIR
	$(CXX) $(CXX_FLAGS) -shared $(LIB_SOURCES) -o $(LIBRARIES)

test: CREATE_BUILD_DIR library
	$(CXX) $(CXX_FLAGS) $(TEST_SOURCES) -o $(TEST_BIN) -L$(BUILD_DIR) -Wl,-rpath=$(BUILD_DIR) -llogger

CREATE_BUILD_DIR: clean
	@mkdir -p $(BUILD_DIR)
	@echo "Created: new directory /build"

clean:
	@rm -rf $(BUILD_DIR) *.txt
	@echo "Deleted: /build | all logs"