#pragma once


// CSetNickDlg �Ի���

class CSetNickDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetNickDlg)

public:
	CSetNickDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSetNickDlg();

    int                  m_nIndex;
    LibJingleEx::TJid    m_tJid;
    CString              m_sOriNick;


// �Ի�������
	enum { IDD = IDD_SET_NICK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
    BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
    CString m_sNick;
    afx_msg void OnBnClickedOk();
};
