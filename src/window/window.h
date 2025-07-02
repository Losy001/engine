#pragma once

#include <core/core.h>

#include <utils/free_list.h>
#include <utils/macros.h>

static constexpr uint16_t MAX_WINDOWS = 16;

typedef uint16_t WindowId;

static inline WindowId create_window(uint16_t width, uint16_t height, const char* title);

static inline void destroy_window(WindowId window);

static inline void poll_events(WindowId window);

static inline bool is_open(WindowId window);

static inline void* get_native_handle(WindowId window);

static inline u16vec2 get_size(WindowId window);

static inline i16vec2 get_mouse(WindowId window);

static inline i16vec2 get_mouse_delta(WindowId window);

static inline bool is_key_down(uint16_t k);