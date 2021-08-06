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









/*****************************************************************

    TOPIC:读取当前目录下CIniFile类文件
    CREATE:2004-3-25

    UPDATE:2005-5-6 Add double support, m_bModified
    UPDATE:2005-5-16 Add m_bLastResult
    UPDATE:2005-8-22 Add comparion before writing


    AUTHOR:JUN LAI
*****************************************************************/

/*****************************************************************
INI文件格式

[Section1]
Item1=Value
Item2=Value1,Value2,Value3
Item3=Value

[Section2]
Item4=Value
Item5=Value
Item6=Value
*****************************************************************/

/*////////////////////////////////////////////
使用方法:
1.先把IniFile.h，IniFile.ccp加入Porject，在要使用该类的头文件中加入
    #include "IniFile.h"，及CIniFile IniFile;

2.ini文件名及路径在初始化时，用SetFileName()设置

3.读取和写入:
    string = IniFile.GetString("section","item","默认");
    int = IniFile.GetInt("section","item",666);

    IniFile.SetString("section","item","设置值");
    IniFile.SetInt("section","item",666);

4.最后用SaveIniFileModified()保存文件

注意事项:ini文件如果太长，读取速度会变慢，可以使用多个ini文件

////////////////////////////////////////////*/
// IniFile.h: interface for the CIniFile class.
//
///////////////////////////////////////////////

#ifndef _INIFILE_H_
#define _INIFILE_H_

#include <string>
#include <vector>

class CIniFile
{
public:
    CIniFile();
    CIniFile( const std::string & filename );
    virtual ~CIniFile();

public:

    //设置ini文件名
    void SetIniFile( const std::string & FileName){m_sFileName=FileName;};
    //获取ini文件名
    const std::string & GetIniFile(void){return m_sFileName;};

    //读ini文件
    bool LoadIniFile(const std::string & FileName );
    //写ini文件
    bool SaveIniFile(const std::string & FileName );
    bool SaveIniFileModified(const std::string & FileName );

    bool GetbLastResult(void){return m_bLastResult;}
    bool GetbModified(void){return m_bModified;}

    //读取CString变量
    std::string GetString(const std::string & Section, const std::string & Item);
    std::string GetString(const std::string & Section, const std::string & Item,
        const std::string & DefaultValue);
    //读取int变量
    int GetInt(const std::string & Section, const std::string & Item);
    int GetInt(const std::string & Section, const std::string & Item, int DefaultValue);
    //读取double变量
    double GetDouble(const std::string & Section, const std::string & Item);
    double GetDouble(const std::string & Section, const std::string & Item, double DefaultValue);
    //读取 string 组
    bool GetStringVector(const std::string & Section, const std::string & Item, std::vector< std::string > & strings, size_t size );
    //bool GetStringVector(const std::string & Section, const std::string & Item, std::vector< std::string > & strings, std::vector< std::string > & defaultStrings );

    //读取 int 组
    bool GetIntVector(const std::string & Section, const std::string & Item, std::vector< int > & numbers, size_t size );
    //bool GetIntVector(const std::string & Section, const std::string & Item, std::vector< int > & numbers, std::vector< int > & defaultNumbers );


    //写入CString变量
    void SetString(const std::string & Section, const std::string & Item, const std::string & Value);
    //写入int变量
    void SetInt(const std::string & Section, const std::string & Item, int Value);
    //写入double变量
    void SetDouble(const std::string & Section, const std::string & Item, double Value);
    ///写入CStringArray变量
    //void SetStringArray(const std::string & Section, const std::string & Item, CStringArray* pStringArray);


protected:
    //ini文件名
    std::string m_sFileName;

    //ini文件行数组
    typedef std::vector< std::string > cStringArray;
    cStringArray m_FileContainer;
//    CArray <CString,CString> FileContainer;

    //最后一次读取是否成功，true:成功
    bool m_bLastResult;
    //ini文件是否被修改
    bool m_bModified;

    bool InsertLine(size_t line,const std::string & str);
    bool ReplaceLine(size_t line,const std::string & str);


    //写入字符串变量
    void SetFileString(const std::string & Section, const std::string & Item, const std::string & Value);
    //读取字符串变量
    std::string GetFileString(const std::string & Section, const std::string & Item);

};

#endif // _INIFILE_H_
