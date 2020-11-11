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
			Timer2::Resolution interval = Timer2::Resolution::ms1;
			Timer2::Capacity capacity = Timer2::Capacity::bits3;
			unsigned long startedAt = 7;
			unsigned long countTo = 3;
			unsigned long ommitedIntervals = 0;

			bool itsTime = Timer2::ItsTime(interval, capacity, &startedAt, countTo, &ommitedIntervals);
			if (itsTime)
			{
				Assert::Fail(L"itsTime");
			}
		}
	};
}
