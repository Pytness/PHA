#ifndef PHA_H
#define PHA_H 1

#include <stdint.h>

#define PHA256_CHAR_DIGEST_SIZE 32
#define PHA256_INT_DIGEST_SIZE (PHA256_CHAR_DIGEST_SIZE / 4)

#define PHA256_CHAR_BLOCK_SIZE (PHA256_CHAR_DIGEST_SIZE * 2)
#define PHA256_INT_BLOCK_SIZE (PHA256_CHAR_BLOCK_SIZE / 4)

#define PHA256_ROUNDS_PER_BLOCK 64

#define PHA512_CHAR_DIGEST_SIZE 64
#define PHA512_INT_DIGEST_SIZE (PHA512_CHAR_DIGEST_SIZE / 8)

#define PHA512_CHAR_BLOCK_SIZE (PHA512_CHAR_DIGEST_SIZE * 2)
#define PHA512_INT_BLOCK_SIZE (PHA512_CHAR_BLOCK_SIZE / 8)

#define PHA512_ROUNDS_PER_BLOCK 64

class PHA {
	protected:
		uint32_t digest_size = 0;

	public:
		PHA();
		virtual void digest(char * result, char * message, uint64_t message_len);
		virtual void hexdigest(char * hexresult, char * message, uint64_t message_len);
		virtual uint32_t getDigestSize();
};

class PHA256 : public PHA {
	private:
		static const uint32_t initial_states[8];
		static const uint32_t round_states[64];

		uint32_t * current_chunk;
		uint32_t message_hash[PHA256_INT_DIGEST_SIZE];

		uint64_t message_length;
		uint64_t message_full_length;
		uint64_t padIndex;
		char * current_message;
		char * padding;

		void initializate();
		void generatePadding();
		void getMessageChunk(uint64_t index);
		void workCurrentBlock();
		void getHash(char * result);

	public:
		PHA256();

		void digest(char * result, char * message, uint64_t message_len);
		void hexdigest(char * hexresult, char * message, uint64_t message_len);
};

class PHA512 : public PHA {
	private:
		static const uint64_t initial_states[8];
		static const uint64_t round_states[64];

		uint64_t * current_chunk;
		uint64_t message_hash[PHA512_INT_DIGEST_SIZE];

		uint64_t message_length;
		uint64_t message_full_length;
		uint64_t padIndex;
		char * current_message;
		char * padding;

		void initializate();
		void generatePadding();
		void getMessageChunk(uint64_t index);
		void workCurrentBlock();
		void getHash(char * result);

	public:
		PHA512();

		void digest(char * result, char * message, uint64_t message_len);
		void hexdigest(char * hexresult, char * message, uint64_t message_len);
};

#endif
