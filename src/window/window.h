#pragma once

#include <core/core.h>
#include <core/log.h>

#include <utils/free_list.h>
#include <utils/macros.h>

static constexpr uint16_t MAX_WINDOWS = 16;

typedef enum WindowId : uint16_t WindowId;

typedef void (*WindowResizeCb)(WindowId);

[[clang::overloadable]]
static inline WindowId create_window(uint16_t width, uint16_t height, const char* title);

[[clang::overloadable]]
static inline void destroy_window(WindowId window);

[[clang::overloadable]]
static inline void poll_events(WindowId window);

[[clang::overloadable]]
static inline bool is_open(WindowId window);

[[clang::overloadable]]
static inline void* get_native_handle(WindowId window);

[[clang::overloadable]]
static inline u16vec2 get_size(WindowId window);

[[clang::overloadable]]
static inline i16vec2 get_mouse(WindowId window);

[[clang::overloadable]]
static inline i16vec2 get_mouse_delta(WindowId window);

[[clang::overloadable]]
static inline bool is_active(WindowId window);

[[clang::overloadable]]
static inline void set_on_resize(WindowId window, WindowResizeCb cb);

// TODO: this will not persist
[[clang::overloadable]]
static inline bool is_key_down(WindowId window, uint16_t k);