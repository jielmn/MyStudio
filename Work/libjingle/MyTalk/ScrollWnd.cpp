#include "stdafx.h"
#include "ScrollWnd.h"

#define HORI_BAR_HEIGHT        17
#define VERT_BAR_WIDTH         17
#define PAGE_SIZE              15

BEGIN_MESSAGE_MAP(CSCrollWnd, CWnd)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
END_MESSAGE_MAP()

CSCrollWnd::CSCrollWnd( )
{
	m_pGrandChild       = NULL;
	m_GrandChildSize.cx = 0;
	m_GrandChildSize.cy = 0;
	m_nHoriPos          = 0;
	m_nVertPos          = 0;
	m_bAlwaysShow       = FALSE;
	m_bHCanScroll       = FALSE;
	m_bVCanScroll       = FALSE;
}

CSCrollWnd::~CSCrollWnd( )
{


}

BOOL CSCrollWnd::AddChild( CWnd * pGrandChild )
{
	if ( NULL == pGrandChild )
	{
		return FALSE;
	}
	
	if ( !::IsWindow(m_childWnd.GetSafeHwnd()) )
	{
		return FALSE;
	}
	
	pGrandChild->SetParent( &m_childWnd );
	CRect rect;
	pGrandChild->GetWindowRect(rect);
	m_GrandChildSize.cx = rect.Width();
	m_GrandChildSize.cy	= rect.Height();
	m_pGrandChild       = pGrandChild;
	pGrandChild->ShowWindow(SW_SHOW);

	return TRUE;
}

BOOL CSCrollWnd::Create( UINT nID, CWnd* pParentWnd /* = NULL */, BOOL bAlwaysShow /* = FALSE */ )
{
	if ( !CWnd::Create( NULL, NULL, WS_CHILD , CRect(0, 0, 0, 0), pParentWnd, nID, NULL ) )
	{
		return FALSE;
	}
	
	if ( !m_childWnd.Create( NULL, 0, CRect(0, 0, 0, 0), this ) )
	{
		return FALSE;
	}

	
	m_childWnd.ShowWindow(SW_SHOW);

	m_bAlwaysShow = bAlwaysShow;
	return TRUE;
}

BOOL CSCrollWnd::Resize( const CSize & size )
{
	return Resize( size.cx, size.cy );
}

BOOL CSCrollWnd::Resize( int cx, int cy )
{
	CRect rect;
	this->GetWindowRect( rect );

	CWnd * pParent = GetParent();
	ASSERT ( pParent );

	pParent->ScreenToClient( rect );
	this->MoveWindow( rect.left, rect.top, cx, cy );

	CSize sizeChild(cx, cy);
	sizeChild.cx -= VERT_BAR_WIDTH;
	sizeChild.cy -= HORI_BAR_HEIGHT;

	if ( sizeChild.cx < 0 )
	{
		sizeChild.cx = 0;
	}

	if ( sizeChild.cy < 0 )
	{
		sizeChild.cy = 0;
	}

	m_childWnd.MoveWindow( 0, 0, sizeChild.cx, sizeChild.cy	);
	
	return TRUE;
}

SIZE CSCrollWnd::GetShowSize()
{
	SIZE size;
	CRect rect;
	m_childWnd.GetClientRect( rect );
	size.cx = rect.Width();
	size.cy = rect.Height();
	return size;
}

BOOL CSCrollWnd::ChangeChildSize( int cx, int cy )
{
	m_GrandChildSize.cx = cx;
	m_GrandChildSize.cy = cy;

	return TRUE;
}

BOOL CSCrollWnd::ChangeChildSize( const CSize & size )
{
	return ChangeChildSize( size.cx, size.cy );
}

