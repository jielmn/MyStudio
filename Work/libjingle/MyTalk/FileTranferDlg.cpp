// FileTranferDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyTalk.h"
#include "ProgressDlg.h"
#include "EmptyDlg.h"
#include "FileTranferDlg.h"

#include "ProgressDlg.h"

static const DWORD PROGRESS_HEIGHT = 64;

// CFileTranferDlg 对话框

IMPLEMENT_DYNAMIC(CFileTranferDlg, CDialog)

CFileTranferDlg::CFileTranferDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFileTranferDlg::IDD, pParent)
{
    m_nIndex = -1;
}   

CFileTranferDlg::~CFileTranferDlg()
{
}

void CFileTranferDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_SEND, m_lstSend);
}


BEGIN_MESSAGE_MAP(CFileTranferDlg, CDialog)
    ON_BN_CLICKED(IDC_BTN_ADD_FILE, &CFileTranferDlg::OnBnClickedBtnAddFile)
    ON_BN_CLICKED(IDC_BTN_ADD_FOLDER, &CFileTranferDlg::OnBnClickedBtnAddFolder)
    ON_BN_CLICKED(IDC_BTN_CANCEL, &CFileTranferDlg::OnBnClickedBtnCancel)
    ON_BN_CLICKED(IDC_BTN_SEND, &CFileTranferDlg::OnBnClickedBtnSend)
    ON_WM_DESTROY()
    ON_MESSAGE( UM_CLOSE_PROGRESS, OnCloseProgress )
    ON_BN_CLICKED(IDC_BTN_DELETE, &CFileTranferDlg::OnBnClickedBtnDelete)
END_MESSAGE_MAP()


BOOL CFileTranferDlg::OnInitDialog()
{
    CDialog::OnInitDialog();


    if ( !m_ScrollWnd.Create( 1, this, FALSE ) )
    {
        MessageBox("error");
    }

    if ( !m_EmptyDlg.Create( IDD_EMPTY, &m_ScrollWnd ) )
    {
        MessageBox("error");
    }

    m_ScrollWnd.AddChild( &m_EmptyDlg );

    CRect rect;
    rect.left  = 20;
    rect.right = rect.left + 555;
    rect.top   = 200;
    rect.bottom = rect.top + 225;

    m_ScrollWnd.MoveWindow( rect.left, rect.top, rect.Width(), rect.Height() );
    m_ScrollWnd.ShowWindow( SW_SHOW );

    CString sTmp;
    sTmp.Format( "%s和%s的文件传输", 
                 GetFullJid( m_tSelfJid.sNode, m_tSelfJid.sDomain, m_tSelfJid.sResource ).c_str(),
                 GetFullJid( m_tPeerJid.sNode, m_tPeerJid.sDomain, m_tPeerJid.sResource ).c_str() );

    this->SetWindowText( sTmp );

    //ModifyStyleEx(0, WS_EX_APPWINDOW);//任务栏显示
    return TRUE;
}

void CFileTranferDlg::Add( LibJingleEx::HSession hSession, LibJingleEx::PFileShareManifest pManifest, BOOL bSender )
{
    CProgressDlg * p = new CProgressDlg;
    p->m_hSession = hSession;
    memcpy( &p->m_manifest, pManifest, sizeof(LibJingleEx::FileShareManifest) );
    p->m_bSender = bSender;
    p->m_nIndex = m_nIndex;
    p->m_bStoped = FALSE;

    p->Create( IDD_PROGRESS, &m_EmptyDlg );

    DWORD dwCnt = m_vProgressDlgs.size();
    DWORD dwTop = PROGRESS_HEIGHT * dwCnt;
    
    CRect r;
    p->GetWindowRect( r );

    p->MoveWindow( 0, dwTop, r.Width(), r.Height() );

    p->ShowWindow( SW_SHOW );

    m_vProgressDlgs.push_back( p );

    m_ScrollWnd.ChangeChildSize( r.Width(), dwTop + PROGRESS_HEIGHT );
    m_ScrollWnd.GetWindowRect( r );
    m_ScrollWnd.Resize( r.Width(), r.Height() );
    m_ScrollWnd.UpdateScrollBar();
}

void CFileTranferDlg::Del( LibJingleEx::HSession hSession )
{
    vProgressDlg::iterator it;
    vProgressDlg & v = m_vProgressDlgs;

    CProgressDlg * p = 0;
    for ( it = v.begin(); it != v.end(); it++ )
    {
        p = *it;
        if ( p->m_hSession == hSession )
        {
            break;
        }
    }

    ASSERT( it != v.end() && p );

    CRect rt;
    p->GetWindowRect( rt );

    p->DestroyWindow();
    delete p;
    
    vProgressDlg::iterator it_a;

    for ( it_a = it + 1; it_a != v.end(); it_a++ )
    {
        p = *it_a;

        CRect r;
        p->GetWindowRect( r );
        m_EmptyDlg.ScreenToClient( r );
        p->MoveWindow( r.left, r.top - PROGRESS_HEIGHT, r.Width(), r.Height() );
    }

    v.erase( it );

    m_ScrollWnd.ChangeChildSize( rt.Width(), PROGRESS_HEIGHT * v.size() );
    m_ScrollWnd.GetWindowRect( rt );
    m_ScrollWnd.Resize( rt.Width(), rt.Height() );
    m_ScrollWnd.UpdateScrollBar();
}

