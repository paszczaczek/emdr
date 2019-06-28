// FastLED
#define FASTLED_INTERNAL
#include <bitswap.h>
#include <chipsets.h>
#include <color.h>
#include <colorpalettes.h>
#include <colorutils.h>
#include <controller.h>
#include <cpp_compat.h>
#include <dmx.h>
#include <FastLED.h>
#include <fastled_config.h>
#include <fastled_delay.h>
#include <fastled_progmem.h>
#include <fastpin.h>
#include <fastspi.h>
#include <fastspi_bitbang.h>
#include <fastspi_dma.h>
#include <fastspi_nop.h>
#include <fastspi_ref.h>
#include <fastspi_types.h>
#include <hsv2rgb.h>
#include <led_sysdefs.h>
#include <lib8tion.h>
#include <noise.h>
#include <pixelset.h>
#include <pixeltypes.h>
#include <platforms.h>
#include <power_mgt.h>

// resources
// https://youtu.be/JpEFAXenTyY?t=1964
// https://youtu.be/UhYu0k2woRM?t=177
// https://github.com/FastLED/FastLED
// https://github.com/FastLED/FastLED/wiki/Overview
// jedna dioda ~20mA * 3 kolory = 60mA
// usb ~400mA -> 6 diod
// wtyczka ~900mA -> 15 diod
// 330ohm, 100uF

const int LEDS_COUNT = 180;
const int LEDS_DATA_PIN = 7;
const EOrder LEDS_COLOR_ORDER = GRB;
CRGB leds[LEDS_COUNT];

void setup() {
  // put your setup code here, to run once:
  // fastled
  // delay(1000); // power-up safety delay
  Serial.begin(115200);
  FastLED.addLeds<WS2811, LEDS_DATA_PIN, LEDS_COLOR_ORDER>(leds, LEDS_COUNT);
  FastLED.setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(2);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 400);

  ledsOff();
}

void loop() {
  unsigned long now = millis();
//  moving_point(now);
//  moving_point1();
//  switchPoint();
  still_point();
}

void moving_point(unsigned long now) {
  static const unsigned int step_interval = 0;
  static unsigned long fired_at;
  static enum struct DIR { RIGHT, LEFT } dir = DIR::RIGHT;
  static const byte first_led_no = 5;
  static const byte last_led_no = LEDS_COUNT - 1;
  static byte curr_led_no = first_led_no;

  // czy juz czas?
  if (now - fired_at < step_interval)
    return;

  // zgas poprzednia diode
  leds[curr_led_no] = CRGB::Black;
  
  // wyznacz kolejna diode do zapalenia
  if (dir == DIR::RIGHT) {
    if (curr_led_no < last_led_no) {
      curr_led_no++;
    } else {
      curr_led_no--;
      dir = DIR::LEFT;
    }
  } else if (dir == DIR::LEFT) {
    if (curr_led_no > first_led_no) {
      curr_led_no--;
    } else {
      curr_led_no++;
      dir = DIR::RIGHT;
    }
  }

  // zapal dolejna diode
  leds[curr_led_no] = CRGB::Orange;

  FastLED.show();
  fired_at = now;
}


void still_point() {
  FastLED.setBrightness(1);
  //leds[LEDS_COUNT/2 +30] = CRGB::Orange;
  leds[89] = CRGB::Red;
  FastLED.show();
}

void switchPoint() {
  const byte left = 5, right = LEDS_COUNT - 1;
  static byte curr = right;
  leds[curr] = CRGB::Black;
  curr = curr == left ? right : left;
  leds[curr] = CRGB::Orange;
  FastLED.delay(1000);
}


void moving_point1() {
  for (int ledNo = 0; ledNo < LEDS_COUNT; ledNo++) {
    leds[ledNo] = CRGB::Orange;
    if (ledNo == 0 || ledNo == LEDS_COUNT - 1)
      FastLED.delay(500);
    else
      FastLED.show();
    leds[ledNo] = CRGB::Black;
    FastLED.show();
  }
  for (int ledNo = LEDS_COUNT - 1; ledNo >= 0; ledNo--) {
    leds[ledNo] = CRGB::Orange;
    FastLED.show();
    leds[ledNo] = CRGB::Black;
    FastLED.show();
  }
}

void ledsOff() {
  for (int ledNo = 0; ledNo < LEDS_COUNT; ledNo++)
    leds[ledNo] = CRGB::Blue;
  leds[0] = CRGB::Blue;
  FastLED.delay(1000);
  leds[0] = CRGB::Black;
}
