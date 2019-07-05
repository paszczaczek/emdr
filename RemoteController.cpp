#include "RemoteController.h"

const std::map<int, RemoteController::Button> RemoteController::codeMapper = 
{
	{ 0, RemoteController::Button::DIGIT_1 },
	{ 1, RemoteController::Button::DIGIT_2 },
	{ 2, RemoteController::Button::DIGIT_3 }
};