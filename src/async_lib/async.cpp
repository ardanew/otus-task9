#include "async.h"
#include <memory>
#include <mutex>
#include "input.h"
#include "cmdprocessor.h"
#include "stdoutput.h"
#include "logoutput.h"

using namespace std;

namespace async
{
/// protects g_openedHandles
std::mutex g_protect;
/// increments for each created instance
size_t g_openedHandles = 0; 

handle_t connect(size_t bulk) 
{ 
	Input* input = new Input();

	auto processor = make_unique<CmdProcessor>(bulk);
	auto stdOutput = make_unique<StdOutput>();

	size_t openedHandles;
	{
		lock_guard l(g_protect);
		openedHandles = ++g_openedHandles;
	}
	string handle_suffix = string("_h") + to_string(openedHandles);
	auto logOutput = make_unique<LogOutput>(handle_suffix);

	processor->addOutput(std::move(stdOutput));
	processor->addOutput(std::move(logOutput));

	input->setCmdProcessor(std::move(processor));

	return input;
}

void receive(handle_t handle, const char* data, size_t size) 
{ 
	IInput* instance = reinterpret_cast<IInput*>(handle);
	instance->receive(data, size);
}

void disconnect(handle_t handle)
{
	IInput* instance = reinterpret_cast<IInput*>(handle);
	instance->eof();
	delete instance;
}

} // namespace async
