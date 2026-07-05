#include "Transport.h"

Transport::Transport(const std::wstring& port) :
	m_port(port) 
{
};

Transport::~Transport() 
{
	if (m_hSeries != INVALID_HANDLE_VALUE) 
	{
		CloseHandle(m_hSeries);
	}
}

bool Transport::open()
{
	std::wstring path = L"\\\\.\\";
	path += m_port;

	m_hSeries = CreateFile(
		path.c_str(),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
	);

	if (m_hSeries == INVALID_HANDLE_VALUE)
	{
		MessageBox(nullptr, L"INVALID_HANDLE_VALUE", L"Error", MB_ICONERROR);
		
		return false;
	}

	if (!GetCommState(m_hSeries, &m_dcb))
	{
		DWORD err = GetLastError();

		CloseHandle(m_hSeries);
		m_hSeries = INVALID_HANDLE_VALUE;

		std::wstring msg = L"Error code: " + std::to_wstring(err);
		MessageBox(nullptr, msg.c_str(), L"Get Device Control Block failed", MB_ICONERROR);

		return false;
	}

	m_dcb.BaudRate = CBR_115200;
	m_dcb.ByteSize = 8;
	m_dcb.StopBits = ONESTOPBIT;
	m_dcb.Parity = NOPARITY;

	if (!SetCommState(m_hSeries, &m_dcb))
	{
		DWORD err = GetLastError();

		CloseHandle(m_hSeries);
		m_hSeries = INVALID_HANDLE_VALUE;

		std::wstring msg = L"Error code: " + std::to_wstring(err);
		MessageBox(nullptr, msg.c_str(), L"Set Device Control Block failed", MB_ICONERROR);
		
		return false;
	}

	return true;
}

void Transport::send(const std::vector<uint8_t>& packet)
{
	WriteFile(m_hSeries, packet.data(), packet.size(), nullptr, nullptr);
}