// HttpTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HttpTest.h"
#include "HttpTestDlg.h"
#include "httpstack.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CHttp * g_http;

#define  INIT_URL   "http://www.baidu.com/"
#define  PROXY_ADDR "172.16.0.1"
#define  PROXY_PORT  "808"

#define  INIT_URL2   "http://shijie.server/"
#define  PROXY_ADDR2 "192.168.1.200"
#define  PROXY_PORT2  "808"

// #define  INIT_URL3   "http://lemon.test/test.html"
#define  INIT_URL3   "https://shijie.server/"
#define  PROXY_ADDR3 "192.168.1.200"
#define  PROXY_PORT3  "1080"


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


// CHttpTestDlg 对话框




CHttpTestDlg::CHttpTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHttpTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHttpTestDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT1,  m_edit1);
    DDX_Control(pDX, IDC_LIST1,  m_list1);
    DDX_Control(pDX, IDC_RADIO3, m_opt1);
    DDX_Control(pDX, IDC_RADIO1, m_opt2);
    DDX_Control(pDX, IDC_RADIO2, m_opt3);
    DDX_Control(pDX, IDC_RADIO6, m_opt4);
    DDX_Control(pDX, IDC_RADIO4, m_opt5);
    DDX_Control(pDX, IDC_RADIO5, m_opt6);
    DDX_Control(pDX, IDC_RADIO9, m_opt7);
    DDX_Control(pDX, IDC_RADIO7, m_opt8);
    DDX_Control(pDX, IDC_RADIO8, m_opt9);
    DDX_Control(pDX, IDC_EDIT2, m_proxy);
    DDX_Control(pDX, IDC_EDIT3, m_proxy_port);
    DDX_Control(pDX, IDC_EDIT5, m_proxy2);
    DDX_Control(pDX, IDC_EDIT6, m_proxy_port2);
    DDX_Control(pDX, IDC_EDIT9, m_proxy3);
    DDX_Control(pDX, IDC_EDIT10, m_proxy_port3);
    DDX_Control(pDX, IDC_EDIT4, m_edit2);
    DDX_Control(pDX, IDC_EDIT8, m_edit3);
}

BEGIN_MESSAGE_MAP(CHttpTestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_BUTTON1, &CHttpTestDlg::OnBnClickedButton1)
    ON_MESSAGE( UM_HTTP_RESPONSE, OnHttpResponse )
    ON_BN_CLICKED(IDC_BUTTON2, &CHttpTestDlg::OnBnClickedButton2)
    ON_BN_CLICKED(IDC_BUTTON3, &CHttpTestDlg::OnBnClickedButton3)
    ON_BN_CLICKED(IDC_BUTTON4, &CHttpTestDlg::OnBnClickedButton4)
END_MESSAGE_MAP()


// CHttpTestDlg 消息处理程序

BOOL CHttpTestDlg::OnInitDialog()
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
    m_edit1.SetWindowText(INIT_URL);
    m_proxy.SetWindowText(PROXY_ADDR);
    m_proxy_port.SetWindowText(PROXY_PORT);

    m_edit2.SetWindowText(INIT_URL2);
    m_proxy2.SetWindowText(PROXY_ADDR2);
    m_proxy_port2.SetWindowText(PROXY_PORT2);

    m_edit3.SetWindowText(INIT_URL3);
    m_proxy3.SetWindowText(PROXY_ADDR3);
    m_proxy_port3.SetWindowText(PROXY_PORT3);

    this->CheckRadioButton( IDC_RADIO1, IDC_RADIO3, IDC_RADIO3 );
    this->CheckRadioButton( IDC_RADIO4, IDC_RADIO6, IDC_RADIO4 );
    this->CheckRadioButton( IDC_RADIO7, IDC_RADIO9, IDC_RADIO8 );

    


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CHttpTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CHttpTestDlg::OnPaint()
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
HCURSOR CHttpTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CHttpTestDlg::OnBnClickedButton1()
{
    // TODO: 在此添加控件通知处理程序代码
    char buf[4096];
    int nLen = m_edit1.GetWindowText( buf, 4096 );

    if ( nLen > 0 )
    {
        static int a = 0;
        a++;
        
        CHttpRequest request( buf );
        
        CHttpRequest::EProxyType eProxyType;

        if ( m_opt1.GetCheck() )
        {
            eProxyType = CHttpRequest::PROXY_TYPE_NONE;
        }
        else if ( m_opt2.GetCheck() )
        {
            eProxyType = CHttpRequest::PROXY_TYPE_HTTP;
        }
        else if ( m_opt3.GetCheck() )
        {
            eProxyType = CHttpRequest::PROXY_TYPE_SOCK5;
        }

        CString  proxy;
        CString  proxy_port;

        m_proxy.GetWindowText( proxy );
        m_proxy_port.GetWindowText( proxy_port );

        DWORD dwProxyPort = 0;
        if ( proxy_port.GetLength() > 0 )
        {
            sscanf( proxy_port, " %u", &dwProxyPort );
        }

        std::string sProxy = proxy;
        request.SetProxy( sProxy, (WORD)dwProxyPort, eProxyType );

        request.SetProxyUser("shijie");
        request.SetProxyPwd("shijie");

        g_http->AddRequest( request, (void*)a );
    }
}


