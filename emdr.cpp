//#define FASTLED_ALLOW_INTERRUPTS 1
#define FASTLED_INTERNAL
#include <Arduino.h>
#include <FastLED.h>
#include <IRremote.h>
#include "Debug.h"
#include "MemoryFree.h"
#include "Timer.h"
#include "EventHandler.h"
#include "Strip.h"
#include "MovingPointStripPlugin.h"
#include "RemoteController.h"
#include "Event.h"
#include "Device.h"
#include "RemoteControllerStripPlugin.h"
//#include "DiagnosticStripPlugin.h"

bool isDevelMode();

//#define DEVEL                   // developing configuration

#define MAX_CURRENT_FROM_EXT 1000 // Total maximum current draw when powered via external power supply
#define MAX_CURRENT_FROM_USB  500 // Total maximum current draw from the Arduino when powered from a USB port

#define STRIP_LEDS_TYPE   WS2812B // led controller type (w pracy dziala rowniez na WS2812B)
#define STRIP_LEDS_PIN          7 // pin for main strip
#define STRIP_LEDS_ONBOARD_PIN  3 // pin for short strip on board for development
#define STRIP_LEDS_PIN_RC       8 // pin for remote control strip
#define STRIP_LEDS_ORDER      GRB
#define STRIP_LEDS_COUNT      179 // diods in celling strip
#define STRIP_LEDS_BRIGHTNESS  2
#define STRIP_LEDS_MAX_CURRENT MAX_CURRENT_FROM_USB

constexpr int ENV_PIN = 4;     // pin for detecting production/development environmetn;

#define RC_PIN  2 // pin for remote controller

// remote controller
IRrecv irrecv(RC_PIN);
RemoteController remoteController;

// strip plugins
//DiagnosticStipPlugin diagnosticStipPlugin;
MovingPointStripPlugin movingPointStripPlugin;
RemoteControllerStripPlugin remoteControllerstripPlugin;
const byte stripPluginsCount = /*3*/2;
StripPlugin* stripPlugins[stripPluginsCount] = {
	//&diagnosticStipPlugin,
	&movingPointStripPlugin,
	&remoteControllerstripPlugin
};

// strip
CRGB stripLeds[STRIP_LEDS_COUNT];
Strip strip(
	stripLeds, STRIP_LEDS_COUNT,
	stripPlugins, stripPluginsCount,
	STRIP_LEDS_MAX_CURRENT, STRIP_LEDS_BRIGHTNESS);
Device& stripDevice = strip;

void setup()
{
	Serial.begin(115200);
	PRINT_FREEMEM(("setup"));

	pinMode(ENV_PIN, INPUT_PULLUP);  
	if (isDevelMode())
	{
		// w srodowisku devel tasma ma dlugosc 10 ledow, jest przyklejona do plyty glownej
		// i jest podlaczona do innego pinu
		movingPointStripPlugin.Setup(10, 0, 5);
		strip.SetController<STRIP_LEDS_TYPE, STRIP_LEDS_ONBOARD_PIN, STRIP_LEDS_ORDER>();
	}
	else
	{
		movingPointStripPlugin.Setup(2);
		strip.SetController<STRIP_LEDS_TYPE, STRIP_LEDS_PIN, STRIP_LEDS_ORDER>();
	}


	irrecv.enableIRIn();
	//diagnosticStipPlugin.PowerOn();
	movingPointStripPlugin.Start();
}

void loop()
{
	remoteController.Loop();
	stripDevice.Loop();
}

bool isDevelMode() {
	// ustawienie hardware sa zalezne od srodowiska production/developement
#ifdef _WIN32
	// wygodnie jest zalozyc ze emulator na windowsie pracuje w trybie production
	bool devel = false;
#else
	// zwracie pinu ENV_PIN do masy oznacza srodowisko production
	bool devel = digitalRead(ENV_PIN) == HIGH;
#endif

	if (devel)
		PRINT(F("prod"));
	else
		PRINT(F("devel"));
	PRINT(" mode\n");

	return devel;
}


