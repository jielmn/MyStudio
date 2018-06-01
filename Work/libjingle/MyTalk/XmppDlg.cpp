// XmppDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyTalk.h"
#include "XmppDlg.h"


// CXmppDlg 对话框

IMPLEMENT_DYNAMIC(CXmppDlg, CDialog)

CXmppDlg::CXmppDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CXmppDlg::IDD, pParent)
{
    m_bChanged = FALSE;
}

CXmppDlg::~CXmppDlg()
{
}

void CXmppDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_MSG, m_editMsg);
    DDX_Control(pDX, IDC_EDIT_CUSTOM_MSG, m_editCustome);
}


BEGIN_MESSAGE_MAP(CXmppDlg, CDialog)
    ON_WM_CTLCOLOR()
    ON_WM_SIZE()
    ON_MESSAGE( UM_XMPP_MSG, OnXmppMsg )
    ON_MESSAGE( UM_DEBUG_MSG, OnDebugInfo )
    ON_WM_DESTROY()
    ON_WM_CLOSE()
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CXmppDlg 消息处理程序
BOOL CXmppDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_editMsg.SetLimitText(0xFFFFFFFF);

    
    this->SetIcon( AfxGetMainWnd()->GetIcon(TRUE), TRUE );


    SetTimer( 1, 200, 0 );

    return TRUE;
}

HBRUSH   CXmppDlg::OnCtlColor(CDC*   pDC,   CWnd*   pWnd,   UINT   nCtlColor)
{
    HBRUSH   hbr   =   CDialog::OnCtlColor(pDC,   pWnd,   nCtlColor);

    if(pWnd-> GetDlgCtrlID()   ==   IDC_EDIT_MSG)
    {
        HBRUSH   hbru   =   ::CreateSolidBrush   (RGB(255,255,255));
        pDC-> SetTextColor(RGB(0,0,0));         //设置字体颜色
        pDC-> SetBkColor(RGB(255,255,255));     //设置背景色
        return   hbru;                          //这句很重要!
    }



    return   hbr; 
}

void CXmppDlg::OnOK()
{

}

const UINT MAX_VOLUME = 1024 * 16;

LRESULT CXmppDlg::OnXmppMsg( WPARAM wParam, LPARAM lParam )
{
    char * pMsg   = (char *)wParam;
    DWORD  dwTmp  = (DWORD)lParam;

    DWORD  dwSize = dwTmp & 0x0000FFFF;
    BOOL   bInput = ( dwTmp >> 16 );

    if ( bInput )
    {
        m_content += "<<<<<<<<<<<<<<<<<<<<<<<\r\n";
    }
    else
    {
        m_content += ">>>>>>>>>>>>>>>>>>>>>>>\r\n";
    }

    m_content += pMsg;
    m_content += "\r\n\r\n";

    if ( m_content.GetLength() > MAX_VOLUME )
    {
        m_content = m_content.Right(MAX_VOLUME);
    }

    //m_editMsg.SetWindowText( m_content );
    //m_editMsg.SetSel( m_content.GetLength(), m_content.GetLength() );

    delete[] pMsg;

    m_bChanged = TRUE;

    return 0;
}

LRESULT CXmppDlg::OnDebugInfo( WPARAM wParam, LPARAM lParam )
{
    char * pMsg   = (char *)wParam;
   
    m_content += pMsg;

    if ( m_content.GetLength() > MAX_VOLUME )
    {
        m_content = m_content.Right(MAX_VOLUME);
    }

    //m_editMsg.SetWindowText( m_content );
    //m_editMsg.SetSel( m_content.GetLength(), m_content.GetLength() );

    delete[] pMsg;

    m_bChanged = TRUE;

    return 0;
}

void CXmppDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy );

    if ( IsWindow(m_editMsg.m_hWnd) )
    {
        CRect r1( 12, 12, cx-12, cy - 50 );

        m_editMsg.MoveWindow( r1 );

        CRect r2;
        r2.left    = r1.left;
        r2.right   = r1.right;
        r2.top     = r1.bottom + 12;
        r2.bottom  = r2.top + 20;

        ASSERT( IsWindow(m_editCustome.m_hWnd) );

        m_editCustome.MoveWindow( r2 );
    }
}

void CXmppDlg::OnDestroy()
{
    CDialog::OnDestroy();
}

void CXmppDlg::OnClose()
{
    CDialog::OnClose();
    SetXmppConsole( 0 );
}

BOOL CXmppDlg::OnEraseBkgnd(CDC* pDC)
{
    return CDialog::OnEraseBkgnd( pDC );
}

void CXmppDlg::OnPaint()
{
    CDialog::OnPaint();
}

void CXmppDlg::OnTimer(UINT_PTR nIDEvent)
{
    if ( m_bChanged )
    {
        m_editMsg.SetWindowText( m_content );
        m_editMsg.SetSel( m_content.GetLength(), m_content.GetLength() );

        m_bChanged = FALSE;
    }
}