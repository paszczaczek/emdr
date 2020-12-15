#pragma once

#include "pixeltypes.h"
#include "color.h"

#define BINARY_DITHER 0x01
typedef uint8_t EDitherMode;

class CLEDController {
protected:
	friend class CFastLED;
	CRGB *m_Data;
	CLEDController *m_pNext;
	CRGB m_ColorCorrection;
	CRGB m_ColorTemperature;
	EDitherMode m_DitherMode;
	int m_nLeds;
	//static CLEDController *m_pHead;
	//static CLEDController *m_pTail;

	/// set all the leds on the controller to a given color
	///@param data the crgb color to set the leds to
	///@param nLeds the numner of leds to set to this color
	///@param scale the rgb scaling value for outputting color
	virtual void showColor(const struct CRGB & data, int nLeds, CRGB scale) = 0;

	/// write the passed in rgb data out to the leds managed by this controller
	///@param data the rgb data to write out to the strip
	///@param nLeds the number of leds being written out
	///@param scale the rgb scaling to apply to each led before writing it out
	virtual void show(const struct CRGB *data, int nLeds, CRGB scale) = 0;

public:
	/// create an led controller object, add it to the chain of controllers
	CLEDController() : m_Data(NULL), m_ColorCorrection(UncorrectedColor), m_ColorTemperature(UncorrectedTemperature), m_DitherMode(BINARY_DITHER), m_nLeds(0) {
		m_pNext = NULL;
		//if (m_pHead == NULL) { m_pHead = this; }
		//if (m_pTail != NULL) { m_pTail->m_pNext = this; }
		//m_pTail = this;
	}
	///initialize the LED controller
	//virtual void init() = 0;

	///clear out/zero out the given number of leds.
	//virtual void clearLeds(int nLeds) { showColor(CRGB::Black, nLeds, CRGB::Black); }

	static CRGB computeAdjustment(uint8_t scale, const CRGB & colorCorrection, const CRGB & colorTemperature) {
#if defined(NO_CORRECTION) && (NO_CORRECTION==1)
		return CRGB(scale, scale, scale);
#else
		CRGB adj(0, 0, 0);

		if (scale > 0) {
			for (uint8_t i = 0; i < 3; i++) {
				uint8_t cc = colorCorrection.raw[i];
				uint8_t ct = colorTemperature.raw[i];
				if (cc > 0 && ct > 0) {
					uint32_t work = (((uint32_t)cc) + 1) * (((uint32_t)ct) + 1) * scale;
					work /= 0x10000L;
					adj.raw[i] = work & 0xFF;
				}
			}
		}

		return adj;
#endif
	}

	/// Get the combined brightness/color adjustment for this controller
	CRGB getAdjustment(uint8_t scale) {
		return computeAdjustment(scale, m_ColorCorrection, m_ColorTemperature);
	}

	/// show function w/integer brightness, will scale for color correction and temperature
	void show(const struct CRGB *data, int nLeds, uint8_t brightness) {
		getAdjustment(brightness);
		show(data, nLeds, getAdjustment(brightness));
	}

	/// show function w/integer brightness, will scale for color correction and temperature
	void showColor(const struct CRGB &data, int nLeds, uint8_t brightness) {
		showColor(data, nLeds, getAdjustment(brightness));
	}
	/// show function using the "attached to this controller" led data
	void showLeds(uint8_t brightness = 255) {
		show(m_Data, m_nLeds, getAdjustment(brightness));
	}

	/// show the given color on the led strip
	void showColor(const struct CRGB & data, uint8_t brightness = 255) {
		showColor(data, m_nLeds, getAdjustment(brightness));
	}

	/*
	/// get the first led controller in the chain of controllers
	static CLEDController *head() { return m_pHead; }
	/// get the next controller in the chain after this one.  will return NULL at the end of the chain
	CLEDController *next() { return m_pNext; }

	*/
	/// set the default array of leds to be used by this controller
	CLEDController & setLeds(CRGB *data, int nLeds) {
		m_Data = data;
		m_nLeds = nLeds;
		return *this;
	}

	/// zero out the led data managed by this controller
	void clearLedData() {
		if (m_Data) {
			//memset8((void*)m_Data, 0, sizeof(struct CRGB) * m_nLeds);
			memset((void*)m_Data, 0, sizeof(struct CRGB) * m_nLeds);
		}
	}

	/// How many leds does this controller manage?
	virtual int size() { return m_nLeds; }

	/// Pointer to the CRGB array for this controller
	CRGB* leds() { return m_Data; }

	/*
	/// Reference to the n'th item in the controller
	CRGB &operator[](int x) { return m_Data[x]; }

	/// set the dithering mode for this controller to use
	inline CLEDController & setDither(uint8_t ditherMode = BINARY_DITHER) { m_DitherMode = ditherMode; return *this; }
	/// get the dithering option currently set for this controller
	inline uint8_t getDither() { return m_DitherMode; }

	/// the the color corrction to use for this controller, expressed as an rgb object
	CLEDController & setCorrection(CRGB correction) { m_ColorCorrection = correction; return *this; }
	/// set the color correction to use for this controller
	CLEDController & setCorrection(LEDColorCorrection correction) { m_ColorCorrection = correction; return *this; }
	/// get the correction value used by this controller
	CRGB getCorrection() { return m_ColorCorrection; }

	/// set the color temperature, aka white point, for this controller
	CLEDController & setTemperature(CRGB temperature) { m_ColorTemperature = temperature; return *this; }
	/// set the color temperature, aka white point, for this controller
	CLEDController & setTemperature(ColorTemperature temperature) { m_ColorTemperature = temperature; return *this; }
	/// get the color temperature, aka whipe point, for this controller
	CRGB getTemperature() { return m_ColorTemperature; }

	*/


	/*
	virtual uint16_t getMaxRefreshRate() const { return 0; }
	*/
};

