#include <simplebluez/Bluez.h>

#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <thread>

std::shared_ptr<SimpleBluez::Bluez> bluez = SimpleBluez::Bluez::create();

std::atomic_bool async_thread_active = true;
void async_thread_function() {
    while (async_thread_active) {
        bluez->run_async();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void millisecond_delay(int ms) {
    for (int i = 0; i < ms; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

int main(int argc, char* argv[]) {
    std::thread* async_thread = new std::thread(async_thread_function);

    auto adapters = bluez->get_adapters();
    std::cout << "The following adapters were found:" << std::endl;
    for (int i = 0; i < adapters.size(); i++) {
        std::cout << "[" << i << "] " << adapters[i]->identifier() << " [" << adapters[i]->address() << "]"
                  << std::endl;
    }

    async_thread_active = false;
    while (!async_thread->joinable()) {
        millisecond_delay(10);
    }
    async_thread->join();
    delete async_thread;

    return 0;
}
