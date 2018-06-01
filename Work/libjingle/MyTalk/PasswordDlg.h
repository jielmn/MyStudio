#pragma once


// CPasswordDlg 对话框

class CPasswordDlg : public CDialog
{
	DECLARE_DYNAMIC(CPasswordDlg)

public:
	CPasswordDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPasswordDlg();

    int     m_nIndex;

// 对话框数据
	enum { IDD = IDD_PASSWORD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
    CString m_sPassword;
    CString m_sConfirm;
    afx_msg void OnBnClickedOk();
};
