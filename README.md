#Trellis Map
##This library is intended to create a map around an Adafruit Trellis pad.

The Trellis is a little board that is composed of 4x4 soft switches that can be individualy pushed, and lit.

This library can be used to create an interface for game, music, etc. using one or several Trellis, and a virtual matrix that you can navigate in.

In other words, you can have just a 4x4 physical matrix, and use this library to manage a virtual 16x16 matrix that you navigate in using buttons, potentiometer, encoders, or whatever mean you want.


It basically reimplement the Adafruit Trellis library methods, so all you have to do is instanciate the class in your sketch, and call the same method on TrellisMap instead of Trellis.

It adds methods to set offsets in both directions, and to get the values of these ofsets if needed.

Create Trellis objects at the beginning of your sketch, like you would usualy do.
Create a TrellisMap object.
In setup, call `begin()` on the Trellis object, liked you would have done.
In setup, call `begin()` on the TrellisMap object, and pass the address of the Trellis object.

Then use it exactly the same has you would have with Trellis.

Trellis' methods can be called if you need to, but TrellisMap won't take these modifications into account.

`setOffset(char offset)` is used to offset the display of the map. Give the value, positive or negative, you want to move the display from the actual position.

`getOffset()` returns the value of the current offset in absolute coordinates.

Take a look at the Trellis library to learn more about it! https://github.com/adafruit/Adafruit_Trellis_Library

You are free to use this library for any project you want. Be kind enough to let me know if you built something with it!