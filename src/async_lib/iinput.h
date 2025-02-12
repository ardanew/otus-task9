#pragma once
#include <cstddef>

/// Interface to handle user input
struct IInput
{
	virtual ~IInput() = default;
	virtual void receive(const char* data, size_t size) = 0;
	virtual void eof() = 0;
};