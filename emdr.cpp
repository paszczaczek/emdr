#define FASTLED_INTERNAL
#include <Arduino.h>
#include <FastLED.h>
#include <Wire.h>
//#include <LCD_I2C.h>
#include "Debug.h"
#include "MemoryFree.h"
#include "Timer.h"
#include "Strip.h"
#include "MovingPointStripPlugin.h"
#include "EmdrController/Event.h"
#include "Device.h"
#include "DiagnosticStripPlugin.h"

bool isDevelMode();
void onReceiveFromController(int);

#define MAX_CURRENT_FROM_EXT 1000 // Total maximum current draw when powered via external power supply
#define MAX_CURRENT_FROM_USB  500 // Total maximum current draw from the Arduino when powered from a USB port

#define STRIP_LEDS_PROD_PIN   7 // pin for long celling strip for production
#define STRIP_LEDS_DEVEL_PIN  3 // pin for short strip on board for development
#define STRIP_LEDS_PIN_RC     8 // pin for remote control strip
#define STRIP_LEDS_BRIGHTNESS 2 // jasnosc swiecenie diod
#define STRIP_LEDS_MAX_CURRENT MAX_CURRENT_FROM_USB

Lcd lcd;

constexpr int ENV_PIN = 4;     // pin for detecting production/development environmetn;
constexpr int emdrI2CAddr = 9;
const __FlashStringHelper* emdr;

Event::Name eventReceived = Event::Name::UnknowCode;
void powerOffReceive(Event::Name eventReceived);

// strip plugins
DiagnosticStipPlugin diagnosticStipPlugin;
MovingPointStripPlugin movingPointStripPlugin;

// strip
Strip strip(STRIP_LEDS_MAX_CURRENT, STRIP_LEDS_BRIGHTNESS);

// devices
Device& stripDevice = strip;

void setup()
{
	emdr = F("emdr");

	Serial.begin(115200);
	PRINT_FREEMEM(F("setup"));

	// inicjalizacja polaczenia z remoteController'em
	Wire.begin(emdrI2CAddr);
	Wire.onReceive(onReceiveFromController);

	// inicjalizacja wyswietlacza lcd
	lcd.begin(true);

	// konfiguracja zalezy od srodowidka prod/devel
	pinMode(ENV_PIN, INPUT_PULLUP);
	if (isDevelMode())
	{
		// w srodowisku devel tasma ma dlugosc 6 ledow, jest przyklejona do plyty glownej
		// i jest podlaczona do innego pinu
		movingPointStripPlugin.Setup(10, 0, 5);
		strip.SetController<WS2812B, STRIP_LEDS_DEVEL_PIN, GRB>();
	}
	else
	{
		movingPointStripPlugin.Setup(2);
		strip.SetController<WS2812B, STRIP_LEDS_PROD_PIN, GRB>();
	}

	// konfiguracja paska
	strip.Setup();

	// wykonanie testu wszystkich urzadzen a potem uruchomienie poruszajacego sie punktu
	diagnosticStipPlugin.Execute(
		DiagnosticStipPlugin::Action::TestAllDevices,
		//DiagnosticStipPlugin::Action::StartMovingPointStripPlugin);
		DiagnosticStipPlugin::Action::Idle);
	diagnosticStipPlugin.Start();
}

void loop()
{
	if (Serial.available() > 0) 
	{
		char c = Serial.read();

		if (c == '?')
		{
			lcd.setCursor(0, 1);
			lcd.print(F("poweroff"));

			Serial.print(emdr);
			Serial.print('\n');
		}
	}

	// obsluga odebranego kodu z remoteController'a
	if (eventReceived != Event::Name::UnknowCode)
	{
		strip.Receive(eventReceived);
		powerOffReceive(eventReceived);
		eventReceived = Event::Name::UnknowCode;
	}
	lcd.loop();
	stripDevice.Loop();
}

// ISR do odbioru polecen z remoteController'a
void onReceiveFromController(int)
{
	while (Wire.available())
		eventReceived = (Event::Name)Wire.read();
}

// ustalenie trypu pracy devel/prod
bool isDevelMode() {
	// ustawienie hardware sa zalezne od srodowiska production/developement
#ifdef _WIN32
	// wygodnie jest zalozyc ze emulator na windowsie pracuje w trybie production
	bool devel = false;
#else
	// zwracie pinu ENV_PIN do masy oznacza srodowisko production
	bool devel = digitalRead(ENV_PIN) == HIGH;
#endif

	const __FlashStringHelper* mode = devel ? F("dev") : F("prod");
	PRINT(emdr);
	PRINT(' ');
	PRINTLN(mode);
	lcd.print(emdr);
	lcd.print(' ');
	lcd.print(mode);

	return devel;
}

void powerOffReceive(Event::Name eventReceived)
{
	const __FlashStringHelper* suspend = F("suspend");
	const __FlashStringHelper* hibernate = F("hibernate");

	switch (eventReceived)
	{
	case Event::Power:
		Serial.print(suspend);
		Serial.print('\n');
		lcd.setCursor(0, 1);
		lcd.print(suspend);
		lcd.print(' ');
		lcd.print(' ');
		lcd.print(' ');
		lcd.print(' ');
		break;
	}
}
