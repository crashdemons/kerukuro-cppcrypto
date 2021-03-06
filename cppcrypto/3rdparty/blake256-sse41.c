
// BLAKE-256 sse4.1 eBASH implementation
// authors:   Jean-Philippe Aumasson <jeanphilippe.aumasson@gmail.com>
//            Peter Schwabe <peter@cryptojedi.org>
//           Samuel Neves  <sneves@dei.uc.pt>
//
// This implementation assumes that no salt is used.
// 
// Level of copyright protection: 0
// Level of patent protection: 0

// Modified by kerukuro for use in cppcrypto.


#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <immintrin.h>
#include <sys/stat.h>


typedef uint64_t u64;
typedef uint32_t u32;
typedef uint8_t u8;


#define U8TO32(p)					\
  (((u32)((p)[0]) << 24) | ((u32)((p)[1]) << 16) |	\
   ((u32)((p)[2]) <<  8) | ((u32)((p)[3])      ))
#define U32TO8(p, v)					\
  (p)[0] = (u8)((v) >> 24); (p)[1] = (u8)((v) >> 16);	\
  (p)[2] = (u8)((v) >>  8); (p)[3] = (u8)((v)      ); 

#define LOADU(p)  _mm_loadu_si128( (__m128i *)(p) )
#define BSWAP32(r) do { \
   r = _mm_shufflehi_epi16(r, _MM_SHUFFLE(2, 3, 0, 1)); \
   r = _mm_shufflelo_epi16(r, _MM_SHUFFLE(2, 3, 0, 1)); \
   r = _mm_xor_si128(_mm_slli_epi16(r, 8), _mm_srli_epi16(r, 8)); \
} while(0)



int blake256_compress_sse41(u32* h, int padding, u64 total, const u8 * datablock) 
{

	__m128i row1, row2, row3, row4;
	__m128i buf1, buf2;
	const __m128i r8 = _mm_set_epi8(12, 15, 14, 13, 8, 11, 10, 9, 4, 7, 6, 5, 0, 3, 2, 1);
	const __m128i r16 = _mm_set_epi8(13, 12, 15, 14, 9, 8, 11, 10, 5, 4, 7, 6, 1, 0, 3, 2);

	uint32_t t0 = (uint32_t)total;
	uint32_t t1 = (uint32_t)((total) >> 32);

	__m128i m0, m1, m2, m3;
	const __m128i u8to32 = _mm_set_epi8(12, 13, 14, 15, 8, 9, 10, 11, 4, 5, 6, 7, 0, 1, 2, 3);
	__m128i tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6;

	m0 = _mm_shuffle_epi8(_mm_loadu_si128((__m128i*)(datablock + 00)), u8to32);
	m1 = _mm_shuffle_epi8(_mm_loadu_si128((__m128i*)(datablock + 16)), u8to32);
	m2 = _mm_shuffle_epi8(_mm_loadu_si128((__m128i*)(datablock + 32)), u8to32);
	m3 = _mm_shuffle_epi8(_mm_loadu_si128((__m128i*)(datablock + 48)), u8to32);

	row1 = _mm_set_epi32(h[3], h[2], h[1], h[0]);
	row2 = _mm_set_epi32(h[7], h[6], h[5], h[4]);
	row3 = _mm_set_epi32(0x03707344, 0x13198A2E, 0x85A308D3, 0x243F6A88);

	if (padding)
		row4 = _mm_set_epi32(0xEC4E6C89, 0x082EFA98, 0x299F31D0, 0xA4093822);
	else
		row4 = _mm_set_epi32(0xEC4E6C89 ^ t1, 0x082EFA98 ^ t1, 0x299F31D0 ^ t0, 0xA4093822 ^ t0);

#include "rounds.sse41.h"
		tmp0 = _mm_load_si128((__m128i*)h);
	tmp0 = _mm_xor_si128(tmp0, _mm_xor_si128(row1, row3));
	_mm_store_si128((__m128i*)h, tmp0);

	tmp0 = _mm_load_si128((__m128i*)&(h[4]));
	tmp0 = _mm_xor_si128(tmp0, _mm_xor_si128(row2, row4));
	_mm_store_si128((__m128i*)&(h[4]), tmp0);

	return 0;
}





