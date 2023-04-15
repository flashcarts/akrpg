/*---------------------------------------------------------------------------------


Copyright (C) 2007 Acekard, www.acekard.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.


---------------------------------------------------------------------------------*/









#ifndef _AKUI_KEYMESSAGE_H_
#define _AKUI_KEYMESSAGE_H_

#include "message.h"


namespace akui {

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//! Message caused by interevent with the keyboard
class cKeyMessage : public cMessage
{
    // ---------------------------------------------------------------------------------------------
    //  Construction / Destruction
    // ---------------------------------------------------------------------------------------------
public:
    //! constructor
    cKeyMessage(Id anId, unsigned char aKeyCode) :
        cMessage( anId ),
        _keyCode( aKeyCode ) {}

    //! destructor
    virtual ~cKeyMessage() {}


    // ---------------------------------------------------------------------------------------------
    //  Accessors
    // ---------------------------------------------------------------------------------------------
public:
    //! returns the code of the key pressed in a keyPressed message
    unsigned short keyCode() const { return _keyCode; }


    // ---------------------------------------------------------------------------------------------
    //  Implementation
    // ---------------------------------------------------------------------------------------------
public:
  static const unsigned char UI_KEY_A            =    1;    //!< Keypad A button.
  static const unsigned char UI_KEY_B            =    2;    //!< Keypad B button.
  static const unsigned char UI_KEY_SELECT        =    3;  //!< Keypad SELECT button.
  static const unsigned char UI_KEY_START        =    4;    //!< Keypad START button.
  static const unsigned char UI_KEY_RIGHT        =    5;    //!< Keypad RIGHT button.
  static const unsigned char UI_KEY_LEFT        =    6;    //!< Keypad LEFT button.
  static const unsigned char UI_KEY_UP            =    7;  //!< Keypad UP button.
  static const unsigned char UI_KEY_DOWN        =    8;    //!< Keypad DOWN button.
  static const unsigned char UI_KEY_R            =    9;    //!< Right shoulder button.
  static const unsigned char UI_KEY_L            =    10;   //!< Left shoulder button.
  static const unsigned char UI_KEY_X            =    11;  //!< Keypad X button.
  static const unsigned char UI_KEY_Y            =    12;  //!< Keypad Y button.
  static const unsigned char UI_KEY_TOUCH        =    13;  //!< Touchscreen pendown.
  static const unsigned char UI_KEY_LID            =    14;  //!< Lid state.

protected:
    // the code of the key pressed in a keyPressed message
    unsigned char _keyCode;
};

//const unsigned char cKeyMessage::UI_KEY_A            =    1;  //!< Keypad A button.
//const unsigned char cKeyMessage::UI_KEY_B            =    2;  //!< Keypad B button.
//const unsigned char cKeyMessage::UI_KEY_SELECT        =    3;  //!< Keypad SELECT button.
//const unsigned char cKeyMessage::UI_KEY_START        =    4;  //!< Keypad START button.
//const unsigned char cKeyMessage::UI_KEY_RIGHT        =    5;  //!< Keypad RIGHT button.
//const unsigned char cKeyMessage::UI_KEY_LEFT        =    6;  //!< Keypad LEFT button.
//const unsigned char cKeyMessage::UI_KEY_UP            =    7;  //!< Keypad UP button.
//const unsigned char cKeyMessage::UI_KEY_DOWN        =    8;  //!< Keypad DOWN button.
//const unsigned char cKeyMessage::UI_KEY_R            =    9;  //!< Right shoulder button.
//const unsigned char cKeyMessage::UI_KEY_L            =    10; //!< Left shoulder button.
//const unsigned char cKeyMessage::UI_KEY_X            =    11; //!< Keypad X button.
//const unsigned char cKeyMessage::UI_KEY_Y            =    12; //!< Keypad Y button.
//const unsigned char cKeyMessage::UI_KEY_TOUCH        =    13; //!< Touchscreen pendown.
//const unsigned char cKeyMessage::UI_KEY_LID            =    14; //!< Lid state.


} // namespace akui


#endif//_AKUI_cKeyMessage_H_
