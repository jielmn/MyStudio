#pragma once


// CTestDmgAddGroupDlg �Ի���

class CTestDmgAddGroupDlg : public CDialog
{
	DECLARE_DYNAMIC(CTestDmgAddGroupDlg)

public:
	CTestDmgAddGroupDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CTestDmgAddGroupDlg();

// �Ի�������
	enum { IDD = IDD_TEST_DMS_ADD_GROUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    CString m_sGroupName;
};
