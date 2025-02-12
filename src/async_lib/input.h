#pragma once
#include "iinput.h"
#include <memory>
#include <mutex>
#include "icmdprocessor.h"

class Input : public IInput
{
public:
	/// set next class in chain to process output of this class
	void setCmdProcessor(std::unique_ptr<ICmdProcessor> cmdProcessor);

	/// receive data, make commands from it, pass to cmdProcessor
	void receive(const char* data, size_t size) override;

	/// user closes the input
	void eof() override;

protected:
	std::unique_ptr<ICmdProcessor> m_cmdProcessor = {};

	/// user input can be without trailing '\n', save the tail of such input here
	/// to process later
	std::string m_lastInputWithouEOL = {};

	/// ensure thread-safety
	std::mutex m_interfaceProtect;
};