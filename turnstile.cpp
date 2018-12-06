#include "turnstile.h"
#include <memory>
#include <iostream>

Mutex::Mutex() : activeThreads(0) {}
std::atomic_uint64_t Mutex::turnstileTaken(0);
std::set<std::shared_ptr<std::mutex>> Mutex::turnstile = {std::shared_ptr<std::mutex>(new std::mutex)};
std::unordered_map<Mutex*, std::shared_ptr<std::mutex>> Mutex::mutexLocker;
std::mutex Mutex::dataRace;

void Mutex::lock() {
    std::unique_lock<std::mutex> lk (dataRace);
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

    std::unique_lock<std::mutex> lk(dataRace);
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
    if (turnstile.empty()) {
        for (size_t i = 0; i < turnstileTaken; ++i) {
            turnstile.insert(std::shared_ptr<std::mutex>(new std::mutex));
        }
    }
    turnstileTaken++;
    auto m = *turnstile.begin();
    turnstile.erase(m);
    return m;
}

void Mutex::giveBackTurnstile(std::shared_ptr<std::mutex> m) {
    turnstile.insert(m);
    turnstileTaken--;

    if (turnstileTaken <= turnstile.size() / 4) {
        auto iter = turnstile.begin();
        for (size_t i = 0; i < (size_t)(turnstile.size() * 0.75); ++i) {
            iter = turnstile.erase(iter);
        }
    }
}
