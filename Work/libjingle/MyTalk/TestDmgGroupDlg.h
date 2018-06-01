#pragma once
#include "afxcmn.h"


// CTestDmgGroupDlg 对话框

class CTestDmgGroupDlg : public CDialog
{
	DECLARE_DYNAMIC(CTestDmgGroupDlg)

public:
	CTestDmgGroupDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTestDmgGroupDlg();

// 对话框数据
	enum { IDD = IDD_TEST_DMS_GROUP };

    int    m_nIndex;

protected:
    typedef struct tagItemData
    {
        DWORD      dwType;           // 0; group,  1: member
        DWORD      dwId;
        tagItemData()
        {
            memset( this, 0, sizeof(*this) );
        }
    }TItemData, *PTItemData;


	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    BOOL OnInitDialog();

    BOOL GetSubGroup( HTREEITEM hParentItem );
    void GetMembers ( HTREEITEM hParentItem );

    HTREEITEM  GetItemByName( const char * szName );
    HTREEITEM  FindItem( HTREEITEM hItem, const char * szName );

    void       DeleteTreeItemData( HTREEITEM hItem );

    void       MoveTreeItem( HTREEITEM hItem, HTREEITEM  hParent );
    void       CopyTreeItem( HTREEITEM hItem, HTREEITEM  hNewItem );

    HTREEITEM  GetLastChildGroupItem( HTREEITEM hItem );

    CImageList m_ImgList;

    HTREEITEM  FindGroupById( HTREEITEM hChild, DWORD dwId );
    HTREEITEM  FindGroupById( DWORD dwId );

    HTREEITEM  FindMemberById( HTREEITEM hChild, DWORD dwId );
    HTREEITEM  FindMemberById( DWORD dwId );

    afx_msg void OnDmsAddGroup();
    afx_msg void OnDmsDelGroup();
    afx_msg void OnDmsModifyGroup();
    afx_msg void OnDmsAddMember();
    afx_msg void OnDmsDelMember();
    afx_msg void OnDmsModifyMember();
    afx_msg LRESULT OnNotifyDmsAddGroup( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnNotifyDmsDelGroup( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnNotifyDmsModifyGroup( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnNotifyDmsAddMember( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnNotifyDmsDelMember( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnNotifyDmsModifyMember( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnNotifyDmsMoveMember( WPARAM wParam, LPARAM lParam );

	DECLARE_MESSAGE_MAP()
public:
    CTreeCtrl m_treeRoster;
    afx_msg void OnNMRClickTreeRoster(NMHDR *pNMHDR, LRESULT *pResult);
    CString m_sMoveGroup;
    CString m_sMoveGroupParent;
    CString m_sMoveMember;
    CString m_sMoveMemberParent;
    afx_msg void OnBnClickedButtonMoveGroup();
    afx_msg void OnBnClickedButtonMoveMember();
    afx_msg void OnDestroy();
};
