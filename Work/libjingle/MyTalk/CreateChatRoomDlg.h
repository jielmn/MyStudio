#pragma once
#include "afxwin.h"


// CCreateChatRoomDlg �Ի���

class CCreateChatRoomDlg : public CDialog
{
	DECLARE_DYNAMIC(CCreateChatRoomDlg)

public:
	CCreateChatRoomDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCreateChatRoomDlg();

    std::vector<int>         m_vInt;

// �Ի�������
	enum { IDD = IDD_CREATE_CHATROOM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
