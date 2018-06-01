// ChatRoomDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyTalk.h"
#include "ChatRoomDlg.h"
#include "ParticipantDlg.h"


// CChatRoomDlg 对话框

IMPLEMENT_DYNAMIC(CChatRoomDlg, CDialog)

CChatRoomDlg::CChatRoomDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChatRoomDlg::IDD, pParent)
    , m_sMessage(_T("")), m_pChatRoom(0), m_bQuit(FALSE), m_fWidthMsgPercent( 0.54f )
{

}

CChatRoomDlg::~CChatRoomDlg()
{
}

void CChatRoomDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_MSG, m_lstMsg);
    DDX_Control(pDX, IDC_LIST_PARTICIPANT, m_lstParticipant);
    DDX_Text(pDX, IDC_EDIT_MSG, m_sMessage);
    DDX_Control(pDX, IDC_STATIC_PARTICIPANT, m_staticParticipant);
    DDX_Control(pDX, IDC_STATIC_SEND_MSG, m_staticSendMsg);
    DDX_Control(pDX, IDC_EDIT_MSG, m_editSendMsg);
    DDX_Control(pDX, IDC_BTN_SEND, m_btnSend);
}


BEGIN_MESSAGE_MAP(CChatRoomDlg, CDialog)
    ON_BN_CLICKED(IDC_BTN_SEND, &CChatRoomDlg::OnBnClickedBtnSend)
    ON_COMMAND( ID_INVITE, OnInvite )
    ON_COMMAND( ID_QUIT_CHATROOM, OnQuitChatRoom )
    ON_COMMAND( ID_DESTROY_CHATROOM, OnDestroyChatRoom )
    ON_COMMAND( ID_KICK, OnKick )
    ON_COMMAND( ID_CHATROOM_TEST, OnTest )
    ON_COMMAND( ID_SCREEN, OnScreen )
    ON_COMMAND( ID_UNSCREEN, OnUnScreen )
    ON_WM_CLOSE()
    ON_WM_SIZE()
END_MESSAGE_MAP()


// CChatRoomDlg 消息处理程序
BOOL CChatRoomDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_lstParticipant.InsertColumn( 0, "JID", 0, 200 );
    m_lstParticipant.InsertColumn( 1, "Status", 0, 80 );
    m_lstParticipant.InsertColumn( 2, "Affiliation", 0, 80 );

    m_menu.LoadMenu( IDR_MENU_CHATROOM );
    this->SetMenu( &m_menu );

    CString s;
    s.Format( "聊天室-%s", GetFullJid( m_tSelfJid.sNode, m_tSelfJid.sDomain, m_tSelfJid.sResource ).c_str() );
    this->SetWindowText( s );

    this->SetIcon( AfxGetApp()->LoadIcon( IDR_MAINFRAME ), TRUE  );

    //ModifyStyleEx(0, WS_EX_APPWINDOW);//任务栏显示
    return TRUE;
}

void CChatRoomDlg::OnBnClickedBtnSend()
{
    // TODO: 在此添加控件通知处理程序代码
    UpdateData( TRUE );
    if ( 0 == m_sMessage.GetLength() )
    {
        return;
    }

    LibJingleEx::TDmsChatContent tMsg;
    // strcpy( (char *)tMsg.m_abyFontBuf, "123" );
    // strcpy( (char *)tMsg.m_abyTimeBuf, "456" );
    tMsg.m_dwTime = 1343099939;
    tMsg.m_dwChatType = 5;
    tMsg.m_dwColor    = 333;
    tMsg.m_dwPicCount = 2;
    strcpy( tMsg.m_szContent, m_sMessage );
    strcpy( tMsg.m_szMsgHeader, "pppxd" );
    tMsg.m_atPicFileInfoTable[0].m_dwIndex = 100;
    strcpy( tMsg.m_atPicFileInfoTable[0].m_achFileName, "qqqq" );

    LibJingleEx::ELibjingleError e =  LibJingleEx::SendRoomMessage( m_pChatRoom->m_hRoom, &tMsg );
    if ( e != 0 )
    {
        MessageBox("Failed to LibJingleEx::SendRoomMessage() !");
        return;
    }

    m_sMessage = "";
    UpdateData( FALSE );
}

