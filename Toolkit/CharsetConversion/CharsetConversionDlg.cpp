// CharsetConversionDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CharsetConversion.h"
#include "CharsetConversionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CCharsetConversionDlg �Ի���




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


// CCharsetConversionDlg ��Ϣ�������

BOOL CCharsetConversionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
    CRichEditCtrl * pRichedit = 0;

    pRichedit = (CRichEditCtrl*)GetDlgItem( IDC_RICHEDIT2_GBK );
    pRichedit->LimitText( 0x7FFFFFFF );

    pRichedit = (CRichEditCtrl*)GetDlgItem( IDC_RICHEDIT2_UTF8 );
    pRichedit->LimitText( 0x7FFFFFFF );


    m_sGbkStatus  = "";
    m_sUtf8Status = "";

    UpdateData( FALSE );

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CCharsetConversionDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CCharsetConversionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CCharsetConversionDlg::OnBnClickedButtonGbkConvert()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
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
        MessageBox("д��utf-8��ʧ�ܣ��뱣��Ϊutf-8��ʽ�ļ���");
    }


    delete[]  pwchUnicode;
    delete[]  pchUtf8;
}

void CCharsetConversionDlg::OnBnClickedButtonUtf8Convert()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������

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
    // TODO: �ڴ���ӿؼ�֪ͨ����������

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
    // TODO: �ڴ���ӿؼ�֪ͨ����������

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
    // TODO: �ڴ���ӿؼ�֪ͨ����������
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
    // TODO: �ڴ���ӿؼ�֪ͨ����������
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
