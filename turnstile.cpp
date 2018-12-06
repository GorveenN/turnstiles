#include "turnstile.h"
#include <memory>

Mutex::Mutex() : activeThreads(0) {}
std::atomic_uint64_t Mutex::turnstileTaken = 0;
std::set<std::shared_ptr<std::mutex>> Mutex::turnstile;
std::unordered_map<Mutex*, std::shared_ptr<std::mutex>> Mutex::mutexLocker;
std::mutex Mutex::dataRace;

void Mutex::lock() {
    std::unique_lock<std::mutex> lk (dataRace);
    activeThreads++;

    if (activeThreads > 1) {
        if (activeThreads == 2) {
            turnstileTaken++;
            mutexLocker[this] = getTurnstile();
            mutexLocker[this]->lock();
        }
        lk.unlock();
        mutexLocker[this]->lock();
    }
}

void Mutex::unlock() {
    std::unique_lock<std::mutex> lk(dataRace);
    activeThreads--;

    if (activeThreads == 0) {
        turnstileTaken--;
        mutexLocker.erase(this);
    }
    if (activeThreads > 0) {
        mutexLocker[this]->unlock();
    }
}

std::shared_ptr<std::mutex> Mutex::getTurnstile() {
    if (turnstile.empty()) {
        for (int i = 0; i < turnstileTaken; ++i) {
            turnstile.insert(std::shared_ptr<std::mutex>(new std::mutex));
        }
    }
    auto m = *turnstile.begin();
    turnstile.erase(m);
    return m;
}

void Mutex::giveBackTurnstile(std::shared_ptr<std::mutex> m) {
    turnstile.insert(m);
    if (activeThreads <= turnstile.size() / 4) {
        auto iter = turnstile.begin();
        for (int i = 0; i < turnstile.size() * 0.75; ++i) {
            iter = turnstile.erase(iter);
        }
    }
}
