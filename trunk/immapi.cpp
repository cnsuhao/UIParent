#include "immapi.h"

namespace immapi
{
	// Empty implementation of the Uniscribe API
	static HRESULT WINAPI Dummy_ScriptApplyDigitSubstitution( const SCRIPT_DIGITSUBSTITUTE*, SCRIPT_CONTROL*, SCRIPT_STATE* ) { return E_NOTIMPL; }
	static HRESULT WINAPI Dummy_ScriptStringAnalyse( HDC, const void *, int, int, int, DWORD, int, SCRIPT_CONTROL*, SCRIPT_STATE*, const int*, SCRIPT_TABDEF*, const BYTE*, SCRIPT_STRING_ANALYSIS* ) { return E_NOTIMPL; }
	static HRESULT WINAPI Dummy_ScriptStringCPtoX( SCRIPT_STRING_ANALYSIS, int, BOOL, int* ) { return E_NOTIMPL; }
	static HRESULT WINAPI Dummy_ScriptStringXtoCP( SCRIPT_STRING_ANALYSIS, int, int*, int* ) { return E_NOTIMPL; }
	static HRESULT WINAPI Dummy_ScriptStringFree( SCRIPT_STRING_ANALYSIS* ) { return E_NOTIMPL; }
	static const SCRIPT_LOGATTR* WINAPI Dummy_ScriptString_pLogAttr( SCRIPT_STRING_ANALYSIS ) { return NULL; }
	static const int* WINAPI Dummy_ScriptString_pcOutChars( SCRIPT_STRING_ANALYSIS ) { return NULL; }

	// Empty implementation of the IMM32 API
	static INPUTCONTEXT* WINAPI Dummy_ImmLockIMC( HIMC ) { return NULL; }
	static BOOL WINAPI Dummy_ImmUnlockIMC( HIMC ) { return FALSE; }
	static LPVOID WINAPI Dummy_ImmLockIMCC( HIMCC ) { return NULL; }
	static BOOL WINAPI Dummy_ImmUnlockIMCC( HIMCC ) { return FALSE; }
	static BOOL WINAPI Dummy_ImmDisableTextFrameService( DWORD ) { return TRUE; }
	static LONG WINAPI Dummy_ImmGetCompositionStringW( HIMC, DWORD, LPVOID, DWORD ) { return IMM_ERROR_GENERAL; }
	static DWORD WINAPI Dummy_ImmGetCandidateListW( HIMC, DWORD, LPCANDIDATELIST, DWORD ) { return 0; }
	static HIMC WINAPI Dummy_ImmGetContext( HWND ) { return NULL; }
	static BOOL WINAPI Dummy_ImmReleaseContext( HWND, HIMC ) { return FALSE; }
	static HIMC WINAPI Dummy_ImmAssociateContext( HWND, HIMC ) { return NULL; }
	static BOOL WINAPI Dummy_ImmGetOpenStatus( HIMC ) { return 0; }
	static BOOL WINAPI Dummy_ImmSetOpenStatus( HIMC, BOOL ) { return 0; }
	static BOOL WINAPI Dummy_ImmGetConversionStatus( HIMC, LPDWORD, LPDWORD ) { return 0; }
	static HWND WINAPI Dummy_ImmGetDefaultIMEWnd( HWND ) { return NULL; }
	static UINT WINAPI Dummy_ImmGetIMEFileNameA( HKL, LPSTR, UINT ) { return 0; }
	static UINT WINAPI Dummy_ImmGetVirtualKey( HWND ) { return 0; }
	static BOOL WINAPI Dummy_ImmNotifyIME( HIMC, DWORD, DWORD, DWORD ) { return FALSE; }
	static BOOL WINAPI Dummy_ImmSetConversionStatus( HIMC, DWORD, DWORD ) { return FALSE; }
	static BOOL WINAPI Dummy_ImmSimulateHotKey( HWND, DWORD ) { return FALSE; }
	static BOOL WINAPI Dummy_ImmIsIME( HKL ) { return FALSE; }

