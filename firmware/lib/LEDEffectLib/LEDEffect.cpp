/*
	LEDEffect.cpp - Library for LED Effecs.
	Created by Harrison H. Jones, October 3, 2014.
*/

#if defined (SPARK)
#include "application.h"
#else
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#endif

#include "LEDEffect.h"

#ifdef ANALOGWRITE
LEDEffect::LEDEffect(int pin)
{
	pinMode(pin, OUTPUT);
	_pin = pin;
	_time = millis();
	_brightness = _maxValue / 2;
	_fadeAmount = 5;    // how many points to fade the LED by
	_fadeDirection = _fadeAmount; 
	_ledState = 0;     // 0 = off, 1 = on, 2 = breath, 3 = fade down, 4 = fade up, 5 = blink
	_ledDelay = 30; // in ms
}
#endif

#ifdef FUNCTIONAL
LEDEffect::LEDEffect(vl::Func<void(int)> callback)
{
	_callback = callback;
	_time = millis();
	_brightness = _maxValue / 2;
	_fadeAmount = 5;    // how many points to fade the LED by
	_fadeDirection = _fadeAmount; 
	_ledState = 0;     // 0 = off, 1 = on, 2 = breath, 3 = fade down, 4 = fade up, 5 = blink
	_ledDelay = 30; // in ms
}
#endif

LEDEffect::LEDEffect(){};

void LEDEffect::update()
{
	if((millis() - _time) > _ledDelay)
	{
		_time = millis();
		if(_ledState == 0)
			_brightness = _minValue;
		else if(_ledState == 1)
			_brightness = _maxValue;
		else if (_ledState == 2)
		{
			// change the _brightness for next time through the loop:
			_brightness = _brightness + _fadeDirection;
			if(_brightness > _maxValue) {
				_brightness = _maxValue;
			}
			if(_brightness < _minValue) {
				_brightness = _minValue;
			}
			// reverse the direction of the fading at the ends of the fade:
			if (_brightness == _minValue)
				_fadeDirection = _fadeAmount;
			else if (_brightness == _maxValue)
				_fadeDirection = -_fadeAmount;
		}
		else if (_ledState == 3)	// Fade down
		{
			// change the _brightness for next time through the loop:
			_brightness = _brightness - _fadeAmount;
			if(_brightness > _maxValue) {
				_brightness = _maxValue;
			}
			if(_brightness < _minValue) {
				_brightness = _minValue;
			}
			// reverse the direction of the fading at the ends of the fade:
			if (_brightness == _minValue)
				_brightness = _maxValue;
		}
		else if (_ledState == 4)	// Fade up
		{
			// change the _brightness for next time through the loop:
			_brightness = _brightness + _fadeAmount;
			if(_brightness > _maxValue) {
				_brightness = _maxValue;
			}
			if(_brightness < _minValue) {
				_brightness = _minValue;
			}
			// reverse the direction of the fading at the ends of the fade:
			if (_brightness == _maxValue)
				_brightness = _minValue;
		}
		else if(_ledState == 5)
		{
			if(_brightness == _maxValue)
				_brightness = _minValue;
			else
				_brightness = _maxValue;
		}
		else if(_ledState == 6) // Dim
		{
		}

		//Serial.print("Brightness: ");
		//Serial.print(_brightness);
		//Serial.print("/");
		//Serial.println(_maxValue);
		if(_pin > -1) {
			#ifdef ANALOGWRITE
				analogWrite(_pin, _brightness);  
			#endif
		} else {
			#ifdef FUNCTIONAL
				_callback(_brightness);
			#endif
		}
	}
}

void LEDEffect::setMax(int max) {
	_maxValue = max;
}

void LEDEffect::off()
{
	_ledState = 0;
	_ledDelay = 10;
}

void LEDEffect::on()
{
	_ledState = 1;
	_ledDelay = 10;
}

void LEDEffect::reset()
{
	_brightness = _minValue;
}

void LEDEffect::breath(int ledDelay)
{
	_fadeAmount = 1;
	_ledDelay = _maxValue ? ledDelay / (_maxValue * 2): 1000;

    if(_brightness == 0)
        _fadeDirection = _fadeAmount;
    else if(_brightness == _maxValue)
        _fadeDirection = -_fadeAmount;
	_ledState = 2;
}

void LEDEffect::fadeDown(int ledDelay)
{
	_fadeAmount = 1;
	_ledDelay = _maxValue ? ledDelay / _maxValue: 1000;

    if(_brightness == 0)
        _brightness = _maxValue;
    _fadeDirection = -_fadeAmount;
	_ledState = 3;
}

void LEDEffect::fadeUp(int ledDelay)
{
	_fadeAmount = 1;
	_ledDelay = _maxValue ? ledDelay / _maxValue: 1000;

    if(_brightness == _maxValue)
        _brightness = 0;
    _fadeDirection = _fadeAmount;
	_ledState = 4;
}

void LEDEffect::blink(int ledDelay)
{
	_ledDelay = ledDelay / 2;
	_ledState = 5;
}

void LEDEffect::dim(unsigned char brightness)
{
    _ledState = 6;
    _brightness = brightness;
    _ledDelay = 1000;   // Not really required. 
}