BOOL CChatRoomDlg::PreTranslateMessage( MSG * pMsg )
{
    //if ( pMsg->message == WM_KEYUP )
    //{
    //    if ( pMsg->hwnd == GetDlgItem(IDC_EDIT_MSG)->m_hWnd  && '\r' == pMsg->wParam )
    //    {
    //        OnBnClickedBtnSend();
    //        return TRUE;
    //    }
    //}
    return CDialog::PreTranslateMessage( pMsg );
}

void CChatRoomDlg::UpdateParticipant( const LibJingleEx::PTJid ptJid, LibJingleEx::EPresence * pPres, 
                                       LibJingleEx::EAffiliation * peAffi, BOOL bKick /*= FALSE*/ )
{
    CString sJid = GetFullJid( ptJid->sNode, ptJid->sDomain, ptJid->sResource ).c_str();
    int nCnt = m_lstParticipant.GetItemCount();
    int i;

    for ( i = 0; i < nCnt; i++ )
    {
        CString s = m_lstParticipant.GetItemText( i, 0 );
        if ( s == sJid )
        {
            break;
        }
    }

    std::string  strBareJid = GetBareJid( ptJid->sNode, ptJid->sDomain );

    // 找到
    if ( i < nCnt )
    {
        // 不应该再有roster item
        DWORD  dwRosterItemIndex = GetRosterItemIndex( ptJid );
        ASSERT( -1 == dwRosterItemIndex );

        if ( pPres )
        {
            // 离开
            if ( *pPres == LibJingleEx::PRESENCE_NONE )
            {
                int nTmp = m_lstMsg.GetCount();
                CString sTmp;
                sTmp.Format("%s离开了房间", GetFullJid( ptJid->sNode, ptJid->sDomain, ptJid->sResource ).c_str() );
                m_lstMsg.InsertString( nTmp, sTmp );

                // 获取之前的affiliation
                CString  sAffiliation = m_lstParticipant.GetItemText( i, 2 );
                m_lstParticipant.DeleteItem( i );

                // 如果被踢出
                if ( bKick )
                {
                    // 其他实例踢出会收到通知的，不用管

                    DWORD dwIndex = GetRosterItemIndex( ptJid );
                    if ( -1 != dwIndex )
                    {
                        m_lstParticipant.DeleteItem( dwIndex );
                    }
                }
                else
                {
                    nCnt = m_lstParticipant.GetItemCount();
                    for ( i = 0; i < nCnt; i++ )
                    {
                        CString s = m_lstParticipant.GetItemText( i, 0 );

                        // 如果还有同样的bare jid的成员在
                        if ( 0 == s.Find( strBareJid.c_str() ) )
                        {
                            break;
                        }
                    }

                    // 如果还有同样的bare jid的成员在
                    if ( i < nCnt )
                    {

                    }
                    else
                    {
                        // 插入一条RosterItem
                        int nRosterItemIndex = m_lstParticipant.InsertItem( nCnt, strBareJid.c_str() );
                        m_lstParticipant.SetItemText( nRosterItemIndex, 1, "off" );
                        m_lstParticipant.SetItemText( nRosterItemIndex, 2, sAffiliation );
                    }
                }                

                return;
            }
            else
            {
                m_lstParticipant.SetItemText( i, 1, ::GetPresence(*pPres) );
            }
        }

        if ( peAffi )
        {
            m_lstParticipant.SetItemText( i, 2, ::GetAffiliation(*peAffi) );
        }
    }
    // 没找到
    else
    {
        if ( pPres )
        {
            // 离开
            if ( *pPres == LibJingleEx::PRESENCE_NONE )
            {
                int nTmp = m_lstMsg.GetCount();
                CString sTmp;
                sTmp.Format("%s离开了房间", sJid );
                m_lstMsg.InsertString( nTmp, sTmp );


                DWORD dwIndex = GetRosterItemIndex( ptJid );
                if ( -1 != dwIndex )
                {
                    m_lstParticipant.DeleteItem( dwIndex );
                }

                return;
            }

            DWORD  dwRosterItemIndex = GetRosterItemIndex( ptJid );
            if ( -1 != dwRosterItemIndex )
            {
                m_lstParticipant.DeleteItem(dwRosterItemIndex);
            }

            DWORD  dwFirstRosterItemIndex = GetFirstRosterItemIndex( );
            DWORD  dwIndex = 0;

            // 如果没有roster item
            if ( -1 == dwFirstRosterItemIndex )
            {
                dwIndex = m_lstParticipant.GetItemCount();
            }
            // 如果至少有一个roster item，把实例信心放在roster item之前
            else
            {
                dwIndex = dwFirstRosterItemIndex;
            }

            m_lstParticipant.InsertItem( dwIndex, sJid );
            m_lstParticipant.SetItemText( dwIndex, 1, ::GetPresence(*pPres) );
            if ( peAffi )
            {
                m_lstParticipant.SetItemText( dwIndex, 2, ::GetAffiliation(*peAffi) );
            }
        }
        else
        {
            DWORD  dwRosterItemIndex = GetRosterItemIndex( ptJid );
            if ( -1 != dwRosterItemIndex )
            {
                m_lstParticipant.DeleteItem(dwRosterItemIndex);
            }

            DWORD  dwFirstRosterItemIndex = GetFirstRosterItemIndex( );
            DWORD  dwIndex = 0;

            // 如果没有roster item
            if ( -1 == dwFirstRosterItemIndex )
            {
                dwIndex = m_lstParticipant.GetItemCount();
            }
            // 如果至少有一个roster item，把实例信心放在roster item之前
            else
            {
                dwIndex = dwFirstRosterItemIndex;
            }

            m_lstParticipant.InsertItem( dwIndex, sJid );
            if ( peAffi )
            {
                m_lstParticipant.SetItemText( dwIndex, 2, ::GetAffiliation(*peAffi) );
            }
        }

        int nTmp = m_lstMsg.GetCount();
        CString sTmp;
        sTmp.Format("%s进入了房间", GetFullJid( ptJid->sNode, ptJid->sDomain, ptJid->sResource ).c_str() );
        m_lstMsg.InsertString( nTmp, sTmp );
    }
}

