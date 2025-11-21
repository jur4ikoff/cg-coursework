#ifndef THREADPOOL_HPP__
#define THREADPOOL_HPP__

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
public:
  explicit ThreadPool(size_t workers_count) : _stop_required(false) {
    if (workers_count == 0)
      throw std::runtime_error("Workers should be > 0!");

    _workers.reserve(workers_count);

    for (size_t i = 0; i < workers_count; i++)
      _workers.emplace_back([this] { worker(); });
  }

  // Если параметров нет, то вызываем конструктор с функцией, определяющей
  // количества ядер
  ThreadPool() : ThreadPool(std::thread::hardware_concurrency()) {}

  ~ThreadPool() noexcept { stop(); }

  template <typename Func, typename... Args>
  std::future<std::invoke_result_t<Func, Args...>> add_task(Func &&func,
                                                            Args &&...args) {
    using ret = std::invoke_result_t<Func, Args...>;

    // Упаковываем задачу
    std::packaged_task<ret()> task(
        // Сохраняем универсальные ссылки
        [func = std::forward<Func>(func),
         args_tuple = std::make_tuple(std::forward<Args>(args)...)] {
          // std::apply вызывает lambda функцию с параметрами
          return std::apply(
              [&](auto &&...captured_args) -> ret {
                // std::invoke запускает функцию с параметрами
                return std::invoke(func, std::forward<decltype(captured_args)>(
                                             captured_args)...);
              },
              std::move(args_tuple));
        });

    std::future<ret> fut = task.get_future();

    {
      std::lock_guard l(_mutex_queue);
      if (_stop_required)
        throw std::runtime_error("ThreadPool is stopping or stopped");

      // mutable позваляет использовать task, как неконстантую
      _task_queue.emplace([task = std::move(task)]() mutable { task(); });
    }

    _pool_notifier.notify_one();

    return fut;
  }

  void stop() {
    {
      std::lock_guard l(_mutex_queue);
      if (_stop_required)
        return;

      _stop_required = true;
    }

    _pool_notifier.notify_all();

    for (auto &w : _workers)
      if (w.joinable())
        w.join();
  }

  size_t Size() const noexcept { return _workers.size(); }

private:
  void worker() {
    while (true) {
      std::packaged_task<void()> task;
      {
        std::unique_lock l(_mutex_queue);
        // Пока предикат возвращает false, поток освобождает мьютекс
        // (разблокирует его) и засыпает, ожидая уведомления.
        _pool_notifier.wait(
            l, [this]() { return !_task_queue.empty() || _stop_required; });

        if (_task_queue.empty() && _stop_required)
          return;

        task = std::move(_task_queue.front());
        _task_queue.pop();
      }

      task();
    }
  }

  std::condition_variable _pool_notifier;

  mutable std::mutex _mutex_queue;
  std::queue<std::packaged_task<void()>> _task_queue;

  std::vector<std::thread> _workers;

  bool _stop_required;
};

#endif // THREADPOOL_HPP__