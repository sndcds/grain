#pragma once

#include "Grain.hpp"

#include <cstdint>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>
#include <unordered_map>
#include <vector>


namespace Grain {


enum class ThreadPoolError : uint8_t {
    None,
    NotInitialized,
    AlreadyStopped,
    InvalidArgument,
    InvalidTask,
    TaskNotFound,
    TaskAlreadyExists,
    ResourceExhausted
};


enum class ThreadPoolTaskStatus : uint8_t {
    Pending,
    Running,
    Completed,
    Failed,
    Cancelled
};


enum class ThreadPoolStopMode : uint8_t {
    Graceful,
    Immediate
};


class ThreadPool {

public:

    explicit ThreadPool(size_t thread_count);
    ~ThreadPool();

    ThreadPool(const ThreadPool&) = delete;
    auto operator = (const ThreadPool&) -> ThreadPool& = delete;

    ThreadPool(ThreadPool&&) = delete;
    auto operator = (ThreadPool&&) -> ThreadPool& = delete;


    // MARK: - Tasks

    auto enqueueTask(
        int32_t task_id,
        std::function<void()> work
    ) noexcept -> ThreadPoolError;


    auto taskStatus(
        int32_t task_id,
        ThreadPoolTaskStatus& status
    ) const noexcept -> ThreadPoolError;


    // MARK: - Lifecycle

    auto stop(
        ThreadPoolStopMode mode = ThreadPoolStopMode::Graceful
    ) noexcept -> ThreadPoolError;


    auto waitForCompletion(
        int32_t task_count
    ) noexcept -> ThreadPoolError;


    // MARK: - Information

    [[nodiscard]]
    auto completedCount() const noexcept -> size_t;

    [[nodiscard]]
    auto finishedCount() const noexcept -> size_t;

    [[nodiscard]]
    auto threadCount() const noexcept -> size_t;

    [[nodiscard]]
    auto isStopped() const noexcept -> bool;


private:

    struct Task {
        int32_t id = 0;
        std::function<void()> work;
    };


    enum class State : uint8_t {
        Running,
        StoppingGraceful,
        StoppingImmediate,
        Stopped
    };


    // MARK: - Worker

    void workerThread() noexcept;


    // MARK: - Internal state

    mutable std::mutex m_mutex;
    std::mutex m_join_mutex;

    std::condition_variable m_task_condition;
    std::condition_variable m_completion_condition;

    std::vector<std::thread> m_workers;
    std::queue<Task> m_task_queue;

    std::unordered_map<int32_t, ThreadPoolTaskStatus> m_task_status;

    State m_state = State::Running;

    size_t m_completed_count = 0;
    size_t m_finished_count = 0;

};


} // namespace Grain