void CChatRoomDlg::UpdateRosterItem( LibJingleEx::PTRoomRosterItem  pItem, BOOL bAdded )
{
    LibJingleEx::TJid & tJid = pItem->m_tJid;

    // 如果是增加roster item
    if ( bAdded )
    {
        CString sJid = GetFullJid( tJid.sNode, tJid.sDomain, tJid.sResource ).c_str();

        int nCnt = m_lstParticipant.GetItemCount();
        int i;

        for ( i = 0; i < nCnt; i++ )
        {
            CString s = m_lstParticipant.GetItemText( i, 0 );

            // 如果有该成员实例在房间或已经有了roster item
            if ( 0 == s.Find(sJid) )
            {
                return;
            }
        }
        
        // 插到末尾
        m_lstParticipant.InsertItem( nCnt, sJid );
        m_lstParticipant.SetItemText( nCnt, 1, "off" );
        m_lstParticipant.SetItemText( nCnt, 2, ::GetAffiliation(pItem->m_eAlliliation) );
    }
    // 如果是减少roster item
    else
    {
        CString sJid = GetFullJid( tJid.sNode, tJid.sDomain, tJid.sResource ).c_str();

        int i = 0;
        while( i < m_lstParticipant.GetItemCount() )
        {
            CString s = m_lstParticipant.GetItemText( i, 0 );

            // 如果有该成员实例在房间或已经有了roster item
            if ( 0 == s.Find(sJid) )
            {
                m_lstParticipant.DeleteItem( i );
            }
            else
            {
                i++;
            }
        }
    }
}


DWORD  CChatRoomDlg::GetFirstRosterItemIndex()
{
    int i;
    int nCnt = m_lstParticipant.GetItemCount();

    for ( i = 0; i < nCnt; i++ )
    {
        CString s = m_lstParticipant.GetItemText( i, 0 );
        
        LibJingleEx::TJid  tJid;
        LibJingleEx::StrJid2TJid( s, tJid );

        if ( tJid.sResource[0] == '\0' )
        {
            return i;
        }
    }

    // 没有barejid
    return -1;
}

