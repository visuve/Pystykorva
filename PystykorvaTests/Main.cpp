#include "PCH.hpp"

#if defined(_WIN32) && defined(_DEBUG)
#include "Win32/MemoryLeakListener.hpp"
#endif

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);

	auto instance = ::testing::UnitTest::GetInstance();

#if defined(_WIN32) && defined(_DEBUG)
	instance->listeners().Append(new MemoryLeakListener());
#endif

	return instance->Run();
}