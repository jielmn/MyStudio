// CreateChatRoomDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyTalk.h"
#include "CreateChatRoomDlg.h"
#include "ParticipantDlg.h"


// CCreateChatRoomDlg 对话框

IMPLEMENT_DYNAMIC(CCreateChatRoomDlg, CDialog)

CCreateChatRoomDlg::CCreateChatRoomDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateChatRoomDlg::IDD, pParent)
    , m_bPersistent(FALSE)
{

}

CCreateChatRoomDlg::~CCreateChatRoomDlg()
{
}

void CCreateChatRoomDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_OWNER, m_comboOwner);
    DDX_Control(pDX, IDC_LIST_PARTICIPANT, m_lstParticipant);
    DDX_Check(pDX, IDC_CHECK1, m_bPersistent);
}


BEGIN_MESSAGE_MAP(CCreateChatRoomDlg, CDialog)
    ON_BN_CLICKED(IDC_BTN_ADD, &CCreateChatRoomDlg::OnBnClickedBtnAdd)
    ON_BN_CLICKED(IDC_BTN_DEL, &CCreateChatRoomDlg::OnBnClickedBtnDel)
    ON_BN_CLICKED(IDOK, &CCreateChatRoomDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &CCreateChatRoomDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CCreateChatRoomDlg 消息处理程序

void CCreateChatRoomDlg::OnBnClickedBtnAdd()
{
    // TODO: 在此添加控件通知处理程序代码
    CParticipantDlg dlg;

    if ( IDOK == dlg.DoModal() )
    {
        int nCnt = m_lstParticipant.GetCount();

        m_lstParticipant.InsertString( nCnt, dlg.m_sParticipant );
    }
}

void CCreateChatRoomDlg::OnBnClickedBtnDel()
{
    // TODO: 在此添加控件通知处理程序代码
    int nSel = m_lstParticipant.GetCurSel();

    if ( nSel >= 0 )
    {
        m_lstParticipant.DeleteString( nSel );

        if ( m_lstParticipant.GetCount() > nSel )
        {
            m_lstParticipant.SetCurSel(nSel);
        }
        else
        {
            int nCnt = m_lstParticipant.GetCount();

            if ( nCnt > 0 )
            {
                m_lstParticipant.SetCurSel( nCnt - 1 );
            }
        }
    }
}

void CCreateChatRoomDlg::OnBnClickedOk()
{
    // TODO: 在此添加控件通知处理程序代码
    UpdateData( TRUE );

    int nSel = m_comboOwner.GetCurSel();
    ASSERT( nSel >= 0 );

    int nIndex = m_comboOwner.GetItemData(nSel);

    LibJingleEx::HROOM  hRoom = 0;
    LibJingleEx::ELibjingleError e =  LibJingleEx::CreateUniqueRoom( g_hXmppApp[nIndex], &hRoom, "测试123", m_bPersistent );
    if ( 0 != e )
    {
        MessageBox("Failed to create LibJingleEx::CreateUniqueRoom() !");
        return;
    }

    vChatRoom & v = g_vChatRooms[nIndex];

    PTChatRoom p = new TChatRoom;
    p->m_hRoom = hRoom;

    v.push_back( p );
    
    int i;
    for ( i = 0; i < m_lstParticipant.GetCount(); i++ )
    {
        PTParticipant pParticipant = new TParticipant;

        CString s;
        m_lstParticipant.GetText( i, s);

        LibJingleEx::StrJid2TJid( s, pParticipant->m_tJid  );

        p->m_vInvited.push_back( pParticipant );
    }

    OnOK();
}

void CCreateChatRoomDlg::OnBnClickedCancel()
{
    // TODO: 在此添加控件通知处理程序代码
    OnCancel();
}

BOOL CCreateChatRoomDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    std::vector<int>::iterator it;
    int i;
    for ( it = m_vInt.begin(), i = 0; it != m_vInt.end(); it++, i++ )
    {
        int nIndex = *it;
        LibJingleEx::TLoginSettings & account = g_tAccounts[nIndex];

        int nTmp = m_comboOwner.InsertString( i, GetBareJid(account.sUserName, account.sDomain).c_str() );
        m_comboOwner.SetItemData( nTmp, nIndex );
    }

    if ( m_comboOwner.GetCount() > 0 )
    {
        m_comboOwner.SetCurSel( 0 );
    }

    return TRUE;
}
