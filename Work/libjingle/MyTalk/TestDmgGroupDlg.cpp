// TestDmgGroupDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyTalk.h"
#include "TestDmgGroupDlg.h"
#include "TestDmgAddGroupDlg.h"
#include "TestDmsAddMemberDlg.h"

#pragma warning(disable: 4996)

// CTestDmgGroupDlg 对话框

IMPLEMENT_DYNAMIC(CTestDmgGroupDlg, CDialog)

CTestDmgGroupDlg::CTestDmgGroupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestDmgGroupDlg::IDD, pParent), m_nIndex( -1 )
    , m_sMoveGroup(_T(""))
    , m_sMoveGroupParent(_T(""))
    , m_sMoveMember(_T(""))
    , m_sMoveMemberParent(_T(""))
{

}

CTestDmgGroupDlg::~CTestDmgGroupDlg()
{
}

void CTestDmgGroupDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TREE_ROSTER, m_treeRoster);
    DDX_Text(pDX, IDC_EDIT_MOVE_GROUP, m_sMoveGroup);
    DDX_Text(pDX, IDC_EDIT_MOVE_GROUP_PARENT, m_sMoveGroupParent);
    DDX_Text(pDX, IDC_EDIT_MOVE_MEMBER, m_sMoveMember);
    DDX_Text(pDX, IDC_EDIT_MOVE_MEMBER_PARENT, m_sMoveMemberParent);
}


BEGIN_MESSAGE_MAP(CTestDmgGroupDlg, CDialog)
    ON_NOTIFY(NM_RCLICK, IDC_TREE_ROSTER, &CTestDmgGroupDlg::OnNMRClickTreeRoster)
    ON_COMMAND( ID_DMS_ADD_GROUP,    OnDmsAddGroup )
    ON_COMMAND( ID_DMS_DEL_GROUP,    OnDmsDelGroup )
    ON_COMMAND( ID_DMS_MODIFY_GROUP, OnDmsModifyGroup )
    ON_COMMAND( ID_DMS_ADD_MEMBER,    OnDmsAddMember )
    ON_COMMAND( ID_DMS_DEL_MEMBER,    OnDmsDelMember )
    ON_COMMAND( ID_DMS_MODIFY_MEMBER,    OnDmsModifyMember )
    ON_MESSAGE( UM_ON_DMS_ADD_GROUP,  OnNotifyDmsAddGroup )
    ON_MESSAGE( UM_ON_DMS_DEL_GROUP,  OnNotifyDmsDelGroup )
    ON_MESSAGE( UM_ON_DMS_MODIFY_GROUP, OnNotifyDmsModifyGroup )
    ON_MESSAGE( UM_ON_DMS_ADD_MEMBER, OnNotifyDmsAddMember )
    ON_MESSAGE( UM_ON_DMS_MODIFY_MEMBER, OnNotifyDmsModifyMember )
    ON_MESSAGE( UM_ON_DMS_DEL_MEMBER, OnNotifyDmsDelMember )
    ON_MESSAGE( UM_ON_DMS_MOVE_MEMBER, OnNotifyDmsMoveMember )
    ON_BN_CLICKED(IDC_BUTTON_MOVE_GROUP, &CTestDmgGroupDlg::OnBnClickedButtonMoveGroup)
    ON_BN_CLICKED(IDC_BUTTON_MOVE_MEMBER, &CTestDmgGroupDlg::OnBnClickedButtonMoveMember)
    ON_WM_DESTROY()
END_MESSAGE_MAP()


// CTestDmgGroupDlg 消息处理程序
void CTestDmgGroupDlg::GetMembers ( HTREEITEM hParentItem )
{
    LibJingleEx::HANDLE hApp = g_hXmppApp[m_nIndex];

    PTItemData pData = (PTItemData)m_treeRoster.GetItemData( hParentItem );
    ASSERT( 0 == pData->dwType );

    LibJingleEx::TDmsMember atMembers[1024];
    DWORD   dwSize = 1024;

    LibJingleEx::ELibjingleError e = LibJingleEx::DmsQueryMembers( hApp, pData->dwId, atMembers, &dwSize );
    if ( 0 != e )
    {
        MessageBox( "Failed to LibJingleEx::DmsQueryMembers()! " );
        return ;
    }

    DWORD  i;
    for ( i = 0; i < dwSize; i++ )
    {
        HTREEITEM hItem = m_treeRoster.InsertItem( atMembers[i].szName, hParentItem );
        m_treeRoster.SetItemImage( hItem, 1, 1 );

        PTItemData pData = new TItemData;
        pData->dwId   = atMembers[i].dwMemberId;
        pData->dwType = 1;

        m_treeRoster.SetItemData( hItem, (DWORD_PTR)pData );
    }
}

