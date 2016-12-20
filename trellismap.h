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

class TrellisMap{

public:

	TrellisMap();
	~TrellisMap();

	bool begin(Adafruit_Trellis* _mat, byte _xMapSize, byte _yMapSize);
//	bool begin(Adafruit_TrellisSet* _mat, byte _xMapSize, byte _yMapSize, byte _xTrellisSize, byte _yTrellisSize);

	void setOffsetX(char _offset);
	char getOffsetX();

	void setOffsetY(char _offset);
	char getOffsetY();

	int getSize();
	int getSizeX();
	int getSizeY();

	bool readSwitches();
	bool isKeyPressed(byte _key);
	bool wasKeyPressed(byte _key);

	bool justPressed(byte _key);
	bool justReleased(byte _key);

	bool isTKeyPressed(byte _key);
	bool wasTKeyPressed(byte _key);

	bool justTPressed(byte _key);
	bool justTReleased(byte _key);

	void writeDisplay();
	void clear();

	bool isLED(byte _led);
	void setLED(byte _led);
	void clrLED(byte _led);


protected:

	byte _mapToTrellis(byte _key);
	byte _trellisToMap(byte _key);
	
	void _updateVisible();
	void _updateMap();

	bool set;

	Adafruit_Trellis *matrice;
//	Adafruit_TrellisSet *matrices;

	int size;
	byte sizeX;
	byte sizeY;

	char offsetX;
	char offsetY;

	byte maxOffsetX;
	byte maxOffsetY;

	int trellisSize;
	byte trellisSizeX;
	byte trellisSizeY;

	bool *keys, *lastKeys, *leds, *visible;

	int _trellisDelay;
	long _prevMillis;

};

#endif