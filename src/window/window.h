#pragma once

#include <core/core.h>

#include <utils/free_list.h>
#include <utils/macros.h>

static constexpr uint16_t MAX_WINDOWS = 16;

typedef uint16_t WindowId;

static inline WindowId create_window(uint16_t width, uint16_t height, const char* title);

static inline void destroy_window(WindowId w);

static inline void poll_events(WindowId w);

static inline bool is_open(WindowId w);

static inline void* get_native_handle(WindowId w);

static inline uint16_t get_width(WindowId w);

static inline uint16_t get_height(WindowId w);

static inline int16_t get_mouse_x(WindowId w);

static inline int16_t get_mouse_y(WindowId w);