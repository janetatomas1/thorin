#pragma once

// thorin's assertions: libassert's, or no-ops on Emscripten, where thorin is built without libassert.
#ifdef __EMSCRIPTEN__
#define THORIN_ASSERT(...) ((void)0)
#define THORIN_DEBUG_ASSERT(...) ((void)0)
#else
#include <libassert/assert.hpp>
#define THORIN_ASSERT(...) ASSERT(__VA_ARGS__)
#define THORIN_DEBUG_ASSERT(...) DEBUG_ASSERT(__VA_ARGS__)
#endif
