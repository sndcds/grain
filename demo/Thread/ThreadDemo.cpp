#include "../../include/Grain.hpp"

#include <cstdio>
#include <chrono>
#include <thread>

using namespace Grain;


auto demoThreadPool() -> void {

    std::printf("\n--- ThreadPool Demo ---\n");

    ThreadPool pool(4);

    int32_t thread_count = 100;

    for (int32_t i = 0; i < thread_count; ++i) {

        const auto error = pool.enqueueTask(
            i,
            [i] {

                std::printf(
                    "Task %d running on thread %zu\n",
                    i,
                    std::hash<std::thread::id>{}(
                        std::this_thread::get_id()
                    )
                );

                std::this_thread::sleep_for(
                    std::chrono::milliseconds(100)
                );
            }
        );

        if (error != ThreadPoolError::None) {
            std::printf("Could not enqueue task %d\n", i);
        }
    }

    // Wait until all tasks have finished.

    const auto error = pool.waitForCompletion(thread_count);

    if (error == ThreadPoolError::None) {
        std::printf("All tasks finished.\n");
    }

    std::printf(
        "Completed: %zu\n",
        pool.completedCount()
    );

    std::printf(
        "Finished: %zu\n",
        pool.finishedCount()
    );

    // Check an individual task.

    ThreadPoolTaskStatus status;

    if (pool.taskStatus(5, status) == ThreadPoolError::None) {

        if (status == ThreadPoolTaskStatus::Completed) {
            std::printf("Task 5 completed successfully.\n");
        }
    }

    pool.stop(ThreadPoolStopMode::Graceful);
}


int main() {

    demoThreadPool();
}