C++11 标准引入了一个新的头文件 `<thread>`，其中包含一些与线程相关的类和函数。以下是 `<thread>` 头文件中一些重要的类和函数：

1. `std::thread`：

   `std::thread` 类表示一个线程对象。可以使用构造函数创建新的线程对象，并使用成员函数 `join()` 或 `detach()` 等待或分离线程。

   ```cpp
   class thread {
    public:
       thread() noexcept;
       template< class Function, class... Args >
       explicit thread( Function&& f, Args&&... args );    
       ~thread();
   
       // 停止当前线程直到这个线程结束
       void join();
   
       // 分离执行的线程，使其在后台运行
       void detach();
   
       // 得到线程标识符
       std::thread::id get_id() const noexcept;
   
       // 如果线程可用，则返回 true
       bool joinable() const noexcept;
   
       // 禁止拷贝操作
       thread(const thread&) = delete;
       thread& operator=(const thread&) = delete;
   };
   ```cpp

2. `std::this_thread`：

   `std::this_thread` 是一个命名空间，它提供了与当前线程相关的函数，如获取线程 ID，休眠线程等。

   - `get_id()`：获取当前线程 ID。
   - `sleep_for()`：让当前线程休眠一段时间。
   - `yield()`：暂停当前线程以让其他线程运行。
   - `hardware_concurrency()`：获取系统支持的并发线程数。

3. `std::mutex`：

   `std::mutex` 类是一个互斥量对象，用于控制对共享资源的访问。它提供了两个基本操作：加锁（lock）和解锁（unlock）。

   ```cpp
   class mutex {
    public:
       mutex() noexcept;
   
       // 禁止拷贝操作
       mutex(const mutex&) = delete;
       mutex& operator=(const mutex&) = delete;
   
       // 加锁和解锁操作
       void lock();
       bool try_lock();
       void unlock();
   };
   ```cpp

4. `std::condition_variable`：

   `std::condition_variable` 类是一个条件变量对象，用于在线程间等待特定条件的发生。当满足该条件时，通知等待的线程继续执行。

   ```cpp
   class condition_variable {
    public:
       condition_variable() noexcept;
   
       // 禁止拷贝操作
       condition_variable(const condition_variable&) = delete;
       condition_variable& operator=(const condition_variable&) = delete;
   
       // 等待和通知操作
       void wait(std::unique_lock<std::mutex>& lock);
       template<class Predicate>
       void wait(std::unique_lock<std::mutex>& lock, Predicate pred);
       void notify_one() noexcept;
       void notify_all() noexcept;
   };
   ```cpp

5. `std::lock_guard`：

   `std::lock_guard` 是一个模板类，用于简化互斥量的使用。它在构造函数中加锁，并在析构函数中解锁，因此可以确保在任何时候都不会忘记解锁互斥量。

   ```cpp
   template <typename Mutex>
   class lock_guard {
    public:
       explicit lock_guard(Mutex& mutex);
   
       // 禁止拷贝操作
       lock_guard(const lock_guard&) = delete;
       lock_guard& operator=(const lock_guard&) = delete;
   };
   ```cpp

以上是 `<thread>` 头文件中一些常用的类和函数，它们提供了一种方便的方法来进行线程编程。



```cpp


#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex mtx;
std::condition_variable cv;

void thread_func(int id, int& task)
{
    // 线程尝试获取互斥锁
    std::unique_lock<std::mutex> lock(mtx);
    std::cout << "Thread " << id << " started." << std::endl;

    while (task > 0) {
        // 等待条件变量通知
        cv.wait(lock, [&task](){ return task > 0; });

        // 完成任务
        if (task <= 0) {
            break;
        }
        task--;
        std::cout << "Thread " << id << " finished one task. Remaining: " << task << std::endl;

        // 唤醒其他线程
        cv.notify_all();
    }

    std::cout << "Thread " << id << " finished." << std::endl;
}

int main()
{
    const int NUM_THREADS = 5;
    int task = 20;
    std::thread threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        threads[i] = std::thread(thread_func, i, std::ref(task));
    }

    while (task > 0) {
        // 睡眠一段时间模拟单位时间
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        // 主线程唤醒等待的线程
        std::cout << "Main thread is waking up waiting threads." << std::endl;
        cv.notify_all();
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        threads[i].join();
    }

    return 0;
}

```


