#ifndef __MYGRID_WINDOW__
#define __MYGRID_WINDOW__
#include <vector>
using namespace std;

#define MULTEXT_MAX      4096
#define SIN_TEXT_MAX     512
#define NAME_MAX         32

#define UM_CEDITNOTIFY_RD             ( WM_USER + 1010 )
#define UM_CEDITNOTIFY_LDC            ( WM_USER + 1011 )
#define UM_CONTENT_CHANGED            ( WM_USER + 1012 )

class CEditNotify : public CEdit
{
public:
	

protected:
	afx_msg void OnRButtonDown( UINT nFlags, CPoint point );
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
};

class CComboBoxNotify : public CComboBox
{

protected:
	DECLARE_MESSAGE_MAP()
};

class CMyGridWnd : public CWnd
{
public:
	CMyGridWnd();
	virtual ~CMyGridWnd();

	virtual BOOL Create( const RECT& rect, CWnd * pParent, UINT nID );
	enum ItemType
	{
		TEXT,
		MULTEXT,
		READONLY,
		NUMBER,
		COMBOBOX,
		TITLE,
		FILE_BUTTON
	};

	//添加单行文本   AppendItem( TEXT,    0,  "A", "T1" );
	//添加多行文本   AppendItem( MULTEXT, 10, "B", "T2\nwww" );
	//添加combo      AppendItem( COMBOBOX,0,  "C", "", "V1", "V2", "V3", NULL, 1);

	DWORD AppendItem( UINT nItemType, UINT nLineNum, LPCTSTR szName, LPCTSTR szValue, ... );
	DWORD GetValue  ( UINT nIndex, LPTSTR  szValue, UINT nSize, UINT * pSelIndex, UINT * pType = NULL );
	DWORD GetValue  ( UINT nIndex, UINT *  pnValue );
	DWORD SetValue  ( UINT nIndex, LPCTSTR szValue, UINT nSelIndex );
	DWORD SetValue  ( UINT nIndex, UINT    nValue  );
	DWORD InsertStrValue( LPCTSTR szValue );

	DWORD GetCount  ();
	int   GetSelIndex();
	VOID  UpdateUI   ();

	HWND  m_hNotifyWnd;

protected:
	typedef struct
	{
		UINT            nType;
		UINT            nLineNum;
		TCHAR           szName [NAME_MAX];
		TCHAR           szValue[SIN_TEXT_MAX];
		vector<LPTSTR>  vectorCombo;
		INT             nSelIndex;
		TCHAR *         pMulText;
		CRichEditCtrl * pRich;
		TCHAR           szFileFilter[128];
		TCHAR           szFileExt[64];
	}GridItem, *PGridItem;

	vector<PGridItem>  m_Items;

	void  DrawItem( PGridItem pItem, int nTop, int nWidth, CDC * pDC, int nIndex );
	void  ActivateItem();
public:
	void  Deactivate();
	void  SaveValue();

public:
	int    m_nSelIndex;
	int    m_nPreSelIndex;

protected:
	int    m_nSplitPos;
	CFont  m_font;
	CFont  m_boldFont;
	CPen   m_grayPen;
	CPen   m_whitePen;
	CBrush m_blueBrush;
	CBrush m_grayBrush;

	CEdit     m_edit;
	CComboBoxNotify m_combo;
	CEdit     m_mulEdit;
	CEdit     m_readonly;
	CEdit     m_number;
	CButton         m_button;

	BOOL      m_bDrag;
	HCURSOR   m_curSplit;
	HCURSOR   m_curArrow;

	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBtnClicked();

	DECLARE_MESSAGE_MAP()
};

#endif