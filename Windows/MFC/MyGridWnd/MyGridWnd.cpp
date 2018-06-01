#include "stdafx.h"
#include "MyGridWnd.h"
#include "stdarg.h"

#pragma warning(disable:4996)

#define LINE_HEIGHT      20
#define BUTTON_WIDTH     50

#ifdef UNICODE
#define SSCANF           swscanf
#define STRLEN           wcslen
#else

#define SSCANF           scanf
#define STRLEN           strlen

#endif

BEGIN_MESSAGE_MAP(CEditNotify, CEdit)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()


void CEditNotify::OnRButtonDown( UINT nFlags, CPoint point )
{
	CMyGridWnd * pParent = (CMyGridWnd *)GetParent();
	::SendMessage( pParent->m_hNotifyWnd, UM_CEDITNOTIFY_RD, pParent->m_nSelIndex, 0 );
}

void CEditNotify::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CMyGridWnd * pParent = (CMyGridWnd *)GetParent();
	::SendMessage( pParent->m_hNotifyWnd, UM_CEDITNOTIFY_LDC, pParent->m_nSelIndex, 0 );
}

BEGIN_MESSAGE_MAP(CComboBoxNotify, CComboBox)
	
END_MESSAGE_MAP()


CMyGridWnd::CMyGridWnd()
{
	ASSERT( NAME_MAX > 0 && SIN_TEXT_MAX > 0 && MULTEXT_MAX > 0 );

	m_nSplitPos = 100;
	m_nSelIndex = -1;
	m_nPreSelIndex = -1;
	m_bDrag     = FALSE;

	m_font.CreateStockObject( DEFAULT_GUI_FONT );
	m_grayPen.CreatePen( PS_SOLID, 1, RGB(236, 233, 216) );
	m_grayBrush.CreateSolidBrush( RGB(236, 233, 216) );
	m_whitePen.CreatePen( PS_SOLID, 1, RGB(255, 0, 0) );
	m_blueBrush.CreateSolidBrush( RGB(0, 0, 255) );

	m_curSplit = LoadCursor( NULL, IDC_SIZEWE );
	m_curArrow = LoadCursor( NULL, IDC_ARROW );

	LOGFONT lf;
	m_font.GetLogFont(&lf);
	lf.lfWeight = 800;
	m_boldFont.CreateFontIndirect(&lf);

	m_hNotifyWnd = NULL;
}

CMyGridWnd::~CMyGridWnd()
{
	m_font.DeleteObject();
	m_grayPen.DeleteObject();
	m_whitePen.DeleteObject();
	m_blueBrush.DeleteObject();
	m_grayBrush.DeleteObject();
	m_boldFont.DeleteObject();

	DestroyCursor( m_curArrow );
	DestroyCursor( m_curSplit );
}

BOOL CMyGridWnd::Create( const RECT& rect, CWnd * pParent, UINT nID )
{
	if ( NULL == pParent )
	{
		return FALSE;
	}

	BOOL bRet = CWnd::Create(NULL, NULL, WS_CHILD, rect, pParent, nID );
	if ( !bRet )
	{
		return FALSE;
	}

	bRet = m_edit.Create( ES_LEFT | ES_AUTOHSCROLL, CRect(0, 0, 0, 0), this, 1001 );
	if ( !bRet )
	{
		return FALSE;
	}
	m_edit.SetFont( &m_font, FALSE );

	if ( !m_combo.Create( CBS_DROPDOWNLIST, CRect(0, 0, 0, 0), this, 1002 ) )
	{
		return FALSE;
	}
	m_combo.SetFont( &m_font, FALSE );

	bRet = m_mulEdit.Create( ES_AUTOVSCROLL | ES_MULTILINE , CRect(0, 0, 0, 0), this, 1003 );
	if ( !bRet )
	{
		return FALSE;
	}
	m_mulEdit.ShowScrollBar( SB_VERT, TRUE );
	m_mulEdit.SetFont( &m_font, FALSE );

	bRet = m_readonly.Create(ES_READONLY , CRect(0, 0, 0, 0), this, 1004);
	if ( !bRet )
	{
		return FALSE;
	}
	m_readonly.SetFont( &m_font, FALSE );

	bRet = m_number.Create(ES_NUMBER , CRect(0, 0, 0, 0), this, 1005);
	if ( !bRet )
	{
		return FALSE;
	}
	m_number.SetFont( &m_font, FALSE );

	bRet = m_button.Create( _T("..."), BS_DEFPUSHBUTTON, CRect(0, 0, 0, 0), this, 1006 );
	if ( !bRet )
	{
		return FALSE;
	}
	m_button.SetFont( &m_font, FALSE );

	//ShowWindow( SW_SHOW );
	return TRUE;
}

