#include <chrono>
#include <coroutine>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

struct RepeatAwaiter // awaiter(原始指针) / awaitable(operator->)
{
  bool await_ready() const noexcept { return false; }

  std::coroutine_handle<>
  await_suspend(std::coroutine_handle<> coroutine) const noexcept {
    if (coroutine.done())
      return std::noop_coroutine();
    else
      return coroutine;
  }

  void await_resume() const noexcept {}
};

struct RepeatAwaitable // awaitable(operator->)
{
  RepeatAwaiter operator co_await() { return RepeatAwaiter(); }
};

struct PreviousAwaiter {
  std::coroutine_handle<> mPrevious;

  bool await_ready() const noexcept { return false; }

  std::coroutine_handle<>
  await_suspend(std::coroutine_handle<> coroutine) const noexcept {
    if (mPrevious)
      return mPrevious;
    else
      return std::noop_coroutine();
  }

  void await_resume() const noexcept {}
};

struct Promise {
  auto initial_suspend() { return std::suspend_always(); }

  auto final_suspend() noexcept { return PreviousAwaiter(mPrevious); }

  void unhandled_exception() { throw; }

  auto yield_value(int ret) {
    mRetValue = ret;
    return std::suspend_always();
  }

  void return_void() { mRetValue = 0; }

  std::coroutine_handle<Promise> get_return_object() {
    return std::coroutine_handle<Promise>::from_promise(*this);
  }

  int mRetValue;
  std::coroutine_handle<> mPrevious = nullptr;
};

struct Task {
  using promise_type = Promise;

  Task(std::coroutine_handle<promise_type> coroutine) : mCoroutine(coroutine) {}

  Task(Task &&) = delete;

  ~Task() { mCoroutine.destroy(); }

  std::coroutine_handle<promise_type> mCoroutine;
};

struct WorldTask {
  using promise_type = Promise;

  WorldTask(std::coroutine_handle<promise_type> coroutine)
      : mCoroutine(coroutine) {}

  WorldTask(WorldTask &&) = delete;

  ~WorldTask() { mCoroutine.destroy(); }

  struct WorldAwaiter {
    bool await_ready() const noexcept { return false; }

    std::coroutine_handle<>
    await_suspend(std::coroutine_handle<> coroutine) const noexcept {
      mCoroutine.promise().mPrevious = coroutine;
      return mCoroutine;
    }

    void await_resume() const noexcept {}

    std::coroutine_handle<promise_type> mCoroutine;
  };

  auto operator co_await() { return WorldAwaiter(mCoroutine); }

  std::coroutine_handle<promise_type> mCoroutine;
};

WorldTask word() {
  SPDLOG_DEBUG("World");
  co_yield 422;
  co_yield 444;
  co_return;
}

Task hello() {
  SPDLOG_DEBUG("Hello 正在构建");
  WorldTask worldTask = word();
  SPDLOG_DEBUG("Hello 构建完成");
  co_await worldTask;
  SPDLOG_DEBUG("hello 得到word的结果: {}",
               worldTask.mCoroutine.promise().mRetValue);
  co_await worldTask;
  SPDLOG_DEBUG("hello 再次得到word的结果: {}",
               worldTask.mCoroutine.promise().mRetValue);
  SPDLOG_DEBUG("hello 42");
  co_yield 42;
  SPDLOG_DEBUG("hello 12");
  co_yield 12;
  SPDLOG_DEBUG("hello 6");
  co_yield 6;
  SPDLOG_DEBUG("hello 结束");
  co_return;
}

int main(int argc, char *argv[]) {
  auto logger = spdlog::stdout_color_mt("console");
  spdlog::set_default_logger(logger);
  spdlog::set_pattern("[%H:%M:%S] [%^%l%$] %v");
  spdlog::set_level(spdlog::level::debug);

  SPDLOG_DEBUG("Starting main");
  auto task = hello();
  while (!task.mCoroutine.done()) {
    task.mCoroutine.resume();
    SPDLOG_DEBUG("main得到hello的结果为{}",
                 task.mCoroutine.promise().mRetValue);
  }
  return 0;
}
