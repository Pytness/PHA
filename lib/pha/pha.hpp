#ifndef PHA_H
#define PHA_H 1

typedef unsigned char uint8;
typedef unsigned int uint32;
typedef unsigned long long uint64;

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

#define PHA_ROTR(b, r) ((b >> r) | (b << ((sizeof(b) << 3) - r)))


#define PHA256_S1(x) (PHA_ROTR(x,  25) ^ PHA_ROTR(x,  8) ^ PHA_ROTR(x, 17))
#define PHA256_S2(x) (PHA_ROTR(x,   5) ^ PHA_ROTR(x, 16) ^ PHA_ROTR(x, 30))
#define PHA256_S3(x) (PHA_ROTR(x,  11) ^ PHA_ROTR(x, 29) ^ PHA_ROTR(x,  2))

#define PHA512_S1(x) (PHA_ROTR(x,  21) ^ PHA_ROTR(x,  9) ^ PHA_ROTR(x, 57))
#define PHA512_S2(x) (PHA_ROTR(x,  60) ^ PHA_ROTR(x, 17) ^ PHA_ROTR(x, 41))
#define PHA512_S3(x) (PHA_ROTR(x,  54) ^ PHA_ROTR(x, 22) ^ PHA_ROTR(x, 36))

class PHA {
	protected:
		uint32 digest_size = 0;

	public:
		PHA();
		virtual void digest(char * result, char * message, uint64 message_len);
		virtual void hexdigest(char * hexresult, char * message, uint64 message_len);
		virtual uint32 getDigestSize();
};

class PHA256 : public PHA {
	private:
		static const uint32 initial_states[8];
		static const uint32 round_states[64];

		uint32 * current_chunk;
		uint32 message_hash[PHA256_INT_DIGEST_SIZE];

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
		PHA256();

		void digest(char * result, char * message, uint64 message_len);
		void hexdigest(char * hexresult, char * message, uint64 message_len);
};

class PHA512 : public PHA {
	private:
		static const uint64 initial_states[8];
		static const uint64 round_states[64];

		uint64 * current_chunk;
		uint64 message_hash[PHA512_INT_DIGEST_SIZE];

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
		PHA512();

		void digest(char * result, char * message, uint64 message_len);
		void hexdigest(char * hexresult, char * message, uint64 message_len);
};

#endif