BOOL CTestDmgGroupDlg::GetSubGroup( HTREEITEM hParentItem )
{
    LibJingleEx::HANDLE hApp = g_hXmppApp[m_nIndex];

    PTItemData pData = (PTItemData)m_treeRoster.GetItemData( hParentItem );
    ASSERT( 0 == pData->dwType );

    LibJingleEx::TDmsGroup atGroups[128];
    DWORD   dwSize = 128;

    LibJingleEx::ELibjingleError e = LibJingleEx::DmsQuerySubGroups( hApp, pData->dwId, atGroups, &dwSize );
    if ( 0 != e )
    {
        // MessageBox( "Failed to LibJingleEx::DmsQuerySubGroups()! " );
        return FALSE;
    }

    DWORD  i;
    for ( i = 0; i < dwSize; i++ )
    {
        HTREEITEM hItem = m_treeRoster.InsertItem( atGroups[i].szGroupName, hParentItem );
        m_treeRoster.SetItemImage( hItem, 0, 0 );

        PTItemData pData = new TItemData;
        pData->dwId = atGroups[i].dwGroupId;
        pData->dwType = 0;

        m_treeRoster.SetItemData( hItem, (DWORD_PTR)pData );

        GetSubGroup( hItem );
        GetMembers( hItem );

        m_treeRoster.Expand( hItem, TVE_EXPAND );
    }

    return TRUE;
}

BOOL CTestDmgGroupDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_ImgList.Create( 16, 16, ILC_COLOR24, 16, 0 );

    CBitmap bm[2];

    bm[0].LoadBitmap(IDB_FOLDER);
    m_ImgList.Add( &bm[0], RGB(0, 0, 0) );

    bm[1].LoadBitmap(IDB_AVAILABLE);
    m_ImgList.Add( &bm[1], RGB(0, 0, 0) );

    m_treeRoster.SetImageList( &m_ImgList, TVSIL_NORMAL );



    LibJingleEx::HANDLE hApp = g_hXmppApp[m_nIndex];

    LibJingleEx::TDmsGroup atGroups[128];
    DWORD   dwSize = 128;

    LibJingleEx::ELibjingleError e = LibJingleEx::DmsQuerySubGroups( hApp, 0, atGroups, &dwSize );
    if ( 0 != e )
    {
        MessageBox( "Failed to LibJingleEx::DmsQuerySubGroups()! " );
        return TRUE;
    }

    DWORD  i;
    for ( i = 0; i < dwSize; i++ )
    {
        HTREEITEM hItem = m_treeRoster.InsertItem( atGroups[i].szGroupName );
        m_treeRoster.SetItemImage( hItem, 0, 0 );

        PTItemData pData = new TItemData;
        pData->dwId = atGroups[i].dwGroupId;

        m_treeRoster.SetItemData( hItem, (DWORD_PTR)pData );

        GetSubGroup( hItem );
        GetMembers(  hItem );

        m_treeRoster.Expand( hItem, TVE_EXPAND );
    }
    

    return TRUE;
}

void CTestDmgGroupDlg::OnNMRClickTreeRoster(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: 在此添加控件通知处理程序代码
    *pResult = 0;

    CPoint   pt;
    CPoint   pt_client;

    GetCursorPos( &pt);

    pt_client = pt;
    m_treeRoster.ScreenToClient( &pt_client );

    TVHITTESTINFO t;
    t.pt = pt_client;
    m_treeRoster.HitTest( &t );

    if ( t.hItem )
    {
        HTREEITEM hItem = t.hItem;
        m_treeRoster.SelectItem( hItem );

        PTItemData pData = (PTItemData)m_treeRoster.GetItemData( hItem );

        // 组
        if ( 0 == pData->dwType )
        {
            CMenu   menu;
            menu.LoadMenu(IDR_MENU_DMS_GROUP);
            CMenu   *pSubMenu   =   menu.GetSubMenu(0);
            pSubMenu->TrackPopupMenu(TPM_LEFTALIGN,pt.x,   pt.y,   this); 
        }
        // 成员
        else
        {
            CMenu   menu;
            menu.LoadMenu(IDR_MENU_DMS_MEMBER);
            CMenu   *pSubMenu   =   menu.GetSubMenu(0);
            pSubMenu->TrackPopupMenu(TPM_LEFTALIGN,pt.x,   pt.y,   this); 
        }
    }
    else
    {
        m_treeRoster.SelectItem( 0 );

        CMenu   menu;
        menu.LoadMenu(IDR_MENU_DMS_TOP_GROUP);
        CMenu   *pSubMenu   =   menu.GetSubMenu(0);
        pSubMenu->TrackPopupMenu(TPM_LEFTALIGN,pt.x,   pt.y,   this); 
    }
}



