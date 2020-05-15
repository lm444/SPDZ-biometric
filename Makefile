structuresPath = ./structures/

structuresClient = *.c
structuresServer = *.c
structuresDealer = TripleArray.c 

srcClient = Client.c Common.c Communication.c SPDZ.c Debug.c Party.c $(structuresPath)$(structuresClient)
srcServer = Server.c Common.c Communication.c SPDZ.c Debug.c Party.c $(structuresPath)$(structuresServer)
srcDealer = TrustedDealer.c Common.c Communication.c $(structuresPath)$(structuresDealer)


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