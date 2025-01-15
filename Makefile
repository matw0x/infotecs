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

INSTALL_DIR = /usr/local/lib

.PHONY: all library test clean CREATE_BUILD_DIR app install uninstall

all: CREATE_BUILD_DIR library app test

app: CREATE_BUILD_DIR library
	$(CXX) $(CXX_FLAGS) $(APP_SOURCES) -o $(APP_BIN) -llogger

library: CREATE_BUILD_DIR
	$(CXX) $(CXX_FLAGS) -shared $(LIB_SOURCES) -o $(LIBRARIES)

test: CREATE_BUILD_DIR library
	$(CXX) $(CXX_FLAGS) $(TEST_SOURCES) -o $(TEST_BIN) -llogger

install: library
	@sudo cp $(LIBRARIES) $(INSTALL_DIR)
	@sudo ldconfig
	@echo "Installed: $(LIBRARIES) to $(INSTALL_DIR)"

uninstall:
	@sudo rm -f $(INSTALL_DIR)/$(LIBRARY_NAME)
	@sudo ldconfig
	@echo "Uninstalled: $(LIBRARY_NAME) from $(INSTALL_DIR)"

CREATE_BUILD_DIR: clean
	@mkdir -p $(BUILD_DIR)
	@echo "Created: new directory /build"

clean:
	@rm -rf $(BUILD_DIR) *.txt
	@echo "Deleted: /build | all logs"