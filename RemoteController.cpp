#include "RemoteController.h"

RemoteController::CodeMapper RemoteController::codeMapper = {
  { 0x160, RemoteController::Button::CHANEL_PLUS },
  { 0x960, RemoteController::Button::CHANEL_PLUS },
  { 0x161, RemoteController::Button::CHANEL_MINUS },
  { 0x961, RemoteController::Button::CHANEL_MINUS },
  { 0x176, RemoteController::Button::PAUSE },
  { 0x976, RemoteController::Button::PAUSE },
  { 0x175, RemoteController::Button::PLAY },
  { 0x975, RemoteController::Button::PLAY }
};
byte RemoteController::codeMapperCount = 8;
