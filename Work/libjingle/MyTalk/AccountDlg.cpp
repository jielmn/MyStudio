// AccountDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyTalk.h"
#include "AccountDlg.h"


// CAccountDlg 对话框

IMPLEMENT_DYNAMIC(CAccountDlg, CDialog)

CAccountDlg::CAccountDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAccountDlg::IDD, pParent)
{

}

CAccountDlg::~CAccountDlg()
{
}

void CAccountDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_ACCOUNT, m_lstAccount);
}


BEGIN_MESSAGE_MAP(CAccountDlg, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_MODIFY, &CAccountDlg::OnBnClickedButtonModify)
    ON_BN_CLICKED(IDC_BUTTON_DELETE, &CAccountDlg::OnBnClickedButtonDelete)
END_MESSAGE_MAP()


// CAccountDlg 消息处理程序
BOOL CAccountDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_lstAccount.InsertColumn( 0, _T("序号"),   LVCFMT_LEFT, 50 );
    m_lstAccount.InsertColumn( 1, _T("用户名"), LVCFMT_LEFT, 370 );

    m_lstAccount.SetExtendedStyle(m_lstAccount.GetExtendedStyle()|LVS_EX_FULLROWSELECT);

    int i;
    for ( i = 0; i < MAX_ACCOUNT_NUMBER; i++ )
    {
        stringstream s;

        s << (i+1);
        m_lstAccount.InsertItem( i, s.str().c_str() );
       
        if ( '\0' != g_tAccounts[i].sUserName[0] && '\0' != g_tAccounts[i].sDomain[0] )
        {
            m_lstAccount.SetItemText( i, 1, GetFullJid(g_tAccounts[i].sUserName,g_tAccounts[i].sDomain,
                g_tAccounts[i].sResource ).c_str() );
        }
    }

    return TRUE;
}


void CAccountDlg::OnBnClickedButtonModify()
{
    POSITION pos = m_lstAccount.GetFirstSelectedItemPosition();
    if ( NULL == pos )
    {
        return;
    }

    int nIndex = m_lstAccount.GetNextSelectedItem(pos);

    m_SubAccountDlg.m_ptAccount = &g_tAccounts[nIndex];

    m_SubAccountDlg.DoModal();

    std::stringstream s;
    s << (nIndex + 1);

    std::string sIndex = s.str();

    std::string sTmp;

    sTmp  = CONFIG_USERNAME;
    sTmp += sIndex;
    g_Config.set( sTmp, g_tAccounts[nIndex].sUserName );

    sTmp  = CONFIG_PASSWORD;
    sTmp += sIndex;
    g_Config.set( sTmp, g_tAccounts[nIndex].sPassword );

    sTmp  = CONFIG_DOMAIN;
    sTmp += sIndex;
    g_Config.set( sTmp, g_tAccounts[nIndex].sDomain );

    sTmp  = CONFIG_HOST;
    sTmp += sIndex;
    g_Config.set( sTmp, g_tAccounts[nIndex].sHost );

    sTmp  = CONFIG_RESOURCE;
    sTmp += sIndex;
    g_Config.set( sTmp, g_tAccounts[nIndex].sResource );

    sTmp  = CONFIG_USE_TLS;
    sTmp += sIndex;

    s.str("");
    s << g_tAccounts[nIndex].bUseTls;
    g_Config.set( sTmp, s.str() );



    sTmp  = CONFIG_STUN_ADDR;
    sTmp += sIndex;
    g_Config.set( sTmp, g_tAccounts[nIndex].szStunAddr );

    sTmp  = CONFIG_RELAY_ADDR;
    sTmp += sIndex;
    g_Config.set( sTmp, g_tAccounts[nIndex].szRelayAddr );

    sTmp  = CONFIG_SAVE_PATH;
    sTmp += sIndex;
    g_Config.set( sTmp, g_tAccounts[nIndex].szDefaultSaveDir );

    sTmp  = CONFIG_STUN_PORT;
    sTmp += sIndex;

    s.str("");
    s << g_tAccounts[nIndex].wStunPort;
    g_Config.set( sTmp, s.str() );


    char path[512] = {0};
    GetModuleFileName( 0, path, 512 );

    char * p = strrchr( path, '\\' );
    if ( p )
    {
        *p = '\0';
    }
    std::string sPath = path;
    sPath += '\\';
    sPath += CONFIG_FILE;

    g_Config.SaveFile( sPath.c_str() );


    if ( '\0' != g_tAccounts[nIndex].sUserName[0] && '\0' != g_tAccounts[nIndex].sDomain[0] )
    {
        m_lstAccount.SetItemText( nIndex, 1, GetFullJid( g_tAccounts[nIndex].sUserName,
                                                         g_tAccounts[nIndex].sDomain,
                                                         g_tAccounts[nIndex].sResource ).c_str() );
    }
    else
    {
        m_lstAccount.SetItemText( nIndex, 1, "" );
    }
}

void CAccountDlg::OnBnClickedButtonDelete()
{
    // TODO: 在此添加控件通知处理程序代码
    POSITION pos = m_lstAccount.GetFirstSelectedItemPosition();
    if ( NULL == pos )
    {
        return;
    }

    int nIndex = m_lstAccount.GetNextSelectedItem(pos);


    memset( &g_tAccounts[nIndex], 0, sizeof(g_tAccounts[nIndex]) );


    std::stringstream s;
    s << (nIndex + 1);

    std::string sTmp;

    sTmp  = CONFIG_USERNAME;
    sTmp += s.str();
    g_Config.set( sTmp, g_tAccounts[nIndex].sUserName );

    sTmp  = CONFIG_PASSWORD;
    sTmp += s.str();
    g_Config.set( sTmp, g_tAccounts[nIndex].sPassword );

    sTmp  = CONFIG_DOMAIN;
    sTmp += s.str();
    g_Config.set( sTmp, g_tAccounts[nIndex].sDomain );

    sTmp  = CONFIG_HOST;
    sTmp += s.str();
    g_Config.set( sTmp, g_tAccounts[nIndex].sHost );

    sTmp  = CONFIG_RESOURCE;
    sTmp += s.str();
    g_Config.set( sTmp, g_tAccounts[nIndex].sResource );

    sTmp  = CONFIG_USE_TLS;
    sTmp += s.str();

    s.str("");
    s << g_tAccounts[nIndex].bUseTls;

    g_Config.set( sTmp, s.str() );


    char path[512] = {0};
    GetModuleFileName( 0, path, 512 );

    char * p = strrchr( path, '\\' );
    if ( p )
    {
        *p = '\0';
    }
    std::string sPath = path;
    sPath += '\\';
    sPath += CONFIG_FILE;

    g_Config.SaveFile( sPath.c_str() );


    if ( '\0' != g_tAccounts[nIndex].sUserName[0] && '\0' != g_tAccounts[nIndex].sDomain[0] )
    {
        m_lstAccount.SetItemText( nIndex, 1, GetFullJid( g_tAccounts[nIndex].sUserName,
            g_tAccounts[nIndex].sDomain,
            g_tAccounts[nIndex].sResource ).c_str() );
    }
    else
    {
        m_lstAccount.SetItemText( nIndex, 1, "" );
    }

}
