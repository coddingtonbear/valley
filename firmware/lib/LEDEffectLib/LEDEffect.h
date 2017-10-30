/*
    LEDEffect.h - Library for LED Effecs.
    Created by Harrison H. Jones, October 3, 2014.
*/

#define FUNCTIONAL
//#define ANALOGWRITE

#ifndef LEDEffect_h
#define LEDEffect_h

#ifdef FUNCTIONAL
#include <functional-vlpp.h>
#endif

#if defined (SPARK)
#include "application.h"
#else
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#endif

class LEDEffect
{
  public:
    #ifdef ANALOGWRITE
        LEDEffect(int pin);
    #endif
    #ifdef FUNCTIONAL
        LEDEffect(vl::Func<void(int)>);
    #endif
    LEDEffect();
    void update();
    void setMin(int);
    void setMax(int);
    void off();
    void on();
    void breath(int ledDelay);
    void fadeDown(int ledDelay);
    void fadeUp(int ledDelay);
    void blink(int ledDelay);
    void dim(unsigned char brightness);
    void reset();
  private:
    int _pin = -1;
    int _minValue = 0;
    int _maxValue = 255;
    #ifdef FUNCTIONAL
        vl::Func<void(int)> _callback;
    #endif
    unsigned int _brightness;
    unsigned int _fadeAmount;    // how many points to fade the LED by
    unsigned int _fadeDirection; 
    unsigned char _ledState;     // 0 = off, 1 = on, 2 = breath, 3 = fade down, 4 = fade up, 5 = blink
    int _ledDelay; // in ms
    unsigned long _time;
};

#endif
