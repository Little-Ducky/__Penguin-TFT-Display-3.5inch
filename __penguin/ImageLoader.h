#pragma once

#include <windows.h>
#include <wincodec.h>
#include <vector>
#include <iostream>

#pragma comment(lib, "Windowscodecs.lib")

#include "Image.h"

class ImageLoader
{
public:
	static ImageLoader* instance();

	bool init();

	Image loadFromResource(int resourceId, const wchar_t* resourceType);

private:
	ImageLoader();
	~ImageLoader();

	ImageLoader(const ImageLoader&) = delete;
	ImageLoader& operator=(const ImageLoader&) = delete;

private:
	bool m_initialized = false;

	IWICImagingFactory* m_factory = nullptr;
};

