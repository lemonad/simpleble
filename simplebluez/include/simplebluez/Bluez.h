#pragma once

#include <simpledbus/base/Connection.h>
#include <simplebluez/BluezRoot.h>
#include <simplebluez/CustomRoot.h>
#include <simplebluez/CustomAdvertisementManager.h>
#include <simplebluez/Adapter.h>
#include <simplebluez/Agent.h>

namespace SimpleBluez {

class Bluez {
  public:

    // Note: This class MUST be consumed as a shared_ptr, as internally objects
    // are linked together and proper usage of `shared_from_this()` is relied upon.
    // TODO: This restriction has been removed, but we'll leave this as-is for now.
    // TODO: We need to separate the initialization procedure from the constructor to avoid crashes if init fails.
    static std::shared_ptr<Bluez> create() {
        static std::shared_ptr<Bluez> instance = std::shared_ptr<Bluez>(new Bluez());
        instance->init();
        return instance;
    }
    virtual ~Bluez();

    // Delete copy and move operations
    Bluez(const Bluez&) = delete;
    Bluez& operator=(const Bluez&) = delete;
    Bluez(Bluez&&) = delete;
    Bluez& operator=(Bluez&&) = delete;

    void run_async();

    std::vector<std::shared_ptr<Adapter>> get_adapters();
    std::shared_ptr<Agent> get_agent();
    void register_agent();

    std::shared_ptr<CustomAdvertisementManager> get_custom_advertisement_manager();
    std::shared_ptr<CustomServiceManager> get_custom_service_manager();
  private:

    Bluez();
    void init();

    std::shared_ptr<SimpleDBus::Connection> _conn;
    std::shared_ptr<SimpleBluez::BluezRoot> _bluez_root;
    std::shared_ptr<SimpleBluez::CustomRoot> _custom_root;

};

}  // namespace SimpleBluez
