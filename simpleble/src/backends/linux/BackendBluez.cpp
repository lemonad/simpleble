#include "BackendBluez.h"

namespace SimpleBLE {

std::shared_ptr<BackendBase> BACKEND_LINUX() {
    return BackendBluez::get();
}

BackendBluez::BackendBluez(buildToken) {
    static std::mutex get_mutex;       // Static mutex to ensure thread safety when accessing the logger
    std::scoped_lock lock(get_mutex);  // Unlock the mutex on function return
    bluez = SimpleBluez::Bluez::create();
    async_thread_active = true;
    // async_thread = new std::thread(&BackendBluez::async_thread_function, this);
}

BackendBluez::~BackendBluez() {
    async_thread_active = false;
    while (!async_thread->joinable()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    async_thread->join();
    delete async_thread;
}

SharedPtrVector<AdapterBase> BackendBluez::get_adapters() {
    SharedPtrVector<AdapterBase> adapter_list;
    auto internal_adapters = bluez->get_adapters();
    for (auto& adapter : internal_adapters) {
        adapter_list.push_back(std::make_shared<AdapterLinux>(adapter));
    }
    return adapter_list;
}

bool BackendBluez::bluetooth_enabled() {
    bool enabled = false;
    auto internal_adapters = bluez->get_adapters();
    for (auto& adapter : internal_adapters) {
        if (adapter->powered()) {
            enabled = true;
            break;
        }
    }
    return enabled;
}

std::string BackendBluez::name() const noexcept {
    return "SimpleBluez";
}

void BackendBluez::async_thread_function() {
    SAFE_RUN({ bluez->register_agent(); });
    while (async_thread_active) {
        SAFE_RUN({ bluez->run_async(); });
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
}

void BackendBluez::run_once() {
    SAFE_RUN({ bluez->run_async(); });
}


}  // namespace SimpleBLE