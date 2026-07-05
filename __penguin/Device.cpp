#include "Device.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <cstdint>

Device::Device(std::unique_ptr<Transport> transport) :
	m_transport(std::move(transport))
{
    setOrientaion(Orientation::VERTICAL);
}

void Device::setOrientaion(Orientation orientation)
{
    m_orientation = orientation;

    if (m_orientation == Orientation::VERTICAL)
    {
        m_resolution = {320, 480};
    }
    else
    {
        m_resolution = { 480, 320 };
    }
}

void Device::run()
{
    if (!m_transport->open())
    {
        std::cout << "error opening com";
        return;
    }

    m_transport->send(DeviceCommand::Packets::Config::Init);

    uint16_t value = DeviceCommand::Packets::Brightness::Max;

    uint8_t high = (value >> 8) & 0xFF;
    uint8_t low = value & 0xFF;

    auto packet = DeviceCommand::Packets::Cmd::SetBrightness;
    packet.insert(packet.begin(), { high, low });

    m_transport->send(packet);

    m_transport->send(DeviceCommand::Packets::Cmd::TurnOn);

    m_transport->send(DeviceCommand::Packets::Config::EnterDisplayMode);

    if (m_orientation == Orientation::VERTICAL)
    {
        m_transport->send(DeviceCommand::Packets::Show::VerticalDefault);

        m_transport->send(DeviceCommand::Packets::Mode::Vertical);
    }
    else
    {
        m_transport->send(DeviceCommand::Packets::Show::HorizontalDefault);

        m_transport->send(DeviceCommand::Packets::Mode::Horizontal);
    }

    //background
    Image background = ImageLoader::instance()->loadFromResource(IDB_PNG1, L"PNG");

    m_transport->send(background.data);

    std::vector<int> resources = {
        IDB_PNG3, IDB_PNG4, IDB_PNG5,
        IDB_PNG6, IDB_PNG7, IDB_PNG8
    };

    std::vector<Image> footages;
    footages.reserve(resources.size());

    for (int id : resources)
    {
        footages.push_back(
            ImageLoader::instance()->loadFromResource(id, L"PNG")
        );
    }

    m_thread = std::thread([this, footages]()
        {               
            uint8_t pack[6];
            pack[5] = 197;
            while (true)
            {
                for (const Image& footage : footages)
                {
                    uint16_t x1 = m_resolution.width / 2 - footage.width / 2;
                    uint16_t y1 = m_resolution.height / 2 - footage.height / 2;

                    uint16_t x2 = x1 + footage.width - 1;
                    uint16_t y2 = y1 + footage.height - 1;

                    pack[0] = (uint8_t)(x1 >> 2);
                    pack[1] = (uint8_t)(((x1 & 3) << 6) + (y1 >> 4));
                    pack[2] = (uint8_t)(((y1 & 15) << 4) + (x2 >> 6));
                    pack[3] = (uint8_t)(((x2 & 63) << 2) + (y2 >> 8));
                    pack[4] = (uint8_t)(y2 & 0xFF);

                    m_transport->send(std::vector<uint8_t>(pack, pack + 6));
                    m_transport->send(footage.data);

                    Sleep(700);
                }
            }
        });

}