BEGIN_MESSAGE_MAP(CMyGridWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_DESTROY()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(1006, OnBtnClicked )
END_MESSAGE_MAP()

VOID CMyGridWnd::OnDestroy()
{
	ASSERT( ::IsWindow( m_hWnd) );
	CWnd::OnDestroy();

	vector<PGridItem>::iterator i;
	for ( i = m_Items.begin(); i != m_Items.end(); i++ )
	{
		PGridItem pItem = *i;

		vector<LPTSTR>::iterator j;
		for ( j = pItem->vectorCombo.begin(); j != pItem->vectorCombo.end(); j++ )
		{
			delete[] *j;
		}
		pItem->vectorCombo.clear();
	
		if ( NULL != pItem->pMulText )
		{
			delete[] pItem->pMulText;
		}

		delete pItem;
	}
	m_Items.clear();

	m_edit.DestroyWindow();
	m_combo.DestroyWindow();
	m_mulEdit.DestroyWindow();
	m_readonly.DestroyWindow();
	m_number.DestroyWindow();
	m_button.DestroyWindow();
}

DWORD CMyGridWnd::AppendItem( UINT nItemType, UINT nLineNum, LPCTSTR szName, LPCTSTR szValue, ... )
{
	va_list ap;
	LPCTSTR str;
	LPTSTR  strTmp;
	size_t  nLen;
	int     iValue;

	PGridItem pItem = new GridItem;

	pItem->nType    = nItemType;

	if ( NAME_MAX > 0 )
	{
		pItem->szName[0] = _T('\0');
	}

	if ( NULL != szName )
	{
		StrCpyN(pItem->szName, szName, NAME_MAX);
	}

	pItem->pMulText = NULL;

	switch( nItemType )
	{
		case FILE_BUTTON:
			pItem->nLineNum = 1;
			if ( SIN_TEXT_MAX > 0 )
			{
				pItem->szValue[0] = _T('\0');
			}

			if ( NULL != szValue )
			{
				StrCpyN(pItem->szValue, szValue, SIN_TEXT_MAX);
			}
			
			va_start( ap, szValue );
			str = va_arg(ap, LPCTSTR);
			if ( NULL == str )
			{
				delete pItem;
				va_end(ap);
				return 1;
			}
			StrCpyN( pItem->szFileFilter, str, 32 );

			str = va_arg(ap, LPCTSTR);
			if ( NULL == str )
			{
				delete pItem;
				va_end(ap);
				return 1;
			}
			StrCpyN( pItem->szFileExt, str, 16 );

			va_end(ap);

			break;

		case READONLY:
		case TEXT:
			pItem->nLineNum = 1;
			if ( SIN_TEXT_MAX > 0 )
			{
				pItem->szValue[0] = _T('\0');
			}

			if ( NULL != szValue )
			{
				StrCpyN(pItem->szValue, szValue, SIN_TEXT_MAX);
			}
			break;

		case NUMBER:
			pItem->nLineNum = 1;
			if ( SIN_TEXT_MAX > 0 )
			{
				pItem->szValue[0] = _T('\0');
			}

			if ( NULL != szValue )
			{
				iValue = 0;
				SSCANF(szValue, _T(" %d"), &iValue );

				CString s;
				s.Format(_T("%d"), iValue );

				StrCpyN(pItem->szValue, s, SIN_TEXT_MAX);
			}
			break;

		case COMBOBOX:
			pItem->nLineNum = 1;

			va_start( ap, szValue );
			str = va_arg(ap, LPCTSTR);
			if ( NULL == str )
			{
				delete pItem;
				va_end(ap);
				return 1;
			}

			while( NULL != str )
			{
				nLen = _tcslen(str);
				strTmp = new TCHAR[nLen + 1];
				StrCpy(strTmp, str);
				pItem->vectorCombo.push_back( strTmp );

				str = va_arg(ap, LPCTSTR);	
			}
			pItem->nSelIndex = va_arg(ap, int);
			va_end(ap);
			
			if ( pItem->nSelIndex >= (int)pItem->vectorCombo.size() )
			{
				pItem->nSelIndex = 0;
			}
			StrCpyN( pItem->szValue, pItem->vectorCombo[pItem->nSelIndex], SIN_TEXT_MAX);

			break;

		case MULTEXT:
			if ( 0 == nLineNum )
			{
				delete pItem;
				return FALSE;
			}
			
			pItem->nLineNum = nLineNum;
			pItem->pMulText = new TCHAR[MULTEXT_MAX];
			if ( MULTEXT_MAX > 0)
			{
				pItem->pMulText[0] = _T('\0');
			}
			StrCpyN( pItem->pMulText, szValue, MULTEXT_MAX );
			break;

		case TITLE:
			pItem->nLineNum = 1;
			break;

		default:
			break;
	}

	m_Items.push_back( pItem );

	return 0;
}

DWORD CMyGridWnd::GetValue( UINT nIndex, UINT *  pnValue )
{
	ASSERT( NULL != pnValue );
	if ( NULL == pnValue )
	{
		return 1;
	}

	TCHAR  szBuff[256];
	DWORD dwRet = GetValue( nIndex, szBuff, 256, NULL, NULL );
	if ( 0 != dwRet )
	{
		return dwRet;
	}

	SSCANF( szBuff, _T(" %d"), pnValue );

	return 0;
}

DWORD CMyGridWnd::GetValue( UINT nIndex, LPTSTR  szValue, UINT nSize, UINT * pSelIndex, UINT * pType )
{
	if ( nIndex + 1 > m_Items.size() )
	{
		return 1;
	}

	SaveValue();

	PGridItem pItem = m_Items[nIndex];
	if ( TEXT == pItem->nType || READONLY == pItem->nType || NUMBER == pItem->nType || FILE_BUTTON == pItem->nType )
	{
		if ( NULL != szValue && nSize > 0 )
		{
			StrCpyN( szValue, pItem->szValue, nSize );
		}
	}
	else if ( MULTEXT == pItem->nType )
	{
		if ( NULL != szValue && nSize > 0 )
		{
			StrCpyN( szValue, pItem->pMulText, nSize );
		}
	}
	else if ( COMBOBOX == pItem->nType )
	{
		if ( NULL != szValue && nSize > 0 )
		{
			StrCpyN( szValue, pItem->szValue, nSize );
		}

		if ( NULL != pSelIndex )
		{
			*pSelIndex = pItem->nSelIndex;
		}
	}

	if ( NULL != pType )
	{
		*pType = pItem->nType;
	}

	return 0;
}

DWORD CMyGridWnd::InsertStrValue( LPCTSTR szValue )
{
	if ( -1 == m_nSelIndex )
	{
		return 1;
	}

	PGridItem pItem = m_Items[m_nSelIndex];

	int nStart;
	int nEnd;
	int nLen;

	if ( TEXT == pItem->nType  )
	{
		if ( NULL == szValue )
		{
			return 0;
		}

		m_edit.GetSel( nStart, nEnd );

		CString strLeft;
		CString strRight;
		CString str;
		m_edit.GetWindowText(str);
		
		nLen     = str.GetLength();
		strLeft  = str.Left( nEnd );
		strRight = str.Right( nLen - nEnd );

		strLeft += szValue;
		strLeft += strRight;

		int nTmp = STRLEN( szValue );

		m_edit.SetWindowText( strLeft );
		m_edit.SetSel( nEnd + nTmp, nEnd + nTmp );
		
		StrCpyN( pItem->szValue, strLeft, SIN_TEXT_MAX );
	}
	else if ( MULTEXT == pItem->nType )
	{
		if ( NULL == szValue )
		{
			return 0;
		}

		m_mulEdit.GetSel( nStart, nEnd );

		CString strLeft;
		CString strRight;
		CString str;
		m_mulEdit.GetWindowText(str);

		nLen     = str.GetLength();
		strLeft  = str.Left( nEnd );
		strRight = str.Right( nLen - nEnd );

		strLeft += szValue;
		strLeft += strRight;

		int nTmp = STRLEN( szValue );

		m_mulEdit.SetWindowText( strLeft );
		m_mulEdit.SetSel( nEnd + nTmp, nEnd + nTmp );

		StrCpyN( pItem->pMulText, strLeft, MULTEXT_MAX );
	}
	
	return 0;
}

DWORD CMyGridWnd::SetValue( UINT nIndex, UINT    nValue )
{
	CString str;
	str.Format( _T("%d"), nValue );
	return SetValue( nIndex, str, 0 );
}

DWORD CMyGridWnd::SetValue( UINT nIndex, LPCTSTR szValue, UINT nSelIndex )
{
	if ( nIndex + 1 > m_Items.size() )
	{
		return 1;
	}

	PGridItem pItem = m_Items[nIndex];
	if ( TEXT == pItem->nType || READONLY == pItem->nType || NUMBER == pItem->nType || FILE_BUTTON == pItem->nType )
	{
		if ( NULL != szValue )
		{
			StrCpyN( pItem->szValue, szValue, SIN_TEXT_MAX );
		}
	}
	else if ( MULTEXT == pItem->nType )
	{
		if ( NULL != szValue )
		{
			StrCpyN( pItem->pMulText, szValue, MULTEXT_MAX );
		}
	}
	else if ( COMBOBOX == pItem->nType )
	{
		if ( nSelIndex < pItem->vectorCombo.size() )
		{
			pItem->nSelIndex = nSelIndex;

			StrCpyN( pItem->szValue, pItem->vectorCombo[pItem->nSelIndex], SIN_TEXT_MAX );
		}
	}

	Invalidate();
	ActivateItem();

	return 0;
}

void CMyGridWnd::DrawItem( PGridItem pItem, int nTop, int nWidth, CDC * pDC, int nIndex )
{
	HGDIOBJ hOldBrush = NULL;

	if ( m_nSelIndex == nIndex )
	{
		pDC->SetBkColor( RGB(0,0,255) );
		pDC->SetTextColor( RGB(255, 255,255) );
	}
	else
	{
		pDC->SetBkColor(RGB(255, 255, 255));
		pDC->SetTextColor( RGB(0, 0, 0) );
	}

	HGDIOBJ hOldPen   = pDC->SelectObject( m_grayPen );
	/////////// draw border
	CRect rectBorder(0, nTop, nWidth, nTop + 2 + pItem->nLineNum * LINE_HEIGHT );
	pDC->Rectangle( rectBorder );
	if ( TITLE != pItem->nType )
	{
		pDC->MoveTo( m_nSplitPos, rectBorder.top );
		pDC->LineTo( m_nSplitPos, rectBorder.bottom );
	}
	pDC->SelectObject( hOldPen );

	if ( TITLE == pItem->nType )
	{
		CRect rtTitle(rectBorder.left + 1,
					rectBorder.top + 1, 
					nWidth - 1, 
					rectBorder.bottom - 1);
		pDC->FillRect( rtTitle, &m_grayBrush );

		pDC->SetBkColor(RGB(236, 233, 216));
		pDC->SetTextColor( RGB(113, 111, 100) );
		rtTitle.DeflateRect( 2, 0, 0, 2 );

		HGDIOBJ hOldFont = pDC->SelectObject( m_boldFont );
		pDC->DrawText( pItem->szName, rtTitle, DT_LEFT | DT_VCENTER | DT_SINGLELINE  );
		pDC->SelectObject(hOldFont);

		return;
	}
	
	
	CRect rectName( rectBorder.left + 1,
		            rectBorder.top + 1, 
					m_nSplitPos, 
					rectBorder.bottom - 1 );

	if ( m_nSelIndex == nIndex )
	{
		pDC->FillRect( rectName, &m_blueBrush );
	}

	////////////// draw name	
	if ( 1 == pItem->nLineNum )
	{
		rectName.DeflateRect( 2, 0, 0, 2 );
		pDC->DrawText( pItem->szName, rectName, DT_LEFT | DT_VCENTER | DT_SINGLELINE  );
	}
	else
	{
		rectName.DeflateRect( 2, 2, 2, 2 );
		pDC->DrawText( pItem->szName, rectName, DT_LEFT | DT_VCENTER |DT_SINGLELINE  );
	}
	
	CRect rectValue( m_nSplitPos + 1,
		             rectBorder.top + 1, 
					 rectBorder.right - 1, 
					 rectBorder.bottom - 1  );

	pDC->SetBkColor(RGB(255, 255, 255));
	pDC->SetTextColor( RGB(0, 0, 0) );

	switch( pItem->nType )
	{
		case FILE_BUTTON:
			if ( m_nSelIndex == nIndex )
			{
				//rectValue.DeflateRect( 2, 0, 0, 2 );
				//pDC->DrawText( pItem->szValue, rectValue, DT_LEFT | DT_VCENTER | DT_SINGLELINE  );
			}
			else
			{
				///////////////////////// draw value
				rectValue.DeflateRect( 2, 0, 0, 2 );
				pDC->DrawText( pItem->szValue, rectValue, DT_LEFT | DT_VCENTER | DT_SINGLELINE  );
			}
			break;

		case NUMBER:
		case TEXT:
			if ( m_nSelIndex == nIndex )
			{
				//ActivateItem( pItem, rectValue );
			}
			else
			{
				///////////////////////// draw value
				rectValue.DeflateRect( 2, 0, 0, 2 );
				pDC->DrawText( pItem->szValue, rectValue, DT_LEFT | DT_VCENTER | DT_SINGLELINE  );
			}
			break;

		case READONLY:
			if ( m_nSelIndex == nIndex )
			{
				//ActivateItem( pItem, rectValue );
			}
			else
			{
				///////////////////////// draw value
				rectValue.DeflateRect( 2, 0, 0, 2 );
				pDC->SetTextColor( RGB(200, 200, 200) );
				pDC->DrawText( pItem->szValue, rectValue, DT_LEFT | DT_VCENTER | DT_SINGLELINE  );
			}
			break;

		case COMBOBOX:
			if ( m_nSelIndex == nIndex )
			{
				//ActivateItem( pItem, rectValue );
			}
			else
			{
				///////////////////////// draw value
				rectValue.DeflateRect( 2, 0, 0, 2 );
				pDC->DrawText( pItem->szValue, rectValue, DT_LEFT | DT_VCENTER | DT_SINGLELINE  );
			}

		case MULTEXT:
			if ( m_nSelIndex == nIndex )
			{
				//ActivateItem( pItem, rectValue );
			}
			else
			{
				///////////////////////// draw value
				rectValue.DeflateRect( 2, 0, 0, 2 );
				pDC->DrawText( pItem->pMulText, rectValue, DT_WORDBREAK  );
			}
			break;

		default:
			break;
	}
}

void CMyGridWnd::OnPaint()
{	
	CPaintDC dc(this);
	HGDIOBJ hOldFont = dc.SelectObject( m_font );
	
	CRect rectAll;
	GetWindowRect( rectAll );

	int nTop = 0;
	vector<PGridItem>::iterator i;
	int j = 0;
	for ( i = m_Items.begin(), j = 0; i != m_Items.end(); i++, j++ )
	{
		DrawItem( *i, nTop, rectAll.Width(), &dc, j );
		nTop += (*i)->nLineNum * LINE_HEIGHT + 1;
	}

	dc.SelectObject( hOldFont );
}

BOOL CMyGridWnd::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;
	GetClientRect( rect );
	pDC->FillRect( rect, &CBrush(RGB(255, 255, 255)) );

	return TRUE;
}

void CMyGridWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	if ( m_nSplitPos - 2 <= point.x && point.x <= m_nSplitPos + 3 )
	{
		m_bDrag = TRUE;
		SetCursor(m_curSplit);
		SetCapture();
		return;
	}

	int nBottom = 0;
	vector<PGridItem>::iterator i;
	int iIndex = 0;

	for ( i = m_Items.begin(), iIndex = 0; i != m_Items.end(); i++, iIndex++ )
	{
		nBottom += (*i)->nLineNum * LINE_HEIGHT + 1;
		if ( nBottom > point.y )
		{
			break;
		}
	}

	if ( i != m_Items.end() )
	{
		//////// if reactivate self
		if ( m_nSelIndex == iIndex )
		{
			return;
		}

		//////// deactiave previcious control and save data
		if ( -1 != m_nSelIndex )
		{
			SaveValue();
			Deactivate();
		}

		m_nSelIndex = iIndex;

		ActivateItem();
		
		Invalidate(FALSE);

		// save previous selected index
		m_nPreSelIndex = m_nSelIndex;
	}
	else
	{
		// click not in the item list
		if ( -1 != m_nSelIndex )
		{
			SaveValue();
			Deactivate();

			m_nPreSelIndex = m_nSelIndex;
			m_nSelIndex = -1;
			Invalidate(FALSE);
		}
	}
}


