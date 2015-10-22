/************************************************************************/
/*                                                                      */
/************************************************************************/
#include <cstdlib>
#include <string>
#include <wchar.h>
#include <stdlib.h>
#include <locale.h>

#include "UIParent.h"

#include "CEGUI\GeometryBuffer.h"
#include "CEGUI\Font.h"
#include "CEGUI\BasicImage.h"

#include "CEGUI\RendererModules\Direct3D9\Renderer.h"
#include "CEGUI\Renderer.h"

#include "CEGUI\Vector.h"
#include "CEGUI\Colour.h"
#include "CEGUI\Vertex.h"
#include "CEGUI\System.h"

#include "CEGUI\Texture.h"
#include "CEGUI\FontManager.h"
#include "CEGUI\DefaultResourceProvider.h"
#include "CEGUI\ImageManager.h"
#include "CEGUI\Scheme.h"
#include "CEGUI\falagard\WidgetLookManager.h"
#include "CEGUI\WindowManager.h"
#include "CEGUI\ScriptModule.h"
#include "CEGUI\AnimationManager.h"
#include "CEGUI\XMLParser.h"
#include "immapi.h"

using namespace immapi;
//////////////////////////////////////////////////////////////////////////
const char* getDataPathPrefix() 
{
	return "D:\\work\\BldUI\\cegui-0.8.4\\datafiles";
}

//////////////////////////////////////////////////////////////////////////

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))
//--------------------------------------------------------------------------------------
// Performs timer operations
// Use DXUTGetGlobalTimer() to get the global instance
//--------------------------------------------------------------------------------------
class CDXUTTimer
{
public:
	CDXUTTimer();

	void Reset(); // resets the timer
	void Start(); // starts the timer
	void Stop();  // stop (or pause) the timer
	void Advance(); // advance the timer by 0.1 seconds
	double GetAbsoluteTime(); // get the absolute system time
	double GetTime(); // get the current time
	double GetElapsedTime(); // get the time that elapsed between GetElapsedTime() calls
	bool IsStopped(); // returns true if timer stopped

protected:
	bool m_bUsingQPF;
	bool m_bTimerStopped;
	LONGLONG m_llQPFTicksPerSec;

	LONGLONG m_llStopTime;
	LONGLONG m_llLastElapsedTime;
	LONGLONG m_llBaseTime;
};


//--------------------------------------------------------------------------------------
CDXUTTimer::CDXUTTimer()
{
	m_bUsingQPF         = false;
	m_bTimerStopped     = true;
	m_llQPFTicksPerSec  = 0;

	m_llStopTime        = 0;
	m_llLastElapsedTime = 0;
	m_llBaseTime        = 0;

	// Use QueryPerformanceFrequency() to get frequency of timer.  
	LARGE_INTEGER qwTicksPerSec;
	m_bUsingQPF = (bool) (QueryPerformanceFrequency( &qwTicksPerSec ) != 0);
	m_llQPFTicksPerSec = qwTicksPerSec.QuadPart;
}


//--------------------------------------------------------------------------------------
void CDXUTTimer::Reset()
{
	if( !m_bUsingQPF )
		return;

	// Get either the current time or the stop time
	LARGE_INTEGER qwTime;
	if( m_llStopTime != 0 )
		qwTime.QuadPart = m_llStopTime;
	else
		QueryPerformanceCounter( &qwTime );

	m_llBaseTime        = qwTime.QuadPart;
	m_llLastElapsedTime = qwTime.QuadPart;
	m_llStopTime        = 0;
	m_bTimerStopped     = FALSE;
}


//--------------------------------------------------------------------------------------
void CDXUTTimer::Start()
{
	if( !m_bUsingQPF )
		return;

	// Get the current time
	LARGE_INTEGER qwTime;
	QueryPerformanceCounter( &qwTime );

	if( m_bTimerStopped )
		m_llBaseTime += qwTime.QuadPart - m_llStopTime;
	m_llStopTime = 0;
	m_llLastElapsedTime = qwTime.QuadPart;
	m_bTimerStopped = FALSE;
}


//--------------------------------------------------------------------------------------
void CDXUTTimer::Stop()
{
	if( !m_bUsingQPF )
		return;

	if( !m_bTimerStopped )
	{
		// Get either the current time or the stop time
		LARGE_INTEGER qwTime;
		if( m_llStopTime != 0 )
			qwTime.QuadPart = m_llStopTime;
		else
			QueryPerformanceCounter( &qwTime );

		m_llStopTime = qwTime.QuadPart;
		m_llLastElapsedTime = qwTime.QuadPart;
		m_bTimerStopped = TRUE;
	}
}


//--------------------------------------------------------------------------------------
void CDXUTTimer::Advance()
{
	if( !m_bUsingQPF )
		return;

	m_llStopTime += m_llQPFTicksPerSec/10;
}


//--------------------------------------------------------------------------------------
double CDXUTTimer::GetAbsoluteTime()
{
	if( !m_bUsingQPF )
		return -1.0;

	// Get either the current time or the stop time
	LARGE_INTEGER qwTime;
	if( m_llStopTime != 0 )
		qwTime.QuadPart = m_llStopTime;
	else
		QueryPerformanceCounter( &qwTime );

	double fTime = qwTime.QuadPart / (double) m_llQPFTicksPerSec;

	return fTime;
}


//--------------------------------------------------------------------------------------
double CDXUTTimer::GetTime()
{
	if( !m_bUsingQPF )
		return -1.0;

	// Get either the current time or the stop time
	LARGE_INTEGER qwTime;
	if( m_llStopTime != 0 )
		qwTime.QuadPart = m_llStopTime;
	else
		QueryPerformanceCounter( &qwTime );

	double fAppTime = (double) ( qwTime.QuadPart - m_llBaseTime ) / (double) m_llQPFTicksPerSec;

	return fAppTime;
}


//--------------------------------------------------------------------------------------
double CDXUTTimer::GetElapsedTime()
{
	if( !m_bUsingQPF )
		return -1.0;

	// Get either the current time or the stop time
	LARGE_INTEGER qwTime;
	if( m_llStopTime != 0 )
		qwTime.QuadPart = m_llStopTime;
	else
		QueryPerformanceCounter( &qwTime );

	double fElapsedTime = (double) ( qwTime.QuadPart - m_llLastElapsedTime ) / (double) m_llQPFTicksPerSec;
	m_llLastElapsedTime = qwTime.QuadPart;

	return fElapsedTime;
}


//--------------------------------------------------------------------------------------
bool CDXUTTimer::IsStopped()
{
	return m_bTimerStopped;
}





CDXUTTimer* DXUTGetGlobalTimer()
{
	// Using an accessor function gives control of the construction order
	static CDXUTTimer timer;
	return &timer;
}


//--------------------------------------------------------------------------------------
// CDXUTControl class
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
CDXUTControl::CDXUTControl( CDXUTDialog *pDialog )
: CEGUI::Window("", "")
{
	m_Type = DXUT_CONTROL_BUTTON;
	m_pDialog = pDialog;
	m_ID = 0;
	m_Index = 0;
	m_pUserData = NULL;

	m_bEnabled = true;
	m_bVisible = true;
	m_bMouseOver = false;
	m_bHasFocus = false;
	m_bIsDefault = false;

	m_pDialog = NULL;

	m_x = 0;
	m_y = 0;
	m_width = 0;
	m_height = 0;

	ZeroMemory( &m_rcBoundingBox, sizeof( m_rcBoundingBox ) );
}


CDXUTControl::~CDXUTControl()
{
	for( int i = 0; i < m_Elements.GetSize(); ++i )
	{
		delete m_Elements[i];
	}
	m_Elements.RemoveAll();
}


//--------------------------------------------------------------------------------------
void CDXUTControl::SetTextColor( D3DCOLOR Color )
{
	CDXUTElement* pElement = m_Elements.GetAt( 0 );

	if( pElement )
		pElement->FontColor.States[DXUT_STATE_NORMAL] = Color;
}


//--------------------------------------------------------------------------------------
HRESULT CDXUTControl::SetElement( UINT iElement, CDXUTElement* pElement )
{
	HRESULT hr = S_OK;

	if( pElement == NULL )
		return E_INVALIDARG;

	// Make certain the array is this large
	for( UINT i=m_Elements.GetSize(); i <= iElement; i++ )
	{
		CDXUTElement* pNewElement = new CDXUTElement();
		if( pNewElement == NULL )
			return E_OUTOFMEMORY;

		hr = m_Elements.Add( pNewElement );
		if( FAILED(hr) )
			return hr;
	}

	// Update the data
	CDXUTElement* pCurElement = m_Elements.GetAt( iElement );
	*pCurElement = *pElement;

	return S_OK;
}


//--------------------------------------------------------------------------------------
void CDXUTControl::Refresh()
{
	m_bMouseOver = false;
	m_bHasFocus = false;

	for( int i=0; i < m_Elements.GetSize(); i++ )
	{
		CDXUTElement* pElement = m_Elements.GetAt( i );
		pElement->Refresh();
	}
}


//--------------------------------------------------------------------------------------
void CDXUTControl::UpdateRects()
{
	SetRect( &m_rcBoundingBox, 0, 0, m_width, m_height );
}

void CDXUTControl::SetLocation( int x, int y )
{
	m_x = x; 
	m_y = y; 
	UpdateRects();
	setPosition(CEGUI::UVector2(cegui_absdim(x),cegui_absdim(y))); 
}

void CDXUTControl::SetSize( int width, int height )
{
	m_width = width; 
	m_height = height; 
	UpdateRects();
	setSize(CEGUI::USize(cegui_absdim(width),cegui_absdim(width)));
}

void CDXUTControl::onActivated( CEGUI::ActivationEventArgs& e )
{
	CEGUI::Window::onActivated(e);
	m_bHasFocus = true;
}

void CDXUTControl::onDeactivated( CEGUI::ActivationEventArgs& e )
{
	CEGUI::Window::onDeactivated(e);
	m_bHasFocus = false;
}
// 
 void CDXUTControl::onMouseMove( CEGUI::MouseEventArgs& e )
 {
	 CEGUI::Window::onMouseMove(e);
	 e.handled = 0;
 }
 
 void CDXUTControl::onMouseButtonDown( CEGUI::MouseEventArgs& e )
 {
	 CEGUI::Window::onMouseButtonDown(e);
	e.handled = 0;
 }
 
 void CDXUTControl::onMouseButtonUp( CEGUI::MouseEventArgs& e )
 {
 	CEGUI::Window::onMouseButtonUp(e);
	 e.handled = 0;
 }
 
 void CDXUTControl::onMouseClicked( CEGUI::MouseEventArgs& e )
 {
	 CEGUI::Window::onMouseButtonUp(e);
	 e.handled = 0;
 }
 
 void CDXUTControl::onKeyUp( CEGUI::KeyEventArgs& e )
 {
	 e.handled = 0;
 }
 
 void CDXUTControl::onKeyDown( CEGUI::KeyEventArgs& e )
 {
	 e.handled = 0;
 }

void CDXUTControl::SetID( int ID )
{
	m_ID = ID;
	wchar_t name[10];
	wsprintf(name, L"ctrl-%d", m_ID);
	this->setName(CEGUI::String(name));
}

 void CDXUTControl::onCharacter( CEGUI::KeyEventArgs& e )
 {
 	Window::onCharacter(e);
 	if (e.handled==0)
 	{
 		e.handled = MsgProc(WM_CHAR, e.codepoint, 0) ? 1:0;
 	}
}

 BOOL CDXUTControl::ContainsPoint( POINT pt )
 {
	 using namespace CEGUI;
	 return isHit(Vector2f(pt.x, pt.y))?TRUE:FALSE;
 }

// Static member initialization
HINSTANCE CDXUTEditBox::CUniBuffer::s_hDll = NULL;



//--------------------------------------------------------------------------------------
// CDXUTEditBox class
//--------------------------------------------------------------------------------------

// When scrolling, EDITBOX_SCROLLEXTENT is reciprocal of the amount to scroll.
// If EDITBOX_SCROLLEXTENT = 4, then we scroll 1/4 of the control each time.
#define EDITBOX_SCROLLEXTENT 4

//--------------------------------------------------------------------------------------
CDXUTEditBox::CDXUTEditBox( CDXUTDialog *pDialog )
{
	m_Type = DXUT_CONTROL_EDITBOX;
	m_pDialog = pDialog;

	m_nBorder = 5;  // Default border width
	m_nSpacing = 4;  // Default spacing

	m_bCaretOn = true;
	m_dfBlink = GetCaretBlinkTime() * 0.001f;
	m_dfLastBlink = DXUTGetGlobalTimer()->GetAbsoluteTime();
	s_bHideCaret = false;
	m_nFirstVisible = 0;
	m_TextColor = D3DCOLOR_ARGB( 255, 16, 16, 16 );
	m_SelTextColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );
	m_SelBkColor = D3DCOLOR_ARGB( 255, 40, 50, 92 );
	m_CaretColor = D3DCOLOR_ARGB( 255, 0, 0, 0 );
	m_nCaret = m_nSelStart = 0;
	m_bInsertMode = true;

	m_bMouseDrag = false;
}


//--------------------------------------------------------------------------------------
CDXUTEditBox::~CDXUTEditBox()
{
}


//--------------------------------------------------------------------------------------
// PlaceCaret: Set the caret to a character position, and adjust the scrolling if
//             necessary.
//--------------------------------------------------------------------------------------
void CDXUTEditBox::PlaceCaret( int nCP )
{
	assert( nCP >= 0 && nCP <= m_Buffer.GetTextSize() );
	m_nCaret = nCP;

	// Obtain the X offset of the character.
	int nX1st, nX, nX2;
	m_Buffer.CPtoX( m_nFirstVisible, FALSE, &nX1st );  // 1st visible char
	m_Buffer.CPtoX( nCP, FALSE, &nX );  // LEAD
	// If nCP is the NULL terminator, get the leading edge instead of trailing.
	if( nCP == m_Buffer.GetTextSize() )
		nX2 = nX;
	else
		m_Buffer.CPtoX( nCP, TRUE, &nX2 );  // TRAIL

	// If the left edge of the char is smaller than the left edge of the 1st visible char,
	// we need to scroll left until this char is visible.
	if( nX < nX1st )
	{
		// Simply make the first visible character the char at the new caret position.
		m_nFirstVisible = nCP;
	}
	else
		// If the right of the character is bigger than the offset of the control's
		// right edge, we need to scroll right to this character.
		if( nX2 > nX1st + RectWidth( m_rcText ) )
		{
			// Compute the X of the new left-most pixel
			int nXNewLeft = nX2 - RectWidth( m_rcText );

			// Compute the char position of this character
			int nCPNew1st, nNewTrail;
			m_Buffer.XtoCP( nXNewLeft, &nCPNew1st, &nNewTrail );

			// If this coordinate is not on a character border,
			// start from the next character so that the caret
			// position does not fall outside the text rectangle.
			int nXNew1st;
			m_Buffer.CPtoX( nCPNew1st, FALSE, &nXNew1st );
			if( nXNew1st < nXNewLeft )
				++nCPNew1st;

			m_nFirstVisible = nCPNew1st;
		}
}


//--------------------------------------------------------------------------------------
void CDXUTEditBox::ClearText()
{
	m_Buffer.Clear();
	PlaceCaret( 0 );
	m_nSelStart = 0;
}


//--------------------------------------------------------------------------------------
void CDXUTEditBox::SetText( LPCWSTR wszText, bool bSelected )
{
	assert( wszText != NULL );

	m_Buffer.SetText( wszText );
	// Move the caret to the end of the text
	PlaceCaret( m_Buffer.GetTextSize() );
	m_nSelStart = bSelected ? 0 : m_nCaret;
}


//--------------------------------------------------------------------------------------
HRESULT CDXUTEditBox::GetTextCopy( LPWSTR strDest, UINT bufferCount )
{
	assert( strDest );

	StringCchCopy( strDest, bufferCount, m_Buffer.GetBuffer() );

	return S_OK;
}


//--------------------------------------------------------------------------------------
void CDXUTEditBox::DeleteSelectionText()
{
	int nFirst = min( m_nCaret, m_nSelStart );
	int nLast = max( m_nCaret, m_nSelStart );
	// Update caret and selection
	PlaceCaret( nFirst );
	m_nSelStart = m_nCaret;
	// Remove the characters
	for( int i = nFirst; i < nLast; ++i )
		m_Buffer.RemoveChar( nFirst );
}


//--------------------------------------------------------------------------------------
void CDXUTEditBox::UpdateRects()
{
	CDXUTControl::UpdateRects();

	// Update the text rectangle
	m_rcText = m_rcBoundingBox;
	// First inflate by m_nBorder to compute render rects
	InflateRect( &m_rcText, -m_nBorder, -m_nBorder );

	// Update the render rectangles
	m_rcRender[0] = m_rcText;
	SetRect( &m_rcRender[1], m_rcBoundingBox.left, m_rcBoundingBox.top, m_rcText.left, m_rcText.top );
	SetRect( &m_rcRender[2], m_rcText.left, m_rcBoundingBox.top, m_rcText.right, m_rcText.top );
	SetRect( &m_rcRender[3], m_rcText.right, m_rcBoundingBox.top, m_rcBoundingBox.right, m_rcText.top );
	SetRect( &m_rcRender[4], m_rcBoundingBox.left, m_rcText.top, m_rcText.left, m_rcText.bottom );
	SetRect( &m_rcRender[5], m_rcText.right, m_rcText.top, m_rcBoundingBox.right, m_rcText.bottom );
	SetRect( &m_rcRender[6], m_rcBoundingBox.left, m_rcText.bottom, m_rcText.left, m_rcBoundingBox.bottom );
	SetRect( &m_rcRender[7], m_rcText.left, m_rcText.bottom, m_rcText.right, m_rcBoundingBox.bottom );
	SetRect( &m_rcRender[8], m_rcText.right, m_rcText.bottom, m_rcBoundingBox.right, m_rcBoundingBox.bottom );

	// Inflate further by m_nSpacing
	InflateRect( &m_rcText, -m_nSpacing, -m_nSpacing );
}


void CDXUTEditBox::CopyToClipboard()
{
	// Copy the selection text to the clipboard
	if( m_nCaret != m_nSelStart && OpenClipboard( NULL ) )
	{
		EmptyClipboard();

		HGLOBAL hBlock = GlobalAlloc( GMEM_MOVEABLE, sizeof(WCHAR) * ( m_Buffer.GetTextSize() + 1 ) );
		if( hBlock )
		{
			WCHAR *pwszText = (WCHAR*)GlobalLock( hBlock );
			if( pwszText )
			{
				int nFirst = min( m_nCaret, m_nSelStart );
				int nLast = max( m_nCaret, m_nSelStart );
				if( nLast - nFirst > 0 )
					CopyMemory( pwszText, m_Buffer.GetBuffer() + nFirst, (nLast - nFirst) * sizeof(WCHAR) );
				pwszText[nLast - nFirst] = L'\0';  // Terminate it
				GlobalUnlock( hBlock );
			}
			SetClipboardData( CF_UNICODETEXT, hBlock );
		}
		CloseClipboard();
		// We must not free the object until CloseClipboard is called.
		if( hBlock )
			GlobalFree( hBlock );
	}
}


void CDXUTEditBox::PasteFromClipboard()
{
	DeleteSelectionText();

	if( OpenClipboard( NULL ) )
	{
		HANDLE handle = GetClipboardData( CF_UNICODETEXT );
		if( handle )
		{
			// Convert the ANSI string to Unicode, then
			// insert to our buffer.
			WCHAR *pwszText = (WCHAR*)GlobalLock( handle );
			if( pwszText )
			{
				// Copy all characters up to null.
				if( m_Buffer.InsertString( m_nCaret, pwszText ) )
					PlaceCaret( m_nCaret + lstrlenW( pwszText ) );
				m_nSelStart = m_nCaret;
				GlobalUnlock( handle );
			}
		}
		CloseClipboard();
	}
}


bool CDXUTEditBox::s_bHideCaret;

//--------------------------------------------------------------------------------------
bool CDXUTEditBox::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !m_bEnabled || !m_bVisible )
		return false;

	bool bHandled = false;

	switch( uMsg )
	{
	case WM_KEYDOWN:
		{
			switch( wParam )
			{
			case VK_HOME:
				PlaceCaret( 0 );
				if( GetKeyState( VK_SHIFT ) >= 0 )
					// Shift is not down. Update selection
					// start along with the caret.
					m_nSelStart = m_nCaret;
				ResetCaretBlink();
				bHandled = true;
				break;

			case VK_END:
				PlaceCaret( m_Buffer.GetTextSize() );
				if( GetKeyState( VK_SHIFT ) >= 0 )
					// Shift is not down. Update selection
					// start along with the caret.
					m_nSelStart = m_nCaret;
				ResetCaretBlink();
				bHandled = true;
				break;

			case VK_INSERT:
				if( GetKeyState( VK_CONTROL ) < 0 )
				{
					// Control Insert. Copy to clipboard
					CopyToClipboard();
				} else
					if( GetKeyState( VK_SHIFT ) < 0 )
					{
						// Shift Insert. Paste from clipboard
						PasteFromClipboard();
					} else
					{
						// Toggle caret insert mode
						m_bInsertMode = !m_bInsertMode;
					}
					break;

			case VK_DELETE:
				// Check if there is a text selection.
				if( m_nCaret != m_nSelStart )
				{
					DeleteSelectionText();
					m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
				}
				else
				{
					// Deleting one character
					if( m_Buffer.RemoveChar( m_nCaret ) )
						m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
				}
				ResetCaretBlink();
				bHandled = true;
				break;

			case VK_LEFT:
				if( GetKeyState( VK_CONTROL ) < 0 )
				{
					// Control is down. Move the caret to a new item
					// instead of a character.
					m_Buffer.GetPriorItemPos( m_nCaret, &m_nCaret );
					PlaceCaret( m_nCaret );
				}
				else
					if( m_nCaret > 0 )
						PlaceCaret( m_nCaret - 1 );
				if( GetKeyState( VK_SHIFT ) >= 0 )
					// Shift is not down. Update selection
					// start along with the caret.
					m_nSelStart = m_nCaret;
				ResetCaretBlink();
				bHandled = true;
				break;

			case VK_RIGHT:
				if( GetKeyState( VK_CONTROL ) < 0 )
				{
					// Control is down. Move the caret to a new item
					// instead of a character.
					m_Buffer.GetNextItemPos( m_nCaret, &m_nCaret );
					PlaceCaret( m_nCaret );
				}
				else
					if( m_nCaret < m_Buffer.GetTextSize() )
						PlaceCaret( m_nCaret + 1 );
				if( GetKeyState( VK_SHIFT ) >= 0 )
					// Shift is not down. Update selection
					// start along with the caret.
					m_nSelStart = m_nCaret;
				ResetCaretBlink();
				bHandled = true;
				break;

			case VK_UP:
			case VK_DOWN:
				// Trap up and down arrows so that the dialog
				// does not switch focus to another control.
				bHandled = true;
				break;

			default:
				bHandled = wParam != VK_ESCAPE;  // Let the application handle Esc.
			}
		}
	}
	return bHandled;
}


//--------------------------------------------------------------------------------------
bool CDXUTEditBox::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
	if( !m_bEnabled || !m_bVisible )
		return false;

	CEGUI::UDim dm = this->getXPosition();
	CEGUI::Sizef sz_ = this->getRootContainerSize();
	float x_pos = pt.x - (sz_.d_width*dm.d_scale+dm.d_offset);

	switch( uMsg )
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		{
			if( !m_bHasFocus )
				m_pDialog->RequestFocus( this );

			if( !ContainsPoint( pt ) )
				return false;

			m_bMouseDrag = true;
			SetCapture( m_hWnd );
			// Determine the character corresponding to the coordinates.
			int nCP, nTrail, nX1st;
			m_Buffer.CPtoX( m_nFirstVisible, FALSE, &nX1st );  // X offset of the 1st visible char
			if( SUCCEEDED( m_Buffer.XtoCP( x_pos - m_rcText.left + nX1st, &nCP, &nTrail ) ) )
			{
				// Cap at the NULL character.
				if( nTrail && nCP < m_Buffer.GetTextSize() )
					PlaceCaret( nCP + 1 );
				else
					PlaceCaret( nCP );
				m_nSelStart = m_nCaret;
				ResetCaretBlink();
			}

			printSelf();
			return true;
		}

	case WM_LBUTTONUP:
		ReleaseCapture();
		m_bMouseDrag = false;
		return true;
		break;

	case WM_MOUSEMOVE:
		if( m_bMouseDrag )
		{
			// Determine the character corresponding to the coordinates.
			int nCP, nTrail, nX1st;
			m_Buffer.CPtoX( m_nFirstVisible, FALSE, &nX1st );  // X offset of the 1st visible char
			if( SUCCEEDED( m_Buffer.XtoCP( x_pos - m_rcText.left + nX1st, &nCP, &nTrail ) ) )
			{
				// Cap at the NULL character.
				if( nTrail && nCP < m_Buffer.GetTextSize() )
					PlaceCaret( nCP + 1 );
				else
					PlaceCaret( nCP );
			}
		}
		return true;
		break;
	}

	return false;
}


//--------------------------------------------------------------------------------------
void CDXUTEditBox::OnFocusIn()
{
	CDXUTControl::OnFocusIn();

	ResetCaretBlink();
}


