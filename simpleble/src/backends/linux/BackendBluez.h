#pragma once

#include "AdapterLinux.h"
#include "BackendBase.h"
#include "BackendUtils.h"
#include "CommonUtils.h"
#include <simplebluez/Bluez.h>
#include <atomic>
#include <memory>
#include <mutex>
#include <thread>

namespace SimpleBLE {

class BackendBluez : public BackendSingleton<BackendBluez> {
public:
    BackendBluez(buildToken);
    virtual ~BackendBluez();

    std::shared_ptr<SimpleBluez::Bluez> bluez;

    virtual SharedPtrVector<AdapterBase> get_adapters() override;
    virtual bool bluetooth_enabled() override;
    std::string name() const noexcept override;
    void run_once();

private:
    std::thread* async_thread;
    std::atomic_bool async_thread_active;
    void async_thread_function();
};

std::shared_ptr<BackendBase> BACKEND_LINUX();

}  // namespace SimpleBLE