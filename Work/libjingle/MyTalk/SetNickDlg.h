#pragma once


// CSetNickDlg 对话框

class CSetNickDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetNickDlg)

public:
	CSetNickDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSetNickDlg();

    int                  m_nIndex;
    LibJingleEx::TJid    m_tJid;
    CString              m_sOriNick;


// 对话框数据
	enum { IDD = IDD_SET_NICK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
    CString m_sNick;
    afx_msg void OnBnClickedOk();
};