void CMyGridWnd::ActivateItem()
{
	if ( -1 == m_nSelIndex )
	{
		return;
	}

	vector<PGridItem>::iterator iItem;
	int iIndex = 0;
	int nTop   = 0;

	for ( iItem = m_Items.begin(), iIndex = 0; iItem != m_Items.end(); iItem++, iIndex++ )
	{
		if ( iIndex == m_nSelIndex )
		{
			break;
		}
		nTop += (*iItem)->nLineNum * LINE_HEIGHT + 1;
	}

	ASSERT( iItem != m_Items.end() );

	PGridItem pItem = *iItem;

	CRect rectClient;
	GetClientRect(rectClient);

	CRect rect( m_nSplitPos + 1, nTop, rectClient.Width() - 1, nTop + pItem->nLineNum * LINE_HEIGHT + 1 );

	size_t nLen;
	vector<LPTSTR>::iterator i;
	int j;

	CRect rectBtn;

	switch( pItem->nType )
	{
		case FILE_BUTTON:
			rect.DeflateRect( 2, 3, 1 + BUTTON_WIDTH + 1, 2 );

			rectBtn.top = rect.top - 3;
			rectBtn.bottom = rect.bottom + 2;
			rectBtn.left = rect.right + 1;
			rectBtn.right = rectBtn.left + BUTTON_WIDTH;
			

			m_button.MoveWindow( rectBtn );
			m_button.ShowWindow( SW_SHOW );

			m_edit.SetWindowText( pItem->szValue );
			nLen = _tcslen( pItem->szValue );
			m_edit.SetSel( 0, nLen, TRUE );
			m_edit.MoveWindow( rect );
			m_edit.ShowWindow( SW_SHOW );

			break;

		case READONLY:
			m_readonly.SetWindowText( pItem->szValue );
			rect.DeflateRect(2, 3, 1, 2 );
			m_readonly.MoveWindow( rect );
			m_readonly.ShowWindow( SW_SHOW );
			//m_readonly.SetFocus();
			break;

		case NUMBER:
			m_number.SetWindowText( pItem->szValue );
			//nLen = _tcslen( pItem->szValue );
			//m_number.SetSel( 0, nLen, TRUE );
			rect.DeflateRect(2, 3, 1, 2 );
			m_number.MoveWindow( rect );
			m_number.ShowWindow( SW_SHOW );
			//m_number.SetFocus();

			break;

		case TEXT:
			m_edit.SetWindowText( pItem->szValue );
			//nLen = _tcslen( pItem->szValue );
			//m_edit.SetSel( 0, nLen, TRUE );
			rect.DeflateRect(2, 3, 1, 2 );
			m_edit.MoveWindow( rect );
			m_edit.ShowWindow( SW_SHOW );
			//m_edit.SetFocus();
			break;

		case COMBOBOX:
			while( m_combo.GetCount() > 0 )
			{
				m_combo.DeleteString(0);
			}
			for ( i = pItem->vectorCombo.begin(), j = 0; i != pItem->vectorCombo.end(); i++, j++ )
			{
				m_combo.InsertString(j, *i);
			}
			m_combo.SetCurSel( pItem->nSelIndex );
			rect.DeflateRect( 1, 1, 1, 1 );
			m_combo.MoveWindow( rect );
			m_combo.ShowWindow( SW_SHOW );
			//m_combo.SetFocus();
			break;

		case MULTEXT:
			m_mulEdit.SetWindowText( pItem->pMulText );
			//nLen = _tcslen( pItem->pMulText );
			//m_mulEdit.SetSel( 0, nLen, TRUE );
			rect.DeflateRect(2, 3, 1, 2);
			m_mulEdit.MoveWindow( rect );
			m_mulEdit.ShowWindow( SW_SHOW );
			//m_mulEdit.SetFocus();
			break;

		default:
			break;
	}
}

