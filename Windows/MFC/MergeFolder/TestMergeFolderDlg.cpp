// TestMergeFolderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestMergeFolder.h"
#include "TestMergeFolderDlg.h"
#include "zlib.h"

static CString s_filePath;

static BOOL    s_bThrdFlag;
static HANDLE  s_h1;
static HANDLE  s_h2;

#define TEST_FOLDER  "D:\\Test"

#define UM_COMPLETE   ( WM_USER + 1 )
#define UM_PROGRESS   ( WM_USER + 2 )

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef UNICODE

#define STRLEN(s)                  wcslen(s)
#define STRCPY(d, s)               wcscpy(d, s)
#define STRCHR(s, c)               wcschr(s, c)
#define STRCMP(a,b)                wcscmp(a, b)
#define STRNCPY(a, b, n)           wcsncpy(a, b, n)
#define STRCAT(a, b)               wcscat(a, b)
#define STRICMP(a, b)              _wcsicmp(a, b)
#define SPRINTF                    wsprintf
#define SSCANF                     swscanf
#define SNPRINTF                   _snwprintf

#else

#define STRLEN(s)                  strlen(s)
#define STRCPY(d, s)               strcpy(d, s)
#define STRCHR(s, c)               strchr(s, c)
#define STRCMP(a,b)                strcmp(a, b)
#define STRNCPY(a, b, n)           strncpy(a, b, n)
#define STRCAT(a, b)               strcat(a, b)
#define STRICMP(a, b)              _stricmp(a, b)
#define SPRINTF                    sprintf
#define SSCANF                     sscanf
#define SNPRINTF                   _snprintf

#endif

LPCTSTR GetFileSubfix( LPCTSTR szFilePath )
{
	int n = STRLEN( szFilePath );
	int i;
	for ( i = n - 1; i >=0; i-- )
	{
		if ( _T('.') == szFilePath[i] )
		{
			break;
		}
	}

	if ( i < 0 )
	{
		return _T("");
	}
	else
	{
		return szFilePath + i + 1;
	}
};


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
// CTestMergeFolderDlg dialog

CTestMergeFolderDlg::CTestMergeFolderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestMergeFolderDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTestMergeFolderDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pMfe = NULL;
}

void CTestMergeFolderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestMergeFolderDlg)
	DDX_Control(pDX, IDC_OPEN, m_btnOpen);
	DDX_Control(pDX, IDC_MERGE, m_btnMerge);
	// DDX_Control(pDX, IDC_STATIC1, m_progCtrl);
	DDX_Control(pDX, IDC_FOLDER, m_lst);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTestMergeFolderDlg, CDialog)
	//{{AFX_MSG_MAP(CTestMergeFolderDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_MERGE, OnMerge)
	ON_BN_CLICKED(IDC_OPEN, OnOpen)
	ON_NOTIFY(NM_DBLCLK, IDC_FOLDER, OnDblclkFolder)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP

    ON_MESSAGE( UM_COMPLETE, OnProgComplete )
    ON_MESSAGE( UM_PROGRESS, OnProgNotify )    
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestMergeFolderDlg message handlers

BOOL CTestMergeFolderDlg::OnInitDialog()
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
	m_lst.InsertColumn( 0, _T("name"), LVCFMT_LEFT, 150 );
	m_lst.InsertColumn( 1, _T("type"), LVCFMT_LEFT, 250 );

	m_lst.SetExtendedStyle( LVS_EX_FULLROWSELECT );

    // m_progCtrl.ShowWindow( SW_HIDE );

    GetDlgItem(IDC_PROGRESS1)->ShowWindow( SW_HIDE );

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestMergeFolderDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTestMergeFolderDlg::OnPaint() 
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
HCURSOR CTestMergeFolderDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

unsigned int test_compress( const BYTE * pIn, unsigned int nInSize, BYTE * pOut, unsigned int nOutSize )
{
    unsigned long n = nOutSize;

    if ( Z_OK == compress( pOut, &n, pIn, nInSize ) )
    {
        return n;
    }
    else
    {
        return 0;
    }
}

unsigned int test_uncompress( const BYTE * pIn, unsigned int nInSize, BYTE * pOut, unsigned int nOutSize )
{
    unsigned long n = nOutSize;

    if ( Z_OK == uncompress( pOut, &n, pIn, nInSize ) )
    {
        return n;
    }
    else
    {
        return 0;
    }
}

    

