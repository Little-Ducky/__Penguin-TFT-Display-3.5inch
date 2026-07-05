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
		DWORD err = GetLastError();

		wchar_t* msg = nullptr;

		FormatMessageW(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr,
			err,
			0,
			(LPWSTR)&msg,
			0,
			nullptr
		);

		wchar_t buffer[512];
		wsprintf(buffer, L"Error opening COM port.\nCode: %lu\n\n%s", err, msg);

		MessageBoxW(nullptr, buffer, L":(", MB_ICONERROR);

		LocalFree(msg);

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