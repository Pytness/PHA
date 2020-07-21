<<<<<<< HEAD
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "pha.hpp"

#define PHA_ROTR(b, r) ((b >> r) | (b << ((sizeof(b) << 3) - r)))

#define PHA256_S1(x) (PHA_ROTR(x,  25) ^ PHA_ROTR(x,  8) ^ PHA_ROTR(x, 17))
#define PHA256_S2(x) (PHA_ROTR(x,   5) ^ PHA_ROTR(x, 16) ^ PHA_ROTR(x, 30))
#define PHA256_S3(x) (PHA_ROTR(x,  11) ^ PHA_ROTR(x, 29) ^ PHA_ROTR(x,  2))

#define PHA512_S1(x) (PHA_ROTR(x,  21) ^ PHA_ROTR(x,  9) ^ PHA_ROTR(x, 57))
#define PHA512_S2(x) (PHA_ROTR(x,  60) ^ PHA_ROTR(x, 17) ^ PHA_ROTR(x, 41))
#define PHA512_S3(x) (PHA_ROTR(x,  54) ^ PHA_ROTR(x, 22) ^ PHA_ROTR(x, 36))

// #define ENABLE_DEBUG 1

#ifdef ENABLE_DEBUG
#define _DEBUG printf
#define _DEBUG_CODE if (true)
#else
#define _DEBUG(...) /**/
#define _DEBUG_CODE if (false)
#endif

template <typename T>
T addWithCarry(T a, uint32_t b) {

	asm volatile (
		// "adc %[a], 0;"
		"adc rdi, rsi;"
		"mov rax, rdi;"
		::
		: "rax"
	);
}

void print_hex(char * data, uint64_t length, bool new_line = false) {
	for (int64_t i = 0; i < length; i++)
		printf("%02hhx", data[i]);

	if (new_line) printf("\n");
}


// PHA (generic class)

PHA::PHA() {};

void PHA::digest(char * hexresult, char * message, uint64_t message_len) {};
void PHA::hexdigest(char * hexresult, char * message, uint64_t message_len) {};

uint32_t PHA::getDigestSize() {
	return this->digest_size;
}

// PHA256
// Define initial and round constants for PHA256 bits

const uint32_t PHA256::initial_states[8] = {
	0xe6454cd7, 0x8e6ce677, 0x6280b347, 0x0aa84ce7,
	0xdebc19b7, 0x86e3b356, 0x031f19c6, 0xd732e696
};

const uint32_t PHA256::round_states[64] = {
	0x536e4d06, 0xfb95e6a6, 0xcfa9b375, 0x77d14d15,
	0xf40cb385, 0x704819f5, 0x445be6c5, 0xc0974d34,
	0x68bee6d4, 0x3cd2b3a4, 0xb90e1a14, 0x6135b3b4,
	0xdd711a23, 0x2dc04d63, 0xd5e7e703, 0xa9fbb3d3,
	0x52234d72, 0x26371a42, 0xce5eb3e2, 0x9ae94d91,
	0x4310e731, 0xbf4c4da1, 0x93601a71, 0xb7c31a80,
	0x8bd6e750, 0x08124dc0, 0x844db430, 0x2c754dd0,
	0xa8b0b43f, 0x24ec1aaf, 0xf8ffe77f, 0x1d62e78e,
	0xf176b45e, 0x999e4dfe, 0x6db21ace, 0x6628e7ad,
	0x5e9fb48d, 0x06c74e2d, 0xdadb1afd, 0x8302b49c,
	0xff3e1b0c, 0xd351e7dc, 0xf7b4e7eb, 0x73f04e5b,
	0xf02bb4cb, 0x6c671b3b, 0x407ae80b, 0xbcb64e7a,
	0x64dde81a, 0x38f1b4ea, 0x5d54b4f9, 0x29df4ea9,
	0xd206e849, 0xa61ab519, 0x4e424eb8, 0x1acce868,
	0x97084ed7, 0xbb6b4ee7, 0x8f7f1bb7, 0x37a6b557,
	0xb3e21bc6, 0x04314f06, 0x806cb576, 0xfca81be5
};

