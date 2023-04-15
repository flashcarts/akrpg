/*
 * 2008/06/18
 * @by bliss (bliss@hanirc.org)
 *
 * Copyright (C) 2008 Bliss. All rights reserved.
 * This code following BSD LICENSE.
 *
 */




#ifndef _SDSPEEDWND_H_
#define _SDSPEEDWND_H_

#include "form.h"
#include "formdesc.h"
#include "spinbox.h"
#include "statictext.h"
#include "message.h"
#include <string>

class cSdSpeedWnd : public akui::cForm
{

public:
    cSdSpeedWnd( s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & text );

    ~cSdSpeedWnd();

public:

    void draw();

    bool process( const akui::cMessage & msg );

    cWindow& loadAppearance(const std::string& aFileName );

protected:

	void onBase1() { _basePosition = 3; }
	void onBase2() { _basePosition = 2; }
	void onBase3() { _basePosition = 1; }
	void onBase4() { _basePosition = 0; }

protected:

    void onOK();

    void onCancel();

    void onReset();

    void onAuto();

    void onShow();

    void onUIKeyUP();

    void onUIKeyDOWN();

    void onUIKeyLEFT();

    void onUIKeyRIGHT();

    bool processKeyMessage( const akui::cKeyMessage & msg );

	void drawNumberText( u32 n);
	void drawNumber( u8 _position, u8 n );


    akui::cButton _buttonOK;
    akui::cButton _buttonCancel;

    akui::cButton _buttonAuto;
    akui::cButton _buttonReset;

    akui::cButton _buttonB4;
    akui::cButton _buttonB3;
    akui::cButton _buttonB2;
    akui::cButton _buttonB1;

    akui::cButton _buttonIncrease;
    akui::cButton _buttonDecrease;

    akui::cFormDesc _renderDesc;

	u8 _basePosition;
	int _sdSpeed;

    cBMP15 _numbers;

};

#endif//_SDSPEEDWND_H_