void CMyGridWnd::Deactivate( )
{
	if ( -1 == m_nSelIndex )
	{
		return;
	}

	PGridItem pItem = m_Items[m_nSelIndex];

	switch( pItem->nType )
	{
		case FILE_BUTTON:
			m_edit.ShowWindow( SW_HIDE );
			m_button.ShowWindow( SW_HIDE );
			break;

		case NUMBER:
			m_number.ShowWindow( SW_HIDE );
			break;

		case READONLY:
			m_readonly.ShowWindow( SW_HIDE );
			break;

		case TEXT:
			m_edit.ShowWindow( SW_HIDE );
			break;

		case COMBOBOX:
			m_combo.ShowDropDown( FALSE );
			m_combo.ShowWindow( SW_HIDE );
			while( m_combo.GetCount() > 0 )
			{
				m_combo.DeleteString(0);
			}
			break;

		case MULTEXT:
			m_mulEdit.ShowWindow( SW_HIDE );
			break;

		default:
			break;
	}
}

void  CMyGridWnd::SaveValue()
{
	if ( -1 == m_nSelIndex )
	{
		return;
	}

	PGridItem pItem = m_Items[m_nSelIndex];

	CString strValue;

	switch( pItem->nType )
	{
		case READONLY:
			break;

		case NUMBER:
			m_number.GetWindowText( strValue );

			if ( 0 != StrCmp( strValue, pItem->szValue ) )
			{
				::SendMessage( m_hNotifyWnd, UM_CONTENT_CHANGED, 0, 0 );
			}

			StrCpyN( pItem->szValue, strValue, SIN_TEXT_MAX);
			break;

		case FILE_BUTTON:
		case TEXT:
			m_edit.GetWindowText( strValue );

			if ( 0 != StrCmp( strValue, pItem->szValue ) )
			{
				::SendMessage( m_hNotifyWnd, UM_CONTENT_CHANGED, 0, 0 );
			}

			StrCpyN( pItem->szValue, strValue, SIN_TEXT_MAX);
			break;

		case COMBOBOX:
			if ( pItem->nSelIndex != m_combo.GetCurSel() )
			{
				::SendMessage( m_hNotifyWnd, UM_CONTENT_CHANGED, 0, 0 );
			}

			pItem->nSelIndex = m_combo.GetCurSel();

			StrCpyN( pItem->szValue, pItem->vectorCombo[pItem->nSelIndex], SIN_TEXT_MAX);
			break;

		case MULTEXT:
			m_mulEdit.GetWindowText( strValue );

			if ( 0 != StrCmp( strValue, pItem->pMulText ) )
			{
				::SendMessage( m_hNotifyWnd, UM_CONTENT_CHANGED, 0, 0 );
			}

			StrCpyN( pItem->pMulText, strValue, MULTEXT_MAX);
			break;

		default:
			break;
	}
}

