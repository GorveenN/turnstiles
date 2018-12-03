#ifndef SRC_TURNSTILE_H_
#define SRC_TURNSTILE_H_

#include <type_traits>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <set>
#include <queue>

class Mutex {
private:
    static std::queue<std::mutex*> turnstile;
    std::atomic_uint activeThreads;
    std::condition_variable barrier;
    std::mutex* myTurnstile;

    std::mutex* takeTurnstile();
    void giveBackTurnstile(std::mutex*);

public:

    Mutex();
    Mutex(const Mutex&) = delete;
    void lock();    // NOLINT
    void unlock();  // NOLINT
};

#endif  // SRC_TURNSTILE_H_
