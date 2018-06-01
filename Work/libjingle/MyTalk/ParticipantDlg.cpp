// ParticipantDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyTalk.h"
#include "ParticipantDlg.h"


// CParticipantDlg 对话框

IMPLEMENT_DYNAMIC(CParticipantDlg, CDialog)

CParticipantDlg::CParticipantDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CParticipantDlg::IDD, pParent)
    , m_sParticipant(_T(""))
{

}

CParticipantDlg::~CParticipantDlg()
{
}

void CParticipantDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_PARTICIPANT, m_sParticipant);
}


BEGIN_MESSAGE_MAP(CParticipantDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CParticipantDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &CParticipantDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CParticipantDlg 消息处理程序

void CParticipantDlg::OnBnClickedOk()
{
    // TODO: 在此添加控件通知处理程序代码
    UpdateData( TRUE );
    if ( 0 == m_sParticipant.GetLength() )
    {
        MessageBox("请输入被邀请人的jid");
        return;
    }

    OnOK();
}

void CParticipantDlg::OnBnClickedCancel()
{
    // TODO: 在此添加控件通知处理程序代码
    OnCancel();
}
