#include "render.h"
#include "drawer.h"
#include "threadpool.h"

// x - высота
// y - ширина

#include "task.h"
void Render::render(const Hittable &world, ColorMatrix &color_matrix,
                    volatile bool &cancel_running, size_t thread_count,
                    std::function<void()> tile_callback) {
  _size = color_matrix.size();
  initialize();

  // Начало замера вермени
  // auto start = std::chrono::high_resolution_clock::now();

  // Инициализируем threadpool
  ThreadPool pool(thread_count);

  // Разбиваем матрицу на тайлы
  auto tile_creator = std::make_shared<TileCreator>(_size.x(), _size.y());
  auto tiles = tile_creator->create(64);

  auto render_task = std::make_shared<RenderTask>(*this);
  std::vector<std::future<void>> futures;

  // auto end = std::chrono::high_resolution_clock::now();
  // auto duration =
  //     std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  // std::cout << duration.count() << " milliseconds\n";

  // Добавляем задачи в тредпул
  for (auto tile : tiles) {
    auto fut = pool.add_task([&color_matrix, tile, &render_task, &world]() {
      render_task->execute(color_matrix, *tile, world);
    });
    futures.push_back(std::move(fut));
  }

  // end = std::chrono::high_resolution_clock::now();
  // duration =
  //     std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  // std::cout << duration.count() << " milliseconds\n";

  // Пробегаемся по задачам и вызываем callback в случае завершения
  size_t completed = 0;
  while (completed < tiles.size()) {
    for (size_t i = 0; i < futures.size(); ++i) {
      // Выходим из цикла, если в GUI-потоке изменился флаг
      if (cancel_running)
        break;
      if (futures[i].valid() && futures[i].wait_for(std::chrono::milliseconds(
                                    0)) == std::future_status::ready) {
        if (tile_callback)
          tile_callback();
        futures[i] = std::future<void>{};
        completed++;
      }
    }

    // Выходим из цикла, если в GUI-потоке изменился флаг
    if (cancel_running)
      break;
  }

  // end = std::chrono::high_resolution_clock::now();
  // duration =
  //     std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  // std::cout << duration.count() << " milliseconds\n";

  pool.stop();

  // end = std::chrono::high_resolution_clock::now();
  // duration =
  //     std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  // std::cout << duration.count() << " milliseconds\n";
}

void Render::render_seq(const Hittable &world, ColorMatrix &color_matrix) {
  _size = color_matrix.size();
  initialize();

  // Начало замера вермени
  // auto start = std::chrono::high_resolution_clock::now();

  auto render_task = std::make_shared<RenderTask>(*this);
  render_task->execute(color_matrix, Tile(0, 0, _size.x(), _size.y()), world);

  // auto end = std::chrono::high_resolution_clock::now();
  // auto duration =
  //     std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  // std::cout << duration.count() << " milliseconds\n";
}