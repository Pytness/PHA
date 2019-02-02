#include <stdio.h>
#include <string.h>
#include "../src/pcha.hpp"

int main(int argc, char * argv[]) {

	if (argc != 2) {
		printf("Usage: %s <string>\n", argv[0]);
	} else {
		char * input = argv[1];
		int inputLength = strlen(input);

		PCHA256 pcha256 = PCHA256();
		PCHA512 pcha512 = PCHA512();

		char hexresult[(PCHA512_CHAR_DIGEST_SIZE * 2) + 1] = "";

		pcha256.hexdigest(hexresult, input, inputLength);
		printf("PCHA-256: %s\n", hexresult);

		pcha512.hexdigest(hexresult, input, inputLength);
		printf("PCHA-512: %s\n", hexresult);
	}
}
