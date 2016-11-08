# This is just a temporary Makefile, it will be changed soon!

SRC = ../../src
INC = ../../include
SDBINCLUDE = $(INC)/serverDB
HDLINCLUDE = $(SRC)/handlers
THR = $(SRC)/threadCore
DB_SRC = $(SRC)/serverDB
CC =  g++

BIN = ../../bin

BUILD = ../../build

OBJPATH = $(BUILD)/TCPsocket.o \
   $(BUILD)/TCPserver.o $(BUILD)/TCPserverThreadCore.o $(BUILD)/TCPclient.o \
   $(BUILD)/httpProxy.o $(BUILD)/HTTPhandlers.o 

####################################################################################
OBJECTS: TCPsocket TCPserver TCPclient HTTPHandlers TCPserverThreadCore httpProxy
####################################################################################
mainProxy: $(OBJECTS)
	$(MAKE) $(OBJECTS)
	$(CC) -g -std=c++17 -I$(INC) ./mainProxy.cpp $(OBJPATH) -lpthread -o $(BIN)/mainProxy
####################################################################################
TCPsocket: $(SRC)/TCPsocket.cpp $(INC)/TCPsocket.hpp
	$(CC) -g -c -I$(INC) $(SRC)/TCPsocket.cpp -o $(BUILD)/TCPsocket.o

TCPserver: $(SRC)/TCPserver.cpp $(INC)/TCPserver.hpp
	$(CC) -g -std=c++17 -c -I$(INC) $(SRC)/TCPserver.cpp -o $(BUILD)/TCPserver.o

TCPclient: $(SRC)/TCPclient.cpp $(INC)/TCPclient.hpp
	$(CC) -g -c -I$(INC) $(SRC)/TCPclient.cpp -o $(BUILD)/TCPclient.o

HTTPHandlers: $(HDLINCLUDE)/HTTPhandlers.cpp $(INC)/handlers/HTTPhandlers.hpp
	$(CC) -g -c -I$(INC) $(HDLINCLUDE)/HTTPhandlers.cpp -o $(BUILD)/HTTPhandlers.o

TCPserverThreadCore: $(THR)/TCPserverThreadCore.cpp
	$(CC) -g -std=c++17 -c -I$(INC) $(THR)/TCPserverThreadCore.cpp \
	 -o $(BUILD)/TCPserverThreadCore.o

httpProxy: $(DB_SRC)/httpProxy.cpp $(SDBINCLUDE)/httpProxy.hpp
	$(CC) -g -c -I$(INC) $(DB_SRC)/httpProxy.cpp -o $(BUILD)/httpProxy.o

clean:
	rm $(BUILD)/*.o