void CDXUTEditBox::populateGeometryBuffer()
{
	this->d_geometry->reset();
	this->Render(NULL, GetTickCount()/1000.0f);
}

void CDXUTEditBox::update( float elapsed )
{
	__super::update(elapsed);
	
	//
	// Blink the caret
	//
	if( DXUTGetGlobalTimer()->GetAbsoluteTime() - m_dfLastBlink >= m_dfBlink )
	{
		m_bCaretOn = !m_bCaretOn;
		m_dfLastBlink = DXUTGetGlobalTimer()->GetAbsoluteTime();
		// state changed, so need a redraw
		invalidate();
	}

}

void CDXUTEditBox::printSelf()
{
	printf("------>\ncaret: %d, select: %d\n", this->m_nCaret, this->m_nSelStart);

}

//--------------------------------------------------------------------------------------
bool CDXUTEditBox::MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !m_bEnabled || !m_bVisible )
		return false;

	switch( uMsg )
	{
	case WM_CHAR:
		{
			switch( (WCHAR)wParam )
			{
				// Backspace
			case VK_BACK:
				{
					// If there's a selection, treat this
					// like a delete key.
					if( m_nCaret != m_nSelStart )
					{
						DeleteSelectionText();
						m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
					}
					else
						if( m_nCaret > 0 )
						{
							// Move the caret, then delete the char.
							PlaceCaret( m_nCaret - 1 );
							m_nSelStart = m_nCaret;
							m_Buffer.RemoveChar( m_nCaret );
							m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
						}
						ResetCaretBlink();
						break;
				}

			case 24:        // Ctrl-X Cut
			case VK_CANCEL: // Ctrl-C Copy
				{
					CopyToClipboard();

					// If the key is Ctrl-X, delete the selection too.
					if( (WCHAR)wParam == 24 )
					{
						DeleteSelectionText();
						m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
					}

					break;
				}

				// Ctrl-V Paste
			case 22:
				{
					PasteFromClipboard();
					m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
					break;
				}

				// Ctrl-A Select All
			case 1:
				if( m_nSelStart == m_nCaret )
				{
					m_nSelStart = 0;
					PlaceCaret( m_Buffer.GetTextSize() );
				}
				break;

			case VK_RETURN:
				// Invoke the callback when the user presses Enter.
				m_pDialog->SendEvent( EVENT_EDITBOX_STRING, true, this );
				break;

				// Junk characters we don't want in the string
			case 26:  // Ctrl Z
			case 2:   // Ctrl B
			case 14:  // Ctrl N
			case 19:  // Ctrl S
			case 4:   // Ctrl D
			case 6:   // Ctrl F
			case 7:   // Ctrl G
			case 10:  // Ctrl J
			case 11:  // Ctrl K
			case 12:  // Ctrl L
			case 17:  // Ctrl Q
			case 23:  // Ctrl W
			case 5:   // Ctrl E
			case 18:  // Ctrl R
			case 20:  // Ctrl T
			case 25:  // Ctrl Y
			case 21:  // Ctrl U
			case 9:   // Ctrl I
			case 15:  // Ctrl O
			case 16:  // Ctrl P
			case 27:  // Ctrl [
			case 29:  // Ctrl ]
			case 28:  // Ctrl \ 
				break;

			default:
				{
					// If there's a selection and the user
					// starts to type, the selection should
					// be deleted.
					if( m_nCaret != m_nSelStart )
						DeleteSelectionText();

					// If we are in overwrite mode and there is already
					// a char at the caret's position, simply replace it.
					// Otherwise, we insert the char as normal.
					if( !m_bInsertMode && m_nCaret < m_Buffer.GetTextSize() )
					{
						m_Buffer[m_nCaret] = (WCHAR)wParam;
						PlaceCaret( m_nCaret + 1 );
						m_nSelStart = m_nCaret;
					} else
					{
						// Insert the char
						if( m_Buffer.InsertChar( m_nCaret, (WCHAR)wParam ) )
						{
							PlaceCaret( m_nCaret + 1 );
							m_nSelStart = m_nCaret;
						}
					}
					ResetCaretBlink();
					m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
				}
			}
			return true;
		}
	}
	return false;
}


//--------------------------------------------------------------------------------------
void CDXUTEditBox::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
	if( m_bVisible == false )
		return;

	HRESULT hr;
	int nSelStartX = 0, nCaretX = 0;  // Left and right X cordinates of the selection region

	CDXUTElement* pElement = GetElement( 0 );
	if( pElement )
	{
		m_Buffer.SetFontNode( m_pDialog->GetFont( pElement->iFont ) );
		PlaceCaret( m_nCaret );  // Call PlaceCaret now that we have the font info (node),
		// so that scrolling can be handled.
	}

	// Render the control graphics
	for( int e = 0; e < 9; ++e )
	{
		pElement = m_Elements.GetAt( e );
		pElement->TextureColor.Blend( DXUT_STATE_NORMAL, fElapsedTime );

		m_pDialog->DrawSprite( this->d_geometry, pElement, &m_rcRender[e] );
	}

	//
	// Compute the X coordinates of the first visible character.
	//
	int nXFirst;
	m_Buffer.CPtoX( m_nFirstVisible, FALSE, &nXFirst );

	//
	// Compute the X coordinates of the selection rectangle
	//
	hr = m_Buffer.CPtoX( m_nCaret, FALSE, &nCaretX );
	if( m_nCaret != m_nSelStart )
		hr = m_Buffer.CPtoX( m_nSelStart, FALSE, &nSelStartX );
	else
		nSelStartX = nCaretX;

	//
	// Render the selection rectangle
	//
	RECT rcSelection;  // Make this available for rendering selected text
	if( m_nCaret != m_nSelStart )
	{
		int nSelLeftX = nCaretX, nSelRightX = nSelStartX;
		// Swap if left is bigger than right
		if( nSelLeftX > nSelRightX )
		{ int nTemp = nSelLeftX; nSelLeftX = nSelRightX; nSelRightX = nTemp; }

		SetRect( &rcSelection, nSelLeftX, m_rcText.top, nSelRightX, m_rcText.bottom );
		OffsetRect( &rcSelection, m_rcText.left - nXFirst, 0 );
		IntersectRect( &rcSelection, &m_rcText, &rcSelection );
		m_pDialog->DrawRect( this->d_geometry, &rcSelection, m_SelBkColor );
	}

	//
	// Render the text
	//
	// Element 0 for text
	m_Elements.GetAt( 0 )->FontColor.Current = m_TextColor;
	m_pDialog->DrawText( this->d_geometry, m_Buffer.GetBuffer() + m_nFirstVisible, m_Elements.GetAt( 0 ), &m_rcText );

	// Render the selected text
	if( m_nCaret != m_nSelStart )
	{
		int nFirstToRender = max( m_nFirstVisible, min( m_nSelStart, m_nCaret ) );
		int nNumChatToRender = max( m_nSelStart, m_nCaret ) - nFirstToRender;
		m_Elements.GetAt( 0 )->FontColor.Current = m_SelTextColor;
		m_pDialog->DrawText(this->d_geometry,  m_Buffer.GetBuffer() + nFirstToRender,
			m_Elements.GetAt( 0 ), &rcSelection, false, nNumChatToRender );
	}

	//
	// Render the caret if this control has the focus
	//
	if( m_bHasFocus && m_bCaretOn && !s_bHideCaret )
	{
		// Start the rectangle with insert mode caret
		RECT rcCaret = { m_rcText.left - nXFirst + nCaretX - 1, m_rcText.top,
			m_rcText.left - nXFirst + nCaretX + 1, m_rcText.bottom };

		// If we are in overwrite mode, adjust the caret rectangle
		// to fill the entire character.
		if( !m_bInsertMode )
		{
			// Obtain the right edge X coord of the current character
			int nRightEdgeX;
			m_Buffer.CPtoX( m_nCaret, TRUE, &nRightEdgeX );
			rcCaret.right = m_rcText.left - nXFirst + nRightEdgeX;
		}

		m_pDialog->DrawRect( this->d_geometry, &rcCaret, m_CaretColor );
	}
}


#define IN_FLOAT_CHARSET( c ) \
	( (c) == L'-' || (c) == L'.' || ( (c) >= L'0' && (c) <= L'9' ) )

void CDXUTEditBox::ParseFloatArray( float *pNumbers, int nCount )
{
	int nWritten = 0;  // Number of floats written
	const WCHAR *pToken, *pEnd;
	WCHAR wszToken[60];

	pToken = m_Buffer.GetBuffer();
	while( nWritten < nCount && *pToken != L'\0' )
	{
		// Skip leading spaces
		while( *pToken == L' ' )
			++pToken;

		if( *pToken == L'\0' )
			break;

		// Locate the end of number
		pEnd = pToken;
		while( IN_FLOAT_CHARSET( *pEnd ) )
			++pEnd;

		// Copy the token to our buffer
		int nTokenLen = min( sizeof(wszToken) / sizeof(wszToken[0]) - 1, int(pEnd - pToken) );
		StringCchCopy( wszToken, nTokenLen, pToken );
		*pNumbers = (float)wcstod( wszToken, NULL );
		++nWritten;
		++pNumbers;
		pToken = pEnd;
	}
}


void CDXUTEditBox::SetTextFloatArray( const float *pNumbers, int nCount )
{
	WCHAR wszBuffer[512] = {0};
	WCHAR wszTmp[64];

	if( pNumbers == NULL )
		return;

	for( int i = 0; i < nCount; ++i )
	{
		StringCchPrintf( wszTmp, 64, L"%.4f ", pNumbers[i] );
		StringCchCat( wszBuffer, 512, wszTmp );
	}

	// Don't want the last space
	if( nCount > 0 && wcslen(wszBuffer) > 0 )
		wszBuffer[wcslen(wszBuffer)-1] = 0;

	SetText( wszBuffer );
}


//--------------------------------------------------------------------------------------
// CDXUTIMEEditBox class
//--------------------------------------------------------------------------------------
// IME constants
#define CHT_IMEFILENAME1    "TINTLGNT.IME" // New Phonetic
#define CHT_IMEFILENAME2    "CINTLGNT.IME" // New Chang Jie
#define CHT_IMEFILENAME3    "MSTCIPHA.IME" // Phonetic 5.1
#define CHS_IMEFILENAME1    "PINTLGNT.IME" // MSPY1.5/2/3
#define CHS_IMEFILENAME2    "MSSCIPYA.IME" // MSPY3 for OfficeXP

#define LANG_CHT            MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL)
#define LANG_CHS            MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED)
#define _CHT_HKL            ( (HKL)(INT_PTR)0xE0080404 ) // New Phonetic
#define _CHT_HKL2           ( (HKL)(INT_PTR)0xE0090404 ) // New Chang Jie
#define _CHS_HKL            ( (HKL)(INT_PTR)0xE00E0804 ) // MSPY
#define MAKEIMEVERSION( major, minor ) \
	( (DWORD)( ( (BYTE)( major ) << 24 ) | ( (BYTE)( minor ) << 16 ) ) )

#define IMEID_CHT_VER42 ( LANG_CHT | MAKEIMEVERSION( 4, 2 ) )   // New(Phonetic/ChanJie)IME98  : 4.2.x.x // Win98
#define IMEID_CHT_VER43 ( LANG_CHT | MAKEIMEVERSION( 4, 3 ) )   // New(Phonetic/ChanJie)IME98a : 4.3.x.x // Win2k
#define IMEID_CHT_VER44 ( LANG_CHT | MAKEIMEVERSION( 4, 4 ) )   // New ChanJie IME98b          : 4.4.x.x // WinXP
#define IMEID_CHT_VER50 ( LANG_CHT | MAKEIMEVERSION( 5, 0 ) )   // New(Phonetic/ChanJie)IME5.0 : 5.0.x.x // WinME
#define IMEID_CHT_VER51 ( LANG_CHT | MAKEIMEVERSION( 5, 1 ) )   // New(Phonetic/ChanJie)IME5.1 : 5.1.x.x // IME2002(w/OfficeXP)
#define IMEID_CHT_VER52 ( LANG_CHT | MAKEIMEVERSION( 5, 2 ) )   // New(Phonetic/ChanJie)IME5.2 : 5.2.x.x // IME2002a(w/Whistler)
#define IMEID_CHT_VER60 ( LANG_CHT | MAKEIMEVERSION( 6, 0 ) )   // New(Phonetic/ChanJie)IME6.0 : 6.0.x.x // IME XP(w/WinXP SP1)
#define IMEID_CHS_VER41 ( LANG_CHS | MAKEIMEVERSION( 4, 1 ) )   // MSPY1.5  // SCIME97 or MSPY1.5 (w/Win98, Office97)
#define IMEID_CHS_VER42 ( LANG_CHS | MAKEIMEVERSION( 4, 2 ) )   // MSPY2    // Win2k/WinME
#define IMEID_CHS_VER53 ( LANG_CHS | MAKEIMEVERSION( 5, 3 ) )   // MSPY3    // WinXP


HKL       CDXUTIMEEditBox::s_hklCurrent;     // Current keyboard layout of the process
bool      CDXUTIMEEditBox::s_bVerticalCand;  // Indicates that the candidates are listed vertically
WCHAR     CDXUTIMEEditBox::s_aszIndicator[5][3] = // String to draw to indicate current input locale
{
	L"En",
	L"\x7B80",
	L"\x7E41",
	L"\xAC00",
	L"\x3042",
};
LPWSTR    CDXUTIMEEditBox::s_wszCurrIndicator   // Points to an indicator string that corresponds to current input locale
= CDXUTIMEEditBox::s_aszIndicator[0];
bool      CDXUTIMEEditBox::s_bInsertOnType;     // Insert the character as soon as a key is pressed (Korean behavior)
HIMC      CDXUTIMEEditBox::s_hImcDef;           // Default input context
CDXUTIMEEditBox::IMESTATE  CDXUTIMEEditBox::s_ImeState = IMEUI_STATE_OFF;
bool      CDXUTIMEEditBox::s_bEnableImeSystem;  // Whether the IME system is active
POINT     CDXUTIMEEditBox::s_ptCompString;      // Composition string position. Updated every frame.
int       CDXUTIMEEditBox::s_nCompCaret;
int       CDXUTIMEEditBox::s_nFirstTargetConv;  // Index of the first target converted char in comp string.  If none, -1.
CDXUTEditBox::CUniBuffer CDXUTIMEEditBox::s_CompString = CDXUTEditBox::CUniBuffer( MAX_COMPSTRING_SIZE );
BYTE      CDXUTIMEEditBox::s_abCompStringAttr[MAX_COMPSTRING_SIZE];
DWORD     CDXUTIMEEditBox::s_adwCompStringClause[MAX_COMPSTRING_SIZE];
WCHAR     CDXUTIMEEditBox::s_wszReadingString[32];
CDXUTIMEEditBox::CCandList CDXUTIMEEditBox::s_CandList;       // Data relevant to the candidate list
bool      CDXUTIMEEditBox::s_bShowReadingWindow; // Indicates whether reading window is visible
bool      CDXUTIMEEditBox::s_bHorizontalReading; // Indicates whether the reading window is vertical or horizontal
bool      CDXUTIMEEditBox::s_bChineseIME;
CGrowableArray< CDXUTIMEEditBox::CInputLocale > CDXUTIMEEditBox::s_Locale; // Array of loaded keyboard layout on system


//--------------------------------------------------------------------------------------
CDXUTIMEEditBox::CDXUTIMEEditBox( CDXUTDialog *pDialog )
{
	m_Type = DXUT_CONTROL_IMEEDITBOX;
	m_pDialog = pDialog;

	s_bEnableImeSystem = true;
	m_nIndicatorWidth = 0;
	m_ReadingColor = D3DCOLOR_ARGB( 188, 255, 255, 255 );
	m_ReadingWinColor = D3DCOLOR_ARGB( 128, 0, 0, 0 );
	m_ReadingSelColor = D3DCOLOR_ARGB( 255, 255, 0, 0 );
	m_ReadingSelBkColor = D3DCOLOR_ARGB( 128, 80, 80, 80 );
	m_CandidateColor = D3DCOLOR_ARGB( 255, 200, 200, 200 );
	m_CandidateWinColor = D3DCOLOR_ARGB( 128, 0, 0, 0 );
	m_CandidateSelColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );
	m_CandidateSelBkColor = D3DCOLOR_ARGB( 128, 158, 158, 158 );
	m_CompColor = D3DCOLOR_ARGB( 255, 200, 200, 255 );
	m_CompWinColor = D3DCOLOR_ARGB( 198, 0, 0, 0 );
	m_CompCaretColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );
	m_CompTargetColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );
	m_CompTargetBkColor = D3DCOLOR_ARGB( 255, 150, 150, 150 );
	m_CompTargetNonColor = D3DCOLOR_ARGB( 255, 255, 255, 0 );
	m_CompTargetNonBkColor = D3DCOLOR_ARGB( 255, 150, 150, 150 );
	m_IndicatorImeColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );
	m_IndicatorEngColor = D3DCOLOR_ARGB( 255, 0, 0, 0 );
	m_IndicatorBkColor = D3DCOLOR_ARGB( 255, 128, 128, 128 );
}


//--------------------------------------------------------------------------------------
CDXUTIMEEditBox::~CDXUTIMEEditBox()
{
}


//--------------------------------------------------------------------------------------
void CDXUTIMEEditBox::SendKey( BYTE nVirtKey )
{
	keybd_event( nVirtKey, 0, 0,               0 );
	keybd_event( nVirtKey, 0, KEYEVENTF_KEYUP, 0 );
}


//--------------------------------------------------------------------------------------
// Called by CDXUTResourceCache's OnCreateDevice.  This gives the class a
// chance to initialize its default input context associated with the app window.
HRESULT CDXUTIMEEditBox::StaticOnCreateDevice(HWND hWnd)
{
	// Save the default input context
	s_hImcDef = _ImmGetContext( hWnd );
	_ImmReleaseContext( hWnd, s_hImcDef );

	return S_OK;
}


//--------------------------------------------------------------------------------------
void CDXUTIMEEditBox::UpdateRects()
{
	// Temporary adjust m_width so that CDXUTEditBox can compute
	// the correct rects for its rendering since we need to make space
	// for the indicator button
	int nWidth = m_width;
	m_width -= m_nIndicatorWidth + m_nBorder * 2; // Make room for the indicator button
	CDXUTEditBox::UpdateRects();
	m_width = nWidth;  // Restore

	// Compute the indicator button rectangle
	SetRect( &m_rcIndicator, m_rcBoundingBox.right, m_rcBoundingBox.top, m_x + m_width, m_rcBoundingBox.bottom );
	//    InflateRect( &m_rcIndicator, -m_nBorder, -m_nBorder );
	m_rcBoundingBox.right = m_rcBoundingBox.left + m_width;
}


//--------------------------------------------------------------------------------------
//  GetImeId( UINT uIndex )
//      returns 
//  returned value:
//  0: In the following cases
//      - Non Chinese IME input locale
//      - Older Chinese IME
//      - Other error cases
//
//  Othewise:
//      When uIndex is 0 (default)
//          bit 31-24:  Major version
//          bit 23-16:  Minor version
//          bit 15-0:   Language ID
//      When uIndex is 1
//          pVerFixedInfo->dwFileVersionLS
//
//  Use IMEID_VER and IMEID_LANG macro to extract version and language information.
//  

// We define the locale-invariant ID ourselves since it doesn't exist prior to WinXP
// For more information, see the CompareString() reference.
#define LCID_INVARIANT MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT)

DWORD CDXUTIMEEditBox::GetImeId( UINT uIndex )
{
	static HKL hklPrev = 0;
	static DWORD dwID[2] = { 0, 0 };  // Cache the result

	DWORD   dwVerSize;
	DWORD   dwVerHandle;
	LPVOID  lpVerBuffer;
	LPVOID  lpVerData;
	UINT    cbVerData;
	char    szTmp[1024];

	if( uIndex >= sizeof( dwID ) / sizeof( dwID[0] ) )
		return 0;

	if( hklPrev == s_hklCurrent )
		return dwID[uIndex];

	hklPrev = s_hklCurrent;  // Save for the next invocation

	// Check if we are using an older Chinese IME
	if( !( ( s_hklCurrent == _CHT_HKL ) || ( s_hklCurrent == _CHT_HKL2 ) || ( s_hklCurrent == _CHS_HKL ) ) )
	{
		dwID[0] = dwID[1] = 0;
		return dwID[uIndex];
	}

	// Obtain the IME file name
	if ( !_ImmGetIMEFileNameA( s_hklCurrent, szTmp, ( sizeof(szTmp) / sizeof(szTmp[0]) ) - 1 ) )
	{
		dwID[0] = dwID[1] = 0;
		return dwID[uIndex];
	}

	// Check for IME that doesn't implement reading string API
	if ( !_GetReadingString )
	{
		if( ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHT_IMEFILENAME1, -1 ) != CSTR_EQUAL ) &&
			( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHT_IMEFILENAME2, -1 ) != CSTR_EQUAL ) &&
			( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHT_IMEFILENAME3, -1 ) != CSTR_EQUAL ) &&
			( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHS_IMEFILENAME1, -1 ) != CSTR_EQUAL ) &&
			( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHS_IMEFILENAME2, -1 ) != CSTR_EQUAL ) )
		{
			dwID[0] = dwID[1] = 0;
			return dwID[uIndex];
		}
	}

	dwVerSize = _GetFileVersionInfoSizeA( szTmp, &dwVerHandle );
	if( dwVerSize )
	{
		lpVerBuffer = HeapAlloc( GetProcessHeap(), 0, dwVerSize );
		if( lpVerBuffer )
		{
			if( _GetFileVersionInfoA( szTmp, dwVerHandle, dwVerSize, lpVerBuffer ) )
			{
				if( _VerQueryValueA( lpVerBuffer, "\\", &lpVerData, &cbVerData ) )
				{
					DWORD dwVer = ( (VS_FIXEDFILEINFO*)lpVerData )->dwFileVersionMS;
					dwVer = ( dwVer & 0x00ff0000 ) << 8 | ( dwVer & 0x000000ff ) << 16;
					if( _GetReadingString
						||
						( GetLanguage() == LANG_CHT &&
						( dwVer == MAKEIMEVERSION(4, 2) || 
						dwVer == MAKEIMEVERSION(4, 3) || 
						dwVer == MAKEIMEVERSION(4, 4) || 
						dwVer == MAKEIMEVERSION(5, 0) ||
						dwVer == MAKEIMEVERSION(5, 1) ||
						dwVer == MAKEIMEVERSION(5, 2) ||
						dwVer == MAKEIMEVERSION(6, 0) ) )
						||
						( GetLanguage() == LANG_CHS &&
						( dwVer == MAKEIMEVERSION(4, 1) ||
						dwVer == MAKEIMEVERSION(4, 2) ||
						dwVer == MAKEIMEVERSION(5, 3) ) )
						)
					{
						dwID[0] = dwVer | GetLanguage();
						dwID[1] = ( (VS_FIXEDFILEINFO*)lpVerData )->dwFileVersionLS;
					}
				}
			}
			HeapFree( GetProcessHeap(), 0, lpVerBuffer );
		}
	}

	return dwID[uIndex];
}


//--------------------------------------------------------------------------------------
void CDXUTIMEEditBox::CheckInputLocale()
{
	static HKL hklPrev = 0;
	s_hklCurrent = GetKeyboardLayout( 0 );
	if ( hklPrev == s_hklCurrent )
		return;

	hklPrev = s_hklCurrent;
	switch ( GetPrimaryLanguage() )
	{
		// Simplified Chinese
	case LANG_CHINESE:
		s_bVerticalCand = true;
		switch ( GetSubLanguage() )
		{
		case SUBLANG_CHINESE_SIMPLIFIED:
			s_wszCurrIndicator = s_aszIndicator[INDICATOR_CHS];
			s_bVerticalCand = GetImeId() == 0;
			break;
		case SUBLANG_CHINESE_TRADITIONAL:
			s_wszCurrIndicator = s_aszIndicator[INDICATOR_CHT];
			break;
		default:    // unsupported sub-language
			s_wszCurrIndicator = s_aszIndicator[INDICATOR_NON_IME];
			break;
		}
		break;
		// Korean
	case LANG_KOREAN:
		s_wszCurrIndicator = s_aszIndicator[INDICATOR_KOREAN];
		s_bVerticalCand = false;
		break;
		// Japanese
	case LANG_JAPANESE:
		s_wszCurrIndicator = s_aszIndicator[INDICATOR_JAPANESE];
		s_bVerticalCand = true;
		break;
	default:
		// A non-IME language.  Obtain the language abbreviation
		// and store it for rendering the indicator later.
		s_wszCurrIndicator = s_aszIndicator[INDICATOR_NON_IME];
	}

	// If non-IME, use the language abbreviation.
	if( s_wszCurrIndicator == s_aszIndicator[INDICATOR_NON_IME] )
	{
		WCHAR wszLang[5];
		GetLocaleInfoW( MAKELCID( LOWORD( s_hklCurrent ), SORT_DEFAULT ), LOCALE_SABBREVLANGNAME, wszLang, 5 );
		s_wszCurrIndicator[0] = wszLang[0];
		s_wszCurrIndicator[1] = towlower( wszLang[1] );
	}
}


