// SubAccountDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MyTalk.h"
#include "SubAccountDlg.h"
#pragma warning(disable: 4996)

// CSubAccountDlg �Ի���

IMPLEMENT_DYNAMIC(CSubAccountDlg, CDialog)

CSubAccountDlg::CSubAccountDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSubAccountDlg::IDD, pParent), m_ptAccount(0)
    , m_username(_T(""))
    , m_password(_T(""))
    , m_domain(_T(""))
    , m_host(_T(""))
    , m_resource(_T(""))
    , m_bUseTls(FALSE)
    , m_strStunAddr(_T(""))
    , m_dwStunPort(0)
    , m_strRelayAddr(_T(""))
    , m_strSavePath(_T(""))
{

}

CSubAccountDlg::~CSubAccountDlg()
{
}

void CSubAccountDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_USERNAME, m_username);
    DDX_Text(pDX, IDC_EDIT_PASSWORD, m_password);
    DDX_Text(pDX, IDC_EDIT_DOMAIN, m_domain);
    DDX_Text(pDX, IDC_EDIT_HOST, m_host);
    DDX_Text(pDX, IDC_EDIT_RESOURCE, m_resource);
    DDX_Check(pDX, IDC_CHECK1, m_bUseTls);
    DDX_Text(pDX, IDC_EDIT_STUN_ADDR, m_strStunAddr);
    DDX_Text(pDX, IDC_EDIT_STUN_PORT, m_dwStunPort);
    DDV_MinMaxUInt(pDX, m_dwStunPort, 0, 65535);
    DDX_Text(pDX, IDC_EDIT_RELAY_ADDR, m_strRelayAddr);
    DDX_Text(pDX, IDC_EDIT_SAVE_PATH, m_strSavePath);
}


BEGIN_MESSAGE_MAP(CSubAccountDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CSubAccountDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDC_BTN_FOLDER, &CSubAccountDlg::OnBnClickedBtnFolder)
END_MESSAGE_MAP()


// CSubAccountDlg ��Ϣ�������
BOOL CSubAccountDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_username = m_ptAccount->sUserName;
    m_password = m_ptAccount->sPassword;
    m_domain   = m_ptAccount->sDomain;
    m_host     = m_ptAccount->sHost;
    m_resource = m_ptAccount->sResource;
    m_bUseTls  = m_ptAccount->bUseTls;

    m_strStunAddr  = m_ptAccount->szStunAddr;
    m_strRelayAddr = m_ptAccount->szRelayAddr;
    m_dwStunPort   = m_ptAccount->wStunPort;
    m_strSavePath  = m_ptAccount->szDefaultSaveDir;

    UpdateData( FALSE );

    return TRUE;
}
void CSubAccountDlg::OnBnClickedOk()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    UpdateData( TRUE );

    strcpy( m_ptAccount->sUserName, m_username );
    strcpy( m_ptAccount->sPassword, m_password );
    strcpy( m_ptAccount->sDomain,   m_domain );
    strcpy( m_ptAccount->sHost,     m_host );
    strcpy( m_ptAccount->sResource, m_resource );
    m_ptAccount->bUseTls = m_bUseTls;

    strcpy( m_ptAccount->szStunAddr,  m_strStunAddr );
    strcpy( m_ptAccount->szRelayAddr, m_strRelayAddr );
    m_ptAccount->wStunPort = (unsigned short)m_dwStunPort;
    strcpy( m_ptAccount->szDefaultSaveDir, m_strSavePath );

    m_ptAccount->bFileShareEnable = TRUE;

    OnOK();
}

void CSubAccountDlg::OnBnClickedBtnFolder()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    LPBROWSEINFO lpbi=new BROWSEINFO;
    lpbi->hwndOwner=GetSafeHwnd();
    lpbi->pidlRoot=NULL;
    lpbi->pszDisplayName=NULL;
    lpbi->lpszTitle="ѡ��Ŀ¼";
    lpbi->ulFlags=BIF_RETURNONLYFSDIRS|BIF_STATUSTEXT;
    lpbi->lpfn=NULL;

    LPITEMIDLIST lpitemidlist=SHBrowseForFolder(lpbi);
    if(lpitemidlist==NULL)
    {
        delete lpbi;
        lpbi=NULL;
        return;
    }

    char path[MAX_PATH];
    if ( SHGetPathFromIDList(lpitemidlist,path) )
    {
        UpdateData( TRUE );
        m_strSavePath = path;
        UpdateData( FALSE );
    }

    delete lpbi;
}