PHA256::PHA256() {
	_DEBUG("new PHA256. %d bytes at %p, \n", sizeof(PHA256), this);

	this->padding = NULL;
	this->current_chunk = NULL;
	this->digest_size = PHA256_CHAR_DIGEST_SIZE;
}

void PHA256::initializate() {

	_DEBUG("PHA256(%p) initializing\n", this);

	// Reset hash
	memcpy(
		this->message_hash,
		this->initial_states,
		PHA256_CHAR_DIGEST_SIZE
	);

	// Delete padding if exists
	if (this->padding != NULL)
		delete [] this->padding;

	this->padding = new char[PHA256_CHAR_BLOCK_SIZE * 2]();

	this->generatePadding();
}

void PHA256::generatePadding() {
	_DEBUG("PHA256(%p) generating padding\n", this);

	uint64_t length = this->message_length;

	uint64_t padLength = PHA256_CHAR_BLOCK_SIZE - (
		(length + sizeof(uint64_t)) % PHA256_CHAR_BLOCK_SIZE
	) ;

	uint64_t padIndex = (length / PHA256_CHAR_DIGEST_SIZE) *
		PHA256_CHAR_DIGEST_SIZE;

	this->padIndex = padIndex;

	this->message_full_length = length + padLength + sizeof(uint64_t);

	char * unpackedLength = new char[sizeof(uint64_t)];
	memcpy(unpackedLength, &length, sizeof(uint64_t));

	uint64_t trashLength = length - padIndex;

	// Copy 'trash' to padding
	memcpy(
		this->padding,
		&this->current_message[padIndex],
		trashLength
	);

	// Append unpacked length to padding
	memcpy(
		&this->padding[trashLength],
		unpackedLength,
		sizeof(uint64_t)
	);

	// Count appended bytes
	trashLength += sizeof(uint64_t);

	// Fill padding left with 0s
	memset(
		&this->padding[trashLength],
		padLength - trashLength,
		0
	);

	delete [] unpackedLength;
}

void PHA256::getMessageChunk(uint64_t index) {

	_DEBUG("PHA256(%p) getting message chunk\n", this);

	char * buffer = NULL;

	// Get chunk either from padded message or message
	if (index < this->padIndex) {
		buffer = this->current_message;
	} else {
		buffer = this->padding;
		index -= this->padIndex;
	}

	this->current_chunk = (uint32_t *) &buffer[index];
}

void PHA256::workCurrentBlock() {

	_DEBUG("PHA256(%p) working block\n", this);

	// Define registers
	register uint32_t ra = this->current_chunk[0] + this->message_hash[0];
	register uint32_t rb = this->current_chunk[1] + this->message_hash[1];
	register uint32_t rc = this->current_chunk[2] + this->message_hash[2];
	register uint32_t rd = this->current_chunk[3] + this->message_hash[3];
	register uint32_t re = this->current_chunk[4] + this->message_hash[4];
	register uint32_t rf = this->current_chunk[5] + this->message_hash[5];
	register uint32_t rg = this->current_chunk[6] + this->message_hash[6];
	register uint32_t rh = this->current_chunk[7] + this->message_hash[7];

	// Mix registers
	for (uint32_t r = 0; r < PHA256_ROUNDS_PER_BLOCK; r++) {
		rh += PHA256_S1(~ra);
		rg += rh;
		rf += PHA256_S2(~rg);
		re += rf;
		rd += PHA256_S3(~re);
		rc += rd;
		rb += rc + this->round_states[rb % 64];
		ra += rb;
	}

	this->message_hash[0] = ra;
	this->message_hash[1] = rb;
	this->message_hash[2] = rc;
	this->message_hash[3] = rd;
	this->message_hash[4] = re;
	this->message_hash[5] = rf;
	this->message_hash[6] = rg;
	this->message_hash[7] = rh;
}

void PHA256::getHash(char * result) {

	_DEBUG("PHA256(%p) getting hash\n", this);

	// Copy hash into result
	memcpy(
		result,
		this->message_hash,
		sizeof(this->message_hash)
	);
}

