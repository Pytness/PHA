#include "../pha/pha.hpp"
#include <stdio.h>
// #include "libpha.hpp"

extern "C" {
	void * PHA256_new() {
		return new PHA256();
	}

	void PHA256_delete(void * ptr) {
		PHA256 * ref = reinterpret_cast<PHA256 *>(ptr);
		delete ref;
	}

	// , char result[PHA256_CHAR_DIGEST_SIZE], char * message, uint64_t message_len

	void digest(void * ptr, char * result, char * message, uint64_t message_len) {

	        PHA256 * ref = reinterpret_cast<PHA256 *>(ptr);
	        ref->digest(result, message, message_len);
	};
}
