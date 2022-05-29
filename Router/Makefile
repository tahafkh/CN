CC = g++
BUILD_DIR = build
SRC_DIR = src
INCLUDE_DIR = include
CFLAGS = -std=c++17 -Wall -Werror -I$(INCLUDE_DIR)
EXECUTABLE_FILE = main.out

all: $(BUILD_DIR) $(EXECUTABLE_FILE) 

OBJECTS = \
	$(BUILD_DIR)/main.o \
	$(BUILD_DIR)/network.o \

NetworkSensitivityList = \
	$(SRC_DIR)/network.cpp \
	$(INCLUDE_DIR)/network.hpp 

MainSensitivityList = \
	$(SRC_DIR)/main.cpp \
	$(INCLUDE_DIR)/network.hpp 

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/network.o: $(NetworkSensitivityList)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/network.cpp -o $(BUILD_DIR)/network.o

$(BUILD_DIR)/main.o: $(MainSensitivityList)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/main.cpp -o $(BUILD_DIR)/main.o

$(EXECUTABLE_FILE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(EXECUTABLE_FILE)


.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) *.o *.out
	