void PHA256::digest(char * result, char * message, uint64_t message_len) {

	_DEBUG("PHA256(%p) digesting %d bytes\n", this, message_len);

	// printf("message: '%s'\n", message);
	// printf("message_len: %d\n", message_len);
	this->current_message = message;
	this->message_length = message_len;

	this->initializate();

	_DEBUG_CODE {
		printf("\tCurrent message dump ", this->message_length);
		print_hex(this->current_message, this->message_length, true);

		printf("\tPadding ", this->message_length);
		print_hex(this->padding, 32, true);

		printf("\tInitial states: ");
		print_hex((char *) this->initial_states, 32, true);

		printf("\tMessage hash: ");
		print_hex((char *) this->message_hash, 32, true);
	}


	// for each chunk
	for (uint64_t i = 0; i < this->message_full_length; i += PHA256_CHAR_DIGEST_SIZE) {
		this->getMessageChunk(i);
		this->workCurrentBlock();

		_DEBUG_CODE {
			printf("\tMessage hash: ");
			print_hex((char *) this->message_hash, 32, true);
		}
	}

	this->getHash(result);
}

void PHA256::hexdigest(char * hexresult, char * message, uint64_t message_len) {

	char * tempResult = new char[PHA256_CHAR_DIGEST_SIZE];

	this->digest(tempResult, message, message_len);

	// result to hex
	for (uint32_t i = 0; i < PHA256_CHAR_DIGEST_SIZE; i++) {
		snprintf(&hexresult[i * 2], 3, "%02hhx", tempResult[i]);
	}

	delete [] tempResult;
}

// PHA512

// define initial and round constants for PHA512

const uint64_t PHA512::initial_states[8] = {
	0xe6454cd7aa297f3c, 0x8e6ce677791ca35f,
	0x6280b34760963571, 0x0aa84ce72f895993,
	0xdebc19b71702eba5, 0x86e3b356e5f60fc8,
	0x031f19c69c62c5fc, 0xd732e69683dc580d
};

const uint64_t PHA512::round_states[64] = {
	0x536e4d063a490e41, 0xfb95e6a6093c3264, 0xcfa9b375f0b5c476, 0x77d14d15bfa8e898,
	0xf40cb38576159ecd, 0x704819f52c825501, 0x445be6c513fbe712, 0xc0974d34ca689d46,
	0x68bee6d4995bc169, 0x3cd2b3a480d5537b, 0xb90e1a14374209af, 0x6135b3b406352dd2,
	0xdd711a23bca1e406, 0x2dc04d635a882c4b, 0xd5e7e703297b506e, 0xa9fbb3d310f4e280,
	0x52234d72dfe806a2, 0x26371a42c76198b4, 0xce5eb3e29654bcd7, 0x9ae94d91eaa7bb50,
	0x4310e731b99adf73, 0xbf4c4da1700795a7, 0x93601a71578127b9, 0xb7c31a80dce10210,
	0x8bd6e750c45a9421, 0x08124dc07ac74a55, 0x844db43031340089, 0x2c754dd0002724ac,
	0xa8b0b43fb693dae0, 0x24ec1aaf6d009115, 0xf8ffe77f547a2326, 0x1d62e78ed9d9fd7d,
	0xf176b45ec1538f8e, 0x999e4dfe9046b3b1, 0x6db21ace77c045c3, 0x6628e7ade499b22b,
	0x5e9fb48d51731e93, 0x06c74e2d206642b6, 0xdadb1afd07dfd4c8, 0x8302b49cd6d2f8ea,
	0xff3e1b0c8d3faf1f, 0xd351e7dc74b94130, 0xf7b4e7ebfa191b87, 0x73f04e5bb085d1bb,
	0xf02bb4cb66f287ef, 0x6c671b3b1d5f3e24, 0x407ae80b04d8d035, 0xbcb64e7abb458669,
	0x64dde81a8a38aa8c, 0x38f1b4ea71b23c9d, 0x5d54b4f9f71216f4, 0x29df4ea94b65156e,
	0xd206e8491a583991, 0xa61ab51901d1cba2, 0x4e424eb8d0c4efc5, 0x1acce8682517ee3f,
	0x97084ed7db84a473, 0xbb6b4ee760e47eca, 0x8f7f1bb7485e10dc, 0x37a6b557175134fe,
	0xb3e21bc6cdbdeb32, 0x04314f066ba43378, 0x806cb5762210e9ac, 0xfca81be5d87d9fe0
};

