#include "Framework.h"
#include "Utils/Parallel.h"

#include "Application.h"

namespace Silent::Utils
{
    ParallelExecutor::ParallelExecutor()
    {
        constexpr int THREAD_COUNT_MIN = 2;

        // Reserve threads.
        int threadCount = std::max(GetCoreCount(), THREAD_COUNT_MIN);
        _threads.reserve(threadCount);

        // Create threads.
        for (int i = 0; i < threadCount; i++)
        {
            _threads.push_back(std::jthread(&ParallelExecutor::Worker, this));
        }

        _deinitialize = false;
    }

    ParallelExecutor::~ParallelExecutor()
    {
        // @lock Restrict shutdown flag access.
        {
            auto taskLock = std::lock_guard(_taskMutex);

            _deinitialize = true;
        }

        // Notify all threads they should stop.
        _taskCond.notify_all();
    }

    int ParallelExecutor::GetThreadCount() const
    {
        return (int)_threads.size();
    }

    int ParallelExecutor::GetPendingTaskCount()
    {
        // @lock Restrict task queue access.
        {
            auto taskLock = std::lock_guard(_taskMutex);

            return (int)_tasks.size();
        }
    }

    std::future<void> ParallelExecutor::AddTask(const ParallelTask& task)
    {
        return AddTasks(ParallelTasks{ task });
    }

    std::future<void> ParallelExecutor::AddTasks(const ParallelTasks& tasks)
    {
        const auto& options = g_App.GetOptions();

        // If parallelism is disabled, execute tasks sequentially.
        if (!options->EnableParallelism)
        {
            for (const auto& task : tasks)
            {
                if (task)
                {
                    task();
                }
            }

            return GenerateReadyFuture();
        }

        // @heapalloc Create counter and promise.
        auto counter = std::make_shared<std::atomic<int>>();
        auto promise = std::make_shared<std::promise<void>>();

        // Add group tasks.
        counter->store((int)tasks.size(), std::memory_order::release);
        for (const auto& task : tasks)
        {
            AddTask(task, counter, promise);
        }

        // Notify available threads to handle tasks.
        _taskCond.notify_all();

        // Return future to wait on task group completion if needed.
        return promise->get_future();
    }

    void ParallelExecutor::Worker()
    {
        while (true)
        {
            auto task = ParallelTask();

            // @lock Restrict task queue access.
            {
                auto taskLock = std::unique_lock(_taskMutex);
                _taskCond.wait(taskLock, [this]
                {
                    return _deinitialize || !_tasks.empty();
                });

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

            // Periodically check for interruption.
            std::this_thread::yield();
        }
    }

    void ParallelExecutor::AddTask(const ParallelTask& task, std::shared_ptr<std::atomic<int>> counter, std::shared_ptr<std::promise<void>> promise)
    {
        // @lock Restrict task queue access.
        {
            auto taskLock = std::lock_guard(_taskMutex);

            // Add task with promise and counter handling.
            _tasks.push([this, task, counter, promise]()
            {
                HandleTask(task, *counter, *promise);
            });
        }
    }

    void ParallelExecutor::HandleTask(const ParallelTask& task, std::atomic<int>& counter, std::promise<void>& promise)
    {
        // Execute task.
        if (task)
        {
            task();
        }

        // Check for task group completion.
        if (counter.fetch_sub(1, std::memory_order::acq_rel) == 1)
        {
            promise.set_value();
        }
    }

    int GetCoreCount()
    {
        return std::max<int>(std::jthread::hardware_concurrency(), 1);
    }

    std::future<void> GenerateReadyFuture()
    {
        auto promise = std::promise<void>();
        promise.set_value();
        return promise.get_future();
    }
}
