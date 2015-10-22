#pragma once
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <imm.h>
#include <dimm.h>
#include <usp10.h>
#include <Strsafe.h>


#include "CEGUI\Window.h"
#include "CEGUI\widgets\DefaultWindow.h"


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#define EVENT_EDITBOX_STRING                0x0601
#define EVENT_EDITBOX_CHANGE                0x0602

//////////////////////////////////////////////////////////////////////////

inline int RectWidth( RECT &rc ) { return ( (rc).right - (rc).left ); }
inline int RectHeight( RECT &rc ) { return ( (rc).bottom - (rc).top ); }

#define DXUTTRACE(s)                  wprintf(s)
#define DXTRACE_MSG(str)              (0L)
#define DXTRACE_ERR(str,hr)           (hr)

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#endif    
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif    
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif
#define DXTRACE_MSG(str)              (0L)
#define DXTRACE_ERR(str,hr)           (hr)
#define DXTRACE_ERR_MSGBOX(str,hr)    (hr)
#define DXUT_ERR(str,hr)           (hr)
#define DXUT_ERR_MSGBOX(str,hr)    (hr)
#ifndef V
#define V(x)           { hr = x; }
#endif
#ifndef V_RETURN
#define V_RETURN(x)    { hr = x; if( FAILED(hr) ) { return hr; } }
#endif

#define DXUT_MAX_EDITBOXLENGTH 0xFFFF

//--------------------------------------------------------------------------------------
// A growable array
//--------------------------------------------------------------------------------------
template< typename TYPE >
class CGrowableArray
{
public:
	CGrowableArray()  { m_pData = NULL; m_nSize = 0; m_nMaxSize = 0; }
	CGrowableArray( const CGrowableArray<TYPE>& a ) { for( int i=0; i < a.m_nSize; i++ ) Add( a.m_pData[i] ); }
	~CGrowableArray() { RemoveAll(); }

	const TYPE& operator[]( int nIndex ) const { return GetAt( nIndex ); }
	TYPE& operator[]( int nIndex ) { return GetAt( nIndex ); }

	CGrowableArray& operator=( const CGrowableArray<TYPE>& a ) { if( this == &a ) return *this; RemoveAll(); for( int i=0; i < a.m_nSize; i++ ) Add( a.m_pData[i] ); return *this; }

	HRESULT SetSize( int nNewMaxSize );
	HRESULT Add( const TYPE& value );
	HRESULT Insert( int nIndex, const TYPE& value );
	HRESULT SetAt( int nIndex, const TYPE& value );
	TYPE&   GetAt( int nIndex ) { assert( nIndex >= 0 && nIndex < m_nSize ); return m_pData[nIndex]; }
	int     GetSize() const { return m_nSize; }
	TYPE*   GetData() { return m_pData; }
	bool    Contains( const TYPE& value ){ return ( -1 != IndexOf( value ) ); }

	int     IndexOf( const TYPE& value ) { return ( m_nSize > 0 ) ? IndexOf( value, 0, m_nSize ) : -1; }
	int     IndexOf( const TYPE& value, int iStart ) { return IndexOf( value, iStart, m_nSize - iStart ); }
	int     IndexOf( const TYPE& value, int nIndex, int nNumElements );

	int     LastIndexOf( const TYPE& value ) { return ( m_nSize > 0 ) ? LastIndexOf( value, m_nSize-1, m_nSize ) : -1; }
	int     LastIndexOf( const TYPE& value, int nIndex ) { return LastIndexOf( value, nIndex, nIndex+1 ); }
	int     LastIndexOf( const TYPE& value, int nIndex, int nNumElements );

	HRESULT Remove( int nIndex );
	void    RemoveAll() { SetSize(0); }

protected:
	TYPE* m_pData;      // the actual array of data
	int m_nSize;        // # of elements (upperBound - 1)
	int m_nMaxSize;     // max allocated

	HRESULT SetSizeInternal( int nNewMaxSize );  // This version doesn't call ctor or dtor.
};


/////////////////////////////////////////////////////////////////

