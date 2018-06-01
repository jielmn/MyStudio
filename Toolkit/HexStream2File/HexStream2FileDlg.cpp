// HexStream2FileDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "HexStream2File.h"
#include "HexStream2FileDlg.h"

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


// CHexStream2FileDlg �Ի���




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


// CHexStream2FileDlg ��Ϣ�������

BOOL CHexStream2FileDlg::OnInitDialog()
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

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CHexStream2FileDlg::OnPaint()
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
HCURSOR CHexStream2FileDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CHexStream2FileDlg::OnBnClickedOk()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    
}

void CHexStream2FileDlg::OnBnClickedCancel()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
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
    // TODO: �ڴ���ӿؼ�֪ͨ����������
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

            // ����һ���ֽ�
            if ( !GetCharFromHex( buf[0], buf[1], result[result_pos] ) )
            {
                MessageBox("��ʽ����ȷ��");
                return;
            }
            result_pos++;
            //

            // �������
            if ( i == len )
            {
                break;
            }

            // ����delimiter
            i += m_Delimiter.GetLength();
            //
        }
        else
        {
            MessageBox("��ʽ����ȷ��");
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