	// Traditional Chinese IME
	static UINT WINAPI Dummy_GetReadingString( HIMC, UINT, LPWSTR, PINT, BOOL*, PUINT ) { return 0; }
	static BOOL WINAPI Dummy_ShowReadingWindow( HIMC, BOOL ) { return FALSE; }

	// Verion library imports
	static BOOL APIENTRY Dummy_VerQueryValueA( const LPVOID, LPSTR, LPVOID *, PUINT ) { return 0; }
	static BOOL APIENTRY Dummy_GetFileVersionInfoA( LPSTR, DWORD, DWORD, LPVOID ) { return 0; }
	static DWORD APIENTRY Dummy_GetFileVersionInfoSizeA( LPSTR, LPDWORD ) { return 0; }

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	HRESULT (WINAPI *_ScriptApplyDigitSubstitution)( const SCRIPT_DIGITSUBSTITUTE*, SCRIPT_CONTROL*, SCRIPT_STATE* ) = Dummy_ScriptApplyDigitSubstitution;
	HRESULT (WINAPI *_ScriptStringAnalyse)( HDC, const void *, int, int, int, DWORD, int, SCRIPT_CONTROL*, SCRIPT_STATE*, const int*, SCRIPT_TABDEF*, const BYTE*, SCRIPT_STRING_ANALYSIS* ) = Dummy_ScriptStringAnalyse;
	HRESULT (WINAPI *_ScriptStringCPtoX)( SCRIPT_STRING_ANALYSIS, int, BOOL, int* ) = Dummy_ScriptStringCPtoX;
	HRESULT (WINAPI *_ScriptStringXtoCP)( SCRIPT_STRING_ANALYSIS, int, int*, int* ) = Dummy_ScriptStringXtoCP;
	HRESULT (WINAPI *_ScriptStringFree)( SCRIPT_STRING_ANALYSIS* ) = Dummy_ScriptStringFree;
	const SCRIPT_LOGATTR* (WINAPI *_ScriptString_pLogAttr)( SCRIPT_STRING_ANALYSIS ) = Dummy_ScriptString_pLogAttr;
	const int* (WINAPI *_ScriptString_pcOutChars)( SCRIPT_STRING_ANALYSIS ) = Dummy_ScriptString_pcOutChars;

	// Function pointers
	INPUTCONTEXT* (WINAPI * _ImmLockIMC)( HIMC ) = Dummy_ImmLockIMC;
	BOOL (WINAPI * _ImmUnlockIMC)( HIMC ) = Dummy_ImmUnlockIMC;
	LPVOID (WINAPI * _ImmLockIMCC)( HIMCC ) = Dummy_ImmLockIMCC;
	BOOL (WINAPI * _ImmUnlockIMCC)( HIMCC ) = Dummy_ImmUnlockIMCC;
	BOOL (WINAPI * _ImmDisableTextFrameService)( DWORD ) = Dummy_ImmDisableTextFrameService;
	LONG (WINAPI * _ImmGetCompositionStringW)( HIMC, DWORD, LPVOID, DWORD ) = Dummy_ImmGetCompositionStringW;
	DWORD (WINAPI * _ImmGetCandidateListW)( HIMC, DWORD, LPCANDIDATELIST, DWORD ) = Dummy_ImmGetCandidateListW;
	HIMC (WINAPI * _ImmGetContext)( HWND ) = Dummy_ImmGetContext;
	BOOL (WINAPI * _ImmReleaseContext)( HWND, HIMC ) = Dummy_ImmReleaseContext;
	HIMC (WINAPI * _ImmAssociateContext)( HWND, HIMC ) = Dummy_ImmAssociateContext;
	BOOL (WINAPI * _ImmGetOpenStatus)( HIMC ) = Dummy_ImmGetOpenStatus;
	BOOL (WINAPI * _ImmSetOpenStatus)( HIMC, BOOL ) = Dummy_ImmSetOpenStatus;
	BOOL (WINAPI * _ImmGetConversionStatus)( HIMC, LPDWORD, LPDWORD ) = Dummy_ImmGetConversionStatus;
	HWND (WINAPI * _ImmGetDefaultIMEWnd)( HWND ) = Dummy_ImmGetDefaultIMEWnd;
	UINT (WINAPI * _ImmGetIMEFileNameA)( HKL, LPSTR, UINT ) = Dummy_ImmGetIMEFileNameA;
	UINT (WINAPI * _ImmGetVirtualKey)( HWND ) = Dummy_ImmGetVirtualKey;
	BOOL (WINAPI * _ImmNotifyIME)( HIMC, DWORD, DWORD, DWORD ) = Dummy_ImmNotifyIME;
	BOOL (WINAPI * _ImmSetConversionStatus)( HIMC, DWORD, DWORD ) = Dummy_ImmSetConversionStatus;
	BOOL (WINAPI * _ImmSimulateHotKey)( HWND, DWORD ) = Dummy_ImmSimulateHotKey;
	BOOL (WINAPI * _ImmIsIME)( HKL ) = Dummy_ImmIsIME;

