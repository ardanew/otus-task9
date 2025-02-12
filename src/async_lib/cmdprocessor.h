#pragma once
#include <deque>
#include "icmdprocessor.h"
#include "basestate.h"

/// \brief Process commands
/// \details Uses pattern 'state', changes state on start/end blocks
class CmdProcessor : public ICmdProcessor
{
public:
	CmdProcessor(size_t maxCommands);
	void startBlock() override;
	void endBlock() override;
	void eof() override;
	void process(std::shared_ptr<Command>&& cmd) override;
	void addOutput(std::unique_ptr<IOutput> output) override;

protected:
	std::deque<std::unique_ptr<IOutput>> m_outputs;

	std::unique_ptr<BaseState> m_currentState;
	std::unique_ptr<BaseState> m_otherState;
};