#pragma once
#include <deque>
#include <memory>
#include "command.h"
#include "ioutput.h"

/// \brief Interface for class that produces batches of commands
class ICmdProcessor
{
public:
	virtual ~ICmdProcessor() = default;
	/// \brief Called on '{' input symbol
	virtual void startBlock() = 0;
	/// \brief Called on '}' output symbol
	virtual void endBlock() = 0;
	/// \brief Called when input ends
	virtual void eof() = 0;
	/// \brief Called on each command
	virtual void process(std::shared_ptr<Command> &&cmd) = 0;
	/// \brief Adds classes to handle batch command lists
	virtual void addOutput(std::unique_ptr<IOutput> output) = 0;
};