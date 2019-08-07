#pragma once
#include <Arduino.h>
#include "Event.h"

class Timer
{
  public:
    class EventArgs
    {
      public:
        //EventArgs(unsigned int elapsedIntervals)
        //{
        //  this->elapsedIntervals = elapsedIntervals;
        //}
        unsigned int elapsedIntervals;
    };

    Timer(byte handlersCapacity = 1) : elapsed(handlersCapacity) { }

    unsigned long interval = 0;
    bool autoReset = true;
    Event<EventArgs> elapsed;

    void Start()
    {
      elapsedAt = startedAt = millis();
    }

    void Stop()
    {
      startedAt = elapsedAt = -1;
    }

    void Loop()
    {
//      Serial.println("Timer Loop");
      if (startedAt == (unsigned long) - 1)
        return;
      auto now = millis();
      if (interval == 0)
      {
        Serial.println("Timer::Loop /0 !");
        Stop();
      }
      unsigned int elapsedIntervals = (now - elapsedAt) / interval;
      if (elapsedIntervals != 0)
      {
        elapsedAt += elapsedIntervals * interval;
        //Serial.println("Timer begin");        
        args.elapsedIntervals = elapsedIntervals;        
        elapsed.Emit(args);                  
        //Serial.println("Timer end");
        if (!autoReset)
          Stop();
      }
    }

  private:
    unsigned long startedAt = -1;
    unsigned long elapsedAt = -1;
    EventArgs args;
};