PHA512::PHA512() {
	this->padding = NULL;
	this->current_chunk = NULL;
	this->digest_size = PHA512_CHAR_DIGEST_SIZE;
}


void PHA512::initializate() {

	// Reset hash
	memcpy(
		this->message_hash,
		this->initial_states,
		PHA512_CHAR_DIGEST_SIZE
	);


	// Delete padding if exists
	if (this->padding != NULL)
		delete [] this->padding;

	this->padding = new char[PHA512_CHAR_BLOCK_SIZE * 2];

	this->generatePadding();
}

void PHA512::generatePadding() {
	uint64_t length = this->message_length;

	uint64_t padLength = PHA512_CHAR_BLOCK_SIZE - (
		(length + sizeof(uint64_t)) % PHA512_CHAR_BLOCK_SIZE
	) ;

	uint64_t padIndex = (length / PHA512_CHAR_DIGEST_SIZE) *
		PHA512_CHAR_DIGEST_SIZE;

	this->padIndex = padIndex;

	this->message_full_length = length + padLength + sizeof(uint64_t);

	char * unpackedLength = new char[sizeof(uint64_t)];
	memcpy(unpackedLength, &length, sizeof(uint64_t));

	uint64_t trashLength = length - padIndex;

	// Copy 'trash' to padding
	memcpy(
		this->padding,
		&this->current_message[padIndex],
		trashLength
	);

	// Append unpacked length to padding
	memcpy(
		&this->padding[trashLength],
		unpackedLength,
		sizeof(uint64_t)
	);

	// Count appended bytes
	trashLength += sizeof(uint64_t);

	// Fill padding left with 0s
	memset(
		&this->padding[trashLength],
		padLength - trashLength,
		0
	);

	delete [] unpackedLength;
}

void PHA512::getMessageChunk(uint64_t index) {

	char * buffer = NULL;

	if (index < this->padIndex) {
		buffer = this->current_message;
	} else {
		buffer = this->padding;
		index -= this->padIndex;
	}

	this->current_chunk = (uint64_t *) &buffer[index];

}

void PHA512::workCurrentBlock() {

	// Define registers
	register uint64_t ra = this->current_chunk[0] + this->message_hash[0];
	register uint64_t rb = this->current_chunk[1] + this->message_hash[1];
	register uint64_t rc = this->current_chunk[2] + this->message_hash[2];
	register uint64_t rd = this->current_chunk[3] + this->message_hash[3];
	register uint64_t re = this->current_chunk[4] + this->message_hash[4];
	register uint64_t rf = this->current_chunk[5] + this->message_hash[5];
	register uint64_t rg = this->current_chunk[6] + this->message_hash[6];
	register uint64_t rh = this->current_chunk[7] + this->message_hash[7];

	// Mix registers
	for (uint64_t r = 0; r < PHA512_ROUNDS_PER_BLOCK; r++) {
		ra ^= rb;
		rb ^= rc + this->round_states[rc % 64];
		rc ^= rd;
		rd ^= PHA512_S1(~re);
		re ^= rf;
		rf ^= PHA512_S2(~rg);
		rg ^= rh;
		rh ^= PHA512_S3(~ra);
	}

	// Update hash
	this->message_hash[0] = ra;
	this->message_hash[1] = rb;
	this->message_hash[2] = rc;
	this->message_hash[3] = rd;
	this->message_hash[4] = re;
	this->message_hash[5] = rf;
	this->message_hash[6] = rg;
	this->message_hash[7] = rh;
}

void PHA512::getHash(char * result) {
	// Copy hash into result
	memcpy(
		result,
		this->message_hash,
		sizeof(this->message_hash)
	);
}

void PHA512::digest(char * result, char * message, uint64_t message_len) {

	this->current_message = message;
	this->message_length = message_len;

	this->initializate();

	// for each chunk
	for (uint64_t i = 0; i < this->message_full_length; i += PHA512_CHAR_DIGEST_SIZE) {
		this->getMessageChunk(i);
		this->workCurrentBlock();
	}

	this->getHash(result);
}

