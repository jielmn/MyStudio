// TestDmsAddMemberDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyTalk.h"
#include "TestDmsAddMemberDlg.h"


// CTestDmsAddMemberDlg 对话框

IMPLEMENT_DYNAMIC(CTestDmsAddMemberDlg, CDialog)

CTestDmsAddMemberDlg::CTestDmsAddMemberDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestDmsAddMemberDlg::IDD, pParent)
    , m_sName(_T(""))
    , m_sJid(_T(""))
    , m_dwType(0), m_bModify( FALSE )
{

}

CTestDmsAddMemberDlg::~CTestDmsAddMemberDlg()
{
}

void CTestDmsAddMemberDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_NAME, m_sName);
    DDX_Text(pDX, IDC_EDIT_JID, m_sJid);
    DDX_Text(pDX, IDC_EDIT_TYPE, m_dwType);

    DDX_Text(pDX, IDC_EDIT2, m_r1);
    DDX_Text(pDX, IDC_EDIT3, m_r2);
    DDX_Text(pDX, IDC_EDIT5, m_r3);
    DDX_Text(pDX, IDC_EDIT6, m_r4);
}


BEGIN_MESSAGE_MAP(CTestDmsAddMemberDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CTestDmsAddMemberDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CTestDmsAddMemberDlg 消息处理程序
BOOL CTestDmsAddMemberDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    if ( m_bModify )
    {
        GetDlgItem( IDC_EDIT_JID )->EnableWindow( FALSE );
        GetDlgItem( IDC_EDIT_TYPE )->EnableWindow( FALSE );
    }

    return TRUE;
}
void CTestDmsAddMemberDlg::OnBnClickedOk()
{
    // TODO: 在此添加控件通知处理程序代码
    OnOK();
}
