#include "Scanner.h"

static void extractVidPid(const std::wstring& instanceId, uint16_t& vid, uint16_t& pid)
{
    std::wregex re(L"VID_([0-9A-Fa-f]{4}).*PID_([0-9A-Fa-f]{4})");
    std::wsmatch match;

    if (std::regex_search(instanceId, match, re))
    {
        vid = static_cast<uint16_t>(std::stoi(match[1].str(), nullptr, 16));
        pid = static_cast<uint16_t>(std::stoi(match[2].str(), nullptr, 16));
    }
}

static std::wstring extractComPort(const std::wstring& name)
{
    std::wregex re(L"(COM\\d+)");

    std::wsmatch match;

    if (std::regex_search(name, match, re))
        return match[0].str();

    return L"";
}

std::vector<DeviceInfo> Scanner::scan()
{
    std::vector<DeviceInfo> result;

    HDEVINFO deviceInfoSet = SetupDiGetClassDevsW(
        &GUID_DEVCLASS_PORTS,
        nullptr,
        nullptr,
        DIGCF_PRESENT
    );

    if (deviceInfoSet == INVALID_HANDLE_VALUE)
        return result;

    SP_DEVINFO_DATA devInfo{};
    devInfo.cbSize = sizeof(devInfo);

    for (DWORD i = 0;
        SetupDiEnumDeviceInfo(deviceInfoSet, i, &devInfo);
        i++)
    {
        wchar_t friendlyName[256] = { 0 };
        wchar_t instanceId[512] = { 0 };

        if (!SetupDiGetDeviceRegistryPropertyW(
            deviceInfoSet,
            &devInfo,
            SPDRP_FRIENDLYNAME,
            nullptr,
            (PBYTE)friendlyName,
            sizeof(friendlyName),
            nullptr))
            continue;

        if (!SetupDiGetDeviceInstanceIdW(
            deviceInfoSet,
            &devInfo,
            instanceId,
            std::size(instanceId),
            nullptr))
            continue;

        DeviceInfo info{};

        info.path = extractComPort(friendlyName);

        uint16_t vid = 0, pid = 0;
        extractVidPid(instanceId, vid, pid);

        info.vid = vid;
        info.pid = pid;

        result.push_back(info);
    }

    SetupDiDestroyDeviceInfoList(deviceInfoSet);

    return result;
}