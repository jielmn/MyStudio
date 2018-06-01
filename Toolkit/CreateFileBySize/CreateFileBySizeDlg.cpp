// CreateFileBySizeDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CreateFileBySize.h"
#include "CreateFileBySizeDlg.h"

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


// CCreateFileBySizeDlg �Ի���




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


// CCreateFileBySizeDlg ��Ϣ�������

BOOL CCreateFileBySizeDlg::OnInitDialog()
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
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CCreateFileBySizeDlg::OnPaint()
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
HCURSOR CCreateFileBySizeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CCreateFileBySizeDlg::OnBnClickedButtonGenerate()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    UpdateData();

    if ( 0 == m_dwSize )
    {
        MessageBox("��С���ܲ���Ϊ0!");
        return;
    }

    if ( m_rdDuplicate.GetCheck() && 0 == m_duplicate.GetLength() )
    {
        MessageBox("Ҫ�ظ����ִ�Ϊ��!");
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
        MessageBox("û���ڴ�!");
        return;
    }

    FILE * fp = LmnOpenFile( m_filename, "wb" );
    if ( 0 == fp )
    {
        MessageBox("�����ļ�ʧ��!");
        delete[] pBuf;
        return;
    }


    // �����������ļ�
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
                MessageBox("д�ļ�ʧ��!");
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
                MessageBox("д�ļ�ʧ��!");
                delete[] pBuf;
                fclose( fp );
                return;
            }

            dwSize -= dwWriteCnt;
        }
    }




    delete[] pBuf;
    fclose(fp);

    MessageBox("�����ļ��ɹ�");
}

void CCreateFileBySizeDlg::OnBnClickedButtonBrowse()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    CFileDialog saveFileDlg( FALSE, "data file(*.data)|*.data", "file",
                             OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                             "data file(*.data)|*.data|any file(*.*)|*.*||", this);

    if ( IDOK == saveFileDlg.DoModal() )
    {
        m_filename = saveFileDlg.GetPathName();
        UpdateData( FALSE);
    }
}