LRESULT  CHttpTestDlg::OnHttpResponse(WPARAM wParam, LPARAM lParam)
{
    char * p = (char *)lParam;

    int n = strlen( p );
    if ( n >= 100 )
    {
        p[100] = '\0';
    }

    CString s;
    s.Format( "code: %u, \nData: [%s]", wParam, p );

    delete[] p;

    m_list1.InsertString( m_list1.GetCount(), s );

    return 0;
}
void CHttpTestDlg::OnBnClickedButton2()
{
    // TODO: 在此添加控件通知处理程序代码
    // TODO: 在此添加控件通知处理程序代码
    char buf[4096];
    int nLen = m_edit2.GetWindowText( buf, 4096 );

    if ( nLen > 0 )
    {
        static int a = 0;
        a++;

        CHttpRequest request( buf );

        CHttpRequest::EProxyType eProxyType;

        if ( m_opt4.GetCheck() )
        {
            eProxyType = CHttpRequest::PROXY_TYPE_NONE;
        }
        else if ( m_opt5.GetCheck() )
        {
            eProxyType = CHttpRequest::PROXY_TYPE_HTTP;
        }
        else if ( m_opt6.GetCheck() )
        {
            eProxyType = CHttpRequest::PROXY_TYPE_SOCK5;
        }

        CString  proxy;
        CString  proxy_port;

        m_proxy2.GetWindowText( proxy );
        m_proxy_port2.GetWindowText( proxy_port );

        DWORD dwProxyPort = 0;
        if ( proxy_port.GetLength() > 0 )
        {
            sscanf( proxy_port, " %u", &dwProxyPort );
        }

        std::string sProxy = proxy;
        request.SetProxy( sProxy, (WORD)dwProxyPort, eProxyType );

        request.SetProxyUser("shijie");
        request.SetProxyPwd("shijie");

        g_http->AddRequest( request, (void*)a );
    }
}

void CHttpTestDlg::OnBnClickedButton3()
{
    // TODO: 在此添加控件通知处理程序代码
    char buf[4096];
    int nLen = m_edit3.GetWindowText( buf, 4096 );

    if ( nLen > 0 )
    {
        static int a = 0;
        a++;

        CHttpRequest request( buf );

        CHttpRequest::EProxyType eProxyType;

        if ( m_opt7.GetCheck() )
        {
            eProxyType = CHttpRequest::PROXY_TYPE_NONE;
        }
        else if ( m_opt8.GetCheck() )
        {
            eProxyType = CHttpRequest::PROXY_TYPE_HTTP;
        }
        else if ( m_opt9.GetCheck() )
        {
            eProxyType = CHttpRequest::PROXY_TYPE_SOCK5;
        }

        CString  proxy;
        CString  proxy_port;

        m_proxy3.GetWindowText( proxy );
        m_proxy_port3.GetWindowText( proxy_port );

        DWORD dwProxyPort = 0;
        if ( proxy_port.GetLength() > 0 )
        {
            sscanf( proxy_port, " %u", &dwProxyPort );
        }

        std::string sProxy = proxy;
        request.SetProxy( sProxy, (WORD)dwProxyPort, eProxyType );

        request.SetProxyUser("shijie");
        request.SetProxyPwd("shijie");

        g_http->AddRequest( request, (void*)a );
    }
}

void CHttpTestDlg::OnBnClickedButton4()
{
    // TODO: 在此添加控件通知处理程序代码
    m_list1.ResetContent();
}