CProgressDlg * CFileTranferDlg::IsContain( LibJingleEx::HSession hSession, BOOL bStoped )
{
    vProgressDlg::iterator it;
    vProgressDlg & v = m_vProgressDlgs;

    for ( it = v.begin(); it != v.end(); it++ )
    {
        CProgressDlg * p = *it;
        if ( p->m_hSession == hSession && p->m_bStoped == bStoped )
        {
            return p;
        }
    }

    return 0;
}

void CFileTranferDlg::OnDestroy()
{
    vProgressDlg::iterator it;
    for ( it = m_vProgressDlgs.begin(); it != m_vProgressDlgs.end(); it++ )
    {
        CProgressDlg * p = *it;
        p->DestroyWindow();
        delete p;
    }
    m_vProgressDlgs.clear();

    CDialog::OnDestroy();
}

LRESULT CFileTranferDlg::OnCloseProgress( WPARAM wParam, LPARAM lParam )
{
    LibJingleEx::HSession hSession = (LibJingleEx::HSession)wParam;

    Del( hSession );

    return 0;
}

// CFileTranferDlg 消息处理程序

void CFileTranferDlg::OnBnClickedBtnAddFile()
{
    if ( m_lstSend.GetCount() > 0 )
    {
        MessageBox("一次只能传送一个文件或文件夹!");
        return;
    }

    // TODO: 在此添加控件通知处理程序代码
    CFileDialog dlg(TRUE);
    if ( IDOK == dlg.DoModal() )
    {
        int n = m_lstSend.GetCount();
        m_lstSend.InsertString( n, dlg.GetPathName() );
        m_lstSend.SetItemData( n, LibJingleEx::FileShareManifest::T_FILE );
    }
}

void CFileTranferDlg::OnBnClickedBtnAddFolder()
{
    if ( m_lstSend.GetCount() > 0 )
    {
        MessageBox("一次只能传送一个文件或文件夹!");
        return;
    }

    // TODO: 在此添加控件通知处理程序代码
    LPBROWSEINFO lpbi=new BROWSEINFO;
    lpbi->hwndOwner=GetSafeHwnd();
    lpbi->pidlRoot=NULL;
    lpbi->pszDisplayName=NULL;
    lpbi->lpszTitle="选择目录";
    lpbi->ulFlags=BIF_RETURNONLYFSDIRS|BIF_STATUSTEXT;
    lpbi->lpfn=NULL;

    LPITEMIDLIST lpitemidlist=SHBrowseForFolder(lpbi);
    if(lpitemidlist==NULL)
    {
        delete lpbi;
        lpbi=NULL;
        return;
    }

    char path[MAX_PATH];
    if ( SHGetPathFromIDList(lpitemidlist,path) )
    {
        int n = m_lstSend.GetCount();
        m_lstSend.InsertString( n, path);
        m_lstSend.SetItemData( n, LibJingleEx::FileShareManifest::T_FOLDER );
    }

    delete lpbi;


}

void CFileTranferDlg::OnBnClickedBtnCancel()
{
    // TODO: 在此添加控件通知处理程序代码
}

void CFileTranferDlg::OnBnClickedBtnSend()
{
    // TODO: 在此添加控件通知处理程序代码
    if ( 0 == m_lstSend.GetCount() )
    {
        MessageBox("没有选择文件或文件夹!");
        return;
    }

    ASSERT( 1 == m_lstSend.GetCount() );


    LibJingleEx::FileShareManifest m;
    
    int nType = m_lstSend.GetItemData(0);
    CString s;
    m_lstSend.GetText( 0, s );

    CString sName;
    CString sDir;

    int n = s.ReverseFind( '\\' );
    if ( n >= 0 )
    {
        sName = s.Right( s.GetLength() - (n+1) );
        sDir  = s.Left( n );
    }
    else
    {
        sName = s;
        sDir  = "";
    }

    ASSERT( sDir.GetLength() > 0 );

    if ( nType == LibJingleEx::FileShareManifest::T_FILE )
    {
        m.AddFile( sName, LibJingleEx::GetFileSize(s) );
    }
    else
    {
        m.AddFolder( sName, LibJingleEx::GetDirSize(s) );
    }

    LibJingleEx::HSession hSession = 0;

    LibJingleEx::ELibjingleError e = LibJingleEx::CreateSession( g_hXmppApp[m_nIndex], &hSession, &m_tPeerJid, &m, sDir );
    if ( 0 != e )
    {
        MessageBox("Failed to LibJingleEx::CreateSession()!");
        return;
    }

    m_lstSend.ResetContent();
}


void CFileTranferDlg::OnBnClickedBtnDelete()
{
    // TODO: 在此添加控件通知处理程序代码
    int nSel = m_lstSend.GetCurSel();
    if ( nSel > 0 )
    {
        m_lstSend.DeleteString( nSel );
    }
}
