# SPDZ-biometric

Main idea: Asynchronous - Trusted Dealer (TD), Client and Server structure.

TD will generate a Secret Key, as well as the shares of it, and will produce multiplicative triples.
Client and Server both have to communicate with the TD:
- once at the beginning to receive the Secret Key share;
- any time it is needed to replenish multiplicative triples.

In this version, there will be three executables.
It is mandatory to run them in the following order:
- dealer -> server -> client