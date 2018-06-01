// HexStream2FileDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HexStream2File.h"
#include "HexStream2FileDlg.h"

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


// CHexStream2FileDlg 对话框




CHexStream2FileDlg::CHexStream2FileDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHexStream2FileDlg::IDD, pParent)
    , m_HexStream(_T(""))
    , m_Delimiter(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHexStream2FileDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_HEX_STREAM, m_HexStream);
    DDX_Text(pDX, IDC_EDIT_DELIMITER, m_Delimiter);
}

BEGIN_MESSAGE_MAP(CHexStream2FileDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDOK, &CHexStream2FileDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &CHexStream2FileDlg::OnBnClickedCancel)
    ON_BN_CLICKED(IDC_BUTTON_SAVE, &CHexStream2FileDlg::OnBnClickedButtonSave)
END_MESSAGE_MAP()


// CHexStream2FileDlg 消息处理程序

BOOL CHexStream2FileDlg::OnInitDialog()
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

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CHexStream2FileDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CHexStream2FileDlg::OnPaint()
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
HCURSOR CHexStream2FileDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CHexStream2FileDlg::OnBnClickedOk()
{
    // TODO: 在此添加控件通知处理程序代码
    
}

void CHexStream2FileDlg::OnBnClickedCancel()
{
    // TODO: 在此添加控件通知处理程序代码
    OnCancel();
}

int GetDigitalFromHex( char ch )
{
    ch = tolower( ch );

    if ( ch >= '0' && ch <= '9')
    {
        return ch - '0';
    }
    else if ( ch >= 'a' && ch <= 'f')
    {
        return ch - 'a' + 10;
    }
    else
    {
        return -1;
    }
}

BOOL GetCharFromHex( char ch1, char ch2, char & ch_out )
{
    int  n1 = GetDigitalFromHex( ch1 );
    int  n2 = GetDigitalFromHex( ch2 );

    if ( n1 < 0 || n2 < 0 )
    {
        return FALSE;
    }

    ch_out = (char)(n1 * 16 + n2);

    return TRUE;
}

void CHexStream2FileDlg::OnBnClickedButtonSave()
{
    // TODO: 在此添加控件通知处理程序代码
    UpdateData( TRUE );

    int i       = 0;
    int len     = m_HexStream.GetLength();
    char buf[2] = {0};

    const int MAX_BUF_SIZE = 1024 * 32;
    char result[MAX_BUF_SIZE] = { 0 };
    int  result_pos = 0;

    while( i < len )
    {
        if ( len - i >= 2 )
        {
            buf[0] = m_HexStream[i];
            buf[1] = m_HexStream[i+1];
            i += 2;

            // 解析一个字节
            if ( !GetCharFromHex( buf[0], buf[1], result[result_pos] ) )
            {
                MessageBox("格式不正确！");
                return;
            }
            result_pos++;
            //

            // 解析完毕
            if ( i == len )
            {
                break;
            }

            // 跳过delimiter
            i += m_Delimiter.GetLength();
            //
        }
        else
        {
            MessageBox("格式不正确！");
            return;
            break;
        }
    }


    CFileDialog saveFileDlg( FALSE, "data file(*.data)|*.data", "value",
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        "data file(*.data)|*.data|any file(*.*)|*.*||", this);

    if ( IDOK == saveFileDlg.DoModal() )
    {
        FILE * fp = LmnOpenFile( saveFileDlg.GetPathName(), "wb" );
        fwrite( result, 1, result_pos, fp );
        fclose( fp );
    }

}
