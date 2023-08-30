INC = ../../include
SRC = ../../src
BIN = ../../bin
HANDLERS = $(SRC)/handlers

CC = g++

httpClient:
	$(CC) -g -I $(INC) $(SRC)/TCPsocket.cpp $(SRC)/TCPclient.cpp \
		$(HANDLERS)/HTTPhandlers.cpp ./httpClient.cpp -o $(BIN)/httpClient
