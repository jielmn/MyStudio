// ProgressDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyTalk.h"
#include "ProgressDlg.h"

#pragma warning( disable: 4996 )

// CProgressDlg 对话框

IMPLEMENT_DYNAMIC(CProgressDlg, CDialog)

CProgressDlg::CProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProgressDlg::IDD, pParent)
    , m_strCurrent(_T(""))
    , m_strNum(_T(""))
{
    m_hSession = 0;
    m_bSender  = FALSE;

    m_nIndex = -1;

    m_bStoped = FALSE;

}

CProgressDlg::~CProgressDlg()
{
}

void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PROGRESS1, m_ctrProgress);
    DDX_Control(pDX, IDC_BTN_CANCEL, m_btnCancel);
    DDX_Control(pDX, IDC_BTN_ACCEPT, m_btnAccept);
    DDX_Text(pDX, IDC_STATIC_CURRENT, m_strCurrent);
    DDX_Text(pDX, IDC_STATIC_NUM, m_strNum);
    DDX_Control(pDX, IDC_BTN_DECLINE, m_btnDecline);
}


BEGIN_MESSAGE_MAP(CProgressDlg, CDialog)
    ON_BN_CLICKED(IDC_BTN_CANCEL, &CProgressDlg::OnBnClickedBtnCancel)
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_BTN_ACCEPT, &CProgressDlg::OnBnClickedBtnAccept)
    ON_BN_CLICKED(IDC_BTN_DECLINE, &CProgressDlg::OnBnClickedBtnDecline)
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

CString CProgressDlg::FormatSize( DWORD dwSize )
{
    CString sTmp;

    if ( dwSize >= 1000 * 1000 * 1000 )
    {
        DWORD a = dwSize / ( 1000 * 1000 * 1000 );
        DWORD t = dwSize % ( 1000 * 1000 * 1000 );

        DWORD b = t / ( 1000 * 1000 );
        DWORD r = t % ( 1000 * 1000 );

        DWORD c = r / 1000;
        DWORD d = r % 1000;

        sTmp.Format("%u,%u,%u,%u 字节", a, b, c, d );
    }
    else if ( dwSize >= 1000 * 1000 )
    {
        DWORD a = dwSize / ( 1000 * 1000 );
        DWORD t = dwSize % ( 1000 * 1000 );

        DWORD b = t / 1000;
        DWORD c = t % 1000;

        sTmp.Format("%u,%u,%u 字节", a, b, c);
    }
    else if ( dwSize >= 1000 )
    {
        DWORD a = dwSize / 1000;
        DWORD b = dwSize % 1000;

        sTmp.Format("%u,%u 字节", a, b);
    }
    else
    {
        sTmp.Format("%u 字节", dwSize);
    }
    return sTmp;
}

BOOL CProgressDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_ctrProgress.SetRange( 0, 100 );
    m_ctrProgress.SetPos( 0 );

    if ( m_bSender )
    {
        m_btnAccept.EnableWindow( FALSE );
        m_btnDecline.EnableWindow( FALSE );
    }
    else
    {
        m_btnCancel.EnableWindow( FALSE );
    }

    this->EnableToolTips( TRUE );

    if ( !m_tip.Create( this ) )
    {
        MessageBox("Failed to create tip control! ");
    }

    m_tip.AddTool( &m_ctrProgress, TTS_ALWAYSTIP);
    m_tip.SetDelayTime( TTDT_AUTOPOP, 10000 );
    m_tip.SetDelayTime( TTDT_INITIAL, 500 );
    m_tip.SetDelayTime( TTDT_RESHOW,  500 );

    CString sTmp;

    int i;
    for ( i = 0; i < (int)m_manifest.GetSize(); i++ )
    {
        const LibJingleEx::FileShareManifest::Item & itm = m_manifest.GetItem( i );

        if ( LibJingleEx::FileShareManifest::T_FILE == itm.m_eTyp  )
        {
            sTmp += "文件: ";
            sTmp += itm.m_szName;
            sTmp += ", 大小: ";    
            sTmp += FormatSize( itm.m_dwSize );
        }
        else if ( LibJingleEx::FileShareManifest::T_FOLDER == itm.m_eTyp )
        {
            sTmp += "文件夹: ";
            sTmp += itm.m_szName;
            sTmp += ", 大小: ";
            sTmp += FormatSize( itm.m_dwSize );
        }
        else
        {
            sTmp += "文件: ";
            sTmp += itm.m_szName;
            sTmp += ", 大小: ";    
            sTmp += FormatSize( itm.m_dwSize );
        }

        sTmp += "\r\n";
    }

    m_tip.UpdateTipText( sTmp, &m_ctrProgress );

    return TRUE;
}

