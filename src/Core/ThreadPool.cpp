#include <grain/Core/ThreadPool.hpp>

#include <algorithm>
#include <exception>


namespace Grain {

//------------------------------------------------------------------------------
//  Construction and destruction
//------------------------------------------------------------------------------

ThreadPool::ThreadPool(size_t thread_count) {

    if (thread_count == 0) {
        thread_count = std::thread::hardware_concurrency();

        if (thread_count == 0) {
            thread_count = 1;
        }
    }

    try {

        m_workers.reserve(thread_count);

        for (size_t i = 0; i < thread_count; ++i) {
            m_workers.emplace_back(&ThreadPool::workerThread, this);
        }

    } catch (...) {

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_state = State::StoppingImmediate;
        }

        m_task_condition.notify_all();

        for (auto& worker : m_workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }

        m_state = State::Stopped;

        throw;
    }
}


ThreadPool::~ThreadPool() {

    stop(ThreadPoolStopMode::Graceful);
}


//------------------------------------------------------------------------------
//  Task submission
//------------------------------------------------------------------------------

auto ThreadPool::enqueueTask(
    int32_t task_id,
    std::function<void()> work
) noexcept -> ThreadPoolError {

    if (!work) {
        return ThreadPoolError::InvalidTask;
    }

    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_state != State::Running) {
            return ThreadPoolError::AlreadyStopped;
        }

        if (m_task_status.find(task_id) != m_task_status.end()) {
            return ThreadPoolError::TaskAlreadyExists;
        }

        try {

            m_task_status.emplace(
                task_id,
                ThreadPoolTaskStatus::Pending
            );

            try {

                m_task_queue.push(Task{
                    task_id,
                    std::move(work)
                });

            } catch (...) {

                m_task_status.erase(task_id);
                return ThreadPoolError::ResourceExhausted;
            }

        } catch (...) {

            return ThreadPoolError::ResourceExhausted;
        }
    }

    m_task_condition.notify_one();

    return ThreadPoolError::None;
}


//------------------------------------------------------------------------------
//  Task status
//------------------------------------------------------------------------------

auto ThreadPool::taskStatus(
    int32_t task_id,
    ThreadPoolTaskStatus& status
) const noexcept -> ThreadPoolError {

    std::lock_guard<std::mutex> lock(m_mutex);

    const auto iterator = m_task_status.find(task_id);

    if (iterator == m_task_status.end()) {
        return ThreadPoolError::TaskNotFound;
    }

    status = iterator->second;

    return ThreadPoolError::None;
}


//------------------------------------------------------------------------------
//  Worker thread
//------------------------------------------------------------------------------

void ThreadPool::workerThread() noexcept {

    while (true) {

        Task task;

        {
            std::unique_lock<std::mutex> lock(m_mutex);

            m_task_condition.wait(
                lock,
                [this] {
                    return !m_task_queue.empty() ||
                           m_state != State::Running;
                }
            );

            // Immediate shutdown: do not start another task.

            if (m_state == State::StoppingImmediate ||
                m_state == State::Stopped) {
                return;
            }

            // Graceful shutdown: finish the remaining queue first.

            if (m_task_queue.empty()) {

                if (m_state == State::StoppingGraceful) {
                    return;
                }

                continue;
            }

            task = std::move(m_task_queue.front());
            m_task_queue.pop();

            m_task_status[task.id] = ThreadPoolTaskStatus::Running;
        }


        // Execute outside the mutex.

        bool success = true;

        try {

            task.work();

        } catch (...) {

            success = false;
        }


        // Update task state and counters.

        {
            std::lock_guard<std::mutex> lock(m_mutex);

            if (success) {
                m_task_status[task.id] = ThreadPoolTaskStatus::Completed;
                ++m_completed_count;
            } else {
                m_task_status[task.id] = ThreadPoolTaskStatus::Failed;
            }

            ++m_finished_count;
        }

        m_completion_condition.notify_all();
    }
}

//------------------------------------------------------------------------------
//  Shutdown
//------------------------------------------------------------------------------

auto ThreadPool::stop(
    ThreadPoolStopMode mode
) noexcept -> ThreadPoolError {

    // Prevent multiple callers from joining the same threads.

    std::lock_guard<std::mutex> join_lock(m_join_mutex);

    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_state == State::Stopped) {
            return ThreadPoolError::None;
        }

        if (mode == ThreadPoolStopMode::Immediate) {

            m_state = State::StoppingImmediate;

            // Cancel all tasks that have not started.

            while (!m_task_queue.empty()) {

                const int32_t task_id = m_task_queue.front().id;
                m_task_queue.pop();

                m_task_status[task_id] = ThreadPoolTaskStatus::Cancelled;

                ++m_finished_count;
            }

        } else if (m_state == State::Running) {

            m_state = State::StoppingGraceful;
        }
    }

    m_task_condition.notify_all();
    m_completion_condition.notify_all();

    // Running tasks are allowed to finish.

    for (auto& worker : m_workers) {

        if (worker.joinable()) {
            worker.join();
        }
    }

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_state = State::Stopped;
    }

    m_completion_condition.notify_all();

    return ThreadPoolError::None;
}


//------------------------------------------------------------------------------
//  Completion
//------------------------------------------------------------------------------

auto ThreadPool::waitForCompletion(
    int32_t task_count
) noexcept -> ThreadPoolError {

    if (task_count < 0) {
        return ThreadPoolError::InvalidArgument;
    }

    if (task_count == 0) {
        return ThreadPoolError::None;
    }

    std::unique_lock<std::mutex> lock(m_mutex);

    m_completion_condition.wait(
        lock,
        [this, task_count] {
            return m_finished_count >= static_cast<size_t>(task_count) ||
                   m_state == State::Stopped;
        }
    );

    if (m_finished_count < static_cast<size_t>(task_count)) {
        return ThreadPoolError::AlreadyStopped;
    }

    return ThreadPoolError::None;
}


//------------------------------------------------------------------------------
//  Information
//------------------------------------------------------------------------------

auto ThreadPool::completedCount() const noexcept -> size_t {

    std::lock_guard<std::mutex> lock(m_mutex);

    return m_completed_count;
}


auto ThreadPool::finishedCount() const noexcept -> size_t {

    std::lock_guard<std::mutex> lock(m_mutex);

    return m_finished_count;
}


auto ThreadPool::threadCount() const noexcept -> size_t {

    return m_workers.size();
}


auto ThreadPool::isStopped() const noexcept -> bool {

    std::lock_guard<std::mutex> lock(m_mutex);

    return m_state == State::Stopped;
}


} // namespace Grain