class CDXUTControl;
class CDXUTEditBox;
class CDXUTIMEEditBox;
typedef VOID (CALLBACK *PCALLBACKDXUTGUIEVENT) ( UINT nEvent, int nControlID, CDXUTControl* pControl );

//--------------------------------------------------------------------------------------
// Structs for shared resources
//--------------------------------------------------------------------------------------
struct DXUTTextureNode
{
	WCHAR strFilename[MAX_PATH];
	CEGUI::Texture* pTexture;
	DWORD dwWidth;
	DWORD dwHeight;
};

struct DXUTFontNode
{
	WCHAR strFace[MAX_PATH];
	CEGUI::Font* pFont;
	LONG  nHeight;
	LONG  nWeight;
};

//--------------------------------------------------------------------------------------
// Enums for pre-defined control types
//--------------------------------------------------------------------------------------
enum DXUT_CONTROL_TYPE 
{ 
	DXUT_CONTROL_BUTTON, 
	DXUT_CONTROL_STATIC, 
	DXUT_CONTROL_CHECKBOX,
	DXUT_CONTROL_RADIOBUTTON,
	DXUT_CONTROL_COMBOBOX,
	DXUT_CONTROL_SLIDER,
	DXUT_CONTROL_EDITBOX,
	DXUT_CONTROL_IMEEDITBOX,
	DXUT_CONTROL_LISTBOX,
	DXUT_CONTROL_SCROLLBAR,
};

enum DXUT_CONTROL_STATE
{
	DXUT_STATE_NORMAL = 0,
	DXUT_STATE_DISABLED,
	DXUT_STATE_HIDDEN,
	DXUT_STATE_FOCUS,
	DXUT_STATE_MOUSEOVER,
	DXUT_STATE_PRESSED,
};
#define MAX_CONTROL_STATES 6

struct DXUTBlendColor
{
	void Init( D3DCOLOR defaultColor, D3DCOLOR disabledColor = D3DCOLOR_ARGB(200, 128, 128, 128), D3DCOLOR hiddenColor = 0 );
	void Blend( UINT iState, float fElapsedTime, float fRate = 0.7f );

	D3DCOLOR  States[ MAX_CONTROL_STATES ]; // Modulate colors for all possible control states
	D3DXCOLOR Current;
};

//-----------------------------------------------------------------------------
// Contains all the display tweakables for a sub-control
//-----------------------------------------------------------------------------
class CDXUTElement
{
public:
	void SetTexture( UINT iTexture, RECT* prcTexture, D3DCOLOR defaultTextureColor = D3DCOLOR_ARGB(255, 255, 255, 255) );
	void SetFont( UINT iFont, D3DCOLOR defaultFontColor = D3DCOLOR_ARGB(255, 255, 255, 255), DWORD dwTextFormat = DT_CENTER | DT_VCENTER );

	void Refresh();

	UINT iTexture;          // Index of the texture for this Element 
	UINT iFont;             // Index of the font for this Element
	DWORD dwTextFormat;     // The format argument to DrawText 

	RECT rcTexture;         // Bounding rect of this element on the composite texture

	DXUTBlendColor TextureColor;
	DXUTBlendColor FontColor;
};

//-----------------------------------------------------------------------------
// Contains all the display information for a given control type
//-----------------------------------------------------------------------------
struct DXUTElementHolder
{
	UINT nControlType;
	UINT iElement;

	CDXUTElement Element;
};

//////////////////////////////////////////////////////////////////////////
struct WND32_MSG
{
	HWND	hWnd;
	UINT	message;
	WPARAM	wParam;
	LPARAM	lParam;
	BOOL	bResult;
};

//-----------------------------------------------------------------------------
// All controls must be assigned to a dialog, which handles
// input and rendering for the controls.
//-----------------------------------------------------------------------------
class CDXUTDialog
{
public:
	CDXUTDialog();
	~CDXUTDialog();

	// Windows message handler
	bool MsgProc( __in HWND hWnd, __in UINT Msg, __in WPARAM wParam, __in LPARAM lParam);

