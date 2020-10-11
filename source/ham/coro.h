#ifndef HAM_CORO_H
#define HAM_CORO_H

#include <functional>

#ifdef USE_COROUTINES
#include "coro_active.inc.h"
#else  // USE_COROUTINES

namespace coro {

inline void next_frame() {}
inline void launch(std::function<void()> entry_point) { entry_point(); }
inline int main() { return 0; }

};  // namespace coro

#define TASK(TY) TY
#define AWAIT
#define CO_RETURN return

#endif  // USE_COROUTINES
#endif  // HAM_CORO_H
