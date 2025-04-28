#include "Framework.h"
#include "Utils/Parallel.h"

namespace Silent::Utils
{
    ParallelTaskManager& g_Parallel = ParallelTaskManager::Get();

    ParallelTaskManager::ParallelTaskManager()
    {
        // Reserve threads.
        uint threadCount = GetCoreCount() * 2;
        _threads.reserve(threadCount);

        // Create threads.
        for (int i = 0; (uint)i < threadCount; i++)
        {
            _threads.push_back(std::thread(&ParallelTaskManager::Worker, this));
        }

        _deinitialize = false;
    }

    ParallelTaskManager::~ParallelTaskManager()
    {
        Log("Deinitializing threads...");

        // LOCK: Restrict shutdown flag access.
        {
            auto taskLock = std::lock_guard(_taskMutex);

            _deinitialize = true;
        }

        // Notify all threads they should stop.
        _taskCond.notify_all();

        // Join all threads.
        for (auto& thread : _threads)
        {
            try
            {
                thread.join();
            }
            catch (const std::exception& ex)
            {
                Log("Failed to join thread: " + std::string(ex.what()), LogLevel::Error);
            }
        }
    }

    ParallelTaskManager& ParallelTaskManager::Get()
    {
        static auto instance = ParallelTaskManager();
        return instance;
    }

    uint ParallelTaskManager::GetThreadCount() const
    {
        return (uint)_threads.size();
    }

    uint ParallelTaskManager::GetCoreCount() const
    {
        return std::max(std::thread::hardware_concurrency(), 1u);
    }

    std::future<void> ParallelTaskManager::AddTask(const ParallelTask& task)
    {
        return AddTasks(ParallelTasks{ task });
    }

    std::future<void> ParallelTaskManager::AddTasks(const ParallelTasks& tasks)
    {
        // HEAP ALLOC: Create counter and promise.
        auto counter = std::make_shared<std::atomic<int>>();
        auto promise = std::make_shared<std::promise<void>>();

        counter->store((int)tasks.size(), std::memory_order_release);

        // Add group tasks.
        for (const auto& task : tasks)
        {
            AddTask(task, counter, promise);
        }

        // Notify available threads to handle tasks.
        _taskCond.notify_all();

        // Return future to wait on task group completion if needed.
        return promise->get_future();
    }

    void ParallelTaskManager::Worker()
    {
        while (true)
        {
            auto task = ParallelTask();

            // LOCK: Restrict task queue access.
            {
                auto taskLock = std::unique_lock(_taskMutex);
                _taskCond.wait(taskLock, [this] { return (_deinitialize || !_tasks.empty()); });

                // Shutting down and no pending tasks; return early.
                if (_deinitialize && _tasks.empty())
                {
                    return;
                }

                // Get task.
                task = _tasks.front();
                _tasks.pop();
            }

            // Execute task.
            if (task)
            {
                task();
            }
        }
    }

    void ParallelTaskManager::AddTask(const ParallelTask& task, std::shared_ptr<std::atomic<int>> counter, std::shared_ptr<std::promise<void>> promise)
    {
        // Increment counter for task group.
        counter->fetch_add(1, std::memory_order_relaxed);

        // LOCK: Restrict task queue access.
        {
            auto taskLock = std::unique_lock(_taskMutex);

            // Add task with promise and counter handling.
            _tasks.push([this, task, counter, promise]() { HandleTask(task, *counter, *promise); });
        }
    }

    void ParallelTaskManager::HandleTask(const ParallelTask& task, std::atomic<int>& counter, std::promise<void>& promise)
    {
        // Execute task.
        if (task)
        {
            task();
        }

        // Check for task group completion.
        if (counter.fetch_sub(1, std::memory_order_acq_rel) == 1)
        {
            promise.set_value();
        }
    }
}
