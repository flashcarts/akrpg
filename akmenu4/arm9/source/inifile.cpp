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









// IniFile.cpp: implementation of the CIniFile class.
//
///////////////////////////////////////////////

#include <cstdio>
#include <cstdlib>
#include "inifile.h"
#include "../../share/stringtool.h"
#include "dbgtool.h"

static bool freadLine( FILE * f, std::string & str )
{
    str.clear();
__read:
    char p = 0;

    size_t readed = fread( &p, 1, 1, f );
    if( 0 == readed ) {
        str = "";
        return false;
    }
    if( '\n' == p || '\r' == p ) {
        str = "";
        return true;
    }

    while( p != '\n' && p != '\r' && readed )
    {
        str += p;
        readed = fread( &p, 1, 1, f );
    }

    if( str.empty() || "" == str ) {
        goto __read;
    }

    return true;
}

static void trimStringLeft( std::string & str )
{
    size_t notSpace = str.find_first_not_of( ' ' );
    if( str.npos == notSpace ) {
        size_t notTab = str.find_first_not_of( '\t' );
        if( str.npos == notTab )
            return;
        str = str.substr( notTab );
    }
    str = str.substr( notSpace );
}

static void trimStringRight( std::string & str )
{
    size_t notSpace = str.find_last_not_of( ' ' );
    if( str.npos == notSpace || str.size() - 1 == notSpace )
    {
        size_t notTab = str.find_last_not_of( '\t' );
        if( str.npos == notTab || str.size() - 1 == notTab )
            return;
        str = str.substr( 0, notTab + 1 );
    } else {
        str = str.substr( 0, notSpace + 1 );
    }

}

///////////////////////////////////////////////
// Construction/Destruction
///////////////////////////////////////////////

CIniFile::CIniFile()
{
    m_sFileName = "Config.ini";
    m_bLastResult = false;
    m_bModified = false;
}

CIniFile::CIniFile( const std::string & filename )
{
    m_sFileName = filename;
    m_bLastResult = false;
    m_bModified = false;
    LoadIniFile( m_sFileName );
}

CIniFile::~CIniFile()
{
    if(m_FileContainer.size() > 0)
    {
        m_FileContainer.clear();
    }
}

void CIniFile::SetString(const std::string & Section, const std::string & Item, const std::string & Value)
{
    if(GetFileString(Section,Item)!=Value)
    {
        //保存到m_FileContainer
        SetFileString(Section, Item, Value);
        m_bModified = true;
    }

    //保存到ini文件
//    SaveIniFile();
}

//void CIniFile::SetStringArray(const std::string & Section, const std::string & Item, cStringArray* pStringArray)
//{
//    const std::string & strValue=_T("");
//
//    if(pStringArray->GetSize()>=1)
//    {
//        strValue=pStringArray->GetAt(0);
//        for(int i=1;i<pStringArray->GetSize();i++)
//        {
//            strValue=','+pStringArray->GetAt(i);
//        }
//    }
//
//    if(GetFileString(Section,Item)!=strValue)
//    {
//        SetFileString(Section, Item, strValue);
//        m_bModified = true;
//    }
//}


void CIniFile::SetInt(const std::string & Section, const std::string & Item, int Value)
{
    std::string strtemp = formatString("%d",Value);

    if(GetFileString(Section,Item)!=strtemp)
    {
        //保存到m_FileContainer
        SetFileString(Section, Item, strtemp);
        m_bModified = true;
    }

    //保存到ini文件
//    SaveIniFile();
}

void CIniFile::SetDouble(const std::string & Section, const std::string & Item, double Value)
{
    std::string strtemp = formatString("%f",Value);

    //保存到m_FileContainer
    if(GetFileString(Section,Item)!=strtemp)
    {
        SetFileString(Section, Item, strtemp);
        m_bModified = true;
    }

    //保存到ini文件
//    SaveIniFile();
}

std::string CIniFile::GetString(const std::string & Section, const std::string & Item)
{
    return GetFileString(Section, Item);
}

std::string CIniFile::GetString(const std::string & Section, const std::string & Item, const std::string & DefaultValue)
{
    std::string temp = GetString(Section,Item);
    if(m_bLastResult==false)
    {
        SetString(Section,Item,DefaultValue);
        temp = DefaultValue;
    }

    return temp;
}

bool CIniFile::GetStringVector(const std::string & Section, const std::string & Item, std::vector< std::string > & strings, size_t size )
{
    const std::string & strValue=GetFileString(Section, Item);
    strings.clear();//->RemoveAll();

    size_t lastCommaPos = 0;
    size_t commaPos = strValue.find(',');
    while( strValue.npos != commaPos )
    {
        //pStringArray->Add(strValue.Left(strValue.Find(',')));
        if( commaPos > lastCommaPos )
            strings.push_back( strValue.substr( lastCommaPos, commaPos - lastCommaPos ) );
        //strValue=strValue.Right(strValue.GetLength()-strValue.Find(',')-1);
        lastCommaPos = commaPos;
        commaPos = strValue.find( ',', commaPos+1 );
    };
    while( strings.size() < size )
    {
        strings.push_back("");
    }
    return true;

}

