INC = ../../include
SRC = ../../src
HANDLERS = $(SRC)/handlers

CC = g++

client:
	$(CC) -I $(INC) $(SRC)/TCPsocket.cpp $(SRC)/TCPclient.cpp \
		$(HANDLERS)/HTTPhandlers.cpp ./client.cpp -o client
