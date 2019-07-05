#pragma once
#include <vector>
#include <map>
#include "Event.h"

class RemoteController
{
public:
	enum Button {
		UNSUPPORTED,
		DIGIT_1,
		DIGIT_2,
		DIGIT_3
	};

	class EventArgs
	{
	public:

		EventArgs(Button button)
		{
			this->button = button;
		}

		Button button;
	};

	void ProcessCode(int code)
	{
		auto it = codeMapper.find(code);
		if (it == codeMapper.end())
			return;

		Button button = it->second;
		buttonPressed.Emit(EventArgs(button));
	}

	Event<EventArgs> buttonPressed;

private:
	static const std::map<int, Button> codeMapper;
};

