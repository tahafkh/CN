CC = g++
BUILD_DIR = build
SRC_DIR = src
INCLUDE_DIR = include
CFLAGS = -std=c++11 -Wall -Werror -I$(INCLUDE_DIR)

all: receiver sender router

receiver: $(INCLUDE_DIR)/defs.hpp $(SRC_DIR)/defs.cpp $(INCLUDE_DIR)/receiver.hpp $(SRC_DIR)/receiver.cpp
	g++ -std=c++11 -pthread $(SRC_DIR)/defs.cpp $(SRC_DIR)/receiver.cpp -o receiver

router: $(INCLUDE_DIR)/defs.hpp $(SRC_DIR)/defs.cpp $(INCLUDE_DIR)/router.hpp $(SRC_DIR)/router.cpp
	g++ -std=c++11 -pthread $(SRC_DIR)/defs.cpp $(SRC_DIR)/router.cpp -o router

sender: $(INCLUDE_DIR)/defs.hpp $(SRC_DIR)/defs.cpp $(INCLUDE_DIR)/sender.hpp $(SRC_DIR)/sender.cpp
	g++ -std=c++11 -pthread $(SRC_DIR)/defs.cpp $(SRC_DIR)/sender.cpp -o sender

clean:
	rm -f receiver sender router
	