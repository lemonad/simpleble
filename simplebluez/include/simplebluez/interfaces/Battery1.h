#pragma once

#include <simpledbus/advanced/Interface.h>
#include "kvn/kvn_safe_callback.hpp"

#include <string>

namespace SimpleBluez {

class Battery1 : public SimpleDBus::Interface {
  public:
    Battery1(std::shared_ptr<SimpleDBus::Connection> conn, SimpleDBus::Proxy* proxy);
    virtual ~Battery1();

    // ----- METHODS -----

    // ----- PROPERTIES -----
    uint8_t Percentage();

    // ----- CALLBACKS -----
    kvn::safe_callback<void()> OnPercentageChanged;

  protected:
    void property_changed(std::string option_name) override;
};

}  // namespace SimpleBluez
