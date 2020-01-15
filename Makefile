OUT		= hps phasum sha2sum
OBJS		= phasum.o sha2sum.o hashesPerSecond.o
HASH_OBJS	= pha.o sha2.o
HASH_SOURCES	= ./src/pha/pha.cpp ./src/sha2/sha2.cpp
CC		= g++
FLAGS		= -O3
all: $(OBJS) $(HASH_OBJS)
	$(CC) $(FLAGS) phasum.o pha.o -o phasum
	$(CC) $(FLAGS) sha2sum.o sha2.o -o sha2sum
	$(CC) $(FLAGS) hashesPerSecond.o -o hps pha.o sha2.o -o hps


pha.o: ./src/pha/pha.cpp
	$(CC) $(FLAGS) -masm=intel -c ./src/pha/pha.cpp -o pha.o

sha2.o: ./src/sha2/sha2.cpp
	$(CC) $(FLAGS) -masm=intel -c ./src/sha2/sha2.cpp -o sha2.o

hashesPerSecond.o: $(HASH_SOURCES) ./src/hashesPerSecond.cpp
	$(CC) $(FLAGS) -masm=intel $(HASH_SOURCES) -c ./src/hashesPerSecond.cpp -fpermissive

phasum.o: ./src/pha/pha.cpp ./src/phasum.cpp
	$(CC) $(FLAGS) -masm=intel $(HASH_SOURCES) -c ./src/phasum.cpp

sha2sum.o: ./src/sha2/sha2.cpp ./src/sha2sum.cpp
	$(CC) $(FLAGS) -masm=intel ./src/sha2/sha2.cpp -c ./src/sha2sum.cpp

clean:
	rm -rf *.o

clean-all: clean
	rm $(OUT)
