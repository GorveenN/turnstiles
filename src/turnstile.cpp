#include "turnstile.h"
#include <iostream>
#include <limits>
#include <memory>

Mutex::Mutex() : activeThreads(0), id(currId++) {}
std::unordered_set<std::shared_ptr<std::mutex>> Mutex::pool;
std::unordered_map<Mutex*, std::shared_ptr<std::mutex>> Mutex::mutexLocker;
std::vector<std::mutex> Mutex::dataRace(std::numeric_limits<u_char>::max() + 1);
u_char Mutex::currId = 0;
std::mutex Mutex::poolLock;

void Mutex::lock() {
  std::unique_lock<std::mutex> lk(dataRace[id]);
  activeThreads++;

  if (activeThreads > 1) {
    if (mutexLocker.find(this) == mutexLocker.end()) {
      mutexLocker[this] = getTurnstile();
      mutexLocker[this]->lock();
    }
    lk.unlock();
    mutexLocker[this]->lock();
  }
}

void Mutex::unlock() {
  std::unique_lock<std::mutex> lk(dataRace[id]);
  activeThreads--;

  if (activeThreads == 0) {
    if (mutexLocker.find(this) != mutexLocker.end()) {
      mutexLocker[this]->unlock();
      giveBackTurnstile(mutexLocker[this]);
      mutexLocker.erase(this);
    }
  } else {
    mutexLocker[this]->unlock();
  }
}

std::shared_ptr<std::mutex> Mutex::getTurnstile() {
  std::unique_lock<std::mutex> lk(poolLock);
  if (pool.empty()) {
    for (size_t i = 0; i < 128; ++i) {
      pool.insert(std::make_shared<std::mutex>());
    }
  }
  auto m = *pool.begin();
  pool.erase(m);
  return m;
}

void Mutex::giveBackTurnstile(std::shared_ptr<std::mutex> m) {
  std::unique_lock<std::mutex> lk(poolLock);
  pool.insert(m);

  if (256 <= pool.size()) {
    auto iter = pool.begin();
    for (size_t i = 0; i < 128; ++i) {
      iter = pool.erase(iter);
    }
  }
}