DWORD  CChatRoomDlg::GetRosterItemIndex( LibJingleEx::PTJid  ptJid )
{
    std::string  strBareJid = ptJid->sNode;
    strBareJid += "@";
    strBareJid += ptJid->sDomain;

    int i;
    int nCnt = m_lstParticipant.GetItemCount();

    for ( i = 0; i < nCnt; i++ )
    {
        CString s = m_lstParticipant.GetItemText( i, 0 );
        if ( strBareJid.c_str() == s )
        {
            return i;
        }
    }

    return -1;
}


void CChatRoomDlg::OnInvite()
{
    CParticipantDlg dlg;

    if ( IDOK == dlg.DoModal() )
    {
        LibJingleEx::TJid tJid;
        LibJingleEx::StrJid2TJid( dlg.m_sParticipant, tJid );

        LibJingleEx::ELibjingleError e = LibJingleEx::InviteParticipant( m_pChatRoom->m_hRoom, &tJid );
        if ( 0 !=  e )
        {
            if ( LibJingleEx::LIBJINGLE_ERR_NOT_OWNER == e )
            {
                MessageBox("只有房间的所有者才能邀请人！");
                return;
            }
            else
            {
                MessageBox("Failed to LibJingleEx::InviteParticipant() !");
            }            
            return;
        }
    }
}

void CChatRoomDlg::OnQuitChatRoom()
{
    if ( IDYES == MessageBox("确定要离开房间吗？","离开房间", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2 ))
    {
        LibJingleEx::ELibjingleError e = LibJingleEx::QuitRoom( m_pChatRoom->m_hRoom );
        if ( 0 != e )
        {
            MessageBox("Failed to LibJingleEx::QuitRoom()");
            return;
        }

        m_bQuit = TRUE;

        this->OnOK();
    }
}

void CChatRoomDlg::OnDestroyChatRoom()
{
    if ( IDYES == MessageBox("确定要销毁房间吗？","销毁房间", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2 ))
    {
        LibJingleEx::ELibjingleError e = LibJingleEx::DestroyRoom( m_pChatRoom->m_hRoom );
        if ( 0 != e )
        {
            if ( LIBJINGLE_ERR_NOT_OWNER == e )
            {
                MessageBox("你不是房间的所有者！");
                return;
            }
            else
            {
                MessageBox("Failed to LibJingleEx::DestroyRoom()");
            }
            return;
        }

        m_bQuit = TRUE;

        this->OnOK();
    }
}

void CChatRoomDlg::OnKick()
{
    CParticipantDlg dlg;

    if ( IDOK == dlg.DoModal() )
    {
        LibJingleEx::TJid tJid;
        LibJingleEx::StrJid2TJid( dlg.m_sParticipant, tJid );

        LibJingleEx::ELibjingleError e = LibJingleEx::KickRoomMember( m_pChatRoom->m_hRoom, &tJid );
        if ( 0 !=  e )
        {
            if ( LibJingleEx::LIBJINGLE_ERR_NOT_OWNER == e )
            {
                MessageBox("只有房间的所有者才能踢人！");
                return;
            }
            else
            {
                MessageBox("Failed to LibJingleEx::KickRoomMember() !");
            }            
            return;
        }
    }

}


void CChatRoomDlg::OnTest()
{
    int i = 0;
    for ( i = 0; i < 10; i++ )
    {
        LibJingleEx::ELibjingleError e = LibJingleEx::SendRoomPicture( m_pChatRoom->m_hRoom, "d:\\shijie.bmp", 46182 );
        if ( 0 !=  e )
        {
            TRACE("%dth: Failed to LibJingleEx::SendRoomPicture()! ", i+1 );
        }
    }

    //LibJingleEx::ELibjingleError e = LibJingleEx::GetChatRoomMembers( m_pChatRoom->m_hRoom );
    //if ( 0 != e )
    //{
    //    MessageBox("Failed to LibJingleEx::GetChatRoomMembers()! ");
    //    return;
    //}
}

void CChatRoomDlg::AddMessage( const LibJingleEx::PTJid ptJid, const char * szMsg )
{
    int nTmp = m_lstMsg.GetCount();
    CString sTmp;
    sTmp.Format("%s说: %s", GetFullJid( ptJid->sNode, ptJid->sDomain, ptJid->sResource ).c_str(), szMsg );
    m_lstMsg.InsertString( nTmp, sTmp );
}

