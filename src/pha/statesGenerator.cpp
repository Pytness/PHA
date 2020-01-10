#include <stdio.h>
#include <quadmath.h>

#define STATES_SIZE 72

typedef unsigned long int uint64;

char buffer[128];

const int STATES_PRIMES[STATES_SIZE] = {
	7, 11, 13, 17, 19, 23, 29, 31,
	37, 41, 43, 47, 53, 59, 61, 67,
	71, 73, 79, 83, 89, 97, 101, 103,
	107, 109, 113, 127, 131, 137, 139,
	149, 151, 157, 163, 167, 173, 179,
	181, 191, 193, 197, 199, 211, 223,
	227, 229, 233, 239, 241, 251, 257,
	263, 269, 271, 277, 281, 283, 293,
	307, 311, 313, 317, 331, 337, 347,
	349, 353, 359, 367, 373, 379
};

uint64 formula(uint64 prime) {
	__float128 b = (__float128) ((prime * prime) * 2);
	b = sqrtq(b);

	b = fmodq(b, 1.0Q);

	b *= (__float128) ((uint64) 1 << 63);
	b *= (__float128) 2;

	return (uint64) b;
}

void getStates(uint64 * states) {
	for (int i = 0; i < STATES_SIZE; i++)
		states[i] = formula(STATES_PRIMES[i]);
}

void printPHA512States(uint64 * states) {

	int i;

	printf("PHA512:\n");

	printf("\tInitial states:\n\t");

	for (i = 0; i < 8; i++) {
		printf("%016lx ", states[i]);

		if (i % 4 == 3)
			printf("\n\t");
	}

	printf("\n\tRound states:\n\t");

	for (i = 8; i < STATES_SIZE; i++) {
		printf("%016lx ", states[i]);

		if (i % 4 == 3)
			printf("\n\t");
	}

	printf("\n");
}

void printPHA256States(uint64 * states) {

	int i;

	printf("PHA256:\n");

	printf("\tInitial states:\n\t");

	for (i = 0; i < 8; i++) {
		printf("%08x ", states[i] >> 32);

		if (i % 4 == 3)
			printf("\n\t");
	}

	printf("\n\tRound states:\n\t");

	for (i = 8; i < STATES_SIZE; i++) {
		printf("%08x ", states[i] >> 32);

		if (i % 4 == 3)
			printf("\n\t");
	}

	printf("\n");
}

int main() {

	uint64 states[STATES_SIZE];

	getStates(states);

	printPHA256States(states);
	printPHA512States(states);

	return 0;
}
