#pragma once


// CSubAccountDlg 对话框

class CSubAccountDlg : public CDialog
{
	DECLARE_DYNAMIC(CSubAccountDlg)

public:
	CSubAccountDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSubAccountDlg();

    LibJingleEx::TLoginSettings *  m_ptAccount;

// 对话框数据
	enum { IDD = IDD_SUB_ACCOUNT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
    CString m_username;
    CString m_password;
    CString m_domain;
    CString m_host;
    CString m_resource;
    BOOL m_bUseTls;
    afx_msg void OnBnClickedOk();
    CString m_strStunAddr;
    UINT m_dwStunPort;
    CString m_strRelayAddr;
    CString m_strSavePath;
    afx_msg void OnBnClickedBtnFolder();
};
