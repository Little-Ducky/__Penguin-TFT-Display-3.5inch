#pragma once

#include <vector>

struct Image
{
	UINT width;
	UINT height;

	std::vector<uint8_t> data;
};