// ChatDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyTalk.h"
#include "ChatDlg.h"


// CChatDlg 对话框

IMPLEMENT_DYNAMIC(CChatDlg, CDialog)

CChatDlg::CChatDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChatDlg::IDD, pParent), m_nIndex(-1)
{
    
}

CChatDlg::~CChatDlg()
{
}

void CChatDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_MSG, m_lstMsg);
    DDX_Control(pDX, IDC_EDIT_MSG, m_editMsg);
    DDX_Control(pDX, IDC_BTN_SEND, m_btnSend);
    DDX_Control(pDX, IDC_STATIC1, m_CharState);
}


BEGIN_MESSAGE_MAP(CChatDlg, CDialog)
    ON_BN_CLICKED(IDC_BTN_SEND, &CChatDlg::OnBnClickedBtnSend)
    ON_WM_SIZE()
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CChatDlg 消息处理程序

BOOL CChatDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    if ( 0 == m_sNick.GetLength() )
    {
        m_sNick = GetFullJid( m_tJid.sNode, m_tJid.sDomain, m_tJid.sResource ).c_str();
    }

    CString s;
    LibJingleEx::TLoginSettings & t = g_tAccounts[m_nIndex];
    s.Format( "%s和%s的对话", GetFullJid( t.sUserName, t.sDomain, t.sResource ).c_str(), m_sNick );
    

    this->SetWindowText( s );

    this->SetIcon( AfxGetApp()->LoadIcon( IDR_MAINFRAME ), TRUE );

    m_eCharState = LibJingleEx::CHAR_STATUS_ACTIVE;
    m_dwComposingTick = 0;
    SetTimer( 1, 1000, 0 );
    return TRUE;
}

void CChatDlg::OnBnClickedBtnSend()
{
    // TODO: 在此添加控件通知处理程序代码
    CString s;
    m_editMsg.GetWindowText( s );

    if ( s.Right(1) == "\n" )
    {
        int n = s.GetLength();
        if ( n >= 2 )
        {
            s = s.Left( n - 2 );
        }
    }

    if ( 0 == s.GetLength() )
    {
        return;
    }

    ASSERT( m_nIndex < MAX_ACCOUNT_NUMBER && m_nIndex >= 0 );

    LibJingleEx::TDmsChatContent tMsg;
    // strcpy( (char *)tMsg.m_abyFontBuf, "123" );
    // strcpy( (char *)tMsg.m_abyTimeBuf, "456" );
    tMsg.m_dwTime = 1343099939;
    
    tMsg.m_dwChatType = 5;
    tMsg.m_dwColor    = 333;
    tMsg.m_dwPicCount = 2;
    strcpy( tMsg.m_szContent, s );
    strcpy( tMsg.m_szMsgHeader, "僵局123" );
    tMsg.m_atPicFileInfoTable[0].m_dwIndex = 100;
    strcpy( tMsg.m_atPicFileInfoTable[0].m_achFileName, "描述11" );

    LibJingleEx::ELibjingleError e = LibJingleEx::SendMsg( g_hXmppApp[m_nIndex], &m_tJid , &tMsg );
    if ( 0 != e )
    {
        MessageBox("Failed to LibJingleEx::SendMsg()!");
    }
    else
    {
        int nCnt = m_lstMsg.GetCount();
        CString t;
        t.Format( "你说: %s", s );

        m_lstMsg.InsertString( nCnt, t );
        m_lstMsg.SetCurSel( nCnt );

        m_editMsg.SetWindowText("");

        m_eCharState = LibJingleEx::CHAR_STATUS_ACTIVE;
    }
}

void CChatDlg::OnChatMessage( const char * szMsg )
{
    int nCnt = m_lstMsg.GetCount();

    CString s;
    s.Format( "%s说: %s", m_sNick, szMsg );

    m_lstMsg.InsertString( nCnt, s );
    m_lstMsg.SetCurSel( nCnt );

    m_CharState.SetWindowText("");

    UpdateData(FALSE);
    
}

void CChatDlg::OnChatState( LibJingleEx::ECharStatus e )
{
    if ( LibJingleEx::CHAR_STATUS_COMPOSING == e)
    {
        m_CharState.SetWindowText("正在输入");
    }
    else if ( LibJingleEx::CHAR_STATUS_PAUSED == e )
    {
        m_CharState.SetWindowText("");
    }
    else
    {
        m_CharState.SetWindowText("");
    }

    UpdateData( FALSE );
}

BOOL CChatDlg::PreTranslateMessage( MSG * pMsg )
{
    if ( pMsg->message == WM_KEYUP )
    {
        if ( pMsg->hwnd == m_editMsg.m_hWnd && '\r' == pMsg->wParam )
        {
            OnBnClickedBtnSend();
            return TRUE;
        }
        // 正在输入
        else if ( pMsg->hwnd == m_editMsg.m_hWnd )
        {
            if ( m_eCharState !=  LibJingleEx::CHAR_STATUS_COMPOSING )
            {
                LibJingleEx::ELibjingleError e = LibJingleEx::SendCharState( g_hXmppApp[m_nIndex], &m_tJid , LibJingleEx::CHAR_STATUS_COMPOSING );
                if ( 0 == e )
                {
                    m_eCharState = LibJingleEx::CHAR_STATUS_COMPOSING;
                    m_dwComposingTick = GetTickCount();
                }
                else
                {
                    CString s;
                    s.Format( "failed to LibJingleEx::SendCharState(), COMPOSING, eRet = %u ", e );

                    MessageBox( s );
                }
            }
            else
            {
                m_dwComposingTick = GetTickCount();
            }
            return TRUE;
        }
    }
    return CDialog::PreTranslateMessage( pMsg );
}

void CChatDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType,cx, cy);

    if ( !::IsWindow(m_lstMsg.m_hWnd) )
    {
        return;
    }

    CRect r1;
    r1.left   = 12;
    r1.top    = 12;
    r1.right  = r1.left + cx - 12 * 2;
    r1.bottom = r1.top + cy - 12 * 3 - 50;

    m_lstMsg.MoveWindow( r1 );

    CRect r2;
    r2.left   = r1.left;
    r2.top    = r1.bottom + 12;
    r2.right  = r2.left + cx - 12 * 3 - 50;
    r2.bottom = cy - 25;

    m_editMsg.MoveWindow( r2 );

    CRect r3;
    r3.left   = r2.right + 12;
    r3.top    = r2.top;
    r3.right  = r3.left + 50;
    r3.bottom = r2.bottom;

    m_btnSend.MoveWindow( r3 );


    CRect r4;
    r4.left = r2.left;
    r4.right = r3.right;
    r4.top = r2.bottom + 5;
    r4.bottom = cy -5;

    m_CharState.MoveWindow( r4 );
}


void CChatDlg::OnTimer(UINT_PTR nIDEvent)
{
    DWORD dwCurTick = GetTickCount();

    // 如果composing5秒后
    if ( m_eCharState == LibJingleEx::CHAR_STATUS_COMPOSING && dwCurTick - m_dwComposingTick >= 5000 )
    {
        LibJingleEx::ELibjingleError e = LibJingleEx::SendCharState( g_hXmppApp[m_nIndex], &m_tJid , LibJingleEx::CHAR_STATUS_PAUSED );
        if ( 0 == e )
        {
            m_eCharState = LibJingleEx::CHAR_STATUS_PAUSED;
        }
        else
        {
            MessageBox("failed to LibJingleEx::SendCharState(), PAUSED");
        }
    }
}
