#pragma once
#include "afxcmn.h"

#include "SubAccountDlg.h"
// CAccountDlg �Ի���

class CAccountDlg : public CDialog
{
	DECLARE_DYNAMIC(CAccountDlg)

public:
	CAccountDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAccountDlg();

// �Ի�������
	enum { IDD = IDD_ACCOUNT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

    CSubAccountDlg   m_SubAccountDlg;

    BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
    CListCtrl m_lstAccount;
    afx_msg void OnBnClickedButtonModify();
    afx_msg void OnBnClickedButtonDelete();
};
