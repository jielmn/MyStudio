#include "stdafx.h"

#include "MyTalk.h"
#include "PictureDlg.h"


// CChatDlg 对话框

IMPLEMENT_DYNAMIC(CPictureDlg, CDialog)

CPictureDlg::CPictureDlg(CWnd* pParent /*=NULL*/)
: CDialog(CPictureDlg::IDD, pParent), m_nIndex(-1)
, m_sPath(_T(""))
{

}

CPictureDlg::~CPictureDlg()
{

}

void CPictureDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_PICTURE_PATH, m_sPath);
}

BEGIN_MESSAGE_MAP(CPictureDlg, CDialog)

    ON_BN_CLICKED(IDC_BTN_BROWSE, &CPictureDlg::OnBnClickedBtnBrowse)
    ON_BN_CLICKED(IDC_BTN_SEND, &CPictureDlg::OnBnClickedBtnSend)
END_MESSAGE_MAP()

void CPictureDlg::OnBnClickedBtnBrowse()
{
    // TODO: 在此添加控件通知处理程序代码
    CFileDialog dlg(TRUE, 0, 0, 0, "*.jpg|*.jpg|*.gif|*.gif|*.bmp|*.bmp||" );
    if ( IDOK == dlg.DoModal() )
    {
        m_sPath = dlg.GetFileName();
        UpdateData( FALSE );
    }
}

void CPictureDlg::OnBnClickedBtnSend()
{
    // TODO: 在此添加控件通知处理程序代码
    UpdateData( TRUE );

    if ( 0 == m_sPath.GetLength() )
    {
        return;
    }

    WCHAR wszStr[512] = { 0 };
    int nCount = MultiByteToWideChar( CP_ACP, 0, m_sPath, m_sPath.GetLength(), wszStr, 512 - 1 );

    FILE * fp = _wfopen( wszStr, L"rb" );

    fseek( fp, 0, SEEK_END );
    int nSize = ftell( fp );
    fseek( fp, 0, SEEK_SET );

    unsigned char * p = new unsigned char[nSize];
    fread( p, 1, nSize, fp );

    fclose(fp);

    LibJingleEx::ELibjingleError e = LibJingleEx::SendPicture( g_hXmppApp[m_nIndex], &m_tJid, p, nSize );
    if ( 0 != e )
    {
        MessageBox( "Failed to LibJingleEx::SendPicture()! " );
        return;
    }
}


void CPictureDlg::UpdatePicture( BYTE * pPicture, DWORD dwSize )
{

}