	// Control creation
	HRESULT AddEditBox( int ID, LPCWSTR strText, int x, int y, int width, int height, bool bIsDefault=false, CDXUTEditBox** ppCreated=NULL );
	HRESULT AddIMEEditBox( int ID, LPCWSTR strText, int x, int y, int width, int height, bool bIsDefault=false, CDXUTIMEEditBox** ppCreated=NULL );


	HRESULT AddControl( CDXUTControl* pControl );
	HRESULT InitControl( CDXUTControl* pControl );

	// Control retrieval
	CDXUTEditBox*     GetEditBox( int ID ) { return (CDXUTEditBox*) GetControl( ID, DXUT_CONTROL_EDITBOX ); }
	CDXUTIMEEditBox*  GetIMEEditBox( int ID ) { return (CDXUTIMEEditBox*) GetControl( ID, DXUT_CONTROL_IMEEDITBOX ); }

	CDXUTControl* GetControl( int ID );
	CDXUTControl* GetControl( int ID, UINT nControlType );
	CDXUTControl* GetControlAtPoint( POINT pt );

	bool GetControlEnabled( int ID );
	void SetControlEnabled( int ID, bool bEnabled );

	void ClearRadioButtonGroup( UINT nGroup );
	void ClearComboBox( int ID );

	// Access the default display Elements used when adding new controls
	HRESULT       SetDefaultElement( UINT nControlType, UINT iElement, CDXUTElement* pElement );
	CDXUTElement* GetDefaultElement( UINT nControlType, UINT iElement );

	// Methods called by controls
	void SendEvent( UINT nEvent, bool bTriggeredByUser, CDXUTControl* pControl );
	void RequestFocus( CDXUTControl* pControl );

	// Render helpers
	virtual HRESULT DrawRect( CEGUI::GeometryBuffer* geobuf, RECT* pRect, D3DCOLOR color );
	virtual HRESULT DrawPolyLine( CEGUI::GeometryBuffer* geobuf, POINT* apPoints, UINT nNumPoints, D3DCOLOR color );
	virtual HRESULT DrawSprite( CEGUI::GeometryBuffer* geobuf, CDXUTElement* pElement, RECT* prcDest );
	virtual HRESULT CalcTextRect( LPCWSTR strText, CDXUTElement* pElement, RECT* prcDest, int nCount = -1 );
	virtual HRESULT DrawText( CEGUI::GeometryBuffer* geobuf, LPCWSTR strText, CDXUTElement* pElement, RECT* prcDest, bool bShadow = false, int nCount = -1 );

	// Attributes
	bool GetVisible() { return m_bVisible; }
	void SetVisible( bool bVisible ) { m_bVisible = bVisible; }
	bool GetMinimized() { return m_bMinimized; }
	void SetMinimized( bool bMinimized ) { m_bMinimized = bMinimized; }
	void SetBackgroundColors( D3DCOLOR colorAllCorners ) { SetBackgroundColors( colorAllCorners, colorAllCorners, colorAllCorners, colorAllCorners ); }
	void SetBackgroundColors( D3DCOLOR colorTopLeft, D3DCOLOR colorTopRight, D3DCOLOR colorBottomLeft, D3DCOLOR colorBottomRight );
	void EnableCaption( bool bEnable ) { m_bCaption = bEnable; }
	int GetCaptionHeight() const { return m_nCaptionHeight; }
	void SetCaptionHeight( int nHeight ) { m_nCaptionHeight = nHeight; }
	void SetCaptionText( const WCHAR *pwszText ) { StringCchCopy( m_wszCaption, sizeof(m_wszCaption)/sizeof(m_wszCaption[0]), pwszText); }
	void GetLocation( POINT &Pt ) const { Pt.x = m_x; Pt.y = m_y; }
	void SetLocation( int x, int y ) { m_x = x; m_y = y; }
	void SetSize( int width, int height );
	int GetWidth() { return m_width; }
	int GetHeight() { return m_height; }

	void SetNextDialog( CDXUTDialog* pNextDialog );

	static void SetRefreshTime( float fTime ){ s_fTimeRefresh = fTime; }

	static CDXUTControl* GetNextControl( CDXUTControl* pControl );
	static CDXUTControl* GetPrevControl( CDXUTControl* pControl );

