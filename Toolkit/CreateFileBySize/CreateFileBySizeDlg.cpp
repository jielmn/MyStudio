// CreateFileBySizeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CreateFileBySize.h"
#include "CreateFileBySizeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CCreateFileBySizeDlg 对话框




CCreateFileBySizeDlg::CCreateFileBySizeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateFileBySizeDlg::IDD, pParent)
    , m_duplicate(_T(""))
    , m_dwSize(0)
    , m_filename(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCreateFileBySizeDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_RADIO_RAND,      m_rdRand);
    DDX_Control(pDX, IDC_RADIO_DUPLICATE, m_rdDuplicate);
    DDX_Text(pDX, IDC_EDIT_DUPLICATE, m_duplicate);
    DDX_Text(pDX, IDC_EDIT_SIZE, m_dwSize);
    DDX_Control(pDX, IDC_COMBO_UNIT, m_cmbUnit);
    DDX_Control(pDX, IDC_BUTTON_BROWSE, m_btnBrowse);
    DDX_Control(pDX, IDC_BUTTON_GENERATE, m_btnGenerate);
    DDX_Text(pDX, IDC_EDIT_FILENAME, m_filename);
}

BEGIN_MESSAGE_MAP(CCreateFileBySizeDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_BUTTON_GENERATE, &CCreateFileBySizeDlg::OnBnClickedButtonGenerate)
    ON_BN_CLICKED(IDC_BUTTON_BROWSE, &CCreateFileBySizeDlg::OnBnClickedButtonBrowse)
END_MESSAGE_MAP()


// CCreateFileBySizeDlg 消息处理程序

BOOL CCreateFileBySizeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
    m_rdRand.SetCheck( BST_CHECKED );
    m_rdDuplicate.SetCheck( BST_UNCHECKED );

    m_duplicate = "1234567890"
                  "--------20"
                  "--------30"
                  "--------40"
                  "--------50"
                  "--------60"
                  "--------70"
                  "--------80"
                  "--------90"
                  "-------100";

    m_cmbUnit.InsertString( 0, "byte" );
    m_cmbUnit.InsertString( 1, "kb" );
    m_cmbUnit.InsertString( 2, "mb" );
    m_cmbUnit.SetCurSel( 0 );

    char szFileName[512];
    HMODULE hModule = GetModuleHandle( 0 );
    ASSERT( hModule );
    GetModuleFileName( hModule, szFileName, 256 );   

    char * pSlash = strrchr( szFileName, '\\' );
    ASSERT(pSlash);

    *pSlash = '\0';
    strcat(szFileName, "\\file.data");
    m_filename = szFileName;

    m_dwSize = 1024;

    UpdateData( FALSE );

    srand( (unsigned int )time(0) );
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CCreateFileBySizeDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCreateFileBySizeDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CCreateFileBySizeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CCreateFileBySizeDlg::OnBnClickedButtonGenerate()
{
    // TODO: 在此添加控件通知处理程序代码
    UpdateData();

    if ( 0 == m_dwSize )
    {
        MessageBox("大小不能不能为0!");
        return;
    }

    if ( m_rdDuplicate.GetCheck() && 0 == m_duplicate.GetLength() )
    {
        MessageBox("要重复的字串为空!");
        return;
    }


    DWORD  dwSize = m_dwSize;
    int    nSel   = m_cmbUnit.GetCurSel();

    if ( 1 == nSel )
    {
        dwSize *= 1024;
    }
    else if ( 2 == nSel )
    {
        dwSize *= 1024 * 1024;
    }

    const DWORD MAX_BUF_SIZE = 1024 * 1024 * 8;
    char * pBuf = new char[MAX_BUF_SIZE];
    if ( 0 == pBuf )
    {
        MessageBox("没有内存!");
        return;
    }

    FILE * fp = LmnOpenFile( m_filename, "wb" );
    if ( 0 == fp )
    {
        MessageBox("创建文件失败!");
        delete[] pBuf;
        return;
    }


    // 如果生成随机文件
    if ( m_rdRand.GetCheck() )
    {
        while( dwSize > 0 )
        {
            DWORD  dwWriteCnt = min(dwSize,MAX_BUF_SIZE);


            DWORD i;
            for ( i = 0; i < dwWriteCnt; i++ )
            {
                pBuf[i] = (char)(rand() % 255);
            }

            size_t dwWrite = fwrite( pBuf, 1, dwWriteCnt, fp );
            if ( dwWrite != dwWriteCnt )
            {
                MessageBox("写文件失败!");
                delete[] pBuf;
                fclose( fp );
                return;
            }

            dwSize -= dwWriteCnt;
        }
    }
    else
    {
        DWORD   dwDuplicatePos  = 0;
        DWORD   dwDuplicateLen  = m_duplicate.GetLength();
        CString sDuplicate      = m_duplicate + m_duplicate;


        while( dwSize > 0 )
        {
            DWORD  dwWriteCnt = min(dwSize,MAX_BUF_SIZE);


            DWORD  dwMutiple = dwWriteCnt / dwDuplicateLen;
            DWORD  dwResue   = dwWriteCnt % dwDuplicateLen;
            char * pTmp      = pBuf;
            CString sTmp     = sDuplicate.Mid( dwDuplicatePos, dwDuplicateLen );


            DWORD i;
            for ( i = 0; i < dwMutiple; i++ )
            {
                memcpy( pTmp, (const char *)sTmp, dwDuplicateLen );
                pTmp += dwDuplicateLen;
            }

            if ( dwResue > 0 )
            {
                memcpy( pTmp, (const char *)sTmp, dwResue );
                // pTmp += dwResue;
                dwDuplicatePos = ( dwDuplicatePos + dwResue ) % dwDuplicateLen;
            }



            size_t dwWrite = fwrite( pBuf, 1, dwWriteCnt, fp );
            if ( dwWrite != dwWriteCnt )
            {
                MessageBox("写文件失败!");
                delete[] pBuf;
                fclose( fp );
                return;
            }

            dwSize -= dwWriteCnt;
        }
    }




    delete[] pBuf;
    fclose(fp);

    MessageBox("生成文件成功");
}

void CCreateFileBySizeDlg::OnBnClickedButtonBrowse()
{
    // TODO: 在此添加控件通知处理程序代码
    CFileDialog saveFileDlg( FALSE, "data file(*.data)|*.data", "file",
                             OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                             "data file(*.data)|*.data|any file(*.*)|*.*||", this);

    if ( IDOK == saveFileDlg.DoModal() )
    {
        m_filename = saveFileDlg.GetPathName();
        UpdateData( FALSE);
    }
}
