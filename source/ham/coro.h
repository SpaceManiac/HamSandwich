#ifndef HAM_CORO_H
#define HAM_CORO_H

#include <functional>
#include "jamultypes.h"

#ifdef USE_COROUTINES
#include "coro.inl"
#else  // USE_COROUTINES

namespace coro {

HAM_API inline void next_frame() {}
HAM_API inline void launch(std::function<void()> entry_point) { entry_point(); }

}  // namespace coro

#define TASK(TY) TY
#define AWAIT
#define CO_RETURN return

#endif  // USE_COROUTINES
#endif  // HAM_CORO_H