	void RemoveControl( int ID );
	void RemoveAllControls();

	// Sets the callback used to notify the app of control events
	void SetCallback( PCALLBACKDXUTGUIEVENT pCallback ) { m_pCallbackEvent = pCallback; }
	void EnableNonUserEvents( bool bEnable ) { m_bNonUserEvents = bEnable; }
	void EnableKeyboardInput( bool bEnable ) { m_bKeyboardInput = bEnable; }
	void EnableMouseInput( bool bEnable ) { m_bMouseInput = bEnable; }

	// Device state notification
	void Refresh();
	HRESULT OnRender( float fElapsedTime );    

	// Shared resource access. Indexed fonts and textures are shared among
	// all the controls.
	HRESULT       SetFont( UINT index, LPCWSTR strFaceName, LONG height, LONG weight );
	DXUTFontNode* GetFont( UINT index );

	HRESULT          SetTexture( UINT index, LPCWSTR strFilename );
	DXUTTextureNode* GetTexture( UINT index );

	static void ClearFocus();
	void FocusDefaultControl();

	bool m_bNonUserEvents;
	bool m_bKeyboardInput;
	bool m_bMouseInput;
	WND32_MSG	m_kMsg;

private:
	int m_nDefaultControlID;

	static double s_fTimeRefresh;
	double m_fTimeLastRefresh;

	// Initialize default Elements
	void InitDefaultElements();

	// Windows message handlers
	void OnMouseMove( POINT pt );
	void OnMouseUp( POINT pt );

	// Control events
	void OnCycleFocus( bool bForward );

	static CDXUTControl* s_pControlFocus;        // The control which has focus
	static CDXUTControl* s_pControlPressed;      // The control currently pressed

	CDXUTControl* m_pControlMouseOver;           // The control which is hovered over

	bool m_bVisible;
	bool m_bCaption;
	bool m_bMinimized;
	WCHAR m_wszCaption[256];

	int m_x;
	int m_y;
	int m_width;
	int m_height;
	int m_nCaptionHeight;

	D3DCOLOR m_colorTopLeft;
	D3DCOLOR m_colorTopRight;
	D3DCOLOR m_colorBottomLeft;
	D3DCOLOR m_colorBottomRight;

	PCALLBACKDXUTGUIEVENT m_pCallbackEvent;

	CGrowableArray< int > m_Textures;   // Index into m_TextureCache;
	CGrowableArray< int > m_Fonts;      // Index into m_FontCache;

	CGrowableArray< CDXUTControl* > m_Controls;
	CGrowableArray< DXUTElementHolder* > m_DefaultElements;

	CDXUTElement m_CapElement;  // Element for the caption

	CDXUTDialog* m_pNextDialog;
	CDXUTDialog* m_pPrevDialog;
};

