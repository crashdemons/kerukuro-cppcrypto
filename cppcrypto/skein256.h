/*
This code is written by kerukuro for cppcrypto library (http://cppcrypto.sourceforge.net/)
and released into public domain.
*/

#ifndef CPPCRYPTO_SKEIN256_H
#define CPPCRYPTO_SKEIN256_H

#include "crypto_hash.h"
#include "alignedarray.h"
#include <functional>

namespace cppcrypto
{
	class skein256 : public crypto_hash
	{
	public:
		skein256(size_t hashsize);
		~skein256();

		void init() override;
		void update(const unsigned char* data, size_t len) override;
		void final(unsigned char* hash) override;

		size_t hashsize() const override { return hs; }
		size_t blocksize() const override { return 256; }
		skein256* clone() const override { return new skein256(hs); }
		void clear() override;

	protected:
		void transform(void* m, uint64_t num_blks, size_t reallen);

		std::function<void(void*, uint64_t, size_t)> transfunc;
		aligned_pod_array<uint64_t, 4, 32> h;
		uint64_t* H;
		unsigned char m[32];
		size_t pos;
		uint64_t total;
		uint64_t tweak[2];
		size_t hs;
	};
}

#endif

