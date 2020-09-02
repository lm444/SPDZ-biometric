# SPDZ-biometric

An implementation of SPDZ with a Trusted Dealer.

Developed for the Bachelor's Degree of "Sapienza Università di Roma" in "Ingegneria Informatica e Automatica", AY 2019/2020.

Two-phase protocol:
- Offline Phase, where the Trusted Dealer (TD) generates the preprocessing material;
- Online Phase, where all the three entities are active.

TD will generate the preprocessing material which will be sent to the Client and to the Server once they start the protocol.

Then Client and Server will compute the Hamming distance between two irises using SPDZ (TD is not involved).

Finally, the computation is checked via commit-and-open with the TD.

In this version, there will be three executables.
It is mandatory to run them in the following order:
- dealer -> server -> client

In the following links will be additional resources:
- [Thesis, with code documentation](https://docs.google.com/document/d/1Ot9xtwCZHZKuxxdobcQ-d9KGzeHg5NgLcbT9VEKzXCw) (in italian)
- [Valgrind Logs](https://drive.google.com/open?id=1wc-314hY08XFIxt-Wy1_6Git62UBIA2P)
- [Output Logs](https://drive.google.com/open?id=1160kQMSFxATaASdUsCVzuDTpn1SKiijS)
