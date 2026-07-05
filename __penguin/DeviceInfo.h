#pragma once
#include <cstdint>
#include <string>

struct DeviceInfo {
    std::wstring path;
    uint16_t vid;
    uint16_t pid;
};