//-----------------------------------------------------------------------------
// Base class for controls
//-----------------------------------------------------------------------------
class CDXUTControl : public CEGUI::Window
{
public:
	CDXUTControl( CDXUTDialog *pDialog = NULL );
	virtual ~CDXUTControl();

	
	virtual HRESULT OnInit() { return S_OK; }
	virtual void Refresh();
	virtual void Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime ) { };

	// Windows message handler
	virtual bool MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam ) { return false; }

	virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam ) { return false; }
	virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam ) { return false; }

	virtual bool CanHaveFocus() { return false; }
	virtual void OnFocusIn() { m_bHasFocus = true; }
	virtual void OnFocusOut() { m_bHasFocus = false; }
	virtual void OnMouseEnter() { m_bMouseOver = true; }
	virtual void OnMouseLeave() { m_bMouseOver = false; }
	virtual void OnHotkey() {}

	virtual void onActivated(CEGUI::ActivationEventArgs& e);
	virtual void onDeactivated(CEGUI::ActivationEventArgs& e);

    virtual void onMouseMove(CEGUI::MouseEventArgs& e);
    virtual void onMouseButtonDown(CEGUI::MouseEventArgs& e);
	virtual void onMouseClicked(CEGUI::MouseEventArgs& e);
    virtual void onMouseButtonUp(CEGUI::MouseEventArgs& e);
	virtual void onKeyUp(CEGUI::KeyEventArgs& e);
	virtual void onKeyDown(CEGUI::KeyEventArgs& e);
	virtual void onCharacter(CEGUI::KeyEventArgs& e);

	virtual void printSelf() {}

	virtual BOOL ContainsPoint( POINT pt );

	virtual void SetEnabled( bool bEnabled ) { m_bEnabled = bEnabled; }
	virtual bool GetEnabled() { return m_bEnabled; }
	virtual void SetVisible( bool bVisible ) { m_bVisible = bVisible; }
	virtual bool GetVisible() { return m_bVisible; }

	UINT GetType() const { return m_Type; }

	int  GetID() const { return m_ID; }
	void SetID( int ID );

	void SetLocation( int x, int y );
	void SetSize( int width, int height );

	void SetHotkey( UINT nHotkey ) { m_nHotkey = nHotkey; }
	UINT GetHotkey() { return m_nHotkey; }

	void SetUserData( void *pUserData ) { m_pUserData = pUserData; }
	void *GetUserData() const { return m_pUserData; }

	virtual void SetTextColor( D3DCOLOR Color );
	CDXUTElement* GetElement( UINT iElement ) { return m_Elements.GetAt( iElement ); }
	HRESULT SetElement( UINT iElement, CDXUTElement* pElement);

	void SetWND(HWND hWnd) { m_hWnd = hWnd; }

	bool m_bVisible;                // Shown/hidden flag
	bool m_bMouseOver;              // Mouse pointer is above control
	bool m_bHasFocus;               // Control has input focus
	bool m_bIsDefault;              // Is the default control

	// Size, scale, and positioning members
	int m_x, m_y;
	int m_width, m_height;

	// These members are set by the container
	CDXUTDialog* m_pDialog;    // Parent container
	UINT m_Index;              // Index within the control list

	CGrowableArray< CDXUTElement* > m_Elements;  // All display elements

	HWND m_hWnd;

protected:
	virtual void UpdateRects();

	int  m_ID;                 // ID number
	DXUT_CONTROL_TYPE m_Type;  // Control type, set once in constructor  
	UINT m_nHotkey;            // Virtual key code for this control's hotkey
	void *m_pUserData;         // Data associated with this control that is set by user.

	bool m_bEnabled;           // Enabled/disabled flag

	RECT m_rcBoundingBox;      // Rectangle defining the active region of the control
};

//-----------------------------------------------------------------------------
// EditBox control
//-----------------------------------------------------------------------------
class CDXUTEditBox : public CDXUTControl
{
public:
	CDXUTEditBox( CDXUTDialog *pDialog = NULL );
	virtual ~CDXUTEditBox();

	virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
	virtual bool MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void UpdateRects();
	virtual bool CanHaveFocus() { return (m_bVisible && m_bEnabled); }
	virtual void Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );
	virtual void OnFocusIn();

	virtual void populateGeometryBuffer();
	virtual void update(float elapsed);

	virtual void printSelf();

	void SetText( LPCWSTR wszText, bool bSelected = false );
	LPCWSTR GetText() { return m_Buffer.GetBuffer(); }
	int GetTextLength() { return m_Buffer.GetTextSize(); }  // Returns text length in chars excluding NULL.
	HRESULT GetTextCopy( LPWSTR strDest, UINT bufferCount );
	void ClearText();
	virtual void SetTextColor( D3DCOLOR Color ) { m_TextColor = Color; }  // Text color
	void SetSelectedTextColor( D3DCOLOR Color ) { m_SelTextColor = Color; }  // Selected text color
	void SetSelectedBackColor( D3DCOLOR Color ) { m_SelBkColor = Color; }  // Selected background color
	void SetCaretColor( D3DCOLOR Color ) { m_CaretColor = Color; }  // Caret color
	void SetBorderWidth( int nBorder ) { m_nBorder = nBorder; UpdateRects(); }  // Border of the window
	void SetSpacing( int nSpacing ) { m_nSpacing = nSpacing; UpdateRects(); }
	void ParseFloatArray( float *pNumbers, int nCount );
	void SetTextFloatArray( const float *pNumbers, int nCount );