	// Traditional Chinese IME
	UINT (WINAPI * _GetReadingString)( HIMC, UINT, LPWSTR, PINT, BOOL*, PUINT ) = Dummy_GetReadingString;
	BOOL (WINAPI * _ShowReadingWindow)( HIMC, BOOL ) = Dummy_ShowReadingWindow;

	BOOL (APIENTRY * _VerQueryValueA)( const LPVOID, LPSTR, LPVOID *, PUINT ) = Dummy_VerQueryValueA;
	BOOL (APIENTRY * _GetFileVersionInfoA)( LPSTR, DWORD, DWORD, LPVOID ) = Dummy_GetFileVersionInfoA;
	DWORD (APIENTRY * _GetFileVersionInfoSizeA)( LPSTR, LPDWORD ) = Dummy_GetFileVersionInfoSizeA;

	/////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

#define UNISCRIBE_DLLNAME L"\\usp10.dll"
#define IMM32_DLLNAME L"\\imm32.dll"
#define VER_DLLNAME L"\\version.dll"


#define GETPROCADDRESS( Module, APIName, Temp ) \
	Temp = GetProcAddress( Module, #APIName ); \
	if( Temp ) \
	*(FARPROC*)&_##APIName = Temp

#define PLACEHOLDERPROC( APIName ) \
	_##APIName = Dummy_##APIName


	static HINSTANCE s_hDll;		// Uniscribe DLL handle
	static HINSTANCE s_hDllImm32;	// IMM32 DLL handle
	static HINSTANCE s_hDllVer;		// Version DLL handle
	static HINSTANCE s_hDllIme;     // Instance handle of the current IME module

	void InitializeUniscribe()
	{
		WCHAR wszPath[MAX_PATH+1];
		if( !::GetSystemDirectory( wszPath, MAX_PATH+1 ) )
			return;

		// Verify whether it is safe to concatenate these strings
		int len1 = lstrlen(wszPath);
		int len2 = lstrlen(UNISCRIBE_DLLNAME);
		if (len1 + len2 > MAX_PATH)
		{
			return;
		}

		// We have verified that the concatenated string will fit into wszPath,
		// so it is safe to concatenate them.
		StringCchCat( wszPath, MAX_PATH, UNISCRIBE_DLLNAME );

		s_hDll = LoadLibrary( wszPath );
		if( s_hDll )
		{
			FARPROC Temp;
			GETPROCADDRESS( s_hDll, ScriptApplyDigitSubstitution, Temp );
			GETPROCADDRESS( s_hDll, ScriptStringAnalyse, Temp );
			GETPROCADDRESS( s_hDll, ScriptStringCPtoX, Temp );
			GETPROCADDRESS( s_hDll, ScriptStringXtoCP, Temp );
			GETPROCADDRESS( s_hDll, ScriptStringFree, Temp );
			GETPROCADDRESS( s_hDll, ScriptString_pLogAttr, Temp );
			GETPROCADDRESS( s_hDll, ScriptString_pcOutChars, Temp );
		}
	}



	//--------------------------------------------------------------------------------------
	void UninitializeUniscribe()
	{
		if( s_hDll )
		{
			PLACEHOLDERPROC( ScriptApplyDigitSubstitution );
			PLACEHOLDERPROC( ScriptStringAnalyse );
			PLACEHOLDERPROC( ScriptStringCPtoX );
			PLACEHOLDERPROC( ScriptStringXtoCP );
			PLACEHOLDERPROC( ScriptStringFree );
			PLACEHOLDERPROC( ScriptString_pLogAttr );
			PLACEHOLDERPROC( ScriptString_pcOutChars );

			FreeLibrary( s_hDll );
			s_hDll = NULL;
		}
	}

	//--------------------------------------------------------------------------------------
	void InitializeImm()
	{
		FARPROC Temp;

		WCHAR wszPath[MAX_PATH+1];
		if( !::GetSystemDirectory( wszPath, MAX_PATH+1 ) )
			return;
		StringCchCat( wszPath, MAX_PATH, IMM32_DLLNAME );
		s_hDllImm32 = LoadLibrary( wszPath );
		if( s_hDllImm32 )
		{
			GETPROCADDRESS( s_hDllImm32, ImmLockIMC, Temp );
			GETPROCADDRESS( s_hDllImm32, ImmUnlockIMC, Temp );
			GETPROCADDRESS( s_hDllImm32, ImmLockIMCC, Temp );
			GETPROCADDRESS( s_hDllImm32, ImmUnlockIMCC, Temp );
			GETPROCADDRESS( s_hDllImm32, ImmDisableTextFrameService, Temp );
			GETPROCADDRESS( s_hDllImm32, ImmGetCompositionStringW, Temp );
			GETPROCADDRESS( s_hDllImm32, ImmGetCandidateListW, Temp );
			GETPROCADDRESS( s_hDllImm32, ImmGetContext, Temp );
			GETPROCADDRESS( s_hDllImm32, ImmReleaseContext, Temp );
			GETPROCADDRESS( s_hDllImm32, ImmAssociateContext, Temp );
			GETPROCADDRESS( s_hDllImm32, ImmGetOpenStatus, Temp );
			GETPROCADDRESS( s_hDllImm32, ImmSetOpenStatus, Temp );
			GETPROCADDRESS( s_hDllImm32, ImmGetConversionStatus, Temp );
			GETPROCADDRESS( s_hDllImm32, ImmGetDefaultIMEWnd, Temp );
			GETPROCADDRESS( s_hDllImm32, ImmGetIMEFileNameA, Temp );
			GETPROCADDRESS( s_hDllImm32, ImmGetVirtualKey, Temp );
			GETPROCADDRESS( s_hDllImm32, ImmNotifyIME, Temp );
			GETPROCADDRESS( s_hDllImm32, ImmSetConversionStatus, Temp );
			GETPROCADDRESS( s_hDllImm32, ImmSimulateHotKey, Temp );
			GETPROCADDRESS( s_hDllImm32, ImmIsIME, Temp );
		}

		if( !::GetSystemDirectory( wszPath, MAX_PATH+1 ) )
			return;
		StringCchCat( wszPath, MAX_PATH, VER_DLLNAME );
		s_hDllVer = LoadLibrary( wszPath );
		if( s_hDllVer )
		{
			GETPROCADDRESS( s_hDllVer, VerQueryValueA, Temp );
			GETPROCADDRESS( s_hDllVer, GetFileVersionInfoA, Temp );
			GETPROCADDRESS( s_hDllVer, GetFileVersionInfoSizeA, Temp );
		}
	}


	//--------------------------------------------------------------------------------------
	void UninitializeImm()
	{
		if( s_hDllImm32 )
		{
			PLACEHOLDERPROC( ImmLockIMC );
			PLACEHOLDERPROC( ImmUnlockIMC );
			PLACEHOLDERPROC( ImmLockIMCC );
			PLACEHOLDERPROC( ImmUnlockIMCC );
			PLACEHOLDERPROC( ImmDisableTextFrameService );
			PLACEHOLDERPROC( ImmGetCompositionStringW );
			PLACEHOLDERPROC( ImmGetCandidateListW );
			PLACEHOLDERPROC( ImmGetContext );
			PLACEHOLDERPROC( ImmReleaseContext );
			PLACEHOLDERPROC( ImmAssociateContext );
			PLACEHOLDERPROC( ImmGetOpenStatus );
			PLACEHOLDERPROC( ImmSetOpenStatus );
			PLACEHOLDERPROC( ImmGetConversionStatus );
			PLACEHOLDERPROC( ImmGetDefaultIMEWnd );
			PLACEHOLDERPROC( ImmGetIMEFileNameA );
			PLACEHOLDERPROC( ImmGetVirtualKey );
			PLACEHOLDERPROC( ImmNotifyIME );
			PLACEHOLDERPROC( ImmSetConversionStatus );
			PLACEHOLDERPROC( ImmSimulateHotKey );
			PLACEHOLDERPROC( ImmIsIME );

			FreeLibrary( s_hDllImm32 );
			s_hDllImm32 = NULL;
		}
		if( s_hDllIme )
		{
			PLACEHOLDERPROC( GetReadingString );
			PLACEHOLDERPROC( ShowReadingWindow );

			FreeLibrary( s_hDllIme );
			s_hDllIme = NULL;
		}
		if( s_hDllVer )
		{
			PLACEHOLDERPROC( VerQueryValueA );
			PLACEHOLDERPROC( GetFileVersionInfoA );
			PLACEHOLDERPROC( GetFileVersionInfoSizeA );

			FreeLibrary( s_hDllVer );
			s_hDllVer = NULL;
		}
	}


	// Helper class that help us automatically initialize and uninitialize external API.
	// Important: C++ does not guaranteed the order global and static objects are
	//            initialized in.  Therefore, do not use edit controls inside
	//            a constructor.
	class CExternalApiInitializer
	{
	public:
		CExternalApiInitializer()
		{
			InitializeUniscribe();
			InitializeImm();
		}
		~CExternalApiInitializer()
		{
			UninitializeUniscribe();
			UninitializeImm();
		}
	} EXTERNAL_API_INITIALIZER;


	//--------------------------------------------------------------------------------------
	// Sets up IME-specific APIs for the IME edit controls.  This is called every time
	// the input locale changes.
	void SetupImeApi(HKL hklCurrent)
	{
		char szImeFile[MAX_PATH + 1];

		_GetReadingString = NULL;
		_ShowReadingWindow = NULL;
		if( _ImmGetIMEFileNameA( hklCurrent, szImeFile, sizeof(szImeFile)/sizeof(szImeFile[0]) - 1 ) == 0 )
			return;

		if( s_hDllIme ) FreeLibrary( s_hDllIme );
		s_hDllIme = LoadLibraryA( szImeFile );
		if ( !s_hDllIme )
			return;
		_GetReadingString = (UINT (WINAPI*)(HIMC, UINT, LPWSTR, PINT, BOOL*, PUINT))
			( GetProcAddress( s_hDllIme, "GetReadingString" ) );
		_ShowReadingWindow =(BOOL (WINAPI*)(HIMC, BOOL))
			( GetProcAddress( s_hDllIme, "ShowReadingWindow" ) );
	}


}