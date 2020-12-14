// te enmu'y jest wspoluzytkowane w projektach 
// - EmdrController (c++ arduino)
// - EmdrEmulator (cs windows)
// modyfikuj w projekcie *EmdrController*
// plik z projektu EmdrController jest kopiowany do projektu EmdEmulator

#if !__LINE__
namespace EmdrEmulator
{
#endif

enum AverMediaIRCode {
	Digit1 = 0x40BFA05F, 
	Digit2 = 0x40BF609F,
	Digit3 = 0x40BFE01F,
	Digit4 = 0x40BF906F,
	Digit5 = 0x40BF50AF,
	Digit6 = 0x40BFD02F,
	Digit7 = 0x40BFB04F,
	Digit8 = 0x40BF708F,
	Digit9 = 0x40BFF00F,
	Digit0 = 0x40BF8877
};

#if !__LINE__
}
#endif