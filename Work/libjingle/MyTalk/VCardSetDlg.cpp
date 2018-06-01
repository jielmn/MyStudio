// VCardSetDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyTalk.h"
#include "VCardSetDlg.h"


#define PIC_LEFT            140
#define PIC_TOP             100
// CVCardSetDlg 对话框

IMPLEMENT_DYNAMIC(CVCardSetDlg, CDialog)

CVCardSetDlg::CVCardSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVCardSetDlg::IDD, pParent)
    , m_sName(_T(""))
{
    m_nIndex   = 0;
    m_pPicture = 0;
    m_bSelf    = TRUE;
}

CVCardSetDlg::~CVCardSetDlg()
{
}

void CVCardSetDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_NAME, m_sName);
    DDX_Control(pDX, IDC_COMBO_SYS_PIC_INDEX, m_cmbSysPicIndex);
}


BEGIN_MESSAGE_MAP(CVCardSetDlg, CDialog)
    ON_WM_PAINT()
    ON_BN_CLICKED(IDOK, &CVCardSetDlg::OnBnClickedOk)
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_BUTTON_BROWN_PIC, &CVCardSetDlg::OnBnClickedButtonBrownPic)
    ON_CBN_SELCHANGE(IDC_COMBO_SYS_PIC_INDEX, &CVCardSetDlg::OnCbnSelchangeComboSysPicIndex)
END_MESSAGE_MAP()


// CVCardSetDlg 消息处理程序

BOOL  CVCardSetDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_cmbSysPicIndex.InsertString( 0, "无" );

    int i;
    for ( i = 0; i < 10; i++ )
    {
        CString   sTmp;
        sTmp.Format( "%u", i+1 );
        m_cmbSysPicIndex.InsertString( i + 1, sTmp );
        m_cmbSysPicIndex.SetItemData( i + 1, i + 1 );
    }
    m_cmbSysPicIndex.SetCurSel( 0 );

    m_sName = m_tVCard.m_achNickName;
    if ( !m_tVCard.m_bCustomHeadPortrait )
    {
        if ( m_tVCard.m_nSysHeadPortraitIndex > 0 )
        {
            m_cmbSysPicIndex.SetCurSel( m_tVCard.m_nSysHeadPortraitIndex );
        }
    }
    else
    {
        LoadFromBuffer( m_tVCard.m_abyCumtomHeadPortrait, m_tVCard.m_dwCustomHeadPortraitSize, m_pPicture );
    }

    GetDlgItem(IDC_STATIC_PIC)->MoveWindow( PIC_LEFT, PIC_TOP, 32, 32 );
    UpdateData( FALSE );

    if ( !m_bSelf )
    {
        GetDlgItem(IDC_EDIT_NAME)->EnableWindow( FALSE );
        m_cmbSysPicIndex.EnableWindow( FALSE );
        GetDlgItem(IDC_BUTTON_BROWN_PIC)->EnableWindow( FALSE );
        GetDlgItem(IDOK)->EnableWindow( FALSE );
        // GetDlgItem(IDCANCEL)->EnableWindow( FALSE );

        SetWindowText("查看好友信息");
    }
    return TRUE;
}


void CVCardSetDlg::OnPaint()
{
    CPaintDC dc(this);

    if ( m_pPicture )
    {
        CRect r( PIC_LEFT, PIC_TOP, PIC_LEFT+32, PIC_TOP+32 );
        Draw2Dc( m_pPicture, &dc, &r );
    }
}
void CVCardSetDlg::OnBnClickedOk()
{
    // TODO: 在此添加控件通知处理程序代码
    UpdateData( TRUE );
    
    strncpy( m_tVCard.m_achNickName, m_sName, LibJingleEx::VCARD_MAX_NICK_LEN );

    m_tVCard.m_bCustomHeadPortrait = FALSE;

    int nCurSel = m_cmbSysPicIndex.GetCurSel();
    if ( nCurSel > 0 )
    {
        m_tVCard.m_bCustomHeadPortrait = FALSE;
        m_tVCard.m_nSysHeadPortraitIndex = m_cmbSysPicIndex.GetItemData( nCurSel );
    }
    else if ( m_pPicture )
    {
        // BYTE  abyData[LibJingleEx::VCARD_MAX_HEAD_PORTRAIT_SIZE];
        DWORD dwSize = LibJingleEx::VCARD_MAX_HEAD_PORTRAIT_SIZE;

        CRect r(0, 0, 32, 32 );
        if ( 0 != GetDataFromPicture( m_tVCard.m_abyCumtomHeadPortrait, dwSize, m_pPicture, &r ) )
        {
            m_tVCard.m_bCustomHeadPortrait = FALSE;
            m_tVCard.m_dwCustomHeadPortraitSize = 0;
        }
        else
        {
            m_tVCard.m_bCustomHeadPortrait      = TRUE; 
            m_tVCard.m_eCustomHeadPortraitType  = IMAGE_TYPE_UNKNOWN;
            m_tVCard.m_dwCustomHeadPortraitSize = dwSize;
        }
    }

    LibJingleEx::ELibjingleError eRet = LibJingleEx::SetVCard( g_hXmppApp[m_nIndex], &m_tVCard );
    if ( 0 != eRet )
    {
        MessageBox("Failed to LibJingleEx::SetVCard()!");
        return;
    }
    

    OnOK();
}

void CVCardSetDlg::OnDestroy()
{
    CDialog::OnDestroy();

    // TODO: 在此处添加消息处理程序代码
    if ( m_pPicture )
    {
        m_pPicture->Release();
        m_pPicture = 0;
    }
}

void CVCardSetDlg::OnBnClickedButtonBrownPic()
{
    // TODO: 在此添加控件通知处理程序代码
    CFileDialog dlg(TRUE, 0, 0, 0, "*.jpg|*.jpg|*.gif|*.gif|*.bmp|*.bmp||" );
    if ( IDOK == dlg.DoModal() )
    {
        FILE * fp;
        WCHAR wszStr[512] = { 0 };

        int nCount = MultiByteToWideChar( CP_ACP, 0, dlg.GetPathName(), strlen(dlg.GetPathName()), wszStr, 512 - 1 );
        fp = _wfopen( wszStr, L"rb" );
        
        fseek( fp, 0, 2 );
        DWORD dwSize = ftell( fp );
        fseek( fp, 0, 0 );

        BYTE * pbyData = new BYTE[dwSize];
        fread( pbyData, 1, dwSize, fp );
        fclose( fp );

        if ( m_pPicture )
        {
            m_pPicture->Release();
            m_pPicture = 0;
        }

        LoadFromBuffer( pbyData, dwSize, m_pPicture );
        Invalidate();

        delete[] pbyData;

        m_cmbSysPicIndex.SetCurSel( 0 );
    }
}

void CVCardSetDlg::OnCbnSelchangeComboSysPicIndex()
{
    // TODO: 在此添加控件通知处理程序代码
    if ( m_pPicture )
    {
        m_pPicture->Release();
        m_pPicture = 0;

        Invalidate();
    }
}
