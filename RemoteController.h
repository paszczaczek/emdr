#pragma once
#include <Arduino.h>
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
		Button button = UNSUPPORTED;
		auto it = codeMapper.find(code);
		if (it != codeMapper.end())
			button = it->second;
		else {
			char msg[32] = "";
			snprintf(msg, sizeof(msg), "unsup. RC code: %d\n", code);
			Serial.print(msg);
		}
		buttonPressed.Emit(EventArgs(button));
	}

	Event<EventArgs> buttonPressed;

private:
	static const std::map<int, Button> codeMapper;
};

