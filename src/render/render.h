#pragma once

#include <core/core.h>

#include <utils/free_list.h>
#include <utils/macros.h>

#include <window/window.h>

static constexpr uint16_t MAX_RENDER_CONTEXTS = 4;

typedef uint16_t RenderContextId;

static inline RenderContextId create_context(WindowId w);

static inline void begin(RenderContextId r);

static inline void end(RenderContextId r);