void CTestDmgGroupDlg::OnDmsAddGroup()
{
    LibJingleEx::HANDLE hApp = g_hXmppApp[m_nIndex];
    LibJingleEx::TDmsGroup tGroup;

    HTREEITEM hItem = m_treeRoster.GetSelectedItem();
    HTREEITEM hChild = 0;

    if ( 0 == hItem )
    {
        CTestDmgAddGroupDlg  dlg;
        if ( IDOK == dlg.DoModal() )
        {
            strncpy( tGroup.szGroupName, dlg.m_sGroupName, LibJingleEx::MAX_DMS_GROUP_NAME_LEN );

            LibJingleEx::ELibjingleError eRet = LibJingleEx::DmsAddGroup( hApp, &tGroup );
            if ( LibJingleEx::LIBJINGLE_ERR_OK != eRet )
            {
                MessageBox( "Failed to LibJingleEx::DmsAddGroup()!" );
                return;
            }

            //hChild = m_treeRoster.InsertItem( tGroup.szGroupName );
            //PTItemData pData = new TItemData;
            //pData->dwId   = tGroup.dwGroupId;
            //pData->dwType = 0;
            //m_treeRoster.SetItemData( hChild, (DWORD_PTR)pData );
        }
    }
    else
    {
        MessageBox("not supported!");
        //PTItemData pData = (PTItemData)m_treeRoster.GetItemData( hItem );

        //if ( 0 == pData->dwType )
        //{
        //    CTestDmgAddGroupDlg  dlg;
        //    if ( IDOK == dlg.DoModal() )
        //    {
        //        tGroup.dwParentId = pData->dwId;
        //        strncpy( tGroup.szGroupName, dlg.m_sGroupName, LibJingleEx::MAX_DMS_GROUP_NAME_LEN );

        //        LibJingleEx::ELibjingleError eRet = LibJingleEx::DmsAddGroup( hApp, &tGroup );
        //        if ( LibJingleEx::LIBJINGLE_ERR_OK != eRet )
        //        {
        //            MessageBox( "Failed to LibJingleEx::DmsAddGroup()!" );
        //            return;
        //        }

        //        HTREEITEM hLastChildGroup = GetLastChildGroupItem( hItem );

        //        hChild = m_treeRoster.InsertItem( tGroup.szGroupName, hItem, hLastChildGroup );
        //        PTItemData pData = new TItemData;
        //        pData->dwId   = tGroup.dwGroupId;
        //        pData->dwType = 0;
        //        m_treeRoster.SetItemData( hChild, (DWORD_PTR)pData );

        //        m_treeRoster.Expand( hItem, TVE_EXPAND );
        //    }
        //}
        //else
        //{
        //    ASSERT( 0 );
        //}
    }
}

void CTestDmgGroupDlg::OnDmsDelGroup()
{
    LibJingleEx::HANDLE hApp = g_hXmppApp[m_nIndex];
 
    HTREEITEM hItem = m_treeRoster.GetSelectedItem();
    ASSERT( hItem );
    

    PTItemData pData = (PTItemData)m_treeRoster.GetItemData( hItem );

    // 组
    if ( 0 == pData->dwType )
    {
        LibJingleEx::ELibjingleError eRet = LibJingleEx::DmsDelGroup( hApp, pData->dwId );
        if ( 0 != eRet )
        {
            CString sTmp;
            sTmp.Format( "%u", eRet );
            MessageBox( sTmp );
            return;
        }

        //DeleteTreeItemData( hItem );
        //m_treeRoster.DeleteItem( hItem );
    }
    // 成员
    else
    {
        ASSERT( 0 );
    }
}

