CC = g++
BUILD_DIR = build
SRC_DIR = src
INCLUDE_DIR = include
CFLAGS = -std=c++17 -Wall -Werror -I$(INCLUDE_DIR)

SERVER_EXECUTABLE_FILE = server.out
CLIENT_EXECUTABLE_FILE = client.out

all: $(BUILD_DIR) $(SERVER_EXECUTABLE_FILE) \
	$(BUILD_DIR) $(CLIENT_EXECUTABLE_FILE)



SERVER_OBJECTS = \
	$(BUILD_DIR)/database.o \
	$(BUILD_DIR)/logger.o \
	$(BUILD_DIR)/user.o \
	$(BUILD_DIR)/exceptions.o \
	$(BUILD_DIR)/commandHandler.o \
	$(BUILD_DIR)/server.o \

CLIENT_OBJECTS = \
	$(BUILD_DIR)/exceptions.o \
	$(BUILD_DIR)/client.o \


DataBaseSensitivityList = \
	$(SRC_DIR)/database.cpp \
	$(INCLUDE_DIR)/database.hpp 

LoggerSensitivityList = \
	$(SRC_DIR)/logger.cpp \
	$(INCLUDE_DIR)/logger.hpp 

UserSensitivityList = \
	$(SRC_DIR)/user.cpp \
	$(INCLUDE_DIR)/user.hpp

ExceptionsSensitivityList = \
	$(SRC_DIR)/exceptions.cpp \
	$(INCLUDE_DIR)/exceptions.hpp

CommandHandlerSensitivityList = \
	$(SRC_DIR)/commandHandler.cpp \
	$(INCLUDE_DIR)/commandHandler.hpp

ServerSensitivityList = \
	$(SRC_DIR)/server.cpp \
	$(INCLUDE_DIR)/server.hpp

ClientSensitivityList = \
	$(SRC_DIR)/client.cpp \
	$(INCLUDE_DIR)/client.hpp



$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/database.o: $(DataBaseSensitivityList)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/database.cpp -o $(BUILD_DIR)/database.o

$(BUILD_DIR)/logger.o: $(LoggerSensitivityList)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/logger.cpp -o $(BUILD_DIR)/logger.o

$(BUILD_DIR)/user.o: $(UserSensitivityList)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/user.cpp -o $(BUILD_DIR)/user.o

$(BUILD_DIR)/exceptions.o: $(ExceptionsSensitivityList)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/exceptions.cpp -o $(BUILD_DIR)/exceptions.o

$(BUILD_DIR)/commandHandler.o: $(CommandHandlerSensitivityList)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/commandHandler.cpp -o $(BUILD_DIR)/commandHandler.o

$(BUILD_DIR)/server.o: $(ServerSensitivityList)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/server.cpp -o $(BUILD_DIR)/server.o

$(BUILD_DIR)/client.o: $(ClientSensitivityList)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/client.cpp -o $(BUILD_DIR)/client.o


$(SERVER_EXECUTABLE_FILE): $(SERVER_OBJECTS)
	$(CC) $(CFLAGS) $(SERVER_OBJECTS) -o $(SERVER_EXECUTABLE_FILE)

$(CLIENT_EXECUTABLE_FILE): $(CLIENT_OBJECTS)
	$(CC) $(CFLAGS) $(CLIENT_OBJECTS) -o $(CLIENT_EXECUTABLE_FILE)



.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) *.o *.out