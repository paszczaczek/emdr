#pragma once
#include "EventHandler.h"
//
template <class TEventArgs>
class Event
{
  public:
    Event(byte handlersCapacity = 1) :
      _handler(nullptr),
      _moreHandlersCapacity(handlersCapacity - 1),
      _moreHandlersCount(0)
    {
      if (_moreHandlersCapacity <= 1)
      {
        _moreHandlers = nullptr;
      } else {
        size_t size = _moreHandlersCapacity * sizeof(EventHandlerBase<TEventArgs> *);
        _moreHandlers = (EventHandlerBase<TEventArgs> **)malloc(size);
        memset(_moreHandlers, 0, size);
      }
    }


    Event& operator += (EventHandlerBase<TEventArgs> &eventHandler)
    {
      if (!_handler)
        _handler = &eventHandler;
      else
      {
        if (_moreHandlersCount < _moreHandlersCapacity)
          _moreHandlers[_moreHandlersCount] = &eventHandler;
        else
        {
          Serial.println(F("_moreHandlersCapacity to small"));
          return *this;
        }
      }
      _moreHandlersCount++;

      return *this;
    }


    void Emit(TEventArgs&/*&*/ eventArgs)
    {
      if (_handler)
      {
        Serial.println("  Event begin");
        _handler->Execute(eventArgs);        
        Serial.println("  Event end");
        //for (byte n = 0; n < _moreHandlersCount; n++)
        //  _moreHandlers[n]->Execute(eventArgs);
      }
    }

  private:
    EventHandlerBase<TEventArgs> *_handler;
    EventHandlerBase<TEventArgs> **_moreHandlers;
    byte _moreHandlersCapacity;
    byte _moreHandlersCount;
};
