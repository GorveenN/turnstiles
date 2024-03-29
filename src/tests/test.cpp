#include "../turnstile.h"
#include <iostream>
#include <map>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

static_assert(std::is_destructible<Mutex>::value,
              "Mutex should be destriuctible.");
static_assert(!std::is_copy_constructible<Mutex>::value,
              "Mutex should not be copy-constructible.");
static_assert(!std::is_move_constructible<Mutex>::value,
              "Mutex should not be move-constructible.");
static_assert(std::is_default_constructible<Mutex>::value,
              "Mutex should be default constructible.");
static_assert(std::is_same<void, decltype(std::declval<Mutex>().lock())>::value,
              "Mutex should have a \"void lock()\" member function.");
static_assert(
    std::is_same<void, decltype(std::declval<Mutex>().unlock())>::value,
    "Mutex should have a \"void unlock()\" member function.");
static_assert(sizeof(Mutex) <= 8, "Mutex is too large");

void DummyTest() {
  int const kNumRounds = 100000;
  int const threads = 2000;
  int const mutexes = 1000;
  Mutex mu_tab[mutexes];
  int shared_cntr[mutexes];
  for (int i = 0; i < mutexes; ++i) shared_cntr[i] = 0;

  std::vector<std::thread> v;
  for (int i = 0; i < threads; ++i) {
    v.emplace_back([&]() {
      for (int i = 0; i < kNumRounds; ++i) {
          std::lock_guard<Mutex> lk(mu_tab[i % mutexes]);
          ++shared_cntr[i % mutexes];
      }
    });
  }

  for (auto &t : v) {
    t.join();
  }

  int sum = 0;
  for (int i = 0; i < mutexes; ++i) {
      sum += shared_cntr[i];
  }

  std::cout << "Your sum " << sum << std::endl;

  if (sum != kNumRounds * threads) {
    throw std::logic_error("Counter==" + std::to_string(sum) +
                           " expected==" + std::to_string(kNumRounds * threads));
  }
}

int main() {
  try {
    Mutex m;
    m.lock();
    m.unlock();
    m.lock();
    m.unlock();
    m.lock();
    m.unlock();
    DummyTest();
  } catch (std::exception &e) {
    std::cout << "Exception: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
