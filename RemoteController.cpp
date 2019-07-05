#include "RemoteController.h"

const std::map<int, RemoteController::Button> RemoteController::codeMapper = 
{
	{ 1, RemoteController::Button::DIGIT_1 },
	{ 2, RemoteController::Button::DIGIT_2 },
	{ 3, RemoteController::Button::DIGIT_3 }
};