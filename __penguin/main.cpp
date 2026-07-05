// scans all coms
#include "Application.h"

#include "Scanner.h"

#include "Device.h"
#include "ImageLoader.h"

#include <Windows.h>
#include <shellapi.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
{
	Application app;

	if (!app.init(hInstance))
	{
		return 1;
	}

	if (!ImageLoader::instance()->init())
	{
		MessageBox(nullptr, L"ImageLoader init failed", L"Error", MB_OK | MB_ICONERROR);

		return 0;
	}

	Scanner scanner;
	
	// screen vid 0x1A86 pid 0x5742

	std::vector<DeviceInfo> devices = scanner.scan();

	auto it = std::find_if(devices.begin(), devices.end(),
		[&](const DeviceInfo& device)
		{
			return device.vid == 0x1A86 && device.pid == 0x5722;
		});

	if (it == devices.end())
	{
		MessageBox(nullptr, L"screen not found", L"Error", MB_OK | MB_ICONERROR);

		return 0;
	}

	Device device(std::make_unique<Transport>(it->path));

	device.run();

	MSG msg;

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}