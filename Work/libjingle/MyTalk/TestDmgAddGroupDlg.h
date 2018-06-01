#pragma once


// CTestDmgAddGroupDlg 对话框

class CTestDmgAddGroupDlg : public CDialog
{
	DECLARE_DYNAMIC(CTestDmgAddGroupDlg)

public:
	CTestDmgAddGroupDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTestDmgAddGroupDlg();

// 对话框数据
	enum { IDD = IDD_TEST_DMS_ADD_GROUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    CString m_sGroupName;
};