void CChatRoomDlg::OnClose()
{
    CDialog::OnClose();

    if ( !m_bQuit )
    {
        LibJingleEx::ELibjingleError e = LibJingleEx::QuitRoom( m_pChatRoom->m_hRoom );
        if ( 0 != e )
        {
            MessageBox("Failed to LibJingleEx::QuitRoom()");
        }
    }
}

void CChatRoomDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy );

    if ( !::IsWindow(m_lstMsg.m_hWnd) )
    {
        return;
    }

    CRect r1;
    r1.left   = 12;
    r1.top    = 12;
    r1.right  = r1.left + (int)( (cx - 12 * 3) * m_fWidthMsgPercent );
    r1.bottom = cy - 12 ; 

    m_lstMsg.MoveWindow( r1 );


    CRect rtTmp;
    m_staticParticipant.GetWindowRect( rtTmp );
    
    CRect r3;
    r3.left   = r1.right + 12;
    r3.top    = r1.top;
    r3.right  = r3.left + rtTmp.Width();
    r3.bottom = r3.top + rtTmp.Height();
    m_staticParticipant.MoveWindow( r3 );

    CRect r2;
    r2.left   = r1.right + 12;
    r2.top    = r3.bottom + 6;
    r2.right  = r2.left + ( cx - 12 * 3 - r1.Width() );
    r2.bottom = r1.bottom - 74;
    
    m_lstParticipant.MoveWindow( r2 );

    m_staticSendMsg.GetWindowRect( rtTmp );
    
    CRect r4;
    r4.left   = r2.left;
    r4.top    = r2.bottom + 6;
    r4.right  = r4.left + rtTmp.Width();
    r4.bottom = r4.top + rtTmp.Height();

    m_staticSendMsg.MoveWindow( r4 );

    
    m_btnSend.GetWindowRect(rtTmp);

    CRect r5;
    r5.top    = r4.bottom + 6;
    r5.right  = r2.right;
    r5.left   = r5.right - rtTmp.Width();
    r5.bottom = r5.top + rtTmp.Height();

    m_btnSend.MoveWindow( r5 );

    CRect r6;
    r6.left = r2.left;
    r6.top  = r5.top;
    r6.right = r5.left - 6;
    r6.bottom = r5.bottom;

    m_editSendMsg.MoveWindow( r6 );
    
}

void CChatRoomDlg::OnScreen()
{
    LibJingleEx::HANDLE h = LibJingleEx::GetHApp( m_pChatRoom->m_hRoom );
    if ( 0 == h )
    {
        MessageBox("Failed to LibJingleEx::GetHApp() ");
        return;
    }

    LibJingleEx::TChatRoomId t;
    LibJingleEx::ELibjingleError e = LibJingleEx::GetRoomId( m_pChatRoom->m_hRoom, &t );
    if ( 0 != e )
    {
        MessageBox("Failed to LibJingleEx::GetRoomId()");
        return;
    }

    e = LibJingleEx::ScreenChatroomsMsg( h, &t, 1 );
    if ( 0 !=  e )
    {
        MessageBox("Failed to LibJingleEx::ScreenChatroomsMsg()! ");
        return;
    }
}

void CChatRoomDlg::OnUnScreen()
{
    LibJingleEx::HANDLE h = LibJingleEx::GetHApp( m_pChatRoom->m_hRoom );
    if ( 0 == h )
    {
        MessageBox("Failed to LibJingleEx::GetHApp() ");
        return;
    }

    LibJingleEx::TChatRoomId t;
    LibJingleEx::ELibjingleError e = LibJingleEx::GetRoomId( m_pChatRoom->m_hRoom, &t );
    if ( 0 != e )
    {
        MessageBox("Failed to LibJingleEx::GetRoomId()");
        return;
    }

    e = LibJingleEx::UnScreenChatroomsMsg( h, &t, 1 );
    if ( 0 !=  e )
    {
        MessageBox("Failed to LibJingleEx::UnScreenChatroomsMsg()! ");
        return;
    }
}