DWORD WINAPI MergeThrd( LPVOID lpThreadParameter )
{
    CTestMergeFolderDlg * pDlg = (CTestMergeFolderDlg *)lpThreadParameter;

    TRACE("=== merge begin ====== \n" );

    MfeInit();
    MfeMerge( s_filePath, test_compress );
    MfeDeinit();

    TRACE("=== merge end  ==== \n" );
    pDlg->PostMessage( UM_PROGRESS, 100, 0 );

    return 0;
}

DWORD WINAPI ProgressThrd( LPVOID lpThreadParameter )
{
    int * pArg = (int *)lpThreadParameter;

    CTestMergeFolderDlg * pDlg = (CTestMergeFolderDlg *)pArg[0];
    DWORD dwMergeThrdId = pArg[1];

    delete[] pArg;



    while( s_bThrdFlag )
    {
        DWORD dwProgress = GetMergeProgress( dwMergeThrdId );

        if ( dwProgress > 0 )
        {
            pDlg->PostMessage( UM_PROGRESS, dwProgress, 0 );
        }
        

        TRACE("=== progress = %u \n", dwProgress );

        if ( dwProgress >= 100 )
        {
            break;
        }
        
        Sleep( 50 );
    }
    return 0;
}

DWORD CALLBACK MergeProgress( LPVOID lpParam )
{
    MfeInit();
    MfeMerge( s_filePath, test_compress );
    MfeDeinit();

    CTestMergeFolderDlg * p = (CTestMergeFolderDlg *)lpParam;
    p->PostMessage( UM_COMPLETE, 0, 0 );

	return 0;
}

void CTestMergeFolderDlg::OnMerge() 
{
	// TODO: Add your control notification handler code here
	if ( m_pMfe )
	{
		MfeClose( m_pMfe );
		m_pMfe = NULL;
		MfeDeinit();
	}

	

    BROWSEINFO bi;
	memset( &bi, 0, sizeof(BROWSEINFO) );

	char name[MAX_PATH];
	char dir[MAX_PATH];

	bi.hwndOwner      = GetSafeHwnd();
	bi.pszDisplayName = name;
	bi.lpszTitle      = "请选择要的目录";
	LPITEMIDLIST idl  = SHBrowseForFolder( &bi );   //显示目录对话框
	if ( NULL == idl)
	{
		return;
	}
	SHGetPathFromIDList( idl, dir );                //把目录对话框值给dir

    s_filePath = dir;

    // m_progCtrl.ShowWindow( SW_SHOW );
    m_btnMerge.EnableWindow( FALSE );
    m_btnOpen.EnableWindow( FALSE );
    m_lst.DeleteAllItems();



//     if ( 0 != m_progCtrl.run(MergeProgress, this) )
// 	{
// 		MessageBox("failed to run");
// 	}
    
    CProgressCtrl * p = (CProgressCtrl *)GetDlgItem(IDC_PROGRESS1);
    p->ShowWindow( SW_SHOW );
    p->SetRange( 0, 100 );
    p->SetPos( 0 );

    s_bThrdFlag = TRUE;

    DWORD dwThrdId = 0;
    s_h1 = CreateThread( 0, 0, MergeThrd, this, 0, &dwThrdId );

    int * pArg = new int[2];
    pArg[0] = (int)this;
    pArg[1] = dwThrdId;

    s_h2 = CreateThread( 0, 0, ProgressThrd, pArg, 0, 0 );


    //s_bThrdFlag = FALSE;
    //WaitForSingleObject( hThrd, INFINITE );
    //CreateThread( 0, 0, MergeThrd, 0, 0, 0 );
	//MfeMerge( _T(TEST_FOLDER), test_compress );
	
}

LRESULT CTestMergeFolderDlg::OnProgComplete(WPARAM wParam, LPARAM lParam)
{
    // m_progCtrl.ShowWindow( SW_HIDE );
    m_btnMerge.EnableWindow( TRUE );
    m_btnOpen.EnableWindow( TRUE );

    return 0;
}

LRESULT CTestMergeFolderDlg::OnProgNotify(WPARAM wParam, LPARAM lParam)
{
    CProgressCtrl * p = (CProgressCtrl *)GetDlgItem(IDC_PROGRESS1);
    p->SetPos( wParam );

    // complete
    if ( wParam >= 100 )
    {
        // p->ShowWindow( SW_HIDE );
        m_btnMerge.EnableWindow( TRUE );
        m_btnOpen.EnableWindow( TRUE );

        WaitForSingleObject( s_h1, INFINITE );
        s_bThrdFlag = FALSE;
        WaitForSingleObject( s_h2, INFINITE );
    }

    return 0;
}

