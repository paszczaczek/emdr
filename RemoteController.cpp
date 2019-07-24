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
};


RemoteController::CodeMapper &RemoteController::codeMapper
	= RemoteController::_3939CodeMapper;