#pragma once
#include "Strip.h"
#include "Timer.h"


class DiagnosticStipPlugin : public StripPlugin
{
public:
	enum class Action {
		Idle = 0,
		TestAllDevices,
		StartMovingPointStripPlugin
	};

	virtual void Loop() override
	{
		if (state == State::Stopped)
			return;

		// czy minal czas wykonania nastepnej akcji/kroku
		unsigned long startedAt = timerStartedAt;
		if (Timer2::ItsTime(
			timerResolution, timerCapacity,
			&startedAt, timerCountTo,
			NULL))
		{
			timerStartedAt = startedAt;
			switch ((Action)actionFirst)
			{
			case Action::Idle:
				break;
			case Action::TestAllDevices:
				switch (actionStep++)
				{
				case 0: strip.SetLeds(CRGB::White); strip.updated = true; TimerStart();               break;
				case 1: strip.SetLeds(CRGB::Red);   strip.updated = true; TimerStart();               break;
				case 2: strip.SetLeds(CRGB::Green); strip.updated = true; TimerStart();               break;
				case 3: strip.SetLeds(CRGB::Blue);  strip.updated = true; TimerStart();               break;
				case 4: strip.SetLeds(CRGB::Black); strip.updated = true; TimerStart(); ActionNext(); break;
				}
				break;
			case Action::StartMovingPointStripPlugin:
				strip.GetMovingPointStripPlugin()->Start();
				ActionNext();
				break;
			default:
				break;
			}
		}
	}

	// wykonaj akcje first a potem akcje next
	void Execute(Action first, Action next = Action::Idle)
	{
		actionFirst = (byte)first;
		actionNext = (byte)next;
		actionStep = 0;
		TimerStart();
	}

private:
	// rozdzielczosc timera
	constexpr static Timer2::Resolution timerResolution = Timer2::Resolution::s1;

	// pojemnosc timera
	constexpr static Timer2::Capacity timerCapacity = Timer2::Capacity::bit1;

	// czas wystartowania timera
	byte timerStartedAt : static_cast<int>(timerCapacity);

	// po ilu interwalach wystrzelic timer
	byte timerCountTo : static_cast<int>(timerCapacity);

	// akcje do sekwencyjnego wykonania
	byte actionFirst: 2; // enum Action
	byte actionNext : 2;  // enum Action 

	// kolejny krok akcji
	byte actionStep : 4;

	// przejdz do wykonania nastepnej akcji
	void ActionNext()
	{
		actionFirst = actionNext;
		actionNext = (byte)Action::Idle;
		if (actionFirst == (byte)Action::Idle)
			TimerStop();
		else
			TimerStart();
	}

	// uruchom timer wykonywania akcji
	void TimerStart()
	{
		timerStartedAt = Timer2::Start(timerResolution, timerCapacity);
		timerCountTo = 1;
	}

	// zatrzymaj timer wykonywania akcji
	void TimerStop()
	{
		timerStartedAt = Timer2::Stop();
		timerCountTo = 0;
	}

	// obsluga eventow
	bool Receive(Event::Name eventName) override
	{
		return false;
	}
};

// sizeof(DiagnosticStipPlugin) = sizeof(StripPlugin):3 + 2 = 5
