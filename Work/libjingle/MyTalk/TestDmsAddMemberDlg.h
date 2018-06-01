#pragma once


// CTestDmsAddMemberDlg 对话框

class CTestDmsAddMemberDlg : public CDialog
{
	DECLARE_DYNAMIC(CTestDmsAddMemberDlg)

public:
	CTestDmsAddMemberDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTestDmsAddMemberDlg();

// 对话框数据
	enum { IDD = IDD_TEST_DMS_ADD_MEMBER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
    CString m_sName;
    CString m_sJid;
    UINT m_dwType;

    CString m_r1;
    CString m_r2;
    CString m_r3;
    CString m_r4;

    BOOL  m_bModify;
    afx_msg void OnBnClickedOk();
};