//bool CIniFile::GetStringVector(const std::string & Section, const std::string & Item, std::vector< std::string > & strings, std::vector< std::string > & defaultStrings )
//{
//    GetStringVector(Section,Item,strings);
//    if(m_bLastResult==false)
//    {
//        //SetStringVector(Section,Item,defaultStrings);
//        //GetStringVector(Section,Item,strings);
//        return false;
//    }
//
//    return true;
//}

bool CIniFile::GetIntVector(const std::string & Section, const std::string & Item, std::vector< int > & numbers, size_t size )
{
    std::vector< std::string > strings;
    if( !GetStringVector( Section, Item, strings, size ) )
        return false;

    numbers.clear();
    for( size_t i = 0; i < strings.size(); ++i ) {
        numbers.push_back( atoi( strings[i].c_str() ) );
    }

    return true;
}


int CIniFile::GetInt(const std::string & Section, const std::string & Item)
{
    std::string value = GetFileString(Section, Item);
    if( value.size() > 2 && '0' == value[0] && ('x'==value[1] || 'X' == value[1] ) )
        return strtol( value.c_str(), NULL, 16 );
    else
        return strtol( value.c_str(), NULL, 10 );
}

int CIniFile::GetInt(const std::string & Section, const std::string & Item, int DefaultValue)
{
    int temp;
    temp = GetInt(Section,Item);
    if(m_bLastResult==false)
    {
        SetInt(Section,Item,DefaultValue);
        temp = DefaultValue;
    }

    return temp;
}

double CIniFile::GetDouble(const std::string & Section, const std::string & Item)
{
    char * stopString = NULL;
    return strtod(GetFileString(Section, Item).c_str(), &stopString);
}

double CIniFile::GetDouble(const std::string & Section, const std::string & Item, double DefaultValue)
{
    double temp;
    temp = GetDouble(Section,Item);
    if(m_bLastResult==false)
    {
        SetDouble(Section,Item,DefaultValue);
        temp = DefaultValue;
    }

    return temp;
}

bool CIniFile::LoadIniFile(const std::string & FileName)
{
    dbg_printf("load %s\n", FileName.c_str() );
    if(FileName!="")
        m_sFileName = FileName;

    FILE * f = fopen( FileName.c_str(), "rb" );

    if( NULL == f )
        return false;

    std::string strline("");
    m_FileContainer.clear();

    //将IniFile文件数据读到m_FileContainer
    //while(m_IniFile.ReadString(strline))
    while( freadLine( f, strline) )
    {
        trimStringLeft( strline );
        trimStringRight( strline );
        if( strline!="" && ';' != strline[0] && '/' != strline[0] && '!' != strline[0] )
            m_FileContainer.push_back(strline);
    }

    //m_IniFile.Close();
    fclose( f );

    m_bLastResult = false;
    m_bModified = false;

    //cwl();
    return true;
}

bool CIniFile::SaveIniFileModified(const std::string & FileName)
{
    if(m_bModified==true)
    {
        return SaveIniFile(FileName);
    }

    return true;
}

bool CIniFile::SaveIniFile(const std::string & FileName)
{
    //CStdioFile m_IniFile;

    if(FileName!="")
        m_sFileName = FileName;

    //if(!m_IniFile.Open(m_sFileName, CFile::modeCreate | CFile::modeWrite))
    FILE * f = fopen( m_sFileName.c_str(), "wb" );
    if( NULL == f )
        return false;


    //将m_FileContainer写到IniFile文件
    for(size_t i = 0; i< m_FileContainer.size(); i++)
    {
        std::string & strline = m_FileContainer[i];
        //strline.TrimLeft();
        size_t notSpace = strline.find_first_not_of( ' ' );
        strline = strline.substr( notSpace );
        if( strline.find('[')==0 && i > 0 )
        {
            if( !m_FileContainer[i-1].empty() && m_FileContainer[i-1] != "" )
                //m_IniFile.WriteString("\n");
                fwrite( "\r\n", 1, 2, f );
        }
        //m_IniFile.WriteString(strline+"\n");
        if( !strline.empty() && strline != "" ) {
            fwrite( strline.c_str(), 1, strline.length(), f );
            fwrite( "\r\n", 1, 2, f );
        }
    }

    //m_IniFile.Close();
    fclose( f );

    m_bModified = false;

    return true;
}

