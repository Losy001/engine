#pragma once

#include <core/core.h>
#include <core/log.h>

#include <utils/free_list.h>
#include <utils/macros.h>

static constexpr uint16_t MAX_WINDOWS = 16;

typedef enum WindowId : uint16_t WindowId;

typedef void (*WindowResizeCb)(uint16_t, uint16_t);
typedef void (*WindowPaintCb)();

static inline WindowId create_window(uint16_t width, uint16_t height, const char* title);

static inline void destroy_window(WindowId window);

static inline void poll_events(WindowId window);

static inline bool is_open(WindowId window);

static inline void* get_native_handle(WindowId window);

static inline u16vec2 get_size(WindowId window);

static inline i16vec2 get_mouse(WindowId window);

static inline i16vec2 get_mouse_delta(WindowId window);

static inline bool is_active(WindowId window);

static inline void set_resize_cb(WindowId window, WindowResizeCb cb);

static inline void set_paint_cb(WindowId window, WindowPaintCb cb);

// TODO: this will not persist
static inline bool is_key_down(WindowId window, uint16_t k);