public:
	//-----------------------------------------------------------------------------
	// CUniBuffer class for the edit control
	//-----------------------------------------------------------------------------
	class CUniBuffer
	{
		friend class CExternalApiInitializer;

		// static member

		static void InitializeUniscribe();
		static void UninitializeUniscribe();

		static HINSTANCE s_hDll;  // Uniscribe DLL handle

		// Instance data
	private:
		WCHAR *m_pwszBuffer;    // Buffer to hold text
		int    m_nBufferSize;   // Size of the buffer allocated, in characters
		// Uniscribe-specific
		DXUTFontNode* m_pFontNode; // Font node for the font that this buffer uses
		bool m_bAnalyseRequired;// True if the string has changed since last analysis.
		SCRIPT_STRING_ANALYSIS m_Analysis;  // Analysis for the current string

	private:
		bool Grow( int nNewSize = -1 );

		// Uniscribe -- Analyse() analyses the string in the buffer
		HRESULT Analyse();

	public:
		CUniBuffer( int nInitialSize = 1 );
		~CUniBuffer();
		int GetBufferSize() { return m_nBufferSize; }
		bool SetBufferSize( int nSize );
		int GetTextSize() { return lstrlenW( m_pwszBuffer ); }
		const WCHAR *GetBuffer() { return m_pwszBuffer; }
		const WCHAR &operator[]( int n ) const { return m_pwszBuffer[n]; }
		WCHAR &operator[]( int n );
		DXUTFontNode *GetFontNode() { return m_pFontNode; }
		void SetFontNode( DXUTFontNode *pFontNode ) { m_pFontNode = pFontNode; }
		void Clear();

		// Inserts the char at specified index.
		// If nIndex == -1, insert to the end.
		bool InsertChar( int nIndex, WCHAR wChar );

		// Removes the char at specified index.
		// If nIndex == -1, remove the last char.
		bool RemoveChar( int nIndex );

		// Inserts the first nCount characters of the string pStr at specified index.
		// If nCount == -1, the entire string is inserted.
		// If nIndex == -1, insert to the end.
		bool InsertString( int nIndex, const WCHAR *pStr, int nCount = -1 );

		bool SetText( LPCWSTR wszText );

		// Uniscribe
		HRESULT CPtoX( int nCP, BOOL bTrail, int *pX );
		HRESULT XtoCP( int nX, int *pCP, int *pnTrail );
		void GetPriorItemPos( int nCP, int *pPrior );
		void GetNextItemPos( int nCP, int *pPrior );
	};

protected:
	void PlaceCaret( int nCP );
	void DeleteSelectionText();
	void ResetCaretBlink();
	void CopyToClipboard();
	void PasteFromClipboard();

	CUniBuffer m_Buffer;     // Buffer to hold text
	int      m_nBorder;      // Border of the window
	int      m_nSpacing;     // Spacing between the text and the edge of border
	RECT     m_rcText;       // Bounding rectangle for the text
	RECT     m_rcRender[9];  // Convenient rectangles for rendering elements
	double   m_dfBlink;      // Caret blink time in milliseconds
	double   m_dfLastBlink;  // Last timestamp of caret blink
	bool     m_bCaretOn;     // Flag to indicate whether caret is currently visible
	int      m_nCaret;       // Caret position, in characters
	bool     m_bInsertMode;  // If true, control is in insert mode. Else, overwrite mode.
	int      m_nSelStart;    // Starting position of the selection. The caret marks the end.
	int      m_nFirstVisible;// First visible character in the edit control
	D3DCOLOR m_TextColor;    // Text color
	D3DCOLOR m_SelTextColor; // Selected text color
	D3DCOLOR m_SelBkColor;   // Selected background color
	D3DCOLOR m_CaretColor;   // Caret color

	// Mouse-specific
	bool m_bMouseDrag;       // True to indicate drag in progress

	// Static
	static bool     s_bHideCaret;   // If true, we don't render the caret.
};


//-----------------------------------------------------------------------------
// IME-enabled EditBox control
//-----------------------------------------------------------------------------
#define MAX_CANDLIST 10
#define MAX_COMPSTRING_SIZE 256

