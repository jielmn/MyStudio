// SelAccountDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyTalk.h"
#include "SelAccountDlg.h"


// CSelAccountDlg 对话框

IMPLEMENT_DYNAMIC(CSelAccountDlg, CDialog)

CSelAccountDlg::CSelAccountDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelAccountDlg::IDD, pParent), m_nIndex(-1)
{

}

CSelAccountDlg::~CSelAccountDlg()
{
}

void CSelAccountDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_ACCOUNT, m_comboAccount);
}


BEGIN_MESSAGE_MAP(CSelAccountDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CSelAccountDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &CSelAccountDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CSelAccountDlg 消息处理程序

void CSelAccountDlg::OnBnClickedOk()
{
    // TODO: 在此添加控件通知处理程序代码
    int nSel = m_comboAccount.GetCurSel();
    if ( nSel < 0 )
    {
        return;
    }

    m_nIndex = m_comboAccount.GetItemData( nSel );
    ASSERT( m_nIndex < MAX_ACCOUNT_NUMBER && m_nIndex >= 0 );

    OnOK();
}

void CSelAccountDlg::OnBnClickedCancel()
{
    // TODO: 在此添加控件通知处理程序代码
    OnCancel();
}

BOOL CSelAccountDlg::OnInitDialog()
{
    CDialog::OnInitDialog();


    std::vector<int>::iterator it;
    int i;
    for ( it = m_v.begin(), i = 0; it != m_v.end(); it++, i++ )
    {
        int nIndex = *it;

        LibJingleEx::TLoginSettings & t = g_tAccounts[nIndex];

        int nTmp = m_comboAccount.InsertString( i, GetFullJid(t.sUserName, t.sDomain, t.sResource).c_str()  );
        m_comboAccount.SetItemData( nTmp, nIndex );
    }

    if ( m_comboAccount.GetCount() > 0 )
    {
        m_comboAccount.SetCurSel( 0 );
    }

    

    return TRUE;
}
