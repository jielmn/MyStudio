// AddFriendDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyTalk.h"
#include "AddFriendDlg.h"


// CAddFriendDlg 对话框

IMPLEMENT_DYNAMIC(CAddFriendDlg, CDialog)

CAddFriendDlg::CAddFriendDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddFriendDlg::IDD, pParent)
    , m_sNick(_T(""))
{

}

CAddFriendDlg::~CAddFriendDlg()
{
}

void CAddFriendDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_ACCOUNT, m_comboAccount);
    DDX_Text(pDX, IDC_EDIT_JID, m_sJid);
    DDX_Text(pDX, IDC_EDIT_NICK, m_sNick);
    DDX_Control(pDX, IDC_COMBO_GROUP, m_comboGroup);
}


BEGIN_MESSAGE_MAP(CAddFriendDlg, CDialog)
    ON_BN_CLICKED(IDC_BTN_ADD, &CAddFriendDlg::OnBnClickedBtnAdd)
    ON_BN_CLICKED(IDC_BTN_CANCEL, &CAddFriendDlg::OnBnClickedBtnCancel)
END_MESSAGE_MAP()


// CAddFriendDlg 消息处理程序
BOOL CAddFriendDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    std::vector<int>::iterator it;
    int i;
    for ( it = m_vInt.begin(), i=0; it != m_vInt.end(); it++,i++ )
    {
        int nIndex = *it;

        LibJingleEx::TLoginSettings & account = g_tAccounts[nIndex];

        int nTmp = m_comboAccount.InsertString( i, GetFullJid( account.sUserName, account.sDomain, account.sResource ).c_str()   );

        m_comboAccount.SetItemData( nTmp, nIndex );
    }

    if ( m_comboAccount.GetCount() > 0 )
    {
        m_comboAccount.SetCurSel( 0 );
    }

    m_comboGroup.InsertString( 0, "friend" );
    

    return TRUE;
}
void CAddFriendDlg::OnBnClickedBtnAdd()
{
    // TODO: 在此添加控件通知处理程序代码
    UpdateData( TRUE );

    if ( 0 == m_sJid.GetLength() )
    {
        MessageBox("请在好友名称栏填上好友的jid");
        return;
    }

    LibJingleEx::TJid tJid;
    LibJingleEx::ELibjingleError e = LibJingleEx::StrJid2TJid( m_sJid, tJid );
    if ( 0 != e )
    {
        MessageBox("不是有效的jid");
        return;
    }

    CString s;
    m_comboGroup.GetWindowText( s );
    
    if ( 0 == s.GetLength() )
    {
        s = "friend";
    }

    int nIndex = m_comboAccount.GetItemData( m_comboAccount.GetCurSel() );

    if ( 0 == m_sNick.GetLength() )
    {
        e = LibJingleEx::AddFriend( g_hXmppApp[nIndex], &tJid, s, 0 );
    }
    else
    {
        e = LibJingleEx::AddFriend( g_hXmppApp[nIndex], &tJid, s, m_sNick );
    }

    if ( 0 != e )
    {
        MessageBox("添加好友失败");
        return;
    }
    
    OnOK();
}

void CAddFriendDlg::OnBnClickedBtnCancel()
{
    // TODO: 在此添加控件通知处理程序代码
    OnCancel();
}