std::string CIniFile::GetFileString(const std::string & Section, const std::string & Item)
{
    std::string strline;
    std::string strSection;
    std::string strItem;
    std::string strValue;

    size_t i = 0;
    size_t iFileLines = m_FileContainer.size();

    if( iFileLines < 0)
    {
        m_bLastResult=false;
        return std::string("");//文件打开出错或文件为空，返回_T("")
    }

    while(i<iFileLines)
    {
        strline = m_FileContainer[i++];
        //strline.TrimLeft();
        //trimStringLeft( strline );

        size_t rBracketPos = strline.find(']');
        bool bracketClosed =  '[' == strline[0]
                                && rBracketPos > 0
                                    && rBracketPos != std::string::npos;
        if( bracketClosed )//查找Section，第一个必须为[
        {
            //读取Section名
            //strSection=strline.Mid(1, strline.Find(']')-1);
            strSection = strline.substr( 1, rBracketPos - 1 );
            //strSection.TrimLeft();
            //strSection.TrimRight();
            if( strSection == Section )//找到Section
            {
                while(i<iFileLines)
                {
                    strline = m_FileContainer[i++];
                    //strline.TrimLeft();
                    trimStringLeft( strline );
                    size_t equalsignPos = strline.find( '=' );
                    if( equalsignPos != strline.npos && equalsignPos > 0 && equalsignPos < strline.size() - 1 )
                    {
                        //读取Item名
                        //strItem = strline.Left(strline.Find('='));//去掉=右边
                        strItem = strline.substr( 0, equalsignPos );
                        //strItem.TrimLeft();
                        //strItem.TrimRight();
                        trimStringRight( strItem );
                        if(strItem == Item)//找到Item,返回Value
                        {
                            //读取Value名
                            //strValue=strline.Right(strline.GetLength()-strline.Find('=')-1);//去掉=左边
                            strValue = strline.substr( equalsignPos + 1 );
                            //strValue.TrimLeft();
                            trimStringLeft( strValue );
                            //strValue.TrimRight();
                            //trimStringRight( strValue );
                            m_bLastResult=true;
                            return strValue;
                        }
                    }
                    //else if('[' == strline[0]('[')==0)
                    else if('[' == strline[0] )
                    {
                        m_bLastResult=false;
                        return std::string("");//如果到达下一个[]，即找不到,返回默认值
                    }
                }
                m_bLastResult=false;
                return std::string("");//找不到,返回默认值
            }

        }

    }
    m_bLastResult=false;
    return std::string("");//找不到,返回默认值
}

void CIniFile::SetFileString(const std::string & Section, const std::string & Item, const std::string & Value)
{
    std::string strline;
    std::string strSection;
    std::string strItem;

    size_t i = 0;
    size_t iFileLines = m_FileContainer.size();

    while(i<iFileLines)
    {
        strline = m_FileContainer[i++];
        trimStringLeft( strline );
        size_t rBracketPos = strline.find(']');
        bool bracketClosed =  '[' == strline[0]
                                && rBracketPos > 0
                                    && rBracketPos != std::string::npos;

        //if( strline.Find('[')==0 && strline.Find(']')>0 )//查找Section，第一个必须为[
        if( bracketClosed )
        {
            //读取Section名
            strSection=strline.substr(1, rBracketPos-1);
            //strSection.TrimLeft();
            //strSection.TrimRight();
            if(strSection == Section)//找到Section
            {
                while(i<iFileLines)
                {
                    strline = m_FileContainer[i++];
                    //strline.TrimLeft();
                    trimStringLeft( strline );
                    size_t equalsignPos = strline.find( '=' );
                    //if(strline.Find('=')>0)
                    if( equalsignPos != strline.npos && equalsignPos > 0 && equalsignPos < strline.size() - 1 )
                    {
                        //读取Item名
                        //strItem = strline.Left(strline.Find('='));//去掉=右边
                        //strItem.TrimLeft();
                        //strItem.TrimRight();
                        strItem = strline.substr( 0, equalsignPos );
                        trimStringRight( strItem );

                        if(Item == strItem)//找到Item,修改Value
                        {
                            ReplaceLine(i-1,Item + " = " + Value);
                            return;
                        }
                    }
                    else if( '[' == strline[0] ) //.Find('[')==0)//到达下一个[],未找到Item
                    {
                        InsertLine(i-1,Item + " = " + Value);
                        return;
                    }
                }
                //Section内未找到Item
                InsertLine(i,Item + " = " + Value);
                return;
            }
        }
    }

    //找不到Section
    //添加"[Section]Item=Value"
    InsertLine(i,"[" + Section + "]");
    InsertLine(i+1,Item + " = " + Value);
    return;
}



bool CIniFile::InsertLine(size_t line,const std::string & str)
{
    m_FileContainer.insert(m_FileContainer.begin() + line, str);
    return true;
}

bool CIniFile::ReplaceLine(size_t line,const std::string & str)
{
    m_FileContainer[line] = str;
    return true;
}

