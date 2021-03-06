// enmu Name jest wspoluzytkowane w projektach 
// - EmdrController (c++ arduino)
// - EmdrEmulator (cs windows)
// modyfikuj w projekcie *EmdrController*
// plik z EmdrController jest kopiowany do EmdEmulator

#if !__LINE__
namespace EmdrEmulator
{
#endif

enum Name
{
	UnknowCode = -1,
	Digit0,
	Digit1, 
	Digit2, 
	Digit3, 
	Digit4, 
	Digit5, 
	Digit6, 
	Digit7, 
	Digit8, 
	Digit9, 
	VolumePlus,
	VolumeMinus,
	ChannelPlus,
	ChannelMinus,
	Mute,
	Power,
	Display,
	Start,
	Stop,
	Pause
};

#if !__LINE__
}
#endif