//--------------------------------------------------------------------------------------
void CDXUTIMEEditBox::CheckToggleState(HWND hWnd)
{
	CheckInputLocale();
	bool bIme = _ImmIsIME( s_hklCurrent ) != 0;
	s_bChineseIME = ( GetPrimaryLanguage() == LANG_CHINESE ) && bIme;

	HIMC hImc;
	if( NULL != ( hImc = _ImmGetContext( hWnd ) ) )
	{
		if( s_bChineseIME )
		{
			DWORD dwConvMode, dwSentMode;
			_ImmGetConversionStatus( hImc, &dwConvMode, &dwSentMode );
			s_ImeState = ( dwConvMode & IME_CMODE_NATIVE ) ? IMEUI_STATE_ON : IMEUI_STATE_ENGLISH;
		}
		else
		{
			s_ImeState = ( bIme && _ImmGetOpenStatus( hImc ) != 0 ) ? IMEUI_STATE_ON : IMEUI_STATE_OFF;
		}
		_ImmReleaseContext( hWnd, hImc );
	}
	else
		s_ImeState = IMEUI_STATE_OFF;
}


//--------------------------------------------------------------------------------------
// Enable/disable the entire IME system.  When disabled, the default IME handling
// kicks in.
void CDXUTIMEEditBox::EnableImeSystem( bool bEnable )
{
	s_bEnableImeSystem = bEnable;
}




//--------------------------------------------------------------------------------------
// Resets the composition string.
void CDXUTIMEEditBox::ResetCompositionString()
{
	s_nCompCaret = 0;
	s_CompString.SetText( L"" );
	ZeroMemory( s_abCompStringAttr, sizeof(s_abCompStringAttr) );
}


//--------------------------------------------------------------------------------------
// Truncate composition string by sending keystrokes to the window.
void CDXUTIMEEditBox::TruncateCompString( bool bUseBackSpace, int iNewStrLen )
{
	if( !s_bInsertOnType )
		return;

	int cc = (int) wcslen( s_CompString.GetBuffer() );
	assert( iNewStrLen == 0 || iNewStrLen >= cc );

	// Send right arrow keystrokes to move the caret
	//   to the end of the composition string.
	for (int i = 0; i < cc - s_nCompCaret; ++i )
		SendMessage( m_hWnd, WM_KEYDOWN, VK_RIGHT, 0 );
	SendMessage( m_hWnd, WM_KEYUP, VK_RIGHT, 0 );

	if( bUseBackSpace || m_bInsertMode )
		iNewStrLen = 0;

	// The caller sets bUseBackSpace to false if there's possibility of sending
	// new composition string to the app right after this function call.
	// 
	// If the app is in overwriting mode and new comp string is 
	// shorter than current one, delete previous comp string 
	// till it's same long as the new one. Then move caret to the beginning of comp string.
	// New comp string will overwrite old one.
	if( iNewStrLen < cc )
	{
		for( int i = 0; i < cc - iNewStrLen; ++i )
		{
			SendMessage( m_hWnd, WM_KEYDOWN, VK_BACK, 0 );  // Backspace character
			SendMessageW( m_hWnd, WM_CHAR, VK_BACK, 0 );
		}
		SendMessage( m_hWnd, WM_KEYUP, VK_BACK, 0 );
	}
	else
		iNewStrLen = cc;

	// Move the caret to the beginning by sending left keystrokes
	for (int i = 0; i < iNewStrLen; ++i )
		SendMessage( m_hWnd, WM_KEYDOWN, VK_LEFT, 0 );
	SendMessage( m_hWnd, WM_KEYUP, VK_LEFT, 0 );
}


//--------------------------------------------------------------------------------------
// Sends the current composition string to the application by sending keystroke
// messages.
void CDXUTIMEEditBox::SendCompString()
{
	for( int i = 0; i < lstrlen( s_CompString.GetBuffer() ); ++i )
		MsgProc( WM_CHAR, (WPARAM)s_CompString[i], 0 );
}


//--------------------------------------------------------------------------------------
// Outputs current composition string then cleans up the composition task.
void CDXUTIMEEditBox::FinalizeString( bool bSend )
{
	HIMC hImc;
	if( NULL == ( hImc = _ImmGetContext( m_hWnd) ) )
		return;

	static bool bProcessing = false;
	if( bProcessing )    // avoid infinite recursion
	{
		DXUTTRACE( L"CDXUTIMEEditBox::FinalizeString: Reentrant detected!\n" );
		_ImmReleaseContext( m_hWnd, hImc );
		return;
	}
	bProcessing = true;

	if( !s_bInsertOnType && bSend )
	{
		// Send composition string to app.
		LONG lLength = lstrlen( s_CompString.GetBuffer() );
		// In case of CHT IME, don't send the trailing double byte space, if it exists.
		if( GetLanguage() == LANG_CHT
			&& s_CompString[lLength - 1] == 0x3000 )
		{
			s_CompString[lLength - 1] = 0;
		}
		SendCompString();
	}

	ResetCompositionString();
	// Clear composition string in IME
	_ImmNotifyIME( hImc, NI_COMPOSITIONSTR, CPS_CANCEL, 0 );
	// the following line is necessary as Korean IME doesn't close cand list
	// when comp string is cancelled.
	_ImmNotifyIME( hImc, NI_CLOSECANDIDATE, 0, 0 ); 
	_ImmReleaseContext( m_hWnd, hImc );
	bProcessing = false;
}


//--------------------------------------------------------------------------------------
// Determine whether the reading window should be vertical or horizontal.
void CDXUTIMEEditBox::GetReadingWindowOrientation( HWND hWnd, DWORD dwId )
{
	s_bHorizontalReading = ( s_hklCurrent == _CHS_HKL ) || ( s_hklCurrent == _CHT_HKL2 ) || ( dwId == 0 );
	if( !s_bHorizontalReading && ( dwId & 0x0000FFFF ) == LANG_CHT )
	{
		WCHAR wszRegPath[MAX_PATH];
		HKEY hKey;
		DWORD dwVer = dwId & 0xFFFF0000;
		StringCchCopy( wszRegPath, MAX_PATH, L"software\\microsoft\\windows\\currentversion\\" );
		StringCchCat( wszRegPath, MAX_PATH, ( dwVer >= MAKEIMEVERSION( 5, 1 ) ) ? L"MSTCIPH" : L"TINTLGNT" );
		LONG lRc = RegOpenKeyExW( HKEY_CURRENT_USER, wszRegPath, 0, KEY_READ, &hKey );
		if (lRc == ERROR_SUCCESS)
		{
			DWORD dwSize = sizeof(DWORD), dwMapping, dwType;
			lRc = RegQueryValueExW( hKey, L"Keyboard Mapping", NULL, &dwType, (PBYTE)&dwMapping, &dwSize );
			if (lRc == ERROR_SUCCESS)
			{
				if ( ( dwVer <= MAKEIMEVERSION( 5, 0 ) && 
					( (BYTE)dwMapping == 0x22 || (BYTE)dwMapping == 0x23 ) )
					||
					( ( dwVer == MAKEIMEVERSION( 5, 1 ) || dwVer == MAKEIMEVERSION( 5, 2 ) ) &&
					(BYTE)dwMapping >= 0x22 && (BYTE)dwMapping <= 0x24 )
					)
				{
					s_bHorizontalReading = true;
				}
			}
			RegCloseKey( hKey );
		}
	}
}


//--------------------------------------------------------------------------------------
// Obtain the reading string upon WM_IME_NOTIFY/INM_PRIVATE notification.
void CDXUTIMEEditBox::GetPrivateReadingString(HWND hWnd)
{
	DWORD dwId = GetImeId();
	if( !dwId )
	{
		s_bShowReadingWindow = false;
		return;
	}

	HIMC hImc;
	hImc = _ImmGetContext( hWnd );
	if( !hImc )
	{
		s_bShowReadingWindow = false;
		return;
	}

	DWORD dwReadingStrLen = 0;
	DWORD dwErr = 0;
	WCHAR *pwszReadingStringBuffer = NULL;  // Buffer for when the IME supports GetReadingString()
	WCHAR *wstr = 0;
	bool bUnicodeIme = false;  // Whether the IME context component is Unicode.
	INPUTCONTEXT *lpIC = NULL;

	if( _GetReadingString )
	{
		UINT uMaxUiLen;
		BOOL bVertical;
		// Obtain the reading string size
		dwReadingStrLen = _GetReadingString( hImc, 0, NULL, (PINT)&dwErr, &bVertical, &uMaxUiLen );
		if( dwReadingStrLen )
		{
			wstr = pwszReadingStringBuffer = (LPWSTR)HeapAlloc( GetProcessHeap(), 0, sizeof(WCHAR) * dwReadingStrLen );
			if( !pwszReadingStringBuffer )
			{
				// Out of memory. Exit.
				_ImmReleaseContext( hWnd, hImc );
				return;
			}

			// Obtain the reading string
			dwReadingStrLen = _GetReadingString( hImc, dwReadingStrLen, wstr, (PINT)&dwErr, &bVertical, &uMaxUiLen );
		}

		s_bHorizontalReading = !bVertical;
		bUnicodeIme = true;
	}
	else
	{
		// IMEs that doesn't implement Reading String API

		lpIC = _ImmLockIMC( hImc );

		LPBYTE p = 0;
		switch( dwId )
		{
		case IMEID_CHT_VER42: // New(Phonetic/ChanJie)IME98  : 4.2.x.x // Win98
		case IMEID_CHT_VER43: // New(Phonetic/ChanJie)IME98a : 4.3.x.x // WinMe, Win2k
		case IMEID_CHT_VER44: // New ChanJie IME98b          : 4.4.x.x // WinXP
			p = *(LPBYTE *)((LPBYTE)_ImmLockIMCC( lpIC->hPrivate ) + 24 );
			if( !p ) break;
			dwReadingStrLen = *(DWORD *)( p + 7 * 4 + 32 * 4 );
			dwErr = *(DWORD *)( p + 8 * 4 + 32 * 4 );
			wstr = (WCHAR *)( p + 56 );
			bUnicodeIme = true;
			break;

		case IMEID_CHT_VER50: // 5.0.x.x // WinME
			p = *(LPBYTE *)( (LPBYTE)_ImmLockIMCC( lpIC->hPrivate ) + 3 * 4 );
			if( !p ) break;
			p = *(LPBYTE *)( (LPBYTE)p + 1*4 + 5*4 + 4*2 );
			if( !p ) break;
			dwReadingStrLen = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16);
			dwErr = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 + 1*4);
			wstr = (WCHAR *)(p + 1*4 + (16*2+2*4) + 5*4);
			bUnicodeIme = false;
			break;

		case IMEID_CHT_VER51: // 5.1.x.x // IME2002(w/OfficeXP)
		case IMEID_CHT_VER52: // 5.2.x.x // (w/whistler)
		case IMEID_CHS_VER53: // 5.3.x.x // SCIME2k or MSPY3 (w/OfficeXP and Whistler)
			p = *(LPBYTE *)((LPBYTE)_ImmLockIMCC( lpIC->hPrivate ) + 4);
			if( !p ) break;
			p = *(LPBYTE *)((LPBYTE)p + 1*4 + 5*4);
			if( !p ) break;
			dwReadingStrLen = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * 2);
			dwErr = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * 2 + 1*4);
			wstr  = (WCHAR *) (p + 1*4 + (16*2+2*4) + 5*4);
			bUnicodeIme = true;
			break;

			// the code tested only with Win 98 SE (MSPY 1.5/ ver 4.1.0.21)
		case IMEID_CHS_VER41:
			{
				int nOffset;
				nOffset = ( GetImeId( 1 ) >= 0x00000002 ) ? 8 : 7;

				p = *(LPBYTE *)((LPBYTE)_ImmLockIMCC( lpIC->hPrivate ) + nOffset * 4);
				if( !p ) break;
				dwReadingStrLen = *(DWORD *)(p + 7*4 + 16*2*4);
				dwErr = *(DWORD *)(p + 8*4 + 16*2*4);
				dwErr = min( dwErr, dwReadingStrLen );
				wstr = (WCHAR *)(p + 6*4 + 16*2*1);
				bUnicodeIme = true;
				break;
			}

		case IMEID_CHS_VER42: // 4.2.x.x // SCIME98 or MSPY2 (w/Office2k, Win2k, WinME, etc)
			{
				OSVERSIONINFOW osi;
				osi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOW);
				GetVersionExW( &osi );

				int nTcharSize = ( osi.dwPlatformId == VER_PLATFORM_WIN32_NT ) ? sizeof(WCHAR) : sizeof(char);
				p = *(LPBYTE *)((LPBYTE)_ImmLockIMCC( lpIC->hPrivate ) + 1*4 + 1*4 + 6*4);
				if( !p ) break;
				dwReadingStrLen = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * nTcharSize);
				dwErr = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * nTcharSize + 1*4);
				wstr  = (WCHAR *) (p + 1*4 + (16*2+2*4) + 5*4);
				bUnicodeIme = ( osi.dwPlatformId == VER_PLATFORM_WIN32_NT ) ? true : false;
			}
		}   // switch
	}

	// Copy the reading string to the candidate list first
	s_CandList.awszCandidate[0][0] = 0;
	s_CandList.awszCandidate[1][0] = 0;
	s_CandList.awszCandidate[2][0] = 0;
	s_CandList.awszCandidate[3][0] = 0;
	s_CandList.dwCount = dwReadingStrLen;
	s_CandList.dwSelection = (DWORD)-1; // do not select any char
	if( bUnicodeIme )
	{
		UINT i;
		for( i = 0; i < dwReadingStrLen; ++i ) // dwlen > 0, if known IME
		{
			if( dwErr <= i && s_CandList.dwSelection == (DWORD)-1 )
			{
				// select error char
				s_CandList.dwSelection = i;
			}

			s_CandList.awszCandidate[i][0] = wstr[i];
			s_CandList.awszCandidate[i][1] = 0;
		}
		s_CandList.awszCandidate[i][0] = 0;
	}
	else
	{
		char *p = (char *)wstr;
		DWORD i, j;
		for( i = 0, j = 0; i < dwReadingStrLen; ++i, ++j ) // dwlen > 0, if known IME
		{
			if( dwErr <= i && s_CandList.dwSelection == (DWORD)-1 )
			{
				s_CandList.dwSelection = j;
			}
			// Obtain the current code page
			WCHAR wszCodePage[8];
			UINT uCodePage = CP_ACP;  // Default code page
			if( GetLocaleInfoW( MAKELCID( GetLanguage(), SORT_DEFAULT ),
				LOCALE_IDEFAULTANSICODEPAGE,
				wszCodePage,
				sizeof(wszCodePage)/sizeof(wszCodePage[0]) ) )
			{
				uCodePage = wcstoul( wszCodePage, NULL, 0 );
			}
			MultiByteToWideChar( uCodePage, 0, p + i, IsDBCSLeadByteEx( uCodePage, p[i] ) ? 2 : 1,
				s_CandList.awszCandidate[j], 1 );
			if( IsDBCSLeadByteEx( uCodePage, p[i] ) )
				++i;
		}
		s_CandList.awszCandidate[j][0] = 0;
		s_CandList.dwCount = j;
	}
	if( !_GetReadingString )
	{
		_ImmUnlockIMCC( lpIC->hPrivate );
		_ImmUnlockIMC( hImc );
		GetReadingWindowOrientation( hWnd, dwId );
	}
	_ImmReleaseContext( hWnd, hImc );

	if( pwszReadingStringBuffer )
		HeapFree( GetProcessHeap(), 0, pwszReadingStringBuffer );

	// Copy the string to the reading string buffer
	if( s_CandList.dwCount > 0 )
		s_bShowReadingWindow = true;
	else
		s_bShowReadingWindow = false;
	if( s_bHorizontalReading )
	{
		s_CandList.nReadingError = -1;
		s_wszReadingString[0] = 0;
		for( UINT i = 0; i < s_CandList.dwCount; ++i )
		{
			if( s_CandList.dwSelection == i )
				s_CandList.nReadingError = lstrlen( s_wszReadingString );
			StringCchCat( s_wszReadingString, 32, s_CandList.awszCandidate[i] );
		}
	}

	s_CandList.dwPageSize = MAX_CANDLIST;
}


//--------------------------------------------------------------------------------------
// This function is used only briefly in CHT IME handling,
// so accelerator isn't processed.
void CDXUTIMEEditBox::PumpMessage()
{
	MSG msg;

	while( PeekMessageW( &msg, NULL, 0, 0, PM_NOREMOVE ) )
	{
		if( !GetMessageW( &msg, NULL, 0, 0 ) )
		{
			PostQuitMessage( (int)msg.wParam );
			return;
		}
		TranslateMessage( &msg );
		DispatchMessageA( &msg );
	}
}


//--------------------------------------------------------------------------------------
void CDXUTIMEEditBox::OnFocusIn()
{
	CDXUTEditBox::OnFocusIn();

	if( s_bEnableImeSystem )
	{
		_ImmAssociateContext( m_hWnd, s_hImcDef );
		CheckToggleState(m_hWnd );
	} else
		_ImmAssociateContext( m_hWnd, NULL );

	//
	// Set up the IME global state according to the current instance state
	//
	HIMC hImc;
	if( NULL != ( hImc = _ImmGetContext( m_hWnd ) ) ) 
	{
		if( !s_bEnableImeSystem )
			s_ImeState = IMEUI_STATE_OFF;

		_ImmReleaseContext( m_hWnd, hImc );
		CheckToggleState(m_hWnd );
	}
}


//--------------------------------------------------------------------------------------
void CDXUTIMEEditBox::OnFocusOut()
{
	CDXUTEditBox::OnFocusOut();

	FinalizeString( false );  // Don't send the comp string as to match RichEdit behavior

	_ImmAssociateContext( m_hWnd, NULL );
}


//--------------------------------------------------------------------------------------
bool CDXUTIMEEditBox::StaticMsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	HIMC hImc;

	if( !s_bEnableImeSystem )
		return false;

	switch( uMsg )
	{
	case WM_ACTIVATEAPP:
		if( wParam )
		{
			// Populate s_Locale with the list of keyboard layouts.
			UINT cKL = GetKeyboardLayoutList( 0, NULL );
			s_Locale.RemoveAll();
			HKL *phKL = new HKL[cKL];
			if( phKL )
			{
				GetKeyboardLayoutList( cKL, phKL );
				for( UINT i = 0; i < cKL; ++i )
				{
					CInputLocale Locale;

					// Filter out East Asian languages that are not IME.
					if( ( PRIMARYLANGID( LOWORD( phKL[i] ) ) == LANG_CHINESE ||
						PRIMARYLANGID( LOWORD( phKL[i] ) ) == LANG_JAPANESE ||
						PRIMARYLANGID( LOWORD( phKL[i] ) ) == LANG_KOREAN ) &&
						!_ImmIsIME( phKL[i] ) )
						continue;

					// If this language is already in the list, don't add it again.
					bool bBreak = false;
					for( int e = 0; e < s_Locale.GetSize(); ++e )
						if( LOWORD( s_Locale.GetAt( e ).m_hKL ) ==
							LOWORD( phKL[i] ) )
						{
							bBreak = true;
							break;
						}
						if( bBreak )
							break;

						Locale.m_hKL = phKL[i];
						WCHAR wszDesc[128] = L"";
						switch( PRIMARYLANGID( LOWORD( phKL[i] ) ) )
						{
							// Simplified Chinese
						case LANG_CHINESE:
							switch( SUBLANGID( LOWORD( phKL[i] ) ) )
							{
							case SUBLANG_CHINESE_SIMPLIFIED:
								StringCchCopy( Locale.m_wszLangAbb, 3, s_aszIndicator[INDICATOR_CHS] );
								break;
							case SUBLANG_CHINESE_TRADITIONAL:
								StringCchCopy( Locale.m_wszLangAbb, 3, s_aszIndicator[INDICATOR_CHT] );
								break;
							default:    // unsupported sub-language
								GetLocaleInfoW( MAKELCID( LOWORD( phKL[i] ), SORT_DEFAULT ), LOCALE_SABBREVLANGNAME, wszDesc, 128 );
								Locale.m_wszLangAbb[0] = wszDesc[0];
								Locale.m_wszLangAbb[1] = towlower( wszDesc[1] );
								Locale.m_wszLangAbb[2] = L'\0';
								break;
							}
							break;
							// Korean
						case LANG_KOREAN:
							StringCchCopy( Locale.m_wszLangAbb, 3, s_aszIndicator[INDICATOR_KOREAN] );
							break;
							// Japanese
						case LANG_JAPANESE:
							StringCchCopy( Locale.m_wszLangAbb, 3, s_aszIndicator[INDICATOR_JAPANESE] );
							break;         
						default:
							// A non-IME language.  Obtain the language abbreviation
							// and store it for rendering the indicator later.
							GetLocaleInfoW( MAKELCID( LOWORD( phKL[i] ), SORT_DEFAULT ), LOCALE_SABBREVLANGNAME, wszDesc, 128 );
							Locale.m_wszLangAbb[0] = wszDesc[0];
							Locale.m_wszLangAbb[1] = towlower( wszDesc[1] );
							Locale.m_wszLangAbb[2] = L'\0';
							break;
						}

						GetLocaleInfoW( MAKELCID( LOWORD( phKL[i] ), SORT_DEFAULT ), LOCALE_SLANGUAGE, wszDesc, 128 );
						StringCchCopy( Locale.m_wszLang, 64, wszDesc );

						s_Locale.Add( Locale );
				}
				delete[] phKL;
			}
		}
		break;

	case WM_INPUTLANGCHANGE:
		DXUTTRACE( L"WM_INPUTLANGCHANGE\n" );
		{
			UINT uLang = GetPrimaryLanguage();
			CheckToggleState(hWnd);
			if ( uLang != GetPrimaryLanguage() )
			{
				// Korean IME always inserts on keystroke.  Other IMEs do not.
				s_bInsertOnType = ( GetPrimaryLanguage() == LANG_KOREAN );
			}

			// IME changed.  Setup the new IME.
			SetupImeApi(s_hklCurrent);
			if( _ShowReadingWindow )
			{
				if ( NULL != ( hImc = _ImmGetContext( hWnd ) ) )
				{
					_ShowReadingWindow( hImc, false );
					_ImmReleaseContext( hWnd, hImc );
				}
			}
		}
		return true;

	case WM_IME_SETCONTEXT:
		DXUTTRACE( L"WM_IME_SETCONTEXT\n" );
		//
		// We don't want anything to display, so we have to clear this
		//
		lParam = 0;
		return false;

		// Handle WM_IME_STARTCOMPOSITION here since
		// we do not want the default IME handler to see
		// this when our fullscreen app is running.
	case WM_IME_STARTCOMPOSITION:
		DXUTTRACE( L"WM_IME_STARTCOMPOSITION\n" );
		ResetCompositionString();
		// Since the composition string has its own caret, we don't render
		// the edit control's own caret to avoid double carets on screen.
		s_bHideCaret = true;
		return true;

	case WM_IME_COMPOSITION:
		DXUTTRACE( L"WM_IME_COMPOSITION\n" );
		return true;
	}

	return false;
}


//--------------------------------------------------------------------------------------
bool CDXUTIMEEditBox::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
	if( !m_bEnabled || !m_bVisible )
		return false;

	switch( uMsg )
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		{
			DXUTFontNode* pFont = m_pDialog->GetFont( m_Elements.GetAt( 9 )->iFont );

			// Check if this click is on top of the composition string
			int nCompStrWidth;
			s_CompString.CPtoX( s_CompString.GetTextSize(), FALSE, &nCompStrWidth );

			if( s_ptCompString.x <= pt.x &&
				s_ptCompString.y <= pt.y &&
				s_ptCompString.x + nCompStrWidth > pt.x &&
				s_ptCompString.y + pFont->nHeight > pt.y )
			{
				int nCharBodyHit, nCharHit;
				int nTrail;

				// Determine the character clicked on.
				s_CompString.XtoCP( pt.x - s_ptCompString.x, &nCharBodyHit, &nTrail );
				if( nTrail && nCharBodyHit < s_CompString.GetTextSize() )
					nCharHit = nCharBodyHit + 1;
				else
					nCharHit = nCharBodyHit;

				// Now generate keypress events to move the comp string cursor
				// to the click point.  First, if the candidate window is displayed,
				// send Esc to close it.
				HIMC hImc = _ImmGetContext( m_hWnd );
				if( !hImc )
					return true;

				_ImmNotifyIME( hImc, NI_CLOSECANDIDATE, 0, 0 );
				_ImmReleaseContext( m_hWnd, hImc );

				switch( GetPrimaryLanguage() )
				{
				case LANG_JAPANESE:
					// For Japanese, there are two cases.  If s_nFirstTargetConv is
					// -1, the comp string hasn't been converted yet, and we use
					// s_nCompCaret.  For any other value of s_nFirstTargetConv,
					// the string has been converted, so we use clause information.

					if( s_nFirstTargetConv != -1 )
					{
						int nClauseClicked = 0;
						while( (int)s_adwCompStringClause[nClauseClicked + 1] <= nCharBodyHit )
							++nClauseClicked;

						int nClauseSelected = 0;
						while( (int)s_adwCompStringClause[nClauseSelected + 1] <= s_nFirstTargetConv )
							++nClauseSelected;

						BYTE nVirtKey = nClauseClicked > nClauseSelected ? VK_RIGHT : VK_LEFT;
						int nSendCount = abs( nClauseClicked - nClauseSelected );
						while( nSendCount-- > 0 )
							SendKey( nVirtKey );

						return true;
					}

					// Not converted case. Fall thru to Chinese case.

				case LANG_CHINESE:
					{
						// For Chinese, use s_nCompCaret.
						BYTE nVirtKey = nCharHit > s_nCompCaret ? VK_RIGHT : VK_LEFT;
						int nSendCount = abs( nCharHit - s_nCompCaret );
						while( nSendCount-- > 0 )
							SendKey( nVirtKey );
						break;
					}
				}

				return true;
			}

			// Check if the click is on top of the candidate window
			if( s_CandList.bShowWindow && PtInRect( &s_CandList.rcCandidate, pt ) )
			{
				if( s_bVerticalCand )
				{
					// Vertical candidate window

					// Compute the row the click is on
					int nRow = ( pt.y - s_CandList.rcCandidate.top ) / pFont->nHeight;

					if( nRow < (int)s_CandList.dwCount )
					{
						// nRow is a valid entry.
						// Now emulate keystrokes to select the candidate at this row.
						switch( GetPrimaryLanguage() )
						{
						case LANG_CHINESE:
						case LANG_KOREAN:
							// For Chinese and Korean, simply send the number keystroke.
							SendKey( '0' + nRow + 1 );
							break;

						case LANG_JAPANESE:
							// For Japanese, move the selection to the target row,
							// then send Right, then send Left.

							BYTE nVirtKey;
							if( nRow > (int)s_CandList.dwSelection )
								nVirtKey = VK_DOWN;
							else
								nVirtKey = VK_UP;
							int nNumToHit = abs( int( nRow - s_CandList.dwSelection ) );
							for( int nStrike = 0; nStrike < nNumToHit; ++nStrike )
								SendKey( nVirtKey );

							// Do this to close the candidate window without ending composition.
							SendKey( VK_RIGHT );
							SendKey( VK_LEFT );

							break;
						}
					}
				} else
				{
					// Horizontal candidate window

					// Determine which the character the click has hit.
					int nCharHit;
					int nTrail;
					s_CandList.HoriCand.XtoCP( pt.x - s_CandList.rcCandidate.left, &nCharHit, &nTrail );

					// Determine which candidate string the character belongs to.
					int nCandidate = s_CandList.dwCount - 1;

					int nEntryStart = 0;
					for( UINT i = 0; i < s_CandList.dwCount; ++i )
					{
						if( nCharHit >= nEntryStart )
						{
							// Haven't found it.
							nEntryStart += lstrlenW( s_CandList.awszCandidate[i] ) + 1;  // plus space separator
						} else
						{
							// Found it.  This entry starts at the right side of the click point,
							// so the char belongs to the previous entry.
							nCandidate = i - 1;
							break;
						}
					}

					// Now emulate keystrokes to select the candidate entry.
					switch( GetPrimaryLanguage() )
					{
					case LANG_CHINESE:
					case LANG_KOREAN:
						// For Chinese and Korean, simply send the number keystroke.
						SendKey( '0' + nCandidate + 1 );
						break;
					}
				}

				return true;
			}
		}
	}

	// If we didn't care for the msg, let the parent process it.
	return CDXUTEditBox::HandleMouse( uMsg, pt, wParam, lParam );
}


//--------------------------------------------------------------------------------------
bool CDXUTIMEEditBox::MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !m_bEnabled || !m_bVisible )
		return false;

	// TEMPORARY!!! TEMPORARY!!! TEMPORARY!!!
	// See how I can get rid of trapped later.
	bool trappedData;
	bool *trapped = &trappedData;

	HIMC hImc;
	static LPARAM lAlt = 0x80000000, lCtrl = 0x80000000, lShift = 0x80000000;

	*trapped = false;
	if( !s_bEnableImeSystem )
		return CDXUTEditBox::MsgProc( uMsg, wParam, lParam );

	switch( uMsg )
	{
		//
		//  IME Handling
		//
	case WM_IME_COMPOSITION:
		DXUTTRACE( L"WM_IME_COMPOSITION\n" );
		{
			LONG lRet;  // Returned count in CHARACTERS
			WCHAR wszCompStr[MAX_COMPSTRING_SIZE];

			*trapped = true;
			if( NULL == ( hImc = _ImmGetContext( m_hWnd) ) )
			{
				break;
			}

			// Get the caret position in composition string
			if ( lParam & GCS_CURSORPOS )
			{
				s_nCompCaret = _ImmGetCompositionStringW( hImc, GCS_CURSORPOS, NULL, 0 );
				if( s_nCompCaret < 0 )
					s_nCompCaret = 0; // On error, set caret to pos 0.
			}

			// ResultStr must be processed before composition string.
			//
			// This is because for some IMEs, such as CHT, pressing Enter
			// to complete the composition sends WM_IME_COMPOSITION with both
			// GCS_RESULTSTR and GCS_COMPSTR.  Retrieving the result string
			// gives the correct string, while retrieving the comp string
			// (GCS_COMPSTR) gives empty string.  GCS_RESULTSTR should be
			// handled first so that the application receives the string.  Then
			// GCS_COMPSTR can be handled to clear the comp string buffer.

			if ( lParam & GCS_RESULTSTR )
			{
				DXUTTRACE( L"  GCS_RESULTSTR\n" );
				lRet = _ImmGetCompositionStringW( hImc, GCS_RESULTSTR, wszCompStr, sizeof( wszCompStr ) );
				if( lRet > 0 )
				{
					lRet /= sizeof(WCHAR);
					wszCompStr[lRet] = 0;  // Force terminate
					TruncateCompString( false, (int)wcslen( wszCompStr ) );
					s_CompString.SetText( wszCompStr );
					SendCompString();
					ResetCompositionString();
				}
			}

			//
			// Reads in the composition string.
			//
			if ( lParam & GCS_COMPSTR )
			{
				DXUTTRACE( L"  GCS_COMPSTR\n" );
				//////////////////////////////////////////////////////
				// Retrieve the latest user-selected IME candidates
				lRet = _ImmGetCompositionStringW( hImc, GCS_COMPSTR, wszCompStr, sizeof( wszCompStr ) );
				if( lRet > 0 )
				{
					lRet /= sizeof(WCHAR);  // Convert size in byte to size in char
					wszCompStr[lRet] = 0;  // Force terminate
					//
					// Remove the whole of the string
					//
					TruncateCompString( false, (int)wcslen( wszCompStr ) );

					s_CompString.SetText( wszCompStr );

					// Older CHT IME uses composition string for reading string
					if ( GetLanguage() == LANG_CHT && !GetImeId() )
					{
						if( lstrlen( s_CompString.GetBuffer() ) )
						{
							s_CandList.dwCount = 4;             // Maximum possible length for reading string is 4
							s_CandList.dwSelection = (DWORD)-1; // don't select any candidate

							// Copy the reading string to the candidate list
							for( int i = 3; i >= 0; --i )
							{
								if( i > lstrlen( s_CompString.GetBuffer() ) - 1 )
									s_CandList.awszCandidate[i][0] = 0;  // Doesn't exist
								else
								{
									s_CandList.awszCandidate[i][0] = s_CompString[i];
									s_CandList.awszCandidate[i][1] = 0;
								}
							}
							s_CandList.dwPageSize = MAX_CANDLIST;
							// Clear comp string after we are done copying
							ZeroMemory( (LPVOID)s_CompString.GetBuffer(), 4 * sizeof(WCHAR) );
							s_bShowReadingWindow = true;
							GetReadingWindowOrientation( m_hWnd, 0 );
							if( s_bHorizontalReading )
							{
								s_CandList.nReadingError = -1;  // Clear error

								// Create a string that consists of the current
								// reading string.  Since horizontal reading window
								// is used, we take advantage of this by rendering
								// one string instead of several.
								//
								// Copy the reading string from the candidate list
								// to the reading string buffer.
								s_wszReadingString[0] = 0;
								for( UINT i = 0; i < s_CandList.dwCount; ++i )
								{
									if( s_CandList.dwSelection == i )
										s_CandList.nReadingError = lstrlen( s_wszReadingString );
									StringCchCat( s_wszReadingString, 32, s_CandList.awszCandidate[i] );
								}
							}
						}
						else
						{
							s_CandList.dwCount = 0;
							s_bShowReadingWindow = false;
						}
					}

					if( s_bInsertOnType )
					{
						// Send composition string to the edit control
						SendCompString();
						// Restore the caret to the correct location.
						// It's at the end right now, so compute the number
						// of times left arrow should be pressed to
						// send it to the original position.
						int nCount = lstrlen( s_CompString.GetBuffer() + s_nCompCaret );
						// Send left keystrokes
						for( int i = 0; i < nCount; ++i )
							SendMessage( m_hWnd, WM_KEYDOWN, VK_LEFT, 0 );
						SendMessage( m_hWnd, WM_KEYUP, VK_LEFT, 0 );
					}
				}

				ResetCaretBlink();
			}

			// Retrieve comp string attributes
			if( lParam & GCS_COMPATTR )
			{
				lRet = _ImmGetCompositionStringW( hImc, GCS_COMPATTR, s_abCompStringAttr, sizeof( s_abCompStringAttr ) );
				if( lRet > 0 )
					s_abCompStringAttr[lRet] = 0;  // ??? Is this needed for attributes?
			}

			// Retrieve clause information
			if( lParam & GCS_COMPCLAUSE )
			{
				lRet = _ImmGetCompositionStringW(hImc, GCS_COMPCLAUSE, s_adwCompStringClause, sizeof( s_adwCompStringClause ) );
				s_adwCompStringClause[lRet / sizeof(DWORD)] = 0;  // Terminate
			}

			_ImmReleaseContext( m_hWnd, hImc );
		}
		break;

	case WM_IME_ENDCOMPOSITION:
		DXUTTRACE( L"WM_IME_ENDCOMPOSITION\n" );
		TruncateCompString();
		ResetCompositionString();
		// We can show the edit control's caret again.
		s_bHideCaret = false;
		// Hide reading window
		s_bShowReadingWindow = false;
		break;

	case WM_IME_NOTIFY:
		DXUTTRACE( L"WM_IME_NOTIFY %u\n", wParam );
		switch( wParam )
		{
		case IMN_SETCONVERSIONMODE:
			DXUTTRACE( L"  IMN_SETCONVERSIONMODE\n" );
		case IMN_SETOPENSTATUS:
			DXUTTRACE( L"  IMN_SETOPENSTATUS\n" );
			CheckToggleState(m_hWnd);
			break;

		case IMN_OPENCANDIDATE:
		case IMN_CHANGECANDIDATE:
			{
				DXUTTRACE( wParam == IMN_CHANGECANDIDATE ? L"  IMN_CHANGECANDIDATE\n" : L"  IMN_OPENCANDIDATE\n" );

				s_CandList.bShowWindow = true;
				*trapped = true;
				if( NULL == ( hImc = _ImmGetContext( m_hWnd) ) )
					break;

				LPCANDIDATELIST lpCandList = NULL;
				DWORD dwLenRequired;

				s_bShowReadingWindow = false;
				// Retrieve the candidate list
				dwLenRequired = _ImmGetCandidateListW( hImc, 0, NULL, 0 );
				if( dwLenRequired )
				{
					lpCandList = (LPCANDIDATELIST)HeapAlloc( GetProcessHeap(), 0, dwLenRequired );
					dwLenRequired = _ImmGetCandidateListW( hImc, 0, lpCandList, dwLenRequired );
				}

				if( lpCandList )
				{
					// Update candidate list data
					s_CandList.dwSelection = lpCandList->dwSelection;
					s_CandList.dwCount = lpCandList->dwCount;

					int nPageTopIndex = 0;
					s_CandList.dwPageSize = min( lpCandList->dwPageSize, MAX_CANDLIST );
					if( GetPrimaryLanguage() == LANG_JAPANESE )
					{
						// Japanese IME organizes its candidate list a little
						// differently from the other IMEs.
						nPageTopIndex = ( s_CandList.dwSelection / s_CandList.dwPageSize ) * s_CandList.dwPageSize;
					}
					else
						nPageTopIndex = lpCandList->dwPageStart;

					// Make selection index relative to first entry of page
					s_CandList.dwSelection = ( GetLanguage() == LANG_CHS && !GetImeId() ) ? (DWORD)-1
						: s_CandList.dwSelection - nPageTopIndex;

					ZeroMemory( s_CandList.awszCandidate, sizeof(s_CandList.awszCandidate) );
					for( UINT i = nPageTopIndex, j = 0;
						(DWORD)i < lpCandList->dwCount && j < s_CandList.dwPageSize;
						i++, j++ )
					{
						// Initialize the candidate list strings
						LPWSTR pwsz = s_CandList.awszCandidate[j];
						// For every candidate string entry,
						// write [index] + Space + [String] if vertical,
						// write [index] + [String] + Space if horizontal.
						*pwsz++ = (WCHAR)( L'0' + ( (j + 1) % 10 ) );  // Index displayed is 1 based
						if( s_bVerticalCand )
							*pwsz++ = L' ';
						WCHAR *pwszNewCand = (LPWSTR)( (LPBYTE)lpCandList + lpCandList->dwOffset[i] );
						while ( *pwszNewCand )
							*pwsz++ = *pwszNewCand++;
						if( !s_bVerticalCand )
							*pwsz++ = L' ';
						*pwsz = 0;  // Terminate
					}

					// Make dwCount in s_CandList be number of valid entries in the page.
					s_CandList.dwCount = lpCandList->dwCount - lpCandList->dwPageStart;
					if( s_CandList.dwCount > lpCandList->dwPageSize )
						s_CandList.dwCount = lpCandList->dwPageSize;

					HeapFree( GetProcessHeap(), 0, lpCandList );
					_ImmReleaseContext( m_hWnd, hImc );

					// Korean and old Chinese IME can't have selection.
					// User must use the number hotkey or Enter to select
					// a candidate.
					if( GetPrimaryLanguage() == LANG_KOREAN ||
						GetLanguage() == LANG_CHT && !GetImeId() )
					{
						s_CandList.dwSelection = (DWORD)-1;
					}

					// Initialize s_CandList.HoriCand if we have a
					// horizontal candidate window.
					if( !s_bVerticalCand )
					{
						WCHAR wszCand[256] = L"";

						s_CandList.nFirstSelected = 0;
						s_CandList.nHoriSelectedLen = 0;
						for( UINT i = 0; i < MAX_CANDLIST; ++i )
						{
							if( s_CandList.awszCandidate[i][0] == L'\0' )
								break;

							WCHAR wszEntry[32];
							StringCchPrintf( wszEntry, 32, L"%s ", s_CandList.awszCandidate[i] );
							// If this is the selected entry, mark its char position.
							if( s_CandList.dwSelection == i )
							{
								s_CandList.nFirstSelected = lstrlen( wszCand );
								s_CandList.nHoriSelectedLen = lstrlen( wszEntry ) - 1;  // Minus space
							}
							StringCchCat( wszCand, 256, wszEntry );
						}
						wszCand[lstrlen(wszCand) - 1] = L'\0';  // Remove the last space
						s_CandList.HoriCand.SetText( wszCand );
					}
				}
				break;
			}

		case IMN_CLOSECANDIDATE:
			{
				DXUTTRACE( L"  IMN_CLOSECANDIDATE\n" );
				s_CandList.bShowWindow = false;
				if( !s_bShowReadingWindow )
				{
					s_CandList.dwCount = 0;
					ZeroMemory( s_CandList.awszCandidate, sizeof(s_CandList.awszCandidate) );
				}
				*trapped = true;
				break;
			}

		case IMN_PRIVATE:
			DXUTTRACE( L"  IMN_PRIVATE\n" );
			{
				if( !s_CandList.bShowWindow )
					GetPrivateReadingString(m_hWnd);

				// Trap some messages to hide reading window
				DWORD dwId = GetImeId();
				switch( dwId )
				{
				case IMEID_CHT_VER42:
				case IMEID_CHT_VER43:
				case IMEID_CHT_VER44:
				case IMEID_CHS_VER41:
				case IMEID_CHS_VER42:
					if( ( lParam == 1 ) || ( lParam == 2 ) )
					{
						*trapped = true;
					}
					break;

				case IMEID_CHT_VER50:
				case IMEID_CHT_VER51:
				case IMEID_CHT_VER52:
				case IMEID_CHT_VER60:
				case IMEID_CHS_VER53:
					if( (lParam == 16) || (lParam == 17) || (lParam == 26) || (lParam == 27) || (lParam == 28) )
					{
						*trapped = true;
					}
					break;
				}
			}
			break;

		default:
			*trapped = true;
			break;
		}
		break;

		// When Text Service Framework is installed in Win2K, Alt+Shift and Ctrl+Shift combination (to switch input
		// locale / keyboard layout) doesn't send WM_KEYUP message for the key that is released first. We need to check
		// if these keys are actually up whenever we receive key up message for other keys.
	case WM_KEYUP:
	case WM_SYSKEYUP:
		if ( !( lAlt & 0x80000000 ) && wParam != VK_MENU && ( GetAsyncKeyState( VK_MENU ) & 0x8000 ) == 0 )
		{
			PostMessageW( GetFocus(), WM_KEYUP, (WPARAM)VK_MENU, ( lAlt & 0x01ff0000 ) | 0xC0000001 );
		}   
		else if ( !( lCtrl & 0x80000000 ) && wParam != VK_CONTROL && ( GetAsyncKeyState( VK_CONTROL ) & 0x8000 ) == 0 )
		{
			PostMessageW( GetFocus(), WM_KEYUP, (WPARAM)VK_CONTROL, ( lCtrl & 0x01ff0000 ) | 0xC0000001 );
		}
		else if ( !( lShift & 0x80000000 ) && wParam != VK_SHIFT && ( GetAsyncKeyState( VK_SHIFT ) & 0x8000 ) == 0 )
		{
			PostMessageW( GetFocus(), WM_KEYUP, (WPARAM)VK_SHIFT, ( lShift & 0x01ff0000 ) | 0xC0000001 );
		}
		// fall through WM_KEYDOWN / WM_SYSKEYDOWN
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		switch ( wParam )
		{
		case VK_MENU:
			lAlt = lParam;
			break;
		case VK_SHIFT:
			lShift = lParam;
			break;
		case VK_CONTROL:
			lCtrl = lParam;
			break;
		}
		//break;
		// Fall through to default case
		// so we invoke the parent.

	default:
		// Let the parent handle the message that we
		// don't handle.
		return CDXUTEditBox::MsgProc( uMsg, wParam, lParam );

	}  // switch

	return *trapped;
}


//--------------------------------------------------------------------------------------
void CDXUTIMEEditBox::RenderCandidateReadingWindow( IDirect3DDevice9* pd3dDevice, float fElapsedTime, bool bReading )
{
	RECT rc;
	UINT nNumEntries = bReading ? 4 : MAX_CANDLIST;
	D3DCOLOR TextColor, TextBkColor, SelTextColor, SelBkColor;
	int nX, nXFirst, nXComp;
	m_Buffer.CPtoX( m_nCaret, FALSE, &nX );
	m_Buffer.CPtoX( m_nFirstVisible, FALSE, &nXFirst );

	if( bReading )
	{
		TextColor = m_ReadingColor;
		TextBkColor = m_ReadingWinColor;
		SelTextColor = m_ReadingSelColor;
		SelBkColor = m_ReadingSelBkColor;
	} else
	{
		TextColor = m_CandidateColor;
		TextBkColor = m_CandidateWinColor;
		SelTextColor = m_CandidateSelColor;
		SelBkColor = m_CandidateSelBkColor;
	}

	// For Japanese IME, align the window with the first target converted character.
	// For all other IMEs, align with the caret.  This is because the caret
	// does not move for Japanese IME.
	if ( GetLanguage() == LANG_CHT && !GetImeId() )
		nXComp = 0;
	else
		if( GetPrimaryLanguage() == LANG_JAPANESE )
			s_CompString.CPtoX( s_nFirstTargetConv, FALSE, &nXComp );
		else
			s_CompString.CPtoX( s_nCompCaret, FALSE, &nXComp );

	// Compute the size of the candidate window
	int nWidthRequired = 0;
	int nHeightRequired = 0;
	int nSingleLineHeight = 0;

	if( ( s_bVerticalCand && !bReading ) ||
		( !s_bHorizontalReading && bReading ) )
	{
		// Vertical window
		for( UINT i = 0; i < nNumEntries; ++i )
		{
			if( s_CandList.awszCandidate[i][0] == L'\0' )
				break;
			SetRect( &rc, 0, 0, 0, 0 );
			m_pDialog->CalcTextRect( s_CandList.awszCandidate[i], m_Elements.GetAt( 1 ), &rc );
			nWidthRequired = max( nWidthRequired, rc.right - rc.left );
			nSingleLineHeight = max( nSingleLineHeight, rc.bottom - rc.top );
		}
		nHeightRequired = nSingleLineHeight * nNumEntries;
	} else
	{
		// Horizontal window
		SetRect( &rc, 0, 0, 0, 0 );
		if( bReading )
			m_pDialog->CalcTextRect( s_wszReadingString, m_Elements.GetAt( 1 ), &rc );
		else
			m_pDialog->CalcTextRect( s_CandList.HoriCand.GetBuffer(), m_Elements.GetAt( 1 ), &rc );
		nWidthRequired = rc.right - rc.left;
		nSingleLineHeight = nHeightRequired = rc.bottom - rc.top;
	}

	// Now that we have the dimension, calculate the location for the candidate window.
	// We attempt to fit the window in this order:
	// bottom, top, right, left.

	bool bHasPosition = false;

	// Bottom
	SetRect( &rc, s_ptCompString.x + nXComp, s_ptCompString.y + m_rcText.bottom - m_rcText.top,
		s_ptCompString.x + nXComp + nWidthRequired, s_ptCompString.y + m_rcText.bottom - m_rcText.top + nHeightRequired );
	// if the right edge is cut off, move it left.
	if( rc.right > m_pDialog->GetWidth() )
	{
		rc.left -= rc.right - m_pDialog->GetWidth();
		rc.right = m_pDialog->GetWidth();
	}
	if( rc.bottom <= m_pDialog->GetHeight() )
		bHasPosition = true;

	// Top
	if( !bHasPosition )
	{
		SetRect( &rc, s_ptCompString.x + nXComp, s_ptCompString.y - nHeightRequired,
			s_ptCompString.x + nXComp + nWidthRequired, s_ptCompString.y );
		// if the right edge is cut off, move it left.
		if( rc.right > m_pDialog->GetWidth() )
		{
			rc.left -= rc.right - m_pDialog->GetWidth();
			rc.right = m_pDialog->GetWidth();
		}
		if( rc.top >= 0 )
			bHasPosition = true;
	}

	// Right
	if( !bHasPosition )
	{
		int nXCompTrail;
		s_CompString.CPtoX( s_nCompCaret, TRUE, &nXCompTrail );
		SetRect( &rc, s_ptCompString.x + nXCompTrail, 0,
			s_ptCompString.x + nXCompTrail + nWidthRequired, nHeightRequired );
		if( rc.right <= m_pDialog->GetWidth() )
			bHasPosition = true;
	}

	// Left
	if( !bHasPosition )
	{
		SetRect( &rc, s_ptCompString.x + nXComp - nWidthRequired, 0,
			s_ptCompString.x + nXComp, nHeightRequired );
		if( rc.right >= 0 )
			bHasPosition = true;
	}

	if( !bHasPosition )
	{
		// The dialog is too small for the candidate window.
		// Fall back to render at 0, 0.  Some part of the window
		// will be cut off.
		rc.left = 0;
		rc.right = nWidthRequired;
	}

	// If we are rendering the candidate window, save the position
	// so that mouse clicks are checked properly.
	if( !bReading )
		s_CandList.rcCandidate = rc;

	// Render the elements
	m_pDialog->DrawRect( this->d_geometry, &rc, TextBkColor );
	if( ( s_bVerticalCand && !bReading ) ||
		( !s_bHorizontalReading && bReading ) )
	{
		// Vertical candidate window
		for( UINT i = 0; i < nNumEntries; ++i )
		{
			// Here we are rendering one line at a time
			rc.bottom = rc.top + nSingleLineHeight;
			// Use a different color for the selected string
			if( s_CandList.dwSelection == i )
			{
				m_pDialog->DrawRect( this->d_geometry, &rc, SelBkColor );
				m_Elements.GetAt( 1 )->FontColor.Current = SelTextColor;
			} else
				m_Elements.GetAt( 1 )->FontColor.Current = TextColor;

			m_pDialog->DrawText( this->d_geometry, s_CandList.awszCandidate[i], m_Elements.GetAt( 1 ), &rc );

			rc.top += nSingleLineHeight;
		}
	} else
	{
		// Horizontal candidate window
		m_Elements.GetAt( 1 )->FontColor.Current = TextColor;
		if( bReading )
			m_pDialog->DrawText( this->d_geometry, s_wszReadingString, m_Elements.GetAt( 1 ), &rc );
		else
			m_pDialog->DrawText( this->d_geometry, s_CandList.HoriCand.GetBuffer(), m_Elements.GetAt( 1 ), &rc );

		// Render the selected entry differently
		if( !bReading )
		{
			int nXLeft, nXRight;
			s_CandList.HoriCand.CPtoX( s_CandList.nFirstSelected, FALSE, &nXLeft );
			s_CandList.HoriCand.CPtoX( s_CandList.nFirstSelected + s_CandList.nHoriSelectedLen, FALSE, &nXRight );

			rc.right = rc.left + nXRight;
			rc.left += nXLeft;
			m_pDialog->DrawRect( this->d_geometry, &rc, SelBkColor );
			m_Elements.GetAt( 1 )->FontColor.Current = SelTextColor;
			m_pDialog->DrawText( this->d_geometry, s_CandList.HoriCand.GetBuffer() + s_CandList.nFirstSelected,
				m_Elements.GetAt( 1 ), &rc, false, s_CandList.nHoriSelectedLen );
		}
	}
}