class CDXUTIMEEditBox : public CDXUTEditBox
{
	static HIMC      s_hImcDef;    // Default input context

protected:


public:
	CDXUTIMEEditBox( CDXUTDialog *pDialog = NULL );
	virtual ~CDXUTIMEEditBox();

	static  HRESULT StaticOnCreateDevice(HWND hWnd);
	static  bool StaticMsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	static  void EnableImeSystem( bool bEnable );

	virtual void Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );
	virtual bool MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
	virtual void UpdateRects();
	virtual void OnFocusIn();
	virtual void OnFocusOut();

	void PumpMessage();

	virtual void RenderCandidateReadingWindow( IDirect3DDevice9* pd3dDevice, float fElapsedTime, bool bReading );
	virtual void RenderComposition( IDirect3DDevice9* pd3dDevice, float fElapsedTime );
	virtual void RenderIndicator( IDirect3DDevice9* pd3dDevice, float fElapsedTime );

protected:
	static WORD GetLanguage() { return LOWORD( s_hklCurrent ); }
	static WORD GetPrimaryLanguage();
	static WORD GetSubLanguage();
	static void SendKey( BYTE nVirtKey );
	static DWORD GetImeId( UINT uIndex = 0 );
	static void CheckInputLocale();
	static void CheckToggleState(HWND hWnd);
	static void ResetCompositionString();
	void TruncateCompString( bool bUseBackSpace = true, int iNewStrLen = 0 );
	void FinalizeString( bool bSend );
	static void GetReadingWindowOrientation( HWND hWnd, DWORD dwId );
	static void GetPrivateReadingString(HWND hWnd);

	void SendCompString();
	bool OnIMENotify(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

protected:
	enum { INDICATOR_NON_IME, INDICATOR_CHS, INDICATOR_CHT, INDICATOR_KOREAN, INDICATOR_JAPANESE };
	enum IMESTATE { IMEUI_STATE_OFF, IMEUI_STATE_ON, IMEUI_STATE_ENGLISH };

	struct CCandList
	{
		WCHAR awszCandidate[MAX_CANDLIST][256];
		CUniBuffer HoriCand; // Candidate list string (for horizontal candidate window)
		int   nFirstSelected; // First character position of the selected string in HoriCand
		int   nHoriSelectedLen; // Length of the selected string in HoriCand
		DWORD dwCount;       // Number of valid entries in the candidate list
		DWORD dwSelection;   // Currently selected candidate entry relative to page top
		DWORD dwPageSize;
		int   nReadingError; // Index of the error character
		bool  bShowWindow;   // Whether the candidate list window is visible
		RECT  rcCandidate;   // Candidate rectangle computed and filled each time before rendered
	};

	struct CInputLocale
	{
		HKL   m_hKL;            // Keyboard layout
		WCHAR m_wszLangAbb[3];  // Language abbreviation
		WCHAR m_wszLang[64];    // Localized language name
	};

	// Application-wide data
	static HKL     s_hklCurrent;          // Current keyboard layout of the process
	static bool    s_bVerticalCand;       // Indicates that the candidates are listed vertically
	static LPWSTR  s_wszCurrIndicator;    // Points to an indicator string that corresponds to current input locale
	static WCHAR   s_aszIndicator[5][3];  // String to draw to indicate current input locale
	static bool    s_bInsertOnType;       // Insert the character as soon as a key is pressed (Korean behavior)
	static IMESTATE  s_ImeState;          // IME global state
	static bool    s_bEnableImeSystem;    // Whether the IME system is active
	static POINT   s_ptCompString;        // Composition string position. Updated every frame.
	static int     s_nCompCaret;          // Caret position of the composition string
	static int     s_nFirstTargetConv;    // Index of the first target converted char in comp string.  If none, -1.
	static CUniBuffer s_CompString;       // Buffer to hold the composition string (we fix its length)
	static BYTE    s_abCompStringAttr[MAX_COMPSTRING_SIZE];
	static DWORD   s_adwCompStringClause[MAX_COMPSTRING_SIZE];
	static WCHAR   s_wszReadingString[32];// Used only with horizontal reading window (why?)
	static CCandList s_CandList;          // Data relevant to the candidate list
	static bool    s_bShowReadingWindow;  // Indicates whether reading window is visible
	static bool    s_bHorizontalReading;  // Indicates whether the reading window is vertical or horizontal
	static bool    s_bChineseIME;
	static CGrowableArray< CInputLocale > s_Locale; // Array of loaded keyboard layout on system

	// Color of various IME elements
	D3DCOLOR       m_ReadingColor;        // Reading string color
	D3DCOLOR       m_ReadingWinColor;     // Reading window color
	D3DCOLOR       m_ReadingSelColor;     // Selected character in reading string
	D3DCOLOR       m_ReadingSelBkColor;   // Background color for selected char in reading str
	D3DCOLOR       m_CandidateColor;      // Candidate string color
	D3DCOLOR       m_CandidateWinColor;   // Candidate window color
	D3DCOLOR       m_CandidateSelColor;   // Selected candidate string color
	D3DCOLOR       m_CandidateSelBkColor; // Selected candidate background color
	D3DCOLOR       m_CompColor;           // Composition string color
	D3DCOLOR       m_CompWinColor;        // Composition string window color
	D3DCOLOR       m_CompCaretColor;      // Composition string caret color
	D3DCOLOR       m_CompTargetColor;     // Composition string target converted color
	D3DCOLOR       m_CompTargetBkColor;   // Composition string target converted background
	D3DCOLOR       m_CompTargetNonColor;  // Composition string target non-converted color
	D3DCOLOR       m_CompTargetNonBkColor;// Composition string target non-converted background
	D3DCOLOR       m_IndicatorImeColor;   // Indicator text color for IME
	D3DCOLOR       m_IndicatorEngColor;   // Indicator text color for English
	D3DCOLOR       m_IndicatorBkColor;    // Indicator text background color

	// Edit-control-specific data
	int            m_nIndicatorWidth;     // Width of the indicator symbol
	RECT           m_rcIndicator;         // Rectangle for drawing the indicator button
};


