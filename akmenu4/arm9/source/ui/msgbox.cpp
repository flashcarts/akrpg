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









#include "ui.h"
#include "msgbox.h"
#include "fontfactory.h"
#include "language.h"

namespace akui {

cMessageBox::cMessageBox( s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & title, const std::string & msg, u32 style )
            : cForm( x, y, w, h, parent, title )
{
    u32 largestLineWidth = 0;
    size_t pos1 = 0;
    size_t pos2 = 0;
    size_t lineCount = 1;

    std::string breakedMsg = font().breakLine( msg, 192 );

    pos1 = breakedMsg.find( '\n' );
    while( breakedMsg.npos != pos1 )  {
        ++lineCount;
        u32 lineWidth = font().getStringScreenWidth( &breakedMsg[pos2], pos1 - pos2 );
        if( largestLineWidth < lineWidth )
            largestLineWidth = lineWidth;
        pos2 = pos1;
        pos1 = breakedMsg.find( '\n', pos2 + 1 );
        dbg_printf("line w %d\n", lineWidth );
    }

    if( 1 == lineCount ) {
        largestLineWidth = font().getStringScreenWidth( &breakedMsg[0], breakedMsg.length() );
    }

    _size.x = largestLineWidth + 48;
    if( _size.x < 192 )
        _size.x = 192;
    if( _size.x > 256 )
        _size.x = 256;
    if( _size.y > 192 )
        _size.y = 192;
    if( _size.x & 1 ) --_size.x;   // 4 byte align, for speed optimization
    _size.y = lineCount * gs().fontHeight + 60;
    _position.x = (SCREEN_WIDTH - _size.x) / 2;
    if( _position.x & 1 ) --_position.x;
    _position.y = (SCREEN_HEIGHT - _size.y) / 2;
    _textPoision.x = _position.x + (_size.x - largestLineWidth ) / 2;
    _textPoision.y = _position.y + 24;
    dbg_printf("_size.x %d largestLineWidth %d\n", _size.x , largestLineWidth );

    _text = title;
    _msg = breakedMsg;
    _style = style;
    //_msgRet = -1;
    _buttonOK = NULL;
    _buttonCANCEL = NULL;
    _buttonYES = NULL;
    _buttonNO = NULL;

    _buttonOK = new cButton( 0, 0, 46, 18, this, "\x01 OK" );
    _buttonOK->setText( "\x01 " + LANG("message box", "ok" ) );
    _buttonOK->setStyle( cButton::press );
    _buttonOK->hide();

    _buttonOK->loadAppearance( SFN_BUTTON3 );
    _buttonOK->setStyle( cButton::press );
    _buttonOK->clicked.connect( this, &cMessageBox::onOK );
    addChildWindow( _buttonOK );

    _buttonCANCEL = new cButton( 0, 0, 46, 18, this, "\x02 Cancel" );
    _buttonCANCEL->setText( "\x02 " + LANG("message box", "cancel") );
    _buttonCANCEL->setStyle( cButton::press );
    _buttonCANCEL->hide();
    _buttonCANCEL->loadAppearance( SFN_BUTTON3 );
    _buttonCANCEL->clicked.connect( this, &cMessageBox::onCANCEL );
    addChildWindow( _buttonCANCEL );

    _buttonYES = new cButton( 0, 0, 46, 18, this, "\x01 Yes" );
    _buttonYES->setText( "\x01 " + LANG("message box", "yes") );
    _buttonYES->setStyle( cButton::press );
    _buttonYES->hide();
    _buttonYES->loadAppearance( SFN_BUTTON3 );
    _buttonYES->clicked.connect( this, &cMessageBox::onOK );
    addChildWindow( _buttonYES );

    _buttonNO = new cButton( 0, 0, 46, 18, this, "\x02 No" );
    _buttonNO->setText( "\x02 " + LANG("message box", "no") );
    _buttonNO->setStyle( cButton::press );
    _buttonNO->hide();
    //_buttonNO->setTextColor( RGB15(20,14,0) );
    _buttonNO->loadAppearance( SFN_BUTTON3 );
    _buttonNO->clicked.connect( this, &cMessageBox::onCANCEL );
    addChildWindow( _buttonNO );

    s16 nextButtonX = size().x;
    s16 buttonPitch = 60;
    s16 buttonY = size().y - _buttonNO->size().y - 4;
    // ��һ��Ҫ���İ�ť��λ��
    if( _style & MB_NO ) {
        // ��nextButtonXλ�û� NO ��ť
        // nextButtonX -= ��ť��� + �հ������
        buttonPitch = _buttonNO->size().x + 8;
        nextButtonX -= buttonPitch;
        _buttonNO->setRelativePosition( cPoint(nextButtonX, buttonY) );
        _buttonNO->show();
    }


    if( _style & MB_YES ) {
        // ��nextButtonXλ�û� YES ��ť
        // nextButtonX -= ��ť��� + �հ������
        buttonPitch = _buttonYES->size().x + 8;
        nextButtonX -= buttonPitch;
        _buttonYES->setRelativePosition( cPoint(nextButtonX, buttonY) );
        _buttonYES->show();
    }


    if( _style & MB_CANCEL ) {
        // ��nextButtonXλ�û� CANCEL ��ť
        // nextButtonX -= ��ť��� + �հ������
        buttonPitch = _buttonCANCEL->size().x + 8;
        nextButtonX -= buttonPitch;
        _buttonCANCEL->setRelativePosition( cPoint(nextButtonX, buttonY) );
        _buttonCANCEL->show();
    }

    if( _style & MB_OK ) {
        // ��nextButtonXλ�û� OK ��ť
        // nextButtonX -= ��ť��� + �հ������
        buttonPitch = _buttonOK->size().x + 8;
        nextButtonX -= buttonPitch;
        _buttonOK->setRelativePosition( cPoint(nextButtonX, buttonY) );
        _buttonOK->show();
    }

    arrangeChildren();

    loadAppearance( "" );
}

cMessageBox::~cMessageBox()
{
    delete _buttonOK;
    delete _buttonCANCEL;
    delete _buttonYES;
    delete _buttonNO;
}

void cMessageBox::onOK()
{
    _modalRet = ID_OK;
}

void cMessageBox::onCANCEL()
{
    _modalRet = ID_CANCEL;
}

bool cMessageBox::process( const cMessage & msg )
{
    bool ret = false;
    if(isVisible() && isEnabled()) {
        ret = cForm::process( msg );
        if( !ret ) {
            if( msg.id() > cMessage::keyMessageStart
                && msg.id() < cMessage::keyMessageEnd ) {
                ret = processKeyMessage( (cKeyMessage &)msg );
            }
            else if( msg.id() > cMessage::touchMessageStart
                && msg.id() < cMessage::touchMessageEnd ) {
                ret = processTouchMessage( (cTouchMessage &)msg );
            }
        }
    }

    return ret;
}

bool cMessageBox::processKeyMessage( const cKeyMessage & msg )
{
    bool ret = false;
    if( msg.id() == cMessage::keyDown ) {
        switch( msg.keyCode() )
        {
        case cKeyMessage::UI_KEY_A:
            onOK();
            ret = true;
            return true;
            break;
        case cKeyMessage::UI_KEY_B:
            onCANCEL();
            ret = true;
            return true;
            break;
        }
    }
    return ret;
}

bool cMessageBox::processTouchMessage( const cTouchMessage & msg )
{
    return false;
}

void cMessageBox::draw()
{
    _renderDesc.draw( windowRectangle(), _engine );
    cForm::draw();

    // draw message text
    gdi().setPenColor( uiSettings().formTextColor, _engine );
    gdi().textOut( _textPoision.x, _textPoision.y, _msg.c_str(), _engine );
}

cWindow& cMessageBox::loadAppearance(const std::string& aFileName )
{
    _renderDesc.loadData(
        SFN_FORM_TITLE_L, SFN_FORM_TITLE_R, SFN_FORM_TITLE_M );

    _renderDesc.setTitleText( _text );
    return *this;
}

u32 messageBox( cWindow * parent, const std::string & title, const std::string & msg, u32 style )
{
    // check point ���������ֵĶԻ�����ʧ����ͼ������
    cMessageBox msgbox( 12, 36, 232, 120, parent, title, msg, style );
    //cMessageBox msgbox( 0, 0, 256, 192, parent, text, style );

    return msgbox.doModal();

}



}
