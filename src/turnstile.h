#ifndef SRC_TURNSTILE_H_
#define SRC_TURNSTILE_H_

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <vector>

struct Locker {
  std::mutex mux;
  std::condition_variable cv;
  std::atomic<bool> locked;
  std::atomic<int> cntr;

  Locker() : locked(true), cntr(0) {}
};

class Menager {
 private:
  size_t batch;
  std::mutex mux;
  std::queue<Locker*> pool;

 public:
  Menager();
  ~Menager();
  Locker* getTurnstile();
  void returnTurnstile(Locker*);
};

class Mutex {
 private:
  static Menager menago;
  static Locker* dummy;
  static std::vector<std::mutex> dataRace;
  static u_char dataRaceMax;

  Locker* lckPtr;

 public:
  Mutex();
  Mutex(const Mutex&) = delete;

  void lock();
  void unlock();
};

#endif  // SRC_TURNSTILE_H_