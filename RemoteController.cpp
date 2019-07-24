#include "RemoteController.h"

RemoteController::CodeMapper RemoteController::AVerMediaCodeMapper = 
{
	{  0xC03FC03F, RemoteController::Button::CHANEL_PLUS },
	{  0xC03F40BF, RemoteController::Button::CHANEL_MINUS }
};

RemoteController::CodeMapper RemoteController::_3939CodeMapper =
{
	{ 0x160, RemoteController::Button::CHANEL_PLUS },
	{ 0x960, RemoteController::Button::CHANEL_PLUS },
	{ 0x161, RemoteController::Button::CHANEL_MINUS },
	{ 0x961, RemoteController::Button::CHANEL_MINUS },
	{ 0x176, RemoteController::Button::PAUSE },
	{ 0x976, RemoteController::Button::PAUSE },
	{ 0x175, RemoteController::Button::PLAY },
	{ 0x975, RemoteController::Button::PLAY }
};


RemoteController::CodeMapper &RemoteController::codeMapper
	= RemoteController::_3939CodeMapper;