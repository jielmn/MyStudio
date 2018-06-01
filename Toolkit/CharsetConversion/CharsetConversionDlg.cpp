// CharsetConversionDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CharsetConversion.h"
#include "CharsetConversionDlg.h"

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


// CCharsetConversionDlg 对话框




CCharsetConversionDlg::CCharsetConversionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCharsetConversionDlg::IDD, pParent)
    , m_sGbkStatus(_T(""))
    , m_sUtf8Status(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCharsetConversionDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_STATIC_GBK, m_sGbkStatus);
    DDX_Text(pDX, IDC_STATIC_UTF8, m_sUtf8Status);
}

BEGIN_MESSAGE_MAP(CCharsetConversionDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_BUTTON_GBK_CONVERT, &CCharsetConversionDlg::OnBnClickedButtonGbkConvert)
    ON_BN_CLICKED(IDC_BUTTON_UTF8_CONVERT, &CCharsetConversionDlg::OnBnClickedButtonUtf8Convert)
    ON_BN_CLICKED(IDC_BUTTON_GBK_SAVE, &CCharsetConversionDlg::OnBnClickedButtonGbkSave)
    ON_BN_CLICKED(IDC_BUTTON_UTF8_SAVE, &CCharsetConversionDlg::OnBnClickedButtonUtf8Save)
    ON_BN_CLICKED(IDC_BUTTON_GBK_CONVERT2, &CCharsetConversionDlg::OnBnClickedButtonGbkConvert2)
    ON_BN_CLICKED(IDC_BUTTON_UTF8_CONVERT2, &CCharsetConversionDlg::OnBnClickedButtonUtf8Convert2)
END_MESSAGE_MAP()


// CCharsetConversionDlg 消息处理程序

BOOL CCharsetConversionDlg::OnInitDialog()
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
    CRichEditCtrl * pRichedit = 0;

    pRichedit = (CRichEditCtrl*)GetDlgItem( IDC_RICHEDIT2_GBK );
    pRichedit->LimitText( 0x7FFFFFFF );

    pRichedit = (CRichEditCtrl*)GetDlgItem( IDC_RICHEDIT2_UTF8 );
    pRichedit->LimitText( 0x7FFFFFFF );


    m_sGbkStatus  = "";
    m_sUtf8Status = "";

    UpdateData( FALSE );

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CCharsetConversionDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CCharsetConversionDlg::OnPaint()
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
HCURSOR CCharsetConversionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CCharsetConversionDlg::OnBnClickedButtonGbkConvert()
{
    // TODO: 在此添加控件通知处理程序代码
    CRichEditCtrl * pRichedit = 0;
    pRichedit = (CRichEditCtrl*)GetDlgItem( IDC_RICHEDIT2_GBK );

    CString   sGbk;
    CString   sUtf8;

    pRichedit->GetWindowText( sGbk );


    int nUnicodeSize    = sGbk.GetLength() * 2;
    WCHAR * pwchUnicode = new WCHAR[nUnicodeSize + 1];

    int nUtf8Size       =   ( ( sGbk.GetLength() - 1 ) / 2 + 1 ) * 3;
    char * pchUtf8      =   new char[nUtf8Size + 1];

    int nUnicodeLen = MultiByteToWideChar( CP_ACP,  0, sGbk,        sGbk.GetLength(),    pwchUnicode, nUnicodeSize );
    int nUtf8Len    = WideCharToMultiByte( CP_UTF8, 0, pwchUnicode, nUnicodeLen,         pchUtf8,     nUtf8Size, 0, 0 );

    pwchUnicode[nUnicodeLen] = L'\0';
    pchUtf8[nUtf8Len]        = '\0';


    pRichedit = (CRichEditCtrl*)GetDlgItem( IDC_RICHEDIT2_UTF8 );
    pRichedit->SetWindowText( pchUtf8 );

    CString  sTmp;
    pRichedit->GetWindowText( sTmp );
    int nEditCtrlLen = sTmp.GetLength();
    if ( nEditCtrlLen != nUtf8Len )
    {
        MessageBox("写入utf-8框失败，请保存为utf-8格式文件！");
    }


    delete[]  pwchUnicode;
    delete[]  pchUtf8;
}

void CCharsetConversionDlg::OnBnClickedButtonUtf8Convert()
{
    // TODO: 在此添加控件通知处理程序代码

    CRichEditCtrl * pRichedit = 0;
    pRichedit = (CRichEditCtrl*)GetDlgItem( IDC_RICHEDIT2_UTF8 );

    CString   sGbk;
    CString   sUtf8;

    pRichedit->GetWindowText( sUtf8 );


    int nUnicodeSize    = sUtf8.GetLength();
    WCHAR * pwchUnicode = new WCHAR[nUnicodeSize + 1];

    int nGbkSize        =   sUtf8.GetLength();
    char * pchGbk       =   new char[nGbkSize + 1];


    int nUnicodeLen = MultiByteToWideChar( CP_UTF8,  0, sUtf8,       sUtf8.GetLength(),    pwchUnicode, nUnicodeSize );
    int nGbkLen     = WideCharToMultiByte( CP_ACP,   0, pwchUnicode, nUnicodeLen,          pchGbk,      nGbkSize, 0, 0 );

    pwchUnicode[nUnicodeLen] = L'\0';
    pchGbk[nGbkLen]          = '\0';


    pRichedit = (CRichEditCtrl*)GetDlgItem( IDC_RICHEDIT2_GBK );
    pRichedit->SetWindowText( pchGbk );


    delete[]  pwchUnicode;
    delete[]  pchGbk;
}

