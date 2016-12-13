CC=g++

CFLAGS=-c -Wall -Werror -g -std=c++11

LFLAGS=-lboost_system -lboost_thread -lpthread -lsqlite3 -lboost_timer

all: server

server: server.o database.o db.o
	$(CC) $^ -o $@ $(LFLAGS)

server.o: server.cpp database.hpp settings.hpp
	$(CC) $< $(CFLAGS) -o $@

database.o: database.cpp database.hpp headeronly_src/db.h
	$(CC) $< -Ilib/sqlite3 $(CFLAGS) -o $@ 

db.o: headeronly_src/db.cpp headeronly_src/db.h
	$(CC) $< -Ilib/sqlite3 $(CFLAGS) -o $@ 

clean: 
	rm *.o server
