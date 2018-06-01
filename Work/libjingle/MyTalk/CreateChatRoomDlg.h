#pragma once
#include "afxwin.h"


// CCreateChatRoomDlg 对话框

class CCreateChatRoomDlg : public CDialog
{
	DECLARE_DYNAMIC(CCreateChatRoomDlg)

public:
	CCreateChatRoomDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCreateChatRoomDlg();

    std::vector<int>         m_vInt;

// 对话框数据
	enum { IDD = IDD_CREATE_CHATROOM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedBtnAdd();
    afx_msg void OnBnClickedBtnDel();
    CComboBox m_comboOwner;
    CListBox m_lstParticipant;
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    BOOL m_bPersistent;
};