void CCharsetConversionDlg::OnBnClickedButtonGbkSave()
{
    // TODO: 在此添加控件通知处理程序代码

    CFileDialog saveFileDlg( FALSE, "text file(*.txt)|*.txt", "gbk",
                             OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                             "text file(*.txt)|*.txt|any file(*.*)|*.*||", this);

    if ( IDOK == saveFileDlg.DoModal() )
    {
        CRichEditCtrl * pRichedit = 0;
        pRichedit = (CRichEditCtrl*)GetDlgItem( IDC_RICHEDIT2_GBK );

        CString   sGbk;
        pRichedit->GetWindowText( sGbk );

        FILE * fp = LmnOpenFile( saveFileDlg.GetPathName(), "wb" );
        fwrite( (const char *)sGbk, 1, sGbk.GetLength(), fp );
        fclose( fp );
    }
}

void CCharsetConversionDlg::OnBnClickedButtonUtf8Save()
{
    // TODO: 在此添加控件通知处理程序代码

    CFileDialog saveFileDlg( FALSE, "text file(*.txt)|*.txt", "utf8",
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        "text file(*.txt)|*.txt|any file(*.*)|*.*||", this);

    if ( IDOK == saveFileDlg.DoModal() )
    {
        CRichEditCtrl * pRichedit = 0;
        pRichedit = (CRichEditCtrl*)GetDlgItem( IDC_RICHEDIT2_UTF8 );

        CString   sUtf8;
        pRichedit->GetWindowText( sUtf8 );

        FILE * fp = LmnOpenFile( saveFileDlg.GetPathName(), "wb" );
        fwrite( (const char *)sUtf8, 1, sUtf8.GetLength(), fp );
        fclose( fp );
    }
}

void CCharsetConversionDlg::OnBnClickedButtonGbkConvert2()
{
    // TODO: 在此添加控件通知处理程序代码
    CFileDialog saveFileDlg( FALSE, "text file(*.txt)|*.txt", "utf8",
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        "text file(*.txt)|*.txt|any file(*.*)|*.*||", this);

    if ( IDOK == saveFileDlg.DoModal() )
    {
        CRichEditCtrl * pRichedit = 0;
        pRichedit = (CRichEditCtrl*)GetDlgItem( IDC_RICHEDIT2_GBK );

        CString   sGbk;
        pRichedit->GetWindowText( sGbk );


        int nUnicodeSize    = sGbk.GetLength() * 2;
        WCHAR * pwchUnicode = new WCHAR[nUnicodeSize + 1];

        int nUtf8Size       =   ( ( sGbk.GetLength() - 1 ) / 2 + 1 ) * 3;
        char * pchUtf8      =   new char[nUtf8Size + 1];

        int nUnicodeLen = MultiByteToWideChar( CP_ACP,  0, sGbk,        sGbk.GetLength(),    pwchUnicode, nUnicodeSize );
        int nUtf8Len    = WideCharToMultiByte( CP_UTF8, 0, pwchUnicode, nUnicodeLen,         pchUtf8,     nUtf8Size, 0, 0 );

        pwchUnicode[nUnicodeLen] = L'\0';
        pchUtf8[nUtf8Len]        = '\0';

        FILE * fp = LmnOpenFile( saveFileDlg.GetPathName(), "wb" );
        fwrite( (const char *)pchUtf8, 1, nUtf8Len, fp );
        fclose( fp );

        delete[]  pwchUnicode;
        delete[]  pchUtf8;
    }
}

void CCharsetConversionDlg::OnBnClickedButtonUtf8Convert2()
{
    // TODO: 在此添加控件通知处理程序代码
    CFileDialog saveFileDlg( TRUE, "text file(*.txt)|*.txt", 0,
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        "text file(*.txt)|*.txt|any file(*.*)|*.*||", this);

    if ( IDOK == saveFileDlg.DoModal() )
    {
        FILE * fp = LmnOpenFile( saveFileDlg.GetPathName(), "rb" );
        fseek( fp, 0, SEEK_END );
        long nLen = ftell( fp );
        char * pSrc = new char[nLen+1];
        fseek( fp, 0, SEEK_SET );
        fread( pSrc, 1, nLen, fp );
        fclose( fp );

        pSrc[nLen] = '\0';



        int nUnicodeSize    = nLen;
        WCHAR * pwchUnicode = new WCHAR[nUnicodeSize + 1];

        int nGbkSize        =   nLen;
        char * pchGbk       =   new char[nGbkSize + 1];


        int nUnicodeLen = MultiByteToWideChar( CP_UTF8,  0, pSrc,        nLen,        pwchUnicode, nUnicodeSize );
        int nGbkLen     = WideCharToMultiByte( CP_ACP,   0, pwchUnicode, nUnicodeLen, pchGbk,      nGbkSize, 0, 0 );

        pwchUnicode[nUnicodeLen] = L'\0';
        pchGbk[nGbkLen]          = '\0';


        CRichEditCtrl* pRichedit = (CRichEditCtrl*)GetDlgItem( IDC_RICHEDIT2_GBK );
        pRichedit->SetWindowText( pchGbk );


        delete[]  pwchUnicode;
        delete[]  pchGbk;
        delete[]  pSrc;

    }
}