void CTestMergeFolderDlg::ShowMfeFolder( PMfeFolder pFolder )
{
	m_lst.DeleteAllItems();

	int i = 0;

	PMfeFolder pParent = MfeGetParentFolder( pFolder );
	if ( pParent )
	{
		m_lst.InsertItem( i, _T("..") );
		m_lst.SetItemText( i, 1, _T("folder") );
		m_lst.SetItemData( i, (DWORD)pParent );
		i++;
	}

	PMfeFolder b = MfeGetFirstSubFolder( pFolder );

	while( b )
	{
		LPCTSTR c = MfeGetFolderName( b );
		m_lst.InsertItem( i, c );
		m_lst.SetItemText( i, 1, _T("folder") );
		m_lst.SetItemData( i, (DWORD)b );

		b = MfeGetNextSiblingFolder( b );
		i++;
	}

	PMfeFile d = MfeGetFirstSubFile( pFolder );
	while( d )
	{
		LPCTSTR c = MfeGetFileName( d );
		m_lst.InsertItem( i, c );
		m_lst.SetItemText( i, 1, _T("file") );
		m_lst.SetItemData( i, (DWORD)d );

		d = MfeGetNextSiblingFile( d );
		i++;
	}
}

void CTestMergeFolderDlg::OnOpen() 
{
	// TODO: Add your control notification handler code here
	if ( m_pMfe )
	{
		MfeClose( m_pMfe );
		m_pMfe = NULL;
		MfeDeinit();
	}

    CFileDialog fd( TRUE, 0, 0, 0, "mgf files (*.mgf)|*.mgf", this );
    if ( IDOK == fd.DoModal() )
    {
        m_pMfe = MfeOpen( fd.GetPathName() );

        PMfeFolder a = MfeGetRootFolder( m_pMfe );
	    ShowMfeFolder( a );
    }
	
    //m_pMfe = MfeOpen( s_filePath + _T(".mgf") );
	//m_pMfe = MfeOpen( _T(TEST_FOLDER) _T(".mgf") );
	
	

//	PMfeFolder g = MfeGetSpecifiedFolder( a, _T("aaa") );
//
//	PMfeFile   f = MfeGetSpecifiedFile( g, _T("aaa.txt") );
//
//	char * y = NULL;
//	int x = MfeReadFullFile( f, (void **)&y );
//	
//	char * z = new char[x + 1];
//	memcpy( z, y, x );
//	z[x] = '\0';
//	
//	::MessageBoxA( NULL, z, "", 0 );
//	
//	MfeFree( y );
//	delete[] z;
}

void CTestMergeFolderDlg::OnDblclkFolder(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	POINT pt;
	::GetCursorPos( &pt );

	m_lst.ScreenToClient( &pt );

	int nIndex = m_lst.HitTest( pt );
	if ( -1 != nIndex )
	{
		void * p = (void *)m_lst.GetItemData( nIndex );

		if ( IsMfeFolder( p ) )
		{
			ShowMfeFolder( p );
		}
		else
		{
			LPCTSTR a = MfeGetFileName( p );
			LPCTSTR c = GetFileSubfix( a );
			if ( 0 == STRCMP( _T("txt"), c )
				 || 0 == STRCMP( _T("cpp"), c )
                 || 0 == STRCMP( _T("h"), c )
                 || 0 == STRCMP( _T("ini"), c ) )
			{
//				char buff[1024];
//				unsigned int w = MfeGetFileSize( p );
//				sprintf( buff, "file size: %d", w );
//				::MessageBoxA( NULL, buff, "", 0 );
//		
//				PMfeRead q = MfeStartReadFile( p );
//				
//				int n = MfeRead( q, buff, 1024 -1  );
//				buff[n] = '\0';
//				::MessageBoxA( NULL, buff, "", 0 );
//
//				MfeCloseReadFile( q );

				char * y = NULL;
				int x = MfeReadFullFile( p, (void **)&y, test_uncompress );

				char * z = new char[x + 1];
				memcpy( z, y, x );
				z[x] = '\0';

                CString tmp;
                tmp.Format( "file length: %d", x );
                MessageBoxA( tmp );

//                FILE * fp = fopen( "test.txt", "wb" );
//                fwrite( z, 1, x + 1, fp );
//                fclose( fp );


				
				::MessageBoxA( NULL, z, "", 0 );
				
				MfeFree( y );
				delete[] z;
			}
		}
	}

	*pResult = 0;
}

void CTestMergeFolderDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	if ( m_pMfe )
	{
		MfeClose( m_pMfe );
		m_pMfe = NULL;
		MfeDeinit();
	}
}
