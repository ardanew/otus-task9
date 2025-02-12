#include "input.h"
#include <vector>
#include <string>
#include <sstream>
#include <chrono>
using namespace std;

void Input::setCmdProcessor(std::unique_ptr<ICmdProcessor> cmdProcessor)
{
	m_cmdProcessor = std::move(cmdProcessor);
}

void Input::eof()
{
	lock_guard l(m_interfaceProtect);
	m_cmdProcessor->eof();
}

void Input::receive(const char* data, size_t size)
{
	lock_guard l(m_interfaceProtect);

	string input{ data, data + size };

	// tokenize input
	vector<string> tokenized;
	{
		stringstream ss;
		ss << m_lastInputWithouEOL;
		ss << input;
		string token;

		while (getline(ss, token, '\n'))
			tokenized.push_back(token);

		// if data doesn't end with '\n' - save the tail to member variable
		if (input.back() != '\n')
		{
			m_lastInputWithouEOL = std::move(token);
			tokenized.pop_back();
		}
		else
			m_lastInputWithouEOL.clear();
	}

	for (string& token : tokenized)
	{
		if (token == "{")
			m_cmdProcessor->startBlock();
		else if (token == "}")
			m_cmdProcessor->endBlock();
		else
		{
			auto now = std::chrono::system_clock::now();

			auto cmd = std::make_unique<Command>();
			cmd->m_name = token;
			cmd->m_time = std::chrono::system_clock::to_time_t(now);

			m_cmdProcessor->process(std::move(cmd));
		}
	}
}

