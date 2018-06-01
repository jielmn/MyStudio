// EmptyDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyTalk.h"
#include "EmptyDlg.h"


// CEmptyDlg 对话框

IMPLEMENT_DYNAMIC(CEmptyDlg, CDialog)

CEmptyDlg::CEmptyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEmptyDlg::IDD, pParent)
{

}

CEmptyDlg::~CEmptyDlg()
{
}

void CEmptyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEmptyDlg, CDialog)
END_MESSAGE_MAP()


// CEmptyDlg 消息处理程序