//--------------------------------------------------------------------------------------
void CDXUTIMEEditBox::RenderComposition( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
	RECT rcCaret = { 0, 0, 0, 0 };
	int nX, nXFirst;
	m_Buffer.CPtoX( m_nCaret, FALSE, &nX );
	m_Buffer.CPtoX( m_nFirstVisible, FALSE, &nXFirst );
	CDXUTElement *pElement = m_Elements.GetAt( 1 );

	// Get the required width
	RECT rc = { m_rcText.left + nX - nXFirst, m_rcText.top,
		m_rcText.left + nX - nXFirst, m_rcText.bottom };
	m_pDialog->CalcTextRect( s_CompString.GetBuffer(), pElement, &rc );

	// If the composition string is too long to fit within
	// the text area, move it to below the current line.
	// This matches the behavior of the default IME.
	if( rc.right > m_rcText.right )
		OffsetRect( &rc, m_rcText.left - rc.left, rc.bottom - rc.top );

	// Save the rectangle position for processing highlighted text.
	RECT rcFirst = rc;

	// Update s_ptCompString for RenderCandidateReadingWindow().
	s_ptCompString.x = rc.left; s_ptCompString.y = rc.top;

	D3DCOLOR TextColor = m_CompColor;
	// Render the window and string.
	// If the string is too long, we must wrap the line.
	pElement->FontColor.Current = TextColor;
	const WCHAR *pwszComp = s_CompString.GetBuffer();
	int nCharLeft = s_CompString.GetTextSize();
	for( ; ; )
	{
		// Find the last character that can be drawn on the same line.
		int nLastInLine;
		int bTrail;
		s_CompString.XtoCP( m_rcText.right - rc.left, &nLastInLine, &bTrail );
		int nNumCharToDraw = min( nCharLeft, nLastInLine );
		m_pDialog->CalcTextRect( pwszComp, pElement, &rc, nNumCharToDraw );

		// Draw the background
		// For Korean IME, blink the composition window background as if it
		// is a cursor.
		if( GetPrimaryLanguage() == LANG_KOREAN )
		{
			if( m_bCaretOn )
			{
				m_pDialog->DrawRect( this->d_geometry, &rc, m_CompWinColor );
			}
			else
			{
				// Not drawing composition string background. We
				// use the editbox's text color for composition
				// string text.
				TextColor = m_Elements.GetAt(0)->FontColor.States[DXUT_STATE_NORMAL];
			}
		} else
		{
			// Non-Korean IME. Always draw composition background.
			m_pDialog->DrawRect( this->d_geometry, &rc, m_CompWinColor );
		}

		// Draw the text
		pElement->FontColor.Current = TextColor;
		m_pDialog->DrawText(this->d_geometry,  pwszComp, pElement, &rc, false, nNumCharToDraw );

		// Advance pointer and counter
		nCharLeft -= nNumCharToDraw;
		pwszComp += nNumCharToDraw;
		if( nCharLeft <= 0 )
			break;

		// Advance rectangle coordinates to beginning of next line
		OffsetRect( &rc, m_rcText.left - rc.left, rc.bottom - rc.top );
	}

	// Load the rect for the first line again.
	rc = rcFirst;

	// Inspect each character in the comp string.
	// For target-converted and target-non-converted characters,
	// we display a different background color so they appear highlighted.
	int nCharFirst = 0;
	nXFirst = 0;
	s_nFirstTargetConv = -1;
	BYTE *pAttr;
	const WCHAR *pcComp;
	for( pcComp = s_CompString.GetBuffer(), pAttr = s_abCompStringAttr;
		*pcComp != L'\0'; ++pcComp, ++pAttr )
	{
		D3DCOLOR bkColor;

		// Render a different background for this character
		int nXLeft, nXRight;
		s_CompString.CPtoX( int(pcComp - s_CompString.GetBuffer()), FALSE, &nXLeft );
		s_CompString.CPtoX( int(pcComp - s_CompString.GetBuffer()), TRUE, &nXRight );

		// Check if this character is off the right edge and should
		// be wrapped to the next line.
		if( nXRight - nXFirst > m_rcText.right - rc.left )
		{
			// Advance rectangle coordinates to beginning of next line
			OffsetRect( &rc, m_rcText.left - rc.left, rc.bottom - rc.top );

			// Update the line's first character information
			nCharFirst = int(pcComp - s_CompString.GetBuffer());
			s_CompString.CPtoX( nCharFirst, FALSE, &nXFirst );
		}

		// If the caret is on this character, save the coordinates
		// for drawing the caret later.
		if( s_nCompCaret == int(pcComp - s_CompString.GetBuffer()) )
		{
			rcCaret = rc;
			rcCaret.left += nXLeft - nXFirst - 1;
			rcCaret.right = rcCaret.left + 2;
		}

		// Set up color based on the character attribute
		if( *pAttr == ATTR_TARGET_CONVERTED )
		{
			pElement->FontColor.Current = m_CompTargetColor;
			bkColor = m_CompTargetBkColor;
		}
		else
			if( *pAttr == ATTR_TARGET_NOTCONVERTED )
			{
				pElement->FontColor.Current = m_CompTargetNonColor;
				bkColor = m_CompTargetNonBkColor;
			}
			else
			{
				continue;
			}

			RECT rcTarget = { rc.left + nXLeft - nXFirst, rc.top, rc.left + nXRight - nXFirst, rc.bottom };
			m_pDialog->DrawRect( this->d_geometry, &rcTarget, bkColor );
			m_pDialog->DrawText( this->d_geometry, pcComp, pElement, &rcTarget, false, 1 );

			// Record the first target converted character's index
			if( -1 == s_nFirstTargetConv )
				s_nFirstTargetConv = int(pAttr - s_abCompStringAttr);
	}

	// Render the composition caret
	if( m_bCaretOn )
	{
		// If the caret is at the very end, its position would not have
		// been computed in the above loop.  We compute it here.
		if( s_nCompCaret == s_CompString.GetTextSize() )
		{
			s_CompString.CPtoX( s_nCompCaret, FALSE, &nX );
			rcCaret = rc;
			rcCaret.left += nX - nXFirst - 1;
			rcCaret.right = rcCaret.left + 2;
		}

		m_pDialog->DrawRect( this->d_geometry, &rcCaret, m_CompCaretColor );
	}
}


//--------------------------------------------------------------------------------------
void CDXUTIMEEditBox::RenderIndicator( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
	CDXUTElement *pElement = m_Elements.GetAt( 9 );
	pElement->TextureColor.Blend( DXUT_STATE_NORMAL, fElapsedTime );

	m_pDialog->DrawSprite( this->d_geometry, pElement, &m_rcIndicator );
	RECT rc = m_rcIndicator;
	InflateRect( &rc, -m_nSpacing, -m_nSpacing );
	pElement->FontColor.Current = s_ImeState == IMEUI_STATE_ON && s_bEnableImeSystem ? m_IndicatorImeColor : m_IndicatorEngColor;
	RECT rcCalc = { 0, 0, 0, 0 };
	// If IME system is off, draw English indicator.
	WCHAR *pwszIndicator = s_bEnableImeSystem ? s_wszCurrIndicator : s_aszIndicator[0];

	m_pDialog->CalcTextRect( pwszIndicator, pElement, &rcCalc );
	m_pDialog->DrawText( this->d_geometry, pwszIndicator, pElement, &rc );
}


//--------------------------------------------------------------------------------------
void CDXUTIMEEditBox::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
	if( m_bVisible == false )
		return;

	// If we have not computed the indicator symbol width,
	// do it.
	if( !m_nIndicatorWidth )
	{
		for( int i = 0; i < 5; ++i )
		{
			RECT rc = { 0, 0, 0, 0 };
			m_pDialog->CalcTextRect( s_aszIndicator[i], m_Elements.GetAt( 9 ), &rc );
			m_nIndicatorWidth = max( m_nIndicatorWidth, rc.right - rc.left );
		}
		// Update the rectangles now that we have the indicator's width
		UpdateRects();
	}

	// Let the parent render first (edit control)
	CDXUTEditBox::Render( pd3dDevice, fElapsedTime );

	CDXUTElement* pElement = GetElement( 1 );
	if( pElement )
	{
		s_CompString.SetFontNode( m_pDialog->GetFont( pElement->iFont ) );
		s_CandList.HoriCand.SetFontNode( m_pDialog->GetFont( pElement->iFont ) );
	}

	//
	// Now render the IME elements
	//
	if( m_bHasFocus )
	{
		// Render the input locale indicator
		RenderIndicator( pd3dDevice, fElapsedTime );

		// Display the composition string.
		// This method should also update s_ptCompString
		// for RenderCandidateReadingWindow.
		RenderComposition( pd3dDevice, fElapsedTime );

		// Display the reading/candidate window. RenderCandidateReadingWindow()
		// uses s_ptCompString to position itself.  s_ptCompString must have
		// been filled in by RenderComposition().
		if( s_bShowReadingWindow )
			// Reading window
			RenderCandidateReadingWindow( pd3dDevice, fElapsedTime, true );
		else if( s_CandList.bShowWindow )
			// Candidate list window
			RenderCandidateReadingWindow( pd3dDevice, fElapsedTime, false );
	}
}





//--------------------------------------------------------------------------------------
bool CDXUTEditBox::CUniBuffer::Grow( int nNewSize )
{
	// If the current size is already the maximum allowed,
	// we can't possibly allocate more.
	if( m_nBufferSize == DXUT_MAX_EDITBOXLENGTH )
		return false;

	int nAllocateSize = ( nNewSize == -1 || nNewSize < m_nBufferSize * 2 ) ? ( m_nBufferSize ? m_nBufferSize * 2 : 256 ) : nNewSize * 2;

	// Cap the buffer size at the maximum allowed.
	if( nAllocateSize > DXUT_MAX_EDITBOXLENGTH )
		nAllocateSize = DXUT_MAX_EDITBOXLENGTH;

	WCHAR *pTempBuffer = new WCHAR[nAllocateSize];
	if( !pTempBuffer )
		return false;
	if( m_pwszBuffer )
		CopyMemory( pTempBuffer, m_pwszBuffer, (lstrlenW( m_pwszBuffer ) + 1) * sizeof(WCHAR) );
	delete[] m_pwszBuffer;
	m_pwszBuffer = pTempBuffer;
	m_nBufferSize = nAllocateSize;
	return true;
}


//--------------------------------------------------------------------------------------
// Uniscribe -- Analyse() analyses the string in the buffer
//--------------------------------------------------------------------------------------
HRESULT CDXUTEditBox::CUniBuffer::Analyse()
{
	if( m_Analysis )
		_ScriptStringFree( &m_Analysis );

	SCRIPT_CONTROL ScriptControl; // For uniscribe
	SCRIPT_STATE   ScriptState;   // For uniscribe
	ZeroMemory( &ScriptControl, sizeof(ScriptControl) );
	ZeroMemory( &ScriptState, sizeof(ScriptState) );
	_ScriptApplyDigitSubstitution ( NULL, &ScriptControl, &ScriptState );

	if( !m_pFontNode )
		return E_FAIL;

	HRESULT hr = _ScriptStringAnalyse( NULL,
		m_pwszBuffer,
		lstrlenW( m_pwszBuffer ) + 1,  // NULL is also analyzed.
		lstrlenW( m_pwszBuffer ) * 3 / 2 + 16,
		-1,
		SSA_BREAK | SSA_GLYPHS | SSA_FALLBACK | SSA_LINK,
		0,
		&ScriptControl,
		&ScriptState,
		NULL,
		NULL,
		NULL,
		&m_Analysis );
	if( SUCCEEDED( hr ) )
		m_bAnalyseRequired = false;  // Analysis is up-to-date
	return hr;
}


//--------------------------------------------------------------------------------------
CDXUTEditBox::CUniBuffer::CUniBuffer( int nInitialSize )
{
	m_pwszBuffer = new WCHAR[nInitialSize];
	if( m_pwszBuffer )
		*m_pwszBuffer = 0;
	m_nBufferSize = nInitialSize;
	m_bAnalyseRequired = true;
	m_Analysis = NULL;
	m_pFontNode = NULL;
}


//--------------------------------------------------------------------------------------
CDXUTEditBox::CUniBuffer::~CUniBuffer()
{
	delete[] m_pwszBuffer;
	if( m_Analysis )
		_ScriptStringFree( &m_Analysis );
}


//--------------------------------------------------------------------------------------
bool CDXUTEditBox::CUniBuffer::SetBufferSize( int nSize )
{
	while( m_nBufferSize < nSize )
	{
		if( !Grow() )
			return false;
	}
	return true;
}


//--------------------------------------------------------------------------------------
WCHAR& CDXUTEditBox::CUniBuffer::operator[]( int n )  // No param checking
{
	// This version of operator[] is called only
	// if we are asking for write access, so
	// re-analysis is required.
	m_bAnalyseRequired = true;
	return m_pwszBuffer[n];
}


//--------------------------------------------------------------------------------------
void CDXUTEditBox::CUniBuffer::Clear()
{
	*m_pwszBuffer = L'\0';
	m_bAnalyseRequired = true;
}


//--------------------------------------------------------------------------------------
// Inserts the char at specified index.
// If nIndex == -1, insert to the end.
//--------------------------------------------------------------------------------------
bool CDXUTEditBox::CUniBuffer::InsertChar( int nIndex, WCHAR wChar )
{
	assert( nIndex >= 0 );

	if( nIndex < 0 || nIndex > lstrlenW( m_pwszBuffer ) )
		return false;  // invalid index

	// Check for maximum length allowed
	if( GetTextSize() + 1 >= DXUT_MAX_EDITBOXLENGTH )
		return false;

	if( lstrlenW( m_pwszBuffer ) + 1 >= m_nBufferSize )
	{
		if( !Grow() )
			return false;  // out of memory
	}

	MoveMemory( m_pwszBuffer + nIndex + 1, m_pwszBuffer + nIndex, sizeof(WCHAR) * ( lstrlenW( m_pwszBuffer ) - nIndex + 1 ) );
	m_pwszBuffer[ nIndex ] = wChar;
	m_bAnalyseRequired = true;

	return true;
}


//--------------------------------------------------------------------------------------
// Removes the char at specified index.
// If nIndex == -1, remove the last char.
//--------------------------------------------------------------------------------------
bool CDXUTEditBox::CUniBuffer::RemoveChar( int nIndex )
{
	if( !lstrlenW( m_pwszBuffer ) || nIndex < 0 || nIndex >= lstrlenW( m_pwszBuffer ) )
		return false;  // Invalid index

	MoveMemory( m_pwszBuffer + nIndex, m_pwszBuffer + nIndex + 1, sizeof(WCHAR) * ( lstrlenW( m_pwszBuffer ) - nIndex ) );
	m_bAnalyseRequired = true;
	return true;
}


//--------------------------------------------------------------------------------------
// Inserts the first nCount characters of the string pStr at specified index.
// If nCount == -1, the entire string is inserted.
// If nIndex == -1, insert to the end.
//--------------------------------------------------------------------------------------
bool CDXUTEditBox::CUniBuffer::InsertString( int nIndex, const WCHAR *pStr, int nCount )
{
	assert( nIndex >= 0 );

	if( nIndex > lstrlenW( m_pwszBuffer ) )
		return false;  // invalid index

	if( -1 == nCount )
		nCount = lstrlenW( pStr );

	// Check for maximum length allowed
	if( GetTextSize() + nCount >= DXUT_MAX_EDITBOXLENGTH )
		return false;

	if( lstrlenW( m_pwszBuffer ) + nCount >= m_nBufferSize )
	{
		if( !Grow( lstrlenW( m_pwszBuffer ) + nCount + 1 ) )
			return false;  // out of memory
	}

	MoveMemory( m_pwszBuffer + nIndex + nCount, m_pwszBuffer + nIndex, sizeof(WCHAR) * ( lstrlenW( m_pwszBuffer ) - nIndex + 1 ) );
	CopyMemory( m_pwszBuffer + nIndex, pStr, nCount * sizeof(WCHAR) );
	m_bAnalyseRequired = true;

	return true;
}


//--------------------------------------------------------------------------------------
bool CDXUTEditBox::CUniBuffer::SetText( LPCWSTR wszText )
{
	assert( wszText != NULL );

	int nRequired = int(wcslen( wszText ) + 1);

	// Check for maximum length allowed
	if( nRequired >= DXUT_MAX_EDITBOXLENGTH )
		return false;

	while( GetBufferSize() < nRequired )
		if( !Grow() )
			break;
	// Check again in case out of memory occurred inside while loop.
	if( GetBufferSize() >= nRequired )
	{
		StringCchCopy( m_pwszBuffer, GetBufferSize(), wszText );
		m_bAnalyseRequired = true;
		return true;
	}
	else
		return false;
}


//--------------------------------------------------------------------------------------
HRESULT CDXUTEditBox::CUniBuffer::CPtoX( int nCP, BOOL bTrail, int *pX )
{
	assert( pX );
	*pX = 0;  // Default

 	HRESULT hr = S_OK;
// 	if( m_bAnalyseRequired )
// 		hr = Analyse();
// 
// 	if( SUCCEEDED( hr ) )
// 		hr = _ScriptStringCPtoX( m_Analysis, nCP, bTrail, pX );
	
	//if (m_pFontNode)
	{
		CEGUI::String ss;
		ss.assign(m_pwszBuffer, nCP);
		
		float pixels = CEGUI::System::getSingleton().getDefaultGUIContext().getDefaultFont()->getTextExtent(ss);

		*pX = int(pixels+0.5f);

	}

	return hr;
}


//--------------------------------------------------------------------------------------
HRESULT CDXUTEditBox::CUniBuffer::XtoCP( int nX, int *pCP, int *pnTrail )
{
	assert( pCP && pnTrail );
	*pCP = 0; *pnTrail = FALSE;  // Default

// 	HRESULT hr = S_OK;
// 	if( m_bAnalyseRequired )
// 		hr = Analyse();
// 
// 	if( SUCCEEDED( hr ) )
// 		hr = _ScriptStringXtoCP( m_Analysis, nX, pCP, pnTrail );

	*pCP = CEGUI::System::getSingleton().getDefaultGUIContext().getDefaultFont()->getCharAtPixel(CEGUI::String(m_pwszBuffer), nX);
	*pnTrail = TRUE;

	// If the coordinate falls outside the text region, we
	// can get character positions that don't exist.  We must
	// filter them here and convert them to those that do exist.
	if( *pCP == -1 && *pnTrail == TRUE )
	{
		*pCP = 0; *pnTrail = FALSE;
	} else
		if( *pCP > lstrlenW( m_pwszBuffer ) && *pnTrail == FALSE )
		{
			*pCP = lstrlenW( m_pwszBuffer ); *pnTrail = TRUE;
		}

		return S_OK;
}


//--------------------------------------------------------------------------------------
void CDXUTEditBox::CUniBuffer::GetPriorItemPos( int nCP, int *pPrior )
{
	*pPrior = nCP;  // Default is the char itself

	if( m_bAnalyseRequired )
		if( FAILED( Analyse() ) )
			return;

	const SCRIPT_LOGATTR *pLogAttr = _ScriptString_pLogAttr( m_Analysis );
	if( !pLogAttr )
		return;

	if( !_ScriptString_pcOutChars( m_Analysis ) )
		return;
	int nInitial = *_ScriptString_pcOutChars( m_Analysis );
	if( nCP - 1 < nInitial )
		nInitial = nCP - 1;
	for( int i = nInitial; i > 0; --i )
		if( pLogAttr[i].fWordStop ||       // Either the fWordStop flag is set
			( !pLogAttr[i].fWhiteSpace &&  // Or the previous char is whitespace but this isn't.
			pLogAttr[i-1].fWhiteSpace ) )
		{
			*pPrior = i;
			return;
		}
		// We have reached index 0.  0 is always a break point, so simply return it.
		*pPrior = 0;
}


//--------------------------------------------------------------------------------------
void CDXUTEditBox::CUniBuffer::GetNextItemPos( int nCP, int *pPrior )
{
	*pPrior = nCP;  // Default is the char itself

	HRESULT hr = S_OK;
	if( m_bAnalyseRequired )
		hr = Analyse();
	if( FAILED( hr ) )
		return;

	const SCRIPT_LOGATTR *pLogAttr = _ScriptString_pLogAttr( m_Analysis );
	if( !pLogAttr )
		return;

	if( !_ScriptString_pcOutChars( m_Analysis ) )
		return;
	int nInitial = *_ScriptString_pcOutChars( m_Analysis );
	if( nCP + 1 < nInitial )
		nInitial = nCP + 1;
	for( int i = nInitial; i < *_ScriptString_pcOutChars( m_Analysis ) - 1; ++i )
	{
		if( pLogAttr[i].fWordStop )      // Either the fWordStop flag is set
		{
			*pPrior = i;
			return;
		}
		else
			if( pLogAttr[i].fWhiteSpace &&  // Or this whitespace but the next char isn't.
				!pLogAttr[i+1].fWhiteSpace )
			{
				*pPrior = i+1;  // The next char is a word stop
				return;
			}
	}
	// We have reached the end. It's always a word stop, so simply return it.
	*pPrior = *_ScriptString_pcOutChars( m_Analysis ) - 1;
}


//--------------------------------------------------------------------------------------
void CDXUTEditBox::ResetCaretBlink()
{
	m_bCaretOn = true;
	m_dfLastBlink = DXUTGetGlobalTimer()->GetAbsoluteTime();
	invalidate();
}

//--------------------------------------------------------------------------------------
// Implementation of CGrowableArray
//--------------------------------------------------------------------------------------

// This version doesn't call ctor or dtor.
template< typename TYPE >
HRESULT CGrowableArray<TYPE>::SetSizeInternal( int nNewMaxSize )
{
	if( nNewMaxSize < 0 )
	{
		assert( false );
		return E_INVALIDARG;
	}

	if( nNewMaxSize == 0 )
	{
		// Shrink to 0 size & cleanup
		if( m_pData )
		{
			free( m_pData );
			m_pData = NULL;
		}

		m_nMaxSize = 0;
		m_nSize = 0;
	}
	else if( m_pData == NULL || nNewMaxSize > m_nMaxSize )
	{
		// Grow array
		int nGrowBy = ( m_nMaxSize == 0 ) ? 16 : m_nMaxSize;
		nNewMaxSize = max( nNewMaxSize, m_nMaxSize + nGrowBy );

		TYPE* pDataNew = (TYPE*) realloc( m_pData, nNewMaxSize * sizeof(TYPE) );
		if( pDataNew == NULL )
			return E_OUTOFMEMORY;

		m_pData = pDataNew;
		m_nMaxSize = nNewMaxSize;
	}

	return S_OK;
}


//--------------------------------------------------------------------------------------
template< typename TYPE >
HRESULT CGrowableArray<TYPE>::SetSize( int nNewMaxSize )
{
	int nOldSize = m_nSize;

	if( nOldSize > nNewMaxSize )
	{
		// Removing elements. Call dtor.

		for( int i = nNewMaxSize; i < nOldSize; ++i )
			m_pData[i].~TYPE();
	}

	// Adjust buffer.  Note that there's no need to check for error
	// since if it happens, nOldSize == nNewMaxSize will be true.)
	HRESULT hr = SetSizeInternal( nNewMaxSize );

	if( nOldSize < nNewMaxSize )
	{
		// Adding elements. Call ctor.

		for( int i = nOldSize; i < nNewMaxSize; ++i )
			::new (&m_pData[i]) TYPE;
	}

	return hr;
}


//--------------------------------------------------------------------------------------
template< typename TYPE >
HRESULT CGrowableArray<TYPE>::Add( const TYPE& value )
{
	HRESULT hr;
	if( FAILED( hr = SetSizeInternal( m_nSize + 1 ) ) )
		return hr;

	// Construct the new element
	::new (&m_pData[m_nSize]) TYPE;

	// Assign
	m_pData[m_nSize] = value;
	++m_nSize;

	return S_OK;
}


//--------------------------------------------------------------------------------------
template< typename TYPE >
HRESULT CGrowableArray<TYPE>::Insert( int nIndex, const TYPE& value )
{
	HRESULT hr;

	// Validate index
	if( nIndex < 0 || 
		nIndex > m_nSize )
	{
		assert( false );
		return E_INVALIDARG;
	}

	// Prepare the buffer
	if( FAILED( hr = SetSizeInternal( m_nSize + 1 ) ) )
		return hr;

	// Shift the array
	MoveMemory( &m_pData[nIndex+1], &m_pData[nIndex], sizeof(TYPE) * (m_nSize - nIndex) );

	// Construct the new element
	::new (&m_pData[nIndex]) TYPE;

	// Set the value and increase the size
	m_pData[nIndex] = value;
	++m_nSize;

	return S_OK;
}