void CTestDmgGroupDlg::OnDmsModifyGroup()
{
    LibJingleEx::HANDLE hApp = g_hXmppApp[m_nIndex];
    LibJingleEx::TDmsGroup tGroup;

    HTREEITEM hItem = m_treeRoster.GetSelectedItem();

    ASSERT( hItem );

    PTItemData pData = (PTItemData)m_treeRoster.GetItemData( hItem );

    if ( 0 == pData->dwType )
    {
        CTestDmgAddGroupDlg  dlg;
        dlg.m_sGroupName = m_treeRoster.GetItemText(hItem);

        if ( IDOK == dlg.DoModal() )
        {
            tGroup.dwGroupId = pData->dwId;
            strncpy( tGroup.szGroupName, dlg.m_sGroupName, LibJingleEx::MAX_DMS_GROUP_NAME_LEN );

            LibJingleEx::ELibjingleError eRet = LibJingleEx::DmsModifyGroup( hApp, &tGroup );
            if ( LibJingleEx::LIBJINGLE_ERR_OK != eRet )
            {
                MessageBox( "Failed to LibJingleEx::DmsModifyGroup()!" );
                return;
            }

            // m_treeRoster.SetItemText( hItem, tGroup.szGroupName );
        }
    }
    else
    {
        ASSERT( 0 );
    }
}

void CTestDmgGroupDlg::OnDmsAddMember()
{
    LibJingleEx::HANDLE hApp = g_hXmppApp[m_nIndex];
   

    HTREEITEM hItem = m_treeRoster.GetSelectedItem();
    HTREEITEM hChild = 0;

    ASSERT( hItem );

    PTItemData pData = (PTItemData)m_treeRoster.GetItemData( hItem );

    ASSERT( 0 == pData->dwType );

    CTestDmsAddMemberDlg dlg;
    if ( IDOK == dlg.DoModal() )
    {
        LibJingleEx::tagDmsMember tMember;

        strncpy( tMember.szJid, dlg.m_sJid, sizeof(tMember.szJid) );
        tMember.dwGroupId = pData->dwId;
        strncpy( tMember.szName, dlg.m_sName, sizeof(tMember.szName) );
        tMember.eType = (LibJingleEx::EDmsMemberType)dlg.m_dwType;

        strncpy( tMember.szReserved[0], dlg.m_r1, LibJingleEx::MAX_DMS_MEMBER_RESERVERD_BUF_LEN );
        strncpy( tMember.szReserved[1], dlg.m_r2, LibJingleEx::MAX_DMS_MEMBER_RESERVERD_BUF_LEN );
        strncpy( tMember.szReserved[2], dlg.m_r3, LibJingleEx::MAX_DMS_MEMBER_RESERVERD_BUF_LEN );
        strncpy( tMember.szReserved[3], dlg.m_r4, LibJingleEx::MAX_DMS_MEMBER_RESERVERD_BUF_LEN );
        

        LibJingleEx::ELibjingleError eRet = LibJingleEx::DmsAddMember( hApp, &tMember );
        if ( LibJingleEx::LIBJINGLE_ERR_OK != eRet )
        {
            MessageBox( "Failed to LibJingleEx::DmsAddMember()!" );
            return;
        }

        //hChild = m_treeRoster.InsertItem( tMember.szName, hItem );
        //m_treeRoster.SetItemImage( hChild, 1, 1 );

        //PTItemData pData = new TItemData;
        //pData->dwId   = tMember.dwMemberId;
        //pData->dwType = 1;
        //m_treeRoster.SetItemData( hChild, (DWORD_PTR)pData );

        //m_treeRoster.Expand( hItem, TVE_EXPAND );
    }
}

void CTestDmgGroupDlg::OnDmsDelMember()
{
    LibJingleEx::HANDLE hApp = g_hXmppApp[m_nIndex];


    HTREEITEM hItem = m_treeRoster.GetSelectedItem();

    ASSERT( hItem );

    PTItemData pData = (PTItemData)m_treeRoster.GetItemData( hItem );

    ASSERT( 1 == pData->dwType );

    LibJingleEx::ELibjingleError eRet = LibJingleEx::DmsDelMember( hApp, pData->dwId );
    if ( 0 != eRet )
    {
        CString sTmp;
        sTmp.Format( "%u", eRet );
        MessageBox( sTmp );

        return;
    }

    //DeleteTreeItemData( hItem );
    //m_treeRoster.DeleteItem( hItem );
}

