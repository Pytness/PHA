#!/bin/bash

g++ hashesPerSecond.cpp ../lib/pcha/pcha.cpp ../lib/sha2/sha2.cpp -fpermissive -o hps
g++ hashesPerSecond.cpp ../lib/pcha/pcha.cpp ../lib/sha2/sha2.cpp -fpermissive -O3 -o fast-hps


g++ pchasum.cpp ../lib/pcha/pcha.cpp -o  -fpermissive pchasum
