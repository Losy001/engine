
// this header file implements a lossless webp decoder
// some of the code is based on:
// - https://github.com/matanui159/jebp/blob/main/jebp.h

#pragma once

#include <core/core.h>
#include <core/log.h>

typedef struct 
{
	uint32_t id;
	uint32_t length;
	uint8_t* payload;
} RIFFChunk;

static inline RIFFChunk consume_header(const uint8_t* data, uint32_t* i)
{
	RIFFChunk c = {};

	c.id = *(uint32_t*)&data[*i];
	*i += sizeof(uint32_t);

	c.length = *(uint32_t*)&data[*i];
	*i += sizeof(uint32_t);

	c.payload = (uint8_t*)&data[*i];
	*i += sizeof(uint32_t);

	return c;
}

static inline RIFFChunk consume_riff(const uint8_t* data, uint32_t* i)
{
	RIFFChunk c = {};

	c.id = *(uint32_t*)&data[*i];
	*i += sizeof(uint32_t);

	c.length = *(uint32_t*)&data[*i];
	*i += sizeof(uint32_t);

	c.payload = (uint8_t*)&data[*i];
	*i += c.length + (c.length % 2);

	return c;
}

static inline uint32_t consume_bits(const uint8_t* data, uint32_t* i, uint32_t n)
{
	uint32_t bit_count = 0;
	uint32_t byte_count = n;

	uint32_t bits = 0;

	while (bit_count < n && byte_count > 0)
	{
		uint8_t v = *(uint8_t*)&data[*i];
		*i += sizeof(uint8_t);

		bits |= v << bit_count;
		bit_count += 8;
		byte_count -= 1;
	}

	bits = bits & (1 << n) - 1;
	bits >>= n; // doesn't work right now

	return bits;
}

static inline void load_webp(const uint8_t* data, uint32_t* out, uint16_t* out_width, uint16_t* out_height)
{
	uint32_t i = 0;

	// webp header

	RIFFChunk c = consume_header(data, &i);

	if (c.id != __builtin_bswap32('RIFF'))
	{
		LOG_FATAL("expected RIFF");
	}

	uint32_t file_size = c.length;

	if (*(uint32_t*)c.payload != __builtin_bswap32('WEBP'))
	{
		LOG_FATAL("expected WEBP");
	}
	
	// VP8L chunk

	c = consume_riff(data, &i);

	if (c.id != __builtin_bswap32('VP8L'))
	{
		LOG_FATAL("expected VP8L. ensure the WebP is lossless");
	}

	// width/height

	uint16_t image_width = consume_bits(data, &i, 14) + 1;
	uint16_t image_height = consume_bits(data, &i, 14) + 1;

	LOG_FATAL("%d, %d", image_width, image_height);
}