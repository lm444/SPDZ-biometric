srcClient = Client.c Common.c Iris.c
srcServer = Server.c Common.c Iris.c

binPath = ./bin/
binClient = client
binServer = server

all: client server

client: $(srcClient)
	gcc -o $(binPath)$(binClient) $(srcClient)

server: $(srcServer)
	gcc -o $(binPath)$(binServer) $(srcServer)

.PHONY: clean
clean: 
	rm -rf $(binPath)$(binClient)
	rm -rf $(binPath)$(binServer)