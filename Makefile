CFLAGS		=
OUT		= hps phasum sha2sum
OBJS		= phasum.o sha2sum.o hashesPerSecond.o
HASH_OBJS	= pha.o libpha.so sha2.o
HASH_SOURCES	= ./src/pha/pha.cpp ./src/sha2/sha2.cpp
CC		= g++
CC_FLAGS	= -O3

all: clean-all $(OBJS) $(HASH_OBJS)
	$(CC) $(CFLAGS) phasum.o pha.o -o phasum $(CC_FLAGS)
	$(CC) $(CFLAGS) sha2sum.o sha2.o -o sha2sum $(CC_FLAGS)
	$(CC) $(CFLAGS) hashesPerSecond.o pha.o sha2.o -o hps $(CC_FLAGS)

pha.o: ./src/pha/pha.cpp
	$(CC) $(CFLAGS) -c -fPIC -shared -masm=intel ./src/pha/pha.cpp -o pha.o  $(CC_FLAGS)

libpha.so: ./src/pylib/libpha.cpp
	$(CC) $(CFLAGS) -fPIC -shared -masm=intel ./src/pha/pha.cpp ./src/pylib/libpha.cpp -o libpha.so $(CC_FLAGS)

sha2.o: ./src/sha2/sha2.cpp
	$(CC) $(CFLAGS) -c ./src/sha2/sha2.cpp -o sha2.o $(CC_FLAGS)

hashesPerSecond.o: $(HASH_SOURCES) ./src/hashesPerSecond.cpp
	$(CC) $(CFLAGS) -masm=intel $(HASH_SOURCES) -c ./src/hashesPerSecond.cpp -fpermissive $(CC_FLAGS)

phasum.o: pha.o ./src/phasum.cpp
	$(CC) $(CFLAGS) -masm=intel $(HASH_SOURCES) -c ./src/phasum.cpp $(CC_FLAGS)

sha2sum.o: ./src/sha2/sha2.cpp ./src/sha2sum.cpp
	$(CC) $(CFLAGS) -masm=intel ./src/sha2/sha2.cpp -c ./src/sha2sum.cpp $(CC_FLAGS)

clean:
	rm -rf *.o || true
	rm -rf *.so || true

clean-all: clean
	rm $(OUT) || true
