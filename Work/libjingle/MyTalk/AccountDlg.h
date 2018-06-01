#pragma once
#include "afxcmn.h"

#include "SubAccountDlg.h"
// CAccountDlg 对话框

class CAccountDlg : public CDialog
{
	DECLARE_DYNAMIC(CAccountDlg)

public:
	CAccountDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAccountDlg();

// 对话框数据
	enum { IDD = IDD_ACCOUNT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    CSubAccountDlg   m_SubAccountDlg;

    BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
    CListCtrl m_lstAccount;
    afx_msg void OnBnClickedButtonModify();
    afx_msg void OnBnClickedButtonDelete();
};
