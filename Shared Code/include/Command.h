#pragma once

#include <vector>

class Command{
public:
	virtual void execute() = 0;
	virtual void unexecute() = 0;

	std::vector<Command *> subCommands;
};
