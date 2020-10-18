// The meat of the coroutine header. In a separate file so that changes to it
// don't cause recompiles on platforms where it isn't in use.

#ifndef __cpp_coroutines
#error "Asked for coroutines, but there's not compiler support."
#endif

#include <stdio.h>
#include <experimental/coroutine>

namespace coro {

using namespace std::experimental::coroutines_v1;

bool __schedule(coroutine_handle<> awaiter, coroutine_handle<> awaitee);

template<typename T>
struct shared_state {
	bool filled = false;
	T value;
};

template<>
struct shared_state<void> {
	bool filled = false;
};

template<typename Result>
class task_base {
public:
	coroutine_handle<> handle;
	std::shared_ptr<shared_state<Result>> state;

	void await_resume_checks() {
		if (!state) {
			printf("task_base::await_resume_checks: state ptr is null\n");
			//exit(1);
		}
		if (!state->filled) {
			printf("task_base::await_resume_checks: state is not filled\n");
			//exit(1);
		}
	}

public:
	task_base(
		coroutine_handle<> handle,
		std::shared_ptr<shared_state<Result>> state
	) : handle(handle), state(state) {}

	task_base(task_base&&) = default;
	task_base(const task_base&) = delete;
	task_base& operator=(task_base&&) = default;
	task_base& operator=(const task_base &) = delete;

	bool await_ready() {
		//printf("  task(%p)::await_ready()\n", handle.address());
		// return false to suspend or true to continue
		return state->filled;
	}

	bool await_suspend(coroutine_handle<> h) {
		//printf("  task(%p)::await_suspend(%p)\n", handle.address(), h.address());
		//return g_executor.schedule(h, handle);
		return __schedule(h, handle);
	}
};

template<typename Result>
struct task : public task_base<Result> {
	task(coroutine_handle<> handle, std::shared_ptr<shared_state<Result>> state)
		: task_base<Result>(handle, state) {}

	Result await_resume() {
		//printf("  task<T>(%p)::await_resume()\n", this);
		this->await_resume_checks();

		Result v = std::move(this->state->value);
		this->state.reset();
		return v;
	}
};

template<>
struct task<void> : public task_base<void> {
	task(coroutine_handle<> handle, std::shared_ptr<shared_state<void>> state)
		: task_base<void>(handle, state) {}

	void await_resume() {
		//printf("  task<void>(%p)::await_resume\n", this);
		await_resume_checks();
	}
};

template<typename Result>
class promise_base {
protected:
	std::shared_ptr<shared_state<Result>> state;
public:
	promise_base() : state(std::make_shared<shared_state<Result>>()) {}

	suspend_never initial_suspend() {
		//printf("  promise_base::initial_suspend()\n");
		return {};
	}

	suspend_always final_suspend() noexcept {
		//printf("  promise_base::final_suspend()\n");
		return {};
	}

	void unhandled_exception() {
		//printf("  promise_base::unhandled_exception()\n");
	}

	task<Result> get_return_object() {
		//printf("  promise<T>::get_return_object\n");
		return {
			coroutine_handle<promise_base<Result>>::from_promise(*this),
			this->state
		};
	}
};

template<typename Result>
struct promise : public promise_base<Result> {
	void return_value(Result value) {
		//printf("  promise<T>::return_value\n");
		this->state->filled = true;
		this->state->value = value;
	}
};

template<>
struct promise<void> : public promise_base<void> {
	void return_void() {
		//printf("  promise<void>::return_void\n");
		this->state->filled = true;
	}
};

task<void> next_frame();
void launch(std::function<task<void>()> entry_point);
task<int> main(int argc, char** argv);

}  // namespace coro

template<typename Result, typename ...Arg>
struct std::experimental::coroutine_traits<coro::task<Result>, Arg...> {
	typedef coro::promise<Result> promise_type;
};

#define TASK(TY) __attribute__((warn_unused_result)) ::coro::task<TY>
#define AWAIT co_await
#define CO_RETURN co_return

#define main coro::main
