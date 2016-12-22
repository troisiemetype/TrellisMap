/*
 * This Arduino library is for virtually extending the size of a trellis panel.
 * Copyright (C) 2016  Pierre-Loup Martin
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TRELLISMAP_H
#define TRELLISMAP_H

#include <Arduino.h>
#include <Adafruit_Trellis.h>

#define TRELLIS_U 			13
#define TRELLIS_D 			14
#define TRELLIS_R 			15
#define TRELLIS_L 			12

#define TRELLIS_U1			1
#define TRELLIS_U2			2
#define TRELLIS_R1			7
#define TRELLIS_R2			11
#define TRELLIS_D1          13
#define TRELLIS_D2          14
#define TRELLIS_L1			4
#define TRELLIS_L2			8

class TrellisMap{

public:

	TrellisMap();
	~TrellisMap();

	bool begin(Adafruit_Trellis* mat, byte xMapSize, byte yMapSize);
//	bool begin(Adafruit_TrellisSet* _mat, byte _xMapSize, byte _yMapSize, byte _xTrellisSize, byte _yTrellisSize);

	void setOffsetX(char offset);
	char getOffsetX();

	void setOffsetY(char offset);
	char getOffsetY();

	int getSize();
	int getSizeX();
	int getSizeY();

	bool readSwitches();
	bool isKeyPressed(byte key);
	bool wasKeyPressed(byte key);

	bool justPressed(byte key);
	bool justReleased(byte key);

	bool isTKeyPressed(byte key);
	bool wasTKeyPressed(byte key);

	bool justTPressed(byte key);
	bool justTReleased(byte key);

	void writeDisplay();
	void clear();

	bool isLED(byte led);
	void setLED(byte led);
	void clrLED(byte led);

	bool isTLED(byte led);
	void setTLED(byte led);
	void clrTLED(byte led);


protected:

	byte _mapToTrellis(byte key);
	byte _trellisToMap(byte key);
	
	void _updateVisible();
	void _updateMap();

	bool _set;

	Adafruit_Trellis *_matrice;
//	Adafruit_TrellisSet *matrices;

	int _size;
	byte _sizeX;
	byte _sizeY;

	char _offsetX;
	char _offsetY;

	byte _maxOffsetX;
	byte _maxOffsetY;

	int _trellisSize;
	byte _trellisSizeX;
	byte _trellisSizeY;

	bool *_keys, *_lastKeys, *_leds, *_visible;

	int _trellisDelay;
	long _prevMillis;

};

#endif