#include "render.h"
#include "threadpool.h"
#include "hittable_list.h"
#include "tile_creator.h"

void Render::render(const hittable &world, ColorMatrix &color_matrix,
                    volatile bool &cancel_running, size_t thread_count,
                    std::function<void()> tile_callback)
{
    // Size size = color_matrix.size();
    _size = color_matrix.size();
    initialize();
    // initialize(size.x(), size.y());
    // initialize(size.y(), size.x());

    // std::cout << "test " << image_height << " " << image_width << std::endl;


    ThreadPool pool(thread_count);

    auto tile_creator = std::make_shared<TileCreator>(_size.x(), _size.y());
    auto tiles = tile_creator->create(64);

    auto render_task = std::make_shared<RenderTask>(*this);
    std::vector<std::future<void>> futures;


    for (auto tile : tiles)
    {
        auto fut = pool.add_task([&color_matrix, tile, &render_task, &world]()
                                 { render_task->execute(color_matrix, *tile, world); });
        futures.push_back(std::move(fut));
    }

    size_t completed = 0;
    while (completed < tiles.size())
    {
        for (size_t i = 0; i < futures.size(); ++i)
        {
            // Выходим из цикла, если в GUI-потоке изменился флаг
            if (cancel_running)
                break;
            if (futures[i].valid() && futures[i].wait_for(std::chrono::milliseconds(
                                          0)) == std::future_status::ready)
            {
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

    pool.stop();
}