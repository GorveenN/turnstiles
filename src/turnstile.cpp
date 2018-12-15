#include "turnstile.h"

std::vector<std::mutex> Mutex::dataRace(std::numeric_limits<u_char>::max());
Locker* Mutex::dummy = new Locker;
Menager Mutex::menago;
u_char Mutex::dataRaceMax = std::numeric_limits<u_char>::max();

Mutex::Mutex() : lckPtr(nullptr) {}

void Mutex::lock() {
  std::unique_lock<std::mutex> ul(dataRace[(size_t)this % dataRaceMax]);
  if (lckPtr == nullptr) {
    lckPtr = dummy;
  } else {
    if (lckPtr == dummy) {
      lckPtr = menago.getTurnstile();
    }
    lckPtr->cntr++;
    ul.unlock();
    std::unique_lock<std::mutex> turnstile(lckPtr->mux);
    lckPtr->cv.wait(turnstile, [&] { return !lckPtr->locked.exchange(true); });
  }
}

void Mutex::unlock() {
  std::unique_lock<std::mutex> ul(dataRace[(size_t)this % dataRaceMax]);
  if (lckPtr == dummy) {
    lckPtr = nullptr;
  } else if (lckPtr->cntr == 0) {
    menago.returnTurnstile(lckPtr);
    lckPtr = nullptr;
  } else {
    std::unique_lock<std::mutex> turn(lckPtr->mux);
    lckPtr->cntr--;
    lckPtr->locked = false;
    lckPtr->cv.notify_one();
  }
}

// Menager
Menager::Menager() : batch(128) {}

Menager::~Menager() {
  size_t size = pool.size();
  for (size_t i = 0; i < size; ++i) {
    auto ptr = pool.front();
    pool.pop();
    delete ptr;
  }
}

Locker* Menager::getTurnstile() {
  std::unique_lock<std::mutex> ul(mux);

  if (pool.empty()) {
    for (size_t i = 0; i < batch; ++i) {
      pool.push(new Locker);
    }
  }

  auto ptr = pool.front();
  pool.pop();
  return ptr;
}

void Menager::returnTurnstile(Locker* ptr) {
  std::unique_lock<std::mutex> ul(mux);
  pool.push(ptr);

  if (pool.size() + 1 >= 2 * batch) {
    for (size_t i = 0; i < batch; ++i) {
      auto rem = pool.front();
      pool.pop();
      delete rem;
    }
  }
}