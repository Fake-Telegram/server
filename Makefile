CC=g++

CFLAGS=-c -Wall -Werror -g

LFLAGS=-lboost_system -lboost_thread -lpthread

all: server

server: server.o database.o
	$(CC) $^ -o $@ $(LFLAGS)

server.o: server.cpp database.hpp settings.hpp
	$(CC) $< $(CFLAGS) -o $@

database.o: database.cpp database.hpp
	$(CC) $< $(CFLAGS) -o $@ 

clean: 
	rm *.o server
