#pragma once


// CPasswordDlg �Ի���

class CPasswordDlg : public CDialog
{
	DECLARE_DYNAMIC(CPasswordDlg)

public:
	CPasswordDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPasswordDlg();

    int     m_nIndex;

// �Ի�������
	enum { IDD = IDD_PASSWORD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

    BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
    CString m_sPassword;
    CString m_sConfirm;
    afx_msg void OnBnClickedOk();
};
