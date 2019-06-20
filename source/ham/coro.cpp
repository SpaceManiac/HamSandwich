#ifdef USE_COROUTINES
#include "coro.h"
#include <functional>
#include <map>
#include <queue>
#include <set>
#include <stack>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif  // __EMSCRIPTEN__

namespace coro {

struct executor {
	// the task which should wake on the next frame
	std::stack<coroutine_handle<>> awake_next_frame;

	// stack of tasks which are waiting to be launched
	std::stack<std::function<task<void>()>> launching;

	// map from awaitee to everything it should wake when done
	std::map<coroutine_handle<>, std::set<coroutine_handle<>>> wake_on_done;

	bool frame() {
		//printf(" frame: %lu\n", awake_next_frame.size());

		std::queue<coroutine_handle<>> awake;

		while (!launching.empty()) {
			launching.top()();
			launching.pop();
		}

		// dump awake_next_frame into awake
		if (!awake_next_frame.empty()) {
			awake.push(awake_next_frame.top());
			awake_next_frame.pop();
		}

		// while anything is awake, attempt to resume it
		while (!awake.empty()) {
			coroutine_handle<> current = awake.front();
			//printf("  resume %p\n", current.address());
			awake.pop();

			current.resume();
			// if resuming it finished it, awake its dependents
			if (current.done()) {
				//printf("  done\n");
				auto it = wake_on_done.find(current);
				if (it != wake_on_done.end()) {
					for (coroutine_handle<> h : it->second) {
						//printf("    wakes %p\n", h.address());
						awake.push(h);
					}
					wake_on_done.erase(it);
				}
			} else {
				// if it isn't put to sleep, it's still awake
				bool asleep = false;
				for (const auto& pair : wake_on_done) {
					if (pair.second.find(current) != pair.second.end()) {
						asleep = true;
						break;
					}
				}
				if (!asleep) {
					printf("a task yielded without going to sleep\n");
					awake.push(current);
				}
			}
		}

		return !awake_next_frame.empty();
	}

	bool schedule(coroutine_handle<> awaiter, coroutine_handle<> awaitee) {
		//printf("schedule(): %p awaits on %p\n", awaiter.address(), awaitee.address());
		wake_on_done[awaitee].insert(awaiter);
		return true;  // return control to resume()r
	}

} g_executor;

struct flip_awaiter {
	bool await_ready() {
		//printf("  flip_awaiter::await_ready\n");
		return false;
	}

	void await_suspend(coroutine_handle<> h) {
		//printf("  flip_awaiter::await_suspend(%p)\n", h.address());
		g_executor.awake_next_frame.push(h);
	}

	void await_resume() {
		//printf("  flip_awaiter::await_resume\n");
	}
};

task<void> next_frame() {
	//printf("next_frame\n");
	co_await flip_awaiter{};
}

void launch(std::function<task<void>()> entry_point) {
	g_executor.launching.push(entry_point);
}

#ifdef __EMSCRIPTEN__

void em_main_loop() {
	if (!g_executor.frame()) {
		printf("calling emscripten_cancel_main_loop\n");
		emscripten_cancel_main_loop();
	}
}

int main() {
	emscripten_set_main_loop(em_main_loop, 0, 1);
	return 0;
}

#else  // __EMSCRIPTEN__

int main() {
	while (g_executor.frame()) {}
	return 0;
}

#endif  // __EMSCRIPTEN__

bool __schedule(coroutine_handle<> awaiter, coroutine_handle<> awaitee) {
	g_executor.schedule(awaiter, awaitee);
	return true;
}

}  // namespace coro

#endif // USE_COROUTINES
