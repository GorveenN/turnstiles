#include <iostream>
#include "turnstile.h"

using namespace std;

int main() {

    cout << sizeof(std::mutex) << "\n";
    cout << sizeof(std::atomic_uint) << "\n";
    cout << sizeof(Mutex) << "\n";

    std::cout << "Hello, World!" << std::endl;
    return 0;
}