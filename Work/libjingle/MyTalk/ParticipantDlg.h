#pragma once


// CParticipantDlg 对话框

class CParticipantDlg : public CDialog
{
	DECLARE_DYNAMIC(CParticipantDlg)

public:
	CParticipantDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CParticipantDlg();

// 对话框数据
	enum { IDD = IDD_PARTICIPANT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    CString m_sParticipant;
};