void PHA512::hexdigest(char * hexresult, char * message, uint64_t message_len) {

		char * tempResult = new char[PHA512_CHAR_DIGEST_SIZE];

		this->digest(tempResult, message, message_len);

		// result to hex
		for (uint32_t i = 0; i < PHA512_CHAR_DIGEST_SIZE; i++) {
			snprintf(&hexresult[i * 2], 3, "%02hhx", tempResult[i]);
		}

		delete [] tempResult;
}
=======
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "pha.hpp"

#define PHA_ROTR(b, r) ((b >> r) | (b << ((sizeof(b) << 3) - r)))

#define PHA256_S1(x) (PHA_ROTR(x,  25) ^ PHA_ROTR(x,  8) ^ PHA_ROTR(x, 17))
#define PHA256_S2(x) (PHA_ROTR(x,   5) ^ PHA_ROTR(x, 16) ^ PHA_ROTR(x, 30))
#define PHA256_S3(x) (PHA_ROTR(x,  11) ^ PHA_ROTR(x, 29) ^ PHA_ROTR(x,  2))

#define PHA512_S1(x) (PHA_ROTR(x,  21) ^ PHA_ROTR(x,  9) ^ PHA_ROTR(x, 57))
#define PHA512_S2(x) (PHA_ROTR(x,  60) ^ PHA_ROTR(x, 17) ^ PHA_ROTR(x, 41))
#define PHA512_S3(x) (PHA_ROTR(x,  54) ^ PHA_ROTR(x, 22) ^ PHA_ROTR(x, 36))


template <typename T>
T addWithCarry(T a, uint32_t b) {

	asm volatile (
		// "adc %[a], 0;"
		"adc rdi, rsi;"
		"mov rax, rdi;"
		::
		: "rax"
	);
}

// PHA (generic class)

PHA::PHA() {};

void PHA::digest(char * hexresult, char * message, uint64_t message_len) {};
void PHA::hexdigest(char * hexresult, char * message, uint64_t message_len) {};

uint32_t PHA::getDigestSize() {
	return this->digest_size;
}

// PHA256
// Define initial and round constants for PHA256 bits

const uint32_t PHA256::initial_states[8] = {
	0xe6454cd7, 0x8e6ce677, 0x6280b347, 0x0aa84ce7,
	0xdebc19b7, 0x86e3b356, 0x031f19c6, 0xd732e696
};

const uint32_t PHA256::round_states[64] = {
	0x536e4d06, 0xfb95e6a6, 0xcfa9b375, 0x77d14d15,
	0xf40cb385, 0x704819f5, 0x445be6c5, 0xc0974d34,
	0x68bee6d4, 0x3cd2b3a4, 0xb90e1a14, 0x6135b3b4,
	0xdd711a23, 0x2dc04d63, 0xd5e7e703, 0xa9fbb3d3,
	0x52234d72, 0x26371a42, 0xce5eb3e2, 0x9ae94d91,
	0x4310e731, 0xbf4c4da1, 0x93601a71, 0xb7c31a80,
	0x8bd6e750, 0x08124dc0, 0x844db430, 0x2c754dd0,
	0xa8b0b43f, 0x24ec1aaf, 0xf8ffe77f, 0x1d62e78e,
	0xf176b45e, 0x999e4dfe, 0x6db21ace, 0x6628e7ad,
	0x5e9fb48d, 0x06c74e2d, 0xdadb1afd, 0x8302b49c,
	0xff3e1b0c, 0xd351e7dc, 0xf7b4e7eb, 0x73f04e5b,
	0xf02bb4cb, 0x6c671b3b, 0x407ae80b, 0xbcb64e7a,
	0x64dde81a, 0x38f1b4ea, 0x5d54b4f9, 0x29df4ea9,
	0xd206e849, 0xa61ab519, 0x4e424eb8, 0x1acce868,
	0x97084ed7, 0xbb6b4ee7, 0x8f7f1bb7, 0x37a6b557,
	0xb3e21bc6, 0x04314f06, 0x806cb576, 0xfca81be5
};

PHA256::PHA256() {
	this->padding = NULL;
	this->current_chunk = NULL;
	this->digest_size = PHA256_CHAR_DIGEST_SIZE;
}

void PHA256::initializate() {

	// Reset hash
	memcpy(
		this->message_hash,
		this->initial_states,
		PHA256_CHAR_DIGEST_SIZE
	);

	// Delete padding if exists
	if (this->padding != NULL)
		delete [] this->padding;

	this->padding = new char[PHA256_CHAR_BLOCK_SIZE * 2];

	this->generatePadding();
}

