#pragma once


// CParticipantDlg �Ի���

class CParticipantDlg : public CDialog
{
	DECLARE_DYNAMIC(CParticipantDlg)

public:
	CParticipantDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CParticipantDlg();

// �Ի�������
	enum { IDD = IDD_PARTICIPANT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    CString m_sParticipant;
};
