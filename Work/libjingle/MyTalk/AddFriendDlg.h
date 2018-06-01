#pragma once
#include "afxwin.h"


// CAddFriendDlg �Ի���

class CAddFriendDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddFriendDlg)

public:
	CAddFriendDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAddFriendDlg();

    std::vector<int> m_vInt;

// �Ի�������
	enum { IDD = IDD_ADD_FRIEND };

protected:
    BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    CComboBox m_comboAccount;
    CString m_sJid;
    CString m_sNick;
    CComboBox m_comboGroup;
    afx_msg void OnBnClickedBtnAdd();
    afx_msg void OnBnClickedBtnCancel();
};
