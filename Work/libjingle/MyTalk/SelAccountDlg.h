#pragma once
#include "afxwin.h"


// CSelAccountDlg 对话框

class CSelAccountDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelAccountDlg)

public:
	CSelAccountDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSelAccountDlg();

// 对话框数据
	enum { IDD = IDD_SEL_ACCOUNT };

    std::vector<int>    m_v;

    int                 m_nIndex;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();

    CComboBox m_comboAccount;
};
