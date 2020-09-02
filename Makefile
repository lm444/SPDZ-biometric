structuresPath = ./structures/

structuresClient = $(structuresPath)*.c
structuresServer = $(structuresPath)*.c
structuresDealer = $(structuresPath)TripleArray.c $(structuresPath)HammingDist.c

srcClient = Client.c Common.c Communication.c SPDZ.c Debug.c Converter.c $(structuresClient)
srcServer = Server.c Common.c Communication.c SPDZ.c Debug.c Converter.c $(structuresServer)
srcDealer = TrustedDealer.c Common.c Communication.c $(structuresDealer)


binPath = ./bin/
binClient = client
binServer = server
binDealer = dealer

flags = -g -Wall

all: client server dealer

client: $(srcClient)
	mkdir -p $(binPath)
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
	rmdir $(binPath)