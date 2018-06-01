// PasswordDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MyTalk.h"
#include "PasswordDlg.h"


// CPasswordDlg �Ի���

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


// CPasswordDlg ��Ϣ�������

void CPasswordDlg::OnBnClickedOk()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    UpdateData();

    if ( m_sPassword != m_sConfirm )
    {
        MessageBox("ȷ����������벻��ͬ��");
        return;
    }

    LibJingleEx::ELibjingleError  eRet = LibJingleEx::SetPassword( g_hXmppApp[m_nIndex], m_sPassword );
    if ( 0 != eRet )
    {
        MessageBox("Failed to LibJingleEx::SetPassword()! ");
        return;
    }

    MessageBox("�޸�����ɹ���");


    OnOK();
}

BOOL CPasswordDlg::OnInitDialog()
{
    CDialog::OnInitDialog();



    return TRUE;
}
