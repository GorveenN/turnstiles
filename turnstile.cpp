#include "turnstile.h"

Mutex::Mutex() : barrier(), activeThreads(0) {}
static std::queue<std::mutex*> turnstile;

void Mutex::lock() {
    activeThreads++;
    if (activeThreads > 1) {
        myTurnstile = takeTurnstile();

    }
    activeThreads--;
}

void Mutex::unlock() {
    activeThreads--;

    barrier.notify_all();
}

std::mutex* Mutex::takeTurnstile() {
    if (turnstile.empty()) {
        turnstile.emplace(new std::mutex());
    }
    std::mutex*& m = turnstile.front();
    turnstile.pop();
    return m;
}

void Mutex::giveBackTurnstile(std::mutex* m) {
    turnstile.push(m);
}
