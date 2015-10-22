#include <Windows.h>
#include <imm.h>
#include <dimm.h>
#include <usp10.h>
#include <Strsafe.h>

namespace immapi
{

// Function pointers
extern HRESULT (WINAPI *_ScriptApplyDigitSubstitution)( const SCRIPT_DIGITSUBSTITUTE*, SCRIPT_CONTROL*, SCRIPT_STATE* );
extern HRESULT (WINAPI *_ScriptStringAnalyse)( HDC, const void *, int, int, int, DWORD, int, SCRIPT_CONTROL*, SCRIPT_STATE*, const int*, SCRIPT_TABDEF*, const BYTE*, SCRIPT_STRING_ANALYSIS* );
extern HRESULT (WINAPI *_ScriptStringCPtoX)( SCRIPT_STRING_ANALYSIS, int, BOOL, int* );
extern HRESULT (WINAPI *_ScriptStringXtoCP)( SCRIPT_STRING_ANALYSIS, int, int*, int* );
extern HRESULT (WINAPI *_ScriptStringFree)( SCRIPT_STRING_ANALYSIS* );
extern const SCRIPT_LOGATTR* (WINAPI *_ScriptString_pLogAttr)( SCRIPT_STRING_ANALYSIS );
extern const int* (WINAPI *_ScriptString_pcOutChars)( SCRIPT_STRING_ANALYSIS );

// Function pointers: IMM32
extern INPUTCONTEXT* (WINAPI * _ImmLockIMC)( HIMC );
extern BOOL (WINAPI * _ImmUnlockIMC)( HIMC );
extern LPVOID (WINAPI * _ImmLockIMCC)( HIMCC );
extern BOOL (WINAPI * _ImmUnlockIMCC)( HIMCC );
extern BOOL (WINAPI * _ImmDisableTextFrameService)( DWORD );
extern LONG (WINAPI * _ImmGetCompositionStringW)( HIMC, DWORD, LPVOID, DWORD );
extern DWORD (WINAPI * _ImmGetCandidateListW)( HIMC, DWORD, LPCANDIDATELIST, DWORD );
extern HIMC (WINAPI * _ImmGetContext)( HWND );
extern BOOL (WINAPI * _ImmReleaseContext)( HWND, HIMC );
extern HIMC (WINAPI * _ImmAssociateContext)( HWND, HIMC );
extern BOOL (WINAPI * _ImmGetOpenStatus)( HIMC );
extern BOOL (WINAPI * _ImmSetOpenStatus)( HIMC, BOOL );
extern BOOL (WINAPI * _ImmGetConversionStatus)( HIMC, LPDWORD, LPDWORD );
extern HWND (WINAPI * _ImmGetDefaultIMEWnd)( HWND );
extern UINT (WINAPI * _ImmGetIMEFileNameA)( HKL, LPSTR, UINT );
extern UINT (WINAPI * _ImmGetVirtualKey)( HWND );
extern BOOL (WINAPI * _ImmNotifyIME)( HIMC, DWORD, DWORD, DWORD );
extern BOOL (WINAPI * _ImmSetConversionStatus)( HIMC, DWORD, DWORD );
extern BOOL (WINAPI * _ImmSimulateHotKey)( HWND, DWORD );
extern BOOL (WINAPI * _ImmIsIME)( HKL );

// Function pointers: Traditional Chinese IME
extern UINT (WINAPI * _GetReadingString)( HIMC, UINT, LPWSTR, PINT, BOOL*, PUINT );
extern BOOL (WINAPI * _ShowReadingWindow)( HIMC, BOOL );

// Function pointers: Verion library imports
extern BOOL (APIENTRY * _VerQueryValueA)( const LPVOID, LPSTR, LPVOID *, PUINT );
extern BOOL (APIENTRY * _GetFileVersionInfoA)( LPSTR, DWORD, DWORD, LPVOID );
extern DWORD (APIENTRY * _GetFileVersionInfoSizeA)( LPSTR, LPDWORD );

//////////////////////////////////////////////////////////////////////////
extern void SetupImeApi(HKL hklCurrent);

}
