srcClient = Client.c Common.c Iris.c Communication.c
srcServer = Server.c Common.c Iris.c Communication.c
srcDealer = TrustedDealer.c Common.c Communication.c

binPath = ./bin/
binClient = client
binServer = server
binDealer = dealer

flags = -g

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