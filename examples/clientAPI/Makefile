INC = ../../include
SRC = ../../src
BIN = ../../bin
HANDLERS = $(SRC)/handlers

CC = g++

client:
	$(CC) -g -I $(INC) $(SRC)/TCPsocket.cpp $(SRC)/TCPclient.cpp \
		$(HANDLERS)/HTTPhandlers.cpp ./client.cpp -o $(BIN)/client