//--------------------------------------------------------------------------------------
template< typename TYPE >
HRESULT CGrowableArray<TYPE>::SetAt( int nIndex, const TYPE& value )
{
	// Validate arguments
	if( nIndex < 0 ||
		nIndex >= m_nSize )
	{
		assert( false );
		return E_INVALIDARG;
	}

	m_pData[nIndex] = value;
	return S_OK;
}


//--------------------------------------------------------------------------------------
// Searches for the specified value and returns the index of the first occurrence
// within the section of the data array that extends from iStart and contains the 
// specified number of elements. Returns -1 if value is not found within the given 
// section.
//--------------------------------------------------------------------------------------
template< typename TYPE >
int CGrowableArray<TYPE>::IndexOf( const TYPE& value, int iStart, int nNumElements )
{
	// Validate arguments
	if( iStart < 0 || 
		iStart >= m_nSize ||
		nNumElements < 0 ||
		iStart + nNumElements > m_nSize )
	{
		assert( false );
		return -1;
	}

	// Search
	for( int i = iStart; i < (iStart + nNumElements); i++ )
	{
		if( value == m_pData[i] )
			return i;
	}

	// Not found
	return -1;
}


//--------------------------------------------------------------------------------------
// Searches for the specified value and returns the index of the last occurrence
// within the section of the data array that contains the specified number of elements
// and ends at iEnd. Returns -1 if value is not found within the given section.
//--------------------------------------------------------------------------------------
template< typename TYPE >
int CGrowableArray<TYPE>::LastIndexOf( const TYPE& value, int iEnd, int nNumElements )
{
	// Validate arguments
	if( iEnd < 0 || 
		iEnd >= m_nSize ||
		nNumElements < 0 ||
		iEnd - nNumElements < 0 )
	{
		assert( false );
		return -1;
	}

	// Search
	for( int i = iEnd; i > (iEnd - nNumElements); i-- )
	{
		if( value == m_pData[i] )
			return i;
	}

	// Not found
	return -1;
}



//--------------------------------------------------------------------------------------
template< typename TYPE >
HRESULT CGrowableArray<TYPE>::Remove( int nIndex )
{
	if( nIndex < 0 || 
		nIndex >= m_nSize )
	{
		assert( false );
		return E_INVALIDARG;
	}

	// Destruct the element to be removed
	m_pData[nIndex].~TYPE();

	// Compact the array and decrease the size
	MoveMemory( &m_pData[nIndex], &m_pData[nIndex+1], sizeof(TYPE) * (m_nSize - (nIndex+1)) );
	--m_nSize;

	return S_OK;
}


//--------------------------------------------------------------------------------------
void DXUTBlendColor::Init( D3DCOLOR defaultColor, D3DCOLOR disabledColor, D3DCOLOR hiddenColor )
{
	for( int i=0; i < MAX_CONTROL_STATES; i++ )
	{
		States[ i ] = defaultColor;
	}

	States[ DXUT_STATE_DISABLED ] = disabledColor;
	States[ DXUT_STATE_HIDDEN ] = hiddenColor;
	Current = hiddenColor;
}


//--------------------------------------------------------------------------------------
void DXUTBlendColor::Blend( UINT iState, float fElapsedTime, float fRate )
{
	D3DXCOLOR destColor = States[ iState ];
	D3DXColorLerp( &Current, &Current, &destColor, 1.0f - powf( fRate, 30 * fElapsedTime ) );
}



//--------------------------------------------------------------------------------------
void CDXUTElement::SetTexture( UINT iTexture, RECT* prcTexture, D3DCOLOR defaultTextureColor )
{
	this->iTexture = iTexture;

	if( prcTexture )
		rcTexture = *prcTexture;
	else
		SetRectEmpty( &rcTexture );

	TextureColor.Init( defaultTextureColor );
}


//--------------------------------------------------------------------------------------
void CDXUTElement::SetFont( UINT iFont, D3DCOLOR defaultFontColor, DWORD dwTextFormat )
{
	this->iFont = iFont;
	this->dwTextFormat = dwTextFormat;

	FontColor.Init( defaultFontColor );
}

//--------------------------------------------------------------------------------------
void CDXUTElement::Refresh()
{
	TextureColor.Current = TextureColor.States[ DXUT_STATE_HIDDEN ];
	FontColor.Current = FontColor.States[ DXUT_STATE_HIDDEN ];
}

//--------------------------------------------------------------------------------------
// Global/Static Members
//--------------------------------------------------------------------------------------
CDXUTDialogResourceManager* DXUTGetGlobalDialogResourceManager()
{
	// Using an accessor function gives control of the construction order
	static CDXUTDialogResourceManager manager;
	return &manager;
}

double        CDXUTDialog::s_fTimeRefresh = 0.0f;
CDXUTControl* CDXUTDialog::s_pControlFocus = NULL;        // The control which has focus
CDXUTControl* CDXUTDialog::s_pControlPressed = NULL;      // The control currently pressed


struct DXUT_SCREEN_VERTEX
{
	float x, y, z, h;
	D3DCOLOR color;
	float tu, tv;

	static DWORD FVF;
};
DWORD DXUT_SCREEN_VERTEX::FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;


// inline int RectWidth( RECT &rc ) { return ( (rc).right - (rc).left ); }
// inline int RectHeight( RECT &rc ) { return ( (rc).bottom - (rc).top ); }


//--------------------------------------------------------------------------------------
// CDXUTDialog class
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
CDXUTDialog::CDXUTDialog()
{
	m_x = 0;
	m_y = 0;
	m_width = 0;
	m_height = 0;

	m_bVisible = true;
	m_bCaption = false;
	m_bMinimized = false;
	m_wszCaption[0] = L'\0';
	m_nCaptionHeight = 18;

	m_colorTopLeft = 0;
	m_colorTopRight = 0;
	m_colorBottomLeft = 0;
	m_colorBottomRight = 0;

	m_pCallbackEvent = NULL;

	m_fTimeLastRefresh = 0;

	m_pControlMouseOver = NULL;

	m_pNextDialog = this;
	m_pPrevDialog = this;

	m_nDefaultControlID = 0xffff;
	m_bNonUserEvents = false;
	m_bKeyboardInput = false;
	m_bMouseInput = true;

	InitDefaultElements();
}


//--------------------------------------------------------------------------------------
CDXUTDialog::~CDXUTDialog()
{
	int i=0;

	RemoveAllControls();

	m_Fonts.RemoveAll();
	m_Textures.RemoveAll();

	for( i=0; i < m_DefaultElements.GetSize(); i++ )
	{
		DXUTElementHolder* pElementHolder = m_DefaultElements.GetAt( i );
		SAFE_DELETE( pElementHolder );
	}

	m_DefaultElements.RemoveAll();
}


//--------------------------------------------------------------------------------------
void CDXUTDialog::RemoveControl( int ID )
{
	for( int i=0; i < m_Controls.GetSize(); i++ )
	{
		CDXUTControl* pControl = m_Controls.GetAt( i );
		if( pControl->GetID() == ID )
		{
			// Clean focus first
			ClearFocus();

			// Clear references to this control
			if( s_pControlFocus == pControl )
				s_pControlFocus = NULL;
			if( s_pControlPressed == pControl )
				s_pControlPressed = NULL;
			if( m_pControlMouseOver == pControl )
				m_pControlMouseOver = NULL;

			SAFE_DELETE( pControl );
			m_Controls.Remove( i );

			return;
		}
	}
}


//--------------------------------------------------------------------------------------
void CDXUTDialog::RemoveAllControls()
{
	if( s_pControlFocus && s_pControlFocus->m_pDialog == this )
		s_pControlFocus = NULL;
	if( s_pControlPressed && s_pControlPressed->m_pDialog == this )
		s_pControlPressed = NULL;
	m_pControlMouseOver = NULL;

	for( int i=0; i < m_Controls.GetSize(); i++ )
	{
		CDXUTControl* pControl = m_Controls.GetAt( i );
		SAFE_DELETE( pControl );
	}

	m_Controls.RemoveAll();
}


//--------------------------------------------------------------------------------------
CDXUTDialogResourceManager::CDXUTDialogResourceManager()
{
}


//--------------------------------------------------------------------------------------
CDXUTDialogResourceManager::~CDXUTDialogResourceManager()
{
	int i;
	for( i=0; i < m_FontCache.GetSize(); i++ )
	{
		DXUTFontNode* pFontNode = m_FontCache.GetAt( i );
		SAFE_DELETE( pFontNode );
	}
	m_FontCache.RemoveAll();   

	for( i=0; i < m_TextureCache.GetSize(); i++ )
	{
		DXUTTextureNode* pTextureNode = m_TextureCache.GetAt( i );
		SAFE_DELETE( pTextureNode );
	}
	m_TextureCache.RemoveAll();   
}


//--------------------------------------------------------------------------------------
void CDXUTDialog::Refresh()
{
	if( s_pControlFocus )
		s_pControlFocus->OnFocusOut();

	if( m_pControlMouseOver )
		m_pControlMouseOver->OnMouseLeave();

	s_pControlFocus = NULL;
	s_pControlPressed = NULL;
	m_pControlMouseOver = NULL;

	for( int i=0; i < m_Controls.GetSize(); i++ )
	{
		CDXUTControl* pControl = m_Controls.GetAt(i);
		pControl->Refresh();
	}

	if( m_bKeyboardInput )
		FocusDefaultControl();
}


//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::OnRender( float fElapsedTime )
{   
	

	return S_OK;
}


//--------------------------------------------------------------------------------------
VOID CDXUTDialog::SendEvent( UINT nEvent, bool bTriggeredByUser, CDXUTControl* pControl )
{
	// If no callback has been registered there's nowhere to send the event to
	if( m_pCallbackEvent == NULL )
		return;

	// Discard events triggered programatically if these types of events haven't been
	// enabled
	if( !bTriggeredByUser && !m_bNonUserEvents )
		return;

	m_pCallbackEvent( nEvent, pControl->GetID(), pControl );
}


//--------------------------------------------------------------------------------------
int CDXUTDialogResourceManager::AddFont( LPCWSTR strFaceName, LONG height, LONG weight )
{
	// See if this font already exists
	for( int i=0; i < m_FontCache.GetSize(); i++ )
	{
		DXUTFontNode* pFontNode = m_FontCache.GetAt(i);
		if( 0 == _wcsnicmp( pFontNode->strFace, strFaceName, MAX_PATH-1 ) &&
			pFontNode->nHeight == height &&
			pFontNode->nWeight == weight )
		{
			return i;
		}
	}

	// Add a new font and try to create it
	DXUTFontNode* pNewFontNode = new DXUTFontNode();
	if( pNewFontNode == NULL )
		return -1;

	ZeroMemory( pNewFontNode, sizeof(DXUTFontNode) );
	StringCchCopy( pNewFontNode->strFace, MAX_PATH, strFaceName );
	pNewFontNode->nHeight = height;
	pNewFontNode->nWeight = weight;
	m_FontCache.Add( pNewFontNode );

	int iFont = m_FontCache.GetSize()-1;

	// If a device is available, try to create immediately
	
	CreateFont( iFont );

	return iFont;
}


//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::SetFont( UINT index, LPCWSTR strFaceName, LONG height, LONG weight )
{
	// Make sure the list is at least as large as the index being set
	UINT i;
	for( i=m_Fonts.GetSize(); i <= index; i++ )
	{
		m_Fonts.Add( -1 );
	}

	int iFont = DXUTGetGlobalDialogResourceManager()->AddFont( strFaceName, height, weight );
	m_Fonts.SetAt( index, iFont );

	return S_OK;
}


//--------------------------------------------------------------------------------------
DXUTFontNode* CDXUTDialog::GetFont( UINT index )
{
	if( NULL == DXUTGetGlobalDialogResourceManager() )
		return NULL;
	return DXUTGetGlobalDialogResourceManager()->GetFontNode( m_Fonts.GetAt( index ) );
}


//--------------------------------------------------------------------------------------
int CDXUTDialogResourceManager::AddTexture( LPCWSTR strFilename )
{
	// See if this texture already exists
	for( int i=0; i < m_TextureCache.GetSize(); i++ )
	{
		DXUTTextureNode* pTextureNode = m_TextureCache.GetAt(i);
		if( 0 == _wcsnicmp( pTextureNode->strFilename, strFilename, MAX_PATH-1 ) )
		{
			return i;
		}
	}

	// Add a new texture and try to create it
	DXUTTextureNode* pNewTextureNode = new DXUTTextureNode();
	if( pNewTextureNode == NULL )
		return -1;

	ZeroMemory( pNewTextureNode, sizeof(DXUTTextureNode) );
	StringCchCopy( pNewTextureNode->strFilename, MAX_PATH, strFilename );
	m_TextureCache.Add( pNewTextureNode );

	int iTexture = m_TextureCache.GetSize()-1;

	// If a device is available, try to create immediately
	//if( m_pd3dDevice )
		CreateTexture( iTexture );

	return iTexture;
}


//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::SetTexture( UINT index, LPCWSTR strFilename )
{
	// Make sure the list is at least as large as the index being set
	for( UINT i=m_Textures.GetSize(); i <= index; i++ )
	{
		m_Textures.Add( -1 );
	}

	int iTexture = DXUTGetGlobalDialogResourceManager()->AddTexture( strFilename );

	m_Textures.SetAt( index, iTexture );
	return S_OK;
}


//--------------------------------------------------------------------------------------
DXUTTextureNode* CDXUTDialog::GetTexture( UINT index )
{
	if( NULL == DXUTGetGlobalDialogResourceManager() )
		return NULL;
	return DXUTGetGlobalDialogResourceManager()->GetTextureNode( m_Textures.GetAt( index ) );
}



//--------------------------------------------------------------------------------------
bool CDXUTDialog::MsgProc(  __in HWND hWnd, __in UINT uMsg, __in WPARAM wParam, __in LPARAM lParam)
{
	bool bHandled = false;

	// For invisible dialog, do not handle anything.
	if( !m_bVisible )
		return false;

	// If caption is enable, check for clicks in the caption area.
	if( m_bCaption )
	{
		static bool bDrag;

		if( uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONDBLCLK )
		{
			POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };

			if( mousePoint.x >= m_x && mousePoint.x < m_x + m_width &&
				mousePoint.y >= m_y && mousePoint.y < m_y + m_nCaptionHeight )
			{
				bDrag = true;
				SetCapture( hWnd );
				return true;
			}
		} else
			if( uMsg == WM_LBUTTONUP && bDrag )
			{
				POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };

				if( mousePoint.x >= m_x && mousePoint.x < m_x + m_width &&
					mousePoint.y >= m_y && mousePoint.y < m_y + m_nCaptionHeight )
				{
					ReleaseCapture();
					bDrag = false;
					m_bMinimized = !m_bMinimized;
					return true;
				}
			}
	}

	// If the dialog is minimized, don't send any messages to controls.
	if( m_bMinimized )
		return false;

	// If a control is in focus, it belongs to this dialog, and it's enabled, then give
	// it the first chance at handling the message.
	if( s_pControlFocus && 
		s_pControlFocus->m_pDialog == this && 
		s_pControlFocus->GetEnabled() )
	{
		// If the control MsgProc handles it, then we don't.
		if( s_pControlFocus->MsgProc( uMsg, wParam, lParam ) )
			return true;
	}

	switch( uMsg )
	{
	case WM_ACTIVATEAPP:
		// Call OnFocusIn()/OnFocusOut() of the control that currently has the focus
		// as the application is activated/deactivated.  This matches the Windows
		// behavior.
		if( s_pControlFocus && 
			s_pControlFocus->m_pDialog == this && 
			s_pControlFocus->GetEnabled() )
		{
			if( wParam )
				s_pControlFocus->OnFocusIn();
			else
				s_pControlFocus->OnFocusOut();
		}
		break;

		// Keyboard messages
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		{
			// If a control is in focus, it belongs to this dialog, and it's enabled, then give
			// it the first chance at handling the message.
			if( s_pControlFocus && 
				s_pControlFocus->m_pDialog == this && 
				s_pControlFocus->GetEnabled() )
			{
				if( s_pControlFocus->HandleKeyboard( uMsg, wParam, lParam ) )
					return true;
			}

			// Not yet handled, see if this matches a control's hotkey
			// Activate the hotkey if the focus doesn't belong to an
			// edit box.
			if( uMsg == WM_KEYDOWN && ( !s_pControlFocus ||
				( s_pControlFocus->GetType() != DXUT_CONTROL_EDITBOX
				&& s_pControlFocus->GetType() != DXUT_CONTROL_IMEEDITBOX ) ) )
			{
				for( int i=0; i < m_Controls.GetSize(); i++ )
				{
					CDXUTControl* pControl = m_Controls.GetAt( i );
					if( pControl->GetHotkey() == wParam )
					{
						pControl->OnHotkey();
						return true;
					}
				}
			}

			// Not yet handled, check for focus messages
			if( uMsg == WM_KEYDOWN )
			{
				// If keyboard input is not enabled, this message should be ignored
				if( !m_bKeyboardInput )
					return false;

				switch( wParam )
				{
				case VK_RIGHT:
				case VK_DOWN:
					if( s_pControlFocus != NULL )
					{
						OnCycleFocus( true );
						return true;
					}
					break;

				case VK_LEFT:
				case VK_UP:
					if( s_pControlFocus != NULL )
					{
						OnCycleFocus( false );
						return true;
					}
					break;

				case VK_TAB: 
					if( s_pControlFocus == NULL )
					{
						FocusDefaultControl();
					}
					else
					{
						bool bShiftDown = ((GetAsyncKeyState( VK_SHIFT ) & 0x8000) != 0);
						OnCycleFocus( !bShiftDown );
					}
					return true;
				}
			}

			break;
		}


		// Mouse messages
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_XBUTTONDBLCLK:
	case WM_MOUSEWHEEL:
		{
			// If not accepting mouse input, return false to indicate the message should still 
			// be handled by the application (usually to move the camera).
			if( !m_bMouseInput )
				return false;

			POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
// 			mousePoint.x -= m_x;
// 			mousePoint.y -= m_y;

			// If caption is enabled, offset the Y coordinate by the negative of its height.
// 			if( m_bCaption )
// 				mousePoint.y -= m_nCaptionHeight;

			// If a control is in focus, it belongs to this dialog, and it's enabled, then give
			// it the first chance at handling the message.
			if( s_pControlFocus && 
				s_pControlFocus->m_pDialog == this && 
				s_pControlFocus->GetEnabled() )
			{
				if( s_pControlFocus->HandleMouse( uMsg, mousePoint, wParam, lParam ) )
					return true;
			}

			// Not yet handled, see if the mouse is over any controls
			CDXUTControl* pControl = GetControlAtPoint( mousePoint );
			if( pControl != NULL && pControl->GetEnabled() )
			{
				bHandled = pControl->HandleMouse( uMsg, mousePoint, wParam, lParam );
				if( bHandled )
					return true;
			}
			else
			{
				// Mouse not over any controls in this dialog, if there was a control
				// which had focus it just lost it
				if( uMsg == WM_LBUTTONDOWN && 
					s_pControlFocus && 
					s_pControlFocus->m_pDialog == this )
				{
					s_pControlFocus->OnFocusOut();
					s_pControlFocus = NULL;
				}
			}

			// Still not handled, hand this off to the dialog. Return false to indicate the
			// message should still be handled by the application (usually to move the camera).
			switch( uMsg )
			{
			case WM_MOUSEMOVE:
				OnMouseMove( mousePoint );
				return false;
			}

			break;
		}
	}

	return false;
}

//--------------------------------------------------------------------------------------
CDXUTControl* CDXUTDialog::GetControlAtPoint( POINT pt )
{
	// Search through all child controls for the first one which
	// contains the mouse point
	for( int i=0; i < m_Controls.GetSize(); i++ )
	{
		CDXUTControl* pControl = m_Controls.GetAt(i);

		if( pControl == NULL )
		{
			continue;
		}

		// We only return the current control if it is visible
		// and enabled.  Because GetControlAtPoint() is used to do mouse
		// hittest, it makes sense to perform this filtering.
		if( pControl->ContainsPoint( pt ) && pControl->GetEnabled() && pControl->GetVisible() )
		{
			return pControl;
		}
	}

	return NULL;
}


//--------------------------------------------------------------------------------------
bool CDXUTDialog::GetControlEnabled( int ID )
{
	CDXUTControl* pControl = GetControl( ID );
	if( pControl == NULL )
		return false;

	return pControl->GetEnabled();
}



//--------------------------------------------------------------------------------------
void CDXUTDialog::SetControlEnabled( int ID, bool bEnabled )
{
	CDXUTControl* pControl = GetControl( ID );
	if( pControl == NULL )
		return;

	pControl->SetEnabled( bEnabled );
}


//--------------------------------------------------------------------------------------
void CDXUTDialog::OnMouseUp( POINT pt )
{
	s_pControlPressed = NULL;
	m_pControlMouseOver = NULL;
}


//--------------------------------------------------------------------------------------
void CDXUTDialog::OnMouseMove( POINT pt )
{
	// Figure out which control the mouse is over now
	CDXUTControl* pControl = GetControlAtPoint( pt );

	// If the mouse is still over the same control, nothing needs to be done
	if( pControl == m_pControlMouseOver )
		return;

	// Handle mouse leaving the old control
	if( m_pControlMouseOver )
		m_pControlMouseOver->OnMouseLeave();

	// Handle mouse entering the new control
	m_pControlMouseOver = pControl;
	if( pControl != NULL )
		m_pControlMouseOver->OnMouseEnter();
}


//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::SetDefaultElement( UINT nControlType, UINT iElement, CDXUTElement* pElement )
{
	// If this Element type already exist in the list, simply update the stored Element
	for( int i=0; i < m_DefaultElements.GetSize(); i++ )
	{
		DXUTElementHolder* pElementHolder = m_DefaultElements.GetAt( i );

		if( pElementHolder->nControlType == nControlType &&
			pElementHolder->iElement == iElement )
		{
			pElementHolder->Element = *pElement;
			return S_OK;
		}
	}

	// Otherwise, add a new entry
	DXUTElementHolder* pNewHolder;
	pNewHolder = new DXUTElementHolder;
	if( pNewHolder == NULL )
		return E_OUTOFMEMORY;

	pNewHolder->nControlType = nControlType;
	pNewHolder->iElement = iElement;
	pNewHolder->Element = *pElement;

	m_DefaultElements.Add( pNewHolder );
	return S_OK;
}


//--------------------------------------------------------------------------------------
CDXUTElement* CDXUTDialog::GetDefaultElement( UINT nControlType, UINT iElement )
{
	for( int i=0; i < m_DefaultElements.GetSize(); i++ )
	{
		DXUTElementHolder* pElementHolder = m_DefaultElements.GetAt( i );

		if( pElementHolder->nControlType == nControlType &&
			pElementHolder->iElement == iElement )
		{
			return &pElementHolder->Element;
		}
	}

	return NULL;
}


 



//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::AddEditBox( int ID, LPCWSTR strText, int x, int y, int width, int height, bool bIsDefault, CDXUTEditBox** ppCreated )
{
	HRESULT hr = S_OK;

	CDXUTEditBox *pEditBox = new CDXUTEditBox( this );

	if( ppCreated != NULL )
		*ppCreated = pEditBox;

	if( pEditBox == NULL )
		return E_OUTOFMEMORY;

	hr = AddControl( pEditBox );
	if( FAILED(hr) )
		return hr;

	// Set the ID and position
	pEditBox->SetID( ID ); 
	pEditBox->SetLocation( x, y );
	pEditBox->SetSize( width, height );
	pEditBox->m_bIsDefault = bIsDefault;

	if( strText )
		pEditBox->SetText( strText );

	return S_OK;
}



//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::AddIMEEditBox( int ID, LPCWSTR strText, int x, int y, int width, int height, bool bIsDefault, CDXUTIMEEditBox** ppCreated )
{
	HRESULT hr = S_OK;
	CDXUTIMEEditBox *pEditBox = new CDXUTIMEEditBox( this );

	if( ppCreated != NULL )
		*ppCreated = pEditBox;

	if( pEditBox == NULL )
		return E_OUTOFMEMORY;

	hr = AddControl( pEditBox );
	if( FAILED(hr) )
		return hr;

	// Set the ID and position
	pEditBox->SetID( ID ); 
	pEditBox->SetLocation( x, y );
	pEditBox->SetSize( width, height );
	pEditBox->m_bIsDefault = bIsDefault;

	if( strText )
		pEditBox->SetText( strText );

	return S_OK;
}




//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::InitControl( CDXUTControl* pControl )
{
	HRESULT hr;

	if( pControl == NULL )
		return E_INVALIDARG;

	pControl->m_Index = m_Controls.GetSize();

	// Look for a default Element entries
	for( int i=0; i < m_DefaultElements.GetSize(); i++ )
	{
		DXUTElementHolder* pElementHolder = m_DefaultElements.GetAt( i );
		if( pElementHolder->nControlType == pControl->GetType() )
			pControl->SetElement( pElementHolder->iElement, &pElementHolder->Element );
	}

	V_RETURN( pControl->OnInit() );

	return S_OK;
}