void CTestDmgGroupDlg::OnDmsModifyMember()
{
    LibJingleEx::HANDLE hApp = g_hXmppApp[m_nIndex];

    HTREEITEM hItem = m_treeRoster.GetSelectedItem();

    ASSERT( hItem );

    PTItemData pData = (PTItemData)m_treeRoster.GetItemData( hItem );

    ASSERT( 1 == pData->dwType );

    CTestDmsAddMemberDlg dlg;
    dlg.m_sName  = m_treeRoster.GetItemText( hItem );

    LibJingleEx::TDmsMember tMember;
    LibJingleEx::DmsQueryMember( g_hXmppApp[m_nIndex], pData->dwId, &tMember );

    dlg.m_sJid  = tMember.szJid;
    dlg.m_dwType = tMember.eType;
    dlg.m_bModify = TRUE;

    dlg.m_r1      = tMember.szReserved[0];
    dlg.m_r2      = tMember.szReserved[1];
    dlg.m_r3      = tMember.szReserved[2];
    dlg.m_r4      = tMember.szReserved[3];

    if ( IDOK == dlg.DoModal() )
    {
        tMember.dwMemberId = pData->dwId;
        strncpy( tMember.szName, dlg.m_sName, sizeof(tMember.szName) );

        strncpy( tMember.szReserved[0], dlg.m_r1, LibJingleEx::MAX_DMS_MEMBER_RESERVERD_BUF_LEN );
        strncpy( tMember.szReserved[1], dlg.m_r2, LibJingleEx::MAX_DMS_MEMBER_RESERVERD_BUF_LEN );
        strncpy( tMember.szReserved[2], dlg.m_r3, LibJingleEx::MAX_DMS_MEMBER_RESERVERD_BUF_LEN );
        strncpy( tMember.szReserved[3], dlg.m_r4, LibJingleEx::MAX_DMS_MEMBER_RESERVERD_BUF_LEN );

        LibJingleEx::ELibjingleError eRet = LibJingleEx::DmsModifyMember( hApp, &tMember );
        if ( LibJingleEx::LIBJINGLE_ERR_OK != eRet )
        {
            MessageBox( "Failed to LibJingleEx::DmsModifyMember()!" );
            return;
        }

        // m_treeRoster.SetItemText( hItem, tMember.szName );
    }
}


void CTestDmgGroupDlg::OnBnClickedButtonMoveGroup()
{
    // TODO: 在此添加控件通知处理程序代码
    LibJingleEx::HANDLE  hApp = g_hXmppApp[m_nIndex];

    UpdateData( TRUE );

    if ( 0 == m_sMoveGroup.GetLength() || 0 == m_sMoveGroupParent.GetLength() )
    {
        MessageBox("请输入组或目的组！");
        return;
    }

    HTREEITEM   hGroup       = GetItemByName( m_sMoveGroup );
    if ( 0 == hGroup )
    {
        MessageBox("无效组数据！");
        return;
    }


    HTREEITEM   hGroupParent = GetItemByName( m_sMoveGroupParent );
    if ( 0 == hGroupParent )
    {
        MessageBox("无效目的数据！");
        return;
    }

    PTItemData  pGroupData       = (PTItemData)m_treeRoster.GetItemData(hGroup);
    PTItemData  pGroupParentData = (PTItemData)m_treeRoster.GetItemData(hGroupParent);

    if ( 0 != pGroupData->dwType )
    {
        MessageBox("不是组节点！");
        return;
    }

    if ( 0 != pGroupParentData->dwType )
    {
        MessageBox("不是目的组节点！");
        return;
    }

    LibJingleEx::ELibjingleError eRet = LibJingleEx::DmsMoveGroup( hApp, pGroupData->dwId, pGroupParentData->dwId );
    if ( 0 != eRet )
    {
        CString  sTmp;
        sTmp.Format( "%u", eRet );
        MessageBox( sTmp );
        return;
    }

    MoveTreeItem( hGroup, hGroupParent );

    m_treeRoster.DeleteItem( hGroup );
}

void CTestDmgGroupDlg::OnBnClickedButtonMoveMember()
{
    // TODO: 在此添加控件通知处理程序代码
    LibJingleEx::HANDLE  hApp = g_hXmppApp[m_nIndex];

    UpdateData( TRUE );

    if ( 0 == m_sMoveMember.GetLength() || 0 == m_sMoveMemberParent.GetLength() )
    {
        MessageBox("请输入成员或目的组！");
        return;
    }

    HTREEITEM   hMember       = GetItemByName( m_sMoveMember );
    if ( 0 == hMember )
    {
        MessageBox("无效成员数据！");
        return;
    }


    HTREEITEM   hGroup       = GetItemByName( m_sMoveMemberParent );
    if ( 0 == hGroup )
    {
        MessageBox("无效组数据！");
        return;
    }

    PTItemData  pMemberData      = (PTItemData)m_treeRoster.GetItemData(hMember);
    PTItemData  pGroupData       = (PTItemData)m_treeRoster.GetItemData(hGroup);

    if ( 1 != pMemberData->dwType )
    {
        MessageBox("不是成员节点！");
        return;
    }

    if ( 0 != pGroupData->dwType )
    {
        MessageBox("不是组节点！");
        return;
    }

    LibJingleEx::ELibjingleError eRet = LibJingleEx::DmsMoveMember( hApp, pMemberData->dwId, pGroupData->dwId );
    if ( 0 != eRet )
    {
        CString  sTmp;
        sTmp.Format( "%u", eRet );
        MessageBox( sTmp );
        return;
    }

    //MoveTreeItem( hMember, hGroup );

    //m_treeRoster.DeleteItem( hMember );
}


