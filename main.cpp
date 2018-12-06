#include <iostream>
#include <mutex>
#include "turnstile.h"
#include <unordered_map>
#include <thread>

using namespace std;

Mutex mut;
int m = 0;


void g() {
    for (int i = 0; i < 10; ++i) {
        mut.lock();
        m++;
        mut.unlock();
    }
}


void f() {
    for (int i = 0; i < 10; ++i) {
        mut.lock();
        m++;
        mut.unlock();
    }
}








int main() {
/*
    cout << sizeof(std::mutex) << "\n";
    cout << sizeof(std::atomic_uint) << "\n";
    cout << sizeof(std::condition_variable) << "\n";
    cout << sizeof(Mutex) << "\n";*/


    std::thread t1(f); // creates a thread executing function f

    std::thread t2(g); // creates another thread executing function g

    t1.join(); // wait for t1 to complete

    t2.join(); // wait for t2 to complete



    return 0;
}