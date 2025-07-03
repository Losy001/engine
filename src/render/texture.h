#pragma once

#include <core/core.h>

static constexpr uint16_t MAX_TEXTURES = 128;

typedef enum : uint8_t
{
	TEXTURE_FORMAT_RGBA
} TextureFormat;

typedef enum TextureId : uint16_t TextureId;

static inline TextureId create_texture(uint16_t width, uint16_t height, uint32_t* pixels, TextureFormat format);

static inline void destroy_texture(TextureId texture);

static inline void use(TextureId texture);

static inline void update(TextureId texture, uint16_t width, uint16_t height, uint32_t* pixels, TextureFormat format);