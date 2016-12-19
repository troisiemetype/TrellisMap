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

/*
 * This library is for virtually extending the size of a trellis panel.
 * For example, you've built an interface using trellis for a music instrument,
 * and want to have more than the size of the physical panel.
 * The library create a matrix the size you want, stores led and switch states.
 * You can access states the same way then with original Adafruit Trellis library, but you can scroll through the matrix.
 * It's as if the Trellis panel was a window that you can move above your matrix.
 */


#include "trellismap.h"

//constructor. Initialize pointers.
TrellisMap::TrellisMap(){
	keys = NULL;
	lastKeys = NULL;
	leds = NULL;
	visible = NULL;

	matrice = NULL;
//	matrices = NULL;
}

//Destructor. free up state tables.
TrellisMap::~TrellisMap(){
	delete [] keys;
	delete [] lastKeys;
	delete [] leds;
	delete [] visible;

	delete matrice;
//	delete matrices;
}

//Start a new map with given size and one trellis panel.
//init all attributes, and set tables sizes.
//trellis is initialized in the main function init
bool TrellisMap::begin(Adafruit_Trellis* _mat, byte _xMapSize = 4, byte _yMapSize = 4){
	set = false;

	sizeX = _xMapSize;
	sizeY = _yMapSize;

	trellisSizeX = 4;
	trellisSizeY = 4;

	maxOffsetX = sizeX - trellisSizeX;
	maxOffsetY = sizeY - trellisSizeX;

	matrice = _mat;

	size = sizeX * sizeY;

	//if the tables are already set, delete them before to create with new values.
	if(keys != NULL) delete keys;
	if(lastKeys != NULL) delete lastKeys;
	if(leds != NULL) delete leds;
	if(visible != NULL) delete visible;

	//init tables with new size.
	keys = new bool[size];
	lastKeys = new bool[size];
	leds = new bool[size];
	visible = new bool[size];

	//set tables to zero.
	memset(keys, 0, size);
	memset(lastKeys, 0, size);
	memset(leds, 0, size);
	memset(visible, 0, size);

	_updateVisible();

	_trellisDelay = 15;
	_prevMillis = millis();

	return true;
}

/*
bool TrellisMap::begin(Adafruit_TrellisSet* _mat, byte _xMapSize, byte _yMapSize, byte _trellisSizeX, byte _trellisSizeY){
	set = true;
	if((_trellisSizeX%4 != 0) || (_trellisSizeY%4 != 0)){
		return false;
	}

	if(_xMapSize < _trellisSizeX){
		_xMapSize = _trellisSizeX;
	}

	if(_yMapSize < _trellisSizeY){
		_yMapSize = _trellisSizeY;
	}

	trellisSizeX = _trellisSizeX;
	trellisSizeY = _trellisSizeY;

	maxOffsetX = _xMapSize - trellisSizeX;
	maxOffsetY = _yMapSize - trellisSizeY;

	matrices = _mat;
	//TODO: see how to declare all the trellis pads, and hox to link them

	return true;
}
*/

//sets an offset. Increment in a direction or another
void TrellisMap::setOffsetX(char _offset){
	offsetX += _offset;
	if(offsetX < 0){
		offsetX = 0;
	}

	if(offsetX > maxOffsetX){
		offsetX = maxOffsetX;
	}

	Serial.print("setOffsetX");
	Serial.println((int)offsetX);

	_updateVisible();
	_updateMap();
	writeDisplay();
}

//Get the current offset / origin
char TrellisMap::getOffsetX(){
	return offsetX;
}

//sets an offset. Increment in a direction or another
void TrellisMap::setOffsetY(char _offset){
	offsetY += _offset;
	if(offsetY < 0){
		offsetY = 0;
	}

	if(offsetY > maxOffsetY){
		offsetY = maxOffsetY;
	}

	_updateVisible();
	_updateMap();
	writeDisplay();
}

//Get the current offset / origin
char TrellisMap::getOffsetY(){
	return offsetY;
}

//Get a reading from trellis.
//If a switch has changed, copy all the switches from it to the map table, with the right offsets.
bool TrellisMap::readSwitches(){
	//Todo: add a deboucne to Adafruit_Trellis library
	while((millis() - _prevMillis) < _trellisDelay){
		Serial.println("délai");
	}
	_prevMillis = millis();

	bool update = matrice->readSwitches();
	if(update){
		memcpy(lastKeys, keys, size);
		for (int j = 0; j < trellisSizeY; j++){
		int posY = (offsetY + j) * sizeX;

			for (int i = 0; i < trellisSizeX; i++){
				keys[posY + offsetX + i] = matrice->isKeyPressed(i + j * trellisSizeX);
			}
		}
	}
	return update;
}

//Get the current state of a key.
bool TrellisMap::isKeyPressed(byte _key){
	if(_key > size) return false;
	return keys[_key];
}

//Get the previous state of a key.
bool TrellisMap::wasKeyPressed(byte _key){
	if(_key > size) return false;
	return lastKeys[_key];
}

bool TrellisMap::justPressed(byte _key){
	if(_key > size) return false;
	return (isKeyPressed(_key) && !wasKeyPressed(_key));
}

bool TrellisMap::justReleased(byte _key){
	if(_key > size) return false;
	return (!isKeyPressed(_key) && wasKeyPressed(_key));
}

//Update the display
void TrellisMap::writeDisplay(){
	matrice->writeDisplay();
}

//Clear all the leds. Beware: write display has to be called in order to apply it.
void TrellisMap::clear(){
	memset(leds, 0, size);
	matrice->clear();
}

//Get the led state.
bool TrellisMap::isLED(byte _led){
	return leds[_led];
}

//Set a led of the map. Send to trellis if "visible".
void TrellisMap::setLED(byte _led){
	leds[_led] = true;
	if(visible[_led]){
		int posX = _led%sizeX - offsetX;
		int posY = _led/sizeX - offsetY;

		matrice->setLED(posX + posY * trellisSizeX);
	}
}

//Unset a led of the map. Send to trellis if "visible".
void TrellisMap::clrLED(byte _led){
	leds[_led] = false;
	if(visible[_led]){
		int posX = _led%sizeX - offsetX;
		int posY = _led/sizeX - offsetY;

		matrice->clrLED(posX + posY * trellisSizeX);
	}
}

//Update the visible table, that stores which led appears on trellis or not.
void TrellisMap::_updateVisible(){
	memset(visible, false, size);
	for (int j = 0; j < trellisSizeY; j++){
		int posY = (offsetY + j) * sizeX;

		for (int i = 0; i < trellisSizeX; i++){
			visible[posY + offsetX + i] = true;
		}
	}
}

//Update all the leds after an offset change
void TrellisMap::_updateMap(){
	for(int i = 0; i < size; i++){
		if(visible[i]){
			if(leds[i]){
				setLED(i);
			} else {
				clrLED(i);
			}
		}
	}
}