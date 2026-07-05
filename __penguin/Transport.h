#pragma once
#include <windows.h>
#include <String>
#include <vector>

class Transport {
private:
    std::wstring m_port;
    HANDLE m_hSeries;
    DCB m_dcb;

public:
    Transport(const std::wstring& port);
    ~Transport();

    bool open();
    void send(const std::vector<uint8_t>& packet);
};