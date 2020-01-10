OUT		= hps phasum
OBJS		= phasum.o hashesPerSecond.o
HASH_OBJS	= pha.o sha2.o
HASH_SOURCES	= ./src/pha/pha.cpp ./src/sha2/sha2.cpp
CC		= g++

all: $(OBJS) $(HASH_OBJS)
	$(CC) phasum.o pha.o sha2.o -o phasum
	$(CC) hashesPerSecond.o -o hps pha.o sha2.o -o hps


pha.o: ./src/pha/pha.cpp
	$(CC) -masm=intel -c ./src/pha/pha.cpp -o pha.o

sha2.o: ./src/sha2/sha2.cpp
	$(CC) -c ./src/sha2/sha2.cpp -o sha2.o

hashesPerSecond.o: $(HASH_SOURCES) ./src/hashesPerSecond.cpp
	$(CC) -masm=intel $(HASH_SOURCES) -c ./src/hashesPerSecond.cpp -fpermissive

phasum.o: $(HASH_SOURCES) ./src/phasum.cpp
	$(CC) -masm=intel $(HASH_SOURCES) -c ./src/phasum.cpp

clean:
	rm -rf *.o

clean-all: clean
	rm $(OUT)
