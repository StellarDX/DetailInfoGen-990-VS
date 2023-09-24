// Data buffer between sub-module and main module.
// this file is written in C-language instead of C++.

#define _CRT_SECURE_NO_WARNINGS

#include "DataBuffer.h"
#include <Windows.h>
#include <iostream>
#include <string>

using namespace std;

#pragma data_seg("InfoGen_DBuf")
// Data buffer declearation
char __Final[0x7FFF]         = {'\0'};
int __OFormat                = 1;
UINT __srcencoding           = 65001;
UINT __outencoding           = 65001;
char __OutputFileName[200]   = { '\0' };

// HTML settings
char __CSSPath[200]     = { '\0' };
int __LnkCSS            = 0;
int __Cpm               = 0;

// Locale
char __LocaleID[5]      = "1033";

#pragma data_seg()
#pragma comment(linker,"/section:InfoGen_DBuf,rws")

// Functions
_EXTERN_C

EXTERNAL_CALL void SendBasicParams(string _Buf, int _OFmt, UINT _ICP, UINT _OCP, string _OFName, string _LCID)
{
    strcpy(__Final, _Buf.c_str());
    __OFormat = _OFmt;
    __srcencoding = _ICP;
    __outencoding = _OCP;
    strcpy(__OutputFileName, _OFName.c_str());
    strcpy(__LocaleID, _LCID.c_str());
    cout << "[DataBuffer.so] Data received: \n"
        << "\tBuffer Content : \n" << __Final << '\n'
        << "\tOutput Format : " << __OFormat << '\n'
        << "\tSource encoding : " << __srcencoding << '\n'
        << "\tOutput encoding : " << __outencoding << '\n'
        << "\tOutput File Name : " << __OutputFileName << '\n'
        << "\tLCID : " << __LocaleID << '\n';
}

EXTERNAL_CALL void SendHTMLParams(int _LCSSM, string _CSSPath, int _CPMethod)
{
    __LnkCSS = _LCSSM;
    strcpy(__CSSPath, _CSSPath.c_str());
    __Cpm = _CPMethod;
    cout << "[DataBuffer.so] Data received: \n"
        << "\tCSS Link Method : " << __LnkCSS << '\n'
        << "\tCSS path : " << __CSSPath << '\n'
        << "\tCopying method : " << _CPMethod << '\n';
}

EXTERNAL_CALL void LoadBasicParams(char* _DBuf, UINT* _OFmt, UINT* _Scp, UINT* _Ocp, char* _OFName, char* _LCID)
{
    if (_DBuf) { strcpy(_DBuf, __Final); }
    if (_OFmt) { *_OFmt = __OFormat; }
    if (_Scp) { *_Scp = __srcencoding; }
    if (_Ocp) { *_Ocp = __outencoding; }
    if (_OFName) { strcpy(_OFName, __OutputFileName); }
    if (_LCID) { strcpy(_LCID, __LocaleID); }
}

EXTERNAL_CALL void LoadHTMLParams(int* _LnkCSS, char* _CPath, int* _Cpm)
{
    if (_LnkCSS) { *_LnkCSS = __LnkCSS; }
    if (_CPath) { strcpy(_CPath, __CSSPath); }
    if (_Cpm) { *_Cpm = __Cpm; }
}

_END_EXTERN_C

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        cout << "[DataBuffer.so] Constructing data buffer...\n";
        break;
    //case DLL_THREAD_ATTACH:
    //case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        cout << "[DataBuffer.so] Destroying data buffer...\n";
        break;
    default:
        break;
    }
    return TRUE;
}

