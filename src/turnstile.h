#ifndef SRC_TURNSTILE_H_
#define SRC_TURNSTILE_H_

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Mutex {
 private:
  static std::unordered_set<std::shared_ptr<std::mutex>> pool;
  static std::mutex poolLock;

  static std::unordered_map<Mutex*, std::shared_ptr<std::mutex>> mutexLocker;
  static std::vector<std::mutex> dataRace;
  static u_char currId;

  std::atomic_uint activeThreads;
  u_char id;

  std::shared_ptr<std::mutex> getTurnstile();
  void giveBackTurnstile(std::shared_ptr<std::mutex>);

 public:
  Mutex();
  Mutex(const Mutex&) = delete;
  void lock();    // NOLINT
  void unlock();  // NOLINT
};

#endif  // SRC_TURNSTILE_H_
