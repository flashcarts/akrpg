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









#ifndef _ROMINFOWND_H_
#define _ROMINFOWND_H_

#include "form.h"
#include "formdesc.h"
#include "spinbox.h"
#include "statictext.h"
#include "message.h"
#include <string>
#include "dsrom.h"

class cRomInfoWnd : public akui::cForm
{
public:
    cRomInfoWnd( s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & text );

    ~cRomInfoWnd();

public:

    void draw();

    bool process( const akui::cMessage & msg );

    cWindow& loadAppearance(const std::string& aFileName );

    void setFileInfo( const std::string & shortName, const std::string & showName );

	void setFullPath( const std::string & longName );

    void setRomInfo( const DSRomInfo & romInfo );

    const DSRomInfo & getRomInfo();

protected:

    void setDiskInfo( const std::string & diskName );

    void showSaveType();

	void setShortcut();

	void setSlot2BootMode();

	int getSaveSlot();

	void copySaveSlot();

    bool processKeyMessage( const akui::cKeyMessage & msg );

    void onOK();

    void onShow();

    akui::cButton _buttonOK;

    akui::cButton _buttonSaveType;

	akui::cButton _buttonShortcut;

    akui::cFormDesc _renderDesc;

    DSRomInfo _romInfo;

    std::string _romInfoText;

    std::string _filenameText;

    std::string _fileDateText;

    std::string _fileSizeText;

    std::string _saveTypeText;

	std::string _shortName;

	std::string _longName;

    std::string _showName;

	int _slotNum;

	bool _isGBArom;
};




#endif//_ROMINFOWND_H_
