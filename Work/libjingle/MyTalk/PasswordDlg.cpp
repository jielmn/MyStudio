// PasswordDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyTalk.h"
#include "PasswordDlg.h"


// CPasswordDlg 对话框

IMPLEMENT_DYNAMIC(CPasswordDlg, CDialog)

CPasswordDlg::CPasswordDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPasswordDlg::IDD, pParent)
    , m_sPassword(_T(""))
    , m_sConfirm(_T(""))
{
    m_nIndex = 0;
}

CPasswordDlg::~CPasswordDlg()
{
}

void CPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_PASSWORD, m_sPassword);
    DDX_Text(pDX, IDC_EDIT_CONFIRM_PASSWORD, m_sConfirm);
}


BEGIN_MESSAGE_MAP(CPasswordDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CPasswordDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CPasswordDlg 消息处理程序

void CPasswordDlg::OnBnClickedOk()
{
    // TODO: 在此添加控件通知处理程序代码
    UpdateData();

    if ( m_sPassword != m_sConfirm )
    {
        MessageBox("确认密码和密码不相同！");
        return;
    }

    LibJingleEx::ELibjingleError  eRet = LibJingleEx::SetPassword( g_hXmppApp[m_nIndex], m_sPassword );
    if ( 0 != eRet )
    {
        MessageBox("Failed to LibJingleEx::SetPassword()! ");
        return;
    }

    MessageBox("修改密码成功！");


    OnOK();
}

BOOL CPasswordDlg::OnInitDialog()
{
    CDialog::OnInitDialog();



    return TRUE;
}