void PHA256::generatePadding() {
	uint64_t length = this->message_length;

	uint64_t padLength = PHA256_CHAR_BLOCK_SIZE - (
		(length + sizeof(uint64_t)) % PHA256_CHAR_BLOCK_SIZE
	) ;

	uint64_t padIndex = (length / PHA256_CHAR_DIGEST_SIZE) *
		PHA256_CHAR_DIGEST_SIZE;

	this->padIndex = padIndex;

	this->message_full_length = length + padLength + sizeof(uint64_t);

	char * unpackedLength = new char[sizeof(uint64_t)];
	memcpy(unpackedLength, &length, sizeof(uint64_t));

	uint64_t trashLength = length - padIndex;

	// Copy 'trash' to padding
	memcpy(
		this->padding,
		&this->current_message[padIndex],
		trashLength
	);

	// Append unpacked length to padding
	memcpy(
		&this->padding[trashLength],
		unpackedLength,
		sizeof(uint64_t)
	);

	// Count appended bytes
	trashLength += sizeof(uint64_t);

	// Fill padding left with 0s
	memset(
		&this->padding[trashLength],
		padLength - trashLength,
		0
	);

	delete [] unpackedLength;
}

void PHA256::getMessageChunk(uint64_t index) {

	char * buffer = NULL;

	// Get chunk either from padded message or message
	if (index < this->padIndex) {
		buffer = this->current_message;
	} else {
		buffer = this->padding;
		index -= this->padIndex;
	}

	this->current_chunk = (uint32_t *) &buffer[index];
}

void PHA256::workCurrentBlock() {

	// Define registers
	register uint32_t ra = this->current_chunk[0] + this->message_hash[0];
	register uint32_t rb = this->current_chunk[1] + this->message_hash[1];
	register uint32_t rc = this->current_chunk[2] + this->message_hash[2];
	register uint32_t rd = this->current_chunk[3] + this->message_hash[3];
	register uint32_t re = this->current_chunk[4] + this->message_hash[4];
	register uint32_t rf = this->current_chunk[5] + this->message_hash[5];
	register uint32_t rg = this->current_chunk[6] + this->message_hash[6];
	register uint32_t rh = this->current_chunk[7] + this->message_hash[7];

	// Mix registers
	for (uint32_t r = 0; r < PHA256_ROUNDS_PER_BLOCK; r++) {
		ra ^= rb;
		rb ^= rc + this->round_states[rc & 0x40];
		rc ^= rd;
		rd ^= PHA256_S1(~re);
		re ^= rf;
		rf ^= PHA256_S2(rg);
		rg ^= rh;
		rh ^= PHA256_S3(~ra);
	}

	// Update hash
	this->message_hash[0] += ra;

	// this->message_hash[1] = addWithCarry(this->message_hash[1], rb);
	// this->message_hash[2] = addWithCarry(this->message_hash[2], rc);
	// this->message_hash[3] = addWithCarry(this->message_hash[3], rd);
	// this->message_hash[4] = addWithCarry(this->message_hash[4], re);
	// this->message_hash[5] = addWithCarry(this->message_hash[5], rf);
	// this->message_hash[6] = addWithCarry(this->message_hash[6], rg);
	// this->message_hash[7] = addWithCarry(this->message_hash[7], rh);

	this->message_hash[1] += rb;
	this->message_hash[2] += rc;
	this->message_hash[3] += rd;
	this->message_hash[4] += re;
	this->message_hash[5] += rf;
	this->message_hash[6] += rg;
	this->message_hash[7] += rh;
}

void PHA256::getHash(char * result) {
	// Copy hash into result
	memcpy(
		result,
		this->message_hash,
		sizeof(this->message_hash)
	);
}

void PHA256::digest(char * result, char * message, uint64_t message_len) {

	this->current_message = message;
	this->message_length = message_len;

	this->initializate();

	// for each chunk
	for (uint64_t i = 0; i < this->message_full_length; i += PHA256_CHAR_DIGEST_SIZE) {
		this->getMessageChunk(i);
		this->workCurrentBlock();
	}

	this->getHash(result);
}

