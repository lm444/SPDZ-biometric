srcClient = Client.c Common.c Iris.c Communication.c
srcServer = Server.c Common.c Iris.c Communication.c
srcDealer = TrustedDealer.c Common.c Communication.c

binPath = ./bin/
binClient = client
binServer = server
binDealer = dealer

all: client server dealer

client: $(srcClient)
	gcc -o $(binPath)$(binClient) $(srcClient)

server: $(srcServer)
	gcc -o $(binPath)$(binServer) $(srcServer)

dealer: $(srcDealer)
	gcc -o $(binPath)$(binDealer) $(srcDealer)

.PHONY: clean
clean: 
	rm -rf $(binPath)$(binClient)
	rm -rf $(binPath)$(binServer)
	rm -rf $(binPath)$(binDealer)