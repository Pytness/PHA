#!/bin/bash

g++ -c ../lib/pha/pha.cpp -o pha.o -O3
g++ -c ../lib/sha2/sha2.cpp -o sha2.o -O3

g++ ../lib/pha/pha.cpp ../lib/sha2/sha2.cpp -c hashesPerSecond.cpp -O3 -fpermissive
g++ ../lib/pha/pha.cpp ../lib/sha2/sha2.cpp -c phasum.cpp -O3


g++ phasum.o -o phasum pha.o sha2.o
g++ hashesPerSecond.o -o hps pha.o sha2.o

rm *.o

#
# g++ hashesPerSecond.cpp ../lib/pha/pha.cpp ../lib/sha2/sha2.cpp -fpermissive -o hps
# g++ hashesPerSecond.cpp ../lib/pha/pha.cpp ../lib/sha2/sha2.cpp -fpermissive -O3 -o fast-hps
#
#
# g++ phasum.cpp ../lib/pha/pha.cpp -o -fpermissive phasum
