#pragma once
#include "afxwin.h"


// CSelAccountDlg �Ի���

class CSelAccountDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelAccountDlg)

public:
	CSelAccountDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSelAccountDlg();

// �Ի�������
	enum { IDD = IDD_SEL_ACCOUNT };

    std::vector<int>    m_v;

    int                 m_nIndex;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

    BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();

    CComboBox m_comboAccount;
};