//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::AddControl( CDXUTControl* pControl )
{
	HRESULT hr = S_OK;

	hr = InitControl( pControl );
	if( FAILED(hr) )
		return DXTRACE_ERR( L"CDXUTDialog::InitControl", hr );

	// Add to the list
	hr = m_Controls.Add( pControl );
	if( FAILED(hr) )
	{
		return DXTRACE_ERR( L"CGrowableArray::Add", hr );
	}

	return S_OK;
}


//--------------------------------------------------------------------------------------
CDXUTControl* CDXUTDialog::GetControl( int ID )
{
	// Try to find the control with the given ID
	for( int i=0; i < m_Controls.GetSize(); i++ )
	{
		CDXUTControl* pControl = m_Controls.GetAt( i );

		if( pControl->GetID() == ID )
		{
			return pControl;
		}
	}

	// Not found
	return NULL;
}



//--------------------------------------------------------------------------------------
CDXUTControl* CDXUTDialog::GetControl( int ID, UINT nControlType )
{
	// Try to find the control with the given ID
	for( int i=0; i < m_Controls.GetSize(); i++ )
	{
		CDXUTControl* pControl = m_Controls.GetAt( i );

		if( pControl->GetID() == ID && pControl->GetType() == nControlType )
		{
			return pControl;
		}
	}

	// Not found
	return NULL;
}



//--------------------------------------------------------------------------------------
CDXUTControl* CDXUTDialog::GetNextControl( CDXUTControl* pControl )
{
	int index = pControl->m_Index + 1;

	CDXUTDialog* pDialog = pControl->m_pDialog;

	// Cycle through dialogs in the loop to find the next control. Note
	// that if only one control exists in all looped dialogs it will
	// be the returned 'next' control.
	while( index >= (int) pDialog->m_Controls.GetSize() )
	{
		pDialog = pDialog->m_pNextDialog;
		index = 0;
	}

	return pDialog->m_Controls.GetAt( index );    
}

//--------------------------------------------------------------------------------------
CDXUTControl* CDXUTDialog::GetPrevControl( CDXUTControl* pControl )
{
	int index = pControl->m_Index - 1;

	CDXUTDialog* pDialog = pControl->m_pDialog;

	// Cycle through dialogs in the loop to find the next control. Note
	// that if only one control exists in all looped dialogs it will
	// be the returned 'previous' control.
	while( index < 0 )
	{
		pDialog = pDialog->m_pPrevDialog;
		if( pDialog == NULL )
			pDialog = pControl->m_pDialog;

		index = pDialog->m_Controls.GetSize() - 1;
	}

	return pDialog->m_Controls.GetAt( index );    
}


//--------------------------------------------------------------------------------------
void CDXUTDialog::ClearRadioButtonGroup( UINT nButtonGroup )
{
	// Find all radio buttons with the given group number
 
}



//--------------------------------------------------------------------------------------
void CDXUTDialog::ClearComboBox( int ID )
{
	 
}




//--------------------------------------------------------------------------------------
void CDXUTDialog::RequestFocus( CDXUTControl* pControl )
{
	if( s_pControlFocus == pControl )
		return;

	if( !pControl->CanHaveFocus() )
		return;

	if( s_pControlFocus )
		s_pControlFocus->OnFocusOut();

	pControl->OnFocusIn();
	s_pControlFocus = pControl;
}


//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::DrawRect( CEGUI::GeometryBuffer* geobuf, RECT* pRect, D3DCOLOR color )
{
	RECT rcScreen = *pRect;
	OffsetRect( &rcScreen, m_x, m_y );

	// If caption is enabled, offset the Y position by its height.
	if( m_bCaption )
		OffsetRect( &rcScreen, 0, m_nCaptionHeight );

	using namespace CEGUI;
	
	D3DXCOLOR cl(color);
	Colour defcol(cl.r, cl.g, cl.b, cl.a);
	Vector2f texcoord(0, 0);

	Vertex pts[6];

	// left-top
	pts[0].colour_val = defcol; 
	pts[0].tex_coords = texcoord;
	pts[0].position = Vector3f((float)rcScreen.left, (float)rcScreen.top, 0);

	// right-top
	pts[1].colour_val = defcol; 
	pts[1].tex_coords = texcoord;
	pts[1].position = Vector3f((float)rcScreen.right, (float)rcScreen.top, 0);

	// left-bottom
	pts[2].colour_val = defcol; 
	pts[2].tex_coords = texcoord;
	pts[2].position = Vector3f((float)rcScreen.left, (float)rcScreen.bottom, 0);

	// right-top
	pts[3].colour_val = defcol; 
	pts[3].tex_coords = texcoord;
	pts[3].position = Vector3f((float)rcScreen.right, (float)rcScreen.top, 0);

	// right-bottom
	pts[4].colour_val = defcol; 
	pts[4].tex_coords = texcoord;
	pts[4].position = Vector3f((float)rcScreen.right, (float)rcScreen.bottom, 0);

	// left-bottom
	pts[5].colour_val = defcol; 
	pts[5].tex_coords = texcoord;
	pts[5].position = Vector3f((float)rcScreen.left, (float)rcScreen.bottom, 0);

	geobuf->setActiveTexture(NULL);
	geobuf->setClippingRegion(Rectf(0,0,2000,2000));
	geobuf->appendGeometry(&pts[0], 6);


// 	return;
// 
// 	Rectf vertices[4] =
// 	{
// 		(float) rcScreen.left -0.5f,  (float) rcScreen.top -0.5f,    0.5f, 1.0f, color, 0, 0,
// 		(float) rcScreen.right -0.5f, (float) rcScreen.top -0.5f,    0.5f, 1.0f, color, 0, 0, 
// 		(float) rcScreen.right -0.5f, (float) rcScreen.bottom -0.5f, 0.5f, 1.0f, color, 0, 0, 
// 		(float) rcScreen.left -0.5f,  (float) rcScreen.bottom -0.5f, 0.5f, 1.0f, color, 0, 0,
// 	};
// 
// 	IDirect3DDevice9* pd3dDevice = DXUTGetGlobalDialogResourceManager()->GetD3DDevice();
// 
// 	// Since we're doing our own drawing here we need to flush the sprites
// 	DXUTGetGlobalDialogResourceManager()->m_pSprite->Flush();
// 	IDirect3DVertexDeclaration9 *pDecl = NULL;
// 	pd3dDevice->GetVertexDeclaration( &pDecl );  // Preserve the sprite's current vertex decl
// 	pd3dDevice->SetFVF( DXUT_SCREEN_VERTEX::FVF );
// 
// 	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
// 	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
// 
// 	pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, vertices, sizeof(DXUT_SCREEN_VERTEX) );
// 
// 	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
// 	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
// 
// 	// Restore the vertex decl
// 	pd3dDevice->SetVertexDeclaration( pDecl );
// 	pDecl->Release();

	return S_OK;
}


//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::DrawPolyLine( CEGUI::GeometryBuffer* geobuf, POINT* apPoints, UINT nNumPoints, D3DCOLOR color )
{
// 	DXUT_SCREEN_VERTEX* vertices = new DXUT_SCREEN_VERTEX[ nNumPoints ];
// 	if( vertices == NULL )
// 		return E_OUTOFMEMORY;
// 
// 	DXUT_SCREEN_VERTEX* pVertex = vertices;
// 	POINT* pt = apPoints;
// 	for( UINT i=0; i < nNumPoints; i++ )
// 	{
// 		pVertex->x = m_x + (float) pt->x;
// 		pVertex->y = m_y + (float) pt->y;
// 		pVertex->z = 0.5f;
// 		pVertex->h = 1.0f;
// 		pVertex->color = color;
// 		pVertex->tu = 0.0f;
// 		pVertex->tv = 0.0f;
// 
// 		pVertex++;
// 		pt++;
// 	}
// 
// 	IDirect3DDevice9* pd3dDevice = DXUTGetGlobalDialogResourceManager()->GetD3DDevice();
// 
// 	// Since we're doing our own drawing here we need to flush the sprites
// 	DXUTGetGlobalDialogResourceManager()->m_pSprite->Flush();
// 	IDirect3DVertexDeclaration9 *pDecl = NULL;
// 	pd3dDevice->GetVertexDeclaration( &pDecl );  // Preserve the sprite's current vertex decl
// 	pd3dDevice->SetFVF( DXUT_SCREEN_VERTEX::FVF );
// 
// 	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
// 	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
// 
// 	pd3dDevice->DrawPrimitiveUP( D3DPT_LINESTRIP, nNumPoints - 1, vertices, sizeof(DXUT_SCREEN_VERTEX) );
// 
// 	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
// 	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
// 
// 	// Restore the vertex decl
// 	pd3dDevice->SetVertexDeclaration( pDecl );
// 	pDecl->Release();
// 
// 	SAFE_DELETE_ARRAY( vertices );
	return S_OK;
}



//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::DrawSprite( CEGUI::GeometryBuffer* geobuf, CDXUTElement* pElement, RECT* prcDest )
{
	// No need to draw fully transparent layers
	if( pElement->TextureColor.Current.a == 0 )
		return S_OK;

	RECT rcTexture = pElement->rcTexture;

	RECT rcScreen = *prcDest;
	OffsetRect( &rcScreen, m_x, m_y );

	// If caption is enabled, offset the Y position by its height.
	if( m_bCaption )
		OffsetRect( &rcScreen, 0, m_nCaptionHeight );

	DXUTTextureNode* pTextureNode = GetTexture( pElement->iTexture );


	using namespace CEGUI;
	
	Colour defcol(128.0f/255.0f, 128.0f/255.0f, 128.0f/255.0f, 200.0f/255.0f);

	float fTexWidth = pTextureNode->pTexture->getSize().d_width;
	float fTexHeight = pTextureNode->pTexture->getSize().d_height;
	float fTexLeft = rcTexture.left/fTexWidth;
	float fTexRight = rcTexture.right/fTexWidth;
	float fTexTop = rcTexture.top/fTexHeight;
	float fTexBottom = rcTexture.bottom/fTexHeight;
	
	Vertex pts[6];

	// left-top
	pts[0].colour_val = defcol; 
	pts[0].tex_coords = Vector2f(fTexLeft, fTexTop);
	pts[0].position = Vector3f((float)rcScreen.left, (float)rcScreen.top, 0);

	// right-top
	pts[1].colour_val = defcol; 
	pts[1].tex_coords = Vector2f(fTexRight, fTexTop);
	pts[1].position = Vector3f((float)rcScreen.right, (float)rcScreen.top, 0);

	// left-bottom
	pts[2].colour_val = defcol; 
	pts[2].tex_coords = Vector2f(fTexLeft, fTexBottom);
	pts[2].position = Vector3f((float)rcScreen.left, (float)rcScreen.bottom, 0);

	// right-top
	pts[3].colour_val = defcol; 
	pts[3].tex_coords = Vector2f(fTexRight, fTexTop);
	pts[3].position = Vector3f((float)rcScreen.right, (float)rcScreen.top, 0);

	// right-bottom
	pts[4].colour_val = defcol; 
	pts[4].tex_coords = Vector2f(fTexRight, fTexBottom);
	pts[4].position = Vector3f((float)rcScreen.right, (float)rcScreen.bottom, 0);

	// left-bottom
	pts[5].colour_val = defcol; 
	pts[5].tex_coords = Vector2f(fTexLeft, fTexBottom);
	pts[5].position = Vector3f((float)rcScreen.left, (float)rcScreen.bottom, 0);

	geobuf->setActiveTexture(pTextureNode->pTexture);
	//geobuf->setClippingRegion(Rectf(0,0,2000,2000));
	geobuf->appendGeometry(&pts[0], 6);
	
	return TRUE;

	//return DXUTGetGlobalDialogResourceManager()->m_pSprite->Draw( pTextureNode->pTexture, &rcTexture, NULL, &vPos, pElement->TextureColor.Current );
}


//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::CalcTextRect( LPCWSTR strText, CDXUTElement* pElement, RECT* prcDest, int nCount )
{
	HRESULT hr = S_OK;

	DXUTFontNode* pFontNode = GetFont( pElement->iFont );
	
	//pFontNode->pFont->drawText()

	//DWORD dwTextFormat = pElement->dwTextFormat | DT_CALCRECT;
	// Since we are only computing the rectangle, we don't need a sprite.
// 	hr = pFontNode->pFont->DrawText( NULL, strText, nCount, prcDest, dwTextFormat, pElement->FontColor.Current );
// 	if( FAILED(hr) )
// 		return hr;

	CEGUI::String sText(strText);

// 	prcDest->left = 0;
// 	prcDest->top = 0;

	prcDest->right = prcDest->left+pFontNode->pFont->getTextExtent(sText);
	if (wcslen(strText)>0)
	{
		prcDest->bottom = prcDest->top+ pFontNode->pFont->getFontHeight();
	}
	else
	{
		prcDest->bottom = prcDest->top+0;
	}
	

	return S_OK;
}


//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::DrawText( CEGUI::GeometryBuffer* geobuf, LPCWSTR strText, CDXUTElement* pElement, RECT* prcDest, bool bShadow, int nCount )
{
	HRESULT hr = S_OK;

	// No need to draw fully transparent layers
	if( pElement->FontColor.Current.a == 0 )
		return S_OK;

	RECT rcScreen = *prcDest;
	OffsetRect( &rcScreen, m_x, m_y );

	// If caption is enabled, offset the Y position by its height.
	if( m_bCaption )
		OffsetRect( &rcScreen, 0, m_nCaptionHeight );

	//debug
	//DrawRect( &rcScreen, D3DCOLOR_ARGB(100, 255, 0, 0) );

// 	D3DXMATRIXA16 matTransform;
// 	D3DXMatrixIdentity( &matTransform );
// 	DXUTGetGlobalDialogResourceManager()->m_pSprite->SetTransform( &matTransform );

	DXUTFontNode* pFontNode = GetFont( pElement->iFont );

	using namespace CEGUI;
	if( bShadow )
	{
		RECT rcShadow = rcScreen;
		OffsetRect( &rcShadow, 1, 1 );

		Rectf rc1(rcShadow.left, rcShadow.top, rcShadow.right, rcShadow.bottom);
		pFontNode->pFont->drawText( *geobuf, String(strText),
			Vector2f(rcShadow.left, rcShadow.top), &rc1, 
			Colour(0, 0, 0,pElement->FontColor.Current.a) );

	}

	Rectf rc2(rcScreen.left, rcScreen.top, rcScreen.right, rcScreen.bottom);
	Colour col2(pElement->FontColor.Current.r, pElement->FontColor.Current.g, pElement->FontColor.Current.b, pElement->FontColor.Current.a);
	pFontNode->pFont->drawText( *geobuf, String(strText), Vector2f(rcScreen.left, rcScreen.top), &rc2, col2 );
	return S_OK;
}


//--------------------------------------------------------------------------------------
void CDXUTDialog::SetBackgroundColors( D3DCOLOR colorTopLeft, D3DCOLOR colorTopRight, D3DCOLOR colorBottomLeft, D3DCOLOR colorBottomRight )
{
	m_colorTopLeft = colorTopLeft;
	m_colorTopRight = colorTopRight;
	m_colorBottomLeft = colorBottomLeft;
	m_colorBottomRight = colorBottomRight;
}


void CDXUTDialog::SetSize( int width, int height )
{
	m_width = width; m_height = height;
}

//--------------------------------------------------------------------------------------
void CDXUTDialog::SetNextDialog( CDXUTDialog* pNextDialog )
{ 
	if( pNextDialog == NULL )
		pNextDialog = this;

	m_pNextDialog = pNextDialog;
	m_pNextDialog->m_pPrevDialog = this;
}


//--------------------------------------------------------------------------------------
void CDXUTDialog::ClearFocus()
{
	if( s_pControlFocus )
	{
		s_pControlFocus->OnFocusOut();
		s_pControlFocus = NULL;
	}

	ReleaseCapture();
}


//--------------------------------------------------------------------------------------
void CDXUTDialog::FocusDefaultControl()
{
	// Check for default control in this dialog
	for( int i=0; i < m_Controls.GetSize(); i++ )
	{
		CDXUTControl* pControl = m_Controls.GetAt( i );
		if( pControl->m_bIsDefault )
		{
			// Remove focus from the current control
			ClearFocus();

			// Give focus to the default control
			s_pControlFocus = pControl;
			s_pControlFocus->OnFocusIn();
			return;
		}
	}
}



//--------------------------------------------------------------------------------------
void CDXUTDialog::OnCycleFocus( bool bForward )
{
	// This should only be handled by the dialog which owns the focused control, and 
	// only if a control currently has focus
	if( s_pControlFocus == NULL || s_pControlFocus->m_pDialog != this )
		return;

	CDXUTControl* pControl = s_pControlFocus;
	for( int i=0; i < 0xffff; i++ )
	{
		pControl = (bForward) ? GetNextControl( pControl ) : GetPrevControl( pControl );

		// If we've gone in a full circle then focus doesn't change
		if( pControl == s_pControlFocus )
			return;

		// If the dialog accepts keybord input and the control can have focus then
		// move focus
		if( pControl->m_pDialog->m_bKeyboardInput && pControl->CanHaveFocus() )
		{
			s_pControlFocus->OnFocusOut();
			s_pControlFocus = pControl;
			s_pControlFocus->OnFocusIn();
			return;
		}
	}

	// If we reached this point, the chain of dialogs didn't form a complete loop
	DXTRACE_ERR( L"CDXUTDialog: Multiple dialogs are improperly chained together", E_FAIL );
}


