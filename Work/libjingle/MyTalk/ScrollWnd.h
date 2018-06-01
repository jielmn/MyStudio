#ifndef __SCROLL_WINDOW__

class CSCrollWnd : public CWnd
{
public:
	CSCrollWnd( );
	virtual ~CSCrollWnd( );

	BOOL Create( UINT nID, CWnd* pParentWnd = NULL, BOOL bAlwaysShow = FALSE );
	BOOL AddChild( CWnd * pChild );
	BOOL Resize( const CSize & size );
	BOOL Resize( int cx, int cy );
	SIZE GetShowSize();
	BOOL ChangeChildSize( const CSize & size );
	BOOL ChangeChildSize( int cx, int cy );
	VOID UpdateScrollBar();

protected:
	CStatic    m_childWnd;
	CWnd    *  m_pGrandChild;
	CSize      m_GrandChildSize;
	int        m_nHoriPos;
	int        m_nVertPos;
	BOOL       m_bAlwaysShow;
	BOOL       m_bHCanScroll;
	BOOL       m_bVCanScroll;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	DECLARE_MESSAGE_MAP()
};

#endif