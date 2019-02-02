#ifndef PCHA_H
#define PCHA_H 1

typedef unsigned char uint8;
typedef unsigned int uint32;
typedef unsigned long long uint64;

#define PCHA256_CHAR_DIGEST_SIZE 32
#define PCHA256_INT_DIGEST_SIZE (PCHA256_CHAR_DIGEST_SIZE / 4)

#define PCHA256_CHAR_BLOCK_SIZE (PCHA256_CHAR_DIGEST_SIZE * 2)
#define PCHA256_INT_BLOCK_SIZE (PCHA256_CHAR_BLOCK_SIZE / 4)

#define PCHA256_ROUNDS_PER_BLOCK 64

#define PCHA512_CHAR_DIGEST_SIZE 64
#define PCHA512_INT_DIGEST_SIZE (PCHA512_CHAR_DIGEST_SIZE / 8)

#define PCHA512_CHAR_BLOCK_SIZE (PCHA512_CHAR_DIGEST_SIZE * 2)
#define PCHA512_INT_BLOCK_SIZE (PCHA512_CHAR_BLOCK_SIZE / 8)

#define PCHA512_ROUNDS_PER_BLOCK 64

#define PCHA_ROTR(b, r) ((b >> r) | (b << ((sizeof(b) << 3) - r)))


#define PCHA256_S1(x) (PCHA_ROTR(x,  25) ^ PCHA_ROTR(x,  8) ^ PCHA_ROTR(x, 17))
#define PCHA256_S2(x) (PCHA_ROTR(x,   5) ^ PCHA_ROTR(x, 16) ^ PCHA_ROTR(x, 30))
#define PCHA256_S3(x) (PCHA_ROTR(x,  11) ^ PCHA_ROTR(x, 29) ^ PCHA_ROTR(x,  2))

#define PCHA512_S1(x) (PCHA_ROTR(x,  21) ^ PCHA_ROTR(x,  9) ^ PCHA_ROTR(x, 57))
#define PCHA512_S2(x) (PCHA_ROTR(x,  60) ^ PCHA_ROTR(x, 17) ^ PCHA_ROTR(x, 41))
#define PCHA512_S3(x) (PCHA_ROTR(x,  54) ^ PCHA_ROTR(x, 22) ^ PCHA_ROTR(x, 36))

class PCHA256 {
	private:
		const static uint32 initial_states[8];
		const static uint32 round_states[64];

		uint32 current_chunk[PCHA256_INT_DIGEST_SIZE];
		uint32 message_hash[PCHA256_INT_DIGEST_SIZE];

		uint64 message_length;
		uint64 message_full_length;
		uint64 padIndex;
		char * current_message;
		char * padding;

		void initializate();
		void createPadding();
		void getMessageChunk(uint64 index);
		void workCurrentBlock();
		void getHash(char * result);

	public:
		PCHA256();
		void digest(char * result, char * message, uint64 message_len);
		void hexdigest(char * hexresult, char * message, uint64 message_len);
};

class PCHA512 {
	private:
		const static uint64 initial_states[8];
		const static uint64 round_states[64];

		uint64 current_chunk[PCHA512_INT_DIGEST_SIZE];
		uint64 message_hash[PCHA512_INT_DIGEST_SIZE];

		uint64 message_length;
		uint64 message_full_length;
		uint64 padIndex;
		char * current_message;
		char * padding;

		void initializate();
		void createPadding();
		void getMessageChunk(uint64 index);
		void workCurrentBlock();
		void getHash(char * result);

	public:
		PCHA512();
		void digest(char * result, char * message, uint64 message_len);
		void hexdigest(char * hexresult, char * message, uint64 message_len);
};

#endif
