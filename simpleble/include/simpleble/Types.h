#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <functional>
#include "kvn/kvn_bytearray.h"

/**
 * @file Types.h
 * @brief Defines types and enumerations used throughout the SimpleBLE library.
 */

namespace SimpleBLE {

using BluetoothAddress = std::string;

// IDEA: Extend BluetoothUUID to include a `uuid` function that
// returns the same string, but provides a homogeneous interface.
using BluetoothUUID = std::string;

/**
 * @typedef ByteArray
 * @brief Represents a byte array using kvn::bytearray from the external library.
 */
using ByteArray = kvn::bytearray;

enum class OperatingSystem {
    WINDOWS,
    MACOS,
    LINUX,
};

// TODO: Add to_string functions for all enums.
enum BluetoothAddressType : int32_t { PUBLIC = 0, RANDOM = 1, UNSPECIFIED = 2 };

struct CharacteristicData {
    BluetoothUUID uuid;
    std::vector<std::string> flags;
    std::function<void(ByteArray value)> write_callback;
};

struct ServiceData {
    BluetoothUUID uuid;
    std::vector<CharacteristicData> characteristics;
};

struct AdvertisementData {
    std::string name;
    std::string path;
};

}  // namespace SimpleBLE