//-----------------------------------------------------------------------------
// Manages shared resources of dialogs
// Use DXUTGetGlobalDialogResourceManager() to get access to the global instance
//-----------------------------------------------------------------------------
class CDXUTDialogResourceManager
{
public:
	int AddFont( LPCWSTR strFaceName, LONG height, LONG weight );
	int AddTexture( LPCWSTR strFilename );

	DXUTFontNode*     GetFontNode( int iIndex )     { return m_FontCache.GetAt( iIndex ); };
	DXUTTextureNode*  GetTextureNode( int iIndex )  { return m_TextureCache.GetAt( iIndex ); };    


	~CDXUTDialogResourceManager();

protected:
	friend CDXUTDialogResourceManager* DXUTGetGlobalDialogResourceManager();

	// Use DXUTGetGlobalDialogResourceManager() to get access to the global instance
	CDXUTDialogResourceManager();

	CGrowableArray< DXUTTextureNode* > m_TextureCache;   // Shared textures
	CGrowableArray< DXUTFontNode* > m_FontCache;         // Shared fonts

	// Resource creation helpers
	HRESULT CreateFont( UINT index );
	HRESULT CreateTexture( UINT index );
};

CDXUTDialogResourceManager* DXUTGetGlobalDialogResourceManager();


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

class UIParent
{
public:
	UIParent(void);
	~UIParent(void);

	bool OnWndMsg(const CEGUI::EventArgs&);
	LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void DeviceReset_Direct3D92(HWND window, CEGUI::Renderer* renderer);
	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	bool Init(HWND hWnd, LPDIRECT3DDEVICE9 lpDevice);
	bool Shutdown();
	void Draw();

	//----------------------------------------------------------------------------//

	CEGUI::GUIContext*			m_pGUI;
	CEGUI::DefaultWindow*		m_pRoot;
	CDXUTDialog					*m_pDialog;
	HWND						m_hWnd;
	LPDIRECT3DDEVICE9			m_lpDevice;

private:
	
};


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
