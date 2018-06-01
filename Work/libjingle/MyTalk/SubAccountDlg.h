#pragma once


// CSubAccountDlg �Ի���

class CSubAccountDlg : public CDialog
{
	DECLARE_DYNAMIC(CSubAccountDlg)

public:
	CSubAccountDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSubAccountDlg();

    LibJingleEx::TLoginSettings *  m_ptAccount;

// �Ի�������
	enum { IDD = IDD_SUB_ACCOUNT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
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