HTREEITEM  CTestDmgGroupDlg::GetItemByName( const char * szName )
{
    HTREEITEM hItem = m_treeRoster.GetRootItem();
    while( hItem )
    {
        HTREEITEM hFind = FindItem(hItem, szName);
        if ( 0 != hFind )
        {
            return hFind;
        }
        
        hItem = m_treeRoster.GetNextSiblingItem( hItem );
    }

    return 0;
}


HTREEITEM  CTestDmgGroupDlg::FindItem( HTREEITEM hItem, const char * szName )
{
    CString  sText = m_treeRoster.GetItemText( hItem );
    if ( sText == szName )
    {
        return hItem;
    }

    HTREEITEM hChild = m_treeRoster.GetChildItem( hItem );
    while( hChild )
    {
        HTREEITEM hFind = FindItem( hChild, szName);
        if ( 0 != hFind )
        {
            return hFind;
        }
        hChild = m_treeRoster.GetNextSiblingItem( hChild );
    }

    return 0;
}


void CTestDmgGroupDlg::DeleteTreeItemData( HTREEITEM hItem )
{
    HTREEITEM hChild = m_treeRoster.GetChildItem( hItem );
    while( hChild )
    {
        DeleteTreeItemData( hChild );

        hChild = m_treeRoster.GetNextSiblingItem( hChild );
    }

    PTItemData p = (PTItemData)m_treeRoster.GetItemData(hItem);
    delete p;
}

void CTestDmgGroupDlg::MoveTreeItem( HTREEITEM hItem, HTREEITEM  hParent )
{
    CString    sText   = m_treeRoster.GetItemText( hItem );
    PTItemData pData   = (PTItemData)m_treeRoster.GetItemData( hItem );
    int nImage         = ( 0 == pData->dwType ) ? 0 : 1;

    HTREEITEM hNewItem = 0;

    if ( 0 == pData->dwType )
    {
        HTREEITEM  hLastChildGroup = GetLastChildGroupItem( hParent );
        hNewItem = m_treeRoster.InsertItem( sText, nImage, nImage, hParent, hLastChildGroup );
    }
    else
    {
        hNewItem = m_treeRoster.InsertItem( sText, nImage, nImage, hParent );
    }

    m_treeRoster.SetItemData( hNewItem, (DWORD_PTR)pData );

    CopyTreeItem( hItem, hNewItem );

    m_treeRoster.Expand( hParent, TVE_EXPAND );
}

void  CTestDmgGroupDlg::CopyTreeItem( HTREEITEM hItem, HTREEITEM  hNewItem )
{
    HTREEITEM hChild = m_treeRoster.GetChildItem( hItem );
    while( hChild )
    {
        CString    sText   = m_treeRoster.GetItemText( hChild );
        PTItemData pData   = (PTItemData)m_treeRoster.GetItemData( hChild );
        int nImage         = ( 0 == pData->dwType ) ? 0 : 1;

        HTREEITEM hNewChild = m_treeRoster.InsertItem( sText, nImage, nImage, hNewItem );
        m_treeRoster.SetItemData( hNewChild, (DWORD_PTR)pData );

        CopyTreeItem( hChild, hNewChild );

        hChild = m_treeRoster.GetNextSiblingItem( hChild );
    }

    m_treeRoster.Expand( hNewItem, TVE_EXPAND );
}
void CTestDmgGroupDlg::OnDestroy()
{
    CDialog::OnDestroy();

    // TODO: 在此处添加消息处理程序代码
    HTREEITEM hItem = m_treeRoster.GetRootItem();
    while( hItem )
    {
        DeleteTreeItemData( hItem );
        hItem = m_treeRoster.GetNextSiblingItem( hItem );
    }
}

HTREEITEM  CTestDmgGroupDlg::GetLastChildGroupItem( HTREEITEM hItem )
{
    HTREEITEM hLastChildGroup  = 0;
    HTREEITEM hChild = m_treeRoster.GetChildItem( hItem );

    while( hChild )
    {
        PTItemData pData = (PTItemData)m_treeRoster.GetItemData( hChild );
        if ( 0 == pData->dwType )
        {
            hLastChildGroup = hChild;
        }
        else
        {
            return hLastChildGroup;
        }
        hChild = m_treeRoster.GetNextSiblingItem( hChild );
    }

    return hLastChildGroup;
}

