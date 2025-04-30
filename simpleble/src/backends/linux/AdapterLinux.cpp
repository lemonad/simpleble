#include <thread>

#include <simpleble/Peripheral.h>

#include "AdapterLinux.h"
#include "BuildVec.h"
#include "BuilderBase.h"
#include "CommonUtils.h"
#include "PeripheralLinux.h"
#include "BackendBluez.h"

using namespace SimpleBLE;

bool AdapterLinux::bluetooth_enabled() { return adapter_->powered(); }

AdapterLinux::AdapterLinux(std::shared_ptr<SimpleBluez::Adapter> adapter) : adapter_(adapter) {}

AdapterLinux::~AdapterLinux() {
    adapter_->clear_on_device_updated();

    if (custom_advertisement_) {
        adapter_->unregister_advertisement(custom_advertisement_);
    }

    if (custom_service_manager_) {
        adapter_->unregister_application(custom_service_manager_->path());
    } }

void* AdapterLinux::underlying() const { return adapter_.get(); }

std::string AdapterLinux::identifier() { return adapter_->identifier(); }

BluetoothAddress AdapterLinux::address() { return adapter_->address(); }

void AdapterLinux::power_on() {
    // TODO: This will be implemented after SimpleDBus and SimpleBluez are updated to the new architecture.
}

void AdapterLinux::power_off() {
    // TODO: This will be implemented after SimpleDBus and SimpleBluez are updated to the new architecture.
}

bool AdapterLinux::is_powered() {
    return adapter_->powered();
}

void AdapterLinux::scan_start() {
    seen_peripherals_.clear();

    adapter_->set_on_device_updated([this](std::shared_ptr<SimpleBluez::Device> device) {
        if (!this->is_scanning_) {
            return;
        }

        if (this->peripherals_.count(device->address()) == 0) {
            // If the incoming peripheral has never been seen before, create and save a reference to it.
            auto base_peripheral = std::make_shared<PeripheralLinux>(device, this->adapter_);
            this->peripherals_.insert(std::make_pair(device->address(), base_peripheral));
        }

        // Update the received advertising data.
        auto peripheral = this->peripherals_.at(device->address());

        // Check if the device has been seen before, to forward the correct call to the user.
        if (this->seen_peripherals_.count(device->address()) == 0) {
            // Store it in our table of seen peripherals
            this->seen_peripherals_.insert(std::make_pair(device->address(), peripheral));
            SAFE_CALLBACK_CALL(this->_callback_on_scan_found, Factory::build(peripheral));
        } else {
            SAFE_CALLBACK_CALL(this->_callback_on_scan_updated, Factory::build(peripheral));
        }
    });

    SimpleBluez::Adapter1::DiscoveryFilter filter;
    filter.Transport = SimpleBluez::Adapter::DiscoveryFilter::TransportType::LE;
    adapter_->discovery_filter(filter);

    // Start scanning and notify the user.
    adapter_->discovery_start();

    SAFE_CALLBACK_CALL(this->_callback_on_scan_start);
    is_scanning_ = true;
}

void AdapterLinux::scan_stop() {
    adapter_->discovery_stop();
    is_scanning_ = false;
    SAFE_CALLBACK_CALL(this->_callback_on_scan_stop);

    // Important: Bluez might continue scanning if another process is also requesting
    // scanning from the adapter. The use of the is_scanning_ flag is to prevent
    // any scan updates to reach the user when not expected.
}

void AdapterLinux::scan_for(int timeout_ms) {
    scan_start();
    std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));
    scan_stop();
}

bool AdapterLinux::scan_is_active() { return is_scanning_ && adapter_->discovering(); }

SharedPtrVector<PeripheralBase> AdapterLinux::scan_get_results() { return Util::values(seen_peripherals_); }

SharedPtrVector<PeripheralBase> AdapterLinux::get_paired_peripherals() {
    SharedPtrVector<PeripheralBase> peripherals;

    auto paired_list = adapter_->device_paired_get();
    for (auto& device : paired_list) {
        peripherals.push_back(std::make_shared<PeripheralLinux>(device, this->adapter_));
    }

    return peripherals;
}

void AdapterLinux::create_service(const ServiceData& service_data) {
    custom_service_manager_ = BackendBluez::get()->bluez->get_custom_service_manager();
    custom_advertisement_manager_ = BackendBluez::get()->bluez->get_custom_advertisement_manager();

    auto service0 = custom_service_manager_->create_service();
    service0->uuid(service_data.uuid);
    service0->primary(true);

    for (const auto& characteristic_data : service_data.characteristics) {
        auto characteristic = service0->create_characteristic();
        characteristic->uuid(characteristic_data.uuid);
        characteristic->flags(characteristic_data.flags);
        if (characteristic_data.write_callback) {
            characteristic->set_on_write_value(characteristic_data.write_callback);
        }
        custom_characteristics_.push_back(characteristic);
    }

    // Register the services and characteristics
    adapter_->register_application(custom_service_manager_->path());

    // NOTE: This long delay is not necessary. However, once an application is
    // registered you want to wait until all services have been added to the adapter.
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

void AdapterLinux::notify_characteristic(const std::string& characteristic_uuid, const ByteArray& value) {
    for (const auto& characteristic : custom_characteristics_) {
        if (characteristic->uuid() == characteristic_uuid) {
            auto flags = characteristic->flags();
            if (std::find(flags.begin(), flags.end(), "notify") != flags.end()) {
                characteristic->value(value);
            }
        }
    }
}

void AdapterLinux::create_advertisement(const AdvertisementData& advertisement_data) {
    if (!custom_advertisement_manager_) {
        throw std::runtime_error("Custom advertisement manager not created");
    }

    custom_advertisement_ = custom_advertisement_manager_->create_advertisement(advertisement_data.path);
    custom_advertisement_->timeout(0);
    custom_advertisement_->discoverable(true);
    custom_advertisement_->local_name(advertisement_data.name);
}

void AdapterLinux::start_advertisement() {
    if (!custom_advertisement_) {
        throw std::runtime_error("Custom advertisement not created");
    }

    adapter_->register_advertisement(custom_advertisement_);
}

void AdapterLinux::stop_advertisement() {
    if (!custom_advertisement_) {
        throw std::runtime_error("Custom advertisement not created");
    }

    adapter_->unregister_advertisement(custom_advertisement_);
}

void AdapterLinux::run_once() {
    BackendBluez::get()->run_once();
}

