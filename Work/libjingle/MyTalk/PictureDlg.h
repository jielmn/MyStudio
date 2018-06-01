#pragma once


// CPictureDlg 对话框

class CPictureDlg : public CDialog
{
	DECLARE_DYNAMIC(CPictureDlg)

public:
	CPictureDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPictureDlg();

    LibJingleEx::TJid   m_tJid;
    CString             m_sNick;
    int                 m_nIndex;

// 对话框数据
	enum { IDD = IDD_PICTURE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedBtnBrowse();
    afx_msg void OnBnClickedBtnSend();
    void UpdatePicture( BYTE * pPicture, DWORD dwSize );
    CString m_sPath;
};
