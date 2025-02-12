#pragma once
#include "iinput.h"
#include <memory>
#include <mutex>
#include "icmdprocessor.h"

class Input : public IInput
{
public:
	void setCmdProcessor(std::unique_ptr<ICmdProcessor> cmdProcessor);
	void receive(const char* data, size_t size) override;
	void eof() override;

protected:
	std::unique_ptr<ICmdProcessor> m_cmdProcessor = {};
	std::string m_lastInputWithouEOL = {};
	std::mutex m_interfaceProtect;
};