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
#include "DiagnosticStripPlugin.h"

bool isDevelMode();

#define MAX_CURRENT_FROM_EXT 1000 // Total maximum current draw when powered via external power supply
#define MAX_CURRENT_FROM_USB  500 // Total maximum current draw from the Arduino when powered from a USB port

#define STRIP_LEDS_PROD_PIN   7 // pin for long celling strip for production
#define STRIP_LEDS_DEVEL_PIN  3 // pin for short strip on board for development
#define STRIP_LEDS_PIN_RC     8 // pin for remote control strip
#define STRIP_LEDS_BRIGHTNESS 2 // jasnosc swiecenie diod
#define STRIP_LEDS_MAX_CURRENT MAX_CURRENT_FROM_USB

constexpr int ENV_PIN = 4;     // pin for detecting production/development environmetn;

#define RC_PIN  2 // pin for remote controller

// remote controller
IRrecv irrecv(RC_PIN);
RemoteController remoteController;

// strip plugins
DiagnosticStipPlugin diagnosticStipPlugin;
MovingPointStripPlugin movingPointStripPlugin;
RemoteControllerStripPlugin remoteControllerstripPlugin;

// strip
Strip strip(STRIP_LEDS_MAX_CURRENT, STRIP_LEDS_BRIGHTNESS);
Device& stripDevice = strip;

void setup()
{
	Serial.begin(115200);
	PRINT_FREEMEM(("setup"));

	// konfiguracja zalezy od srodowidka prod/devel
	pinMode(ENV_PIN, INPUT_PULLUP);  
	if (isDevelMode())
	{
		// w srodowisku devel tasma ma dlugosc 10 ledow, jest przyklejona do plyty glownej
		// i jest podlaczona do innego pinu
		movingPointStripPlugin.Setup(10, 0, 5);
		strip.SetController<WS2812B, STRIP_LEDS_DEVEL_PIN, GRB>();
	}
	else
	{
		movingPointStripPlugin.Setup(2);
		strip.SetController<WS2812B, STRIP_LEDS_PROD_PIN, GRB>();
	}

	// dodanie pluginow do stripu
	strip.AddDiagnosticStipPlugin(&diagnosticStipPlugin);
	strip.AddMovingPointStripPlugin(&movingPointStripPlugin);

	irrecv.enableIRIn();

	// wykonanie testu wszystkich urzadzen a potem uruchomienie poruszajacego sie punktu
	diagnosticStipPlugin.Execute(
		DiagnosticStipPlugin::Action::TestAllDevices, 
		DiagnosticStipPlugin::Action::StartMovingPointStripPlugin);
	diagnosticStipPlugin.Start();
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
		PRINT(F("devel"));
	else
		PRINT(F("prod"));
	PRINT(" mode\n");

	return devel;
}