//--------------------------------------------------------------------------------------
HRESULT CDXUTDialogResourceManager::CreateFont( UINT iFont )
{
	HRESULT hr = S_OK;

	DXUTFontNode* pFontNode = m_FontCache.GetAt( iFont );

	//SAFE_RELEASE( pFontNode->pFont );

// 	V_RETURN( D3DXCreateFont( m_pd3dDevice, pFontNode->nHeight, 0, pFontNode->nWeight, 1, FALSE, DEFAULT_CHARSET, 
// 		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
// 		pFontNode->strFace, &pFontNode->pFont ) );

	using namespace CEGUI;
	char nm[100], face[MAX_PATH];
	sprintf(nm, "DXUT-Fnt%u", iFont);
	setlocale(LC_CTYPE, "");
	wcstombs(face, pFontNode->strFace, MAX_PATH);
	//Font* ft = System::getSingleton().getDefaultGUIContext().getDefaultFont();
	Font& ft = FontManager::getSingleton().createFreeTypeFont(String(nm), pFontNode->nHeight, true, String("simsun.ttc"));
	System::getSingleton().getDefaultGUIContext().setDefaultFont(&ft);

	pFontNode->pFont = &ft;
	return S_OK;
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTDialogResourceManager::CreateTexture( UINT iTexture )
{
	HRESULT hr = S_OK;

	DXUTTextureNode* pTextureNode = m_TextureCache.GetAt( iTexture );

	// Make sure there's a texture to create
	if( pTextureNode->strFilename[0] == 0 )
		return S_OK;

	// Find the texture on the hard drive
// 	WCHAR strPath[MAX_PATH];
// 	hr = DXUTFindDXSDKMediaFileCch( strPath, MAX_PATH, pTextureNode->strFilename );
// 	if( FAILED(hr) )
// 	{
// 		return DXTRACE_ERR( L"DXUTFindDXSDKMediaFileCch", hr );
// 	}

	// Create texture
// 	D3DXIMAGE_INFO info;
// 	hr =  D3DXCreateTextureFromFileEx( m_pd3dDevice, strPath, D3DX_DEFAULT, D3DX_DEFAULT, 
// 		D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 
// 		D3DX_DEFAULT, D3DX_DEFAULT, 0, 
// 		&info, NULL, &pTextureNode->pTexture );
// 	if( FAILED(hr) )
// 	{
// 		return DXTRACE_ERR( L"D3DXCreateTextureFromFileEx", hr );
// 	}

	using namespace CEGUI;
	char sName[100]={0}, sPath[MAX_PATH]={0};
	sprintf(sName, "DXUT-Tex%u", iTexture);
	setlocale(LC_CTYPE, "");
	wcstombs(sPath, pTextureNode->strFilename, MAX_PATH);
	
	Texture& texobj = CEGUI::System::getSingleton().getRenderer()->createTexture(String(sName), String(sPath), String(L""));
	pTextureNode->pTexture = &texobj;

	// Store dimensions
	pTextureNode->dwWidth = (DWORD)texobj.getSize().d_width;
	pTextureNode->dwHeight = (DWORD)texobj.getSize().d_height;

	return S_OK;
}


//--------------------------------------------------------------------------------------
void CDXUTDialog::InitDefaultElements()
{
	SetTexture( 0, L"UI\\DXUTControls.png" );
	SetFont( 0, L"Arial", 14, FW_NORMAL );

	CDXUTElement Element;
	RECT rcTexture;

	//-------------------------------------
	// Element for the caption
	//-------------------------------------
	m_CapElement.SetFont( 0 );
	SetRect( &rcTexture, 17, 269, 241, 287 );
	m_CapElement.SetTexture( 0, &rcTexture );
	m_CapElement.TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	m_CapElement.FontColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	m_CapElement.SetFont( 0, D3DCOLOR_ARGB(255, 255, 255, 255), DT_LEFT | DT_VCENTER );
	// Pre-blend as we don't need to transition the state
	m_CapElement.TextureColor.Blend( DXUT_STATE_NORMAL, 10.0f );
	m_CapElement.FontColor.Blend( DXUT_STATE_NORMAL, 10.0f );

	//-------------------------------------
	// CDXUTStatic
	//-------------------------------------
	Element.SetFont( 0 );
	Element.FontColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB( 200, 200, 200, 200 );

	// Assign the Element
	SetDefaultElement( DXUT_CONTROL_STATIC, 0, &Element );


	//-------------------------------------
	// CDXUTButton - Button
	//-------------------------------------
	SetRect( &rcTexture, 0, 0, 136, 54 );
	Element.SetTexture( 0, &rcTexture );
	Element.SetFont( 0 );
	Element.TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(150, 255, 255, 255);
	Element.TextureColor.States[ DXUT_STATE_PRESSED ] = D3DCOLOR_ARGB(200, 255, 255, 255);
	Element.FontColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 0, 0, 0);

	// Assign the Element
	SetDefaultElement( DXUT_CONTROL_BUTTON, 0, &Element );


	//-------------------------------------
	// CDXUTButton - Fill layer
	//-------------------------------------
	SetRect( &rcTexture, 136, 0, 272, 54 );
	Element.SetTexture( 0, &rcTexture, D3DCOLOR_ARGB(0, 255, 255, 255) );
	Element.TextureColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(160, 255, 255, 255);
	Element.TextureColor.States[ DXUT_STATE_PRESSED ] = D3DCOLOR_ARGB(60, 0, 0, 0);
	Element.TextureColor.States[ DXUT_STATE_FOCUS ] = D3DCOLOR_ARGB(30, 255, 255, 255);


	// Assign the Element
	SetDefaultElement( DXUT_CONTROL_BUTTON, 1, &Element );


	//-------------------------------------
	// CDXUTCheckBox - Box
	//-------------------------------------
	SetRect( &rcTexture, 0, 54, 27, 81 );
	Element.SetTexture( 0, &rcTexture );
	Element.SetFont( 0, D3DCOLOR_ARGB(255, 255, 255, 255), DT_LEFT | DT_VCENTER );
	Element.FontColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB( 200, 200, 200, 200 );
	Element.TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(150, 255, 255, 255);
	Element.TextureColor.States[ DXUT_STATE_FOCUS ] = D3DCOLOR_ARGB(200, 255, 255, 255);
	Element.TextureColor.States[ DXUT_STATE_PRESSED ] = D3DCOLOR_ARGB(255, 255, 255, 255);

	// Assign the Element
	SetDefaultElement( DXUT_CONTROL_CHECKBOX, 0, &Element );


	//-------------------------------------
	// CDXUTCheckBox - Check
	//-------------------------------------
	SetRect( &rcTexture, 27, 54, 54, 81 );
	Element.SetTexture( 0, &rcTexture );

	// Assign the Element
	SetDefaultElement( DXUT_CONTROL_CHECKBOX, 1, &Element );


	//-------------------------------------
	// CDXUTRadioButton - Box
	//-------------------------------------
	SetRect( &rcTexture, 54, 54, 81, 81 );
	Element.SetTexture( 0, &rcTexture );
	Element.SetFont( 0, D3DCOLOR_ARGB(255, 255, 255, 255), DT_LEFT | DT_VCENTER );
	Element.FontColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB( 200, 200, 200, 200 );
	Element.TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(150, 255, 255, 255);
	Element.TextureColor.States[ DXUT_STATE_FOCUS ] = D3DCOLOR_ARGB(200, 255, 255, 255);
	Element.TextureColor.States[ DXUT_STATE_PRESSED ] = D3DCOLOR_ARGB(255, 255, 255, 255);

	// Assign the Element
	SetDefaultElement( DXUT_CONTROL_RADIOBUTTON, 0, &Element );


	//-------------------------------------
	// CDXUTRadioButton - Check
	//-------------------------------------
	SetRect( &rcTexture, 81, 54, 108, 81 );
	Element.SetTexture( 0, &rcTexture );

	// Assign the Element
	SetDefaultElement( DXUT_CONTROL_RADIOBUTTON, 1, &Element );


	//-------------------------------------
	// CDXUTComboBox - Main
	//-------------------------------------
	SetRect( &rcTexture, 7, 81, 247, 123 );
	Element.SetTexture( 0, &rcTexture );
	Element.SetFont( 0 );
	Element.TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(150, 200, 200, 200);
	Element.TextureColor.States[ DXUT_STATE_FOCUS ] = D3DCOLOR_ARGB(170, 230, 230, 230);
	Element.TextureColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB(70, 200, 200, 200);
	Element.FontColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 0, 0, 0);
	Element.FontColor.States[ DXUT_STATE_PRESSED ] = D3DCOLOR_ARGB(255, 0, 0, 0);
	Element.FontColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB(200, 200, 200, 200);


	// Assign the Element
	SetDefaultElement( DXUT_CONTROL_COMBOBOX, 0, &Element );


	//-------------------------------------
	// CDXUTComboBox - Button
	//-------------------------------------
	SetRect( &rcTexture, 272, 0, 325, 49 );
	Element.SetTexture( 0, &rcTexture );
	Element.TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(150, 255, 255, 255);
	Element.TextureColor.States[ DXUT_STATE_PRESSED ] = D3DCOLOR_ARGB(255, 150, 150, 150);
	Element.TextureColor.States[ DXUT_STATE_FOCUS ] = D3DCOLOR_ARGB(200, 255, 255, 255);
	Element.TextureColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB(70, 255, 255, 255);

	// Assign the Element
	SetDefaultElement( DXUT_CONTROL_COMBOBOX, 1, &Element );


	//-------------------------------------
	// CDXUTComboBox - Dropdown
	//-------------------------------------
	SetRect( &rcTexture, 7, 123, 241, 265 );
	Element.SetTexture( 0, &rcTexture );
	Element.SetFont( 0, D3DCOLOR_ARGB(255, 0, 0, 0), DT_LEFT | DT_TOP );

	// Assign the Element
	SetDefaultElement( DXUT_CONTROL_COMBOBOX, 2, &Element );


	//-------------------------------------
	// CDXUTComboBox - Selection
	//-------------------------------------
	SetRect( &rcTexture, 7, 266, 241, 289 );
	Element.SetTexture( 0, &rcTexture );
	Element.SetFont( 0, D3DCOLOR_ARGB(255, 255, 255, 255), DT_LEFT | DT_TOP );

	// Assign the Element
	SetDefaultElement( DXUT_CONTROL_COMBOBOX, 3, &Element );


	//-------------------------------------
	// CDXUTSlider - Track
	//-------------------------------------
	SetRect( &rcTexture, 1, 290, 280, 331 );
	Element.SetTexture( 0, &rcTexture );
	Element.TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(150, 255, 255, 255);
	Element.TextureColor.States[ DXUT_STATE_FOCUS ] = D3DCOLOR_ARGB(200, 255, 255, 255);
	Element.TextureColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB(70, 255, 255, 255);

	// Assign the Element
	SetDefaultElement( DXUT_CONTROL_SLIDER, 0, &Element );

	//-------------------------------------
	// CDXUTSlider - Button
	//-------------------------------------
	SetRect( &rcTexture, 248, 55, 289, 96 );
	Element.SetTexture( 0, &rcTexture );

	// Assign the Element
	SetDefaultElement( DXUT_CONTROL_SLIDER, 1, &Element );

	//-------------------------------------
	// CDXUTScrollBar - Track
	//-------------------------------------
	SetRect( &rcTexture, 243, 144, 265, 155 );
	Element.SetTexture( 0, &rcTexture );
	Element.TextureColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 200, 200, 200);


	// Assign the Element
	SetDefaultElement( DXUT_CONTROL_SCROLLBAR, 0, &Element );

	//-------------------------------------
	// CDXUTScrollBar - Up Arrow
	//-------------------------------------
	SetRect( &rcTexture, 243, 124, 265, 144 );
	Element.SetTexture( 0, &rcTexture );
	Element.TextureColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 200, 200, 200);


	// Assign the Element
	SetDefaultElement( DXUT_CONTROL_SCROLLBAR, 1, &Element );

	//-------------------------------------
	// CDXUTScrollBar - Down Arrow
	//-------------------------------------
	SetRect( &rcTexture, 243, 155, 265, 176 );
	Element.SetTexture( 0, &rcTexture );
	Element.TextureColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 200, 200, 200);


	// Assign the Element
	SetDefaultElement( DXUT_CONTROL_SCROLLBAR, 2, &Element );

	//-------------------------------------
	// CDXUTScrollBar - Button
	//-------------------------------------
	SetRect( &rcTexture, 266, 123, 286, 167 );
	Element.SetTexture( 0, &rcTexture );

	// Assign the Element
	SetDefaultElement( DXUT_CONTROL_SCROLLBAR, 3, &Element );


	//-------------------------------------
	// CDXUTEditBox
	//-------------------------------------
	// Element assignment:
	//   0 - text area
	//   1 - top left border
	//   2 - top border
	//   3 - top right border
	//   4 - left border
	//   5 - right border
	//   6 - lower left border
	//   7 - lower border
	//   8 - lower right border

	Element.SetFont( 0, D3DCOLOR_ARGB( 255, 0, 0, 0 ), DT_LEFT | DT_TOP );

	// Assign the style
	SetRect( &rcTexture, 14, 90, 241, 113 );
	Element.SetTexture( 0, &rcTexture );
	SetDefaultElement( DXUT_CONTROL_EDITBOX, 0, &Element );
	SetRect( &rcTexture, 8, 82, 14, 90 );
	Element.SetTexture( 0, &rcTexture );
	SetDefaultElement( DXUT_CONTROL_EDITBOX, 1, &Element );
	SetRect( &rcTexture, 14, 82, 241, 90 );
	Element.SetTexture( 0, &rcTexture );
	SetDefaultElement( DXUT_CONTROL_EDITBOX, 2, &Element );
	SetRect( &rcTexture, 241, 82, 246, 90 );
	Element.SetTexture( 0, &rcTexture );
	SetDefaultElement( DXUT_CONTROL_EDITBOX, 3, &Element );
	SetRect( &rcTexture, 8, 90, 14, 113 );
	Element.SetTexture( 0, &rcTexture );
	SetDefaultElement( DXUT_CONTROL_EDITBOX, 4, &Element );
	SetRect( &rcTexture, 241, 90, 246, 113 );
	Element.SetTexture( 0, &rcTexture );
	SetDefaultElement( DXUT_CONTROL_EDITBOX, 5, &Element );
	SetRect( &rcTexture, 8, 113, 14, 121 );
	Element.SetTexture( 0, &rcTexture );
	SetDefaultElement( DXUT_CONTROL_EDITBOX, 6, &Element );
	SetRect( &rcTexture, 14, 113, 241, 121 );
	Element.SetTexture( 0, &rcTexture );
	SetDefaultElement( DXUT_CONTROL_EDITBOX, 7, &Element );
	SetRect( &rcTexture, 241, 113, 246, 121 );
	Element.SetTexture( 0, &rcTexture );
	SetDefaultElement( DXUT_CONTROL_EDITBOX, 8, &Element );


	//-------------------------------------
	// CDXUTIMEEditBox
	//-------------------------------------

	Element.SetFont( 0, D3DCOLOR_ARGB( 255, 0, 0, 0 ), DT_LEFT | DT_TOP );

	// Assign the style
	SetRect( &rcTexture, 14, 90, 241, 113 );
	Element.SetTexture( 0, &rcTexture );
	SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 0, &Element );
	SetRect( &rcTexture, 8, 82, 14, 90 );
	Element.SetTexture( 0, &rcTexture );
	SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 1, &Element );
	SetRect( &rcTexture, 14, 82, 241, 90 );
	Element.SetTexture( 0, &rcTexture );
	SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 2, &Element );
	SetRect( &rcTexture, 241, 82, 246, 90 );
	Element.SetTexture( 0, &rcTexture );
	SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 3, &Element );
	SetRect( &rcTexture, 8, 90, 14, 113 );
	Element.SetTexture( 0, &rcTexture );
	SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 4, &Element );
	SetRect( &rcTexture, 241, 90, 246, 113 );
	Element.SetTexture( 0, &rcTexture );
	SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 5, &Element );
	SetRect( &rcTexture, 8, 113, 14, 121 );
	Element.SetTexture( 0, &rcTexture );
	SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 6, &Element );
	SetRect( &rcTexture, 14, 113, 241, 121 );
	Element.SetTexture( 0, &rcTexture );
	SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 7, &Element );
	SetRect( &rcTexture, 241, 113, 246, 121 );
	Element.SetTexture( 0, &rcTexture );
	SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 8, &Element );
	// Element 9 for IME text, and indicator button
	SetRect( &rcTexture, 0, 0, 136, 54 );
	Element.SetTexture( 0, &rcTexture );
	Element.SetFont( 0, D3DCOLOR_ARGB( 255, 0, 0, 0 ), DT_CENTER | DT_VCENTER );
	SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 9, &Element );

	//-------------------------------------
	// CDXUTListBox - Main
	//-------------------------------------

	SetRect( &rcTexture, 13, 124, 241, 265 );
	Element.SetTexture( 0, &rcTexture );
	Element.SetFont( 0, D3DCOLOR_ARGB(255, 0, 0, 0), DT_LEFT | DT_TOP );

	// Assign the Element
	SetDefaultElement( DXUT_CONTROL_LISTBOX, 0, &Element );

	//-------------------------------------
	// CDXUTListBox - Selection
	//-------------------------------------

	SetRect( &rcTexture, 17, 269, 241, 287 );
	Element.SetTexture( 0, &rcTexture );
	Element.SetFont( 0, D3DCOLOR_ARGB(255, 255, 255, 255), DT_LEFT | DT_TOP );

	// Assign the Element
	SetDefaultElement( DXUT_CONTROL_LISTBOX, 1, &Element );
}


UIParent::UIParent(void)
{
	m_pDialog=NULL;
}

UIParent::~UIParent(void)
{

}

bool UIParent::OnWndMsg( const CEGUI::EventArgs& evt)
{
	if (m_pDialog)
	{
		 HWND hWnd = m_pDialog->m_kMsg.hWnd;
		 UINT uMsg = m_pDialog->m_kMsg.message;
		 WPARAM wParam = m_pDialog->m_kMsg.wParam;
		 LPARAM lParam = m_pDialog->m_kMsg.lParam;
		
		return m_pDialog->MsgProc(hWnd, uMsg, wParam, lParam);
	}

	return false;
}

LRESULT UIParent::WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	m_pDialog->m_kMsg.hWnd = hWnd;
	m_pDialog->m_kMsg.message = message;
	m_pDialog->m_kMsg.bResult = FALSE;
	m_pDialog->m_kMsg.wParam = wParam;
	m_pDialog->m_kMsg.lParam = lParam;

	bool bRet = false;
	switch(message)
	{
	case WM_CHAR:
		bRet = m_pGUI->injectChar((CEGUI::utf32)wParam);
		break;

	case WM_MOUSELEAVE:
		//mouseLeaves();
		break;

	case WM_NCMOUSEMOVE:
		//mouseLeaves();
		break;

	case WM_MOUSEMOVE:
		//mouseEnters();

		bRet = m_pGUI->injectMousePosition((float)(LOWORD(lParam)), (float)(HIWORD(lParam)));
		break;

	case WM_LBUTTONDOWN:
		bRet = m_pGUI->injectMouseButtonDown(CEGUI::LeftButton);
		break;

	case WM_LBUTTONUP:
		bRet = m_pGUI->injectMouseButtonUp(CEGUI::LeftButton);
		break;

	case WM_RBUTTONDOWN:
		bRet = m_pGUI->injectMouseButtonDown(CEGUI::RightButton);
		break;

	case WM_RBUTTONUP:
		bRet = m_pGUI->injectMouseButtonUp(CEGUI::RightButton);
		break;

	case WM_MBUTTONDOWN:
		bRet = m_pGUI->injectMouseButtonDown(CEGUI::MiddleButton);
		break;

	case WM_MBUTTONUP:
		bRet = m_pGUI->injectMouseButtonUp(CEGUI::MiddleButton);
		break;

	case 0x020A: // WM_MOUSEWHEEL:
		bRet = m_pGUI->injectMouseWheelChange(static_cast<float>((short)HIWORD(wParam)) / static_cast<float>(120));
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		bRet = true;
		break;

	case WM_SIZE:
		{
			// get CEGUI::System as a pointer so we can detect if it's not
			// instantiated yet.
			CEGUI::System* cegui_system = CEGUI::System::getSingletonPtr();

			// only continue if CEGUI is up an running and window was not
			// minimised (else it's just a waste of time)
			if ((cegui_system != 0) && (wParam != SIZE_MINIMIZED))
			{
				// get renderer identification string
				CEGUI::Renderer* renderer = cegui_system->getRenderer();
				CEGUI::String id(renderer->getIdentifierString());
				if (id.find("Official Direct3D 9") != id.npos)
					DeviceReset_Direct3D92(hWnd, renderer);
			}
			bRet = true;
		}
		break;

	case WM_PAINT:
		{
			HDC         hDC;
			PAINTSTRUCT ps;

			hDC = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			bRet = true;
			break;
		}

	default:
		break;
	}

	if (bRet)
		return TRUE;

	using namespace CEGUI;
	if (m_pDialog->MsgProc(hWnd, message, wParam, lParam))
		return TRUE;

	if( CDXUTIMEEditBox::StaticMsgProc( hWnd, message, wParam, lParam ) )
		return TRUE;
	else
		return DefWindowProc(hWnd, message, wParam, lParam);
}

void UIParent::DeviceReset_Direct3D92(HWND window, CEGUI::Renderer* renderer)
{
	CEGUI::Direct3D9Renderer* d3d_renderer =
		static_cast<CEGUI::Direct3D9Renderer*>(renderer);

	LPDIRECT3DDEVICE9 d3d_device = d3d_renderer->getDevice();

	LPDIRECT3D9 d3d;
	d3d_device->GetDirect3D(&d3d);

	D3DDISPLAYMODE d3ddm;
	d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);

	D3DPRESENT_PARAMETERS ppars;
	ZeroMemory(&ppars, sizeof(ppars));
	ppars.BackBufferFormat = d3ddm.Format;
	ppars.hDeviceWindow    = window;
	ppars.SwapEffect       = D3DSWAPEFFECT_DISCARD;
	ppars.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	ppars.Windowed         = true;

	d3d_renderer->preD3DReset();
	d3d_device->Reset(&ppars);
	d3d_renderer->postD3DReset();
}


UIParent* g_UIParent = NULL;

LRESULT CALLBACK UIParent::StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (NULL==g_UIParent)
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return g_UIParent->WndProc(hWnd, message, wParam, lParam);
}

HWND createApplicationWindow( int width, int height )
{
	static const wchar_t* APPLICATION_NAME = L"UIParentClass";
	static const wchar_t* REGISTER_CLASS_ERROR= L"UIParentClass Register failed!";

	WNDCLASS    wndClass;       // structure used to register window class

	// Initialise WNDCLASS structure.
	wndClass.style          = 0;
	wndClass.lpfnWndProc    = &UIParent::StaticWndProc;
	wndClass.cbClsExtra     = 0;
	wndClass.cbWndExtra     = 0;
	wndClass.hInstance      = GetModuleHandle(0);
	wndClass.hIcon          = LoadIcon(0, IDI_WINLOGO);
	wndClass.hCursor        = LoadCursor(0, IDC_ARROW);
	wndClass.hbrBackground  = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wndClass.lpszMenuName   = 0;
	wndClass.lpszClassName  = APPLICATION_NAME;

	HWND window = 0;

	// register class.  Report error & exit upon failure
	if (RegisterClass(&wndClass))
	{
		// get size of window to use so that client area is desired size
		RECT wnd_size = {0,0,width,height};
		AdjustWindowRect(&wnd_size, WS_OVERLAPPEDWINDOW, FALSE);

		// create new window
		window = CreateWindow(APPLICATION_NAME, APPLICATION_NAME,
			WS_OVERLAPPEDWINDOW,
			0, 0,
			wnd_size.right - wnd_size.left,
			wnd_size.bottom - wnd_size.top,
			0, 0, GetModuleHandle(0), 0);
	}
	else
	{
		MessageBox(0, REGISTER_CLASS_ERROR, APPLICATION_NAME, MB_ICONERROR|MB_OK);
	}

	return window;
}

bool initialiseDirect3D( LPDIRECT3DDEVICE9& device, HWND d_window, unsigned int width, unsigned int height, unsigned int adapter, bool windowed )
{
	IDirect3D9 *m_lpD3D9 = Direct3DCreate9(D3D_SDK_VERSION);

	// display error and exit if D3D creation failed
	if (m_lpD3D9)
	{
		D3DDISPLAYMODE d3ddm;
		m_lpD3D9->GetAdapterDisplayMode(adapter, &d3ddm);

		D3DFORMAT format = d3ddm.Format;

		// complete window initialisation
		ShowWindow(d_window, SW_NORMAL);
		UpdateWindow(d_window);

		D3DPRESENT_PARAMETERS d_ppars;
		ZeroMemory(&d_ppars, sizeof(d_ppars));
		d_ppars.BackBufferFormat = format;
		d_ppars.hDeviceWindow    = d_window;
		d_ppars.SwapEffect       = D3DSWAPEFFECT_DISCARD;
		d_ppars.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		d_ppars.Windowed         = windowed;

		if (!windowed)
		{
			d_ppars.BackBufferWidth          = width;
			d_ppars.BackBufferHeight         = height;
			d_ppars.BackBufferCount          = 1;
			d_ppars.MultiSampleType          = D3DMULTISAMPLE_NONE;
			d_ppars.FullScreen_RefreshRateInHz       = D3DPRESENT_RATE_DEFAULT;
		}

		if (SUCCEEDED(m_lpD3D9->CreateDevice(adapter, D3DDEVTYPE_HAL, d_window, 
			D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d_ppars, &device)))
		{
			return true;
		}
		else
		{
			//MessageBox(0, Win32AppHelper::CREATE_DEVICE_ERROR, Win32AppHelper::APPLICATION_NAME, MB_ICONERROR|MB_OK);
		}

		m_lpD3D9->Release();
		m_lpD3D9 = 0;
	}
	else
	{
		//MessageBox(0, Win32AppHelper::CREATE_D3D_ERROR, Win32AppHelper::APPLICATION_NAME, MB_ICONERROR|MB_OK);
	}

	return false;
}

void initialiseResourceGroupDirectories()
{
	// initialise the required dirs for the DefaultResourceProvider
	CEGUI::DefaultResourceProvider* rp =
		static_cast<CEGUI::DefaultResourceProvider*>
		(CEGUI::System::getSingleton().getResourceProvider());

	const char* dataPathPrefix = getDataPathPrefix();
	char resourcePath[MAX_PATH];

	// for each resource type, set a resource group directory
	sprintf(resourcePath, "%s/%s", dataPathPrefix, "schemes/");
	rp->setResourceGroupDirectory("schemes", resourcePath);
	sprintf(resourcePath, "%s/%s", dataPathPrefix, "imagesets/");
	rp->setResourceGroupDirectory("imagesets", resourcePath);
	sprintf(resourcePath, "%s/%s", dataPathPrefix, "fonts/");
	rp->setResourceGroupDirectory("fonts", resourcePath);
	sprintf(resourcePath, "%s/%s", dataPathPrefix, "layouts/");
	rp->setResourceGroupDirectory("layouts", resourcePath);
	sprintf(resourcePath, "%s/%s", dataPathPrefix, "looknfeel/");
	rp->setResourceGroupDirectory("looknfeels", resourcePath);
	sprintf(resourcePath, "%s/%s", dataPathPrefix, "lua_scripts/");
	rp->setResourceGroupDirectory("lua_scripts", resourcePath);
	sprintf(resourcePath, "%s/%s", dataPathPrefix, "xml_schemas/");
	rp->setResourceGroupDirectory("schemas", resourcePath);   
	sprintf(resourcePath, "%s/%s", dataPathPrefix, "animations/");
	rp->setResourceGroupDirectory("animations", resourcePath);
}

void initialiseDefaultResourceGroups()
{
	// set the default resource groups to be used
	CEGUI::ImageManager::setImagesetDefaultResourceGroup("imagesets");
	CEGUI::Font::setDefaultResourceGroup("fonts");
	CEGUI::Scheme::setDefaultResourceGroup("schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
	CEGUI::WindowManager::setDefaultResourceGroup("layouts");
	CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");
	CEGUI::AnimationManager::setDefaultResourceGroup("animations");

	// setup default group for validation schemas
	CEGUI::XMLParser* parser = CEGUI::System::getSingleton().getXMLParser();
	if (parser->isPropertyPresent("SchemaDefaultResourceGroup"))
		parser->setProperty("SchemaDefaultResourceGroup", "schemas");
}


bool UIParent::Init(HWND hWnd, LPDIRECT3DDEVICE9 lpDevice)
{
	m_hWnd = hWnd;
	m_lpDevice = lpDevice;

	using namespace CEGUI;
	m_pRoot = new CEGUI::DefaultWindow("DefaultWindow", "root");
	m_pDialog = new CDXUTDialog;
	m_pDialog->SetSize(800,600);
	System::getSingleton().subscribeEvent("win32msg", Event::Subscriber(&UIParent::OnWndMsg, this) );

	CDXUTIMEEditBox::StaticOnCreateDevice(m_hWnd);

	CDXUTIMEEditBox* ppCreated = NULL;
	m_pDialog->AddIMEEditBox(11, L"国产苹果手机微软Office365正版授权", 100, 200, 600, 40,  true, &ppCreated);
	ppCreated->SetWND(m_hWnd);
	m_pRoot->addChild(ppCreated);

	CDXUTEditBox* pEditor;
	m_pDialog->AddEditBox(12, L"微软Office365正版授权", 400, 50, 300, 40,  true, &pEditor);
	pEditor->SetWND(m_hWnd);
	m_pRoot->addChild(pEditor);

	m_pGUI = &System::getSingleton().getDefaultGUIContext();
	m_pGUI->setRootWindow(m_pRoot);

	//命名空间，凡是需要使用CEGUI的函数，都应该声明这个命名空间。
	using namespace CEGUI;

	//设置大纲，或者叫风格吧。CEGUI使用falagard皮肤系统，
	//不过不只是简单的换一个表面，而是可以更换布局的级别。
	SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");

	//获取CEGUI的窗口管理器对象（设计模式-单件，
	//它提供了更加安全的访问机制，暂时就当获取全局变量好了。）
	WindowManager&winMgr=WindowManager::getSingleton();

	//创建一个默认样式的窗口。默认样式不是通过大纲文件配置的，而是CEGUI系统默认的，因此它总是可用。
	DefaultWindow*root=(DefaultWindow*)winMgr.createWindow("DefaultWindow","Root");

	//创建FrameWindow。所谓FrameWindow就是一个带有标题栏和工作区的窗口。
	Window*wnd=(Window*)winMgr.createWindow("TaharezLook/FrameWindow","DemoWindow");

	//现在将创建的FrameWindow和rootWindow关联起来，这样才能显示出来。
	m_pRoot->addChild(wnd);

	//一组设置窗口坐标、大小、拉伸上下限的操作。
	wnd->setPosition(UVector2(cegui_reldim(0.25f),cegui_reldim(0.25f)));
	wnd->setSize(USize(cegui_reldim(0.5f),cegui_reldim(0.5f)));
	wnd->setMaxSize(USize(cegui_reldim(1.0f),cegui_reldim(1.0f)));
	wnd->setMinSize(USize(cegui_reldim(0.1f),cegui_reldim(0.1f)));

	//设置窗体的标题栏
	wnd->setText("HelloWorld!");

	return true;
}

void UIParent::Draw()
{
	m_lpDevice->BeginScene();
	m_lpDevice->Clear(0, 0, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0);

	CEGUI::System::getSingleton().getRenderer()->beginRendering();
	m_pRoot->update(0);
	m_pGUI->draw();
	CEGUI::System::getSingleton().getRenderer()->endRendering();
	CEGUI::WindowManager::getSingleton().cleanDeadPool();

	m_lpDevice->EndScene();
	m_lpDevice->Present(0,0,0,0);
}

bool doWin32Events(bool& idle)
{
	MSG msg;

	if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return false;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
		idle = false;
	}
	else
	{
		idle = true;
	}

	return true;
}

int main(_In_ int _Argc, _In_count_(_Argc) _Pre_z_ char ** _Argv, _In_z_ char ** _Env)
{
	HWND hWnd = createApplicationWindow(800, 600);

	LPDIRECT3DDEVICE9 device = NULL;
	initialiseDirect3D(device, hWnd, 800, 600, D3DADAPTER_DEFAULT, true);

	CEGUI::Direct3D9Renderer::bootstrapSystem(device);
	initialiseResourceGroupDirectories();
	initialiseDefaultResourceGroups();

	UIParent uiparet;
	g_UIParent = &uiparet;
	uiparet.Init(hWnd, device);

	//uiparet.setName("UIParent");

	bool idle = false;
	while( doWin32Events(idle) )
	{
		if (idle)
		{
			uiparet.Draw();
		}
	}

	return 0;
}


