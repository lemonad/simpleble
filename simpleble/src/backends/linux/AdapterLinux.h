#pragma once

#include <simpleble/Exceptions.h>
#include <simpleble/Types.h>

#include "../common/AdapterBase.h"

#include <kvn_safe_callback.hpp>

#include <simplebluez/Adapter.h>
#include <simplebluez/CustomServiceManager.h>
#include <simplebluez/CustomAdvertisementManager.h>

#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace SimpleBLE {

class PeripheralLinux;

class AdapterLinux : public SimpleBLE::AdapterBase {
  public:
    AdapterLinux(std::shared_ptr<SimpleBluez::Adapter> adapter);
    virtual ~AdapterLinux();

    void* underlying() const override;

    virtual std::string identifier() override;
    virtual BluetoothAddress address() override;

    virtual void power_on() override;
    virtual void power_off() override;
    virtual bool is_powered() override;

    virtual void scan_start() override;
    virtual void scan_stop() override;
    virtual void scan_for(int timeout_ms) override;
    virtual bool scan_is_active() override;
    virtual std::vector<std::shared_ptr<PeripheralBase>> scan_get_results() override;

    virtual std::vector<std::shared_ptr<PeripheralBase>> get_paired_peripherals() override;

    virtual bool bluetooth_enabled() override;

    virtual void create_service(const ServiceData& service_data) override;
    virtual void create_advertisement(const AdvertisementData& advertisement_data) override;
    virtual void start_advertisement() override;
    virtual void stop_advertisement() override;
    virtual void run_once() override;
    virtual void notify_characteristic(const std::string& characteristic_uuid, const ByteArray& value) override;

  private:
    std::shared_ptr<SimpleBluez::Adapter> adapter_;
    std::shared_ptr<SimpleBluez::CustomServiceManager> custom_service_manager_;
    std::shared_ptr<SimpleBluez::CustomAdvertisementManager> custom_advertisement_manager_;
    std::shared_ptr<SimpleBluez::CustomAdvertisement> custom_advertisement_;
    std::vector<std::shared_ptr<SimpleBluez::CustomCharacteristic>> custom_characteristics_;

    std::atomic_bool is_scanning_;

    std::map<BluetoothAddress, std::shared_ptr<PeripheralLinux>> peripherals_;
    std::map<BluetoothAddress, std::shared_ptr<PeripheralLinux>> seen_peripherals_;
};

}  // namespace SimpleBLE
