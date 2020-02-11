/*
 * This code implements the MD5 message-digest algorithm.
 * The algorithm is due to Ron Rivest.  This code was
 * written by Colin Plumb in 1993, no copyright is claimed.
 * This code is in the public domain; do with it what you wish.
 *
 * Equivalent code is available from RSA Data Security, Inc.
 * This code has been tested against that, and is equivalent,
 * except that you don't need to include two pages of legalese
 * with every copy.
 *
 * To compute the message digest of a chunk of bytes, declare an
 * MD5Context structure, pass it to MD5Init, call MD5Update as
 * needed on buffers full of bytes, and then call MD5Final, which
 * will fill a supplied 16-byte array with the digest.
 *
 * An implementation of HMAC-MD5 (originally for OpenJK) is also
 * provided, and also released into the public domain.
 */

#pragma once

#include "qcommon/q_shared.hpp"



constexpr size_t MD5_BLOCK_SIZE = 64;
constexpr size_t MD5_DIGEST_SIZE = 16;



struct MD5Context {
	uint32_t buf[4];
	uint32_t bits[2];
	unsigned char in[64];
};

struct hmacMD5Context_t {
	struct MD5Context md5Context;
	unsigned char iKeyPad[MD5_BLOCK_SIZE];
	unsigned char oKeyPad[MD5_BLOCK_SIZE];
};



void HMAC_MD5_Final(hmacMD5Context_t *ctx, unsigned char *digest);
void HMAC_MD5_Init(hmacMD5Context_t *ctx, unsigned char const *key, unsigned int keylen);
void HMAC_MD5_Reset(hmacMD5Context_t *ctx);
void HMAC_MD5_Update(hmacMD5Context_t *ctx, unsigned char const *buf, unsigned int len);
void MD5Final(struct MD5Context *ctx, unsigned char *digest);
void MD5Init(struct MD5Context *ctx);
void MD5Update(struct MD5Context *ctx, unsigned char const *buf, unsigned len);