void PHA256::hexdigest(char * hexresult, char * message, uint64_t message_len) {

	char * tempResult = new char[PHA256_CHAR_DIGEST_SIZE];

	this->digest(tempResult, message, message_len);

	// result to hex
	for (uint32_t i = 0; i < PHA256_CHAR_DIGEST_SIZE; i++) {
		snprintf(&hexresult[i * 2], 3, "%02hhx", tempResult[i]);
	}

	delete [] tempResult;
}

// PHA512

// define initial and round constants for PHA512

const uint64_t PHA512::initial_states[8] = {
	0xe6454cd7aa297f3c, 0x8e6ce677791ca35f,
	0x6280b34760963571, 0x0aa84ce72f895993,
	0xdebc19b71702eba5, 0x86e3b356e5f60fc8,
	0x031f19c69c62c5fc, 0xd732e69683dc580d
};

const uint64_t PHA512::round_states[64] = {
	0x536e4d063a490e41, 0xfb95e6a6093c3264, 0xcfa9b375f0b5c476, 0x77d14d15bfa8e898,
	0xf40cb38576159ecd, 0x704819f52c825501, 0x445be6c513fbe712, 0xc0974d34ca689d46,
	0x68bee6d4995bc169, 0x3cd2b3a480d5537b, 0xb90e1a14374209af, 0x6135b3b406352dd2,
	0xdd711a23bca1e406, 0x2dc04d635a882c4b, 0xd5e7e703297b506e, 0xa9fbb3d310f4e280,
	0x52234d72dfe806a2, 0x26371a42c76198b4, 0xce5eb3e29654bcd7, 0x9ae94d91eaa7bb50,
	0x4310e731b99adf73, 0xbf4c4da1700795a7, 0x93601a71578127b9, 0xb7c31a80dce10210,
	0x8bd6e750c45a9421, 0x08124dc07ac74a55, 0x844db43031340089, 0x2c754dd0002724ac,
	0xa8b0b43fb693dae0, 0x24ec1aaf6d009115, 0xf8ffe77f547a2326, 0x1d62e78ed9d9fd7d,
	0xf176b45ec1538f8e, 0x999e4dfe9046b3b1, 0x6db21ace77c045c3, 0x6628e7ade499b22b,
	0x5e9fb48d51731e93, 0x06c74e2d206642b6, 0xdadb1afd07dfd4c8, 0x8302b49cd6d2f8ea,
	0xff3e1b0c8d3faf1f, 0xd351e7dc74b94130, 0xf7b4e7ebfa191b87, 0x73f04e5bb085d1bb,
	0xf02bb4cb66f287ef, 0x6c671b3b1d5f3e24, 0x407ae80b04d8d035, 0xbcb64e7abb458669,
	0x64dde81a8a38aa8c, 0x38f1b4ea71b23c9d, 0x5d54b4f9f71216f4, 0x29df4ea94b65156e,
	0xd206e8491a583991, 0xa61ab51901d1cba2, 0x4e424eb8d0c4efc5, 0x1acce8682517ee3f,
	0x97084ed7db84a473, 0xbb6b4ee760e47eca, 0x8f7f1bb7485e10dc, 0x37a6b557175134fe,
	0xb3e21bc6cdbdeb32, 0x04314f066ba43378, 0x806cb5762210e9ac, 0xfca81be5d87d9fe0
};

PHA512::PHA512() {
	this->padding = NULL;
	this->current_chunk = NULL;
	this->digest_size = PHA512_CHAR_DIGEST_SIZE;
}


void PHA512::initializate() {

	// Reset hash
	memcpy(
		this->message_hash,
		this->initial_states,
		PHA512_CHAR_DIGEST_SIZE
	);


	// Delete padding if exists
	if (this->padding != NULL)
		delete [] this->padding;

	this->padding = new char[PHA512_CHAR_BLOCK_SIZE * 2];

	this->generatePadding();
}

