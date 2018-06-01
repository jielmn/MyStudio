#pragma once
#include "afxwin.h"


// CAddFriendDlg 对话框

class CAddFriendDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddFriendDlg)

public:
	CAddFriendDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAddFriendDlg();

    std::vector<int> m_vInt;

// 对话框数据
	enum { IDD = IDD_ADD_FRIEND };

protected:
    BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    CComboBox m_comboAccount;
    CString m_sJid;
    CString m_sNick;
    CComboBox m_comboGroup;
    afx_msg void OnBnClickedBtnAdd();
    afx_msg void OnBnClickedBtnCancel();
};
