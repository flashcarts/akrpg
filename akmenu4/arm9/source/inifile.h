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

    TOPIC:��ȡ��ǰĿ¼��CIniFile���ļ�
    CREATE:2004-3-25

    UPDATE:2005-5-6 Add double support, m_bModified
    UPDATE:2005-5-16 Add m_bLastResult
    UPDATE:2005-8-22 Add comparion before writing


    AUTHOR:JUN LAI
*****************************************************************/

/*****************************************************************
INI�ļ���ʽ

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
ʹ�÷���:
1.�Ȱ�IniFile.h��IniFile.ccp����Porject����Ҫʹ�ø����ͷ�ļ��м���
    #include "IniFile.h"����CIniFile IniFile;

2.ini�ļ�����·���ڳ�ʼ��ʱ����SetFileName()����

3.��ȡ��д��:
    string = IniFile.GetString("section","item","Ĭ��");
    int = IniFile.GetInt("section","item",666);

    IniFile.SetString("section","item","����ֵ");
    IniFile.SetInt("section","item",666);

4.�����SaveIniFileModified()�����ļ�

ע������:ini�ļ����̫������ȡ�ٶȻ����������ʹ�ö��ini�ļ�

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

    //����ini�ļ���
    void SetIniFile( const std::string & FileName){m_sFileName=FileName;};
    //��ȡini�ļ���
    const std::string & GetIniFile(void){return m_sFileName;};

    //��ini�ļ�
    bool LoadIniFile(const std::string & FileName );
    //дini�ļ�
    bool SaveIniFile(const std::string & FileName );
    bool SaveIniFileModified(const std::string & FileName );

    bool GetbLastResult(void){return m_bLastResult;}
    bool GetbModified(void){return m_bModified;}

    //��ȡCString����
    std::string GetString(const std::string & Section, const std::string & Item);
    std::string GetString(const std::string & Section, const std::string & Item,
        const std::string & DefaultValue);
    //��ȡint����
    int GetInt(const std::string & Section, const std::string & Item);
    int GetInt(const std::string & Section, const std::string & Item, int DefaultValue);
    //��ȡdouble����
    double GetDouble(const std::string & Section, const std::string & Item);
    double GetDouble(const std::string & Section, const std::string & Item, double DefaultValue);
    //��ȡ string ��
    bool GetStringVector(const std::string & Section, const std::string & Item, std::vector< std::string > & strings, size_t size );
    //bool GetStringVector(const std::string & Section, const std::string & Item, std::vector< std::string > & strings, std::vector< std::string > & defaultStrings );

    //��ȡ int ��
    bool GetIntVector(const std::string & Section, const std::string & Item, std::vector< int > & numbers, size_t size );
    //bool GetIntVector(const std::string & Section, const std::string & Item, std::vector< int > & numbers, std::vector< int > & defaultNumbers );


    //д��CString����
    void SetString(const std::string & Section, const std::string & Item, const std::string & Value);
    //д��int����
    void SetInt(const std::string & Section, const std::string & Item, int Value);
    //д��double����
    void SetDouble(const std::string & Section, const std::string & Item, double Value);
    ///д��CStringArray����
    //void SetStringArray(const std::string & Section, const std::string & Item, CStringArray* pStringArray);


protected:
    //ini�ļ���
    std::string m_sFileName;

    //ini�ļ�������
    typedef std::vector< std::string > cStringArray;
    cStringArray m_FileContainer;
//    CArray <CString,CString> FileContainer;

    //���һ�ζ�ȡ�Ƿ�ɹ���true:�ɹ�
    bool m_bLastResult;
    //ini�ļ��Ƿ��޸�
    bool m_bModified;

    bool InsertLine(size_t line,const std::string & str);
    bool ReplaceLine(size_t line,const std::string & str);


    //д���ַ�������
    void SetFileString(const std::string & Section, const std::string & Item, const std::string & Value);
    //��ȡ�ַ�������
    std::string GetFileString(const std::string & Section, const std::string & Item);

};

#endif // _INIFILE_H_
