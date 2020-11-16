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
			Timer::Interval interval = Timer::Interval::ms1;
			Timer::Capacity capacity = Timer::Capacity::bits3;
			unsigned long startedAt = 7;
			unsigned long countTo = 3;
			unsigned long ommitedIntervals = 0;

			bool itsTime = Timer::ItsTime(interval, capacity, &startedAt, countTo, &ommitedIntervals);
			if (itsTime)
			{
				Assert::Fail(L"itsTime");
			}
		}
	};
}
