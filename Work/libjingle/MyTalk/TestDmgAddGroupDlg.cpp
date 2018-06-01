// TestDmgAddGroupDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyTalk.h"
#include "TestDmgAddGroupDlg.h"


// CTestDmgAddGroupDlg 对话框

IMPLEMENT_DYNAMIC(CTestDmgAddGroupDlg, CDialog)

CTestDmgAddGroupDlg::CTestDmgAddGroupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestDmgAddGroupDlg::IDD, pParent)
    , m_sGroupName(_T(""))
{

}

CTestDmgAddGroupDlg::~CTestDmgAddGroupDlg()
{
}

void CTestDmgAddGroupDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_GROUP_NAME, m_sGroupName);
}


BEGIN_MESSAGE_MAP(CTestDmgAddGroupDlg, CDialog)
END_MESSAGE_MAP()


// CTestDmgAddGroupDlg 消息处理程序
