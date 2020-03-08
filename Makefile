all: client server

client: Client.cpp Common.h Common.cpp
	g++ -o client Client.cpp Common.h Common.cpp

server: Server.cpp Common.h Common.cpp
	g++ -o server Server.cpp Common.h Common.cpp
