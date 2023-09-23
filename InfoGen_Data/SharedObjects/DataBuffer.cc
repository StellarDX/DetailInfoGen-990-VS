#include "DataBuffer.h"
#include <Windows.h>
#include <iostream>
#include <string>

using namespace std;

#pragma data_seg("InfoGen_DBuf")
// Data buffer declearation
string Final          = string();
int OFormat           = 1;
int outencoding       = 65001;

// HTML settings
string CPath          = string();
string HTMLcontent    = string();

#pragma data_seg()
#pragma comment(linker,"/section:InfoGen_DBuf,rws")

// Functions
//_EXTERN_C

EXTERNAL_CALL void SendBasicParams(string _Buf, int _OFmt, int _OCP)
{
    Final = _Buf;
    OFormat = _OFmt;
    outencoding = _OCP;
}

EXTERNAL_CALL void SendHTMLParams(string _Buf, string _CSSPath)
{
    HTMLcontent = _Buf;
    CPath = _CSSPath;
}

EXTERNAL_CALL string CurrentBuffer() {return Final;}
EXTERNAL_CALL int OutFormat() { return OFormat; }
EXTERNAL_CALL int OutEncoding() { return outencoding; }

EXTERNAL_CALL string CurrentHBuf() { return HTMLcontent; }
EXTERNAL_CALL string CSSPath() { return CPath; }

//_END_EXTERN_C

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        cout << "[common.so] Initializing data buffer...\n";
        break;
    //case DLL_THREAD_ATTACH:
    //case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        cout << "[common.so] Destroying data buffer...\n";
        break;
    default:
        break;
    }
    return TRUE;
}

