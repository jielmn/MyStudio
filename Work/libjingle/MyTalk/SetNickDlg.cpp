// SetNickDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyTalk.h"
#include "SetNickDlg.h"


// CSetNickDlg 对话框

IMPLEMENT_DYNAMIC(CSetNickDlg, CDialog)

CSetNickDlg::CSetNickDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetNickDlg::IDD, pParent)
    , m_sNick(_T(""))
{
    m_nIndex = -1;
}

CSetNickDlg::~CSetNickDlg()
{
}

void CSetNickDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_NICK, m_sNick);
}


BEGIN_MESSAGE_MAP(CSetNickDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CSetNickDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CSetNickDlg 消息处理程序
BOOL CSetNickDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_sNick = m_sOriNick;
    UpdateData( FALSE );

    return TRUE;
}

void CSetNickDlg::OnBnClickedOk()
{
    // TODO: 在此添加控件通知处理程序代码
    UpdateData( TRUE );

    if ( 0 == m_sNick.GetLength() )
    {
        MessageBox("请输入别名");
        return;
    }

    LibJingleEx::ELibjingleError e = LibJingleEx::SetFriendNick( g_hXmppApp[m_nIndex], &m_tJid, m_sNick );
    if ( 0 != e )
    {
        MessageBox("Failed to SetFriendNick()");
        return;
    }

    OnOK();
}
