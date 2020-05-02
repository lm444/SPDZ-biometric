srcClient = Client.c Common.c Iris.c Communication.c SPDZ.c Debug.c MultTriple.c
srcServer = Server.c Common.c Iris.c Communication.c SPDZ.c Debug.c MultTriple.c
srcDealer = TrustedDealer.c Common.c Communication.c MultTriple.c

binPath = ./bin/
binClient = client
binServer = server
binDealer = dealer

flags = -g -Wall

all: client server dealer

client: $(srcClient)
	gcc $(flags) -o $(binPath)$(binClient) $(srcClient) 

server: $(srcServer)
	gcc $(flags) -o $(binPath)$(binServer) $(srcServer) 

dealer: $(srcDealer)
	gcc $(flags) -o $(binPath)$(binDealer) $(srcDealer)

.PHONY: clean
clean: 
	rm -rf $(binPath)$(binClient)
	rm -rf $(binPath)$(binServer)
	rm -rf $(binPath)$(binDealer)