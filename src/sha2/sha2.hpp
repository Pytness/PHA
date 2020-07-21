#ifndef SHA2_H
#define SHA2_H

#include <stdint.h>

#define SHA256_DIGEST_SIZE 32
#define SHA256_BLOCK_SIZE  64

typedef struct {
	uint64_t tot_len;
	uint64_t len;
	uint8_t block[2 * SHA256_BLOCK_SIZE];
	uint32_t hash[8];
} sha256_ctx;

void sha256_init(sha256_ctx * ctx);
void sha256_update(sha256_ctx * ctx, const uint8_t * message, uint64_t len);
void sha256_final(sha256_ctx * ctx, uint8_t * digest);
void sha256(const uint8_t * message, uint64_t len, uint8_t * digest);

#endif
