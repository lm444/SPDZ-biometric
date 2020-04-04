# SPDZ-biometric

Main idea: Asynchronous - Trusted Dealer (TD), Client and Server structure.

TD will generate a Secret Key, as well as the shares of it, and will produce multiplicative triples.
Client and Server both have to communicate with the TD:
- once at the beginning to receive the Secret Key share;
- any time it is needed to replenish multiplicative triples.