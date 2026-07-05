#pragma once

#include <string>
#include <memory>
#include <thread>

#include "Transport.h"
#include "ImageLoader.h"
#include "DevicePackets.h"

#include "Resolution.h"
#include "Orientation.h"

#include "resource.h"

class Device
{
public:
	Device(std::unique_ptr<Transport> transport);

	void setOrientaion(Orientation orientation);

	void run();

private:
	std::unique_ptr<Transport> m_transport;

	Resolution m_resolution;
	Orientation m_orientation;

	std::thread m_thread;
};