VOID CSCrollWnd::UpdateScrollBar()
{
	if ( m_pGrandChild )
	{
		SCROLLINFO scrollInfo;

		SIZE sizeChild = GetShowSize();

		if ( m_GrandChildSize.cx > sizeChild.cx )
		{
			ShowScrollBar( SB_HORZ, TRUE );

			scrollInfo.cbSize = sizeof(scrollInfo);
			scrollInfo.fMask  = SIF_ALL;
			scrollInfo.nMin   = 0;
			scrollInfo.nMax   = m_GrandChildSize.cx;
			scrollInfo.nPage  = sizeChild.cx;
			scrollInfo.nPos   = m_nHoriPos > (int)(scrollInfo.nMax - scrollInfo.nPage) ? (int)(scrollInfo.nMax - scrollInfo.nPage) : m_nHoriPos;
			scrollInfo.nTrackPos = 0;

			SetScrollInfo( SB_HORZ, &scrollInfo );

			m_nHoriPos = scrollInfo.nPos;

			m_bHCanScroll = TRUE;
		}
		else
		{
			if ( m_bAlwaysShow )
			{
				scrollInfo.cbSize = sizeof(scrollInfo);
				scrollInfo.fMask  = SIF_ALL;
				scrollInfo.nMin   = 0;
				scrollInfo.nMax   = 0xFFFF;
				scrollInfo.nPage  = 0xFFFF - 1;
				scrollInfo.nPos   = 0;
				scrollInfo.nTrackPos = 0;
				SetScrollInfo( SB_HORZ, &scrollInfo );
			}
			else
			{
				ShowScrollBar( SB_HORZ, FALSE );
			}
			
			m_nHoriPos = 0;

			m_bHCanScroll = FALSE;
		}

		if ( m_GrandChildSize.cy > sizeChild.cy )
		{
			ShowScrollBar( SB_VERT, TRUE );
			
			scrollInfo.cbSize = sizeof(scrollInfo);
			scrollInfo.fMask  = SIF_ALL;
			scrollInfo.nMin   = 0;
			scrollInfo.nMax   = m_GrandChildSize.cy;
			scrollInfo.nPage  = sizeChild.cy;
			scrollInfo.nPos   = m_nVertPos > int(scrollInfo.nMax - scrollInfo.nPage) ? int(scrollInfo.nMax - scrollInfo.nPage) : m_nVertPos;
			scrollInfo.nTrackPos = 0;

			SetScrollInfo( SB_VERT, &scrollInfo );

			m_nVertPos = scrollInfo.nPos;

			m_bVCanScroll = TRUE;
		}
		else
		{
			if ( m_bAlwaysShow )
			{
				scrollInfo.cbSize = sizeof(scrollInfo);
				scrollInfo.fMask  = SIF_ALL;
				scrollInfo.nMin   = 0;
				scrollInfo.nMax   = 0xFFFF;
				scrollInfo.nPage  = 0xFFFF - 1;
				scrollInfo.nPos   = 0;
				scrollInfo.nTrackPos = 0;
				SetScrollInfo( SB_VERT, &scrollInfo );
			}
			else
			{
				ShowScrollBar( SB_VERT, FALSE );
			}

			m_nVertPos = 0;

			m_bVCanScroll = FALSE;
		}

		m_pGrandChild->MoveWindow( -m_nHoriPos, -m_nVertPos, m_GrandChildSize.cx, m_GrandChildSize.cy );
	}
}


void CSCrollWnd::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if ( m_pGrandChild && m_bHCanScroll )
	{
		switch ( nSBCode )
		{
			case SB_LINELEFT:
				m_nHoriPos -= 1;
				break;
			case SB_LINERIGHT:
				m_nHoriPos += 1;
				break;
			case SB_PAGELEFT:
				m_nHoriPos -= PAGE_SIZE;
				break;
			case SB_PAGERIGHT:
				m_nHoriPos += PAGE_SIZE;
				break;
			case SB_THUMBPOSITION:
			case SB_THUMBTRACK:
				m_nHoriPos = nPos;
				break;
			default:
				break;
		}


		SCROLLINFO info;
		info.cbSize = sizeof(info);
		info.fMask  = SIF_ALL;
		GetScrollInfo( SB_HORZ, &info );

		m_nHoriPos = ( m_nHoriPos <= int(info.nMax - info.nPage) ? m_nHoriPos : int(info.nMax - info.nPage) );
		m_nHoriPos = ( m_nHoriPos >= 0 ? m_nHoriPos : 0 );

		m_pGrandChild->MoveWindow( -m_nHoriPos, -m_nVertPos, m_GrandChildSize.cx, m_GrandChildSize.cy );
		SetScrollPos( SB_HORZ, m_nHoriPos );
	}
}

void CSCrollWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if ( m_pGrandChild && m_bVCanScroll )
	{
		switch ( nSBCode )
		{
		case SB_LINEUP:
			m_nVertPos -= 1;
			break;
		case SB_LINEDOWN:
			m_nVertPos += 1;
			break;
		case SB_PAGEUP:
			m_nVertPos -= PAGE_SIZE;
			break;
		case SB_PAGEDOWN:
			m_nVertPos += PAGE_SIZE;
			break;
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			m_nVertPos = nPos;
			break;
		default:
			break;
		}


		SCROLLINFO info;
		info.cbSize = sizeof(info);
		info.fMask  = SIF_ALL;
		GetScrollInfo( SB_VERT, &info );

		m_nVertPos = ( m_nVertPos <= int(info.nMax - info.nPage) ? m_nVertPos : int(info.nMax - info.nPage) );
		m_nVertPos = ( m_nVertPos >= 0 ? m_nVertPos : 0 );

		m_pGrandChild->MoveWindow( -m_nHoriPos, -m_nVertPos, m_GrandChildSize.cx, m_GrandChildSize.cy );
		SetScrollPos( SB_VERT, m_nVertPos );
	}
}