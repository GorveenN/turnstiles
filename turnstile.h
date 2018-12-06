#ifndef SRC_TURNSTILE_H_
#define SRC_TURNSTILE_H_

#include <atomic>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <unordered_set>
#include <set>

class Mutex {
private:
    static std::set<std::shared_ptr<std::mutex>> turnstile;
    static std::unordered_map<Mutex*, std::shared_ptr<std::mutex>> mutexLocker;
    static std::mutex dataRace;
    static std::atomic_uint64_t turnstileTaken;


    std::atomic_uint activeThreads;

    std::shared_ptr<std::mutex> getTurnstile();
    void giveBackTurnstile(std::shared_ptr<std::mutex>);

public:

    Mutex();
    Mutex(const Mutex&) = delete;
    void lock();    // NOLINT
    void unlock();  // NOLINT
};

#endif  // SRC_TURNSTILE_H_
