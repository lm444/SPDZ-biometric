# SPDZ-biometric

Main idea: Asynchronous - Trusted Dealer (TD), Client and Server structure.

TD will generate a Secret Key, as well as the shares of it, and will produce multiplicative triples.
Client and Server both have to communicate with the TD:
- once at the beginning to receive the Secret Key share;
- any time it is needed to replenish multiplicative triples.

In this version, there will be three executables.
It is mandatory to run them in the following order:
- dealer -> server -> client

In the following links will be additional resources:
- [Documentation](https://docs.google.com/document/d/1Ot9xtwCZHZKuxxdobcQ-d9KGzeHg5NgLcbT9VEKzXCw) (in italian)
- [Valgrind Logs](https://drive.google.com/open?id=1wc-314hY08XFIxt-Wy1_6Git62UBIA2P)
- [Output Logs](https://drive.google.com/open?id=1160kQMSFxATaASdUsCVzuDTpn1SKiijS)