HTREEITEM  CTestDmgGroupDlg::FindGroupById( HTREEITEM hItem, DWORD dwId )
{
    PTItemData pData = (PTItemData)m_treeRoster.GetItemData(hItem);
    
    if ( 0 != pData->dwType )
    {
        return 0;
    }

    if ( pData->dwId == dwId )
    {
        return hItem;
    }

    HTREEITEM hChild = m_treeRoster.GetChildItem(hItem );
    while( hChild )
    {
        HTREEITEM hRet = FindGroupById(hChild, dwId);
        if ( 0 != hRet )
        {
            return hRet;
        }
        hChild = m_treeRoster.GetNextSiblingItem(hChild);
    }

    return 0;
}

HTREEITEM  CTestDmgGroupDlg::FindGroupById( DWORD dwId )
{
    HTREEITEM hChild = m_treeRoster.GetRootItem();
    while( hChild )
    {
        HTREEITEM hRet = FindGroupById(hChild, dwId);
        if ( 0 != hRet )
        {
            return hRet;
        }

        hChild = m_treeRoster.GetNextSiblingItem(hChild);
    }

    return 0;
}


HTREEITEM  CTestDmgGroupDlg::FindMemberById( HTREEITEM hItem, DWORD dwId )
{
    PTItemData pData = (PTItemData)m_treeRoster.GetItemData(hItem);

    if ( 1 == pData->dwType && pData->dwId == dwId )
    {
        return hItem;
    }

    HTREEITEM hChild = m_treeRoster.GetChildItem(hItem );
    while( hChild )
    {
        HTREEITEM hRet = FindMemberById(hChild, dwId);
        if ( 0 != hRet )
        {
            return hRet;
        }
        hChild = m_treeRoster.GetNextSiblingItem(hChild);
    }

    return 0;
}

HTREEITEM  CTestDmgGroupDlg::FindMemberById( DWORD dwId )
{
    HTREEITEM hChild = m_treeRoster.GetRootItem();
    while( hChild )
    {
        HTREEITEM hRet = FindMemberById( hChild, dwId );
        if ( 0 != hRet )
        {
            return hRet;
        }

        hChild = m_treeRoster.GetNextSiblingItem(hChild);
    }

    return 0;
}


LRESULT CTestDmgGroupDlg::OnNotifyDmsAddGroup( WPARAM wParam, LPARAM lParam )
{
    LibJingleEx::HANDLE hApp            = (LibJingleEx::HANDLE)wParam;
    LibJingleEx::TDmsGroup * ptDmsGroup = (LibJingleEx::TDmsGroup *)lParam;

    int nIndex = (int)LibJingleEx::GetUserData( hApp );

    if ( nIndex != m_nIndex )
    {
        delete ptDmsGroup;
        return 0;
    }

    if ( 0 == ptDmsGroup->dwParentId )
    {
        HTREEITEM hChild = m_treeRoster.InsertItem( ptDmsGroup->szGroupName );
        PTItemData pData = new TItemData;
        pData->dwId   = ptDmsGroup->dwGroupId;
        pData->dwType = 0;
        m_treeRoster.SetItemData( hChild, (DWORD_PTR)pData );
    }
    else
    {
        HTREEITEM hItem = FindGroupById(ptDmsGroup->dwParentId);
        ASSERT( 0 != hItem );

        HTREEITEM hLastChildGroup = GetLastChildGroupItem( hItem );
        HTREEITEM hChild = m_treeRoster.InsertItem( ptDmsGroup->szGroupName, hItem, hLastChildGroup );
        PTItemData pData = new TItemData;
        pData->dwId   = ptDmsGroup->dwGroupId;
        pData->dwType = 0;
        m_treeRoster.SetItemData( hChild, (DWORD_PTR)pData );

        m_treeRoster.Expand( hItem, TVE_EXPAND );
    }


    delete ptDmsGroup;

    return 0;
}


LRESULT CTestDmgGroupDlg::OnNotifyDmsDelGroup( WPARAM wParam, LPARAM lParam )
{
    LibJingleEx::HANDLE hApp            = (LibJingleEx::HANDLE)wParam;
    DWORD dwId                          = (DWORD)lParam;

    int nIndex = (int)LibJingleEx::GetUserData( hApp );

    if ( nIndex != m_nIndex )
    {
        return 0;
    }

    HTREEITEM hItem = FindGroupById( dwId );
    ASSERT( 0 != hItem );

    DeleteTreeItemData( hItem );
    m_treeRoster.DeleteItem( hItem );

    return 0;
}

