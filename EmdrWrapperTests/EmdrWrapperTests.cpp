#include "pch.h"
#include "CppUnitTest.h"
#include "Timer.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace EmdrWrapperTests
{
	TEST_CLASS(EmdrWrapperTests)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			unsigned long startedAt = 0;
			unsigned long countTo = 0;
			unsigned long ommitedIntervals = 0;
			bool itsTime = Timer2::ItsTime(Timer2::Interval::ms1, Timer2::Capacity::bits32, &startedAt, countTo, &ommitedIntervals);
			if (itsTime)
			{
				Assert::Fail(L"itsTime");
			}
		}
	};
}
