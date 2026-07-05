#pragma once

#include <windows.h>
#include <setupapi.h>
#include <devguid.h>
#include <initguid.h>
#include <cfgmgr32.h>
#include <vector>
#include <string>
#include <regex>

#pragma comment(lib, "Setupapi.lib")

#include "DeviceInfo.h"

class Scanner
{
public:
	std::vector<DeviceInfo> scan();
};