BOOL CProgressDlg::PreTranslateMessage(MSG* pMsg)
{
    if( WM_MOUSEMOVE == pMsg->message && m_tip.m_hWnd != NULL ) 
    {
        m_tip.RelayEvent(pMsg);
    }
    return CDialog::PreTranslateMessage( pMsg );
}

// CProgressDlg 消息处理程序

void CProgressDlg::OnBnClickedBtnCancel()
{
    // TODO: 在此添加控件通知处理程序代码
    LibJingleEx::ELibjingleError e = LibJingleEx::CancelSession( m_hSession );
    if ( 0 != e )
    {
        MessageBox("Failed to LibJingleEx::CancelSession()! ");
        return;
    }

    m_btnCancel.EnableWindow(FALSE);
}


void CProgressDlg::UpdateProgress( DWORD dwProgress, const char * szCurFileName, DWORD dwTransferSize, DWORD dwRate )
{
    m_ctrProgress.SetPos( dwProgress );

    m_strCurrent = szCurFileName;

    CString sTmp;
    sTmp.Format(" 总共传输：%u， 速率：%0.2f K", dwTransferSize, (float)(dwRate /1024.0f) );
    
    m_strCurrent += sTmp;

    UpdateData( FALSE );
}

void CProgressDlg::OnComplete()
{
    m_btnAccept.EnableWindow( FALSE );
    m_btnDecline.EnableWindow( FALSE );
    m_btnCancel.EnableWindow( FALSE );

    m_strCurrent = "完成";
    m_ctrProgress.SetPos( 100 );

    UpdateData( FALSE );

    SetTimer( 1, 1000, 0 );

    m_bStoped = TRUE;
}

void CProgressDlg::OnTransfer()
{
    m_btnCancel.EnableWindow( TRUE );
}

void CProgressDlg::OnFileCancel()
{
    m_btnAccept.EnableWindow( FALSE );
    m_btnDecline.EnableWindow( FALSE );
    m_btnCancel.EnableWindow( FALSE );

    m_strCurrent = "取消";
    UpdateData( FALSE );

    SetTimer( 1, 1000, 0 );

    m_bStoped = TRUE;
}

void CProgressDlg::OnFailure()
{
    m_btnAccept.EnableWindow( FALSE );
    m_btnDecline.EnableWindow( FALSE );
    m_btnCancel.EnableWindow( FALSE );

    m_strCurrent = "失败";
    UpdateData( FALSE );

    SetTimer( 1, 1000, 0 );

    m_bStoped = TRUE;
}

void CProgressDlg::OnTimer( UINT_PTR nIDEvent )
{
    UpdateData( TRUE );

    if ( 0 == m_strNum.GetLength() )
    {
        m_strNum = "9";

        UpdateData( FALSE );
    }
    else
    {
        int nNum = 0;
        sscanf( m_strNum, " %d", &nNum );
        nNum--;

        m_strNum.Format( "%d", nNum );
        UpdateData( FALSE );


        // 关闭
        if ( 0 == nNum )
        {
            KillTimer( 1 );

            CWnd * pParent = GetParent();
            ASSERT( pParent );

            pParent = pParent->GetParent();
            ASSERT( pParent );

            pParent = pParent->GetParent();
            ASSERT( pParent );

            pParent = pParent->GetParent();
            ASSERT( pParent );

            ::PostMessage( pParent->m_hWnd, UM_CLOSE_PROGRESS, (WPARAM)m_hSession, 0 );
        }
    }
}

void CProgressDlg::OnBnClickedBtnAccept()
{
    // TODO: 在此添加控件通知处理程序代码

    LibJingleEx::ELibjingleError e = LibJingleEx::AcceptSession( m_hSession );
    if ( 0 != e )
    {
        MessageBox("Failed to LibJingleEx::AcceptSession()! ");
        return;
    }

    m_btnAccept.EnableWindow(FALSE);
    m_btnDecline.EnableWindow( FALSE );
}

void CProgressDlg::OnBnClickedBtnDecline()
{
    // TODO: 在此添加控件通知处理程序代码
    LibJingleEx::ELibjingleError e = LibJingleEx::DeclineSession( m_hSession );
    if ( 0 != e )
    {
        MessageBox("Failed to LibJingleEx::DeclineSession()! ");
        return;
    }

    m_btnAccept.EnableWindow(FALSE);
    m_btnDecline.EnableWindow( FALSE );
}

BOOL CProgressDlg::OnEraseBkgnd(CDC* pDC)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    return TRUE;
    return CDialog::OnEraseBkgnd(pDC);
}
