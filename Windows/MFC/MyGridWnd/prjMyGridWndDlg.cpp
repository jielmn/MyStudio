// prjMyGridWndDlg.cpp : implementation file
//

#include "stdafx.h"
#include "prjMyGridWnd.h"
#include "prjMyGridWndDlg.h"
#include "MyGridWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMyGridWnd * p;
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrjMyGridWndDlg dialog

CPrjMyGridWndDlg::CPrjMyGridWndDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPrjMyGridWndDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPrjMyGridWndDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPrjMyGridWndDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrjMyGridWndDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPrjMyGridWndDlg, CDialog)
	//{{AFX_MSG_MAP(CPrjMyGridWndDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrjMyGridWndDlg message handlers

BOOL CPrjMyGridWndDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	p = new CMyGridWnd;
	p->Create( CRect(0, 0, 200, 200), this, 1001 );
	p->AppendItem( CMyGridWnd::TEXT, 0, _T("text1"), _T("value1") );
	p->AppendItem( CMyGridWnd::COMBOBOX, 0, _T("combo1"), _T(""), _T("s1"), _T("s2"), _T("s3"), NULL, 1 );
	p->AppendItem( CMyGridWnd::TEXT, 0, _T("text2"), _T("value2") );
	p->AppendItem( CMyGridWnd::TEXT, 0, _T("text3"), _T("value3") );

	p->ShowWindow( SW_SHOW );
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPrjMyGridWndDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPrjMyGridWndDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPrjMyGridWndDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BOOL CPrjMyGridWndDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
//	POINT pt;
//	GetCursorPos(&pt);
//	ScreenToClient( &pt );
//
//	if ( pt.x > 200 )
//	{
//		SetCursor(AfxGetApp()->LoadCursor(IDC_WAIT));
//	}
//	else
//	{
//		SetCursor(AfxGetApp()->LoadCursor(IDC_ARROW));
//	}

//	HCURSOR   hc     =LoadCursor(NULL,IDC_WAIT);
//	SetCursor(hc);
//	return TRUE;
//	
//	HCURSOR   h1 = AfxGetApp()->LoadCursor(IDC_WAIT	);
//	SetCursor(h1);
//	return TRUE;
	
	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

void CPrjMyGridWndDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	CString str;
	m_rich.GetWindowText( str );

	VARIANT v;
	v.vt   = VT_I4;
	

	m_rich.SetSelStart(1);
	m_rich.SetSelLength(1);
	v.lVal = RGB(0, 255, 0);
	m_rich.SetSelColor( v );


	m_rich.SetSelStart(2);
	m_rich.SetSelLength(1);
	v.lVal = RGB(0, 0, 255);
	m_rich.SetSelColor( v );

	m_rich.SetSelStart(3);
	m_rich.SetSelLength(1);
	v.lVal = RGB(255, 0, 0);
	m_rich.SetSelColor( v );


	
	str = m_rich.GetTextRTF();
	MessageBox( str );
}
