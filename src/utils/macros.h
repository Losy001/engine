#pragma once

#define count_of(array) sizeof(array) / sizeof(array[0])

#define do_once(...) do { static bool once = false; if (!once) { __VA_ARGS__ once = true; } } while (0)