#include <stdio.h>
#include <string.h>
#include "pcha.hpp"


/*
	This hash function use little endian
*/


/*
	Initial states and round states are generate with:

		//being 'a' the state and 'b' a prime
		a**2 = b**2 + b**2

		// Transform double to 32bit integer
		a *= 0x100000000

	primes used on initial_statess: 7..31
	primes used on round_states: 37..379
*/


const uint32 PCHA256::initial_states[8] = {
	0xe6454cd7, 0x8e6ce677, 0x6280b347, 0x0aa84ce7,
	0xdebc19b7, 0x86e3b356, 0x031f19c6, 0xd732e696
};

const uint32 PCHA256::round_states[64] = {
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

PCHA256::PCHA256() {
	this->padding = NULL;
}

void PCHA256::initializate() {

	// Reset hash
	memcpy(
		this->message_hash,
		this->initial_states,
		PCHA256_CHAR_DIGEST_SIZE
	);

	// Fill with 0s
	memset(
		this->current_chunk,
		PCHA256_INT_DIGEST_SIZE,
		0
	);

	// Delete padding if exists
	if (this->padding != NULL)
		delete [] this->padding;

	this->padding = new char[PCHA256_CHAR_BLOCK_SIZE * 2];

	this->createPadding();
}

void PCHA256::createPadding() {
	uint64 length = this->message_length;

	uint64 padLength = PCHA256_CHAR_BLOCK_SIZE - ((length + sizeof(uint64)) % PCHA256_CHAR_BLOCK_SIZE) ;

	uint64 padIndex = (length / PCHA256_CHAR_DIGEST_SIZE) * PCHA256_CHAR_DIGEST_SIZE;
	this->padIndex = padIndex;

	this->message_full_length = length + padLength + sizeof(uint64);

	char * unpackedLength = new char[sizeof(uint64)];
	memcpy(unpackedLength, &length, sizeof(uint64));

	uint64 trashLength = length - padIndex;

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
		sizeof(uint64)
	);

	// Count appended bytes
	trashLength += sizeof(uint64);

	// Fill padding left with 0s
	memset(
		&this->padding[trashLength],
		padLength - trashLength,
		0
	);

	// No longer need unpackedLength
	delete [] unpackedLength;
}

void PCHA256::getMessageChunk(uint64 index) {

	char * buffer = NULL;

	if (index < this->padIndex) {
		buffer = this->current_message;
	} else {
		buffer = this->padding;
		index -= this->padIndex;
	}

	memcpy(
		this->current_chunk,
		&buffer[index],
		sizeof(this->current_chunk)
	);
}

void PCHA256::workCurrentBlock() {

	// Define registers
	register uint32 ra = this->current_chunk[0] + this->message_hash[0];
	register uint32 rb = this->current_chunk[1] + this->message_hash[1];
	register uint32 rc = this->current_chunk[2] + this->message_hash[2];
	register uint32 rd = this->current_chunk[3] + this->message_hash[3];
	register uint32 re = this->current_chunk[4] + this->message_hash[4];
	register uint32 rf = this->current_chunk[5] + this->message_hash[5];
	register uint32 rg = this->current_chunk[6] + this->message_hash[6];
	register uint32 rh = this->current_chunk[7] + this->message_hash[7];

	// Mix registers
	for (uint32 r = 0; r < PCHA256_ROUNDS_PER_BLOCK; r++) {
		ra += rb;
		rb += rc + this->round_states[rc % 64];
		rc += rd;
		rd += PCHA256_S1(re);
		re += rf;
		rf += PCHA256_S2(rg);
		rg += rh;
		rh += PCHA256_S3(~ra);
	}

	// Update hash
	this->message_hash[0] += ra;
	this->message_hash[1] += rb;
	this->message_hash[2] += rc;
	this->message_hash[3] += rd;
	this->message_hash[4] += re;
	this->message_hash[5] += rf;
	this->message_hash[6] += rg;
	this->message_hash[7] += rh;
}

void PCHA256::getHash(char * result) {
	memcpy(
		result,
		this->message_hash,
		sizeof(this->message_hash)
	);
}

void PCHA256::digest(char * result, char * message, uint64 message_len) {

	this->current_message = message;
	this->message_length = message_len;

	this->initializate();

	for (uint64 i = 0; i < this->message_full_length; i += PCHA256_CHAR_DIGEST_SIZE) {
		this->getMessageChunk(i);
		this->workCurrentBlock();
	}

	this->getHash(result);
}

void PCHA256::hexdigest(char * hexresult, char * message, uint64 message_len) {

	char * tempResult = new char[PCHA256_CHAR_DIGEST_SIZE];

	this->digest(tempResult, message, message_len);

	// result to hex
	for (uint32 i = 0; i < PCHA256_CHAR_DIGEST_SIZE; i++) {
		snprintf(&hexresult[i * 2], 3, "%02hhx", tempResult[i]);
	}

	// No longer need tempResult
	delete [] tempResult;
}

