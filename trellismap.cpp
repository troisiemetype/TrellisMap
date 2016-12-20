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
	_keys = NULL;
	_lastKeys = NULL;
	_leds = NULL;
	_visible = NULL;

	_matrice = NULL;
//	matrices = NULL;
}

//Destructor. free up state tables.
TrellisMap::~TrellisMap(){
	delete [] _keys;
	delete [] _lastKeys;
	delete [] _leds;
	delete [] _visible;
//	delete matrices;
}

//Start a new map with given size and one trellis panel.
//init all attributes, and set tables sizes.
//trellis is initialized in the main function init
bool TrellisMap::begin(Adafruit_Trellis* mat, byte xMapSize = 4, byte yMapSize = 4){
	_set = false;

	_sizeX = xMapSize;
	_sizeY = yMapSize;

	_size = _sizeX * _sizeY;

	_trellisSizeX = 4;
	_trellisSizeY = 4;

	_trellisSize = _trellisSizeX * _trellisSizeY;

	_maxOffsetX = _sizeX - _trellisSizeX;
	_maxOffsetY = _sizeY - _trellisSizeX;

	_matrice = mat;


	//if the tables are already set, delete them before to create with new values.
	if(_keys != NULL) delete _keys;
	if(_lastKeys != NULL) delete _lastKeys;
	if(_leds != NULL) delete _leds;
	if(_visible != NULL) delete _visible;

	//init tables with new size.
	_keys = new bool[_size];
	_lastKeys = new bool[_size];
	_leds = new bool[_size];
	_visible = new bool[_size];

	//set tables to zero.
	memset(_keys, 0, _size);
	memset(_lastKeys, 0, _size);
	memset(_leds, 0, _size);
	memset(_visible, 0, _size);

	_updateVisible();

	_trellisDelay = 20;
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

	_maxOffsetX = _xMapSize - trellisSizeX;
	_maxOffsetY = _yMapSize - trellisSizeY;

	matrices = _mat;
	//TODO: see how to declare all the trellis pads, and hox to link them

	return true;
}
*/

//sets an offset. Increment in a direction or another
void TrellisMap::setOffsetX(char offset){
	_offsetX += offset;
	if(_offsetX < 0){
		_offsetX = 0;
	}

	if(_offsetX > _maxOffsetX){
		_offsetX = _maxOffsetX;
	}

	_updateVisible();
	_updateMap();
	writeDisplay();
}

//Get the current offset / origin
char TrellisMap::getOffsetX(){
	return _offsetX;
}

//sets an offset. Increment in a direction or another
void TrellisMap::setOffsetY(char offset){
	_offsetY += offset;
	if(_offsetY < 0){
		_offsetY = 0;
	}

	if(_offsetY > _maxOffsetY){
		_offsetY = _maxOffsetY;
	}

	_updateVisible();
	_updateMap();
	writeDisplay();
}

//Get the current offset / origin
char TrellisMap::getOffsetY(){
	return _offsetY;
}

//Get the size of the pannel
int TrellisMap::getSize(){
	return _size;
}

int TrellisMap::getSizeX(){
	return _sizeX;
}

int TrellisMap::getSizeY(){
	return _sizeY;
}

//Get a reading from trellis.
//If a switch has changed, copy all the switches from it to the map table, with the right offsets.
bool TrellisMap::readSwitches(){
	//Trellis needs at least a 20ms delay between two readings of its switches.
	//This is a kind of debounce. So if this delay has not been passed since last reading, return false immediately.
	//TODO: A real debounce would be nice, here or directly in the Adafruit_Trellis library.
	if((millis() - _prevMillis) < _trellisDelay) return false;

	bool update = _matrice->readSwitches();

	_prevMillis = millis();

	if(update){
		memcpy(_lastKeys, _keys, _size);
		
		for(int i = 0; i < _trellisSize; i++){
			_keys[_trellisToMap(i)] = _matrice->isKeyPressed(i);
		}
	}
	return update;
}

//Get the current state of a key.
bool TrellisMap::isKeyPressed(byte key){
	if(key > _size) return false;
	return _keys[key];
}

//Get the previous state of a key.
bool TrellisMap::wasKeyPressed(byte key){
	if(key > _size) return false;
	return _lastKeys[key];
}

bool TrellisMap::justPressed(byte key){
	if(key > _size) return false;
	return (isKeyPressed(key) && !wasKeyPressed(key));
}

bool TrellisMap::justReleased(byte key){
	if(key > _size) return false;
	return (!isKeyPressed(key) && wasKeyPressed(key));
}

//These four methods do the same as above, but for a key on trellis directly, not mapped.
bool TrellisMap::isTKeyPressed(byte key){
	if(key > _trellisSize) return false;
	return _keys[_trellisToMap(key)];
}

bool TrellisMap::wasTKeyPressed(byte key){
	if(key > _trellisSize) return false;
	return _lastKeys[_trellisToMap(key)];
}

bool TrellisMap::justTPressed(byte key){
	if(key > _trellisSize) return false;
	return (isTKeyPressed(key) && !wasTKeyPressed(key));
}

bool TrellisMap::justTReleased(byte key){
	if(key > _trellisSize) return false;
	return (!isTKeyPressed(key) && wasTKeyPressed(key));
}

//Update the display
void TrellisMap::writeDisplay(){
	_matrice->writeDisplay();
}

//Clear all the leds. Beware: write display has to be called in order to apply it.
void TrellisMap::clear(){
	memset(_leds, 0, _size);
	_matrice->clear();
}

//Get the led state.
bool TrellisMap::isLED(byte _led){
	return _leds[_led];
}

//Set a led of the map. Send to trellis if "visible".
void TrellisMap::setLED(byte led){
	_leds[led] = true;
	if(_visible[led]){
		_matrice->setLED(_mapToTrellis(led));
	}
}

//Unset a led of the map. Send to trellis if "visible".
void TrellisMap::clrLED(byte _led){
	_leds[_led] = false;
	if(_visible[_led]){
		_matrice->clrLED(_mapToTrellis(_led));
	}
}

//Update the visible table, that stores which led appears on trellis or not.
void TrellisMap::_updateVisible(){
	memset(_visible, false, _size);

	for(int i = 0; i < _trellisSize; i++){
			_visible[_trellisToMap(i)] = true;
	}
}

//Update all the leds after an offset change
void TrellisMap::_updateMap(){
	for(int i = 0; i < _size; i++){
		if(_visible[i]){
			if(_leds[i]){
				setLED(i);
			} else {
				clrLED(i);
			}
		}
	}
}

//Get the position on trellis, given the position on map.
byte TrellisMap::_mapToTrellis(byte key){
	int posX = key%_sizeX - _offsetX;
	int posY = key/_sizeX - _offsetY;
	return posX + posY * _trellisSizeX;
}

//Get the position on map, given the position on trellis.
byte TrellisMap::_trellisToMap(byte key){
	int posX = key%_trellisSizeX;
	int posY = key/_trellisSizeX;
	return (_offsetX + posX + ((_offsetY + posY) * _sizeX));
}
