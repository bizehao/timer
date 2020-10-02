#pragma once

#include <atomic>
#include <cassert>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <ctime>
#include <functional>
#include <iostream>
#include <limits>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

namespace bzh {

    using namespace std::chrono_literals;

//智能指针形式 std::vector 无内存泄露
    class TimerTask {
    public:
        explicit TimerTask(std::function<void()> function) : taskFun(std::move(function)){};

        ~TimerTask() = default;

        /**
         * 取消任务
         */
        bool cancel() {
            std::unique_lock<std::mutex> lock(mtx); //互斥锁
            bool result = (state == State::SCHEDULED);
            state = State::CANCELLED;
            return result;
        }

        /**
         * 获取这个任务计划执行的时间
         */
        long scheduledExecutionTime() {
            std::unique_lock<std::mutex> lock(mtx); //互斥锁
            return period < 0 ? nextExecutionTime + period : nextExecutionTime - period;
        }

    private:
        friend class Timer;

        enum class State {
            VIRGIN = 0,    //这个任务尚未安排
            SCHEDULED = 1, //此任务计划执行。如果它是非重复任务，它尚未执行
            EXECUTED = 2,  //此非重复任务已执行（或当前正在执行）且尚未取消
            CANCELLED = 3  //此任务已取消
        };

        State state = State::VIRGIN;     //任务的状态
        long long nextExecutionTime = 0; //下一次任务的执行时间
        std::mutex mtx;
        long long period = 0;          //重复任务的时间段（以毫秒为单位）
        std::function<void()> taskFun; //要执行得任务实现
    };

    class Timer final {
    public:
        static Timer& getInstanse() {
            static Timer timer;
            return timer;
        }

        ~Timer() {
            std::unique_lock<std::mutex> lock(mainMtx);
            isWaitChildThread = true;
            mainCv.wait(lock);
            mainCv.wait(lock, [this]() { return taskQueue.isEmpty(); });
        };

        template <typename _Rep_Delay, typename _Period_Delay, typename _Rep_Period = long long, typename _Period_Period = std::milli,
                typename Task>
        void schedule(Task&& task, const std::chrono::duration<_Rep_Delay, _Period_Delay>& delay,
                      const std::chrono::duration<_Rep_Period, _Period_Period>& period = std::chrono::milliseconds{0}) {

            static_assert(std::disjunction_v<std::is_invocable<std::decay_t<Task>>, std::is_same<std::decay_t<Task>, TimerTask*>>,
                          "参数错误");

            std::chrono::milliseconds delay_ms = std::chrono::duration_cast<std::chrono::milliseconds>(delay);
            std::chrono::milliseconds period_ms = std::chrono::duration_cast<std::chrono::milliseconds>(period);
            if (delay_ms.count() < 0) {
                std::cerr << "Negative delay." << std::endl;
                return;
            }

            if (period_ms.count() < 0) {
                std::cerr << "Non-positive period." << std::endl;
                return;
            }

            if constexpr (std::is_same_v<std::decay_t<Task>, TimerTask*>) {
                sched(std::shared_ptr<TimerTask>(task), currentTimeMillis() + delay_ms.count(), -period_ms.count());
            } else {
                sched(std::make_shared<TimerTask>(task), currentTimeMillis() + delay_ms.count(), -period_ms.count());
            }
        }

        void cancel() {
            std::unique_lock<std::mutex> queueLock(mtx);
            newTasksMayBeScheduled = false;
            taskQueue.clear();
            cv.notify_all();
        }

        int purge() {
            int result = 0;

            std::unique_lock<std::mutex> queueLock(mtx);
            for (int i = taskQueue.getSize(); i > 0; i--) {
                if (taskQueue.get(i)->state == TimerTask::State::CANCELLED) {
                    taskQueue.quickRemove(i);
                    result++;
                }
            }
            if (result != 0)
                taskQueue.heapify();

            return result;
        }

        //指定时间
        static long long getTime(int year, int month, int day, int hour = 0, int minute = 0, int second = 0) {
            std::tm time{};
            time.tm_year = year - 1900;
            time.tm_mon = month - 1;
            time.tm_mday = day;
            time.tm_hour = hour;
            time.tm_min = minute;
            time.tm_sec = second;
            time.tm_isdst = -1;
            std::time_t tt = std::mktime(&time);

            return tt * 1000 - currentTimeMillis();
        }

    private:
        Timer() {

            std::thread thread([&] { mainLoop(); });
            thread.detach();
        };

        Timer(const Timer&) = delete;

        Timer& operator=(const Timer&) = delete;

        class TaskQueue {
        public:
            TaskQueue() = default;

            int getSize() { return queue.size(); }

            void add(std::shared_ptr<TimerTask> timerTask) {
                queue.push_back(timerTask);
                fixUp();
            }

            std::shared_ptr<TimerTask> getMin() { return queue[0]; }

            std::shared_ptr<TimerTask> get(int i) { return queue[i]; }

            void removeMin() {
                quickRemove(0);
                fixDown(0);
            }