void PHA512::generatePadding() {
	uint64_t length = this->message_length;

	uint64_t padLength = PHA512_CHAR_BLOCK_SIZE - (
		(length + sizeof(uint64_t)) % PHA512_CHAR_BLOCK_SIZE
	) ;

	uint64_t padIndex = (length / PHA512_CHAR_DIGEST_SIZE) *
		PHA512_CHAR_DIGEST_SIZE;

	this->padIndex = padIndex;

	this->message_full_length = length + padLength + sizeof(uint64_t);

	char * unpackedLength = new char[sizeof(uint64_t)];
	memcpy(unpackedLength, &length, sizeof(uint64_t));

	uint64_t trashLength = length - padIndex;

	// Copy 'trash' to padding
	memcpy(
		this->padding,
		&this->current_message[padIndex],
		trashLength
	);

	// Append unpacked length to padding
	memcpy(
		&this->padding[trashLength],
		unpackedLength,
		sizeof(uint64_t)
	);

	// Count appended bytes
	trashLength += sizeof(uint64_t);

	// Fill padding left with 0s
	memset(
		&this->padding[trashLength],
		padLength - trashLength,
		0
	);

	delete [] unpackedLength;
}

void PHA512::getMessageChunk(uint64_t index) {

	char * buffer = NULL;

	if (index < this->padIndex) {
		buffer = this->current_message;
	} else {
		buffer = this->padding;
		index -= this->padIndex;
	}

	this->current_chunk = (uint64_t *) &buffer[index];

}

void PHA512::workCurrentBlock() {

	// Define registers
	register uint64_t ra = this->current_chunk[0] + this->message_hash[0];
	register uint64_t rb = this->current_chunk[1] + this->message_hash[1];
	register uint64_t rc = this->current_chunk[2] + this->message_hash[2];
	register uint64_t rd = this->current_chunk[3] + this->message_hash[3];
	register uint64_t re = this->current_chunk[4] + this->message_hash[4];
	register uint64_t rf = this->current_chunk[5] + this->message_hash[5];
	register uint64_t rg = this->current_chunk[6] + this->message_hash[6];
	register uint64_t rh = this->current_chunk[7] + this->message_hash[7];

	// Mix registers
	for (uint64_t r = 0; r < PHA512_ROUNDS_PER_BLOCK; r++) {
		ra ^= rb;
		rb ^= rc + this->round_states[rc % 64];
		rc ^= rd;
		rd ^= PHA512_S1(~re);
		re ^= rf;
		rf ^= PHA512_S2(rg);
		rg ^= rh;
		rh ^= PHA512_S3(~ra);
	}

	// Update hash
	this->message_hash[0] += ra;

	// this->message_hash[1] = addWithCarry(this->message_hash[1], rb);
	// this->message_hash[2] = addWithCarry(this->message_hash[2], rc);
	// this->message_hash[3] = addWithCarry(this->message_hash[3], rd);
	// this->message_hash[4] = addWithCarry(this->message_hash[4], re);
	// this->message_hash[5] = addWithCarry(this->message_hash[5], rf);
	// this->message_hash[6] = addWithCarry(this->message_hash[6], rg);
	// this->message_hash[7] = addWithCarry(this->message_hash[7], rh);

	this->message_hash[1] += rb;
	this->message_hash[2] += rc;
	this->message_hash[3] += rd;
	this->message_hash[4] += re;
	this->message_hash[5] += rf;
	this->message_hash[6] += rg;
	this->message_hash[7] += rh;
}

void PHA512::getHash(char * result) {
	// Copy hash into result
	memcpy(
		result,
		this->message_hash,
		sizeof(this->message_hash)
	);
}

void PHA512::digest(char * result, char * message, uint64_t message_len) {

	this->current_message = message;
	this->message_length = message_len;

	this->initializate();

	// for each chunk
	for (uint64_t i = 0; i < this->message_full_length; i += PHA512_CHAR_DIGEST_SIZE) {
		this->getMessageChunk(i);
		this->workCurrentBlock();
	}

	this->getHash(result);
}

void PHA512::hexdigest(char * hexresult, char * message, uint64_t message_len) {

		char * tempResult = new char[PHA512_CHAR_DIGEST_SIZE];

		this->digest(tempResult, message, message_len);

		// result to hex
		for (uint32_t i = 0; i < PHA512_CHAR_DIGEST_SIZE; i++) {
			snprintf(&hexresult[i * 2], 3, "%02hhx", tempResult[i]);
		}

		delete [] tempResult;
}
>>>>>>> master
