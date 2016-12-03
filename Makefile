all: server

server: server.cpp
	g++ $^ -o $@ -lboost_system -lboost_thread -lpthread
