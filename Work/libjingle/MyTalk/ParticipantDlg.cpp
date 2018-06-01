// ParticipantDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MyTalk.h"
#include "ParticipantDlg.h"


// CParticipantDlg �Ի���

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


// CParticipantDlg ��Ϣ�������

void CParticipantDlg::OnBnClickedOk()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    UpdateData( TRUE );
    if ( 0 == m_sParticipant.GetLength() )
    {
        MessageBox("�����뱻�����˵�jid");
        return;
    }

    OnOK();
}

void CParticipantDlg::OnBnClickedCancel()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    OnCancel();
}
