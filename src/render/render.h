#pragma once

#include <core/core.h>
#include <core/math.h>
#include <core/log.h>

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

static inline void clear_background(vec4 tint);

static inline void begin(RenderContextId render_context);

static inline void end();

static inline void begin_3d(Camera camera);

static inline void end_3d();

static inline void resize_viewport(RenderContextId render_context);

static inline RenderContextId get_current_context();