// PCHA512

const uint64 PCHA512::initial_states[8] = {
	0xe6454cd7aa297f3c, 0x8e6ce677791ca35f,
	0x6280b34760963571, 0x0aa84ce72f895993,
	0xdebc19b71702eba5, 0x86e3b356e5f60fc8,
	0x031f19c69c62c5fc, 0xd732e69683dc580d
};

const uint64 PCHA512::round_states[64] = {
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

PCHA512::PCHA512() {
	this->padding = NULL;
}

void PCHA512::initializate() {

	// Reset hash
	memcpy(
		this->message_hash,
		this->initial_states,
		PCHA512_CHAR_DIGEST_SIZE
	);

	// Fill with 0s
	memset(
		this->current_chunk,
		PCHA512_INT_DIGEST_SIZE,
		0
	);

	// Delete padding if exists

	if (this->padding != NULL) {
		delete [] this->padding;
	}

	this->padding = new char[PCHA512_CHAR_BLOCK_SIZE * 2];

	this->createPadding();
}

void PCHA512::createPadding() {
	uint64 length = this->message_length;

	uint64 padLength = PCHA512_CHAR_BLOCK_SIZE - ((length + sizeof(uint64)) % PCHA512_CHAR_BLOCK_SIZE) ;

	uint64 padIndex = (length / PCHA512_CHAR_DIGEST_SIZE) * PCHA512_CHAR_DIGEST_SIZE;
	this->padIndex = padIndex;

	this->message_full_length = length + padLength + sizeof(uint64);

	char * unpackedLength = new char[8];
	memcpy(unpackedLength, &length, sizeof(uint64));

	uint64 trashLength = length - padIndex;


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
		8
	);

	trashLength += 8;

	// Fill padding left with 0s
	memset(
		&this->padding[trashLength],
		padLength - trashLength,
		0
	);

	// No longer need unpackedLength
	delete [] unpackedLength;
}

void PCHA512::getMessageChunk(uint64 index) {

	char * buffer = NULL;

	if (index < this->padIndex) {
		buffer = this->current_message;
	} else {
		buffer = this->padding;
		index -= this->padIndex;
	}

	memcpy(
		this->current_chunk,
		&buffer[index],
		sizeof(this->current_chunk)
	);
}

void PCHA512::workCurrentBlock() {

	// Define registers
	register uint64 ra = this->current_chunk[0] + this->message_hash[0];
	register uint64 rb = this->current_chunk[1] + this->message_hash[1];
	register uint64 rc = this->current_chunk[2] + this->message_hash[2];
	register uint64 rd = this->current_chunk[3] + this->message_hash[3];
	register uint64 re = this->current_chunk[4] + this->message_hash[4];
	register uint64 rf = this->current_chunk[5] + this->message_hash[5];
	register uint64 rg = this->current_chunk[6] + this->message_hash[6];
	register uint64 rh = this->current_chunk[7] + this->message_hash[7];

	// Mix registers
	for (uint64 r = 0; r < PCHA512_ROUNDS_PER_BLOCK; r++) {
		ra += rb;
		rb += rc + this->round_states[rc % 64];
		rc += rd;
		rd += PCHA512_S1(re);
		re += rf;
		rf += PCHA512_S2(rg);
		rg += rh;
		rh += PCHA512_S3(~ra);
	}

	// Update hash
	this->message_hash[0] += ra;
	this->message_hash[1] += rb;
	this->message_hash[2] += rc;
	this->message_hash[3] += rd;
	this->message_hash[4] += re;
	this->message_hash[5] += rf;
	this->message_hash[6] += rg;
	this->message_hash[7] += rh;
}

void PCHA512::getHash(char * result) {
	memcpy(
		result,
		this->message_hash,
		sizeof(this->message_hash)
	);
}

void PCHA512::digest(char * result, char * message, uint64 message_len) {

	this->current_message = message;
	this->message_length = message_len;

	this->initializate();

	for (uint64 i = 0; i < this->message_full_length; i += PCHA512_CHAR_DIGEST_SIZE) {
		this->getMessageChunk(i);
		this->workCurrentBlock();
	}

	this->getHash(result);
}

void PCHA512::hexdigest(char * hexresult, char * message, uint64 message_len) {

		char * tempResult = new char[PCHA512_CHAR_DIGEST_SIZE];

		this->digest(tempResult, message, message_len);

		// result to hex
		for (uint32 i = 0; i < PCHA512_CHAR_DIGEST_SIZE; i++) {
			snprintf(&hexresult[i * 2], 3, "%02hhx", tempResult[i]);
		}

		// No longer need tempResult
		delete [] tempResult;
}