            /**
             * 快速删除
             */
            void quickRemove(int i) {
                if (queue.size() != 1) {
                    std::swap(queue[i], queue[queue.size() - 1]);
                }
                queue.erase(std::cend(queue) - 1);
            }

            /**
             * 重新排列
             */
            void rescheduleMin(long long newTime) {
                queue[0]->nextExecutionTime = newTime;
                fixDown(0);
            }

            bool isEmpty() { return queue.empty(); }

            void clear() { queue.clear(); }

            void heapify() {
                for (auto i = queue.size() / 2; i >= 1; i--)
                    fixDown(i);
            }

        private:
            std::vector<std::shared_ptr<TimerTask>> queue; //任务容器

            //把时间最小的放在第一位
            void fixUp() {
                std::size_t k = queue.size() - 1;
                while (k > 0) {
                    std::size_t j = k / 2;
                    if (queue[j]->nextExecutionTime <= queue[k]->nextExecutionTime)
                        break;
                    std::swap(queue[j], queue[k]);
                    k = j;
                }
            }

            //把时间最小的放在第一位
            void fixDown(int k) {
                if (queue.empty() || queue.size() == 1) {
                    return;
                }
                int j = 0;
                while ((j = k * 2) <= queue.size() - 1) {
                    if (j <= static_cast<int>(queue.size() - 2) && queue[j]->nextExecutionTime > queue[j + 1]->nextExecutionTime) {
                        j++; // j indexes smallest kid
                    }
                    if (queue[k]->nextExecutionTime <= queue[j]->nextExecutionTime) {
                        break;
                    }
                    std::swap(queue[j], queue[k]);
                    k = j;
                }
            }
        };

        void mainLoop() {
            while (true) {

                std::unique_lock<std::mutex> mainLock(mainMtx);
                std::shared_ptr<TimerTask> task;
                bool taskFired; //任务被解除 false等待 true执行任务
                std::unique_lock<std::mutex> queueLock(mtx);

                if (!newTasksMayBeScheduled) { //取消任务
                    mainCv.notify_all();
                    break;
                }

                if (taskQueue.isEmpty() && isWaitChildThread) {

                    mainCv.notify_all();
                    break;
                }
                if (taskQueue.isEmpty()) {
                    mainCv.notify_all();
                    continue;
                }
                long long currentTime = 0;
                long long executionTime = 0;
                task = taskQueue.getMin();
                std::unique_lock<std::mutex> taskLock(task->mtx);
                if (task->state == TimerTask::State::CANCELLED) {
                    taskQueue.removeMin();
                    continue; // No action required, poll queue again
                }
                currentTime = currentTimeMillis();
                executionTime = task->nextExecutionTime;
                taskFired = (executionTime <= currentTime);
                if (taskFired) {
                    if (task->period == 0) { // Non-repeating, remove 不是循环任务 移除掉
                        taskQueue.removeMin();
                        task->state = TimerTask::State::EXECUTED;
                    } else { // Repeating task, reschedule
                        taskQueue.rescheduleMin(task->period < 0 ? (currentTime - task->period) : (executionTime + task->period));
                    }
                }
                taskLock.unlock();
                if (!taskFired) // Task hasn't yet fired; wait
                    cv.wait_for(queueLock, std::chrono::milliseconds(executionTime - currentTime));
                queueLock.unlock();
                if (taskFired) { // Task fired; run it, holding no locks
                    task->taskFun();
                }
                mainCv.notify_all();
            } //结束子线程
        }

        //获取当前系统时间
        static long long currentTimeMillis() {
            auto time = std::chrono::system_clock::now();
            return std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch()).count();
        }

        void sched(std::shared_ptr<TimerTask> task, long long time, long long period) {
            if (time < 0)
                std::cerr << "Illegal execution time." << std::endl;

            // Constrain value of period sufficiently to prevent numeric
            // overflow while still being effectively infinitely large.
            //绝对值
            if (std::llabs(period) > (std::numeric_limits<long long>::max() / 2))
                period /= 2;

            std::unique_lock<std::mutex> queueLock(mtx);
            if (!newTasksMayBeScheduled)
                std::cerr << "Timer already cancelled." << std::endl;

            std::unique_lock<std::mutex> taskLock(task->mtx);
            if (task->state != TimerTask::State::VIRGIN)
                std::cerr << "Task already scheduled or cancelled" << std::endl;
            task->nextExecutionTime = time;
            task->period = period;
            task->state = TimerTask::State::SCHEDULED;
            taskLock.unlock();
            taskQueue.add(task);
            if (taskQueue.getMin().get() == task.get())
                cv.notify_one();
        }

        TaskQueue taskQueue; //任务队列
        std::mutex mtx;
        std::mutex mainMtx;             //阻塞主线程
        std::condition_variable mainCv; //阻塞主线程
        std::condition_variable cv;
        bool newTasksMayBeScheduled = true;          //可以安排新任务
        std::atomic<bool> isWaitChildThread = false; // main线程是否需要等待子线程
    };
}; // namespace bzh
