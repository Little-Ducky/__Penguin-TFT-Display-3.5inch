#include "ImageLoader.h"

ImageLoader::ImageLoader()
{
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    if (FAILED(hr))
    {
        MessageBox(nullptr, L"CoInitializeEx failed", L"Error", MB_OK | MB_ICONERROR);
        m_initialized = false;
    }
    else
    {
        m_initialized = true;
    }
}

ImageLoader::~ImageLoader()
{
    if (m_initialized)
    {
        m_factory->Release();
        CoUninitialize();
    }
}

ImageLoader* ImageLoader::instance()
{
    static ImageLoader inst;
    return &inst;
}

bool ImageLoader::init()
{
	if (!m_initialized)
	{
		return false;
	}

	HRESULT hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&m_factory)
	);

	if (FAILED(hr))
	{
        MessageBox(nullptr, L"Cannot create WIC Factory", L"Error", MB_OK | MB_ICONERROR);
		CoUninitialize();

        m_initialized = false;
	}

	return m_initialized;
}
Image ImageLoader::loadFromResource(int resourceId, const wchar_t* resourceType)
{
    if (!m_initialized)
    {
        MessageBox(nullptr, L"!m_initialized ImageLoader", L"Error", MB_OK | MB_ICONERROR);
        return { 0, 0, {0} };
    }

    HINSTANCE hInst = GetModuleHandle(nullptr);

    HRSRC hRes = FindResource(hInst, MAKEINTRESOURCE(resourceId), resourceType);

    if (!hRes)
    {
        MessageBox(nullptr, L"Resource not found", L"Error", MB_OK | MB_ICONERROR);
        return { 0,0,{0} };
    }

    HGLOBAL hData = LoadResource(hInst, hRes);

    if (!hData)
    {
        MessageBox(nullptr, L"LoadResource failed", L"Error", MB_OK | MB_ICONERROR);
        return { 0,0,{0} };
    }

    void* data = LockResource(hData);
    DWORD size = SizeofResource(hInst, hRes);

    IWICStream* stream = nullptr;
    IWICBitmapDecoder* decoder = nullptr;
    IWICBitmapFrameDecode* frame = nullptr;
    IWICFormatConverter* converter = nullptr;

    HRESULT hr;

    hr = m_factory->CreateStream(&stream);
    if (FAILED(hr)) return { 0,0,{0} };

    hr = stream->InitializeFromMemory((BYTE*)data, size);
    if (FAILED(hr)) return { 0,0,{0} };

    hr = m_factory->CreateDecoderFromStream(
        stream,
        nullptr,
        WICDecodeMetadataCacheOnLoad,
        &decoder
    );
    if (FAILED(hr))
    {
        stream->Release();
        return { 0,0,{0} };
    }

    hr = decoder->GetFrame(0, &frame);
    if (FAILED(hr))
    {
        decoder->Release();
        stream->Release();
        return { 0,0,{0} };
    }

    UINT width = 0, height = 0;
    frame->GetSize(&width, &height);

    hr = m_factory->CreateFormatConverter(&converter);
    if (FAILED(hr))
    {
        frame->Release();
        decoder->Release();
        stream->Release();
        return { 0,0,{0} };
    }

    hr = converter->Initialize(
        frame,
        GUID_WICPixelFormat32bppRGBA,
        WICBitmapDitherTypeNone,
        nullptr,
        0.0,
        WICBitmapPaletteTypeCustom
    );

    if (FAILED(hr))
    {
        converter->Release();
        frame->Release();
        decoder->Release();
        stream->Release();
        return { 0,0,{0} };
    }

    std::vector<uint8_t> pixels(width * height * 4);

    hr = converter->CopyPixels(
        nullptr,
        width * 4,
        pixels.size(),
        pixels.data()
    );

    if (FAILED(hr))
    {
        converter->Release();
        frame->Release();
        decoder->Release();
        stream->Release();
        return { 0,0,{0} };
    }

    std::vector<uint8_t> pixels565(width * height * 2);

    for (size_t i = 0, j = 0; i < pixels.size(); i += 4, j += 2)
    {
        uint8_t r = pixels[i + 0];
        uint8_t g = pixels[i + 1];
        uint8_t b = pixels[i + 2];

        uint16_t color =
            ((r >> 3) << 11) |
            ((g >> 2) << 5) |
            (b >> 3);

        pixels565[j + 0] = color & 0xFF;
        pixels565[j + 1] = (color >> 8) & 0xFF;
    }

    converter->Release();
    frame->Release();
    decoder->Release();
    stream->Release();

    return { width, height, pixels565 };
}