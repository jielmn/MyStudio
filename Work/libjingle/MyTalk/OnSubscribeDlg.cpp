// OnSubscribeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyTalk.h"
#include "OnSubscribeDlg.h"
#include "AddFriendDlg.h"


// COnSubscribeDlg 对话框

IMPLEMENT_DYNAMIC(COnSubscribeDlg, CDialog)

COnSubscribeDlg::COnSubscribeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COnSubscribeDlg::IDD, pParent)
    , m_sOnScribeMsg(_T(""))
{

}

COnSubscribeDlg::~COnSubscribeDlg()
{
}

void COnSubscribeDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BTN_AGREE, m_btnAgree);
    DDX_Control(pDX, IDC_BTN_REJECT, m_btnReject);
    DDX_Control(pDX, IDC_BTN_CLOSE, m_btnClose);
    DDX_Text(pDX, IDC_STATIC_MSG, m_sOnScribeMsg);
}


BEGIN_MESSAGE_MAP(COnSubscribeDlg, CDialog)
    ON_WM_SIZE()
    ON_WM_PAINT()
    ON_BN_CLICKED(IDC_BTN_AGREE, &COnSubscribeDlg::OnBnClickedBtnAgree)
    ON_BN_CLICKED(IDC_BTN_CLOSE, &COnSubscribeDlg::OnBnClickedBtnClose)
    ON_BN_CLICKED(IDC_BTN_REJECT, &COnSubscribeDlg::OnBnClickedBtnReject)
END_MESSAGE_MAP()


// COnSubscribeDlg 消息处理程序

BOOL COnSubscribeDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    return TRUE;
}

void COnSubscribeDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    // TODO: 在此处添加消息处理程序代码

    if ( ::IsWindow( m_btnAgree.m_hWnd ) )
    {
        m_btnAgree.MoveWindow( cx - 45 * 3 - 3 * 2 - 2, cy - 20 - 2, 45, 20 );
        m_btnReject.MoveWindow( cx - 45 * 2 - 3 - 2, cy - 20 - 2, 45, 20 );
        m_btnClose.MoveWindow( cx - 45 - 2, cy - 20 - 2, 45, 20 );

        GetDlgItem(IDC_STATIC_MSG)->MoveWindow( 2, 2, cx-2-2, cy - 20 - 2 - 2 - 2-2);
    }
}

void COnSubscribeDlg::OnPaint()
{
    PAINTSTRUCT ps;
    CDC* pDC = BeginPaint(&ps);
    EndPaint(&ps);
}

void COnSubscribeDlg::OnBnClickedBtnAgree()
{
    // TODO: 在此添加控件通知处理程序代码
    LibJingleEx::ELibjingleError e = LibJingleEx::AcceptFriend( m_hApp, &m_tJid, TRUE );
    if ( 0 !=  e )
    {
        MessageBox("Failed to LibJingleEx::AcceptFriend");
        return;
    }

    OnBnClickedBtnClose();

    if ( !LibJingleEx::IfHasFriend( m_hApp, &m_tJid ) )
    {
        CAddFriendDlg dlg;
        dlg.m_vInt.push_back( (int)LibJingleEx::GetUserData(m_hApp) );
        dlg.m_sJid = GetBareJid( m_tJid.sNode, m_tJid.sDomain ).c_str();
        dlg.DoModal();
    }
}

void COnSubscribeDlg::UpdateMsg()
{
    LibJingleEx::TJid tJid;

    LibJingleEx::ELibjingleError e = LibJingleEx::GetSelfJid( m_hApp, &tJid );
    if ( 0 != e )
    {
        MessageBox("Faield to LibJingleEx::GetSelfJid()!");
        return;
    }
        
    m_sOnScribeMsg.Format( "%s想要将你(%s)加为好友，同意吗？", 
                                    GetBareJid(m_tJid.sNode, m_tJid.sDomain).c_str(),
                                    GetBareJid(tJid.sNode,   tJid.sDomain).c_str()
                          );

    UpdateData(FALSE);
}
void COnSubscribeDlg::OnBnClickedBtnClose()
{
    // TODO: 在此添加控件通知处理程序代码
    CWnd * pParent = this->GetParent();
    ::SendMessage( pParent->m_hWnd, UM_ON_SUBSCRIBE_CLOSE, 0, 0 );
}

void COnSubscribeDlg::OnBnClickedBtnReject()
{
    // TODO: 在此添加控件通知处理程序代码
    LibJingleEx::ELibjingleError e = LibJingleEx::AcceptFriend( m_hApp, &m_tJid, FALSE );
    if ( 0 !=  e )
    {
        MessageBox("Failed to LibJingleEx::AcceptFriend");
        return;
    }

    OnBnClickedBtnClose();
}