VOID CMyGridWnd::UpdateUI()
{
	if ( -1 == m_nSelIndex )
	{
		return;
	}

	vector<PGridItem>::iterator i;
	int iIndex = 0;
	int nTop   = 0;

	for ( i = m_Items.begin(), iIndex = 0; i != m_Items.end(); i++, iIndex++ )
	{
		if ( iIndex == m_nSelIndex )
		{
			break;
		}
		nTop += (*i)->nLineNum * LINE_HEIGHT + 1;
	}

	CRect rectClient;
	GetClientRect( rectClient );

	CRect rectValue( m_nSplitPos + 1, nTop + 1, rectClient.Width() - 1, nTop + (*i)->nLineNum * LINE_HEIGHT + 1 );
	
	PGridItem pItem = *i;
	CRect     rect = rectValue;

	CRect     rectBtn;
	switch( pItem->nType )
	{
		case FILE_BUTTON:
			rect.DeflateRect( 2, 3, 1 + BUTTON_WIDTH + 1, 2 );

			rectBtn.top = rect.top - 3;
			rectBtn.bottom = rect.bottom + 2;
			rectBtn.left = rect.right + 1;
			rectBtn.right = rectBtn.left + BUTTON_WIDTH;

			m_button.MoveWindow( rectBtn );
			m_edit.MoveWindow( rect );
			break;

		case READONLY:
			rect.DeflateRect(2, 3, 1, 2 );
			m_readonly.MoveWindow( rect );
			break;

		case NUMBER:
			rect.DeflateRect(2, 3, 1, 2 );
			m_number.MoveWindow( rect );
			break;

		case TEXT:
			rect.DeflateRect(2, 3, 1, 2 );
			m_edit.MoveWindow( rect );
			break;

		case COMBOBOX:
			rect.DeflateRect( 1, 1, 1, 1 );
			m_combo.MoveWindow( rect );
			break;

		case MULTEXT:
			rect.DeflateRect(2, 3, 1, 2);
			m_mulEdit.MoveWindow( rect );
			break;

		default:
			break;
	}
}

void CMyGridWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if ( m_bDrag )
	{
		if ( point.x <= 0 )
		{
			return;
		}
		
		CRect rect;
		GetClientRect( rect );

		if ( rect.Width() <= 0  )
		{
			return;
		}

		if ( point.x >= rect.Width() - 1 )
		{
			return;
		}

		m_nSplitPos = point.x;
		RedrawWindow();
		UpdateUI();

		SetCursor(m_curSplit);
	}
	else
	{
		if ( m_nSplitPos - 2 <= point.x && point.x <= m_nSplitPos + 3 )
		{
			SetCursor(m_curSplit);
		}
		else
		{
			SetCursor( m_curArrow );
		}		
	}
}

void CMyGridWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if ( m_bDrag )
	{
		ReleaseCapture();
		m_bDrag = FALSE;
	}
}

DWORD CMyGridWnd::GetCount()
{
	return m_Items.size();
}

int CMyGridWnd::GetSelIndex()
{
	return m_nSelIndex;
}

void CMyGridWnd::OnBtnClicked()
{
	ASSERT( -1 != m_nSelIndex );

	PGridItem pItem = m_Items[m_nSelIndex];

	CFileDialog opendialog(TRUE, pItem->szFileExt, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, pItem->szFileFilter, ::AfxGetMainWnd() );

	if ( IDOK != opendialog.DoModal() )
	{
		return;
	}

	CString strFileName = opendialog.GetFileName();
	
	StrCpyN( pItem->szValue, strFileName, SIN_TEXT_MAX );
	m_edit.SetWindowText( strFileName );

	Invalidate();
}