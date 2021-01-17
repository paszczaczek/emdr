// te enmu'y jest wspoluzytkowane w projektach 
// - EmdrController (c++ arduino)
// - EmdrEmulator (cs windows)
// modyfikuj w projekcie *EmdrController*
// plik z projektu EmdrController jest kopiowany do projektu EmdEmulator

#if !__LINE__
namespace EmdrEmulator
{
#endif

enum RC8205LX_IRCode {
	Digit0 = 0x00,
	Digit1 = 0x01, 
	Digit2 = 0x02,
	Digit3 = 0x03,
	Digit4 = 0x04,
	Digit5 = 0x05,
	Digit6 = 0x06,
	Digit7 = 0x07,
	Digit8 = 0x08,
	Digit9 = 0x09,
	VolumePlus = 0x10,
	VolumeMinus = 0x11,
	ChannelPlus = 0x20,
	ChannelMinus = 0x21,
	Mute = 0x0d,
	Power = 0x0c,
	Display = 0x3c,
	UnknownCode = -1
};

#if !__LINE__
}
#endif