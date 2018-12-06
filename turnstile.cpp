#include "turnstile.h"

Mutex::Mutex() : activeThreads(0) {}
std::set<std::mutex> Mutex::turnstile;
std::unordered_map<Mutex*, std::mutex*> Mutex::mutexLocker;
std::mutex Mutex::dataRace;

void Mutex::lock() {
    std::unique_lock<std::mutex> lk (dataRace);
    activeThreads++;

    if (activeThreads > 1) {
        if (activeThreads == 2) {
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
        mutexLocker.erase(this);
    }
    if (activeThreads > 0) {
        mutexLocker[this]->unlock();
    }
}

std::mutex* Mutex::getTurnstile() {
    //if (turnstile.empty()) {
        //std::mutex newMutex;
        //turnstile.emplace(newMutex);
    //}
    //std::mutex*& m = turnstile.front();
    //turnstile.pop();
    std::mutex* a = new std::mutex;
    return a;
}

void Mutex::giveBackTurnstile(std::mutex* m) {
    //turnstile.push(m);
}
