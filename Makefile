CXX = g++
CXX_FLAGS = -Wall -Wextra -Werror -std=c++17 -fPIC -pthread
LIB_FLAG = -llogger

SOURCE_DIR = src
BUILD_DIR = build
APP_DIR = app
LIBRARY_DIR = lib
TEST_DIR = tests

LIBRARY_NAME = liblogger.so
APP_TARGET = app
TEST_TARGET = test

LIB_HEADERS = $(SOURCE_DIR)/$(LIBRARY_DIR)/*.h
LIB_SOURCES = $(SOURCE_DIR)/$(LIBRARY_DIR)/*.cpp
APP_SOURCES = $(SOURCE_DIR)/$(APP_DIR)/*.cpp
TEST_SOURCES = $(SOURCE_DIR)/$(TEST_DIR)/*.cpp $(shell find $(SOURCE_DIR)/$(APP_DIR) -type f -name '*.cpp' ! -name 'main.cpp')

APP_BIN = $(BUILD_DIR)/$(APP_TARGET)
TEST_BIN = $(BUILD_DIR)/$(TEST_TARGET)
LIBRARIES = $(BUILD_DIR)/$(LIBRARY_NAME)

INSTALL_LIB_DIR = /usr/local/lib
INSTALL_INCLUDE_DIR = /usr/local/include/logger

.PHONY: all library test clean CREATE_BUILD_DIR app install uninstall

all: CREATE_BUILD_DIR library app test

app: CREATE_BUILD_DIR
	$(CXX) $(CXX_FLAGS) $(APP_SOURCES) -o $(APP_BIN) $(LIB_FLAG)

library: CREATE_BUILD_DIR
	$(CXX) $(CXX_FLAGS) -shared $(LIB_SOURCES) -o $(LIBRARIES)

test: CREATE_BUILD_DIR
	$(CXX) $(CXX_FLAGS) $(TEST_SOURCES) -o $(TEST_BIN) $(LIB_FLAG)

install: library
	@sudo mkdir -p $(INSTALL_LIB_DIR)
	@sudo mkdir -p $(INSTALL_INCLUDE_DIR)
	@sudo cp $(LIBRARIES) $(INSTALL_LIB_DIR)
	@sudo cp $(LIB_HEADERS) $(INSTALL_INCLUDE_DIR)
	@sudo ldconfig
	@echo "Installed: $(LIBRARIES) to $(INSTALL_LIB_DIR)"
	@echo "Installed headers: $(LIB_HEADERS) to $(INSTALL_INCLUDE_DIR)"

uninstall:
	@sudo rm -f $(INSTALL_LIB_DIR)/$(LIBRARY_NAME)
	@sudo rm -rf $(INSTALL_INCLUDE_DIR)
	@sudo ldconfig
	@echo "Uninstalled: $(LIBRARY_NAME) from $(INSTALL_LIB_DIR)"
	@echo "Uninstalled headers from $(INSTALL_INCLUDE_DIR)"

CREATE_BUILD_DIR:
	@mkdir -p $(BUILD_DIR)
	@echo "Created: new directory /build"

clean:
	@rm -rf $(BUILD_DIR) *.txt
	@echo "Deleted: /build | all logs"