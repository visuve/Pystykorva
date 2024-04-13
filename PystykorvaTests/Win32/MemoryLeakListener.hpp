#pragma once

#include <crtdbg.h>

class MemoryLeakListener : public testing::EmptyTestEventListener
{
public:
	MemoryLeakListener();
	~MemoryLeakListener();

	void OnTestStart(const testing::TestInfo&) override;
	void OnTestEnd(const testing::TestInfo& testInfo) override;

private:
	_CrtMemState _testBegin = {};
};