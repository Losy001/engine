#pragma once

#include <core/core.h>
#include <core/math.h>

#include <utils/free_list.h>
#include <utils/macros.h>

#include <window/window.h>

#include <render/texture.h>
#include <render/mesh.h>
#include <render/camera.h>

static constexpr uint16_t MAX_RENDER_CONTEXTS = 4;

typedef enum RenderContextId : uint16_t RenderContextId;

static inline RenderContextId create_context(WindowId window);

static inline void destroy_context(RenderContextId render_context);

static inline void begin(RenderContextId render_context);

static inline void end(RenderContextId render_context);

static inline void begin_3d(RenderContextId render_context, Camera camera);

static inline void end_3d(RenderContextId render_context);

