#pragma once


// CPictureDlg �Ի���

class CPictureDlg : public CDialog
{
	DECLARE_DYNAMIC(CPictureDlg)

public:
	CPictureDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPictureDlg();

    LibJingleEx::TJid   m_tJid;
    CString             m_sNick;
    int                 m_nIndex;

// �Ի�������
	enum { IDD = IDD_PICTURE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedBtnBrowse();
    afx_msg void OnBnClickedBtnSend();
    void UpdatePicture( BYTE * pPicture, DWORD dwSize );
    CString m_sPath;
};