LRESULT CTestDmgGroupDlg::OnNotifyDmsModifyGroup( WPARAM wParam, LPARAM lParam )
{
    LibJingleEx::HANDLE hApp            = (LibJingleEx::HANDLE)wParam;
    LibJingleEx::TDmsGroup * ptDmsGroup = (LibJingleEx::TDmsGroup *)lParam;

    int nIndex = (int)LibJingleEx::GetUserData( hApp );

    if ( nIndex != m_nIndex )
    {
        delete ptDmsGroup;
        return 0;
    }

    HTREEITEM hItem = FindGroupById( ptDmsGroup->dwGroupId );
    ASSERT( 0 != hItem );

    m_treeRoster.SetItemText( hItem, ptDmsGroup->szGroupName );

    delete ptDmsGroup;
    return 0;
}

LRESULT CTestDmgGroupDlg::OnNotifyDmsAddMember( WPARAM wParam, LPARAM lParam )
{
    LibJingleEx::HANDLE hApp              = (LibJingleEx::HANDLE)wParam;
    LibJingleEx::TDmsMember * ptDmsMember = (LibJingleEx::TDmsMember *)lParam;

    int nIndex = (int)LibJingleEx::GetUserData( hApp );

    if ( nIndex != m_nIndex )
    {
        delete ptDmsMember;
        return 0;
    }

    HTREEITEM hItem = FindGroupById( ptDmsMember->dwGroupId );
    ASSERT( hItem );

    HTREEITEM hChild = m_treeRoster.InsertItem( ptDmsMember->szName, hItem );
    m_treeRoster.SetItemImage( hChild, 1, 1 );

    PTItemData pData = new TItemData;
    pData->dwId   = ptDmsMember->dwMemberId;
    pData->dwType = 1;
    m_treeRoster.SetItemData( hChild, (DWORD_PTR)pData );

    m_treeRoster.Expand( hItem, TVE_EXPAND );

    delete ptDmsMember;
    return 0;
}

LRESULT CTestDmgGroupDlg::OnNotifyDmsDelMember( WPARAM wParam, LPARAM lParam )
{
    LibJingleEx::HANDLE hApp              = (LibJingleEx::HANDLE)wParam;
    DWORD dwId                            = (DWORD)lParam;

    int nIndex = (int)LibJingleEx::GetUserData( hApp );

    if ( nIndex != m_nIndex )
    {
        return 0;
    }


    HTREEITEM hItem = FindMemberById(dwId);
    ASSERT( hItem );

    DeleteTreeItemData( hItem );
    m_treeRoster.DeleteItem( hItem );

    return 0;
}

LRESULT CTestDmgGroupDlg::OnNotifyDmsModifyMember( WPARAM wParam, LPARAM lParam )
{
    LibJingleEx::HANDLE hApp              = (LibJingleEx::HANDLE)wParam;
    LibJingleEx::TDmsMember * ptDmsMember = (LibJingleEx::TDmsMember *)lParam;

    int nIndex = (int)LibJingleEx::GetUserData( hApp );
    if ( nIndex != m_nIndex )
    {
        delete ptDmsMember;
        return 0;
    }

    HTREEITEM hItem = FindMemberById(ptDmsMember->dwMemberId);
    ASSERT( hItem );

    m_treeRoster.SetItemText( hItem, ptDmsMember->szName );

    delete ptDmsMember;
    return 0;
}

LRESULT CTestDmgGroupDlg::OnNotifyDmsMoveMember( WPARAM wParam, LPARAM lParam )
{
    LibJingleEx::HANDLE hApp              = (LibJingleEx::HANDLE)wParam;
    LibJingleEx::TDmsMember * ptDmsMember = (LibJingleEx::TDmsMember *)lParam;

    int nIndex = (int)LibJingleEx::GetUserData( hApp );
    if ( nIndex != m_nIndex )
    {
        delete ptDmsMember;
        return 0;
    }

    HTREEITEM hMember = FindMemberById(ptDmsMember->dwMemberId);
    ASSERT( hMember );

    HTREEITEM hGroup = FindGroupById(ptDmsMember->dwGroupId);
    ASSERT( hGroup );

    MoveTreeItem( hMember, hGroup );
    m_treeRoster.DeleteItem( hMember );

    delete ptDmsMember;
    return 0;
}