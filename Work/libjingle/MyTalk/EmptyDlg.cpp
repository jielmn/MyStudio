// EmptyDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MyTalk.h"
#include "EmptyDlg.h"


// CEmptyDlg �Ի���

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


// CEmptyDlg ��Ϣ�������
