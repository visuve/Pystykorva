#include "PCH.hpp"
#include "MemoryLeakListener.hpp"

MemoryLeakListener::MemoryLeakListener() :
	testing::EmptyTestEventListener()
{
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);

	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);

	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
}

MemoryLeakListener::~MemoryLeakListener()
{
}

void MemoryLeakListener::OnTestStart(const testing::TestInfo&)
{
	memset(&_testBegin, 0, sizeof(_CrtMemState));
	_CrtMemCheckpoint(&_testBegin);
}

void MemoryLeakListener::OnTestEnd(const testing::TestInfo& testInfo)
{
	if (!testInfo.result()->Passed())
	{
		return;
	}

	_CrtMemState testEnd = {};
	_CrtMemCheckpoint(&testEnd);

	_CrtMemState diff = {};
	int significantlyDifferent = _CrtMemDifference(&diff, &_testBegin, &testEnd);

	if (significantlyDifferent != 0)
	{
		_CrtMemDumpStatistics(&diff);
	}

	EXPECT_LE(significantlyDifferent, 0) << "Probable memory leak detected";
}