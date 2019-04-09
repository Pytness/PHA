#include <stdio.h>
#include <string>
#include <chrono>
#include "../lib/pcha/pcha.hpp"
#include "../lib/sha2/sha2.hpp"

using namespace std;

void testSHA256(char * input, uint length = 1, uint secs=1) {

	uint count = 0;
	char result[PCHA256_CHAR_DIGEST_SIZE] = {0};
	PCHA256 pcha = PCHA256();

	auto start = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed;

	do {
		sha256(input, length, result);
		count += 1;
		elapsed = std::chrono::high_resolution_clock::now() - start;
	} while (elapsed.count() < (double) secs);

	printf("SHA-256: %.15lf hashes/s\n", (double)(count) / elapsed.count());
}

void testPCHA256(char * input, uint length = 1, uint secs=1) {

	uint count = 0;
	char result[PCHA256_CHAR_DIGEST_SIZE] = {0};
	PCHA256 pcha = PCHA256();

	auto start = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed;

	do {
		pcha.digest(result, input, length);
		count += 1;
		elapsed = std::chrono::high_resolution_clock::now() - start;
	} while (elapsed.count() < (double) secs);

	printf("PCHA-256: %.15lf hashes/s\n", (double)(count) / elapsed.count());
}


void testPCHA512(char * input, uint length = 1, uint secs=1) {

	uint count = 0;
	char result[PCHA512_CHAR_DIGEST_SIZE];
	PCHA512 pcha = PCHA512();

	auto start = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed;

	do {
		pcha.digest(result, input, length);
		count += 1;
		elapsed = std::chrono::high_resolution_clock::now() - start;
	}while (elapsed.count() < (double) secs);

	printf("PCHA-512: %.15lf hashes/s\n", (double)(count) / elapsed.count());
}



int main() {

	uint length = 0;
	uint seconds = 0;
	char * input = 0;

	while (length == 0) {
		printf("Enter the length  (greater than 0): ");
		scanf("%u", &length);
	}

	while (seconds == 0) {
		printf("Enter the seconds (greater than 0): ");
		scanf("%u", &seconds);
	}

	input = new char[length];

	testSHA256(input, length, seconds);
	testPCHA256(input, length, seconds);
	testPCHA512(input, length, seconds);

	delete [] input;

	return 0;
}
