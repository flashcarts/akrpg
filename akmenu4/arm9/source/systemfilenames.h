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









#ifndef _SYSTEMFILENAMES_H_
#define _SYSTEMFILENAMES_H_

#define SFN_SYSTEM_DIR                "fat0:/__rpg/"
#define SFN_OFFICIAL_SAVELIST        SFN_SYSTEM_DIR"savelist.bin"
#define SFN_CUSTOM_SAVELIST            SFN_SYSTEM_DIR"savelistex.bin"
#define SFN_LAST_SAVEINFO            SFN_SYSTEM_DIR"lastsave.ini"

#define SFN_GLOBAL_SETTINGS            SFN_SYSTEM_DIR"globalsettings.ini"

#define SFN_UI_DIRECTORY            SFN_SYSTEM_DIR"ui/"
#define SFN_USER_CUSTOM             SFN_UI_DIRECTORY + gs().uiName + "/custom.ini"
#define SFN_UI_SETTINGS                SFN_UI_DIRECTORY + gs().uiName + "/uisettings.ini"
#define SFN_UPPER_SCREEN_BG            SFN_UI_DIRECTORY + gs().uiName + "/upper_screen.bmp"
#define SFN_LOWER_SCREEN_BG            SFN_UI_DIRECTORY + gs().uiName + "/lower_screen.bmp"
#define SFN_FORM_TITLE_L            SFN_UI_DIRECTORY + gs().uiName + "/title_left.bmp"
#define SFN_FORM_TITLE_M            SFN_UI_DIRECTORY + gs().uiName + "/title_bg.bmp"
#define SFN_FORM_TITLE_R            SFN_UI_DIRECTORY + gs().uiName + "/title_right.bmp"
#define SFN_BUTTON2                    SFN_UI_DIRECTORY + gs().uiName + "/btn2.bmp"
#define SFN_BUTTON3                    SFN_UI_DIRECTORY + gs().uiName + "/btn3.bmp"
#define SFN_BUTTON4                    SFN_UI_DIRECTORY + gs().uiName + "/btn4.bmp"
#define SFN_SPINBUTTON_L            SFN_UI_DIRECTORY + gs().uiName + "/spin_btn_left.bmp"
#define SFN_SPINBUTTON_R            SFN_UI_DIRECTORY + gs().uiName + "/spin_btn_right.bmp"
#define SFN_BRIGHTNESS_BUTTON        SFN_UI_DIRECTORY + gs().uiName + "/brightness.bmp"
#define SFN_FOLDERUP_BUTTON            SFN_UI_DIRECTORY + gs().uiName + "/folder_up.bmp"
#define SFN_STARTMENU_BG            SFN_UI_DIRECTORY + gs().uiName + "/menu_bg.bmp"
#define SFN_CLOCK_NUMBERS            SFN_UI_DIRECTORY + gs().uiName + "/calendar/clock_numbers.bmp"
#define SFN_CLOCK_COLON                SFN_UI_DIRECTORY + gs().uiName + "/calendar/clock_colon.bmp"
#define SFN_DAY_NUMBERS                SFN_UI_DIRECTORY + gs().uiName + "/calendar/day_numbers.bmp"
#define SFN_YEAR_NUMBERS            SFN_UI_DIRECTORY + gs().uiName + "/calendar/year_numbers.bmp"
#define SFN_CARD_ICON_BLUE            SFN_UI_DIRECTORY + gs().uiName + "/card_icon_blue.bmp"
#define SFN_PROGRESS_WND_BG            SFN_UI_DIRECTORY + gs().uiName + "/progress_wnd.bmp"
#define SFN_PROGRESS_BAR_BG            SFN_UI_DIRECTORY + gs().uiName + "/progress_bar.bmp"

#define SFN_LANGUAGE_DIRECTORY      SFN_SYSTEM_DIR"language/"
#define SFN_LANGUAGE_TEXT            SFN_LANGUAGE_DIRECTORY + gs().langDirectory + "/language.txt"
#define SFN_LOCAL_FONT_FILE            SFN_LANGUAGE_DIRECTORY + gs().langDirectory + "/local.fon"
#define SFN_LOCAL_TO_UNICODE        SFN_LANGUAGE_DIRECTORY + gs().langDirectory + "/unicode.l2u"
#define SFN_UNICODE_TO_LOCAL        SFN_LANGUAGE_DIRECTORY + gs().langDirectory + "/unicode.u2l"
#define SFN_LOCAL_ANK_FILE            SFN_LANGUAGE_DIRECTORY + gs().langDirectory + "/unicode.ank"

#define SFN_SDCARD_LIST                SFN_SYSTEM_DIR"sdlist.ini"

#endif//_SYSTEMFILENAMES_H_
